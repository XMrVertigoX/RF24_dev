#include <libopencm3/stm32/spi.h>

#include "spi.hpp"

Spi::Spi(uint32_t spi, IGpio& ss) : _spi(spi), _ss(ss)
{
  _ss.set();
}

Spi::~Spi()
{
  _ss.clear();
}

uint8_t Spi::transceive(const uint8_t txBytes[], uint8_t rxBytes[], uint32_t numBytes)
{
  _ss.clear();

  for (int i = 0; i < numBytes; i++)
  {
    spi_send8(_spi, txBytes[i]);
    rxBytes[i] = spi_read8(_spi);
  }

  _ss.set();

  return 0;
}
