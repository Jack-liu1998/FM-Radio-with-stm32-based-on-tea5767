#include "GUI.h"

extern Pen_Holder Pen_Point;//触控坐标

int TOUCH_LOC=0;//按键位置1为SEARCH 2为LAST 3为NEXT

extern int freqset;
extern int freqget_buf_i;
extern int cur_statue;

void show_freq()
{
	int temp_num[2];
	temp_num[0]=freqset/1000;
	temp_num[1]=(freqset/100)%10;
	

  LCD_ShowNum(50,210,temp_num[0],3,16);
	LCD_ShowString(80,210,".");
	LCD_ShowNum(90,210,temp_num[1],1,16);
}
void Judge_loc(void)
{
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)==0)
	{
	if((Pen_Point.X0<50)&&(Pen_Point.Y0>(Gus_LCM_YMAX-50)))
		TOUCH_LOC=SEARCH;
	else if((Pen_Point.X0<145)&&(Pen_Point.X0>50)&&(Pen_Point.Y0>Gus_LCM_YMAX-50))
		TOUCH_LOC=LAST;
	else if((Pen_Point.X0>145)&&(Pen_Point.Y0>Gus_LCM_YMAX-50))
		TOUCH_LOC=NEXT;
	}
}

void GUI_draw(void)
{
	int temp_num[2];
	temp_num[0]=freqset/100;
	temp_num[1]=(freqset/10)%10;
	
	LCD_Clear(WHITE);
	Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0); //画出鼠标
	//画出框体
	LCD_DrawLine(0,Gus_LCM_YMAX-50,Gus_LCM_XMAX,Gus_LCM_YMAX-50);//下方横线
	//左方竖线
	LCD_DrawLine(50,Gus_LCM_YMAX,50,0);
	//下方横线
	LCD_DrawLine(50,235,Gus_LCM_XMAX,235);
	//下方竖线
	LCD_DrawLine(145,Gus_LCM_YMAX-50,145,Gus_LCM_YMAX);
	//上方横线
	LCD_DrawLine(50,90,Gus_LCM_XMAX,90);
	//画出文字
	LCD_ShowString(20,Gus_LCM_YMAX-70,"Digital radio");
	LCD_ShowString(0,Gus_LCM_YMAX-35,"Search");
	LCD_ShowString(170,Gus_LCM_YMAX-35,"Next");
	LCD_ShowString(80,Gus_LCM_YMAX-35,"Last");
	
	LCD_ShowString(60,188,"Current Channal   ");
	LCD_ShowNum(50,210,temp_num[0],3,16);
	LCD_ShowString(80,210,".");
	LCD_ShowNum(90,210,temp_num[1],1,16);
	LCD_ShowString(100,210,"MHZ");
	
	LCD_ShowString(60,110,"Channels Searched");
	LCD_ShowNum(60,130,freqget_buf_i,2,16);
	
	LCD_ShowString(60,20,"Current Condition");
	switch(cur_statue)
	{
		case standby:
			LCD_ShowString(60,40,"Standby");
		break;
		case loading:
			LCD_ShowString(60,40,"Loading");
		break;
		case saving:
			LCD_ShowString(60,40,"Saving");
		break;
		case searching:
			LCD_ShowString(60,40,"Searching");
		break;
	}
}
