#include "remap_vtor.h"
#include "main.h"

#define APPLICATION_ADDRESS     (uint32_t)0x08007000
#if   (defined ( __CC_ARM ))
  __IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
#elif (defined (__ICCARM__))
#pragma location = 0x20000000
  __no_init __IO uint32_t VectorTable[48];
#elif defined ( __GNUC__ )
  __IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
#endif



void Remap_VectorTable(void)
{
	uint32_t i=0;

	for(i = 0; i < 48; i++)
  {
    VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
  }
	
	/* Enable the SYSCFG peripheral clock*/
  //__HAL_RCC_SYSCFG_CLK_ENABLE();
  
  /* Remap SRAM at 0x00000000 */
  __HAL_SYSCFG_REMAPMEMORY_SRAM();
}


