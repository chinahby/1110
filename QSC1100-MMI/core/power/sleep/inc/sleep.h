#ifndef SLEEP_H
#define SLEEP_H
/*===========================================================================

              S L E E P    T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Task.
  This module is used in the portable only.

      Copyright (c) 1992-2009 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/inc/sleep.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/09   cab     Change back to clkregim.h
11/04/08   jbh     Convert pointer in WinMob sleep_register to guarantee a
                   global address instead of a slot 0 address.
09/16/08   clm     1H08 add support for QSC6270.
08/22/08   cab     Added charger featurization
06/27/08   clm     1H08 sleep re-work.
03/31/08   taw     Added prototype for sleep_get_tcxo_shutdown_count().
03/20/08   jfl     Added support for VDD min restrictions
                   and reverted SLEEP_UIM_OKTS_SIG removal
03/13/08   jfl     Removed SLEEP_UIM_OKTS_SIG
03/13/08   jfl     Removed SLEEP_HS_OKTS_SIG
12/17/07    th     Update to WinMob sleep IOCTLs to add 
                   sleep_set_swfi_restriction                   
12/10/07   cab     Removed SLEEP_USB_OKTS_SIG.
11/27/07   cab     Removed SLEEP_HDR_OKTS_SIG.
10/04/07   taw     Added function sleep_init().
09/07/07   cab     Fixed compilation problem on single proc targets
08/15/07   ebb     Specified values for #define APPS_SLEEP and MODEM_SLEEP
07/20/07   taw     Removed the sleep_hs_usb_in_use global.
07/06/07   cab     Removed SLEEP_CAMERA_OKTS_SIG
06/12/07   ebb     Removed SLEEP_VOC_OKTS_SIG, SLEEP_SND_OKTS_SIG, 
                   SLEEP_VS_OKTS_SIG
05/22/07   cab     Added API sleep_set_swfi_restriction.
05/22/07   cab     Added sleep_hs_usb_in_use to check before going into SWFI.
05/03/07   cab     Removed SLEEP_RXC_OKTS_SIG
05/02/07   cab     Removed SLEEP_SRCH_OKTS_SIG
04/16/07   cab     Mainlined CLK_REGIME_UP_SLEEP__MCLK_HCLK
03/29/07   cab     Removed SLEEP_HDRDEC_OKTS_SIG
03/29/07    th     TCXO vote deregistration for WinMob builds
02/21/07   cab     Removed reference to 0x80000000 REX signal
11/27/06   cab     Removed SLEEP_UI_OKTS_SIG
11/09/06   ddh     Removed SLEEP_MDDI_OKTS_SIG and SLEEP_MDI_OKTS_SIG
11/09/06    th     Remove USB vote from mARM for FEATURE_USB_ON_APPS_PROC
11/01/06   cab     Removed forced definition of FEATURE_NEW_SLEEP_API
10/30/06   jhs     Removed SLEEP_KYPD_OKTS_SIG.
10/13/06   ddh     Added error if FEATURE_NEW_SLEEP_API is NOT defined. 
                   Removed SLEEP_BT_OKTS_SIG.
10/09/06   ddh     Removed use of SLEEP_BT_OKTS_SIG when
                   FEATURE_NEW_SLEEP_API is defined. Also removed DEM and APPS
                   OKTS signals.
10/04/06    th     Implementation of WinMob Sleep IOCTLs
09/05/06   taw     Moved USB vote to be available on both apps and modem.
08/16/06   cab     Removed rex_set_sigs reference from WINCE builds
08/11/06   cab     7200 integration updates 
08/03/06   cab     Remove UI/HS/KEYPAD sleep voting for thinUI builds
                   Add mclk and hclk restrictions to new sleep interface
07/28/06   jwob    Set SLEEP_LSM_OKTS_SIG to 0 if not defined
07/20/06   jwob    Stub SLEEP_(ASSERT|NEGATE)_OKTS macros when not needed
06/28/06   TMR     Added FEATURE_NO_USB_HW_ON_PROCESSOR
06/14/06   ddh     Moved modem specific functions to sleepmod.c
06/12/06   ddh     Moved SLEEP_SND_OKTS_SIG to be checked for both modem and
                   apps for dual proc builds
05/10/06   cab     First implementation of "new" voting interface functions
05/03/06   taw     Removed the DMOV vote for sleep.
04/21/06   taw     Define the battery charger vote signal if 
                   FEATURE_BATTERY_CHARGER is defined.  MSM7500 coupling
                   will be removed in the near future.
04/21/06   taw     Added votes from MDP and MDDI on the modem if the secondary
                   display is controlled by the modem during apps power 
                   collapse.  Also, added a vote for DMOV on the apps.
04/17/06   ejv     Added SLEEP_MC_INIT_OKTS_SIG for T_QSC60X0.
04/03/06   ptm     Reduced header coupling for 7500.
03/29/06   dy      Added GSM/GPRS support for multi-mode build.
03/14/06   taw     Moved the UIM vote to be on the modem side only.
02/13/06   cab     Added SDCC_DRV_OKTS_SIG for the SDCC/SDIO driver.
02/08/06   taw     Removed SLEEP_APPS_PC_OKTS_SIG, will just use 
                   SLEEP_APPS_OKTS_SIG
01/10/06   ddh     Added SLEEP_DEM_OKTS_SIG to hold off sleep from the
                   Apps side until all subsystems are up and running.
11/11/05   cab     Set SLEEP_GRAPHICS_OKTS_SIG to be 0x00000400
07/13/05   ddh     Added defining of APPS_SLEEP for none dual proc chips
06/22/05   ddh     Cleaned up chip specific sleep defines
05/19/05   ajn     Removed unused DS & DOG OKTS signals.
05/16/05   ajn     Moved signals to recreate required TASK sigs hole.
05/02/05   ajn     Added SLEEP_GRP_OKTS_SIG
04/06/05   ddh     Set SLEEP_GRAPHICS_OKTS_SIG to be 0
03/21/05   ddh     Added modem and apps support 

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "task.h"
#include "rex.h"
#include "clkregim.h"

#ifdef T_MSM7500
#error code not present
#else /* !T_MSM7500 */
  /* The following #include files are not needed here, but may be required
     by other files which include this file.  Encourage the owners of those
     files to include the required headers directly */
  #include "rf.h"
  #ifdef CUST_H
    #include "customer.h"
  #endif /* CUST_H */
