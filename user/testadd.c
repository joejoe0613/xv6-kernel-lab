#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int result;

    result = add(7, 35);

    printf("7 + 35 = %d\n", result);

    exit(0);
}