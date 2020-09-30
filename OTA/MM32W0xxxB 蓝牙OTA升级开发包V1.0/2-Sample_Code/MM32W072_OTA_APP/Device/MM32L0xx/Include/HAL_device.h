/**************************************************************************//**
* @file HAL_device.h
* @brief CMSIS Cortex-M Peripheral Access Layer for MindMotion
*        microcontroller devices
*
* This is a convenience header file for defining the part number on the
* build command line, instead of specifying the part specific header file.
*
* Example: Add "-DMM32X031" to your build options, to define part
*          Add "#include "HAL_device.h" to your source files
*
*
* @version 1.5.0
*
*
*****************************************************************************/

#ifndef __HAL_device_H
#define __HAL_device_H

//#define  MM32L072PF

#if defined(MM32L072PF)
#error MM32L072PF_dontwantincludethisfile
#include "MM32L072PF.h"
//#include "system_MM32L072PF.h"

#elif defined(MM32L073PF)
//#error  dontwantincludethisfile
#define MM32L0xxxx
#include "MM32L073PF.h"
//#include "system_MM32L073PF.h"

#elif defined(MM32X031)
#include "MM32X031.h"
#error MM32X031_dontwantincludethisfile
//#include "system_MM32X031.h"

#else
#error "HAL_device.h: PART NUMBER undefined"
#endif
#endif /* __HAL_device_H */
/*-------------------------(C) COPYRIGHT 2016 MindMotion ----------------------*/
