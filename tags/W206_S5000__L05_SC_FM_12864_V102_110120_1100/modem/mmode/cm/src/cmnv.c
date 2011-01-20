/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   N V   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager NV Interface block, which groups
  all the functionality that is a associated with CM<->NV interfacing.




EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmnv_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmnv.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   ks      Added support for disable_call_type to cmnv_data_s_type
03/26/09   ks      Removing temp target specific Feature FEATURE_MMCP_MDM_8200
01/26/09   fj      Added nv_cmd_remote() function call for NV read and
                   NV write operations.
02/02/09   ks      Adding support for QDSP 6 based compiler
09/15/08   np      Added HPCD Rev B.
09/05/08   sv      Added explicit cast to remove compiler warnings
06/12/08   sv      Resolve Klocwork defects
01/03/08   ka      Adding handset based plus code dialing feature.
12/12/07   rm      Adding RAT-balancing feature support
08/07/07   ic      Lint clean up
07/16/07   sk      Removed featurization to avoid garbage values in
                   bss_type_pref when ADHOC feature is not defined.
06/05/07   pk      Added support to read mob_cai_rev from NV
03/09/07   sk      RAM usage reduction.
08/28/08   ka      Increasing the queue size for buffered NV items. 
11/17/06   pk      Corrected featurization for Hybrid preferences
08/29/06   sk      Added support for savign WLAN Adhoc prefs.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
01/13/06   ic      Fixed compile errors in cmnv_ruim_report() 
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding check for NV_GPRS_ANITE_GCF_I in cmnv_read_wait()
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/03/05   pk      Added NV_DDTM_SETTINGS_I DDTM settings NV item
07/15/05   ic      Mainlined FEATURE_FACTORY_TESTMODE
06/06/05   pk      Modified cmnv_read_wait(), to read the access overload 
                   class information
03/25/05   dk      Increase the NV Q size to 8
02/09/10   dk      Expanded cm_band_pref to 64 bit.
11/05/04   dk      Added WLAN Support.
08/20/04   sj      Added support for PRL Preference.
08/11/04   ka      Adding support for AMR. NV_GSM_AMR_CALL_CONFIG_I
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/03/04   ic      CM Lint clean up - cmnv.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/02/04   sj      Fixed lint errors.
05/15/04   ka      Merged changes from 6250. Fixed Lint errors.
02/06/04   ws      Initial jaguar/mainline merge.
10/31/03   ws      Remove T_RF_ZRF6200 feature.
10/28/03   ws      Remove uim pref slot support.
10/03/03   ws      Support for nv ps data orig pref
10/03/03   ws      jar's changes for USIM.
09/16/03   vt      Added preferred slot handling.
08/19/03   ws      Merged in changes for dual slot
08/08/03   vt      Made cm band pref bitmask to 32 bit enum
07/24/03   vt      Initialized the protocol and slot fields in the ACCESS
                   command to UIM.
07/11/03   ck      Initialized the protocol and slot fields in the STATUS
                   command to UIM.
04/29/03   AT      Lint Cleanup.
04/14/03   RI      Changed CM_ERR to CM_MSG_HIGH in cmnv_read_wait().
03/11/03   vt      Set the default value of NV ftm mode item to DMSS for 1x.
02/28/03   prk     Removed FEATURE_FTM_PREF_NORMAL_MODE.
02/28/03   prk     Added code to support FEATURE_FTM_PREF_NORMAL_MODE.
02/27/03   vt      Set the default value of NV ftm mode item to FTM.
02/18/03   vt      Fixed problems to build for HDR.
02/14/03   jqi     Added support for runtime FTM.
                   Limited it to CDMA only.
01/28/03   jar     Added Virtual RUIM Support, Feature FEATURE_VIRTUAL_SIM.
01/21/03   ws      Updated copyright information for 2003
11/19/02   vt      Added changing RTRE configuration.
                   Removed handling of powerup pref mode nv item.
10/31/02   vt      Mainlined feature SD20.
10/05/02   vt      Added NV_FTM_TEST_I.
06/12/02   RC      Use different NV command buffers for synchronous and
                   asynchronous NV read/write operations.
