#include <string.h>
#include <SoftwareSerial.h>
#include "finger.h"



uint8_t finger_RxBuf[9];			
uint8_t finger_TxBuf[9];	

uint8_t  Finger_SleepFlag = 0;

/***************************************************************************
* @brief      initialize the SoftwareSerial to communicate with Fingerprint module
****************************************************************************/
void Finger_SoftwareSerial_Init(void)
{
	Serial2.begin(19200);	
}

/***************************************************************************
* @brief      Send a byte of data to the serial port
* @param      temp : Data to send
****************************************************************************/
void  TxByte(uint8_t temp)
{
	Serial2.write(temp);
}

/***************************************************************************
* @brief      send a command, and wait for the response of module
* @param      Scnt: The number of bytes to send
	      Rcnt: expect the number of bytes response module
	      Nms: wait timeout: Delay
* @return     ACK_SUCCESS: success
  	      other: see the macro definition
****************************************************************************/
uint8_t TxAndRxCmd(uint8_t Scnt, uint8_t Rcnt, uint16_t Nms)
{
	uint8_t  i, j, CheckSum;
	uint16_t   uart_RxCount = 0;
	unsigned long  time_before = 0;
	unsigned long  time_after = 0;
	uint8_t   overflow_Flag = 0;;	
	
	 TxByte(CMD_HEAD);		 
	 CheckSum = 0;
	 for (i = 0; i < Scnt; i++)
	 {
		TxByte(finger_TxBuf[i]);		 
		CheckSum ^= finger_TxBuf[i];
	 }	
	 TxByte(CheckSum);
	 TxByte(CMD_TAIL);  
	 
	 memset(finger_RxBuf,0,sizeof(finger_RxBuf));   ////////
 
	 serialFlush();  /////
	 
	 // Receive time out: Nms
	time_before = millis();	 
	 do
	 {
		overflow_Flag = 0;
		if(Serial2.available())
		{
			finger_RxBuf[uart_RxCount++] = Serial2.read();
		}
		time_after = millis();	
		if(time_before > time_after)   //if overflow (go back to zero)
		{
			time_before = millis();	  // get time_before again
			overflow_Flag = 1;
		}
		
	 } while (((uart_RxCount < Rcnt) && (time_after - time_before < Nms)) || (overflow_Flag == 1));
	  	  
	 if (uart_RxCount != Rcnt)return ACK_TIMEOUT;
	 if (finger_RxBuf[0] != CMD_HEAD) return ACK_FAIL;
	 if (finger_RxBuf[Rcnt - 1] != CMD_TAIL) return ACK_FAIL;
	 if (finger_RxBuf[1] != (finger_TxBuf[0])) return ACK_FAIL;	 
	 CheckSum = 0;
	 for (j = 1; j < uart_RxCount - 1; j++) CheckSum ^= finger_RxBuf[j];
	 if (CheckSum != 0) return ACK_FAIL; 	  
 	 return ACK_SUCCESS;
}	 

