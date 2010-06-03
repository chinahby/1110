/* =======================================================================
                              sdp_error.h
DESCRIPTION
  Defines the SDP_ERROR_CODE struct, which are used as messages indicating
  a successful operation, or the type of failure an operation encountered.

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
#ifndef __SDP_ERROR_H__
#define __SDP_ERROR_H__


/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/sdp_error.h#2 $
$DateTime: 2008/07/11 08:12:20 $
$Change: 700132 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAXIMUM_OUTPUT_STRING_SIZE	1000
#define SDP_INT_STRING_LENGTH	9
#define SDP_FLOAT_STRING_LENGTH	10
#define SDP_CR_LF 2
#define SDP_SPACE 1
#define MAX_PARSERS_SUPPORTED 8
#define MAX_CODEC_MODE_LIST 30
#define MAX_PAYLOADS 10		     /*Defined arbitrarily*/
#define MAX_PAYLOADFMT_NUM  2   /* maximum number of payload types per media */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum 
{
  SDP_SUCCESS,
  SDP_PARSER_SWITCH,
  SDP_VALUE_OUT_OF_RANGE,
  SDP_MISSING_ORIGIN_FIELD,
  SDP_MISSING_SESSION_NAME_FIELD,
  SDP_MISSING_AUTHOR_VERSION_FIELD,
  SDP_MISSING_AUTHOR_FIELD,
  SDP_MISSING_COPYRIGHT_FIELD,
  SDP_MISSING_RATING_FIELD,
  SDP_MISSING_CREATION_DATE_FIELD,
  SDP_MISSING_RANGE_FIELD,
  SDP_MISSING_WMF_VERSION_FIELD,
  SDP_MISSING_CONTROL_URL_FIELD,
  SDP_INVALID_MEDIA_PARAMETER,
  SDP_MISSING_MEDIA_DESCRIPTION,
  SDP_MISSING_RTP_MAP,
  SDP_OUTPUT_BUFFER_OVERFLOW,
  SDP_BAD_FORMAT,
  SDP_BAD_MEDIA_FORMAT,
  SDP_BAD_SESSION_FORMAT,
  SDP_PAYLOAD_MISMATCH,
  SDP_NOT_SUPPORTED,
  SDP_FAILURE
} SDP_ERROR_CODE;


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

#endif  // __SDP_ERROR_H__
