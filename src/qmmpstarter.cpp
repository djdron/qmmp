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
#include <QTcpSocket>

#include <unistd.h>

#include "mainwindow.h"
#include "version.h"
#include "qmmpstarter.h"
#include "guard.h"

QMMPStarter::QMMPStarter(int argc,char ** argv,QObject* parent) : QObject(parent),mw(0)
{	
	QStringList tmp;
	for(int i = 1;i < argc;i++)
		tmp << QString(argv[i]);

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
	
	if(Guard::exists(QApplication::applicationFilePath()))
	{
		m_tcpSocket = new QTcpSocket(this);
		connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(displayError(QAbstractSocket::SocketError)));
		connect(m_tcpSocket, SIGNAL(connected()),this, SLOT(writeCommand()));
		
		m_tcpSocket->connectToHost("127.0.0.1",TCPSERVER_PORT_NUMBER + getuid());
		
	}
	else
	{
		Guard::create(QApplication::applicationFilePath());
		QStringList arg_l = argString.split("\n", QString::SkipEmptyParts);
		mw = new MainWindow(arg_l,0);
	}
}

void QMMPStarter::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) 
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			qWarning("The host was not found");
			break;
		case QAbstractSocket::ConnectionRefusedError:
			qWarning("The connection was refused by the peer. ");
			break;
		default:
			qWarning("The following error: %s:",qPrintable(m_tcpSocket->errorString()));
	}

	Guard::create(QApplication::applicationFilePath());
	mw = new MainWindow(argString.split("\n", QString::SkipEmptyParts),0);
}

QMMPStarter::~ QMMPStarter()
{
	if(mw)
	{
		Guard::destroy(QApplication::applicationFilePath());
		delete mw;
	}
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
		
		m_tcpSocket->write(barray);
		m_tcpSocket->flush();
	}
	else
	{
		qWarning("It seems that another version of application is already running ...\n");
		printUsage();
	}
	
	m_tcpSocket->close();
	QApplication::quit();
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

