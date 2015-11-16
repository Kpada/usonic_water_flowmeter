#include "stdAfx.h"
#include "spi.h"

/*********************** SPI *************************/

#define NSS_HIGH        GPIOA->ODR |=    0x01 << 4
#define NSS_LOW         GPIOA->ODR &= ~( 0x01 << 4 )

/// spi initialization procedure
///
void SpiInit (void)
{
    SPI_InitTypeDef spiStruct;
    GPIO_InitTypeDef spiGpio;
    // spi1 clock en
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    // port a clock en
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    // gpio
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	spiGpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	spiGpio.GPIO_Mode = GPIO_Mode_AF;
	spiGpio.GPIO_Speed = GPIO_Speed_40MHz;
	spiGpio.GPIO_OType = GPIO_OType_PP;
	spiGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &spiGpio);
    
    // spi
    SPI_StructInit(&spiStruct);	
	spiStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // Set direction 
	spiStruct.SPI_Mode = SPI_Mode_Master;  //Set chip as master
	spiStruct.SPI_DataSize = SPI_DataSize_8b; //Set word width to 8 bit (include data/command bit)
	spiStruct.SPI_CPOL = SPI_CPOL_Low; //Assume clock high in IDLE
	spiStruct.SPI_CPHA = SPI_CPHA_2Edge; //Set data set at upper edge of a clock
	spiStruct.SPI_NSS = SPI_NSS_Soft; //Disable hardware Chip Select
	spiStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //Set SPI bitrate
	spiStruct.SPI_FirstBit = SPI_FirstBit_MSB; //Transmit most significant byte first
	SPI_Init(SPI1, &spiStruct);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set); //Set internal value of Chip Select to high
	SPI_Cmd(SPI1, ENABLE);
    
    // NSS external pin (PC9)
    GPIOA->MODER |= ( 0x01 << ( 4 << 1) );
    NSS_HIGH;
    SPI_I2S_ReceiveData(SPI1);
    DelayMSec(1);
}
//---------------------------------------------------------------------------

/// put a byte (internal template)
///
static void SpiPutByte_Internal (BYTE data)
{
    volatile BYTE byte_;
    SPI1->DR = data;
    while( SPI1->SR & SPI_SR_BSY );
    byte_ = SPI1->DR; // dummy read
}
//---------------------------------------------------------------------------

/// get a byte (internal template)
///
static BYTE SpiGetByte_Internal (void)
{
    SPI_I2S_SendData(SPI1, 0xff);
    while( SPI1->SR & SPI_SR_BSY );
    while( !(SPI1->SR & SPI_SR_RXNE) );
    return SPI1->DR;
}
//---------------------------------------------------------------------------

/// put a byte (u8)
///
void SpiPutByte (BYTE data)
{
    NSS_LOW;
    SpiPutByte_Internal(data);
    NSS_HIGH;
}
//---------------------------------------------------------------------------

/// get a byte (u8)
///
BYTE SpiGetByte (void)
{
    BYTE data;
    NSS_LOW;
    data = SpiGetByte_Internal();
    NSS_HIGH;
    return data;
}
//---------------------------------------------------------------------------

/// get a word (u16)
///
WORD SpiGetWord (BYTE addr)
{
    WORD result = 0; 
    
    NSS_LOW;
    
    SpiPutByte_Internal(addr);
    result |= (WORD)SpiGetByte_Internal() << 8;
    result |= (WORD)SpiGetByte_Internal() << 0;

    NSS_HIGH;
    return result;
}
//---------------------------------------------------------------------------

/// put a word (u16)
/// 
void SpiPutWord (BYTE addr, BYTE data)
{
    NSS_LOW;
    
    SpiPutByte_Internal(addr);
    SpiPutByte_Internal(data);
    
    NSS_HIGH;
}
//---------------------------------------------------------------------------

/// put a dword (u32)
///
void SpiPutDword (BYTE addr, DWORD data)
{
    NSS_LOW;
    
    SpiPutByte_Internal(addr);
    SpiPutByte_Internal(data >> 24);
    SpiPutByte_Internal((data >> 16 ) & 0xFF);
    SpiPutByte_Internal((data >> 8 )  & 0xFF);
    SpiPutByte_Internal((data >> 0 )  & 0xFF);
    
    NSS_HIGH;
}
//---------------------------------------------------------------------------

/// get a dword (u32)
///
DWORD SpiGetDword (BYTE addr)
{
    DWORD result = 0; 
    
    NSS_LOW;
    
    SpiPutByte_Internal(addr);
    result |= (DWORD)SpiGetByte_Internal() << 24;
    result |= (DWORD)SpiGetByte_Internal() << 16;
    result |= (DWORD)SpiGetByte_Internal() << 8;
    result |= (DWORD)SpiGetByte_Internal() << 0;

    NSS_HIGH;
    return result;
}
//---------------------------------------------------------------------------

/// get a qword (u64)
///
QWORD SpiGetQword (BYTE addr)
{
    QWORD result = 0;
    
    NSS_LOW;
    
    SpiPutByte_Internal(addr);
    result |= (QWORD)SpiGetByte_Internal() << 48 ;
    result |= (QWORD)SpiGetByte_Internal() << 40 ;
    result |= (QWORD)SpiGetByte_Internal() << 32 ;
    result |= (QWORD)SpiGetByte_Internal() << 24 ;
    result |= (QWORD)SpiGetByte_Internal() << 16 ;
    result |= (QWORD)SpiGetByte_Internal() << 8 ;
    result |= (QWORD)SpiGetByte_Internal() << 0 ;
    
    NSS_HIGH;  
    return result;    
}
//---------------------------------------------------------------------------
