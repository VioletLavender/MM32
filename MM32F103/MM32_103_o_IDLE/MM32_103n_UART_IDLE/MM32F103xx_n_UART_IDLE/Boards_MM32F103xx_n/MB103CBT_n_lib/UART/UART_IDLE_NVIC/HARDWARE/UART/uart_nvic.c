/**
******************************************************************************
* @file    uart_nvic.c
* @author  AE Team
* @version V1.1.0
* @date    28/08/2019
* @brief   This file provides all the uart_nvic firmware functions.
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
#include "uart_nvic.h"




#if EN_UART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取UARTx->SR能避免莫名其妙的错误
u8 UART_RX_BUF[UART_REC_LEN];     //接收缓冲,最大UART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 UART_RX_STA = 0;     //接收状态标记
u8  UART_IDLE_start=0;


void uart_nvic_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能UART1，GPIOA时钟

    //UART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

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

void UART1_IRQHandler(void)                	//串口1中断服务程序
{
    u8 Res;
	/*
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        Res = UART_ReceiveData(UART1);	//读取接收到的数据
			
        if((Res == 0x0d) && ((UART_RX_STA & 0X3FFF) > 0))
        {
            UART_RX_STA |= 0x4000;
            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
            UART_RX_STA++;
        }
        else if((UART_RX_STA & 0x4000) && ((UART_RX_STA & 0X3FFF) > 0)) //接收到了0x0d
        {
            if(Res == 0x0a)
            {
                UART_RX_STA |= 0x8000;
            }
            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
            UART_RX_STA++;
        }
        else
        {
            UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
            UART_RX_STA++;
            UART_RX_STA = UART_RX_STA & 0X3FFF;
            if((UART_RX_STA) > (UART_REC_LEN - 1))
                UART_RX_STA = 0; //接收数据错误,重新开始接收
        }

    }
		*/

    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //接收中断(使用定时器定时设定超时中断模拟UART空闲中断)
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        Res = UART_ReceiveData(UART1);	//读取接收到的数据			  
				UART_RX_BUF[UART_RX_STA & 0X3FFF] = Res ;
				UART_RX_STA++;
				UART_RX_STA = UART_RX_STA & 0X3FFF;
				if((UART_RX_STA) > (UART_REC_LEN - 1))
				{
						UART_RX_STA = 0; //接收数据错误,重新开始接收
					  UART_IDLE_start=0;
				}        
        //=================================================
				TIM2->CNT=0;          //每收到一个字节数据则清零定时器计数器使定时计算器重新计数
				if(UART_IDLE_start==0)//UART接收到数据帧第一个字节则使能定时器
				{
				  UART_IDLE_start=1;
					TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
					TIM_Cmd(TIM2,ENABLE); 	//使能定时器2 开始进行UART空闲计时
				}
				
    }
}
#endif

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

