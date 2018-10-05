DEVICE       := STM32F303VC
OPENCM3_DIR  := lib/libopencm3
PROJECT_NAME := rf24_dev

INCLUDE_DIRS := \
	lib/libnrf24l01/inc
SOURCE_FILES := \
	$(wildcard src/*.cpp) \
	$(wildcard src/support/*.cpp) \
	$(wildcard lib/libnrf24l01/src/*.cpp)
OBJS := \
	$(patsubst %.c,%.o,$(filter %.c,$(SOURCE_FILES))) \
	$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCE_FILES)))
DEPS := \
	$(patsubst %.c,%.d,$(filter %.c,$(SOURCE_FILES))) \
	$(patsubst %.cpp,%.d,$(filter %.cpp,$(SOURCE_FILES)))
# LIBDEPS      :=

# -----

# CFLAGS       += -O3 -flto
CFLAGS       += -std=c17
CFLAGS       += -ffunction-sections -fdata-sections

# CXXFLAGS     += -O3 -flto
CXXFLAGS     += -std=c++17
CXXFLAGS     += -ffunction-sections -fdata-sections

CPPFLAGS     += $(addprefix -I,$(INCLUDE_DIRS))
CPPFLAGS     += -MD -MP

LDFLAGS      += --specs=nosys.specs --specs=nano.specs
LDFLAGS      += $(addprefix -L,$(dir $(LIBDEPS)))
LDFLAGS      += -nostartfiles
LDFLAGS      += -Wl,--gc-sections

LDLIBS       += -lstdc++

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

download: $(PROJECT_NAME).bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink

# -----

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# -----

-include $(DEPS)
