#include "libcom.h"

int open(char const *name, int flags, int mode)
{
    int r;

    asm volatile ("syscall" : "=a" (r)
			    : "0" (2), "D" (name), "S" (flags), "d" (mode)
			    : "rcx", "r11", "memory");
    if (r < 0) {
	errno = -r;
	r = -1;
    }
    return r;
}
