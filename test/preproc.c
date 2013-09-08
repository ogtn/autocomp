#include <stdio.h>
#include "my_header.h"
#include <sys/siginfo.h>

#if 0
    static const double DBL = 11.22;
#else
    static const double DBL = 33.44;
#endif

#define INT_MACRO       42
#define FLOAT_MACRO     42.F
#define STR_MACRO       "Hello, world!"

#define fn_macro(x, y)  ((x) > (y) ? (x) : (y))
