#include <libcom.h>
MAIN(int argc, char *argv[]) {
    fprint(1, "hello\n");
    return argc == 2 ? argv[1][0] : 0;
}
