#define BLYNK_PRINT Serial
#include <BlynkSimpleSerialBLE.h>
#include <SoftwareSerial.h>
SoftwareSerial SwSerial(34, 35); // RX, TX


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "d51JGrzELHUnd5l3BZOtZbZN56d8vhI-";

SoftwareSerial SerialBLE(34, 35); // RX, TX

void setup()
{
  // Debug console
  Serial.begin(115200);

  SerialBLE.begin(115200);
  Blynk.begin(SerialBLE, auth);

  Serial.println("Waiting for connections...");
}

void loop()
{
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
