
#include "nrf24l01.h"
#include "spi.h"
#include "gpio.h"

typedef struct {
uint16_t addr 				: 8;
uint16_t prim_rx      : 1;
uint16_t pwr_up       : 1;
uint16_t crco         : 1;
uint16_t en_crc       : 1;
uint16_t mask_max_rt  : 1;
uint16_t mask_tx_ds   : 1;
uint16_t mask_rx_dr   : 1;
uint16_t reserved     : 1;
}CONFIG_t;

static CONFIG_t configure = {
	.addr = 0x00,	
	.reserved = 0,   .mask_rx_dr = 1, 
	.mask_tx_ds = 1, .mask_max_rt = 1,
	.en_crc = 1, .crco = 0, 
	.pwr_up = 1, .prim_rx = 0,
};

/* enhanced shock Burst
 */
typedef struct{
	uint16_t addr      :8;
	uint16_t enaa_p0   :1;
	uint16_t enaa_p1   :1;
	uint16_t enaa_p2   :1;
	uint16_t enaa_p3   :1;
	uint16_t enaa_p4   :1;
	uint16_t enaa_p5   :1;
	uint16_t reserved  :2;
}ENAA_t;

static ENAA_t enaa_reg = {
	.addr = 0x01,
	.enaa_p0 = 0, .enaa_p1 = 0, .enaa_p2 = 0,
	.enaa_p3 = 0, .enaa_p4 = 0, .enaa_p5 = 0,
	.reserved = 0,
	};
	
typedef struct{
		uint16_t addr 		 :8;
		uint16_t erx_p0 	 :1;
		uint16_t erx_p1 	 :1;
		uint16_t erx_p2 	 :1;
		uint16_t erx_p3 	 :1;
		uint16_t erx_p4 	 :1;
		uint16_t erx_p5 	 :1;
		uint16_t reserved  :2;
}EN_RXADDR_t;
	
static EN_RXADDR_t en_rxaddr_reg = {
	.addr = 0x02,
	.erx_p0 = 0, .erx_p1 = 0, .erx_p2 = 0,
	.erx_p3 = 0, .erx_p4 = 0, .erx_p5 = 0,
	.reserved = 0,
};

typedef struct{
  uint16_t addr       :8;
	uint16_t aw         :2;
	uint16_t reserved   :6;	
}SETUP_AW_t;

static SETUP_AW_t setup_aw_reg ={
  .addr = 0x03,
	.aw = 0,
	.reserved = 0,
};

typedef struct{
  uint16_t addr       :8;
	uint16_t arc        :4;
	uint16_t ard        :4;
}SETUP_RETR_t;

static SETUP_RETR_t setup_retr_reg = {
	.addr = 0x04,
	.arc  = 0,
	.ard  = 0,
};


typedef struct {
uint16_t addr 				: 8;
uint16_t rf_ch        : 7;
uint16_t reserved     : 1;
}RF_CH_t;

static RF_CH_t rf_channel = {
	.addr = 0x05,
	.reserved = 0, .rf_ch = 0x02,
	};

typedef struct {
uint16_t addr 				: 8;
uint16_t obsolete     : 1;
uint16_t rf_pwr       : 2;
uint16_t rf_dr_high   : 1;
uint16_t pll_lock     : 1;
uint16_t rf_dr_low    : 1;
uint16_t reserved 		: 1;
uint16_t cont_wave    : 1;
}RF_SETUP_t;

static RF_SETUP_t rf_setup = {
	.addr = 0x06,
	.obsolete = 0,  .rf_pwr = 3,   .rf_dr_high = 1, .pll_lock = 1,
	.rf_dr_low = 0, .reserved = 0, .cont_wave = 1,
};


void Enable_nRF24Module()
{
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
}

void Disable_nRF24Module()
{
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
}

void Enable_CS_nRF24()
{
  HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_RESET);
}

void Disable_CS_nRF24()
{
  HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_SET);
}

void Write_Data_SPI(uint8_t * pData, uint16_t Size)
{
	uint8_t Wcmd = 0x20;
	pData[0] &=0x1f;//clear bit7-bit5
	pData[0] |= Wcmd;
	Enable_CS_nRF24();
  HAL_SPI_Transmit(&hspi2, pData, Size, 100);
	Disable_CS_nRF24();
}

