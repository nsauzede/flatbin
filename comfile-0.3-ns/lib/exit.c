#include "libcom.h"

void __attribute__((noreturn)) exit(int exitcode)
{
    asm ("syscall" : : "a" (60), "D" (exitcode));
    __builtin_unreachable();
}
