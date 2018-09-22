#ifndef __SPI_HPP
#define __SPI_HPP

#include <libnrf24l01/ispi.hpp>

class Spi : public ISpi
{
public:
  Spi(uint32_t spi);
  virtual ~Spi();

  uint32_t transmit_receive(uint8_t txBytes[], uint8_t rxBytes[], size_t numBytes);

private:
  uint32_t _spi;
};

#endif /* __SPI_HPP */
