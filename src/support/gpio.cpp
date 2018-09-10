#include "gpio.hpp"

#include <libopencm3/stm32/gpio.h>

Gpio::Gpio(uint32_t gpioport, uint8_t mode, uint8_t pull_up_down, uint16_t gpios)
    : _gpioport(gpioport),
      _mode(_mode),
      _pull_up_down(pull_up_down),
      _gpios(gpios)
{
  gpio_mode_setup(_gpioport, _mode, _pull_up_down, _gpios);
}

Gpio::~Gpio() {}

void Gpio::clear()
{
  gpio_clear(_gpioport, _gpios);
}

int Gpio::get()
{
  return gpio_get(_gpioport, _gpios);
}

void Gpio::set()
{
  gpio_set(_gpioport, _gpios);
}

void Gpio::disableInterrupt() {}

void Gpio::enableInterrupt(IGpio_Callback_t callback, void *user) {}
