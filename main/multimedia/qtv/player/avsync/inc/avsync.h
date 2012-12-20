#ifndef _AVSYNC_H_
#define _AVSYNC_H_
/* =======================================================================
                               avsync.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/inc/avsync.h#13 $
$DateTime: 2009/11/27 05:33:40 $
$Change: 1097489 $


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
#include "qcplayer.h"
#include "qtvsessionclock.h"
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define QTV_MAX_UINT32 (0xFFFFFFFF)
// Rounded up value for 0x200000000LLU/0x5A
#define MAX_PTS_FOR_27MHZ_PCR (0x5B05B06)

#define NUM_FILTERS            3
#define DEC_CLK_FILTER_ID      1
#define WARM_RESET             1
#define DEC_CLK_FILTER_ID      1
#define ARM_CLK_FILTER_ID      2
#define PCR_FILTER_ID          0
#define COLD_RESET             0
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  AVSync

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class AVSync 
{

public:

  AVSync();
  ~AVSync(){};

  bool Init(QCCritSectType *pCS, QtvConfig *pQtvConfig);
  void Cleanup();

  enum AVType
  {
    AudioAV,
    VideoAV
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    ,TextAV
#endif
  };

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR 
#error code not present
#endif

  void ProhibitAVSync(bool bFlag);
  void ResetPlaybackOffset(AVType type);
  void SetPlaybackOffset(AVType type, long nClockTime, int nPresentationTime);
  long ReadPlaybackOffset(AVType type, long nClockTime, int nPresentationTime,bool bWriteAVoffset);
  bool IsOffsetValid(AVType type);
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

 /*  Audio session clock Object, driven by ARM clock */
SessionClock m_AudioRefClock;

 /*  Audio session clock Object, driven by AFE clock*/
SessionClock m_AudioAFEClock;

private:

  bool bProhibitAVSync;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  //the current offset.
  long playbackOffset[3]; 
  //has offset been set?
  bool bPlaybackOffsetValid[3]; 
#else
  //the current offset.
  long playbackOffset[2]; 
  //has offset been set?
  bool bPlaybackOffsetValid[2]; 
#endif

  QCCritSectType *pPlaybackCritSect;
  QtvConfig *qtvConfigObject;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
private:
  // whether media object associated is live or not 
  // Useful in hybrid streams to selective execute the 
  // proper procedures
  bool m_bLive;    

public:
  // SetLive & IsLive could be used externally 
  // Currently SetLive is tied up with PCR activation local to the class
  // In case of Live streams without PCR, SetLive could be used
  // externally also. 
  

  // Set the TSB source status 
  inline void SetLive(bool bLiveParam) 
  { 
    m_bLive = bLiveParam; 
    return;
  }

  // Return if truly live or not for repositionable live sources , such as TSB. 
  inline bool IsLive(void) 
  { 
    return m_bLive; 
  }
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /*FEATURE_QTV_GENERIC_BCAST_PCR*/
};  

#endif

