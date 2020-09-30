#include "uart.h"
#define MAX_SIZE 100
u32 BaudRate = 9600/*115200*/; //115200;
extern u8 txBuf[MAX_SIZE], rxBuf[MAX_SIZE], txLen, PosW;
extern u16 RxCont;
extern unsigned int RxTimeout, TxTimeout, SysTick_Count;

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif

#ifdef USE_IAR
PUTCHAR_PROTOTYPE
{
  while ((UART2->CSR & UART_IT_TXIEN) == 0); //循环发送,直到发送完毕
  UART2->TDR = (ch & (uint16_t)0x00FF);
  return ch;
}

#else
#pragma import(__use_no_semihosting)
struct __FILE//标准库需要的支持函数
{
  int handle;
};

FILE __stdout;

void _sys_exit(int x)//定义_sys_exit()以避免使用半主机模式
{
  x = x;
}

int fputc(int ch, FILE *f)//重定义fputc函数
{
  while ((UART2->CSR & UART_IT_TXIEN) == 0); //循环发送,直到发送完毕
  UART2->TDR = (ch & (uint16_t)0x00FF);
  return ch;
}

#endif

void uart_initwBaudRate(void)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  UART_InitTypeDef UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE); //使能UART2，GPIOA时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  //UART2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3 ; //抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

  //UART 初始化设置
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

  UART_InitStructure.UART_BaudRate = BaudRate;//串口波特率
  UART_InitStructure.UART_WordLength = UART_WordLength_8b;//字长为8位数据格式
  UART_InitStructure.UART_StopBits = UART_StopBits_1;//一个停止位
  UART_InitStructure.UART_Parity = UART_Parity_No;//无奇偶校验位
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//无硬件数据流控制
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx; //收发模式

  UART_Init(UART2, &UART_InitStructure); //初始化串口1
  UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);//开启串口接受中断
  UART_ITConfig(UART2, UART_IT_TXIEN, DISABLE);
  UART_Cmd(UART2, ENABLE);                    //使能串口1

  //UART2_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

  //UART2_RX    GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_3);

  //RTS
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //CTS
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}

void ChangeBaudRate(void)
{
  uint32_t apbclock = 0x00;
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  UART_Cmd(UART2, DISABLE);
  apbclock = RCC_ClocksStatus.PCLK1_Frequency;
  UART2->BRR = (apbclock / BaudRate) / 16;
  UART2->FRA = (apbclock / BaudRate) % 16;
  UART_Cmd(UART1, ENABLE);
}

void moduleOutData(u8 *data, u8 len) //api
{
  unsigned char i;
  if ((txLen + len) < MAX_SIZE) //buff not overflow
  {
    for (i = 0; i < len; i++)
    {
      txBuf[txLen + i] = *(data + i);
    }
    txLen += len;
  }
}

void UART2_IRQHandler(void)                 //串口1中断服务程序
{
  if (UART_GetITStatus(UART2, UART_IT_RXIEN)  != RESET) //接收中断
  {
    UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
    rxBuf[RxCont] = UART_ReceiveData(UART2);
    RxTimeout = SysTick_Count + 1000;
    RxCont++;
    if (RxCont >= MAX_SIZE)
    {
      RxCont = 0;
    }
  }
  if (UART_GetITStatus(UART2, UART_IT_TXIEN)  != RESET)
  {
    UART_ClearITPendingBit(UART2, UART_IT_TXIEN);
    TxTimeout = SysTick_Count + (20000 / BaudRate);
    if (PosW < txLen)
    {
      UART_SendData(UART2, txBuf[PosW++]);
      if (PosW == txLen)
      {
        txLen = 0;
        PosW = 0;
      }
    }
    else
    {
      UART_ITConfig(UART2, UART_IT_TXIEN, DISABLE);
    }
  }
}




