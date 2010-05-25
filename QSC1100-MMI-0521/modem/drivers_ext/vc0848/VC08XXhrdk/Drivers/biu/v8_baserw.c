
#include "mmp_rdk.h"
#include "v8_baserw.h"
#include "v8_reg.h"
#include "v8_biu.h"
#include "v8_userbaserw.h"


MMD_U32 g_biu_width=0;		// 0: 8 bit, 1:16 bit

#define V8_WriteMul8Addr(addr)  \
{\
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((addr & 0xff000000) >> 24)); \
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((addr & 0xff0000) >> 16)); \
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((addr & 0xff00) >> 8)); \
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, (addr & 0xff)); \
}

#define V8_WriteMul8Dat(dat)    \
{\
    HOSTSETVAL8(MMD_HOSTSIDE_VALUE_ADDR, ((dat & 0xff000000) >> 24)); \
    HOSTSETVAL8(MMD_HOSTSIDE_VALUE_ADDR, ((dat & 0xff0000) >> 16)); \
    HOSTSETVAL8(MMD_HOSTSIDE_VALUE_ADDR, ((dat & 0xff00) >> 8)); \
    HOSTSETVAL8(MMD_HOSTSIDE_VALUE_ADDR, (dat & 0xff)); \
}

#define V8_ReadMul8Dat(dat) \
{\
    dat =  HOSTGETVAL8(MMD_HOSTSIDE_VALUE_ADDR); \
    dat <<= 8; \
    dat += HOSTGETVAL8(MMD_HOSTSIDE_VALUE_ADDR); \
    dat <<= 8; \
    dat += HOSTGETVAL8(MMD_HOSTSIDE_VALUE_ADDR); \
    dat <<= 8; \
    dat += HOSTGETVAL8(MMD_HOSTSIDE_VALUE_ADDR); \
}

#define V8_WriteMul16Addr(addr) \
{\
    HOSTSETADDR16(MMD_HOSTSIDE_INDEX_ADDR, (MMD_U32)((addr & 0xffff0000) >> 16)); \
    HOSTSETADDR16(MMD_HOSTSIDE_INDEX_ADDR, (MMD_U32)(addr & 0xffff)); \
}

#define V8_WriteMul16Dat(dat)   \
{\
    HOSTSETVAL16(MMD_HOSTSIDE_VALUE_ADDR, (MMD_U32)((dat & 0xffff0000) >> 16)); \
    HOSTSETVAL16(MMD_HOSTSIDE_VALUE_ADDR, (MMD_U32)(dat & 0xffff)); \
}

#define V8_ReadMul16Dat(dat) \
{\
    dat = HOSTGETVAL16(MMD_HOSTSIDE_VALUE_ADDR); \
    dat <<= 16; \
    dat += HOSTGETVAL16(MMD_HOSTSIDE_VALUE_ADDR); \
}
#if 0
/* Exclusive process macro */
#define BIU_BEGIN_EX {                                 \
	MMD_USER_LockIRQFromChip();                    \
	MMD_USER_OS_GetResource(MMD_OS_BIU_INDEXRESOURCE); \
}

#define BIU_END_EX {                                   \
	MMD_USER_OS_RelResource(MMD_OS_BIU_INDEXRESOURCE); \
	MMD_USER_UnlockIRQFromChip();                  \
}
#else
/* Exclusive process macro */
#define BIU_BEGIN_EX 
#define BIU_END_EX 
#endif

#define READ_DELAY_COUNT     100
#define BIU_STATUS_TIMEOUT		1000


 void Sram_Read_Delay(MMD_U32 cnt)
{
    while(cnt--);
}

 void    V8_Mul8SetReg(MMD_U32 addr, MMD_U32 dat)
{
    BIU_BEGIN_EX
    V8_Mul8SetReg_NEX(addr,dat);
    BIU_END_EX
}

