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

#include "constants.h"
#include "buffer.h"
#include "output.h"
#include "visual.h"
#include "decoderfactory.h"
#include "streamreader.h"
#include "volumecontrol.h"
extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"


Decoder::Decoder(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : QThread(parent), m_factory(d), m_input(i), m_output(o),m_eqInited(FALSE),
        m_useEQ(FALSE)
{
    m_output->recycler()->clear();
    int b[] = {0,0,0,0,0,0,0,0,0,0};
    setEQ(b, 0);
    qRegisterMetaType<Qmmp::State>("Qmmp::State");
    blksize = Buffer::size();
    m_effects = Effect::create(this);
    m_handler = StateHandler::instance();
}

Decoder::~Decoder()
{
    m_factory = 0;
    m_input = 0;
    m_output = 0;
    blksize = 0;
}

DecoderFactory *Decoder::factory() const
{
    return m_factory;
}

QIODevice *Decoder::input()
{
    return m_input;
}

Output *Decoder::output()
{
    return m_output;
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
    return m_handler;
}

void Decoder::setEQ(int bands[10], int preamp)
{
    set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    for (int i=0; i<10; ++i)
    {
        int value = bands[i];
        set_gain(i,0, 0.03*value+0.000999999*value*value);
        set_gain(i,1, 0.03*value+0.000999999*value*value);
    }
}

void Decoder::setEQEnabled(bool on)
{
    m_useEQ = on;
}

void Decoder::configure(quint32 srate, int chan, int bps)
{
    Effect* effect = 0;
    foreach(effect, m_effects)
    {
        effect->configure(srate, chan, bps);
        srate = effect->sampleRate();
        chan = effect->channels();
        bps = effect->bitsPerSample();
    }
    if (m_output)
        m_output->configure(srate, chan, bps);
}

qint64 Decoder::produceSound(char *data, qint64 size, quint32 brate, int chan)
{
    ulong sz = size < blksize ? size : blksize;

    if (m_useEQ)
    {
        if (!m_eqInited)
        {
            init_iir();
            m_eqInited = TRUE;
        }
        iir((void*) data, sz, chan);
    }
    changeVolume(data, sz, chan);
    char *out_data = data;
    char *prev_data = data;
    qint64 w = sz;
    Effect* effect = 0;
    foreach(effect, m_effects)
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

    if (w < blksize + b->exceeding)
        memset(b->data + w, 0, blksize + b->exceeding - w);

    b->nbytes = w;// blksize;
    b->rate = brate;

    output()->recycler()->add();

    size -= sz;
    memmove(data, data + sz, size);
    return sz;
}

void Decoder::finish()
{
    //output()->wait();
    emit finished();
}

void Decoder::changeVolume(char *data, qint64 size, int chan)
{
    if (!SoftwareVolume::instance())
        return;
    if (chan > 1)
        for (qint64 i = 0; i < size/2; i+=2)
        {
            ((short*)data)[i]*= SoftwareVolume::instance()->left()/256.0;
            ((short*)data)[i+1]*= SoftwareVolume::instance()->right()/256.0;
        }
    else
    {
        int l = qMax(SoftwareVolume::instance()->left(), SoftwareVolume::instance()->right());
        for (qint64 i = 0; i < size/2; i++)
            ((short*)data)[i]*= l/256.0;
    }
}

// static methods
QList<DecoderFactory*> *Decoder::m_factories = 0;
QStringList Decoder::m_files;

void Decoder::checkFactories()
{
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );

    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<DecoderFactory *>;

        QDir pluginsDir (qApp->applicationDirPath());
        pluginsDir.cdUp();
        pluginsDir.cd("./"LIB_DIR"/qmmp/Input");
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
            }
        }
        //remove physically deleted plugins from disabled list
        QStringList names;
        foreach (QString filePath, m_files)
        {
            names.append(filePath.section('/',-1));
        }
        int i = 0;
        QStringList disabledList  = settings.value("Decoder/disabled_plugins").toStringList ();
        while (i < disabledList.size())
        {
            if (!names.contains(disabledList.at(i)))
                disabledList.removeAt(i);
            else
                i++;
        }
        settings.setValue("Decoder/disabled_plugins",disabledList);
    }
}


QStringList Decoder::all()
{
    checkFactories();

    QStringList l;
    DecoderFactory *fact;
    foreach(fact, *m_factories)
    {
        l << fact->properties().description;
    }
    return l;
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

/*Decoder *Decoder::create(QObject *parent, const QString &source,
                         QIODevice *input,
                         Output *output)
{
    Decoder *decoder = 0;
    DecoderFactory *fact = 0;
    if (!input->open(QIODevice::ReadOnly))
    {
        qDebug("Decoder: cannot open input");
        return decoder;
    }
    StreamReader* sreader = qobject_cast<StreamReader *>(input);
    if (sreader)
    {
        fact = Decoder::findByMime(sreader->contentType());
        if (!fact)
            fact = Decoder::findByContent(sreader);
    }
    else
        fact = Decoder::findByPath(source);
    if (fact)
    {
        decoder = fact->create(parent, input, output);
    }
    if (!decoder)
        input->close();
    else
        output->setStateHandler(decoder->stateHandler());
    return decoder;
}*/

DecoderFactory *Decoder::findByPath(const QString& source)
{
    checkFactories();
    DecoderFactory *fact;
    foreach(fact, *m_factories)
    {
        if (fact->supports(source) && isEnabled(fact))
        {
            return fact;
        }
    }
    qDebug("Decoder: unable to find factory by path");
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

    QString name = m_files.at(m_factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
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
    QString name = m_files.at(m_factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList disabledList = settings.value("Decoder/disabled_plugins").toStringList();
    return !disabledList.contains(name);
}

FileInfo *Decoder::getFileInfo(const QString &fileName)
{
    DecoderFactory *fact = Decoder::findByPath(fileName);
    if (fact && QFile::exists(fileName))
    {
        return fact->getFileInfo(fileName);
    }
    return 0;
}

QStringList Decoder::filters()
{
    checkFactories();
    QStringList filters;
    foreach(DecoderFactory *fact, *m_factories)
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