#endif /* !T_MSM7500 */


#ifndef FEATURE_NEW_SLEEP_API 
  #error "FEATURE_NEW_SLEEP_API must be defined with this version of sleep.h"
#endif /* FEATURE_NEW_SLEEP_API */

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                     Chip Specific Sleep Definitions
--------------------------------------------------------------------------*/

#if defined(T_MSM7500) || defined(T_MSM7200)
  #ifdef IMAGE_MODEM_PROC
    #define MODEM_SLEEP             0

    #ifndef FEATURE_STANDALONE_MODEM
      #define DUAL_PROC_MODEM_SLEEP
    #endif /* FEATURE_STANDALONE_MODEM */
  #else /* !IMAGE_MODEM_PROC If not modem it MUST be APPS */ 
    #define APPS_SLEEP              1

    #if !defined(FEATURE_STANDALONE_APPS) && \
        !defined(FEATURE_SLEEP_APPS_DISABLED)
      #define DUAL_PROC_APPS_SLEEP
    #endif /* !FEATURE_STANDALONE_APPS && !FEATURE_SLEEP_APPS_DISABLED */
  #endif /* !IMAGE_MODEM_PROC */
#else /* !defined(T_MSM7500) && !defined(T_MSM7200) */
  #define MODEM_SLEEP               0
  #define APPS_SLEEP                0
#endif /* T_MSM7500 */


/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

typedef uint32 sleep_okts_handle;
     
/*--------------------------------------------------------------------------
                             Signal Definitions
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  The following 3 TASK signals occupy bit positions 13, 14 and 15 in the
  current task signal mask.  If they change, then the remaining OKTS bits
  must also change to work around the task-reserved bits.
--------------------------------------------------------------------------*/

