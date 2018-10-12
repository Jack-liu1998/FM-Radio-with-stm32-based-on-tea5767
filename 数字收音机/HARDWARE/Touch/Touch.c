/**************************************************************
** 	火牛开发板
**	功能介绍： 触摸屏驱动代码
**  版本：V1.0  
**	论坛：bbs.openmcu.com
**	旺宝：www.openmcu.com   
**  邮箱；support@openmcu.com 
***************************************************************/
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "hardware_conf.h"
#include "sys.h"
#include "Touch.h"
#include "ili932x_conf.h"
#include "stdlib.h"
#include "math.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
Pen_Holder Pen_Point;	/* 定义笔实体 */ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
EXTI_InitTypeDef EXTI_InitStructure;

extern void Delay(__IO uint32_t nTime);
//SPI写数据
//向7843写入1byte数据   
void ADS_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0;//上升沿有效	   	 
		TCLK=1;      
	} 			    
} 		 
//SPI读数据 
//从7846/7843/XPT2046/UH7843/UH7846读取adc值	   
uint16_t ADS_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t i;
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK=0;//先拉低时钟 	 
	TCS=0; //选中ADS7843	 
	ADS_Write_Byte(CMD);//发送命令字
	for(i=100;i>0;i--);
//	delay_us(6);//ADS7846的转换时间最长为6us
	TCLK=1;//给1个时钟，清除BUSY   	    
	TCLK=0; 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK=0;//下降沿有效  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   //只有高12位有效.
	TCS=1;//释放ADS7843	 
	return(Num);   
}
//读取一个坐标值
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值
uint16_t ADS_Read_XY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//带滤波的坐标读取
//最小值不能少于100.
uint8_t Read_ADS(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//50,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
#define ERR_RANGE 50 //误差范围 
uint8_t Read_ADS2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
//读取一次坐标值	
//仅仅读取一次,知道PEN松开才返回!					   
uint8_t Read_TP_Once(void)
{
	uint8_t t=0;	    
	Pen_Int_Set(0);//关闭中断
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(PEN==0&&t<=250)
	{
		t++;
		Delay(10);
	};
	Pen_Int_Set(1);//开启中断		 
	if(t>=250)return 0;//按下2.5s 认为无效
	else return 1;	
}

//////////////////////////////////////////////////
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
void Drow_Touch_Point(uint8_t x,uint16_t y)
{
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6);//画中心圈
}	  
//画一个大点
//2*2的点			   
void Draw_Big_Point(uint8_t x,uint16_t y)
{	    
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}
//////////////////////////////////////////////////

//转换结果
//根据触摸屏的校准参数来决定转换后的结果,保存在X0,Y0中
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=340-Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	}
}
//触摸屏校准代码
//得到四个校准参数
void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//清屏   
	POINT_COLOR=RED;//红色 
	LCD_Clear(WHITE);//清屏 
	Drow_Touch_Point(20,20);//画点1 
	Pen_Point.Key_Sta=Key_Up;//消除触发信号 
	Pen_Point.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//按键按下了
		{
			if(Read_TP_Once())//得到单次按键值
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(WHITE);//清屏 
					Drow_Touch_Point(220,20);//画点2
					break;
				case 2:
					LCD_Clear(WHITE);//清屏 
					Drow_Touch_Point(20,300);//画点3
					break;
				case 3:
					LCD_Clear(WHITE);//清屏 
					Drow_Touch_Point(220,300);//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    	//对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
						LCD_Clear(WHITE);//清屏 
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						LCD_Clear(WHITE);//清屏 
						Drow_Touch_Point(20,20);
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
						LCD_Clear(WHITE);//清屏 
						Drow_Touch_Point(20,20);
						continue;
					}//正确了
					//计算结果
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//清屏
					LCD_ShowString(35,110,(uint8_t*)"Touch Screen Adjust OK!");//校正完成
					Delay(1000);
					LCD_Clear(WHITE);//清屏   
					return;//校正完成				 
			}
		}
	} 
}
void Touch_Init()
{
	Touch_Configuration();
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);//第一次读取初始化			 

  	/* Connect PEN EXTI Line to Key Button GPIO Pin */
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);

  	/* Configure PEN EXTI Line to generate an interrupt on falling edge */  
  	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

  	/* Generate software interrupt: simulate a falling edge applied on PEN EXTI line */
  //	EXTI_GenerateSWInterrupt(EXTI_Line4);

	LCD_Clear(WHITE);//清屏
  // Touch_Adjust();  //屏幕校准,带自动保存	   
}
void Touch_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//对触控屏校准参数初始化
	Pen_Point.xfac=0.123067;
	Pen_Point.yfac=0.183126;
	Pen_Point.xoff=0.000000;
	Pen_Point.yoff=0.000000;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE );  //重要！！

	//下面是SPI相关GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//下面是SPI相关GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//Configure PC6 pin: TP_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//Configure PC4 pin: TP_INT pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	//上拉输入
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}
//PEN中断设置	 
void Pen_Int_Set(uint8_t en)
{
	if(en)EXTI->IMR|=1<<4;   //开启line4上的中断	  	
	else EXTI->IMR&=~(1<<4); //关闭line4上的中断	   
}
//中断,检测到PEN脚的一个下降沿.
//置位Pen_Point.Key_Sta为按下状态
//中断线4线上的中断检测
void EXTI4_IRQHandler()
{
	//Pen_Int_Set(DISABLE);
  if(EXTI_GetITStatus(EXTI_IMR_MR4) != RESET)
	{
		Pen_Point.Key_Sta=Key_Down;//按键按下  		
		Read_ADS2(&Pen_Point.X,&Pen_Point.Y);//刷新触控坐标
		Convert_Pos();
    	/* Clear the Key Button EXTI line pending bit */
    	EXTI_ClearITPendingBit(EXTI_IMR_MR4);

	}
	//Pen_Int_Set(ENABLE);
}
