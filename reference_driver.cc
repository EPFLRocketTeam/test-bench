/**
 * @file reference_driver.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Copied the reference program for the HX711
 * @version 0.1
 * @date 2022-03-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>

#include "wiringPi.h"


#define DATA_PIN_0 11
#define DATA_PIN_1 26
#define DATA_PIN_2 18
#define DATA_PIN_3 22
#define DATA_PIN_4 29
#define DATA_PIN_5 32
#define DCLK_PIN   23


/**
 * @brief Reference driver (adapted) for the hx711 ADC
 *
 * @return unsigned long
 */
int read_value();

int main( int argc, char const* argv[] )
{
    wiringPiSetupGpio();
    pinMode( DATA_PIN_0, INPUT );
    pinMode( DCLK_PIN, OUTPUT );

    std::cout << read_value() << std::endl;

    return 0;
}

int read_value()
{
    int adc_val = 0;
    while ( digitalRead( DATA_PIN_0 ) )
        ;
    for ( int i = 0; i < 24; i++ )
        {
            digitalWrite( DCLK_PIN, HIGH );
            adc_val << 1;
            digitalWrite( DCLK_PIN, LOW );

            if ( digitalRead( DATA_PIN_0 ) )
                {
                    adc_val++;
                }
        }
    digitalWrite( DCLK_PIN, HIGH );
    adc_val = adc_val ^ 0x800000;  // because  24 bit signed value on 32 bit int
    digitalWrite( DCLK_PIN, LOW );
    return adc_val;
}
