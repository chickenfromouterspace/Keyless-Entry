#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "rXdCyr7ngI3v_RYtsVhjVLRfoVABi_qS";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MySpectrumWiFi30-2G";
char pass[] = "famousbird589";
#include <Servo.h>
#include <millisDelay.h>
#define LED V1
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

Servo servo1;
Servo servo2;
int doorState; // 1 for locked, 0  for unlocked
BlynkTimer timer;


void loop()
{
  Blynk.run();
  timer.run();
 
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  doorState=1;
  checklock_state();
}

void checklock_state()
{
  //change LED color
  Blynk.setProperty(LED, "color", BLYNK_RED);

  //change LED label
  Blynk.setProperty(LED, "label", "Door Locked");
  WidgetLED led(V1);
  led.on();
}

BLYNK_WRITE(V0) 
  {
    int pinValue = param.asInt();
    if ((pinValue == 1) && (doorState==1)) // if Button sends 1 and door is locked
    {
      doorState=0;          //set door to unlocked, blocking continuous unlocking commands
      Unlock_Door();              // start the function 
      
    }
  }


void Unlock_Door() 
{
  
  Serial.println("Unlocking Door");
  servo1.attach(4);  // attaches the servo on GIO2 to the servo object
  int pos1;

  for (pos1 = 0; pos1 <= 40; pos1 += 1) 
  { 
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  //change LED color
  Blynk.setProperty(LED, "color", BLYNK_GREEN);

  //change LED label
  Blynk.setProperty(LED, "label", "Door Unlocked");
  servo1.detach();
  timer.setTimeout(5000L, Auto_Lock);
}

void Auto_Lock() 
{
  servo2.attach(4);  // attaches the servo on GIO2 to the servo object
  int pos2;
   for (pos2 = 160; pos2 >= 90; pos2 -= 1) 
  { 
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  servo2.detach();
  doorState=1;
  //change LED color
  Blynk.setProperty(LED, "color", BLYNK_RED);

  //change LED label
  Blynk.setProperty(LED, "label", "Door Locked");
}
