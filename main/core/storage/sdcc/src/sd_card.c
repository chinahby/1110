#include "target.h"

#ifdef FEATURE_SDCC_VOTE_AGAINST_SLEEP
#include "sleep.h"
#endif

#if defined (T_MSM6275) || defined (T_MSM6280)
#error code not present
#else
#include "tlmm.h"
#endif

#include "pm.h"
#include CLKRGM_H
#include "sd_card.h"


#include "assert.h"

#ifdef FEATURE_DRV_SD_SPI_MODE
/*******************************************************************************
* Function Name : SD_WriteByte
* Description    : SPI写一个字节（发送完成后返回本次通讯读取的数据）
* Input          : u8 TxData 待发送的数
* Output         : None
* Return         : none
*******************************************************************************/
void SD_WriteByte(uint8 value)
{
	uint8 i; 

	//Software SPI
	for (i=0; i<8; i++) 
	{  
		gpio_out(SD_CLK_PIN,0); //set Clock Impuls low
		
		//write a byte
		if (((value >> (7-i)) & 0x01)==0x01)
		{
			gpio_out(SD_CMD_PIN,1); //Send bit by bit(MSB First)
		}
		else
		{
			gpio_out(SD_CMD_PIN,0);		 
		}

		gpio_out(SD_CLK_PIN,1); //set Clock Impuls High	 
	}//write a byte
	
	gpio_out(GPIO_OUTPUT_29,1);	//set Output High 
}

/*******************************************************************************
* Function Name : SPI_ReadWriteByte
* Description    : SPI读一个字节（发送完成后返回本次通讯读取的数据）
* Input          : none
* Output         : None
* Return         : u8 RxData 收到的数
*******************************************************************************/
uint8 SD_ReadByte(void)
{
	uint8 temp=0;
	uint8 i;

	//Software SPI
	for (i=0; i<8; i++) //MSB First
	{
		gpio_out(SD_CLK_PIN,0); //Clock Impuls (Low)
		temp = (temp << 1) + gpio_in(SD_DATA_PIN); //read mmc data out pin 
		gpio_out(SD_CLK_PIN,1);//set Clock Impuls High	
	}
	return (temp);
}


//****************************************************************************
//Routine for set the cs of the MMC/SD-Card
void SD_SetCS(boolean csid)
{
	if ( csid == TRUE)	
	{
		gpio_out(SD_SEL_PIN,1);
	}
	else
	{
		gpio_out(SD_SEL_PIN,0);
	}
}
//****************************************************************************

//****************************************************************************
//Routine for Delay
void SD_Delay(uint32 uiCSId,uint32 uiNum)
{
	uint8 i;
    /* Null for now */
    for (i = 0; i < uiNum; i++)
    {
        /* Clock out an idle byte (0xFF) */
        SD_WriteByte(0xFF);
    }
}
//****************************************************************************

/*******************************************************************************
* Function Name : SD_SendCommand_NoDeassert
* Description    : 向SD卡发送一个命令(结束是不失能片选，还有后续数据传来）
* Input          : uint8 cmd   命令 
*                  uint32 arg 命令参数
*                  uint8 crc   crc校验值
* Output         : None
* Return         : uint8 r1 SD卡返回的响应
*******************************************************************************/
uint8 SD_SendCommand_NoDeassert(uint8 cmd, uint32 arg, uint8 crc)
{
	uint8 r1;
    uint8 Retry = 0;

    //
    SD_WriteByte(0xFF);
    //片选端置低，选中SD卡
    SD_SetCS(FALSE);

    //发送
    SD_WriteByte(cmd | 0x40);                         //分别写入命令
    SD_WriteByte(arg >> 24);
    SD_WriteByte(arg >> 16);
    SD_WriteByte(arg >> 8);
    SD_WriteByte(arg);
    SD_WriteByte(crc);
    
    //等待响应，或超时退出
    while((r1 = SD_ReadByte())==0xFF)
    {
        Retry++;
        if(Retry > 200)
        {
            break;
        }
    }

    for ( Retry=0; Retry<20; Retry++);
    
    //返回响应值
    return r1;	
}

