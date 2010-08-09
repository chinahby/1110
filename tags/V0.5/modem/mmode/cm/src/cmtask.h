#ifndef CMTASK_H
#define CMTASK_H
/*===========================================================================

         C A L L   M A N A G E R   T A S K   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMTASK.C


Copyright (c) 2001 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmtask.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/09   ks      Include new RR interface to support camped cell info

04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes.
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added callback function for the WPS result
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
04/01/06   ka      Changes for emerg serv category during orig, aborting 
                   manual search and returning current band with srv ind.                  
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/06/06   ic      Lint cleanup  
12/07/05   ka      Exporting cm_si_network_list_cb
10/13/05   ic      Externalized cmtask_crit_sect_ptr()
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
06/29/04   jqi     Avoid using the value of common signals.
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
06/09/04   dk      Added CM Heap support.
04/13/04   ic      Added prototype of cm_si_acq_fail_cb()
03/24/04   ic      Added prototype of cm_si_ok_to_orig_cb()
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
11/01/02   vt      Rearranged CM signals.
10/07/02   vt      Defined signals for regproxy.
07/08/02   vt      Changed the definitions of SIG's to avoid conflicts with
                   TASK signal definitions.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
09/13/01   RI      Initial Release
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cmi.h"
#include "cmcall.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/*
** CM task signals
*/
#define CM_CLIENT_CMD_Q_SIG   0x0001           /* CM client command signal */
#define CM_RPT_Q_SIG          0x0002       /* CM lower layer report signal */

#define CM_TIMER_SIG          0x0004                  /* Time to update CM */
#define CM_RSSI_TIMER_SIG     0x0008      /* Time to check and update RSSI */
#define CM_RPT_TIMER_SIG      0x0010              /* Time to kick watchdog */

#define CM_REGPRX_CMD_Q_SIG   0x0040         /* CM RegProxy command q sig. */

#define CM_NV_SYNC_RESERVED   0x0080     /* CM_UI_NV_SIG - defined in cm.h */
#define CM_NV_ASYNC_SIG       0x0100          /* Asynchronous NV operation */

#define CM_RUIM_STATUS_SIG    0x0200                /* CM RUIM Done signal */

#define CM_ACM_TIMER_SIG      0x0400 /* Signal sent when ACM timer expires */
#define CM_CCM_TIMER_SIG      0x0800 /* Signal sent when CCM timer expires */
#define CM_GSDI_RESERVED_SIG  0x1000  /* Responds to synchronous responses */
#ifdef FEATURE_CM_MMGSDI
#define CM_MMGSDI_REPORT_SIG  0x10000     /* Responds to MMGSDI operations */
#else
#define CM_GSDI_REPORT_SIG    0x10000        /* Responds to GSDI operation */
#endif  /* FEATURE_CM_MMGSDI */

#define CM_HDRMC_SIG          0x20000              /* CM HDRMC done signal */

/*
** Alias for signals
*/
#define CM_NV_SYNC_SIG        CM_UI_NV_SIG
  /* Signal for synchronous NV operation. Also used for UI initiated NV
  ** operation done in CM task context. Defined in cm.h as 0x0080 */

#define CM_SEGMENT_SIG        CM_CCM_TIMER_SIG
  /* Signal when segment is transferred. */

/*
** Timer durations
*/
#define CM_TIME               1000                    /* Time to update CM */
#define CM_RSSI_TIME          2240                             /* 320*7 ms */


/**--------------------------------------------------------------------------
** externs
** --------------------------------------------------------------------------
*/

extern rex_timer_type  cm_rssi_timer;
    /* RSSI checking timer */

extern cm_cmd_q_type   cm_rpt_q;
    /* Queue to manage clients commands. Clients command are first
    ** being queued in this queue and then serviced in a FIFO order */


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

#ifdef CM_DEBUG
#error code not present
#else
#define cm_cmd_alloc() cm_cmd_alloc_debug(__LINE__)
#endif

/*===========================================================================

FUNCTION cm_cmd_alloc

DESCRIPTION
  Allocate a CM command buffer from the Heap.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_s_type *cm_cmd_alloc_debug
(
  #ifdef CM_DEBUG
#error code not present
  #endif
  
  uint32 line
    /* Line number in the file above */
);


