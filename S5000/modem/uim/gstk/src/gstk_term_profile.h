#ifndef gstk_term_profile_h
#define gstk_term_profile_h
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


         G S T K   T E R M I N A L   P R O F I L E   H E A D E R


GENERAL DESCRIPTION
  This header file contains function definitions for gstk_term_profile.c


FUNCTIONS
  gstk_uim_terminal_profile_report
    This is the callback function for terminal profile response.

  gstk_cache_tp_info
    This function caches the terminal profile data received from GSTK clients

  gstk_pack_tp_cmd_to_uim
    This function puts the gstk_terminal_profile_cache info into uim format
    and put onto UIM's command queue
  
  gstk_toolkit_mode_change
    Inform clients that there is a mode change
  
  gstk_process_mode_change
    This function will perform UIM reset and toolkit change based on new CM
	mode if required
					   
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_term_profile.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id 
                   when parse client TP and simplify TP download procedure
10/22/08   sk      Changes parameter prototype for gstk_cache_tp_info_ext
02/06/08   sk      Lint fixes
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
10/01/04   tml     Change function prototype for terminal profile caching
09/04/03   tml     dual slot support
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstki.h"
#include "gstk_exp.h"
#include "uim.h"


/*===========================================================================
FUNCTION gstk_uim_terminal_profile_report

DESCRIPTION

  This is the callback function for terminal profile response.  UIM will call
  this function when it receives terminal profile response from the Card
  Based on the return status of the terminal profile response, this function
  will build the gstk_term_profile_rsp_type and set GSTK_CMD_Q_SIG so that
  this command can be processed in the gstk state machine

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_uim_terminal_profile_report (uim_rpt_type *report);

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_uim_terminal_profile_reset_report

DESCRIPTION

  This is the callback function for terminal profile with reset response.  
  UIM will upon finishing the terminal profile with reset

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_uim_terminal_profile_reset_report (uim_rpt_type *report);
#endif /*FEATURE_GSTK_DUAL_SLOTS */

/*===========================================================================
FUNCTION gstk_update_tp_dl_cache

DESCRIPTION
  This function updates the terminal profile cache if the client ui
  client differentiates between sat and usat tp dl

PARAMETERS
  term_profile: [Input] Mobile Terminal Profile info from GSTK client

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_update_tp_dl_cache(
    gstk_terminal_profile_cmd_type  *term_profile_ptr );

/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim

DESCRIPTION

  This function puts the gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_pack_tp_cmd_to_uim ( void );

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_toolkit_mode_change

DESCRIPTION
  This function notifies all gstk clients regarding a mode change.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_toolkit_mode_change(void);


/*===========================================================================
FUNCTION gstk_process_mode_change

DESCRIPTION
  This function processes a mode change indication from CM and sends  
  reset commands to UIM if needed.

PARAMETERS
  new_slot [Input] - slot to change the toolkit sessino to

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_process_mode_change(gstk_slot_id_enum_type new_slot);	  


#endif /* FEATURE_GSTK_DUAL_SLOTS */

/*===========================================================================
FUNCTION gstk_build_tp_dl_update_cmd

DESCRIPTION
  This function is currently called ONLY by GSTK to build the terminal 
  profile array to update the tp cache once the card type is known.

PARAMETERS
  term_profile: [Input] Mobile Terminal Profile info from GSTK client

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_build_tp_dl_update_cmd(
    gstk_terminal_profile_cmd_type **term_profile_ptr);

/*===========================================================================
FUNCTION gstk_cache_tp_info

DESCRIPTION
  This function caches the terminal profile data received from GSTK clients

PARAMETERS
  term_profile_ptr: [Input] Mobile Terminal Profile info from GSTK client

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_cache_tp_info(
    gstk_cmd_type  *term_profile_ptr );
#endif /*gstk_term_profile_h */

