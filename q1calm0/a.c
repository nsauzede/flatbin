#include <stdio.h>
int main() {
    unsigned char barr[] = {0x48, 0x89, 0xC7, 0xB0, 0x3C, 0x0F, 0x05, 0x90};
    for (int i = 0; i < sizeof(barr)/8; i++) {
        printf("l=0x%016lx\n", *((unsigned long *)barr + i));
    }
    return 0;
}
