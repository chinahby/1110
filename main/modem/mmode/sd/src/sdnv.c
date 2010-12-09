/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                       N V   I N T E R F A C E

GENERAL DESCRIPTION
  This file makes up the NV-Interface component of the SD.

  The SD NV-Interface component encapsulates all the functionality that is
  associated with reading and writing from/to NV SD related items, such as
  the PRL and MRU table.

EXTERNALIZED FUNCTIONS
  sdnv_init
  sdnv_write
  sdnv_read

REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  sdnv_init() must be call to initialize this component before any other
  function declared in this component is called.


Copyright (c) 2000 - 2009 by Qualcomm Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdnv.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
03/16/09   mj      Add default HDR channel from NV to scan list
08/25/08   sv      Memory reduction changes
08/12/08   fj      Modified sdnv_nv_cmd() to support new API prl_commit2()
08/25/07   jqi/cl  Removed the cell/pcs band mapping features.
01/04/07   jqi     Lint fixes for barred API -sprintf.
08/15/06   pk      UOOS for GW modifications, added support to read GCF flag
                   from NV
05/04/06   jqi     Lint fixes.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/09/06   sk      Added support for custom homes bitmap in NV.
12/02/05   jqi     Lint8 fixes.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/05/05   sk      Added support for configuring SD values from NV.
02/24/05   pk      Fixed lint errors.
01/30/05   sj      Added support for WLAN.
01/07/05   jqi     Lint fixes.
12/04/04   jqi     Do not read PRL and SSPR_P_REV from NV if neither 1x nor
                   gw subscription is available.
11/02/04   jqi     Fixed lint warnings.
11/01/04   jqi     More read subscription related changes.
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
04/19/04   jqi     Lint fix - for error level : error.
07/15/03   JQ      Added Extended PRL support
05/27/03   JQ      Featurize NV dependent code for IS683B PRL support.
05/06/03   JQ      Added IS683B PRL support.
03/24/03   RC      Changed some SD_ERR() to SD_MSG_ERR().
01/25/02   sj      Added support to read PUZL for User Zone feature.
09/07/01   RC      Added support for T53 feature.
02/26/01   RC      Changes to support dual-mode targets.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "sdnv.h"     /* External interface to sdnv.c */
#include "sd.h"       /* External interface to sd.c */
#include "sddbg.h"    /* SD debug macros */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */

#include "nv.h"        /* Interface to NV services */
#include "rex.h"       /* Interface to rex services */

#ifdef FEATURE_EFS
#include "fs_public.h"  /* Interface to EFS services */
#endif

#include "prl.h"
#include "sdss.h"


#if ( defined(FEATURE_USERZONE) && defined(FEATURE_EFS) && (TG != T_PC) )
#include <stdio.h>    /* For calling sprintf.*/
#endif

#ifdef FEATURE_NV_RPC_SUPPORT
#include "AEEstd.h"  /* For string operation header files*/
#endif

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Type for holding the information that is associated with the SD
** NV-Interface component.
*/
typedef struct {

  /* Rex signal to be used by SD for waiting on NV to completes a read/write
  ** operation.
  */
  rex_sigs_type             nv_sig;


  /* Rex signal to be used by SD for watchdog reporting while SD waits on NV
  ** to complete a read/write operation.
  */
  rex_sigs_type             dog_sig;


  /* Pointer to a watchdog reporting function to be called by SD when the
  ** watchdog sig is set while SD waits on NV to complete a read/write
  ** operation.
  */
  void                      (*dog_rpt_func_ptr) (void);

} sdnv_s_type;


/* SD-Interface item.
*/
static sdnv_s_type  sdnv;






#if defined(FEATURE_WLAN) && (TG!=T_PC)
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_init

DESCRIPTION
  Initialize the SD NV-Interface component.

  This function must be called before the NV interface block
  is being used in any way, place or form.

DEPENDENCIES
  This function must be called before any other sdnv_xxx() function is
  ever called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
