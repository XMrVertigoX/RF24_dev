#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <libnrf24l01/igpio.hpp>

#include <cstdint>

class Gpio : public IGpio
{
private:
  uint32_t _gpioport;
  uint8_t _mode;
  uint8_t _pull_up_down;
  uint16_t _gpios;

public:
  Gpio(uint32_t gpioport, uint8_t mode, uint8_t pull_up_down, uint16_t gpios);
  virtual ~Gpio();

  void clear();
  int get();
  void set();

  void disableInterrupt();
  void enableInterrupt(IGpio_Callback_t callback, void *user);
};

#endif /* GPIO_HPP_ */
