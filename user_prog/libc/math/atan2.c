
#include <math.h>

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