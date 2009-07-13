// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//
#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QApplication>
#include <QSettings>
#include <math.h>

#include "effect.h"
#include "effectfactory.h"

#include "buffer.h"
#include "output.h"
#include "visual.h"
#include "decoderfactory.h"
#include "streamreader.h"
#include "qmmp.h"

extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"


Decoder::Decoder(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : QThread(parent), _m_factory(d), _m_input(i), _m_output(o), _m_eqInited(FALSE),
        _m_useEQ(FALSE)
{
    init();
}

Decoder::Decoder(QObject *parent, DecoderFactory *d, Output *o)
        : QThread(parent), _m_factory(d), _m_input(0), _m_output(o), _m_eqInited(FALSE),
        _m_useEQ(FALSE)
{
    init();
}

Decoder::~Decoder()
{
    _m_factory = 0;
    _m_input = 0;
    _m_output = 0;
    _blksize = 0;
    _m_handler = 0;
    _m_done = FALSE;
    _m_finish = FALSE;
    _m_totalTime = 0;
    _m_seekTime = -1;
    _m_output_at = 0;
    _m_user_stop = FALSE;
    _m_bks = Buffer::size();
    _m_output_buf = 0;
    _m_bitrate = 0;
    _m_chan = 0;
    _m_freq = 0;
    _m_bps = 0;
    _m_offset_in_bytes = 0;
    _m_length_in_bytes = 0;
    _m_totalBytes = 0;
    _m_offset = 0;
    if(_m_output_buf)
        delete [] _m_output_buf;
    _m_output_buf = 0;
}

void Decoder::init()
{
    if (_m_output)
        _m_output->recycler()->clear();
    double b[] = {0,0,0,0,0,0,0,0,0,0};
    setEQ(b, 0);
    qRegisterMetaType<Qmmp::State>("Qmmp::State");
    _blksize = Buffer::size();
    _m_effects = Effect::create(this);
    _m_handler = 0;
    _m_done = FALSE;
    _m_finish = FALSE;
    _m_totalTime = 0;
    _m_seekTime = -1;
    _m_output_at = 0;
    _m_user_stop = FALSE;
    _m_bks = Buffer::size();
    _m_output_buf = 0;
    _m_bitrate = 0;
    _m_chan = 0;
    _m_freq = 0;
    _m_bps = 0;
    _m_offset_in_bytes = 0;
    _m_length_in_bytes = 0;
    _m_offset = 0;
    _m_totalBytes = 0;
}

DecoderFactory *Decoder::factory() const
{
    return _m_factory;
}

QIODevice *Decoder::input()
{
    return _m_input;
}

Output *Decoder::output()
{
    return _m_output;
}

QMutex *Decoder::mutex()
{
    return &m_mutex;
}

QWaitCondition *Decoder::cond()
{
    return &m_waitCondition;
}

StateHandler *Decoder::stateHandler()
{
    return _m_handler;
}

void Decoder::setStateHandler(StateHandler *handler)
{
    _m_handler = handler;
}

void Decoder::setEQ(double bands[10], double preamp)
{
    set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    for (int i=0; i<10; ++i)
    {
        double value = bands[i];
        set_gain(i,0, 0.03*value+0.000999999*value*value);
        set_gain(i,1, 0.03*value+0.000999999*value*value);
    }
}

void Decoder::setEQEnabled(bool on)
{
    _m_useEQ = on;
}

void Decoder::configure(quint32 srate, int chan, int bps)
{
    Effect* effect = 0;
    _m_freq = srate;
    _m_chan = chan;
    _m_bps = bps;
    foreach(effect, _m_effects)
    {
        effect->configure(srate, chan, bps);
        srate = effect->sampleRate();
        chan = effect->channels();
        bps = effect->bitsPerSample();
    }
    _m_chan = chan;
    if (_m_output)
    {
        _m_output->configure(srate, chan, bps);
        if (!_m_output_buf)
            _m_output_buf = new unsigned char[Qmmp::globalBufferSize()];
    }
}

void Decoder::seek(qint64 time)
{
    _m_seekTime = time;
}

int Decoder::bitrate()
{
    return 0;
}

void Decoder::pause(){}

void Decoder::setFragment(qint64 offset, qint64 length)
{
    _m_offset_in_bytes = offset * _m_freq * _m_bps * _m_chan / 8000;
    _m_length_in_bytes = length * _m_freq * _m_bps * _m_chan / 8000;
    _m_offset = offset;
}

void Decoder::stop()
{
    _m_user_stop = TRUE;
}

qint64 Decoder::produceSound(char *data, qint64 size, quint32 brate, int chan)
{
    ulong sz = size < _blksize ? size : _blksize;

    if (_m_useEQ)
    {
        if (!_m_eqInited)
        {
            init_iir();
            _m_eqInited = TRUE;
        }
        iir((void*) data, sz, chan);
    }
    char *out_data = data;
    char *prev_data = data;
    qint64 w = sz;
    Effect* effect = 0;
    foreach(effect, _m_effects)
    {
        w = effect->process(prev_data, sz, &out_data);

        if (w <= 0)
        {
            // copy data if plugin can not process it
            w = sz;
            out_data = new char[w];
            memcpy(out_data, prev_data, w);
        }
        if (data != prev_data)
            delete prev_data;
        prev_data = out_data;
    }

    Buffer *b = output()->recycler()->get(w);

    memcpy(b->data, out_data, w);

    if (data != out_data)
        delete out_data;

    if (w < _blksize + b->exceeding)
        memset(b->data + w, 0, _blksize + b->exceeding - w);

    b->nbytes = w;
    b->rate = brate;

    output()->recycler()->add();

    size -= sz;
    memmove(data, data + sz, size);
    return sz;
}

void Decoder::finish()
{
    if (output())
    {
        output()->mutex()->lock ();
        output()->finish();
        output()->mutex()->unlock();
    }
    emit playbackFinished();
}

qint64 Decoder::readAudio(char*, qint64)
{
    return 0;
}

void Decoder::seekAudio(qint64){}

void Decoder::run()
{
    Q_ASSERT(_m_chan == 0);
    Q_ASSERT(!_m_output_buf);
    mutex()->lock ();

    qint64 len = 0;
    if(_m_offset > 0)
        seekAudio(_m_offset);

    mutex()->unlock();

    while (! _m_done && ! _m_finish)
    {
        mutex()->lock ();
        // decode

        if (_m_seekTime >= 0)
        {
            seekAudio(_m_seekTime + _m_offset);
            _m_totalBytes = _m_seekTime * _m_freq * _m_bps * _m_chan / 8000;
            _m_seekTime = -1;
        }

        len = readAudio((char *)(_m_output_buf + _m_output_at), Qmmp::globalBufferSize() - _m_output_at);

        if(_m_length_in_bytes && len > 0)
        {
            len = qMin(_m_length_in_bytes - _m_totalBytes, len);
            len = qMax((qint64)0, len);
        }

        if (len > 0)
        {
            _m_bitrate = bitrate();
            _m_output_at += len;
            _m_totalBytes += len;
            if (output())
                flush();
        }
        else if (len == 0)
        {
            flush(TRUE);

            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (!output()->recycler()->empty() && !_m_user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            _m_done = TRUE;
            _m_finish = !_m_user_stop;
        }
        else
            _m_finish = TRUE;
        mutex()->unlock();
    }

    mutex()->lock ();
    if (_m_finish)
        finish();
    mutex()->unlock();
}

void Decoder::flush(bool final)
{
    ulong min = final ? 0 : _m_bks;

    while ((!_m_done && !_m_finish) && _m_output_at > min)
    {
        output()->recycler()->mutex()->lock ();
        while ((!_m_done && !_m_finish) && output()->recycler()->full())
        {
            mutex()->unlock();
            output()->recycler()->cond()->wait(output()->recycler()->mutex());
            mutex()->lock ();
            _m_done = _m_user_stop;
        }

        if (_m_user_stop || _m_finish)
            _m_done = TRUE;
        else
        {
            _m_output_at -= produceSound((char*)_m_output_buf, _m_output_at, _m_bitrate, _m_chan);
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}

// static methods
QList<DecoderFactory*> *Decoder::m_factories = 0;
DecoderFactory *Decoder::m_lastFactory = 0;
QStringList Decoder::m_files;

void Decoder::checkFactories()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);

    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<DecoderFactory *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Input");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("Decoder: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("Decoder: %s", qPrintable(loader.errorString ()));
            DecoderFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<DecoderFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
        //remove physically deleted plugins from disabled list
        QStringList names;
        foreach (DecoderFactory *factory, *m_factories)
        {
            names.append(factory->properties().shortName);
        }
        QStringList disabledList  = settings.value("Decoder/disabled_plugins").toStringList ();
        foreach (QString name, disabledList)
        {
            if (!names.contains(name))
                disabledList.removeAll(name);
        }
        settings.setValue("Decoder/disabled_plugins",disabledList);
    }
}

QStringList Decoder::files()
{
    checkFactories();
    return m_files;
}


bool Decoder::supports(const QString &source)
{
    checkFactories();

    DecoderFactory *fact;
    foreach(fact, *m_factories)
    {
        if (fact->supports(source) && isEnabled(fact))
        {
            return TRUE;
        }
    }
    return FALSE;
}

DecoderFactory *Decoder::findByPath(const QString& source)
{
    checkFactories();
    DecoderFactory *fact = m_lastFactory;
    if (fact && fact->supports(source) && isEnabled(fact)) //try last factory
        return fact;
    foreach(fact, *m_factories)
    {
        if (fact->supports(source) && isEnabled(fact))
        {
            m_lastFactory = fact;
            return fact;
        }
    }
    return 0;
}

DecoderFactory *Decoder::findByMime(const QString& type)
{
    checkFactories();
    DecoderFactory *fact;
    foreach(fact, *m_factories)
    {
        if (isEnabled(fact))
        {
            QStringList types = fact->properties().contentType.split(";");
            for (int j=0; j<types.size(); ++j)
            {
                if (type == types[j] && !types[j].isEmpty())
                    return fact;
            }
        }
    }
    qDebug("Decoder: unable to find factory by mime");
    return 0;
}

DecoderFactory *Decoder::findByContent(QIODevice *input)
{
    checkFactories();
    foreach(DecoderFactory *fact, *m_factories)
    {
        if (fact->canDecode(input) && isEnabled(fact))
        {
            return fact;
        }
    }
    qDebug("Decoder: unable to find factory by content");
    return 0;
}

DecoderFactory *Decoder::findByURL(const QUrl &url)
{
    checkFactories();
    foreach(DecoderFactory *fact, *m_factories)
    {
        if (fact->supports(url.path()) && isEnabled(fact) &&
                fact->properties().protocols.split(" ").contains(url.scheme()))
        {
            return fact;
        }
    }
    qDebug("Decoder: unable to find factory by url");
    return 0;
}

void Decoder::setEnabled(DecoderFactory* factory, bool enable)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    QString name = factory->properties().shortName;
    QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
    QStringList disabledList = settings.value("Decoder/disabled_plugins").toStringList();

    if (enable)
        disabledList.removeAll(name);
    else
    {
        if (!disabledList.contains(name))
            disabledList << name;
    }
    settings.setValue("Decoder/disabled_plugins", disabledList);
}

