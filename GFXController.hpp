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
      using CommandBufferView = ByteArrayView<COMMAND_BUFFER_SIZE>;

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

      uint16_t getBufferViewSize()
      {
        return reinterpret_cast<uint16_t*>(getExecuteBuffer().data())[0];
      }

      CommandBufferView createCommandBufferView()
      {
        // the sizeof(uint16_t) offset is for the two bytes of getBufferViewSize()
        return CommandBufferView(getExecuteBuffer(), getBufferViewSize(), sizeof(uint16_t));
      }

      GFXCommand getCommand(CommandBufferView& in_view)
      {
        return toGFXCommand(in_view.getAs<uint16_t>());
      }

      void swap()
      {
        m_dviGFX.swap();
      }

      void fillScreen(CommandBufferView& in_view)
      {
        uint16_t color = in_view.getAs<uint16_t>();
        m_dviGFX.fillScreen(color);
      }

      void fillRect(CommandBufferView& in_view)
      {
        int16_t x = in_view.getAs<int16_t>();
        int16_t y = in_view.getAs<int16_t>();
        int16_t width = in_view.getAs<int16_t>();
        int16_t height = in_view.getAs<int16_t>();
        uint16_t color = in_view.getAs<uint16_t>();
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

      void executeFromBuffer()
      {
        CommandBufferView bufferView = createCommandBufferView();
        /*Serial.println(*reinterpret_cast<uint16_t*>(getExecuteBuffer().data()));
        Serial.println(*reinterpret_cast<uint16_t*>(getExecuteBuffer().data() + 2));
        Serial.println(*reinterpret_cast<uint16_t*>(getExecuteBuffer().data() + 4));
        Serial.println("....");
        Serial.println(bufferView.getAs<uint16_t>());
        Serial.println(bufferView.getAs<uint16_t>());
        Serial.println(bufferView.getAs<uint16_t>());
        Serial.println("----");*/
        
        switch (getCommand(bufferView))
        {
          case GFXCommand::invalid: /* ToDo: Add debug message! */ break;
          case GFXCommand::swap: swap(); break;
          case GFXCommand::fillScreen: fillScreen(bufferView); break;
          case GFXCommand::fillRect: fillRect(bufferView); break;
        }
      }
  };
}