//Multiplex 8 bit mode access
 void    V8_Mul8SetReg_NEX(MMD_U32 addr, MMD_U32 dat)
{
    // in mpw version
    //Address must 4 byte align
    addr >>= 2;
    addr <<= 2;
    addr += 1;//bit 0 of address set 1 to inform write

    V8_WriteMul8Addr(addr)
    V8_WriteMul8Dat(dat)
}

 MMD_U32  V8_Mul8GetReg(MMD_U32 addr)
{
    MMD_U32 dat;
	
    BIU_BEGIN_EX
    dat=V8_Mul8GetReg_NEX(addr);
    BIU_END_EX
    return dat;
}

 MMD_U32  V8_Mul8GetReg_NEX(MMD_U32 addr)
{
    MMD_U32 dat;

    //Address must 4 byte align
    addr >>= 2;
    addr <<= 2;

    V8_WriteMul8Addr(addr)
    Sram_Read_Delay(READ_DELAY_COUNT);
    V8_ReadMul8Dat(dat)

    return dat;
}

 MMD_U32  V8_Mul8WrSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize;
    BIU_BEGIN_EX
    actsize=V8_Mul8WrSram_NEX(addr, pbuf,size);
    BIU_END_EX
    return actsize;
}

 MMD_U32  V8_Mul8WrSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize, i;
    MMD_U32 tmp;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;

    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul8GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }

    //Set burst base address
    V8_Mul8SetReg_NEX(REG_BIU_BASE_ADDR, addr + 1);//bit 0 of address set 1 to inform write
    //Set burst length
    V8_Mul8SetReg_NEX(REG_BIU_BURST_LEN, size);//word size

    
    //Write data continously
    while(size--)
    {
        tmp = *(MMD_U32*)pbuf;
        V8_WriteMul8Dat(tmp)
        pbuf += 4;
    }
	
    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul8GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }

    return actsize;
}
 MMD_U32  V8_Mul8RdSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize;
    BIU_BEGIN_EX
    actsize=V8_Mul8RdSram_NEX(addr, pbuf,size);
    BIU_END_EX
    return actsize;
}

 MMD_U32  V8_Mul8RdSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize,i;
    MMD_U32 tmp;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;

    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul8GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }
    //Set burst base address
    V8_Mul8SetReg_NEX(REG_BIU_BASE_ADDR, addr);
    //Set burst length
    V8_Mul8SetReg_NEX(REG_BIU_BURST_LEN, size);//word size


    //Read data continously
    while(size--)
    {
        V8_ReadMul8Dat(tmp)
        *(MMD_U32*)pbuf = tmp;
        pbuf += 4;
    }

    return actsize;
}

 void    V8_Mul8SetBiuHdFlag(MMD_U32 flag)
{
    BIU_BEGIN_EX
    V8_Mul8SetBiuHdFlag_NEX(flag);		
    BIU_END_EX
}


 void    V8_Mul8SetBiuHdFlag_NEX(MMD_U32 flag)
{
    //V8_WriteMul8Addr(flag)
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((flag & 0xff000000) >> 24));
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((flag & 0xff0000) >> 16));
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, ((flag & 0xff00) >> 8));
    HOSTSETADDR8(MMD_HOSTSIDE_INDEX_ADDR, (flag & 0xff));
}

//Multiplex 16 bit mode access
 void    V8_Mul16SetReg(MMD_U32 addr, MMD_U32 dat)
{
    BIU_BEGIN_EX
    V8_Mul16SetReg_NEX(addr, dat);
    BIU_END_EX		
}

//Multiplex 16 bit mode access
 void    V8_Mul16SetReg_NEX(MMD_U32 addr, MMD_U32 dat)
{
    //Address must 4 byte align
    addr >>= 2;
    addr <<= 2;
    addr += 1;//bit 0 of address set 1 to inform write

    V8_WriteMul16Addr(addr)
    V8_WriteMul16Dat(dat)
}

 MMD_U32  V8_Mul16GetReg(MMD_U32 addr)
{
    MMD_U32 dat;
	
    BIU_BEGIN_EX
    dat=V8_Mul16GetReg_NEX(addr);
    BIU_END_EX
		
    return dat;
}

 MMD_U32  V8_Mul16GetReg_NEX(MMD_U32 addr)
{
    MMD_U32 dat;

    //Address must 4 byte align
    addr >>= 2;
    addr <<= 2;

    V8_WriteMul16Addr(addr)
    Sram_Read_Delay(READ_DELAY_COUNT);
    V8_ReadMul16Dat(dat)

    return dat;
}
 MMD_U32  V8_Mul16WrSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize;
	
    BIU_BEGIN_EX
    actsize=V8_Mul16WrSram_NEX(addr, pbuf, size);
    BIU_END_EX
    
    return actsize;
}

 MMD_U32  V8_Mul16WrSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize,i;
    MMD_U32 tmp;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;

    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }
    //Set burst base address
    V8_Mul16SetReg_NEX(REG_BIU_BASE_ADDR, addr + 1);//bit 0 of address set 1 to inform write
    //Set burst length
    V8_Mul16SetReg_NEX(REG_BIU_BURST_LEN, size);//word size


    //Write data continously
    while(size--)
    {
        tmp = *(MMD_U32*)pbuf;
        V8_WriteMul16Dat(tmp)
        pbuf += 4;
    }
     //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }
    return actsize;
}

