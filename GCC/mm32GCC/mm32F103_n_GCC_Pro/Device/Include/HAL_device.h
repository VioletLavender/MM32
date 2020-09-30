/**************************************************************************//**
 * @file HAL_device.h
 * @brief CMSIS Cortex-M Peripheral Access Layer for MindMotion
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * Example: Add "-DMM32X103" to your build options, to define part
 *          Add "#include "HAL_device.h" to your source files
 *
 *
 * @version 4.2.1
 *
 *
 *****************************************************************************/

#ifndef __HAL_device_H
#define __HAL_device_H


#define MM32F103

#if defined(MM32F103)
#include "MM32F103.h"
#include "SYSTEM_MM32F103.h"
#else
#error "HAL_device.h: PART NUMBER undefined"
#endif


#include "HAL_rcc.h"
#include "HAL_gpio.h"
#include "HAL_uart.h"
#include "HAL_misc.h"

#endif /* HAL_device_H */
