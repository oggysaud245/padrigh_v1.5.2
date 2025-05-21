#include "padrack.h"
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#define RST_PIN 2    // Configurable, see typical pin layout above
#define SS_PIN 10    // Configurable, see typical pin layout above
#define FILLCARD 70  //card identifier as machine filling
#define RECHCARD 82  //card identifier as recharge mode
#define PADCARD 80   //card identifier as set padAmount
#define RFID_MACHINE 0
#define CASH_MACHINE 1


//

#define DEFAULT_MAX_RACK  2
#define DEFAULT_PAD_AMOUNT  10
#define DEFAULT_MACHINE_TYPE  RFID_MACHINE
#define DEFAULT_MOTOR_TIME  1500
#define DEFAULT_MAX_RACK_CAPACITY 48

//for Rupees Count
#define FIVE_RUPEES 5
#define TEN_RUPEES 10
#define FIFTEEN_RUPEES 15
#define TWENTY_RUPEES 20
#define TWENTY_FIVE_RUPEES 25
#define THIRTY_RUPEES 30
#define THITY_FIVE_RUPEES 35
#define FOURTY_RUPEES 40
#define FOURTY_FIVE_RUPEES 45
#define FIFTY_RUPEES 50
#define MAX_CASH_LIMIT 250
#define DEFAULT_CASH_LIMIT TWENTY_RUPEES
#define DEFAULT_CASH_WAIT_TIME 5000
/*
maxExtraMotorRun =>> This means that there will be 4 more pads in rack even rack quantity is displayed zero, 
this is for lets say there is is only 1 pad in stock displayed in lcd, but 20 rupees is 
inserted to machine to tackle this condition 4 more pads are added.
*/

#define maxExtraMotorRun 4        
#define MAX_CASH_WAIT_TIME 20000  // max 20 milliseconds i.e. 20sec to wait for another cash note
const byte availablePulses[] = { 1, 2, 5, 10 };
int CASH_LIMIT = DEFAULT_CASH_LIMIT;
const byte cashLimitAddress = 35;
int currentCash = 0;
int cashWaitTime = DEFAULT_CASH_WAIT_TIME;  // in milliseconds
const byte cashWaitTimeAddress = 45;
volatile int cashPulseCounter = 0;
int pulseForFiveRupeeNote = availablePulses[0];
int totalCash = 0;
float cashPulseFactor = (float)(5.0 / pulseForFiveRupeeNote);  // used to determine total amount received; total amount = cashPulseounter*cashPulseFactor
const byte pulseForFiveRupeeNoteAddress = 40;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
MFRC522::StatusCode rfidstatus;

const int rs = 4, en = 5, d4 = 6, d5 = 7, d6 = 8, d7 = 9;  // LCD pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// setup variable for the machine type
// rfid card system or pushbutton or coin based
// this variable needs to be setup while manufacturing process;
byte mType = RFID_MACHINE;  // 0 for rfid based and 1 for pushbutton or coin based;
const byte mTypeAddress = 25;

//// this checks if the system is booted for the first time
char firstBoot = 'y';
const byte firstBootAddress = 100;

// --------- eeprom address for menu variables ---------
const byte rackAddress[] = { 1, 2, 3, 4, 5, 6 };
/*
availableRack variable is effective for when maxRack is changed from menu
if machine is rfid type available rack will be 4 in KAWACH_V3.1 PCB if machine is cash type then
available rack will be 3 one pin from motor connector will be used for cash acceptor inhibit line 
*/
byte availableRack = 3;
byte maxRack = DEFAULT_MAX_RACK;  // default rack quantity is two, it should be manipulated in manufacturing process through menu;
const byte maxRackAddress = 30;
const byte maxRackCapacityAddress = 20;
const byte motorTimeAddress = 10;
const byte maxPadAddress = 15;

// regarding rfid card
byte blockAddr = 2;  // this block is uded to set the value of padAmount in rfid card
byte readByte[18];   // for padAmount
byte writeByte[16];  // for padAmount
byte authAddr = 3;
byte byteSize = sizeof(readByte);

//----------- class objects ---------------------
padrack rack[6];

