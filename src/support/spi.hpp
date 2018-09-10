#ifndef SPIDEVICE_HPP_
#define SPIDEVICE_HPP_

#include <libnrf24l01/ispi.hpp>

class Spi : public ISpi
{
public:
  Spi();
  virtual ~Spi();

  uint8_t transmit_receive(uint8_t txBytes[], uint8_t rxBytes[], size_t numBytes);
};

#endif /* SPIDEVICE_HPP_ */
