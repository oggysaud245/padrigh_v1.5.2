# MCU and programmer configuration
MCU = atmega328p
PROGRAMMER = usbasp
BAUD = 19200

# File to flash
TARGET = padright_v1.5.1.ino.with_bootloader.eightanaloginputs.hex

# AVRDUDE command
AVRDUDE = avrdude
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -V

# Default flash target
flash:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET):i
fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFD:m

# Optional: verify connection
# check:
# 	$(AVRDUDE) $(AVRDUDE_FLAGS)

# Clean target (if you later generate .hex via make)
# clean:
# 	rm -f *.hex

