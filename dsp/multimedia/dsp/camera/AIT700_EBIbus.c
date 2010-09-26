/**
 * @file AIT700_EBIbus.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note Data bus interface between Host and AIT chip. (Address, CS time, Control pin)
 * @bug N/A
 */
#define  DEFINE_GSBINDDATAPORT  
#include "AIT700_ebibus.h"
#undef  DEFINE_GSBINDDATAPORT  

#include "sys_IF_ait_api.h"
#if defined(__QSC_TARGET__)

#endif

u_short gA8HostBusMode; 
u_short gA8FIFOAddr;

#pragma O0

//=====================================================================//
void SetHostBusMode()
{
}

#if defined(MTK_PLATFORM)
#pragma arm section code = "INTERNCODE"
#endif
#ifdef	EBI_BUS_8BIT_MODE

#define HUGE

void	SetA8RegW(u_short addr, u_short data)
{
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 8);
	
	A8IndCmdP = HII_RW_REG_B_INC;
	A8IndDataPB(0) = (u_char)(data & 0xFF);
	A8IndDataPB(0) = (u_char)(data >> 8);
}
/*=====================================================================*/
void	SetA8RegB(u_short addr, u_char data)
{
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 8);

	A8IndCmdP = HII_RW_REG_B;
	A8IndDataPB(0) = (u_char)(data & 0xFF);
}
/*=====================================================================*/
u_short	GetA8RegW(u_short addr)
{
	u_short temp1, temp2;

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 8);
	
	A8IndCmdP = HII_RW_REG_B_INC;
	temp1 = A8IndDataPB(0);
	temp2 = A8IndDataPB(0);
	
	return (temp1+(temp2<<8));
}
/*=====================================================================*/
u_char	GetA8RegB(u_short addr)
{
	u_short temp1;

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 8);
	
	A8IndCmdP = HII_RW_REG_B;
	temp1 = A8IndDataPB(0);

	return (temp1);
}
/*=====================================================================*/
void	SetA8MemW(u_int addr, u_short data)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char)((addr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 24);
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)((addr >> 8) & 0xFF);
	
	A8IndCmdP = HII_RW_MEM_B_INC;
	A8IndDataPB(0) = (u_char)(data & 0xFF);
	A8IndDataPB(0) = (u_char)(data >> 8);
}
/*=====================================================================*/
void	SetA8MemB(u_int addr, u_char data)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char)((addr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 24);
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)((addr >> 8) & 0xFF);
	
	A8IndCmdP = HII_RW_MEM_B;
	A8IndDataPB(0) = (u_char)(data & 0xFF);
}
/*=====================================================================*/
u_short	GetA8MemW(u_int addr)
{
	u_short temp1, temp2;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char)((addr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 24);
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)((addr >> 8) & 0xFF);
	
	A8IndCmdP = HII_RW_MEM_B_INC;
	temp1 = A8IndDataPB(0);
	temp2 = A8IndDataPB(0);

	return (temp1+(temp2<<8));
}
/*=====================================================================*/
u_char	GetA8MemB(u_int addr)
{
	u_short temp1;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char)((addr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 24);
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)((addr >> 8) & 0xFF);
	
	A8IndCmdP = HII_RW_MEM_B;
	temp1 = A8IndDataPB(0);

	return (temp1);
}
/*=====================================================================*/
void	SetA8MemAddr(u_int addr)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char)((addr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char)(addr >> 24);
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(addr & 0xFF);
	A8IndDataPB(0) = (u_char)((addr >> 8) & 0xFF);
}
/*=====================================================================*/
void	SetA8MemNextW(u_short data)
{
	A8IndCmdP = HII_RW_MEM_B_INC;
	A8IndDataPB(0) = (u_char)(data & 0xFF);
	A8IndDataPB(0) = (u_char)(data >> 8);
}
/*
//=====================================================================//
//void	SetA8MemNextB(u_char data)
//{
//	A8IndCmdP = HII_RW_MEM_B_INC;
//	A8IndDataPB(0) = data; 
//}
//=====================================================================//
*/
u_short	GetA8MemNextW()
{
	u_short temp1, temp2;

	A8IndCmdP = HII_RW_MEM_B_INC;
	temp1 = A8IndDataPB(0);
	temp2 = A8IndDataPB(0);

	return (temp1+(temp2<<8));
}
/*
//=====================================================================//
//u_char	GetA8MemNextB()
//{
//	A8IndCmdP = HII_RW_MEM_B_INC;
//	return A8IndDataPB(0);
//}
//=====================================================================//
*/
void	CopyMemByteHostToA8(u_int destAddr, u_char HUGE *srcAddr, u_int length)
{
	register u_char HUGE *AITTmpAddr;
	register u_char HUGE *AITTmpAddrLength;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char) ((destAddr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char) (destAddr >> 24);

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (destAddr & 0xFF);
	A8IndDataPB(0) = (u_char) ((destAddr >> 8) & 0xFF);

	A8IndCmdP = HII_RW_MEM_B_INC;
	AITTmpAddr = srcAddr;
	AITTmpAddrLength = srcAddr + length;

	do {
		*(gsbIndDataPortB) = *AITTmpAddr++;
	} while(AITTmpAddr < AITTmpAddrLength);
}
/*=====================================================================*/
void	CopyMemByteA8ToHost(u_char HUGE *destAddr, u_int srcAddr, u_int length)
{
	register u_char HUGE *AITTmpAddr;
	register u_char HUGE *AITTmpAddrLength;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char) ((srcAddr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char) (srcAddr >> 24);

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (srcAddr & 0xFF);
	A8IndDataPB(0) = (u_char) ((srcAddr >> 8) & 0xFF);

	A8IndCmdP = HII_RW_MEM_B_INC;
	AITTmpAddr = destAddr;
	AITTmpAddrLength = destAddr + length;
	do {
		*AITTmpAddr++ = *gsbIndDataPortB;
	} while(AITTmpAddr < AITTmpAddrLength);
}
/*=====================================================================// */
void	CopyMemWordHostToA8(u_int destAddr, u_short HUGE *srcAddr, u_int length)
{
	u_char HUGE *tmpSrcAddr;
	register u_char HUGE *AITTmpAddr;

	tmpSrcAddr = (u_char HUGE *) srcAddr;
	AITTmpAddr = tmpSrcAddr + length * 2;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char) ((destAddr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char) (destAddr >> 24);

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (destAddr & 0xFF);
	A8IndDataPB(0) = (u_char) ((destAddr >> 8) & 0xFF);

	A8IndCmdP = HII_RW_MEM_B_INC;
	do {
		*(gsbIndDataPortB) = *(tmpSrcAddr++);
		*(gsbIndDataPortB) = *(tmpSrcAddr++);
	} while(tmpSrcAddr < AITTmpAddr);
}
/*=====================================================================*/
void	CopyMemWordA8ToHost(u_short HUGE *destAddr, u_int srcAddr, u_int length)
{
	u_char HUGE *tmpDestAddr;
	register u_char HUGE *AITTmpAddr;

	tmpDestAddr = (u_char HUGE *) destAddr;
	AITTmpAddr = tmpDestAddr + length;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char) ((srcAddr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char) (srcAddr >> 24);

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (srcAddr & 0xFF);
	A8IndDataPB(0) = (u_char) ((srcAddr >> 8) & 0xFF);

	A8IndCmdP = HII_RW_MEM_B_INC;

	do {
		*(tmpDestAddr++) = *gsbIndDataPortB;
	} while(tmpDestAddr < AITTmpAddr);
}
/*=====================================================================*/
void	ClearA8Mem(u_int destAddr, u_short data, u_int length )
{
	register u_int i = 0;
	u_int RealLen;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPB(0) = (u_char) ((destAddr >> 16) & 0xFF);
	A8IndDataPB(0) = (u_char) (destAddr >> 24);

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (destAddr & 0xFF);
	A8IndDataPB(0) = (u_char) ((destAddr >> 8) & 0xFF);

	RealLen = (length+1) >> 1;
	A8IndCmdP = HII_RW_MEM_B_INC;
	do {
		*(gsbIndDataPortB) = (u_char) (data & 0xFF);
		*(gsbIndDataPortB) = (u_char) ((data >> 8) & 0xFF);
		
		i++;
	} while(i < RealLen);
}
/*=====================================================================*/
void OpenFIFO(u_short fifoAddr)
{
	gA8FIFOAddr = fifoAddr;

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char) (fifoAddr & 0xFF);
	A8IndDataPB(0) = (u_char) (fifoAddr >> 8);   
	
	A8IndCmdP = HII_RW_REG_W;
}
/*=====================================================================*/
void PutWordFIFO(u_short data)
{
	//A8IndCmdP = HII_RW_REG_W;
#if defined(AIT_BIG_ENDIAN)
	A8IndDataPB(0) = (u_char) (data >> 8);
	A8IndDataPB(0) = (u_char) (data & 0xFF);
#else
	A8IndDataPB(0) = (u_char) (data & 0xFF);
   	A8IndDataPB(0) = (u_char) (data >> 8);
#endif
}
/*=====================================================================*/
u_short GetWordFIFO()
{
	register u_short temp1, temp2;

#if defined(AIT_BIG_ENDIAN)
    	temp1 = A8IndDataPB(0);
    	temp2 = A8IndDataPB(0);

	return (temp2 + (temp1 << 8));
#else
	//A8IndCmdP = HII_RW_REG_W;
    	temp1 = A8IndDataPB(0);
    	temp2 = A8IndDataPB(0);

	return (temp1 + (temp2 << 8));
#endif	
}
/*=====================================================================*/
short	SendA8Cmd(u_short cmd)
{
//	u_short	i;
	u_int   timeout = 0;
	
	SetA8RegB(A8_HOST_CMD_RG + 1, (cmd >> 8));
	SetA8RegB(A8_HOST_CMD_RG, (cmd & 0xFF));

	GetA8RegW(A8_HOST_CMD_RG);       // Add dummy read for ESD
	while(GetA8RegW((A8_HOST_CMD_RG)) && timeout < 0x2000)
	{
	   	sys_IF_ait_delay1us(100);
		timeout++;
	}
	
	//if (timeout < CMD_READY_TIME_OUT) {
	if (timeout < 0x2000)
	{
		return  0;
	}
	
    	return 1;
}

