DEVICE       := STM32F303VC
OPENCM3_DIR  := lib/libopencm3
PROJECT_NAME := rf24_dev

INCLUDE_DIRS := lib/libnrf24l01/inc
SOURCE_FILES := $(shell find src -type f -name *.cpp) $(wildcard lib/libnrf24l01/src/*.cpp)

OBJS         := $(patsubst %.cpp,%.o,$(SOURCE_FILES))
DEPS         := $(patsubst %.cpp,%.d,$(SOURCE_FILES))
# LIBDEPS      := lib/libnrf24l01/libnrf24l01.a

# Flags

# CFLAGS       += -O3 #-flto
CFLAGS       += -std=gnu11
CFLAGS       += -ffunction-sections -fdata-sections

# CXXFLAGS     += -O3 #-flto
CXXFLAGS     += -std=gnu++17
CXXFLAGS     += -ffunction-sections -fdata-sections
CXXFLAGS     += -fno-rtti -fno-threadsafe-statics
CXXFLAGS     += -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables

CPPFLAGS     += $(addprefix -I,$(INCLUDE_DIRS))
CPPFLAGS     += -MD -MP

LDFLAGS      += --specs=nano.specs --specs=nosys.specs
LDFLAGS      += $(addprefix -L,$(dir $(LIBDEPS)))
LDFLAGS      += -nostartfiles -static
LDFLAGS      += -Wl,--gc-sections

LDLIBS       += -lstdc++ -lc -lgcc -lnosys #-lnrf24l01

# -----

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

# -----

all: $(PROJECT_NAME).elf $(PROJECT_NAME).hex $(PROJECT_NAME).bin
	@echo # New line for better reading
	@$(SIZE) $<
	@echo # Another new line for even better reading

clean:
	$(RM) -rf $(OBJS) $(DEPS) *.bin *.elf *.hex
	$(MAKE) -C lib/libnrf24l01 clean

download: $(PROJECT_NAME).bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink

# export CROSS_COMPILE=$(PREFIX)-
# export CXXFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
# lib/libnrf24l01/libnrf24l01.a:
# 	$(MAKE) --directory=$(dir $@)

# -----

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# -----

-include $(DEPS)
