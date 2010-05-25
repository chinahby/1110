#ifndef DS707_EPZID_H
#define DS707_EPZID_H 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               E P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle PZID changes.

EXTERNALIZED FUNCTIONS
  DS707_EPZID_RECONNECT_CB
    Called by MC/CM when the system PZID changes.  Sends a message to DS
    pkt mgr telling it to come out of dormancy.
    
  DS707_EPZID_DISABLE_RECON
    Disables dormant re-origination when PZID changes.
    
  DS707_EPZID_REG_RECON
    Registers the PZID reconnect callback with MC/CM.
    
  DS707_EPZID_DEREG_RECON
    De-registers the PZID reconnect callback with MC/CM.
  
  DS707_PZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    PZID reconnect parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Need to call ds707_pzid_reg_recon() at startup, if want to be notified
    of PZID changes.

   Copyright (c) 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.h_v   1.1   18 Nov 2002 21:33:56   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_epzid.h#2 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   ms      Created the Initial Revision


===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_EPZID
#include "snm.h"
#include "ds707_pkt_mgr.h"

/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define DS707_EPZID_MAX_LEN    16            /* Maximum size of array       */
/* Default Extended PZID contains just the PZID  */
#define DEFAULT_ACTIVE_EPZID_TYPE    NV_EPZID_PZID

extern byte ds707_epzid_oldest_epzid;         /* Oldest PZID index +1        */
extern byte ds707_epzid_max_list_len;        /* Maximum PZID list           */
extern ds707_epzid_type ds707_epzid_list[DS707_EPZID_MAX_LEN];  /* PZID list */
extern byte ds707_active_epzid_type;  
                             /* Type of the currently active Enhanced PZID */

/*===========================================================================
                                TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  Type for the service option control message for Packet Zone ID 
  Reconnection Control.
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  byte  cntl;                            /* Packet Zone Connection Control */
  byte  list_len;                        /* Length of visited PZID list    */
} ds707_pzid_socm_msg_type;

/*===========================================================================
                                 EXTERNS
===========================================================================*/

/*===========================================================================
FUNCTION       DS707_EPZID_CLEAR_EPZID_LIST

DESCRIPTION    This function clears the EPZID list from the index specified
               by the first parameter to the index specified by the second
               parameter.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_clear_epzid_list
(
 byte first_pzid,                /* First list element  to be cleared      */
 byte last_pzid                  /* Last list element  to be cleared       */
);

/*===========================================================================
FUNCTION      DS707_EPZID_RECONNECT_CB

DESCRIPTION   Sends a message to DS asking it to originate a call.  Occurs
              when EPZID change requires reconnection.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_reconnect_cb(void);

/*===========================================================================
FUNCTION      DS707_EPZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the epzidi_active_change_cb()
              and the pzidi_idle_change_cb(). The list of visited
              packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable EPZID reconnection
             (b) BS sends a EPZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_disable_recon(void);

/*===========================================================================
FUNCTION      DS707_EPZID_REG_RECON

DESCRIPTION   This function registers EPZID based reconnection for packet
              calls.  It registers the active and idle change callbacks and
              initializes the EPZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a EPZID change.
          
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_reg_recon(void);

/*===========================================================================
FUNCTION      DS707_PZID_DEREG_RECON

DESCRIPTION   This function deregisters EPZID based reconnection for packet
              calls.  It sets the active and idle EPZID change handlers to
              NULL and resets the visited EPZID list.
   
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is ended (packet
              service goes into Inactive state) - not called when packet data
              service goes dormant.
   
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_dereg_recon(void);

/*===========================================================================
FUNCTION      DS707_EPZID_CHECK_FOR_NEW_EPZID

DESCRIPTION   This function calls the pzid change handler, passing the 
              pzid contained in element zero of the array, and the new
              pzid as told by MC.  May cause an origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_check_for_new_epzid(void);

/*===========================================================================
FUNCTION      DS707_PZID_COMPARE_PZIDS

DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_compare_pzids
(
  ds707_epzid_type    *old_pzid,
  ds707_epzid_type    *new_pzid
);

/*===========================================================================
FUNCTION      DS707_EPZID_PROCESS_DELAY_TIMER_EXPIRED

DESCRIPTION   This function calls the epzid change handler, passing the 
              epzid contained in element zero of the array, and the new
              epzid as told by MC.  May cause an origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_process_delay_timer_expired(void);

/*===========================================================================
FUNCTION      DS707_EPZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_epzid_parse_socm
(
  byte                       cntl,            /* SOCM CNTL field           */
  ds707_pzid_socm_msg_type   *pzid_socm_msg   /* SOCM with PZID info       */
);

/*===========================================================================
FUNCTION      DS707_PZID_RLP_DATA_RCVD_CB

DESCRIPTION   This callback function should be called when a (new) RLP packet
              is sent/received after dormancy. It deregisters itself with
              RLP and notifies the EPZID hysterisis engine of the data
              transfer.

DEPENDENCIES  This callback should be registered with RLP when the data 
              session goes dormant and should be called by RLP when the first 
              PPP packet is sent received after dormancy.

              This function is only applicable for 1X RLP3.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_rlp_data_transfer_cb( byte rlp_instance );

/*===========================================================================
FUNCTION      DS707_EPZID_CHANGED_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new EPZID was 0, it resets EPZID based reconnection.
              It adds the new EPZID to the visited EPZID list.
              If the new EPZID is not on the list of the EPZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_changed_cb
(
  ds707_epzid_type *old_pzid,                                   /* Previous Pkt Zone ID */
  ds707_epzid_type *new_pzid                                    /* New Pkt Zone ID      */
);
#endif /* FEATURE_DATA_EPZID */
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_EPZID_H */

