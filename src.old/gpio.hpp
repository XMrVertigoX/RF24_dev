#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <stm32f3xx_hal.h>

#include <xXx/interfaces/igpio.hpp>

using namespace xXx;

class Gpio : public IGpio {
   public:
    Gpio(GPIO_TypeDef *port, uint16_t pin);
    virtual ~Gpio();

    /* General functionality */
    void clear();
    bool get();
    void set();
    void toggle();

    void disableInterrupt();
    void enableInterrupt(IGpio_Callback_t callback, void *user);

    static void callback(uint16_t pin);

   private:
    GPIO_TypeDef *_port;
    uint16_t _pin;

    static IGpio_Callback_t _callback[];
    static void *_user[];
};

#endif /* GPIO_HPP_ */
