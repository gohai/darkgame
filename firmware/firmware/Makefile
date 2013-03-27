# does not work with atmega8u2, but the two are virtually identical
MCU          = at90usb82
ARCH         = AVR8
BOARD        = NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = Darkgame
SRC          = $(TARGET).c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =
PROGRAMMER   = usbasp

# Default target
all: $(TARGET).hex

$(TARGET).hex:
# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

check:
	avrdude -c $(PROGRAMMER) -p m8u2

fuses:
	avrdude -c $(PROGRAMMER) -p m8u2 -e
	avrdude -c $(PROGRAMMER) -p m8u2 -U lock:w:0x0f:m
	avrdude -c $(PROGRAMMER) -p m8u2 -U lfuse:w:0xff:m -U hfuse:w:0xd9:m -U efuse:w:0xf4:m

flash: $(TARGET).hex
	avrdude -c $(PROGRAMMER) -p m8u2 -U flash:w:$(TARGET).hex