//-------------------- input and output varaibles --------------
const byte motor1 = 19;
const byte motor2 = 18;
const byte motor3 = 17;
const byte motor4 = 10;
const byte menuButton = 3;
const byte selectButton = 14;
const byte okButton = 15;
const byte buzzer = 16;
const byte interrupt = 2;  // this is the interrupt pin for cash acceptor or coin based machine
const byte inhibit = motor4;
const byte motor[] = { motor1, motor2, motor3, motor4 };


////----------- logic variables -------------
static uint32_t previous_time;   // for use of button press while managing menu
static uint32_t previous_time2;  // to track the time while receiving cash
int count = 0;
bool change = false;
bool changeDone = true;  // this is for homepage changes // toggle
int maxRackCapacity = DEFAULT_MAX_RACK_CAPACITY;
int motorTimeVariable = DEFAULT_MOTOR_TIME;
int topMenuPosition = 0;
int state = 0;
bool makeChange = false;  /// this bool variable is for top menu toogle
char status = 'n';
int padAmount = DEFAULT_PAD_AMOUNT;
volatile bool isInterrupt = false;
bool rechCardDetected = false;

byte arrow[8] = {
  0b00000,
  0b11100,
  0b10010,
  0b01001,
  0b01001, +0b10010,
  0b11100,
  0b00000
};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // initialize the lcd
  lcd.createChar(0, arrow);
  readFromEEPROM();
  pinSetup();
  disableCashAcceptor();
  if (mType == CASH_MACHINE) {
    attachInterrupt(digitalPinToInterrupt(interrupt), _interrupt, FALLING);
  } else {
    // detachInterrupt(digitalPinToInterrupt(interrupt));
    SPI.begin();         // Init SPI bus
    mfrc522.PCD_Init();  // Init MFRC522 card
  }
  if (EEPROM.read(firstBootAddress) != firstBoot) {
    Serial.println("Frist Boot");
    writeToEPPROM('y');
    delay(100);
    EEPROM.write(firstBootAddress, firstBoot);
  } else {
    Serial.println("Welcome Back");
    // writeToEPPROM('n');
  }
  // Serial.println(maxRackCapacity);
  // Serial.println(padAmount);
  // key for auth rfid
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  startMessage();
  // delay(2000);
  // menu();
  machineDetails();
  if (!digitalRead(menuButton)) {
    makeChange = true;
    changeDone = true;
    state = 1;
    status = 'n';
    menu();
    while (!digitalRead(menuButton)) {
      ;
    }
  } else
    enableCashAcceptor();
}

void loop() {
  menuManagement();
  if (mType == RFID_MACHINE) {
    manageRFID();  // when machine is rfid type
  } else if (mType == CASH_MACHINE) {
    // disable cash acceptor when stock is out
    if (getStock() > 0 && state == 0) {
      enableCashAcceptor();
    } else {
      disableCashAcceptor();
    }
    mangaeCash();  // when machine is pushbutton or coin based
  }
  // machineDetails();
}

