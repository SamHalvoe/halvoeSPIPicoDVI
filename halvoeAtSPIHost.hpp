#pragma once

#include <Adafruit_GFX.h>
#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtHost
{
#ifdef ARDUINO_ARCH_RP2040
  #error Currently not implemented!
#elif __IMXRT1062__
  const uint8_t IS_DVI_READY_PIN = 30;
  const uint8_t CS_PIN = 0;

  class SPILink
  {
    private:
      HALVOE_SPI_CLASS& m_spiInterface;
      SPISettings m_spiSettings;

    public:
      SPILink(HALVOE_SPI_CLASS& io_spiInterface = HALVOE_SPI_DEFAULT) : m_spiInterface(io_spiInterface)
      {}

      void begin(const SPISettings& in_spiSettings = SPI_DEFAULT_SETTINGS)
      {
        pinMode(IS_DVI_READY_PIN, INPUT);
        //pinMode(CS_PIN, OUTPUT);
        //digitalWriteFast(CS_PIN, HIGH);
        m_spiSettings = in_spiSettings;
        m_spiInterface.begin();
      }

      bool isDVIReady() const
      {
        return digitalReadFast(IS_DVI_READY_PIN) == HIGH;
      }
      
      void transferFrame(const GFXcanvas8& in_frame)
      {
        m_spiInterface.beginTransaction(m_spiSettings);
        //digitalWriteFast(CS_PIN, LOW);
        m_spiInterface.transfer(in_frame.getBuffer(), nullptr, FRAME_SIZE);
        //digitalWriteFast(CS_PIN, HIGH);
        m_spiInterface.endTransaction();
      }
  };
#else
  #error Unsupported MCU!
#endif
}
