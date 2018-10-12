#ifndef __TOUCH_H
#define __TOUCH_H	

/* 按键状态	*/ 
#define Key_Down 0x01
#define Key_Up   0x00 
/* 笔杆结构体 */
typedef struct 
{
	u16 X0;//原始坐标
	u16 Y0;
	u16 X; //最终/暂存坐标
	u16 Y;						   	    
	u8  Key_Sta;//笔的状态			  
	//触摸屏校准参数
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

extern Pen_Holder Pen_Point;

/*触摸屏芯片连接引脚配置	*/   
#define PEN  PCin(4)   //PC4 INT
#define DOUT PAin(6)   //PA6  MISO
#define TDIN PAout(7)  //PA7  MOSI
#define TCLK PAout(5)  //PA5  SCLK
#define TCS  PCout(6)  //PC6  CS 

/* ADS7843/7846/UH7843/7846/XPT2046/TSC2046 指令集 */
#define CMD_RDY 0X90  //0B10010000即用差分方式读X坐标
#define CMD_RDX	0XD0  //0B11010000即用差分方式读Y坐标  
  
void Touch_Init(void);
void Touch_Adjust(void);
void Convert_Pos(void);
void Pen_Int_Set(uint8_t en);
void Touch_Configuration(void);
void ADS_Write_Byte(uint8_t num);
uint16_t ADS_Read_AD(uint8_t CMD);
uint16_t ADS_Read_XY(uint8_t xy);
uint8_t Read_TP_Once(void);
uint8_t Read_ADS2(uint16_t *x,uint16_t *y);
uint8_t Read_ADS(uint16_t *x,uint16_t *y);
void Draw_Big_Point(uint8_t x,uint16_t y);
void Drow_Touch_Point(uint8_t x,uint16_t y);

#endif