/*******************************************************************************
* Function Name : SD_SendCommand
* Description    : 向SD卡发送一个命令
* Input          : uint8 cmd   命令 
*                  uint32 arg 命令参数
*                  uint8 crc   crc校验值
* Output         : None
* Return         : uint8 r1 SD卡返回的响应
*******************************************************************************/
uint8 SD_SendCommand(uint8 cmd, uint32 arg, uint8 crc)
{
	uint8 r1;
    uint8 Retry = 0;

    //????????
    SD_WriteByte(0xff);
    //片选端置低，选中SD卡
    SD_SetCS(FALSE);

    //发送
    SD_WriteByte(cmd | 0x40);                         //分别写入命令
    SD_WriteByte(arg >> 24);
    SD_WriteByte(arg >> 16);
    SD_WriteByte(arg >> 8);
    SD_WriteByte(arg);
    SD_WriteByte(crc);
    
    //等待响应，或超时退出
    while((r1 = SD_ReadByte())==0xFF)
    {
        Retry++;
        if(Retry > 200)
        {
            break;
        }
    }
   

    //关闭片选
    SD_SetCS(TRUE);
    //在总线上额外增加8个时钟，让SD卡完成剩下的工作
    SD_WriteByte(0xFF);

    //返回状态值
    return r1;
}

