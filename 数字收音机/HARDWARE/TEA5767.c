#include "TEA5767.h"
#include "stm32f10x.h"
#include "USART.h"
#include "delay.h"
#include "myiic.h"

#define max_freq 108000
#define min_freq 87500

unsigned char radio_data[5]={0x29,0xc2,0x20,0x11,0x00};

unsigned char read_data[5];        //TEA5767读出的状态
unsigned long frequency;
unsigned int pll;

u8 key_down=0;


/**********************************************************
** 函数名:TEA5767_Write
** 功能描述:向TEA5767写入5个字节数据
** 输入参数:无
** 输出参数:无
** 返    回:无
***********************************************************/
void TEA5767_Write(void)
{
    unsigned char i;
    IIC_Start(); //发送起始信号
    IIC_Send_Byte(0xc0);        //TEA5767写地址
    IIC_Wait_Ack();	  //等待应答
    for(i=0;i<5;i++)
    {
        IIC_Send_Byte(radio_data[i]);//连续写入5个字节数据
        IIC_Ack(); //发送应答
    }
    IIC_Stop(); //发送停止信号   
}

/**********************************************************
** 函数名:Get_PLL
** 功能描述:由频率值计算PLL
** 输入参数:无
** 输出参数:pll--得到PLL值
** 返    回:无
***********************************************************/
void Get_PLL(void)
{
    unsigned char hlsi;
    hlsi=radio_data[2]&0x10;  //HLSI位
    if (hlsi)
        pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //频率单位:k
    else
        pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //频率单位:k
}


/**********************************************************
** 函数名:Get_Frequency
** 功能描述:由PLL计算频率
** 输入参数:无
** 输出参数:frequency--得到频率值
** 返    回:无
***********************************************************/
void Get_Frequency(void)
{
    unsigned char hlsi;
    unsigned int npll=0;
    npll=pll;
    hlsi=radio_data[2]&0x10;
    if(hlsi)
        frequency=(unsigned long)((float)(npll)*(float)8.192-225);    //频率单位:KHz
    else
        frequency=(unsigned long)((float)(npll)*(float)8.192+225);    //频率单位:KHz
}

/**********************************************************
** 函数名:Get_Frequency
** 功能描述:由PLL计算频率
** 输入参数:无
** 输出参数:frequency--得到频率值
** 返    回:无
***********************************************************/
void TEA5767_Read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    IIC_Start();
    IIC_Send_Byte(0xc1);        //TEA5767读地址
    IIC_Wait_Ack();
    for(i=0;i<5;i++)   //读取5个字节数据
    {
        read_data[i]=IIC_Read_Byte(1);//读取数据后，发送应答
    }
    IIC_Stop();
    temp_l=read_data[1];//得到PLL低8位 
    temp_h=read_data[0];//得到PLL高6位
    temp_h&=0x3f;
    pll=temp_h*256+temp_l; //PLL值
    Get_Frequency();//转换为频率值
}
/**********************************************************
** 函数名:Set_Frequency
** 功能描述:设置电台频率
** 输入参数:fre--写入的电台频率值（kHz）
** 输出参数:无
** 返    回:无
***********************************************************/
int Set_Frequency(u32 fre)
{
	frequency=fre;
	Get_PLL();
    radio_data[0]=pll/256;
    radio_data[1]=pll%256;
    radio_data[2]=0x20;
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();
	
	    TEA5767_Read();//读取频率值
 
    if((read_data[0]&0x80))     //搜台成功，可保存其频率值待用
    {
       return 1;
    }    
		else return 0;
}
/**********************************************************
** 函数名:Search
** 功能描述:手动搜索电台，不用考虑TEA5767用于搜台的相关位:SM,SUD
** 输入参数:mode=1，向上搜索，频率值+0.1MHz
			mode=0，向下搜索，频率值-0.1MHz
** 输出参数:无
** 返    回:无
***********************************************************/
void Search(char mode)
{
    TEA5767_Read(); //读取当前频率值       
    if(mode) //向上搜索
    {
        frequency+=10;
        if(frequency>max_freq)//频率达到最大值
            frequency=min_freq;
    }
    else   //向下搜索
    {
        frequency-=10;
        if(frequency<min_freq)//频率达到最小值
            frequency=max_freq;
    }          
    Get_PLL();//计算PLL值
    radio_data[0]=pll/256; //重新写入5个字节数据
    radio_data[1]=pll%256;
    radio_data[2]=0x20;
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();
	TEA5767_Read();//读取频率值
	//while(GPIOA->IDR&GPIO_Pin_0); //等待PA0按键释放
}

/**********************************************************
** 函数名:Auto_Search
** 功能描述:自动搜索电台
** 输入参数:mode=1，频率增加搜索
			mode=0，频率减小搜索
** 输出参数:无
** 返    回:无
***********************************************************/
void Auto_Search(char mode)
{
	TEA5767_Read();//读取频率
    Get_PLL();	   //转换为PLL值
    if(mode)//频率增加搜台
        radio_data[2]=0xa0;
    else	//频率减小搜台
        radio_data[2]=0x20;  
	  
    radio_data[0]=pll/256+0x40;
    radio_data[1]=pll%256;    
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();  //写入5个字节数据
    TEA5767_Read();//读取频率值
//	printf(" |%d\n",frequency);	//串口输出频率值 
    if((read_data[0]&0x80))     //搜台成功，可保存其频率值待用
    {
        TEA5767_Read();
//		printf(" frequency=%d\n",frequency);
		// while(GPIOA->IDR&GPIO_Pin_0); //等待PA0按键释放
    }    
}

 