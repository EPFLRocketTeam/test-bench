/**
 * @file wiringPi_test_env.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief empty functions to replace wiringPi when testing other functionality.
 * @version 0.1
 * @date 2022-03-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>

#include <iostream>

#include "wiringPi_test_env.h"


int wiringPiSetupGpio()
{
    return 0;
}
int wiringPiSetupPhys()
{
    return 0;
}
void pinMode( int p1, int p2 )
{
    std::cout << "Set pinMode: pin " << p1 << " mode: " << p2 << std::endl;
}
int digitalRead( int p )
{
    int read_val = rand() > RAND_MAX / 2;
    std::cout << "Read pin:  " << p << " value: " << read_val << std::endl;
    return read_val;
}

void digitalWrite( int pin, int val )
{
    std::cout << "Write pin: " << pin << " value: " << val << std::endl;
}
