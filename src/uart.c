#include "stdAfx.h"
#include "uart.h"

// uart settings
#define TX_BUFF_SIZE        256
#define RX_BUFF_SIZE        128

// some macro
#define TXE1_INTERRUPT_EN           USART1->CR1 |=  USART_CR1_TXEIE
#define TXE1_INTERRUPT_DIS          USART1->CR1 &= ~USART_CR1_TXEIE
#define UART1_GET_BYTE(byte)        byte = USART1->DR
#define UART1_PUT_BYTE(byte)        USART1->DR = byte; TXE1_INTERRUPT_EN


static volatile BYTE        txBuff [TX_BUFF_SIZE];
static volatile BYTE        rxBuff [RX_BUFF_SIZE];

static RingBuff				uartTxQueue;
static RingBuff				uartRxQueue;

const WORD rxTimeout = 3;

static WORD statusReg;
static WORD uartFlags = 0;

static __inline void uartInitPort (void);


enum {
    uartRXOverflow  = 0x01,
    uartTxReady     = 0x02,

};

/// initialization procedure
///
void uartInit (void)
{
    USART_InitTypeDef USART_InitStructure;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    uartInitPort();
      
    USART_InitStructure.USART_BaudRate = 115200;// скорость
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 бит данных
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //один стоп бит
    USART_InitStructure.USART_Parity = USART_Parity_Odd;//USART_Parity_No; //четность - нет
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // управлени потоком - нет
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       // разрешаем прием и передачу
            
    USART_Init(USART1, &USART_InitStructure); //инизиализируе
    
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ (USART1_IRQn);
    USART1->CR1 |= (USART_CR1_TXEIE | USART_CR1_RXNEIE);
    
    USART1->CR1 |= USART_CR1_M; // parity format
    
    USART_Cmd(USART1, ENABLE);
    
    rbInit( &uartTxQueue, (BYTE*)txBuff, TX_BUFF_SIZE );
    rbInit( &uartRxQueue, (BYTE*)rxBuff, RX_BUFF_SIZE );
    
    uartFlags |= uartTxReady;
}
//---------------------------------------------------------------------------

/// put a data byte
///
void uartPutByte (BYTE byte)
{
    while(!(USART1->SR & USART_SR_TC)); //Проверка завершения передачи предыдущих данных
    USART1->DR = byte; //Передача данных    
}
//---------------------------------------------------------------------------


/// usart1 irq
///
void USART1_IRQHandler (void) __irq
{  
    BYTE dummy = 0;
    
    statusReg = USART1->SR;
    // new data byte is avalible
    if( statusReg & USART_SR_RXNE ) {
        UART1_GET_BYTE(dummy);
            if( !rbPushFromIsrB( &uartRxQueue, dummy ) )
                uartFlags |= uartRXOverflow;
            
    }
    // transmitter is empty
    if( statusReg & USART_SR_TC ) {
         if( rbPopFromIsrB(&uartTxQueue, &dummy) ) {
            UART1_PUT_BYTE(dummy);
         }
         else {
            uartFlags |= uartTxReady;
            // interrupt clear?
            TXE1_INTERRUPT_DIS;
        }
    }
}
//---------------------------------------------------------------------------

/// put a buffer
///
DWORD uartPutBytes (const BYTE* bytes, DWORD count)
{
    const BYTE* pos = bytes;
	const BYTE* end = bytes + count;
    BOOL ok = TRUE;
    
    if( !(uartFlags & uartTxReady) ) 
        return 0;
            
    // fill the buffer
    while( pos < end && ok ) {
        ok = rbPushB( &uartTxQueue, *pos);
        ++pos;
    }
    
    if( ok ) {
        // clear ready flag
        uartFlags &= ~ uartTxReady;
        // en irq
        TXE1_INTERRUPT_EN;
        
        return count;
    }
    else {
        // clear buff
        BYTE b ;
        while( rbPopB(&uartTxQueue, &b) );
        return 0;
    }

}
//---------------------------------------------------------------------------

/// get a buffer
///
DWORD uartGetBytes(BYTE* pBytes, DWORD count, DWORD timeout)
{
	DWORD dwReceived = 0;

	if( pBytes != NULL && count ) {
		if( !timeout )
			timeout = 1;
	
        while( dwReceived < count && timeout ) {
            if( rbPopTimeoutB( &uartRxQueue, pBytes, rxTimeout) ) {		
                ++pBytes;
                ++dwReceived;
            }
            else
                timeout = (timeout > rxTimeout)	? timeout - rxTimeout : 0;
        }
    }
    
	return dwReceived;
}
//---------------------------------------------------------------------------

/// estimation
///
DWORD uartEstimateGet (DWORD byteCnt)
{
    return byteCnt * rxTimeout;
}
//---------------------------------------------------------------------------

/// target io initialization
///
static __inline void uartInitPort (void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // PB6 -> TX UART.
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //инициализируем
     
    //PB7  -> RX UART. 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
            
    // set AF
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //PD5 to TX 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); //PD6 to RX 
}
//---------------------------------------------------------------------------
