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


void setup()
{
  Serial.begin(115200);
  /* Enable the SPI interface */
  SPI.begin();
  /* Initialise the RFID reader */
  RC522.init();
}

void loop()
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

    Serial.println(match);
    match = 0;
  }
  delay(1000);
}
