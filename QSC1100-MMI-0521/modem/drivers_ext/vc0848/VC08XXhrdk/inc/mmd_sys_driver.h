/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_sys_driver.h
 * [Description]          : System mode header file
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-13
 * [Platform]             : Platform independent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-13  zhangxf    Created
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
#ifndef _MMD_SYS_H_
#define _MMD_SYS_H_

/*
 * INCLUDE FILE                                              
 */
#include "mmp_rdk.h"

// whole system work mode
extern MMD_U32 g_system_mode;
extern volatile MMD_U32 g_IPC_A2B0,g_IPC_A2B1;
typedef MMD_U32 (*DriverInitQuitFunc)(void);

#define MMD_CHIP_MODE_NORMAL 0 // normal mode. Set BYPASS PIN to low
#define MMD_CHIP_MODE_BYPASS 1 // bypass mode. Set BYPASS PIN to high

typedef void (*SYSCallback)(MMD_U32 status);//by dnn 2008/5/22

/*
 * PROTOTYPE DECLARATION                                     
 * 
 */

 MMD_U32 MMD_VC08XX_Init(void);
MMD_U32 MMD_SYS_GetVersion(MMD_U32* pHostVersion, MMD_U32* pCoreVersion);
MMD_U32 MMD_SYS_Mode_Switch(MMD_U32 mode);
MMD_U32 MMD_SYS_Codec_Switch(MMD_U32 codec);
//MMD_U32 GetCoreMode(void);
MMD_U32 MMD_SYS_Mode_Get(void);
MMD_U32 MMD_SYS_DownCore(MMD_U32 idx, MMD_U32 load_flag);
MMD_U32 MMD_SYS_TestRW( void );
void MMD_SYS_Reg_Set(MMD_U32 addr, MMD_U32 data);
MMD_U32 MMD_SYS_Reg_Get(MMD_U32 addr);
void MMD_SYS_Mem_Read(MMD_U32 addr, MMD_S08* pBuf, MMD_U32 size);
void MMD_SYS_Mem_Write(MMD_U32 addr, MMD_S08* pBuf, MMD_U32 size);
void MMD_SYS_Sdram_Init(void);
void MMD_SYS_Reset_8XX(void);
MMD_U32 MMD_Mode_Sys_Lisr(MMD_U32 cmd);
MMD_U32 MMD_SYS_Malloc(MMD_U32 size);
MMD_U32 MMD_SYS_Free(MMD_U32 addr);
MMD_U32 MMD_SYS_MallocInfo(void);
MMD_U32 MMD_SYS_MemPoolInfo(MMD_U32 poll,MMD_U32* pAddr, MMD_U32* pSize);
MMD_U32 MMD_SYS_GetTimer2Value(void);
MMD_U32 MMD_SYS_FillMem(MMD_U32 addr, MMD_U32 size);
MMD_U32 MMD_SYS_FillMemData(MMD_U32 addr, MMD_U32 size,MMD_U32 data);
MMD_U32 MMD_SYS_SetMem(MMD_U32 addr, MMD_U08 data);
MMD_U32 MMD_SYS_Scene_Info_Init(void);
MMD_U32 MMD_SYS_Bypass_Set(MMD_U32 flag);
MMD_U32 MMD_SYS_Codec_Info_Init(void);
MMD_U32 MMD_SYS_Mem_Set(MMD_U32 addr, MMD_U08 ch, MMD_U16 size);
MMD_U32 MMD_SYS_Mem_Pool_Create(MMD_U32 start_addr);
MMD_U32 MMD_SYS_Func_Register(MMD_U32 codec_type);

MMD_U32 MMD_SYS_SetClk(MMD_SYS_CLK sys_clk);
MMD_U32 MMD_SYS_SetModeClk(MMD_SYS_CLK sys_clk);
MMD_U32 MMD_SYS_GetClk(MMD_SYS_FREQ *pFreq);


void MMD_SYS_SetCKDCtrl(MMD_CKD ckd,MMD_U08 enable,MMD_U08 bypass);

void MMD_SYS_SetClkGate(MMD_CLKGATE clkgate, MMD_U32 gate_flag);
void MMD_SYS_SetModeClkGate(CLKGATE_MODE mode);

//void MMD_SYS_Mode_LDO_PowerCtrl(MMD_PMU_MODE pmu_mode);
//void MMD_SYS_Mode_REG_PowerCtrl(MMD_PMU_MODE pmu_mode);

void MMD_SYS_Reg_PowerCtrl(MMD_REG_POWER module_power,MMD_U08 status);
void MMD_SYS_LDO_PowerCtrl(MMD_REG_POWER module_power,MMD_U08 status);
MMD_U32 MMD_SYS_SetCodecOnly(void);
MMD_U32 MMD_SYS_SetPSMode(MMD_PS_MODE mode);
MMD_U32 MMD_SYS_Wakeup(MMD_PS_MODE mode);
void MMD_SYS_PMU_SWITCH(MMD_PMU_MODE pmu_mode);
void MMD_SYS_Set_TriggerMode( void );
void MMD_Set_SysCallback(SYSCallback Callback);
MMD_U32 MMD_ERROR_Deal(MMD_U32 errorNO,MMD_U32 sysNO);
MMD_U32 MMD_SYS_Icache_Clean(void);
void MMD_SYS_SnrPowerOff(void);
MMD_U32 MMD_SYS_SnrPowerOn(void);
void MMD_SYS_SetPlayVolume(MMD_U32 channel,MMD_U32 volume);
void MMD_SYS_GPIO_PowerCtrl(MMD_U32 GPIO );

#endif  /* _MMD_SYS_H_ */

