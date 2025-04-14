#include "libcom.h"

MAIN(void)
{
    write(1, "hello, world\n", 13);
    exit(errno ? 1 : 0);
}
