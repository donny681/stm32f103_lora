//*****************************************************************************
//
//! \file main.c
//! \brief main application
//! \version 1.0.0.0
//! \date $Creat_time$
//! \author $Creat_author$
//! \copy
//!
//! Copyright (c) 2014 CooCox.  All rights reserved.
//
//! \addtogroup project
//! @{
//! \addtogroup main
//! @{
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "system_Stm32f10x.h"
#include "timer.h"
#include "radio.h"
#include "sx1276-Hal.h"
#include "led.h"
#include "stm32f10x.h"
#include "core_cm3.h"
#define BUFFER_SIZE                                 9 // Define the payload size here


static uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
static uint8_t Buffer[BUFFER_SIZE];					// RF buffer

static uint8_t EnableMaster = true; 				// Master/Slave selection

tRadioDriver *Radio = NULL;

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

/*
 * Manages the master operation
 */
void OnMaster( void )
{
    uint8_t i;

    switch( Radio->Process( ) )
    {
    case RF_RX_TIMEOUT:
        // Send the next PING frame
        Buffer[0] = 'P';
        Buffer[1] = 'I';
        Buffer[2] = 'N';
        Buffer[3] = 'G';
        for( i = 4; i < BufferSize; i++ )
        {
            Buffer[i] = i - 4;
        }
        Radio->SetTxPacket( Buffer, BufferSize );
        break;
    case RF_RX_DONE:
        Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );

        if( BufferSize > 0 )
        {
            if( strncmp( ( const char* )Buffer, ( const char* )PongMsg, 4 ) == 0 )
            {
                // Indicates on a LED that the received frame is a PONG
                LedToggle( LED_GREEN );

                // Send the next PING frame
                Buffer[0] = 'P';
                Buffer[1] = 'I';
                Buffer[2] = 'N';
                Buffer[3] = 'G';
                // We fill the buffer with numbers for the payload
                for( i = 4; i < BufferSize; i++ )
                {
                    Buffer[i] = i - 4;
                }
                Radio->SetTxPacket( Buffer, BufferSize );
            }
            else if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
            { // A master already exists then become a slave
                EnableMaster = false;
                LedOff( LED_RED );
            }
        }
        break;
    case RF_TX_DONE:
        // Indicates on a LED that we have sent a PING
        LedToggle( LED_RED );
        Radio->StartRx( );
        break;
    default:
        break;
    }
}

/*
 * Manages the slave operation
 */
void OnSlave( void )
{
    uint8_t i;

    switch( Radio->Process( ) )
    {
    case RF_RX_DONE:
        Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );

        if( BufferSize > 0 )
        {
            if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
            {
                // Indicates on a LED that the received frame is a PING
                LedToggle( LED_GREEN );

               // Send the reply to the PONG string
                Buffer[0] = 'P';
                Buffer[1] = 'O';
                Buffer[2] = 'N';
                Buffer[3] = 'G';
                // We fill the buffer with numbers for the payload
                for( i = 4; i < BufferSize; i++ )
                {
                    Buffer[i] = i - 4;
                }

                Radio->SetTxPacket( Buffer, BufferSize );
            }
        }
        break;
    case RF_TX_DONE:
        // Indicates on a LED that we have sent a PONG
        LedToggle( LED_RED );
        Radio->StartRx( );
        break;
    default:
        break;
    }
}
void Board_Init( void )
{
	SystemInit();

	if( SysTick_Config( SystemCoreClock / 1000 ) )
	    {
	        /* Capture error */
	        while (1);
	    }
}

int main(void)
{
	Board_Init();
	USART1_Config();
	UART1SendStr("hello\r\n");
//	printf("hello\n\r");
	SpiInit();
	Radio = RadioDriverInit( );
	Radio->Init( );
//	Radio->StartRx( );
//	printf("hello\n\r");
    while( 1 )
    {
//    	printf("hello\n\r");
    	UART1SendStr("hello\r\n");
//        if( EnableMaster == true )
//        {
//            OnMaster( );
//        }
//        else
//        {
//            OnSlave( );
//        }
//#if( PLATFORM == SX12xxEiger ) && ( USE_UART == 1 )
//
//        UartProcess( );
//
//        {
//            uint8_t data = 0;
//            if( UartGetChar( &data ) == UART_OK )
//            {
//                UartPutChar( data );
//            }
//        }
//#endif
    }
}
