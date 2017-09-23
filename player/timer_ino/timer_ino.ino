#define main __main
#define setup __setup
#define loop __loop
#include "timer.cpp"

void __setup()
{
    __main();
}

void __loop() {}
