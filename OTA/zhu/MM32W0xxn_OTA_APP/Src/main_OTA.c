#include <string.h>
#include <stdlib.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "mg_api.h"
void LED_ONOFF(unsigned char onFlag);
void LED_ONOFFPB1(unsigned char onFlag);
void LED_ONOFFPB9(unsigned char onFlag);

extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}
const unsigned char kkkk[28000] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0x90, 0xfe
};

const unsigned char OTAAdvDat[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0x90, 0xfe
};
unsigned char OTARspDat[] = {
    0x4, 0x09, 'O', 'T', 'B',
    0x05, 0xff, 0xcd, 0xab, 0x23, 0x56
};
static void delayloop(u32 delay);


/********************************************************************************************************
**函数信息 ：PVU_CheckStatus(void)                       
**功能描述 ：检查独立看门狗预分频位状态
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void PVU_CheckStatus(void)
{
    while(1)
    {
        /*检查预分频位状态,为RESET才可改变预分频值*/
        if(IWDG_GetFlagStatus(IWDG_FLAG_PVU)==RESET)                                                       
        {
            break;
        }
    }
}
/********************************************************************************************************
**函数信息 ：RVU_CheckStatus(void)                  
**功能描述 ：检查独立看门狗重载标志
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
void RVU_CheckStatus(void)
{
    while(1)
    {
        /*检查重载标志状态*/
        if(IWDG_GetFlagStatus(IWDG_FLAG_RVU)==RESET)  
        {
            break;
        }
    }
}
/********************************************************************************************************
**函数信息 ：Write_Iwdg_PR(void)             
**功能描述 ：启动独立看门狗
**输入参数 ：IWDG_Prescaler 可选IWDG_Prescaler_X, X为4,8,16,32,64,128,256,对应分频值与X取值相同
Reload<=0xfff,为计数器重载值
**输出参数 ：无
**    备注 ：复位时常计算已LSI 40KHz为参考
Tiwdg=(X/LSI)*Reload
********************************************************************************************************/
void Write_Iwdg_ON(unsigned short int IWDG_Prescaler,unsigned short int Reload)
{
    /*启动内部低速时钟,等待时钟就绪*/
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
    
    /*设置时钟预分频*/	
    PVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetPrescaler(IWDG_Prescaler);
    
    /*设置重载寄存器值*/	
    RVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetReload(Reload&0xfff);	
    
    /*装载并使能计数器*/	
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/********************************************************************************************************
**函数信息 ：void Write_Iwdg_RL(void)           
**功能描述 ：喂狗函数
**输入参数 ：
**输出参数 ：无
********************************************************************************************************/
void Write_Iwdg_RL(void)
{
    IWDG_ReloadCounter();
}

int main(void)
{
    int i;
    unsigned long temp = 0x180000;
    if(kkkk[19000]==0)
    {
        temp = 0x180000;
    }
    else{
        temp = 0x180001;
    }
    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.

    BSP_Init();
    //below Blink code can be deleted
    for(i = 0; i < 23000; i++)
    {
        temp+=kkkk[i];
    }
        if(temp==0)
    {
        for(i = 0; i < 5; i++) { LED_ONOFFPB9(0); delayloop(300); LED_ONOFFPB9(1); delayloop(300);}
    }
    else{
        for(i = 0; i < 6; i++) { LED_ONOFFPB9(0); delayloop(300); LED_ONOFFPB9(1); delayloop(300);}
    }
 //   Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    //end Blink code
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
//    SetFixAdvChannel(1);
    ble_set_adv_data((unsigned char *)OTAAdvDat, sizeof(OTAAdvDat));
    ble_set_adv_rsp_data((unsigned char *)OTARspDat, sizeof(OTARspDat));


    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1){
       IrqMcuGotoSleepAndWakeup();
        
    }
}
static void delayloop(u32 delay)
{

    u32 i, j;
    for(i = 0; i < delay; i++) {
        for(j = 0; j < 10000; j++) {
            __NOP();
        }
    }
}
