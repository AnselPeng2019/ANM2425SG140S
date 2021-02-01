/************************************************************************
Master board in 2 channels system
File name: RS485.h
RS485 protocol, using RTU N81
19200bps, 1 start bit, 8 data bit, none parity, 1 stop
Frame definition:
0,1 Sync,         2bytes,0x1616
2 Source ID,      1byte, 0x01 for master, 0x02 for slave
3 Destination ID, 1byte, ...
4 Command,        1byte, details are shown below
5 LEN,            1byte, length of SUBDATA
6 SUB_DATA,       6bytes(MAX), application data
7 CHECK_SUM,      1byte, calculate value 
8 END,            1byte, 0xF5
************************************************************************/

#ifndef _RS485_h
#define _RS485_h

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"
#include "dac.h"

#include "algorithm.h"
#include "AT24C04.h"

extern volatile uint8_t SwpCH;
extern volatile uint8_t SweepStart;
extern volatile uint8_t SweepStop;
extern volatile uint8_t SweepPower;
extern volatile int8_t  TempData[64];

/***************************GetRequestCMD*******************************/
void ResetUART(void);
uint8_t CheckSum(uint8_t *pdata, uint8_t num);

/*************************GetRequestCMD END*****************************/





#endif

