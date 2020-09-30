#include "BSP.h"
#include "mg_api.h"
#define APPLICATION_ADDRESS     (uint32_t)0x8001010
#define FLASH_E2PROM_ADDR_BASE (0x08000000)
#define FLASH_BOOT_ROM_SIZE    (4*1024)
#define FLASH_E2PROM_ADDR_USR  (FLASH_E2PROM_ADDR_BASE + 4*1024)
#define FLASH_E2PROM_ADDR_OTA  (FLASH_E2PROM_ADDR_BASE + 64*1024)
#define FLASH_E2PROM_ADDR_OTA_KB (32)


unsigned int RxTimeout = 0;
unsigned int TxTimeout = 0;
extern unsigned int SysTick_Count;
extern unsigned int RxTimeout;
unsigned char SleepStop = 0x00; //1-Sleep, 2-Stop
unsigned char SleepStatus = 0;
void CodeNvcRemap(void);
static void delay_us(__IO uint32_t nTime)
{		
    u16 i=0;   
    while(nTime--)   
    {   
        i=10;  //自己定义      
        while(i--);       
    }					 
}
/********************************************************************************************************
**函数信息 ：SPIM_TXEn(SPI_TypeDef* SPIx)
**功能描述 :关闭 SPI 在双向模式下的数据传输方向
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXEn(SPI_TypeDef* SPIx)
{
    //Transmit Enable bit TXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_TXDisable(SPI_TypeDef* SPIx)
**功能描述 :关闭 SPI 在双向模式下的数据传输方向
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_TXDisable(SPI_TypeDef* SPIx)
{
    //disable TXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Tx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXEn(SPI_TypeDef* SPIx)
**功能描述 :关闭 SPI 在双向模式下的数据传输方向
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXEn(SPI_TypeDef* SPIx)
{
    //enable RXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
}

/********************************************************************************************************
**函数信息 ：SPIM_RXDisable(SPI_TypeDef* SPIx)
**功能描述 :关闭 SPI 在双向模式下的数据传输方向
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2
**输出参数 ：无
********************************************************************************************************/
void SPIM_RXDisable(SPI_TypeDef* SPIx)
{
    //disable RXEN
    SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Rx);
}

/********************************************************************************************************
**函数信息 ：SPIM_Init(SPI_TypeDef* SPIx, unsigned short spi_baud_div)
**功能描述 :可修改参数初始化SPI
**输入参数 ：SPI_TypeDef* SPIx,可选择SPI1,SPI2  ;spi_baud_div
**输出参数 ：无
********************************************************************************************************/
void SPIM_Init(SPI_TypeDef* SPIx, unsigned short spi_baud_div)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef        GPIO_InitStructure;

    if(SPIx == SPI2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

        SPI_CS_Disable;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

        GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_4); //MISO
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_4); //MOSI
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_3); //SCK
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_1); //CSN


        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;   		//spi2_cs  pb15
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	// 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_SetBits(GPIOB, GPIO_Pin_15); //csn=H

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;   		//spi2_sck  pb14
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;   		//spi2_mosi  pb13
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 推免复用输出
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;  		//spi2_miso  pb12
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		//上拉输入
        GPIO_Init(GPIOB, &GPIO_InitStructure);

    }

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;    // mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  // mode3 SPI_CPOL_High,SPI_CPHA_2Edge
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_BaudRatePrescaler = spi_baud_div;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE); //Enables the specified SPI peripheral SPI使能、主机模式 8位数据模式 SPI 的波特率

    SPIM_TXEn(SPIx);
    SPIM_RXEn(SPIx);
}

