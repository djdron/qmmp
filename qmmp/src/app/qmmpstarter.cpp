/***************************************************************************
 *   Copyright (C) 2006-2012 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QApplication>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSettings>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <qmmp/qmmp.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/uiloader.h>
#include <qmmpui/qmmpuisettings.h>
#include "qmmpstarter.h"
#include "builtincommandlineoption.h"

#ifdef Q_OS_WIN
#define UDS_PATH QString("qmmp")
#else
#define UDS_PATH QString("/tmp/qmmp.sock.%1").arg(getuid()).toAscii().constData()
#endif

using namespace std;

QMMPStarter::QMMPStarter(int argc,char **argv, QObject* parent) : QObject(parent)
{
    m_player = 0;
    m_core = 0;
    m_ui = 0;
#ifdef Q_OS_WIN
    m_named_mutex = 0;
#endif
    m_option_manager = new BuiltinCommandLineOption(this);
    QStringList tmp;
    for (int i = 1;i < argc;i++)
        tmp << QString::fromLocal8Bit(argv[i]).trimmed();

    argString = tmp.join("\n");
    QHash <QString, QStringList> commands = m_option_manager->splitArgs(tmp);

    if(commands.keys().contains("--help"))
    {
        printUsage();
        exit(0);
    }
    if(commands.keys().contains("--version"))
    {
        printVersion();
        exit(0);
    }

    if(!commands.isEmpty())
    {
        foreach(QString arg, commands.keys())
        {
            if(!m_option_manager->identify(arg) && !CommandLineManager::hasOption(arg) &&
                    arg != "--no-start")
            {
                cout << qPrintable(tr("Unknown command")) << endl;
                exit(0);
            }
        }
    }

    m_server = new QLocalServer(this);
    m_socket = new QLocalSocket(this);
    bool noStart = commands.keys().contains("--no-start");

#ifdef Q_OS_WIN
    //Windows IPC implementation (named mutex and named pipe)
    m_named_mutex = CreateMutexA(NULL, TRUE, "QMMP-403cd318-cc7b-4622-8dfd-df18d1e70057");
    if(GetLastError() == NO_ERROR && !noStart)
    {
        m_server->listen (UDS_PATH);
        startPlayer();
    }
    else
    {
        m_socket->connectToServer(UDS_PATH); //connecting
        m_socket->waitForConnected();
        if(!m_socket->isValid()) //invalid connection
        {
            qWarning("QMMPStarter: unable to connect to server");
            exit(0);
        }
        writeCommand();
    }
#else
    if(!noStart && m_server->listen (UDS_PATH)) //trying to create server
    {
        startPlayer();
    }
    else if(QFile::exists(UDS_PATH))
    {
        m_socket->connectToServer(UDS_PATH); //connecting
        m_socket->waitForConnected();
        if(!m_socket->isValid()) //invalid connection
        {
            if(!QLocalServer::removeServer(UDS_PATH))
            {
                qWarning("QMMPStarter: unable to remove invalid socket file");
                exit(1);
                return;
            }
            qWarning("QMMPStarter: removed invalid socket file");
            if(noStart)
                exit(0);
            else if(m_server->listen (UDS_PATH))
                startPlayer();
            else
            {
                qWarning("QMMPStarter: server error: %s", qPrintable(m_server->errorString()));
                exit(1);
            }
        }
        else
            writeCommand();
    }
    else
        exit(0);
#endif
}

QMMPStarter::~QMMPStarter()
{
    if (m_ui)
        delete m_ui;
#ifdef Q_OS_WIN
    if(m_named_mutex)
        ReleaseMutex(m_named_mutex);
#endif
}

void QMMPStarter::startPlayer()
{
    connect(m_server, SIGNAL(newConnection()), SLOT(readCommand()));
    QStringList args = argString.split("\n", QString::SkipEmptyParts);

    //prepare libqmmp and libqmmpui libraries for usage
    m_player = new MediaPlayer(this);
    m_core = SoundCore::instance();

    //additional featuries
    new PlaylistParser(this);
    new UiHelper(this);

    //interface
    UiFactory *factory = UiLoader::selected();
    if(factory)
        m_ui = factory->create();
    else
    {
        qWarning("QMMPStarter: no user interface found");
        exit(1);
        return;
    }
    connect(qApp, SIGNAL(aboutToQuit()), SLOT(savePosition()));
    processCommandArgs(args, QDir::currentPath());
    if(args.isEmpty())
    {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        settings.beginGroup("General");
        if(settings.value("resume_playback", false).toBool())
        {
            qint64 pos =  settings.value("resume_playback_time").toLongLong();
            m_player->play(pos);
        }
    }
}

void QMMPStarter::savePosition()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("resume_playback",m_core->state() == Qmmp::Playing &&
                      QmmpUiSettings::instance()->resumeOnStartup());
    settings.setValue("resume_playback_time", m_core->totalTime() > 0 ? m_core->elapsed() : 0);
    settings.endGroup();
    m_core->stop();
}

void QMMPStarter::writeCommand()
{
    if (!argString.isEmpty())
    {
        QString workingDir = QDir::currentPath() + "\n";

        QByteArray barray;
        barray.append(workingDir.toUtf8 ());
        barray.append(argString.toUtf8 ());
        while(!barray.isEmpty())
        {
            qint64 size = m_socket->write(barray);
            barray.remove(0, size);
        }
        m_socket->flush();
        //reading answer
        if(m_socket->waitForReadyRead(1500))
            cout << m_socket->readAll().data();
    }
    else
    {
        printUsage();
    }

    exit(0);
}

void QMMPStarter::readCommand()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    socket->waitForReadyRead();
    QByteArray inputArray = socket->readAll();
    if(inputArray.isEmpty())
        return;
    QStringList slist = QString::fromUtf8(inputArray.data()).split("\n",QString::SkipEmptyParts);
    QString cwd = slist.takeAt(0);
    QString out = processCommandArgs(slist, cwd);
    if(!out.isEmpty())
    {
        //writing answer
        socket->write(out.toLocal8Bit());
        socket->flush();
    }
    socket->deleteLater();
}

QString QMMPStarter::processCommandArgs(const QStringList &slist, const QString& cwd)
{
    if(slist.isEmpty())
        return QString();
    QStringList paths;
    foreach(QString arg, slist) //detect file/directory paths
    {
        if(arg.startsWith("-"))
            break;
        paths.append(arg);
    }
    if(!paths.isEmpty())
    {
        m_option_manager->executeCommand(QString(), paths, cwd); //add paths only
        return QString();
    }
    QHash<QString, QStringList> commands = m_option_manager->splitArgs(slist);
    if(commands.isEmpty())
        return QString();
    foreach(QString key, commands.keys())
    {
        if(key == "--no-start")
            continue;
        if (CommandLineManager::hasOption(key))
            return CommandLineManager::executeCommand(key, commands.value(key));
        else if (m_option_manager->identify(key))
            m_option_manager->executeCommand(key, commands.value(key), cwd);
        else
            return QString();
    }
    return QString();
}

void QMMPStarter::printUsage()
{
    cout << qPrintable(tr("Usage: qmmp [options] [files]")) << endl;
    cout << qPrintable(tr("Options:")) << endl;
    cout << "--------" << endl;
    cout << qPrintable(m_option_manager->helpString()) << endl;
    CommandLineManager::printUsage();
    cout << "--no-start               " << qPrintable(tr("Don't start the application")) << endl;
    cout << "--help                   " << qPrintable(tr("Display this text and exit")) << endl;
    cout << "--version                " << qPrintable(tr("Print version number and exit")) << endl;
    cout << qPrintable(tr("Ideas, patches, bugreports send to forkotov02@hotmail.ru")) << endl;
}

void QMMPStarter::printVersion()
{
    cout << qPrintable(tr("QMMP version: %1").arg(Qmmp::strVersion())) << endl;
    cout << qPrintable(tr("Compiled with Qt version: %1").arg(QT_VERSION_STR)) << endl;
    cout << qPrintable(tr("Using Qt version: %1").arg(qVersion())) << endl;
}