/*===========================================================================

FUNCTION cm_cmd_dealloc

DESCRIPTION
  Deallocate a CM command buffer by returning it to the
  heap.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_cmd_dealloc( void  *ptr );


#define cm_mem_malloc(x) cm_mem_malloc_debug(x, #x)

/*===========================================================================

FUNCTION cm_mem_alloc

DESCRIPTION
  Allocate a chunk of Memory from the Heap. If CM heap is exhausted,
  then it tries to get memory from the TMC Heap.

DEPENDENCIES

   Heap is already initialized.

RETURN VALUE
   Pointer to the memory block.

SIDE EFFECTS
  none

===========================================================================*/
extern void *cm_mem_malloc_debug(

  size_t size,

  char *objname
    /* Used to see the name of the object being freed if "sizeof" operator is
    ** used.
    */
);


#define cm_mem_free(x) cm_mem_free_debug(x, #x)

/*===========================================================================

FUNCTION cm_mem_free

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from CM heap then it is returned their else
   it is returned to the TMC heap.

DEPENDENCIES
   CM and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_mem_free_debug
(
  
  void *ptr,
  
  char *objname
    /* Name of object being freed. */

);





/*===========================================================================

FUNCTION cm_cmd_queue

DESCRIPTION
  Queue a CM command in its appropriate CM command queue.

DEPENDENCIES
  All CM command queues must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_cmd_queue
(
  cm_cmd_s_type *cmd_ptr
);



/*===========================================================================

FUNCTION cm_wait

DESCRIPTION
  Suspend CM from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES
  cm_rpt_timer timer must have been defined with rex_def_timer().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
extern void cm_wait
(
  rex_sigs_type    mask
    /* Mask of signals to wait on */
);


/*===========================================================================
FUNCTION CM_TASK

DESCRIPTION
  The main call manager function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
extern void cm_task
(
  dword dummy
);

/*===========================================================================

FUNCTION cm_si_ok_to_orig_cb

DESCRIPTION
  Ok To Orig callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Ok To Orig status changes from FALSE
  to TRUE while the origination mode is other than none.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_ok_to_orig_cb(

  sd_ss_e_type                     ss,
      /* System selection - MAIN or HDR.
      */

  const sd_si_ok_to_orig_s_type    *si_ok_to_orig_ptr
        /* System indications related to call origination 
        */

);

/*===========================================================================

FUNCTION cm_si_acq_fail_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_acq_fail_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
);


/*===========================================================================

FUNCTION cm_si_data_suspend_cb

DESCRIPTION
  Data suspend state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever data suspend state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_data_suspend_cb(

     boolean             data_suspend
        /* New Data suspend state.
        */
);

/*===========================================================================

FUNCTION cmtask_crit_sect_ptr

DESCRIPTION

  Return pointer to CM critical section. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern rex_crit_sect_type* cmtask_crit_sect_ptr(void);

/*===========================================================================

FUNCTION cm_si_network_list_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_network_list_cb(

    sys_plmn_id_s_type       *rplmn_ptr,
       /* Pointer to a structure containing the last registered PLMN id.
       */

    sys_detailed_plmn_list_s_type *plmn_list_ptr,
       /* Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type    plmn_list_status
       /* Gives the status of plmn list request (success/abort).
       */
);

/*===========================================================================

FUNCTION cm_si_wlan_wps_result_cb

DESCRIPTION
  WLAN WPS Start callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_wlan_wps_result_cb(

   const sys_wlan_wps_result_s_type             *wlan_wps_result_ptr
       /**< Pointer to a structure containing the result of a WPS procedure.
       */
);

#if defined(FEATURE_WCDMA)
#error code not present
#endif

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#error code not present
#endif

/*===========================================================================

FUNCTION cm_srv_lost_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_srv_lost_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
);

/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_mode_pref(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
);

/*===========================================================================

FUNCTION cmtask_cmheap_ptr

DESCRIPTION
  Return pointer to CM heap. 
  Needed by CM unit test cases that test heap memory allocation. 

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  Pointer to CM heap

SIDE EFFECTS
  None

===========================================================================*/
extern mem_heap_type* cmtask_cmheap_ptr( void );
#endif  /* CMTASK_H */

