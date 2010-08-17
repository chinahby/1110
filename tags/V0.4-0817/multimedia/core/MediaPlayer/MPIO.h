/*=============================================================================
  FILE: MPIO.h

  SERVICES: this file defines mediaplayer IO objects
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPIO.h#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPIO_H
#define MPIO_H

#include "AEE.h"
#include "AEEFile.h"
#include "MediaPlayer.h"

typedef enum EnumeType
{
  IO_ENUME_FILE = 0,
  IO_ENUME_DIR,
  IO_ENUME_MAX
}EnumeType;

typedef struct {

  IShell*         m_pIShell;

  IFileMgr*       m_pIFileMgr;
  AEEFileInfoEx   m_FileInfo;
  char*           m_pFileName;
  EnumeType       m_EnumeType;

} stMPIO;

stMPIO* MPMakeIO(IShell* m_pIShell);
/*
 * return true if file/dir exist. pFileMgr can be NULL.
 */
boolean MP_IsExist(CMediaPlayer *pme, IFileMgr *pFileMgr, const char *p);
boolean MP_MakeDir(IFileMgr *pFileMgr, const char *pszDir);
boolean MPIOInit(stMPIO* pIO, const char *pszDirName);
boolean MPIOEnumNext(stMPIO* pIO);
int MPIOGetNumItems(stMPIO* pIO, const char *pszDir);

void MPIOFree(stMPIO** pIO);
#define MPIO_FREEIF(p) MPIOFree((stMPIO **)&(p))

#endif //MPIO_H
