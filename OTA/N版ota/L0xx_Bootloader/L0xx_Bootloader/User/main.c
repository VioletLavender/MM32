/******************** (C) COPYRIGHT 2009 MM32 ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Custom HID demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, MM32 SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "MM32L073PF.h"
#include "HAL_gpio.h"
#include "boot.h"

#include "HAL_syscfg.h"
#define ApplicationAddress APP_ENTRY_ADDRESS//0x08002000	//APP Address

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
/******************************************************************************
// MM32L073PF 128K flash
Bootloader 0x08000000 -- 0x08001BFF  total 7K byte flash
Bootloader Flag  0x08001C00  need 1K
Application  0x08002000 -- 0x08020000  total 120K byte flash

*******************************************************************************/

#define VECTOR_SIZE  0xC0   //48*4=192 = 0xC0

//extern uint8_t Flag_Receive;

//uint16_t DFU_state;
typedef  void (*pFunction)(void);

pFunction Jump_To_Application;
uint32_t JumpAddress;

extern u32 SystemCoreClock;

//uint8_t  Boot_value __attribute__((section(".ARM.__at_0x20000000"))) = 0x00;   //在RAM定义一个Flag

/*******************************************************************************
* @name   : LED_Init
* @brief  : Init LED
* @param  : void
* @retval : void
*******************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    LED1_OFF();
    LED2_OFF();
    LED3_OFF();
    LED4_OFF();
}
static void delay(u32 u32delay)
{
    u32 i,j;
    for(i=0;i<u32delay;i++)
    {
        for(j=0;j<10000;j++)
        {
            __NOP();
        }
    }
}
/********************************************************************************************************
    LED_SetOFF(void)                        
********************************************************************************************************/
void LED_SetOFF(void)
{

    LED3_OFF();
    LED4_OFF();
}
void LED_SetON(void)
{

    LED3_ON();
    LED4_ON();
}
void ledtest(u32 i)
{
    LED_Init();
    if(i==0)
        return;
    while(i--){
        LED_SetOFF();
        delay(100);
        LED_SetON();
        delay(100);
    }
}
int main(void)
{
    ledtest(10);
			/* Test if user code is programmed starting from address "ApplicationAddress" */
			
//			if(((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)	//检查栈顶地址是否合法
			{
				/* Jump to user application */ 
				JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
				Jump_To_Application = (pFunction) JumpAddress;

				/* Initialize user application's Stack Pointer */ 
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);
				
				//Jump to APP
				Jump_To_Application();	
			}	

	
  while (1)
  {
}	
	
}


/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
//void Delay(__IO uint32_t nCount)
//{
//  for(; nCount!= 0;nCount--);
//}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while(1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2009 MM32 *****END OF FILE****/
