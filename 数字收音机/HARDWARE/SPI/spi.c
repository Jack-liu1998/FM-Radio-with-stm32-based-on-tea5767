#include "spi.h"
#include "stm32f10x_spi.h"

							  
//SPI2口初始化
void SPIx_Init(void)
{
	RCC->APB1RSTR|=1<<14;      //SPI2复位 
	RCC->APB1RSTR&=~(1<<14);   //SPI2结束复位
		 
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能 	 
	RCC->APB1ENR|=1<<14;      //SPI2时钟使能 
		   
	
	GPIOB->CRH&=0X000FFFFF; 
	GPIOB->CRH|=0XBBB00000;//PB13.14.15复用推挽	    
	GPIOB->ODR|=0X7<<13;    //PB13.14.15上拉
		
	SPI2->CR1|=0<<10;//全双工模式	
	SPI2->CR1|=1<<9; //软件nss管理
	SPI2->CR1|=1<<8;  

	SPI2->CR1|=1<<2; //SPI主机
	SPI2->CR1|=0<<11;//8bit数据格式	
	//对24L01要设置 CPHA=0;CPOL=0;
	SPI2->CR1|=1<<1; //CPOL=0时空闲模式下SCK为1 
	SPI2->CR1|=1<<0; //第一个时钟的下降沿,CPHA=1 CPOL=1
	  
	SPI2->CR1|=7<<3; //Fsck=Fcpu/256
	SPI2->CR1|=0<<7; //MSBfirst   
	SPI2->CR1|=1<<6; //SPI设备使能
	SPIx_ReadWriteByte(0xff);//启动传输		 
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_4   4分频   (SPI 18M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI2->CR1&=0XFFC7;//Fsck=Fcpu/256
	switch(SpeedSet)
	{
		case SPI_SPEED_2://二分频
			SPI2->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
			break;
		case SPI_SPEED_4://四分频
			SPI2->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
			break;
		case SPI_SPEED_8://八分频
			SPI2->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
			break;
		case SPI_SPEED_16://十六分频
			SPI2->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
			break;
		case SPI_SPEED_256://256分频
			SPI2->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
			break;
	}		 
	SPI2->CR1|=1<<6; //SPI设备使能	  
} 
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI2->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI2->DR;          //返回收到的数据				    
}































