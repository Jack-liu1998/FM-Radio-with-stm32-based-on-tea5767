#include "sys.h"
#include "usart.h"	  

/*********************usart1串口初始化*************************/
void vrs232_usart1_init(uint32_t Baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	/* config PAx|USART1 clock */
    RCC_APB2PeriphClockCmd( __232__FCOFF_CLK , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO  ,ENABLE);

	/* USART1 GPIO config */
   	/*  U1TXD-->GPIOA.9  */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);		       
  	/*  U1RXD-->GPIOA.10 */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = __232__FCOFF_PIN;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(__232__FCOFF_PORT, &GPIO_InitStructure);
    __MAX3221_P16_FC0FF_Off;
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate 				= Baud;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx|USART_Mode_Tx;      //只接收
    USART_Init(USART1, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//Interrupt 	
    USART_ClearITPendingBit(USART1,USART_IT_TC);   
    USART_Cmd(USART1, ENABLE); 
}


void vrs485_usart2_init(uint32_t Baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd( __485__TXRX_CLK , ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);
	/*  U2TXD-->GPIOA.2  */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);		     
	/*  U1RXD-->GPIOA.3  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  RDE-->GPIOA.1  */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = __485__TXRX_PIN;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(__485__TXRX_PORT, &GPIO_InitStructure);
    //初始化完，485收发器状态
  	__485_CS_RX_EN;//__485_CS_TX_EN  __485_CS_RX_EN //准备发送数据

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure USART2 */
	USART_Init(USART2, &USART_InitStructure);	  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
	USART_ClearITPendingBit(USART2,USART_IT_TC);
	
}



/*************************usart1串口中断***********************/
#define RS232_USART_BUFF_LENGTH   20
char uRS232_USART_BUFF[RS232_USART_BUFF_LENGTH]={0};
u32 uRS232_buff_ptr = 0;
void USART1_IRQHandler(void){
    u8  utemp = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET)
   {
        utemp= USART_ReceiveData(RS232_USART);  
	}
}


#define RS485_USART_BUFF_LENGTH   20
char uRS485_USART_BUFF[RS485_USART_BUFF_LENGTH]={0};
u32 uRS485_buff_ptr = 0;
/******************usart2串口中断**************************/
void USART2_IRQHandler(void){
    u8  utemp = 0;
	if(USART_GetITStatus(RS485_USART, USART_IT_RXNE)!=RESET)
    {
        utemp = USART_ReceiveData(RS485_USART);
	}
}

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* 发送一个字节数据到USART */
    USART_SendData(pUSARTx,ch);

    /* 等待发送数据寄存器为空 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}
/*****************  指定长度的发送字符串 **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
	
    unsigned int k=0;
    if(RS485_USART == pUSARTx)
    {
        __485_CS_TX_EN;//发送
    } 
    else if(RS232_USART == pUSARTx)
    {
        __MAX3221_P16_FC0FF_On;
    
    }
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
    if(RS485_USART == pUSARTx)
    {
        __485_CS_RX_EN;//接收
    }
     else if(RS232_USART == pUSARTx)
    {
        __MAX3221_P16_FC0FF_Off;
    
    }
}

void RS485_SendString( uint8_t *str)
{
    __485_CS_TX_EN;//发送
    Usart_SendString(RS485_USART, (char *)str);
    __485_CS_RX_EN;//接收
}
