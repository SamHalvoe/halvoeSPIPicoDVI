#pragma once

#include <Arduino.h>
#include <type_traits>
#include <array>

namespace halvoeDVI
{
  template<size_t in_arraySize>
  class ByteArrayView
  {
    private:
      const uint8_t* m_begin = nullptr;
      const uint8_t* m_end = nullptr;
      const size_t m_size = 0;

    public:
      ByteArrayView(const std::array<uint8_t, in_arraySize>& in_array, size_t in_viewSize, size_t in_offset = 0) :
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
        
        Type value = *reinterpret_cast<const Type*>(m_begin);
        m_begin = m_begin + sizeof(Type);
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

      size_t getCursor() const
      {
        return m_cursor;
      }

      void writeByte(uint8_t in_byte)
      {
        if (m_cursor + sizeof(uint8_t) > c_arraySize) { return; }
        *(m_begin + m_cursor) = in_byte;
        ++m_cursor;
      }

      template<typename Type>
      void write(Type in_value)
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return; }

        *reinterpret_cast<Type*>(m_begin + m_cursor) = in_value;
        m_cursor = m_cursor + sizeof(Type);
      }
  };
}
