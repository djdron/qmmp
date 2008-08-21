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
extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"


Decoder::Decoder(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : QThread(parent), fctry(d), in(i), m_output(o),m_eqInited(FALSE),
        m_useEQ(FALSE)
{
    m_output->recycler()->clear();
    int b[] = {0,0,0,0,0,0,0,0,0,0};
    setEQ(b, 0);
    qRegisterMetaType<DecoderState>("DecoderState");
    blksize = Buffer::size();
    m_effects = Effect::create(this);
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    m_useVolume = settings.value("Volume/software_volume", FALSE).toBool();
    m_volL = settings.value("Volume/left", 80).toInt();
    m_volR = settings.value("Volume/right", 80).toInt();
    setVolume(m_volL, m_volR);
}

Decoder::~Decoder()
{
    fctry = 0;
    in = 0;
    m_output = 0;
    blksize = 0;
}

// static methods

static QList<DecoderFactory*> *factories = 0;
static QStringList files;

static void checkFactories()
{
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );

    if (! factories)
    {
        files.clear();
        factories = new QList<DecoderFactory *>;

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
                factories->append(factory);
                files << pluginsDir.absoluteFilePath(fileName);
            }
        }
        //remove physically deleted plugins from disabled list
        QStringList names;
        foreach (QString filePath, files)
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
    foreach(fact, *factories)
    {
        l << fact->properties().description;
    }
    return l;
}

QStringList Decoder::decoderFiles()
{
    checkFactories();
    return files;
}


bool Decoder::supports(const QString &source)
{
    checkFactories();

    DecoderFactory *fact;
    foreach(fact, *factories)
    {
        if (fact->supports(source) && isEnabled(fact))
        {
            return TRUE;
        }
    }
    return FALSE;
}

Decoder *Decoder::create(QObject *parent, const QString &source,
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

    return decoder;
}

DecoderFactory *Decoder::findByPath(const QString& source)
{
    checkFactories();
    DecoderFactory *fact;
    foreach(fact, *factories)
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
    foreach(fact, *factories)
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
    DecoderFactory *fact;
    foreach(fact, *factories)
    {
        if (fact->canDecode(input) && isEnabled(fact))
        {
            return fact;
        }
    }
    qDebug("Decoder: unable to find factory by content");
    return 0;
}

void Decoder::setEnabled(DecoderFactory* factory, bool enable)
{
    checkFactories();
    if (!factories->contains(factory))
        return;

    QString name = files.at(factories->indexOf(factory)).section('/',-1);
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
    if (!factories->contains(factory))
        return FALSE;
    QString name = files.at(factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList disabledList = settings.value("Decoder/disabled_plugins").toStringList();
    return !disabledList.contains(name);
}

FileTag *Decoder::createTag(const QString& source)
{
    DecoderFactory *fact = Decoder::findByPath(source);
    if (fact && QFile::exists(source))
    {
        return fact->createTag(source);
    }
    return 0;
}

QStringList Decoder::filters()
{
    checkFactories();
    QStringList filters;
    foreach(DecoderFactory *fact, *factories)
    filters << fact->properties().description + " (" + fact->properties().filter + ")";
    return filters;
}

QStringList Decoder::nameFilters()
{
    checkFactories();
    QStringList filters;
    for (int i=0; i<factories->size(); ++i)
    {
        if (isEnabled(factories->at(i)))
            filters << factories->at(i)->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    return filters;
}

QList<DecoderFactory*> *Decoder::decoderFactories()
{
    checkFactories();
    return factories;
}

void Decoder::dispatch(const DecoderState &st)
{
    emit stateChanged(st);
}

void Decoder::dispatch(DecoderState::Type st)
{
    emit stateChanged(DecoderState(st));
}

void Decoder::dispatch(const FileTag &tag)
{
    emit stateChanged(DecoderState(tag));
}

void Decoder::error(const QString &e)
{
    emit stateChanged(DecoderState(e));
}

ulong Decoder::produceSound(char *data, ulong output_bytes, ulong bitrate, int nch)
{
    ulong sz = output_bytes < blksize ? output_bytes : blksize;

    if (m_useEQ)
    {
        if (!m_eqInited)
        {
            init_iir();
            m_eqInited = TRUE;
        }
        iir((void*) data,sz,nch);
    }
    if (m_useVolume)
    {
        changeVolume(data, sz, nch);
    }
    char *out_data = data;
    char *prev_data = data;
    ulong w = sz;
    Effect* effect = 0;
    foreach(effect, m_effects)
    {
        w = effect->process(prev_data, sz, &out_data);

        if (w <= 0)
        {
            // copy data if plugin can not procees it
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
    b->rate = bitrate;

    output()->recycler()->add();

    output_bytes -= sz;
    memmove(data, data + sz, output_bytes);
    return sz;
}

void Decoder::configure(long freq, int channels, int prec, int bitrate)
{
    Effect* effect = 0;
    foreach(effect, m_effects)
    {
        effect->configure(freq, channels, prec);
        freq = m_effects.at(0)->frequency();
        channels = effect->channels();
        prec = effect->resolution();
    }
    if (m_output)
        m_output->configure(freq, channels, prec, bitrate);
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

void Decoder::changeVolume(char *data, ulong sz, int channels)
{
    if (channels > 1)
        for (ulong i = 0; i < sz/2; i+=2)
        {
            ((short*)data)[i]*= m_volLF/256.0;
            ((short*)data)[i+1]*= m_volRF/256.0;
        }
    else
    {
        int l = qMax(m_volLF,m_volRF);
        for (ulong i = 0; i < sz/2; i++)
            ((short*)data)[i]*= l/256.0;
    }
}

void Decoder::setVolume(int l, int r)
{
    mtx.lock();
    m_volL = l;
    m_volR = r;
    m_volLF = pow( 10, (l - 100)/40.0 ) * 256;
    m_volRF = pow( 10, (r - 100)/40.0 ) * 256;
    mtx.unlock();
}

void Decoder::volume(int *l, int *r)
{
    mtx.lock();
    *l = m_volL;
    *r = m_volR;
    mtx.unlock();
}

