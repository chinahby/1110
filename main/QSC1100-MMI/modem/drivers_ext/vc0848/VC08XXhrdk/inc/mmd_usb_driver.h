/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_usb_driver.h
 * [Description]          : USB mode header file
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-20
 * [Platform]             : Platform independent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-20  zhangxf    Created
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
 
#ifndef _MMD_USB_H_
#define _MMD_USB_H_

/*
 * INCLUDE FILE                                              
 */
#include "mmp_rdk.h"

/*
 * MACRO DEFINITION
 * in the order: constant macros first, then function macros.
 */
 

typedef void (*USBCallback)(MMD_U32 command,MMD_U32 OP1, MMD_U32 OP2);

typedef enum
{
    USB_MASS_STORAGE=0
}USB_CLASS;

typedef enum
{
    USB_SD=0,
    USB_NAND=1,
    USB_SD_PLUS_NAND=2
}USB_MASS_DEVICE;

typedef enum
{
    USB_HIGH_SPEED=0,
    USB_FULL_SPEED
}USB_SPEED_MODE;

typedef struct
{
    MMD_U08 device;
    MMD_U08 speed;
}USB_MASS_INFO;

/*
 * PROTOTYPE DECLARATION                                     
 * 
 */
MMD_U32 MMD_USB_Init(void);
MMD_U32 MMD_USB_Quit(void);
MMD_U32 MMD_USB_Enable(USB_CLASS type, USBCallback Callback);
MMD_U32 MMD_USB_SetProperty(USB_CLASS type,void *usb_property);
MMD_U32 MMD_USB_Disable(USB_CLASS type);

MMD_U32 MMD_Mode_Usb_Lisr(MMD_U32 cmd);

MMD_U32 MMD_USB_Plugin(void);
MMD_U32 MMD_USB_Plugout(void);

#endif  /* _MMD_USB_H_ */

