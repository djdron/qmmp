// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   __buffer_h
#define   __buffer_h

#define QMMP_BLOCK_SIZE 2048
#define QMMP_BUFFER_SIZE (QMMP_BLOCK_SIZE*128)

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
        data = new unsigned char[QMMP_BLOCK_SIZE];
        nbytes = 0;
        rate = 0;
        size = QMMP_BLOCK_SIZE;
    }
    /*!
     * Destructor.
     */
    ~Buffer()
    {
        delete[] data;
        data = 0;
        nbytes = 0;
        rate = 0;
        size = 0;
    }

    unsigned char *data;      /*!< Audio data */
    unsigned long nbytes;     /*!< Audio data size */
    unsigned long rate;       /*!< Buffer bitrate */
    unsigned long size;       /*!< Buffer size */
};


#endif // __buffer_h