u_short SetA8GrpFIFODataW(u_short **pshorttmp)
{
    u_short GrapData,pGrapData;

    GrapData = *((*pshorttmp)++);
    GrapData = ((GrapData&0xe000)>>8)|((GrapData&0x700)>>6)|((GrapData&0x18)>>3);
    pGrapData = *((*pshorttmp)++);
    pGrapData = ((pGrapData&0xe000))|((pGrapData&0x700)<<2)|((pGrapData&0x18)<<5);
    pGrapData |= GrapData;
    SetA8RegW(0x6b28, pGrapData); 	

    return A8_NO_ERROR;
}


void A8L_Get16WDataFromFIFO(u_short FIFO_Reg, u_short* DestAddr, u_int Length)
{

	u_char* DestAddr8b = (u_char*)DestAddr;
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPB(0) = (u_char)(FIFO_Reg & 0xFF);
	A8IndDataPB(0) = (u_char)(FIFO_Reg >> 8);
	
	A8IndCmdP = HII_RW_REG_B_INC;


	while(Length--)
	{
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0);
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0);
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0);
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0);
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
		*(DestAddr8b++) = A8IndDataPB( 0 );
	}
}

#else /* EBI_BUS_8BIT_MODE */
void	SetA8RegW(u_short addr, u_short data)
{

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = addr;
	A8IndCmdP = HII_RW_REG_W;
	A8IndDataPW(0) = data;
}
//=====================================================================//
void	SetA8RegB(u_short addr, u_char data)
{
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = addr;
	A8IndCmdP = HII_RW_REG_B;
	A8IndDataPB(addr) = data;
}
//=====================================================================//
u_short	GetA8RegW(u_short addr)
{
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = addr;
	A8IndCmdP = HII_RW_REG_W;
	return A8IndDataPW(0);
}
//=====================================================================//
u_char	GetA8RegB(u_short addr)
{
	volatile u_short temp;
	
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = addr;
	A8IndCmdP = HII_RW_REG_W;
	temp = A8IndDataPW(0);
	if ( addr & 0x01 )
		temp = temp >> 8;
	else
		temp = temp & 0xFF;
	return (u_char) temp;
}
#if defined(MTK_PLATFORM)
#pragma arm section code
#endif

