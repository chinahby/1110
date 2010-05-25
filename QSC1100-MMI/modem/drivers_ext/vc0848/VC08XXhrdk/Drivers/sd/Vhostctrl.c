#if 0 //cje
#include "def.h"
#include "platform.h"
#endif

#include "mmp_rdk.h"

#include "Vhostctrl.h"

void Vhost_WriteReg(MMD_U32 dAddr, MMD_U32 dData)
{
	Vm_SetReg(dAddr, dData);
}

MMD_U32 Vhost_ReadReg(MMD_U32 dAddr)
{
	return Vm_GetReg(dAddr);
}

void Vhost_Delay(MMD_U32 cnt)
{
#if 1
	MMD_U32 i;
	for (i = 0; i < cnt; i++) {
		/* delay 1 ms */
		MMD_USER_WaitMICRS(10);
	}
#else	
	MMD_U32 t1=0,t2=0;
	for(t1=0; t1<cnt; )
	{	for(t2=0; t2 < 0xff; )
		{t2++;}
		t1++;
	}
#endif	
}


#if 0
#if (CURSYSTEM == AASP||CURSYSTEM == ECOS)
#include <hal_io.h>
#endif


static UINT32 g_Ticks=0;
#if (BIU_MODE == 1)
static UINT32 gVirBIUAddr=HAL_ADDR, gVirBIUData=HAL_DATA;
void 	BiuInit(UINT32 addr, UINT32 data)
{
	gVirBIUAddr = addr;
	gVirBIUData = data;
}
void 	V820Reset(void)
{
//smem 0x60010004 0xffc00000 32
	HAL_WRITEv8(0x60010004,0xffc00000);
//com all clocks are enable
//smem 0x60010008  0x00000000 32
	HAL_WRITEv8(0x60010008,0x00000000);
//com nfc swreset
//smem 0x60010100  0x20000800  32     
	HAL_WRITEv8(0x60010100,0x20000800);                
//smem 0x60010100  0x20000000  32
	HAL_WRITEv8(0x60010100,0x20000000);

//com  marb reset
//smem 0x60040018 0x00070000  32       
	HAL_WRITEv8(0x60040018,0x00070000);
//smem 0x60040018 0x00000000  32 
	HAL_WRITEv8(0x60040018,0x00000000);
	
	//umonitor
	//HAL_WRITEv8(0x6001013c,0x681);
	//HAL_WRITEv8(0x6001013c,0x115);
	//HAL_WRITEv8(0x6001013c,0x80);

}
/* addr 00 byte2 00 byte3
*  addr 00 byte0 00 byte1
*/
UINT32 HAL_READv8(UINT32 addr)
{
	UINT32 t,t1,t2,t3;
	t = (addr>>24) & 0xff;	//byte 3
	*(UINT16 *)gVirBIUAddr = t;
	t = (addr>>16)&0xff;
	*(UINT16 *)gVirBIUAddr = t;//byte 2
	t = (addr>>8) & 0xff; //byte 1
	*(UINT16 *)gVirBIUAddr = t;
	t = (addr)&0xff;		//byte 0
	*(UINT16 *)gVirBIUAddr = t;
	
	Vhost_Delay(30);
	t = *(UINT16 *)gVirBIUData;	//byte 3
	t1 = *(UINT16 *)gVirBIUData;	//byte  2
	t2 = *(UINT16 *)gVirBIUData;	//byte 1
	t3 = *(UINT16 *)gVirBIUData;	//byte 0
	t = (t<<24) | ((t1<<16)&0xff0000) | ((t2<<8)&0xff00) | (t3&0xff);
	return t;
}
/* addr  byte3  byte2
*  addr  byte1  byte0
*/
UINT32 HAL_READv16(UINT32 addr)
{
	UINT32 t,t1;
	t = (addr>>16) & 0xffff;	//byte 3 2
	*(UINT16 *)gVirBIUAddr = t;
		
	t = (addr) & 0xffff; //byte 1 0
	*(UINT16 *)gVirBIUAddr = t;
	
	Vhost_Delay(1);
	t = *(UINT16 *)gVirBIUData;	//byte 3 2
	
	t1 = *(UINT16 *)gVirBIUData;	//byte  1 0
	
	t = (t<<16) | ((t1)&0xffff);
	return t;
}

