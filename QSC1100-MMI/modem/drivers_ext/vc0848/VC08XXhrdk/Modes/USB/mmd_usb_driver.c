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

/*
 * INCLUDE FILE                                              
 */
#include "mmp_rdk.h"

/*
 * INTERNAL MACRO DEFINITION
 */
 
//#define MMD_USB_DEBUG_INFO
#define MMD_USB_DEBUG_ERR
#define MMD_USB_DEBUG_NOTE
#define MMD_USB_DEBUG_WARNING

#ifdef MMD_USB_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_USB_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_USB_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_USB_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif



extern volatile MMD_U32 g_Ack_MinCmd;
extern volatile MMD_U32 g_Ack_OP1;
extern volatile MMD_U32 g_Ack_OP2;

extern unsigned int volatile g_usb_detect;

static MMD_U32 USB_Send_Cmd(MMD_U32 cmd, MMD_U32 op1, MMD_U32 op2)
{
	MMD_MSG Msg;
	MMD_U32 timeout=10000;
	MMD_U08 min;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=cmd;
	Msg.OP1=op1;
	Msg.OP2=op2;
	MMD_BIU_Send_H2D_Cmd(&Msg);

	min = cmd & CMD_MASK_MIN;

	//for some reason the PLUGIN PLUGOUT command don't need ack
	if((min!=MIN_USB_PLUGIN) &&(min!=MIN_USB_PLUGOUT)){
		while(timeout){
			if(g_Ack_MinCmd == MIN_RESERVED)
				MMD_Check_Irq_Staus_And_Wait(1);
			else
				break;
			timeout--;
		}
	}

	if(timeout)
		return MMD_RET_NOERR;
	else
		return MMD_RET_TIMEOUT;
}



/*************************************************
  Function:       
  	Mode_Usb_Lisr
  Description:    
 	sys mode command process call back function, called in LISR
  Input:
  	pMsg:	Host to device cmd msg
  Output:        
	NULL 	
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		06/12/31     	create this function  
*************************************************/
MMD_U32 MMD_Mode_Usb_Lisr(MMD_U32 cmd)
{
	MMD_MSG Msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&Msg);
	MMD_ASSERT(Msg.CMD==cmd);
	
	min = Msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd=min;
	g_Ack_OP1=Msg.OP1;
	g_Ack_OP2=Msg.OP2;

	switch(min)
	{
	default:
		break;
	}

	return MMD_RET_NOERR;
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USB_Init
[DESCRIPTION]
  . 
[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_USB_Init(void)
{
	PRINTF(("[USB] MMD_USB_Init\r\n"));
	return MMD_RET_NOERR;
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USB_Quit
[DESCRIPTION]
  .
[ARGUMENT]
  None
[RETURN VALUE]
  MMD_RET_NOERR: success
  MMD_RET_ERR:    fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_USB_Quit(void)
{

	PRINTF(("[USB] MMD_USB_Quit\r\n"));

	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USB_Enable
[DESCRIPTION]
  Enable USB function, include USB MassStorage, USB bridge...
[ARGUMENT]
	type: 
		typedef enum
		{
			    USB_MASS_STORAGE=0,
		}USB_CLASS;

[RETURN VALUE]
  MMD_RET_NOERR        : Success
  MMD_RET_ERR          : Fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_USB_Enable(USB_CLASS type, USBCallback Callback)
{
	MMD_U32 ret = MMD_RET_NOERR;

	if(MMD_SYS_Mode_Get()!=SCENE_USB)
	{
		PRINTFE(("USB mode error \r\n"));
		return MMD_RET_MODEERR;
	}

	ret = USB_Send_Cmd(MAJ_USB|MIN_USB_ENABLE, 0, 0);

	if (ret != MMD_RET_NOERR)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_USB);
		PRINTFE(("Send commond error\r\n"));
		return ret;
	}
	return MMD_RET_NOERR;

}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USB_Enable
[DESCRIPTION]
  
[ARGUMENT]
	type: 
		typedef enum
		{
			    USB_MASS_STORAGE=0,
		}USB_CLASS;
		
[RETURN VALUE]
  MMD_RET_NOERR        : Success
  MMD_RET_ERR          : Fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_USB_SetProperty(USB_CLASS type,void *usb_property)
{
	MMD_U32 ret = MMD_RET_NOERR;
	MMD_U32 cmd;
	MMD_U32 op2;

	if(MMD_SYS_Mode_Get()!=SCENE_USB)
	{
		PRINTFE(("USB mode error\r\n"));
		return MMD_RET_MODEERR;
	}

	switch(type)
	{
	case USB_MASS_STORAGE:
		PRINTF(("device=[%d] speed=[%d]\r\n", ((USB_MASS_INFO*)usb_property)->device, ((USB_MASS_INFO*)usb_property)->speed));
		PRINTF(("usb_property=[0x%x] \r\n",(MMD_U32)usb_property));
		op2= ((USB_MASS_INFO*)usb_property)->device|(((USB_MASS_INFO*)usb_property)->speed)<<8;
		break;
	default:
		PRINTFE(("Unknown USB class.\r\n"));
		return MMD_RET_ERR;
		break;
	}

	cmd=MAJ_USB|MIN_USB_PROPERTY;

	ret = USB_Send_Cmd(cmd, type, op2);

	if (ret != MMD_RET_NOERR)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_USB);
		PRINTFE(("send command error\r\n"));
		return ret;
	}

