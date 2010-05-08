/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                     D E B U G      M O D U L E

GENERAL DESCRIPTION
   This module contains the debug related functionality of MMoC.

EXTERNALIZED FUNCTIONS
  mmocdbg_print_message ()
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before DEBUG APIs are called.


Copyright (c) 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/08   sv      Correcting Featurization for Memory Reduction
09/19/08   jd      Memory reduction changes
04/28/08   fj      Lint cleanup, CR 144389
03/12/08   aj      Edit header to reflect p4 path and revision
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
08/02/06   ka      Adding additional entry in prot_state string map. 
04/03/06   ka      Changing copyright.
12/20/05   ka      Initial revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmocdbg.h" /* Includes mmoc debug related info */

/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG STRING MAPS =================================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMOC_LOW_MEM_TARGET

/* Provide strings for  values mmoc_cmd_e_type */
const char* mmocdbg_cmd_e_type_string_map [ MMOC_CMD_MAX ] = {
  "MMOC_CMD_SUBSCRIPTION_CHGD",
  "MMOC_CMD_PROT_GEN_CMD",
  "MMOC_CMD_OPRT_MODE_CHGD",
  "MMOC_CMD_WAKEUP_FROM_PWR_SAVE"
};


  /* Provide strings for  values mmoc_rpt_e_type */
const char* mmocdbg_rpt_e_type_string_map [ MMOC_RPT_MAX ] = {
  "MMOC_RPT_PROT_DEACTD_CNF",
  "MMOC_RPT_PROT_AUTO_DEACTD_IND",
  "MMOC_RPT_PH_STAT_CHGD_CNF",
  "MMOC_RPT_PROT_GEN_CMD_CNF",
  "MMOC_RPT_TMC_MODE_CNF"
};


    /* Provide strings for  values mmoc_trans_state_e_type */
const char* mmocdbg_trans_state_e_type_string_map [ MMOC_TRANS_STATE_MAX ] = {
  "MMOC_TRANS_STATE_NULL",
  "MMOC_TRANS_STATE_PROT_DEACT_ENTER",
  "MMOC_TRANS_STATE_WAIT_DEACTD_CNF",
  "MMOC_TRANS_STATE_PROT_PH_STAT_ENTER",
  "MMOC_TRANS_STATE_WAIT_PH_STAT_CNF",
  "MMOC_TRANS_STATE_GEN_CMD_ENTER",
  "MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF",
  "MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND",
  "MMOC_TRANS_STATE_HDR_DEACT_ENTER",
  "MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF",
  "MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF"
};

    /* Provide strings for  values mmoc_trans_e_type */
const char* mmocdbg_trans_e_type_string_map [ MMOC_TRANS_MAX ] = {
  "MMOC_TRANS_NULL",
  "MMOC_TRANS_SUBSC_CHGD",
  "MMOC_TRANS_ONLINE",
  "MMOC_TRANS_OFFLINE",
  "MMOC_TRANS_PROT_GEN_CMD",
  "MMOC_TRANS_PWR_DOWN",
  "MMOC_TRANS_PWR_SAVE_ENTER",
  "MMOC_TRANS_PWR_SAVE_EXIT",
  "MMOC_TRANS_PROT_AUTO_DEACTD_IND",
  "MMOC_TRANS_WAKEUP_FROM_PWR_SAVE"  
};

/* Provide strings for  values prot_state_e_type */
const char* mmocdbg_prot_state_e_type_string_map [ PROT_STATE_MAX ] = {
  "PROT_STATE_NULL",
  "PROT_STATE_ONLINE_CDMA",
  "PROT_STATE_OFFLINE_CDMA",
  "PROT_STATE_ONLINE_AMPS",
  "PROT_STATE_OFFLINE_AMPS",
  "PROT_STATE_ONLINE_GW",
  "PROT_STATE_ONLINE_HDR",
  "PROT_STATE_ONLINE_WLAN",
  "PROT_STATE_OFFLINE",
  "PROT_STATE_FTM",
  "PROT_STATE_PWR_SAVE",
  "PROT_STATE_ONLINE_DED_MEAS",
  "PROT_STATE_RESET",
  "PROT_STATE_LPM",
  "PROT_STATE_PWR_DOWN",
  "PROT_STATE_GPSONE_MSBASED",
};



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG RELATED FUNCTIONS ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION MMOCDBG_PRINT_MESSAGE

DESCRIPTION

 Behaves as printf() - takes the very same format string and any number of
 parameters and allows to print <stuff> to diagnostic monitor output
 screen.

 The problem is that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmocdbg_print_message
(
     
     const char *string,
       /* Format in which string needs to be printed */
     ...
)
{
  AEEVaList   arg_ptr;
  char        msg_str[ MMOC_MAX_STRING_LENGTH];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the string buffer 
  */
  memset (msg_str, 0, sizeof(msg_str));

  /*lint -save -e64 -e718 -e746 -e63 */
  AEEVA_START( arg_ptr, string );
  /*lint -restore */

  /*  Writing max of MMOC_MAX_STRING_LENGTH-1 characters only, 
  **  so the end null character stays untouched.
  */  
  (void)std_vstrlprintf (msg_str, MMOC_MAX_STRING_LENGTH - 1, 
                                                        string, arg_ptr);
  AEEVA_END( arg_ptr );

  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LVL_HIGH, "=MMOC= %s", msg_str);

} /* mmocdbg_print_message() */

#endif /* FEATURE_MMOC_LOW_MEM_TARGET */