/* After creation and initialization, the Sleep Task waits for the Main
   Control Task to set this signal before beginning its normal execution. */
#define  SLEEP_START_SIG          TASK_START_SIG

/* The Main Control Task never sets this signal to tell the Sleep Task to
   stop execution. */
#define  SLEEP_STOP_SIG           TASK_STOP_SIG

/* This signal is set to tell Sleep to go into offline mode.
   Actually, this doesn't do anything to Sleep. */
#define  SLEEP_OFFLINE_SIG        TASK_OFFLINE_SIG

/* Modem OKTS Signal definitions 
   For Dual processor builds these SIGNALS are only used on the
   MODEM processor for all other builds the signals are always
   present */

#ifdef CUST_EDITION  
#ifndef SLEEP_UI_OKTS_SIG
  #define  SLEEP_UI_OKTS_SIG        0x00000020
#endif
#endif /*CUST_EDITION*/

#ifdef MODEM_SLEEP
  /* This signal is used when the SDCC/SDIO driver is active */
  #define  SDCC_DRV_OKTS_SIG           0x40000000

  #ifdef FEATURE_MFLO
#error code not present
  #endif /* FEATURE_KEEPER */
  
  #ifdef T_QSC60X0
    #define SLEEP_MC_INIT_OKTS_SIG     0x10000000
  #endif /* T_QSC60X0 */

  #ifdef FEATURE_GPS_MODE
#error code not present
  #endif /* FEATURE_GPS_MODE */

  #ifdef T_SURF
    #if (defined(FEATURE_PM1000) || defined(FEATURE_FFA))
#error code not present
    #endif /* (defined(FEATURE_PM1000) || defined(FEATURE_FFA)) */
  #else /* !T_SURF */
    #if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK)
      #define  SLEEP_CHGT_OKTS_SIG     0x00200000
    #elif !defined(T_CH) && !defined(T_MSM7500)
      #define  SLEEP_CHGT_OKTS_SIG     0x00200000
    #endif /* !defined(T_CH) && !defined(T_MSM7500) */
  #endif /* !T_SURF */

  /* This signal is when the RF driver wants Sleep to trigger its temp
     comp process. */
  #define  SLEEP_RF_TEMP_COMP_SIG      0x00100000

  /* NOTE:  This should be temporary.  For 7200, which is a dual proc build,
     the following signal is defined for the modem processor.  It uses a
     value that is already in use on the apps processor, but since this is
     a dual processor build, this will be OK.  For single processor builds
     that unionize the MODEM and APPS signals, this is not OK, hence the
     7200 ifdef.  This is somewhat convoluted, and the expectation is for this
     signal to move to the new sleep voting interface soon (8/11/2006) */
  #if defined(T_MSM7200) && defined(FEATURE_MMGPS)
    #define SLEEP_LSM_OKTS_SIG         0x00000010
  #endif /* defined(T_MSM7200) && defined(FEATURE_MMGPS) */

  #if defined(FEATURE_UIM) && !defined(FEATURE_UIM_USES_NEW_SLEEP_API) 
    #define  SLEEP_UIM_OKTS_SIG        0x00000008
  #endif /* FEATURE_UIM */
#endif /* MODEM_SLEEP */

/* 
                  TASK_START_SIG == 0x00008000
                   TASK_STOP_SIG == 0x00004000
                TASK_OFFLINE_SIG == 0x00002000
*/

#ifdef APPS_SLEEP
  /* Apps OKTS Signal definitions 
     For Dual processor builds these SIGNALS are only used on the
     APPS processor for all other builds the signals are always
     present */
  #if defined(FEATURE_I3D) || defined(FEATURE_GRAPHICS_OPENGLES_CL)
    #ifndef T_MSM7500
      #define SLEEP_GRAPHICS_OKTS_SIG  0x00000400
    #else /* !T_MSM7500 */
#error code not present
    #endif /* !T_MSM7500 */
  #endif /* FEATURE_I3D || FEATURE_GRAPHICS_OPENGLES_CL */

  #ifdef FEATURE_MP4_DECODER
    #define  SLEEP_MP4_OKTS_SIG        0x00000100
  #endif /* FEATURE_MP4_DECODER */