/*******************************************************************************
* Function Name : SD_Init
* Description    : 初始化SD卡
* Input          : None
* Output         : None
* Return         : uint8 
*                  0：NO_ERR
*                  1：TIME_OUT
*                  99：NO_CARD
*******************************************************************************/
uint8 SD_Init(void)
{
	uint16 i;      // 用来循环计数
    uint8 r1;      // 存放SD卡的返回值
    uint16 retry; // 用来进行超时计数
    uint8 buff[6];
	
    // 纯延时，等待SD卡上电完成
    for(i=0;i<0xf00;)i++;

    //先产生>74个脉冲，让SD卡自己初始化完成
    for(i=0;i<16;i++)
    {
        SD_WriteByte(0xFF);
    }

    //-----------------SD卡复位到idle开始-----------------
    //循环连续发送CMD0，直到SD卡返回0x01,进入IDLE状态
    //超时则直接退出
    retry = 0;
    do
    {
        //发送CMD0，让SD卡进入IDLE状态
        r1 = SD_SendCommand(CMD0, 0, 0x95);
        retry++;
    }while((r1 != 0x01) && (retry<200));
    //跳出循环后，检查原因：初始化成功？or 重试超时？
    if(retry==200)
    {
        return r1;   //超时返回1
    }
    //-----------------SD卡复位到idle结束-----------------

    //获取卡片的SD版本信息
    r1 = SD_SendCommand_NoDeassert(8, 0x1aa, 0x87);

    //如果卡片版本信息是v1.0版本的，即r1=0x05，则进行以下初始化
    if(r1 == 0x05)
    {
        //设置卡类型为SDV1.0，如果后面检测到为MMC卡，再修改为MMC
        sdcc_pdata.card_type = SDCC_CARD_SD;

        //如果是V1.0卡，CMD8指令后没有后续数据
        //片选置高，结束本次命令
        SD_SetCS(TRUE);
        //多发8个CLK，让SD结束后续操作
        SD_WriteByte(0xFF);

        //-----------------SD卡、MMC卡初始化开始-----------------

        //发卡初始化指令CMD55+ACMD41
        // 如果有应答，说明是SD卡，且初始化完成
        // 没有回应，说明是MMC卡，额外进行相应初始化
        retry = 0;
        do
        {
            //先发CMD55，应返回0x01；否则出错
            r1 = SD_SendCommand(CMD55, 0, 0);
            if(r1 != 0x01)
            {
                return r1; 
            }
            //得到正确响应后，发ACMD41，应得到返回值0x00，否则重试200次
            r1 = SD_SendCommand(ACMD41, 0, 0);
            retry++;
        }while((r1!=0x00) && (retry<400));
        // 判断是超时还是得到正确回应
        // 若有回应：是SD卡；没有回应：是MMC卡
        
        //----------MMC卡额外初始化操作开始------------
        if(retry==400)
        {
            retry = 0;
            //发送MMC卡初始化命令（没有测试）
            do
            {
                r1 = SD_SendCommand(1, 0, 0);
                retry++;
            }while((r1!=0x00)&& (retry<400));
            if(retry==400)
            {
                return r1;   //MMC卡初始化超时
            }
            //写入卡类型
            sdcc_pdata.card_type = SDCC_CARD_MMC;
        }
        //----------MMC卡额外初始化操作结束------------
        
        SD_WriteByte(0xFF);
        
        //禁止CRC校验
        
		r1 = SD_SendCommand(CMD59, 0, 0x01);
        if(r1 != 0x00)
        {
            return r1; //命令错误，返回r1
        }
           
        //设置Sector Size
        r1 = SD_SendCommand(CMD16, 512, 0xff);
        if(r1 != 0x00)
        {
            return r1; //命令错误，返回r1
        }
        //-----------------SD卡、MMC卡初始化结束-----------------

    }
    //下面是V2.0卡的初始化
    //其中需要读取OCR数据，判断是SD2.0还是SD2.0HC卡
    else if(r1 == 0x01)
    {
        //V2.0的卡，CMD8命令后会传回4字节的数据，要跳过再结束本命令
        buff[0] = SD_ReadByte(); //should be 0x00
        buff[1] = SD_ReadByte(); //should be 0x00
        buff[2] = SD_ReadByte(); //should be 0x01
        buff[3] = SD_ReadByte(); //should be 0xAA
     
        SD_SetCS(TRUE);
        //the next 8 clocks
        SD_WriteByte(0xFF);
        
        //判断该卡是否支持2.7V-3.6V的电压范围
        if(buff[2]==0x01 && buff[3]==0xAA)
        {
            //支持电压范围，可以操作
            retry = 0;
            //发卡初始化指令CMD55+ACMD41
       		do
       		{
       			r1 = SD_SendCommand(CMD55, 0, 0);
        		if(r1!=0x01)
        		{
         			return r1;
        	}
        	r1 = SD_SendCommand(ACMD41, 0x40000000, 0);
            if(retry>200)   
            {
            	return r1; //超时则返回r1状态
            }
            }while(r1!=0);
          
            //初始化指令发送完成，接下来获取OCR信息

            //-----------鉴别SD2.0卡版本开始-----------
            r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
            if(r1!=0x00)
            {
                return r1; //如果命令没有返回正确应答，直接退出，返回应答
            }
            //读OCR指令发出后，紧接着是4字节的OCR信息
            buff[0] = SD_ReadByte();
            buff[1] = SD_ReadByte();
            buff[2] = SD_ReadByte();
            buff[3] = SD_ReadByte();

            //OCR接收完成，片选置高
            SD_SetCS(TRUE);
            SD_WriteByte(0xFF);

            //检查接收到的OCR中的bit30位（CCS），确定其为SD2.0还是SDHC
            //如果CCS=1：SDHC   CCS=0：SD2.0
            if(buff[0]&0x40)    //检查CCS
            {
                sdcc_pdata.card_type = SDCC_CARD_SDHC;
            }
            //-----------鉴别SD2.0卡版本结束-----------
        }

    }
    
    return r1;
}
/*******************************************************************************
* Function Name : SD_WriteSingleBlock
* Description    : 写入SD卡的一个block
* Input          : uint32 sector 扇区地址（sector值，非物理地址） 
*                  uint8 *buffer 数据存储地址（大小至少512byte） 
* Output         : None
* Return         : uint8 r1 
*                   0： 成功
*                   other：失败
*******************************************************************************/
uint8 SD_WriteSingleBlock(uint32 sector, const uint8 *data)
{
	uint8 r1;
    uint16 i;
    uint16 retry;

    //设置为高速模式

    //如果不是SDHC，给定的是sector地址，将其转换成byte地址
    //if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<SD_BLOCKSIZE_NBITS;
    }

    r1 = SD_SendCommand(CMD24, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1; //应答不正确，直接返回
    }
    
    //开始准备数据传输
    SD_SetCS(0);
    //先放3个空数据，等待SD卡准备好
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    //放起始令牌0xFE
    SD_WriteByte(0xFE);

    //放一个sector的数据
    for(i=0;i<512;i++)
    {
        SD_WriteByte(*data++);
    }
    //发2个Byte的dummy CRC
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);
    
    //等待SD卡应答
    r1 = SD_ReadByte();
    if((r1&0x1F)!=0x05)
    {
        SD_SetCS(TRUE);
        return r1;
    }
    
    //等待操作完成
    retry = 0;
    while(!SD_ReadByte())
    {
        retry++;
        if(retry>0xfffe)        //如果长时间写入没有完成，报错退出
        {
            SD_SetCS(TRUE);
            return 1;           //写入超时返回1
        }
    }

    //写入完成，片选置1
    SD_SetCS(TRUE);
    SD_WriteByte(0xff);

    return 0;
}

