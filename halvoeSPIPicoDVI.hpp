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
  const size_t FRAME_HEIGHT = 240;
  const size_t FRAME_WIDTH = 320;
  const size_t FRAME_SIZE = FRAME_HEIGHT * FRAME_WIDTH;
  const uint16_t COLOR_COUNT = 256;

  const SPISettings SPI_DEFAULT_SETTINGS(24000000, MSBFIRST, SPI_MODE0);
}
