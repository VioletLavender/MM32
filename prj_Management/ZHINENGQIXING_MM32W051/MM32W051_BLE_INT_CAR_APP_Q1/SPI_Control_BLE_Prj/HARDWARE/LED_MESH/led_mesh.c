#include "led_mesh.h"

void PWM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //TIM3_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //TIM3_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 255 * 100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值   1K
  TIM_TimeBaseStructure.TIM_Prescaler = 2; //设置用来作为TIMx时钟频率除数的预分频值  480分频 100k
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能

  TIM_ARRPreloadConfig(TIM2, ENABLE); //使能TIMx在ARR上的预装载寄存器

  TIM_Cmd(TIM2, ENABLE);  //使能TIM1
  GPIOA->AFRL &= 0xFFF000F;
  GPIOA->AFRL |= 0x0002220;

  TIM_TimeBaseStructure.TIM_Period = 255 * 100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值   1K
  TIM_TimeBaseStructure.TIM_Prescaler = 2; //设置用来作为TIMx时钟频率除数的预分频值  480分频 100k
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1预装载使能
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH1预装载使能

  TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
  TIM_Cmd(TIM3, ENABLE);  //使能TIM1
  GPIOB->AFRL &= 0xF00FFFF;
  GPIOB->AFRL |= 0x0110000;
}

void UpdateLEDValueAll(void) //porting function
{
  int t;
  unsigned char data[7];
  unsigned char EnableLED_Flag = 0;
  unsigned int Led_Lum_percent = 100;

  Led_getInfo(data);
  EnableLED_Flag = data[0];
  if (EnableLED_Flag == 0)
  {
    TIM_SetCompare2(TIM2, 0); //G
    TIM_SetCompare3(TIM2, 0); //B
    TIM_SetCompare4(TIM2, 0); //R
    TIM_SetCompare1(TIM3, 0); //Y
    TIM_SetCompare2(TIM3, 0); //W
  }
  else
  {
    Led_Lum_percent = data[6];
    t = data[1] * Led_Lum_percent;
    TIM_SetCompare2(TIM3, t); //Rx100
    t = data[2] * Led_Lum_percent;
    TIM_SetCompare1(TIM3, t); //Gx100
    t = data[3] * Led_Lum_percent;
    TIM_SetCompare2(TIM2, t); //Bx100
    t = data[4] * Led_Lum_percent;
    TIM_SetCompare3(TIM2, t); //Yx100
    t = data[5] * Led_Lum_percent;
    TIM_SetCompare4(TIM2, t); //Wx100
  }

}