/**
 * @file main_csv.cc
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Program to create csv of measurements.
 * @version 0.1
 * @date 2022-04-12
 *
 * @copyright Copyright (c) 2022
 *
 */


#include <fstream>
#include <iostream>
#include <string>

#include "adc_driver_hx711.h"


#define DATA_PIN_0 11
#define DATA_PIN_1 26
#define DATA_PIN_2 18
#define DATA_PIN_3 22
#define DATA_PIN_4 29
#define DATA_PIN_5 32

#define DCLK_PIN 23


void read( AdcDriverHx711& drvr, const std::string& file_name,
           int nbr_of_measurements );
void update_file( std::string& file, unsigned& meas_nbr );
void edit_config( AdcDriverHx711& drvr, unsigned& measurements_per_reading );
void print_config( AdcDriverHx711& drvr, unsigned& measurements_per_reading );
void zero_adc( AdcDriverHx711& drvr );
void print_help();


int main( int argc, char const* argv[] )
{
    unsigned    meas_nbr        = 0;
    unsigned    nbr_of_readings = 50;
    std::string base_file;
    if ( argc > 1 )
        base_file = argv[ 1 ];
    else
        base_file = "test_bench_measurement";
    Pins adc_input_pins = { DATA_PIN_0, DATA_PIN_1, DATA_PIN_2,
                            DATA_PIN_3, DATA_PIN_4, DATA_PIN_5 };

    AdcDriverHx711 adc_driver( DCLK_PIN, adc_input_pins );

    std::cout << "Running " __FILE__
                 "\nIt is suggested to check (p) and edit (e) configuration "
                 "before starting.\nPress h for help"
              << std::endl;


    char action;
    for ( ;; )
        {
            std::cout << ">> ";
            std::cin >> action;
            switch ( action )
                {
                    case 'q' :
                        return 0;
                        break;
                    case 'r' :
                        read( adc_driver,
                              base_file + '_' + std::to_string( meas_nbr++ )
                                  + ".csv",
                              nbr_of_readings );
                        break;
                    case 'f' :
                        update_file( base_file, meas_nbr );
                        break;
                    case 'e' :
                        edit_config( adc_driver, nbr_of_readings );
                        print_config( adc_driver, nbr_of_readings );
                        break;
                    case 'p' :

                        print_config( adc_driver, nbr_of_readings );
                        std::cout << "CSV base file name: " << base_file
                                  << std::endl;
                        break;
                    case 'z' :
                        zero_adc( adc_driver );
                        break;
                    case 'h' :
                        print_help();
                        break;
                    default :
                        std::cout
                            << "NOT RECOGNIZED, valid inputs are:"
                               "\n(q)uit, (r)ead, (f)ile settings, "
                               "(e)dit configuration, "
                               "(p)rint configuration, (z)ero ADCs, (h)elp"
                            << std::endl;
                        ;
                        break;
                }
        }
    return 0;
}
void print_config( AdcDriverHx711& drvr, unsigned& measurements_per_reading )
{
    std::cout << "Current  configuration:\nPins:\n    Clock:\n        DCLK:  "
              << drvr.dclk_pin() << "\n    Data:             Offset:";
    for ( unsigned i = 0; i < drvr.pins().size(); i++ )
        {
            std::cout << "\n        PIN " << i << ": " << drvr.pins()[ i ]
                      << "         " << drvr.offset()[ i ];
        }
    std::cout << "\nGain:\n    " << drvr.gain_mode()
              << "\nMeasurements per reading:\n    "
              << measurements_per_reading << std::endl;
}


