#include "spi.hpp"

#include <libopencm3/stm32/spi.h>

Spi::Spi(uint32_t spi) : _spi(spi) {}

Spi::~Spi() {}

uint32_t Spi::transmit_receive(uint8_t txBytes[], uint8_t rxBytes[], size_t numBytes)
{
  for (int i = 0; i < numBytes; i++)
  {
    spi_send8(_spi, txBytes[i]);
    rxBytes[i] = spi_read8(_spi);
  }

  return 0;
}