#endif /* APPS_SLEEP */

/* For dual processor builds the following signal definitions are present 
   on BOTH the APPS and MODEM processors. For all other builds these 
   signals are are always present */
   
#ifdef FEATURE_DIAG_TASK
  #define  SLEEP_DIAG_OKTS_SIG         0x00000002
#endif /* FEATURE_DIAG_TASK */

/* This signal is set when the gSleepRptTimer expires.
   This timer is used to enable Sleep to kick the watchdog on time
   during a wait for some event. */
#define  SLEEP_RPT_TIMER_SIG           0x00000001

/* Any OKTS signal that is not defined should be set to 0 */

#ifndef SLEEP_LSM_OKTS_SIG
  #define SLEEP_LSM_OKTS_SIG        0
#endif /* SLEEP_LSM_OKTS_SIG */

#ifndef SLEEP_MC_INIT_OKTS_SIG
  #define SLEEP_MC_INIT_OKTS_SIG    0
#endif /* SLEEP_MC_INIT_OKTS_SIG */

#ifndef SLEEP_GPSONE_OKTS_SIG  
  #define SLEEP_GPSONE_OKTS_SIG     0
#endif /* SLEEP_GPSONE_OKTS_SIG */

#ifndef  SLEEP_CHGT_OKTS_SIG
  #define  SLEEP_CHGT_OKTS_SIG      0
#endif /* SLEEP_CHGT_OKTS_SIG */

#ifndef  SLEEP_RF_TEMP_COMP_SIG
  #define  SLEEP_RF_TEMP_COMP_SIG   0
#endif /* SLEEP_RF_TEMP_COMP_SIG */

#ifndef  SLEEP_DIAG_OKTS_SIG
  #define  SLEEP_DIAG_OKTS_SIG      0
#endif /* SLEEP_DIAG_OKTS_SIG */

#ifndef  SLEEP_GRAPHICS_OKTS_SIG
  #define  SLEEP_GRAPHICS_OKTS_SIG  0
#endif /* SLEEP_GRAPHICS_OKTS_SIG */

#ifndef  SLEEP_MP4_OKTS_SIG
  #define  SLEEP_MP4_OKTS_SIG       0
#endif /* SLEEP_MP4_OKTS_SIG */

#ifndef  SLEEP_UIM_OKTS_SIG
  #define  SLEEP_UIM_OKTS_SIG       0
#endif /* SLEEP_UIM_OKTS_SIG */

#ifndef  SLEEP_KEEPER_OKTS_SIG
  #define  SLEEP_KEEPER_OKTS_SIG    0
#endif /* SLEEP_KEEPER_OKTS_SIG */


/*--------------------------------------------------------------------------
  Sleep swfi vote type 
--------------------------------------------------------------------------*/
typedef enum
{
  SLEEP_SWFI_RESTRICT_OFF = 0x0,
  SLEEP_SWFI_RESTRICT_ON
} sleep_swfi_restrict_type;

/*--------------------------------------------------------------------------
  Sleep vdd min vote type (for modem processor)
--------------------------------------------------------------------------*/
typedef enum
{
  VDD_MIN_ALLOWED = 0x0,
  VDD_MIN_NOT_ALLOWED
} sleep_vdd_min_restrict_type;

#if !defined(FEATURE_WINCE) && !defined(FEATURE_WINCE_BOOTLOADER) && \
   ( defined(T_SLEEP) || defined(FEATURE_SLEEP) )

  #define SLEEP_ASSERT_OKTS( sig ) \
    (void) rex_set_sigs( &sleep_tcb, (sig) )

  #define SLEEP_NEGATE_OKTS( sig ) \
    (void) rex_clr_sigs( &sleep_tcb, (sig) )

