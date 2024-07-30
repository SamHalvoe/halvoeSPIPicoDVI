#pragma once

#include <PicoDVI.h>

#include "halvoeInfo.hpp"
#include "halvoeSPIPicoDVI.hpp"
#include "halvoePioSPI/halvoePioSPI.hpp"

#ifndef ARDUINO_ARCH_RP2040
  #error halvoeDVI::AtPico does only work on RP2040!
#endif

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

  const pin_size_t SPI_DEFAULT_PIN_RX = 10;
  const pin_size_t SPI_DEFAULT_PIN_CS = 12;
  const pin_size_t SPI_DEFAULT_PIN_SCK = 11;
  const pin_size_t SPI_DEFAULT_PIN_TX = 9;
  int stateMachine = -1;

  void handleDmaIrq()
  {
    dviGFX.swap();
    clear_dma_interrupt_request();
    restart_dma_channel(dviGFX.getBuffer());
  }

  bool beginSPI()
  {
    stateMachine = setupPioSPI(pio1, SPI_DEFAULT_PIN_RX, dviGFX.getBuffer(), FRAME_SIZE, handleDmaIrq);
    return stateMachine != -1;
  }

  void setupIsDVIReadyPin()
  {
    pinMode(IS_DVI_READY_PIN, OUTPUT);
  }

  void writeIsDVIReady(bool in_isReady)
  {
    digitalWrite(IS_DVI_READY_PIN, in_isReady ? HIGH : LOW);
  }
}
