/**
******************************************************************************
* @file    main.c
* @author  AE Team
* @version V1.1.0
* @date    28/08/2019
* @brief   This file provides all the main firmware functions.
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
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "tim2.h"
#include "uart_nvic.h"
/********************************************************************************************************
**函数信息 ：int main (void)
**功能描述 ：1、HSE 96M ; 3.3V ; PA9(UART1-TXD)、PA10(UART1-RXD) ;波特率=115200,8Bit,1Stop
             2、该例程使用TIM2定时器配合UART实现了UART空闲中断功能，可以接收不定长度的数据帧也不需要特定的结束符，
						    UART空闲时间可以按实际需求设定。
**输入参数 ：
**输出参数 ：
********************************************************************************************************/
int main(void)
{
    u8 t;
    u8 len;
    u16 times = 0;
	
    delay_init();	    	     //延时函数初始化
    uart_nvic_init(115200);	 //串口初始化为115200
	  /*PCLK1倍频后(96MHz)96分频,计数器值为1000,即1ms中断一次*/
    Tim2_UPCount_test(SystemCoreClock / 1000000 - 1, 1000 - 1);//以1ms超时设置为UART的空闲时间，该时间可以按实际需求设定
    LED_Init();		  	       //初始化与LED连接的硬件接口
	
	  printf("\r\n 使用TIM2模拟UART1空闲中断实验\r\n");
	
    while(1)
    {
        if(UART_RX_STA & 0x8000)//表示不定长数据帧接收完成
        {
            len = UART_RX_STA & 0x3fff; //得到此次接收到的数据长度
            printf("\r\n您发送的消息为:\r\n");
            for(t = 0; t < len; t++)
            {
                while (UART_GetFlagStatus(UART1, UART_IT_TXIEN) == RESET);
                UART_SendData(UART1, (u8)UART_RX_BUF[t]);
            }
            printf("\r\n\r\n");//插入换行
            UART_RX_STA = 0;
        }
        else
        {
            times++;
            if(times % 200 == 0)printf("请输入数据,无需回车键结束\r\n");
            if(times % 30 == 0)LED1 = !LED1; //闪烁LED,提示系统正在运行.
            delay_ms(10);
        }
    }
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

