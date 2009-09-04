/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#include <QLocalServer>
#include <QLocalSocket>

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <qmmpui/commandlinemanager.h>

#ifndef Q_OS_WIN32
#include "unixdomainsocket.h"
#endif
#include "mainwindow.h"
#include "qmmpstarter.h"
#include "builtincommandlineoption.h"

#define UDS_PATH QString("/tmp/qmmp.sock.%1").arg(getuid()).toAscii().constData()

using namespace std;

QMMPStarter::QMMPStarter(int argc,char **argv, QObject* parent) : QObject(parent), mw(NULL)
{
    m_option_manager = new BuiltinCommandLineOption(this);
    QStringList tmp;
    for (int i = 1;i < argc;i++)
        tmp << QString::fromLocal8Bit(argv[i]);

    argString = tmp.join("\n");

    if (argString == "--help")
    {
        printUsage();
        exit(0);
    }
    else if (argString == "--version")
    {
        printVersion();
        exit(0);
    }

    if (argString.startsWith("-") && // command?
            !(m_option_manager->identify(argString) ||
              CommandLineManager::hasOption(argString) ||
              argString.startsWith("--enqueue") ||
              argString.startsWith("-e")))
    {
        qFatal("QMMP: Unknown command...");
        exit(1);
    }
#ifndef Q_OS_WIN32
    m_sock = new UnixDomainSocket(this);
    if (m_sock->bind(UDS_PATH))
    {
        startMainWindow();
    }
    else if (!m_sock->alive(UDS_PATH))
    {
        // Socket is present but not connectable - application was terminated previously???
        unlink(UDS_PATH);
        if (m_sock->bind(UDS_PATH))
        {
            startMainWindow();
        }
        else
        {
            qWarning("QMMPStarter: fatal socket error, exiting");
            exit(1);
        }
    }
    else // socket is alive, qmmp application is already running. passing command to it!
        writeCommand();
#else
    startMainWindow();
#endif
}

QMMPStarter::~ QMMPStarter()
{
    if (mw) delete mw;
}

void QMMPStarter::startMainWindow()
{
#ifndef Q_OS_WIN32
    connect(m_sock, SIGNAL(readyRead()),this, SLOT(readCommand()));
#endif
    QStringList arg_l = argString.split("\n", QString::SkipEmptyParts);
    mw = new MainWindow(arg_l,m_option_manager,0);
}

void QMMPStarter::writeCommand()
{
#ifndef Q_OS_WIN32
    if (!argString.isEmpty())
    {
        char buf[PATH_MAX + 1];
        QString workingDir = QString::fromLocal8Bit(getcwd(buf,PATH_MAX)) + "\n";

        QByteArray barray;
        barray.append(workingDir.toUtf8 ());
        barray.append(argString.toUtf8 ());
        m_sock->writeDatagram ( barray.data(),UDS_PATH);
    }
    else
    {
        printUsage();
    }

    exit(0);
#endif
}

void QMMPStarter::readCommand()
{
#ifndef Q_OS_WIN32
    QByteArray inputArray;
    inputArray.resize(m_sock->pendingDatagramSize ());
    bzero(inputArray.data(),inputArray.size());
    m_sock->readDatagram(inputArray.data(), inputArray.size());
    QStringList slist = QString::fromUtf8(inputArray.data()).split("\n",QString::SkipEmptyParts);
    QString cwd = slist.takeAt(0);
    if (mw)
    {
        mw->processCommandArgs(slist,cwd);
    }
    if(m_sock->pendingDatagramSize () > 0)
        readCommand();
#endif
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
