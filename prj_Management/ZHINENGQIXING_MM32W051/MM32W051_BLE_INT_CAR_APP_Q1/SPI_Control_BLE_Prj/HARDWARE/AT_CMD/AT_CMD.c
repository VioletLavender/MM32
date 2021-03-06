#include "AT_CMD.h"
#include "mg_api.h"
#include "string.h"

/********************************************************************************************************
**函数信息 ：LED_Init(void)
**功能描述 ：LED初始化
**输入参数 ：无
**输出参数 ：无
********************************************************************************************************/
#define MAX_SIZE 100
#define MAX_AT_CMD_BUF_SIZE 52
#define MM126_M305_VERSION_STRING  "IND:Ver1.0\n"
#define comrxbuf_wr_pos RxCont
#define MAX_AT_CMD_NAME_SIZE 16
#define at_cmd_num  (sizeof(at_func_list)/sizeof(at_func_list[0]))
    
extern u16 comrxbuf_rd_pos; //init, com rx buffer
extern u8 AtCmdBuf[MAX_AT_CMD_BUF_SIZE], AtCmdBufDataSize;
extern u32 BaudRate;
extern unsigned char SleepStop, rxBuf[], PosW;
extern u16 RxCont;
extern void moduleOutData(u8 *data, u8 len);
extern void updateDeviceInfoData(u8 *name, u8 len);
extern char GetConnectedStatus(void);

unsigned char WaitSetBaud = 0;

void atcmd_Minfo(u8 *parameter, u8 len);
void atcmd_Help(u8 *parameter, u8 len);

typedef void (*ATCMDFUNC)(u8 *cmd/*NULL ended, leading with the cmd NAME string, checking usage*/, u8 len);

typedef struct _tagATCMD
{
  ATCMDFUNC func;
  u8 name[MAX_AT_CMD_NAME_SIZE]; //max len is 11 bytes
} AT_CMD_FUNC;

//AT+SETNAME=abcdefg
void atcmd_SetName(u8 *parameter, u8 len)
{
  if (len <= 8)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }
  updateDeviceInfoData(parameter + 8, len - 8);
  moduleOutData((u8 *)"IND:OK\n", 7);
}

//AT+SETINTERVAL=160
void atcmd_SetAdvInt(u8 *parameter, u8 len)
{
  int v = 0;
  unsigned char i;
  if (len <= 12)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }

  for (i = 12 ; i < len ; i ++)
  {
    v *= 10;
    v += (parameter[i] - '0');
  }
  
  if (v > 3200)v = 3200;
  ble_set_interval(v);
  moduleOutData((u8 *)"IND:OK\n", 7);
}

u8 Hex2Int(unsigned char C)
{
  if (C <= '9')
  {
    return C - '0';
  }
  else if (C <= 'F')
  {
    return C - 'A' + 10;
  }
  else
  {
    return C - 'a' + 10;
  }
}

//AT+BLESEND=N,0x123456abc
void atcmd_SendData(u8 *parameter, u8 len)
{
  u8 *data = parameter;
  u8  data_len = 0, i;
  //I am here check the data format
  //tbd...
  if (len <= 8)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }
  parameter += 8;//move to N
  while (parameter[0] != ',')
  {
    data_len *= 10;
    data_len += (parameter[0] - '0');
    parameter ++;
  }
  parameter += 3; //move 0x
  for (i = 0 ; i < data_len ; i ++)
  {
    data[i] = ((Hex2Int(parameter[0]) << 4) | Hex2Int(parameter[1]));
    parameter += 2;
  }
  //ble send data [data, data_len]
  sconn_notifydata(data, data_len); //pls check the connect status if necessary
  moduleOutData((u8 *)"IND:OK\n", 7);
}

//AT+SETADVFLAG=x   x=0/1, 0 disable adv, 1 enable adv
void atcmd_SetAdvFlag(u8 *parameter, u8 len)
{
  if (len <= 11)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }
  parameter += 11;//move to x
  if ('0' == parameter[0]) //diable adv
  {
    ble_set_adv_enableFlag(0);
  }
  else
  {
    ble_set_adv_enableFlag(1);
  }
  moduleOutData((u8 *)"IND:OK\n", 7);
}

//AT+DISCON, disconnect the connection is any
void atcmd_DisconnecteBle(u8 *parameter, u8 len)
{
  if (len != 6)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }
  ble_disconnect();
  moduleOutData((u8 *)"IND:OK\n", 7);
}

//AT+LOWPOWER=x   x=0/1/2
void atcmd_LowPower(u8 *parameter, u8 len)
{
  if (len <= 9)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }

  parameter += 9;//move to x
  if (0x31 == parameter[0])
  {
    SleepStop = 1;
  }
  else if (0x32 == parameter[0])
  {
    SleepStop = 2;
  }
  else
  {
    SleepStop = 0;
  }
  moduleOutData((u8 *)"IND:OK\n", 7);
}

