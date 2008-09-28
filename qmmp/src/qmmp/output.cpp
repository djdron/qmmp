// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QApplication>
#include <QTimer>

#include "constants.h"
#include "output.h"

#include <stdio.h>

Output::Output (QObject* parent) : QThread (parent), m_recycler (stackSize())
{
    m_handler = 0;
}

Recycler *Output::recycler()
{
    return &m_recycler;
}

QMutex *Output::mutex()
{
    return &m_mutex;
}

void Output::setStateHandler(StateHandler *handler)
{
    m_handler = handler;
}

Output::~Output()
{}

void Output::dispatchVisual (Buffer *buffer, unsigned long written,
                             int chan, int prec)
{
    if (!buffer)
        return;
    foreach (Visual *visual, *Visual::visuals())
    {
        visual->mutex()->lock ();
        visual->add ( buffer, written, chan, prec );
        visual->mutex()->unlock();
    }
}


void Output::clearVisuals()
{
    foreach (Visual *visual, *Visual::visuals())
    {
        visual->mutex()->lock ();
        visual->clear ();
        visual->mutex()->unlock();
    }
}

void Output::dispatch(qint64 elapsed,
                      qint64 totalTime,
                      int bitrate,
                      int frequency,
                      int precision,
                      int channels)
{
    if (m_handler)
        m_handler->dispatch(elapsed, totalTime, bitrate, frequency, precision, channels);
}

void Output::dispatch(const Qmmp::State &state)
{
    if (m_handler)
        m_handler->dispatch(state);
}



// static methods

QList<OutputFactory*> *Output::m_factories = 0;
QStringList Output::m_files;
QTimer *Output::m_timer = 0;

void Output::checkFactories()
{
    if ( ! m_factories )
    {
        m_files.clear();
        m_factories = new QList<OutputFactory *>;

        QDir pluginsDir ( qApp->applicationDirPath() );
        pluginsDir.cdUp();
        pluginsDir.cd ( "./"LIB_DIR"/qmmp/Output" );
        foreach ( QString fileName, pluginsDir.entryList ( QDir::Files ) )
        {
            QPluginLoader loader ( pluginsDir.absoluteFilePath ( fileName ) );
            QObject *plugin = loader.instance();
            if ( loader.isLoaded() )
                qDebug ( "Output: plugin loaded - %s", qPrintable ( fileName ) );
            else
                qWarning("Output: %s", qPrintable(loader.errorString ()));

            OutputFactory *factory = 0;
            if ( plugin )
                factory = qobject_cast<OutputFactory *> ( plugin );

            if ( factory )
            {
                Output::registerFactory ( factory );
                m_files << pluginsDir.absoluteFilePath(fileName);
            }
        }
    }
}

void Output::registerFactory ( OutputFactory *fact )
{
    m_factories->append ( fact );
}

Output *Output::create (QObject *parent)
{
    Output *output = 0;

    checkFactories();
    if (m_factories->isEmpty ())
    {
        qDebug("Output: unable to find output plugins");
        return output;
    }
    OutputFactory *fact = Output::currentFactory();
    if (!fact && !m_factories->isEmpty())
        fact = m_factories->at(0);
    if (fact)
        output = fact->create (parent);
    return output;
}

QList<OutputFactory*> *Output::outputFactories()
{
    checkFactories();
    return m_factories;
}

QStringList Output::outputFiles()
{
    checkFactories();
    return m_files;
}

void Output::setCurrentFactory(OutputFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    QString name = m_files.at(m_factories->indexOf(factory)).section('/',-1);
    //TODO use plugin id instead
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue ("Output/plugin_file", name);
}

OutputFactory *Output::currentFactory()
{
    checkFactories();
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    QString name = settings.value("Output/plugin_file", "libalsa.so").toString();
    foreach(QString path, m_files)
    {
        if (path.section('/',-1) == name)
        {
            int i = m_files.indexOf(path);
            if (i < 0)
                return 0;
            else
                return m_factories->at(i);
        }
    }
    return 0;
}