MMEXTN  void                      sdnv_init(

        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void)
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */
)
{
  sdnv_s_type         *sdnv_ptr = &sdnv;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( nv_sig != 0 );
  SD_ASSERT( dog_sig != 0 );
  SD_ASSERT( dog_rpt_func_ptr != NULL );

  SD_FUNC_START( "sdnv_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SD NV-Interface sigs and callback function.
  */
  sdnv_ptr->nv_sig              = nv_sig;
  sdnv_ptr->dog_sig             = dog_sig;
  sdnv_ptr->dog_rpt_func_ptr    = dog_rpt_func_ptr;

} /* sdnv_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_wait

DESCRIPTION
  Suspend the SS-Client from executing until NV completes the read/write
  operation.

  Note that this function keep reporting to the watchdog while waiting
  for the NV signal to be set.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static  void                      sdnv_wait( void )
{

  sdnv_s_type       *sdnv_ptr   = &sdnv;
  rex_sigs_type     sigs;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for the NV signal to be set.
  */
  do
  {
    /* Wait on the NV and watchdog signals.
    */
    sigs = rex_wait( (rex_sigs_type) (sdnv_ptr->nv_sig | sdnv_ptr->dog_sig));

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the watchdog signal is set, call on the watchdog reporting function
    ** that was provided by the SS-Client.
    */
    if( sigs & sdnv_ptr->dog_sig )
    {
      sdnv_ptr->dog_rpt_func_ptr();
    }

  } while( ! (sigs & sdnv_ptr->nv_sig) );

} /* sdnv_wait */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_nv_cmd

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static  void                      sdnv_nv_cmd(

        nv_cmd_type               *nv_cmd_ptr
           /* pointer to the NV command block */
)
{
  sdnv_s_type                     *sdnv_ptr   = &sdnv;
  sdss_subsc_e_type               subsc;
                                      /* Subscription type in the SS-core */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( nv_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get subscription status from SS-core.
  */
  subsc = sdss_get_subsc_type( SD_SS_MAIN );



  switch ( nv_cmd_ptr->cmd )  /*lint -e788 */
  {

    case NV_READ_F:

      /* If the subscription does not contains 1x subscription
      ** skip the following NV items.Or both 1x and GW subscription are
      ** unavailable, skip reading PRL.
      ** Otherwise send command to nv command queue as usual.
      */
      if( ( ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_1X ) &&
            ( nv_cmd_ptr->item == NV_HOME_SID_NID_I      ||
              nv_cmd_ptr->item == NV_SID_NID_I           ||
              nv_cmd_ptr->item == NV_ANALOG_HOME_SID_I   ||
              nv_cmd_ptr->item == NV_PCDMACH_I           ||
              nv_cmd_ptr->item == NV_SCDMACH_I           ||
              nv_cmd_ptr->item == NV_JTACS_PCDMACH_I     ||
              nv_cmd_ptr->item == NV_JTACS_SCDMACH_I))
#ifndef CUST_EDITION
                         ||
          ( ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_1X ) &&
            ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_GW ) &&
            ( nv_cmd_ptr->item == NV_SSPR_P_REV_I        ||
              nv_cmd_ptr->item == NV_ROAMING_LIST_683_I )))
#else
       )
#endif
      {

        /*
        ** Set status other than NV_NOTACTIVE_S or NV_FAIL_S
        ** to subtitue a default value and do not report an error in
        ** sdnv_read function.
        */

        nv_cmd_ptr->status = NV_NOTALLOC_S;

        return;
      }
      break;

    default:
      break;
  }/* switch */ /*lint +e788 */

  /* If coming here, do regular nv operation.*/
  nv_cmd_ptr->tcb_ptr = rex_self();

  #ifdef FEATURE_NV_RPC_SUPPORT
  if (0 == std_strncmp ((nv_cmd_ptr->tcb_ptr)->task_name,"ONCR", 4))
  {

    (void)nv_cmd_remote(nv_cmd_ptr->cmd,
                  nv_cmd_ptr->item,
                  nv_cmd_ptr->data_ptr
                  );

  }
  else
  #endif
  {
    nv_cmd( nv_cmd_ptr );
  }

  sdnv_wait();
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );

} /* sdnv_nv_cmd */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_write

DESCRIPTION
  Write a SD related item to NV memory.

  Note that the NV write is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
