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

#include "unixdomainsocket.h"
#include <unistd.h>

#include "mainwindow.h"
#include "version.h"
#include "qmmpstarter.h"

#define MAXCOMMANDSIZE 1024

QMMPStarter::QMMPStarter(int argc,char ** argv,QObject* parent) : QObject(parent),mw(NULL)
{	
	QStringList tmp;
	for(int i = 1;i < argc;i++)
		tmp << QString::fromLocal8Bit(argv[i]);

	argString = tmp.join("\n");
	
	if(argString == "--help")
	{
		printUsage();
		exit(0);
	}
	else if(argString == "--version")
	{
		printVersion();
		exit(0);
	}
	
	if(argString.startsWith("--") &&  // command?
		  argString != "--play" && 
		  argString != "--previous" && 
		  argString != "--next" && 
		  argString != "--stop" && 
		  argString != "--pause" &&
		  argString != "--play-pause" 
	  )
	{
		qFatal("QMMP: Unknown command...");
		exit(1);
	}

    m_sock = new UnixDomainSocket(this);
    if(m_sock->bind(UDS_PATH))
    {
        startMainWindow();
    }
    else if(!m_sock->alive(UDS_PATH)){ 
        // Socket is present but not connectable - application was terminated previously???
        unlink(UDS_PATH);
        if(m_sock->bind(UDS_PATH))
        {
            startMainWindow();
        }
        else
        {
            qDebug("Fatal socket error, exiting");
            exit(1);
        }
    }
    else // socket is alive, qmmp application is already running. passing command to it!
        writeCommand();
}

QMMPStarter::~ QMMPStarter()
{
    qWarning("QMMPStarter::~ QMMPStarter()");
	if(mw) delete mw;
}

void QMMPStarter::startMainWindow()
{
    connect(m_sock, SIGNAL(readyRead()),this, SLOT(readCommand()));
    QStringList arg_l = argString.split("\n", QString::SkipEmptyParts);
    mw = new MainWindow(arg_l,0);
}

void QMMPStarter::writeCommand()
{
	if(!argString.isEmpty())
	{
		char buf[PATH_MAX + 1];
		QString workingDir = QString(getcwd(buf,PATH_MAX)) + "\n";
		
		QByteArray barray;
		barray.append(workingDir);
		barray.append(argString);
        m_sock->writeDatagram ( barray.data(),UDS_PATH);
	}
	else
	{
		printUsage();
	}
	
    exit(0);
}

void QMMPStarter::readCommand()
{
        QByteArray inputArray;
        inputArray.resize(MAXCOMMANDSIZE);
        bzero(inputArray.data(),inputArray.size());
        m_sock->readDatagram(inputArray.data(), inputArray.size());
        QStringList slist = QString(inputArray).split("\n",QString::SkipEmptyParts);
        QString cwd = slist.takeAt(0);
        if(mw)
        {
            mw->processCommandArgs(slist,cwd);
        }

}

void QMMPStarter::printUsage()
{
	qWarning(
			"Usage: qmmp [options] [files] \n"
			"Options:\n"
			"--------\n"
			"--help              Display this text and exit.\n"
			"--previous          Skip backwards in playlist\n"
			"--play              Start playing current playlist\n"
			"--pause             Pause current song\n"
			"--play-pause        Pause if playing, play otherwise\n"
			"--stop              Stop current song\n"
			"--next              Skip forward in playlist\n"
			"--version           Print version number and exit.\n\n"
			"Ideas, patches, bugreports send to forkotov02@hotmail.ru\n"
			  );
}

void QMMPStarter::printVersion()
{
	qWarning("QMMP version:  %s",QMMP_STR_VERSION);
}



