#ifndef _QMMP_VERSION_H
#define _QMMP_VERSION_H

#ifndef QMMP_VERSION
#define QMMP_VERSION 0.2.0
#endif

#ifndef QMMP_STR_VERSION
#define QMMP_STR_VERSION "0.2.0"
#endif

#define LISTEN_PORT_BASE 33000
#define UDS_PATH QString("/tmp/qmmp.sock.%1").arg(getuid()).toAscii().constData()
#endif
