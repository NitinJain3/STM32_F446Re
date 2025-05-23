/**
 ******************************************************************************
 * @file           : dht11.c
 * @brief          : Source file for DHT11 temperature and humidity sensor
 *                   interface using STM32 HAL library.
 *
 *                   This file includes all the necessary functions to
 *                   communicate with the DHT11 sensor: sending start signal,
 *                   checking response, reading data bytes, and microsecond
 *                   delay functions using DWT (Data Watchpoint and Trace).
 *
 * @author         : Nitin R
 * @version        : 1.0
 * @date           : May 21, 2025
 ******************************************************************************
 */

#include "dht11.h"
#include <stdio.h>  /* Required for printf */
#include "my_debug.h"
/**
 * @brief Initializes the DWT (Data Watchpoint and Trace) cycle counter.
 *        Used for precise microsecond delay generation.
 */
void DWT_Init(void) {
	/* Enable trace */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* Reset cycle counter */
	DWT->CYCCNT = 0U;

	/* Enable cycle counter */
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	/* DEBUG_PRINT("DWT Initialized for microsecond delay\n"); */
}

/**
 * @brief Delays the code execution for given microseconds.
 * @param us: Delay in microseconds
 * @note Requires DWT_Init() to be called once during setup.
 */
void delay_us(uint32_t us) {
	uint32_t startTick = DWT->CYCCNT;
	uint32_t delayTicks = us * (SystemCoreClock / 1000000); /* Convert us to CPU cycles */
	while ((DWT->CYCCNT - startTick) < delayTicks)
		;
}

/**
 * @brief Configures the DHT11 data pin as an open-drain output.
 *        This is required when initiating communication by pulling the line low.
 */
void DHT11_SetPinOutput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = DHT_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; /* Open-drain output */
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT_PIN_GPIO_Port, &GPIO_InitStruct);
	/*DEBUG_PRINT("DHT11 Pin set to OUTPUT mode\n");*/
}

/**
 * @brief Configures the DHT11 data pin as input.
 *        This allows the microcontroller to read data sent by the sensor.
 */
void DHT11_SetPinInput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = DHT_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT_PIN_GPIO_Port, &GPIO_InitStruct);
	/*DEBUG_PRINT("DHT11 Pin set to INPUT mode\n");*/
}

/**
 * @brief Sends the start signal to the DHT11 sensor.
 *        Pulls the pin LOW for ≥18ms and then HIGH for 20-40us,
 *        followed by setting pin to input mode to receive response.
 */
void DHT11_Start(void) {
	DEBUG_PRINT("Sending DHT11 start signal...\r\n");

	/* Set as output */
	DHT11_SetPinOutput();

	/* Pull LOW */
	HAL_GPIO_WritePin(DHT_PIN_GPIO_Port, DHT_PIN_Pin, GPIO_PIN_RESET);

	/* Delay ≥18 ms (Previously 18 ms; increased to 20 ms for reliability) */
	HAL_Delay(18U);

	/* Pull HIGH */
	HAL_GPIO_WritePin(DHT_PIN_GPIO_Port, DHT_PIN_Pin, GPIO_PIN_SET);

	/* Wait for 20–40 us (Previously 30 us; increased to 40 us) */
	delay_us(30U);

	/* Set as input to read response */
	DHT11_SetPinInput();

	DEBUG_INFO("DHT11 Start signal sent\n");

	/* Optionally check pin state
	 * DEBUG_PRINT("Pin state before response wait: %d\r\n", HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin));
	 */
}

/**
 * @brief Checks the DHT11 sensor’s response after the start signal.
 * @retval 1 if sensor responds correctly, 0 otherwise.
 * @note DHT11 responds with LOW for 80us, then HIGH for 80us.
 */

