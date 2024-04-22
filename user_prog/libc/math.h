
#ifndef __MEDUZAOS_MATH
#define __MEDUZAOS_MATH

#define M_PI 3.14159
#define M_E  2.71828

#ifndef NAN
#define NAN 0.0 / 0.0
#endif

#ifndef INF
#define INF 1.0 / 0.0
#endif

double ceil(double __x);
double floor(double __x);
double min(double __x, double __y);
double max(double __x, double __y);
double round(double __x);
double sin(double __x);
double cos(double __x);
double tan(double __x);
double sqrt(double __x);
double asin(double __x);
double acos(double __x);
double atan(double __x);
double atan2(double __y, double __x);
double abs(double __x);

#endif