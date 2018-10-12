/**************************************Copyright (c)********************************
**                            Shenzhen KaNuoDi electronics Co.,LTD.
**
**                                 http://www.openmcu.com
**
**------File Info-------------------------------------------------------------------
** File name:               main.c
** Latest modified Date:    2010-11-06
** Latest Version:          1.0
** Descriptions:            TFT液晶的驱动文件
************************************************************************************/
#include "lcd.h"
#include "hz16.h"
#include "font.h" 
#include "stdio.h"	 
#include "sys.h"
//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	
//写8位数据函数
//用宏定义,提高速度.
#define LCD_WR_DATA(data){\
LCD_RS=1;\
LCD_CS=0;\
DATAOUT(data);\
LCD_WR=0;\
LCD_WR=1;\
LCD_CS=1;\
} 
//写寄存器函数
void LCD_WR_REG(u8 data)
{ 
	LCD_RS=0;//写地址  
 	LCD_CS=0; 
	DATAOUT(data); 
	LCD_WR=0; 
	LCD_WR=1; 
 	LCD_CS=1;  
}  
//写寄存器
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   
//读寄存器
u16 LCD_ReadReg(u8 LCD_Reg)
{										   
	u16 t;
	LCD_WR_REG(LCD_Reg);  //写入要读的寄存器号  
	GPIOE->CRL=0X88888888; //PB0-7  上拉输入
	GPIOE->CRH=0X88888888; //PB8-15 上拉输入
	GPIOE->ODR=0XFFFF;    //全部输出高
	LCD_RS=1;
	LCD_CS=0;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD=0;					   
	LCD_RD=1;
	t=DATAIN;  
	LCD_CS=1;   
	GPIOE->CRL=0X33333333; //PB0-7  上拉输出
	GPIOE->CRH=0X33333333; //PB8-15 上拉输出
	GPIOE->ODR=0XFFFF;    //全部输出高
	return t;  
} 
//读数据
u16 LCD_ReadData(void)
{										   
	u16 t;
	GPIOE->CRL=0X88888888; //PB0-7  上拉输入
	GPIOE->CRH=0X88888888; //PB8-15 上拉输入
	GPIOE->ODR=0XFFFF;    //全部输出高
	LCD_RS=1;
	LCD_CS=0;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD=0;					   
	LCD_RD=1;
	t=DATAIN;  
	LCD_CS=1;   
	GPIOE->CRL=0X33333333; //PB0-7  上拉输出
	GPIOE->CRH=0X33333333; //PB8-15 上拉输出
	GPIOE->ODR=0XFFFF;    //全部输出高
	return t;  
}  
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(R34);
}	 
//LCD写GRAM
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);
}		 
//读取个某点的颜色值	 
//x:0~239
//y:0~319
//返回值:此点的颜色
u16 LCD_ReadPoint(u8 x,u16 y)
{
	u16 t;			   
	LCD_SetCursor(x,y);
	LCD_WR_REG(R34);       //选择GRAM地址 
	GPIOE->CRL=0X88888888; //PB0-7  上拉输入
	GPIOE->CRH=0X88888888; //PB8-15 上拉输入
	GPIOE->ODR=0XFFFF;     //全部输出高
	LCD_RS=1;
	LCD_CS=0;
	//读取数据(读GRAM时,需要读2次)
	LCD_RD=0;					   
	LCD_RD=1;
	//dummy READ
	LCD_RD=0;					   
	LCD_RD=1;
	t=DATAIN;  
	LCD_CS=1;   
	GPIOE->CRL=0X33333333; //PB0-7  上拉输出
	GPIOE->CRH=0X33333333; //PB8-15 上拉输出
	GPIOE->ODR=0XFFFF;    //全部输出高  
	return LCD_BGR2RGB(t);
}
//LCD开启显示
void LCD_DisplayOn(void)
{					   
	LCD_WriteReg(R7, 0x0173); //26万色显示开启
}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   
	LCD_WriteReg(R7, 0x0);//关闭显示 
}    
//LCD延时函数 10MS
void Delay (u32 nCount)
{
	volatile int i;	 	
	for (i=0;i<nCount*100;i++);
}
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
__inline void LCD_SetCursor(u8 Xpos, u16 Ypos)
{
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
		LCD_WriteReg(0x004E, Xpos);
		LCD_WriteReg(0X004F, Ypos);
	}
	else 
	{
		LCD_WriteReg(0x0020, Xpos);
		LCD_WriteReg(0X0021, Ypos);
	}
}
//设置窗口
void LCD_WindowMax (unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end) {

	if(DeviceCode==0x8999)
	{
		LCD_WriteReg(0x44,x|((x_end-1)<<8));
		LCD_WriteReg(0x45,y);
		LCD_WriteReg(0x46,y_end-1);
	}
	else
	{
		LCD_WriteReg(0x50, x);                      /* Horizontal GRAM Start Address      */
		LCD_WriteReg(0x51, x_end-1);               /* Horizontal GRAM End   Address (-1) */
		LCD_WriteReg(0x52, y);                      /* Vertical   GRAM Start Address      */
		LCD_WriteReg(0x53, y_end-1);                /* Vertical   GRAM End   Address (-1) */
	}
}  
//画点
//x:0~239
//y:0~319
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u8 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	LCD_WR_REG(R34);//开始写入GRAM
	LCD_WR_DATA(POINT_COLOR); 
} 	 
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void LCD_Init(void)
{ 

 	RCC->APB2ENR|=1<<5;//先使能外设PORTD时钟
 	RCC->APB2ENR|=1<<6;//先使能外设PORTE时钟

	RCC->APB2ENR|=1<<0;    //开启辅助时钟

	//PORTD12~15推挽输出 	
	GPIOD->CRH&=0X0000FFFF;
	GPIOD->CRH|=0X33330000; 
	GPIOD->ODR|=0XF000; 	 
	//PORTE 推挽输出 	
	GPIOE->CRH=0X33333333;
	GPIOE->CRL=0X33333333; 	 
	GPIOE->ODR=0XFFFF;
	  					 
	Delay(5); // delay 50 ms 
	LCD_WriteReg(0x0000,0x0001);
	Delay(5); // delay 50 ms 
	DeviceCode = LCD_ReadReg(0x0000);   
	//printf("\r\n");   
	//printf("ID=0x%x \r\n",DeviceCode);  
	if(DeviceCode==0x9325||DeviceCode==0x9328)//ILI9325
	{
  		LCD_WriteReg(0x00e7,0x0010);      
        LCD_WriteReg(0x0000,0x0001);//开启内部时钟
        LCD_WriteReg(0x0001,0x0100);     
        LCD_WriteReg(0x0002,0x0700);//电源开启                    
		//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
		LCD_WriteReg(0x0003,0x1030);
        //LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0207);	           
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);//display setting         
        LCD_WriteReg(0x000c,0x0001);//display setting          
        LCD_WriteReg(0x000d,0x0000);//0f3c          
        LCD_WriteReg(0x000f,0x0000);
		//电源配置
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000);                 
        Delay(5); 
        LCD_WriteReg(0x0010,0x1590);   
        LCD_WriteReg(0x0011,0x0227);
        Delay(5); 
        LCD_WriteReg(0x0012,0x009c);                  
        Delay(5); 
        LCD_WriteReg(0x0013,0x1900);   
        LCD_WriteReg(0x0029,0x0023);
        LCD_WriteReg(0x002b,0x000e);
        Delay(5); 
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);           
		Delay(5); 
		//伽马校正
        LCD_WriteReg(0x0030,0x0007); 
        LCD_WriteReg(0x0031,0x0707);   
        LCD_WriteReg(0x0032,0x0006);
        LCD_WriteReg(0x0035,0x0704);
        LCD_WriteReg(0x0036,0x1f04); 
        LCD_WriteReg(0x0037,0x0004);
        LCD_WriteReg(0x0038,0x0000);        
        LCD_WriteReg(0x0039,0x0706);     
        LCD_WriteReg(0x003c,0x0701);
        LCD_WriteReg(0x003d,0x000f);
        Delay(5); 
        LCD_WriteReg(0x0050,0x0000); //水平GRAM起始位置 
        LCD_WriteReg(0x0051,0x00ef); //水平GRAM终止位置                    
        LCD_WriteReg(0x0052,0x0000); //垂直GRAM起始位置                    
        LCD_WriteReg(0x0053,0x013f); //垂直GRAM终止位置  
        
        LCD_WriteReg(0x0060,0xa700);        
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
      
        LCD_WriteReg(0x0090,0x0010);     
        LCD_WriteReg(0x0092,0x0000);  
        LCD_WriteReg(0x0093,0x0003);
        LCD_WriteReg(0x0095,0x0110);
        LCD_WriteReg(0x0097,0x0000);        
        LCD_WriteReg(0x0098,0x0000);  
        //开启显示设置    
        LCD_WriteReg(0x0007,0x0133);   
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x013f);
	}
	else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteReg(0x03,0x1030);//Entry Mode Set.
		//LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.
	
		LCD_WriteReg(0x04,0x0000);	//Scalling Contral.
		LCD_WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		Delay(5); 
		LCD_WriteReg(0x07,0x0101);	//Display Contral.
		Delay(5); 								  
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		LCD_WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		LCD_WriteReg(0x13,0x0b00);								//Power Control 4.
		LCD_WriteReg(0x29,0x0000);								//Power Control 7.
	
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));	    
		LCD_WriteReg(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		LCD_WriteReg(0x51,239);	//Set Y Star
		LCD_WriteReg(0x52,0);	//Set Y End.t.
		LCD_WriteReg(0x53,319);	//
	
		LCD_WriteReg(0x60,0x2700);	//Driver Output Control.
		LCD_WriteReg(0x61,0x0001);	//Driver Output Control.
		LCD_WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		LCD_WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		LCD_WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97,(0<<8));	//
		LCD_WriteReg(0x98,0x0000);	//Frame Cycle Contral.	   
		LCD_WriteReg(0x07,0x0173);	//(0x0173)
	}				    
	else if(DeviceCode==0x1505)
	{
		// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        LCD_WriteReg(0x0007,0x0000);
        Delay(5); 
        LCD_WriteReg(0x0012,0x011C);//0x011A   why need to set several times?
        LCD_WriteReg(0x00A4,0x0001);//NVM	 
        LCD_WriteReg(0x0008,0x000F);
        LCD_WriteReg(0x000A,0x0008);
        LCD_WriteReg(0x000D,0x0008);	    
  		//伽马校正
        LCD_WriteReg(0x0030,0x0707);
        LCD_WriteReg(0x0031,0x0007); //0x0707
        LCD_WriteReg(0x0032,0x0603); 
        LCD_WriteReg(0x0033,0x0700); 
        LCD_WriteReg(0x0034,0x0202); 
        LCD_WriteReg(0x0035,0x0002); //?0x0606
        LCD_WriteReg(0x0036,0x1F0F);
        LCD_WriteReg(0x0037,0x0707); //0x0f0f  0x0105
        LCD_WriteReg(0x0038,0x0000); 
        LCD_WriteReg(0x0039,0x0000); 
        LCD_WriteReg(0x003A,0x0707); 
        LCD_WriteReg(0x003B,0x0000); //0x0303
        LCD_WriteReg(0x003C,0x0007); //?0x0707
        LCD_WriteReg(0x003D,0x0000); //0x1313//0x1f08
        Delay(5); 
        LCD_WriteReg(0x0007,0x0001);
        LCD_WriteReg(0x0017,0x0001);//开启电源
        Delay(5); 
  		//电源配置
        LCD_WriteReg(0x0010,0x17A0); 
        LCD_WriteReg(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        LCD_WriteReg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        LCD_WriteReg(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        LCD_WriteReg(0x002A,0x0000);  
        LCD_WriteReg(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        LCD_WriteReg(0x0012,0x013E);// 0x013C  power supply on
        //Coordinates Control//
        LCD_WriteReg(0x0050,0x0000);//0x0e00
        LCD_WriteReg(0x0051,0x00EF); 
        LCD_WriteReg(0x0052,0x0000); 
        LCD_WriteReg(0x0053,0x013F); 
    	//Pannel Image Control//
        LCD_WriteReg(0x0060,0x2700); 
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006A,0x0000); 
        LCD_WriteReg(0x0080,0x0000); 
    	//Partial Image Control//
        LCD_WriteReg(0x0081,0x0000); 
        LCD_WriteReg(0x0082,0x0000); 
        LCD_WriteReg(0x0083,0x0000); 
        LCD_WriteReg(0x0084,0x0000); 
        LCD_WriteReg(0x0085,0x0000); 
  		//Panel Interface Control//
        LCD_WriteReg(0x0090,0x0013);//0x0010 frenqucy
        LCD_WriteReg(0x0092,0x0300); 
        LCD_WriteReg(0x0093,0x0005); 
        LCD_WriteReg(0x0095,0x0000); 
        LCD_WriteReg(0x0097,0x0000); 
        LCD_WriteReg(0x0098,0x0000); 
  
        LCD_WriteReg(0x0001,0x0100); 
        LCD_WriteReg(0x0002,0x0700); 
        LCD_WriteReg(0x0003,0x1030); 
        LCD_WriteReg(0x0004,0x0000); 
        LCD_WriteReg(0x000C,0x0000); 
        LCD_WriteReg(0x000F,0x0000); 
        LCD_WriteReg(0x0020,0x0000); 
        LCD_WriteReg(0x0021,0x0000); 
        LCD_WriteReg(0x0007,0x0021); 
        Delay(20);
        LCD_WriteReg(0x0007,0x0061); 
        Delay(20);
        LCD_WriteReg(0x0007,0x0173); 
        Delay(20);
	}							 
	else if(DeviceCode==0x8989)
	{
		LCD_WriteReg(0x0000,0x0001);Delay(5);//打开晶振
    	LCD_WriteReg(0x0003,0xA8A4);Delay(5);//0xA8A4
    	LCD_WriteReg(0x000C,0x0000);Delay(5);    
    	LCD_WriteReg(0x000D,0x080C);Delay(5);    
    	LCD_WriteReg(0x000E,0x2B00);Delay(5);    
    	LCD_WriteReg(0x001E,0x00B0);Delay(5);    
    	LCD_WriteReg(0x0001,0x2B3F);Delay(5);//驱动输出控制320*240  0x6B3F
    	LCD_WriteReg(0x0002,0x0600);Delay(5); 
    	LCD_WriteReg(0x0010,0x0000);Delay(5); 
    	LCD_WriteReg(0x0011,0x6070);Delay(5);//定义数据格式  16位色 		横屏 0x6058
    	LCD_WriteReg(0x0005,0x0000);Delay(5); 
    	LCD_WriteReg(0x0006,0x0000);Delay(5); 
    	LCD_WriteReg(0x0016,0xEF1C);Delay(5); 
    	LCD_WriteReg(0x0017,0x0003);Delay(5); 
    	LCD_WriteReg(0x0007,0x0233);Delay(5);//0x0233       
    	LCD_WriteReg(0x000B,0x0000);Delay(5); 
    	LCD_WriteReg(0x000F,0x0000);Delay(5);//扫描开始地址
    	LCD_WriteReg(0x0041,0x0000);Delay(5); 
    	LCD_WriteReg(0x0042,0x0000);Delay(5); 
    	LCD_WriteReg(0x0048,0x0000);Delay(5); 
    	LCD_WriteReg(0x0049,0x013F);Delay(5); 
    	LCD_WriteReg(0x004A,0x0000);Delay(5); 
    	LCD_WriteReg(0x004B,0x0000);Delay(5); 
    	LCD_WriteReg(0x0044,0xEF00);Delay(5); 
    	LCD_WriteReg(0x0045,0x0000);Delay(5); 
    	LCD_WriteReg(0x0046,0x013F);Delay(5); 
    	LCD_WriteReg(0x0030,0x0707);Delay(5); 
    	LCD_WriteReg(0x0031,0x0204);Delay(5); 
    	LCD_WriteReg(0x0032,0x0204);Delay(5); 
    	LCD_WriteReg(0x0033,0x0502);Delay(5); 
    	LCD_WriteReg(0x0034,0x0507);Delay(5); 
    	LCD_WriteReg(0x0035,0x0204);Delay(5); 
    	LCD_WriteReg(0x0036,0x0204);Delay(5); 
    	LCD_WriteReg(0x0037,0x0502);Delay(5); 
    	LCD_WriteReg(0x003A,0x0302);Delay(5); 
    	LCD_WriteReg(0x003B,0x0302);Delay(5); 
    	LCD_WriteReg(0x0023,0x0000);Delay(5); 
    	LCD_WriteReg(0x0024,0x0000);Delay(5); 
    	LCD_WriteReg(0x0025,0x8000);Delay(5); 
    	LCD_WriteReg(0x004f,0);        //行首址0
    	LCD_WriteReg(0x004e,0);        //列首址0
	}
	else if(DeviceCode==0x8999)	   //对应的驱动IC为SSD1289
	{
		//************* Start Initial Sequence **********//
		LCD_WriteReg(0x00, 0x0001); // Start internal OSC.
		LCD_WriteReg(0x01, 0x3B3F); // Driver output control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
		LCD_WriteReg(0x02, 0x0600); // set 1 line inversion
		//************* Power control setup ************/
		LCD_WriteReg(0x0C, 0x0007); // Adjust VCIX2 output voltage
		LCD_WriteReg(0x0D, 0x0006); // Set amplitude magnification of VLCD63
		LCD_WriteReg(0x0E, 0x3200); // Set alternating amplitude of VCOM
		LCD_WriteReg(0x1E, 0x00BB); // Set VcomH voltage
		LCD_WriteReg(0x03, 0x6A64); // Step-up factor/cycle setting
		//************ RAM position control **********/
		LCD_WriteReg(0x0F, 0x0000); // Gate scan position start at G0.
		LCD_WriteReg(0x44, 0xEF00); // Horizontal RAM address position
		LCD_WriteReg(0x45, 0x0000); // Vertical RAM address start position
		LCD_WriteReg(0x46, 0x013F); // Vertical RAM address end position
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x30, 0x0000);
		LCD_WriteReg(0x31, 0x0706);
		LCD_WriteReg(0x32, 0x0206);
		LCD_WriteReg(0x33, 0x0300);
		LCD_WriteReg(0x34, 0x0002);
		LCD_WriteReg(0x35, 0x0000);
		LCD_WriteReg(0x36, 0x0707);
		LCD_WriteReg(0x37, 0x0200);
		LCD_WriteReg(0x3A, 0x0908);
		LCD_WriteReg(0x3B, 0x0F0D);
		//************* Special command **************/
		LCD_WriteReg(0x28, 0x0006); // Enable test command
		LCD_WriteReg(0x2F, 0x12EB); // RAM speed tuning
		LCD_WriteReg(0x26, 0x7000); // Internal Bandgap strength
		LCD_WriteReg(0x20, 0xB0E3); // Internal Vcom strength
		LCD_WriteReg(0x27, 0x0044); // Internal Vcomh/VcomL timing
		LCD_WriteReg(0x2E, 0x7E45); // VCOM charge sharing time  
		//************* Turn On display ******************/
		LCD_WriteReg(0x10, 0x0000); // Sleep mode off.
		Delay(8); // Wait 30mS
		LCD_WriteReg(0x11, 0x6870); // Entry mode setup. 262K type B, take care on the data bus with 16it only
		LCD_WriteReg(0x07, 0x0033); // Display ON	*/
	}						  
	Delay(5000);
	//LCD_LED=0;//点亮背光	 
	LCD_Clear(WHITE);
}  		  
  
