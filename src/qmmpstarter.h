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

#ifndef _QMMPSTARTER_H
#define _QMMPSTARTER_H

#include <QObject>
#include <QAbstractSocket>
#include <QStringList>

class QTcpSocket;
class MainWindow;


/*!
 *	QMMPStarter represents wrapper object that is responsible
 * for proper QMMP initialization(only one instance of running 
 * MainWindow) and passing command line args to the TcpServer.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class QMMPStarter : public QObject
{
Q_OBJECT
public:
    QMMPStarter(int argc,char ** argv,QObject* parent = 0);
	 ~QMMPStarter();
protected slots:
	/*!
	 * Displays error message.
	 */
	 void displayError(QAbstractSocket::SocketError socketError);
	 
	 /*!
	  * Passes command args to the running TCP server
	  */
	 void writeCommand();
private:
	/*!
	 * Prints usage
	 */
	 void printUsage();
	 
	 /*!
	  * Prints version of program
	  */
	 void printVersion();
private:
	 MainWindow* mw;
	 QTcpSocket* m_tcpSocket;
	 QString argString;
};

#endif 


