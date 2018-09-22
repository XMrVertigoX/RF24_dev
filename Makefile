DEVICE       := STM32F303VC
OPENCM3_DIR  := lib/libopencm3
PROJECT_NAME := rf24_dev

INCLUDE_DIRS := lib/libnrf24l01/inc
SOURCE_FILES := $(shell find src -type f -name *.cpp)

OBJS         := $(patsubst %.cpp,%.o,$(SOURCE_FILES))
LIBDEPS      := lib/libnrf24l01/libnrf24l01.a

# Flags

CFLAGS       += -O3 -flto
CFLAGS       += -std=gnu11
CFLAGS       += -ffunction-sections -fdata-sections

CXXFLAGS     += -O3 -flto
CXXFLAGS     += -std=gnu++17
CXXFLAGS     += -ffunction-sections -fdata-sections
CXXFLAGS     += -fno-rtti -fno-threadsafe-statics

CPPFLAGS     += $(addprefix -I,$(INCLUDE_DIRS))
CPPFLAGS     += -MD -MP

LDFLAGS      += --specs=nano.specs --specs=nosys.specs
LDFLAGS      += $(addprefix -L,$(dir $(LIBDEPS)))
LDFLAGS      += -nostartfiles -static
LDFLAGS      += -Wl,--gc-sections

LDLIBS       += -lnrf24l01 -lstdc++ -lc -lgcc -lnosys

# -----

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

# -----

all: $(PROJECT_NAME).elf $(PROJECT_NAME).hex $(PROJECT_NAME).bin
	@echo # New line for better reading
	@$(SIZE) $<
	@echo # Another new line for even better reading

clean:
	$(RM) -rf $(OBJS) *.bin *.elf *.hex

download: $(PROJECT_NAME).bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink

lib/libnrf24l01/libnrf24l01.a:
	CROSS_COMPILE=$(PREFIX)- $(MAKE) --directory=$(dir $@)

# -----

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# -----
