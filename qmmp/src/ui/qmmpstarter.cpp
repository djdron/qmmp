/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <qmmpui/commandlinemanager.h>
#include "mainwindow.h"
#include "qmmpstarter.h"
#include "builtincommandlineoption.h"

#ifdef Q_OS_WIN32
#define UDS_PATH QString("qmmp")
#else
#define UDS_PATH QString("/tmp/qmmp.sock.%1").arg(getuid()).toAscii().constData()
#endif

using namespace std;

QMMPStarter::QMMPStarter(int argc,char **argv, QObject* parent) : QObject(parent), mw(NULL)
{
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
            if(!m_option_manager->identify(arg) &&
               !CommandLineManager::hasOption(arg) &&
               arg != "--enqueue" &&
               arg != "-e")
            {
                cout << qPrintable(tr("Unknown command")) << endl;
                exit(0);
            }
        }
    }

    m_server = new QLocalServer(this);
    m_socket = new QLocalSocket(this);
    if(m_server->listen (UDS_PATH)) //trying to create server
    {
        startMainWindow();
    }
    else
    {
        m_socket->connectToServer(UDS_PATH); //connecting
        m_socket->waitForConnected();
        if(!m_socket->isValid()) //invalid connection
        {
            qWarning("QMMPStarter: trying to remove invalid socket file");
            if(!QLocalServer::removeServer(UDS_PATH))
            {
                qWarning("QMMPStarter: unable to remove invalid socket file");
                exit(1);
                return;
            }
            if(m_server->listen (UDS_PATH))
                startMainWindow();
            else
            {
                qWarning("QMMPStarter: server error: %s", qPrintable(m_server->errorString()));
                exit(1);
            }
        }
        else
            writeCommand();
    }
}

QMMPStarter::~ QMMPStarter()
{
    if (mw)
        delete mw;
}

void QMMPStarter::startMainWindow()
{
    connect(m_server, SIGNAL(newConnection()), SLOT(readCommand()));
    QStringList arg_l = argString.split("\n", QString::SkipEmptyParts);
    mw = new MainWindow(arg_l,m_option_manager,0);
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
    QString out = mw ? mw->processCommandArgs(slist, cwd) : QString();
    if(!out.isEmpty())
    {
        //writing answer
        socket->write(out.toLocal8Bit());
        socket->flush();
    }
    socket->deleteLater();
}

void QMMPStarter::printUsage()
{
    cout << qPrintable(tr("Usage: qmmp [options] [files]")) << endl;
    cout << qPrintable(tr("Options:")) << endl;
    cout << "--------" << endl;
    cout << "-e, --enqueue        " << qPrintable(tr("Don't clear the playlist")) << endl;
    cout << qPrintable(m_option_manager->helpString()) << endl;
    CommandLineManager::printUsage();
    cout << "--help               " << qPrintable(tr("Display this text and exit")) << endl;
    cout << "--version            " << qPrintable(tr("Print version number and exit")) << endl;
    cout << qPrintable(tr("Ideas, patches, bugreports send to forkotov02@hotmail.ru")) << endl;
}

void QMMPStarter::printVersion()
{
    cout << qPrintable(tr("QMMP version:")) << " " << qPrintable(Qmmp::strVersion()) << endl;
    cout << qPrintable(tr("Qt version:")) << " " << QT_VERSION_STR << endl;
}
