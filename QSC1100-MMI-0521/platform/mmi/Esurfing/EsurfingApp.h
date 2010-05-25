#ifndef _ESURFING_APP_H_
#define _ESURFING_APP_H_
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
typedef struct _IEsurfingMenu  IEsurfingMenu;
 QINTERFACE(IEsurfingMenu)
{
	INHERIT_IApplet(IEsurfingMenu);
};
 
/*==============================================================================
                                 ¨¤¨¤?¨®?¨²o¡¥¨ºy?¡§¨°?
==============================================================================*/
#define IEsurfingMenu_AddRef(p)                GET_PVTBL(p,IEsurfingMenu)->AddRef(p)

#define IEsurfingMenu_Release(p)                GET_PVTBL(p,IEsurfingMenu)->Release(p)

#define IEsurfingMenu_HandleEvent(p,ec,wp,dw) \
GET_PVTBL(p,IEsurfingMenu)->HandleEvent(p, ec, wp, dw)

#endif
