/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Minimal example how to use the interrupts to read the UID of a MIFARE Classic PICC
 * (= card/tag).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        3                10
 * IRQ         ?            ?             ?         ?          2                10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */
 
/*#define  Finger_RST_Pin     24
#define  Finger_WAKE_Pin    23*/

/* Include the standard Arduino SPI library */
#include <SPI.h>
#include <MFRC522.h>
/* Include the RFID library */
#include "finger.h"
#include <Servo.h>
#include "Keypad.h"
#include "EEPROM.h"
#include <SimpleTimer.h>

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define RST_PIN             8           // Configurable, see typical pin layout above
#define SS_PIN              53           // Configurable, see typical pin layout above
#define IRQ_PIN             2           // Configurable, depends on hardware
#define BLUETOOTH_STATE_PIN 3
#define LED                 13
#define SERVO_PIN           5

SimpleTimer timer;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

volatile bool bNewInt = false;
byte regVal = 0x7F;
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);

void Fingerprintsetup(void);
void Fingerprintloop(void);
void FingerprintSleepMode(void);
void RFIDsetup(void);
void RFIDloop(void);
void Bluetoothsetup(void);
void Bluetoothloop(void);
void Keypadsetup(void);
void Keypadloop(void);
void Unlock_Door(void);
void Auto_Lock(void);

char password[4];
char initial_password[4],new_password[4];
int i=0;
int unlock_motor = 10;
int incorrect_pswd = 11;
char key_pressed=0;
const byte rows = 4; 
const byte columns = 3; 
char hexaKeys[rows][columns] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte row_pins[rows] = {A1, A6, A5, A3};
byte column_pins[columns] = {A2, A0, A4};

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

Servo servo1;
Servo servo2;

int pos = 90;
char choice;
String str = String(11);
boolean BLEon = false;
boolean DoorIsLocked = true;

void setup()
{
 Serial.begin(115200);
 while (!Serial)
 {
    ; // wait for serial port to connect. Needed for native USB port only
 }
 Bluetoothsetup();
 RFIDsetup();
 Keypadsetup();
}

void loop()
{
  timer.run();
  Bluetoothloop();
  RFIDloop();
  Keypadloop();
}

void Fingerprintsetup()
{
  pinMode(Finger_RST_Pin  , OUTPUT);
  digitalWrite(Finger_RST_Pin , LOW);
  delay(100);
  digitalWrite(Finger_RST_Pin , HIGH);
  pinMode(Finger_WAKE_Pin , INPUT);
  Finger_SoftwareSerial_Init(); 
  Finger_Wait_Until_OK();

  //Enter Sleep Mode
  digitalWrite(Finger_RST_Pin , LOW);
  Finger_SleepFlag = 1;
}

void Fingerprintloop()
{
  // If in sleep mode, turn on the auto wake-up function of the finger, 
  //begin to check if the finger is pressed, and wake up the module and match it
  if(Finger_SleepFlag == 1)
  {
    if(digitalRead(Finger_WAKE_Pin) == HIGH)   // If you press your finger
  { 
    delay(20);      
    if(digitalRead(Finger_WAKE_Pin) == HIGH)   
    {
      FingerprintSleepMode();
    }
  }
  }
}

void Bluetoothsetup()
{
  Serial2.begin(9600);
  Serial.println("AT commands: okay");
  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_STATE_PIN), Bluetoothcheck, CHANGE);
}

void Bluetoothloop()
{
  if(BLEon)
  {
    //read from the HM-10 and print in the Serial
    if(Serial2.available())
    {
      str = Serial2.readString();
      Serial.print(str);
      if(str.compareTo("255,255,255\n") == 0)
      {
        Serial.println("Yes.");
        if(DoorIsLocked == true)
        {
          DoorIsLocked = false;
          Unlock_Door();
        }
      }
    }
    if(Serial.available())
      Serial2.write(Serial.read());
  }
}

void Bluetoothcheck()
{
  if(digitalRead(3) == 1)
  {
    BLEon = true;
    Serial.println("Bluetooth On.");
  } 
  else
  {
    BLEon = false;
    Serial.println("Bluetooth Off.");
  }
}

void Keypadsetup()
{
  pinMode(unlock_motor, OUTPUT);
  pinMode(incorrect_pswd, OUTPUT);
  Serial.println("Keyless Entry");
  initialpassword();
}

void Keypadloop()
{
  digitalWrite(unlock_motor, LOW);
  digitalWrite(incorrect_pswd, LOW);
  key_pressed = keypad_key.getKey();
  if(key_pressed=='#')
    change();
  if (key_pressed)
  {
    password[i++]=key_pressed;
    Serial.println(key_pressed);
      }
  if(i==4)
  {
    delay(200);
    for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
    if(!(strncmp(password, initial_password,4)))
    {
      Serial.println("Password Accepted");
      if(DoorIsLocked == true)
      {
        DoorIsLocked = false;
        Unlock_Door();
      }
      i=0;
    }
    else
    {
      Serial.println("Wrong Password");
      digitalWrite(incorrect_pswd, HIGH);
      i=0;
    }
  }
}

