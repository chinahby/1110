/*=============================================================================
  FILE: MPConfigFile.h

  SERVICES: this file defines functions to handle MediaPlayer 
            configuration read and write operations

  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPConfigFile.h#1 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPCONFIGFILE_H
#define MPCONFIGFILE_H
#include "MediaPlayer.h"

// this function reads config data from .ini and .dat files
int MPConfigFile_Read(CMediaPlayer *pme);
// this function writes config data to .ini and .dat files
int MPConfigFile_Write(CMediaPlayer *pme);

#endif //MPCONFIGFILE_H
