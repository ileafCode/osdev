
#include <math.h>

double asin(double __x)
{
    if (__x > 1.0 || __x < -1.0)
        return INF;
    // Compute arcsine using Newton's method
    double result = __x;
    double term = __x;
    double x_squared = __x * __x;
    double factor = __x;
    int n = 1;

    while (n < 10)
    {
        factor *= (2 * n - 1) * (2 * n - 1) * x_squared / (2 * n * (2 * n + 1));
        term = factor / (2 * n + 1);
        result += term;
        ++n;
    }

    return result;
}