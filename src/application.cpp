#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f3xx_hal_conf.h>

#include <xXx/components/wireless/rf24/rf24.hpp>
#include <xXx/templates/queue.hpp>
#include <xXx/utils/logging.hpp>

#include "gpio.hpp"
#include "led.hpp"
#include "spi.hpp"

static const RF24_Address_t address = 0xE7E7E7E7E7;

static Queue<RF24_DataPackage_t> rxQueue(3);
static Queue<RF24_DataPackage_t> txQueue(3);

uint32_t counter = 0;

extern SPI_HandleTypeDef hspi2;

Gpio port1_CE(PORT1_CE_GPIO_Port, PORT1_CE_Pin);
Gpio port1_CS(PORT1_CS_GPIO_Port, PORT1_CS_Pin);
Gpio port1_INT(PORT1_INT_GPIO_Port, PORT1_INT_Pin);

Gpio port2_CE(PORT2_CE_GPIO_Port, PORT2_CE_Pin);
Gpio port2_CS(PORT2_CS_GPIO_Port, PORT2_CS_Pin);
Gpio port2_INT(PORT2_INT_GPIO_Port, PORT2_INT_Pin);

Spi port1_SPI(hspi2, port1_CS);
Spi port2_SPI(hspi2, port2_CS);

Gpio button(BUTTON_GPIO_Port, BUTTON_Pin);

RF24 transmitter(port1_SPI, port1_CE, port1_INT);
RF24 receiver(port2_SPI, port2_CE, port2_INT);

Led led[] = {Led(LD3), Led(LD5), Led(LD7), Led(LD9), Led(LD10), Led(LD8), Led(LD6), Led(LD4)};

static void buttonCallback(void* user){};

extern "C" void initializeApplication() {
    LOG("%s", __PRETTY_FUNCTION__);

    button.enableInterrupt(buttonCallback, NULL);
}

extern "C" void applicationTaskFunction(void const* argument) {
    RF24_Status status = RF24_Status::Success;

    vTaskDelay(100);

    transmitter.enterStandbyMode();
    receiver.enterStandbyMode();

    status = transmitter.setTxAddress(address);
    LOG("setTxAddress: %d", status);
    status = transmitter.setDataRate(RF24_DataRate::DR_2MBPS);
    LOG("setDataRate: %d", status);
    status = transmitter.setCrcConfig(RF24_CRCConfig::CRC_2Bytes);
    LOG("setCrcConfig: %d", status);
    status = transmitter.setChannel(2);
    LOG("setChannel: %d", status);
    status = transmitter.setOutputPower(RF24_OutputPower::PWR_18dBm);
    LOG("setOutputPower: %d", status);
    status = transmitter.setRetryCount(0xF);
    LOG("setRetryCount: %d", status);
    status = transmitter.setRetryDelay(0xF);
    LOG("setRetryDelay: %d", status);
    status = transmitter.configureTxDataPipe(&txQueue);
    LOG("enableTxDataPipe: %d", status);

    status = receiver.setRxAddress(0, address);
    LOG("setRxAddress: %d", status);
    status = receiver.setDataRate(RF24_DataRate::DR_2MBPS);
    LOG("setDataRate: %d", status);
    status = receiver.setCrcConfig(RF24_CRCConfig::CRC_2Bytes);
    LOG("setCrcConfig: %d", status);
    status = receiver.setChannel(2);
    LOG("setChannel: %d", status);
    status = receiver.setOutputPower(RF24_OutputPower::PWR_18dBm);
    LOG("setOutputPower: %d", status);
    status = receiver.configureRxDataPipe(0, &rxQueue);
    LOG("enableRxDataPipe: %d", status);

    assert(status == RF24_Status::Success);

    receiver.enterRxMode();
    transmitter.enterTxMode();

    for (;;) {
        RF24_DataPackage_t package;

        if (txQueue.queueSpacesAvailable()) {
            memcpy(package.bytes, &counter, sizeof(counter));
            package.numBytes = sizeof(counter);
            txQueue.enqueue(package);
            counter++;
        }

        if (rxQueue.queueMessagesWaiting()) {
            rxQueue.dequeue(package);
            BUFFER("package", package.bytes, package.numBytes);
        }

        transmitter.loop();
        receiver.loop();
    }
}
