#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#include "unixdomainsocket.h"


UnixDomainSocket::UnixDomainSocket(QObject * parent ) : QUdpSocket(parent){
    _binded = false;
    _s = socket(AF_UNIX, SOCK_DGRAM, 0);
    this->setSocketDescriptor(_s);
}

UnixDomainSocket::~UnixDomainSocket(){

    if(_binded){
        ::unlink(_local.sun_path);
    }
}

bool UnixDomainSocket::bind(const QString& path){

    int len;
    bzero(&_local,sizeof(_local));
    _local.sun_family = AF_UNIX;
    strcpy(_local.sun_path,path.toLocal8Bit().data());
    len = strlen(_local.sun_path) + sizeof(_local.sun_family);
    bool res = !(::bind(_s, (struct sockaddr *)&_local, len));
    if(res)
	_binded = true;
    return res;
}


bool UnixDomainSocket::alive(const QString& path)
{
    socklen_t len;
    struct sockaddr_un server;
    bzero(&server,sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,path.toLocal8Bit().data());
    len = strlen(server.sun_path) + sizeof(server.sun_family);

    if (::connect(_s, (struct sockaddr *)&server, len) == -1)
    {
        perror("connect");
        return false;
    }
    return true;
}

void UnixDomainSocket::writeDatagram(const char* command,const QString& path)
{
    socklen_t len;
    struct sockaddr_un server;
    bzero(&server,sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,path.toLocal8Bit().data());

    len = strlen(server.sun_path) + sizeof(server.sun_family);

    sendto(_s,command,strlen(command),0,(struct sockaddr*)&server,len);
}



