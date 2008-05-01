#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef QMMP_STR_VERSION
#define QMMP_STR_VERSION "0.2.0"
#endif

#ifndef QMMP_VERSION
#define QMMP_VERSION 0.2.0
#endif

#ifndef LIB_DIR
#define LIB_DIR "/lib"
#endif

const unsigned int historySize      = 100;
const unsigned int globalBlockSize  = 2 * 1024;    //2*1024
const unsigned int globalBufferSize = globalBlockSize * 32;
const unsigned int groupOpenTimeout = 750;

#endif // CONSTANTS_H
