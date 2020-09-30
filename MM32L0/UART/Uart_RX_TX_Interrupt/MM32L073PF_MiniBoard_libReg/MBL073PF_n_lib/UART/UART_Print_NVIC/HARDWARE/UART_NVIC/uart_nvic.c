#include "uart_nvic.h"
void uart_send_dates(u8 *date, u8 len);

//串口1中断服务程序
u8 UART_RX_BUF[UART_REC_LEN];     //接收缓冲,最大UART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 UART_RX_STA=0;       //接收状态标记	  

void uart_nvic_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);	//使能UART1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //开启GPIOA时钟
    
    //UART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    //UART 初始化设置
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
    
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
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))      //表示数组a中元素的个数

uint8_t TxBuffer1[] = "USART Interrupt Example: This is USART1 DEMO";
uint8_t RxBuffer1[RxBufferSize1],rec_f;
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00;
uint8_t NbrOfDataToTransfer1 = TxBufferSize1;
uint8_t NbrOfDataToRead1 = RxBufferSize1;
void UART1_IRQHandler(void)                	//串口1中断服务程序
	{
    u8 Res;
    if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)  //?????ж?(????????????????0x0d 0x0a??β)
    {
        UART_ClearITPendingBit(UART1,UART_IT_RXIEN);
        Res =UART_ReceiveData(UART1);	//??????????????
        if((Res==0x0d)&&((UART_RX_STA&0X3FFF)>0))
        {
            UART_RX_STA|=0x4000;
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
        }
        else if((UART_RX_STA&0x4000)&&((UART_RX_STA&0X3FFF)>0))//???????0x0d
        {
            if(Res==0x0a)
            {
                UART_RX_STA|=0x8000;
            }
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
        }
        else{
            UART_RX_BUF[UART_RX_STA&0X3FFF]=Res ;
            UART_RX_STA++;
            UART_RX_STA=UART_RX_STA&0X3FFF;
            if((UART_RX_STA)>(UART_REC_LEN-1))
                UART_RX_STA=0;//???????????,??????????
        }
	//TxBuffer1[UART_RX_STA]  = UART_RX_BUF[UART_RX_STA];
     }
if(UART_GetITStatus(UART1, UART_IT_TXIEN)  != RESET)
  {
	uart_send_dates(UART_RX_BUF,UART_RX_STA);
	UART_ClearITPendingBit(UART1,UART_IT_TXIEN);
	UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE); //关闭发送中断
  }
}

void uart_send_dates(u8 *date, u8 len){
			u16 t=1;
			for(t=1;t<len;t++)
			{
				UART_SendData(UART1, UART_RX_BUF[t]);//向串口1发送数据
				while(UART_GetFlagStatus(UART1,UART_IT_TXIEN)!=SET);//等待发送结束
			}
}