uint8_t DHT11_CheckResponse(void) {
	/*DEBUG_PRINT("Pin state before response wait: %d\r\n", HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin)); */

	uint8_t response = 0;
	uint32_t timeout = 100; /*100 us max wait */

	/*Wait for LOW signal from sensor*/
	while (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin) && timeout--)
		delay_us(1);

	/*	 After start signal, DHT11 pulls LOW within 20-40us */

	/*delay_us(40); */

	if (!(HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin))) /*Expect LOW */
	{
		DEBUG_INFO("DHT11 pulled line LOW (expected)\r\n");
		delay_us(80);
		if (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin)) /* Then HIGH */
		{
			response = 1;
		}
	}
	/* Wait for HIGH to end (DHT11 keeps HIGH for ~80us) */

	/*while (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin));*/

	timeout = 100;
	while (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin) && timeout--)
		delay_us(1);

	/* Debug print moved AFTER timing critical operations */
	if (response) {
		DEBUG_INFO("DHT11 Response OK (Initial LOW + HIGH confirmed)\n");
	} else {
		DEBUG_ERROR("DHT11 Response Failed (Initial LOW or HIGH missing)\n");
	}
	return response;
}

/**
 * @brief Reads one byte (8 bits) from the DHT11 sensor.
 *        Each bit is transmitted based on pulse width timing.
 * @retval The byte read (can be humidity, temperature, or checksum).
 * @note 0 is ~26-28us HIGH, 1 is ~70us HIGH after initial LOW.
 */
uint8_t DHT11_ReadByte(void) {
	uint8_t i;
	uint8_t byte = 0U;

	for (i = 0U; i < 8U; i++) {
		/* Wait for HIGH */
		while (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin)
				== GPIO_PIN_RESET) {
			/* Wait */
		}

		/* Wait for pulse duration */
		delay_us(40U);

		/* If still HIGH after 40us, it is a ‘1’ */
		if (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin) == GPIO_PIN_SET) {
			/* Set bit */
			byte |= (uint8_t) (1U << (7U - i));
		} else {
			/* Clear bit */
			byte &= (uint8_t) (~(1U << (7U - i)));
		}

		/* Wait for pin to go LOW */
		while (HAL_GPIO_ReadPin(DHT_PIN_GPIO_Port, DHT_PIN_Pin) == GPIO_PIN_SET) {
			/* Wait */
		}
	}

	DEBUG_PRINT("Read Byte: 0x%02X\n", byte);

	return byte;
}

/**
 * @brief Reads temperature and humidity data from the DHT11 sensor and displays it over UART.
 *
 * This function initiates communication with the DHT11 sensor by sending a start signal,
 * waits for a response, and reads 5 bytes of data:
 *   - Humidity integer part
 *   - Humidity decimal part
 *   - Temperature integer part
 *   - Temperature decimal part
 *   - Checksum (for data integrity validation)
 *
 * If the checksum is valid, it toggles an onboard LED (e.g., GPIOA Pin 5) and prints the
 * humidity and temperature readings via UART using both `printf` and `DEBUG_PRINT` macros.
 * If the checksum fails, it prints an error message.
 *
 * The function waits 2 seconds before returning to allow the DHT11 sensor to be ready
 * for the next read (as per DHT11 timing specifications).
 */

void ReadAndDisplayDHT11(void) {

	uint8_t rh_int;
	uint8_t rh_dec;
	uint8_t t_int;
	uint8_t t_dec;
	uint8_t checksum;
	HAL_Delay(1); /*  Give DHT11 time to stabilize */
	DHT11_Start(); /*  Send start signal to DHT11 */

	if (DHT11_CheckResponse()) {
		/** Read 5 bytes from DHT11: humidity integer, humidity decimal,
		 * temperature integer, temperature decimal, checksum */
		rh_int = DHT11_ReadByte();
		rh_dec = DHT11_ReadByte();
		t_int = DHT11_ReadByte();
		t_dec = DHT11_ReadByte();
		checksum = DHT11_ReadByte();

		/* Validate checksum */
		if (checksum == (rh_int + rh_dec + t_int + t_dec)) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			/* Insert a 100ms delay */
			/* HAL_Delay(100); */
			/* printf("Raw received1: %s\r\n", rx_buffer); */

			DEBUG_INFO("DHT11 Initialized\r\n");
			DEBUG_PRINT("Humidity: %d.%d %%\tTemperature: %d.%d °C\r\n", rh_int,
					rh_dec, t_int, t_dec);
			printf("Humidity: %d.%d %% RH \t Temperature: %d.%d deg C\r\n",
					rh_int, rh_dec, t_int, t_dec);
		} else {
			printf("DHT11 checksum error\r\n");
		}

		HAL_Delay(2000); /* Wait 2 seconds before next reading */
	}
}