/********************************************************************************************************
**函数信息 ：SystemClk_HSEInit (void)
**功能描述 ：系统时钟初始化函数，初始化之前先复位所有时钟
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
#if 1 //using internal RC as clock ref
void SetSysClock_HSI(u8 PLL)
{
    unsigned char temp = 0;

    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR = RCC_CFGR_PPRE1_1; //APB1=DIV2;APB2=DIV1;AHB=DIV1;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;	 //PLLSRC ON

    RCC->CR &= ~(RCC_CR_PLLON);		//clear PLL//	RCC->CR &=~(7<<20);		//clear PLL

    RCC->CR &= ~(0x1f << 26);
    RCC->CR |= (PLL - 1) << 26; //setting PLL value 2~16

    FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE;	 //FLASH 2 delay clk cycles

    RCC->CR |= RCC_CR_PLLON; //PLLON
    while(!(RCC->CR & RCC_CR_PLLRDY)); //waiting for PLL locked
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL; //PLL to be the sys clock
    while(temp != 0x02) { //waiting PLL become the sys clock
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}

void SystemClk_HSEInit(void)
{
    SetSysClock_HSI(4);//HSI:12*4=48M

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//selecting PLL clock as sys clock
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
}

//////DO NOT REMOVE, used in ble lib///////
void SysClk8to48(void)
{
    SystemClk_HSEInit();
    SysTick_Config(48000);
}
void SysClk48to8(void)
{
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//selecting PLL clock as sys clock

    while (RCC_GetSYSCLKSource() != 0x0)
    {}

    RCC->CR &= ~(RCC_CR_PLLON);		//clear PLL
    SysTick_Config(8000);
}
#endif

void SysTick_Configuration(void)
{
    SysTick_Config(48000);
}

void LED_ONOFF(unsigned char onFlag)//module indicator,GPA8
{
    if(onFlag) {
        GPIOA->BRR = GPIO_Pin_8;  //low, on
    } else {
        GPIOA->BSRR = GPIO_Pin_8; //high, off
    }
}
void LED_ONOFFPB9(unsigned char onFlag)//module indicator,GPA8
{
    if(onFlag) {
        GPIOB->BRR = GPIO_Pin_9;  //low, on
    } else {
        GPIOB->BSRR = GPIO_Pin_9; //high, off
    }
}
void LED_ONOFFPB1(unsigned char onFlag)//module indicator,GPA8
{
    if(onFlag) {
        GPIOB->BRR = GPIO_Pin_1;  //low, on
    } else {
        GPIOB->BSRR = GPIO_Pin_1; //high, off
    }
}
//void Sys_Standby(void)
//{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
//    //	RCC->APB2RSTR|=0X01FC;//复位所有IO口
//	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
//	PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式
//
//}
void BSP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

#ifdef OTA_SUPPORT
    CodeNvcRemap();
#endif

    SystemClk_HSEInit();
    //PWM_Init();
    SysTick_Configuration();

    //SPIM_Init(SPI1,0x08); //6Mhz
    SPIM_Init(SPI2,/*0x06*/0x08); //6Mhz
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    //IRQ - pb8
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /*	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
        GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//下拉输入
        GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    	*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO


    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_5);
    //module led indicator GPA8
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    LED_ONOFF(1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    //SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
    //SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
    SYSCFG_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);

    EXTI_InitStructure.EXTI_Line = /*EXTI_Line10|*/EXTI_Line0 | EXTI_Line11/*|EXTI_Line12*/;
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    PWR->CR = PWR->CR & 0xfffd; //PDDS = 0;enter stop mode
    if (1 == SleepStop) {
        SCB->SCR &= 0xFB;
    } else if(2 == SleepStop) {
        SCB->SCR |= 0x4;
    }
}

/////////////////////Following functions are porting functions/////////////////////////////////

unsigned char SPI_WriteRead(unsigned char SendData, unsigned char WriteFlag) //porting api
{
    SPI2->TXREG = SendData;
    while(!(SPI2->CSTAT & SPI_FLAG_TXEPT));
    while(!(SPI2->CSTAT & SPI_FLAG_RXAVL));
    return SPI2->RXREG;
}

