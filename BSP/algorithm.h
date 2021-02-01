#ifndef algorithm_h
#define algorithm_h

#include "main.h"
#include "usart.h"
#include "adc.h"
#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "tim.h"

#include "rs232.h"
#include "ntc.h"
#include "getkby.h"



typedef enum
{
  Idling=0,
	Heating,
	PreIdling,
	PreHeating,
	Sweeping,
	Pausing,
	PrePausing
}WorkStateDef;
extern volatile WorkStateDef Work_State;

typedef struct
{
	int cal[2][2];
	int f1[3];
	int f2[3];
	int k;
	int b;
	int fstrt;
	int fstop;
	int pstrt;
	int pstop;
} PwrCal;


typedef struct
{
	uint16_t  temp;
	uint16_t  fwd;
	uint16_t  rev;
	uint16_t  freq;
	uint8_t   power;//used for power compensation of temperature
}ChannelStateDef;
extern volatile ChannelStateDef ChState;


extern volatile uint8_t  PowerStage;
extern volatile uint32_t HeatingTimer;
extern volatile uint32_t HTRecord;//Heating Timer Record
extern volatile uint32_t RandomTime;
extern volatile uint32_t SweepTimer;
extern volatile uint32_t TotalTime;

bool GetDebugMode(void);
void SetDebugMode(uint8_t dm);
void GetPowerCaliFlag(void);
void FanControl(void);
void ResetSweepTimer(uint8_t seconds);
void AdcSampling(void);
uint8_t SweepFrequency(int16_t start, int16_t stop, uint8_t power, uint8_t chx);
void PreHeatState(void);
void PreidleState(void);
void ReportStatus(void);
void ReportSweepResults(void);
void ReportSweepResultsInHeat(void);
void ReportTempData(void);
void JitterFrequency(uint8_t pwr);
void ChangingPhase(uint8_t chx);
void PowerCompensation(void);
void Confirm_Stop(void);
void Confirm_heating(void);
void Confirm_Pause(void);
void GetCalFactor(void);
void SetRFPower(uint16_t freq, uint8_t pwr);
void PrePauseState(void);
void MakeBand(uint8_t pwr);
void Initial_PowerCoeff(void);
void Initial_FWDCoeff(void);
void ClearPowCompFlag(void);
void SetPowCompflag(void);
void Initial_RevCoeff(void);
void SetFwdCaliflag(void);
void ClearFwdCaliFlag(void);
void SetRevCaliflag(void);
void ClearRevCaliFlag(void);
void SetPowCaliflag(void);
void ClearPowCaliFlag(void);




#endif
