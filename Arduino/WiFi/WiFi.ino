#include <Servo.h>

void setup()
{
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop()
{
  if(Serial1.available())
  {
    Serial.println(Serial1.read());  
  }
}
