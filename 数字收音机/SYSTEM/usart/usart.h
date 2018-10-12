#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"

#define RS232_USART                           USART1
#define RS485_USART                           USART2

//控制485发送数据，CS引脚为高电平
#define __485__TXRX_CLK              RCC_APB2Periph_GPIOA   
#define __485__TXRX_PORT             GPIOA    
#define __485__TXRX_PIN              GPIO_Pin_8

#define __485_CS_TX_EN               delay_us(500);PAout(8)=1;delay_us(500);
//控制485接收数据，CS引脚为低电平
#define __485_CS_RX_EN               delay_us(500); PAout(8)=0;delay_us(500); 


#define __232__FCOFF_CLK              RCC_APB2Periph_GPIOA  
#define __232__FCOFF_PORT             GPIOA    
#define __232__FCOFF_PIN              GPIO_Pin_1
 
#define __MAX3221_P16_FC0FF_On      delay_us(500); PAout(1)=1;delay_us(500);//见MAX3221手册第7、8页
#define __MAX3221_P16_FC0FF_Off     delay_us(500); PAout(1)=0;delay_us(500);




void vrs232_usart1_init(uint32_t Baud);
void vrs485_usart2_init(uint32_t Baud);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void USART1_SendByte(u8 Data);
void USART1Write(u8* data,u16 len);
void RS485_SendString( uint8_t *str);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
static void __485_delay(__IO u32 nCount)
{

	for(; nCount != 0; nCount--);
}

extern char uRS232_USART_BUFF[];
extern char uRS485_USART_BUFF[];
extern u32 uRS232_buff_ptr;
extern u32 uRS485_buff_ptr;
extern u8  uRS232_Flag;
extern u8  uRS485_Flag;


#endif


