// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef INLINES_H
#define INLINES_H

#include "fft.h"

// *fast* convenience functions
static inline void
calc_freq(short* dest, short *src)
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

static inline void
calc_mono_freq(short dest[2][256], short src[2][512], int nch)
{
    int i;
    short *d, *sl, *sr, tmp[512];

    if (nch == 1)
        calc_freq(dest[0], src[0]);
    else
    {
        d = tmp;
        sl = src[0];
        sr = src[1];
        for (i = 0; i < 512; i++)
        {
            *(d++) = (*(sl++) + *(sr++)) >> 1;
        }
        calc_freq(dest[0], tmp);
    }
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


static inline void stereo16_from_stereopcm16(register short *l,
                                             register short *r,
                                             register short *s,
                                             long cnt)
{
    while (cnt >= 4l)
    {
        l[0] = s[0];
        r[0] = s[1];
        l[1] = s[2];
        r[1] = s[3];
        l[2] = s[4];
        r[2] = s[5];
        l[3] = s[6];
        r[3] = s[7];
        l += 4;
        r += 4;
        s += 8;
        cnt -= 4l;
    }

    if (cnt > 0l)
    {
        l[0] = s[0];
        r[0] = s[1];
        if (cnt > 1l)
        {
            l[1] = s[2];
            r[1] = s[3];
            if (cnt > 2l)
            {
                l[2] = s[4];
                r[2] = s[5];
            }
        }
    }
}

static inline void mono16_from_monopcm16(register short *l,
                                         register short *s,
                                         long cnt)
{
    while (cnt >= 4l)
    {
        l[0] = s[0];
        l[1] = s[1];
        l[2] = s[2];
        l[3] = s[3];
        l += 4;
        s += 4;
        cnt -= 4l;
    }

    if (cnt > 0l)
    {
        l[0] = s[0];
        if (cnt > 1l)
        {
            l[1] = s[1];
            if (cnt > 2l)
            {
                l[2] = s[2];
            }
        }
    }
}

#endif // INLINES_H
