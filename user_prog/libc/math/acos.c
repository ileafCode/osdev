
#include <math.h>

double acos(double __x)
{
    if (__x > 1.0 || __x < -1.0)
        return INF;
    // Compute arccosine using identity: acos(x) = PI/2 - asin(x)
    return 1.5707963267948966192313216916398 - asin(__x);
}