// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef __recycler_h
#define __recycler_h

#include <QMutex>
#include <QWaitCondition>

class Buffer;

/*! @brief The Recycler class provides a queue of audio buffers.
 *  @author Brad Hughes <bhughes@trolltech.com>
 */
class Recycler
{
public:
    /*!
     * Constructor.
     * @param sz Preferd total size of the all buffers in bytes.
     */
    Recycler(unsigned int sz);
    /*!
     * Destructor.
     */
    ~Recycler();
    /*!
     * Returns \b true if queue if full, otherwise returns \b false
     */
    bool full() const;
    /*!
     * Returns \b true if queue if empty, otherwise returns \b false
     */
    bool empty() const;
    /*!
     * Returns a number of available buffers.
     */
    int available() const;
    /*!
     * Returns a number of used buffers.
     */
    int used() const;
    /*!
     *  Returns next buffer in queue.
     */
    Buffer *next();
    /*!
     * Returns current buffer for writing.
     */
    Buffer *get(); // get next in recycle
    /*!
     * Adds current buffer to queue.
     */
    void add(); // add to queue
    /*!
     * Removes current buffer from queue.
     */
    void done(); // add to recycle
    /*!
     * Clears queue.
     */
    void clear(); // clear queue
    /*!
     * Returns size of all buffers in bytes.
     */
    unsigned int size() const; // size in bytes
    /*!
    * Returns mutex pointer.
    */
    QMutex *mutex()
    {
        return &mtx;
    }
    /*!
     * Returns wait condition pointer.
     */
    QWaitCondition *cond()
    {
        return &cnd;
    }

private:
    unsigned int buffer_count, add_index, done_index, current_count;
    Buffer **buffers;
    QMutex mtx;
    QWaitCondition cnd;
};

#endif // __recycler_h
