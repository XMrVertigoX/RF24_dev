DEVICE      := STM32F303VC
OPENCM3_DIR := lib/libopencm3

CFLAGS      += -O3 -flto
CFLAGS      += -std=gnu11
CFLAGS      += -ffunction-sections -fdata-sections

CXXFLAGS    += -O3 -flto
CXXFLAGS    += -std=gnu++17
CXXFLAGS    += -ffunction-sections -fdata-sections
CXXFLAGS    += -fno-rtti -fno-threadsafe-statics

CPPFLAGS    += -Ilib/libnrf24l01/inc
CPPFLAGS    += -MD -MP

LDFLAGS     += --specs=nano.specs --specs=nosys.specs
LDFLAGS     += -Llib/libnrf24l01
LDFLAGS     += -nostartfiles -static
LDFLAGS     += -Wl,--gc-sections

LDLIBS      += -lnrf24l01 # -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

LIBDEPS     += libnrf24l01.a

OBJS        := $(patsubst %.cpp,%.o,$(shell find src -type f -name *.cpp))

# -----

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

# -----

all: binary.elf binary.hex binary.bin
	@echo # New line for better reading
	@$(SIZE) $<
	@echo # Another new line for even better reading

clean:
	$(Q)$(RM) -rf binary.* *.o
	$(MAKE)	--directory=lib/libnrf24l01 clean

download: binary.bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink

libnrf24l01.a:
	CROSS_COMPILE=$(PREFIX)- $(MAKE) --directory=lib/libnrf24l01

# -----

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# -----