/* addr 00 byte2 00 byte3
*  addr 00 byte0 00 byte1
*/

void 	HAL_WRITEv8(UINT32 addr, UINT32 data)
{
	UINT32 t;

	t= (addr>>24) & 0xff;	//byte 3
	*(UINT16 *)gVirBIUAddr = t;
		t = (addr>>16)&0xff;
	
	*(UINT16 *)gVirBIUAddr = t;//byte 2
	
	t = (addr>>8) & 0xff; //byte 1
	*(UINT16 *)gVirBIUAddr = t;
	t = (addr)&0xff;		//byte 0
	*(UINT16 *)gVirBIUAddr = t;
	
	Vhost_Delay(3);
	t = (data>>24)&0xff;//byte3 
	*(UINT16 *)gVirBIUData = t;
	t = (data>>16)&0xff;//byte2 
	*(UINT16 *)gVirBIUData = t;
	t = (data&0xff00)>>8;//byte 1
	*(UINT16 *)gVirBIUData = t;
	t = data&0xff;//byte 0
	*(UINT16 *)gVirBIUData = t;
	Vhost_Delay(30);
}
/*
addr  byte3  byte2
*  addr  byte1  byte0
*/
void 	HAL_WRITEv16(UINT32 addr, UINT32 data)
{
	UINT32 t;

	t = (addr>>16) & 0xffff;	//byte 3 2
	*(UINT16 *)gVirBIUAddr = t;
		
	t = (addr) & 0xffff; //byte 1 0
	*(UINT16 *)gVirBIUAddr = t;
	
	t = (data>>16)&0xffff;//byte3 2
	*(UINT16 *)gVirBIUData = t;
	t = (data)&0xffff;//byte1 0
	*(UINT16 *)gVirBIUData = t;
	
	
	
}

 int  V8_Mul8WrSram(int addr, char *pbuf ,int size)
{
	SINT32 actsize;
	UINT32 tmp;
	UINT32 t;
	
	//Address and size must 4 byte align
	addr >>= 2;
	addr <<= 2;
	size >>= 2;
	actsize = size << 2;

	while(!(HAL_READv8(REG_BIU_STATUS) & 0x1));//Wait untile ready

	//Set burst base address
	HAL_WRITEv8(REG_BIU_BASE_ADDR, addr + 1);//bit 0 of address set 1 to inform write
	//Set burst length
	HAL_WRITEv8(REG_BIU_BURST_LEN, size);//word size

	
	//Write data continously
	while(size--)
	{
		tmp = *(int*)pbuf;
		
		t = (tmp>>24)&0xff;//byte3 
		*(UINT16 *)gVirBIUData = t;
		t = (tmp>>16)&0xff;//byte2 
		*(UINT16 *)gVirBIUData = t;
		t = (tmp&0xff00)>>8;//byte 1
		*(UINT16 *)gVirBIUData = t;
		t = tmp&0xff;//byte 0
		*(UINT16 *)gVirBIUData = t;
		pbuf += 4;
	}

	return actsize;
}
 int  V8_Mul8RdSram(int addr,char *pbuf ,int size )
{
	SINT32 actsize;
	UINT32 t,t1,t2,t3;

	//Address and size must 4 byte align
	addr >>= 2;
	addr <<= 2;
	size >>= 2;
	actsize = size << 2;

	while(!(HAL_READv8(REG_BIU_STATUS) & 0x1));//Wait untile ready

	//Set burst base address
	HAL_WRITEv8(REG_BIU_BASE_ADDR, addr);
	//Set burst length
	HAL_WRITEv8(REG_BIU_BURST_LEN, size);//word size

	Vhost_Delay(3);

	//Read data continously
	while(size--)
	{
		t = *(UINT16 *)gVirBIUData;	//byte 3
		t1 = *(UINT16 *)gVirBIUData;	//byte  2
		t2 = *(UINT16 *)gVirBIUData;	//byte 1
		t3 = *(UINT16 *)gVirBIUData;	//byte 0
		t = (t<<24) | ((t1<<16)&0xff0000) | ((t2<<8)&0xff00) | (t3&0xff);
		
		*(int*)pbuf = t;
		pbuf += 4;
	}

	return actsize;
}

