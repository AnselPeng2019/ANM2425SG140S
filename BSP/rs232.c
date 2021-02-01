#include "rs232.h"
#include "nrf24l01.h"


uint8_t msg_temp[300] = {0};
uint8_t msg_status[30] = {0};
volatile int8_t  TempData[64] = {0};
volatile uint8_t SwpCH      = 0;
volatile uint8_t SweepStart = 0;
volatile uint8_t SweepStop = 100;
volatile uint8_t SweepPower  = 7;

uint8_t handshk[9]  = {0x16,0x16,0x02,0x01,0x1B,0x01,0xB0,0xFB,0xF5};
uint8_t version[13] = {0x16,0x16,0x02,0x01,0x1C,0x05,0x20,0x19,0x05,0x30,0x01,0x00,0xF5};//2019053001 ver

extern volatile bool fwdpower_flag;

void EmptyBuff(uint8_t *buff, uint8_t id)
{
  int k;
	id = 14;
  for(k=0;k<id;k++)
		buff[k] = 0;
}

uint8_t CheckSum(uint8_t *pdata, uint8_t num)
{
  uint8_t i;
	uint8_t temp = 0;
	for(i=0;i<num;i++)
		temp += pdata[i];
	return temp;
}

void Confirm_RT(uint8_t *rt)//report runtime to pc
{
  uint8_t rt_data[11] = {0x16,0x16,0x02,0x01,0x1d,0x03,0x01,0x00,0x00,0x00,0xf5};
	rt_data[7] = rt[0];
	rt_data[8] = rt[1];
	rt_data[9] = CheckSum(rt_data,9);
	
	HAL_UART_Transmit(&huart1,rt_data,11,100);
}

void Confirm_SetRFPower()
{
  //                 b0   b1   b2   b3   b4   b5   b6   b7   b8   b9
  uint8_t cmd[10] = {0x16,0x16,0x02,0x01,0x20,0x02,0x00,0x00,0x00,0xf5};
	cmd[8] = CheckSum(cmd, 8);

	HAL_UART_Transmit(&huart1,cmd,10,100);
}

//将校准系数读出发送给上位机比对
void SendCalData(uint8_t *data)
{
  uint8_t cmd[26] = {0};
	
	cmd[0] = 0x16;
	cmd[1] = 0x16;
	cmd[2] = 0x02;
	cmd[3] = 0x01;
	cmd[4] = 0x1e;//cmd
	cmd[5] = 0x12;//len
	cmd[6] = 0xb0;//cmd
	cmd[7] = 0x00;//len
	for(int i=0;i<16;i++){
		cmd[8+i] = data[i];
		}
	cmd[24] = CheckSum(cmd, 24);
	cmd[25] = 0xf5;
	
	HAL_UART_Transmit(&huart1,cmd,26,100);
}

