#ifndef SRCH_RX_H
#define SRCH_RX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ R X  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_rx.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2005 - 2010
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_rx.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/10   sst     Added srch_rx_mdsp_pause() api
04/05/10   pk      Removed _chain_assn() as it should not be exported.
                   and few Lint fixes.
01/30/09   sst     Added srch_rx_release_owned_chains()
11/10/08   mca     Added new RF warmup API
10/27/08   aps     Removed featurization of srch_rx_mdsp_chain_assn()
09/08/08   aps     srch_rx_enter_mode() api change
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
                   Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
06/23/08   aps     FTS - support for band configurability
06/20/08   mca     Merge from //depot
05/01/08   aps     Added parameter to srch_rx_exchange_devices() api
03/17/08   pa      Pass on origination pending to srch_rx_request_both_chains()
03/13/08   sst     Modify FTS version to be determined at runtime
02/22/08   aps     FTS3 - added srch_rx_request_and_notify_last_failed()
02/11/08   aps     Full Time SHDR 3/4 changes
04/27/07   aps     Wrapper function for SRCH_RX_MDSP_CHAIN_ASSN()
07/17/06   aps     Fix for rf sub packet being wrongly built in diversity
                   mode.
05/23/06   rkc     Add bypass_rtc_sync parameter to srch_rx_init_and_tune()
04/09/06   grl     Converted TRM_RX_BEST_POSSIBLE to TRM_RX_ANY if not enough
                   time from reservation to request.
04/04/06   awj     Added translation for TRM_RX_BEST and TRM_RX_BEST_POSSIBLE
02/07/06   bt      Added srch_rx_prepare_to_sleep.
02/02/06   awj     Added srch_rx_chain_is_ready
10/25/05   awj     Added default chain selection NV support
10/18/05   awj     Removed deprecated type
10/17/05   kwo     Lint Cleanup
09/20/05   bt      Use last valid AGC on home band/channel for SOOS and added
                   srch_rx_get_home_rx_agc.
08/29/05   kwo     Added srch_rx_complete_tune()
08/19/05   bt      Added srch_rx_bypass_rtc_sync.
08/19/05   ejv     Use srch_rx_chan_type for chan references.
08/09/05   grl     Added SOOS algorithm support.
07/19/05   grl     Simplified RF chain initialization API.
06/22/05   sfm     Added several new functions and updated function headers
06/09/05   sfm     Added srch_rx_change_priority()
06/07/05   sfm     removed srch_rx_tune_wait_cb()
06/03/05   sfm     srch_rx naming changes
06/01/05   grl     Added functions to get the current band and channel.
05/31/05   rng     Merged from SHDR Sandbox.
01/05/05   bt      Implementation, first cut.
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "rfm.h"
#include "timetick.h"
#include "sample_server.h"
#include "srch_nv.h"
#include "srch_rx_t.h"
#include "srch_genlog.h"

#ifndef FEATURE_SRCH_HAS_NO_TRM
#include "trm.h"
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

#ifndef FEATURE_SRCH_HAS_NO_TRM
/* Callback function definition for unlock callback events. */
typedef void (*srch_rx_unlock_cb_t)
(
  trm_unlock_event_enum_t event,           /* Event being sent to the client */
  uint32                  unlock_by_sclk   /* Sclk timestamp for UNLOCK_BY*/
);
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/* Callback function definition for RF granted events. */
typedef void (*srch_rx_granted_cb_t)( void );

/* Callback function definition for tune completions. */
typedef void (*srch_rx_tune_cb_t)( void );

/* this is emulating the rf device type provided by rfcom_device_enum_type,
   found in rfcom.h */
typedef enum
{
  SRCH_RF_TRANSCEIVER_0,             /* Primary Radio transceiver */
  SRCH_RF_RECEIVER_1,                /* Secondary Receiver        */
  SRCH_RF_RECEIVER_DIV,              /* Recieve Diversity         */
  SRCH_RF_RECEIVER_DUAL,             /* Independent Rx Chains     */
  SRCH_RF_MAX_DEVICES
}
srch_rx_device_enum_type;

