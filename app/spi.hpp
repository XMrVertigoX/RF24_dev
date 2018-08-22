#ifndef SPI_HPP_
#define SPI_HPP_

#include <stm32f3xx_hal_conf.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <xXx/interfaces/ispi.hpp>
#include <xXx/os/simpletask.hpp>
#include <xXx/templates/queue.hpp>

#include "gpio.hpp"

namespace xXx {

class Spi : public ISpi {
   private:
    SPI_HandleTypeDef *_hspi;
    Gpio &_cs;

    static SemaphoreHandle_t _semaphore[2];

   public:
    Spi(SPI_HandleTypeDef &hspi, Gpio &cs);
    ~Spi();

    uint8_t transmit_receive(uint8_t txBytes[], uint8_t rxBytes[], size_t numBytes);

    static void irq(SPI_HandleTypeDef *hspi);
};

} /* namespace xXx */

#endif /* SPI_HPP_ */
