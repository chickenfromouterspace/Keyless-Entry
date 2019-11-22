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
#define WiFiint 13 //d7

char auth[] = "rXdCyr7ngI3v_RYtsVhjVLRfoVABi_qS";             // You should get Auth Token in the Blynk App.
//Justins Phone
//char ssid[] = "Justin's iPhone";                          // Your WiFi credentials.
//char pass[] = "Keylesss"; 

//Ethans Home WiFI
//char ssid[] = "MySpectrumWiFi30-2G";                          // Your WiFi credentials.
//char pass[] = "famousbird589";                                // Your WiFi credentials.

//Ethans laptop
char ssid[] = "laptop";                          // Your WiFi credentials.
char pass[] = "Superfly1";                                // Your WiFi credentials.


//char doorUnlocked[15] = "Door Unlocked";
//char doorLocked[15] = "Door Locked";
char rxChar= 0;                                               // RXcHAR holds the received command.
boolean DoorIsLocked= true;                                              // true for locked, false for unlocked
int Keypadloop, changeloop;


Servo servo1;
Servo servo2;
BlynkTimer timer;
WidgetLED led(V1);
WidgetLCD lcd(V2);

const byte numChars = 32;
char receivedChars[numChars];
char unlockdoor[numChars]= "unlockdoor";
char doorlocked[numChars]= "doorlocked";
boolean newData = false;

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    led.on();  //Turn on LED in app
    lcd.clear(); //Clear LCD widget
    lcd.print(3, 0, "Keyless"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Entry");
    pinMode(WiFiint,OUTPUT); 
    digitalWrite(WiFiint,LOW);    
    lcd_doorclosed();
}

void loop() {
    Blynk.run();  
  //  timer.run();                                               //Enable timer for autoLock functiom
    recvWithStartEndMarkers();
    showNewData();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        Serial.print("rc");
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
        update_doorState();
    }
}


void update_doorState()
{
  if(array_cmp(receivedChars, unlockdoor,32, 32) == true)
  {
    DoorIsLocked=false;
    Serial.println("TRUE1");
    lcd_dooropen();
    
  }
  
  if(array_cmp2(receivedChars, doorlocked,32, 32) == true)
    {
      DoorIsLocked=true;
      Serial.println("TRUE2");
      lcd_doorclosed();
      
    }
    if(WiFiint==HIGH)
    {
     DoorIsLocked=true;
      Serial.println("TRUE3");
      lcd_doorclosed(); 
    }
  else
  {
    Serial.println("FALSE");
    Serial.println(receivedChars);
    Serial.println(unlockdoor);
  }
}

boolean array_cmp(char *a, char *b, char len_a, char len_b){
     int n;

     // if their lengths are different, return false
     if (len_a != len_b) return false;

     // test each element to be the same. if not, return false
     for (n=0;n<len_a;n++) if (a[n]!=b[n]) return false;

     //ok, if we have not returned yet, they are equal :)
     return true;
}

boolean array_cmp2(char *a, char *b, char len_a, char len_b){
     int n;

     // if their lengths are different, return false
     if (len_a != len_b) return false;

     // test each element to be the same. if not, return false
     for (n=0;n<len_a;n++) if (a[n]!=b[n]) return false;

     //ok, if we have not returned yet, they are equal :)
     return true;
}





BLYNK_WRITE(V0)                                                  //Enters Loop when button is pressed on App
  {
    int pinValue = param.asInt();
    if ((pinValue == 1) && (DoorIsLocked==true))                       // if Button sends 1 and door is locked
    {
      DoorIsLocked=false;                                               //set door to unlocked, blocking continuous unlocking commands
      Serial.write("<unlockdoor>");
      delay(10);
      lcd_dooropen();             
    }
  }

  void lcd_dooropen()
{
   lcd.clear(); //Clear LCD widget
   lcd.print(4, 0, "Welcome"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
   lcd.print(1, 1, "Please Come In"); 
   Blynk.setProperty(LED, "color", BLYNK_GREEN);                 //change LED color
   Blynk.setProperty(LED, "label", "Door Unlocked");             //change LED label
}

void lcd_doorclosed()
{
   lcd.clear(); //Clear LCD widget
   lcd.print(3, 0, "Front Door"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
   lcd.print(5, 1, "Locked"); 
   Blynk.setProperty(LED, "color", BLYNK_RED);                   //change LED color
   Blynk.setProperty(LED, "label", "Door Locked");               //change LED label
}
