// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include "recycler.h"
#include "buffer.h"

Recycler::Recycler (unsigned int sz)
        : add_index (0), done_index (0), current_count (0)
{
    buffer_count = sz / QMMP_BLOCK_SIZE;
    if (buffer_count < 4)
    {
        buffer_count = 4;
    }

    buffers = new Buffer*[buffer_count];

    for (unsigned int i = 0; i < buffer_count; i++)
    {
        buffers[i] = new Buffer;
    }
}


Recycler::~Recycler()
{
    for (unsigned int i = 0; i < buffer_count; i++)
    {
        delete buffers[i];
        buffers[i] = 0;
    }

    delete [] buffers;
}


bool Recycler::full() const
{
    return current_count == buffer_count;
}


bool Recycler::empty() const
{
    return current_count == 0;
}


int Recycler::available() const
{
    return buffer_count - current_count;
}

int Recycler::used() const
{
    return current_count;
}


Buffer *Recycler::get()
{
    if (full())
        return 0;
    return buffers[add_index];
}


void Recycler::add()
{
    add_index = ++add_index % buffer_count;
    current_count++;
}


Buffer *Recycler::next()
{
    return !current_count ? 0 : buffers[done_index];
}


void Recycler::done()
{
    done_index = ++done_index % buffer_count;
    if (current_count)
        current_count--;
}


void Recycler::clear()
{
    current_count = 0;
}


unsigned int Recycler::size() const
{
    return buffer_count * QMMP_BLOCK_SIZE;
}