/*-------------------------------------------------------------------------
      Prototypes
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       SRCH_RX_INIT

DESCRIPTION    This function initializes the SRCH_RX module.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_init( void );

/*===========================================================================

FUNCTION      SRCH_RX_COMPLETE_TUNE

DESCRIPTION   This function is called after the tune timer expires
              to complete the tune

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
extern void srch_rx_complete_tune
(
  srch_rx_chain_name_type chain  /* The completed tune's chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_DEVICE

DESCRIPTION    This function returns the RF device for the specified chain.
               RFCOM_RECEIVER_DIV will be returned if diversity is enabled.

DEPENDENCIES   None.

RETURN VALUE   The rf device.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_device_enum_type srch_rx_get_device
(
  srch_rx_chain_name_type chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_SAMPLE_BUFFER

DESCRIPTION    This function returns the sample buffer for the specified
               chain. SAMPLE_SERVER_RX_CHAIN_BOTH is returned if diversity
               is enabled.

DEPENDENCIES   None.

RETURN VALUE   Returns the sample server chain assosiated with the given
               chain name.

SIDE EFFECTS   None.

===========================================================================*/
extern sample_server_rx_chain_type srch_rx_get_sample_buffer
(
  srch_rx_chain_name_type chain_name    /* chain */
);

/*===========================================================================

FUNCTION SRCH_RX_MDSP_PAUSE

DESCRIPTION    This function is designed for subsystems outside of search
               to pause and resume mDSP writes to the RxF rotators for
               any ready chains. This function must be called in pairs,
               first TRUE, then FALSE.

DEPENDENCIES   Note these functions are not thread safe, so if anyone calls
               this function, someone else could still re-enable the writes
               async to these calls.

RETURN VALUE   None.

SIDE EFFECTS   mDSP writes to the RxF rotators may be disabled

===========================================================================*/
extern void srch_rx_mdsp_pause
(
  boolean pause                       /* TRUE = pause mdsp, FALSE = resume */
);

/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_IS_READY

DESCRIPTION    This function determines if the specified chain is ready

DEPENDENCIES   None.

RETURN VALUE   TRUE if specified chain is ready, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_chain_is_ready
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_IS_RXTX

DESCRIPTION    This function determines if the specified chain is RXTX

DEPENDENCIES   None.

RETURN VALUE   True if specified chain is RXTX, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_chain_is_rxtx
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

#ifndef FEATURE_SRCH_SINGLE_ANTENNA
/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_IS_RX_ONLY

DESCRIPTION    This function determines if the specified chain is RX

DEPENDENCIES   None.

RETURN VALUE   True if the specified chain is RX1, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_chain_is_rx_only
(
  srch_rx_chain_name_type  chain_name    /* chain */
);
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

/*===========================================================================

FUNCTION       SRCH_RX_GET_STATUS

DESCRIPTION    This function returns the status of a chain

DEPENDENCIES   None.

RETURN VALUE   Chain status.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_status_type srch_rx_get_status
(
  srch_rx_chain_name_type chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_REQUEST_CHAIN

DESCRIPTION    This function requests a chain from TRM if we don't already
               own it.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the request was granted, FALSE otherwise.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
#ifdef FEATURE_SRCH_HAS_NO_TRM
extern boolean srch_rx_request_chain
(
  srch_rx_chain_name_type  chain_name    /* primary chain */
);
#else /* !FEATURE_SRCH_HAS_NO_TRM */
extern boolean srch_rx_request_chain
(
  srch_rx_chain_name_type  chain_name,   /* primary or secondary chain */
  trm_resource_enum_t      resource,     /* any specific resource */
  timetick_type            duration,     /* in sclks */
  trm_reason_enum_t        reason        /* what chain is to be used for */
);
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*===========================================================================

FUNCTION       SRCH_RX_RELEASE_CHAIN

DESCRIPTION    This function releases a given chain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_release_chain
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_RELEASE_OWNED_CHAINS

DESCRIPTION    This function releases all owned or requested chains.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_release_owned_chains( void );

