#include "HAL_conf.h"
#include "HAL_device.h"
#include "common.h"
unsigned char UpdateFlag __attribute__((at(0x801FFFC)));//变量的绝对定位
/* 变量 ----------------------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
/* 函数声明 ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void LED_Configuration(void);
static void IAP_Init(void);
void KEY_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
/* 函数功能 ------------------------------------------------------------------*/

/*******************************************************************************
  * @函数名称   main
  * @函数说明   主函数
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/

int main(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);//重新设置中断向量表//0x0000为向量表的偏移地址
    FLASH_Unlock(); //Flash 解锁
    IAP_Init();//串口初始化操作
    //检测到更新标记
    if (!UpdateFlag)
    {
        //执行IAP驱动程序更新Flash程序
        Main_Menu();
    }
    //否则执行用户程序
    else
    {
        //判断用户是否已经下载程序，因为正常情况下此地址是栈地址。
        //若没有这一句的话，即使没有下载程序也会进入而导致跑飞。
        if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000) == 0x20000000)//判断是否存在数据也可以写成(((*(__IO uint32_t *)ApplicationAddress) ！=0xffffffff)
        //if (!((*(__IO uint32_t *)ApplicationAddress) ==0xFFFFFFFF))
			{
				SerialPutString("Execute user Program11111\r\n");
            SerialPutString("Execute user Program\r\n");
            //跳转至用户代码
            JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);
            Jump_To_Application = (pFunction)JumpAddress;
						__disable_irq();
						UART_ITConfig(UART1 ,UART_IT_RXIEN,DISABLE),
            //初始化用户程序的堆栈指针
            __set_MSP(*(__IO uint32_t *)ApplicationAddress);

            Jump_To_Application();
        }
        else
        {
            SerialPutString("no user Program\r\n");
            Main_Menu();
        }
    }

    while (1)
    {
    }
}

/*******************************************************************************
  * @函数名称   GPIO_Configuration
  * @函数说明   配置使用USART1的相关IO管脚
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*复用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);

    /*UART1_RX  GPIOA.10初始化*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    /*浮空输入*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);
}

/*******************************************************************************
  * @函数名称   IAP_Init
  * @函数说明   配置使用IAP
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void IAP_Init(void)
{
    UART_InitTypeDef UART_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    /* USART1 配置 ------------------------------------------------------------
           USART1 配置如下:
            - 波特率      = 115200 baud
            - 字长        = 8 Bits
            - 一个停止位
            - 无校验
            - 无硬件流控制
            - 接受和发送使能
      --------------------------------------------------------------------------*/
    UART_InitStructure.UART_BaudRate = 19200;
    /*字长为8位数据格式*/
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    /*一个停止位*/
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    /*无奇偶校验位*/
    UART_InitStructure.UART_Parity = UART_Parity_No;
    /*无硬件数据流控制*/
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    /*收发模式*/
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    /*初始化串口1*/
    UART_Init(UART1, &UART_InitStructure);
    /*开启串口接受中断*/
    /*使能串口1*/
    GPIO_Configuration();
    // 使能 USART3
    UART_Cmd(UART1, ENABLE);
}

/*******************************************************************************
  * @函数名称   Delay
  * @函数说明   插入一段延时时间
  * @输入参数   nCount: 指定延时时间长度
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--)
        ;
}



void UARTx_WriteByte(UART_TypeDef *UARTx,uint8_t Data)
{ 
    /* send a character to the UART */
    UART_SendData(UARTx, Data);

     /* Loop until the end of transmission */
    while(UART_GetFlagStatus(UARTx, UART_FLAG_TXEPT) == RESET);
}


#ifdef USE_FULL_ASSERT

/*******************************************************************************
  * @函数名称   assert_failed
  * @函数说明   报告在检查参数发生错误时的源文件名和错误行数
  * @输入参数   file: 源文件名
                line: 错误所在行数
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    /* 用户可以增加自己的代码用于报告错误的文件名和所在行数,
         例如：printf("错误参数值: 文件名 %s 在 %d行\r\n", file, line) */

    //死循环
    while (1)
    {
    }
}
#endif