/*******************************************************************************
* Function Name : SD_ReadSingleBlock
* Description    : 读SD卡的一个block
* Input          : uint32 sector 取地址（sector值，非物理地址） 
*                  uint8 *buffer 数据存储地址（大小至少512byte） 
* Output         : None
* Return         : uint8 r1 
*                   0： 成功
*                   other：失败
*******************************************************************************/
uint8 SD_ReadSingleBlock(uint32 sector, uint8 *buffer)
{
	uint8 r1;

    //如果不是SDHC，将sector地址转成byte地址
    sector = sector<<SD_BLOCKSIZE_NBITS;

	r1 = SD_SendCommand(CMD17, sector, 0);//读命令

	if(r1 != 0x00)
    {
        return r1;
    }
    
    r1 = SD_ReceiveData(buffer, SD_BLOCKSIZE, 1);
    if(r1 != 0)
    {
        return r1;   //读数据出错！
    }
    else
    {
        return 0;
    }
}
/*******************************************************************************
* Function Name : SD_ReceiveData
* Description    : 从SD卡中读回指定长度的数据，放置在给定位置
* Input          : uint8 *data(存放读回数据的内存>len)
*                  uint16 len(数据长度）
*                  uint8 release(传输完成后是否释放总线CS置高 0：不释放 1：释放）
* Output         : None
* Return         : uint8 
*                  0：NO_ERR
*                  other：错误信息
*******************************************************************************/
uint8 SD_ReceiveData(uint8 *data, uint16 len, uint8 release)
{
	uint16 retry;
    uint8 r1;

    // 启动一次传输
    SD_SetCS(0);
    //等待SD卡发回数据起始令牌0xFE
    //SD_WriteByte(0xff);
    retry = 0;
    do
    {
        
        r1 = SD_ReadByte();
        retry++;
        if(retry>0x2000) //2000次等待后没有应答，退出报错
        {
            SD_SetCS(TRUE);
            return 1;
        }
    }while(r1 != 0xFE);
    //while((SPI_ReadByte())!=0xfe); 
    //开始接收数据
    while(len--)
    {
        *data = SD_ReadByte();
        data++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_WriteByte(0xFF);
    SD_WriteByte(0xFF);
    //按需释放总线，将CS置高
    if(release == 1)
    {
        //传输结束
        SD_SetCS(TRUE);
        SD_WriteByte(0xFF);
    }

    return 0;
}

/*******************************************************************************
* Function Name : SD_WaitReady
* Description    : 等待SD卡Ready
* Input          : None
* Output         : None
* Return         : uint8 
*                   0： 成功
*                   other：失败
*******************************************************************************/
uint8 SD_WaitReady(void)
{
	uint8 r1;
    uint16 retry;
    retry = 0;
    do
    {
        r1 = SD_ReadByte();
        if(retry==0xfffe)
        {
            return 1;
        }
    }while(r1!=0xFF);

    return 0;
}

/*******************************************************************************
* Function Name : SD_ReadMultiBlock
* Description    : 读SD卡的多个block
* Input          : uint32 sector 取地址（sector值，非物理地址） 
*                  uint8 *buffer 数据存储地址（大小至少512byte）
*                  uint8 count 连续读count个block
* Output         : None
* Return         : uint8 r1 
*                   0： 成功
*                   other：失败
*******************************************************************************/
uint8 SD_ReadMultiBlock(uint32 sector, uint8 *buffer, uint8 count)
{
	uint8 r1;

    //如果不是SDHC，将sector地址转成byte地址
    sector = sector<<SD_BLOCKSIZE_NBITS;
    //SD_WaitReady();
    //发读多块命令
	r1 = SD_SendCommand(CMD18, sector, 0);//读命令
	if(r1 != 0x00)
    {
        return r1;
    }
    //开始接收数据
    do
    {
        if(SD_ReceiveData(buffer, SD_BLOCKSIZE, 0) != 0x00)
        {
            break;
        }
        buffer += 512;
    } while(--count);

    //全部传输完毕，发送停止命令
    SD_SendCommand(CMD12, 0, 0);
    //释放总线
    SD_SetCS(TRUE);
    SD_WriteByte(0xFF);
    
    if(count != 0)
    {
        return count;   //如果没有传完，返回剩余个数
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* Function Name : SD_WriteMultiBlock
* Description    : 写入SD卡的N个block
* Input          : uint32 sector 扇区地址（sector值，非物理地址） 
*                  uint8 *buffer 数据存储地址（大小至少512byte）
*                  uint8 count 写入的block数目
* Output         : None
* Return         : u8 r1 
*                   0： 成功
*                   other：失败
*******************************************************************************/
uint8 SD_WriteMultiBlock(uint32 sector, const uint8 *data, uint8 count)
{
	uint8 r1;
    uint16 i;

    //如果不是SDHC，给定的是sector地址，将其转换成byte地址
    //if(SD_Type != SD_TYPE_V2HC)
    {
        sector = sector<<SD_BLOCKSIZE_NBITS;
    }
    
    //发多块写入指令
    r1 = SD_SendCommand(CMD25, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1; //应答不正确，直接返回
    }
    
    //开始准备数据传输
    SD_SetCS(FALSE);
    //先放3个空数据，等待SD卡准备好
    SD_WriteByte(0xff);
    SD_WriteByte(0xff);

    //--------下面是N个sector写入的循环部分
    do
    {
        //放起始令牌0xFC 表明是多块写入
        SD_WriteByte(0xFC);
    
        //放一个sector的数据
        for(i=0;i<512;i++)
        {
            SD_WriteByte(*data++);
        }
        //发2个Byte的dummy CRC
        SD_WriteByte(0xff);
        SD_WriteByte(0xff);
        
        //等待SD卡应答
        r1 = SD_ReadByte();
        if((r1&0x1F)!=0x05)
        {
            SD_SetCS(TRUE);    //如果应答为报错，则带错误代码直接退出
            return r1;
        }

        //等待SD卡写入完成
        if(SD_WaitReady()==1)
        {
            SD_SetCS(TRUE);    //等待SD卡写入完成超时，直接退出报错
            return 1;
        }

        //本sector数据传输完成
    }while(--count);
    
    //发结束传输令牌0xFD
    SD_WriteByte(0xFD);
    r1 = SD_ReadByte();
    if(r1==0x00)
    {
        count = 0xfe;
    }

    if(SD_WaitReady())
    {
        while(1)
        {
        }
    }
    
    //写入完成，片选置1
    SD_SetCS(TRUE);
    SD_WriteByte(0xff);

    return count;   //返回count值，如果写完则count=0，否则count=1
}

uint32 SD_Initialize(void)
{
	#define SD_MAXTEST_TIMES 5 //changed by pengyu.li to 5 times.
	uint8         i = 0;
	uint32 rtStatus = 0; 
	
	
	//try to find sd card in N times.
	while(i++ < SD_MAXTEST_TIMES)
	{    
		//power down and power on once again.
		sdcc_mdelay(30);
		
		//initialize the sd card.
	    rtStatus = SD_Init();
	    if( 0 == rtStatus) 
	    {
	    	break; //successful to find sd card.
	    }
	    else
	    {
	    	ERR("sd card error status is %ld", rtStatus,0,0);
	    }
	  
	}
	if(i >= SD_MAXTEST_TIMES)
		ERR("not find tflash in many times",0,0,0);//added by lipengyu for n times init of sd card.
	return 0;
}

/*******************************************************************************
* Function Name : SD_GetCID
* Description    : 获取SD卡的CID信息，包括制造商信息
* Input          : uint8 *cid_data(存放CID的内存，至少16Byte）
* Output         : None
* Return         : uint8 
*                  0：NO_ERR
*                  1：TIME_OUT
*                  other：错误信息
*******************************************************************************/
uint8 SD_GetCID(uint8 *cid_data)
{
	uint8 r1;

    //发CMD10命令，读CID
    r1 = SD_SendCommand(CMD10, 0, 0xFF);
    if(r1 != 0x00)
    {
        return r1; //没返回正确应答，则退出，报错
    }
    //接收16个字节的数据
    SD_ReceiveData(cid_data, 16, 1);

    return 0;
}

/*******************************************************************************
* Function Name : SD_GetCSD
* Description    : 获取SD卡的CSD信息，包括容量和速度信息
* Input          : uint8 *cid_data(存放CID的内存，至少16Byte）
* Output         : None
* Return         : uint8 
*                  0：NO_ERR
*                  1：TIME_OUT
*                  other：错误信息
*******************************************************************************/
uint8 SD_GetCSD(uint8 *csd_data)
{
	uint8 r1;

    //发CMD9命令，读CSD
    r1 = SD_SendCommand(CMD9, 0, 0xFF);
    if(r1 != 0x00)
    {
        return r1; //没返回正确应答，则退出，报错
    }
    //接收16个字节的数据
    SD_ReceiveData(csd_data, 16, 1);

    return 0;
}

/*******************************************************************************
* Function Name : SD_GetCapacity
* Description    : 获取SD卡的容量
* Input          : None
* Output         : None
* Return         : uint32 capacity 
*                   0： 取容量出错 
*******************************************************************************/
uint32 SD_GetCapacity(void)
{
	uint8 rsp[16];
	sdcc_csd_type    csd;

    //取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(rsp)!=0)
    {
        return 0;
    }
       
    SD_DecodeCsd((uint32*)rsp, &csd);

	switch (sdcc_pdata.card_type)
	{
		case SDCC_CARD_SD:
		case SDCC_CARD_SDHC:
		{
			if (csd.csd_structure == 1)
			{
				/* csd_structure = 1 means CSD Version 2.0 for */
				/* High Capacity SD memory card */
				SD_GetHcMemoryInfo(csd.read_bl_len, csd.c_size);
			}
			else
			{
				/* csd_structure = 0 means CSD Version 1.0 for */
				/* Standard Capacity SD memory card */
				SD_GetStdMemoryInfo(csd.read_bl_len, csd.c_size_mult,csd.c_size);
			}
		}
		break;

		case SDCC_CARD_MMCHC:
		{
			/* defer to sdcc_config_mmc_modes_segment as EXT_CSD register */
			/* is read there.  SEC_COUNT must be used to calculate the high */
			/* density card size. */
		}
		break;

		case SDCC_CARD_MMC:
		default:
		{
			/* get the standard memory info anyway */
			SD_GetStdMemoryInfo(csd.read_bl_len, csd.c_size_mult,csd.c_size);
		}
		break;
	}
    return csd.c_size;
}

/******************************************************************************
* Name: SD_DecodeCsd
*
* Description:
*    This function is to parse out the CSD data.
*
* Arguments:
*       data  pointer to data read in from response registers.
*       csd   pointer to the stucture to save the csd data to.
*
******************************************************************************/
void SD_DecodeCsd(const uint32   *data,sdcc_csd_type  *csd)
{
	uint32   value;

	/*----------------------------------------------------------------------*/

	if(!csd)
		return;

	/* In resp[0] */
	value = *data;
	csd->csd_structure      = (uint8)(value >> 30);

	switch (sdcc_pdata.card_type)
	{
		case SDCC_CARD_SD:
		case SDCC_CARD_SDHC:
		{
			if (csd->csd_structure == 1)
			{
				/* CSD Version 2.0: CSD_STRUCTURE = 1 */
				SD_DecodeHcCcsd(data, csd);
			}
			else
			{
				/* CSD Version 1.0: CSD_STRUCTURE = 0 */
				SD_DecodeStdCsd(data, csd);
			}
		}
		break;

		case SDCC_CARD_MMC:
		case SDCC_CARD_MMCHC:
		default:
		{
			SD_DecodeStdCsd(data, csd);
			/* System Specification version is for MMC only. */
			sdcc_pdata.mem.spec_vers = csd->spec_vers;
		}
		break;
	}

	return;
}

/******************************************************************************
* Name: SD_DecodeHcCcsd
*
* Description:
*    This function is to read the passed in data of the High Capacity
*    SD memory card (CSD version 2.0) and assign the proper bit values
*    to the fields in csd structure.

*
* Arguments:
*       data             pointer to data read in from response registers.
*       csd              pointer to the stucture to save the csd data to.
*
* Returns:
*    None
*
******************************************************************************/
void SD_DecodeHcCcsd(const uint32 *data,sdcc_csd_type *csd)
{
	uint32    value;
	uint32    tmp;

	if (NULL == csd)
	{
		return;
	}

	/* In resp[0] */
	value                   = *data;
	/* taac: data read access-time is fixed to 0Eh (means 1ms) --> csd[119:112]*/
	csd->taac               = SDCC_CSD_TAAC;
	/* nsac: data read access-time in CLK cycles is fixed to 00h --> csd[111:104] */
	csd->nsac               = SDCC_CSD_NSAC;
	/* tran_speed: max. data transfer rate --> csd[103:96] */
	csd->tran_speed         = (uint8)(value & 0x000000FF);

	/* read_bl_len: max. read data block length is fixed to */
	/* 9h (means 512 Bytes) --> csd[83:80] */
	value                   = *(++data);
	csd->read_bl_len        = SDCC_CSD_READ_BL_LEN;

	/* c_size: device size --> csd[69:48] */
	/* This field is expanded to 22 bits and can indicate up to 2TBytes. */
	tmp         = value & 0x0000003F;
	value       = *(++data);
	csd->c_size = (uint32)((tmp << 16) | ((value & 0xFFFF0000) >> 16));
}

/******************************************************************************
* Name: SD_DecodeStdCsd
*
* Description:
*    This function is to read the passed in data of the Standard Capacity
*    SD memory card (CSD version 1.0) and assign the proper bit values
*    to the fields in csd structure.
*
* Arguments:
*       data             pointer to data read in from response registers.
*       csd              pointer to the stucture to save the csd data to.
*
* Returns:
*    None
*
******************************************************************************/
void SD_DecodeStdCsd(const uint32 *data,sdcc_csd_type  *csd)
{
	uint32    value;
	uint32    tmp;

	if (NULL == csd)
	{
		return;
	}

	/* In resp[0] */
	value                   = *data;
	/* For MMC only: spec_vers: system specification version --> csd[125:122] */
	csd->spec_vers          = (uint8)((value & 0x3C000000) >> 26);
	/* taac: data read access-time-1 --> csd[119:112] */
	csd->taac               = (uint8)((value & 0x00FF0000) >> 16);
	/* nsac: data read access-time-2 in CLK cycles --> csd[111:104] */
	csd->nsac               = (uint8)((value & 0x0000FF00) >> 8);
	/* tran_speed: max. data transfer rate --> csd[103:96] */
	csd->tran_speed         = (uint8)(value & 0x000000FF);

	/* read_bl_len: max. read data block length --> csd[83:80] */
	value                   = *(++data);
	csd->read_bl_len        = (uint8)((value & 0x000F0000) >> 16);

	/* c_size: device size --> csd[73:62] */
	tmp         = value & 0x000003FF;
	value       = *(++data);
	csd->c_size = (uint32)((tmp << 2) | ((value & 0xC0000000) >> 30));

	/* c_size_mult: device size multiplier --> csd[49:47] */
	csd->c_size_mult        = (uint16)((value & 0x00038000) >> 15);
}

/******************************************************************************
* Name: SD_GetStdMemoryInfo
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the Standard Capacity SD memory card.
*
* Arguments:
*       block_len        data block length
*       c_size_mult      device size multiplier
*       c_size           device size
*
* Returns:
*    None
*
******************************************************************************/
void SD_GetStdMemoryInfo(uint32 block_len,uint8 c_size_mult,uint32 c_size)
{
	uint32    card_size;

	card_size     = 1 << (c_size_mult + 2);
	card_size    *= (c_size + 1); 

	sdcc_pdata.mem.block_len = 1UL << block_len; /* in Bytes */

	/* Since the block length can be in diffrent size
	* 512, 1024, 2048 etc..Calculate the card size in the absolute 
	* block length of multiple 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE)
	*/
	sdcc_pdata.mem.card_size = card_size*(sdcc_pdata.mem.block_len/SDCC_DEFAULT_BLK_LENGTH_SIZE);
}

/******************************************************************************
* Name: SD_GetHcMemoryInfo
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the High Capacity SD memory card.
*
* Arguments:
*       block_len        data block length
*       c_size           device size
*
* Returns:
*    None
*
******************************************************************************/
void SD_GetHcMemoryInfo(uint32 block_len,uint32 c_size)
{
	sdcc_pdata.mem.block_len = 1UL << block_len; /* in Bytes */

	/* The user data area capacity is calculated from the c_size as follows: */
	/*    memory capacity = (c_size+1)*512K byte    */
	/* where 512 is the block len of the High Capacity SD memory card and */
	/*       1K byte = 1024 bytes */
	/* Calculate the card size in the absolute block length of muliple */
	/* 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE) */
	sdcc_pdata.mem.card_size = (c_size + 1)*((sdcc_pdata.mem.block_len*1024)/SDCC_DEFAULT_BLK_LENGTH_SIZE);
}
#endif