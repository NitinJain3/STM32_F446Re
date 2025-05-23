/**
 ******************************************************************************
 * @file           : dht11.h
 * @brief          : Header file for DHT11 temperature and humidity sensor
 *                   interface using STM32 HAL library.
 *
 *                   This module provides low-level functions to communicate
 *                   with the DHT11 sensor using a single GPIO pin. It includes
 *                   microsecond-level delays, pin direction switching, and
 *                   reading sensor data bit-by-bit.
 *
 *                   Make sure TIM6 (or any timer used for delay_us) is
 *                   initialized before calling delay_us.
 *
 * @author         : Nitin R
 * @version        : 1.0
 * @date           : May 21, 2025
 ******************************************************************************
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "main.h"

/**
 * @brief Initializes the DWT cycle counter for microsecond delays.
 *        (Used only if using DWT for delay_us instead of TIM6).
 */
void DWT_Init(void);

/**
 * @brief Generates a delay in microseconds.
 * @param us: Duration of delay in microseconds.
 * @note Requires a properly initialized TIM6 or DWT.
 */
void delay_us(uint32_t us);

/**
 * @brief Configures the DHT11 data pin as open-drain output.
 *        Required for pulling the line low to start communication.
 */
void DHT11_SetPinOutput(void);

/**
 * @brief Configures the DHT11 data pin as input with pull-up.
 *        Required to read the sensor's response and data bits.
 */
void DHT11_SetPinInput(void);

/**
 * @brief Sends the start signal to the DHT11 sensor.
 *        Pulls the data pin LOW for ≥18 ms, then HIGH for 20–40 us.
 */
void DHT11_Start(void);

/**
 * @brief Checks the sensor's response after start signal.
 * @retval 1 if response is valid (LOW then HIGH), 0 otherwise.
 */
uint8_t DHT11_CheckResponse(void);

/**
 * @brief Reads 1 byte of data from the sensor.
 *        Each bit is determined based on pulse width timing.
 * @retval 8-bit data (humidity/temperature or checksum)
 */
uint8_t DHT11_ReadByte(void);

/**
 * @brief Reads temperature and humidity data from the DHT11 sensor and prints the values.
 *
 * This function initiates communication with the DHT11 sensor, reads the 5 bytes of data
 * (humidity integer, humidity decimal, temperature integer, temperature decimal, and checksum),
 * verifies the checksum, and then displays the values via UART using printf.
 * It also toggles an LED on successful data reception.
 * The function includes a 2-second delay between each read, suitable for calling inside a loop.
 */
void ReadAndDisplayDHT11(void);

#endif /* DHT11_H_ */