void Read_Data_SPI(uint8_t* pWData, uint8_t * pRData, uint16_t Size)
{
  Enable_CS_nRF24();
  HAL_SPI_TransmitReceive(&hspi2, pWData, pRData, Size, 100);
  Disable_CS_nRF24();
}

/**
  * @brief  config nRF24L01P basic register.
  * @para   None
  * @retval None
  */
void nRF24_Config()
{
  uint8_t *pData = (uint8_t*)&configure;
	
  Write_Data_SPI(pData, 2);
}

void nRF24_PowerUp()
{
  uint8_t *pData = (uint8_t*)&configure;
	configure.pwr_up   = 1;

  Write_Data_SPI(pData, 2);
}

void nRF24_PowerDown()
{
  uint8_t *pData = (uint8_t*)&configure;
	configure.pwr_up   = 0;

  Write_Data_SPI(pData, 2);
}

/**
  * @brief  config nRF24L01P setup register, data rate,power level, continous wave.
  * @para   None
  * @retval None
  */
void nRF24_RF_Setup()
{
  uint8_t *pData = (uint8_t*)&rf_setup;

	Write_Data_SPI(pData, 2);
}

void Read_RF_Setup_Reg()
{
  uint8_t pWData[2] = {0x06,0};
	uint8_t pRData[2] = {0};

	Read_Data_SPI(pWData, pRData, 2);
	
}

/*Enable Auto Acknowledgment*/
void nRF24_Set_ENAA()
{
  uint8_t *pData = (uint8_t*)&enaa_reg;

	Write_Data_SPI(pData, 2);
}

/*Setup of Automatic Retransmission*/
void nRF24_Set_RETR()
{
  uint8_t *pData = (uint8_t*)&setup_retr_reg;

	Write_Data_SPI(pData, 2);
}

/*Enabled RX Addresses*/
void nRF24_EN_RXADDR()
{
  uint8_t *pData = (uint8_t*)&en_rxaddr_reg;

	Write_Data_SPI(pData, 2);
}

/*Setup of Address Widths */
void nRF24_Setup_AW()
{
  uint8_t *pData = (uint8_t*)&setup_aw_reg;

	Write_Data_SPI(pData, 2);
}

/**
  * @brief  Set nRF24L01P frequency.
  * @para   pwr,0-3,-18dbm,-12dbm,-6dbm,0dbm
  * @retval 0,abnomal, pwr is out of range;1, normal result
  */
uint8_t nRF24_SetPower(uint8_t pwr)
{
  if(pwr > 3)
		return 0;
	
  uint8_t *pData = (uint8_t*)&rf_setup;

	rf_setup.rf_pwr    = pwr;
	
	Write_Data_SPI(pData, 2);

	return 1;
}

void nRF24_PLL_Lock()
{
  if(rf_setup.pll_lock == 1)
		return;

  uint8_t *pData = (uint8_t*)&rf_setup;
	rf_setup.pll_lock = 1;
	
	Write_Data_SPI(pData, 2);
}

void nRF24_PLL_unLock()
{
	if(rf_setup.pll_lock == 0)
		return;

	uint8_t *pData = (uint8_t*)&rf_setup;
	rf_setup.pll_lock = 0;
	
	Write_Data_SPI(pData, 2);
}



/**
  * @brief  Set nRF24L01P frequency.
  * @para   freq,2400MHz - 2500MHz
  * @retval 0,abnomal, freq is out of range;1, normal result
  */
uint8_t nRF24_SetFreq(uint16_t freq)
{
  if((freq < 2400) ||(freq > 2500))
		return 0;
	
  uint8_t *pData = (uint8_t*)&rf_channel;
	rf_channel.rf_ch = freq - 2400;

	Write_Data_SPI(pData, 2);
	
	return 1;
}


/**
  * @brief  initial nRF24L01.
  * @para   None
  * @retval None
  */
void Initial_nRF24L01P()
{

  Enable_nRF24Module();
  HAL_Delay(50);
	
  //setup the config register
  nRF24_Config();
	
  nRF24_PowerDown();
	
  //set frequency
  nRF24_SetFreq(2450);
  //setup continous wave mode and power level
  nRF24_RF_Setup();
	
	nRF24_PowerUp();
	
	HAL_Delay(50);
}

void Set_Freq(uint16_t freq)
{
		nRF24_PowerDown();
		nRF24_PLL_unLock();
		nRF24_SetFreq(freq);
		nRF24_PLL_Lock();
	//	nRF24_RF_Setup();
	
		nRF24_PowerUp();
}

