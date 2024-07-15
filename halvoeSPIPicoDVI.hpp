#pragma once

#include <SPI.h>

#define HALVOE_DVI_DEBUG

namespace halvoeDVI
{
  const SPISettings SPI_DEFAULT_SETTINGS(1000000, MSBFIRST, SPI_MODE0);
}
