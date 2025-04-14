#include "libcom.h"

MAIN(void)
{
    return -1 == write(1, "hello, world\n", 13);
}
