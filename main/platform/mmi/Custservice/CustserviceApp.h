#ifndef _MORSE_APP_H_
#define _MORSE_APP_H_
/*******************************************************************
*	Copyright(c)2008-2009 TCL CDMA SW developement Team Reserved
*
********************************************************************/
/********************************************************************

	Team		: 	CDMA JRD SW
	Author		: 	Qiong.chen Create 
	Version 	:	1.0.0
	Function Name 	:
	Change Time 	:
		Qiong.chen Create 2009/02/04
		
*********************************************************************/

/*==============================================================================
                                 ¨¤¨¤?¨®?¨²?¡§¨°?
==============================================================================*/
typedef struct _ICustServiceMenu  ICustServiceMenu;
 QINTERFACE(ICustServiceMenu)
{
	INHERIT_IApplet(ICustServiceMenu);
};
 
/*==============================================================================
                                 ¨¤¨¤?¨®?¨²o¡¥¨ºy?¡§¨°?
==============================================================================*/
#define ICustServiceMenu_AddRef(p)                GET_PVTBL(p,ICustServiceMenu)->AddRef(p)

#define ICustServiceMenu_Release(p)                GET_PVTBL(p,ICustServiceMenu)->Release(p)

#define ICustServiceMenu_HandleEvent(p,ec,wp,dw) \
GET_PVTBL(p,ICustServiceMenu)->HandleEvent(p, ec, wp, dw)

#endif
