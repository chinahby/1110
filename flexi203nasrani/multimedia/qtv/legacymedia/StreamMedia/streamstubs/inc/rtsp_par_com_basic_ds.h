/* =======================================================================
                              rtsp_par_com_basic_ds.h
DESCRIPTION
  This file contains enumerations and macros for the RTSP - Real Time
  Streaming Protocol.  It defines the status codes, and provides a few macros
  to interpret the status codes.
	
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/rtsp_par_com_basic_ds.h#2 $
$DateTime: 2008/07/11 08:12:20 $
$Change: 700132 $

========================================================================== */
/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef RTSP_PAR_COM_BASIC_DS_H_
#define RTSP_PAR_COM_BASIC_DS_H_

#include <stdlib.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include "oscl_str_ptr_len.h"
#include "rtprtcp.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/*
   Enumeration of the methods and method-like items known to the RTSP
 engine.  Not all are supported.
*/
enum RTSPMethod
{
   METHOD_UNRECOGNIZED,  // not a method
   METHOD_DESCRIBE,
   METHOD_GET_PARAMETER,
   METHOD_OPTIONS,
   METHOD_REDIRECT,
   METHOD_PAUSE,
   METHOD_PLAY,
   METHOD_SETUP,
   METHOD_RECORD,
   METHOD_TEARDOWN,
   METHOD_SET_PARAMETER,
   METHOD_ANNOUNCE,
   METHOD_END_OF_STREAM, 
   METHOD_BINARY_DATA,    // not a method
#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
   METHOD_PLAYLIST_PLAY,
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
   METHOD_SETUP_TRACK_FOR_WM,
   METHOD_TEARDOWN_TRACK_FOR_WM,
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */
   METHOD_COUNT // not a real method, denotes number of real or pseudo methods
};


// type of message
enum RTSPMsgType
{
   RTSPRequestMsg,
   RTSPResponseMsg,
   RTSPUnknownMsg

};

// Type of SET_PARAMETER message
enum setParameterType
{
  SET_PARAMETER_NA, // not applicable
  SET_PARAMETER_LOG_CONNECT_STATS,
  SET_PARAMETER_LOG_PLAY_STATS,
  SET_PARAMETER_STREAM_SWITCH,
  SET_PARAMETER_QOS_UPDATE
};

// return type for isMalformed
enum RTSPStructureStatus
{
   RTSPOk,
   RTSPErrorVersion,
   RTSPErrorTooBig,
   RTSPErrorTooManyFields,
   RTSPErrorSyntax
};


// RTSP response code values
typedef uint16 RTSPStatusCode;

// RTSP session id
typedef StrPtrLen RTSPSessionId;

// RTSP entity body
typedef struct MemoryFragment RTSPEntityBody;

