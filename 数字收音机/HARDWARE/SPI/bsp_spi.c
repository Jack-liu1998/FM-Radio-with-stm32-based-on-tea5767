#include "bsp_spi.h"
/**
  * @brief  SPI2初始化
  * @param  None
  * @retval None
  */
void vSPIx_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	
    /*** PB13--SCK   PB15--MOSI  PB14--MISO   PB12--NSS***/			  		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13| GPIO_Pin_15;	
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    DAC5311_OFF;
     
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Soft SPI_NSS_Hard
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure); 

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设
    
}  

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  
    uint16_t SPITimeout = 200;

    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) return 0;
    }

    /* Send Half Word through the FLASH_SPI peripheral */
    SPI_I2S_SendData(SPI2, HalfWord);

    SPITimeout = 200;

    /* Wait to receive a Half Word */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if((SPITimeout--) == 0) return 0;
    }
    /* Return the Half Word read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}

