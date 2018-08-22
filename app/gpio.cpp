#include <stm32f3xx_hal.h>

#include <xXx/interfaces/igpio.hpp>
#include <xXx/utils/logging.hpp>

#include "gpio.hpp"

IGpio_Callback_t Gpio::_callback[16];
void *Gpio::_user[16];

Gpio::Gpio(GPIO_TypeDef *port, uint16_t pin) : _port(port), _pin(pin) {}

Gpio::~Gpio() {}

void Gpio::clear() {
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
}

bool Gpio::get() {
    return ((bool)HAL_GPIO_ReadPin(_port, _pin));
}

void Gpio::toggle() {
    HAL_GPIO_TogglePin(_port, _pin);
}

void Gpio::set() {
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);
}

void Gpio::disableInterrupt() {
    _callback[__builtin_ctz(_pin)] = NULL;
    _user[__builtin_ctz(_pin)]     = NULL;
}

void Gpio::enableInterrupt(IGpio_Callback_t callback, void *user) {
    if (_callback[__builtin_ctz(_pin)]) {
        return;
    }

    _callback[__builtin_ctz(_pin)] = callback;
    _user[__builtin_ctz(_pin)]     = user;
}

void Gpio::callback(uint16_t pin) {
    if (_callback[__builtin_ctz(pin)]) {
        _callback[__builtin_ctz(pin)](_user[__builtin_ctz(pin)]);
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    Gpio::callback(GPIO_Pin);
}
