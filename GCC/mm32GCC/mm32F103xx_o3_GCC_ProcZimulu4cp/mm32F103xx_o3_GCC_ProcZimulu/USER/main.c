#include "HAL_device.h"
#include "HAL_gpio.h"
#include "HAL_rcc.h"
void LED_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

static void delay_us(u32 uLdelay)
{
    while (uLdelay--)
    {
        __NOP();

    }
}
static void delay_ms(u32 uLdelay)
{
    while (uLdelay--)
    {
        delay_us(1000);
    }
}
int main(void)
{

    LED_Init();
    while (1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);
        delay_ms(300);
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
        delay_ms(300);
    }
}

