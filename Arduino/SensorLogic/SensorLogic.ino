/*
PINOUT:
RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D9
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V
*/

/*#define  Finger_RST_Pin     24
#define  Finger_WAKE_Pin    23*/

/* Include the standard Arduino SPI library */
#include <SPI.h>
/* Include the RFID library */
#include <RFID.h>
#include "finger.h"
#include <Servo.h>
#include "Keypad.h"
#include "EEPROM.h"
/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 9
#define RESET_DIO 8
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 
// read a line from user into buffer, return char count

void Fingerprintsetup(void);
void Fingerprintloop(void);
void RFIDsetup(void);
void RFIDloop(void);
void Servoloop(int dir);
void Bluetoothsetup(void);
void Bluetoothloop(void);
void Keypadsetup(void);
void Keypadloop(void);
void WiFisetup(void);
void WiFiloop(void);

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

Servo myservo;

int pos = 90;
char choice;
String str = String(11);
unsigned char usr0[5] = {0xA2, 0x7D, 0x51, 0x1C, 0x92},
              usr1[5] = {0x92, 0xA2, 0x3C, 0x1C, 0x10}, 
              usr2[5] = {0x99, 0x80, 0x6E, 0xA2, 0xD5}, 
              usr3[5] = {0x49, 0x95, 0x0, 0xA3, 0x7F};

void setup()
{
 Serial.begin(115200);
 while (!Serial)
 {
    ; // wait for serial port to connect. Needed for native USB port only
 }
 Fingerprintsetup();
 RFIDsetup();
 Bluetoothsetup();
 Keypadsetup();
 WiFisetup();
}

void loop()
{
  Serial.println("Which sensor do you want to use?");
  Serial.println("1 for RFID");
  Serial.println("2 for Fingerprint");
  Serial.println("3 for Bluetooth");
  Serial.println("4 for Keypad");
  Serial.println("5 for WiFi");
    
  while(Serial.available()<1){

  }


    choice = Serial.read();
    Serial.print("You chose: ");
    Serial.println(choice);
    Serial.println("Press the RST button to pick again.");
    if((char)choice == '1' )
    {
      while(1)
      {
        RFIDloop();
      }
    }
    else if((char)choice == '2')
    {
      while(1)
      {
        Fingerprintloop();
      }
    }
    else if((char)choice == '3')
    {
      while(1)
      {
        Bluetoothloop();
      }
    }
    else if((char)choice == '4')
    {
      while(1)
      {
        Keypadloop();
      }
    }
    else if((char)choice == '5')
    {
      while(1)
      {
        WiFiloop();
      }
    }
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
}

void Fingerprintloop()
{   
  Analysis_PC_Command();
  
  // If in sleep mode, turn on the auto wake-up function of the finger, 
  //begin to check if the finger is pressed, and wake up the module and match it
  if(Finger_SleepFlag == 1)
  {
    Auto_Verify_Finger();
  }
}

void RFIDsetup()
{ 
  /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();
}

void RFIDloop()
{
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.println("Card detected:");
    for(int i=0;i<5;i++)
    {
    Serial.print(RC522.serNum[i],DEC);
    //Serial.print(RC522.serNum[i],HEX); //to print card detail in Hexa Decimal format
    }
    Servoloop(0);
    Serial.println();
    Serial.println();
  }
  delay(1000);
}

void Servoloop(int dir)
{
  myservo.attach(7);
  myservo.write(dir);
  delay(1000);
  myservo.detach();
}

void Bluetoothsetup()
{
  Serial3.begin(9600);
  while(!Serial);
  Serial.println("AT commands: okay");
}

void Bluetoothloop()
{
  //read from the HM-10 and print in the Serial
  if(Serial3.available())
  {
    str = Serial3.readStringUntil('\n');
    Serial.println(str.length());
    if(str.compareTo("255,255,255") == 0)
      Servoloop(0);
    if(str.compareTo("0,0,0") == 0)
      Servoloop(180);
  }
  if(Serial.available())
    Serial3.write(Serial.read());
}
//----------------------------------------------------------------------------------



