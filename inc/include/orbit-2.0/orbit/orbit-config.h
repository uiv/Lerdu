#ifndef ORBIT_MULTILIB
#define ORBIT_MULTILIB

#include <bits/wordsize.h>

#if __WORDSIZE == 32
# include "orbit-config-32.h"
#elif __WORDSIZE == 64
# include "orbit-config-64.h"
#else
# error "unexpected value for __WORDSIZE macro"
#endif

#endif
