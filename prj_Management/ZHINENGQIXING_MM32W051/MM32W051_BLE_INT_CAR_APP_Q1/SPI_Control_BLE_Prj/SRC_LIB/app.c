/*
    Copyright (c) 2015 Macrogiga Electronics Co., Ltd.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include "app.h"
#include "mg_api.h"
#include "callback.h"
extern u32 BaudRate;
extern u8 txBuf[], rxBuf[], txLen, PosW;
extern u16 RxCont;
u16 NotifyCont = 0;
u8 CanNotifyFlag = 0;
extern unsigned char SleepStop;
#define MAX_SIZE 100
#define NOTIFYSIZE  20

uint16_t HexToASCII(uint8_t data_hex,uint8_t l);

extern void moduleOutData(u8 *data, u8 len);
void ChangeBaudRate(void);
char data_name[20] = {0};
int CHANGE_DEVINCEINFO = 0;
extern char Password_Flag;
/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                     			0x10
#define ATT_CHAR_PROP_IND                           0x20
#define GATT_PRIMARY_SERVICE_UUID                   0x2800

#define TYPE_CHAR      0x2803
#define TYPE_CFG       0x2902
#define TYPE_INFO      0x2901
#define TYPE_xRpRef    0x2907
#define TYPE_RpRef     0x2908
#define TYPE_INC       0x2802
#define UUID16_FORMAT  0xff
char Battery_Level[2]={0x50,0x00};
char Read_data1[2]={0x01,0x00};
char Read_data2[2]={0x02,0x00};
char Read_data3[6]={0x5A,0x05,0X01,0X01,0X61,0X00};
char Read_data4[6]={0x5A,0x05,0X05,0X01,0X65,0X00};
extern unsigned char *ble_mac_addr;
#define SOFTWARE_INFO "SV1.0.0"
#define MANU_INFO     "MindMOtion Bluetooth"
//char DeviceInfo[24] =  "MindMotion";  /*max len is 24 bytes*/
char DeviceName[3] =  "DMX";
char DeviceName1[24] =  "123";
u16 cur_notifyhandle = 0x16;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);
char DeviceInfo[24] =  "DMX";  /*max len is 24 bytes*/
u8 *getDeviceInfoData(u8 *len)
{
	
  *len = sizeof(DeviceInfo);
  return (u8 *)DeviceInfo;
}

void updateDeviceInfoData(u8 *name, u8 len)
{
  memset(DeviceInfo, 0, 24);

  memcpy(DeviceInfo, name, len);

  ble_set_name(name, len);
}

/**********************************************************************************
                 *****DataBase****

01 - 06  GAP (Primary service) 0x1800
  03:04  name
07 - 0f  Device Info (Primary service) 0x180a
  0a:0b  firmware version
  0e:0f  software version
10 - 19  LED service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
  13     cfg
  14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
  16     cfg
  17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
  19     0x2901  info
************************************************************************************/

typedef struct ble_character16
{
  u16 type16;          //type2
  u16 handle_rec;      //handle
  u8  characterInfo[5];//property1 - handle2 - uuid2
  u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
} BLE_CHAR;

typedef struct ble_UUID128
{
  u8  uuid128[16];//uuid128 string: little endian
} BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] =
{
// ======  gatt =====  Do NOT Change!!
  {TYPE_CHAR, 0x03, ATT_CHAR_PROP_RD, 0x04, 0, 0x00, 0x2a, UUID16_FORMAT}, //name
  //05-06 reserved

// ======  device info =====    Do NOT Change if using the default!!!
  {TYPE_CHAR, 0x08, ATT_CHAR_PROP_RD, 0x09, 0, 0x29, 0x2a, UUID16_FORMAT}, //manufacture
  {TYPE_CHAR, 0x0a, ATT_CHAR_PROP_RD, 0x0b, 0, 0x26, 0x2a, UUID16_FORMAT}, //firmware version
	{TYPE_CHAR, 0x0C, ATT_CHAR_PROP_RD, 0x0D, 0, 0x23, 0x2a, UUID16_FORMAT},
  {TYPE_CHAR, 0x0e, ATT_CHAR_PROP_RD, 0x0f, 0, 0x28, 0x2a, UUID16_FORMAT}, //sw version

// ======  User service or other services added here =====  User defined
	{TYPE_CHAR, 0x11, ATT_CHAR_PROP_NTF | ATT_CHAR_PROP_RD, 0x12, 0, 0x19, 0x2A, UUID16_FORMAT},//CAR
  {TYPE_CFG,  0x13, ATT_CHAR_PROP_RD | ATT_CHAR_PROP_W}, //cfg
	{TYPE_CHAR, 0x15, ATT_CHAR_PROP_NTF | ATT_CHAR_PROP_RD, 0x16, 0, 0xE4, 0xFF, UUID16_FORMAT},//CAR
  {TYPE_CFG,  0x17, ATT_CHAR_PROP_RD | ATT_CHAR_PROP_W}, //cfg
	{TYPE_CHAR, 0x19, ATT_CHAR_PROP_RD, 									 0x1A, 0, 0xE7, 0xFF, UUID16_FORMAT},//CAR
	{TYPE_CHAR, 0x1B, ATT_CHAR_PROP_RD| ATT_CHAR_PROP_W, 	 0x1C, 0, 0xE9, 0xFF, UUID16_FORMAT},//CAR
	{TYPE_CFG,  0x1D, ATT_CHAR_PROP_RD | ATT_CHAR_PROP_W}, //cfg
};