/*===========================================================================

FUNCTION       SRCH_RX_RESERVE_CHAIN

DESCRIPTION    This function reserves a chain in the future.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.  If an OWNED chain is requested, there
               is an implicit release.

===========================================================================*/
#ifdef FEATURE_SRCH_HAS_NO_TRM
extern void srch_rx_reserve_chain
(
  srch_rx_chain_name_type chain_name    /* chain */
);
#else /* !FEATURE_SRCH_HAS_NO_TRM */
extern void srch_rx_reserve_chain
(
  srch_rx_chain_name_type chain_name,   /* chain */
  trm_resource_enum_t     resource,     /* any specific resource */
  timetick_type           when,         /* sclk timetick point */
  timetick_type           duration,     /* in sclks */
  trm_reason_enum_t       reason        /* what the chain is to be used for */
);
#endif /* FEATURE_SRCH_HAS_NO_TRM */

#ifndef FEATURE_SRCH_HAS_NO_TRM
/*===========================================================================

FUNCTION       SRCH_RX_REQUEST_AND_NOTIFY

DESCRIPTION    This function requests a lock from TRM and notifies the caller
               when it gets granted.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.  If an OWNED chan is requested, there
               is an implicit release.

===========================================================================*/
extern void srch_rx_request_and_notify
(
  srch_rx_chain_name_type chain_name,     /* chain */
  trm_resource_enum_t     resource,       /* The RF resource being requested */
  trm_duration_t          duration,       /* How long it's needed (in sclks) */
  trm_reason_enum_t       reason,         /* what the chain will be used for */
  void                    (*call_back)(void)  /* Callback to notify client */
);

/*===========================================================================

FUNCTION       SRCH_RX_CANCEL_REQUEST

DESCRIPTION    This function cancels any outstanding request_and_notify.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_cancel_request
(
  srch_rx_chain_name_type chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_RETAIN_LOCK

DESCRIPTION    This function retains a lock for a given chain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_retain_lock
(
  srch_rx_chain_name_type    chain_name,    /* chain */
  srch_rx_unlock_cb_t        call_back      /* Callback to notify client */
);

/*===========================================================================

FUNCTION       SRCH_RX_EXTEND_LOCK_DURATION

DESCRIPTION    This function attempts to extend the lock duration for
               a given chain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_extend_lock_duration
(
  srch_rx_chain_name_type  chain_name,   /* chain */
  timetick_type            duration      /* how long needed in sclks */
);

#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*===========================================================================

FUNCTION       SRCH_RX_GET_BAND

DESCRIPTION    Returns the current band for the specified chain

DEPENDENCIES   None.

RETURN VALUE   The current band.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_band_type srch_rx_get_band
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_CHAN

DESCRIPTION    Returns the current channel for the specified chain

DEPENDENCIES   None.

RETURN VALUE   The current channel.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_chan_type srch_rx_get_chan
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_BAND

DESCRIPTION    This function returns the home band

DEPENDENCIES   None.