bool Decoder::isEnabled(DecoderFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return FALSE;
    QString name = factory->properties().shortName;
    QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
    QStringList disabledList = settings.value("Decoder/disabled_plugins").toStringList();
    return !disabledList.contains(name);
}

QList <FileInfo *> Decoder::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo *> list;
    DecoderFactory *fact = 0;

    if (QFile::exists(fileName)) //is it file?
        fact = Decoder::findByPath(fileName);
    else if (fileName.contains("://")) //looks like url
        fact = Decoder::findByURL(QUrl(fileName));

    if (fact)
        list << fact->createPlayList(fileName, useMetaData);
    else if (QUrl(fileName).scheme() == "http")
        list << new FileInfo(fileName); //create empty FileInfo for stream TODO transports support
    //append path if it is empty
    foreach(FileInfo *info, list)
    {
        if (info->path().isEmpty())
            info->setPath(fileName);
    }
    return list;
}

QStringList Decoder::filters()
{
    checkFactories();
    QStringList filters;
    foreach(DecoderFactory *fact, *m_factories)
    if (isEnabled(fact) && !fact->properties().filter.isEmpty())
        filters << fact->properties().description + " (" + fact->properties().filter + ")";
    return filters;
}

QStringList Decoder::nameFilters()
{
    checkFactories();
    QStringList filters;
    for (int i=0; i<m_factories->size(); ++i)
    {
        if (isEnabled(m_factories->at(i)))
            filters << m_factories->at(i)->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    return filters;
}

QList<DecoderFactory*> *Decoder::factories()
{
    checkFactories();
    return m_factories;
}
