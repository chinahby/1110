/*=============================================================================
  FILE: MPContent.h

  SERVICES: this file defines MPContent object and functions
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPContent.h#12 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPCONTENT_H
#define MPCONTENT_H

#include "MediaPlayer.h"

#ifdef MP_FEATURE_ICONTENT
#include "AEEIContent.h"
#include "AEEICategory.h"
#include "MPUtils.h"

typedef struct CMediaPlayer CMediaPlayer;

typedef struct stContentUpdate
{
 char*        m_pszURI;
 char*        m_pszLocalURL;
 char*        m_pszMimeType;
 AECHAR*      m_pwszName;
 AECHAR*      m_pwszDesc;
 
 AEECallback  m_cbUpdateCallback;
 int32        m_nRetContentUpdate;
 IContentMgr* m_pIContentMgr;
}stContentUpdate;

struct stContent
{
  CMediaPlayer*       m_pOwner;       // point to MediaPlayer

  char*               m_pszFile;
  stMPItemStore*      m_pStack;        // store URI AEE string, this is browsing level,
                                       // size equal to zero indicates root
  stContentUpdate     m_CTUpdate;      // used when updating content attribute
};

stContent* MPContentMake(CMediaPlayer* pme);
void MPContentFree(stContent* pct);

// browse content DB
void MPContentBrowse(stContent* pct);
// this function will find the local URL string from given URI id
int MPContentGetLocalURLfromURI(stContent *pct, const char* szURI, char* pstrOutput, int length);
int MPContentGetURIfromLocalURL(stContent *pct, const char* pszURL, char* pstrOutput, int length);

int MPContentSelectFile(stContent* pct, const char* szURI, uint16 nFileIdx, boolean bPlay);
#ifdef FEATURE_MULTISEQUENCER
int MPContentSequencerSelectFiles(stContent *pct);
#endif

// add an entry in content DB
//   pszCatName, category name
//   pszCatDesc, category description (option)
//   pszFile, file path and name
//   pszFileDesc, file description (option)
//   pstrMimeType, file mine type
void MPContentAdd(stContent* pct, 
                  const char* pszCatName, const char* pszCatDesc,
                  const char* pszFile,    const char* pszFileDesc, 
                  const char* pstrMimeType);

// delete media from IContent DB
int MPContentDelete(stContent* pct, char *pszURI);

#endif //MP_FEATURE_ICONTENT
#endif //MPCONTENT_H