//=====================================================================//
void	SetA8MemW(u_int addr, u_short data)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(addr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(addr & 0xFFFF);

	A8IndCmdP = HII_RW_MEM_W;
	A8IndDataPW(0) = data;
}
//=====================================================================//
void	SetA8MemB(u_int addr, u_char data)
{
	//volatile u_short temp;

	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(addr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(addr & 0xFFFF);

	A8IndCmdP = HII_RW_MEM_B;
	A8IndDataPB(addr) = data;
}
//=====================================================================//
u_short	GetA8MemW(u_int addr)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(addr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(addr & 0xFFFF);

	A8IndCmdP = HII_RW_MEM_W;
	return A8IndDataPW(0);
}
//=====================================================================//
u_char	GetA8MemB(u_int addr)
{
	volatile u_short temp;
	
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(addr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(addr & 0xFFFF);

	A8IndCmdP = HII_RW_MEM_W;
	temp = A8IndDataPW(0);
	if ( addr & 0x01 )
		temp = temp >> 8;
	else
		temp = temp & 0xFF;
	return (u_char) temp;
}
//=====================================================================//
void	SetA8MemAddr(u_int addr)
{
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(addr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)addr;
}
//=====================================================================//
void	SetA8MemNextW(u_short data)
{
	A8IndCmdP = HII_RW_MEM_W_INC;
	A8IndDataPW(0) = data; 
}
//=====================================================================//
//void	SetA8MemNextB(u_char data)
//{
//	A8IndCmdP = HII_RW_MEM_B_INC;
//	A8IndDataPB(0) = data; 
//}
//=====================================================================//
u_short	GetA8MemNextW()
{
	A8IndCmdP = HII_RW_MEM_W_INC;
	return A8IndDataPW(0);
}
//=====================================================================//
//u_char	GetA8MemNextB()
//{
//	A8IndCmdP = HII_RW_MEM_B_INC;
//	return A8IndDataPB(0);
//}
//=====================================================================//
#if defined(MTK_PLATFORM)
#pragma arm section code = "INTERNCODE"
#endif
#if 1
void	CopyMemByteHostToA8(u_int destAddr, u_char *srcAddr, u_int length)
{
    volatile u_int tlen;
    u_short *fsrcAddr;
	u_short	tmp;
	if(destAddr & 1){
		SetA8MemB(destAddr++,*srcAddr++);
		length--;
	}
	tlen = length >> 1;
	A8IndCmdP = HII_SET_ADDR_H;
    	A8IndDataPW(0) = (u_short)(destAddr>>16);
    	A8IndCmdP = HII_SET_ADDR_L;
    	A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);
    	A8IndCmdP = HII_RW_MEM_W_INC;
	while(tlen--){
		tmp = *srcAddr++;
		tmp += (*srcAddr++) << 8;
		A8IndDataPW(0) = tmp;
	}
	if(length & 1)
        	SetA8MemB( destAddr+length-1, *srcAddr );
#if 0	
    volatile u_int tlen;
    u_short *fsrcAddr;

    fsrcAddr = (u_short *) srcAddr;
    tlen = length >> 1;
    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (u_short)(destAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);

    A8IndCmdP = HII_RW_MEM_W_INC;
#if 1
	while(tlen--){
//    		data = *(fsrcAddr+i);
    		A8IndDataPW(0) = *(fsrcAddr++);
	}
#else
    for (i = 0; i < tlen; i++) {
    data = *(fsrcAddr+i);
    A8IndDataPW(0) = data;
    }
#endif	
    if ( length & 0x01 )
    {
        SetA8MemB( destAddr+length-1, *(srcAddr+length-1) );
    }
#endif	
}
#endif
//=====================================================================//
void	CopyMemByteA8ToHost(u_char *destAddr, u_int srcAddr, u_int length)
{
    volatile u_int	i, tlen;
    volatile u_short data;
    //u_short *fdestAddr;
#if 0	
	fdestAddr = (u_short *)destAddr;
	tlen = length >> 1;
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (srcAddr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = srcAddr;
	A8IndCmdP = HII_RW_MEM_W_INC;
	
	for (i = 0; i < tlen; i++) {
		data = A8IndDataPW(0);
		*(fdestAddr+i) = data;
		
	if (length & 0x01)
		{
		*(destAddr+length-1) = GetA8MemB(srcAddr+length-1);
		}
	}
#else
    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (srcAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = srcAddr;
    A8IndCmdP = HII_RW_MEM_W_INC;

    if(srcAddr & 0x1)
    {
    	data = A8IndDataPW(0);
    	*(destAddr++) = data >> 8;
    	length -= 1;
    }
    tlen = length >> 1;	
#if 1
	i = tlen;
	while(i--){
    		data = A8IndDataPW(0);
	    	*(destAddr++) = data;
    		*(destAddr++) = data >> 8;		
	}
#else
    for(i = 0; i < tlen; i++)
    {
    	data = A8IndDataPW(0);
    	*(destAddr++) = data;
    	*(destAddr++) = data >> 8;
    }
#endif	
    if(length > (tlen<<1))
    {
    	data = A8IndDataPW(0);
    	*(destAddr++) = data;
    }
#endif	
}
#if defined(MTK_PLATFORM)
#pragma arm section code
#endif
//=====================================================================//
void	CopyMemWordHostToA8(u_int destAddr, u_short *srcAddr, u_int length)
{

    u_short*		pdata;
    u_int	RealLen;

    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (u_short)(destAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);

    RealLen = (length+1) >> 1;
    pdata = srcAddr;
    A8IndCmdP = HII_RW_MEM_W_INC;
#if	1
	while(RealLen--){
	    	A8IndDataPW(0) = *pdata++;
	}
#else
    for (i = 0; i < RealLen; i++) {
    	//data = *(srcAddr+i);
    	A8IndDataPW(0) = *pdata;
    	pdata ++;
    }
#endif	
}
//=====================================================================//
void	CopyMemWordA8ToHost(u_short *destAddr, u_int srcAddr, u_int length)
{

    u_short* pdata;
    u_int	RealLen;

    A8IndCmdP = HII_SET_ADDR_H; 
    A8IndDataPW(0) = (u_short)(srcAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(srcAddr & 0xFFFF);

    RealLen = (length+1) >> 1;
    pdata = destAddr;
    A8IndCmdP = HII_RW_MEM_W_INC;
#if 1
	while(RealLen--){
		*pdata++ = A8IndDataPW(0);
		}
#else	
	for (i = 0; i < RealLen; i++) {
		*pdata = A8IndDataPW(0);
		//*(destAddr+i) = data;
		pdata ++ ;
	}
#endif	
}
//=====================================================================//
void	ClearA8Mem(u_int destAddr, u_short data, u_int length )
{
    s_int i;
    u_int	RealLen;

    A8IndCmdP = HII_SET_ADDR_H; 
    A8IndDataPW(0) = (u_short)(destAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);

    RealLen = (length+1) >> 1;
    A8IndCmdP = HII_RW_MEM_W_INC;
    for (i = 0; i < RealLen; i++) {
    	A8IndDataPW(0) = data;
    }
}
//=====================================================================//
void OpenFIFO(u_short fifoAddr)
{
    gA8FIFOAddr = fifoAddr;
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = fifoAddr;
}
//=====================================================================//
void PutWordFIFO(u_short data)
{
    A8IndCmdP = HII_RW_REG_W;
    A8IndDataPW(gA8FIFOAddr & 0x00F0) = data;
}
//=====================================================================//
u_short GetWordFIFO()
{
    A8IndCmdP = HII_RW_REG_W;
    return A8IndDataPW(gA8FIFOAddr & 0x00F0); 
}
//=====================================================================//
u_short SetA8GrpFIFODataW(u_short **pshorttmp)
{
    u_short GrapData,pGrapData;

    GrapData = *((*pshorttmp)++);
    GrapData = ((GrapData&0xe000)>>8)|((GrapData&0x700)>>6)|((GrapData&0x18)>>3);
    pGrapData = *((*pshorttmp)++);
    pGrapData = ((pGrapData&0xe000))|((pGrapData&0x700)<<2)|((pGrapData&0x18)<<5);
    pGrapData |= GrapData;
    SetA8RegW(0x6b28, pGrapData); 	

    return A8_NO_ERROR;
}

//=====================================================================//
s_short	SendA8Cmd(u_short cmd)
{
	u_int   timeout = 0;

	SetA8RegW(A8_HOST_CMD_RG, cmd);
	sys_IF_ait_delay1us(100);
	GetA8RegW(A8_HOST_CMD_RG);       // Add dummy read for ESD

	while((GetA8RegW(A8_HOST_CMD_RG)) && (timeout < A8_HOST_CMD_RG_TIME_OUT)) 
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}

	if (timeout >= A8_HOST_CMD_RG_TIME_OUT)
	{
		AIT_Message_P0("SendA8Cmd timeput!");
		return  1;
	}

	return A8_NO_ERROR;
}

//=====================================================================//
s_short	SendA8LongCmd(u_short cmd, u_short arg)
{
    u_short i = 0;
    u_int   timeout = 0;

    SetA8RegW(A8_HOST_ARG_RG, arg);
    SetA8RegW(A8_HOST_CMD_RG, cmd);
    while(GetA8RegW((A8_HOST_CMD_RG)) && timeout < A8_HOST_CMD_RG_TIME_OUT) 
    {
        for (i = 0; i < 100; i++);
            timeout++;
    }

    if (timeout >= A8_HOST_CMD_RG_TIME_OUT) 
    {
        return  A8_TIMEOUT_ERROR;
    }
    
    return A8_NO_ERROR;
}	


void A8L_Get16WDataFromFIFO(u_short FIFO_Reg, u_short* DestAddr, u_int Length)//0x6220,JpegBufferPtr,CurrentJpegPageSize-CurrentJpegReadCount);
{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = FIFO_Reg;
				A8IndCmdP = HII_RW_REG_W;

				while(Length--)
				{
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0);
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0);
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
					*(DestAddr++) = A8IndDataPW( 0 );
				}
}

//=====================================================================//
#endif /* EBI_BUS_8BIT_MODE */
u_short	GetA8DSCStatus(void)
{
	return GetA8RegW(A8_DSC_STATUS_RG);
}
#if 1
#if 0
u_char	RG[0x100]=
{
	0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,
	0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,
	0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,	
	0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,
	0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,
	0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,
	0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,			
	0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,
	0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,
	0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,
	0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,				
	0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,
	0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,
	0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,
	0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,
	0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,					
};
u_char	B[0x100]=
{
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,												
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
	0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,		
};
void Trans565to332ToA8(u_int destAddr, u_short *srcAddr, u_int length)
{
	u_short RGB565,RGB332,RGB332tmp;
    u_int	i;
    u_char*	pdata;
    u_int	RealLen;
    u_short	usRGB332;
    u_char* usRGB332tmp;

	usRGB332tmp = &(u_char)usRGB332;
    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (u_short)(destAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);

    RealLen = (length+1) >> 1;
    pdata = (u_char*)srcAddr;
    A8IndCmdP = HII_RW_MEM_W_INC;
    while(RealLen--){
		*usRGB332tmp = B[*pdata++]+RG[*pdata++];
		*(usRGB332tmp+1) = B[*pdata++]+RG[*pdata++];
    	A8IndDataPW(0) = *(u_short*)usRGB332tmp;		
    }
	
}
#else
#if defined(MTK_PLATFORM)
#pragma arm section code = "INTERNCODE"
#endif
void Trans565to332ToA8(u_int destAddr, u_short *srcAddr, u_int length)
{
	u_short RGB565,RGB332,RGB332tmp;
//    u_int	i;
    u_short*		pdata;
    u_int	RealLen;

    A8IndCmdP = HII_SET_ADDR_H;
    A8IndDataPW(0) = (u_short)(destAddr>>16);
    A8IndCmdP = HII_SET_ADDR_L;
    A8IndDataPW(0) = (u_short)(destAddr & 0xFFFF);

    RealLen = (length+1) >> 1;
    pdata = srcAddr;
    A8IndCmdP = HII_RW_MEM_W_INC;
//    for (i = 0; i < RealLen; i++) {
	while(RealLen--){	
    	//data = *(srcAddr+i);
	    RGB565 = *pdata++;
    	RGB332 = ((RGB565&0xe000)>>8)+((RGB565&0x700)>>6)+((RGB565&0x18)>>3);
	    RGB565 = *pdata++;
    	RGB332tmp = (RGB565&0xe000)+((RGB565&0x700)<<2)+((RGB565&0x18)<<5);
	    RGB332 += RGB332tmp;    	
    	A8IndDataPW(0) = RGB332;
    }
}

#if defined(MTK_PLATFORM)
#pragma arm section code
#endif
#endif
#endif
void A800_YUV422toYUV420(u_short width, u_short height, u_char *yuv422Buf, u_char* yuv420Buf)
{
	u_int	i, j;
	u_char	*YUV420_Y,*YUV420_U,*YUV420_V;
	
	YUV420_Y = (u_char*)yuv420Buf;
	YUV420_U = (u_char*)(YUV420_Y + (width * height));
	YUV420_V = (u_char*)(YUV420_U + (width * height / 4));
	
	for(j=0;j<height;j+=2){
		for(i=0;i<width;i+=2){
			*(u_char*)YUV420_U++ = *(u_char*)yuv422Buf++;
			*(u_char*)YUV420_Y++ = *(u_char*)yuv422Buf++;
			*(u_char*)YUV420_V++ = *(u_char*)yuv422Buf++;
			*(u_char*)YUV420_Y++ = *(u_char*)yuv422Buf++;			
		}
		for(i=0;i<width;i+=2){
			(u_char*)yuv422Buf++;
			*(u_char*)YUV420_Y++ = *(u_char*)yuv422Buf++;
			(u_char*)yuv422Buf++;
			*(u_char*)YUV420_Y++ = *(u_char*)yuv422Buf++;			
		}
	}	

}
u_int 	m_TimeCounter = 0;
u_int 	aitcTimeOutMax = 0xA0000;
u_short	TimeOutWhile(u_short status)
{
	if(m_TimeCounter >= aitcTimeOutMax){
		return	0;
	}else{
		m_TimeCounter++;
		return	status;
	}	
}
u_short CheckTimeOut(void)
{
	u_short fRet;
	if(m_TimeCounter >= aitcTimeOutMax){
		fRet = 0xffff;
	}else{
		fRet = 0;
	}
	m_TimeCounter = 0;
	return fRet;
}


