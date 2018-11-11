DEVICE       := STM32F303VC
OPENCM3_DIR  := lib/libopencm3
PROJECT_NAME := libnrf24l01_example

SOURCE_FILES := \
	$(wildcard src/*.cpp) \
	$(wildcard src/support/*.cpp) \
	$(wildcard lib/libnrf24l01/src/adapter/*.cpp)
OBJS := \
	$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCE_FILES)))
DEPS := \
	$(patsubst %.cpp,%.d,$(filter %.cpp,$(SOURCE_FILES)))
INCLUDE_DIRS := \
	lib/libnrf24l01/inc
LIBDEPS := \
	lib/libnrf24l01/libnrf24l01.a

# -----

CFLAGS       += -Os
CFLAGS       += -std=gnu17
CFLAGS       += -ffunction-sections -fdata-sections

CXXFLAGS     += -Os
CXXFLAGS     += -std=gnu++17
CXXFLAGS     += -ffunction-sections -fdata-sections

CPPFLAGS     += $(addprefix -I,$(INCLUDE_DIRS))

LDFLAGS      += --specs=nosys.specs --specs=nano.specs
LDFLAGS      += -nostartfiles -static
LDFLAGS      += $(addprefix -L,$(dir $(LIBDEPS)))
LDFLAGS      += -Wl,--gc-sections

LDLIBS       += -lnrf24l01 -lstdc++

# -----

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

# -----

all: $(PROJECT_NAME).elf $(PROJECT_NAME).bin
	@echo # New line for better reading
	@$(SIZE) $<
	@echo # Another new line for even better reading

clean:
	$(RM) -rf $(OBJS) $(DEPS) $(PROJECT_NAME).elf $(PROJECT_NAME).bin

clean_all: clean
	$(MAKE) -C lib/libnrf24l01 clean

download: $(PROJECT_NAME).bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink

export CXX
export CXXFLAGS += $(ARCH_FLAGS)
lib/libnrf24l01/libnrf24l01.a:
	$(MAKE) -C lib/libnrf24l01 all

# -----

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# -----

-include $(DEPS)