//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u32 index=0;      
	LCD_SetCursor(0x00,0x0000);//设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
	for(index=0;index<76800;index++)
	{
		LCD_WR_DATA(Color);    
	}
}  
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u8 xsta,u16 ysta,u8 xend,u16 yend,u16 color)
{                    
    u32 n;
	//设置窗口										
	LCD_WriteReg(R80, xsta); //水平方向GRAM起始地址
	LCD_WriteReg(R81, xend); //水平方向GRAM结束地址
	LCD_WriteReg(R82, ysta); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, yend); //垂直方向GRAM结束地址	
	LCD_SetCursor(xsta,ysta);//设置光标位置  
	LCD_WriteRAM_Prepare();  //开始写入GRAM	 	   	   
	n=(u32)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){LCD_WR_DATA(color);}//显示所填充的颜色. 
	//恢复设置
	LCD_WriteReg(R80, 0x0000); //水平方向GRAM起始地址
	LCD_WriteReg(R81, 0x00EF); //水平方向GRAM结束地址
	LCD_WriteReg(R82, 0x0000); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, 0x013F); //垂直方向GRAM结束地址	    
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2)
{
    u16 x, y, t;
	if((x1==x2)&&(y1==y2))LCD_DrawPoint(x1, y1);
	else if(abs(y2-y1)>abs(x2-x1))//斜率大于1 
	{
		if(y1>y2) 
		{
			t=y1;
			y1=y2;
			y2=t; 
			t=x1;
			x1=x2;
			x2=t; 
		}
		for(y=y1;y<y2;y++)//以y轴为基准 
		{
			x=(u32)(y-y1)*(x2-x1)/(y2-y1)+x1;
			LCD_DrawPoint(x, y);  
		}
	}
	else     //斜率小于等于1 
	{
		if(x1>x2)
		{
			t=y1;
			y1=y2;
			y2=t;
			t=x1;
			x1=x2;
			x2=t;
		}   
		for(x=x1;x<=x2;x++)//以x轴为基准 
		{
			y =(u32)(x-x1)*(y2-y1)/(x2-x1)+y1;
			LCD_DrawPoint(x,y); 
		}
	} 
}    
//画矩形
void LCD_DrawRectangle(u8 x1, u16 y1, u8 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u8 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//在指定位置显示一个字符
//x:0~234
//y:0~308
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u8 x,u16 y,u8 num,u8 size,u8 mode)
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp;
    u8 pos,t;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	    
	//设置窗口
	LCD_WindowMax(x,y,x+size/2,y+size);										
