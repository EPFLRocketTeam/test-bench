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


#define HIGH   1
#define LOW    0
#define OUTPUT 1
#define INPUT  0


int  wiringPiSetupGpio();
void pinMode( int p1, int p2 );
int  digitalRead( int p );
void digitalWrite( int pin, int val );

#endif  // WIRINGPI_TEST_ENV_H