void pinSetup() {
  pinMode(menuButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(okButton, INPUT_PULLUP);
  if (mType == CASH_MACHINE) {
    pinMode(inhibit, OUTPUT);
    pinMode(interrupt, INPUT_PULLUP);
  }
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  for (int i = 0; i < maxRack; i++) {
    pinMode(motor[i], OUTPUT);
    digitalWrite(motor[i], LOW);
  }
}
void menuManagement() {
  previous_time = millis();
  static bool buzzerNotification = false;
  while (!digitalRead(menuButton)) {
    if (!digitalRead(okButton) && digitalRead(selectButton) && mType == RFID_MACHINE) {
      if (millis() - previous_time >= 5000) {
        if (!buzzerNotification) {
          success(300);
          success(300);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Batch Write");
        lcd.setCursor(0, 1);
        lcd.print("Mode");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan Card!");

        while (digitalRead(okButton)) {
          Serial.println("inside ");
          if (mfrc522.PICC_IsNewCardPresent()) {
            if (mfrc522.PICC_ReadCardSerial()) {
              if (readCard()) {
                if (readByte[0] == FILLCARD || readByte[0] == RECHCARD || readByte[0] == PADCARD) {
                  lcd.setCursor(0, 1);
                  lcd.print("Error, Menu Card");
                  delay(1000);
                } else {
                  writeByte[0] = 0;
                  writeByte[15] = padAmount;
                  if (writeCard(writeByte)) {
                    lcd.setCursor(0, 1);
                    lcd.print("Done");
                    success(500);
                  }
                }
              }
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Scan Card!");
              halt();
            }
          }
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Exiting...");
        delay(500);
        state = 0;
        status = 'n';
        makeChange = true;
        changeDone = true;
        buzzerNotification = !buzzerNotification;
        menu();
      }
    } else if (!digitalRead(selectButton) && digitalRead(okButton)) {
      if (millis() - previous_time >= 5000) {
        success(300);
        success(300);
        /// filling all rack
        fillingAllRack();
        state = 0;
        status = 'n';
        makeChange = true;
        changeDone = true;
        menu();
      }
    } else {
      state = 0;
      status = 'v';
    }
  }

  if (status == 'n') {
    if (!digitalRead(selectButton)) {
      delay(300);
      topMenuPosition++;
      if (mType == RFID_MACHINE && topMenuPosition == 6)
        topMenuPosition = 0;
      if (mType == CASH_MACHINE && topMenuPosition >= 8)
        topMenuPosition = 0;
      makeChange = true;
      changeDone = true;
    }

    if (!digitalRead(okButton) && state == 1) {
      delay(300);
      if (state == 1 && topMenuPosition == 0)
        status = 'r';  // pads in  rack
      else if (state == 1 && topMenuPosition == 1)
        status = 'f';  // fill all
      else if (state == 1 && topMenuPosition == 2)
        status = 'M';  // max racks
      else if (state == 1 && topMenuPosition == 3)
        status = 'm';  // motor time
      else if (state == 1 && topMenuPosition == 4) {
        if (mType == RFID_MACHINE)
          status = 'b';  // batch write
        else
          status = 't';
      } else if (state == 1 && topMenuPosition == 5) {
        if (mType == RFID_MACHINE)
          status = 't';
        else
          status = 'c';  // cash limiting
      } else if (state == 1 && topMenuPosition == 6) {
        if (mType == CASH_MACHINE)
          status = 'p';  // pulses setting
      } else if (state == 1 && topMenuPosition == 7) {
        if (mType == CASH_MACHINE)
          status = 'w';  // inserting cash wait time  setting
      }
      // Serial.println(status);

      switch (status) {
        case 'f':
          fillMenu();
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              maxRackCapacity = maxRackCapacity + 1;
              if (maxRackCapacity > 60)
                maxRackCapacity = 1;
              fillMenu();
            }
          }
          fillAll(maxRackCapacity);
          break;
        case 'M':  // for maximum racks setting
          _maxRack();
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              maxRack = maxRack + 1;
              if (maxRack > availableRack)
                maxRack = 1;
              _maxRack();
            }
          }
          break;
        case 'm':
          motorTime();
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              motorTimeVariable = motorTimeVariable + 100;
              if (motorTimeVariable > 5000)
                motorTimeVariable = 100;
              motorTime();
            }
          }
          break;
        case 'r':
          for (int i = 0; i < maxRack; i++) {
            manageRack(i);
            while (digitalRead(okButton)) {
              // delay(300);
              if (!digitalRead(selectButton)) {
                delay(300);
                rack[i].incQuantity();
                manageRack(i);
              }
            }
            while (!digitalRead(okButton))
              ;
          }
          break;
        case 'b':
          batchWrite();
          while (digitalRead(okButton)) {
            if (!digitalRead(selectButton)) {
              delay(300);
              padAmountInc();
              batchWrite();
            }
          }
          while (!digitalRead(okButton))
            ;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Scan Card!");
          while (digitalRead(okButton)) {
            if (mfrc522.PICC_IsNewCardPresent()) {
              if (mfrc522.PICC_ReadCardSerial()) {
                //               writeByte[0] = 0;
                writeByte[15] = padAmount;
                if (writeCard(writeByte)) {
                  lcd.setCursor(0, 1);
                  lcd.print("Done");
                  success(500);
                }
              }
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Scan Card!");
              halt();
            }
          }
          // halt();
          while (!digitalRead(okButton))
            ;
          /////   batch code
          break;
        case 't':
          machineType();
          while (digitalRead(okButton)) {
            if (!digitalRead(selectButton)) {
              delay(300);
              if (mType == RFID_MACHINE) {
                mType = CASH_MACHINE;
              } else {
                mType = RFID_MACHINE;
              }
              machineType();
            }
          }
          while (!digitalRead(okButton))
            ;
          break;
        case 'c':
          cashLimit();
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              CASH_LIMIT = CASH_LIMIT + 5;
              if (CASH_LIMIT > MAX_CASH_LIMIT)
                CASH_LIMIT = 5;
              cashLimit();
            }
          }
          while (!digitalRead(okButton))
            ;
          break;
        case 'w':
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              cashWaitTime = cashWaitTime + 1000;
              if (cashWaitTime > MAX_CASH_WAIT_TIME)
                cashWaitTime = 1000;
              cashWait();
            }
          }
          while (!digitalRead(okButton))
            ;
          break;
        case 'p':
          pulseLimit();
          byte length = sizeof(availablePulses) / sizeof(availablePulses[0]);
          byte currentIndex = -1;
          for (int i = 0; i < length; i++) {
            if (availablePulses[i] == pulseForFiveRupeeNote) {
              currentIndex = i;
              break;  // Stop after finding the first match
            }
          }
          while (digitalRead(okButton)) {
            // delay(300);
            if (!digitalRead(selectButton)) {
              delay(300);
              currentIndex++;
              if (currentIndex >= length)
                currentIndex = 0;
              pulseForFiveRupeeNote = availablePulses[currentIndex];
              pulseLimit();
            }
          }
          while (!digitalRead(okButton))
            ;
          break;
      }
      writeToEPPROM(status);
      success(800);
      save();
      changeDone = true;
    }
  }

  menu();
  makeChange = false;
}

void motorTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Motor Time (ms)");
  lcd.setCursor(0, 1);
  // enter number
  lcd.print(motorTimeVariable);
}
void save() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Saving...");
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  status = 'n';
  state = 0;
}
void writeToEPPROM(char status) {

  if (status == 'r' || status == 'f') {
    for (int i = 0; i < maxRack; i++) {
      EEPROM.write(rackAddress[i], rack[i].getQuantity());
    }
    EEPROM.write(maxRackCapacityAddress, maxRackCapacity);
  } else if (status == 'm') {
    writeIntIntoEEPROM(motorTimeAddress, motorTimeVariable);
  } else if (status == 'b') {
    EEPROM.write(maxPadAddress, padAmount);
  } else if (status == 't') {
    EEPROM.write(mTypeAddress, mType);
  } else if (status == 'M') {
    EEPROM.write(maxRackAddress, maxRack);
  } else if (status == 'y') {
    // write when first boot
    EEPROM.write(mTypeAddress, mType);
    EEPROM.write(maxRackAddress, maxRack);
    EEPROM.write(maxPadAddress, padAmount);
    EEPROM.write(maxRackCapacity, maxRackAddress);
    writeIntIntoEEPROM(motorTimeAddress, motorTimeVariable);
    for (int i = 0; i < maxRack; i++) {
      EEPROM.write(rackAddress[i], maxRackCapacity);
    }
  } else if (status == 'c') {
    EEPROM.put(cashLimitAddress, CASH_LIMIT);
  } else if (status == 'p') {
    EEPROM.put(pulseForFiveRupeeNoteAddress, pulseForFiveRupeeNote);
    cashPulseFactor = (float)(5.0 / pulseForFiveRupeeNote);
  } else if (status == 'w') {
    EEPROM.put(cashWaitTimeAddress, cashWaitTime);
  }
}
void readFromEEPROM() {
  EEPROM.get(maxPadAddress,padAmount);
  if(padAmount <= 0){
    padAmount = DEFAULT_PAD_AMOUNT;
    EEPROM.put(maxPadAddress, padAmount);
  }
  EEPROM.get(mTypeAddress,mType);
  if(mType != CASH_MACHINE && mType != RFID_MACHINE){
    mType = DEFAULT_MACHINE_TYPE;
    EEPROM.put(mTypeAddress, mType);
  }
  EEPROM.get(maxRackAddress, maxRack);  /// upper three variable must be read first
  if(maxRack > availableRack || maxRack <= 0){
    maxRack = DEFAULT_MAX_RACK;
    EEPROM.put(maxRackAddress, maxRack);
  } 
  EEPROM.get(maxRackCapacityAddress, maxRackCapacity);
  if(maxRackCapacity <= 0){
    maxRackCapacity = DEFAULT_MAX_RACK_CAPACITY;
    EEPROM.put(maxRackCapacityAddress, maxRackCapacity);
  }
  motorTimeVariable = readIntFromEEPROM(motorTimeAddress);
  if(motorTimeVariable <= 0){
    motorTimeVariable = DEFAULT_MOTOR_TIME;
    EEPROM.put(motorTimeAddress, motorTimeVariable);
  }
  EEPROM.get(cashLimitAddress, CASH_LIMIT);
  if (CASH_LIMIT % FIVE_RUPEES != 0 || CASH_LIMIT > MAX_CASH_LIMIT) {
    CASH_LIMIT = DEFAULT_CASH_LIMIT;
    EEPROM.put(cashLimitAddress, DEFAULT_CASH_LIMIT);
  }

  EEPROM.get(pulseForFiveRupeeNoteAddress, pulseForFiveRupeeNote);
  bool valid = false;
  for (int i = 0; i < 4; i++) {
    if (pulseForFiveRupeeNote == availablePulses[i]) {
      valid = true;
      break;
    }
  }
  if (!valid) {
    pulseForFiveRupeeNote = availablePulses[0];
    writeIntIntoEEPROM(pulseForFiveRupeeNoteAddress, pulseForFiveRupeeNote);
  }
  // to update cashPulseFactor
  cashPulseFactor = (float)(5.0 / pulseForFiveRupeeNote);
  EEPROM.get(cashWaitTimeAddress, cashWaitTime);
  if (cashWaitTime > MAX_CASH_WAIT_TIME && cashWaitTime <= 0) {
    cashWaitTime = 5000;
    EEPROM.put(cashWaitTimeAddress, cashWaitTime);
  }
  for (int i = 0; i < maxRack; i++) {
    rack[i].setQuantity(EEPROM.read(rackAddress[i]));
  }
  for (int i = 0; i < maxRack; i++) {
    rack[i].setMaxQuantity(maxRackCapacity);
  }
}

