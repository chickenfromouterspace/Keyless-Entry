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
//#define  Finger_RST_Pin     24
//#define  Finger_WAKE_Pin    23
/* Include the standard Arduino SPI library */
#include <SPI.h>
/* Include the RFID library */
#include <RFID.h>
#include "finger.h"
#include <Servo.h>
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
}

void loop()
{
  Serial.println("Which sensor do you want to use?");
  Serial.println("1 for RFID");
  Serial.println("2 for Fingerprint");

  
 
    choice = '1';
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
  unsigned char buff[6] = {'\0'};
  int match;
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.println("Card detected:");
    for(int i=0;i<5;i++)
    {
      //Serial.print(RC522.serNum[i],DEC);
      Serial1.println(RC522.serNum[i],HEX); //to print card detail in Hexa Decimal format
      buff[i] = RC522.serNum[i];
    }
    str = Serial1.readStringUntil('\n');
    Serial.println();
    for(int i=0;i<5;i++)
    {
      Serial.println(buff[i], HEX);
      Serial.println(buff[i], BIN);
    }
    Serial.println();
    for(int i=0;i<5;i++)
    {
      Serial.println(usr3[i], HEX);
      Serial.println(usr3[i], BIN);
    }
    Serial.println();

    match = 0;
    for(int i=0;i<5;i++)
    {
      if(buff[i] != usr3[i])
        break;
      else
        match++;
    }
    if(match == 5)
      Servoloop(0);

    Serial.println(match);
    match = 0;
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
