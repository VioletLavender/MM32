#ifndef __UART_NVIC_H
#define __UART_NVIC_H
#include "HAL_conf.h"
#include  "stdio.h"
#include "dtype.h"
//串口1初始化		   
#define UART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_UART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  UART_RX_BUF[UART_REC_LEN]; //接收缓冲,最大UART_REC_LEN个字节.末字节为换行符 
extern u16 UART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_nvic_init(u32 bound);
void uart_send_dates(u8 *date, u8 len);
#endif