void writeIntIntoEEPROM(int address, int number) {
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}
int readIntFromEEPROM(int address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void runMotor() {
  Serial.println("motor running");
  if (getStock() > 0) {
    for (int i = 0; i < maxRack; i++) {
      if (rack[i].getQuantity() != 0) {
        // Serial.println("motor");
        digitalWrite(motor[i], HIGH);
        delay(motorTimeVariable);
        digitalWrite(motor[i], LOW);
        rack[i].decQuantity();
        break;
      }
    }
    writeToEPPROM('r');
  }
}
void fillAll(int num) {
  for (int i = 0; i < maxRack; i++) {
    rack[i].setQuantity(num);
  }
}

int getStock() {
  int totalStock = 0;
  for (int i = 0; i < maxRack; i++) {
    totalStock += rack[i].getQuantity();
  }
  return totalStock;
}
void success(int _time) {
  digitalWrite(buzzer, HIGH);
  delay(_time);
  digitalWrite(buzzer, LOW);
}
void warning() {
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
}
void padAmountInc() {
  padAmount++;
  if (padAmount > 255) {
    padAmount = 1;
  }
}
void machineDetails() {
  String var = "\nMachine Type : " + String(mType);
  var += "\nMaximum Rack: " + String(maxRack);
  var += "\nRack Capacity: " + String(maxRackCapacity);
  var += "\nPad Amount: " + String(padAmount);
  for (int i = 0; i < maxRack; i++) {
    var += "\nRack " + String(i + 1) + " Quantity: " + String(rack[i].getQuantity());
  }

  Serial.println(var);
  delay(1000);
}