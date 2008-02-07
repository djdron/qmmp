// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef __logscale_h
#define __logscale_h


class LogScale
{
public:
    LogScale(int = 0, int = 0);
    ~LogScale();

    int scale() const { return s; }
    int range() const { return r; }

    void setMax(int, int);

    int operator[](int);


private:
    int *indices;
    int s, r;
};


#endif // __logscale_h
