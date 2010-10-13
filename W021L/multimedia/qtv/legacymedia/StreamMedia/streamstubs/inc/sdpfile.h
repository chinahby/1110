/* =======================================================================
         sdpfile.h
DESCRIPTION
   This module defines the sdpfile class.

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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/sdpfile.h#2 $
$DateTime: 2008/07/11 08:12:20 $
$Change: 700132 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef SDPFILE_H_
#define SDPFILE_H_

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "AppFile.h"
#include "QtvPlayer.h"
#include "QtvConfig.h"  
#include "sdp_error.h"

class SDPSessionDescription
{
public:
  SDPSessionDescription()
  {
  };
  ~SDPSessionDescription()
  {
  };
  const char *getControlURL() const;
};


class SDPInfo 
{
public:
  SDPInfo()
  {
  };
  ~SDPInfo()
  {
  };
  const SDPSessionDescription *getSessionDescription() const ;

public:
  bool isSDPInfoLocallyAllocated;
};
class SDP_Parser
{

public:
  SDP_Parser()
  {
  };
  ~SDP_Parser()
  {
  };

  enum SdpMediaFormatT
  {
    SDP_MEDIA_FORMAT_UNKNOWN,
    SDP_MEDIA_FORMAT_AUDIO,
    SDP_MEDIA_FORMAT_VIDEO
  };

  SDP_ERROR_CODE parseSDPandHandleErrors(const char *sdpText, 
                                         int text_length, 
                                         SDPInfo *sdp);
  void init(QtvConfig *configObject);
};



class SDPFile
{
private:
  QtvPlayer::URLTypeT URLType;
  QtvPlayer::URNTypeT URNType;
  URL                 URN;
  URL                 pURLString;
  KEY_DEF           * pKey;
  SDPInfo             sdpInfo;
  SDP_Parser          sdpParser;

public:
  SDPFile();
  ~SDPFile();
  ERROR_CODE ParseApplicationFile(const URL &URN);
  QtvPlayer::URLTypeT GetSDPURLType() ;
  KEY_DEF * GetSDPKey();
  SDPInfo * GetSDPInfo();
  URL & GetSDPURL();
  SDP_Parser &getSDPParser()
  {
    return sdpParser;
  }

};

class SDPInterfaceT
{
public:
  SDPInterfaceT()
  {
  };
  ~SDPInterfaceT()
  {
  };
  SDPFile  sdpFile;
  SDP_Parser         sdpParser;
  URL                url;
  QtvPlayer::URLTypeT URLType;
  KEY_DEF          * pKey;
  SDPInfo          * pSDPInfo;
  bool               bValid;
  int                nPlaylistLength;
  int                nPlaylistIndex;
  bool               bPlaylistComplete;

  void SetConfigObject(QtvConfig *config);
};

#endif
