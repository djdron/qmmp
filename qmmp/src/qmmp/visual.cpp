/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>
#include <QObject>
#include <QList>
#include <QApplication>

#include "visualfactory.h"
#include "constants.h"
#include "output.h"

#include "visual.h"


static QList<VisualFactory*> *factories = 0;
static QStringList files;

static void checkFactories()
{
    if (! factories)
    {
        files.clear();
        factories = new QList<VisualFactory *>;

        QDir pluginsDir (qApp->applicationDirPath());
        pluginsDir.cdUp();
        pluginsDir.cd("./"LIB_DIR"/qmmp/Visual");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("Visual: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("Visual: %s", qPrintable(loader.errorString ()));

            VisualFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<VisualFactory *>(plugin);

            if (factory)
            {
                factories->append(factory);
                files << pluginsDir.absoluteFilePath(fileName);
            }
        }
    }
}


Visual::Visual(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, TRUE);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
}

Visual::~Visual()
{
    qDebug("Visual::~Visual()");
}

Decoder *Visual::decoder() const
{
    return m_decoder;
}

void Visual::setDecoder(Decoder *decoder)
{
    m_decoder = decoder;
}

Output *Visual::output() const
{
    return m_output;
}

void Visual::setOutput(Output *output)
{
    m_output = output;
}

QMutex *Visual::mutex()
{
    return &m_mutex;
}

QList<VisualFactory*> *Visual::visualFactories()
{
    checkFactories();
    return factories;
}

QStringList Visual::visualFiles()
{
    checkFactories();
    return files;
}

void Visual::setEnabled(VisualFactory* factory, bool enable)
{
    checkFactories();
    if (!factories->contains(factory))
        return;

    QString name = files.at(factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList visList = settings.value("Visualization/plugin_files").toStringList();

    if (enable)
    {
        if (!visList.contains(name))
            visList << name;
    }
    else
        visList.removeAll(name);
    settings.setValue("Visualization/plugin_files", visList);
}

bool Visual::isEnabled(VisualFactory* factory)
{
    checkFactories();
    if (!factories->contains(factory))
        return FALSE;
    QString name = files.at(factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList visList = settings.value("Visualization/plugin_files").toStringList();
    return visList.contains(name);
}

void Visual::closeEvent (QCloseEvent *event)
{
    m_output->processCloseEvent(this, event);
    QWidget::closeEvent(event);
}
