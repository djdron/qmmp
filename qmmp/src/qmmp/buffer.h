// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   __buffer_h
#define   __buffer_h

#include "constants.h"

/*! @brief Audio buffer class.
 * @author Brad Hughes <bhughes@trolltech.com>
 */
class Buffer
{
public:
    /*!
     * Constructs an empty buffer object.
     */
    Buffer()
    {
        data = new unsigned char[Buffer::size()];
        nbytes = 0;
        rate = 0;
        exceeding = 0;
    }
    /*!
     * Destructor.
     */
    ~Buffer()
    {
        delete data;
        data = 0;
        nbytes = 0;
        rate = 0;
        exceeding = 0;
    }

    unsigned char *data;      /*!< Audio data */
    unsigned long nbytes;     /*!< Audio data size */
    unsigned long rate;       /*!< Buffer bitrate */
    unsigned long exceeding;  /*!< The number of bytes on which the size of buffer exceeds the size of the block */
    /*!
     * Returns block size.
     */
    static unsigned long size()
    {
        return globalBlockSize;
    }
};


#endif // __buffer_h