RETURN VALUE   The home band.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_band_type srch_rx_get_home_band( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_CHAN

DESCRIPTION    This function returns the home channel

DEPENDENCIES   None.

RETURN VALUE   The home channel.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_chan_type srch_rx_get_home_chan( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_CURR_BAND_CHAN

DESCRIPTION    This function returns the home band and channel

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_get_curr_band_chan
(
  srch_rx_chain_name_type  chain_name,    /* chain */
  srch_rx_band_type       *band,          /* returned band class */
  srch_rx_chan_type       *channel        /* returned channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_ENTER_MODE

DESCRIPTION    This function performs the rfm_enter_mode for the given chain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_enter_mode( srch_rx_chain_name_type chain );

#ifndef FEATURE_SRCH_SINGLE_ANTENNA
/*===========================================================================

FUNCTION       SRCH_RX_ENABLE_RF_FOR_DIV

DESCRIPTION    This function enables the RF for diversity.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_enable_rf_for_div
(
  void (*enable_cb)(void)      /* callback for enable done */
);
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

/*===========================================================================

FUNCTION       SRCH_RX_COMPLETE_WARMUP

DESCRIPTION    This function will call the handle the complete_warmup
               for TXRX0 if that is the chain owned.

DEPENDENCIES   srch_rx_init_and_tune must have been called with
               complete_warmup set to false.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_complete_warmup( void );

/*===========================================================================

FUNCTION       SRCH_RX_COMPLETE_SLEEP_WARMUP

DESCRIPTION    This function must be called after the RTC is on when
               srch_rx_init_and_tune is called with complete set to FALSE.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_complete_sleep_warmup
(
  srch_rx_chain_name_type  chain_name     /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_DISABLE_RX

DESCRIPTION    Disables RF for the specified chain

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_disable_rx
(
  srch_rx_chain_name_type chain_name     /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_PREPARE_TO_SLEEP

DESCRIPTION    This function informs the RF driver that we are ready to
               give up the RF.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_prepare_to_sleep
(
  srch_rx_chain_name_type chain_name     /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_TUNE

DESCRIPTION    This function tunes the specified chain to the band and channel
               requested. It also tunes successfully when diversity is turned
               on.  It will inform the user via a callback once the tune has
               completed.

               The currently owned receiver will be automatically enabled
               if necessary.  The sample server will be started in continuous
               mode.  The sample ram will be flushed if the demod chain is
               tuned.

DEPENDENCIES   The chain_name must be associated with an RF chain owned
               by 1x.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_tune
(
  srch_rx_chain_name_type  chain,                /* chain */
  srch_rx_band_type        band,                 /* band class */
  srch_rx_chan_type        channel,              /* channel number */
  srch_rx_tune_cb_t        tune_cb               /* callback for tune_done */
);

/*===========================================================================

FUNCTION       SRCH_RX_INIT_AND_TUNE

DESCRIPTION    This function initializes the RF and tunes the radio.  Note
               that this call is only necessary when initializing the RF for
               the sleep timeline where complete, flush_sample_ram, or
               start_sample_server may be false. srch_rx_tune should be called in
               any other circumstances whether the RF is enabled or disabled.

               This function only supports synchronous operation.

               If complete is set to FALSE, srch_rx_complete_sleep_warmup must
               be called after the RTC is on. srch_rx_complete_sleep_warmup
               shall not be called in any other circumstance.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_init_and_tune
(
  srch_rx_chain_name_type  chain,                /* chain */
  srch_rx_band_type        band,                 /* band class */
  srch_rx_chan_type        channel,              /* channel number */
  boolean                  complete,             /* Turn on AGC loops?. */
  boolean                  flush_sample_ram,     /* Flush sample RAM? */
  boolean                  start_sample_server,  /* Start sample server? */
  boolean                  bypass_rtc_sync       /* Bypass RTC SYNC? */
);

/*===========================================================================

FUNCTION       SRCH_RX_TUNE_TO_HOME

DESCRIPTION    This function tunes the specified chain to the home band and
               channel. It also tunes successfully when diversity is turned
               on. It will inform the user via a callback once the tune has
               completed.

               The currently owned receiver will be automatically enabled
               if necessary. The sample server will be started in continuous
               mode. The sample ram will be flushed if the demod chain is
               tuned.

DEPENDENCIES   The chain_name must be associated with a RF chain owned
               by 1x.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_tune_to_home
(
  srch_rx_chain_name_type  chain_name,           /* chain to tune to */
  srch_rx_tune_cb_t        tune_cb               /* callback for tune_done */
);

/*===========================================================================

FUNCTION       SRCH_RX_TUNE_AND_WAIT

DESCRIPTION    This function tunes the specified chain to the band and channel
               requested. It also tunes successfully when diversity is turned
               on.  It will inform the user via a callback once the tune has
               completed.

               The currently owned receiver will be automatically enabled
               if necessary.  The sample server will be started in continuous
               mode.  The sample ram will be flushed if the demod chain is
               tuned.

               This is a synchronous call that will return after the RF is
               tuned and ready.

DEPENDENCIES   The chain_name must be associated with a RF chain owned
               by 1x.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_tune_and_wait
(
  srch_rx_chain_name_type chain_name,        /* chain to tune to */
  srch_rx_band_type       band,              /* band class */
  srch_rx_chan_type       channel            /* channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_TUNE_TO_HOME_AND_WAIT

DESCRIPTION    This function tunes the specified chain to the home band and
               channel. It also tunes successfully when diversity is turned
               on. It will inform the user via a callback once the tune has
               completed.

               The currently owned receiver will be automatically enabled
               if necessary. The sample server will be started in continuous
               mode. The sample ram will be flushed if the demod chain is
               tuned.

               This is a synchronous call that will return after the RF is
               tuned and ready.

DEPENDENCIES   The chain_name must be associated with a RF chain owned
               by 1x.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_tune_to_home_and_wait
(
  srch_rx_chain_name_type chain_name        /* chain to tune to */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_FREQ_RANGE