05/29/02   PRK     Modified cmnv_read_wait to process error statuses for
                   NV_ACQ_ORDER_PREF_I, NV_NET_SEL_MODE_PREF_I and
                   NV_SERVICE_DOMAIN_PREF_I.
03/05/02   az      Changed the signal name for NV read and write
02/06/02   sh      Added hybrid 3.0 support
12/11/01   sh      Added preliminary hybrid operation NV item support
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/14/01   WLI     Added RUIM related functions.
03/14/01   RC      Fixed a problem where cmnv_q_write_done_cb was calling
                   cmnv_q_get twice instead of once.
02/07/01   RC      Added support for System Determination 2.0.
08/19/98   SH      Removed silent redial with apms feature flag
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmnv.h"      /* Interface to CM NV services */
#include "nv.h"        /* Interface to NV services */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "rex.h"       /* Interface to rex services */
#include <string.h>    /* Interface to string library */
#include "cmtask.h"

#if defined( FEATURE_CDMA_800 ) || defined( FEATURE_CDMA_1900 )
#include "cmxll.h"      /* MC to CM interface*/
#endif

#ifdef FEATURE_UIM_RUIM
#include "uim.h"       /* Interface to R-UIM services */
#endif

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_VIRTUAL_SIM
#include "virtuim.h"
#endif
#include "gsdi_exp.h"
#endif

#include "ftm.h"

#ifdef FEATURE_NV_RPC_SUPPORT
#include "AEEstd.h"  /* For string operation header files*/
#endif
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


static nv_cmd_type   cmnv_cmd_sync;
    /* NV command buffer to do NV synchronous NV write and read */

static nv_cmd_type   cmnv_cmd_async;
    /* NV command buffer to do NV asynchronous NV write and read */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/*===========================================================================
=============================================================================
=============================== NV WRITE QUEUE ==============================
=============================================================================
===========================================================================*/


/* Note that NV writes are done in asynchronous fashion; that is,
** initially write requests are only being queued in the CM NV queue.
** The CM NV queue then services the requests in the background.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for cmnv item. Such an item stores all the information that
** is needed to buffer an NV write operation.
*/
typedef struct {

    nv_items_enum_type    item;      /* NV item */
    cmnv_data_s_type      data;      /* Data associated with NV item */

} cmnv_item_s_type;



#define CM_NV_Q_SIZE  20
    /* Size of CM NV Q */



static CIRC_Q_DEF( cmnv_item_s_type, cmnv_q, CM_NV_Q_SIZE );
    /* Queue to buffer NV write requests */




/*===========================================================================

FUNCTION cmnv_q_init

DESCRIPTION
  Initialize the NV queue object.

  This function must be called before the NV queue object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void  cmnv_q_init( void )
{
  CIRC_Q_INIT( cmnv_q )
}




/*===========================================================================

FUNCTION cmnv_q_put

DESCRIPTION
  Enqueue a CM NV item onto the NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  TRUE if item is successfully enqueued.
  FALSE if queue is full and item cannot be enqueued.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmnv_q_put(

    const cmnv_item_s_type  *item_ptr
        /* pointer to CM NV item to be enqueued */
)
/*lint -save -e818*/
{
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return CIRC_Q_PUT( cmnv_q, *item_ptr );

}
/*lint -restore*/




/*===========================================================================

FUNCTION cmnv_q_get

DESCRIPTION
  Dequeue a CM NV from the NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  TRUE if item is dequeued successfully.
  FALSE if queue is empty and NO item is dequeued.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmnv_q_get(

    cmnv_item_s_type  *item_ptr
        /* pointer to CM NV item to receive dequeued item */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return CIRC_Q_GET( cmnv_q, *item_ptr );

}




/*===========================================================================

FUNCTION cmnv_q_check

DESCRIPTION
  Return a pointer to item at the head of the CM NV queue. Note that the
  item is not removed from the queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  A pointer to item at the head of the CM NV queue.
  If the queue is empty, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cmnv_item_s_type  *cmnv_q_check( void )
{
  return CIRC_Q_CHECK( cmnv_q );
}




/*===========================================================================

FUNCTION cmnv_q_cnt

DESCRIPTION
  Return the number of items currently queued on the CM NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  Return the number of items currently queued on the CM NV queue.

SIDE EFFECTS
  none

===========================================================================*/
static unsigned int cmnv_q_cnt( void )
{
  return CIRC_Q_CNT( cmnv_q );
}




