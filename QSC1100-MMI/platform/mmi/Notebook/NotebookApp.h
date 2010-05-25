#ifndef _NOTEBOOK_APP_H_
#define _NOTEBOOK_APP_H_
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
typedef struct _INotebookMenu  INotebookMenu;
 QINTERFACE(INotebookMenu)
{
	INHERIT_IApplet(INotebookMenu);
};
 
/*==============================================================================
                                 ¨¤¨¤?¨®?¨²o¡¥¨ºy?¡§¨°?
==============================================================================*/
#define INotebookMenu_AddRef(p)                GET_PVTBL(p,INotebookMenu)->AddRef(p)

#define INotebookMenu_Release(p)                GET_PVTBL(p,INotebookMenu)->Release(p)

#define INotebookMenu_HandleEvent(p,ec,wp,dw) \
GET_PVTBL(p,INotebookMenu)->HandleEvent(p, ec, wp, dw)

#endif
