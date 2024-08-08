#pragma once

#include "halvoeDVIUtility.hpp"
#include "halvoeSPIPicoDVI.hpp"

namespace halvoeDVI::AtHost
{
  class GFXCmdSerializer
  {
    public:
      using CommandBufferWriter = ByteArrayWriter<COMMAND_BUFFER_SIZE>;

    private:
      CommandBuffer& m_buffer;

    public:
  };
}
