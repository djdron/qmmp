// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef INLINES_H
#define INLINES_H

#include "fft.h"

// *fast* convenience functions
static inline void calc_freq(short* dest, short *src)
{
    static fft_state *state = NULL;
    float tmp_out[257];
    int i;

    if (!state)
        state = fft_init();

    fft_perform(src, tmp_out, state);

    for (i = 0; i < 256; i++)
        dest[i] = ((int) sqrt(tmp_out[i + 1])) >> 8;
}

static inline void stereo16_from_multichannel(register short *l,
                                              register short *r,
                                              register short *s,
                                              long cnt, int chan)
{
    while (cnt > 0)
    {
        l[0] = s[0];
        r[0] = s[1];
        s += chan;
        l++;
        r++;
        cnt--;
    }
}

static inline void mono16_from_multichannel(register short *l,
                                            register short *s,
                                            long cnt, int chan)
{
    while (cnt > 0)
    {
        l[0] = s[0];
        s += chan;
        l++;
        cnt--;
    }
}

#endif // INLINES_H
