/*=============================================================================
  FILE: MPUtils.h

  SERVICES: this file defines mediaplayer util objects
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPUtils.h#15 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPUTILS_H
#define MPUTILS_H

#include "AEEStdLib.h"
#include "AEEVector.h"
#include "AEEMedia.h"
#include "target.h"                    // Target configuration file
#include "customer.h"                  // Customer configuration file
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#endif //FEATURE_APP_MEDIA_VIDEO
#include "AEE.h"
#include "AEEComdef.h"
#include "AEEShell.h"
#include "AEEDisp.h"
#include "AEEFile.h"
#include "BREWVersion.h"

typedef struct {
  IVector*  m_pVector;
} stMPItemStore;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Define some types for mimicEncryptDnld
 */
typedef enum CMimicEncryptDnld
{
  MIMIC_ENCRYPT_DNLD_NONE = 0,  // Feature is disabled, always return MM_MPEG4_QTV_TO_HANDLE_CONTENT
  MIMIC_ENCRYPT_DNLD_ENABLE,    // Feature is enabled, may return MM_MPEG4_APP_TO_HANDLE_CONTENT
  MIMIC_ENCRYPT_DNLD_STOP       // Feature is stop, always return MM_MPEG4_QTV_TO_STOP
} CMimicEncryptDnld;

#if MIN_BREW_VERSION(4,0)
boolean MP_GetUIMMediaPlayerRoot(IFileMgr *pIFileMgr, char *pszFolderName, int folderNameSize);
#endif //#if MIN_BREW_VERSION(4,0)

stMPItemStore* MPItemStoreMake(IShell*  pIShell);
void MPItemStoreFree(stMPItemStore* pItemStore);

uint32  MPItemStoreGetSize(const stMPItemStore* pItemStore);
void* MPItemStoreGetAt(stMPItemStore* pItemStore, int idx);
// return the latest added item
void* MPItemStoreGetTop(stMPItemStore* pItemStore);

//add an item 
boolean MPStackPush(const stMPItemStore* pItemStore, void* pData);
// get the latest item and remove it from the list
void* MPStackPop(stMPItemStore* pItemStore);

// for debuging
void MPStackDump(stMPItemStore* pItemStore);

// return AECHAR string in an allocated memory  
AECHAR* MPWStrDup(const char* pName);
char* MPStrDup(const char* pName);

char *MP_MediaStateToString(int mediaState);
char *MP_ErrorStateToString(int errorState);

uint32 MPClipTimeInSeconds(uint32 timeMili);

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
int MP_CreateMedia(IShell * ps, AEEMediaData * pmd, IMedia ** ppm);
int MP_FindClass(IShell * ps, AEEMediaData * pmd, AEECLSID * pCls);
char* MP_GetMIMEType(unsigned char *pBuf, uint32 dwBufSiz);
#endif //#ifdef FEATURE_APP_MEDIA_VIDEO

void MP_FrameRect(IDisplay *pd, AEERect *pRect);
void MP_FitStaticTextNormal(IDisplay *pd, IStatic *ps, AECHAR *pszText);
void MP_FitStaticText(IDisplay *pd, IStatic *ps, AEEFont font, AECHAR *pszText);
char *MP_GetFileName(const char *psz);
char *MP_GetFileNameNoExtension(const char *psz, char *pszOut, int32 len);
char *MP_GetFileNameExtension(const char *pszFileName);
boolean MP_AddExtension(char **ppszExtList, char *psz);
boolean MP_isValidRect(AEERect rect, int16 nMinHeight, int16 nMinWidth);
boolean MP_IsMediaVideo(IShell * ps, char* pszFileName);

#ifdef __cplusplus
}
#endif

#endif //MPUTILS_H
