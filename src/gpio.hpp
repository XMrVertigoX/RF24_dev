#ifndef __GPIO_HPP
#define __GPIO_HPP

#include <cstdint>

class Gpio
{
public:
  Gpio(uint32_t gpioport, uint16_t gpios);
  virtual ~Gpio();

  void clear();
  int get();
  void set();
  void toggle();

private:
  uint32_t _gpioport;
  uint16_t _gpios;
};

#endif /* __GPIO_HPP */
