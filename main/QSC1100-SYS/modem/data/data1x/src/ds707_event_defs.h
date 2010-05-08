#ifndef _DS707_EVENT_DEFS_H
#define _DS707_EVENT_DEFS_H

/*===========================================================================

             D S 7 0 7   E V E N T S    D E F I N I T I O N S

DESCRIPTION

 The Data Services 707 events Header File. Contains the definition for 707 
 events.

 Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_event_defs.h#1 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/08   sn      Initial definition for 707 events.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "event.h"

/*===========================================================================

                             EXPORTED DEFINITIONS

===========================================================================*/

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_ENUM_TYPE

  Type enumerating the possible  ds 707 (pkt manager) events to be generated. 
  Translated by the tool into strings. 
---------------------------------------------------------------------------*/

typedef enum
{
  EVENT_DS707_PPP_RESYNC = 0, 
  EVENT_DS707_ASYNC_ORIG =            1,
  EVENT_DS707_ASYNC_CALL_CONNECTED =  2,
  EVENT_DS707_ASYNC_INCOMING_CALL =   3,
  EVENT_DS707_ASYNC_CALL_ENDED =      4,
  EVENT_DS707_PKT_ORIG =              5,
  EVENT_DS707_PKT_CALL_CONNECTED =    6,
  EVENT_DS707_PKT_INCOMING_CALL =     7,
  EVENT_DS707_PKT_CALL_ENDED =        8,
  EVENT_DS707_DATA_CALL_REQ_ACCEPTED= 9,   /* Data call request accepted */
  EVENT_DS707_DATA_CALL_REQ_REJECTED=10,   /* Data call request rejected */
  EVENT_DS707_INVALID  = 255
} ds707_event_enum_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_INFO_ENUM

  Possible values for the info field for ds707 ppp_resync event. 
  Translated by the tool into strings.
---------------------------------------------------------------------------*/

/* This ENUM is not used and is just for compilation */
typedef enum
{
  DS707_RESYNC_ON_HDR_1X_HANDOFF = 0,
  DS707_RESYNC_ON_ALN = 1,
  DS707_RESYNC_ON_QCODH = 2,
  DS707_RESYNC_INVALID_REASON = 255
} ds707_event_resync_info_enum_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_INFO_TYPE

  Union of info contents in 707 events depending on 'ds707_event' type
---------------------------------------------------------------------------*/

typedef union
{
  uint32                            zero;     /* use this to zero structure */
  ds707_event_resync_info_enum_type i_resync; /* info on ppp_resync event */
} ds707_event_info_type;

/*---------------------------------------------------------------------------
  TYPEDEF DS707_EVENT_PAYLOAD_TYPE

  DS707 event payload type. 
---------------------------------------------------------------------------*/

typedef struct
{
  ds707_event_enum_type ds707_event;     /* event                       */
  ds707_event_info_type info;            /* info field (event specific) */
} ds707_event_payload_type;

#endif /* DS707_EVENT_DEFS_H */
