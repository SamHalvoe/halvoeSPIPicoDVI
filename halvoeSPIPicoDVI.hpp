#pragma once

#include <SPI.h>

#ifdef ARDUINO_ARCH_RP2040
  #define HALVOE_SPI_CLASS SPIClassRP2040
  #define HALVOE_SPI_DEFAULT SPI
#elif __IMXRT1062__
  #define HALVOE_SPI_CLASS SPIClass
  #define HALVOE_SPI_DEFAULT SPI1
#else
  #error Unsupported MCU!
#endif

#define HALVOE_DVI_DEBUG

namespace halvoeDVI
{
  constexpr const size_t FRAME_SIZE = 240 * 320;
  constexpr const uint16_t COLOR_COUNT = 256;

  const SPISettings SPI_DEFAULT_SETTINGS(1000000, MSBFIRST, SPI_MODE0);
}