/*===========================================================================

FUNCTION cmnv_q_srv

DESCRIPTION
  Services the cmnv_q by writing the item at the head of the queue to NV.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmnv_q_srv( void )
{

  /* Point at item at the head of the CM NV Q.
  */
  cmnv_item_s_type *item_ptr = cmnv_q_check();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Q is NOT empty, prepare the NV write command
  ** and send it to be serviced by the NV task.
  */
  if( item_ptr != NULL )
  {
    cmnv_cmd_async.item       = item_ptr->item;/* item to write */
    cmnv_cmd_async.cmd        = NV_WRITE_F;    /* NV operation */
    /*lint -save -e826 */
    cmnv_cmd_async.data_ptr   = (nv_item_type*) &item_ptr->data;/* data to write */
    /*lint -restore */
    cmnv_cmd_async.tcb_ptr    = rex_self();    /* Notify CM task when done */
    cmnv_cmd_async.sigs       = CM_NV_ASYNC_SIG;/* Signal with this sig when done */
    cmnv_cmd_async.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

    nv_cmd( &cmnv_cmd_async );                 /* Send command to NV task */
  }

}




/*===========================================================================

FUNCTION cmnv_q_write_done_cb

DESCRIPTION
  Callback function for completion of NV write operations

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  Dequeue item from the CM NV queue.

===========================================================================*/
void cmnv_q_write_done_cb( void )
{

  cmnv_item_s_type  nv_item;
      /* Container to hold dequeued NV item */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue should NOT be empty!.
  */
  //CM_ASSERT( cmnv_q_cnt() > 0 );

  /* Dequeue item from head of CM NV queue into the
  ** nv_item buffer. If queue is empty log an error and return.
  */
  if( ! cmnv_q_get(&nv_item) ) {
    CM_ERR( "cmnv_q is empty!",0,0,0 );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If status is other than NV_DONE_S,
  ** NV write failed!
  */
  if( cmnv_cmd_async.status != NV_DONE_S )
  {
    CM_ERR( "NV write failed, item=%d, stat=%d",
            nv_item.item, cmnv_cmd_async.status, 0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Service the next item (if any).
  */
  cmnv_q_srv();

}




/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmnv_init

DESCRIPTION
  Initialize the NV interface block.

  This function must be called before the NV interface block
  is being used in any way, place or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_init( void )
{

  /* Initializes cmnv_q.
  */
  cmnv_q_init();
}




/*===========================================================================

FUNCTION cmnv_write

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr
        /* pointer to data associated with the NV item */
)
{
  cmnv_item_s_type  cmnv_item;
      /* Container to hold NV item to be enqueued */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(data_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Prepare item for enqueuing.
  */
  cmnv_item.item    = nv_item;
  cmnv_item.data    = *((cmnv_data_s_type*) data_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try enqueuing item onto the CM NV write queue.
  */
  if( cmnv_q_put(&cmnv_item) )
  {
    CM_MSG_LOW( "cmnv_write(), nv_q_cnt=%d",cmnv_q_cnt(),0,0 );

    /* Enqueuing is successful - if queue was previously
    ** empty, start servicing the queue now.
    */
    if( cmnv_q_cnt() == 1 )
    {
      cmnv_q_srv();
    }
  }
  else
  {
    /* Queue is full!
    */
    CM_ERR( "NV write Q is full, nv_item=%d",cmnv_item.item,0,0 );
  }

}




/*===========================================================================

FUNCTION cmnv_write_wait

DESCRIPTION
  Write an item from the NV memory in a synchronous fashion; that is, this
  function only returns  after the NV write is done being serviced by the
  NV task.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to write items to NV.

DEPENDENCIES
  mc_task_start() must have already been called.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to items to NV.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_wait(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    nv_item_type          *data_ptr
        /* pointer to data associated with the NV item */
)
{

  /* Prepare the NV write command
  */
  cmnv_cmd_sync.item       = nv_item;       /* Item to write */
  cmnv_cmd_sync.cmd        = NV_WRITE_F;    /* NV operation */
  cmnv_cmd_sync.data_ptr   = data_ptr;      /* Read into this buffer */
  cmnv_cmd_sync.tcb_ptr    = rex_self();    /* Notify CM task when done */
  cmnv_cmd_sync.sigs       = CM_NV_SYNC_SIG;/* Signal with this sig when done */
  cmnv_cmd_sync.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write from NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a write command to the NV task
  ** 3. Waiting for NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  nv_cmd( &cmnv_cmd_sync );
  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV write failed, generate an error message.
  */
  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
    CM_ERR( "NV write failed, item=%d, stat=%d",
             cmnv_cmd_sync.item, cmnv_cmd_sync.status, 0 );
  }
}





/*===========================================================================

FUNCTION cmnv_read_wait

DESCRIPTION
  Read an item from the NV memory.

  Note that NV reads are done in a synchronous fashion; that is, this
  function only returns  after the NV read is done being serviced by the
  NV task.

DEPENDENCIES
  mc_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_read_wait(

    nv_items_enum_type  nv_item,
        /* NV item to read */

    nv_item_type        *data_ptr
        /* pointer to buffer to place data associated with the NV item */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(data_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Prepare the NV read command
  */
  cmnv_cmd_sync.item       = nv_item;       /* Item to read */
  cmnv_cmd_sync.cmd        = NV_READ_F;     /* NV operation */
  cmnv_cmd_sync.data_ptr   = data_ptr;      /* Read into this buffer */
  cmnv_cmd_sync.tcb_ptr    = rex_self();    /* Notify CM task when done */
  cmnv_cmd_sync.sigs       = CM_NV_SYNC_SIG;/* Signal with this sig when done */
  cmnv_cmd_sync.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a read command to the NV task
  ** 3. Waiting for NV task to finish servicing the read command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  cmnv_cmd_sync.tcb_ptr = rex_self();

  #ifdef FEATURE_NV_RPC_SUPPORT
  if (0 == std_strncmp ((cmnv_cmd_sync.tcb_ptr)->task_name,"ONCR", 4))
  {
    (void)nv_cmd_remote(cmnv_cmd_sync.cmd,
                        cmnv_cmd_sync.item,
                        cmnv_cmd_sync.data_ptr
                        );
  }
  else
  #endif
  {
    nv_cmd( &cmnv_cmd_sync );
  }

  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV read failed, substitute a default value.
  */
  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
    CM_MSG_HIGH( "NV read failed, item=%d, stat=%d",
             cmnv_cmd_sync.item, cmnv_cmd_sync.status, 0 );

    /*lint -save -e641 */
    switch( cmnv_cmd_sync.item )
    {
      case NV_PREF_MODE_I:

        /* If this target supports ACP, default the mode preference
        ** to automatic. Else, default mode preference to digital only.
        */
        #ifdef FEATURE_ACP
#error code not present
        #else
          data_ptr->pref_mode.mode = NV_MODE_DIGITAL_ONLY;
        #endif
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SYSTEM_PREF_I:
        data_ptr->system_pref.sys = NV_SP_STANDARD;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_I:
        data_ptr->band_pref.band = (nv_band_pref_enum_type)CM_BAND_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_16_31_I:
        data_ptr->band_pref.band = (nv_band_pref_enum_type)(CM_BAND_PREF_ANY >> 16) ;
                                               //(NV_BAND_PREF_ANY & 0xFFFF);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_32_63_I:
        data_ptr->band_pref_32_63.band = (uint32)(CM_BAND_PREF_ANY >> 32 );

        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ROAM_PREF_I:
        data_ptr->roam_pref.roam = NV_ROAM_PREF_ANY;
        break;

      #if (defined (FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
      #endif /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_CURR_NAM_I:
        data_ptr->curr_nam = (int)CM_NAM_1;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_FTM_MODE_I:
        #if defined( FEATURE_CDMA_800 ) || defined( FEATURE_CDMA_1900 )
        data_ptr->ftm_mode = DMSS_MODE;
        #else
        data_ptr->ftm_mode = AMSS_MODE;
        #endif /* defined( FEATURE_CDMA_800 ) || defined( FEATURE_CDMA_1900 ) */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       case NV_AUTO_NAM_I:
         data_ptr->auto_nam = FALSE;
         break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SRDA_ENABLED_I:
        data_ptr->srda_enabled = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SEC_CODE_I:
        (void) memset( (byte*) data_ptr->sec_code.digits,
                       '0',
                       sizeof(data_ptr->sec_code.digits) );
        return;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_ACQ_ORDER_PREF_I:
      #ifdef FEATURE_WCDMA
#error code not present
      #else
        data_ptr->acq_order_pref.acq_order = CM_GW_ACQ_ORDER_PREF_GSM_WCDMA;
      #endif
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_NET_SEL_MODE_PREF_I:
        data_ptr->net_sel_mode_pref.net_sel_mode = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SERVICE_DOMAIN_PREF_I:
        data_ptr->service_domain_pref.srv_domain = CM_SRV_DOMAIN_PREF_CS_ONLY;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #if (defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE))
      case NV_RTRE_CONFIG_I:
        data_ptr->rtre_config = NV_RTRE_CONFIG_NV_ONLY;
        break;
      #endif /* (defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)) */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #if ( defined(FEATURE_DUAL_SLOTS) )
      case NV_UIM_CDMA_PREF_SLOT_I:
        data_ptr->uim_cdma_pref_slot = NV_UIM_SLOT_NONE;
        break;

      case NV_UIM_GSM_PREF_SLOT_I:
        data_ptr->uim_gsm_pref_slot = NV_UIM_SLOT_NONE;
        break;
      #endif /* ( defined(FEATURE_DUAL_SLOTS) ) */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_PS_DATA_ORIG_PREF_I:
        data_ptr->ps_data_orig_pref = NV_PS_DATA_ORIG_PREF_ANY;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     case NV_GSM_AMR_CALL_CONFIG_I:
        data_ptr->gsm_amr_call_config = FALSE;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_PRL_PREF_I:
        data_ptr->prl_pref.prl = CM_PRL_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_WLAN_TECH_PREF_I:
        data_ptr->wlan_tech_pref.tech_pref = CM_WLAN_TECH_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_WLAN_BSS_TYPE_PREF_I:
        data_ptr->wlan_bss_type_pref.bss_type_pref = CM_WLAN_BSS_TYPE_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_WLAN_SCAN_PREF_I:
        data_ptr->wlan_scan_pref.scan_mode = CM_WLAN_SCAN_PREF_AUTO;
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_WLAN_NET_SEL_MODE_PREF_I:
        data_ptr->wlan_net_sel_mode_pref.net_sel_mode =
                                  CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ACCOLC_I:

        (void) memset( (byte*)  data_ptr->accolc.ACCOLCpClass,
                       0,
                       sizeof( data_ptr->accolc.ACCOLCpClass) );
        return;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_DDTM_CNTL
      case NV_DDTM_SETTINGS_I:

        /* Return error, the calling function should write the
        ** correct values to NV
        */
        data_ptr->ddtm_settings.num_srv_opt = CM_INVALID_DDTM_NUM_SRV_OPT;
        return;
      #endif


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/     

      case NV_GPRS_ANITE_GCF_I:
        data_ptr->gprs_anite_gcf = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/    

      case NV_UMTS_CALL_VT_CODEC_LIST_I:
        data_ptr->umts_call_vt_codec_list = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_MOB_CAI_REV_I:
        #if defined( FEATURE_CDMA_800 ) || defined( FEATURE_CDMA_1900 )
        /* P_REV_IS2000 (6) is the default suggested by CP
        */
        data_ptr->mob_cai_rev = P_REV_IS2000;
        #else
        data_ptr->mob_cai_rev = 0;
        #endif
        break;

      case NV_ENS_ENABLED_I:
        #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
        #endif
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_HOME_MCC_I:
        data_ptr->home_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_USR_SID_TO_MCC_ASSOC_TBL_I:
        data_ptr->usr_sid_to_mcc_assoc_tbl.tbl_len = 0;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_HS_BASED_PLUS_DIAL_SETTING_I:
        data_ptr->hs_based_plus_dial_setting = CM_HS_BASED_PLUS_DIAL_DISABLED;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_IMSI_MCC_I:
        data_ptr->imsi_mcc.imsi_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DISABLE_CM_CALL_TYPE_I:
        data_ptr->disable_cm_call_type = CM_CALL_TYPE_NONE_MASK;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        CM_SWITCH_ERR( "Unexpected NV item=%d",cmnv_cmd_sync.item,0,0 );
        break;

    } /* switch */
    /*lint -restore */

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV read failed because of NV_NOTACTIVE_S or NV_FAIL_S
  ** try writing the defaulted value back to NV.
  */
  if( cmnv_cmd_sync.status == NV_NOTACTIVE_S ||
      cmnv_cmd_sync.status == NV_FAIL_S )
  {
    CM_MSG_HIGH("Write default value to NV, item=%d",cmnv_cmd_sync.item,0,0);

    /* Write to NV by:
    ** 1. Indicating a write command.
    ** 2. Clearing the NV signal
    ** 3. Queuing a write command to the NV task
    ** 4. Waiting for NV task to finish servicing the write command.
    */
    cmnv_cmd_sync.cmd = NV_WRITE_F;
    (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
 
    cmnv_cmd_sync.tcb_ptr = rex_self();

    #ifdef FEATURE_NV_RPC_SUPPORT
    if (0 == std_strncmp ((cmnv_cmd_sync.tcb_ptr)->task_name,"ONCR", 4))
    {
      (void)nv_cmd_remote(cmnv_cmd_sync.cmd,
                          cmnv_cmd_sync.item,
                          cmnv_cmd_sync.data_ptr
                          );
    }
    else
    #endif
    {
      nv_cmd( &cmnv_cmd_sync );
    }

    cm_wait( CM_NV_SYNC_SIG );
    (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  }

}

#ifdef FEATURE_UIM_RUIM

/*---------------------------------------------------------------------------
                          R-UIM INTERFACE BLOCK
---------------------------------------------------------------------------*/

/* Status reported by R-UIM for each command */
static uim_rpt_status cm_uim_status_report = UIM_FAIL;

/*===========================================================================

FUNCTION cmnv_ruim_report

DESCRIPTION
  Accept status reports from R-UIM
  Set the global status and signal CM task that status has arrived.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmnv_ruim_report
(
  /*lint -e{818} 
  ** Can't be const or compile errors due to diff function prototype in UIM
  */ 
  uim_rpt_type *report           /* R-UIM command status report */ 
)
/*lint -save -e818*/
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(report != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cm_uim_status_report = report->rpt_status;

  (void) rex_set_sigs( &cm_tcb, CM_RUIM_STATUS_SIG );

} /* cmnv_ruim_report */
/*lint -restore*/



/*===========================================================================

FUNCTION cmnv_get_uim_status

DESCRIPTION
  Get the status from the UIM by actually pinging the card.

DEPENDENCIES
  Non-reentrant.

RETURN VALUE
  Returns UIM_PASS or UIM_FAIL.

SIDE EFFECTS
  None.

===========================================================================*/
uim_rpt_status cmnv_get_uim_status(void)
{
  uim_cmd_type cmd;
   /* R-UIM command buffer */

#if defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_GSM && FEATURE_MMGSDI */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate callback function to be called upon command completion.
  */
  cmd.hdr.rpt_function              = cmnv_ruim_report;
  cmd.hdr.options                   = UIM_OPTION_ALWAYS_RPT;

  /* Indicate command completion is NOT to be signaled.
  */
  cmd.status.hdr.cmd_hdr.task_ptr   = NULL;
  cmd.status.hdr.cmd_hdr.sigs       = 0;
  cmd.status.hdr.cmd_hdr.done_q_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the protocol and the slot for the command
  */
#if defined(FEATURE_MMGSDI_UMTS)
#error code not present
#else
  cmd.hdr.protocol                  = UIM_CDMA;
#endif /* FEATURE_MMGSDI_GSM && FEATURE_MMGSDI */

  cmd.hdr.slot                      = UIM_SLOT_1;

  /* Indicate that card status is to be returned.
  */
  cmd.status.hdr.command            = UIM_STATUS_F;

  /* ---------------------------------------------------------------------*/

  /* Fill the slot information */

#ifdef FEATURE_DUAL_SLOTS
  cmd.status.hdr.slot = uim_get_slot_for_app(UIM_CDMA_APP);
#else
  cmd.status.hdr.slot = UIM_SLOT_1;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the command to the R-UIM:
  **  1. Clear the "command done signal"
  **  2. Send the command
  **  3. Wait for the command to be done
  */
  (void) rex_clr_sigs( &cm_tcb, CM_RUIM_STATUS_SIG );
  #ifndef FEATURE_VIRTUAL_SIM
  uim_cmd(&cmd);
  #else
  virtuim_cmd(&cmd);
  #endif
  (void) rex_wait( CM_RUIM_STATUS_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return status.
  */
  return( cm_uim_status_report );
}




/*===========================================================================

FUNCTION cmnv_get_uim_spc_status

DESCRIPTION
  Read the spc status from RUIM.

DEPENDENCIES

RETURN VALUE
  TRUE    if the spc status is other than 0.
  FLASE   if the spc status is 0.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmnv_get_uim_spc_status( void )
{
  uim_cmd_type cmd;
      /* R-UIM command buffer */

  byte spc_status    = (byte)~0;
      /* Status buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate callback function to be called upon command completion.
  */
  cmd.hdr.options           = UIM_OPTION_ALWAYS_RPT;
  cmd.hdr.rpt_function      = cmnv_ruim_report;

  /* Indicate command completion is NOT to be signaled.
  */
  cmd.hdr.cmd_hdr.task_ptr   = NULL;
  cmd.hdr.cmd_hdr.sigs       = 0;
  cmd.hdr.cmd_hdr.done_q_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the protocol and the slot for the command
  */
  cmd.hdr.protocol           = UIM_CDMA;
  cmd.hdr.slot               = UIM_SLOT_1;

  /* Indicate an access operation.
  */
  cmd.hdr.command           = UIM_ACCESS_F;

  cmd.access_uim.hdr.slot       = UIM_SLOT_1;

  cmd.access_uim.access     = UIM_READ_F;
  cmd.access_uim.item       = UIM_CDMA_SPC_STATUS;
  cmd.access_uim.offset     = 0;
  cmd.access_uim.num_bytes  = 1;
  cmd.access_uim.data_ptr   = &spc_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs( &cm_tcb, CM_RUIM_STATUS_SIG );
  #ifndef FEATURE_VIRTUAL_SIM
  uim_cmd(&cmd);
  #else
  virtuim_cmd(&cmd);
  #endif
  (void) rex_wait( CM_RUIM_STATUS_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cm_uim_status_report == UIM_PASS )
  {
    spc_status &= UIM_SPC_STATUS_MASK;
  }
  else
  {
    /* If it failed set to zero as default */
    spc_status = 0;
  }

  if( spc_status ) {
    return TRUE;
  }

  return FALSE;
}

#ifdef FEATURE_UIM_RUN_TIME_ENABLE

/*===========================================================================

FUNCTION cmnv_send_rtre_command

DESCRIPTION
  Sends RTRE configuration to NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_send_rtre_command
(
  nv_item_type *data_ptr                     /* pointer to space for item */
)
{
  /* Prepare the NV command
  */
  cmnv_cmd_sync.item = NV_RTRE_CONFIG_I;
  cmnv_cmd_sync.cmd = NV_RTRE_OP_CONFIG_F;
  cmnv_cmd_sync.data_ptr = data_ptr;
  cmnv_cmd_sync.tcb_ptr = rex_self();
  cmnv_cmd_sync.sigs = CM_NV_SYNC_SIG;
  cmnv_cmd_sync.done_q_ptr = NULL;

  /* Send RTRE command to NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a write command to the NV task
  ** 3. Waiting for NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  nv_cmd( &cmnv_cmd_sync );
  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
    CM_ERR( "Failed sending the CONFIG Command to NV %d",
      cmnv_cmd_sync.status, 0, 0 );
  }

} /* cmnv_send_rtre_command() */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */


#endif /* FEATURE_UIM_RUIM */
