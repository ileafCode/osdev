
#include "math.h"

double pow(double x, int n)
{
    double num = 1;
    for (int a = 1; a <= n; a++)
        num *= x;
    return num;
}

double ceil(double __x) { return __builtin_ceil(__x); }

double floor(double __x) { return __builtin_floor(__x); }

double min(double __x, double __y)
{
    return __x < __y ? __x : __y;
}
double max(double __x, double __y)
{
    return __x < __y ? __y : __x;
}

double round(double __x)
{
    double __y = __x - floor(__x);
    if (__y > 0.5)
        return ceil(__x);
    return floor(__x);
}

double fact(double n)
{
    double result = 1.0;
    while (n > 1.0)
    {
        result *= n;
        n -= 1.0;
    }
    return result;
}

// Calculates sin(__x) using radians
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

// Calculates cos(__x) using radians
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

double tan(double __x)
{
    return sin(__x) / cos(__x);
}

double sqrt(double __x)
{
    if (__x == 0) return 0;
    double guess = __x / 2;
    double prevGuess;
    do {
        prevGuess = guess;
        guess = (guess + __x / guess) / 2;
    } while (prevGuess != guess);
    return guess;
}

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

double acos(double __x)
{
    if (__x > 1.0 || __x < -1.0)
        return INF;
    // Compute arccosine using identity: acos(x) = PI/2 - asin(x)
    return 1.5707963267948966192313216916398 - asin(__x);
}

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

double atan2(double __y, double __x)
{
    // Handle special cases: x = 0
    if (__x == 0)
    {
        if (__y > 0)
        {
            return 1.5707963267948966192313216916398; // PI/2
        }
        else if (__y < 0)
        {
            return -1.5707963267948966192313216916398; // -PI/2
        }
        else
        {
            return 0; // Undefined, return 0
        }
    }

    // Compute atan(y/x) using arctangent formula
    double result = atan(__y / __x);

    // Adjust result based on quadrant
    if (__x < 0)
    {
        if (__y >= 0)
        {
            result += M_PI;
        }
        else
        {
            result -= M_PI;
        }
    }

    return result;
}

double abs(double __x)
{
    if (__x < 0)
        return -__x;
    return __x;
}
