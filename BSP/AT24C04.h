#ifndef AT24C04_H
#define AT24C04_H

#include "i2c.h"


HAL_StatusTypeDef AT24C04_WriteBytes(I2C_HandleTypeDef *hi2c, uint8_t *pData,uint8_t pageAddr, uint8_t byteAddr, uint8_t byteNum);
HAL_StatusTypeDef AT24C04_ReadBytes(I2C_HandleTypeDef *hi2c, uint8_t *pData,uint8_t pageaddr, uint8_t byteaddr, uint16_t bytenum);






#endif
