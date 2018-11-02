#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#include <libnrf24l01/nrf24.hpp>

#include "support/gpio.hpp"
#include "support/spi.hpp"

void* __dso_handle = nullptr;

// static Gpio button(GPIOA, GPIO0);

// static Gpio LD3(GPIOE, GPIO9);
// static Gpio LD4(GPIOE, GPIO8);
// static Gpio LD5(GPIOE, GPIO10);
// static Gpio LD6(GPIOE, GPIO15);
// static Gpio LD7(GPIOE, GPIO11);
// static Gpio LD8(GPIOE, GPIO14);
// static Gpio LD9(GPIOE, GPIO12);
// static Gpio LD10(GPIOE, GPIO13);

static Gpio ledBlue(GPIOE, (GPIO8 | GPIO12));
static Gpio ledGreen(GPIOE, (GPIO15 | GPIO11));
static Gpio ledRed(GPIOE, (GPIO9 | GPIO13));
static Gpio ledYellow(GPIOE, (GPIO10 | GPIO14));

static Gpio rf24_1_ss(GPIOB, GPIO12);
static Gpio rf24_1_en(GPIOF, GPIO2);
static Gpio rf24_1_irq(GPIOC, GPIO2);
static Spi rf24_1_spi(SPI2, rf24_1_ss);
static nRF24 rf24_1(rf24_1_spi, rf24_1_en);

static Gpio rf24_2_ss(GPIOD, GPIO15);
static Gpio rf24_2_en(GPIOD, GPIO13);
static Gpio rf24_2_irq(GPIOB, GPIO11);
static Spi rf24_2_spi(SPI2, rf24_2_ss);
static nRF24 rf24_2(rf24_2_spi, rf24_2_en);

static nRF24_Datagram_t globalData;

static uint8_t defaultAddress      = 0xE7;
static uint32_t defaultBaseAddress = 0xE7E7E7E7;

extern "C" int _write(int fd, const void* buf, size_t count)
{
  for (int i = 0; i < count; i++)
  {
    usart_send_blocking(USART2, static_cast<const char*>(buf)[i]);
  }

  return count;
}

static void spi_setup()
{
  // Enable spi2 clock
  rcc_periph_clock_enable(RCC_SPI2);
  rcc_periph_clock_enable(RCC_GPIOB);

  // Setup GPIO alternate functions for SPI2 signals.
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13 | GPIO14 | GPIO15);
  gpio_set_af(GPIOB, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);

  // spi initialization;
  spi_set_master_mode(SPI2);
  spi_set_baudrate_prescaler(SPI2, SPI_CR1_BR_FPCLK_DIV_64);
  spi_set_clock_polarity_0(SPI2);
  spi_set_clock_phase_0(SPI2);
  spi_set_full_duplex_mode(SPI2);
  spi_set_unidirectional_mode(SPI2); /* bidirectional but in 3-wire */
  spi_set_data_size(SPI2, SPI_CR2_DS_8BIT);
  spi_enable_software_slave_management(SPI2);
  spi_send_msb_first(SPI2);
  spi_set_nss_high(SPI2);
  // spi_enable_ss_output(SPI2);
  spi_fifo_reception_threshold_8bit(SPI2);
  SPI_I2SCFGR(SPI2) &= ~SPI_I2SCFGR_I2SMOD;
  spi_enable(SPI2);
}

static void usart_setup()
{
  /* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
  rcc_periph_clock_enable(RCC_USART2);
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

  /* Setup UART parameters. */
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(USART2);
}

static void gpio_setup()
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_GPIOE);
  rcc_periph_clock_enable(RCC_GPIOF);

  // nRF24 ss
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);

  // nRF24 irq
  gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO2);
  gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO11);

  // nRF24 en
  gpio_mode_setup(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

  // Button
  gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO0);

  // LEDs
  gpio_mode_setup(
      GPIOE,
      GPIO_MODE_OUTPUT,
      GPIO_PUPD_NONE,
      (GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15));
}

