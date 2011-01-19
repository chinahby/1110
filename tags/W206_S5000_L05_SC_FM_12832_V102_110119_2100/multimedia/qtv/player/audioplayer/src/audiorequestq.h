#ifndef _AUDIOREQUESTQ_H_
#define _AUDIOREQUESTQ_H_
/* =======================================================================
                               audiorequestq.h
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

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiorequestq.h#7 $
$DateTime: 2008/05/08 08:09:22 $
$Change: 656074 $


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
extern "C" {
#include "cmx.h"
}

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
// Maximum queued data requests.  I've been told CMX stops sending requests
// once 2 are outstanding, so this allows a safe margin.
#define MAX_DATA_REQUESTS 4

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
  AudioRequestQ

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
class AudioRequestQ 
{

public: 
  AudioRequestQ();
  ~AudioRequestQ();

  //A cmx data request
  struct DataRequest
  {
    uint32 max_len;
    cmx_client_req_cb_func_ptr_type data_req_cb;
    const void *server_client_data;
    bool stale;
    uint32 offset;
  };

  //Get the oldest data request
  bool GetDataRequest(DataRequest &req);

  //Save a data request
  void SaveDataRequest(
    const bool bNewRequest,
    uint32 max_len,
    cmx_client_req_cb_func_ptr_type data_req_cb,
    const void *server_client_data,
    uint32 offset,
    bool &bError);

  int NumberOfQueuedItems()
  {
    return nDataRequestCount;
  }

  void InvalidateDataRequests();

private:

  //Init empty queue
  void InitDataRequests();

  //Data request queue
  QCCritSectType CriticalSection;
  DataRequest dataRequests[MAX_DATA_REQUESTS] ;
  int nDataRequestFirstOut;
  int nDataRequestLastIn;
  int nDataRequestCount;
};

#endif //_AUDIOREQUESTQ_H_  

