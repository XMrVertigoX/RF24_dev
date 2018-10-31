#ifndef __GPIO_HPP
#define __GPIO_HPP

#include <cstdint>

#include <libnrf24l01/igpio.hpp>

class Gpio : public IGpio
{
public:
  Gpio(uint32_t gpioport, uint16_t gpios);
  virtual ~Gpio();

  void clear();
  int get();
  void set(bool enable = true);
  void toggle();

private:
  uint32_t _gpioport;
  uint16_t _gpios;
};

#endif /* __GPIO_HPP */