MMEXTN  boolean                      sdnv_write(

        nv_items_enum_type        nv_item,
            /* NV item to write.
            */

        nv_item_type              *data_ptr
            /* Pointer to the data that is associated with the NV item.
            */
)
{
  sdnv_s_type         *sdnv_ptr   = &sdnv;
  nv_cmd_type         sdnv_cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare the NV write command buffer.
  */
  sdnv_cmd.item       = nv_item;          /* item to write */
  sdnv_cmd.cmd        = NV_WRITE_F;       /* NV operation */
  sdnv_cmd.data_ptr   = data_ptr;         /* data to write */
  sdnv_cmd.tcb_ptr    = rex_self();       /* Notify this task when done */
  sdnv_cmd.sigs       = sdnv_ptr->nv_sig; /* Signal with this sig when done */
  sdnv_cmd.done_q_ptr = NULL;             /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the write command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );
  sdnv_nv_cmd( &sdnv_cmd );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV write failed, generate an error.
  */
  if( sdnv_cmd.status != NV_DONE_S )
  {
    SD_MSG_ERROR( "NV write failed, item=%d, stat=%d",
                  sdnv_cmd.item, sdnv_cmd.status, 0 );

    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* sdnv_write */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_read

DESCRIPTION
  Read a SD related item to NV memory.

  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the read operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if NV read operation is successful (or alternatively the NV item is
  set to a default value). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sdnv_read(

        nv_items_enum_type        nv_item,
            /* NV item to read.
            */

        nv_item_type              *data_ptr
            /* Pointer to a data buffer where to store the data that is
            ** associated with the NV item.
            */
)
{
  sdnv_s_type                    *sdnv_ptr   = &sdnv;
  nv_cmd_type                    sdnv_cmd;
  unsigned int                   i;
  sd_mru_table_entry_u_type      *sd_mru_entry_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare the NV read command buffer.
  */
  sdnv_cmd.item       = nv_item;          /* item to read */
  sdnv_cmd.cmd        = NV_READ_F;        /* NV operation */
  sdnv_cmd.data_ptr   = data_ptr;         /* buffer to store read data  */
  sdnv_cmd.tcb_ptr    = rex_self();       /* Notify this task when done */
  sdnv_cmd.sigs       = sdnv_ptr->nv_sig; /* Signal with this sig when done */
  sdnv_cmd.done_q_ptr = NULL;             /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the read command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the read command.
  */
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );
  sdnv_nv_cmd( &sdnv_cmd );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV the read operation is successful, return TRUE.
  */
  if( sdnv_cmd.status == NV_DONE_S )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the NV read operation has failed. If appropriate,
  ** substitute a default value for the NV item. Else return FALSE.
  */
  SD_MSG_ERROR( "NV read failed, item=%d, stat=%d",
                sdnv_cmd.item, sdnv_cmd.status, 0 );

  switch( sdnv_cmd.item ) /*lint -e788 */
  {
    case NV_HOME_SID_NID_I:
      data_ptr->home_sid_nid.pair[0].sid = SD_WILDCARD_SID;
      data_ptr->home_sid_nid.pair[0].nid = SD_WILDCARD_NID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SID_NID_I:
      data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].sid = SD_WILDCARD_SID;
      data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].nid = SD_WILDCARD_NID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_ANALOG_HOME_SID_I:
      data_ptr->analog_home_sid.sid = SD_WILDCARD_SID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_MRU2_TABLE_I:

      for( i=0; i < ARR_SIZE(data_ptr->mru2_table.entry); i++ )
      {
        data_ptr->mru2_table.entry[i].mode = NV_MODE_INACTIVE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_MRU3_TABLE_I:

      for( i=0; i < ARR_SIZE(data_ptr->mru3_table.entry); i++ )
      {
        sd_mru_entry_ptr = (sd_mru_table_entry_u_type *)
                           &data_ptr->mru3_table.entry[i];
        sd_mru_entry_ptr->cdma.mode = (byte)SD_MODE_INACT;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_PCDMACH_I:
        data_ptr->pcdmach.channel_a = 283;
        data_ptr->pcdmach.channel_b = 384;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SCDMACH_I:
        data_ptr->scdmach.channel_a = 691;
        data_ptr->scdmach.channel_b = 777;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_PRL_EXTENDED
    case NV_JTACS_PCDMACH_I:
        data_ptr->jtacs_pcdmach.channel_a = 76;
        data_ptr->jtacs_pcdmach.channel_b = 76;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_JTACS_SCDMACH_I:
        data_ptr->jtacs_scdmach.channel_a = 872;
        data_ptr->jtacs_scdmach.channel_b = 872;
      break;
  #endif
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_ROAMING_LIST_683_I:
    case NV_PUZL_ENABLED_I:
    case NV_HDR_IS683A_SCAN_LIST_I:

      /* Default value substitution is not appropriate for these items -
      ** return FALSE.
      */
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_PRL_ENABLED_I:
      data_ptr->prl_enabled.enabled = TRUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SRDA_ENABLED_I:

      /* If FEATURE_MC_SILENT_REDIAL_AMPS is defined, default the SRDA to
      ** enabled. Else default the SRDA to disabled.
      */
      #ifdef FEATURE_MC_SILENT_REDIAL_AMPS
        data_ptr->srda_enabled = TRUE;
      #else
        data_ptr->srda_enabled = FALSE;
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_AMPS_BACKSTOP_ENABLED_I:

      /* For dual-band targets default AMPS backstop to enabled. For all
      ** other targets default AMPS backstop to disabled.
      */
      #ifdef SD_TARGET_DUALMODE
        data_ptr->amps_backstop_enabled.enabled = TRUE;
      #else
        data_ptr->amps_backstop_enabled.enabled = FALSE;
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SID_NID_LOCK_I:
      for( i=0; i < ARR_SIZE(data_ptr->sid_nid_lock.pair); i++ )
      {
        data_ptr->sid_nid_lock.pair[i].sid = SD_WILDCARD_SID;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SSPR_P_REV_I:
      data_ptr->sspr_p_rev = (byte) PRL_SSPR_P_REV_1;
      break;

    case NV_SD_CFG_ITEMS_I:
      /* Default value substitution taken care of in sdssscr_nv_items_init().
      */
      return FALSE;

    case NV_ROAM_IND_CUSTOM_HOME_I:
      /* Default value substitution taken care of in sdprl_custom_home_read().
      */
      return FALSE;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_GPRS_ANITE_GCF_I:
      data_ptr->gprs_anite_gcf = FALSE;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      SD_CASE_ERR( "Unexpected NV item=%d",sdnv_cmd.item,0,0 );
      return FALSE;

  } /* switch */ /*lint +e788 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, a default value was substituted for the NV item. If the
  ** NV read operation failed because of NV_NONACTIVE_S or NV_FAIL_S, attempt
  ** writing the default value back to NV.
  */
  if( sdnv_cmd.status == NV_NOTACTIVE_S ||
      sdnv_cmd.status == NV_FAIL_S )
  {
    SD_MSG_HIGH( "Write default value to NV, item=%d",sdnv_cmd.item,0,0 );
    (void)sdnv_write( sdnv_cmd.item, sdnv_cmd.data_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the NV item was successfully read (or alternatively the
  ** item was set to a default value) - return TRUE.
  */
  return TRUE;

} /* sdnv_read */



/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_wlan_mru_read

DESCRIPTION

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if NV read operation is successful.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sdnv_wlan_mru_read(
       /*lint -save -esym(818, data_ptr, size_ptr) \
                    -esym(715, data_ptr, size_ptr)
       ** Pointer parameter 'size_ptr' could be declared as pointing to const
       */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the WLAN MRU.
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
        /*lint -esym(715, data_ptr, size_ptr ) */ /* Symbol not used*/
)
{
  #ifdef FEATURE_WLAN
#error code not present
  #else
  return FALSE;
  #endif

} /* sdnv_wlan_mru_read() */
/*lint -restore */


/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_wlan_mru_write

DESCRIPTION

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if NV read operation is successful.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sdnv_wlan_mru_write(

        byte                     *data_ptr,
            /* Pointer to a data buffer to write to WLAN MRU.
            */
        int32                    size
            /* Size of the data_ptr buffer.
            */
)
/*lint -esym(715, data_ptr, size ) */ /* Symbol not used*/
{
  #ifdef FEATURE_WLAN
#error code not present
  #else
  return FALSE;
  #endif

} /* sdnv_wlan_mru_read() */
/*lint +esym(715, data_ptr, size ) */ /* Symbol not used*/


#ifdef FEATURE_USERZONE
/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_puzl_read

DESCRIPTION
  Read the PUZL table from EFS file system. If T_PC is the target, then call
  sdnv_puzl_io_cmd()

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if read operation is successful.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                  sdnv_puzl_read(

        byte                     nam,
            /* NAM for which PUZL is to be read
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the PUZL
            */
        int32                   *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
)
{
  #ifdef FEATURE_EFS
  #if (TG==T_PC)
#error code not present
  #else
  int   fd;
  static char sdnv_puzl_file_name[30];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( (sd_nam_e_type) nam, SD_NAM_MAX );
  SD_ASSERT( data_ptr != NULL );
  SD_ASSERT( size_ptr != NULL );
  SD_ASSERT( *size_ptr <= NV_PUZL_MAX_SIZE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Open the specified resource file.
  ** If open fails, return error code
  */
  /*lint -save -e64 */
  /*Type mismatch (arg. no. 4) (char * = unsigned char) */
  (void) vsnprintf( sdnv_puzl_file_name, 
                    ARR_SIZE(sdnv_puzl_file_name), "nvm/puzl/puzl_%d", nam );
  /*lint -restore */
  MSG_LOW( "Open file=%s",sdnv_puzl_file_name,0,0 );
  fd= efs_open( sdnv_puzl_file_name,
                O_RDONLY
              );

  if( fd < 0 )
  {
    SD_MSG_HIGH( "open efs_err=%d ", fd, 0, 0 );
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read data from file.
  */
  *size_ptr = efs_read( fd, data_ptr, (fs_size_t)*size_ptr );


  if( *size_ptr < 0 )
  {
    SD_MSG_HIGH( "efs read error %d", *size_ptr, 0, 0 );
    *size_ptr = 0;
    (void) efs_close( fd );
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Return the number of bytes actually read.
  */
  SD_MSG_LOW( "size_read=%d", *size_ptr, 0, 0 );


  /* Close the file */
  (void) efs_close( fd );
  return TRUE;
  #endif /* T_PC */
  #else
  return FALSE;
  #endif /* FEATURE_EFS */
}

#if ( TG==T_PC)
#error code not present
#endif /* (TG==T_PC) */
#endif /* FEATURE_USERZONE */


#if ( TG==T_PC)
#error code not present
#endif /* (TG==T_PC) */
