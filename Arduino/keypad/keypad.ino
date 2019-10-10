#include <OnewireKeypad.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,5,6,7);

char KEYS[] = {

  '1', '2', '3',

  '4', '5', '6',

  '7', '8', '9',

  '*', '0', '#',

};

OnewireKeypad <Print, 16 > KP2(Serial, KEYS, 4, 4, A0, 4700, 1000 );

void setup () {

  lcd.begin(16, 2);

  Serial.begin(9600);

  KP2.SetKeypadVoltage(3.3);

}

void loop()

{

  char Key;

  byte KState = KP2.Key_State();
  
  if (KState == PRESSED)

  {

    if ( Key = KP2.Getkey() )
    
    {

      Serial << "Key: " << Key << "\n";

    }

  }

}
