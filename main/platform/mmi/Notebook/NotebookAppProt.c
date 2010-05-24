#ifndef _NOTEBOOK_APP_PROT_C_
#define _NOTEBOOK_APP_PROT_C_
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
#include "NotebookAppProt.h"
void Notebook_LoadRecord(CNotebookMenu *pMe)
{
}

int Notebook_LoadData(CNotebookMenu *pMe,const char *allpath)
{
	IFile     *pFile;
	int32	   bytesRead;

	pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, allpath, _OFM_READ);//changed by Joe 090212

	if(pFile != NULL)
	{
		if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
		{
		    (void)IFILE_Release(pFile);
		    return EFAILED;
		}
		bytesRead = IFILE_Read(pFile,&(pMe->m_NoteData), 
                            sizeof(pMe->m_NoteData));

        if (bytesRead != (int32)sizeof(pMe->m_NoteData))
        {
            IFILE_Release(pFile);		
                return EFAILED;
        }	
            IFILE_Release(pFile);
                return SUCCESS;
	}
    return EFAILED;
}

int Notebook_SaveData(CNotebookMenu *pMe,const char *allpath)
{
	IFile  *pFile;
	uint32  bytesWritten;

	pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, 
	                     allpath, 
	                     _OFM_READWRITE);

	(void) IFILE_Truncate(pFile, 0);
	if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
	{
		(void)IFILE_Release(pFile);
		return EFAILED;
	}
	
	bytesWritten = IFILE_Write(pFile, &(pMe->m_NoteData),
						sizeof(pMe->m_NoteData));
	
	if (bytesWritten != sizeof(pMe->m_NoteData)) 
	{ 
		IFILE_Release(pFile);
		return EFAILED;
	}
	IFILE_Release(pFile);
	return SUCCESS;

}

int Notebook_GetNewFileName(CNotebookMenu *pMe,char* fileNameBuffer, int fileNameBufferLenght)
{

    int offset = STRLEN( pMe->m_pszSaveDir);

    if( fileNameBuffer && fileNameBufferLenght >= ( offset + 18))
    {
        JulianType julian = {0};

        GETJULIANDATE( GETTIMESECONDS(), &julian);

        MEMSET( fileNameBuffer, 0, fileNameBufferLenght);
        SPRINTF( fileNameBuffer,      "%s",    pMe->m_pszSaveDir);
        SPRINTF( fileNameBuffer      + offset, "%04d", julian.wYear);
        SPRINTF( fileNameBuffer + 4  + offset, "%02d", julian.wMonth);
        SPRINTF( fileNameBuffer + 6  + offset, "%02d", julian.wDay);
        SPRINTF( fileNameBuffer + 8  + offset, "%02d", julian.wHour);
        SPRINTF( fileNameBuffer + 10 + offset, "%02d", julian.wMinute);
        SPRINTF( fileNameBuffer + 12 + offset, "%02d", julian.wSecond);
        SPRINTF( fileNameBuffer + 14 + offset, "%s",   pMe->m_pszFileExtension);

        //debug( ";file name, %s", fileNameBuffer);
        return TRUE;
    }

    return FALSE;
}

int Notebook_CreateNewFile(CNotebookMenu *pMe)
{
    IFile  *pFile;
    int result;
    MEMSET(pMe->m_FileName,0,sizeof(pMe->m_FileName));
    Notebook_GetNewFileName(pMe,pMe->m_FileName,sizeof(pMe->m_FileName));


    pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr,
                                            pMe->m_FileName,
                                            _OFM_CREATE);
    if(pFile == NULL)
    {
        (void)IFILE_Release(pFile);
        return FALSE;
    }
    /*last Error result  and used*/
    result = IFILEMGR_GetLastError(pMe->m_pFileMgr);

    if(result == 0)
    {
        (void)IFILE_Release(pFile);
        return TRUE;
    }
    else
    {
        (void)IFILE_Release(pFile);
        return result;
    }
    

    //return  TRUE;
}

uint8 Notebook_ConvertIntToBcd(uint16 i)
{
    uint8 bcd;

    bcd = ((int)(i/10)) << 4;
    bcd |= i % 10;

    return bcd;
}

