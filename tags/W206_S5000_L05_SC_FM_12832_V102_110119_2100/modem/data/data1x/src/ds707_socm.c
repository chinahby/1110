/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       D A T A   S E R V I C E S

              S E R V I C E   O P T I O N   C O N T R O L

GENERAL DESCRIPTION
  This file contains a handler function to parse and process service option
  control messages related to data service options.

EXTERNALIZED FUNCTIONS
  ds707_socm_parse_socm
    Called when a SOCM (service option control message) is received from
    the base station.  Function parses SOCM to see if it has info regarding
    either PZID or the dormant (holddown) timer. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2002 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_socm.c_v   1.1   19 Nov 2002 18:41:40   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_socm.c#3 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
11/17/02   ak      Updated file header comments.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707

#include "assert.h"
#include "cai.h"
#include "err.h"
#include "msg.h"
#include "snm.h"

#include "ds707.h"
#ifndef FEATURE_DATA_EPZID
#include "ds707_pzid.h"
#else
#include "ds707_epzid.h"
#endif /* FEATURE_DATA_EPZID */
#include "ds707_dorm_timer.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Field type codes for Packet Data service option control messages.
---------------------------------------------------------------------------*/
#define SOCMI_DORM_CNTL_FIELD_TYPE     0x03

/*---------------------------------------------------------------------------
  Field type codes for Packet Data service option control messages related
  to optional PZID based reconnection
---------------------------------------------------------------------------*/
#define SOCMI_PZID_CNTL_FIELD_TYPE     0x04

/*---------------------------------------------------------------------------
  Mask to get the field type bits.
---------------------------------------------------------------------------*/
#define SOCMI_FIELD_TYPE_MASK          0x07

/*---------------------------------------------------------------------------
  Union of all the Packet Data Service Option Control Messages.
---------------------------------------------------------------------------*/
typedef PACKED union
{
  byte                           cntl;        /* Control & Field Type      */
  ds707_dorm_timer_socm_msg_type dorm_timer;  /* Pkt Data dormant timer ctl*/
  ds707_pzid_socm_msg_type       pzid;        /* PZID related info.        */
} socmi_msg_type;


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION      DS707_SOCM_PARSE_SOCM

DESCRIPTION  This function processes service option control messages related
             to data service options. Currently, the only Service Option
             Control Message that is supported is for Packet Data Dormant
             Timer control.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_socm_parse_socm
(
  word  length,          /* Length of the type-specific fields (in bytes)  */
  byte *soctl_msg,       /* Pointer to the type-specific fields            */
  word  service_option   /* The service option that is currently connected */
)
{
  socmi_msg_type    *pkt_socm_msg;      /* Service Option Control Message  */
                                        /* for Packet data service options */
  byte                  field_type;     /* Field type of Service Option    */
                                        /* Control Message                 */  
  snm_socm_return_type  result = SNM_VALID; /* return value                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -esym(715, length) */

  /*-------------------------------------------------------------------------
    For now, Service Option Control Messages are processed only for Packet
    Data service options.
  -------------------------------------------------------------------------*/
  if( DS_ISDATA_PPP_PKT(service_option) )
  {
    pkt_socm_msg = (socmi_msg_type *)soctl_msg;

    /*-----------------------------------------------------------------------
      Get the Field Type bits.
    -----------------------------------------------------------------------*/
    field_type = pkt_socm_msg->cntl & SOCMI_FIELD_TYPE_MASK;

    switch(field_type)
    {
      case SOCMI_DORM_CNTL_FIELD_TYPE:
        result = ds707_dorm_timer_parse_socm(&(pkt_socm_msg->dorm_timer));
        break;

      /*---------------------------------------------------------------------
        Service option control message for optional Packet Zone ID based 
        reconnection.
      ---------------------------------------------------------------------*/
      case SOCMI_PZID_CNTL_FIELD_TYPE:
#ifdef FEATURE_DS_PZID_HYSTERESIS
        result = ds707_pzid_parse_socm(pkt_socm_msg->cntl, 
                                       &(pkt_socm_msg->pzid));
#else
        result = ds707_epzid_parse_socm(pkt_socm_msg->cntl, 
                                       &(pkt_socm_msg->pzid));
#endif /* FEATURE_DS_PZID_HYSTERESIS */
        break;

      default:
        MSG_MED("Unknown SO Ctl Msg w/ FIELD_TYPE: 0x%x", field_type, 0, 0);
        result = SNM_INVALID_MSG_TYP;
        break;
    }  /* switch */
  }
  else
  {
    MSG_HIGH("SO Ctl Msg rx'ed for Non-Packet SO", 0, 0, 0);
    result = SNM_MESSAGE_NOT_SUPPORTED;
  }

  return(result);
  /*lint +e715 */
} /* ds707_socm_parse_socm() */


#endif /* FEATURE_DATA_IS707 */