const BLE_UUID128 AttUuid128List[] =
{
	{0xfb, 0x34, 0x09b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00},
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 1, 0, 0x40, 0x6e}, //idx0,little endian, service uuid
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 3, 0, 0x40, 0x6e}, //idx1,little endian, RxNotify
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 2, 0, 0x40, 0x6e}, //idx2,little endian, Tx
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 4, 0, 0x40, 0x6e}, //idx3,little endian, BaudRate
	{0xfb, 0x34, 0x09b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xe5, 0xff, 0x00, 0x00},
};

u8 GetCharListDim(void)
{
  return sizeof(AttCharList) / sizeof(AttCharList[0]);
}
const BLE_UUID128 AttUuid16List[][2] = {
{0x0D,0x18},
{0x0f,0x18},
};
//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType(u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type)
{
//!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!

    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
        //GAP Device Name
        u8 t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(u8*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        u8 t[] = {0x0a,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);//2为用来显示UUID的长度
        return;
    }
		 else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
				u8 t[] = {0x0f,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x13,(u8*)(AttUuid16List[1]),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x15)) //usr's service
    {
      att_server_rdByGrTypeRspPrimaryService(pdu_type, 0x15, 0x17, (u8 *)(AttUuid128List[0].uuid128), 16);
			return;
    }
		    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x19)) //usr's service
    {
      att_server_rdByGrTypeRspPrimaryService(pdu_type, 0x19, 0x1d, (u8 *)(AttUuid128List[5].uuid128), 16);
			return;
    }
		
	
  //other service added here if any
  //to do....

  ///error handle
  att_notFd(pdu_type, attOpcode, st_hd);
}

///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/,
                   u16 att_hd, u8 *attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
  u8 i;
  switch (att_hd)
  {
	case 0x1C://0X1B
		//moduleOutData("Write_Server_1B \r\n",17);   
	  moduleOutData(attValue, valueLen_w);//此处为文件的串口打印的log，在最终程序处可以进行屏蔽。
    for (i = 0; i < valueLen_w; i++)
    {
      data_name[i] = attValue[i];
    }
		ser_write_rsp_pkt(pdu_type);
		Data_Password_Analysis();//此函数带检测，第一次的时候会进入，超过第一次就不会进入此函数
		DATA_Analysis_Deal();
	break;
	case 0x16://Rx
		moduleOutData("Write_Server_16 \r\n",17);   
    moduleOutData(attValue, valueLen_w);//此处为文件的串口打印的log，在最终程序处可以进行屏蔽。
    CHANGE_DEVINCEINFO = 1;
	ser_write_rsp_pkt(pdu_type);
		break;
			case 0x1A://Rx
		moduleOutData("Write_Server_1A \r\n",17);   
    moduleOutData(attValue, valueLen_w);//此处为文件的串口打印的log，在最终程序处可以进行屏蔽。
    CHANGE_DEVINCEINFO = 1;
ser_write_rsp_pkt(pdu_type);
		break;
/////////////////////////////////////////////////
  case 0x15://Rx
    moduleOutData(attValue, valueLen_w);//此处为文件的串口打印的log，在最终程序处可以进行屏蔽。
    CHANGE_DEVINCEINFO = 1;
	ser_write_rsp_pkt(pdu_type);
		break;
  case 0x12://cmd
  case 0x13://cfg
    ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
    break;
	
  case 0x17://cfg如果丢掉此步则会显示没有ATT得服务
    ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
    break;

  default:
    att_notFd(pdu_type, attOpcode, att_hd);    /*the default response, also for the purpose of error robust */
    break;
  }
}

///STEP2.1:Queued Writes data if any
void ser_prepare_write(u16 handle, u8 *attValue, u16 attValueLen, u16 att_offset)//user's call back api
{
  //queued data:offset + data(size)
  //when ser_execute_write() is invoked, means end of queue write.

  //to do
}

void ser_execute_write(void)//user's call back api
{
  //end of queued writes

  //to do...
}

