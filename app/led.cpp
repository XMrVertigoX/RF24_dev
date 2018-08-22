#include <stm32f3xx_hal.h>

#include "led.hpp"

Led::Led(LedPin_t ledPin) : Gpio(GPIOE, (uint16_t)ledPin) {}

Led::~Led() {}
