#include "callback.h"
#include "app.h"
#include "rcc.h"
#include "mg_api.h"
#include "uart.h"
#include "delay.h"
#include "stdio.h"
///////////////FIFO proc for AT cmd///////////////
#define TO_HIGH_CASE
#define comrxbuf_wr_pos RxCont
u16 comrxbuf_rd_pos = 0; //init, com rx buffer
#define MAX_AT_CMD_BUF_SIZE 52
u8 AtCmdBuf[MAX_AT_CMD_BUF_SIZE], AtCmdBufDataSize = 0;
#define MAX_SIZE 100
u8 txBuf[MAX_SIZE], rxBuf[MAX_SIZE], txLen = 0;
u16 RxCont = 0;
//u16 PosR = 0;
u8 PosW = 0;
extern unsigned char WaitSetBaud;
u32 tempI2cData = 0x32;
unsigned int TxTimeout;
extern  unsigned int SysTick_Count;
extern u32 BaudRate;
unsigned char SleepStop = 0x00; //0为正常工作，
unsigned char SleepStatus = 0;

unsigned int RxTimeout;
extern char GetConnectedStatus(void);
extern void CheckAtCmdInfo(void);
extern void ChangeBaudRate(void);
extern void moduleOutData(u8 *data, u8 len);

void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{

  if ((SleepStop) &&
      (TxTimeout < SysTick_Count) &&
      (RxTimeout < SysTick_Count))
  {
    if (SleepStop == 1)  //sleep
    {
      SCB->SCR &= 0xfb;
      __WFE();

    }
    else     //stop
    {
      SysClk48to8();
      SCB->SCR |= 0x4;

      __WFI();

      RCC->CR |= RCC_CR_HSION;
      SysClk8to48();

      GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    }
  }
}

void SysClk8M(void)
{
  __ASM volatile("cpsid i");
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//selecting PLL clock as sys clock

  while (RCC_GetSYSCLKSource() != 0x0)
  {}

  RCC->CR &= ~(RCC_CR_PLLON); //clear PLL
  SysTick_Config(8000);
  __ASM volatile("cpsie i");
}

void IrqMcuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
  //有个问题： SysTick_Count 应该需要在串口有数据收发时 清零！！！
  if (ble_run_interrupt_McuCanSleep() == 0) return;

#ifdef USE_UART
  if ((SleepStop) &&
      (TxTimeout < SysTick_Count) &&
      (RxTimeout < SysTick_Count))
  {
    if (SleepStop == 1)  //sleep
    {
      SleepStatus = 1;
      SCB->SCR &= 0xfb;
      __WFE();

    }
    else     //stop
    {
      SleepStatus = 2;
      SysClk8M();
      SCB->SCR |= 0x4;

      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
      RCC->CR |=  0X1 << 19;
      RCC->CR |=  0X1 << 19;
      RCC->CR |=  0X1 << 19;
      delay_us(5);

      __WFI();
    }
  }
#endif
}
void NMI_Handler(void)
{
  RCC->CIR |= 0x1 << 23;
}

static char dis_int_count = 0;
void DisableEnvINT(void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  //to disable int
  __ASM volatile("cpsid i");

  dis_int_count ++;
}

void EnableEnvINT(void)
{
  //to enable/recover int
  dis_int_count --;
  if (dis_int_count <= 0) //protection purpose
  {
    dis_int_count = 0; //reset
    __ASM volatile("cpsie i");
  }
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}


void CheckComPortInData(void) //at cmd NOT supported
{
  u16 send;

  if (comrxbuf_wr_pos != comrxbuf_rd_pos) //not empty
  {
    if (!GetConnectedStatus())
    {
      comrxbuf_rd_pos = comrxbuf_wr_pos; //empty the buffer if any
    }
    else //connected
    {
      if (comrxbuf_wr_pos > comrxbuf_rd_pos)
      {
        send = sconn_notifydata(rxBuf + comrxbuf_rd_pos, comrxbuf_wr_pos - comrxbuf_rd_pos);
        comrxbuf_rd_pos += send;
      }
      else
      {
        send = sconn_notifydata(rxBuf + comrxbuf_rd_pos, MAX_SIZE - comrxbuf_rd_pos);
        comrxbuf_rd_pos += send;
        comrxbuf_rd_pos %= MAX_SIZE;
      }
    }
  }
}

void Sys_Standby(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR外设时钟
  RCC->APB2RSTR |= 0X01FC; //复位所有IO口
  PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
  PWR_EnterSTANDBYMode();   //进入待命（STANDBY）模式
}

