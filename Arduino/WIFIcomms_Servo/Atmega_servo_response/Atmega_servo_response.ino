
//Recieves serial from nodeMCU
//Unlocks and updates BLYNK
//#define BLYNK_PRINT Serial

#include <SimpleTimer.h>
#include <Servo.h>
#include <millisDelay.h>

#define LED V1
#define LED 13                                        // Pin 13 is connected to the LED
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

char doorUnlocked[15] = "Door Unlocked";
char doorLocked[15] = "Door Locked";
char rxChar= 0;         // RXcHAR holds the received command.
int doorState; // 1 for locked, 0  for unlocked

Servo servo1;
Servo servo2;
SimpleTimer timer;
millisDelay srvoDelay;

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  pinMode(LED, OUTPUT); // Sets pin 13 as OUTPUT.
}


void loop(){
  timer.run();
  //if (srvoDelay.justFinished())           //Check if AutoLock Timer Finished
  //{
  //  Auto_Lock();                          //Execute AutoLock  
  //}
  
  if (Serial3.available() >0)           // Check receive buffer.
  {          
    rxChar = Serial3.read();            // Save character received. 
    Serial3.flush();                    // Clear receive buffer.
  
  switch (rxChar) 
  {
    
    case '0':                               // If recieved door Unlocked
      if (digitalRead(LED) == LOW){        // If LED is Off:
              digitalWrite(LED,HIGH);      // Turn On the LED.
              Unlock_Door();               
              doorState=0;
              Serial.print("Welcome");
              
      }
            else Serial.println("Door already Unlocked!");
            break;

    case '1':
    
      if (digitalRead(LED) == HIGH){       // If LED is On:
              digitalWrite(LED,LOW);       // Turn Off the LED.
              doorState=1;
              //Serial.println("Door Locked");
      }
            else Serial.println("Door already Locked!");
            break;
 
   
  }
  }
 
}  

void Unlock_Door() 
{
  
  Serial.write("Unlocking Door \n");
  servo1.attach(4);                         // attaches the servo to pin 4 on Atmega
  int pos1;

  for (pos1 = 0; pos1 <= 20; pos1 += 1) 
  { 
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    Serial.println(pos1);
  }
  
  servo1.detach();
  Serial.println("Door Unlocked");
  Serial3.write("0");
  delay(15);
  Serial3.println("");
  doorState=0;
  timer.setTimeout(5000, Auto_Lock);
  //timer.setInterval(1000, repeatMe)
  //srvoDelay.start(1000); //start 5sec delay
  
}

void Auto_Lock() 
{
  //servo1.detach();
  //servo2.detach();
  Serial.write("Locking Door \n");
  servo2.attach(4);  // attaches the servo on GIO2 to the servo object
  int pos2;
   
  for (pos2 = 180; pos2 >= 160; pos2 -= 1) 
  { 
   
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);  
    Serial.println(pos2);
    // waits 15ms for the servo to reach the position
  }
  //doorState=1;            
  Serial.println("Door Locked");
  servo2.detach();
  Serial3.write("1");
  delay(15);
  digitalWrite(LED,LOW);             // Turn Off the LED.
  
}



// End of the Sketch.
