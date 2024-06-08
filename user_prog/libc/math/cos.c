
#include <math.h>

double cos(double __x)
{
    // Convert angle to radians
    // __x *= 0.01745329251994329576923690768489; // PI / 180

    // Compute cosine using Taylor series approximation
    double result = 1.0;
    double term = 1.0;
    double sign = -1.0;
    double x_squared = __x * __x;
    double factorial = 1.0;

    for (int i = 1; i < 10; ++i)
    {
        factorial *= (2 * i) * (2 * i - 1);
        term *= x_squared;
        result += sign * term / factorial;
        sign *= -1.0;
    }

    return result;
}