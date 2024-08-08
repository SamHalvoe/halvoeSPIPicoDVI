#pragma once

#include "GFXCmdSerialzer.hpp"
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

    private:
      void transferCommand(uint16_t in_commandSize)
      {
        m_spiInterface.beginTransaction(m_spiSettings);
        digitalWriteFast(CS_PIN, LOW);
        m_spiInterface.transfer(m_commandBuffer.data(), nullptr, in_commandSize);
        digitalWriteFast(CS_PIN, HIGH);
        m_spiInterface.endTransaction();
      }

    public:
      SPILink(HALVOE_SPI_CLASS& io_spiInterface = HALVOE_SPI_DEFAULT) :
        m_spiInterface(io_spiInterface)
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
      
      void swap()
      {
        CommandBufferWriter commandBufferWriter(m_commandBuffer);
        commandBufferWriter.skip<uint16_t>(); // field for size, will be written at the end
        commandBufferWriter.write<uint16_t>(fromGFXCommand(GFXCommand::swap));
        commandBufferWriter.writeAt<uint16_t>(commandBufferWriter.getCursor() - 2, 0); // field for size
        transferCommand(commandBufferWriter.getCursor());
      }

      void fillScreen(uint16_t in_color)
      {
        CommandBufferWriter commandBufferWriter(m_commandBuffer);
        commandBufferWriter.skip<uint16_t>(); // field for size, will be written at the end
        commandBufferWriter.write<uint16_t>(fromGFXCommand(GFXCommand::fillScreen));
        commandBufferWriter.write<uint16_t>(in_color);
        commandBufferWriter.writeAt<uint16_t>(commandBufferWriter.getCursor() - 2, 0); // field for size
        transferCommand(commandBufferWriter.getCursor());
      }

      void fillRect(int16_t in_x, int16_t in_y, int16_t in_width, int16_t in_height, uint16_t in_color)
      {
        CommandBufferWriter commandBufferWriter(m_commandBuffer);
        commandBufferWriter.skip<uint16_t>(); // field for size, will be written at the end
        commandBufferWriter.write<uint16_t>(fromGFXCommand(GFXCommand::fillRect));
        commandBufferWriter.write<int16_t>(in_x);
        commandBufferWriter.write<int16_t>(in_y);
        commandBufferWriter.write<int16_t>(in_width);
        commandBufferWriter.write<int16_t>(in_height);
        commandBufferWriter.write<uint16_t>(in_color);
        commandBufferWriter.writeAt<uint16_t>(commandBufferWriter.getCursor() - 2, 0); // field for size
        transferCommand(commandBufferWriter.getCursor());
      }
  };
#else
  #error Unsupported MCU!
#endif
}