void SPI_CS_Enable_(void) //porting api
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void SPI_CS_Disable_(void) //porting api
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
//    if(1 == SleepStop) { //sleep
//        SCB->SCR &= 0xfb;
//        __WFE();

//    } else if(2 == SleepStop) { //stop
//        SysClk48to8();
//        SCB->SCR |= 0x4;
//        __WFI();

//        RCC->CR |= RCC_CR_HSION;
//        SysClk8to48();
////            RCC->CR|=RCC_CR_HSION;
////            RCC->CR |= RCC_CR_PLLON;
////            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
//        GPIO_ResetBits(GPIOB, GPIO_Pin_7);
//    }
//	 if (1 == SleepStop){ //sleep
//        SysClk48to8();
//        //SCB->SCR &= 0xFB;
//        __WFE();
//        
//        SysClk8to48();
//    }
//    else if(2 == SleepStop){
//        SysClk48to8();
//        //SCB->SCR |= 0x4;
//        __WFI();
//        
//        RCC->CR|=RCC_CR_HSION;
//        RCC->CR |= RCC_CR_PLLON;
//        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
//        SysTick_Config(48000);
//    }
}
void SysClk8M(void)
{
    __ASM volatile("cpsid i");
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//selecting PLL clock as sys clock
    
    while (RCC_GetSYSCLKSource() != 0x0)
    {}

    RCC->CR &=~(RCC_CR_PLLON);  //clear PLL
    SysTick_Config(8000);
    __ASM volatile("cpsie i");
}

void IrqMcuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
    if(ble_run_interrupt_McuCanSleep() == 0) return;


//    if ((SleepStop)&&
//        (TxTimeout < SysTick_Count)&&
//        (RxTimeout < SysTick_Count))
//    {
//        if(SleepStop == 1){//sleep
//            SleepStatus = 1;
//            SCB->SCR &= 0xfb;
//            __WFE();

//        }else{ //stop
//            SleepStatus = 2;
//            SysClk8M();
//            SCB->SCR |= 0x4;
//            
//            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//            RCC->CR |=  0X1<<19;
//            RCC->CR |=  0X1<<19;
//            RCC->CR |=  0X1<<19;
//            delay_us(5);
//            
//            __WFI();
//        }
//    }
}
static char dis_int_count = 0;
void DisableEnvINT(void)
{
    //to disable int
    __ASM volatile("cpsid i");

    dis_int_count ++;
}

void EnableEnvINT(void)
{
    //to enable/recover int
    dis_int_count --;
    if(dis_int_count <= 0) { //protection purpose
        dis_int_count = 0; //reset
        __ASM volatile("cpsie i");
    }
}


char IsIrqEnabled(void) //porting api
{
    return (!(GPIO_ReadInputData(GPIOB) & 0x100));
}







void CodeNvcRemap(void)
{
    uint32_t i = 0;
    for(i = 0; i < 48; i++) {
        *((uint32_t*)(0x20000000 +  (i << 2))) = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i << 2));
    }
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    SYSCFG->CFGR |= 0x03;
}


u32 GetOtaAddr(void)
{
    return FLASH_E2PROM_ADDR_OTA;
}

u32 GetCodeAddr(void)
{
    return (FLASH_E2PROM_ADDR_BASE + FLASH_BOOT_ROM_SIZE);
}

void WriteFlashE2PROM(u8* data, u16 len, u32 pos, u8 flag) //4 bytes aligned
{
    u32 t;

    if(flag)FLASH_Unlock();
    while(len >= 4) {
        t = data[3]; t <<= 8;
        t |= data[2]; t <<= 8;
        t |= data[1]; t <<= 8;
        t |= data[0];
        FLASH_ProgramWord(pos, t);
        pos += 4;
        len -= 4;
        data += 4;
    }

    if(flag)FLASH_Lock();
}

void OtaSystemReboot(void)//porting api
{
    NVIC_SystemReset();
}

//api provide in blelib
void UpdateLEDValueAll(void) //porting function
{
}
