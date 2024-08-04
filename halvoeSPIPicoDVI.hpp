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

  const size_t COMMAND_BUFFER_SIZE = 16384; // in bytes (CommandBuffer uses uint8_t)
  const SPISettings SPI_DEFAULT_SETTINGS(24000000, MSBFIRST, SPI_MODE0);

  enum class GFXCommand : uint16_t
  {
    invalid = 0,
    swap,
    fillScreen,
    fillRect
  };

  GFXCommand toGFXCommand(uint16_t in_value)
  {
    switch (static_cast<GFXCommand>(in_value))
    {
      case GFXCommand::invalid:
      case GFXCommand::swap:
      case GFXCommand::fillScreen:
      case GFXCommand::fillRect:
        return static_cast<GFXCommand>(in_value);
    }

    return GFXCommand::invalid;
  }

  uint16_t fromGFXCommand(GFXCommand in_command)
  {
    switch (in_command)
    {
      case GFXCommand::invalid:
      case GFXCommand::swap:
      case GFXCommand::fillScreen:
      case GFXCommand::fillRect:
        return static_cast<uint16_t>(in_command);
    }

    return static_cast<uint16_t>(GFXCommand::invalid);
  }
}
