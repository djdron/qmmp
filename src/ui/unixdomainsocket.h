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

