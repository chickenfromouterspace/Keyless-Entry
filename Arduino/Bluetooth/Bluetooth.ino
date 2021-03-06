String str = String(11);
boolean BLEon = false;

void setup()
{
  Serial.begin(115200);
  Serial3.begin(9600);
  while(!Serial);
  Serial.println("AT commands: okay");
  attachInterrupt(digitalPinToInterrupt(3), check, CHANGE);
}

void loop()
{
  if(BLEon)
  {
    //read from the HM-10 and print in the Serial
    if(Serial3.available())
    {
      str = Serial3.readString();
      Serial.print(str);
      if(str.compareTo("255,255,255\n") == 0)
        Serial.println("Yes.");
    }
    if(Serial.available())
      Serial3.write(Serial.read());
  }
}

void check()
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
