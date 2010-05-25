
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_SDRAM_Info.h
 * [Description]        :   ”√”⁄578b.
 *				    LCD panel parameter setting structure.
 *				    				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-06-06 16:01
 * [Platform]               : not be limited by platform
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-06-06 		angela     		 Created
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/



#ifndef _VIM_5XRDK_SDRAMINFO_H_
#define _VIM_5XRDK_SDRAMINFO_H_

#ifdef __cplusplus
extern "C" {
#endif
/*! sdram page size */
#define	VM_SDRC_PAGESIZE_128			(0<<6)		/*!<  */
#define	VM_SDRC_PAGESIZE_256			(1<<6)		/*!<  */
#define	VM_SDRC_PAGESIZE_512			(2<<6)		/*!<  */
#define	VM_SDRC_PAGESIZE_1024			(3<<6)		/*!<  */

/*! sdram bank number */
#define	VM_SDRC_BANKNUMBER_2			(0<<5)		/*!< 2 banks */
#define	VM_SDRC_BANKNUMBER_4			(1<<5)		/*!< 4 banks */

/*! sdram data width */
#define	VM_SDRC_DATAWIDTH_16BIT			(0<<4)		/*!< *16bit */
#define	VM_SDRC_DATAWIDTH_32BIT			(1<<4)		/*!< *32bit */

/*! sdram density */
#define	VM_SDRC_SDRAMDENSITY_1M_16		0			/*!< 1M*16bit */
#define	VM_SDRC_SDRAMDENSITY_2M_16		1			/*!< 2M*16bit */
#define	VM_SDRC_SDRAMDENSITY_4M_16		2			/*!< 4M*16bit */
#define	VM_SDRC_SDRAMDENSITY_8M_16		3			/*!< 8M*16bit */
#define	VM_SDRC_SDRAMDENSITY_16M_16		4			/*!< 16M*16bit */
#define	VM_SDRC_SDRAMDENSITY_32M_16		5			/*!< 32M*16bit */
#define	VM_SDRC_SDRAMDENSITY_512K_32	6			/*!< 512K*32bit */
#define	VM_SDRC_SDRAMDENSITY_1M_32		7			/*!< 1M*32bit */
#define	VM_SDRC_SDRAMDENSITY_2M_32		8			/*!< 2M*32bit */
#define	VM_SDRC_SDRAMDENSITY_4M_32		9			/*!< 4M*32bit */
#define	VM_SDRC_SDRAMDENSITY_8M_32		10			/*!< 8M*32bit */
#define	VM_SDRC_SDRAMDENSITY_16M_32		11			/*!< 16M*32bit */

/*! sdram  size */
#define	VM_SDRC_SIZE_2M			(1|0X8)		/*!<  */
#define	VM_SDRC_SIZE_4M			(2|0X8)		/*!<  */
#define	VM_SDRC_SIZE_8M			(3|0X8)		/*!<  */
#define	VM_SDRC_SIZE_16M			(4|0X8)		/*!<  */
#define	VM_SDRC_SIZE_32M			(5|0x8)
#define	VM_SDRC_SIZE_64M			(6|0x8)
#define	VM_SDRC_SIZE_128M			(7|0x8)


/*! sram cas latency */
#define	VM_SDRC_CASLATENCY_RESERVED		(0<<4)		/*!<  */
#define	VM_SDRC_CASLATENCY_1			(1<<4)		/*!<  */
#define	VM_SDRC_CASLATENCY_2			(2<<4)		/*!<  */
#define	VM_SDRC_CASLATENCY_3			(3<<4)		/*!<  */
#define	VM_SDRC_CASLATENCY				VM_SDRC_CASLATENCY_2

/*! sdram select the bank address in the interface bus */
#define	VM_SDRC_BANKADDR_20_19			(0<<20)		/*!< bank_addr = addr_decode[20:19] */
#define	VM_SDRC_BANKADDR_21_20			(1<<20)		/*!< bank_addr = addr_decode[21:20] */
#define	VM_SDRC_BANKADDR_22_21			(2<<20)		/*!< bank_addr = addr_decode[22:21] */
#define	VM_SDRC_BANKADDR_23_22			(3<<20)		/*!< bank_addr = addr_decode[23:22] */
#define	VM_SDRC_BANKADDR_24_23			(4<<20)		/*!< bank_addr = addr_decode[24:23] */
#define	VM_SDRC_BANKADDR_25_24			(5<<20)		/*!< bank_addr = addr_decode[25:24] */
#define	VM_SDRC_BANKADDR_26_25			(6<<20)		/*!< bank_addr = addr_decode[26:25] */
#define	VM_SDRC_BANKADDR_RESERVED		(7<<20)		/*!< reserved */

/*! sdram select the row address in the interface bus */
#define	VM_SDRC_ROWADDR_20_08			(0<<16)		/*!< row_addr = addr_decode[20:08] */
#define	VM_SDRC_ROWADDR_21_09			(1<<16)		/*!< row_addr = addr_decode[21:09] */
#define	VM_SDRC_ROWADDR_22_10			(2<<16)		/*!< row_addr = addr_decode[22:10] */
#define	VM_SDRC_ROWADDR_23_11			(3<<16)		/*!< row_addr = addr_decode[23:11] */
#define	VM_SDRC_ROWADDR_24_12			(4<<16)		/*!< row_addr = addr_decode[24:12] */
#define	VM_SDRC_ROWADDR_25_13			(5<<16)		/*!< row_addr = addr_decode[25:13] */
#define	VM_SDRC_ROWADDR_RESERVED		(6<<16)		/*!< reserved */

typedef struct tag_TSdramTiming
{
	UINT32 Tase;// 1-16clock
	UINT32 Tmrd; // 1-4clock
	UINT32 Tdpl; // 1-16clock
	UINT32 Trfc; // 1-16clock
	UINT32 Trp;	// 1-4clock
	UINT32 Trcd;// 1-4clock
	UINT32 Txsr;// 1-16clock
	UINT32 Trc;	// 1-16clock
	UINT32 Tras;// 1-16clock
} TSdramTiming, *PTSdramTiming;


typedef struct tag_TSdramInfo
{
	UINT16 SdramSize;//mbyte
	UINT32 BankAddress;
	UINT32 RowAddress; 
	UINT32 PageSize; 
	UINT32 BankNum; 
	UINT32 DataWidth; 
	UINT32 Density; 
	UINT32 Cas;
	UINT32 RefreshInt;
	TSdramTiming Timing;
} TSdraminfo, *PTSdraminfo;

extern const TSdraminfo g_SdramInfo;
#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */
