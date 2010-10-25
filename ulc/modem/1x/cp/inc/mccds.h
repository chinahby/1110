#ifndef MCCDS_H
#define MCCDS_H

/*===========================================================================

            MAIN CONTROL DATA SERVICE SUPPORT FUNCTIONS

GENERAL DESCRIPTION
  This module contains utilities to handle requests from Data Service Task.

Copyright (c) 2001-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccds.h_v   1.1.2.0   30 Nov 2001 17:11:44   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccds.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/07   pg      Fixed featurization problem on FEATURE_IS2000_CHS and
                   FEATURE_IS2000_R_SCH.
05/09/06   TMR     Removed remoting of mccds_reg_chs_notifier()... now being
                   shimmed as callback is called in interrupt context.
02/01/06   pg      RPC related changes.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
06/26/01   sb      Merged the following from common archive:
		   06/03/01   ht      Added $Header line for revision control.
05/11/01   jq      Added mccds_reg_chs_notifier.
05/02/01   fc      Added support of T_ADD_ABORT.
04/10/01   jq      Moved all SCRM related data definitions and function
                   prototypes from mccsch.h to here.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "err.h"
#include "cai.h"
#include "caii.h"
#include "mccsrch.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

// Type for requesting channel resources
typedef enum
{
  MCCDS_RES_NONE,
  MCCDS_RES_SCRM,
  MCCDS_RES_RRM
} mccds_resource_req_type;

#ifdef FEATURE_IS2000_CHS
// Enum type for channel state
typedef enum
{
  MCCDS_CONTROL_HOLD_STATE,
  MCCDS_ACTIVE_STATE
} mccds_ch_state_type;
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_CHS
/* function pointer to hold the DS call back function to set the Control Hold State */
extern void (*mccds_notify_chs) (boolean chs_on);
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================

FUNCTION mccds_init_scrm

DESCRIPTION
  This function initializes SCRM related variables and functions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_init_scrm(void);

/*===========================================================================

FUNCTION mccds_clear_scrm_req_blob

DESCRIPTION
  This function clears SCRM BLOB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_clear_scrm_req_blob(void);

/*===========================================================================

FUNCTION mccds_update_t_mulchan

DESCRIPTION
  This function updates SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_update_t_mulchan
(
  uint8 t_mulchan
);

/*===========================================================================

FUNCTION mccds_get_t_mulchan

DESCRIPTION
  This function returns SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_t_mulchan(void);

/*===========================================================================

FUNCTION mccds_get_scrm_seq_num

DESCRIPTION
  This function returns the last used SCRM sequence number.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_scrm_seq_num(void);

/*===========================================================================

FUNCTION mccds_build_sup_chan_req_msg

DESCRIPTION
  This function builds a Supplemental Channel Request message from DS task's
  SCRM blob info and/or SRCH's task SCRM pilot strength measurement report.

DEPENDENCIES
  mccds_scrm.sz_req_blob and mccds_scrm.req_blob must already setup 
  properly.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_build_sup_chan_req_msg
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Supplemental Channel Request Message */
  mccsrch_rpt_type *srch_rpt_ptr,
    /* SRCH SCRM Pilot Strength Measurements or T_ADD_ABORT report */
  boolean use_scrm_seq_num
    /* Indicator whether to use SCRM sequence number */
);
#endif // FEATURE_IS2000_R_SCH

/*===========================================================================

FUNCTION mccds_reg_resource_generator

DESCRIPTION
  This function binds a SCRM blob generator to a function pointer. If an old
  SCRM blob generator already exists, it overwrites it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef mccds_resource_req_type (*mccds_resource_req_func_type) (uint8* sz_blob, 
                                                                 uint8* blob, 
                                                                 uint16 so);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM OUT sz_blob POINTER */
/*~ PARAM OUT blob VARRAY CAI_MAX_SCRM_BLOB_SIZE LENGTH CAI_MAX_SCRM_BLOB_SIZE */
#endif /* FEATURE_HTORPC_METACOMMENTS */

void mccds_reg_resource_generator
(
  mccds_resource_req_func_type ds_resource_generator
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccds_reg_resource_generator */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION mccds_process_resource_request

DESCRIPTION
  This function handles resource request from Data Service Task.

  SCRM request: It gets SCRM blob from DS task. If the sise of SCRM blob is not
  zero, it commands SRCH task to send SCRM pilot strength measurement report.
  Otherwise, it sends SCRM right away.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_process_resource_request(void);

#ifdef FEATURE_IS2000_CHS
/*===========================================================================

FUNCTION mccds_reg_chs_notifier

DESCRIPTION
  This function binds a DS function to modify the Control Hold State status
  inside DS to synchronize state changes in MC. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*mccds_chs_notifier_type) (boolean chs_on);

void mccds_reg_chs_notifier
(
  mccds_chs_notifier_type ds_chs_notifier
);
#endif /* FEATURE_IS2000_CHS */

#endif // MCCDS_H
