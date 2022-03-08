/**
 * @file adc_driver_hx711.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Driver for multiple HX711 ADCs (with common clock)
 * @version 0.1
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <algorithm>

#include "adc_driver_hx711.h"
#include "wiringPi.h"

//=====================================================================================
// Constructor
//=====================================================================================

AdcDriverHx711::AdcDriverHx711( int dclk, const Pins& pins ) : dclk_( dclk )
{
    std::unique_copy( pins.begin(), pins.end(), std::back_inserter( pins_ ) );
    wiringPiSetupGpio();
    pinMode( dclk_, OUTPUT );
    for ( auto pin : pins_ )
        {
            pinMode( pin, INPUT );
        }
}

//=====================================================================================
// Getters
//=====================================================================================
const Pins& AdcDriverHx711::pins() const
{
    return pins_;
}
int AdcDriverHx711::dclk_pin() const
{
    return dclk_;
}


//=====================================================================================
// Driver settings
//=====================================================================================
bool AdcDriverHx711::add_pin( int pin, bool force )
{
    if ( ! force
         && ( pin == dclk_
              || std::find( pins_.begin(), pins_.end(), pin ) != std::end( pins_ ) ) )
        {
            return false;
        }
    else
        {
            pinMode( pin, INPUT );
            pins_.push_back( pin );
            return true;
        }
}


bool AdcDriverHx711::remove_pin( int pin )
{
    auto remove = std::remove( pins_.begin(), pins_.end(), pin );
    if ( remove == pins_.end() )
        {
            return false;
        }
    else
        {
            pins_.erase( remove, pins_.end() );
            return true;
        }
}

bool AdcDriverHx711::set_dclk( int pin, bool force )
{
    auto existing_pin = std::find( pins_.begin(), pins_.end(), pin );
    if ( existing_pin == pins_.end() )
        {
            pinMode( pin, OUTPUT );
            return true;
        }
    if ( force )
        {
            pins_.erase( existing_pin );
            pinMode( pin, OUTPUT );
            return true;
        }

    return false;
}


//=====================================================================================
// ADC interactions
//=====================================================================================
bool AdcDriverHx711::data_ready()
{
    for ( int pin : pins_ )
        {
            if ( ! digitalRead( pin ) )
                return false;
        }
    return true;
}

Measurement AdcDriverHx711::read()
{
    Measurement measurement( 0, pins_.size() );
    while ( ! data_ready() )
        ;
    for ( int i = 0; i < 24; i++ )
        {
            digitalWrite( dclk_, HIGH );
            for ( int i = 0; i < pins_.size(); i++ )
                {
                    measurement[ i ] << 1;
                    digitalWrite( dclk_, LOW );
                    if ( digitalRead( pins_[ i ] ) )
                        {
                            measurement[ i ]++;
                        }
                }
        }
    digitalWrite( dclk_, HIGH );
    for ( auto& adc_val : measurement )
        {
            // because  24 bit signed value on 32 bit int
            adc_val = adc_val ^ 0x800000;
        }
    digitalWrite( dclk_, LOW );
    return measurement;
}
