/* =======================================================================
                               audiorequestq.cpp
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
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiorequestq.cpp#8 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "audioRequestQ.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define CS_ENTER()   QCUtils::EnterCritSect(&CriticalSection)
#define CS_LEAVE()   QCUtils::LeaveCritSect(&CriticalSection)
#define CS_INIT()    QCUtils::InitCritSect(&CriticalSection)
#define CS_DINIT()   QCUtils::DinitCritSect(&CriticalSection)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION 
  AudioRequestQ::InitDataRequests

DESCRIPTION
//
// init empty data request queue.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void AudioRequestQ::InitDataRequests()
{
  CS_ENTER();
  nDataRequestCount=0;
  nDataRequestFirstOut=0;
  nDataRequestLastIn=(-1);
  CS_LEAVE();
}


/* ======================================================================
FUNCTION 
  AudioRequestQ::GetDataRequest

DESCRIPTION
//
// Get oldest data request, return true if success.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool AudioRequestQ::GetDataRequest(DataRequest &req)
{
  bool bSuccess=true;

  CS_ENTER();
  if ( nDataRequestCount>0 )
  {
    req = dataRequests[nDataRequestFirstOut];
    nDataRequestFirstOut = ((nDataRequestFirstOut + 1) % MAX_DATA_REQUESTS);
    nDataRequestCount--;
  }
  else
  {
    bSuccess=false;
  }
  CS_LEAVE();

  return bSuccess;
}

/* ======================================================================
FUNCTION 
  AudioRequestQ::SaveDataRequest

DESCRIPTION
//
// save a data request.  if "bNewRequest" is set then this request
// is the most recent.  Otherwise it's the oldest.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void AudioRequestQ::SaveDataRequest(
                                   const bool bNewRequest,
                                   uint32 max_len,
                                   cmx_client_req_cb_func_ptr_type data_req_cb,
                                   const void *server_client_data,
                                   uint32 offset,
                                   bool &bError) //output: error occurred.
{
  bool bAdded=false;
  bError=false;

  CS_ENTER();

  if ( nDataRequestCount<MAX_DATA_REQUESTS )
  {
    if ( nDataRequestCount<MAX_DATA_REQUESTS )
    {
      int i;
      if ( bNewRequest )
      {
        //add new request after current "last in" position.
        nDataRequestLastIn = ((nDataRequestLastIn+1) % MAX_DATA_REQUESTS);
        i = nDataRequestLastIn;
      }
      else
      {
        //add new request before current "first out" position.
        if ( nDataRequestFirstOut==0 )
        {
          nDataRequestFirstOut = MAX_DATA_REQUESTS;
        }
        nDataRequestFirstOut--;
        i = nDataRequestFirstOut;
      }
      dataRequests[i].max_len = max_len;
      dataRequests[i].data_req_cb = data_req_cb;
      dataRequests[i].server_client_data = server_client_data;
      dataRequests[i].stale = false;
      dataRequests[i].offset = offset;

      nDataRequestCount++;
      bAdded=true;
    }
  }

  CS_LEAVE();

  if ( !bAdded )
  {
    //no space in queue!  this should not happen, cmx is only
    //supposed to request 2 in advance.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "ERROR: Data Request Queue Overflow");
    bError=true;
  }
}

/* ======================================================================
FUNCTION
  AudioRequestQ::InvalidateDataRequests

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

========================================================================== */
void AudioRequestQ::InvalidateDataRequests()
{
  CS_ENTER();
  for (int i = 0; i < MAX_DATA_REQUESTS; i++)
  {
    dataRequests[i].stale = true;
  }
  CS_LEAVE();
}

/* ======================================================================
FUNCTION 
  AudioRequestQ::AudioRequestQ

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
AudioRequestQ::AudioRequestQ()
{
  memset(dataRequests, 0, sizeof(dataRequests) );
  CS_INIT();
  InitDataRequests();
}

/* ======================================================================
FUNCTION 
  AudioRequestQ::~AudioRequestQ

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
AudioRequestQ::~AudioRequestQ()
{
  CS_DINIT();
}
