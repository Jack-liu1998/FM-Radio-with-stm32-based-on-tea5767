#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_RUN_CLK     RCC_APB2Periph_GPIOC
#define LED_RUN_PORT    GPIOC
#define LED_RUN_PIN     GPIO_Pin_13


	
    
    
    
    
    
    
    
    
    
    
    
    
    
void LED_Init(void);//≥ı ºªØ
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void vLedRun(uint32_t timevalue);		

#endif
