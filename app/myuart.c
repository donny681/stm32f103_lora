/***************************************
* �ļ���  ��usart1.c
* ����    ������USART1
* ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
* Ӳ�����ӣ�------------------------
*          | PA9  - USART1(Tx)      |
*          | PA10 - USART1(Rx)      |
*           ------------------------
* ��汾  ��ST3.0.0

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

  /* ��1?�� USART1 ����?��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  /* USART1 ��1��?IO???��???? */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //?�䨮?��?������?3?
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//????��?��?
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //3?��??��GPIOA

  NVIC_InitTypeDef NVIC_InitStructure;
  //Usart1 NVIC
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//?��??��??��??
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //������??��??3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ��1?�������̨�
  NVIC_Init(&NVIC_InitStructure); //?��?Y???����?2?��y3?��??��NVIC??��??��
  /* USART1 1�����?�꨺????? */
  USART_InitStructure.USART_BaudRate = 115200;	//2����??������??��o115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//��y?Y??��y����??��o8??
  USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//����?1??����??��o1??
  USART_InitStructure.USART_Parity = USART_Parity_No ;  //��?��?????D��?����o?T
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��2?t����?????�꨺?����??��o??��D��1?��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//?����?��?����?��??��1?��
  USART_Init(USART1, &USART_InitStructure);  //3?��??��USART1
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//?a?????D?D??
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a?????D?D??
  USART_Cmd(USART1, ENABLE);// USART1��1?��
}


void UART_PutChar (unsigned char SendData)
{
  USART_SendData(USART1,SendData);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*����?����???��??����y?Y*/
void UART1SendByte(unsigned char SendData)
{
  USART_SendData(USART1,SendData);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*?����?��???��??����y?Y*/
unsigned char UART1GetByte(unsigned char* GetData)
{
  if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
  {  return 0;//??��D��?��?��y?Y
  }
  *GetData = USART_ReceiveData(USART1);
  return 1;//��?��?��y?Y
}
/*?����?��???��y?Y��??����?����???����?��?��??a??��y?Y*/
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
uint8_t ReceiveState=0;//?D??��?��?��?������y?Y
void USART1_IRQHandler(void)
{
  uint8_t Clear=Clear;

  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//?����?��?��???��??����y?Y�����?
  {
    usart_rx_buffer[Rx_Counter++]=USART1->DR;
  }
  else if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)//��?1?��?��?��???��y?Y
  {
    Clear=USART1->SR;
    Clear=USART1->DR;
    ReceiveState=1;
    Rx_Counter=0;
//    UART1SendStr(usart_rx_buffer);
  }
}
