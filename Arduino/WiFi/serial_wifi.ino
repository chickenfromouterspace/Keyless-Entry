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
char auth[] = "iEoxTGsRaI8JtyT-tdQR2smeAl8rJ3VT";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Robotics";
char pass[] = "auvsiauvsi";

void servo() {
  Serial.write("1");
}

BLYNK_WRITE(V0) 
{
  //digitalWrite(5,LOW);
  int pinValue = param.asInt();
  if (pinValue == 1) {    // if Button sends 1
    servo();             // start the function 
    Blynk.run(); // Run rest of show in-between waiting for this loop to repeat or quit.
    int pinValue = 0;  // Set V3 status to 0 to quit, unless button is still pushed (as per below)
    Blynk.syncVirtual(V0); // ...Then force BLYNK_WRITE(V3) function check of button status to determine if repeating or done.
  }
}



void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

}

void loop()
{
  Blynk.run();
  //digitalWrite(2,LOW);
}
