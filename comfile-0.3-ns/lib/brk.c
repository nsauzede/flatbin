#include "libcom.h"

void *brk(void const *p)
{
    long r;

    asm volatile ("syscall" : "=a" (r)
			    : "0" (12), "D" (p)
			    : "rcx", "r11", "memory");

    if (r < 0) {
	errno = -r;
	r = 0;
    }
    return (void*)(unsigned long)r;
}
