/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#ifndef UNIXDOMAINSOCKET_H
#define UNIXDOMAINSOCKET_H

#include <QUdpSocket>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/un.h>


/*!
 *  UnixDomainSocket class is a wrapper around the unix domain sockets implementation.
 *  Used for QMMP interprocess communications.
 *  @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class UnixDomainSocket : public QUdpSocket
{
Q_OBJECT
public:
    UnixDomainSocket(QObject * parent = 0 );
    
    /*!
     * Try to bind socket to \b path. Returns \b true on success, otherwise \b false
     */
    bool bind(const QString& file);
    
    /*!
     * Checks if socket at \b path path is alive( i.e. connectable). 
     */
    bool alive(const QString& path);
    ~UnixDomainSocket();

public slots:
    /*!
     * Sends the datagram \b command to the socket at \b path path.
     */
    void writeDatagram(const char* command,const QString& path);    
private:
    unsigned int _s;
    struct sockaddr_un _local;
    bool _bound;
};


#endif

