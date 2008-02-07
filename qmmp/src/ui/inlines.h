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

static inline void stereo16_from_stereopcm8(register short *l,
        register short *r,
        register uchar *c,
        long cnt)
{
    while (cnt >= 4l)
    {
        l[0] = c[0];
        r[0] = c[1];
        l[1] = c[2];
        r[1] = c[3];
        l[2] = c[4];
        r[2] = c[5];
        l[3] = c[6];
        r[3] = c[7];
        l += 4;
        r += 4;
        c += 8;
        cnt -= 4l;
    }

    if (cnt > 0l)
    {
        l[0] = c[0];
        r[0] = c[1];
        if (cnt > 1l)
        {
            l[1] = c[2];
            r[1] = c[3];
            if (cnt > 2l)
            {
                l[2] = c[4];
                r[2] = c[5];
            }
        }
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


static inline void mono16_from_monopcm8(register short *l,
                                        register uchar *c,
                                        long cnt)
{
    while (cnt >= 4l)
    {
        l[0] = c[0];
        l[1] = c[1];
        l[2] = c[2];
        l[3] = c[3];
        l += 4;
        c += 4;
        cnt -= 4l;
    }

    if (cnt > 0l)
    {
        l[0] = c[0];
        if (cnt > 1l)
        {
            l[1] = c[1];
            if (cnt > 2l)
            {
                l[2] = c[2];
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


static inline void fast_short_set(register short *p,
                                  short v,
                                  long c)
{
    while (c >= 4l)
    {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        p[0] = v;
        if (c > 1l)
        {
            p[1] = v;
            if (c > 2l)
            {
                p[2] = v;
            }
        }
    }
}

#ifdef FFTW
static inline void fast_real_set(register fftw_real *p,
                                 fftw_real v,
                                 long c)
{
    while (c >= 4l)
    {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        p[0] = v;
        if (c > 1l)
        {
            p[1] = v;
            if (c > 2l)
            {
                p[2] = v;
            }
        }
    }
}

static inline void fast_complex_set(register fftw_complex *p,
                                    fftw_complex v,
                                    long c)
{
    while (c >= 4l)
    {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        p[0] = v;
        if (c > 1l)
        {
            p[1] = v;
            if (c > 2l)
            {
                p[2] = v;
            }
        }
    }
}


static inline void fast_real_set_from_short(register fftw_real *d,
        register short *s,
        long c)
{
    while (c >= 4l)
    {
        d[0] = fftw_real(s[0]);
        d[1] = fftw_real(s[1]);
        d[2] = fftw_real(s[2]);
        d[3] = fftw_real(s[3]);
        d += 4;
        s += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        d[0] = fftw_real(s[0]);
        if (c > 1l)
        {
            d[1] = fftw_real(s[1]);
            if (c > 2l)
            {
                d[2] = fftw_real(s[2]);
            }
        }
    }
}

static inline void fast_complex_set_from_short(register fftw_complex *d,
        register short *s,
        long c)
{
    while (c >= 4l)
    {
        d[0].re = fftw_real(s[0]);
        d[0].im = 0;
        d[1].re = fftw_real(s[1]);
        d[1].im = 0;
        d[2].re = fftw_real(s[2]);
        d[2].im = 0;
        d[3].re = fftw_real(s[3]);
        d[3].im = 0;
        d += 4;
        s += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        d[0].re = fftw_real(s[0]);
        d[0].im = 0;
        if (c > 1l)
        {
            d[1].re = fftw_real(s[1]);
            d[1].im = 0;
            if (c > 2l)
            {
                d[2].re = fftw_real(s[2]);
                d[2].im = 0;
            }
        }
    }
}


static inline void fast_real_avg_from_shorts(register fftw_real *d,
        register short *s1,
        register short *s2,
        long c)
{
    fftw_real t0, t1, t2, t3;
    while (c >= 4l)
    {
        t0 = (s1[0] + s2[0]) / 2;
        t1 = (s1[1] + s2[1]) / 2;
        t2 = (s1[2] + s2[2]) / 2;
        t3 = (s1[3] + s2[3]) / 2;
        d[0] = t0;
        d[1] = t1;
        d[2] = t2;
        d[3] = t3;
        d += 4;
        s1 += 4;
        s2 += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        d[0] = fftw_real((s1[0] + s2[0]) / 2);
        if (c > 1l)
        {
            d[1] = fftw_real((s1[1] + s2[1]) / 2);
            if (c > 2l)
            {
                d[2] = fftw_real((s1[2] + s2[2]) / 2);
            }
        }
    }
}

static inline void fast_complex_avg_from_shorts(register fftw_complex *d,
        register short *s1,
        register short *s2,
        long c)
{
    fftw_real t0, t1, t2, t3;
    while (c >= 4l)
    {
        t0 = (s1[0] + s2[0]) / 2;
        t1 = (s1[1] + s2[1]) / 2;
        t2 = (s1[2] + s2[2]) / 2;
        t3 = (s1[3] + s2[3]) / 2;
        d[0].re = t0;
        d[0].im = 0;
        d[1].re = t1;
        d[1].im = 0;
        d[2].re = t2;
        d[2].im = 0;
        d[3].re = t3;
        d[3].im = 0;
        d += 4;
        s1 += 4;
        s2 += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        d[0].re = fftw_real((s1[0] + s2[0]) / 2);
        d[0].im = 0;
        if (c > 1l)
        {
            d[1].re = fftw_real((s1[1] + s2[1]) / 2);
            d[1].im = 0;
            if (c > 2l)
            {
                d[2].re = fftw_real((s1[2] + s2[2]) / 2);
                d[2].im = 0;
            }
        }
    }
}


static inline fftw_complex fftw_complex_from_real( fftw_real re )
{
    fftw_complex c;

    c.re = re;
    c.im = 0;

    return c;
}

static inline void fast_reals_set(register fftw_real *p1,
                                  register fftw_real *p2,
                                  fftw_real v,
                                  long c)
{
    while (c >= 4l)
    {
        p1[0] = v;
        p1[1] = v;
        p1[2] = v;
        p1[3] = v;
        p2[0] = v;
        p2[1] = v;
        p2[2] = v;
        p2[3] = v;
        p1 += 4;
        p2 += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        p1[0] = v;
        p2[0] = v;
        if (c > 1l)
        {
            p1[1] = v;
            p2[1] = v;
            if (c > 2l)
            {
                p1[2] = v;
                p2[2] = v;
            }
        }
    }
}

static inline void fast_complex_set(register fftw_complex *p1,
                                    register fftw_complex *p2,
                                    fftw_complex v,
                                    long c)
{
    while (c >= 4l)
    {
        p1[0] = v;
        p1[1] = v;
        p1[2] = v;
        p1[3] = v;
        p2[0] = v;
        p2[1] = v;
        p2[2] = v;
        p2[3] = v;
        p1 += 4;
        p2 += 4;
        c -= 4l;
    }

    if (c > 0l)
    {
        p1[0] = v;
        p2[0] = v;
        if (c > 1l)
        {
            p1[1] = v;
            p2[1] = v;
            if (c > 2l)
            {
                p1[2] = v;
                p2[2] = v;
            }
        }
    }
}
#endif // FFTW

#endif // INLINES_H
