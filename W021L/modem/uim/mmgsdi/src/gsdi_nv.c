/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                                    A N D

                     N O N V O L A T I L E   M E M O R Y

                              I N T E R F A C E


GENERAL DESCRIPTION

  This file contains the header information for the GSDI to NV Interface.  This
  interface will be exercised by the GSDI Task and Virtual SIM Task.

EXTERNALIZED FUNCTIONS

  gsdi_get_nv()  - Used get items from nv

  gsdi_put_nv()  - Used put items into nv

  gsdi_get_nv_slot_preferences() - Used to populate global gsdi_data slot
                           preference information.  If not active,
                   GSDI will write a default of None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires that global variable gsdi_nvi is declared.


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2005,2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/09   sun     Added support for Virtuim
03/08/06   jar     Merged in Branched SIM Lock
05/11/05   sst     Lint fixes
11/15/04   jar     Clear out GSDI NV Signal when NV Response is received
02/21/04   jar     Removed gsdi_wait_nv() since it is not being used and
                   causes compiler errors.
11/05/03   jar     Added support for Slot Preference Changes.  Switch from
                   gsdi_wait_nv() to gsdi_wait() to allow for GSDI_RPT_SIG
                   handling.
07/02/03   tml     Fixed gsdi_put_nv
12/05/02   jar     Added #include "err.h" for ERR Macros.
11/26/02   jar     Removed RTRE Code.
08/30/02 jar/ck    Added gsdi_send_rtre_command.  changed gsdi_nvi to gsdi_nv
                   _cmd.  Localized gsdi_wait_nv.  Added internal feature
                   MMGSDI_RTRE.
08/18/02   jar     Created gsdi_nv.h
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_nv.h"
#include "nv.h"
#include "msg.h"
#include "err.h"

static nv_cmd_type gsdi_nv_cmd;           /* nv command buffer */

/*===========================================================================
FUNCTION GSDI_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type gsdi_get_nv(
  nv_items_enum_type item,
  nv_item_type *data_ptr
)
{
  MSG_MED(" -- In GSDI_GET_NV",0,0,0);

  /*
  ** notify this task when done
  */
  gsdi_nv_cmd.tcb_ptr = rex_self();
  gsdi_nv_cmd.sigs = GSDI_NV_SIG;

  /*
  ** command goes on no queue when done
  */
  gsdi_nv_cmd.done_q_ptr = NULL;

  /*
  ** item to read
  */
  gsdi_nv_cmd.item = item;
  gsdi_nv_cmd.cmd = NV_READ_F;

  /*
  ** Set the Data Ptr for NV ITEM TO read
  */
   gsdi_nv_cmd.data_ptr = data_ptr;

  /*
  ** Clear REX Signals
  */
  (void) rex_clr_sigs( rex_self(), GSDI_NV_SIG );

  /*
  ** Send Command to NV
  */
  #ifdef FEATURE_MMGSDI_LTK 
  
  /* 
  ** Non Context Switching Function Call...therfore, not signal will ever 
  ** get set to get us out of the wait...which is why the wait is also 
  ** in the #else below
  */
  mmgsdi_ltk_nv_cmd(&gsdi_nv_cmd); 

  #else 
#ifndef TEST_FRAMEWORK

  nv_cmd( &gsdi_nv_cmd );

  /*
  ** Wait for Signal from NV that it's complete
  */
  (void)gsdi_wait(GSDI_NV_SIG);
#endif /*TEST_FRAMEWORK*/

  /*
  ** Clear Signal
  */
  (void) rex_clr_sigs( rex_self(), GSDI_NV_SIG );

  MSG_MED(" -- NV STATUS = %x",gsdi_nv_cmd.status,0,0);

  #endif /* FEATURE_MMGSDI_LTK */

  if ( gsdi_nv_cmd.status != NV_DONE_S )
  {
    MMGSDI_DEBUG_ERR(" NV GET FAILED ",0,0,0);
  }
  return ( gsdi_nv_cmd.status );
}

/*===========================================================================
FUNCTION GSDI_PUT_NV

DESCRIPTION
  Put an item on to the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type gsdi_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{

  MSG_MED(" -- In GSDI_PUT_NV",0,0,0);

  /*
  ** notify this task when done
  */
  gsdi_nv_cmd.tcb_ptr = rex_self();
  gsdi_nv_cmd.sigs    = GSDI_NV_SIG;

  /*
  ** command goes on no queue when done
  */
  gsdi_nv_cmd.done_q_ptr = NULL;

  /*
  ** item to read
  */
  gsdi_nv_cmd.item = item;
  gsdi_nv_cmd.cmd  = NV_WRITE_F;
  gsdi_nv_cmd.data_ptr = data_ptr;

  /*
  ** Clear REX Signals
  */
  (void) rex_clr_sigs( rex_self(), GSDI_NV_SIG );

  #ifndef TEST_FRAMEWORK
  /*
  ** Send Command to NV
  */
  nv_cmd( &gsdi_nv_cmd );

  /*
  ** Wait for Signal from NV that it's complete
  */
  (void)gsdi_wait(GSDI_NV_SIG);
