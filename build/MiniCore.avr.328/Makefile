# MCU and programmer configuration
MCU = atmega328p
PROGRAMMER = usbasp
BAUD = 19200

# File to flash
TARGET = padright_v1.5.2.ino.with_bootloader.hex
EEPROM_TARGET = padright_v1.5.1.ino.eep

# AVRDUDE command
AVRDUDE = avrdude
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -V

# Flash with proper erase but preserve EEPROM
flash:
	@echo "Reading EEPROM before flash..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:r:eeprom_temp.hex:i
	@echo "Erasing and flashing..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -e -U flash:w:$(TARGET):i
	@echo "Restoring EEPROM..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:eeprom_temp.hex:i
	@rm -f eeprom_temp.hex

# Alternative: Use -D but ensure full erase of flash
flash_force:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -e -U flash:w:$(TARGET):i

# Flash and erase all (including EEPROM)
flash_erase_all:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -e -U flash:w:$(TARGET):i

# Flash with EEPROM write
flash_with_eeprom:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -e -U flash:w:$(TARGET):i -U eeprom:w:$(EEPROM_TARGET):i

# Simple flash with auto-erase (erases everything including EEPROM)
flash_simple:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET):i 

# Backup EEPROM before flashing
flash_backup_eeprom:
	@echo "Backing up EEPROM..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:r:eeprom_backup_$(shell date +%Y%m%d_%H%M%S).hex:i
	@echo "Erasing and flashing..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -e -U flash:w:$(TARGET):i
	@echo "EEPROM preserved separately"

fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFD:m

help:
	@echo "Available targets:"
	@echo "  flash              - Flash program while preserving EEPROM (reads first)"
	@echo "  flash_simple       - Simple flash (erases EEPROM)"
	@echo "  flash_force        - Force erase and flash"
	@echo "  flash_erase_all    - Erase everything and flash"
	@echo "  flash_with_eeprom  - Flash program and EEPROM together"
	@echo "  flash_backup_eeprom- Backup EEPROM, erase, flash"
	@echo "  fuse               - Program fuses"
