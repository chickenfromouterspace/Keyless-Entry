#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#define BLYNK_PRINT Serial
#define LAD D2
#define LED V1
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

char auth[] = "rXdCyr7ngI3v_RYtsVhjVLRfoVABi_qS";             // You should get Auth Token in the Blynk App.

//Ethans laptop
char ssid[] = "laptop";                          // Your WiFi credentials.
char pass[] = "Superfly1";                                // Your WiFi credentials.
boolean DoorIsLocked= true; 
int i;

WidgetLED led(V1);
WidgetLCD lcd(V2);

SimpleTimer timer;

void setup() {
  // put your setup code here, to run once:
  pinMode(LAD, INPUT_PULLUP);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  i=1;
  lcd_doorclosed();
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.run();
  Blynk.run();
  delay(100);
  if(digitalRead(LAD) == LOW)
    {
        Serial.println("On0."); 
        timer.setTimeout(50, checkon);  
    }
    
  else
      {
       Serial.println("Off0."); 
        timer.setTimeout(50, checkoff); 
      }
    
}

BLYNK_WRITE(V0)                                                  //Enters Loop when button is pressed on App
  {
    int pinValue = param.asInt();
    if ((pinValue == 1)&& (DoorIsLocked==true))                      // if Button sends 1 and door is locked
    {
                                        //set door to unlocked, blocking continuous unlocking commands
      Serial.write("<unlockdoor>");
      delay(10);          
    }
  }


void lcd_dooropen()
{
   DoorIsLocked=false; 
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
   DoorIsLocked=true; 
}

void checkoff()
{
  Serial.println("Off."); 
  if(digitalRead(LAD) == HIGH)
  {
    Serial.println("Off2"); 
    if(i==0) //if door is previosly unlocked
    {
    lcd_doorclosed();
    i=1;
    }
  }
}

void checkon()
{
  Serial.println("On.");
  if(digitalRead(LAD) == LOW)
  {
    Serial.println("On2");
    if(i==1) //if door is initially locked
    {
      i=0;
      lcd_dooropen();
    }

  }
}