#endif /*TEST_FRAMEWORK*/
  
  /*
  ** Clear Signal
  */
  (void) rex_clr_sigs( rex_self(), GSDI_NV_SIG );

  MSG_MED(" -- NV STATUS = %x",gsdi_nv_cmd.status,0,0);

  if( gsdi_nv_cmd.status != NV_DONE_S ) {
      MMGSDI_DEBUG_ERR( "NV Write Failed Item %d Code %d",
                        gsdi_nv_cmd.item,
                        gsdi_nv_cmd.status,
                        0 );
  }

  return ( gsdi_nv_cmd.status );
}
#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_GET_NV_SLOT_PREFERENCES

DESCRIPTION
  Function is used with Dual Slot Builds.  It is used to determine what the
  user preferences are for which Subscription they would like and from what
  slot.

  If the NV Item is not active, it will be set to the equivalent of
  NO USER PREFERENCE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets: gsdi_data.user_slot_prefs.gsm_app_pref
        gsdi_data.user_slot_prefs.cdma_app_pref
  To:
        GSDI_SLOT_1, GSDI_SLOT_2, or GSDI_SLOT_NONE.
===========================================================================*/
gsdi_returns_T gsdi_get_nv_slot_preferences( void )
{
  nv_item_type        gsdi_nv_data_buf; /* NV data buffer */
  nv_stat_enum_type   nv_status;        /* NV read status */

  /* ---------------------------------------------------------------------
     GSDI CDMA APP/Subscription Slot Preference.
     --------------------------------------------------------------------- */
  nv_status = gsdi_get_nv( NV_UIM_CDMA_PREF_SLOT_I,
                          (nv_item_type *) &gsdi_nv_data_buf );

  switch (nv_status)
  {
  case NV_DONE_S:
    /* Information was successfully obtained */
    switch ( gsdi_nv_data_buf.uim_cdma_pref_slot )
    {
    case NV_UIM_SLOT_1:
      gsdi_data.user_slot_prefs.cdma_app_pref = GSDI_SLOT_1;
      break;

    case NV_UIM_SLOT_2:
      gsdi_data.user_slot_prefs.cdma_app_pref = GSDI_SLOT_2;
      break;

    case NV_UIM_SLOT_NONE:
      gsdi_data.user_slot_prefs.cdma_app_pref = GSDI_SLOT_NONE;
      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT FROM NV: %x",
                             gsdi_nv_data_buf.uim_cdma_pref_slot,
                             0,0);
      return GSDI_ERROR;
    }
    break;

  case NV_NOTACTIVE_S:
    /* Set the NV ITEM TO NONE */
    gsdi_nv_data_buf.uim_cdma_pref_slot = NV_UIM_SLOT_NONE;
    nv_status = gsdi_put_nv(NV_UIM_CDMA_PREF_SLOT_I,&gsdi_nv_data_buf);

    if ( nv_status != NV_DONE_S )
    {
      MMGSDI_DEBUG_MSG_ERROR("NV set to GSDI_SLOT_NONE for CDMA FAIL: %x",
                             nv_status,0,0);
      gsdi_data.user_slot_prefs.cdma_app_pref = GSDI_SLOT_AUTOMATIC;
      return GSDI_ERROR;
    }
    gsdi_data.user_slot_prefs.cdma_app_pref = GSDI_SLOT_NONE;
    break;

  default:
    MMGSDI_DEBUG_MSG_ERROR("Unhandled NV Status %x", nv_status,0,0);
    return GSDI_ERROR;
  }

  /* ---------------------------------------------------------------------
     GSDI GSM APP/Subscription Slot Preference.
     --------------------------------------------------------------------- */
  nv_status = gsdi_get_nv( NV_UIM_GSM_PREF_SLOT_I,
                          (nv_item_type *) &gsdi_nv_data_buf );

  switch (nv_status)
  {
  case NV_DONE_S:
    /* Information was successfully obtained */
    switch ( gsdi_nv_data_buf.uim_gsm_pref_slot )
    {
    case NV_UIM_SLOT_1:
      gsdi_data.user_slot_prefs.gsm_app_pref = GSDI_SLOT_1;
      break;

    case NV_UIM_SLOT_2:
      gsdi_data.user_slot_prefs.gsm_app_pref = GSDI_SLOT_2;
      break;

    case NV_UIM_SLOT_NONE:
      gsdi_data.user_slot_prefs.gsm_app_pref = GSDI_SLOT_NONE;
      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT FROM NV: %x",
                             gsdi_nv_data_buf.uim_gsm_pref_slot,
                             0,0);
      return GSDI_ERROR;
    }
    break;

  case NV_NOTACTIVE_S:
    /* Set the NV ITEM TO NONE */
    gsdi_nv_data_buf.uim_gsm_pref_slot = NV_UIM_SLOT_NONE;
    nv_status = gsdi_put_nv(NV_UIM_GSM_PREF_SLOT_I,&gsdi_nv_data_buf);

    if ( nv_status != NV_DONE_S )
    {
      MMGSDI_DEBUG_MSG_ERROR("NV set to GSDI_SLOT_NONE for GSM FAIL: %x",
                             nv_status,0,0);
      gsdi_data.user_slot_prefs.gsm_app_pref = GSDI_SLOT_AUTOMATIC;
      return GSDI_ERROR;
    }
    gsdi_data.user_slot_prefs.gsm_app_pref = GSDI_SLOT_NONE;
    break;

  default:
    MMGSDI_DEBUG_MSG_ERROR("Unhandled NV Status %x", nv_status,0,0);
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
