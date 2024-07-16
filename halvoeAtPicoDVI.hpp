#pragma once

#include <cstring>
#include <PicoDVI.h>
#include <SPISlave.h>

#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtPico
{
  // Here's how a 320x240 8-bit (color-paletted) framebuffer is declared.
  // Second argument ('true' here) enables double-buffering for flicker-free
  // animation. Third argument is a hardware configuration -- examples are
  // written for Adafruit Feather RP2040 DVI, but that's easily switched out
  // for boards like the Pimoroni Pico DV (use 'pimoroni_demo_hdmi_cfg') or
  // Pico DVI Sock ('pico_sock_cfg').
  DVIGFX8 dviGFX(DVI_RES_320x240p60, true, adafruit_feather_dvi_cfg);

  constexpr const pin_size_t SPI_DEFAULT_PIN_RX = D12;
  constexpr const pin_size_t SPI_DEFAULT_PIN_CS = D13;
  constexpr const pin_size_t SPI_DEFAULT_PIN_SCK = D10;
  constexpr const pin_size_t SPI_DEFAULT_PIN_TX = D11;

  constexpr const size_t CANVAS_SIZE = 240 * 320;
  constexpr const uint16_t COLOR_COUNT = 256;

  void setupDefaultPalette()
  {
    for (uint16_t index = 0; index < COLOR_COUNT; ++index)
    {
      dviGFX.setColor(index, index, index, index);
    }

    dviGFX.swap(false, true); // Duplicate same palette into front & back buffers
  }

  void receiveCallback(uint8_t* in_data, size_t in_size)
  {
    static size_t bufferOffset = 0;

    if (bufferOffset >= CANVAS_SIZE)
    {
      bufferOffset = 0;
      dviGFX.swap();
    }

    std::memcpy(dviGFX.getBuffer() + bufferOffset, in_data, in_size);

    if (bufferOffset + in_size < CANVAS_SIZE)
    {
      bufferOffset = in_size;
    }
  }

  class SPILink
  {
    private:
      SPISlaveClass& m_spiInterface;

    public:
      SPILink(SPISlaveClass& io_spiInterface = SPISlave1) : m_spiInterface(io_spiInterface)
      {}

      // These set methods panic the core, if set to invalid pins!!!
      bool setPins(pin_size_t in_rx, pin_size_t in_cs, pin_size_t in_sck, pin_size_t in_tx)
      {
        if (not m_spiInterface.setRX(in_rx)) { return false; }
        if (not m_spiInterface.setCS(in_cs)) { return false; }
        if (not m_spiInterface.setSCK(in_sck)) { return false; }
        if (not m_spiInterface.setTX(in_tx)) { return false; }
        return true;
      }

      bool setDefaultPins()
      {
        return setPins(SPI_DEFAULT_PIN_RX, SPI_DEFAULT_PIN_CS, SPI_DEFAULT_PIN_SCK, SPI_DEFAULT_PIN_TX);
      }

      bool begin(const SPISettings& in_settings = SPI_DEFAULT_SETTINGS)
      {
        if (not dviGFX.begin()) { return false; }
        setupDefaultPalette();
        
        if (not setDefaultPins()) { return false; }
        m_spiInterface.onDataRecv(receiveCallback);
        //m_spiInterface.onDataSent(sentCallback);
        m_spiInterface.begin(in_settings);

        return true;
      }
  };
}
