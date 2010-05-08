
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_SDRAM_Driver.h
 * [Description]        : 适用于578b.和sdram 相关的头文件
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-05-20 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-06-6		angela      		 Created
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


#ifndef _VIM_5XRDK_SDRAM_H_
#define _VIM_5XRDK_SDRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! sdram safe row mode */
#define	VM_SDRC_SAFEROW_DISABLE			(0<<13)		/*!< disable safe row mode */
#define	VM_SDRC_SAFEROW_ENABLE			(1<<13)		/*!< enable safe row mode */
#define	VM_SDRC_SAFEROWMODE				VM_SDRC_SAFEROW_DISABLE

/*! sdram cache mode */
#define	VM_SDRC_CACHE_OFF				(0<<12)		/*!< sdram cache off */
#define	VM_SDRC_CACHE_ON				(1<<12)		/*!< sdram cache on */
#define	VM_SDRC_CACHEMODE				VM_SDRC_CACHE_OFF

/*! sdram auto power down entry */
#define	VM_SDRC_APDE_DISABLE			(0<<11)		/*!< disable auto enter power down mode entery */
#define	VM_SDRC_APDE_ENABLE				(1<<11)		/*!< enable auto enter power down mode entery */
#define	VM_SDRC_APDE					VM_SDRC_APDE_DISABLE

/*! sdram clock gate */
#define	VM_SDRC_SDRAMCLOCK_UNGATE		(0<<10)		/*!< ungate sdram clock */
#define	VM_SDRC_SDRAMCLOCK_GATE			(1<<10)		/*!< gate sdram clock */
#define	VM_SDRC_SDRAMCLOCK				VM_SDRC_SDRAMCLOCK_UNGATE

/*! sdram auto self refresh entry */
#define	VM_SDRC_ASRE_DISABLE			(0<<9)		/*!< disable auto enter selfrefresh mode entry */
#define	VM_SDRC_ASRE_ENABLE				(1<<9)		/*!< enable auto enter selfrefresh mode entry */
#define	VM_SDRC_ASRE					VM_SDRC_ASRE_DISABLE

/*! sdram extended mode register enable */
#define	VM_SDRC_EMR_NOTEXIT				(0<<8)		/*!< extended mode register not exit */
#define	VM_SDRC_EMR_EXIT				(1<<8)		/*!< extended mode register exit */
#define	VM_SDRC_EMR						VM_SDRC_EMR_NOTEXIT



/*! sdram driver strength */
#define	VM_SDRC_DRIVERSTRENGTH_100		(0<<21)		/*!<  */
#define	VM_SDRC_DRIVERSTRENGTH_75		(1<<21)		/*!<  */
#define	VM_SDRC_DRIVERSTRENGTH_50		(2<<21)		/*!<  */
#define	VM_SDRC_DRIVERSTRENGTH_25		(3<<21)		/*!<  */
#define	VM_SDRC_DRIVERSTRENGTH			VM_SDRC_DRIVERSTRENGTH_100

/*! sdram max case temperature */
#define	VM_SDRC_TCSR_85C				(0<<19)		/*!<  */
#define	VM_SDRC_TCSR_70C				(1<<19)		/*!<  */
#define	VM_SDRC_TCSR_45C				(2<<19)		/*!<  */
#define	VM_SDRC_TCSR_AUTO				(3<<19)		/*!<  */
#define	VM_SDRC_TCSR					VM_SDRC_TCSR_85C


/*! sdram partial array self refresh */
#define	VM_SDRC_PASR_ALLBANKS			(0<<16)		/*!<  */
#define	VM_SDRC_PASR_RESERVED			(1<<16)		/*!<  */
#define	VM_SDRC_PASR_ONEBANK			(2<<16)		/*!< bs=0 */
#define	VM_SDRC_PASR					VM_SDRC_PASR_ALLBANKS

/*! sdram write burst mode */
#define	VM_SDRC_WRTIE_BURSTLENGTH		(0<<9)		/*!<  */
#define	VM_SDRC_WRITE_SINGLE			(1<<9)		/*!<  */
#define	VM_SDRC_WRITEBURSTMODE			VM_SDRC_WRTIE_BURSTLENGTH

/*! sdram op mode */
#define	VM_SDRC_OPMODE_DEFINED			(0<<7)		/*!<  */
#define	VM_SDRC_OPMODE_RESERVED			(1<<7)		/*!<  */
#define	VM_SDRC_OPMODE					VM_SDRC_OPMODE_DEFINED


/*! sdram write burst mode */
#define	VM_SDRC_WRTIE_BURSTLENGTH		(0<<9)		/*!<  */
#define	VM_SDRC_WRITE_SINGLE			(1<<9)		/*!<  */
#define	VM_SDRC_WRITEBURSTMODE			VM_SDRC_WRTIE_BURSTLENGTH


/*! sdram burst type */
#define	VM_SDRC_BURST_SEQUENTIAL		(0<<3)		/*!< only sequential support  */
#define	VM_SDRC_BURST_INTERVAL			(1<<3)		/*!<  */
#define	VM_SDRC_BURSTTYPE				VM_SDRC_BURST_SEQUENTIAL

/*! sdram burst length */
#define	VM_SDRC_BURSTLENGTH_1			0			/*!<  */
#define	VM_SDRC_BURSTLENGTH_2			1			/*!<  */
#define	VM_SDRC_BURSTLENGTH_4			2			/*!<  */
#define	VM_SDRC_BURSTLENGTH_8			3			/*!<  */
#define	VM_SDRC_BURSTLENGTH_RESERVED	4			/*!<  */
#define	VM_SDRC_BURSTLENGTH_FULLPAGE	7			/*!< only full page read/write support in normal mode */
#define	VM_SDRC_BURSTLENGTH				VM_SDRC_BURSTLENGTH_FULLPAGE

/*! sdram refresh interval */
#define	VM_SDRC_REFRESHUNTERVAL			0xaf
/*****************************************************************************************************/


/*****************************************************************************************************/
/*! sdrc command */
typedef enum _ENUM_VIM_SDRC_COMMAND
{
	VIM_SDRC_COMMAND_NOP			= 0x00,		/*!<  */
	VIM_SDRC_COMMAND_SLFRSH		= 0x01,		/*!< self refresh entery */
	VIM_SDRC_COMMAND_DPPWDN		= 0x02,		/*!< deep power down mode entry */
	VIM_SDRC_COMMAND_SLFRSHX		= 0x03,		/*!< self refresh exit */
	VIM_SDRC_COMMAND_DPPWDNEX	= 0x04,		/*!< deep power down mode exit */
	VIM_SDRC_COMMAND_CBR			= 0x05,		/*!< auto refresh */
	VIM_SDRC_COMMAND_MRS			= 0x06,		/*!< load mode register */
	VIM_SDRC_COMMAND_EMRS		= 0x07,		/*!< load extended mode register */
	VIM_SDRC_COMMAND_INIT		= 0x08,		/*!< initial */
	VIM_SDRC_COMMAND_PWDNN		= 0x09,		/*!< power down mode entry */
	VIM_SDRC_COMMAND_PWDNX		= 0x0a,		/*!< power down mode exit */
	
	VIM_SDRC_COMMAND_RESERVED	= 0x0b		/*!< reserved */
}VIM_SDRC_COMMAND;



#define	VIM_SDRAM_STATUSDETECT_MAXTIME	0x100		/*!<  */




extern VIM_RESULT VIM_SDRAM_Initialize(PTSdraminfo Info);

UINT32 VIM_SDRAM_GetSize(void);


#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */

