# ----- Configuration ---------------------------------------------------------

PROJECT_NAME = nRF24L01

# ----- Directories and files -------------------------------------------------

# Directories
LIB = ../libs
SDK = sdk

#  Files
LINKER_SCRIPT = TrueSTUDIO/STM32F303_nRF24L01/STM32F303VC_FLASH.ld

# ----- Symbols ---------------------------------------------------------------

SYMBOLS += STM32F303xC
SYMBOLS += ARM_MATH_CM4
# SYMBOLS += NDEBUG

# ----- Source files ----------------------------------------------------------

INCLUDE_DIRS += $(SDK)/Drivers/CMSIS/Device/ST/STM32F3xx/Include
INCLUDE_DIRS += $(SDK)/Drivers/CMSIS/Include
INCLUDE_DIRS += $(SDK)/Drivers/STM32F3xx_HAL_Driver/Inc
INCLUDE_DIRS += $(SDK)/Inc
INCLUDE_DIRS += $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
INCLUDE_DIRS += $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/include
INCLUDE_DIRS += $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

SOURCE_FILES += $(wildcard $(SDK)/*.s)
SOURCE_FILES += $(wildcard $(SDK)/Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Drivers/STM32F3xx_HAL_Driver/Src/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/*.c)
SOURCE_FILES += $(wildcard $(SDK)/Src/*.c)

# -----

INCLUDE_DIRS += $(LIB)
INCLUDE_DIRS += $(LIB)/SEGGER_RTT/RTT

SOURCE_FILES += $(LIB)/SEGGER_RTT/RTT/SEGGER_RTT.c
SOURCE_FILES += $(wildcard $(LIB)/xXx/components/wireless/rf24/*.cpp)
SOURCE_FILES += $(wildcard $(LIB)/xXx/os/*.cpp)
SOURCE_FILES += $(wildcard $(LIB)/xXx/support/*.cpp)
SOURCE_FILES += $(wildcard $(LIB)/xXx/utils/*.cpp)

SOURCE_FILES += $(wildcard app/*.cpp)

# ----- Libraries -------------------------------------------------------------

LIBRARIES +=

# ----- Flags -----------------------------------------------------------------

GCCFLAGS += -mcpu=cortex-m4
GCCFLAGS += -mthumb
GCCFLAGS += -mfloat-abi=hard
GCCFLAGS += -mfpu=fpv4-sp-d16

CPPFLAGS +=

COMMON_CFLAGS += -g
COMMON_CFLAGS += -Og

CFLAGS += -std=gnu11

CXXFLAGS += -std=gnu++17

LDFLAGS += -T $(SDK)/STM32F303VCTx_FLASH.ld

# ----- Rules -----------------------------------------------------------------

TOOLCHAIN_PREFIX = arm-none-eabi-

include $(LIB)/xXx/utils/rules.mk

download: $(EXECUTABLE)
	$(TOOLCHAIN_PREFIX)gdb -q -x download.gdb $<
