#pragma once

#include "halvoeDVIUtility.hpp"
#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtHost
{
  class GFXCmdSerializer
  {
    public:
      using CommandBufferWriter = ByteArrayWriter<COMMAND_BUFFER_SIZE>;
      using GFXCmdSize_t = uint16_t;

      const uint16_t GFXCmdTag = 0b111001101100001; // magic serialization tag -> 's' and 'h' in ascii
      const uint16_t GFXCmdVersion = 1; // serialization format version
      const size_t GFXCmdSizeLength = sizeof(GFXCmdSize_t);

    private:
      CommandBufferWriter m_writer;

    private:
      void beginCommand(GFXCommand in_command)
      {
        // todo: save cursor -> command begin
        m_writer.skip<GFXCmdSize_t>(); // field for size command, will be written in endCommand()
        m_writer.write<uint16_t>(fromGFXCommand(in_command));
      }

      void endCommand()
      {
        Serial.println("endCommand()");
        Serial.println(m_writer.getCursor());
        m_writer.writeAt<GFXCmdSize_t>(m_writer.getCursor() - GFXCmdSizeLength, /* todo: use saved cursor from beginCommand (see comment there) */0); // field for size command
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

      void endFrame()
      {
        addSwap();
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
  };
}
