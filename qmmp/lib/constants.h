// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef __constants_h
#define __constants_h

#define VERSION "MQ3 Release Fourteen"

#if defined(Q_OS_UNIX)
//#  include "../config.h"
#  ifndef INSTALLDIR
#    define INSTALLDIR "/usr/local"
#  endif
#elif defined( Q_OS_WIN32 )
#  define INSTALLDIR "c:/depot/mq3/main"
#endif

const unsigned int historySize      = 100;
const unsigned int globalBlockSize  = 2 * 1024;    //2*1024
const unsigned int globalBufferSize = globalBlockSize * 32;
const unsigned int groupOpenTimeout = 750;

#if defined( Q_OS_WIN32 )
#define BUFFERBLOCKS 16
#endif

#endif // __constants_h
