BINARY = src/spi

LDSCRIPT = stm32f3-discovery.ld

LIBNAME = opencm3_stm32f3
DEFS += -DSTM32F3

FP_FLAGS ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS = -mthumb -mcpu=cortex-m4 $(FP_FLAGS)

OOCD ?= openocd
OOCD_INTERFACE ?= stlink-v2
OOCD_TARGET ?= stm32f3x

BMP_PORT ?=

include rules.mk