#else /* defined(FEATURE_WINCE) || defined(FEATURE_WINCE_BOOTLOADER) ||
         ( !defined(T_SLEEP) && !defined(FEATURE_SLEEP) ) */

  #define SLEEP_ASSERT_OKTS( sig )
  #define SLEEP_NEGATE_OKTS( sig )

  #if (!defined(FEATURE_WINCE_BOOTLOADER))

  typedef struct
  {
  	const char * name;
  	boolean      okts;

  } tSleepRegister;

  typedef struct
  {
    sleep_okts_handle        handle;
    sleep_swfi_restrict_type swfi_restriction;
    
  } tSleepSetSWFIRestriction;

  #endif /* !FEATURE_WINCE_BOOTLOADER */

#endif /* !defined(FEATURE_WINCE) && !defined(FEATURE_WINCE_BOOTLOADER) &&
          ( defined(T_SLEEP) || defined(FEATURE_SLEEP) ) */


#ifndef TCXO_WARMUP_MS
  /* It takes about 1ms for TCXO to be stable on SURF */
  #define TCXO_WARMUP_MS 1
#endif /* !TCXO_WARMUP_MS */

#ifdef FEATURE_TCXO_SHUTDOWN
  #define TCXO_WARMUP_US (TCXO_WARMUP_MS * 1000)

  #ifdef FEATURE_NO_TCXO_VOTE
#error code not present
  #endif /* FEATURE_NO_TCXO_VOTE */
#else /* !FEATURE_TCXO_SHUTDOWN */
  /* Allow for the extra time it takes to switch back to TCXO */
  #ifndef TCXO_CLOCK_SWITCH_OVERHEAD_MS
    #define TCXO_CLOCK_SWITCH_OVERHEAD_MS 6
  #endif /* !TCXO_CLOCK_SWITCH_OVERHEAD_MS */
#endif /* !FEATURE_TCXO_SHUTDOWN */

/* Number of 4.92 MHz TCXO/4 clocks in one millisecond,
   assuming that the clock error is zero. */
#define NOMINAL_CLOCKS_PER_MS    4920L

/* Used to indicate if TCXO must be kept on or not for the UART */
extern boolean sleep_uart_clock_rgm;

/* Used to indicate from BT if TCXO must be kept on or not */
extern boolean sleep_bt_clock_rgm;

#ifdef FEATURE_SLEEP_TASK
extern rex_tcb_type sleep_tcb;
#endif

/* These are just for backwards compatibility and don't actually do anything */
#define LSM_SLOW_REX_TICKS_BMSK  0x00000002
#define sleep_require_5ms_rex_ticks(mask)
#define sleep_relinquish_5ms_rex_ticks(mask)

/* These are just for backwards compatibility and don't actually do anything */
#define LSM_TRK_LO_ADJ_PDM_BMSK         0x00000001
#define sleep_require_trk_lo_adj_pdm(mask)
#define sleep_relinquish_trk_lo_adj_pdm(mask)

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SLEEP_TASK

DESCRIPTION
  This procedure contains the main processing loop for the sleep task.
  It continually loops, looking for a consensus that the CPU may be
  put to sleep.  Lacking a consensus, it still halts the processor, but
  in "idle" mode -- in which any normal interrupt will restart processing
  with minimal delay.

DEPENDENCIES
  This procedure should be called by rex_def_task.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  The CPU may be put to sleep by this task.

===========================================================================*/

extern void sleep_task
(
  /* Parameter received from Main Control task - ignored */
  /*lint -esym(715,ignored)
     Have lint not complain about the ignored parameter 'ignored' which is
     specified to make this routine match the template for rex_def_task(). */
  dword ignored
);

/*===========================================================================

FUNCTION SLEEP_REGISTER

DESCRIPTION
  Required at init time for any client that wants a unique vote for sleep.
  Pointer passed in must be unique, and the initial vote for sleep is
  indicated by the second parameter.

DEPENDENCIES
  None.

RETURN VALUE
  Sleep client handle, used for subsequent calls to assert and negate.

SIDE EFFECTS
  After this function completes, a new client record in the internal sleep 
  array structure is reserved for the calling client.

===========================================================================*/

#if !defined(FEATURE_WINCE) || defined(FEATURE_WINCE_OAL)

  sleep_okts_handle sleep_register
  ( 
    const char      *pszName, 
    boolean          bOkts
  );

