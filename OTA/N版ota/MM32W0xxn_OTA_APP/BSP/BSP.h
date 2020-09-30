#ifndef _BSP_H_
#define _BSP_H_

#include "HAL_conf.h"
#include "spi.h"

#define SPI_CS_Enable	SPI2->SCSR &= SPI_CS_BIT0
#define SPI_CS_Disable	SPI2->SCSR |= ~SPI_CS_BIT0

void BSP_Init(void);
void UartInit(UART_TypeDef* UARTx);
void IIC_Init(I2C_TypeDef* I2Cx);
void SysClk8to48(void);
void SysClk48to8(void);

unsigned char SPI_WriteRead(unsigned char SendData, unsigned char WriteFlag);
unsigned GetIRQSta(void);

//void EnableLED(char Enable);
//void SetLEDLum(int r, int g, int b, int L/*101 means not used*/);//[0~255]
//void SetLEDLumPercent(int percent);
//void LED_ONOFF(unsigned char OnOff);//for ui use

//void SetBaudRate(unsigned long bps);
void ChangeBaudRate(void);


#endif
