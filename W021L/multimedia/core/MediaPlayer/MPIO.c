/*=============================================================================
  FILE: MPIO.cpp

  SERVICES: IO object Handlers
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPIO.c#8 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPIO.h"
#include "MediaPlayer.h"
#include "MPSettings.h"

// this function creates IO object
stMPIO* MPMakeIO(IShell* m_pIShell)
{
  stMPIO* pIO = (stMPIO*)MALLOC(sizeof(stMPIO));
  if(pIO)
  {
    pIO->m_pIShell = m_pIShell;
    pIO->m_pIFileMgr = NULL;
    pIO->m_pFileName = NULL;
    pIO->m_EnumeType = IO_ENUME_DIR;

  }
  else
    DBGPRINTF_ERROR("MP: failed to create stMPIO.");

  return pIO;
}
/*
 * test for the existence of a directory
 * create one if it doesn’t exist
 * return TRUE when success. otherwise, return FALSE
 */
boolean MP_MakeDir(IFileMgr *pFileMgr, const char *pszDir)
{
  char      strDir[MP_MAX_STRLEN] = { 0 };
  if (pFileMgr)
  {
    if (pszDir == NULL)
    {
      DBGPRINTF_HIGH("MP: Trying to create NULL directory!");
      return FALSE;
    }
    else {
      DBGPRINTF_MED("MP: Trying to mkdir %s", pszDir);
    }
    (void)STRLCPY(strDir, pszDir, sizeof(strDir));
   /*
     * test directory. Create one if it doesn't exist
     */
    if (IFILEMGR_Test(pFileMgr, strDir) != SUCCESS)
    {
      DBGPRINTF_MED("MP: Creating directory %s", strDir);
      if (IFILEMGR_MkDir(pFileMgr, strDir) != SUCCESS)
      {
        DBGPRINTF_HIGH("MP: Unable to create directory %s", strDir);
        return FALSE;
      }
    }
    else {
      DBGPRINTF_MED("MP: Directory already exists.");
    }
    return TRUE;
  }
  else {
    DBGPRINTF_HIGH("MP: IFileMgr pointer is NULL!");
  }
  return FALSE;
}

/*
 * return true if file/dir exist. pFileMgr can be NULL.
 */
boolean MP_IsExist(CMediaPlayer *pme, IFileMgr *pFileMgr, const char *p)
{
  int       ret = EFAILED;

  if (pFileMgr)
  {
    ret = IFILEMGR_Test(pFileMgr, p);
  }
  else
  {
    (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
    if (pFileMgr)
    {
      ret = IFILEMGR_Test(pFileMgr, p);
      (void)IFILEMGR_Release(pFileMgr);
    }
    else
      DBGPRINTF_ERROR("MP: Failed to create file mgr.");
  }
  return (ret == SUCCESS);
}

//init IO object, this function must be called before enumeration
boolean MPIOInit(stMPIO* pIO, const char *pszDirName)
{
  int nRet;

  if(pIO == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL pIO ptr.");
    return FALSE;
  }

  if(pIO->m_pIFileMgr == NULL)
  {
    nRet = ISHELL_CreateInstance(pIO->m_pIShell, AEECLSID_FILEMGR, (void **)&pIO->m_pIFileMgr);
    DBGPRINTF_MED("MP: ISHELL_CreateInstance returns %d = %s", nRet, MP_ErrorStateToString(nRet));
    if(SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: failed to create file manager.");
      return FALSE;
    }
  }

  nRet = IFILEMGR_EnumInit(pIO->m_pIFileMgr, pszDirName, pIO->m_EnumeType);
  DBGPRINTF_MED("MP: IFILEMGR_EnumInit returns %d = %s", nRet, MP_ErrorStateToString(nRet));
  if(SUCCESS != nRet)
  {
    MP_RELEASEIF(pIO->m_pIFileMgr);
    pIO->m_pIFileMgr = NULL;
    DBGPRINTF_ERROR("MP: IFILEMGR_EnumInit failed.");
    return FALSE;
  }

  if(pIO->m_pFileName == NULL)
  {
    pIO->m_pFileName = MALLOC(MP_MAX_FILE_NAME);
    if(!pIO->m_pFileName)
    {
      MP_RELEASEIF(pIO->m_pIFileMgr);
      pIO->m_pIFileMgr = NULL;
      DBGPRINTF_ERROR("MP: cannot allocate memory.");
      return FALSE;
    }
  }
  
  (void)MEMSET (pIO->m_pFileName, 0, MP_MAX_FILE_NAME);
  // initialize fi for enumeration
  pIO->m_FileInfo.nStructSize = sizeof(pIO->m_FileInfo);
  pIO->m_FileInfo.attrib = 0;
  pIO->m_FileInfo.dwCreationDate = 0;
  pIO->m_FileInfo.dwSize = 0;
  pIO->m_FileInfo.pszFile = pIO->m_pFileName;
  pIO->m_FileInfo.nMaxFile = MP_MAX_FILE_NAME;
  pIO->m_FileInfo.pszDescription = 0;
  pIO->m_FileInfo.nDescriptionSize = 0;
  pIO->m_FileInfo.pClasses = 0;
  pIO->m_FileInfo.nClassesSize = 0;

  return TRUE;
}

// enume next IO item 
boolean MPIOEnumNext(stMPIO* pIO)
{
  boolean bRet = FALSE;

  if(pIO && pIO->m_pIFileMgr)
    bRet = IFILEMGR_EnumNextEx(pIO->m_pIFileMgr, &pIO->m_FileInfo);
  else 
  {
    DBGPRINTF_ERROR("MP: NULL ptr pIO = 0x%X, m_pIFileMgr = 0x%X.", pIO, (pIO)?pIO->m_pIFileMgr : 0);
  }
  return bRet;
}
// Free allocated memories and object handler
// this function must be called in pair with MPIOInit
void MPIOFree(stMPIO** pIO)
{
  if(pIO && *pIO)
  {
    FREEIF((*pIO)->m_pFileName);
    MP_RELEASEIF((*pIO)->m_pIFileMgr);
    FREE(*pIO);
    *pIO = NULL;
  }
}

// return number of IO items
int MPIOGetNumItems(stMPIO* pIO, const char *pszDir)
{
  int            i;
  int            nCount = 0;

  if(pIO == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL pIO ptr.");
    return FALSE;
  }

  if (pszDir == NULL)
  {
    DBGPRINTF_HIGH("MP: pszDir is NULL!");
  }
  else
  {
    DBGPRINTF_LOW("MP: counting files in %s", pszDir);
  }

  //enume all directories and files
  for (i = IO_ENUME_FILE; i < IO_ENUME_MAX; i++)
  {
    pIO->m_EnumeType = (EnumeType)i;
    if (MPIOInit(pIO, pszDir))
    {
      while (MPIOEnumNext(pIO))
      {
        nCount++;
      }
    }
  }
  return (nCount);
}
