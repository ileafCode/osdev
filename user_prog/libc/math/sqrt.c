
#include <math.h>

double sqrt(double __x)
{
    if (__x <= 0) return 0;
    double guess = __x / 2;
    double prevGuess;
    do {
        prevGuess = guess;
        guess = (guess + __x / guess) / 2;
    } while (prevGuess != guess);
    return guess;
}