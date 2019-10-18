#ifndef _FINGER_H
#define _FINGER_H


#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#include <stdio.h>

#define TRUE  1
#define FALSE 0

#define Finger_RST_Pin 24
#define Finger_WAKE_Pin 25

// Basic response message definition
#define ACK_SUCCESS       0x00
#define ACK_FAIL          0x01
#define ACK_FULL	  0x04
#define ACK_NO_USER	  0x05
#define ACK_TIMEOUT       0x08
#define ACK_GO_OUT	  0x0F		// The center of the fingerprint is out of alignment with sensor

//User information definition
#define ACK_ALL_USER       	0x00
#define ACK_GUEST_USER 	  	0x01
#define ACK_NORMAL_USER 	0x02
#define ACK_MASTER_USER    	0x03

#define USER_MAX_CNT	   	1000	// Maximum fingerprint number

// Command definition
#define CMD_HEAD	 0xF5
#define CMD_TAIL	 0xF5
#define CMD_ADD_1        0x01
#define CMD_ADD_2        0x02
#define CMD_ADD_3	 0x03
#define CMD_MATCH	 0x0C
#define CMD_DEL		 0x04
#define CMD_DEL_ALL	 0x05
#define CMD_USER_CNT     0x09
#define CMD_COM_LEV	 0x28
#define CMD_LP_MODE      0x2C
#define CMD_TIMEOUT      0x2E

#define CMD_FINGER_DETECTED   0x14


extern  uint8_t  Finger_SleepFlag;


void Finger_SoftwareSerial_Init(void);
void  TxByte(uint8_t temp);
uint8_t GetUserCount(void);
uint8_t GetcompareLevel(void);
uint8_t SetcompareLevel(uint8_t temp);	// The default value is 5, can be set to 0-9, the bigger, the stricter
uint8_t AddUser(void);
uint8_t ClearAllUser(void);
uint8_t VerifyUser(void);
uint8_t GetTimeOut(void);
void Finger_Wait_Until_OK(void);	
void Analysis_PC_Command(void);
void Auto_Verify_Finger(void);
void serialFlush(void);

#endif


