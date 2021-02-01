#include "AT24C04.h"

#define ADDR_AT24C04_WRITE_FIRST_16_PAGES 0xA0
#define ADDR_AT24C04_WRITE_LAST_16_PAGES 0xA2
#define PAGE_SIZE 16
#define PAGE_NUM  32
#define AT24C04_TIMEOUT 0xFF

 // Write bytes to the eeprom, location the page and byte address
 // pageAddr: 0-31
 // byteAddr: 0-15
 // byteNum:  1-16, the number to write in eeprom
HAL_StatusTypeDef AT24C04_WriteBytes(I2C_HandleTypeDef *hi2c, uint8_t *pData,uint8_t pageAddr, uint8_t byteAddr, uint8_t byteNum)
{
	HAL_StatusTypeDef Status;
	uint16_t MemAddress = 0;
	
	if((byteAddr + byteNum)>16)
		byteNum = 16 - byteAddr;
	
	if(pageAddr<16){
	MemAddress = (pageAddr<<4)|(byteAddr);
	Status = HAL_I2C_Mem_Write(&hi2c1, ADDR_AT24C04_WRITE_FIRST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, byteNum, AT24C04_TIMEOUT);
	}
	else if(pageAddr < 32){
	pageAddr -= 16;
	MemAddress = (pageAddr<<4)|(byteAddr);
	Status = HAL_I2C_Mem_Write(&hi2c1, ADDR_AT24C04_WRITE_LAST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, byteNum, AT24C04_TIMEOUT);
	}
	else
		return HAL_ERROR;
	
	while(Status != HAL_OK);

	return Status;
}

// Read bytes from the eeprom, location the page and byte address
// pageAddr: 0-31
// byteAddr: 0-15
// byteNum:  1-16, the number be readed from eeprom
HAL_StatusTypeDef AT24C04_ReadBytes(I2C_HandleTypeDef *hi2c, uint8_t *pData,uint8_t pageaddr, uint8_t byteaddr, uint16_t bytenum)
{
	HAL_StatusTypeDef Status;
	uint16_t MemAddress = 0;
	uint16_t Readnum_max = 512-pageaddr*16-(byteaddr+1);

	bytenum = (bytenum>Readnum_max)?(Readnum_max):(bytenum);
	
	if(pageaddr<16){
	MemAddress = (pageaddr<<4)|(byteaddr);
	Status = HAL_I2C_Mem_Read(&hi2c1, ADDR_AT24C04_WRITE_FIRST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, bytenum, AT24C04_TIMEOUT);
	}
	else if(pageaddr < 32){
	pageaddr -= 16;
	MemAddress = (pageaddr<<4)|(byteaddr);
	Status = HAL_I2C_Mem_Read(&hi2c1, ADDR_AT24C04_WRITE_LAST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, bytenum, AT24C04_TIMEOUT);
	}
	while(Status != HAL_OK);

	return Status;
}





