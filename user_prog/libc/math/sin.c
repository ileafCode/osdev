
#include <math.h>

double sin(double __x)
{
    // Convert angle to radians
    //__x *= 0.01745329251994329576923690768489; // PI / 180

    const int iterations = 10;
    double result = 0.0;
    double term = __x;
    double power = __x;
    double factorial = 1.0;
    int sign = 1;

    for (int i = 1; i <= iterations; ++i)
    {
        result += sign * term;
        power *= __x * __x;
        factorial *= (2 * i) * (2 * i + 1);
        term = power / factorial;
        sign *= -1; // alternate the sign
    }

    return result;
}