MMD_U32  V8_Mul16WrSramOnWord(MMD_U32 addr, MMD_U16 data, MMD_U32 size)
{
    MMD_U32 actsize;
	
    BIU_BEGIN_EX
    actsize=V8_Mul16WrSram_NEX_OnWord(addr, data, size);
    BIU_END_EX
    
    return actsize;
}

 MMD_U32  V8_Mul16WrSram_NEX_OnWord(MMD_U32 addr, MMD_U16 data, MMD_U32 size)
{
	
	MMD_U32 actsize,i;
    MMD_U32 tmp;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;
	tmp=((MMD_U32)data<<16)|data;
    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
		if(i==0)
		{
           MMD_PRINTFE(("BIU status time out!\n"));
	    	return 0; 
		}	
   	}
    //Set burst base address
    addr|=1;	//write enable
    V8_Mul16SetReg_NEX(REG_BIU_BASE_ADDR, addr);//bit 0 of address set 1 to inform write
    //Set burst length
    V8_Mul16SetReg_NEX(REG_BIU_BURST_LEN, size);//word size

    //Write data continously
    while(size--)
    {
        V8_WriteMul16Dat(tmp);
        
    }
     //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
		if(i==0)
		{
           MMD_PRINTFE(("BIU status time out!\n"));
	    	return 0; 
		}	
   }
    return actsize;
}
 MMD_U32  V8_Mul16RdSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize;

    BIU_BEGIN_EX
    actsize=V8_Mul16RdSram_NEX(addr, pbuf, size);
    BIU_END_EX

    return actsize;
}

 MMD_U32  V8_Mul16RdSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize,i;
    MMD_U32 tmp;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;
    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMICRS(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }
    //Set burst base address
    V8_Mul16SetReg_NEX(REG_BIU_BASE_ADDR, addr);
    //Set burst length
    V8_Mul16SetReg_NEX(REG_BIU_BURST_LEN, size);//word size


    //Write data continously
    while(size--)
    {
        V8_ReadMul16Dat(tmp)
        *(MMD_U32*)pbuf = tmp;
        pbuf += 4;
    }

    return actsize;
}
/*MMD_U32  V8_Mul16RdSramForCap(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize;

    BIU_BEGIN_EX
    actsize=V8_Mul16RdSram_NEXForCap(addr, pbuf, size);
    BIU_END_EX

    return actsize;
}

 MMD_U32  V8_Mul16RdSram_NEXForCap(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size)
{
    MMD_U32 actsize,i;
    MMD_U32 tmp;
	//MMD_U32 stime,etime;

    //Address and size must 4 byte align
    addr >>= 2;
    addr <<= 2;
    size >>= 2;
    actsize = size << 2;
    //Wait ready or time out
    i=BIU_STATUS_TIMEOUT;
    while(!(V8_Mul16GetReg_NEX(REG_BIU_STATUS) & 0x1))
    {
    	MMD_USER_WaitMSEC(1);
    	i--;
	if(i==0)
	{
           MMD_PRINTFE(("BIU status time out!\n"));
	    return 0; 
	}	
   }
    //Set burst base address
    V8_Mul16SetReg_NEX(REG_BIU_BASE_ADDR, addr);
    //Set burst length
    V8_Mul16SetReg_NEX(REG_BIU_BURST_LEN, size);//word size

	//stime=VIM_USER_GetSystemTime_1us();

    //Write data continously
    while(size--)
    {
        V8_ReadMul16Dat(tmp)
        *(MMD_U32*)pbuf = tmp;
        pbuf += 4;
    }
	//etime=VIM_USER_GetSystemTime_1us();

	//MMDPrintf("\n[VC0578B][MID] V8_Mul16RdSram_NEX() read %d count,use %d us\n",actsize,etime-stime);


    return actsize;
}*/
 void    V8_Mul16SetBiuHdFlag(MMD_U32 flag)
{
    BIU_BEGIN_EX
    V8_Mul16SetBiuHdFlag_NEX(flag);		
    BIU_END_EX
}

 void    V8_Mul16SetBiuHdFlag_NEX(MMD_U32 flag)
{
    V8_WriteMul16Addr(flag)
}



MMD_U32 MMD_BIU_IRQGetStatusAll_NEX(void)
{
    return 0;
}

MMD_U32 MMD_BIU_IRQGetEnableAll_NEX(void)
{
    return 0;
}

