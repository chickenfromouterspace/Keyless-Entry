#include <SoftwareSerial.h>
#include "finger.h"


void setup() 
{
	// Open serial communications (connect to PC) and wait for port to open:
	Serial.begin(115200);
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for native USB port only
	}
	
	pinMode(Finger_RST_Pin  , OUTPUT);
  digitalWrite(Finger_RST_Pin , LOW);
  delay(100);
	digitalWrite(Finger_RST_Pin , HIGH);
	pinMode(Finger_WAKE_Pin , INPUT);
	Finger_SoftwareSerial_Init();	
	Finger_Wait_Until_OK();
}


void loop()
{ 	
	Analysis_PC_Command();
	// If in sleep mode, turn on the auto wake-up function of the finger, 
	//begin to check if the finger is pressed, and wake up the module and match it
	if(Finger_SleepFlag == 1)
	{
		Auto_Verify_Finger();
	}	
}