extern int CHANGE_DEVINCEINFO;
extern  char  data_name[24];
char Password[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
char Num_List[13] = "123456789abcd";
unsigned char  Num_List_Send[17]={0X5A,0X11,0X09,0};
unsigned char Car_State_Feed_Back[6] = {0x5A, 0x05, 0, 0, 0, 0x00}; //蓝牙数据反馈
unsigned char Data_Feed_Back[15] = {0x5A,0X0F,0X03,0}; //蓝牙数据反馈
char Data_Statues_All[20] = {0x5A, //蓝牙上传数据头
                             0x05,//蓝牙上传数据标志位
                             0x01,//密码状态
                             0x01,//锁车状态
                             0x10,//当前电量
                             0x01,//当前速度高位
                             0x20,//当前速度低位
                             0x7,//总里程高位
                             0xEC,//总里程低位
                             0X00,//骑行时间高位
                             0X00,//骑行时间低位
                             0X0,//本次里程高位
                             0XE0,//本次里程低位
                             0x80,//当前状态
                             0x00,//故障状态
                             0x00,//大灯状态
                             0x00,//档位指令
                             0x00,//巡航指令
                             0x00,//助力方式
                             0x00//密码是否修改成功
                            };
char Password_Flag = 0;
                            
                            unsigned  char checkdata;
int l;
int  Checksum(unsigned char* data,unsigned char len)
{
    unsigned  char sum1=0;
    checkdata=0;
    for(l=0;l<len;l++)
    {
    sum1+=data[1];
    }
    checkdata=sum1;
}

void Data_Password_Analysis()
{
  if (1)
  {
    if ((data_name[0] == 0xA5) && (data_name[2] == 0x01))
    {
      if ((data_name[3] == Password[0]) &&
          (data_name[4] == Password[1]) &&
          (data_name[5] == Password[2]) &&
          (data_name[6] == Password[3]) &&
          (data_name[7] == Password[4]) &&
          (data_name[8] == Password[5]))
      {
        Password_Flag = 1;
        Car_State_Feed_Back[2] = data_name[2];
        Car_State_Feed_Back[3] = Data_Statues_All[2];
        Car_State_Feed_Back[4]=0xB0;
        sconn_notifydata(Car_State_Feed_Back, 6);  
      }
			memset(data_name, 0, 24);
    }
  }
}
void changge_password()
{
  int i = 0;
  if ((data_name[3] == Password[0]) &&
      (data_name[4] == Password[1]) &&
      (data_name[5] == Password[2]) &&
      (data_name[6] == Password[3]) &&
      (data_name[7] == Password[4]) &&
      (data_name[8] == Password[5]))
  {
    for (i = 0; i < 6; i++)
    {
      Password[i] = data_name[i + 9];
    }
    Data_Statues_All[19] = 0x01;
  }
  else
  {
    Data_Statues_All[19] = 0x00;
  }
}


int j=0;
int k=0;
void DATA_Analysis_Deal()
{
	
  if (Password_Flag == 1)
  {
    if ((data_name[0] == 0xA5) && (data_name[2] == 0x02))
    {
			Data_Statues_All[6]=Data_Statues_All[6]+1;
      if (data_name[3] == 0x01) //上锁
      {
				Data_Statues_All[13]&= 0x7F;
				Data_Statues_All[13]|=  0x80;
				printf("%x", Data_Statues_All[3]);
        Car_State_Feed_Back[2] = data_name[2];
        Car_State_Feed_Back[3] = 0x01;
				Car_State_Feed_Back[4] = Car_State_Feed_Back[0]+Car_State_Feed_Back[1]+Car_State_Feed_Back[2]+Car_State_Feed_Back[3];
        sconn_notifydata(Car_State_Feed_Back, 5);
      }
      else//解锁
      {
				Data_Statues_All[13]&= 0x7F;
				Data_Statues_All[13]|= 0x00;
        Car_State_Feed_Back[2] = data_name[2];
        Car_State_Feed_Back[3] = 0x01;
				Car_State_Feed_Back[4] = Car_State_Feed_Back[0]+Car_State_Feed_Back[1]+Car_State_Feed_Back[2]+Car_State_Feed_Back[3];
        sconn_notifydata(Car_State_Feed_Back, 5);
      }
    }
    else if ((data_name[0] == 0xA5)&& (data_name[1] == 0x04) && (data_name[2] == 0x03))//数据传输
    {
			
        for(k=0;k<10;k++)
        {
        Data_Feed_Back[3+k]=Data_Statues_All[k+4];
        }
				
        sconn_notifydata(Data_Feed_Back,15);
    }
    else if ((data_name[0] == 0xA5) &&(data_name[2] == 0x04))//灯光控制接口
    {
      //printf("5\r\n");
      Car_State_Feed_Back[2] = data_name[2];
			if (data_name[3] == 0x01)
			{
				Data_Statues_All[13] &= 0xDF;
				Data_Statues_All[13] |= 0x20;
				Car_State_Feed_Back[3] = 0x01;
				Car_State_Feed_Back[4]=0x64;
				sconn_notifydata(Car_State_Feed_Back, 6);
			}
			else if (data_name[3] == 0x02)
			{
				Data_Statues_All[13]&=0xEF;
				Data_Statues_All[13]|= 0x10;
				Car_State_Feed_Back[3] = 0x02;
				Car_State_Feed_Back[4]=0x65;
				sconn_notifydata(Car_State_Feed_Back, 6);
			}
			else
			{
				Data_Statues_All[13]=Data_Statues_All[13] & 0xCF;
			  Car_State_Feed_Back[3] = 0x00;
				Car_State_Feed_Back[4]=0x63;
			sconn_notifydata(Car_State_Feed_Back, 6);
			}
      sconn_notifydata(Car_State_Feed_Back, 6);
			//sconn_notifydata(Data_Feed_Back,15);
      memset(data_name, 0, 24);
    }
    else if ((data_name[0] == 0xA5) &&(data_name[2] == 0x05))//挡位控制
    {
      Car_State_Feed_Back[2] = data_name[2];
      Car_State_Feed_Back[3] = 0X01;
			Data_Statues_All[13] &= 0xfc;
			Data_Statues_All[13] |=  data_name[3];
      sconn_notifydata(Car_State_Feed_Back, 5);
      memset(data_name, 0, 24);
    }
    else if ((data_name[0] == 0xA5) &&(data_name[2] == 0x06))//下传巡航指令
    {
      //printf("7\r\n");
      Car_State_Feed_Back[2] = data_name[2];
			Data_Statues_All[13] &= 0xfb;
			if(data_name[3]==0x01)
			{
			Data_Statues_All[13] |= 0x04;
			}
			else
			{
			Data_Statues_All[13] |= 0x00;
			}
      Car_State_Feed_Back[3] = Data_Statues_All[17];///////////////////////////////无用的参数
      sconn_notifydata(Car_State_Feed_Back, 5);///////////////////////////////////
      memset(data_name, 0, 24);
    }
    else if ((data_name[0] == 0xA5) &&(data_name[2] == 0x07))//下传助力方式
    {
      //printf("8\r\n");
      Car_State_Feed_Back[2] = data_name[2];
			Data_Statues_All[13] &= 0xf7;
			if(data_name[3]==0x01)
			{
			Data_Statues_All[13] |= 0x08;
			}
			else
			{
			Data_Statues_All[13] |= 0x00;
			}
      Car_State_Feed_Back[3] = Data_Statues_All[18];/////////////////////////////////无用的参数
      sconn_notifydata(Car_State_Feed_Back, 5);//////////////////////////////////////
      memset(data_name, 0, 24);
    }
    else if ((data_name[0] == 0xA5) &&(data_name[2] == 0x08))//下传修改密码指令
    {
      changge_password();
      //printf("9\r\n");
      Car_State_Feed_Back[2] = data_name[2];
      Car_State_Feed_Back[3] = Data_Statues_All[19];
      sconn_notifydata(Car_State_Feed_Back, 5);
      memset(data_name, 0, 24);
    }
   else  if ((data_name[0] == 0xA5) && (data_name[1] == 0x04) && (data_name[2] == 0x09))//下传读序列号指令
    {
        //printf("10\r\n");
       for(j=0;j<13;j++)
        {
        Num_List_Send[j+3]=Num_List[j];
        }
        Num_List_Send[16]=0x99;
        sconn_notifydata(Num_List_Send,17);
        memset(data_name, 0, 24);
    }
		memset(data_name, 0, 24);
		sconn_notifydata(Data_Feed_Back,15);
  }
}

u8 connect_data = 0;
void UsrProcCallback(void) //porting api
{
  IWDG_ReloadCounter();
  //CheckComPortInData();//APP下传数据log打印开关,在最终程序处可以进行屏蔽
  if ((2 != SleepStop) || (!(GPIO_ReadInputData(GPIOB) & 0x40))) //CTL low
  {
    if ((txLen) && (0 == PosW))
    {
      UART_ITConfig(UART_BLE, UART_IT_TXIEN, ENABLE);
      UART_SendData(UART_BLE, txBuf[PosW++]);

      TxTimeout = SysTick_Count + (20000 / BaudRate);
    }
  }
  if ((SleepStop == 2) && (RxTimeout < SysTick_Count))
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    RxTimeout = SysTick_Count + (20000 / BaudRate);
  }

}




