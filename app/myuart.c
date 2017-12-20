/***************************************
* 文件名  ：usart1.c
* 描述    ：配置USART1
* 实验平台：MINI STM32开发板 基于STM32F103C8T6
* 硬件连接：------------------------
*          | PA9  - USART1(Tx)      |
*          | PA10 - USART1(Rx)      |
*           ------------------------
* 库版本  ：ST3.0.0

**********************************************************************************/

#include "myuart.h"
#include <stdarg.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
//#ifdef __GNUC__
//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif
//
//PUTCHAR_PROTOTYPE
//{
//
//  USART_SendData(USART1, (u16) ch);
//
//  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//  {}
//  return ch;
//}

void USART1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* ê1?ü USART1 ê±?ó*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  /* USART1 ê1ó?IO???ú???? */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //?′ó?í?íìê?3?
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//????ê?è?
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //3?ê??ˉGPIOA

  NVIC_InitTypeDef NVIC_InitStructure;
  //Usart1 NVIC
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//?à??ó??è??
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //×óó??è??3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQê1?üí¨μà
  NVIC_Init(&NVIC_InitStructure); //?ù?Y???¨μ?2?êy3?ê??ˉNVIC??′??÷
  /* USART1 1¤×÷?￡ê????? */
  USART_InitStructure.USART_BaudRate = 115200;	//2¨ì??êéè??￡o115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//êy?Y??êyéè??￡o8??
  USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//í￡?1??éè??￡o1??
  USART_InitStructure.USART_Parity = USART_Parity_No ;  //ê?·?????D￡?é￡o?T
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ó2?tá÷?????￡ê?éè??￡o??óDê1?ü
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//?óê?ó?·￠?í??ê1?ü
  USART_Init(USART1, &USART_InitStructure);  //3?ê??ˉUSART1
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//?a?????D?D??
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a?????D?D??
  USART_Cmd(USART1, ENABLE);// USART1ê1?ü
}


void UART_PutChar (unsigned char SendData)
{
  USART_SendData(USART1,SendData);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*·￠?íò???×??úêy?Y*/
void UART1SendByte(unsigned char SendData)
{
  USART_SendData(USART1,SendData);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*?óê?ò???×??úêy?Y*/
unsigned char UART1GetByte(unsigned char* GetData)
{
  if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
  {  return 0;//??óDê?μ?êy?Y
  }
  *GetData = USART_ReceiveData(USART1);
  return 1;//ê?μ?êy?Y
}
/*?óê?ò???êy?Y￡??íé?·μ???óê?μ?μ??a??êy?Y*/
void UART1Test(void)
{
  unsigned char i = 0;

  while(1)
  {
    while(UART1GetByte(&i))
    {
      USART_SendData(USART1,i);
    }
  }
}

void UART1SendStr(unsigned char *str)
{
  while(*str)
  {
    UART1SendByte(*str++);
  }
}


uint8_t usart_rx_buffer[USART_RX_BUFFER_LEN];
uint8_t Rx_Counter=0;
uint8_t ReceiveState=0;//?D??ê?·?ê?íêêy?Y
void USART1_IRQHandler(void)
{
  uint8_t Clear=Clear;

  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//?óê?μ?ò???×??úêy?Y±￡′?
  {
    usart_rx_buffer[Rx_Counter++]=USART1->DR;
  }
  else if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)//è?1?ê?μ?ò???êy?Y
  {
    Clear=USART1->SR;
    Clear=USART1->DR;
    ReceiveState=1;
    Rx_Counter=0;
//    UART1SendStr(usart_rx_buffer);
  }
}
