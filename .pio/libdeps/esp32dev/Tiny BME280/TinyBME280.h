/******************************************************************************
TinyBME280.h

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __TINY_BME280_H__
#define __TINY_BME280_H__

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if defined(TINY_BME280_SPI)
#include <SPI.h>
#ifndef TINY_BME280_SPI_CLOCK
#define TINY_BME280_SPI_CLOCK 1000000
#endif
#ifndef TINY_BME280_SPI_MODE
#define TINY_BME280_SPI_MODE SPI_MODE0
#endif

#elif defined(TINY_BME280_I2C)
#include <Wire.h>

//Uncomment the following line to enable software I2C
//You will need to have the SoftwareWire library installed
//#include <SoftwareWire.h> //SoftwareWire by Testato. Installed from library manager.

#define NO_WIRE 0
#define HARD_WIRE 1
#define SOFT_WIRE 2

#else
#error "Missing define TINY_BME280_SPI or TINY_BME280_I2C"
#endif

#include "lib/TinyBME280Impl.h"
#include "lib/TinyBME280Impl.cpp"

#endif  // End of __TINY_BME280_H__ definition check
