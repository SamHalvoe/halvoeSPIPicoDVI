#pragma once

#include "halvoeDVIUtility.hpp"
#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtHost
{
  class GFXCmdSerializer
  {
    public:
      using CommandBufferWriter = ByteArrayWriter<COMMAND_BUFFER_SIZE>;
      using GFXCmdSizeType = uint16_t;

      const uint16_t GFXCmdTag = ( 's' << 8 ) + ( 'h' >> 8 ); // magic serialization tag
      const uint16_t GFXCmdVersion = 1; // serialization format version
      const uint16_t GFXCmdSizeLength = sizeof(GFXCmdSizeType);

    private:
      CommandBufferWriter m_writer;

    private:
      void beginCommand(GFXCommand in_command)
      {
        m_writer.skip<GFXCmdSizeType>(); // field for size command, will be written at the end
        m_writer.write<uint16_t>(fromGFXCommand(in_command));
      }

      void endCommand()
      {
        m_writer.writeAt<GFXCmdSizeType>(m_writer.getCursor() - GFXCmdSizeLength, 0); // field for size command
      }

      void addSwap()
      {
        beginCommand(GFXCommand::swap);
        endCommand();
      }

    public:
      GFXCmdSerializer(CommandBuffer& in_buffer) : m_writer(in_buffer)
      {}

      size_t getFrameSize() const
      {
        return m_writer.getCursor();
      }

      void beginFrame()
      {
        m_writer.resetCursor();
        m_writer.write<uint16_t>(GFXCmdTag);
        m_writer.write<uint16_t>(GFXCmdVersion);
      }

      void addFillScreen(uint16_t in_color)
      {
        beginCommand(GFXCommand::fillScreen);
        m_writer.write<uint16_t>(in_color);
        endCommand();
      }

      void addFillRect(int16_t in_x, int16_t in_y, int16_t in_width, int16_t in_height, uint16_t in_color)
      {
        beginCommand(GFXCommand::fillRect);
        m_writer.write<int16_t>(in_x);
        m_writer.write<int16_t>(in_y);
        m_writer.write<int16_t>(in_width);
        m_writer.write<int16_t>(in_height);
        m_writer.write<uint16_t>(in_color);
        endCommand();
      }

      void endFrame()
      {
        addSwap();
      }
  };
}
