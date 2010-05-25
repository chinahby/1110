/*=============================================================================
  FILE: MPFileListWin.h

  SERVICES: this file defines file list win objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPFileListWin.h#3 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPFILELISTWIN_H
#define MPFILELISTWIN_H

#include "MediaPlayer.h"

/*
 * File list window: Lists files in media dir for selection.
 */
typedef struct _CFileListWin
{
  INHERIT_CWindow(IWindow);
  IMenuCtl *m_pFileMenu;
}CFileListWin;

/*
 * File List window
 */
IWindow *CFileListWin_New(CMediaPlayer *pOwner);
void CFileListWin_Delete(IWindow *po);
void CFileListWin_Enable(IWindow *po, boolean bEnable);
void CFileListWin_Redraw(IWindow *po);
boolean CFileListWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean CFileListWin_MakeList(CFileListWin *pme, CFileExplore *pFileExp);

void CMediaPlayer_RefreshExplorer(CMediaPlayer *pme, const char *pszDir);


int MP_GetNumFiles(CMediaPlayer *MP, const char *pszDir);
int MP_GetCurrentIndex(CMediaPlayer *MP, const char *pszDir, const char *pszFile);
int MP_GetCurrentDepth(const char *pszDir);
boolean MP_GetParentDir(CMediaPlayer *MP, const char *pszFilePath, char *pszDest, uint16 nSizeDest);

void CMediaPlayer_InitializeExplorer(CMediaPlayer *pme);
void CMediaPlayer_FreeExplorerData(CMediaPlayer *pme);
boolean CMediaPlayer_ResetExplorerData(CMediaPlayer *pme, int size);
#endif //MPFILELISTWIN_H
