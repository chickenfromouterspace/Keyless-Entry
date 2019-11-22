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
 * RST/Reset   RST          9             8         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            20        D10        3                10
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
#define IRQ_PIN             69           // Configurable, depends on hardware
#define BLUETOOTH_STATE_PIN 3
#define ServoPin            5
#define WiFiInt             13
#define AUTOLOCK            21
#define greenLED            11
#define redLED              12
#define RFID                19

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
void EasyRFIDsetup(void);
void RFIDloop(void);
void EasyRFIDloop(void);
void Bluetoothsetup(void);
void Bluetoothloop(void);
void Keypadsetup(void);
void Keypadloop(void);
void Wifisetup(void);
void Wifiloop(void);
void Unlock_Door(void);
void Auto_Lock(void);
void recvWithStartEndMarkers(void);
void showNewData(void);
void update_doorState(void);
void blinkLED(void);
boolean array_cmp(char *a, char *b, char len_a, char len_b);


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
int doorState; // 1 for locked, 0  for unlocked
const byte numChars = 13;
char receivedChars[numChars];
char unlockdoor[numChars]= "unlockdoor";
boolean newData = false;
boolean CheckSerial= true;

void setup()
{
 Serial.begin(115200);
 attachInterrupt(digitalPinToInterrupt(AUTOLOCK), Auto_Lock_timer, FALLING);
 pinMode(AUTOLOCK, INPUT_PULLUP);
 pinMode(RFID, INPUT_PULLUP);
 pinMode(greenLED, OUTPUT);
 pinMode(redLED, OUTPUT);
 digitalWrite(greenLED, LOW);
 digitalWrite(redLED, LOW);
                                              //Enable timer for autoLock function
 
 while (!Serial)
 {
    ; // wait for serial port to connect. Needed for native USB port only
 }
 //RFIDsetup();
 Keypadsetup();
 Wifisetup();
 Bluetoothsetup();
}

void loop()
{
  timer.run(); 
  Wifiloop();
  checkRFIDrecv();
 // RFIDloop();
  Keypadloop();
  Bluetoothloop();
  //timer.setTimeout(300, blinkLED);
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
  Serial3.begin(115200);
  Serial.println("AT commands: okay");
  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_STATE_PIN), Bluetoothcheck, CHANGE);
}

