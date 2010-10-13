/* =======================================================================
         sdpfile.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/sdpfile.cpp#4 $
$DateTime: 2008/09/18 21:24:40 $
$Change: 746805 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "sdpfile.h"

//forward dec
SDPInterfaceT sdpIF;
SDP_Parser sdpParser;

const char *SDPSessionDescription::getControlURL() const
{
  return NULL;
}

const SDPSessionDescription *SDPInfo::getSessionDescription() const 
{
  return NULL;
}

SDP_ERROR_CODE SDP_Parser::parseSDPandHandleErrors(const char *sdpText, 
                                                   int text_length, 
                                                   SDPInfo *sdp)
{
  QTV_USE_ARG1(sdpText);
  QTV_USE_ARG2(text_length,sdp);
  return SDP_FAILURE;
}
void SDP_Parser::init(QtvConfig *configObject)
{
  QTV_USE_ARG1(configObject);
}

SDPFile::SDPFile()
{
}
SDPFile::~SDPFile()
{
}
ERROR_CODE SDPFile::ParseApplicationFile(const URL &URN)
{
  QTV_USE_ARG1(URN.MaxUrlSize);
  return PV_SERVER_FAILURE;
}
QtvPlayer::URLTypeT SDPFile::GetSDPURLType() 
{
  return URLType; 
}
KEY_DEF * SDPFile::GetSDPKey()
{
  return 0; 
}
SDPInfo * SDPFile::GetSDPInfo()
{
  return NULL; 
}
URL & SDPFile::GetSDPURL() 
{
  return pURLString;
}

void SDPInterfaceT::SetConfigObject(QtvConfig *config)
{
  QTV_USE_ARG1(config);
}

