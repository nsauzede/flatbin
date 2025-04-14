#include "libcom.h"

long strlen(char const *str)
{
    long n;

    for (n = 0 ; str[n] ; ++n) ;
    return n;
}
