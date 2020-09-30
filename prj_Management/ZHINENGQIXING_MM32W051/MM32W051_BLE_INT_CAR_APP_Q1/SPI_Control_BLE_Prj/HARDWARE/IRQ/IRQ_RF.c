#include "irq_rf.h"
#include "mg_api.h"
#include "delay.h"
char BlueTooth_On_Off=0;
unsigned char pld_adv[] =
{
  0x02,  0x01, 0x06,
  0x09,  0xFF, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xA7, 0xF9,
  0x05,  0x03, 0xE7, 0xFE, 0xE0, 0xFE,
  0x06,  0x09, 0x4C, 0x53, 0x4C, 0x45, 0x44,
};

/********************************************************************************************************
**函数信息 ：LED_Init(void)
**功能描述 ：LED初始化
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void IRQ_RF(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  //IRQ - pb1
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  PWR->CR = PWR->CR & 0xfffd; //PDDS = 0;enter stop mode
  SCB->SCR |= 0x4;
}

char IsIrqEnabled(void) //porting api
{
  return (!(GPIO_ReadInputData(GPIOB) & 0x02));
}

void EXTI0_1_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line1);
  ble_run(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BlueTooth_OnOff()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);	//GPIOA时钟使能
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//GPIO——init
	
	  SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15);

  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);//

  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI4_15_IRQHandler()
{
		if(EXTI_GetITStatus(EXTI_Line15) != RESET)	//检测是否触发对应的中断
	{
		delay_ms(100);
		if(BlueTooth_On_Off==0)
		{
		ble_disconnect();
		}
		BlueTooth_On_Off=~BlueTooth_On_Off;

		ble_set_adv_enableFlag(~BlueTooth_On_Off);
	}
	EXTI_ClearITPendingBit(EXTI_Line15);//清除中断标志

}
