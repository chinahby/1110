#ifndef DS707_PZID_H
#define DS707_PZID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle PZID changes.

EXTERNALIZED FUNCTIONS
  DS707_PZID_RECONNECT_CB
    Called by MC/CM when the system PZID changes.  Sends a message to DS
    pkt mgr telling it to come out of dormancy.
    
  DS707_PZID_DISABLE_RECON
    Disables dormant re-origination when PZID changes.
    
  DS707_PZID_REG_RECON
    Registers the PZID reconnect callback with MC/CM.
    
  DS707_PZID_DEREG_RECON
    De-registers the PZID reconnect callback with MC/CM.
  
  DS707_PZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    PZID reconnect parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Need to call ds707_pzid_reg_recon() at startup, if want to be notified
    of PZID changes.

   Copyright (c) 2002 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.h_v   1.1   18 Nov 2002 21:33:56   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_pzid.h#2 $ $DateTime: 2009/05/30 06:04:02 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
09/08/04   vr      Added function prototype ds707_pzid_rlp_data_transfer_cb
10/14/03   sy      Removing unused current pzid information.
10/01/03   ak      Added extern for PZID delay timer processing function.
09/13/03   ak      Added code to check for a new PZID when holddown timer
                   expires.
09/02/03   sy      Added support for pzid hysteresis under 
                   FEATURE_DS_PZID_HYSTERESIS.
11/17/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifndef FEATURE_DATA_EPZID
#include "snm.h"

/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define DS707_PZID_MAX_LEN    16            /* Maximum size of array       */
extern byte ds707_pzid_oldest_pzid;         /* Oldest PZID index +1        */
extern byte ds707_pzid_max_list_len;        /* Maximum PZID list           */
extern byte ds707_pzid_list[DS707_PZID_MAX_LEN];
                                            /* PZID list                   */

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
FUNCTION       DS707_PZID_CLEAR_PZID_LIST

DESCRIPTION    This function clears the PZID list from the index specified
               by the first parameter to the index specified by the second
               parameter.
  
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pzid_clear_pzid_list
(
 byte first_pzid,                /* First list element  to be cleared      */
 byte last_pzid                  /* Last list element  to be cleared       */
);

/*===========================================================================
FUNCTION      DS707_PZID_RECONNECT_CB

DESCRIPTION   Sends a message to DS asking it to originate a call.  Occurs
              when PZID change requires reconnection.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_reconnect_cb(void);

/*===========================================================================
FUNCTION      DS707_PZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the pzidi_active_change_cb()
              and the pzidi_idle_change_cb(). The list of visited
              packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable PZID reconnection
             (b) BS sends a PZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_disable_recon(void);

/*===========================================================================
FUNCTION      DS707_PZID_REG_RECON

DESCRIPTION   This function registers PZID based reconnection for packet
              calls.  It registers the active and idle change callbacks and
              initializes the PZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a PZID change.
          
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_reg_recon(void);

/*===========================================================================
FUNCTION      DS707_PZID_DEREG_RECON

DESCRIPTION   This function deregisters PZID based reconnection for packet
              calls.  It sets the active and idle PZID change handlers to
              NULL and resets the visited PZID list.
   
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
FUNCTION      DS707_PZID_CHECK_FOR_NEW_PZID

DESCRIPTION   This function calls the pzid change handler, passing the 
              pzid contained in element zero of the array, and the new
              pzid as told by MC.  May cause an origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_check_for_new_pzid(void);

/*===========================================================================
FUNCTION      DS707_PZID_COMPARE_PZIDS

DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_compare_pzids
(
  byte    old_pzid,
  byte    new_pzid
);

/*===========================================================================
FUNCTION      DS707_PZID_PROCESS_DELAY_TIMER_EXPIRED

DESCRIPTION   This function calls the pzid change handler, passing the 
              pzid contained in element zero of the array, and the new
              pzid as told by MC.  May cause an origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_pzid_process_delay_timer_expired(void);

/*===========================================================================
FUNCTION      DS707_PZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a PZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_pzid_parse_socm
(
  byte                       cntl,            /* SOCM CNTL field           */
  ds707_pzid_socm_msg_type   *pzid_socm_msg   /* SOCM with PZID info       */
);

/*===========================================================================
FUNCTION      DS707_PZID_RLP_DATA_RCVD_CB

DESCRIPTION   This callback function should be called when a (new) RLP packet
              is sent/received after dormancy. It deregisters itself with
              RLP and notifies the PZID hysterisis engine of the data
              transfer.

DEPENDENCIES  This callback should be registered with RLP when the data 
              session goes dormant and should be called by RLP when the first 
              PPP packet is sent received after dormancy.

              This function is only applicable for 1X RLP3.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_rlp_data_transfer_cb( byte rlp_instance );

/*===========================================================================
FUNCTION      DS707_PZID_CHANGE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new PZID was 0, it resets PZID based reconnection.
              It adds the new PZID to the visited PZID list.
              If the new PZID is not on the list of the PZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pzid_change_cb
(
  byte old_pzid,                                   /* Previous Pkt Zone ID */
  byte new_pzid                                    /* New Pkt Zone ID      */
);
#endif /* FEATURE_DATA_EPZID */
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_PZID_H */