MMD_U32 MMD_BIU_IRQGetStatus_NEX(void)
{
    return 0;
}
void    MMD_BIU_AssertIRQ_NEX(void)
{
    Vm_SetReg_NEX(DAVINCI_SIB2A_ST, 1<<0);
    // must added delay here, don't know reason
    //MMD_USER_WaitMSEC(10);
}
// temp use, remove later
void Host_HIF_SetReg8(MMD_U32 adr, MMD_U08 val)       
{
	MMD_U32 temp_val;

	temp_val = Vm_GetReg(adr);
	
	if((adr & 0x3) == 0)
	{
		Vm_SetReg(adr, ((temp_val&0xffffff00)|(MMD_U32)val));
	}
	else if((adr & 0x3) == 1)
	{
		Vm_SetReg(adr, ((temp_val&0xffff00ff)|(MMD_U32)val<<8));
	}
	else if((adr & 0x3) == 2)
	{
		Vm_SetReg(adr, ((temp_val&0xff00ffff)|(MMD_U32)val<<16));
	}	
	else if((adr & 0x3) == 3)
	{
		Vm_SetReg(adr, ((temp_val&0x00ffffff)|(MMD_U32)val<<24));
	}	
}

MMD_U08 Host_HIF_GetReg8(MMD_U32 adr)       
{
	MMD_U32 temp_val;
	MMD_U08 ret;

	temp_val = Vm_GetReg(adr);
	
	if((adr & 0x3) == 0)
	{
		ret = (temp_val & 0xff);
	}
	else if((adr & 0x3) == 1)
	{
		ret = (temp_val & 0x0000ff00)>>8;
	}
	else if((adr & 0x3) == 2)
	{
		ret = (temp_val & 0x00ff0000)>>16;
	}	
	else if((adr & 0x3) == 3)
	{
		ret = (temp_val & 0xff000000)>>24;
	}	
	return ret;
}

void Host_HIF_SetReg16(MMD_U32 adr, MMD_U16 val)       
{
	MMD_U32 temp_val;

	temp_val = Vm_GetReg(adr);
	
	if((adr & 0x3) == 0)
	{
		Vm_SetReg(adr, ((temp_val&0xffff0000)|(MMD_U32)val));
	}
	else if((adr & 0x3) == 2)
	{
		Vm_SetReg(adr, ((temp_val&0x0000ffff)|(MMD_U32)val<<16));
	}
	else
	{
		MMD_PRINTFE(("illegal SetReg16(0x%x,0x%x)\r\n",adr,val));
		while(1);
	}	
}

MMD_U16 Host_HIF_GetReg16(MMD_U32 adr)       
{
	MMD_U32 temp_val;
	MMD_U16 ret;

	temp_val = Vm_GetReg(adr);
	
	if((adr & 0x3) == 0)
	{
		ret = (temp_val & 0xffff);
	}
	else if((adr & 0x3) == 2)
	{
		ret = (temp_val & 0xffff0000)>>16;
	}
	else
	{
		MMD_PRINTFE(("illegal GetReg16(0x%x)\r\n",adr));
		while(1);
	}	

	return ret;
}


void Host_HIF_SetReg32(MMD_U32 adr, MMD_U32 val)       
{
	Vm_SetReg(adr,val);
}

MMD_U32 Host_HIF_GetReg32(MMD_U32 adr)       
{
	return Vm_GetReg(adr);
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_BIU_Init
[DESCRIPTION]
  This routine is used to set data width at BIU unit.
[ARGUMENT]
  width: used to indicate width which of it value can be 8 bit or 16 bit .
  0: 8 bit
  1: 16 bit
[RETURN VALUE]
  MMD_RET_NOERR        : Success
  MMD_RET_ERR          : Fail
-----------------------------------------------------------------------------*/
void MMD_BIU_Init(MMD_U32 data_width)
{
	MMD_U32 temp,data_delay=0x1B,cs_delay=0x16;

	//Set BIU width to 16bit
	if( data_width )
	{
		if(!g_biu_width)
		{
			temp=V8_Mul8GetReg(0x60000004);
			temp|=(0x01<<24)|(data_delay<<8)|cs_delay;
			V8_Mul8SetReg(0x60000004, temp);
			g_biu_width=1;
		}	
	}
	else
	{
		if(g_biu_width)
		{
			temp=V8_Mul16GetReg(0x60000004);
			temp&=~(0x01<<24);
			temp|=(data_delay<<8)|cs_delay;
			V8_Mul16SetReg(0x60000004, temp);
			g_biu_width=0;
		}
	}	

}

