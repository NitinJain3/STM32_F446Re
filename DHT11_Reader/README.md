# DHT11 Reader for STM32F446RE

This project demonstrates how to interface a DHT11 temperature and humidity sensor with an STM32F446RE Nucleo board using STM32 HAL drivers.

---
![License](https://img.shields.io/badge/License-MIT-brightgreen)
![Microcontroller](https://img.shields.io/badge/Microcontroller-STM32-blue)
![Board](https://img.shields.io/badge/Board-NUCLEOF44RE-blue)
![Sensor](https://img.shields.io/badge/Sensor-DHT11-orange)


![Status](https://img.shields.io/badge/Status-working-brightgreen)
![Testing](https://img.shields.io/badge/Tested-brightgreen)
![Project_status](https://img.shields.io/badge/Project-complete-Green)


---
# Board
![IMG_20250524_011214305~2](https://github.com/user-attachments/assets/a72b5bb7-3722-4016-9690-0af65d2477d1)



## Features

- Reads humidity and temperature from DHT11 sensor
- Validates sensor data using checksum
- Outputs data to UART using redirected `printf`
- Microsecond-level delay using DWT (Data Watchpoint and Trace Unit)
- LED toggle to indicate successful data reception

---

##  DHT11 Timing Overview

DHT11 uses a single-wire protocol with **strict timing requirements**:

### MCU Start Signal:
- LOW for ≥18 ms  
- HIGH for 20–40 µs  

### Sensor Response:
- LOW 80 µs  
- HIGH 80 µs  

### Data Transmission (40 bits total):
- Each bit starts with a 50 µs LOW pulse  
- `0` bit: 26–28 µs HIGH  
- `1` bit: ~70 µs HIGH  

**Accurate microsecond delays are mandatory** for:
- Differentiating between `0` and `1`
- Properly receiving the 5-byte data frame
- Handling the initial sensor response

This project uses **DWT-based delay** (`DWT->CYCCNT`) to achieve accurate timing.

---

##  Tools Used

1. STM32CubeMX + STM32CubeIDE  
2. STM32 HAL drivers  
3. NUCLEO-F446RE development board  
4. DHT11 temperature and humidity sensor  
5. Docklight / PuTTY / Arduino Serial Monitor (for viewing UART logs)

---

##  Project Folder Structure
<pre lang="markdown"><code>```text 
  STM32_F446Re/ 
  └── dht11_reader/ 
  ├── Core/
  │ ├── Inc/ 
  │ │ ├── dht11.h 
  │ │ └── ... │ 
  ├── Src/  
  │ │ ├── dht11.c 
  │ │ ├── main.c │
  │ └── ... 
  ├── Drivers/ 
  │ └── ... 
  ├── .ioc (CubeMX project file) 
  └── README.md ``` </code></pre>

---

## Debug Logging – `debug.h`

This project includes a lightweight debug interface through debug.h, which uses macro-based logging levels:

```c
#define DEBUG_INFO(fmt, ...)   printf("INFO: " fmt, ##__VA_ARGS__)
#define DEBUG_PRINT(fmt, ...)  printf("DEBUG: " fmt, ##__VA_ARGS__)
#define DEBUG_ERROR(fmt, ...)  printf("ERROR: " fmt, ##__VA_ARGS__)
```

These macros help to:
1. Clearly separate **INFO**, **DEBUG**, and **ERROR** messages  
2. Keep source files clean and maintainable  
3. Easily enable or disable logs by commenting/uncommenting the macros

#  Output
![image](https://github.com/user-attachments/assets/978cdf07-266a-4a0a-a2ce-4c44ffed3eba)

# Results with Reference Temperature Meter
RH = ±10% observed(Callibration maybe required)  
Temp: ±1 Observed ( Okay)  

![IMG_20250524_012640089~2](https://github.com/user-attachments/assets/05d69988-db3c-4950-81e3-2f0995392540)



# License
This project is licensed under the MIT License. See the LICENSE file for details.

# Author
Nitin R
[LinkedIn](https://www.linkedin.com/in/nitin-r-9733n)
