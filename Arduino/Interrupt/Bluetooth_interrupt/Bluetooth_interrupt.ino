#define BLUETOOTH_STATE_PIN 3

String str = String(11);

void setup()
{
  Serial.begin(115200);
  Serial3.begin(9600);
  pinMode(BLUETOOTH_STATE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLUETOOTH_STATE_PIN), Bluetoothinterrupt, CHANGE);
  while(!Serial);
  Serial.println("AT commands: okay");
}

void loop()
{
  Serial.println(digitalRead(BLUETOOTH_STATE_PIN));
  delay(1000);
}

void Bluetoothinterrupt()
{
  //read from the HM-10 and print in the Serial
  if(Serial3.available())
  {
    str = Serial3.readString();
    Serial.println(str.length());
    if(str.compareTo("255,255,255\n") == 0)
      Serial.println("Yes.");
  }
  if(Serial.available())
    Serial3.write(Serial.read());
}