DESCRIPTION    This function returns the frequency range corresponding to
               a given band class.

DEPENDENCIES   None.

RETURN VALUE   Frequency range.

SIDE EFFECTS   none

===========================================================================*/
extern srch_rx_freq_range_type srch_rx_get_freq_range
(
  srch_rx_band_type band_class  /* Band class */
);

/*===========================================================================

FUNCTION       SRCH_RX_FAST_AGC_LOOP

DESCRIPTION    This function sets the AGC loop time constant for the chain(s)
               in use.  This function should be called after the tune since
               the act of tuning will automatically switch the AGC loop back
               into normal mode.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_fast_agc_loop
(
  srch_rx_chain_name_type  chain_name,   /* chain */
  boolean                  enable        /* enable fast agc or not */
);

/*===========================================================================

FUNCTION       SRCH_RX_BYPASS_RTC_SYNC

DESCRIPTION    This function turns off/on the rtc synchronization bypass and
               flushed the sample RAM if required.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_bypass_rtc_sync
(
  srch_rx_bypass_rtc_sync_type    bypass_rtc_sync,
  srch_rx_flush_samp_ram_type     flush_samp_ram,
  srch_rx_chain_name_type         chain_name
);

/*===========================================================================

FUNCTION       SRCH_RX_TUNED

DESCRIPTION    This function determines if 1X has an active antenna that
               is tuned and ready to demodulate.

DEPENDENCIES   None.

RETURN VALUE   True if we are active on an antenna, otherwise FALSE.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_tuned
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_OWNS_CHAIN

DESCRIPTION    This function determines if 1X owns a chain associated with
               the specified chain_name.

DEPENDENCIES   None.

RETURN VALUE   True if we own an antenna, otherwise FALSE.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_owns_chain
(
  srch_rx_chain_name_type  chain_name    /* chain */
);

#ifndef FEATURE_SRCH_SINGLE_ANTENNA
/*===========================================================================

FUNCTION       SRCH_RX_EXCHANGE_DEVICES

DESCRIPTION    This function exchanges the devices used by the
               DEMOD and ALT chains.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_exchange_devices
(
 boolean fing_setup
);
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

#ifndef FEATURE_SRCH_HAS_NO_TRM
/*===========================================================================

FUNCTION       SRCH_RX_CHANGE_PRIORITY

DESCRIPTION    This function notifies TRM that the reason for holding a chain
               has changed.  TRM will modifify the clients priority as needed.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_change_priority
(
  srch_rx_chain_name_type  chain_name,   /* chain name */
  trm_reason_enum_t        reason        /* reason for changing the priority */
);
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*===========================================================================

FUNCTION       SRCH_RX_GET_RX_AGC

DESCRIPTION    This function gets the AGC for the specified receive chain

DEPENDENCIES   None.

RETURN VALUE   AGC for the specified receive chain.

SIDE EFFECTS   None.

===========================================================================*/
extern int8 srch_rx_get_rx_agc
(
  srch_rx_chain_name_type  chain_name   /* chain name */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_RX_AGC

DESCRIPTION    This function gets the AGC for the specified receive chain
               when it is or was last tuned to the home band/channel

DEPENDENCIES   None.

RETURN VALUE   AGC for the demod chain when it is or was last tuned to the
               home band/channel.

SIDE EFFECTS   None.

===========================================================================*/
extern int8 srch_rx_get_home_rx_agc(void);

/*===========================================================================

FUNCTION       SRCH_RX_IS_TUNED_HOME

DESCRIPTION    This function determines whether we are tuned to the home band
               and channel.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the specified chain is already tuned to the given
               band class and channel.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_tuned_home
(
  srch_rx_chain_name_type chain_name    /* chain name */
);

