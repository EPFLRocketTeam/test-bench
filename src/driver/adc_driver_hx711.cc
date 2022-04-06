/**
 * @file adc_driver_hx711.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Driver for multiple HX711 ADCs (with common clock)
 * @version 0.3
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <unistd.h>

#include <algorithm>
#include <cmath>

#include "adc_driver_hx711.h"

#ifdef TESTENV
#include "wiringPi_test_env.h"
#else
#include "wiringPi.h"
#endif  // TESTING


//=====================================================================================
// Constructor
//=====================================================================================

AdcDriverHx711::AdcDriverHx711( int dclk, const Pins& pins, bool reset_adc,
                                int zero_measurements, int gain_mode )
    : dclk_( dclk ), gain_mode_( gain_mode )
{
    if ( gain_to_pulse( gain_mode_ ) < 0 )
        gain_mode_ = 128;
    std::unique_copy( pins.begin(), pins.end(), std::back_inserter( pins_ ) );
    wiringPiSetupPhys();
    pinMode( dclk_, OUTPUT );
    for ( auto pin : pins_ )
        {
            pinMode( pin, INPUT );
        }
    offset_ = Offset( pins_.size(), 0 );
    if ( reset_adc )
        {
            reset();
            read();  // sets gain-mode
        }
    if ( zero_measurements > 0 )
        {
            read_zero( zero_measurements );
        }
}

void AdcDriverHx711::offset( const Offset& offset )
{
    offset_ = offset;
    for ( size_t i = offset_.size(); i < pins_.size(); i++ )
        {
            offset_.push_back( 0 );
        }
    while ( offset_.size() > pins_.size() )
        {
            offset_.pop_back();
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
const Offset& AdcDriverHx711::offset() const
{
    return offset_;
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
            offset_.push_back( 0 );
            return true;
        }
}


bool AdcDriverHx711::remove_pin( int pin )
{
    auto remove = std::find( pins_.begin(), pins_.end(), pin );
    if ( remove == pins_.end() )
        {
            return false;
        }
    else
        {
            pins_.erase( remove, pins_.end() );
            offset_.erase( offset_.begin() + std::distance( pins_.begin(), remove ) );
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
            offset_.erase( offset_.begin()
                           + std::distance( pins_.begin(), existing_pin ) );
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

void AdcDriverHx711::read_zero( int nbr_measurements )
{
    if ( nbr_measurements > 0 )
        {
            offset_ = Offset( pins_.size(), 0 );
            Offset offset_sum( pins_.size(), 0 );
            for ( int cnt = 0; cnt < nbr_measurements; cnt++ )
                {
                    Measurement reading = read();
                    for ( unsigned i = 0; i < pins_.size(); i++ )
                        {
                            offset_sum[ i ] += reading[ i ];
                        }
                }
            for ( unsigned i = 0; i < offset_.size(); i++ )
                {
                    offset_[ i ] = -offset_sum[ i ] / nbr_measurements;
                }
        }
}

//=====================================================================================
// ADC interactions
//=====================================================================================
bool AdcDriverHx711::data_ready()
{
    return std::none_of( pins_.begin(), pins_.end(), digitalRead );
}

Measurement AdcDriverHx711::read()
{
    Measurement measurement( pins_.size(), 0 );
    while ( ! data_ready() ) usleep( 0.1 );
    for ( int cnt = 0; cnt < 24; cnt++ )
        {
            digitalWrite( dclk_, HIGH );
            for ( unsigned i = 0; i < pins_.size(); i++ )
                {
                    measurement[ i ] = measurement[ i ] << 1;
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
    for ( size_t i = 0; i < measurement.size(); i++ )
        {
            // because  24 bit signed value on 32 bit int (sign extend)
            measurement[ i ] = ( measurement[ i ] & 0x800000 )
                                 ? measurement[ i ] | 0xFF000000
                                 : measurement[ i ];
            measurement[ i ] += offset_[ i ];
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
                return 27;

            case 32 :
                return 26;

            default :
                return -1;
        }
}
