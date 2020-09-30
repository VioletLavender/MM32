#include "HAL_conf.h"
#include "HAL_device.h"
#include "stdio.h"

char printBuf[100];
void UartSendGroup(u8* buf,u16 len);

void GPIO_ConfigIni(void);
void CBC_128_encry(void);
void CBC_192_encry(void);
void CBC_256_encry(void);
void CBC_128_decry(void);
void CBC_192_decry(void);
void CBC_256_decry(void);
void uart_initwBaudRate(u32 bound);


uint8_t key_data_128[16] = {0x65,0x32,0x19,0x8C,0xC6,0xE3,0xF1,0xF8,0x7C,0x3E,0x1F,0x0F,0x87,0x43,0x21,0x10};

uint8_t input_data_128[] = {0x8C,0x46,0xA3,0xD1,0xC1,0x60,0x30,0x18,0x16,0x0B,0x05,0x82,0x6E,0xB7,0x5B,0x2D};
uint8_t cipher_data_128[] = {0xE5,0x1E,0x1B,0x17,0xC4,0x4E,0x5C,0x14,0x62,0xBB,0xFE,0xC0,0xDD,0x60,0xC0,0xDC};

uint8_t key_data_192[] = {0xAD,0xD6,0x6B,0xB5,0x5A,0x2D,0x96,0xCB,0x65,0x32,0x19,0x8C,0xC6,0xE3,0xF1,0xF8,0x7C,
0x3E,0x1F,0x0F,0x87,0x43,0x21,0x10};
uint8_t input_data_192[] = {0x16,0x0B,0x05,0x82,0x6E,0xB7,0x5B,0x2D,0xE7,0x73,0xB9,0xDC,0x73,0x39,0x9C,0xCE};
uint8_t cipher_data_192[] = {0x3F,0x37,0xD1,0x29,0x7B,0x8A,0x34,0x65,0x61,0x68,0xA8,0xD3,0xDF,0xBF,0x18,0x3A};

uint8_t key_data_256[] = {0x68,0xB4,0x5A,0xAD,0xD6,0x6B,0xB5,0x5A,0xAD,0xD6,0x6B,0xB5,0x5A,0x2D,0x96,0xCB,0x65,
0x32,0x19,0x8C,0xC6,0xE3,0xF1,0xF8,0x7C,0x3E,0x1F,0x0F,0x87,0x43,0x21,0x10};
uint8_t input_data_256[] = {0xE7,0x73,0xB9,0xDC,0x73,0x39,0x9C,0xCE,0x3A,0x9D,0xCE,0xE7,0xA2,0xD1,0xE8,0x74};
uint8_t cipher_data_256[] = {0xC7,0x53,0xE0,0x9C,0x9B,0x1B,0x29,0xAD,0x27,0x2A,0x68,0x8D,0xC9,0x65,0x39,0x25};

uint8_t iv_data[16] = {0x68,0xB4,0x5A,0xAD,0xD6,0x6B,0xB5,0x5A,0xAD,0xD6,0x6B,0xB5,0x5A,0x2D,0x96,0xCB};	
uint8_t iv_data_192[16] = {0xC1,0x60,0x30,0x18,0x8C,0x46,0xA3,0xD1,0x68,0xB4,0x5A,0xAD,0xD6,0x6B,0xB5,0x5A};
uint8_t iv_data_256[16] = {0x6E,0xB7,0x5B,0x2D,0x16,0x0B,0x05,0x82,0xC1,0x60,0x30,0x18,0x8C,0x46,0xA3,0xD1};
uint8_t output_data[16] = {0};

/********************************************************************************************************
**函数信息 ：int main (void)                          
**功能描述 ：开机后，ARMLED闪动
**输入参数 ：
**输出参数 ：
********************************************************************************************************/

int main(void)
{    
    /*enable aes*/
    RCC->AHB2ENR |=0x10;  
    uart_initwBaudRate(9600);

    CBC_128_encry();	
    CBC_192_encry();
    CBC_256_encry();

    CBC_128_decry();
    CBC_192_decry();
    CBC_256_decry();

    while(1)           
    {

    }
}