const RTSPStatusCode
   Code0 =                                 0,  // No code available
   CodeNone =                              0,  

   /* informational statuses */
   Code100 =                             100,  // continue
   CodeContinue =                        100,

   /* success statuses */
   Code200 =                             200,  // ok
   CodeOK =                              200,

   Code201 =                             201,  // created
   CodeCreated =                         201,

   Code250 =                             250,  // low on storage space
   CodeLowOnStorageSpace =               250,

   /* redirection statuses */
   Code300 =                             300,  // multiple choices
   CodeMultipleChoices =                 300,

   Code301 =                             301,  // moved permanently
   CodeMovedPermanently =                301,

   Code302 =                             302,  // moved temporarily
   CodeMovedTemporarily =                302,

   Code303 =                             303,  // see other
   CodeSeeOther =                        303,

   Code304 =                             304,  // not modified
   CodeNotModified =                     304,

   Code305 =                             305,  // use proxy
   CodeUseProxy =                        305,

   /* client error statuses */
   Code400 =                             400,  // bad request
   CodeBadRequest =                      400,

   Code401 =                             401,  // unauthorized
   CodeUnauthorized =                    401,

   Code402 =                             402,  // payment required
   CodePaymentRequired =                 402,

   Code403 =                             403,  // forbidden
   CodeForbidden =                       403,

   Code404 =                             404,  // not found
   CodeNotFound =                        404,

   Code405 =                             405,  // method not allowed
   CodeMethodNotAllowed =                405,

   Code406 =                             406,  // not acceptable
   CodeNotAcceptable =                   406,

   Code407 =                             407,  // proxy authentication required
   CodeProxyAuthenticationRequired =     407,

   Code408 =                             408,  // request time-out
   CodeRequestTimeOut =                  408,

   Code410 =                             410,  // gone
   CodeGone =                            410,

   Code411 =                             411,  // length required
   CodeLengthRequired =                  411,

   Code412 =                             412,  // precondition failed
   CodePreconditionFailed =              412,

   Code413 =                             413,  // request entity too large
   CodeRequestEntityTooLarge =           413,

   Code414 =                             414,  // request-uri too large
   CodeRequestURITooLarge =              414,

   Code415 =                             415,  // unsupported media type
   CodeUnsupportedMediaType =            415,

   Code451 =                             451,  // parameter not understood
   CodeParameterNotUnderstood =          451,

   Code452 =                             452,  // conference not found
   CodeConferenceNotFound =              452,

   Code453 =                             453,  // not enough bandwidth
   CodeNotEnoughBandwidth =              453,

   Code454 =                             454,  // session not found
   CodeSessionNotFound =                 454,

   Code455 =                             455,  // method not valid in this state
   CodeMethodNotValidInThisState =       455,

   Code456 =                             456,  // header field not valid for resource
   CodeHeaderFieldNotValidForResource =  456,

   Code457 =                             457,  // invalid range
   CodeInvalidRange =                    457,

   Code458 =                             458,  // parameter is read-only
   CodeParameterIsReadOnly =             458,

   Code459 =                             459,  // aggregate operation not allowed
   CodeAggregateOperationNotAllowed =    459,

   Code460 =                             460,  // only aggregate operation allowed
   CodeOnlyAggregateOperationAllowed =   460,

   Code461 =                             461,  // unsupported transport
   CodeUnsupportedTransport =            461,

   Code462 =                             462,  // destination unreachable
   CodeDestinationUnreachable =          462,

   Code480 =                             480,  // unsupported client
   CodeUnsupportedClient =               480,

   /* server error statuses */
   Code500 =                             500,  // internal server error
   CodeInternalServerError =             500,

   Code501 =                             501,  // not implemented	
   CodeNotImplemented =                  501,

   Code502 =                             502,  // bad gateway
   CodeBadGateway =                      502,

   Code503 =                             503,  // service unavailable
   CodeServiceUnavailable =              503,

   Code504 =                             504,  // gateway timeout
   CodeGatewayTimeout =                  504,

   Code505 =                             505,  // rtsp version not supported
   CodeRTSPVersionNotSupported =         505,

   Code551 =                             551,  // option not supported
   CodeOptionNotSupported =              551,

   /* alias status code range values */
   CodeInformationalBegin =              100,
   CodeInformationalEnd   =              199,
   CodeSuccessBegin       =              200,
   CodeSuccessEnd         =              299,
   CodeRedirectionBegin   =              300,
   CodeRedirectionEnd     =              399,
   CodeClientErrorBegin   =              400,
   CodeClientErrorEnd     =              499,
   CodeServerErrorBegin   =              500,
   CodeServerErrorEnd     =              599
;

/* Macro to say whether a rtsp status code is "SUCCESS" */
#define RTSP_SUCCESS(rtsp_val) \
 ( (rtsp_val >= CodeSuccessBegin) && \
   (rtsp_val <= CodeSuccessEnd) )

/* Macro to say whether RTSP status code is a redirection */
#define RTSP_REDIRECTION(rtsp_val) \
 ( (rtsp_val >= CodeRedirectionBegin) && \
   (rtsp_val <= CodeRedirectionEnd) )

typedef struct
{
   StrPtrLen     url;
   bool          urlIsSet;

   RtpSeqType    seq;
   bool          seqIsSet;

   RtpTimeStamp  rtptime;
   bool          rtptimeIsSet;

   uint32        ssrc;
   bool          ssrcIsSet;

} RTSPRTPInfo;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/* =======================================================================
**                        Class Declarations
** ======================================================================= */
#endif // RTSP_PAR_COM_BASIC_DS_H_
