#include "delay.h"
#include "sys.h"
#include "uart_nvic.h"
#include "led.h"

int main(void)
{
    u8 t;
    u8 len;	
    u16 times=0; 
    
    delay_init();
    LED_Init();
    uart_nvic_init(115200);	 //串口初始化为115200
    
    while(1)
    {
			if(UART_RX_STA&0x8000)
			{
				UART_SendData(UART1, UART_RX_BUF[0]);//向串口1发送数据
				UART_ITConfig(UART1, UART_IT_TXIEN, ENABLE);
				UART_RX_STA=0;
			}
    }	 
}




