/**
 * @file adc_driver_hx711.h
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Driver for multiple HX711 ADCs (with common clock)
 * @version 0.1
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef adc_driver_hx711_h
#define adc_driver_hx711_h

#include <vector>

// List of signed ADC read values.
typedef std::vector<int> Measurement;
// List of pins
typedef std::vector<int> Pins;


/**
 * @brief Driver for HX711 ADCs connected to the same clock.
 *
 * @details concerning pins, only internal checks are completed when changing the pin
 * configuration. If multiple instances of this class are in use or if pins are
 * addressed outside this class, it is the responsibility of the user to assert correct
 * functioning of the programm.
 *
 */
class AdcDriverHx711
{
  public:
    //=================================================================================
    // Constructor
    //=================================================================================
    /**
     * @brief Construct a new Adc Driver Hx711 object to handle ADC input.
     *
     * @param dclk The clock pin to be used. All ADC pins listed in @p pins must be
     *             connected to the same clock pin.
     * @param pins The pins to which the ADC data pins are connected. Notably, the
     *             ADC readings will be read and returned in the same order as the
     *             pins.
     */
    AdcDriverHx711( int dclk, const Pins& pins );


    //=================================================================================
    // Getters
    //=================================================================================
    /**
     * @brief Read-only access to the vector of currently used pins.
     *
     * @return const Pins& pins_
     */
    const Pins& pins() const;

    /**
     * @brief Get the pin currently used as clock.
     *
     * @return int dclk_
     */
    int dclk_pin() const;


    //=================================================================================
    // Driver settings
    //=================================================================================
    /**
     * @brief Add an ADC input pin to the driver. It will be positioned last.
     *
     * If the @p pin is already used as input pin in the driver, it will not be added.
     * If the @p pin is used as dclk, it will not be added.
     *
     * @attention If @p force is set, the pin is added to the back regardless of
     *            current pin configurations and it is up to the user to assert correct
     *            execution of the program.
     *
     * @param pin
     * @param force
     * @return true if the pin was added to the driver.
     * @return false if the operation was aborted.
     */
    bool add_pin( int pin, bool force = false );

    /**
     * @brief Remove an ADC input pin from driver.
     *
     * @param pin
     * @return true if pin was removed
     * @return false if pin was not removed (pin was not listed as ADC input)
     */
    bool remove_pin( int pin );

    /**
     * @brief Set the clock pin.
     *
     * If the @p pin is used as an ADC input pin, the operation is aborted.
     *
     * @attention If @p force is set, the clock pin will be changed regardless of
     *            current pin configuration and it is up to the user to assert correct
     *            execution of the program.
     *
     * @param pin
     * @param force
     * @return true if the clock pin was updated.
     * @return false if the operation was aborted.
     */
    bool set_dclk( int pin, bool force = false );


    //=================================================================================
    // ADC interactions
    //=================================================================================
    /**
     * @brief Checks if all ADCs are ready to deliver data.
     *
     * @return true if all data is ready
     * @return false if at least one ADC is not ready.
     */
    bool data_ready();

    /**
     * @brief Read the signed value of all ADCs listed in pins_ in the same order as
     *        the pins are listed.
     *
     * @return Measurement (std::vector<int> of signed ADC values)
     */
    Measurement read();


  protected:
    int  dclk_;
    Pins pins_;
    // bool data_ready( int pin );

  private:
    // If the same driver is used in multiple places, it is preferable to use only one
    // driver and access it through pointers.
    AdcDriverHx711( const AdcDriverHx711& ) = delete;
    AdcDriverHx711()                        = delete;
};


#endif  // adc_driver_hx711_h
