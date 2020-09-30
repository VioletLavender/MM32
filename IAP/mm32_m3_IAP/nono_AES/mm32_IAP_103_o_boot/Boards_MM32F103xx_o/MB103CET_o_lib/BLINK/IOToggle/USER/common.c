#include "common.h"
//#include "ymodem.h"

/* 变量 ----------------------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;

/*******************************************************************************
  * @函数名称   Int2Str
  * @函数说明   整形数据转到字符串
  * @输入参数   intnum:数据
  * @输出参数   str：转换为的字符串
  * @返回参数   无
*******************************************************************************/
void Int2Str(uint8_t *str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j - 1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
  * @函数名称   Int2Str
  * @函数说明   字符串转到数据
  * @输入参数   inputstr:需转换的字符串
  * @输出参数   intnum：转好的数据
  * @返回参数   转换结果
                1：正确
                0：错误
*******************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else //最多10为2输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

/*******************************************************************************
  * @函数名称   Int2Str
  * @函数说明   字符串转到数据
  * @输入参数   inputstr:需转换的字符串
  * @输出参数   intnum：转好的数据
  * @返回参数   转换结果
                1：正确
                0：错误
*******************************************************************************/
/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t *num)
{
    uint8_t inputstr[16];

    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0')
            continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }

        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

/*******************************************************************************
  * @函数名称   SerialKeyPressed
  * @函数说明   测试超级终端是否有按键按下
  * @输入参数   key:按键
  * @输出参数   无
  * @返回参数   1：正确
                0：错误
*******************************************************************************/
uint32_t SerialKeyPressed(uint8_t *key)
{

    if (UART_GetFlagStatus(UART1, UART_FLAG_RXAVL) != RESET)
    {
        *key = (uint8_t)UART1->RDR;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
  * @函数名称   GetKey
  * @函数说明   通过超级中断回去键码
  * @输入参数   无
  * @输出参数   无
  * @返回参数   按下的键码
*******************************************************************************/
uint8_t GetKey(void)
{
    uint8_t key = 0;

    //等待按键按下
    while (1)
    {
        if (SerialKeyPressed((uint8_t *)&key))
            break;
    }
    return key;
}

/*******************************************************************************
  * @函数名称   SerialPutChar
  * @函数说明   串口发送一个字符
  * @输入参数   C:需发送的字符
  * @输出参数   i无
  * @返回参数   无
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
    UART_SendData(UART1, c);
    while (UART_GetFlagStatus(UART1, UART_FLAG_TXEMPTY) == RESET)
    {
    }
}

/*******************************************************************************
  * @函数名称   SerialPutChar
  * @函数说明   串口发送一个字符串
  * @输入参数   *s:需发送的字符串3
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}

/*******************************************************************************
  * @函数名称   GetInputString
  * @函数说明   从串口获取一个字符串
  * @输入参数   *s:存字符串的地址
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void GetInputString(uint8_t *buffP)
{
    uint32_t bytes_read = 0;
    uint8_t c = 0;
    do
    {
        c = GetKey();
        if (c == '\r')
            break;
        if (c == '\b') // Backspace 按键
        {
            if (bytes_read > 0)
            {
                SerialPutString("\b \b");
                bytes_read--;
            }
            continue;
        }
        if (bytes_read >= CMD_STRING_SIZE)
        {
            SerialPutString("Command string size overflow\r\n");
            bytes_read = 0;
            continue;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
            buffP[bytes_read++] = c;
            SerialPutChar(c);
        }
    }
    while (1);
    SerialPutString(("\n\r"));
    buffP[bytes_read] = '\0';
}

/*******************************************************************************
  * @函数名称   FLASH_PagesMask
  * @函数说明   计算Falsh页
  * @输入参数   Size:文件长度
  * @输出参数   无
  * @返回参数   页的数量
*******************************************************************************/
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

/*******************************************************************************
  * @函数名称   FLASH_DisableWriteProtectionPages
  * @函数说明   接触Flash写保护
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    //测试是否写保护
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
        //用处选项字是否有编程
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //重新保存选项字
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE)
        {
            SerialPutString("Write Protection disabled...\r\n");

            SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");
            //系统复位重新加载选项字
            NVIC_SystemReset();
        }
        else
        {
            SerialPutString("Error: Flash write unprotection failed...\r\n");
        }
    }
    else
    {
        SerialPutString("Flash memory not write protected\r\n");
    }
}

/*******************************************************************************
  * @函数名称   Main_Menu
  * @函数说明   显示菜单栏在超级终端
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Main_Menu(void)
{
    uint8_t key = 0;
    /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
    /*从用户程序将要加载的地方获取块的数量(4或2页)*/
    BlockNbr = (FlashDestination - DefaultStartAddress) >> 12;//FLASH分页与块，每页为1/2k,每块4k
    /* Compute the mask to test if the Flash memory, where the user program will be
        loaded, is write protected */
    if (BlockNbr < 62)
    {
        UserMemoryMask = ((uint32_t) ~((1 << BlockNbr) - 1));//此处用来获取掩码的值
    }
    else
    {
        UserMemoryMask = ((uint32_t)DefaultStartAddress);//此处无用
    }
//返回FLASH写保护选择字节的值FLASH_GetWriteProtectionOptionByte()
    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
        //如果被写保护为0，判断掩码的数值&写保护寄存器的数值是否等于自己来判断是否可以进行升级
    {
        FlashProtection = 1;
    }
    else
    {
        FlashProtection = 0;
    }

    while (1)
    {
        SerialPutString("\r\n=========== Main Menu ==========\r\n");
        SerialPutString("= Download Image To Flash -- 1 =\r\n");
        SerialPutString("= Execute The New Program -- 2 =\r\n");

        if (FlashProtection != 0)
        {
            SerialPutString("Disable the write protection -- 3\r\n");
        }
        SerialPutString("================================\r\n");
        key = GetKey();

        if (key == 0x31)
        {
            //下载程序
            SerialDownload();
        }
        else if (key == 0x32)
        {
						SerialPutString("Execute user 11111\r\n");
            SerialPutString("Execute user Program\r\n");
            JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);

            //跳转到用户程序
            Jump_To_Application = (pFunction)JumpAddress;
            //初始化用户程序的堆栈指针
            __set_MSP(*(__IO uint32_t *)ApplicationAddress);
            Jump_To_Application();
        }
        else if ((key == 0x33) && (FlashProtection == 1))
        {
            //解除写保护
            FLASH_DisableWriteProtectionPages();
        }
        else if (key == 'r')
        {
            SerialPutString("\r\nSystem Reboot...\r\n");
            __set_FAULTMASK(1);
            NVIC_SystemReset();
        }
        else
        {
            if (FlashProtection == 0)
            {
                SerialPutString("The number should be either 1 or 2\r\n");
            }
            else
            {
                SerialPutString("The number should be either 1, 2 or 3\r\n");
            }
        }
    }
}

/*******************************文件结束***************************************/