/*===========================================================================

FUNCTION       SRCH_RX_IS_BAND_CHAN

DESCRIPTION    This function determines if the band class and channel for a
               specified chain match the passed in parameters.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the band class and channel match the parameters.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_band_chan
(
  srch_rx_chain_name_type chain_name,    /* chain name */
  srch_rx_band_type       band,          /* band_class */
  srch_rx_chan_type       channel        /* channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_TX_PWR_LIMIT

DESCRIPTION    This function returns the CDMA Tx power limit setting in 10 bit
               binary format.  The returned value is interpreted as follows:

               Returned value               0........1023
               Tx power limit          -52.25........+31dBm
               Tx power limit register   +511........-512

DEPENDENCIES   None.

RETURN VALUE   10 bit Tx power limit value.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_rx_get_tx_pwr_limit( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_TX_AGC

DESCRIPTION    This function returns the CDMA Tx AGC setting in 10 bit
               binary format.

DEPENDENCIES   None.

RETURN VALUE   10 bit Tx AGC value.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_rx_get_tx_agc( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_TX_GAIN_ADJ

DESCRIPTION    This function returns the 9 bit sign extended TX Gain Adjust
               register value.

DEPENDENCIES   None.

RETURN VALUE   9 bit sign extended TX Gain Adjust register value.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_rx_get_tx_gain_adj( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_TX_PWR

DESCRIPTION    This function returns the CDMA TX power in dBm in ~1/10 dB units.

DEPENDENCIES   None.

RETURN VALUE   CDMA TX power in dBm in ~1/10 dB units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_rx_get_tx_pwr( void );

#ifndef FEATURE_SRCH_HAS_NO_TRM
/*========================================================================

FUNCTION SRCH_RX_GET_WAKEUP_CHAIN

DESCRIPTION    This function returns the TRM chain resource that is required
               based on the last RX AGC reading.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern trm_resource_enum_t srch_rx_get_wakeup_chain( void );

/*========================================================================

FUNCTION SRCH_RX_GET_RESERVED_CHAIN

DESCRIPTION    This function returns the TRM chain that was last reserved.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern trm_resource_enum_t srch_rx_get_reserved_chain( void );

#endif /* FEATURE_SRCH_HAS_NO_TRM */

#ifdef FEATURE_SOOS
/*========================================================================

FUNCTION SRCH_RX_SET_RX_CHAIN_SEL_THRESH

DESCRIPTION    This function writes the RX chain selection thresholds that
               are read from NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_set_rx_chain_sel_thresh.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern void srch_rx_set_rx_chain_sel_thresh
(
  int16                 upper_thresh, /* Above, transition to any RX chain */
  int16                 lower_thresh  /* Below, transition to best RX chain */
);

/*========================================================================

FUNCTION SRCH_RX_DET_WAKEUP_CHAIN

DESCRIPTION    This function determines the next wakeup chain based on
               the current RX AGC and the current demod chain.

DEPENDENCIES   None.

RETURN VALUE   RF AGC of the demod chain when it is/was tuned to
               home band/channel.

SIDE EFFECTS   None.

========================================================================*/
extern int16 srch_rx_det_wakeup_chain(void);

#endif /* FEATURE_SOOS */

#ifdef FEATURE_SRCH_IS_FTS_CAPABLE /* FTS 3 */
/*========================================================================

FUNCTION SRCH_RX_REQUEST_AND_NOTIFY_LAST_FAILED

DESCRIPTION    This function makes a call to srch_rx_request_and_notify
                for the last failed chain, resource and reason.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern void srch_rx_request_and_notify_last_failed
(
 trm_duration_t          duration,       /* How long it's needed (in sclks) */
 srch_rx_granted_cb_t    granted_cb      /* Callback to notify client */
 );

/*========================================================================

FUNCTION SRCH_RX_GET_ALT_CHAIN_DEMOD_REASON

DESCRIPTION    This function returns the appropriate DEMOD reason for
                ALT_CHAIN depending on whether maximum sensitivy is needed
                or not

DEPENDENCIES   None.

RETURN VALUE   TRM Request Reason.

SIDE EFFECTS   None.

========================================================================*/
extern trm_reason_enum_t srch_rx_get_alt_chain_demod_reason(void);

/*========================================================================

FUNCTION SRCH_RX_SET_MAX_SENSE_MODE

DESCRIPTION    This function sets the maximum sensitivity mode internally
                and if it is set to TRUE, the calls the RF API to flip the
                required GPIO switch

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   RF API to flip the GPIO switch which connects secondary
                chain to primary antenna

========================================================================*/
extern void srch_rx_set_max_sense_mode( boolean max_sense_mode );

