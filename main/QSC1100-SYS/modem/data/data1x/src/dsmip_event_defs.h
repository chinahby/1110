#ifndef _DSMIP_EVENT_DEFS_H
#define _DSMIP_EVENT_DEFS_H

/*===========================================================================

   M O B I L E   I P   E V E N T S    D E F I N I T I O N S
                   
DESCRIPTION

 The Data Services Mobile IP Header File.  Contains the mobile I/P task
 entry point and exports any necessary information.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_event_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/07/03   js      Initial definition. 
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "event.h"
#include "event_defs.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

#define UNDEFINED 0

/*--------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_ENUM_TYPE

  Type of mobile IP events to be generated. Translated by the tool to 
  strings. 
--------------------------------------------------------------------------*/
typedef enum
{
  event_mip_asm_sent,
  event_mip_aam_rcvd,
  event_mip_rrq_sent,
  event_mip_rrp_rcvd
} mip_event_enum_type;


/*--------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_INFO_ENUM

  Some typical values to be used by the info field in the event payload. 
  Not translated by the tools. 
--------------------------------------------------------------------------*/
enum mip_event_info_enum
{
   MIP_EVENT_INFO_NA = 0,           /* info filed no applicable/available */  
   MIP_EVENT_INFO_SENT_OTA,         /* Sent over the air (for ASM, RRQ)   */
   MIP_EVENT_INFO_SENT_FAIL         /* Unsuccessful sent                  */
};


/*--------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_PAYLOAD_TYPE

  MIP event payload type. 
--------------------------------------------------------------------------*/
typedef struct
{
   mip_event_enum_type mip_event;
   int                 info;
}mip_event_payload_type;


#endif /* DSMIP_EVENT_DEFS_H */


