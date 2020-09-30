/**
******************************************************************************
* @file    uart.c
* @author  AE Team
* @version V1.1.0
* @date    28/08/2019
* @brief   This file provides all the uart firmware functions.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/
#include "sys.h"
#include "uart.h"
//////////////////////////////////////////////////////////////////
#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif

#ifdef USE_IAR
PUTCHAR_PROTOTYPE
{
    UART_SendData(UART1, (uint8_t) ch);
    while (UART_GetFlagStatus(UART1, UART_IT_TXIEN) == RESET);
    return ch;
}

#else
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((UART1->CSR & UART_IT_TXIEN) == 0); //循环发送,直到发送完毕
    UART1->TDR = (ch & (uint16_t)0x00FF);
    return ch;
}

#endif

void uart_initwBaudRate(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能UART1，GPIOA时钟

    //UART 初始化设置
    UART_InitStructure.UART_BaudRate = bound;//串口波特率
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;//字长为8位数据格式
    UART_InitStructure.UART_StopBits = UART_StopBits_1;//一个停止位
    UART_InitStructure.UART_Parity = UART_Parity_No;//无奇偶校验位
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//无硬件数据流控制
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	//收发模式

    UART_Init(UART1, &UART_InitStructure); //初始化串口1
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);//开启串口接受中断
    UART_Cmd(UART1, ENABLE);                    //使能串口1

    //UART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //UART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

}


/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/*-------------------------(C) COPYRIGHT 2019 MindMotion ----------------------*/
