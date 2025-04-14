#include "libcom.h"

int close(int fd)
{
    int r;

    asm volatile ("syscall" : "=a" (r)
			    : "0" (3), "D" (fd)
			    : "rcx", "r11", "memory");
    if (r < 0) {
	errno = -r;
	r = -1;
    }
    return r;
}
