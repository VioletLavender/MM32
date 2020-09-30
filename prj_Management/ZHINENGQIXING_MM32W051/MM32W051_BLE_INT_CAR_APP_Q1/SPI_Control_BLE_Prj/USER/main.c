#include "sys.h"
#include "mg_api.h"
#include "delay.h"
#include "app.h"
#include "rcc.h"
#include "callback.h"
#include "uart.h"
#include "spi.h"
#include "iic.h"
#include "irq_rf.h"
#include "led_mesh.h"
//#include "usb.h"

unsigned char *ble_mac_addr;
extern unsigned char SleepStop;
extern void IrqMcuGotoSleepAndWakeup(void)  ;

unsigned char *get_local_addr(void) //used for ble pairing case
{
  return ble_mac_addr;
}
GPIO_InitTypeDef  GPIO_InitStructure;

int main(void)
{
  unsigned long temp = 0x800000;
  u8 value_t[2];
  SystemClk_HSEInit();
  PWM_Init();

#ifdef USE_UART
#ifdef USE_AT_CMD
  SleepStop = 0x00;
#endif
#endif

#ifdef USE_UART
  uart_initwBaudRate();
#endif

#ifdef USE_I2C
  IIC_Init(I2C1);
#endif

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_5;   //i2c1_scl  pb6  i2c1_sda  pb7
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 复用开漏输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef USE_USB
  usb_test();
#endif

  SysTick_Configuration();
  SPIM_Init(SPI2,/*0x06*/0x06); //6Mhz
  IRQ_RF();
	BlueTooth_OnOff();
  while (temp--);
  SetBleIntRunningMode();
  radio_initBle(TXPWR_0DBM, &ble_mac_addr);
  printf("\r\nMAC:%02x-%02x-%02x-%02x-%02x-%02x\r\n", ble_mac_addr[5], ble_mac_addr[4], ble_mac_addr[3], ble_mac_addr[2], ble_mac_addr[1], ble_mac_addr[0]);
  
	CONNECT_STATUS_SHOW();
	NAME_SET();
  value_t[0] = 0xc0;
  value_t[1] = *(u8 *)0x1FFFF820; //读取FT值
  printf("\r\nREG FT Value:%x %x\r\n", value_t[0], value_t[1]);//串口打印判断此处是否含有FT的值

//  mg_activate(0x53);//如果没有FT的数值开启此处
//  mg_writeBuf(0x4, value_t, 2); //写入FT值
//  mg_activate(0x56);

  ble_run_interrupt_start(160 * 2); //320*0.625=200 ms
  while (1)
  {

  }
}