#endif
/*

VOID Vhost_WriteReg(UINT32 dAaddr, UINT32 dData)
{
#if (BIU_MODE ==	1)
	HAL_WRITEv8(dAaddr,dData);
#else
	HAL_WRITE_UINT32(dAaddr, dData);
#endif
	
}

UINT32	Vhost_ReadReg(UINT32 dAddr)
{
	UINT32 data;
#if (BIU_MODE ==	1)
	data = HAL_READv8(dAddr);
#else
	 HAL_READ_UINT32(dAddr, data);
#endif
	 return data;
}
*/
VOID Vhost_MainIsr()
{}

VOID HOSTSETADDR8(volatile UINT8 *_target,UINT32  _data) 
{
	*_target = _data ;
}
VOID HOSTSETVAL8(volatile UINT8 *_target,UINT32  _data)
{
{
	*_target = _data ;
}
}
UINT32 HOSTGETVAL8(volatile UINT8 *_src)  
{
	UINT32 data;
	data = *_src ;
	return data;
}

VOID Vhost_Delay( UINT32 cnt)
{
#if (CURSYSTEM != LINUX)
	#if 0
	AASP_DELAY(cnt);
	#else
	UINT32 t1=0,t2=0;
	for(t1=0; t1<cnt; )
	{	for(t2=0; t2 < 0x41; )
		{
			t2++;
			_HAL_NOP;
		}
		t1++;
		
	}
	#endif
#else
	udelay(cnt);
#endif
}

VOID Vhost_memcpy(VOID *dest,VOID *src, UINT32 len)
{
	#if 0
	UINT32 i,*t,*s;
	t= (UINT32 *)dest;
	s= (UINT32 *)src;
	if((!(UINT32)dest &0xf)&&((UINT32)src&0xf)&&(len>16))
		{
			for(i=0;i<len/16;i++)
				{
					*((UINT32 *)t++)=*((UINT32 *)s++);
					*((UINT32 *)t++)=*((UINT32 *)s++);
					*((UINT32 *)t++)=*((UINT32 *)s++);
					*((UINT32 *)t++)=*((UINT32 *)s++);
				}
		}
	else
	#endif
		{
		memcpy(dest,src,len);
		}
}

VOID Vhost_memset(VOID *dest,UINT8 data,UINT32 len)
{
	#if 0
	UINT32 newdata,i,*t;
	t= (UINT32 *)dest;
	newdata=data;
	newdata+=(newdata<<8) + (newdata<<16) + (newdata<<24);
	if((!(UINT32)dest &0xf)&&(len>16))
		{
				for(i=0;i<len/16;i++)
				{
					*((UINT32 *)t++)=newdata;
					*((UINT32 *)t++)=newdata;
					*((UINT32 *)t++)=newdata;
					*((UINT32 *)t++)=newdata;
				}
		}
	else
	#endif
		{
		memset(dest,data,len);
		}
}
UINT32 Vhost_rand(void)
{
	return rand();
}

void Vhost_srand(void)
{
	srand();
}

int Vhost_printf()
{
	return 0;
}

UINT32 platform_get_ticks(VOID)
{
	g_Ticks+=1000;
	Vhost_Delay(1);
	return g_Ticks;
}


VOID oem_getsysdate(UINT16 *data,UINT16 *time)
{
	*data=0x0610;
	*time=0x34;
	return;
}

VOID STD_itoa(UINT32 data, UINT8 *buffer, UINT8 mode)

{
	UINT32 t,i=0,j=0;
	UINT8 tbuf[8];
	if(data==0)
		{
		buffer[0]='0';
		buffer[1]=0;
		}
	while(data >0)
	{
		t=data%mode;
		tbuf[i]=t+'0';
		i++;
		data-=t;
		data/=mode;
	}
	buffer[i]=0;
	while(i!=0)
		{
			buffer[i-1]=tbuf[j];
			i--;
			j++;
		}
}

char STD_getch(void)
{
	UINT32 reg;
	reg=Vhost_ReadReg(0x60121024);
	if((reg &0x1)!=1)
		return 0;
	reg=Vhost_ReadReg(0x60121000);
	return (char)reg;
}
#if defined(__ECOS__)

#if 0
int
iprintf( const char *name, ... ) 
{
	
	MMDPrintf( name  );
}
#endif
#endif

#endif

