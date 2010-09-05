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
        buffer_count = 4;

    buffers = new Buffer*[buffer_count];

    for (unsigned int i = 0; i < buffer_count; i++)
    {
        buffers[i] = new Buffer;
    }
    m_blocked = 0;
}


Recycler::~Recycler()
{
    for (unsigned int i = 0; i < buffer_count; i++)
    {
        delete buffers[i];
        buffers[i] = 0;
    }
    delete [] buffers;
    m_blocked = 0;
}


bool Recycler::full() const
{
    return current_count == buffer_count;
}

bool Recycler::blocked()
{
    return buffers[add_index] == m_blocked;
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
    if(buffers[add_index]->nbytes)
    {
        add_index = (add_index + 1) % buffer_count;
        current_count++;
    }
}

Buffer *Recycler::next()
{
    if(current_count)
    {
        m_blocked = buffers[done_index];
        return m_blocked;
    }
    return 0;
}

void Recycler::done()
{
    m_blocked = 0;
    if (current_count)
    {
        current_count--;
        done_index = (done_index + 1) % buffer_count;
    }
}

void Recycler::clear()
{
    current_count = 0;
    add_index = 0;
    done_index = 0;
}

unsigned int Recycler::size() const
{
    return buffer_count * QMMP_BLOCK_SIZE;
}
