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
    //qRegisterMetaType<OutputState>("OutputState");
    m_bl = -1;
    m_br = -1;
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

void Output::checkVolume()
{
    int ll = 0, lr = 0;
    volume(&ll,&lr);
    ll = (ll > 100) ? 100 : ll;
    lr = (lr > 100) ? 100 : lr;
    ll = (ll < 0) ? 0 : ll;
    lr = (lr < 0) ? 0 : lr;
    if (m_bl!=ll || m_br!=lr)
    {
        m_bl = ll;
        m_br = lr;
        //dispatchVolume(ll,lr);
    }
}

void Output::checkSoftwareVolume()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    int L = settings.value("Volume/left", 80).toInt();
    int R = settings.value("Volume/right", 80).toInt();
    //dispatchVolume(L, R);
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
    OutputFactory *fact = 0;
    foreach(fact, *m_factories)
    {
        if (isEnabled(fact))
            break;
        else
            fact = m_factories->at(0);
    }
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    bool useVolume = !settings.value("Volume/software_volume", FALSE).toBool();
    if (fact)
    {
        output = fact->create (parent, useVolume);
        if (useVolume)
        {
            m_timer = new QTimer(output);
            connect(m_timer, SIGNAL(timeout()), output, SLOT(checkVolume()));
            m_timer->start(125);
        }
        else
        {
            QTimer::singleShot(125, output, SLOT(checkSoftwareVolume()));
        }
    }
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

void Output::setEnabled(OutputFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    QString name = m_files.at(m_factories->indexOf(factory)).section('/',-1);
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue ("Output/plugin_file", name);
}

bool Output::isEnabled(OutputFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return FALSE;
    QString name = m_files.at(m_factories->indexOf(factory)).section('/',-1);
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    return name == settings.value("Output/plugin_file", "libalsa.so").toString();
}

