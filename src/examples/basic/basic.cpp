#include "cstdio"

#include <engine/test.h>

int GameMain()
{
    int testNumber = GetTestNumber();
    printf("The magic number is: %d", testNumber);

    return testNumber;
}