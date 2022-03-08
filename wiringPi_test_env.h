/**
 * @file wiringPi_test_env.h
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief empty functions to replace wiringPi when testing other functionality.
 * @version 0.1
 * @date 2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef WIRINGPI_TEST_ENV_H
#define WIRINGPI_TEST_ENV_H
#include <iostream>
#include <stdlib.h>

#define HIGH   1
#define LOW    0
#define OUTPUT 1
#define INPUT  0


int wiringPiSetupGpio()
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

#endif  // WIRINGPI_TEST_ENV_H
