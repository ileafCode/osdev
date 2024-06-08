
#include <math.h>

double pow(double x, int n)
{
    double num = 1;
    for (int a = 1; a <= n; a++)
        num *= x;
    return num;
}