//	LCD_WriteReg(R80,x);           //水平方向GRAM起始地址
//	LCD_WriteReg(R81,x+(size/2-1));//水平方向GRAM结束地址
//	LCD_WriteReg(R82,y);           //垂直方向GRAM起始地址
//	LCD_WriteReg(R83,y+size-1);    //垂直方向GRAM结束地址	
	LCD_SetCursor(x,y);            //设置光标位置  
	LCD_WriteRAM_Prepare();        //开始写入GRAM	   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)
				{
					LCD_WR_DATA(POINT_COLOR);
				}else LCD_WR_DATA(BACK_COLOR);	        
		        temp>>=1; 
		    }
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}	    
	//恢复窗体大小
	LCD_WindowMax(0x0000,0x0000,240,320);	 

}  
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);	 
void LCD_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void LCD_ShowString(u8 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
        LCD_ShowChar(x,y,*p,16,0);
        x+=8;
        p++;
    }  
}
/*********************************************************************************************************
** 函数名称: __writeOneHzChar
** 功能描述: 在指定的起始坐标处写一个汉字
** 输  　入: pucMsk 指向汉字字模存放的起始地址
**           x0 起始坐标所在列的位置
**           y0 起始坐标所在行的位置
**           color 字符显示的颜色
** 输　  出: 无
** 全局变量: 无
** 调用模块: setCoordinate()、__writeData16()、__readData16()
** 返    回：所写字符的列宽16
** 作  　者: Dream
** 日　  期: 2010年11月6日
**------------------------------------------------------------------------------------------------------
** 修 改 人:
** 日　  期:
**------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
unsigned long __writeOneHzChar(unsigned char *pucMsk,
                               unsigned short x0,
                               unsigned short y0,
                               unsigned short color)
{
    unsigned long i,j;
    unsigned short mod[16];                                             /* 当前字模                     */
    unsigned short *pusMsk;                                             /* 当前字库地址                 */
    unsigned short y;
	LCD_WriteReg(R80,x0);           //水平方向GRAM起始地址
	LCD_WriteReg(R81,x0+15);		//水平方向GRAM结束地址
	LCD_WriteReg(R82,y0);           //垂直方向GRAM起始地址
	LCD_WriteReg(R83,y0+15);    	//垂直方向GRAM结束地址	
	   
    pusMsk = (unsigned short *)pucMsk;
    for(i=0; i<16; i++)                                                 /* 保存当前汉字点阵式字模       */
    {
        mod[i] = *pusMsk++;                                             /* 取得当前字模，半字对齐访问   */
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);   /* 字模交换高低字节（为了显示   */
                                                                        /* 需要）                       */
    }
    y = y0;
    for(i=0; i<16; i++) {                                               /* 16行                         */
        #ifdef __DISPLAY_BUFFER                                         /* 使用显存显示                 */
        for(j=0; j<16; j++) {                                           /* 16列                         */
            if((mod[i] << j)& 0x8000) {                                 /* 显示字模                     */
                DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                                           /* 直接显示                     */
        
        LCD_SetCursor(x0, y);                               /* 设置写数据地址指针           */
		LCD_WriteRAM_Prepare();     						/* 开始写入GRAM */	 	  
        for(j=0; j<16; j++) {                         		/* 16列                         */
            if((mod[i] << j) & 0x8000) {                	/* 显示字模                     */
				LCD_WR_DATA(color);    
            } else {
				LCD_WR_DATA(BACK_COLOR);          			/* 用读方式跳过写空白点的像素   */
            }
        }
        y++;
        #endif
    }
    return (16);                                            /* 返回16位列宽                 */
}



/*********************************************************************************************************
** 函数名称: __writeOneASCII
** 功能描述: 在指定的起始坐标处写一个ASCII码字符
** 输  　入: pucMsk 指向ASCII码字符字模存放的起始地址
**           x0 起始坐标所在列的位置
**           y0 起始坐标所在行的位置
**           color 字符显示的颜色
** 输　  出: 无
** 全局变量: 无
** 调用模块: setCoordinate()、__writeData16()、__readData16()
** 返    回：所写字符的列宽8
** 作  　者: Dream
** 日　  期: 2010年11月6日
**------------------------------------------------------------------------------------------------------
** 修 改 人:
** 日　  期:
**------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
unsigned long __writeOneASCII(unsigned char *pucMsk,
                              unsigned short x0,
                              unsigned int y0,
                              unsigned int color)
{
    unsigned long i,j;
    unsigned int y;
    unsigned char ucChar;
	LCD_WriteReg(R80,x0);           //水平方向GRAM起始地址
	LCD_WriteReg(R81,x0+7);//水平方向GRAM结束地址
	LCD_WriteReg(R82,y0);           //垂直方向GRAM起始地址
	LCD_WriteReg(R83,y0+15);    //垂直方向GRAM结束地址	
	    
    y = y0;
    for(i=0; i<16; i++) {                                               /* 16行                         */
        ucChar = *pucMsk++;
        #ifdef __DISPLAY_BUFFER                                         /* 使用显存显示                 */
        for(j=0; j<8; j++) {                                            /* 8列                          */
            if((ucChar << j)& 0x80) {                                   /* 显示字模                     */
                DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                                           /* 直接显示                     */
        
        LCD_SetCursor(x0, y);                                           /* 设置写数据地址指针           */
		LCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
        for(j=0; j<8; j++) {                                            /* 8列                          */
            if((ucChar << j) & 0x80) 			  /* 显示字模                     */
			{                                  
				LCD_WR_DATA(color);    
            }
			else 
			{
				LCD_WR_DATA(BACK_COLOR);
            }  
        }
        y++;
        #endif
    }
    return (8);                                                         /* 返回16位列宽                 */
}



/*********************************************************************************************************
** 函数名称: writeString
** 功能描述: 在指定的坐标处开始显示字符串
** 输  　入: pcStr 字符串存放首地址
**           x0 起始坐标所在列的位置
**           y0 起始坐标所在行的位置
**           color 字符串的显示颜色
** 输　  出: 无
** 全局变量: GBHZ_16 自定义汉字库字模数表
**           ASCII   自定义的ASCII码字库字模数表
** 调用模块: findHzIndex()、__writeOneHzChar()、__writeOneASCII()
** 作  　者: Dream
** 日　  期: 2010年11月6日
**------------------------------------------------------------------------------------------------------
** 修 改 人:
** 日　  期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void writeString(unsigned char *pcStr, unsigned short x0, unsigned int y0, unsigned int color)
{
    unsigned int usIndex;
    unsigned int usWidth = 0;
    FNT_GB16 *ptGb16 = 0;
    
    ptGb16 = (FNT_GB16 *)GBHZ_16;                                       /*                              */
    while(1)
    {
        if(*pcStr == 0) {
            break;                                                      /* 字符串结束                   */
        }
        
        x0 = x0 + (usWidth);                                            /* 调节字符串显示松紧度         */
        if(*pcStr > 0x80) {                                             /* 判断为汉字                   */
            if((x0 + 16) >= Gus_LCM_XMAX) {                             /* 检查剩余空间是否足够         */
                x0 = 0;
                y0 = y0 + 16;                                           /* 改变显示坐标                 */
                if(y0 >= Gus_LCM_YMAX) {                                /* 纵坐标超出                   */
                    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = __writeOneHzChar((unsigned char *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                                        /* 显示字符                     */
            pcStr += 2;
        } else {                                                        /* 判断为非汉字                 */
            if (*pcStr == '\r') {                                       /* 换行                         */
                y0 = y0 + 16;                                           /* 改变显示坐标                 */
                if(y0 >= Gus_LCM_YMAX) {                                /* 纵坐标超出                   */
                    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } else if (*pcStr == '\n') {                                /* 对齐到起点                   */
                x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            } else {
                if((x0 + 8) >= Gus_LCM_XMAX) {                          /* 检查剩余空间是否足够         */
                    x0 = 0;
                    y0 = y0 + 16;                                       /* 改变显示坐标                 */
                    if(y0 >= Gus_LCM_YMAX) {                            /* 纵坐标超出                   */
                        y0 = 0;
                    }
                }
                usWidth = __writeOneASCII((unsigned char *)&ASCII[(*pcStr - 0x20)][0], x0, y0, color);
                                                                        /* ASCII码表21H的值对应区位码3区*/
                pcStr += 1;
            }
        }
    }
		//恢复窗体大小	 
	LCD_WriteReg(R80, 0x0000); //水平方向GRAM起始地址
	LCD_WriteReg(R81, 0x00EF); //水平方向GRAM结束地址
	LCD_WriteReg(R82, 0x0000); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, 0x013F); //垂直方向GRAM结束地址
}
