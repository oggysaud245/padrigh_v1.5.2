void _interrupt() {  // ISR FUNCTION
  if (mType == CASH_MACHINE) {
    disableCashAcceptor();
    cashPulseCounter++;
    if (cashPulseCounter % pulseForFiveRupeeNote == 0) {
      isInterrupt = true;
      // enableCashAcceptor();
    }
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print(String(cashPulseCounter));
  }
}
void mangaeCash() {
  if (isInterrupt) {
    disableCashAcceptor();
    previous_time2 = millis();
    totalCash = 0;
    while (millis() - previous_time2 < cashWaitTime) {
      if (isInterrupt) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Receiving Cash");
        lcd.setCursor(0, 1);
        lcd.print("Please wait...");
        success(200);
        delay(500);
        delay(1000);
        isInterrupt = false;
        if (isInterrupt)
          continue;
        currentCash = (int)(cashPulseCounter * cashPulseFactor);
        totalCash += currentCash;
        previous_time2 = millis();
        switch (currentCash) {
          case FIVE_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.5");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case TEN_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.10");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case FIFTEEN_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.15");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case TWENTY_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.20");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case TWENTY_FIVE_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.25");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case THIRTY_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.30");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case THITY_FIVE_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.35");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case FOURTY_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.40");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case FOURTY_FIVE_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.45");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
          case FIFTY_RUPEES:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Received Rs.50");
            lcd.setCursor(0, 1);
            lcd.print("Total: ");
            lcd.print(String(totalCash));
            cashPulseCounter = 0;
            break;
        }
        if (totalCash >= CASH_LIMIT) {
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Cash Limit");
          lcd.setCursor(0, 1);
          lcd.print("Reached");
          delay(1000);
          totalCash = CASH_LIMIT;
          break;
        } else {
          enableCashAcceptor();
        }
      }
    }
    disableCashAcceptor();
    switch (totalCash) {
      case FIVE_RUPEES:
        dispenseProduct(1);
        break;
      case TEN_RUPEES:
        dispenseProduct(2);
        break;
      case FIFTEEN_RUPEES:
        dispenseProduct(3);
        break;
      case TWENTY_RUPEES:
        dispenseProduct(4);
        break;
      case TWENTY_FIVE_RUPEES:
        dispenseProduct(5);
        break;
      case THIRTY_RUPEES:
        dispenseProduct(6);
        break;
      case THITY_FIVE_RUPEES:
        dispenseProduct(7);
        break;
      case FOURTY_RUPEES:
        dispenseProduct(8);
        break;
      case FOURTY_FIVE_RUPEES:
        dispenseProduct(9);
        break;
      case FIFTY_RUPEES:
        dispenseProduct(10);
        break;
      default:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid Amount");
        lcd.setCursor(0, 0);
        lcd.print("Error!!");
        warning();
        break;
    }
    currentCash = 0;
    totalCash = 0;
    cashPulseCounter = 0;
    enableCashAcceptor();
    isInterrupt = false;
    state = 0;
    status = 'n';
    changeDone = true;
    menu();
  }
}

// run motor when cash is accepted
void dispenseProduct(int runCount) {
  // int totalProductToDispense = runCount;
  if (getStock() != 0) {
    for (int i = 0; i < maxRack && runCount > 0; i++) {
      bool hadStock = rack[i].getQuantity() > 0;
      lcd.clear();
      while (rack[i].getQuantity() > 0 && runCount > 0) {
        lcd.setCursor(0, 0);
        lcd.print("Dispensing Pad");
        lcd.setCursor(0, 1);
        lcd.print("Collect Now");
        success(750);
        digitalWrite(motor[i], HIGH);
        delay(motorTimeVariable);
        digitalWrite(motor[i], LOW);
        rack[i].decQuantity();
        --runCount;
        lcd.clear();
        delay(150);
      }
      //  at this movement stock fininshed at rack[i] and still more runs needed
      if (hadStock && rack[i].getQuantity() == 0 && runCount > 0) {
        runExtra(runCount, i);
      }
    }
    writeToEPPROM('r');
  } else  // print if no stock remaining
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sorry");
    lcd.setCursor(0, 1);
    lcd.print("No Stocks");
    warning();
  }
  state = 0;
  status = 'n';
  changeDone = true;
  menu();
}
void disableCashAcceptor() {
  digitalWrite(inhibit, HIGH);
}
void enableCashAcceptor() {
  digitalWrite(inhibit, LOW);
}
void runExtra(int &runCount, int i) {
  int extraToRun = min(maxExtraMotorRun, runCount);
  for (int j = 0; j < extraToRun; j++) {    
    lcd.setCursor(0, 0);
    lcd.print("Dispensing Pad");
    lcd.setCursor(0, 1);
    lcd.print("Collect Now");
    success(750);
    digitalWrite(motor[i], HIGH);
    delay(motorTimeVariable);
    digitalWrite(motor[i], LOW);
    rack[i].decQuantity();
    --runCount;
    lcd.clear();
    delay(150);
  }
}