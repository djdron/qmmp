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

// static methods

static QList<OutputFactory*> *factories = 0;
static QStringList files;
static QTimer *timer = 0;

static void checkFactories()
{
    if ( ! factories )
    {
        files.clear();
        factories = new QList<OutputFactory *>;

        QDir pluginsDir ( qApp->applicationDirPath() );
        pluginsDir.cdUp();
        pluginsDir.cd ( "./"LIB_DIR"/qmmp/Output" );
        foreach ( QString fileName, pluginsDir.entryList ( QDir::Files ) )
        {
            QPluginLoader loader ( pluginsDir.absoluteFilePath ( fileName ) );
            QObject *plugin = loader.instance();
            if ( loader.isLoaded() )
            {
                qDebug ( "Output: plugin loaded - %s", qPrintable ( fileName ) );
            }
            OutputFactory *factory = 0;
            if ( plugin )
                factory = qobject_cast<OutputFactory *> ( plugin );

            if ( factory )
            {
                Output::registerFactory ( factory );
                files << pluginsDir.absoluteFilePath(fileName);
            }
        }
    }
}

void Output::registerFactory ( OutputFactory *fact )
{
    factories->append ( fact );
}

Output *Output::create ( QObject *parent )
{
    Output *output = 0;

    checkFactories();
    if ( factories->isEmpty () )
    {
        qDebug("Output: unable to find output plugins");
        return output;
    }
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QString pluginFileName =
        settings.value("Output/plugin_file","libalsa.so").toString();
    int j = 0;
    for (int i = 0; i < factories->size(); ++i)
    {
        if (files.at(i).section('/',-1) == pluginFileName)
            j = i;
    }
    OutputFactory *fact = factories->at ( j );
    if ( fact )
    {
        output = fact->create ( parent );
    }
    switch ((int) output->volumeControl())
    {
    case Output::Standard:
        {
            break;
        }
    case Output::Custom:
        {
            timer = new QTimer(output);
            connect(timer, SIGNAL(timeout()), output, SLOT(checkVolume()));
            timer->start(125);
            break;
        }
    case Output::Disabled:
        {
            break;
        }
    }
    return output;
}

QList<OutputFactory*> *Output::outputFactories()
{
    checkFactories();
    return factories;
}

QStringList Output::outputFiles()
{
    checkFactories();
    return files;
}

Output::Output ( QObject* parent, VolumeType vt) : QThread (parent), r (stackSize())
{
    qRegisterMetaType<OutputState>("OutputState");
    m_vol = vt;
}


Output::~Output()
{}

void Output::error ( const QString &e )
{
    emit stateChanged ( OutputState ( e ) );
}


void Output::addVisual ( Visual *v )
{
    if (visuals.indexOf (v) == -1)
    {
        visuals.append (v);
    }
}


void Output::removeVisual (Visual *v)
{
    visuals.removeAll (v);
}

void Output::dispatchVisual ( Buffer *buffer, unsigned long written,
                              int chan, int prec )
{
    if ( ! buffer || !visuals.size())
        return;
    Visual* visual = 0;
    foreach (visual , visuals);
    {
        visual->mutex()->lock ();
        visual->add ( buffer, written, chan, prec );
        visual->mutex()->unlock();
    }
}


void Output::clearVisuals()
{
    Visual *visual = 0;
    foreach (visual, visuals );
    {
        visual->mutex()->lock ();
        visual->clear();
        visual->mutex()->unlock();
    }
}

void Output::dispatch(OutputState::Type st)
{
    if(st == OutputState::Stopped)
        clearVisuals();
    emit stateChanged ( OutputState(st) );
}

void Output::dispatch(long s, unsigned long w, int b, int f, int p, int c)
{
    emit stateChanged ( OutputState(s, w, b, f, p, c) );
}

void Output::dispatch ( const OutputState &st )
{
    if(st.type() == OutputState::Stopped)
        clearVisuals();
    emit stateChanged ( st );
}

void Output::dispatchVolume(int L, int R)
{
    emit stateChanged ( OutputState(L, R) );
}
