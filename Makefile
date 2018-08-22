DEVICE      := STM32F303VC
OPENCM3_DIR := lib/libopencm3

OBJS        += src/spi.o

CFLAGS      += -Os -ggdb3
CPPFLAGS    += -MD
LDFLAGS     += -static -nostartfiles
LDLIBS      += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: all clean

all: binary.elf binary.hex binary.bin

clean:
	$(Q)$(RM) -rf binary.* *.o

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

# download: $(BIN) | download.jlink
#   JLinkExe -AutoConnect 1 -ExitOnError 1 -Device ATSAMD21G18 -IF SWD -Speed adaptive -CommandFile download.jlink

# mo:
#   curl -sSL https://git.io/get-mo -o $@ && chmod +x $@

# download.jlink: download.jlink.mustache | mo
#   BIN=$(BIN) ./mo $< > $@
