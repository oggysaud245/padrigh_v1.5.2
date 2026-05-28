# PadRight V1.5.2

PadRight V1.5.2 is an Arduino-based project for managing pad vending machines. It supports RFID and cash-based operations, allowing users to interact with the machine via an LCD interface.

**This code works for KAWACH_V3.1 PCB.**

## Project Structure

## Features

### LCD Interface

- Displays messages to guide users through operations.
- Shows stock levels, machine type, and other relevant information.
- Functions include:
  - `startMessage()`: Displays the startup message.
  - `homePage()`: Displays the home screen.
  - `menu()`: Handles navigation through the menu.
  - `fillingAllRack()`: Guides the user through filling all racks.

### RFID Operations

- Supports reading and writing to RFID cards.
- Functions include:
  - `readCard()`: Reads data from an RFID card.
  - `writeCard()`: Writes data to an RFID card.
  - `manageRFID()`: Manages RFID-based interactions, such as scanning cards and dispensing pads.

### Cash Operations

- Handles cash-based transactions (details in `cashInterrupt.ino`).

### Rack Management

- Tracks the quantity of pads in each rack.
- Functions include:
  - `manageRack(byte num)`: Displays and updates the quantity of a specific rack.
  - `fillAll(int capacity)`: Fills all racks to the specified capacity.

## Dependencies

- **MFRC522 Library**: Used for RFID operations.
- **LiquidCrystal Library**: Used for LCD display management.

## How to Use

1. **Setup**:
   - Upload the `padright_v1.5.1.ino` sketch to your Arduino board.
   - Ensure all hardware components (LCD, RFID reader, etc.) are connected properly.

2. **Operation**:
   - Use RFID cards or cash to interact with the machine.
   - Follow the on-screen instructions displayed on the LCD.

3. **Maintenance**:
   - Use the "Fill All Rack" option to restock pads.
   - Monitor stock levels via the LCD interface.

## File Descriptions

### [lcd.ino](lcd.ino)

Contains functions for managing the LCD display, including menus, messages, and user prompts.

### [rfid.ino](rfid.ino)

Handles RFID-based operations, such as reading and writing card data and managing user interactions.

### [padrack.h](padrack.h) and [padrack.cpp](padrack.cpp)

Defines and implements the `padrack` class, which manages the quantity and capacity of individual racks.

### [cashInterrupt.ino](cashInterrupt.ino)

Handles cash-based transactions (details not provided in the excerpts).

### [padright_v1.5.1.ino](padright_v1.5.1.ino)

The main Arduino sketch that integrates all components and manages the overall workflow.

## Build Outputs

Compiled files are located in the `build/MiniCore.avr.328/` directory. These include:

- `.hex` and `.bin` files for flashing the Arduino.
- `.lst` and `.elf` files for debugging.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

- Developed by KAICHO GROUP.
- Special thanks to contributors and open-source libraries used in this project.

## Caution

The content of this file is AI generated. Not verified by me, possible of misinformation or minimum inforamtion.
