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
/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 9
#define RESET_DIO 8
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 
// read a line from user into buffer, return char count

char choice;

void setup()
{
 Serial.begin(115200);
 RFIDsetup();
 //Fingerprintsetup();
}
void loop(){
Serial.println("Which sensor do you want to use?");
Serial.println("1 for RFID");
Serial.println("2 for Fingerprint");

  while(Serial.available()<1){

  }
 
    choice = Serial.read();
    Serial.print("You chose: ");
    Serial.println(choice);
    if((char)choice == '1' )
      while(1){
      RFIDloop();
      choice = Serial.read();
      if((char)choice == '~')
      break;
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
  }
  delay(1000);
}
