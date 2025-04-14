#include "libcom.h"

long write(int fd, void const *buf, long size)
{
    long r;

    asm volatile ("syscall" : "=a" (r)
			    : "0" (1), "D" (fd), "S" (buf), "d" (size)
			    : "rcx", "r11", "memory");

    if (r < 0) {
        errno = -r;
        r = -1;
    }
    return r;
}
