#ifndef MCCSCM_H
#define MCCSCM_H
/*********************************************************************
*
*    Supplemental Channel Processing Routines
*
* GENERAL DESCRIPTION
*   This module contains functions to process SCAM messages
* received from a base station.  It also includes functions to
* process the data pattern testing information.  The SCAM processing is
* currently only for version 0.1 of MDR, and the DPR information is
* usable for versions 0.1 or 0.2 of MDR, and possibly also 1.0.
*
* Copyright (c) 1998-2005 by QUALCOMM Incorporated.  All Rights Reserved.
*
*
*                       EDIT HISTORY FOR MODULE
*   This section contains comments describing changes made to the module.
*   Notice that changes are listed in reverse chronological order.
*
*
* $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccscm.h_v   1.1   01 Oct 2002 16:51:40   azafer  $
* $Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccscm.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
*
*
* when       who     what, where, why
* --------   ---     -------------------------------------------------------
* 10/10/05   fh      Removed including of dec.h. changed dec_rate_set_type to 
                     cai_rate_set_type.
* 10/20/04   an      Replaced inclusion of srch.h with srchmc.h
* 08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
* 10/01/02   az      Mainlined FEATURE_IS2000.
* 10/20/99   lcc     Added MSM5000/IS2000 MDR support.
* 07/26/99   jq      Redefined the timer types to a enum structure
* 07/22/99   jq      Added shared data type for SCAM-GHDM linking
* 03/30/99   ram     Updated the header information for the function
                     prototypes to match those in the c file. Externalised
                     mccscm_cancel_prev_dur_timer since it's used by other
                     modules.
* 03/15/99   ram     Updated scam_timer_msg to be of caii_scam_type rather
                     than mccrxtx_msg_type since it's used only for SCAM.
                     fixed incorrect comment about SCAM timer.
* 03/09/99   ram     Externalized SCAM/GHDM duration related functions.
* 02/19/99   ram     Externalized IS95B SCAM processing function and related
                     data. Removed unused data structures, lint cleanup
                     and added more comments to the function prototypes.
* 02/11/99   ram     Added mdr_pre_process to pre-process the MDR related
                     fields of SCAM and GHDM. Created scam_process function
                     to process SCAM message. Featurized code under
                     IS95B_MDR and SPECIAL_MDR.
* 01/19/99   lcc     Moved FEATURE_IS95B_MDR below #includes for header files.
                     Now it builds correctly with and without FEATURE_IS95B_MDR.
* 01/19/99   ram     Added FEATURE_IS95B_MDR defines so that the file
*                    builds correctly when FEATURE_IS95B is turned off.
* 12/23/98   lcc     Corrected definition of mdr_fwd_dpt_frames to take care
                     of RS2 frames also.
* 12/22/98   nmn     improved output
* 12/16/98   nmn     surrounded all DPT code with #defines
* 12/11/98   nmn     Linted for 0.2
* 12/01/98   ram     Added prototypes for MDR DPT related functions
* 11/23/98   nmn     DPT works sucessfully with two channels
* 11/19/98   nmn     add DPT processing using byte length data
* 10/15/98   nmn     added separate functions to perform different parts
*                    of SCAM processing
* 07/13/98   nmn     linted file
* 07/10/98   nmn     removed variables in favor of those in cai.h
* 07/09/98   nmn     created variables, changed spacing
* 07/08/98   nmn     Initial document
*
*
*
********************************************************************/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cai.h"
#include "caii.h"
#include "comdef.h"
#include "mccrxtx.h"

#if defined (FEATURE_IS95B_MDR) 
#include "srchmc.h"


/*********************************************************************
*
*               DEFINITIONS AND DECLARATIONS FOR MODULE
*
********************************************************************/


/*********************************************************************
*
*                       DATA DEFINITIONS
*
********************************************************************/

#define MAX_WALSH_VALUE 0x3f

#define CAI_SCAM_STOP_ONLY 0x0
#define CAI_SCAM_START_ONLY 0x1
#define CAI_SCAM_STOP_CHANGE 0x2
#define CAI_SCAM_START_CHANGE 0x3

#define MCCSCM_FIRST_HALF_WORD 0xff00
#define MCCSCM_SECOND_HALF_WORD 0x00ff
#define MCCSCM_RATE_ONE_DATA_BYTES ((171 + 7)/ 8)
#define MCCSCM_RATE_TWO_DATA_BYTES ((266 + 7)/ 8)
#define MCCSCM_BYTES_NOT_COMPARED 3

#define MCCSCM_CHANNEL_ONE 1
#define MCCSCM_CHANNEL_TWO 2
#define MCCSCM_CHANNEL_THREE 3
#define MCCSCM_CHANNEL_FOUR 4
#define MCCSCM_CHANNEL_FIVE 5
#define MCCSCM_CHANNEL_SIX 6
#define MCCSCM_CHANNEL_SEVEN 7

/* A default error value for for_sup_config. for_sup_config
can only be 0x00, 0x01, 0x10, or 0x11 */
#define SUP_CONFIG_ERR 0xAA

/* An ennumerated type of return values for mdr_pre_process and
mdr_process functions */
typedef enum
{
  MDR_DONE_S = 0,                 /* The processing was done correctly */
  MDR_REJ_CAP_S = CAI_REJ_CAP,    /* The caller should reject the msg
                                  because the capability requested is
                                  not supported by the mobile          */
  MDR_REJ_FIELD_S = CAI_REJ_FIELD,/* The caller should reject the msg
                                  because atleast one of the fields was
                                  invalid                              */
  MDR_REJ_STRUCT_S = CAI_REJ_STRUCT, /* The caller should reject the msg
                                  because the msg structure is incorrect*/
  MDR_REJ_STATE_S = CAI_REJ_STATE,/* the caller should reject the msg
                                  because the mobile's current state does
                                  not allow processing this message    */
  MDR_UNKNOWN_S = 0xFE,           /* The msg is unknown (neither GHDM
                                  nor SCAM)                            */
  MDR_FAIL_S = 0xFF               /* The required processing failed 
                                  to happen correctly                  */
                                  /* The last two values are unsigned byte
                                  values assigned so that they don't
                                  conflict with other reject reasons   */
}mccscm_stat_enum_type;

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_IS95B_MDR
extern caii_scam_type scam_timer_msg; 
/* To maintain a copy of scam message,in case the message has a start time
 * associated with it */

/* this structure contains the info used in linking the SCAM and GHDM */
typedef struct
{
  boolean use_for_hdm_seq; // indication of whether SCAM is linked to GHDM
  byte for_linked_hdm_seq; // the seq of GHDM which SCAM is linked to
  byte scam_for_order; // least sign't bit of for_sup_config, 1=start, 0=stop
  boolean scam_for_dur_mode; // use forward duration indicator
  byte for_dur; // forward duration
} mccscm_link_type;

extern mccscm_link_type mccscm_link_data;
/* the data storage of the linking info. that can be updated by both GHDM and
 * SCAM code */

/* these define the start timer types of SCAM */
typedef enum
{
MCCSCM_NULL_TM,
MCCSCM_EXPLICIT_TM,
MCCSCM_IMPLICIT_TM,
MCCSCM_NONVALID_TM
} mccscm_timer_type;

#endif /* FEATURE_IS95B_MDR */

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV
 
#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

/*===========================================================================

FUNCTION MCCSCM_MDR_PRE_PROCESS

DESCRIPTION
  This function pre-processes an MDR supplemental channel message
  (either SCAM or GHDM) and makes sure all the relevant fields are
  within expected range. If not, the message may have to be rejected
  and if so, this function will return the reject code to the calling 
  function.  It also checks if an MDR service option connection
  has been established and the negotiated mux options allow the number
  of supplementals assigned by the message.

DEPENDENCIES                            
  FEATURE_IS95B_MDR must be turned on.
  The function should be called before the GHDM action time
  processing and before the SCAM start/stop time processing.

RETURN VALUE
  A value of type mccscm_stat_enum_type to indicate sucess or reason
  of failiure. The reason of failiure may be a reject reason. If so,
  the message has to be rejected.

SIDE EFFECTS
  None.

===========================================================================*/

extern mccscm_stat_enum_type mccscm_mdr_pre_process
(
  caii_rx_msg_type *msg_ptr /* Pointer to the GHDM or SCAM message */
);

#ifdef FEATURE_IS95B_MDR
/*===========================================================================

FUNCTION MCCSCM_SCAM_SUP_PROCESS

DESCRIPTION
  This function processes the supplemental channel info given by SCAM
  message and takes the necessary actions.
  The actions include sending a command to SRCH task to start or stop
  supplemental channels and also updating the local code channel
  list information maintained by CP task if necessary. Duration timer
  is also started if necessary.

  Only Forward traffic channel supplementals are supported.
  
DEPENDENCIES                            
  FEATURE_IS95B_MDR must be turned on, and a MDR call must be in
  progress. The function should be called after the 
  after the SCAM start time processing.

RETURN VALUE
  A value of type mccscm_stat_enum_type indicating success or
  failiure.

SIDE EFFECTS
  None.

===========================================================================*/