void read( AdcDriverHx711& drvr, const std::string& file_name,
           int nbr_of_measurements )
{
    std::ofstream file = std::ofstream( file_name );
    std::cout << "Starting measurements" << std::endl;
    for ( int i = 0; i < nbr_of_measurements; i++ )
        {
            std::cout << "Reading " << i << std::endl;
            for ( auto m : drvr.read() )
                {
                    file << m << ',';
                }
            file << std::endl;
        }
    file.close();
    std::cout << "Finished measurements" << std::endl;
}
void update_file( std::string& file, unsigned& meas_nbr )
{
    char        choice;
    std::string new_file;
    unsigned    new_meas_nbr;
    std::cout << "Current filename: " << file << std::endl
              << "Enter a new filename:\n>> ";
    std::cin >> new_file;
    std::cout << "Current enumeration: " << meas_nbr << std::endl
              << "Would you like to reset to 0? y/n/c (yes, no, custom)\n>> ";
    std::cin >> choice;
    switch ( choice )
        {
            case 'y' :
                new_meas_nbr = 0;
                break;
            case 'n' :
                new_meas_nbr = meas_nbr;
                break;
            case 'c' :
                std::cout << "Enter new enumeration number\n>> ";
                std::cin >> new_meas_nbr;
                break;
            default :
                std::cout << "Answer not recognized, assume \'n\'"
                          << std::endl;
                break;
        }
    std::cout << "\nNew configuration:\nFilename: " << new_file
              << "\nEnumeration: " << new_meas_nbr
              << "\n\nWould you like to accept changes? y/n\n>> ";
    std::cin >> choice;
    switch ( choice )
        {
            case 'y' :
                meas_nbr = new_meas_nbr;
                file     = new_file;
                std::cout << "Changes accepted" << std::endl;
                break;
            case 'n' :
                std::cout << "Changes discarded" << std::endl;
                break;
            default :
                std::cout << "Unknown argument, changes discarded"
                          << std::endl;
                break;
        }
}
void edit_config( AdcDriverHx711& drvr, unsigned& measurements_per_reading )
{
    unsigned new_measurements_per_reading = 0;
    int      new_clock                    = 0;
    int      new_gain                     = 0;
    int      add_pin                      = 0;
    int      rm_pin                       = 0;
    bool     valid                        = true;
    std::cout << "Do you wish to change the number of measurements per "
                 "reading? y/n\n>> ";
    char choice;
    std::cin >> choice;
    if ( choice == 'y' )
        {
            std::cout << "Currently, each reading consists of "
                      << measurements_per_reading
                      << " measurements.\nPlease enter the new number of"
                         "measurements:\n>> ";
            std::cin >> new_measurements_per_reading;
            std::cout << "Accept " << new_measurements_per_reading
                      << " measurements per reading? y/n\n>> ";
            std::cin >> choice;
            if ( choice == 'y' )
                {
                    measurements_per_reading = new_measurements_per_reading;
                    std::cout << "Changes accepted" << std::endl;
                }
            else
                {
                    std::cout << "Changes discarded";
                }
        }

    std::cout << "Do you wish to edit the gain mode? y/n\n>> ";
    std::cin >> choice;
    if ( choice == 'y' )
        {
            std::cout << "Current gain mode is " << drvr.gain_mode()
                      << ".\nAvailable modes are:\n"
                         " * Gain:   Channel\n"
                         " * 128     A( Default )\n"
                         " * 64      A\n"
                         " * 32      B "
                         "\n>> ";
            do
                {
                    std::cin >> new_gain;
                    std::cout << "Do you want to accept gain mode " << new_gain
                              << "? y/n\n>> ";
                    std::cin >> choice;
                    if ( choice == 'y' )
                        {
                            valid = drvr.set_gain_mode( new_gain );
                            if ( ! valid )
                                std::cout
                                    << new_gain
                                    << " was not a valid argument. Enter the "
                                       "desired gain mode\n>> ";
                            else
                                std::cout << "Gain mode updated" << std::endl;
                        }
                    else
                        {
                            std::cout << "Input discarded" << std::endl;
                            valid = true;
                        }
                }
            while ( ! valid );
        }


    std::cout << "Do you wish to change the clock pin? y/n\n>> ";
    std::cin >> choice;
    if ( choice == 'y' )
        {
            std::cout << "Current clock pin is " << drvr.dclk_pin()
                      << std::endl;
            do
                {
                    std::cout << "Enter new clock pin\n>> ";
                    std::cin >> new_clock;
                    std::cout << "Do you wish to accept pin " << new_clock
                              << " as clock pin? y/n\n>> ";
                    std::cin >> choice;
                    if ( choice == 'y' )
                        {
                            valid = drvr.set_dclk( new_clock );
                            if ( ! valid )
                                {
                                    std::cout << "Something went wrong, "
                                                 "attempt to force? y/n\n>> ";
                                    std::cin >> choice;
                                    if ( choice == 'y' )
                                        valid =
                                            drvr.set_dclk( new_clock, true );
                                }
                            if ( valid )
                                std::cout << "Clock was updated" << std::endl;
                            else
                                std::cout << "Clock was not updated"
                                          << std::endl;
                        }
                    else
                        {
                            valid = true;
                            std::cout << "Change discarded" << std::endl;
                        }
                }
            while ( ! valid );
        }
    bool more_input = true;
    do
        {
            std::cout << "Currently the following data pins are in use:\n";
            for ( auto p : drvr.pins() ) std::cout << p << std::endl;
            std::cout << "Do you wish to remove a data pin? y/n\n>> ";
            std::cin >> choice;
            if ( choice == 'y' )
                {
                    std::cout << "Which pin would you like to remove?\n>> ";
                    std::cin >> rm_pin;
                    std::cout << "Remove pin " << rm_pin << "? y/n\n>> ";
                    std::cin >> choice;
                    if ( choice == 'y' )
                        if ( drvr.remove_pin( rm_pin ) )
                            std::cout << "pin " << rm_pin << " removed"
                                      << std::endl;
                        else
                            std::cout << "Failed to remove pin " << rm_pin
                                      << std::endl;
                    else
                        std::cout << "Change discarded" << std::endl;
                }
            else
                more_input = false;
        }
    while ( more_input );
    more_input = true;
    do
        {
            std::cout << "Current data pins are\n";
            for ( auto p : drvr.pins() ) std::cout << p << std::endl;
            std::cout << "Do you wish to add a pin? y/n\n>> ";
            std::cin >> choice;
            if ( choice == 'y' )
                {
                    std::cout << "Enter the pin to add\n>> ";
                    std::cin >> add_pin;
                    std::cout << "Add pin " << add_pin << "? y/n\n>> ";
                    std::cin >> choice;
                    if ( choice == 'y' )
                        {
                            bool success = drvr.add_pin( add_pin );
                            if ( success )
                                {
                                    std::cout << "Pin " << add_pin
                                              << " appended" << std::endl;
                                    continue;
                                }
                            std::cout << "Failed to add pin " << add_pin
                                      << ", do you wish to force? y/n\n>> ";
                            std::cin >> choice;
                            if ( choice == 'y' )
                                if ( drvr.add_pin( add_pin, true ) )
                                    std::cout << "Pin" << add_pin
                                              << " appended " << std::endl;
                                else
                                    std::cout << "Failed to add pin "
                                              << add_pin << std::endl;
                            else
                                std::cout << "Change discarded" << std::endl;
                        }
                    else
                        std::cout << "Change discarded" << std::endl;
                }
            else
                more_input = false;
        }
    while ( more_input );
}
void zero_adc( AdcDriverHx711& drvr )
{
    std::cout << "Do you wish to recalibrate the driver? y/n\n>> ";
    char choice;
    std::cin >> choice;
    switch ( choice )
        {
            case 'y' :
                drvr.read_zero();
                std::cout << "Driver calibrated to zero." << std::endl;
                break;
            case 'n' :
                std::cout << "Request abandoned" << std::endl;
            default :
                std::cout << "Unknown command, request abandoned" << std::endl;
                break;
        }
}

void print_help()
{
    std::cout << "q: Quit cli"
                 "\n"
                 "r: Read ADC values $(NBR) times and write to "
                 "$(FILENAME)_$(RNBR++).csv"
                 "\n"
                 "f: Edit file related data: $(FILENAME) and $(RNBR)"
                 "\n"
                 "e: Edit driver configuration (one after the other in order):"
                 "\n"
                 "   - $(NBR): How many measurements to make per reading"
                 "\n"
                 "   - Gain mode at which to operate"
                 "\n"
                 "   - Clock pin (DCLK)"
                 "\n"
                 "   - Pin(s) to remove"
                 "\n"
                 "   - Pin(s) to append"
                 "\n"
                 "p: Print current driver and file configurations"
                 "\n"
                 "z: Read current values and set as zero"
                 "\n"
                 "h: prints out this help message"
              << std::endl;
    ;
}