/***************************************************************************
* @brief      Query the number of existing fingerprints
* @return     0xFF: error
  	      other: success, the value is the number of existing fingerprints
****************************************************************************/
uint8_t GetUserCount(void)
{
  uint8_t m;
	
	finger_TxBuf[0] = CMD_USER_CNT;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
			
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

/***************************************************************************
* @brief      Get Compare Level
* @return     0xFF: error
  	      other: success, the value is compare level
****************************************************************************/
uint8_t GetcompareLevel(void)
{
	uint8_t m;
	
	finger_TxBuf[0] = CMD_COM_LEV;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 1;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
		
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

/***************************************************************************
* @brief      Set Compare Level
* @param      temp: Compare Level,the default value is 5, can be set to 0-9, the bigger, the stricter
* @return     0xFF: error
  	      other: success, the value is compare level
****************************************************************************/
uint8_t SetcompareLevel(uint8_t temp)
{
	uint8_t m;
	
	finger_TxBuf[0] = CMD_COM_LEV;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = temp;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);

	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

/***************************************************************************
* @brief      Get the time that fingerprint collection wait timeout 
* @return     0xFF: error
  	      other: success, the value is the time that fingerprint collection wait timeout 
****************************************************************************/
uint8_t GetTimeOut(void)
{
	uint8_t m;
	
	finger_TxBuf[0] = CMD_TIMEOUT;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 1;
	finger_TxBuf[4] = 0;	
	
	m = TxAndRxCmd(5, 8, 200);
		
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
	    return finger_RxBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

/***************************************************************************
* @brief      Register fingerprint
* @return     ACK_SUCCESS: success
  	      other: see the macro definition
****************************************************************************/
uint8_t AddUser(void)
{
	uint8_t m;
	
	m = GetUserCount();
	if (m >= USER_MAX_CNT)
		return ACK_FULL;


	finger_TxBuf[0] = CMD_ADD_1;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = m +1;
	finger_TxBuf[3] = 3;
	finger_TxBuf[4] = 0;		
	m = TxAndRxCmd(5, 8, 6000);	
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{
		finger_TxBuf[0] = CMD_ADD_3;
		m = TxAndRxCmd(5, 8, 6000);
		if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
		{
			return ACK_SUCCESS;
		}
		else
		return ACK_FAIL;
	}
	else
		return ACK_FAIL;
}

/***************************************************************************
* @brief      Clear fingerprints
* @return     ACK_SUCCESS:  success
  	      ACK_FAIL:     error
****************************************************************************/
uint8_t  ClearAllUser(void)
{
 	uint8_t m;
	
	finger_TxBuf[0] = CMD_DEL_ALL;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;
	
	m = TxAndRxCmd(5, 8, 500);
	
	if (m == ACK_SUCCESS && finger_RxBuf[4] == ACK_SUCCESS)
	{	    
		return ACK_SUCCESS;
	}
	else
	{
		return ACK_FAIL;
	}
}

/***************************************************************************
* @brief      Check if user ID is between 1 and 3
* @return     TRUE
  	      FALSE
****************************************************************************/
uint8_t IsMasterUser(uint8_t UserID)
{
    if ((UserID == 1) || (UserID == 2) || (UserID == 3)) return TRUE;
			else  return FALSE;
}	 

/***************************************************************************
* @brief      Fingerprint matching
* @return     ACK_SUCCESS: success
  	      other: see the macro definition
****************************************************************************/
uint8_t VerifyUser(void)
{
	uint8_t m;
	
	finger_TxBuf[0] = CMD_MATCH;
	finger_TxBuf[1] = 0;
	finger_TxBuf[2] = 0;
	finger_TxBuf[3] = 0;
	finger_TxBuf[4] = 0;
	
	m = TxAndRxCmd(5, 8, 5000);
		
	if ((m == ACK_SUCCESS) && (IsMasterUser(finger_RxBuf[4]) == TRUE))
	{	
		 return ACK_SUCCESS;
	}
	else if(finger_RxBuf[4] == ACK_NO_USER)
	{
		return ACK_NO_USER;
	}
	else if(finger_RxBuf[4] == ACK_TIMEOUT)
	{
		return ACK_TIMEOUT;
	}
	else
	{
		return ACK_GO_OUT;   // The center of the fingerprint is out of alignment with sensor
	}
}

/***************************************************************************
* @brief      Wait until the fingerprint module works properly
****************************************************************************/
void Finger_Wait_Until_OK(void)
{		
    digitalWrite(Finger_RST_Pin , LOW);
	delay(300); 
    digitalWrite(Finger_RST_Pin , HIGH);
	delay(300);  // Wait for module to start
    
	 // ERROR: Please ensure that the module power supply is 3.3V or 5V, 
	 // the serial line is correct, the baud rate defaults to 19200,
	 // and finally the power is switched off, and then power on again !
	while(SetcompareLevel(5) != 5)
	{		
		delay(1000); 
		Serial.println("***ERROR***");
	}

	Serial.write("*************** WaveShare Capacitive Fingerprint Reader Test ***************\r\n");
	Serial.write("Compare Level:  5    (can be set to 0-9, the bigger, the stricter)\r\n"); 
	Serial.write("Number of fingerprints already available:  ");  Serial.print(GetUserCount());
	Serial.write("\r\n Use the serial port to send the commands to operate the module:\r\n"); 
	Serial.write(" CMD1 : Query the number of existing fingerprints\r\n"); 
	Serial.write(" CMD2 : Add fingerprint  (Each entry needs to be read two times: \"beep\", "); Serial.write("put the finger on sensor, \"beep\", put up, \"beep\", put on again)\r\n"); 
	Serial.write(" CMD3 : Fingerprint matching  (Send the command, put your finger on sensor after \"beep\". "); Serial.write("Each time you send a command, module waits and matches once)\r\n"); 
	Serial.write(" CMD4 : Clear fingerprints\r\n"); 
	Serial.write(" CMD5 : Switch to sleep mode, you can use the finger Automatic wake-up function"); 
	Serial.write(" CMD6 : Wake up and make all commands valid\r\n");
	Serial.write("*************** WaveShare Capacitive Fingerprint Reader Test ***************\r\n");	
}

/***************************************************************************
* @brief      Analysis of the serial port 2 command (computer serial assistant)
****************************************************************************/
void Analysis_PC_Command(void)
{	
	static  uint8_t  step;
	uint8_t temp;
		
	if(Serial.available())
	{
		temp = Serial.read();
		
		switch(step)
		{
			case 0:
				if(temp == 'C')     step++;		
				else    step = 0;		
				break;		
			case 1:
				if(temp == 'M')     step++;		
				else    step = 0;
				break;				
			case 2:
				if(temp == 'D')     step++;		
				else    step = 0;
				break;				
			case 3:		
				switch(temp)
				{						
					case '1':
						if(Finger_SleepFlag == 1)  break;
						Serial.write("Number of fingerprints already available:  "); Serial.println(GetUserCount());
						break;			
					case '2':
						if(Finger_SleepFlag == 1)  break;
                        Serial.write(" Add fingerprint  (Each entry needs to be read two times: \"beep\", "); Serial.write("put the finger on sensor, \"beep\", put up, \"beep\", put on again)\r\n"); 
						switch(AddUser())
						{
							case ACK_SUCCESS:
								Serial.println("Fingerprint added successfully !");
								break;
							
							case ACK_FAIL: 			
								Serial.println("Failed: Please try to place the center of the fingerprint flat to sensor, or this fingerprint already exists !");
								break;
							
							case ACK_FULL:			
								Serial.println("Failed: The fingerprint library is full !");
								break;		
						}
						break;					
					case '3':
						if(Finger_SleepFlag == 1)  break;
						Serial.println("Waiting Finger......Please try to place the center of the fingerprint flat to sensor !");
						switch(VerifyUser())
						{
							case ACK_SUCCESS:	
								Serial.println("Matching successful !");
								break;
							case ACK_NO_USER:
								Serial.println("Failed: This fingerprint was not found in the library !");
								break;
							case ACK_TIMEOUT:	
								Serial.println("Failed: Time out !");
								break;	
							case ACK_GO_OUT:
								Serial.println("Failed: Please try to place the center of the fingerprint flat to sensor !");
								break;
						}
						break;				
					case '4':
						if(Finger_SleepFlag == 1)  break;
						ClearAllUser();
						Serial.println("All fingerprints have been cleared !");
						break;				
					case '5':
						if(Finger_SleepFlag == 1)  break;
						digitalWrite(Finger_RST_Pin , LOW);
						Finger_SleepFlag = 1;
						Serial.println("Module has entered sleep mode: you can use the finger Automatic wake-up function, in this mode, only CMD6 is valid, send CMD6 to pull up the RST pin of module, so that the module exits sleep !");	
						break;
					case '6':					
						digitalWrite(Finger_RST_Pin , HIGH);
						delay(300);  // Wait for module to start				
						Finger_SleepFlag = 0;							
						Serial.println("The module is awake. All commands are valid !");	
						break;
					
					default: break;
				}
							
				step = 0;				
				break;
				
			default:   break;	
		}
	}
}

/***************************************************************************
* @brief  
     If you enter the sleep mode, then open the Automatic wake-up function of the finger,
     begin to check if the finger is pressed, and then start the module and match
****************************************************************************/
void Auto_Verify_Finger(void)
{
	if(digitalRead(Finger_WAKE_Pin) == HIGH)   // If you press your finger
	{	
		delay(20);			
		if(digitalRead(Finger_WAKE_Pin) == HIGH)   
		{
			digitalWrite(Finger_RST_Pin , HIGH);    // Pull up the RST to start the module and start matching the fingers
			delay(300);	 // Wait for module to start
					
			Serial.println("Waiting Finger......Please try to place the center of the fingerprint flat to sensor !");
			switch(VerifyUser())
			{
				case ACK_SUCCESS:	
					Serial.println("Matching successful !");
					break;
				case ACK_NO_USER:
					Serial.println("Failed: This fingerprint was not found in the library !");
					break;
				case ACK_TIMEOUT:	
					Serial.println("Failed: Time out !");
					break;	
				case ACK_GO_OUT:
					Serial.println("Failed: Please try to place the center of the fingerprint flat to sensor !");
					break;
			}
			
			//After the matching action is completed, drag RST down to sleep
			//and continue to wait for your fingers to press
			digitalWrite(Finger_RST_Pin , LOW);    
		}
	}
}

void serialFlush(){
  while(Serial2.available() > 0) {
    char t = Serial2.read();
  }
}   