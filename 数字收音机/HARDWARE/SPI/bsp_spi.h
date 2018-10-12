#ifndef _BSP_SPI_H
#define _BSP_SPI_H
#include "stm32f10x.h"





#define  DAC5311_ON    GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define  DAC5311_OFF   GPIO_SetBits(GPIOB,GPIO_Pin_12)	


























void vSPIx_Config(void);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);


#endif


