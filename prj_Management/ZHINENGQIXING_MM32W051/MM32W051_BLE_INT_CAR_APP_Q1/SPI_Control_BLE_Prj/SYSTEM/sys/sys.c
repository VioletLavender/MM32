#include "sys.h"
#include "mg_api.h"

unsigned int SysTick_Count = 0;

unsigned int GetSysTickCount(void) //porting api
{
  return SysTick_Count;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
*/
void SysTick_Handler(void)
{
  SysTick_Count ++;
  ble_nMsRoutine();
}

void SysTick_Configuration(void)
{
  SysTick_Config(48000);
}



