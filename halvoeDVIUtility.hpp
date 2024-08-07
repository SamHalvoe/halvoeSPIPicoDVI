#pragma once

#include <Arduino.h>
#include <type_traits>
#include <array>

namespace halvoeDVI
{
  template<size_t c_arraySize>
  class ByteArrayReader
  {
    private:
      const uint8_t* m_begin;
      size_t m_cursor = 0;

    public:
      ByteArrayReader(const std::array<uint8_t, c_arraySize>& in_array, size_t in_offset = 0) :
        m_begin(in_array.data()),
        m_cursor(in_offset)
      {
        // ToDo: Add debug message! assert(m_cursor < c_arraySize, "m_cursor must be less than c_arraySize!");
      }

      constexpr size_t getSize() const
      {
        return c_arraySize;
      }

      size_t getCursor() const
      {
        return m_cursor;
      }

      uint8_t readByte()
      {
        if (m_cursor + sizeof(uint8_t) > c_arraySize) { return 0; } // ToDo: add debug message!
        
        uint8_t value = *(m_begin + m_cursor);
        ++m_cursor;
        return value;
      }

      template<typename Type>
      Type read()
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return 0; } // ToDo: add debug message!
        
        Type value = *reinterpret_cast<const Type*>(m_begin + m_cursor);
        m_cursor = m_cursor + sizeof(Type);
        return value;
      }
  };

  template<size_t c_arraySize>
  class ByteArrayWriter
  {
    private:
      uint8_t* m_begin;
      size_t m_cursor = 0;

    public:
      ByteArrayWriter(std::array<uint8_t, c_arraySize>& in_array, size_t in_offset = 0) :
        m_begin(in_array.data()), m_cursor(in_offset)
      {
        // ToDo: Add debug message! assert(m_cursor < c_arraySize, "m_cursor must be less than c_arraySize!");
      }

      constexpr size_t getSize() const
      {
        return c_arraySize;
      }

      size_t getCursor() const
      {
        return m_cursor;
      }

      void writeByte(uint8_t in_byte)
      {
        if (m_cursor + sizeof(uint8_t) > c_arraySize) { return; } // ToDo: add debug message!
        *(m_begin + m_cursor) = in_byte;
        ++m_cursor;
      }

      template<typename Type>
      void write(Type in_value)
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return; } // ToDo: add debug message!

        *reinterpret_cast<Type*>(m_begin + m_cursor) = in_value;
        m_cursor = m_cursor + sizeof(Type);
      }
  };
}
