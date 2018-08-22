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

download: binary.bin
	JLinkExe -AutoConnect 1 -ExitOnError 1 -Device $(DEVICE) -IF SWD -Speed auto -CommandFile download.jlink
