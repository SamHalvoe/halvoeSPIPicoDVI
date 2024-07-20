#include "Arduino.h"
#pragma once

#include <cstring>
#include <PicoDVI.h>
#include <SPISlave.h>

#include "halvoeInfo.hpp"
#include "halvoeSPIPicoDVI.hpp"

#ifndef ARDUINO_ARCH_RP2040
  #error halvoeDVI::AtPico does only work on RP2040!
#endif

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
  const pin_size_t IS_DVI_READY_PIN = 5;

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

  bool beginDVI()
  {
    if (not dviGFX.begin()) { return false; }
    dviGFX.cp437(true);
    setupDefaultPalette();
    printVersion();

    return true;
  }

  const pin_size_t SPI_DEFAULT_PIN_RX = 12;
  const pin_size_t SPI_DEFAULT_PIN_CS = 13;
  const pin_size_t SPI_DEFAULT_PIN_SCK = 10;
  const pin_size_t SPI_DEFAULT_PIN_TX = 11;

  // These set methods panic the core, if set to invalid pins!!!
  bool setPins(pin_size_t in_rx, pin_size_t in_cs, pin_size_t in_sck, pin_size_t in_tx)
  {
    if (not SPISlave.setRX(in_rx)) { return false; }
    if (not SPISlave.setCS(in_cs)) { return false; }
    if (not SPISlave.setSCK(in_sck)) { return false; }
    if (not SPISlave.setTX(in_tx)) { return false; }
    return true;
  }

  bool setDefaultPins()
  {
    return setPins(SPI_DEFAULT_PIN_RX, SPI_DEFAULT_PIN_CS, SPI_DEFAULT_PIN_SCK, SPI_DEFAULT_PIN_TX);
  }

  bool beginSPI(const SPISettings& in_spiSettings = SPI_DEFAULT_SETTINGS)
  {
    if (not setDefaultPins()) { return false; }
    Callback::onSent();
    SPISlave.onDataRecv(Callback::onReceive);
    SPISlave.onDataSent(Callback::onSent);
    SPISlave.begin(in_spiSettings);

    return true;
  }

  void setupDVIIsReadyPin()
  {
    pinMode(IS_DVI_READY_PIN, OUTPUT);
  }

  void writeIsDVIReady(bool in_isReady)
  {
    digitalWrite(IS_DVI_READY_PIN, in_isReady ? HIGH : LOW);
  }
}

namespace halvoeDVI::AtPico::Callback
{
  size_t onReceiveBufferOffset = 0;

  void onReceive(uint8_t* in_data, size_t in_size)
  {
    std::memcpy(dviGFX.getBuffer() + onReceiveBufferOffset, in_data, in_size);
    onReceiveBufferOffset = onReceiveBufferOffset + in_size;

    if (onReceiveBufferOffset >= FRAME_SIZE)
    {
      onReceiveBufferOffset = 0;
      dviGFX.swap();
    }
  }

  uint8_t onSentBuffer[64];

  void onSent()
  {
    std::memset(onSentBuffer, 4, sizeof(onSentBuffer));
    SPISlave.setData(onSentBuffer, sizeof(onSentBuffer));
  }
}
