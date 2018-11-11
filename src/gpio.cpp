#include "gpio.hpp"

#include <libopencm3/stm32/gpio.h>

Gpio::Gpio(uint32_t gpioport, uint16_t gpios)
    : _gpioport(gpioport), _gpios(gpios) {}

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

void Gpio::toggle()
{
  gpio_toggle(_gpioport, _gpios);
}
