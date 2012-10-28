/*
 * drem() wrapper for remainder().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include <math.h>

#ifdef __STDC__
	double drem(double x,double y)
#else
	double drem(x)
	double x,y;
#endif
{
	return remainder(x, y);
}