///STEP3:Read data
//// read response
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
  u8 tab[3];
  switch (attHandle) //hard code
  {
	case 0x1D: 
	{
		u8  t[2] = {0, 0};
		att_server_rd(pdu_type, attOpcode, attHandle, t, 2);
	}
	break;
	case 0x1C: 
		moduleOutData("Read_Server_1C\r\n",17);
		att_server_rd( pdu_type, attOpcode, attHandle, (u8 *)Read_data2, 2);//读相对手机而言
	break;
	case 0x1A: 
		moduleOutData("Read_Server_1A\r\n",17);   
		att_server_rd( pdu_type, attOpcode, attHandle, (u8 *)Read_data1, 2);//读相对手机而言
	break;
		  case 0x17://cfg
  {
    u8 t[2] = {0, 0};
    att_server_rd(pdu_type, attOpcode, attHandle, t, 2);
  }
	case 0x16: 
		moduleOutData("Read_Server_16\r\n",17);   
		att_server_rd( pdu_type, attOpcode, attHandle, (u8 *)Read_data4, sizeof(Read_data4));//读相对手机而言
	break;
	  case 0x13://cfg
  {
    u8 t[2] = {0, 0};
    att_server_rd(pdu_type, attOpcode, attHandle, t, 2);
  }
		case 0x12: 
		moduleOutData("Read_Server_12\r\n",17);   
		att_server_rd( pdu_type, attOpcode, attHandle, (u8 *)Battery_Level, sizeof(Battery_Level));//读相对手机而言
	break;
  case 0x0f://SOFTWARE_INFO
    att_server_rd(pdu_type, attOpcode, attHandle, (u8 *)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO) - 1);
  break;
	case 0x0d: 
		moduleOutData("133466799",9);   
		att_server_rd( pdu_type, attOpcode, attHandle, "133466799", 9);//读相对手机而言
	break;
	case 0x0b: //FIRMWARE_INFO
    //do NOT modify this code!!!
    att_server_rd(pdu_type, attOpcode, attHandle, GetFirmwareInfo(), strlen((const char *)GetFirmwareInfo()));
  break;
	case 0x09: //MANU_INFO
    //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
    att_server_rd(pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char *)get_ble_version())); //ble lib build version

  break;
	case 0x04: 
		moduleOutData("MINDMOTION",9);   
		att_server_rd( pdu_type, attOpcode, attHandle, "MINDMOTION", 9);//读相对手机而言
	break;
  default:
    att_notFd(pdu_type, attOpcode, attHandle);  /*the default response, also for the purpose of error robust */
  break;
  }
}

void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP, u16 offset)
{


}

//return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,
                            unsigned short *hd_start_r, unsigned short *hd_end_r)
{
// example
//    if((uuid16 == 0x1812) && (hd_start <= 0x19))// MUST keep match with the information save in function  att_server_rdByGrType(...)
//    {
//        *hd_start_r = 0x19;
//        *hd_end_r = 0x2a;
//        return 1;
//    }

  return 0;
}


//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{
  //to do if any ...
  //add user sending data notify operation ....
  // sconn_notifydata(pld_adv,len_adv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 *getsoftwareversion(void)
{
  return (u8 *)SOFTWARE_INFO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char gConnectedFlag = 0;
char GetConnectedStatus(void)
{
  return gConnectedFlag;
}
void LED_ONOFF(unsigned char onFlag)//module indicator,GPA8
{
  if (onFlag)
  {
    GPIOB->BRR = GPIO_Pin_1;  //low, on
  }
  else
  {
    GPIOB->BSRR = GPIO_Pin_1; //high, off
  }
}
void CONNECT_STATUS_SHOW()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;   //i2c1_scl  pb6  i2c1_sda  pb7
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 复用开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_3);

}
void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
  //[IsConnectedFlag] indicates the connection status
  LED_ONOFF(IsConnectedFlag);
  if (IsConnectedFlag != gConnectedFlag)
  {
    gConnectedFlag = IsConnectedFlag;
        if(gConnectedFlag){
            SleepStop = 1;
					GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        }
        else{
            SleepStop = 2;
					GPIO_SetBits(GPIOB, GPIO_Pin_3);
        }
  }
}

uint16_t HexToASCII(uint8_t data_hex,uint8_t l)

{
    uint8_t data_ASCII_H;
    uint8_t data_ASCII_L;
    uint16_t data_ASCII;
    data_ASCII_H = ((data_hex >> 4) & 0x0F);
    data_ASCII_L = data_hex & 0x0F;
    if(data_ASCII_H <= 9)
    {
        data_ASCII_H += 0x30;
    }
    else if((data_ASCII_H >= 10) && (data_ASCII_H <= 15))
    {
        data_ASCII_H += 0x37;
    }
    if(data_ASCII_L <= 9)
    {
        data_ASCII_L += 0x30;
    }
    else if((data_ASCII_L >= 10) && (data_ASCII_L <= 15))
    {
     data_ASCII_L += 0x37;
    }
    //data_ASCII = (((data_ASCII_H & 0x00FF) << 8) | data_ASCII_L);
    //return data_ASCII;
		DeviceName1[l]=data_ASCII_H;
		DeviceName1[l+1]=data_ASCII_L;
}

void NAME_SET()
{
	int i=0;
	int COUNT=0;
	for(i=0;i<6;i++)
	{
		COUNT=2*i;
		HexToASCII(ble_mac_addr[5-i],COUNT);
	}
	strcat(DeviceInfo,DeviceName1);
	//sprintf(DeviceInfo,"DMX123%s",DeviceName1);
}