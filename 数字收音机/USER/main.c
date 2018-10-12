#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "myiic.h"
#include "timer.h"
#include "TEA5767.h"
#include "LCD.h"
#include "Touch.h"
#include "gui.h"
#include "save_flash.h"

void NVIC_Configuration(void);
extern int TOUCH_LOC;//按键判断

int main(void)
{	 	
		
    NVIC_Configuration();
    delay_init();	    	      //延时函数初始化	
    TIM2_Int_Init();            //10ms计数定时器初始化
	GPIO_Configuration(); 
		LCD_Init();	
	Pen_Int_Set(ENABLE);//??????
	Touch_Init();
			IIC_Init();	//IIC接口初始化
GUI_draw();
	//Set_Frequency(93700);
	while(1)
	{	
		TOUCH_LOC=0;
		Judge_loc();//??????????	
		//GUI_draw();
		//Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);
		switch(TOUCH_LOC)
		{
			case SEARCH:
				value_set_click();
			break;
			case LAST:
				FM_Last();
			break;
			case NEXT:
				FM_Next();
			break;
		}
	}
	
	
}
void NVIC_Configuration(void)
{ 
  	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//??IO????,????????!!!

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // ????? ?2 
	  
	/* Enable the EXTI4_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;	// ?????4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ????????2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // ???????0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // ?????4
	NVIC_Init(&NVIC_InitStructure);							  // ??????4
}


 
 








