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
#include <QIODevice>
#include <math.h>

#include "effect.h"
#include "effectfactory.h"

#include "buffer.h"
#include "output.h"
#include "visual.h"
#include "decoderfactory.h"
#include "qmmp.h"

extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"

Decoder::Decoder(QIODevice *input) : m_input(input)
{}

Decoder::~Decoder()
{}

void Decoder::configure(quint32 srate, int chan, int bps)
{
    m_parameters = AudioParameters(srate, chan, bps);
}

void Decoder::next()
{}

const QString Decoder::nextURL()
{
    return QString();
}

const AudioParameters Decoder::audioParameters()
{
    return m_parameters;
}

QIODevice *Decoder::input()
{
    return m_input;
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

QPixmap Decoder::findCover(const QString &path)
{
    QString p = QFileInfo(path).absolutePath();
    QDir dir(p);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.jpg" << "*.png";
    QFileInfoList file_list = dir.entryInfoList(filters);
    foreach(QFileInfo i, file_list)
    {
        if(!i.absoluteFilePath().contains("back", Qt::CaseInsensitive))
            return QPixmap (i.absoluteFilePath());
    }
    return QPixmap();
}
