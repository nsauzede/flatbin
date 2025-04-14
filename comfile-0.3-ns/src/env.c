#include "libcom.h"

MAIN(int argc, char *argv[], char **envp)
{
    int i;

    for (i = 0 ; envp[i] ; ++i) {
	print(envp[i]);
	print("\n");
    }
    exit(0);
    (void)argc;
    (void)argv;
}
