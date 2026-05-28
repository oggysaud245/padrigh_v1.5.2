void startMessage() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Powered By");
  lcd.setCursor(2, 1);
  lcd.print("KAICHO GROUP");
  delay(3000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("-- KAWACH --");
  lcd.setCursor(2, 1);
  lcd.print("Pad Vending");
  delay(2000);
  homePage();
}
void homePage() {
  if (changeDone == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (mType == RFID_MACHINE)
      lcd.print("SCAN CARD HERE");
    else if (mType == CASH_MACHINE)
      lcd.print("  INSERT CASH");
    lcd.setCursor(0, 1);
    lcd.print("N0 OF STOCKS:");
    lcd.print(getStock());
    changeDone = false;
  }
}

void topMenu() {
  switch (topMenuPosition) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write((byte)0);
      lcd.setCursor(2, 0);
      lcd.print("Pads In Rack");
      lcd.setCursor(2, 1);
      lcd.print("Fill All Rack");
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Pads In Rack");
      lcd.setCursor(0, 1);
      lcd.write((byte)0);
      lcd.setCursor(2, 1);
      lcd.print("Fill All Rack");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Fill All Rack");
      lcd.setCursor(0, 1);
      lcd.write((byte)0);
      lcd.setCursor(2, 1);
      lcd.print("Total Racks");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Total Racks");
      lcd.setCursor(0, 1);
      lcd.write((byte)0);
      lcd.setCursor(2, 1);
      lcd.print("Motor Time");
      break;
    case 4:
      if (mType == CASH_MACHINE) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Motor Time");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Machine Type");
      } else {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Motor Time");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Batch Write");
      }
      break;
    case 5:
      if (mType == RFID_MACHINE) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Batch Write");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Machine Type");
      } else {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Machine Type");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Cash Limit");
      }
      break;
    case 6:
      if (mType == CASH_MACHINE) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Cash Limit");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Pulse Setting");
      }
      break;
    case 7:
      if (mType == CASH_MACHINE) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Pulse Setting");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Cash Wait Time");
      }
      break;
    case 8:
      if (mType == CASH_MACHINE) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Cash Wait Time");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(2, 1);
        lcd.print("Cash Per Pad");
      }
      break;
  }
}

void menu() {
  switch (state) {
    case 0:
      homePage();
      topMenuPosition = 0;
      status = 'n';
      break;
    case 1:
      if (makeChange)
        topMenu();
      break;
  }
}

void batchWrite() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pad Amount");
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(padAmount);
}
void machineType() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Machine Type");
  lcd.setCursor(0, 1);
  // enter number
  if (mType == RFID_MACHINE) {
    lcd.print("RFID");
  } else if (mType == CASH_MACHINE) {
    lcd.print("Cash / Coin");
  }
}
void cashWait() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cash Wait Time");
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(cashWaitTime / 1000);
  lcd.print(" Sec");
}
void cashLimit() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Cash Limit");
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(CASH_LIMIT);
  lcd.print(" Rupees");
}
void cashPerPad() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Cash Per Pad");
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(CASH_PER_PAD);
  lcd.print(" Rupees");
}
void pulseLimit() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Pulse For");
  lcd.setCursor(0, 1);
  lcd.print("RS.5: ");
  // enter number
  lcd.print(pulseForFiveRupeeNote);
  if (pulseForFiveRupeeNote == availablePulses[0])
    lcd.print(" Pulse");
  else
    lcd.print(" Pulses");
}
void manageRack(byte num) {
  String message = "Rack" + String(num + 1) + " Quantity";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(rack[num].getQuantity());
}
void _maxRack() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Total Rack");
  lcd.setCursor(0, 1);
  lcd.print(maxRack);
}

void fillMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter capacity");
  lcd.setCursor(0, 1);
  lcd.print("of one Rack:");
  lcd.print(maxRackCapacity);
}
void fillingAllRack() {
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fill All");
    lcd.setCursor(0, 1);
    lcd.print("Rack");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press Okay");
    while (digitalRead(okButton))
      ;
    fillAll(maxRackCapacity);
    writeToEPPROM('f');  // write to eeprom
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Success..");
    success(500);
  }
}