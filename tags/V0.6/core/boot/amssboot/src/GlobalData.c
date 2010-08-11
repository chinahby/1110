#include "comdef.h"
#include "armasm.h"
#include "boothw.h"
#include "target.h"
#include "customer.h"
#include "GlobalData.h"
#include "task.h"

uint32 g_BootResetFlag;

#ifdef CUST_EDITION
/*---------------------------------------------------------------------------

                NON-VOLATILE DATABASE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        nv_tcb;                      /* TCB for NV task.       */
rex_stack_word_type nv_stack [NV_STACK_SIZ];     /* Stack for NV task.     */

/*---------------------------------------------------------------------------

                EMBEDDED FILE SYSTEM TASK DECLARATIONS

---------------------------------------------------------------------------*/

#if defined FEATURE_EFS
rex_tcb_type        fs_tcb;                  /* TCB for File System task.  */
rex_stack_word_type fs_stack [FS_STACK_SIZ]; /* Stack for File System task.*/

#if defined(FS_FEATURE_RMTEFS_SERVER) && !defined(FEATURE_IG_EFS_EXT_SERVER)
rex_tcb_type        efs_srvr_tcb;
/* Actually, stack size of 2048 bytes is sufficient but we use 4096 here */
rex_stack_word_type efs_srvr_stack[EFS_SRVR_STACK_SIZ];
#endif /* FS_FEATURE_RMTEFS_SERVER && !FEATURE_IG_EFS_EXT_SERVER */

#endif /* FEATURE_EFS */

#ifdef FEATURE_EFS_SFAT
rex_tcb_type        sfat_tcb;                    /* TCB for SFAT task.  */
rex_stack_word_type sfat_stack [SFAT_STACK_SIZ]; /* Stack for SFAT task.*/
#endif /* FEATURE_EFS_SFAT */

#ifdef FEATURE_EFS_FDI
rex_tcb_type        fdi_bg_tcb;                  /* TCB for FDI Background task.  */ 
rex_stack_word_type fdi_bg_stack [FDI_BG_STACK_SIZ]; /* Stack for FDI Background task.*/
#endif


#if ( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || defined(FEATURE_ACP) )
/*---------------------------------------------------------------------------

                      TRANSMIT TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        tx_tcb;                    /* TCB for TX task.         */
rex_stack_word_type tx_stack [TX_STACK_SIZ];   /* Stack for TX task.       */
#endif /* !FEATURE_HWTC */

/*---------------------------------------------------------------------------

                   RECEIVE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        rx_tcb;                      /* TCB for RX task.       */
rex_stack_word_type rx_stack [RX_STACK_SIZ];     /* Stack for RX task.     */

/*---------------------------------------------------------------------------

                   SEARCHER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        srch_tcb;                    /* TCB for SRCH task.     */
rex_stack_word_type srch_stack [SRCH_STACK_SIZ]; /* Stack for SRCH task.   */
#endif /* !FEATURE_HWTC */

/*---------------------------------------------------------------------------

                   LAYER2(RXTX) TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
rex_tcb_type        rxtx_tcb;                    /* TCB for RXTX task.     */
rex_stack_word_type rxtx_stack [RXTX_STACK_SIZ]; /* Stack for RXTX task.   */
#endif /* !FEATURE_HWTC */


/*---------------------------------------------------------------------------

                   MAIN CONTROL TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        mc_tcb;                      /* TCB for MC task.       */
rex_stack_word_type mc_stack [MC_STACK_SIZ];     /* Stack for MC task.     */

#endif/* FEATURE_CDMA_800 || FEATURE_CDMA_1900 || FEATURE_ACP */

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif /* #ifdef FEATURE_DIAG_RPC */

/*---------------------------------------------------------------------------

                     HANDSET TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        hs_tcb;                      /* TCB for the HS task.   */
rex_stack_word_type hs_stack [HS_STACK_SIZ];     /* Stack for HS task.     */

/*---------------------------------------------------------------------------

                     DIAGNOSTIC TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        diag_tcb;                    /* TCB for DIAG task.     */
rex_stack_word_type diag_stack [DIAG_STACK_SIZ]; /* Stack for DIAG Task.   */

#if defined(FEATURE_IG_QDIAG)
rex_tcb_type        qdiag_tcb;                     /* TCB for QDIAG task.   */
rex_stack_word_type qdiag_stack [QDIAG_STACK_SIZ]; /* Stack for QDIAG Task. */
#endif  /* defined(FEATURE_IG_QDIAG) */


/*---------------------------------------------------------------------------

                     CALL MANAGER TASK DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_CM
rex_tcb_type        cm_tcb;                      /* TCB for CM task.       */
rex_stack_word_type cm_stack [CM_STACK_SIZ];     /* Stack for CM Task.     */
#endif /* FEATURE_CM */

#ifdef FEATURE_BT
/*---------------------------------------------------------------------------

                    BLUETOOTH TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        bt_tcb;                      /* TCB for the BT task.   */
rex_stack_word_type bt_stack [BT_STACK_SIZ];     /* Stack for BT task.     */

#ifdef FEATURE_BT_EXTPF
/*---------------------------------------------------------------------------

              BLUETOOTH EXTERNAL PROFILE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        bt_pf_tcb;
rex_stack_word_type bt_pf_stack [BT_PF_STACK_SIZ];

#endif /* FEATURE_BT_EXTPF */

#endif /* FEATURE_BT */

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION SESSION MANAGER DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION SMS CLIENT DECLARATIONS

---------------------------------------------------------------------------*/

#ifdef FEATURE_GPSONE_SMS_CLIENT
#error code not present
#endif

/*--------------------------------------------------------------------------

                   POSITION DETERMINATION COMMUNICATION MANAGER DECLARATIONS

---------------------------------------------------------------------------*/

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS)
#error code not present
#endif


#ifdef FEATURE_GPSONE_PE
#error code not present
#endif /* FEATURE_GPSONE_PE */

#ifdef FEATURE_GPSSRCH_TASK
#error code not present
#endif /* FEATURE_GPSSRCH_TASK */


#ifdef FEATURE_GPSONE_STANDALONE
#error code not present
#endif /* FEATURE_GPSONE_STANDALONE */



#ifdef FEATURE_DBGR
/*---------------------------------------------------------------------------

                     DEBUGGER TASK DECLARATIONS

---------------------------------------------------------------------------*/
rex_tcb_type        dbgr_tcb;                    /* TCB for DBGR task.     */
rex_stack_word_type dbgr_stack [DBGR_STACK_SIZ]; /* Stack for DBGR Task.   */
#endif /*FEATURE_DBGR*/

/*---------------------------------------------------------------------------

                     USER INTERFACE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        ui_tcb;                      /* TCB for UI task.       */
rex_stack_word_type ui_stack [UI_STACK_SIZ];     /* Stack for UI Task.     */

#endif

