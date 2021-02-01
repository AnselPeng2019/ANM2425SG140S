#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "main.h"


void Initial_nRF24L01P(void);
uint8_t nRF24_SetFreq(uint16_t freq);
uint8_t nRF24_SetPower(uint8_t pwr);
void nRF24_RF_Setup(void);
void nRF24_PLL_unLock(void);
void Read_RF_Setup_Reg(void);
void nRF24_Config(void);
void Set_Freq(uint16_t freq);





#endif