void Notebook_ConvertMStoMcTime(uint32 sec, JulianType *pmc_time)
{
    uint8      year = 0;
    JulianType julian;
    
    if (NULL == pmc_time)
    {
        return;
    }
    GETJULIANDATE(sec, &julian);
    
    if (julian.wYear <= 1999)
    {
        if (julian.wYear <= 1996)
        {
            year = 96;
        }
        else
        {
            year = julian.wYear - 1900;
        }
    }
    else
    {
        if (julian.wYear >= 2095)
        {
            year = 95;
        }
        else
        {
            year = julian.wYear - 2000;
        }
    }

    pmc_time->wYear     = ConvertIntToBcd(year);
    pmc_time->wMonth    = ConvertIntToBcd(julian.wMonth);
    pmc_time->wDay      = ConvertIntToBcd(julian.wDay);
    pmc_time->wHour     = ConvertIntToBcd(julian.wHour);
    pmc_time->wMinute   = ConvertIntToBcd(julian.wMinute);
    pmc_time->wSecond   = ConvertIntToBcd(julian.wSecond);
    pmc_time->wWeekDay = 0;
    
}

void Notebook_BuildRecListMenu(CNotebookMenu *pMe,IMenuCtl *pMenuCtl)
{
    IMENUCTL_DeleteAll(pMenuCtl);
    
    if(IFILEMGR_EnumInit( pMe->m_pFileMgr, pMe->m_pszSaveDir, 0) == SUCCESS)
    {
		FileInfo 	info 	 				= {0};
		int			i 		 				= 0;
		AECHAR		name[AEE_MAX_FILE_NAME] = {0}; 

        while( IFILEMGR_EnumNext( pMe->m_pFileMgr, &info))
        {
			if( info.dwSize == 0)
			{
				IFILEMGR_Remove( pMe->m_pFileMgr, info.szName);
				continue;
			}
            if( STRENDS( pMe->m_pszFileExtension, info.szName))
            {
                {
					char* fileName = info.szName + STRLEN( pMe->m_pszSaveDir);
					int	  len = STRLEN( fileName);
					FileInfo* 	data  = MALLOC( sizeof( FileInfo));
					if( data)
					{

						IFILEMGR_GetInfo( pMe->m_pFileMgr, info.szName, &info);

						MEMSET( name, 0, sizeof( name));
						MEMCPY( data, &info, sizeof( FileInfo));
                        
                        *(fileName + (len - 4)) = '\0';
                        STRTOWSTR(fileName,name,sizeof(name));
						//recorder_str_2_wstr( fileName, len - 4, name, sizeof( name));
						//debug( ";filename %s", fileName);
						//debug( ";date 0x%x, 0x%x", info.dwCreationDate, data->dwCreationDate);

#if defined( FEATURE_RECORDER_RECORD_LIST_WITH_ICON)
						{
							CtlAddItem ai = {0};

							ai.pszResImage	= AEE_RECORDER_IMAGES_RES_FILE;
							ai.wImage		= IDI_RECORDER_RECORDER_LIST_ICON;
							ai.pText		= name;
							ai.dwData		= (uint32)data;
							ai.wItemID		= i ++;

							IMENUCTL_AddItemEx( pMenuCtl, &ai);
						}
#else
						IMENUCTL_AddItem( pMenuCtl, 0, 0, i++, name, (uint32)data);
#endif
					}
					else
					{
						break;
					}
				}           
            }
        }
    }
    
    if( IMENUCTL_GetItemCount(pMenuCtl) > 0)
    {
        IMENUCTL_SortCustom(pMenuCtl, (PFNMENUSORTCOMPARE)Notebook_MenuItemSort, pMe);
    }
}

void Notebook_AddNewRec(CNotebookMenu *pMe)
{

}

int Notebook_DeleteRec(CNotebookMenu *pMe,IMenuCtl* pMenu)
{
    pMe->m_nLastOperationError = SUCCESS;
    
	if( IMENUCTL_GetItemCount( pMenu) == 0)
	{
		return 1;
	}
    else
    {
	int 	sel 	= IMENUCTL_GetSel( pMenu);
	char* 	data 	= 0;
	if( !( data = Notebook_GetMenuItemName( pMe, pMenu)))
	{
	//result macro define error 090216
		return 2;
	}
    
	if( IFILEMGR_Remove( pMe->m_pFileMgr, pMe->m_FileName) == SUCCESS)
	{
		//IMENUCTL_DeleteItem( pMenu, sel);
		FREEIF( data);
	}
	else
	{
		pMe->m_nLastOperationError = IFILEMGR_GetLastError( pMe->m_pFileMgr);
		return 3;
	}        
        

    }

}

