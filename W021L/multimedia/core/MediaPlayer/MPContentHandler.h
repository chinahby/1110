/*=============================================================================
  FILE: MPContentHandler.h

  SERVICES: this file defines MPContentHandler object and functions
 
  GENERAL DESCRIPTION: 


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPContentHandler.h#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#ifndef MPCONTENTHANDLER_H
#define MPCONTENTHANDLER_H

#include "MediaPlayer.h"
#ifdef MP_FEATURE_ICONTENTHANDLER

#include "AEEIContentHandler.h"

typedef struct stIContentHandlerSelectFile stIContentHandlerSelectFile;
struct stContent
{
  CMediaPlayer*       m_pOwner;       // point to MediaPlayer
  CHttpBlock*         m_psHttpBlock;  // Used for url based content handling
  boolean             m_bPlay;        // Boolean flag passed through URL handling
  stIContentHandlerSelectFile* m_pSF;
};

stContent* MPContentMake(CMediaPlayer* pme);
void MPContentFree(stContent* pct);
int MPContentSelectFile(stContent* pct, const char* szURI, uint16 nFileIdx, boolean bPlay);
int MPContentSelectFileUrl(stContent* pct, const char* szURL, uint16 nFileIdx, boolean bPlay);

#endif //MP_FEATURE_ICONTENTHANDLER
#endif //MPCONTENTHANDLER_H
