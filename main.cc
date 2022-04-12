/**
 * @file main.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief main
 * @version 0.1
 * @date 2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>

#include "adc_driver_hx711.h"

#define DATA_PIN_0 16
#define DCLK_PIN   23

#define DCLK_PIN 23

int main( int argc, char const* argv[] )
{
    Pins           adc_input_pins = { DATA_PIN_0 };
    AdcDriverHx711 adc_driver( DCLK_PIN, adc_input_pins );
    int            i = 0;
    for ( ;; )
        {
            Measurement meas = adc_driver.read();
            std::cout << "Measurement " << i++ << ':';
            for ( auto& m : meas )
                {
                    std::cout << '\n' << m;
                }
            std::cout << std::endl;
        }


    return 0;
}
