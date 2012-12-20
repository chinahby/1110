/* =======================================================================
                              PlayerBaseIF.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/src/PlayerBaseIF.cpp#6 $
$DateTime: 2010/04/23 00:39:40 $
$Change: 1276762 $

========================================================================== */



/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "Events.h"
#include "QtvConfig.h"
#include "PlayerBaseIF.h"
#include "Video.h"


#ifdef FEATURE_QTV_DRM_DCF
 #include "zrex.h"
 #include "rex.h"
 #include "ixipc.h"
#endif

////////////////////////////////////////////////////////////////////
//
// video thread event table
//
////////////////////////////////////////////////////////////////////

// Source
EVENT_POOL( VIDEO_POOL, 10 )
  //to player
EVENT_SOURCE( VIDEO_STATUS, VIDEO_POOL )
  //EVENT_SOURCE( AV_TICK, VIDEO_POOL ); //already in audio pool



  /* Creates an Instance of Video Player */
PlayerBaseIF* PlayerBaseIF::CreateVideoPlayerInstance()
{
    PlayerBaseIF* pActiveVideoPlayer = NULL;

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

    pActiveVideoPlayer = (PlayerBaseIF*)(QTV_New(VideoPlayer));

    if(pActiveVideoPlayer)
    {
        if(!(pActiveVideoPlayer->IsVideoPlayerAvailable()))
        {
            QTV_Delete(pActiveVideoPlayer);
            pActiveVideoPlayer = NULL;
        }
    }

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

    return pActiveVideoPlayer;
}

/* Destroys the Instance of Video Player */
void PlayerBaseIF::DestroyVideoPlayerInstance(PlayerBaseIF* pActiveVideoPlayer)
{
  if(pActiveVideoPlayer != NULL)
  {
#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER
    QTV_Delete(pActiveVideoPlayer);
#else
    QTV_USE_ARG1(pActiveVideoPlayer);
#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */
  }
}