void change()
{
  int j=0;
  Serial.println("Current Password");
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)
    {
      new_password[j++]=key;
      Serial.println(key);
    }
    key=0;
  }
  delay(500);
  if((strncmp(new_password, initial_password, 4)))
  {
    Serial.println("Wrong Password");
    Serial.println("Try Again");
    delay(500);
  }
  else
  {
    j=0;
    Serial.println("New Password");
    while(j<4)
    {
      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        Serial.println(key);
        EEPROM.write(j,key);
        j++;
      }
    }
    Serial.println("Password Changed");
  }
  Serial.println("Enter Password");
  key_pressed=0;
}

void initialpassword(){
  for(int j=0;j<4;j++)
    EEPROM.write(j, j+49);
  for(int j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);
}

void FingerprintSleepMode()
{
      digitalWrite(Finger_RST_Pin , HIGH);    // Pull up the RST to start the module and start matching the fingers
      delay(1000);   // Wait for module to start
          
      Serial.println("Waiting Finger......Please try to place the center of the fingerprint flat to sensor !");
      switch(VerifyUser())
      {
        case ACK_SUCCESS: 
          Serial.println("Matching successful !");
          break;
        case ACK_NO_USER:
          Serial.println("Failed: This fingerprint was not found in the library !");
          break;
        case ACK_TIMEOUT: 
          Serial.println("Failed: Time out !");
          break;  
        case ACK_GO_OUT:
          Serial.println("Failed: Please try to place the center of the fingerprint flat to sensor !");
          break;
      }
      
      //After the matching action is completed, drag RST down to sleep
      //and continue to wait for your fingers to press
      digitalWrite(Finger_RST_Pin , LOW);
}

void RFIDsetup() {
  SPI.begin();          // Init SPI bus

  mfrc522.PCD_Init(); // Init MFRC522 card

  /* read and printout the MFRC522 version (valid values 0x91 & 0x92)*/
  Serial.print(F("Ver: 0x"));
  byte readReg = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.println(readReg, HEX);
  /* setup the IRQ pin*/
  pinMode(IRQ_PIN, INPUT_PULLUP);
  /*
   * Allow the ... irq to be propagated to the IRQ pin
   * For test purposes propagate the IdleIrq and loAlert
   */
  regVal = 0xA0; //rx irq
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);
  bNewInt = false; //interrupt flag
  /*Activate the interrupt*/
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
  bNewInt = false;

  Serial.println(F("End setup"));
}

void RFIDloop() {
  if (bNewInt) { //new read interrupt
    clearInt(mfrc522);
    Serial.print(F("Interrupt. "));
    mfrc522.PICC_ReadCardSerial(); //read the tag data
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    if(DoorIsLocked == true)
    {
      DoorIsLocked = false;
      Unlock_Door();
    }
    mfrc522.PICC_HaltA();
    delay(100);
  }

  // The receiving block needs regular retriggering (tell the tag it should transmit??)
  // (mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg,mfrc522.PICC_CMD_REQA);)
  activateRec(mfrc522);
  bNewInt = false;
  delay(125);
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
/**
 * MFRC522 interrupt serving routine
 */
void readCard() {
  bNewInt = true;
}

/*
 * The function sending to the MFRC522 the needed commands to activate the reception
 */
void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}

/*
 * The function to clear the pending interrupt bits after interrupt serving routine
 */
void clearInt(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}

void Unlock_Door() 
{
  
  Serial.write("Unlocking Door \n");
  servo1.attach(SERVO_PIN);                         // attaches the servo to pin on Atmega
  int pos1;

  //for (pos1 = 0; pos1 <= 20; pos1 += 1) 
  for (pos1 = 180; pos1 >= 160; pos1 -= 1) 
  { 
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
  servo1.detach();
  Serial.println("Door Unlocked");
  timer.setTimeout(5000, Auto_Lock);
}

void Auto_Lock() 
{
  Serial.print("Locking Door \n");
  servo2.attach(SERVO_PIN);  // attaches the servo on GIO2 to the servo object
  int pos2;
   
  for (pos2 = 180; pos2 >= 160; pos2 -= 1) 
  { 
   
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);  
    Serial.println(pos2);
    // waits 15ms for the servo to reach the position
  }
  
  Serial.println("Door Locked");
  servo2.detach();
  delay(15);
  digitalWrite(LED,LOW);             // Turn Off the LED.
  DoorIsLocked = true;
}
