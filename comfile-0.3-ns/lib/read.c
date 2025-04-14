#include "libcom.h"

long read(int fd, void *buf, long size)
{
    long r;

    asm volatile ("syscall" : "=a" (r)
			    : "0" (0), "D" (fd), "S" (buf), "d" (size)
			    : "rcx", "r11", "memory");
    if (r < 0) {
	errno = -r;
	r = -1;
    }
    return r;
}
