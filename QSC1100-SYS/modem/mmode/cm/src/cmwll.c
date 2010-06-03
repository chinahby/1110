/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   L O W E R   L A Y E R

                        I N T E R F A C E   M O D U L E

GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another.

EXTERNALIZED FUNCTIONS
  cm_put_cmd                     - Allows the lower layer to send a command
                                   to CM.
  cm_get_cmd_buf                 - Allocates a CM command buffer.
  cm_mn_get_buf_else_err_fatal   - Allocates a MN command buffer.
  cm_mn_send_cmd                 - Allows CM to send a command to MN.
  cm_reg_get_buf_else_err_fatal  - Allocates a REG command buffer.
  cm_reg_send_cmd                - Allows CM to send a command to REG.
  cm_sm_get_buf_else_err_fatal   - Allocates a SM command buffer.
  cm_sm_send_cmd                 - Allows CM to send a command to SM.
  cm_rabm_get_buf_else_err_fatal - Allocates a RABM command buffer.
  cm_rabm_send_cmd               - Allows CM to send a command to RABM.
  cm_tmc_get_buf_else_err_fatal  - Allocates a TMC command buffer.
  cm_tmc_send_cmd                - Allows CM to send a command to TMC.
  cm_tc_get_buf_else_err_fatal   - Allocates a TC command buffer.
  cm_tc_send_cmd                 - Allows CM to send a command to TC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmwll.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/08   rm      Mainlining of MBMS changes
08/07/07   ic      Lint clean up
02/18/07   rp      Removed cm_wms_bc_gw_cb_filter
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
05/25/05   ic      Removed FEATURE_CM_CMD_Q_DEBUG
12/03/04   ws      Support for Remote WMS
08/04/04   ic      CM Lint clean up - cmwll.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwll_dummy() to get rid of ADS 1.2 compiler warning
06/09/04   dk      Replaced mem_malloc with cm_mem_malloc to support CM Heap.
05/06/04   ic      Changed cm_mn_get_buf_else_err_fatal() to it's easier to
                   set up break points in the code.
04/13/04   ic      CM_ASSERT() clean up
02/06/04   ws      Initial jaguar/mainline merge.
04/28/03   AT      Lint cleanup.
01/24/03   vt      Fixed the Header path information.
01/21/03   ws      Updated copyright information for 2003
06/24/02   PRK     Modified debug messages in cm_reg_send_cmd.
06/03/02   PRK     Mainlined FEATURE_PLMN.
05/06/02   PRK     Added FEATURE_CM_USE_UMTS_REG_INTERFACE.
04/30/02   PRK     Modified cm_rabm_get_buf_else_err_fatal to set the
                   message_set field to MS_CM_RAB.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
01/04/02   PRK     Replaced FEATURE_PLMN with FEATURE_NEW_PLMN.  Added more
                   debug statements.
11/22/01   RI      Added message_set field in the SM commands.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added support for PLMN selection/reselection (FEATURE_PLMN).
                   Re-added Serving System group and moved srv_state, roam_status,
                   rssi from phone group to serving system group (FEATURE_NEWSS).
10/17/01   RI/HQ   Initial release
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwll_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmwll_dummy( void )
/*lint -esym(714,cmwll_dummy) */
/*lint -esym(765,cmwll_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */

