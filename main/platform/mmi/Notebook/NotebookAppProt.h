#ifndef _NOTEBOOK_APP_PROT_H_
#define _NOTEBOOK_APP_PROT_H_
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
		Qiong.chen Create 2009/02/06
		
*********************************************************************/
#include "Notebook_priv.h"

extern void Notebook_LoadRecord(CNotebookMenu *pMe);
extern void Notebook_BuildRecListMenu(CNotebookMenu *pMe,IMenuCtl *pMenuCtl);
extern void Notebook_AddNewRec(CNotebookMenu *pMe);
extern int Notebook_DeleteRec(CNotebookMenu *pMe,IMenuCtl* pMenu);
extern int Notebook_DeleteAllRec(CNotebookMenu *pMe);
extern void Notebook_ViewRec(CNotebookMenu *pMe);
extern void Notebook_SaveEditRec(CNotebookMenu *pMe);
extern void Notebook_DisplayNoRecord(CNotebookMenu *pMe,IMenuCtl *pMenuCtl);
extern int Notebook_SaveData(CNotebookMenu *pMe,const char *allpath);
extern int Notebook_LoadData(CNotebookMenu *pMe,const char *allpath);
extern int Notebook_GetNewFileName(CNotebookMenu *pMe,char* fileNameBuffer, int fileNameBufferLenght);
extern int Notebook_CreateNewFile(CNotebookMenu *pMe);
extern char* Notebook_GetMenuItemName(CNotebookMenu *pMe,IMenuCtl* pMenu);
extern int Notebook_SetFullName(CNotebookMenu *pMe,char *filename,char *fullfilename);
extern int Notebook_MenuItemSort(CNotebookMenu *pMe, FileInfo* item1, FileInfo* item2);
extern void Notebook_DrawNorecord(CNotebookMenu *pMe, IMenuCtl *pMenuCtl);
#endif
