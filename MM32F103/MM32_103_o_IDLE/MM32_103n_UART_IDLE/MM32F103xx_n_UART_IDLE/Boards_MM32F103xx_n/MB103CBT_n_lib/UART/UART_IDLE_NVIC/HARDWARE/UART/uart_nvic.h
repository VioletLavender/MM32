/**
******************************************************************************
* @file    uart_nvic.h
* @author  AE Team
* @version V1.1.0
* @date    28/08/2019
* @brief   This file contains all the uart_nvic inc file for the library.
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
#ifndef __UART_NVIC_H
#define __UART_NVIC_H
#include "stdio.h"
#include "sys.h"
//开发板
//串口1初始化
#define UART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_UART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  UART_RX_BUF[UART_REC_LEN]; //接收缓冲,最大UART_REC_LEN个字节.末字节为换行符
extern u16 UART_RX_STA;         		//接收状态标记
extern u8  UART_IDLE_start;
//如果想串口中断接收，请不要注释以下宏定义
void uart_nvic_init(u32 bound);
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

