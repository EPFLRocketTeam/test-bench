/**
 * @file adc_driver_hx711.h
 * @author Simon Thür (simon.thur@epfl.ch)
 * @brief Driver for multiple HX711 ADCs (with common clock)
 * @version 0.3
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __ADC_DRIVER_HX711_H
#define __ADC_DRIVER_HX711_H

#include <vector>

// List of signed ADC read values.
typedef std::vector<int> Measurement;
// List of pins
typedef std::vector<int> Pins;
// List of measuement offset values
typedef std::vector<int> Offset;

/**
 * @brief Driver for HX711 ADCs connected to the same clock.
 *
 * @details concerning pins, only internal checks are completed when changing
 * the pin configuration. If multiple instances of this class are in use or if
 * pins are addressed outside this class, it is the responsibility of the user
 * to assert correct functioning of the programm.
 *
 */
class AdcDriverHx711
{
  public:
    //=========================================================================
    // Constructor
    //=========================================================================
    /**
     * @brief Construct a new Adc Driver Hx711 object to handle ADC input.
     *
     * @param dclk The clock pin to be used. All ADC pins listed in @p pins
     * must be connected to the same clock pin.
     * @param pins The pins to which the ADC data pins are connected. Notably,
     * the ADC readings will be read and returned in the same order as the
     * pins.
     * @param reset_adc If true, the ADCs will be reset.
     * @param zero_measurements Number of measurements to be made to average
     * out as zero offset. To forego this step, set measurements to equal 0 (or
     * negative). To set a custom offset, use the method `void
     * AdcDriverHx711::offset( const Offset& offset )`
     * @param gain_mode Set the gain of for the ADCs. Note that gains are tied
     * to input input channels. (See corresponding datasheet.)
     *                      Gain:   Channel
     *                      - 128   A (Default)
     *                      - 64    A
     *                      - 32    B
     *
     */
    AdcDriverHx711( int dclk, const Pins& pins, bool reset_adc = true,
                    int zero_measurements = 10, int gain_mode = 128 );


    //=========================================================================
    // Getters
    //=========================================================================
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

    /**
     * @brief Return current offset values.
     *
     * @return const Offset&
     */
    const Offset& offset() const;

    /**
     * @brief Return current gain mode.
     *
     * @return int (See corresponding datasheet.)
     *                      Gain:   Channel
     *                      - 128   A (Default)
     *                      - 64    A
     *                      - 32    B
     */
    int gain_mode() const;


    //=========================================================================
    // Driver settings
    //=========================================================================
    /**
     * @brief Add an ADC input pin to the driver. It will be positioned last.
     *
     * If the @p pin is already used as input pin in the driver, it will not be
     * added. If the @p pin is used as dclk, it will not be added.
     *
     * @attention If @p force is set, the pin is added to the back regardless
     * of current pin configurations and it is up to the user to assert correct
     * execution of the program.
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
     * @attention If @p force is set, the clock pin will be changed regardless
     * of current pin configuration and it is up to the user to assert correct
     * execution of the program.
     *
     * @param pin
     * @param force
     * @return true if the clock pin was updated.
     * @return false if the operation was aborted.
     */
    bool set_dclk( int pin, bool force = false );

    /**
     * @brief Set the gain mode on the HX711 ADC.
     *
     * @param gain Set the gain of for the ADCs. Note that gains are tied to
     * input input channels. (See corresponding datasheet.)
     *              Gain:   Channel
     *              - 128   A (Default)
     *              - 64    A
     *              - 32    B
     * @note that the configuration will be applied AFTER the next
     * read operation.
     *
     * @return True if the gain mode was updated.
     */
    bool set_gain_mode( int gain );

    /**
     * @brief Set reading offset such that current reading returns 0.
     *
     * @param nbr_measurements Number of readings to average over.
     */
    void read_zero( int nbr_measurements = 10 );

    /**
     * @brief set offset. (missing values will be set to 0. Additional values
     * are are dropped. The offset value must be in the same order as the
     *        corresponding pins. offset[0] will be applied to the measurement
     * of pin[0].
     *
     */
    void offset( const Offset& offset );


    //=========================================================================
    // ADC interactions
    //=========================================================================
    /**
     * @brief Checks if all ADCs are ready to deliver data.
     *
     * @return true if all data is ready
     * @return false if at least one ADC is not ready.
     */
    bool data_ready();

    /**
     * @brief Read the signed value of all ADCs listed in pins_ in the same
     * order as the pins are listed.
     *
     * @return Measurement (std::vector<int> of signed ADC values)
     */
    Measurement read();

    /**
     * @brief Send Reset signal to ADCs
     *
     */
    void reset();


  protected:
    int    dclk_;
    Pins   pins_;
    Offset offset_;
    int    gain_mode_;
    // bool data_ready( int pin );
    int gain_to_pulse( int gain ) const;

  private:
    // If the same driver is used in multiple places, it is preferable to use
    // only one driver and access it through pointers.
    AdcDriverHx711( const AdcDriverHx711& ) = delete;
    AdcDriverHx711()                        = delete;
};


#endif  // __ADC_DRIVER_HX711_H
