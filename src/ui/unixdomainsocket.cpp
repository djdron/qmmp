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

#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#include "unixdomainsocket.h"

#define LISTEN_PORT_BASE 33000

UnixDomainSocket::UnixDomainSocket(QObject * parent ) : QUdpSocket(parent){
    _bound = false;
    _s = socket(AF_UNIX, SOCK_DGRAM, 0);
    this->setSocketDescriptor(_s);
}

UnixDomainSocket::~UnixDomainSocket(){

    if(_bound){
        ::unlink(_local.sun_path);
    }
}

bool UnixDomainSocket::bind(const QString& path){

    int len;
    bzero(&_local,sizeof(_local));
    _local.sun_family = AF_UNIX;
    strcpy(_local.sun_path,path.toLocal8Bit().data());
    len = strlen(_local.sun_path) + sizeof(_local.sun_family) + 1;
    bool res = !(::bind(_s, (struct sockaddr *)&_local, len));
    if(res)
        _bound = true;
    return res;
}


bool UnixDomainSocket::alive(const QString& path)
{
    socklen_t len;
    struct sockaddr_un server;
    bzero(&server,sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,path.toLocal8Bit().data());
    len = strlen(server.sun_path) + sizeof(server.sun_family) + 1;

    if (::connect(_s, (struct sockaddr *)&server, len) == -1)
    {
        perror("connect");
        return false;
    }
    return true;
}
// 
void UnixDomainSocket::writeDatagram(const char* command,const QString& path)
{
    socklen_t len;
    struct sockaddr_un server;
    bzero(&server,sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,path.toLocal8Bit().data());

    len = strlen(server.sun_path) + sizeof(server.sun_family) + 1;

    sendto(_s,command,strlen(command),0,(struct sockaddr*)&server,len);
}



