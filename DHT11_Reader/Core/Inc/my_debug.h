/**
 ******************************************************************************
 * @file    :	my_debug.h
 * @author  :	Nitin
 * @brief   :	Debug logging utility macros for STM32 firmware projects.
 *          	Allows enabling/disabling debug prints based on a macro.
 *          	Supports info, warning, error, and debug levels.
 * @version : 1.0
 * @date    : May 21, 2025
 ******************************************************************************
 * @attention
 *
 * This file is provided as-is for educational or project development use.
 * Use with caution in production builds — ensure debug is disabled for release.
 *
 ******************************************************************************
 */

#ifndef INC_MY_DEBUG_H_
#define INC_MY_DEBUG_H_

#include <stdio.h>
#include "main.h"

/* Set to 1 to enable debug prints, 0 to disable */
#define MY_DEBUG (0)

#if MY_DEBUG != 0

    /* Helper macros for log levels */
    #define DEBUG_INFO(fmt, ...) \
        (void)printf("[INFO]  [%lu] " fmt, HAL_GetTick(), ##__VA_ARGS__)

    #define DEBUG_WARN(fmt, ...) \
        (void)printf("[WARN]  [%lu] " fmt, HAL_GetTick(), ##__VA_ARGS__)

    #define DEBUG_ERROR(fmt, ...) \
        (void)printf("[ERROR] [%lu] " fmt, HAL_GetTick(), ##__VA_ARGS__)

    #define DEBUG_PRINT(fmt, ...) \
        (void)printf("[DEBUG] [%lu] " fmt, HAL_GetTick(), ##__VA_ARGS__)

#else

    /* Macros expand to no-op when debugging is disabled */
    #define DEBUG_INFO(...)       ((void)0)
    #define DEBUG_WARN(...)       ((void)0)
    #define DEBUG_ERROR(...)      ((void)0)
    #define DEBUG_PRINT(...)      ((void)0)

#endif /* MY_DEBUG */

#endif /* INC_MY_DEBUG_H_ */