//AT+SETBAUD
void atcmd_SetBaud(u8 *parameter, u8 len)
{
  int v = 0;
  unsigned char i;
  if (len <= 8)
  {
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return;
  }

  for (i = 8 ; i < len ; i ++)
  {
    v *= 10;
    v += (parameter[i] - '0');
  }
  
  if (BaudRate != v)
  {
    BaudRate = v;
    WaitSetBaud = 1;
  }
  else
  {
    moduleOutData((u8 *)"IND:OK\n", 7);
  }
  //moduleOutData((u8*)"IND:TBD\n",8);
}

//function list
AT_CMD_FUNC at_func_list[] =
{
  {atcmd_SetName, "SETNAME="},
  {atcmd_SetAdvInt, "SETINTERVAL="},
  {atcmd_SendData, "BLESEND="},
  {atcmd_LowPower, "LOWPOWER="},
  {atcmd_SetBaud, "SETBAUD="},
  {atcmd_SetAdvFlag, "SETADVFLAG="},
  {atcmd_DisconnecteBle, "DISCON"},
  {atcmd_Minfo, "MINFO"},
  {atcmd_Help, "HELP"},
};

u8 IsExactCmdInclude(u8 *data, const u8 *cmd)
{
  u8 i, len = strlen((const char *)cmd);

  for (i = 0 ; i < len ; i ++)
  {
    if (cmd[i] != data[i])
    {
      return 0;
    }
  }

  return 1;
}

//AT+MINFO
void atcmd_Minfo(u8 *parameter, u8 len)
{
  moduleOutData((u8 *)"IND:OK\n", 7);
  moduleOutData((u8 *)MM126_M305_VERSION_STRING, sizeof(MM126_M305_VERSION_STRING) - 1);
  moduleOutData((u8 *)"IND:CON=", 8);
  if (GetConnectedStatus())
  {
    moduleOutData((u8 *)"1\n", 2);
  }
  else
  {
    moduleOutData((u8 *)"0\n", 2);
  }
}

//AT+HELP
void atcmd_Help(u8 *parameter, u8 len)
{
  char i, templen;
  char name[20] = "AT+";
  moduleOutData((u8 *)"IND:OK\n", 7);
  moduleOutData((u8 *)MM126_M305_VERSION_STRING, sizeof(MM126_M305_VERSION_STRING) - 1);
  for (i = 0; i < at_cmd_num ; i ++)
  {
    strcpy(name + 3, (const char *)(at_func_list[i].name));
    templen = strlen(name);
    name[templen] = 0X0A;
    moduleOutData((u8 *)name, templen + 1); //NULL terminated format
  }
}

void AtCmdParser(u8 *cmd, u8 len)
{
  u8 i;
  for (i = 0 ; i < at_cmd_num ; i++)
  {
    if (IsExactCmdInclude(cmd, at_func_list[i].name))
    {
      //found
      at_func_list[i].func(cmd/*including the CMD name*/, len);
      return;
    }
  }
#if UART_DBG_EN
  printf("IND:ERROR CMD not supported.\n");
#endif
  moduleOutData((u8 *)"IND:ERR\n", 8);
}

void AtCmdPreParser(u8 *cmd, u8 len)
{
  if (!IsExactCmdInclude(cmd, (const u8 *)"AT+")) //AT+MINFO
  {
#ifdef UART_DBG_EN
    printf("IND:ERROR CMD format error.\n");
#endif
    moduleOutData((u8 *)"IND:ERR\n", 8);
    return; //cmd error
  }
  AtCmdParser(cmd + 3, len - 3);
}

void CheckAtCmdInfo(void) //main entrance
{
  while (comrxbuf_wr_pos != comrxbuf_rd_pos) //not empty
  {
    //has data
    AtCmdBuf[AtCmdBufDataSize++] = rxBuf[comrxbuf_rd_pos];
    if (AtCmdBufDataSize >= MAX_AT_CMD_BUF_SIZE) //error found
    {
      AtCmdBufDataSize = 0;//I just reset the position, drop the cmd
    }
    if ((rxBuf[comrxbuf_rd_pos] == 0x0a) || (rxBuf[comrxbuf_rd_pos] == 0x0d))
    {
      if (AtCmdBufDataSize == 1)
      {
        AtCmdBufDataSize = 0;
      }
      else//found one cmd
      {
        AtCmdBuf[AtCmdBufDataSize - 1] = '\0';
        AtCmdPreParser(AtCmdBuf, AtCmdBufDataSize - 1);
        AtCmdBufDataSize = 0;//move to next cmd if any
      }
    }
    comrxbuf_rd_pos ++;
    comrxbuf_rd_pos %= MAX_SIZE; //com buff len
  }
}