/*========================================================================

FUNCTION SRCH_RX_GET_MAX_SENSE_MODE

DESCRIPTION    This function returns the current maximum sensitivity mode
                which indicates whether secondary chain is using primary
                antenna or not

DEPENDENCIES   None

RETURN VALUE   TRUE if max_sensitivity_mode is set

SIDE EFFECTS   None

========================================================================*/
extern boolean srch_rx_get_max_sense_mode( void );

/*========================================================================

FUNCTION SRCH_RX_REQUEST_BOTH_CHAINS

DESCRIPTION    This function, based on SOOS threshold conditions, requests
                both primary and secondary chains with appropriate reason.

DEPENDENCIES   None

RETURN VALUE   TRUE if requests resulted in a viable antenna-chain
                combination.

SIDE EFFECTS   None

========================================================================*/
extern boolean srch_rx_request_both_chains
(
  timetick_type duration,
  boolean       orig_pending
);

/*========================================================================

FUNCTION SRCH_RX_RESERVE_BOTH_CHAINS

DESCRIPTION    This function, based on SOOS threshold conditions, reserves
                both primary and secondary chains with appropriate reason.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

========================================================================*/
extern void srch_rx_reserve_both_chains
(
 timetick_type when,
 timetick_type duration
);

#ifdef FEATURE_SRCH_FTS_BAND_CONFIGURABILITY
/*========================================================================

FUNCTION SRCH_RX_IS_BAND_TUNABLE

DESCRIPTION    In FTS, return if the current antenna-chain resources that
                1X has a lock on can be tuned to the given band.

DEPENDENCIES   None

RETURN VALUE   TRUE if antenna is tunable for band class

SIDE EFFECTS   None

========================================================================*/
extern boolean srch_rx_is_band_tunable
(
 srch_rx_band_type       band
);

/*========================================================================

FUNCTION SRCH_RX_SET_MAX_SENSE_REQUIRED

DESCRIPTION    Sets whether a subsequent request for DEMOD chain should be
                made with maximum sensitivity reason or not

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Subsequent request for DEMOD chain will be made with
                MAX_SENSE reason if set to TRUE

========================================================================*/
extern void srch_rx_set_max_sense_required ( boolean max_sense_required );

/*========================================================================

FUNCTION SRCH_RX_GET_MAX_SENSE_REQUIRED

DESCRIPTION    Returns whether max sensitivity requirement has been
                enabled or not

DEPENDENCIES   None

RETURN VALUE   TRUE if max sense requirement has been set

SIDE EFFECTS   None

========================================================================*/
extern boolean srch_rx_get_max_sense_required ( void );

#endif  /* FEATURE_SRCH_FTS_BAND_CONFIGURABILTY */

#endif /* FEATURE_SRCH_IS_FTS_CAPABLE */

#ifndef FEATURE_SRCH_HAS_NO_TRM
/*===========================================================================

FUNCTION SRCH_RX_SET_RX_CHAIN_TRANS

DESCRIPTION    This function writes the RX chain translation as set in NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_set_rx_chain_trans.

RETURN VALUE   None.

SIDE EFFECTS   Updates static structure with trm_resource_enum_t translation
               of TRM_RX_ANY.

===========================================================================*/
extern void srch_rx_set_rx_chain_trans
(
  srch_nv_dbg_mask_type    rx_chain_trans,
  trm_resource_enum_t      request_type
);

/*===========================================================================

FUNCTION SRCH_RX_GET_RX_CHAIN_TRANS

DESCRIPTION    This function reads the RX chain translation as set in NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_get_rx_chain_trans.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern trm_resource_enum_t srch_rx_get_rx_chain_trans
(
  trm_resource_enum_t   request_type
);

#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*===========================================================================

FUNCTION srch_rx_build_rf_sub

DESCRIPTION
  This function builds the rf sub packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_rx_build_rf_sub
(
  srch_genlog_packet_id_type   id,       /* id of packet to commit sub */
  sub_commit_func_type         commit_fp /* function to call to commit the
                                            subpacket */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_RF_WARMUP_TIME

DESCRIPTION    This function returns the RF warmup base + overhead time in us.

DEPENDENCIES   Must be called after rfm_enter_mode().

RETURN VALUE   The rf warmup base + overhead time in us.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_rx_get_rf_warmup_time ( void );

#endif /* SRCH_RX_H */
