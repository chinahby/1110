/* =============================================================================
                               avsync.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
============================================================================= */

/* =============================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/src/avsync.cpp#11 $
$DateTime: 2008/09/23 06:32:03 $
$Change: 748913 $


============================================================================= */

/* =============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================= */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "AVSync.h"
#include <limits.h>
#include <stdlib.h>

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

/* =============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
// debug support
#define SHOW_SYNC
#define DEC_OR_ARM_CLK_DISCONTINUITY_THRESHOLD_CONST     1350000 
#define DEC_OR_ARM_CLK_DISCONTINUITY_THRESHOLD_PPM       20

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

/* ======================================================================
FUNCTION
  AVSync::ProhibitAVSync

DESCRIPTION
  if set, stops the AV sync all togather.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void AVSync::ProhibitAVSync(bool bFlag)
{
  QCUtils::EnterCritSect(AVSync::pPlaybackCritSect);
  AVSync::bProhibitAVSync = bFlag;
  QCUtils::LeaveCritSect(AVSync::pPlaybackCritSect);
}

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  AVSync::ResetPlaybackOffset

DESCRIPTION
//
// reset playback offset for current playback. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void AVSync::ResetPlaybackOffset(AVType type)
{
  int index;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVSync::ResetPlaybackOffset");

  if(AVSync::bProhibitAVSync)
    index = (int)type;
  else
    index = 0;

  QCUtils::EnterCritSect(AVSync::pPlaybackCritSect);
  AVSync::bPlaybackOffsetValid[index] = false;
  QCUtils::LeaveCritSect(AVSync::pPlaybackCritSect);
}

/* ======================================================================
FUNCTION
  AVSync::ReadPlaybackOffset

DESCRIPTION
// 
// read the playback time offset.  if it is not currently
// set, then this will set it on the first call using the provided
// clock time and presentation time.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
long AVSync::ReadPlaybackOffset(AVType type, long nClockTime, int nPresentationTime, bool bWriteAVSyncOffset)
{
  long n;
  int index;

  if(AVSync::bProhibitAVSync)
  {
    index = (int)type;
    if( !AVSync::bPlaybackOffsetValid[index] )
    {
      switch(type)
      {
          case AudioAV:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Audio is trying to set playback offset");
        break;
          case VideoAV:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Video is trying to set playback offset ");
        break;
          case TextAV:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Text is trying to set playback offset ");
        break;
      }
    }
  }
  else
    index = 0;
 
  if (false == bWriteAVSyncOffset)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Type %d not allowed to set av sync offset",type);       
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"AV sync index is mapped to type %d", index);
  }
  
  QCUtils::EnterCritSect(AVSync::pPlaybackCritSect);
  if( (!AVSync::bPlaybackOffsetValid[index]) &&
       (bWriteAVSyncOffset||AVSync::bProhibitAVSync) )
  {
    
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "A/V Passive Init%d(Clock %ld, Pres %ld)",
       type, nClockTime, nPresentationTime);

    //init the offset to the given timing.
    AVSync::playbackOffset[index] = nClockTime - nPresentationTime;
    AVSync::bPlaybackOffsetValid[index] = true;
  }
  //offset is now valid, read it.
  n = AVSync::playbackOffset[index];
  QCUtils::LeaveCritSect(AVSync::pPlaybackCritSect);

#ifdef SHOW_SYNC
  QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "A/V read%d: off=%ld(Clock %ld, Pres %ld)", 
     type, AVSync::playbackOffset[index], nClockTime, nPresentationTime);
#endif

  return n;
}

/* ======================================================================
FUNCTION
  AVSync::SetPlaybackOffset

DESCRIPTION
//
// set or update playback offset based on given rendering time and
// presentation time

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void AVSync::SetPlaybackOffset(AVType type, long nRenderingTime, int nPresentationTime)
{
  int index;

  if(AVSync::bProhibitAVSync)
  {
    index = (int)type;
    switch(type)
    {
       case AudioAV:
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, " Audio is setting playback offdset");
       break;
       case VideoAV:
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, " Video is setting playback offset ");
       break;
       case TextAV:
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, " Text is setting playback offset ");
       break;
     }
  }
  else
    index = 0;

  long prev_offset = AVSync::playbackOffset[index];

  // Set the offset
  QCUtils::EnterCritSect(AVSync::pPlaybackCritSect);
  AVSync::playbackOffset[index] = nRenderingTime - nPresentationTime;
  bPlaybackOffsetValid[index] = true;
  QCUtils::LeaveCritSect(AVSync::pPlaybackCritSect);

  if (abs((prev_offset-AVSync::playbackOffset[index])) > 1)
  {
    // Only want to print this when the time actually changes, as opposed
    // to common 1ms drifts up or down (which are due to floating point 
    // divide error)
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
             "AV time offset has been changed - old: %d, new: %d",
             prev_offset, AVSync::playbackOffset[index]);
  }


#ifdef SHOW_SYNC
  QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "A/V Set%u: Render %ld,Pres %ld,Offset %ld)", 
     type, nRenderingTime, nPresentationTime, AVSync::playbackOffset[index]);
#endif
}

/* ======================================================================
FUNCTION
  AVSync::IsOffsetValid

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
bool AVSync::IsOffsetValid(AVType type)
{
  int index;

  if(AVSync::bProhibitAVSync)
  {
    index = (int)type;
  }
  else
  {
    index = 0;
  }
  return bPlaybackOffsetValid[index];
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

/* ======================================================================
FUNCTION
  AVSync::Init

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
bool AVSync::Init(QCCritSectType *pCS, QtvConfig *pQtvConfig)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "AVSync::Init()");
   AVSync::pPlaybackCritSect = pCS;
   AVSync::bProhibitAVSync = false;
   AVSync::qtvConfigObject = pQtvConfig;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
   return (AVSync::pPlaybackCritSect!=NULL);
}

/* ======================================================================
FUNCTION
  AVSync::Cleanup

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
void AVSync::Cleanup()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "AVSync::Cleanup()");
  AVSync::pPlaybackCritSect = NULL;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}

/* ======================================================================
FUNCTION
  AVSync::AVSync

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
AVSync::AVSync()
: qtvConfigObject( 0 )
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif // FEATURE_QTV_GENERIC_BCAST_PCR
{

//
// These are used to sync two playback streams.  One stream is the
// reference, and one is dependent.  We maintain synchronization by
// keeping the same offset between presentation time and clock time
// in both streams.  Each frame in the dependent stream should play at: 
//     clock time = presentation time + playback offset
//  
// The reference stream sets the offset using SetPlaybackOffset and
// UpdatePlaybackOffset.
// The dependent stream reads the offset using ReadPlaybackOffset.  
// In case the reference stream does not set the offset, the timing will 
// follow the dependent stream until the reference stream sets it.
//

  pPlaybackCritSect = NULL;

  bProhibitAVSync = FALSE;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  for(uint8 index=0; index < 3; index++)
  {
    //the current offset.
    playbackOffset[index] = 0;  
    //has offset been set?
    bPlaybackOffsetValid[index] = 0;
  }
#else
  for(uint8 index=0; index < 2; index++)
  {
    //the current offset.
    playbackOffset[index] = 0;  
    //has offset been set?
    bPlaybackOffsetValid[index] = 0;
  }
#endif
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "Player mode initialized to buffered");
  SetLive(false);
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /*FEATURE_QTV_GENERIC_BCAST_PCR */
