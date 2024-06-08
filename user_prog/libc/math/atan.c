
#include <math.h>

double atan(double __x)
{
    if (__x > 1.0 || __x < -1.0)
        return INF;
    // Compute arctangent using Taylor series approximation
    double result = __x;
    double term = __x;
    double x_squared = __x * __x;
    double sign = -1.0;

    for (int i = 1; i < 10; ++i)
    {
        term *= x_squared;
        result += sign * term / (2 * i + 1);
        sign *= -1.0;
    }

    return result;
}