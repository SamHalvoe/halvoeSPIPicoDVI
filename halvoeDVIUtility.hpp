#pragma once

#include <Arduino.h>
#include <type_traits>
#include <array>

namespace halvoeDVI
{
  template<size_t c_arraySize, size_t c_cursorOffset = 0>
  class ByteArrayReader
  {
    private:
      const uint8_t* m_begin;
      size_t m_cursor = 0;

    public:
      ByteArrayReader(const std::array<uint8_t, c_arraySize>& in_array, size_t in_offset = 0) :
        m_begin(in_array.data()), m_cursor(c_cursorOffset)
      {
        static_assert(c_cursorOffset < c_arraySize, "Cursor offset must be less than array size!");
      }

      constexpr size_t getSize() const
      {
        return c_arraySize;
      }

      size_t getCursor() const
      {
        return m_cursor;
      }

      void resetCursor()
      {
        m_cursor = 0;
      }

      bool isInBounds(size_t in_size) const
      {
        return m_cursor + in_size <= c_arraySize;
      }

      template<typename Type>
      bool isInBounds() const
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        return m_cursor + sizeof(Type) <= c_arraySize;
      }

      template<typename Type>
      bool skip()
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return false; }

        m_cursor = m_cursor + sizeof(Type);
        return true;
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

      template<typename Type>
      Type readAt(size_t in_index)
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (in_index + sizeof(Type) > c_arraySize) { return 0; } // ToDo: add debug message!
        
        return *reinterpret_cast<const Type*>(m_begin + in_index);
      }
  };

  template<size_t c_arraySize, size_t c_cursorOffset = 0>
  class ByteArrayWriter
  {
    private:
      uint8_t* m_begin;
      size_t m_cursor = 0;

    public:
      ByteArrayWriter(std::array<uint8_t, c_arraySize>& in_array) :
        m_begin(in_array.data()), m_cursor(c_cursorOffset)
      {
        static_assert(c_cursorOffset < c_arraySize, "Cursor offset must be less than array size!");
      }

      constexpr size_t getSize() const
      {
        return c_arraySize;
      }

      size_t getCursor() const
      {
        return m_cursor;
      }

      void resetCursor()
      {
        m_cursor = 0;
      }

      bool isInBounds(size_t in_size) const
      {
        return m_cursor + in_size <= c_arraySize;
      }
      
      template<typename Type>
      bool isInBounds() const
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        return m_cursor + sizeof(Type) <= c_arraySize;
      }

      template<typename Type>
      bool skip()
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return false; }

        m_cursor = m_cursor + sizeof(Type);
        return true;
      }

      template<typename Type>
      bool write(Type in_value)
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (m_cursor + sizeof(Type) > c_arraySize) { return false; }

        *reinterpret_cast<Type*>(m_begin + m_cursor) = in_value;
        m_cursor = m_cursor + sizeof(Type);
        return true;
      }

      template<typename Type>
      bool writeAt(Type in_value, size_t in_index)
      {
        static_assert(std::is_arithmetic<Type>::value, "Type must be arithmetic!");
        if (in_index + sizeof(Type) > c_arraySize) { return false; }

        *reinterpret_cast<Type*>(m_begin + in_index) = in_value;
        return true;
      }
  };
}
