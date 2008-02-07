// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include "logscale.h"

#include <math.h>
#include <stdio.h>


LogScale::LogScale(int maxscale, int maxrange)
    : indices(0), s(0), r(0)
{
    setMax(maxscale, maxrange);
}


LogScale::~LogScale()
{
    if (indices)
	delete [] indices;
}


void LogScale::setMax(int maxscale, int maxrange)
{
    if (maxscale == 0 || maxrange == 0)
	return;

    s = maxscale;
    r = maxrange;

    if (indices)
	delete [] indices;

    double alpha;
    int i, scaled;
    double domain = double(maxscale),
	    range = double(maxrange),
		x = 1.0,
	       dx = 1.0,
		y = 0.0,
	       yy = 0.0,
		t = 0.0,
	       e4 = double(1.0E-8);

    indices = new int[maxrange];
    for (i = 0; i < maxrange; i++)
	indices[i] = 0;

    // initialize log scale
    while (fabs(dx) > e4) {
	t = log((domain + x) / x);
	y = (x * t) - range;
	yy = t - (domain / (x + domain));
	dx = y / yy;
	x -= dx;
    }

    alpha = x;
    for (i = 1; i < (int) domain; i++) {
	scaled = (int) floor(0.5 + (alpha * log((double(i) + alpha) / alpha)));
	if (indices[scaled - 1] < i)
	    indices[scaled - 1] = i;
    }
}


int LogScale::operator[](int index)
{
    return indices[index];
}
