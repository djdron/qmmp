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

#include <QStringList>
#include <QApplication>
#include <QTcpSocket>
#include <QRegExp>
#include <QTimer>
		
#include <unistd.h>

#include "mainwindow.h"
#include "tcpserver.h"
#include "version.h"

TcpServer::TcpServer(MainWindow* parent) : QTcpServer(parent) , m_mainWindow(parent)
{
	if (!listen(QHostAddress::LocalHost,TCPSERVER_PORT_NUMBER + getuid())) 
	{
		qFatal("Unable to start the server: %s ",qPrintable(errorString()));
		QApplication::exit(1);
	}

	connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
	qDebug("TcpServer running at localost port %d",TCPSERVER_PORT_NUMBER + getuid());
}

void TcpServer::handleNewConnection()
{
	clientConnection = nextPendingConnection();
	connect(clientConnection, SIGNAL(readyRead()),this, SLOT(readCommand()));
}


void TcpServer::readCommand()
{
	QByteArray inputArray(clientConnection->readAll());
	QStringList slist = QString(inputArray).split("\n",QString::SkipEmptyParts);
	QString cwd = slist.takeAt(0);
	m_mainWindow->processCommandArgs(slist,cwd);

	if(clientConnection)
	{
		clientConnection->disconnectFromHost();
		if(clientConnection)
			delete clientConnection;
	}
}




