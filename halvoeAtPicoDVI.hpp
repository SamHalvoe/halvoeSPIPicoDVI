#pragma once

#include <cstring>
#include <PicoDVI.h>
#include <SPISlave.h>

#include "halvoeInfo.hpp"
#include "halvoeSPIPicoDVI.hpp"

#ifndef ARDUINO_ARCH_RP2040
  #error halvoeDVI::AtPico does only work on RP2040!
#endif

#define DEFAULT_SPI_SLAVE_INSTANCE SPISlave
#define USED_SPI_SLAVE_INSTANCE DEFAULT_SPI_SLAVE_INSTANCE

namespace halvoeDVI::AtPico::Callback
{
  void onReceive(uint8_t* in_data, size_t in_size);
  void onSent();
}

namespace halvoeDVI::AtPico
{
  // Here's how a 320x240 8-bit (color-paletted) framebuffer is declared.
  // Second argument ('true' here) enables double-buffering for flicker-free
  // animation. Third argument is a hardware configuration -- examples are
  // written for Adafruit Feather RP2040 DVI, but that's easily switched out
  // for boards like the Pimoroni Pico DV (use 'pimoroni_demo_hdmi_cfg') or
  // Pico DVI Sock ('pico_sock_cfg').
  DVIGFX8 dviGFX(DVI_RES_320x240p60, true, adafruit_feather_dvi_cfg);

  constexpr const pin_size_t SPI_DEFAULT_PIN_RX = 8;//12;
  constexpr const pin_size_t SPI_DEFAULT_PIN_CS = 9;//13;
  constexpr const pin_size_t SPI_DEFAULT_PIN_SCK = 10;
  constexpr const pin_size_t SPI_DEFAULT_PIN_TX = 11;

  void setupDefaultPalette()
  {
    for (uint16_t index = 0; index < COLOR_COUNT; ++index)
    {
      dviGFX.setColor(index, index, index, index);
    }

    dviGFX.swap(false, true); // Duplicate same palette into front & back buffers
  }

  void printVersion()
  {
    dviGFX.setTextColor(255);
    dviGFX.setCursor(5, 5);
    dviGFX.print("halvoeDVI");
    dviGFX.setCursor(5, 15);
    dviGFX.print("Version: ");
    dviGFX.print(buildVersion);
    dviGFX.setCursor(5, 25);
    dviGFX.print("Build: ");
    dviGFX.print(buildTimestamp);
    #ifdef HALVOE_DVI_DEBUG
      dviGFX.setCursor(5, 35);
      dviGFX.print("HALVOE_DVI_DEBUG is enabled!");
    #endif // HALVOE_DVI_DEBUG
    dviGFX.swap();
  }

  class SPILink
  {
    private:
      SPISlaveClass& m_spiInterface;

    public:
      SPILink(SPISlaveClass& io_spiInterface = USED_SPI_SLAVE_INSTANCE) : m_spiInterface(io_spiInterface)
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

      bool begin(const SPISettings& in_spiSettings = SPI_DEFAULT_SETTINGS)
      {
        if (not dviGFX.begin()) { return false; }
        dviGFX.cp437(true);
        setupDefaultPalette();
        printVersion();
        
        if (not setDefaultPins()) { return false; }
        Callback::onSent();
        m_spiInterface.onDataRecv(Callback::onReceive);
        m_spiInterface.onDataSent(Callback::onSent);
        //m_spiInterface.begin(in_spiSettings);

        return true;
      }
  };
}

namespace halvoeDVI::AtPico::Callback
{
  size_t onReceiveBufferOffset = 0;

  void onReceive(uint8_t* in_data, size_t in_size)
  {
    if (onReceiveBufferOffset >= FRAME_SIZE)
    {
      onReceiveBufferOffset = 0;
      dviGFX.swap();
    }

    std::memcpy(dviGFX.getBuffer() + onReceiveBufferOffset, in_data, in_size);

    if (onReceiveBufferOffset + in_size < FRAME_SIZE)
    {
      onReceiveBufferOffset = in_size;
    }
  }

  uint8_t onSentBuffer[8];

  void onSent()
  {
    memset(onSentBuffer, 0, sizeof(onSentBuffer));
    USED_SPI_SLAVE_INSTANCE.setData(onSentBuffer, sizeof(onSentBuffer));
  }
}