void Bluetoothloop()
{
  if(BLEon)
  {
    //read from the HM-10 and print in the Serial
    if(Serial3.available())
    {
      str = Serial3.readString();
      Serial.print(str);
      if(str.compareTo("255,0,0\n") == 0)
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
      Serial3.write(Serial.read());
  }
}

void Bluetoothcheck()
{
  if(digitalRead(BLUETOOTH_STATE_PIN) == 1)
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
    digitalWrite(greenLED,HIGH);
    delay(20);
    digitalWrite(greenLED,LOW);
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
      digitalWrite(redLED, HIGH);
      delay(50);
      digitalWrite(redLED,LOW);
      delay(50);
      digitalWrite(redLED, HIGH);
      delay(50);
      digitalWrite(redLED, LOW);
      delay(50);
      digitalWrite(redLED, HIGH);
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
      digitalWrite(greenLED,HIGH);
      delay(20);
      digitalWrite(greenLED,LOW);    
    }
    key=0;
  }
  delay(500);
  if((strncmp(new_password, initial_password, 4)))
  {
    Serial.println("Wrong Password");
    Serial.println("Try Again");
    digitalWrite(redLED, HIGH);
    delay(50);
    digitalWrite(redLED,LOW);
    delay(50);
    digitalWrite(redLED, HIGH);
    delay(50);
    digitalWrite(redLED, LOW);
    delay(50);
    digitalWrite(redLED, HIGH);
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
        digitalWrite(greenLED,HIGH);
        delay(20);
        digitalWrite(greenLED,LOW);
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
  delay(100);
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
  if (bNewInt == true) { //new read interrupt
    mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
    Serial.print(F("Interrupt. "));
    mfrc522.PICC_ReadCardSerial(); //read the tag data
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    if(DoorIsLocked == true)
    {
      Serial.println("RFID");
      DoorIsLocked = false;
      Unlock_Door();
    }
    mfrc522.PICC_HaltA();
    delay(100);
  }

  // The receiving block needs regular retriggering (tell the tag it should transmit??)
  //mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg,mfrc522.PICC_CMD_REQA);
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
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  DoorIsLocked=false;
  digitalWrite(WiFiInt, LOW);
  Serial1.write("<unlockdoor>");
  Serial.write("Unlocking Door \n");
  servo1.attach(ServoPin);                         // attaches the servo to pin 4 on Atmega
  int pos1;

  //for (pos1 = 0; pos1 <= 20; pos1 += 1) 
  for (pos1 = 180; pos1 >= 160; pos1 -= 1) 
  { 
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
  servo1.detach();
  Serial.println("Door Unlocked");
  doorState=0;
  timer.setTimeout(5000, Auto_Lock);
}

void  Auto_Lock_timer()
{
  //debounce button
   timer.setTimeout(10, Lock_Buttoncheck);
}

void Lock_Buttoncheck()
{
  Serial.println("Debounce.");
  if((digitalRead(AUTOLOCK))==LOW)
  {
    detachInterrupt(digitalPinToInterrupt(AUTOLOCK));
    Auto_Lock();
    delay(2000);
    attachInterrupt(digitalPinToInterrupt(AUTOLOCK), Auto_Lock_timer, FALLING);
  }
}


void Auto_Lock() 
{
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, HIGH);
  Serial.write("Locking Door \n");
  servo2.attach(ServoPin);  // attaches the servo on GIO2 to the servo object
  int pos2;
   
  //for (pos2 = 180; pos2 >= 160; pos2 -= 1)
  for (pos2 = 0; pos2 <= 20; pos2 += 1)  
  { 
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);  
  }
           
  Serial.println("Door Locked");
  servo2.detach();
  delay(15);
  Serial1.write("<doorlocked>");
  DoorIsLocked=true;
  digitalWrite(WiFiInt, HIGH);
 // CheckSerial=true;

  
}

void recvWithStartEndMarkers() 
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();
        //if(CheckSerial==true)
        
        if (recvInProgress == true) {
            if (rc != endMarker) {
              
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
                Serial.println("error");
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
            Serial.println("false statement");
        }
        
    }
}

void showNewData() 
{
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
        //CheckSerial=false;
        update_doorState();
    }
}

void update_doorState()
{
  if(array_cmp(receivedChars, unlockdoor,13, 13) == true)
  {
    Serial.println("TRUE");
    if(DoorIsLocked==true)
    {
    DoorIsLocked=false;
    Unlock_Door();
    }
  }

  else
  {
    Serial.println("FALSEcmp");
    Serial.println(receivedChars);
  }
}

boolean array_cmp(char *a, char *b, char len_a, char len_b){
     int n;

     // if their lengths are different, return false
     if (len_a != len_b) return false;

     // test each element to be the same. if not, return false
     for (n=0;n<len_a;n++) if (a[n]!=b[n]) return false;

     //ok, if we have not returned yet, they are equal :)
     return true;
}

void Wifisetup()
{
  Serial1.begin(115200);
  pinMode(WiFiInt, OUTPUT);
  digitalWrite(WiFiInt, HIGH);
}

void Wifiloop()
{
    //timer.run();                                               //Enable timer for autoLock functiom
    recvWithStartEndMarkers();
    showNewData();
}

void blinkLED()
{
  if(DoorIsLocked==true)
  {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, CHANGE);
  }

  if(DoorIsLocked==false)
  {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, CHANGE);
  }
}

void EasyRFIDsetup()
{
  SPI.begin();
  mfrc522.PCD_Init();
}

void EasyRFIDloop()
 {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())    return;

  Serial.print("Card UID:");    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i]);
    Serial.println(" ");
  
    //          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //          Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
    delay(2000);
    Serial.print("unlock"); 
    Unlock_Door();  
} 

void checkRFIDrecv()
{
  if((digitalRead(RFID) == LOW)&&(DoorIsLocked==true))
    {
      Serial.println("RFID Unlock");
      Unlock_Door();
    }
}