#elif defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#else /* !FEATURE_WINCE_BOOTLOADER */
#error code not present
#endif /* !FEATURE_WINCE || FEATURE_WINCE_OAL */

/*===========================================================================

FUNCTION SLEEP_DEGREGISTER

DESCRIPTION
  Remove a client from the list of those currently registered

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#if !defined(FEATURE_WINCE) || defined(FEATURE_WINCE_BOOTLOADER)

  #define sleep_deregister

#elif defined(FEATURE_WINCE_OAL)
#error code not present
#else /* FEATURE_WINCE_OAL */
#error code not present
#endif /* !FEATURE_WINCE || FEATURE_WINCE_BOOTLOADER */

/*===========================================================================

FUNCTION SLEEP_ASSERT_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is ok to sleep (tcxo off).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to TRUE.

===========================================================================*/

#if !defined(FEATURE_WINCE) || defined(FEATURE_WINCE_OAL)

  void sleep_assert_okts
  (
    sleep_okts_handle handle
  );

#elif defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#else /* !FEATURE_WINCE_BOOTLOADER */
#error code not present
#endif /* !FEATURE_WINCE || FEATURE_WINCE_OAL */

/*===========================================================================

FUNCTION SLEEP_NEGATE_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is not ok to sleep (tcxo on).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to FALSE.

===========================================================================*/

#if !defined(FEATURE_WINCE) || defined(FEATURE_WINCE_OAL)

  void sleep_negate_okts
  (
    sleep_okts_handle handle
  );

#elif defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#else /* !FEATURE_WINCE_BOOTLOADER */
#error code not present
#endif /* !FEATURE_WINCE || FEATURE_WINCE_OAL */

/*===========================================================================

FUNCTION SLEEP_SET_MCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on mclk.  If a client requires
  mclk during sleep, the client can use this function to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal mclk_restriction is set

===========================================================================*/

void sleep_set_mclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_mclk_sleep_type  mclk_restriction
);

/*===========================================================================

FUNCTION SLEEP_SET_HCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on hclk.  If a client requires
  hclk during sleep, the client can use this function to indicate the speed
  that is required.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal hclk_restriction is set

===========================================================================*/

void sleep_set_hclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_hclk_sleep_type  hclk_restriction
);

/*===========================================================================

FUNCTION SLEEP_SET_SWFI_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on swfi.  If a client requires 
  that SWFI (microprocessor sleep) not happen, then it can use this function
  to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal swfi_restriction is set

===========================================================================*/

#if !defined(FEATURE_WINCE) || defined(FEATURE_WINCE_OAL)

  void sleep_set_swfi_restriction
  (
    sleep_okts_handle          handle,
    sleep_swfi_restrict_type   swfi_restriction
  );

#elif defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#else /* !FEATURE_WINCE_BOOTLOADER */
#error code not present
#endif /* !FEATURE_WINCE || FEATURE_WINCE_OAL */

/*===========================================================================

FUNCTION SLEEP_SET_VDD_MIN_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on vdd minimization.
  This would only occur if tcxo shutdown (sleep) occurs, so is only relevant
  if the clients sleep vote is okts.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal vdd_min_restriction is set

===========================================================================*/

void sleep_set_vdd_min_restriction
(
  sleep_okts_handle             handle,
  sleep_vdd_min_restrict_type   vdd_min_restriction
);

/*===========================================================================

FUNCTION SLEEP_INIT

DESCRIPTION
  Initialize the sleep subsystem.

DEPENDENCIES
  timer_init() must already be called, and the timer task must be started.

RETURN VALUE
  None

SIDE EFFECTS
  A timer is set which causes sleep to be withheld until it expires.

===========================================================================*/

extern void sleep_init( void );

/*===========================================================================

FUNCTION SLEEP_GET_TCXO_SHUTDOWN_COUNT

DESCRIPTION
  Returns the number of times the TCXO has been shutdown since the phone
  was turned on.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/

extern uint32 sleep_get_tcxo_shutdown_count( void );

#endif /* SLEEP_H */

