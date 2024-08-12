#pragma once

#include "GFXCmdSerializer.hpp"
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
    public:
      using CommandBufferWriter = ByteArrayWriter<COMMAND_BUFFER_SIZE>;

    private:
      HALVOE_SPI_CLASS& m_spiInterface;
      SPISettings m_spiSettings;
      CommandBuffer m_commandBuffer;
      GFXCmdSerializer m_cmdSerializer;

    private:
      void transferFrame(uint16_t in_frameSize)
      {
        m_spiInterface.beginTransaction(m_spiSettings);
        digitalWriteFast(CS_PIN, LOW);
        m_spiInterface.transfer(m_commandBuffer.data(), nullptr, in_frameSize);
        digitalWriteFast(CS_PIN, HIGH);
        m_spiInterface.endTransaction();
      }

    public:
      SPILink(HALVOE_SPI_CLASS& io_spiInterface = HALVOE_SPI_DEFAULT) :
        m_spiInterface(io_spiInterface), m_cmdSerializer(m_commandBuffer)
      {}

      void begin(const SPISettings& in_spiSettings = SPI_DEFAULT_SETTINGS)
      {
        pinMode(IS_DVI_READY_PIN, INPUT);
        pinMode(CS_PIN, OUTPUT);
        digitalWriteFast(CS_PIN, HIGH);
        m_spiSettings = in_spiSettings;
        m_spiInterface.begin();
      }

      bool isDVIReady() const
      {
        return digitalReadFast(IS_DVI_READY_PIN) == HIGH;
      }
      
      GFXCmdSerializer& getGFXCmdSerializer()
      {
        return m_cmdSerializer;
      }

      void sendFrame()
      {
        transferFrame(m_cmdSerializer.getFrameSize());
      }

      // for debugging
      void dumpFrame()
      {
        ByteArrayReader<COMMAND_BUFFER_SIZE> frameReader(m_commandBuffer);
        String frameDump;
        concatCount = 0;

        while (frameReader.getCursor() < m_cmdSerializer.getFrameSize())
        {
          frameDump.concat(String(frameReader.read<uint16_t>(), HEX));
          ++concatCount;
          if (concatCount % 32 == 0) { frameDump.concat('\n'); }
        }

        Serial.println(frameDump);
      }
  };
#else
  #error Unsupported MCU!
#endif
}