#ifndef SUPPORT_USB_DETECT
	MMD_USB_Plugin();
#else
	if(g_usb_detect == 1)
		MMD_USB_Plugin();
#endif
	return MMD_RET_NOERR;

}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  MMD_USB_Disable
[DESCRIPTION]
  Disable USB function, include USB MassStorage, USB bridge...
[ARGUMENT]
	type: 
		typedef enum
		{
			    USB_MASS_STORAGE=0,
		}USB_CLASS;
[RETURN VALUE]
  MMD_RET_NOERR        : Success
  MMD_RET_ERR          : Fail
-----------------------------------------------------------------------------*/
MMD_U32 MMD_USB_Disable(USB_CLASS type)
{
	MMD_U32 ret = MMD_RET_NOERR;
	MMD_U32 cmd;

	if(MMD_SYS_Mode_Get()!=SCENE_USB)
	{
		PRINTFE(("USB mode error\r\n"));
		return MMD_RET_MODEERR;
	}

	cmd=MAJ_USB|MIN_USB_DISABLE;

	ret = USB_Send_Cmd(cmd, type, 0);

	if (ret != MMD_RET_NOERR)
	{
		MMD_ERROR_Deal(CMD_FAILED,MAJ_USB);
		PRINTFE(("send command error\r\n"));
		return ret;
	}
	return ret;	
}


/*
*MMD_USB_Plugin - usb plugin action
*
*@void
*
*Note: this function is called while usb plugin detection isr is triggered
*/
MMD_U32 MMD_USB_Plugin(void)
{
	MMD_U32 ret = MMD_RET_NOERR;
	MMD_U32 cmd;

	if(MMD_SYS_Mode_Get()!=SCENE_USB){
		PRINTFE(("[USB]: USB mode error\r\n"));
		return MMD_RET_MODEERR;
	}

	cmd=MAJ_USB|MIN_USB_PLUGIN;

	ret = USB_Send_Cmd(cmd, 0, 0);

	if (ret != MMD_RET_NOERR){
		MMD_ERROR_Deal(CMD_FAILED,MAJ_USB);
		PRINTFE(("[USB]: Send command PLUGIN error\r\n"));
		return ret;
	}
	return ret;	
}

/*
*MMD_USB_Plugout - usb plugout action
*
*@void
*
*Note: this function is called while usb plugout detection isr is triggered
*/
MMD_U32 MMD_USB_Plugout(void)
{
	MMD_U32 ret = MMD_RET_NOERR;
	MMD_U32 cmd;

	if(MMD_SYS_Mode_Get()!=SCENE_USB){
		PRINTFE(("[USB]: USB mode error\r\n"));
		return MMD_RET_MODEERR;
	}

	cmd=MAJ_USB|MIN_USB_PLUGOUT;

	ret = USB_Send_Cmd(cmd, 0, 0);

	if (ret != MMD_RET_NOERR){
		MMD_ERROR_Deal(CMD_FAILED,MAJ_USB);
		PRINTFE(("[USB]: Send command PLUGOUT error\r\n"));
		return ret;
	}
	return ret;	
}

