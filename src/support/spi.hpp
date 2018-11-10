#ifndef __SPI_HPP
#define __SPI_HPP

#include <libnrf24l01/igpio.hpp>
#include <libnrf24l01/ispi.hpp>

class Spi : public ISpi
{
public:
  Spi(uint32_t spi, IGpio& ss);
  virtual ~Spi();

  uint8_t transceive(const void* txBytes, void* rxBytes, size_t numBytes);

private:
  uint32_t _spi;
  IGpio& _ss;
};

#endif /* __SPI_HPP */
