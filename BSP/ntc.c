/*
****************************************************************
using  NCU18XH103J60RB to detect temperature.
R25 = 10kohm
pullup resistor is 1Kohm.
****************************************************************
*/
#include "ntc.h"

int32_t B25XX = 3380; //B2550=3380;B2585=3434

#define NTCREF3V3
//#define NTCREF2V9

#ifdef NTCREF2V9
static int32_t kf[5] = { -165,-98,-68,-57,-55 };
static int32_t bf[5] = {  460,281,209,184,181 };
/***************20C  40C  60C  80C  100C  **********/
static int Tmark[5] = { 2679,2474,2182,1830,1467 };//5 temp points for adc
#endif

#ifdef NTCREF3V3
static int32_t kf[5] = { -145,-85,-60,-50,-48 };
static int32_t bf[5] = { 462,281,210,184,181 };
/***************20C  40C  60C  80C  100C  **********/
static int Tmark[5] = { 3049,2815,2493,2083,1670 };//5 temp points for adc
#endif


/**
  * @brief  Get temperature function.
  * @retval vtemp, the ntc resistor voltage
  */
int16_t GetTemp(uint16_t vtemp)
{
	int32_t temp = 0;
	if (vtemp > Tmark[0]) {
		temp = kf[0] * vtemp + bf[0]*1000;
	}
	else if (vtemp > Tmark[1]) {
		temp = kf[1] * vtemp + bf[1]*1000;
	}
	else if (vtemp > Tmark[2]) {
		temp = kf[2] * vtemp + bf[2]*1000;
	}
	else if (vtemp > Tmark[3]) {
		temp = kf[3] * vtemp + bf[3]*1000;
	}
	else {
		temp = kf[4] * vtemp + bf[4]*1000;
	}
	temp /= 1000;

	

	return (int16_t)temp;

}



