/* =======================================================================
         streammedia.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/streammedia.h#4 $
$DateTime: 2008/09/18 21:24:40 $
$Change: 746805 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef STREAMMEDIA_H_
#define STREAMMEDIA_H_

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtvstreambase.h"
#include "Media.h"

class QtvStream;
class QtvConfig;

class StreamMedia: public Media
{
public:
  StreamMedia(void* pContext, QtvStream * pStream, QtvConfig *configObject);
  virtual ~StreamMedia();


#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  int32 GetNumClipsInServerSidePlaylist() const;
  int   IsPreviousClipInServerSidePlaylistAvailable() const;
  int   IsNextClipInServerSidePlaylistAvailable() const;
  ServerSidePlaylistType GetServerSidePlaylistType();
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
  uint32 GetAudioSampleRate(uint32 trackIdx = 0);
  void ResetError();
  virtual bool GetLaunchURL(OSCL_STRING& launchUrl);
  EndOfClipActionT GetEndofClipAction();
  virtual bool IsLive();
#ifdef FEATURE_QTV_MCAST
  bool IsMcast();
#endif /* FEATURE_QTV_MCAST */
  virtual bool CheckError(const Common::Mp4ErrorContext context) ;
  static StreamMedia *OpenStream(void *pContext, QtvStream *pStream, 
                          QtvConfig *configObject);

};

#endif

