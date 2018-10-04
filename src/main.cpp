#include <cstdio>
#include <cstring>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#include <libnrf24l01/rf24.hpp>

#include "support/gpio.hpp"
#include "support/spi.hpp"

extern "C" int _write(int f, char* bytes, size_t numBytes)
{
  (void)f;

  for (int i = 0; i < numBytes; i++)
  {
    usart_send_blocking(USART2, bytes[i]);
  }

  return numBytes;
}

extern "C" int _write_r(struct _reent* r, int f, const void* bytes, size_t numBytes)
{
  (void)r;
  (void)f;

  for (int i = 0; i < numBytes; i++)
  {
    usart_send_blocking(USART2, static_cast<const char*>(bytes)[i]);
  }

  return numBytes;
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

  gpio_mode_setup(
      GPIOE,
      GPIO_MODE_OUTPUT,
      GPIO_PUPD_NONE,
      GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

static void clock_setup()
{
  rcc_clock_setup_hsi(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
}

static void rxCallback(RF24_Datagram_t data, void* context)
{
  printf("%s -> %u\r\n", __func__, data.bytes[0]);
}

static void txCallback(void* context)
{
  printf("%s\r\n", __func__);
}

int main()
{
  clock_setup();
  gpio_setup();
  usart_setup();
  spi_setup();

  // Wait a moment for rf24 startup
  for (int i = 0; i < 800000; i++)
  {
    __asm__("nop");
  }

  printf("%s\r\n", __func__);

  Gpio button(GPIOA, GPIO0);

  Gpio LD3(GPIOE, GPIO9);
  Gpio LD4(GPIOE, GPIO8);
  Gpio LD5(GPIOE, GPIO10);
  Gpio LD6(GPIOE, GPIO15);
  Gpio LD7(GPIOE, GPIO11);
  Gpio LD8(GPIOE, GPIO14);
  Gpio LD9(GPIOE, GPIO12);
  Gpio LD10(GPIOE, GPIO13);

  LD3.set();
  LD4.set();
  LD5.set();
  LD6.set();
  LD7.set();
  LD8.set();
  LD9.set();
  LD10.set();

  Gpio rf24_1_ss(GPIOB, GPIO12);
  Gpio rf24_1_en(GPIOF, GPIO2);
  Gpio rf24_1_irq(GPIOC, GPIO2);
  Spi rf24_1_spi(SPI2, rf24_1_ss);
  RF24 rf24_1(rf24_1_spi, rf24_1_en);

  Gpio rf24_2_ss(GPIOD, GPIO15);
  Gpio rf24_2_en(GPIOD, GPIO13);
  Gpio rf24_2_irq(GPIOB, GPIO11);
  Spi rf24_2_spi(SPI2, rf24_2_ss);
  RF24 rf24_2(rf24_2_spi, rf24_2_en);

  rf24_1.setup();
  rf24_1.setRxCallback(rxCallback, &rf24_1);
  // rf24_1.setTxCallback(txCallback, &rf24_1);
  rf24_1.enterRxMode();

  rf24_2.setup();
  rf24_2.setRxCallback(rxCallback, &rf24_2);
  // rf24_2.setTxCallback(txCallback, &rf24_2);
  rf24_2.enterTxMode();

  rf24_1.startListening();
  rf24_2.startListening();

  for (int i = 0; i < 80000; i++)
  {
    __asm__("nop");
  }

  static RF24_Datagram_t dummyData;
  dummyData.bytes[0] = 0;
  dummyData.numBytes = 1;

  while (true)
  {
    // if (!rf24_1_irq.get())
    // {
    rf24_1.loop();
    // }

    // if (!rf24_2_irq.get())
    // {
    rf24_2.loop();
    // }

    if (button.get())
    {
      if (rf24_2.enqueueData(dummyData))
      {
        dummyData.bytes[0]++;
      }
    }
  }
}
