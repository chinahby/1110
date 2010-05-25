/*======================================================
FILE:  AEEMediaMultipleFile.h

SERVICES:  AEE Media Multiple File Definitions

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc. for AEE
        Real Player.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "aeeclassids.h"
#include "AEEMedia.h"

#include "MediaMultipleFile.bid"

// 
// MIME base prefixes
//
#define MEDIAMULTIPLEFILE_MIME      "video/mft"


//*******************************************************************
//
// IMediaMultipleFile Interface
//
//*******************************************************************

/*-------------------------------------------------------------------
      IMediaReal Type Declarations
-------------------------------------------------------------------*/
typedef struct _IMediaMultipleFile  IMediaMultipleFile;

//-------------------------------------------------------------------
// IMediaReal Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaMultipleFile)
{
   INHERIT_IMedia(IMedia);
};



