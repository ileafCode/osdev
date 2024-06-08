
#include <math.h>

double round(double __x)
{
    double __y = __x - floor(__x);
    if (__y > 0.5)
        return ceil(__x);
    return floor(__x);
}