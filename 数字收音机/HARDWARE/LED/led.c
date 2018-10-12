#include "led.h"
#include "timer.h"
	
static __IO uint32_t ulLocal_Led_Time=0;

void LED_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_RUN_CLK, ENABLE);	

    GPIO_InitStructure.GPIO_Pin = LED_RUN_PIN;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(LED_RUN_PORT, &GPIO_InitStructure);					
    GPIO_ResetBits(LED_RUN_PORT,LED_RUN_PIN);						

}
 
/**
  * @brief  引脚翻转 
  * @param  GPIOx：端口号  GPIO_Pin：引脚
  * @retval 无
  */
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    GPIOx->ODR ^= GPIO_Pin;
}

//void vLedRun(uint32_t timevalue)
//{
//    if((ulGlobal_Time - ulLocal_Led_Time) > timevalue)
//    {
//        GPIO_ToggleBits(LED_RUN_PORT,LED_RUN_PIN);
//        ulLocal_Led_Time = ulGlobal_Time;
//    }  
//}


