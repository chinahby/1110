/* =======================================================================
         streammedia.cpp
DESCRIPTION
   This module defines the streammedia class.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/streammedia.cpp#3 $
$DateTime: 2008/07/29 21:17:21 $
$Change: 712884 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "streammedia.h"
#include "qtvstreambase.h"
#include "QtvConfig.h" 


StreamMedia::StreamMedia(void* pContext, QtvStream *pStream, QtvConfig *configObject)
{
  QTV_USE_ARG2(pContext, pStream);
  QTV_USE_ARG1(configObject);
}
StreamMedia::~StreamMedia()
{
}
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
int32 StreamMedia::GetNumClipsInServerSidePlaylist() const
{
  return 0;
}
int   StreamMedia::IsPreviousClipInServerSidePlaylistAvailable() const
{
  return 0;
}
int   StreamMedia::IsNextClipInServerSidePlaylistAvailable() const
{
  return 0;
}
ServerSidePlaylistType StreamMedia::GetServerSidePlaylistType()
{
  return SERVER_SIDE_PLAYLIST_TYPE_NONE;
}
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
uint32 StreamMedia::GetAudioSampleRate(uint32 trackIdx)
{
  QTV_USE_ARG1(trackIdx);
  return 0;
}
void StreamMedia::ResetError()
{
  return;
}
bool StreamMedia::GetLaunchURL(OSCL_STRING& launchUrl)
{
  QTV_USE_ARG1(launchUrl.get_size());
  return FALSE;
}
EndOfClipActionT StreamMedia::GetEndofClipAction()
{
  return NO_END_OF_CLIP_ACTION;
}
bool StreamMedia::IsLive()
{
  return FALSE;
}
#ifdef FEATURE_QTV_MCAST
bool StreamMedia::IsMcast()
{
  return FALSE;
}
#endif /* FEATURE_QTV_MCAST */
bool StreamMedia::CheckError(const Common::Mp4ErrorContext context) 
{
  QTV_USE_ARG1(context);
  return FALSE;
}
StreamMedia *StreamMedia::OpenStream(void *pContext, QtvStream *pStream, 
                                     QtvConfig *configObject)
{
  QTV_USE_ARG2(pContext, pStream);
  QTV_USE_ARG1(configObject);
  return NULL;
}

