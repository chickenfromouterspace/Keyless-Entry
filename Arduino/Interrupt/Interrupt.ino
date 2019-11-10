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

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 9
#define RESET_DIO 8
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 
String str = String(11);
unsigned char usr0[5] = {0xA2, 0x7D, 0x51, 0x1C, 0x92},
              usr1[5] = {0x92, 0xA2, 0x3C, 0x1C, 0x10}, 
              usr2[5] = {0x99, 0x80, 0x6E, 0xA2, 0xD5}, 
              usr3[5] = {0x49, 0x95, 0x0, 0xA3, 0x7F};
              
volatile bool RFIDpresent;

void setup() {
  Serial.begin(115200);
  /* Enable the SPI interface */
  SPI.begin();
  /* Initialise the RFID reader */
  RC522.init();
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), RFIDinterrupt, CHANGE);
}

void loop() {
  if(RFIDpresent == true)
  {
    Serial.println("Interrupt triggered.");
    RFIDpresent = false;
  }
}

void RFIDinterrupt()
{
  RFIDpresent = true;
}
