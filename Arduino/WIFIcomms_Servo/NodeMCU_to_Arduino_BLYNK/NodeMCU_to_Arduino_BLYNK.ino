// Code to send and recieve information from Atmega through serial communication
// Send Serial to Atmega if wifi App sends unlock
// Receive Serial from Atmega to update the state of the lock on the wifi App

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#define LED V1
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

char auth[] = "rXdCyr7ngI3v_RYtsVhjVLRfoVABi_qS";             // You should get Auth Token in the Blynk App.
char ssid[] = "MySpectrumWiFi30-2G";                          // Your WiFi credentials.
char pass[] = "famousbird589";                                // Your WiFi credentials.
char doorUnlocked[15] = "Door Unlocked";
char doorLocked[15] = "Door Locked";
char rxChar= 0;                                               // RXcHAR holds the received command.
int doorState=1;                                              // 1 for locked, 0  for unlocked

Servo servo1;
Servo servo2;
BlynkTimer timer;
WidgetLED led(V1);



void loop()
{
  Blynk.run();  
  timer.run();                                               //Enable timer for autoLock functiom
  checklock_state();                                         //Checks Current State of Lock, updates LED
}


void setup()
{
  Serial.begin(115200);
  while (!Serial)                                            // wait for serial port to connect. Needed for native USB port only
 {
    ; 
 }
 
 Blynk.begin(auth, ssid, pass);
 led.on();                                                    //Turn on LED in app
  
}

void checklock_state()                                        // function reads transmitted data from Arduino
{
  

   if (Serial.available() >0)
  {          // Check receive buffer.
    rxChar = Serial.read();                                   // Save character received. 
    Serial.flush();                                           // Clear receive buffer.
  
    switch (rxChar) 
    {
      
      case '0':                       
        if (digitalRead(LED) == LOW)                             // If LED is Off:
        {                           
                digitalWrite(LED,HIGH);                          // Turn On the LED.
                doorState=0;                                     // Update Local doorState Variable
                Serial.print("Welcome");
                
        }
        else Serial.println("Door already Unlocked!");
                break;
  
      case '1':
      
        if (digitalRead(LED) == HIGH)                             // If LED is On:
        {                            
                digitalWrite(LED,LOW);                            // Turn Off the LED.
                doorState=1;                                      // Update Local doorState Variable  
                //Serial.println("Door Locked");
        }
        else Serial.println("Door already Locked!");
                doorState=1;
                break;
   
      
    }
  }

  if(doorState==1)
  {
    Blynk.setProperty(LED, "color", BLYNK_RED);                   //change LED color
    Blynk.setProperty(LED, "label", "Door Locked");               //change LED label
  
    //doorState=1;
  }
  if(doorState==0)
  {
    Blynk.setProperty(LED, "color", BLYNK_GREEN);                 //change LED color
    Blynk.setProperty(LED, "label", "Door Unlocked");             //change LED label
    //.WidgetLED led(V1);
  }
}



BLYNK_WRITE(V0)                                                  //Enters Loop when button is pressed on App
  {
    int pinValue = param.asInt();
    if ((pinValue == 1) && (doorState==1))                       // if Button sends 1 and door is locked
    {
      doorState=0;                                               //set door to unlocked, blocking continuous unlocking commands
      Serial.write("0");
      delay(50);
      Serial.write("\n");          
    }
  }
