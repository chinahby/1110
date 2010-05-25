#ifndef DS707_SCRM_H
#define DS707_SCRM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               D S 7 0 7 _ S C R M   E X T E R N A L   F I L E 

GENERAL DESCRIPTION
  This file contains functions that are used to implement the SCRM processing
  available in IS2000.
  
  SCRM's can happen during SO33 data calls.  SCRM's usually happen while a
  call is active, but can also be used to come out of control/hold state.
  
  This module assumes that RLP 3 calls ds_scrm_sch_chng_cback() every 20
  msec.
  
EXTERNALIZED FUNCTIONS
  DS707_SCRM_REG_FUNCS
    Registers various callbacks with MC.
    
  DS707_SCRM_CALL_INIT
    Called when an SO 33 call connects.  Resets various SCRM state
    variables.
  
  DS707_SCRM_RLP_CBACK
    Called by RLP every 20-msec.  Tells SCRM if RLP has data to send.  If
    the conditions are correct, SCRM module will ask MC to send a request
    to base station for R-SCH (i.e., ask MC to send SCRM).
    
  DS707_SCRM_ESCAM_RXED
    Called by MC when an ESCAM for an R-SCH is received.  
  
  DS707_SCRM_EXTEND_BURST
    Called by MC when there are x frames or less before the current R-SCH
    burst ends.  Indicates to SCRM module that it may want to SCRM in order
    to "extend" the current R-SCH burst.
  
  DS707_SCRM_MC_CBACK
    Called by MC whenever the F-SCH or R-SCH config changes.  This allows
    mobile to figure out if it has enough CPU with current config, and then
    to SCRM accordingly.
  
  DS707_SCRM_MAX_ALLOWABLE_SCH_RATE
    Called by MUX when the F-SCH and R-SCH config changes.  Tells MUX how
    many PDU's it can build without phone CPU running out.
  
  DS707_SCRM_BUILD_BLOB
    Called by ds707_rrm.c.  This function builds the SCRM BLOB, which tells
    the BS what the requested R-SCH will be.
  
  DS707_SCRM_RETRY_PROCESSING
    Called when the SCRM retry delay expires.  Resets SCRM retry delay params
  
  DS707_SCRM_RETRY_DELAY
    Called when MS receives a retry delay (for SCRM) from BS.  Sets the
    SCRM retry delay timer to the value assigned by BS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  At startup, must call ds707_scrm_reg_funcs().
  
  Before every call, must call ds707_scrm_call_init().

Copyright (c) 2000 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2001 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_scrm.h_v   1.1   19 Nov 2002 16:01:48   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_scrm.h#1 $ $DateTime: 2008/04/11 08:25:15 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/18/02   ak         Updated file header.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_IS2000_R_SCH
#include "mccsch.h"
#include "mccdma.h"

/*===========================================================================
                         TYPEDEFS & VARIABLES
===========================================================================*/
/*-------------------------------------------------------------------------
  Frames before a SCRM to extend a burst.
 -------------------------------------------------------------------------*/
#define DS707_SCRM_EXTENSION_TIME 20


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SCRM_REG_FUNCS

DESCRIPTION   SCRM Registers various callbacks with MC. Called only at 
              startup.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_reg_funcs(void);

/*===========================================================================
FUNCTION      DS707_SCRM_CALL_INIT

DESCRIPTION   Called when a new data call is started.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_call_init(void);

/*===========================================================================
FUNCTION      DS707_SCRM_RLP_CBACK

DESCRIPTION   Called by RLP every 20 msec.  See if vars allow us to send a
              SCRM.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_rlp_cback
(
  uint32     bytes_to_send                       /*bytes RLP needs to xmit */
);

/*===========================================================================
FUNCTION      DS707_SCRM_ESCAM_RXED

DESCRIPTION   Called by MC when an ESCAM with an R-SCH assignment is rx'ed.
              This ESCAM indicates our last SCRM was answered, and so the
              appropriate state vars should be updated.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_escam_rxed(byte rev_sch_duration);

/*===========================================================================
FUNCTION      DS707_SCRM_EXTEND_BURST

DESCRIPTION   Called by MC when there are x frames or less before end of
              this burst assignment.  Data should SCRM regardless, in order
              to extend this burst.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_extend_burst(void);

/*===========================================================================
FUNCTION      DS707_SCRM_MC_CBACK

DESCRIPTION   Called by MC when the SCH config changes.

              Does two things: 1) calls txc so that it can do it's R-SCH
              throttling calculations.  2) Calculates R-SCH and F-SCH
              parameters for future calculations.
             
              Do not need to see if a SCRM needs to be sent in this function.
              RLP 3 will call ds_scrm_sch_chng_cback() every 20 msec, and 
              that will trigger a SCRM if necessary.
             
DEPENDENCIES  None.

RETURN VALUE  NONE
             
SIDE EFFECTS  None.
===========================================================================*/
void ds707_scrm_mc_cback
(
  mccsch_sch_info_type* sch_info_ptr
);

/*===========================================================================
FUNCTION     DS707_SCRM_MAX_ALLOWABLE_SCH_RATE

DESCRIPTION  Look at the table scrm_sch_rate_matrix[] for the max allowable
             rate the MSM can do in the Rev. SCH. 
             
             Do not use current_f_sch0, etc.  That's because mux updates
             itself asynchronously.  Use the values MUX returns, by
             using the mux information.

DEPENDENCIES None.

RETURN VALUE Max number of MuxPDUs.

SIDE EFFECTS None.
===========================================================================*/
byte ds707_scrm_max_allowable_sch_rate( void );

/*===========================================================================
FUNCTION      DS707_SCRM_BUILD_BLOB

DESCRIPTION   This builds the SCRM blob to be sent to the Base Station.  This
              is called from the RRM module.

DEPENDENCIES  None

RETURN VALUE  FALSE = MC should send nothing to BS.  TRUE means a BLOB
              should be sent.  In the TRUE case, a blob_size of zero
              means MC should send a SCRM that cancels the assigned R-SCH
              configuration.
  
SIDE EFFECTS  None
===========================================================================*/
boolean ds707_scrm_build_blob
(
  uint8 *blob_size,   /* eventual size of the blob                         */
  uint8 *blob_ptr,    /* points to BLOB that is to be filled out           */
  word   so           /* SCRM is for this active service option            */
);

/*===========================================================================
FUNCTION     DS707_SCRM_RETRY_PROCESSING

DESCRIPTION  Called to clear SCRM timer.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_scrm_retry_processing(void);

/*===========================================================================
FUNCTION      DS707_SCRM_RETRY_DELAY

DESCRIPTION   Called when the SCRM retry delay is changed from the BS.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_retry_delay
(
  retry_delay_type retry_parm
);

#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_DATA_IS707   */
#endif /* DS707_SCRM_H         */