static void exti_setup()
{
  rcc_periph_clock_enable(RCC_SYSCFG);

  nvic_enable_irq(NVIC_EXTI0_IRQ);
  nvic_enable_irq(NVIC_EXTI2_TSC_IRQ);
  nvic_enable_irq(NVIC_EXTI15_10_IRQ);

  // Button
  exti_select_source(EXTI0, GPIOA);
  exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
  exti_enable_request(EXTI0);

  // nRF24_1
  exti_select_source(EXTI2, GPIOC);
  exti_set_trigger(EXTI2, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI2);

  // nRF24_2
  exti_select_source(EXTI11, GPIOB);
  exti_set_trigger(EXTI11, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI11);
}

extern "C" void exti0_isr(void)
{
  exti_reset_request(EXTI0);
}

extern "C" void exti2_tsc_isr(void)
{
  rf24_1.notify();
  exti_reset_request(EXTI2);
}

extern "C" void exti15_10_isr(void)
{
  rf24_2.notify();
  exti_reset_request(EXTI11);
}

static void clock_setup()
{
  rcc_clock_setup_hsi(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
}

static void rxCallback(nRF24_Datagram_t data, void* context)
{
  // nRF24* self = static_cast<nRF24*>(context);

  static int counter = 0;

  if (counter++ % 100 == 0)
  {
    ledYellow.toggle();
  }

  if (*reinterpret_cast<int*>(data.bytes) % 100 == 0)
  {
    ledGreen.toggle();
  }

  static int counter2 = 0;

  if (*reinterpret_cast<int*>(data.bytes) == counter2)
  {
    // ledRed.clear();
    counter2++;
  }
  else
  {
    ledRed.set();
    counter2 = *reinterpret_cast<int*>(data.bytes);
  }
}

static void txCallback(void* context)
{
  // nRF24* self = static_cast<nRF24*>(context);

  static int counter = 0;

  if (counter++ % 100 == 0)
  {
    ledBlue.toggle();
  }
}

int main()
{
  clock_setup();
  gpio_setup();
  usart_setup();
  spi_setup();
  exti_setup();

  ledRed.set();

  // Wait a moment for rf24 startup
  for (int i = 0; i < 800000; i++)
  {
    __asm__("nop");
  }

  printf("Application\r\n");

  rf24_1.enterStandbyMode();
  rf24_2.enterStandbyMode();

  ledYellow.set();

  for (int i = 0; i < 800000; i++)
  {
    __asm__("nop");
  }

  rf24_1.setup();
  rf24_1.writeRxAddress(0, defaultAddress);
  rf24_1.writeRxBaseAddress(0, defaultBaseAddress);
  rf24_1.writeTxAddress(defaultAddress);
  rf24_1.writeTxBaseAddress(defaultBaseAddress);
  rf24_1.setRetryCount(0xf);
  rf24_1.setRetryDelay(0xf);
  rf24_1.setDataRate(nRF24_DataRate_t::DR_2MBPS);
  rf24_1.setRxCallback(rxCallback, &rf24_1);
  rf24_1.setTxCallback(txCallback, &rf24_1);
  rf24_1.startListening();

  rf24_2.setup();
  rf24_2.writeRxAddress(0, defaultAddress);
  rf24_2.writeRxBaseAddress(0, defaultBaseAddress);
  rf24_2.writeTxAddress(defaultAddress);
  rf24_2.writeTxBaseAddress(defaultBaseAddress);
  rf24_2.setRetryCount(0xf);
  rf24_2.setRetryDelay(0xf);
  rf24_2.setDataRate(nRF24_DataRate_t::DR_2MBPS);
  rf24_2.setRxCallback(rxCallback, &rf24_2);
  rf24_2.setTxCallback(txCallback, &rf24_2);
  rf24_2.startListening();

  ledGreen.set();

  for (int i = 0; i < 800000; i++)
  {
    __asm__("nop");
  }

  rf24_1.enterRxMode();
  rf24_2.enterTxMode();

  ledBlue.set();

  for (int i = 0; i < 800000; i++)
  {
    __asm__("nop");
  }

  *reinterpret_cast<int*>(globalData.bytes) = 0;
  globalData.numBytes                       = 32;

  ledRed.clear();

  for (;;)
  {
    if (rf24_2.enqueueData(globalData) == EXIT_SUCCESS)
    {
      *reinterpret_cast<int*>(globalData.bytes) += 1;
    }

    rf24_1.loop();
    rf24_2.loop();
  }
}