/********************************************************************************************************
**函数信息 ：void CBC_128_decry()     
**功能描述 ：加密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_128_encry()
{
    int i;
    ErrorStatus Status = SUCCESS; 

    AES_KeySize(CRL_AES128_KEY);
    Status = AES_CBC_Encrypt(key_data_128, iv_data, input_data_128,4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 128 encrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=cipher_data_128[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tciper=%x\r\n",output_data[i],cipher_data_128[i]));
        }
    } 
}


/********************************************************************************************************
**函数信息 ：void CBC_192_decry()     
**功能描述 ：加密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_192_encry()
{
    int i;
    ErrorStatus Status = SUCCESS; 

    AES_KeySize(CRL_AES192_KEY);
    Status = AES_CBC_Encrypt(key_data_192, iv_data_192, input_data_192,4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 192 encrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=cipher_data_192[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tciper=%x\r\n",output_data[i],cipher_data_192[i]));
        }
    } 
}


/********************************************************************************************************
**函数信息 ：void CBC_256_decry()     
**功能描述 ：加密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_256_encry()
{
    int i;
    ErrorStatus Status = SUCCESS; 

    AES_KeySize(CRL_AES256_KEY);
    Status = AES_CBC_Encrypt(key_data_256, iv_data_256, input_data_256,4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 256 encrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=cipher_data_256[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tciper=%x\r\n",output_data[i],cipher_data_256[i]));
        }
    }
}


/********************************************************************************************************
**函数信息 ：void CBC_128_decry()     
**功能描述 ：解密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_128_decry()
{
    int i;
    ErrorStatus Status = SUCCESS;
    AES_KeySize(CRL_AES128_KEY);
    Status = AES_CBC_Decrypt(key_data_128, iv_data, cipher_data_128, 4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 128 decrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=input_data_128[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tplain=%x\r\n",output_data[i],input_data_128[i]));
        }
    } 
}


/********************************************************************************************************
**函数信息 ：void CBC_192_decry()     
**功能描述 ：解密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_192_decry()
{
    int i;
    ErrorStatus Status = SUCCESS;

    AES_KeySize(CRL_AES192_KEY);
    Status = AES_CBC_Decrypt(key_data_192, iv_data_192, cipher_data_192, 4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 192 decrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=input_data_192[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tplain=%x\r\n",output_data[i],input_data_192[i]));
        }
    }
}


/********************************************************************************************************
**函数信息 ：void CBC_256_decry()     
**功能描述 ：解密模式
**输入参数 ：
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void CBC_256_decry()
{
    int i;
    ErrorStatus Status = SUCCESS;

    AES_KeySize(CRL_AES256_KEY);
    Status = AES_CBC_Decrypt(key_data_256, iv_data_256, cipher_data_256, 4, output_data);
    if(Status)
    {
        UartSendGroup((u8*)printBuf, sprintf(printBuf,"CBC 256 decrypt OK\r\n")); 
    }
    else
    {
        for(i=0;i<16;i++)
        {
            if(output_data[i]!=input_data_256[i])
                UartSendGroup((u8*)printBuf, sprintf(printBuf,"out=%x\tplain=%x\r\n",output_data[i],input_data_256[i]));
        }
    }
}

/********************************************************************************************************
**函数信息 ：void uart_initwBaudRate(u32 bound)     
**功能描述 ：UART初始化
**输入参数 ：bound
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void uart_initwBaudRate(u32 bound)
{
    /*GPIO端口设置*/
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    /*UART1_TX   GPIOA.9*/
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
    
    /*UART 初始化设置*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);	                   

    /*串口波特率*/
    UART_InitStructure.UART_BaudRate = bound;
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
    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
    /*使能串口1*/    
    UART_Cmd(UART1, ENABLE);                    			                    
}


/********************************************************************************************************
**函数信息 ：void UartSendByte(u8 dat)      
**功能描述 ：UART发送数据
**输入参数 ：dat
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void UartSendByte(u8 dat)
{
    UART_SendData( UART1, dat);
    while(!UART_GetFlagStatus(UART1,UART_FLAG_TXEPT));
}


/********************************************************************************************************
**函数信息 ：void UartSendGroup(u8* buf,u16 len)     
**功能描述 ：UART发送数据
**输入参数 ：buf,len
**输出参数 ：
**    备注 ：
********************************************************************************************************/
void UartSendGroup(u8* buf,u16 len)
{
    while(len--)
        UartSendByte(*buf++);
}

