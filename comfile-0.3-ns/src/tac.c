#include "libcom.h"

static char *buf, *bufend;
static long bufsize;

/* Wrapper around write() that resumes on partial write or EAGAIN.
 */
static int fullwrite(int fd, char const *p, long size)
{
    long n;

    while (size) {
        n = write(fd, p, size);
        if (n > 0) {
            p += n;
            size -= n;
        } else if (errno != 11) {
            return 1;
        }
    }
    return 0;
}

/* Indicate that an error occurred.
 */
static int err(char const *msg)
{
    fullwrite(2, "error: ", 7);
    fullwrite(2, msg, strlen(msg));
    fullwrite(2, "\n", 1);
    return 1;
}

/* Read a file into the buffer, growing it as necessary.
 */
static int readloop(char const *filename, int fd)
{
    long n;

    for (;;) {
        if (buf + bufsize >= bufend) {
            bufend = brk(bufend + bufsize);
            if (!bufend)
                return err("memory allocation");
        }
        n = read(fd, buf + bufsize, bufend - buf);
        if (n < 0)
            return err(filename);
        if (n == 0)
            return 0;
        bufsize += n;
    }
}

/* Output the heap buffer's contents, line-reversed.
 */
static int writeloop(void)
{
    long n;

    for (n = bufsize ; n > 0 ; --n) {
        if (buf[n - 1] == '\n') {
            if (fullwrite(1, buf + n, bufsize - n))
                return err("stdout");
            bufsize = n;
        }
    }
    if (fullwrite(1, buf, bufsize))
        return err("stdout");
    return 0;
}

MAIN(int argc, char *argv[])
{
    int fd, i, r;

    buf = brk(0);
    bufend = brk(buf + 4096);
    bufsize = 0;
    if (!buf || !bufend)
        exit(err("memory allocation"));

    r = 0;
    if (argc <= 1) {
        r = readloop("stdin", 0);
    } else {
        for (i = 1 ; i < argc ; ++i) {
            fd = open(argv[i], O_RDONLY, 0);
            if (fd < 0) {
                r = err(argv[i]);
                continue;
            }
            if (readloop(argv[i], fd))
                r = 1;
            close(fd);
        }
    }
    if (writeloop())
        r = 1;
    exit(r);
}
