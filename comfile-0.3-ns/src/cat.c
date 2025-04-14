#include "libcom.h"

static char buf[8192];

static void err(char const *prefix)
{
    long n;

    n = strlen(prefix);
    write(2, prefix, n);
    write(2, ": failed\n", 9);
}

static int copyloop(int fd)
{
    long n;

    for (;;) {
	n = read(fd, buf, sizeof buf - 1);
	if (n == 0)
	    return 0;
	if (n < 0)
	    return errno;
	buf[n] = '\0';
	print(buf);
    }
}

MAIN(int argc, char *argv[])
{
    long r, r1;
    int fd, i;

    if (argc <= 1)
	exit(copyloop(0));

    r = 0;
    for (i = 1 ; i < argc ; ++i) {
	fd = open(argv[i], O_RDONLY, 0);
	if (fd < 0) {
	    err(argv[i]);
	    r = errno;
	    continue;
	}
	r1 = copyloop(fd);
	close(fd);
	if (r1) {
	    err(argv[i]);
	    r = r1;
	}
    }
    exit(r);
}
