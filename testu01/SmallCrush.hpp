#include <iostream>

extern "C" {
    #include <testu01/gdef.h>
    #include <testu01/swrite.h>
    #include <testu01/bbattery.h>
}

int main (void)
{
    swrite_Basic = FALSE;
    bbattery_SmallCrushFile((char*)"pcg_data.bin", 1520000000);
    return 0;
}