/***************************GetRequestCMD*******************************/
void UART1_process(unsigned char *buff, unsigned char id)
{
  uint8_t CheckByte = 0;
	uint8_t len = buff[5];
	uint8_t i;
	for(i=0;i<len+6;i++)
		CheckByte += buff[i];
  if(CheckByte == buff[len+6])
  	{
			if(buff[4] == STA_REQ_FLAG)//status request
			{
				ReportStatus();
			}
			else if(buff[4] == HEAT_CMD_FLAG)//control set
			{
				if((buff[6] == 0x0A)&&(Work_State == Idling)){//get in heating state
  				PowerStage		 = buff[7];
  				RandomTime		 = buff[9] * 60;
  				HTRecord			 = buff[8] * 60 + RandomTime + 0;
					Work_State     = PreHeating;
					}
				else if((buff[6] == 0x0B)&&(Work_State == Heating)){//get in idling state
					Work_State     = PreIdling;
					}
				else if((buff[6] == 0x0C)&&(Work_State == Heating)){//get in pausing state
					Work_State     = PrePausing;
					}
				else if((buff[6] == 0x0A)&&(Work_State == Pausing)){//get in heating state
					Work_State     = PreHeating;
					}
				else if((buff[6] == 0x0B)&&(Work_State == Pausing)){//get in idling state
					Work_State     = PreIdling;
					}
			}
			else if(buff[4] == ADJ_VGS_FLAG)//Adjust VC for ATT
			{
			  if(buff[6] == 0x00)
					HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,((buff[7]<<8)|buff[8])*4095/3300);//set vc1
			}
			else if(buff[4] == HANDSHAKE)//Handshake
			{
			  handshk[7]=CheckSum(handshk,7);
				HAL_UART_Transmit(&huart1,handshk,9,100);
			}
			else if(buff[4] == GETVERSION)//get firmware version
			{
				if(buff[5] == 0x01){//query the version
					AT24C04_ReadBytes(&hi2c1,&version[6],0,0,5);
					version[11]=CheckSum(version,11);
					HAL_UART_Transmit(&huart1,version,13,100);
					}
				else if(buff[5] == 0x05){//update the version in eeprom at24c04
					AT24C04_WriteBytes(&hi2c1,&buff[6],0,0,5);
					}
			}
			else if(buff[4] == RUNTIME)//rumtime query and modify
			{
				uint8_t rt[2] = {0};
				if(buff[6] == 0x01){//query valid runtime
					AT24C04_ReadBytes(&hi2c1,rt,0,14,2);
					Confirm_RT(rt);
					}
				else if(buff[6] == 0x10){//modify the valid runtime data
					rt[0]=buff[7];
					rt[1]=buff[8];
					AT24C04_WriteBytes(&hi2c1,rt,0,14,2);
					}
			}
			else if(buff[4] == CALIBRATION){//power calibration
				if(buff[6] == 0x0a){//write calibration data to eeprom
				  AT24C04_WriteBytes(&hi2c1, &buff[8], 1, 14, 2);
					SetPowCaliflag();
					SetFwdCaliflag();
					osDelay(10);
				  AT24C04_WriteBytes(&hi2c1, &buff[10], 2, 0, 16);
					osDelay(10);
				  AT24C04_WriteBytes(&hi2c1, &buff[26], 3, 0, 16);
					osDelay(10);
					Initial_PowerCoeff();
					osDelay(10);
					Initial_FWDCoeff();
					}
				else if(buff[6] == 0x0b){//read calibration data from eeprom
					uint8_t cal_data[16] ={0};
					AT24C04_ReadBytes(&hi2c1, cal_data, 2, 0, 16);
				  SendCalData(cal_data);
					}
				else if(buff[6] == 0x0c){//正向检测校准标志开关，方便校准时操作验证
					if(buff[7] == 0x0a)
						ClearPowCompFlag();
					else
						SetPowCompflag();
					}
				else if(buff[6] == 0x0d){//写反射功率校准数据
				  AT24C04_WriteBytes(&hi2c1, &buff[8], 1, 13, 1);
					SetRevCaliflag();
					osDelay(10);
				  AT24C04_WriteBytes(&hi2c1, &buff[10], 4, 0, 16);
					osDelay(10);
					Initial_RevCoeff();
					}
				else if(buff[6] == 0x0e){//校准复位，模块回到未校准状态
					uint8_t temp[3] = {0xff,0xff,0xff};
				  AT24C04_WriteBytes(&hi2c1, temp, 1, 13, 3);
					ClearFwdCaliFlag();
					ClearPowCaliFlag();
					ClearRevCaliFlag();
					ClearPowCompFlag();
					}
					
				}
			else if(buff[4] == DEBUGMODE_FLAG){//DEBUG MODE SET 
			  SetDebugMode(buff[6]);
				}
			else if(buff[4] == RFPOWER_FLAG){//set rf power function 
			  SetRFPower(ChState.freq, buff[6]);
				ChState.power = buff[6];
				Confirm_SetRFPower();
				}
			else if(buff[4] == ADJ_FREQ_FALG){//set rf power function 
				Set_Freq(2400 + buff[7]);
				ChState.freq = 2400 + buff[7];
				Confirm_SetRFPower();
				}
	  	}
	else
		{
		}
	
	EmptyBuff(buff,id);
}

/*************************GetRequestCMD END*****************************/

void ResetUART1()
{
  if(recv_end_flag232 == true)
  	{
  	  UART1_process(rx_buffer232,rx_len232);
			recv_end_flag232 = CLEAR;

			__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
			HAL_UART_Receive_DMA(&huart1,(uint8_t *)rx_buffer232,BUFFER_SIZE);
  	}
}

void ResetUART()
{
  ResetUART1();
}




