//11/20/19

//Recieves serial from nodeMCU
//Unlocks and updates BLYNK
//#define BLYNK_PRINT Serial

#include <SimpleTimer.h>
#include <Servo.h>
#include "Keypad.h"
#include "EEPROM.h"
#define LED 13                                        // Pin 13 is connected to the LED
#define ServoPin 5

char rxChar= 0;         // RXcHAR holds the received command.
int doorState; // 1 for locked, 0  for unlocked
Servo servo1;
Servo servo2;
SimpleTimer timer;


char password[4];
char initial_password[4],new_password[4];
int i=0;
int unlock_motor = 10;
int incorrect_pswd = 11;
char key_pressed=0;
const byte rows = 4; 
const byte columns = 3; 
char hexaKeys[rows][columns] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte row_pins[rows] = {A1, A6, A5, A3};
byte column_pins[columns] = {A2, A0, A4};

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

const byte numChars = 32;
char receivedChars[numChars];
char unlockdoor[numChars]= "unlockdoor";
boolean newData = false;
boolean DoorIsLocked= true;
boolean CheckSerial= true;
void Keypadsetup(void);
void recvWithStartEndMarkers(void); 
void showNewData(void); 
void update_doorState(void);
void Keypadloop(void);
void change(void);
void Unlock_Door(void);
void Auto_Lock(void) ;


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(LED, OUTPUT); // Sets pin 13 as OUTPUT.
  Keypadsetup();
}

void loop() {
    timer.run();                                               //Enable timer for autoLock functiom
    recvWithStartEndMarkers();
    showNewData();
    Keypadloop();
}

void recvWithStartEndMarkers() 
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();
        //if(CheckSerial==true)
        
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
                Serial.println("error");
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
            Serial.println("false statement");
        }
        
    }
}

void showNewData() 
{
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
        //CheckSerial=false;
        update_doorState();
    }
}

void update_doorState()
{
  if(array_cmp(receivedChars, unlockdoor,32, 32) == true)
  {
    Serial.println("TRUE");
    if(DoorIsLocked==true)
    {
    DoorIsLocked=false;
    Unlock_Door();
    }
  }

  else
  {
    Serial.println("FALSEcmp");
    Serial.println(receivedChars);
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

void Keypadsetup()
{
  pinMode(unlock_motor, OUTPUT);
  pinMode(incorrect_pswd, OUTPUT);
  Serial.println("Keyless Entry");
  initialpassword();
}

void Keypadloop()
{
  key_pressed = keypad_key.getKey();
  if(key_pressed=='#')
    change();
  if (key_pressed)
  {
    password[i++]=key_pressed;
    Serial.println(key_pressed);
      }
  if(i==4)
  {
    delay(200);
    for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
    if(!(strncmp(password, initial_password,4)))
    {
      Serial.println("Password Accepted");
      digitalWrite(unlock_motor, HIGH);
      if(DoorIsLocked==true)
      {
      Unlock_Door();
      }
      i=0;
    }
    else
    {
      Serial.println("Wrong Password");
      delay(100);

      i=0;
    }
  }
}

void change()
{
  int j=0;
  Serial.println("Current Password");
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)
    {
      new_password[j++]=key;
      Serial.println(key);
    }
    key=0;
  }
  delay(500);
  if((strncmp(new_password, initial_password, 4)))
  {
    Serial.println("Wrong Password");
    Serial.println("Try Again");
    delay(500);
  }
  else
  {
    j=0;
    Serial.println("New Password");
    while(j<4)
    {
      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        Serial.println(key);
        EEPROM.write(j,key);
        j++;
      }
    }
    Serial.println("Password Changed");
    delay(500);
  }
  Serial.println("Enter Password");
  key_pressed=0;
}

void initialpassword(){
  for(int j=0;j<4;j++)
    EEPROM.write(j, j+49);
  for(int j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);
}




void Unlock_Door() 
{
  DoorIsLocked=false;
  Serial1.write("<unlockdoor>");
  Serial.write("Unlocking Door \n");
  servo1.attach(ServoPin);                         // attaches the servo to pin 4 on Atmega
  int pos1;

  //for (pos1 = 0; pos1 <= 20; pos1 += 1) 
  for (pos1 = 180; pos1 >= 160; pos1 -= 1) 
  { 
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
  servo1.detach();
  Serial.println("Door Unlocked");
  doorState=0;
  timer.setTimeout(5000, Auto_Lock);
}

void Auto_Lock() 
{
  Serial.write("Locking Door \n");
  servo2.attach(ServoPin);  // attaches the servo on GIO2 to the servo object
  int pos2;
   
  //for (pos2 = 180; pos2 >= 160; pos2 -= 1)
  for (pos2 = 0; pos2 <= 20; pos2 += 1)  
  { 
    servo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);  
  }
           
  Serial.println("Door Locked");
  servo2.detach();
  delay(15);
  digitalWrite(LED,LOW);             // Turn Off the LED.
  Serial1.write("<doorlocked>");
  DoorIsLocked=true;
 // CheckSerial=true;

  
}
