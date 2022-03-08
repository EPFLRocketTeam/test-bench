/**
 * @file adc_driver_hx711.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Driver for multiple HX711 ADCs (with common clock)
 * @version 0.2
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <algorithm>
#include <unistd.h>

#include "adc_driver_hx711.h"
#include "wiringPi.h"

//=====================================================================================
// Constructor
//=====================================================================================

AdcDriverHx711::AdcDriverHx711( int dclk, const Pins& pins, bool reset_adc,
                                int gain_mode )
    : dclk_( dclk ), gain_mode_( gain_mode )
{
    if ( gain_to_pulse( gain_mode_ ) < 0 )
        gain_mode_ = 128;
    std::unique_copy( pins.begin(), pins.end(), std::back_inserter( pins_ ) );
    wiringPiSetupGpio();
    pinMode( dclk_, OUTPUT );
    for ( auto pin : pins_ )
        {
            pinMode( pin, INPUT );
        }
    if ( reset_adc )
        {
            reset();
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

bool AdcDriverHx711::set_gain_mode( int gain )
{
    if ( gain_to_pulse( gain ) < 0 )
        {
            return false;
        }

    gain_mode_ = gain;
    return true;
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

    // set the gain_mode for the next reading (between 1 and 3 pulses)
    for ( int i = 0; i < gain_to_pulse( gain_mode_ ) - 24; i++ )
        {
            usleep( 0.2 );  // minimum high/low duration is 0.2us, potentially measure
                            // the frequency and adjust values.
            digitalWrite( dclk_, HIGH );
            usleep( 0.2 );
            digitalWrite( dclk_, LOW );
        }
    for ( auto& adc_val : measurement )
        {
            // because  24 bit signed value on 32 bit int
            adc_val = adc_val ^ 0x800000;
        }
    return measurement;
}

void AdcDriverHx711::reset()
{
    digitalWrite( dclk_, HIGH );
    usleep( 60 );
    digitalWrite( dclk_, LOW );
}


int AdcDriverHx711::gain_to_pulse( int gain ) const
{
    switch ( gain )
        {
            case 128 :
                return 25;

            case 64 :
                return 26;
            case 32 :
                return 27;

            default :
                return -1;
        }
}
