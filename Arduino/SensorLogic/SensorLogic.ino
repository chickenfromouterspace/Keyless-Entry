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
/* Include the standard Arduino SPI library */
#include <SPI.h>
/* Include the RFID library */
#include <RFID.h>
#include <SoftwareSerial.h>
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
void Servoloop(void);

Servo myservo;

int pos = 90;
char choice;

void setup()
{
 Serial.begin(115200);
 while (!Serial)
 {
    ; // wait for serial port to connect. Needed for native USB port only
 }
 Fingerprintsetup();
 RFIDsetup();
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
    
    Serial.println();
    Serial.println();
    Servoloop();
  }
  delay(1000);
}

void Servoloop()
{
  myservo.attach(7);
  for(pos = 0; pos <= 180; pos += 1)
  {
    myservo.write(pos);
    Serial.println(myservo.read());
    delay(100);
  }
  for(pos = 180; pos >= 0; pos -= 1)
  {
    myservo.write(pos);
    Serial.println(myservo.read());
    delay(100);
  }
  myservo.detach();
}