void Keypadsetup()
{
  pinMode(unlock_motor, OUTPUT);
  pinMode(incorrect_pswd, OUTPUT);
  Serial.println("Keyless Entry");
  //liquid_crystal_display.begin(16,2);
  //liquid_crystal_display.print("UCF Knights");
  //liquid_crystal_display.setCursor(0,1);
  //liquid_crystal_display.print("Keyless Entry");
  delay(2000);
  //liquid_crystal_display.clear();
  Serial.println("Enter Password");
  //liquid_crystal_display.print("Enter Password");
  //liquid_crystal_display.setCursor(0,1);
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
    //liquid_crystal_display.print(key_pressed);
      }
  if(i==4)
  {
    delay(200);
    for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
    if(!(strncmp(password, initial_password,4)))
    {
      //liquid_crystal_display.clear();
      Serial.println("Password Accepted");
      //liquid_crystal_display.print("Pass Accepted");
      digitalWrite(unlock_motor, HIGH);
      Servoloop(0);

      delay(2000);
      //liquid_crystal_display.setCursor(0,1);
      Serial.println("Press # to change");
      //liquid_crystal_display.print("Pres # to change");
      delay(2000);
      //liquid_crystal_display.clear();
      Serial.println("Enter Password");
      //liquid_crystal_display.print("Enter Password:");
      //liquid_crystal_display.setCursor(0,1);
      i=0;
    }
    else
    {
      digitalWrite(unlock_motor, LOW);
      //liquid_crystal_display.clear();
      Serial.println("Wrong Password");
      //liquid_crystal_display.print("Wrong Password");
      digitalWrite(incorrect_pswd, HIGH);
      //liquid_crystal_display.setCursor(0,1);
      Serial.println("Press # to change");
      //liquid_crystal_display.print("Pres # to Change");
      delay(2000);
      //liquid_crystal_display.clear();
      Serial.println("Enter Password");
      //liquid_crystal_display.print("Enter Password");
      //liquid_crystal_display.setCursor(0,1);
      i=0;
    }
  }
}

void change()
{
  int j=0;
  //liquid_crystal_display.clear();
  Serial.println("Current Password");
  //liquid_crystal_display.print("Current Password");
  //liquid_crystal_display.setCursor(0,1);
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)
    {
      new_password[j++]=key;
      Serial.println(key);
      //liquid_crystal_display.print(key);
    }
    key=0;
  }
  delay(500);
  if((strncmp(new_password, initial_password, 4)))
  {
    //liquid_crystal_display.clear();
    Serial.println("Wrong Password");
    //liquid_crystal_display.print("Wrong Password");
    //liquid_crystal_display.setCursor(0,1);
    Serial.println("Try Again");
    //liquid_crystal_display.print("Try Again");
    delay(1000);
  }
  else
  {
    j=0;
    //liquid_crystal_display.clear();
    Serial.println("New Password");
    //liquid_crystal_display.print("New Password:");
    //liquid_crystal_display.setCursor(0,1);
    while(j<4)
    {
      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        Serial.println(key);
        //liquid_crystal_display.print(key);
        EEPROM.write(j,key);
        j++;
      }
    }
    Serial.println("Password Changed");
    //liquid_crystal_display.print("Pass Changed");
    delay(1000);
  }
  //liquid_crystal_display.clear();
  Serial.println("Enter Password");
  //liquid_crystal_display.print("Enter Password");
  //liquid_crystal_display.setCursor(0,1);
  key_pressed=0;
}

void initialpassword(){
  for(int j=0;j<4;j++)
    EEPROM.write(j, j+49);
  for(int j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);
}

void WiFisetup()
{
  Serial1.begin(115200);
}

void WiFiloop()
{
  char x;
  if(Serial1.available())
  {
    x = Serial1.read();
    Serial.println(x);
    if(x == '1')
    {
      myservo.attach(7);
      Serial1.end();
      myservo.write(0);
      delay(1000);
      Serial.println("Running.");
      myservo.detach();
      x = 0;
      Serial1.begin(115200);
    }
  }
}
