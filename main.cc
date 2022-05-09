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
#include <chrono>
using namespace std::chrono;

#include "adc_driver_hx711.h"

#define DATA_PIN_0 16
#define DATA_PIN_1 11
#define DATA_PIN_2 18
#define DATA_PIN_3 22
#define DATA_PIN_4 29
#define DATA_PIN_5 32

#define DCLK_PIN 23

int main( int argc, char const* argv[] )
{
    Pins adc_input_pins = { DATA_PIN_0, DATA_PIN_1, DATA_PIN_2,
                            DATA_PIN_3, DATA_PIN_4, DATA_PIN_5};

    AdcDriverHx711 adc_driver( DCLK_PIN, adc_input_pins );
    auto start = high_resolution_clock::now();

    while (1){

    Measurement    meas = adc_driver.read();
    std::cout << "Measurement:";
    
    for ( auto& m : meas )
        {
            std::cout << ' ' << m;
        }
auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << " | Time: " << duration.count() << std::endl;
    }

    return 0;
}
