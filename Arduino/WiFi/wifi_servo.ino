/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
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

Servo servo1;
Servo servo2;
int i,x;

void servo() 
{
  i = 0;
  int pinValue = 0;  // Set V3 status to 0 to quit, unless button is still pushed (as per below)
  Serial.write("0");
  servo1.attach(2);  // attaches the servo on GIO2 to the servo object
   int pos1;

  for (pos1 = 0; pos1 <= 40; pos1 += 1) 
  { 
    i = 0;
    int pinValue = 0;  // Set V3 status to 0 to quit, unless button is still pushed (as per below)
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    //servo_return();
    Serial.write("1");

  }
  i = 0;
  Serial.println("2");
  servo1.detach();
  servo_return();
}

void servo_return() 
{
  i = 0;
  Serial.println("x");
  delay(10000);
  Serial.println("y");
  servo2.attach(2);  // attaches the servo on GIO2 to the servo object
  int pos2;
   for (pos2 = 160; pos2 >= 90; pos2 -= 1) 
  { 
    i = 0;
    int pinValue = 0;  // Set V3 status to 0 to quit, unless button is still pushed (as per below)
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    Serial.write("2");
    i=1;
  }
  servo2.detach();
  i=1;
}

  
   




void read_input(){
  if (i==1)
  {
   Blynk.run();
  }
}

  BLYNK_WRITE(V0) 
  {
    Serial.println("b");
    //digitalWrite(5,LOW);
    int pinValue = param.asInt();
    if ((pinValue == 1 && (i==1))) {    // if Button sends 1
      i = 0;
      servo();             // start the function 
      //Blynk.run(); // Run rest of show in-between waiting for this loop to repeat or quit.
      int pinValue = 0;  // Set V3 status to 0 to quit, unless button is still pushed (as per below)
      Blynk.syncVirtual(V0); // ...Then force BLYNK_WRITE(V3) function check of button status to determine if repeating or done.
    }
  }


void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  i=1;
  Serial.println("f");

}

void loop()
{
  read_input();
}
