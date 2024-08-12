#pragma once

#include <PicoDVI.h>

#include "halvoeDVIUtility.hpp"
#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtPico
{
  class GFXController
  {
    private:
      using CommandBuffer = std::array<uint8_t, COMMAND_BUFFER_SIZE>;
      using CommandBufferReader = ByteArrayReader<COMMAND_BUFFER_SIZE>;

    private:
      DVIGFX8& m_dviGFX;
      size_t m_receiveBufferIndex = 0;
      size_t m_executeBufferIndex = 1;
      std::array<CommandBuffer, 2> m_commandBuffers;

    private:
      CommandBuffer& getExecuteBuffer()
      {
        return m_commandBuffers[m_executeBufferIndex];
      }

      void swap()
      {
        m_dviGFX.swap();
      }

      void fillScreen(CommandBufferReader& in_reader)
      {
        uint16_t color = in_reader.read<uint16_t>();
        m_dviGFX.fillScreen(color);
      }

      void fillRect(CommandBufferReader& in_reader)
      {
        int16_t x = in_reader.read<int16_t>();
        int16_t y = in_reader.read<int16_t>();
        int16_t width = in_reader.read<int16_t>();
        int16_t height = in_reader.read<int16_t>();
        uint16_t color = in_reader.read<uint16_t>();
        m_dviGFX.fillRect(x, y, width, height, color);
      }

    public:
      GFXController(DVIGFX8& io_dviGFX) : m_dviGFX(io_dviGFX)
      {
        getReceiveBuffer().fill(0);
        getExecuteBuffer().fill(0);
      }

      CommandBuffer& getReceiveBuffer()
      {
        return m_commandBuffers[m_receiveBufferIndex];
      }

      void swapCommandBuffers()
      {
        size_t newReceiveBufferIndex = m_executeBufferIndex;
        m_executeBufferIndex = m_receiveBufferIndex;
        m_receiveBufferIndex = newReceiveBufferIndex;
      }

      bool executeFromBuffer()
      {
        CommandBufferReader bufferReader = CommandBufferReader(getExecuteBuffer());
        bufferReader.read<uint16_t>(); // GFXCmdTag
        bufferReader.read<uint16_t>(); // GFXCmdVersion

        while (bufferReader.isInBounds<uint16_t>())
        {
          switch (toGFXCommand(bufferReader.read<uint16_t>()))
          {
            case GFXCommand::invalid:
              /* ToDo: Add debug message! */
              return false;
            case GFXCommand::swap: swap();
              return true;
            
            case GFXCommand::fillScreen: fillScreen(bufferReader); break;
            case GFXCommand::fillRect: fillRect(bufferReader);     break;
          }
        }

        /* ToDo: Add debug message! */
        return false;
      }
  };
}
