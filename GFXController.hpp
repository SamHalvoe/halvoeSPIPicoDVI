#pragma once

#include <type_traits>
#include <array>
#include <PicoDVI.h>

#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtPico
{
  template<size_t in_arraySize>
  class ByteArrayView
  {
    private:
      uint8_t* m_begin = nullptr;
      uint8_t* m_end = nullptr;
      const size_t m_size = 0;

    public:
      ByteArrayView(std::array<uint8_t, in_arraySize>& in_array, size_t in_viewSize, size_t in_offset = 0) :
        m_begin(in_array.data() + in_offset),
        m_end(in_array.data() + in_offset + in_viewSize + 1), // + 1 for past the end "iterator"
        m_size(in_viewSize)
      {
        // ToDo: Add debug message! assert(in_offset + in_viewSize <= in_arraySize, "in_viewSize must be less than or equal to in_arraySize!");
        // ToDo: Add debug message! assert(m_begin < m_end, "m_begin must be less than m_end!");
      }

      size_t getSize() const
      {
        return m_size;
      }

      uint8_t getByte()
      {
        if (m_begin + sizeof(uint8_t) > m_end) { return 0; }

        uint8_t value = *m_begin;
        ++m_begin;
        return value;
      }

      template<typename Type>
      Type getAs()
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_begin + sizeof(Type) > m_end) { return 0; }
        
        Type value = *reinterpret_cast<Type*>(m_begin);
        m_begin = m_begin + sizeof(Type);
        return value;
      }
  };

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
