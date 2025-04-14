#include "libcom.h"

MAIN(int argc, char *argv[])
{
    int i;

    for (i = 1 ; i < argc ; ++i) {
        if (i > 1)
            print(" ");
        print(argv[i]);
    }
    print("\n");
    exit(errno);
}