char* Notebook_GetMenuItemName(CNotebookMenu *pMe,IMenuCtl* pMenu)
{
    CtlAddItem item = {0};

    if( IMENUCTL_GetItem( pMenu, IMENUCTL_GetSel( pMenu), &item) && item.dwData)
    {
            int offset = STRLEN( pMe->m_pszSaveDir);
            FileInfo* pFileInfo = (FileInfo*)item.dwData;
            char   file_temp[60] = {0};
            WSTRTOSTR(item.pText,file_temp,sizeof(file_temp));
            MEMSET( pMe->m_FileName, 0, sizeof( pMe->m_FileName));
            STRCPY( pMe->m_FileName, pMe->m_pszSaveDir);
            STRCPY( pMe->m_FileName + offset, file_temp);
            offset = STRLEN(pMe->m_FileName);
            STRCPY( pMe->m_FileName + offset,pMe->m_pszFileExtension);/*.dat*/
        
            DBGPRINTF("Notebook_GetMenuItemNamep,m_FileName = %s",pMe->m_FileName);
            DBGPRINTF("090330 szName = %s",pFileInfo->szName);
            DBGPRINTF("090330 file_temp =  %s",file_temp);
            MEMSET(pMe->m_NoteData.pNoteFileName,0,sizeof(pMe->m_NoteData.pNoteFileName));
            STRCPY(pMe->m_NoteData.pNoteFileName,file_temp);
            return pFileInfo->szName;
        
	}

	return NULL;

}

int Notebook_SetFullName(CNotebookMenu *pMe,char *filename,char *fullfilename)
{
    CtlAddItem item = {0};
    
    int offset = STRLEN( pMe->m_pszSaveDir);
    char  file_temp[60] = {0};
    MEMSET(fullfilename, 0, sizeof(fullfilename));
    STRCPY(fullfilename, pMe->m_pszSaveDir);
    STRCPY(fullfilename + offset, filename);
    offset = STRLEN(fullfilename);
    STRCPY(fullfilename + offset,pMe->m_pszFileExtension);/*.dat*/

    return SUCCESS;  
}


int Notebook_DeleteAllRec(CNotebookMenu *pMe)
{    
    if(IFILEMGR_EnumInit( pMe->m_pFileMgr, pMe->m_pszSaveDir, 0) == SUCCESS)
    {
        FileInfo    info                    = {0};
        int         i                       = 0;
        AECHAR      name[AEE_MAX_FILE_NAME] = {0}; 

        while( IFILEMGR_EnumNext( pMe->m_pFileMgr, &info))
        { 
            IFILEMGR_Remove(pMe->m_pFileMgr, info.szName);            
        }
        return TRUE;
    }
    return TRUE;
}

void Notebook_ViewRec(CNotebookMenu *pMe)
{
    

}

void Notebook_SaveEditRec(CNotebookMenu *pMe)
{

}

void Notebook_DisplayNoRecord(CNotebookMenu *pMe,IMenuCtl *pMenuCtl)
{
    AEERect rc;
    AECHAR wStrBuf[64] = {(AECHAR)'\0'};
    RGBVAL  oldColor; 
    
    if(pMenuCtl)
    {
        IMENUCTL_GetRect(pMenuCtl, &rc);
    }
    
    rc.y = GetTitleBarHeight(pMe->m_pDisplay);

    rc.dy -= (GetTitleBarHeight(pMe->m_pDisplay) + 2);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);    
    
    (void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_NOTEBOOK_RES_FILE,
                                IDS_EMPTY,
                                wStrBuf,sizeof(wStrBuf));
    
    oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                                AEE_FONT_BOLD,
                                wStrBuf,
                                -1,
                                rc.x,
                                rc.y,
                                &rc,
                                IDF_TEXT_TRANSPARENT);
    
    IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
}

int Notebook_BuildListMenu(CNotebookMenu *pMe, IMenuCtl *pMenuCtl)
{
    return SUCCESS;
}

int Notebook_MenuItemSort(CNotebookMenu *pMe, FileInfo* item1, FileInfo* item2)
{

    if( item1 && item2)
    {
        return ( item2->dwCreationDate - item1->dwCreationDate);
    }

    return 0;
}

void Notebook_DrawNorecord(CNotebookMenu *pMe, IMenuCtl *pMenuCtl)
{
    AEERect rc={0};
    AECHAR wStrBuf[64] = {(AECHAR)'\0'};
    RGBVAL  oldColor=0; 
    
    if(pMenuCtl)
    {
        IMENUCTL_GetRect(pMenuCtl, &rc);
    }
    
    rc.y = GetTitleBarHeight(pMe->m_pDisplay);
 
    rc.dy -= (GetTitleBarHeight(pMe->m_pDisplay) + 2);
    IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);    
    
    (void)ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_NOTEBOOK_RES_FILE,
                                                IDS_NOTEBOOK_NULL,
                                                wStrBuf,sizeof(wStrBuf));
    
    oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    
    (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                                                AEE_FONT_BOLD,
                                                wStrBuf,
                                                -1,
                                                rc.x,
                                                rc.y,
                                                &rc,
                                                IDF_TEXT_TRANSPARENT);
    
    IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);

}
#endif


