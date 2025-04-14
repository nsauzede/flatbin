#include "libcom.h"

int fprint(int fd, char const *buf)
{
    long len, r;

    len = strlen(buf);
    r = 0;
    while (len) {
	asm volatile ("syscall" : "=a" (r)
				: "0" (1), "D" (fd), "S" (buf), "d" (len)
				: "rcx", "r11", "memory");
	if (r > 0) {
	    buf += r;
	    len -= r;
	} else if (r != -11) {
	    errno = -r;
	    return 0;
	}
    }
    return 1;
}
