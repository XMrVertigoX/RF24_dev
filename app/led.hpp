#ifndef LED_HPP_
#define LED_HPP_

#include <stm32f3xx_hal.h>

#include "gpio.hpp"

enum LedPin_t {
    LD3  = GPIO_PIN_9,
    LD4  = GPIO_PIN_8,
    LD5  = GPIO_PIN_10,
    LD6  = GPIO_PIN_15,
    LD7  = GPIO_PIN_11,
    LD8  = GPIO_PIN_14,
    LD9  = GPIO_PIN_12,
    LD10 = GPIO_PIN_13
};

class Led : public Gpio {
  public:
    Led(LedPin_t ledPin);
    ~Led();
};

#endif /* LED_HPP_ */