extern mccscm_stat_enum_type mccscm_scam_sup_process
(
  caii_scam_type *msg_ptr /* Pointer to the SCAM message */
);

/*===========================================================================

FUNCTION MCCSCM_SCAM_PROCESS

DESCRIPTION
  This function processes a SCAM message. This includes pre-processing
  and checking for errors, starting a start timer if necessary, or
  processing the supplemental channel info if there is no explicit
  start time provided.
  
  If there is a reason to reject the SCAM message, this
  function will send the reject order with the appropriate reject
  reason.
  
  Only Forward traffic channel supplementals are supported.
  
DEPENDENCIES                            
  FEATURE_IS95B_MDR must be turned on, and a MDR call must be in
  progress. The function should be called before the 
  after the SCAM start/stop time processing, since the pre-processing
  happens before the start time of the message. However, if the
  start time is implicit, we can process the message right away, so
  the function will call the supplemental processing function.

RETURN VALUE
  A value of type mccscm_stat_enum_type indicating success or
  failiure. 

SIDE EFFECTS
  None.

===========================================================================*/
extern mccscm_stat_enum_type mccscm_scam_process
(
  caii_rx_msg_type *msg_ptr /* Pointer to the SCAM message */
);

#endif /* FEATURE_IS95B_MDR */

/*===========================================================================

FUNCTION MCCSCM_DURATION_TIMER_CHK

DESCRIPTION
  This function checks the SCAM or GHDM message to see if a duration 
  timer is needed. If so, it starts a timer for the required time. If
  the start timer is already on, it cancels the start timer and
  starts a new start timer based on the given message.

  The start time from the message is the actual system time at 
  which the SCAM processing must start. This function uses the
  current system time and the system time given by the message to
  find out the actual time in milliseconds to wait before processing
  the rest of the message.

DEPENDENCIES                            
  A MDR call must be in progress.

RETURN VALUE
  MDR_DONE_S indicating that the timer is set as needed. No
  other return values are needed here.

SIDE EFFECTS
  None.

===========================================================================*/
extern mccscm_stat_enum_type mccscm_duration_timer_chk
(
  byte duration,         /* An unsigned 8 bit value which represents
                         the duration time in 80ms units */

  boolean implicit_start /* Indication of whether the message
                         had an implicit action time or start time */
);

/*===========================================================================

FUNCTION MCCSCM_PROCESS_STOP_REQ

DESCRIPTION
  This function process a request to stop processing supplementals.
  The function needs to be invoked when the duration timer expires
  so that the mobile can stop processing supplementals at that time.
  This function may also be invoked at the end of an MDR call if the
  supplementals are active.

  This function sends a SRCH command to stop processing supplementals.
  It will not affect the code channel list maintenance, ie. all the
  supplemental channel walsh codes in the current actice set will be retained.

DEPENDENCIES                            
  None

RETURN VALUE
  None

SIDE EFFECTS
  None. Note: This function does not modify the code channel list.
===========================================================================*/
extern void mccscm_process_stop_req( void );

/*===========================================================================

FUNCTION MCCSCM_CANCEL_PREV_DUR_TIMER

DESCRIPTION
  This function cancels a previously set duration timer and
  the corrosponding signal for a SCAM or GHDM processing message.
  This function needs to be called to clear a duration timer
  set by a previous SCAM or a GHDM message whenever a new SCAM
  or GHDM superseeds the old message's duration time. 
  
DEPENDENCIES                            
  FEATURE_IS95B_MDR must be turned on,  
  and a MDR call must be in progress. This function assumes that
  the calling function is either a SCAM or GHDM processing function
  for MDR forward supplementals.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void mccscm_cancel_prev_dur_timer( void );

/*===========================================================================

FUNCTION MCCSCM_SET_DEC_SUP

DESCRIPTION
  This function sends a command to RXC which will program the deinterleaver
  and decoder to decode the number of supplementals requested.  This function
  must be called right after sending the command to searcher to update
  supplementals.  Since the information is already contained in the searcher
  command and this function is always called after, this command will just
  take the srch command as input parameter.

DEPENDENCIES                            
  mcc_srch_buf must contain the just executed SRCH command (this is the case
  since SRCH does not corrupt the command packet).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mccscm_set_dec_sup
(
  srch_cmd_type *srch_cmd
);

#endif /* defined (FEATURE_IS95B_MDR) */ 

#endif /* MCCSCM_H */

