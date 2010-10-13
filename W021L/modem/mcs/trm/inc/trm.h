#ifndef TRM_H
#define TRM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

                   Transceiver Resource Manager Header File

GENERAL DESCRIPTION

  This file provides management of the RF Rx/Tx resources


EXTERNALIZED FUNCTIONS

  trm_init() - Must be called before any other function, to initialize module.


  trm_reserve_at() - Registers a client's need to access the RF at a specific
    moment in the future for the indicated duration, such as for a paging
    slot.  trm_request() must be used at the specified moment, to actually
    obtain the RF resource.  Implicitly performs a trm_release().

  trm_request() - Request access to an RF chain immediately, for the indicated
    duration.  Implicitly performs a trm_release().

  trm_request_and_notify() - Request access to an RF chain, when it becomes
    available.  Implicitly performs a trm_release().


  trm_extend_duration() - Requests an increase in the duration the RF is
    held for by the client.  If the increase cannot be granted, the original
    grant duration remains in effect.

  trm_change_duration() - Requests a change in the duration the RF is held for
    by the client.  If the full extension cannot be granted, a partial
    extension may result.

  trm_change_priority() - Specifies a change in the reason (and consequently
    priority) for a client's hold on an RF resource.

  trm_exchange() - Swaps the resources that two clients hold.  This is useful
    when the primary client holds the secondary RF chain, and the secondary
    client acquires the primary RF chain.  After this function is called, the
    primary client will hold the primary RF chain, and the secondary client
    will hold the secondary RF chain.

  trm_release() - Releases the clients hold on an RF resource.  If no
    client is waiting for the RF resource, the RF will be turned off.


  trm_get_granted() - This function indicates if a client holds an RF
    resource.  Ideally, the client should already know and maintain knowledge
    of whether or not it holds an RF resource.

  trm_get_rf_device() - Returns the identifier associated with the granted
    RF chain.

  trm_get_1x_mode() - Return the 1X mode


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  trm_init() must be called before any other function.

  Before using any RF functions, a client must be granted permission via
  trm_request( ) or trm_request_and_notify().

  When the client is finished using the RF, it must release its hold on the
  RF chain either directly via a call to trm_release(), or indirectly by
  calling trm_reserve_at(), trm_request_and_notify(), or trm_request().


Copyright (c) 2004 - 2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/trm/main/latest/inc/trm.h#5 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
09/18/2008   rkc     Added TRM_DEMOD_PAGE_CONTINUATION to trm_reason_enum_t
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
06/06/2008   sg      Added TRM_HP_BROADCAST in trm_reason_enum_t
05/20/2008   sg      Consolidated new NV items into new 1x_mode enum/API
03/18/2008   sg      Remove FTS-3 Featurization from num_antennas/1x SecondaryOnly 
                     functions
02/11/2008   sg      Added NV fields for num antennas and 1x SecondaryOnly mode 
01/24/2008   sg      Added Full Time SHDR-3 support
07/11/2007   cab     Added trm_timestamp_t
10/17/2006   cab     Added TRM_DEMOD_SMALL_SCI_PAGE reason.
09/20/2006   cab     Integrated UMTS
09/20/2006   cab     Added include for customer.h
07/28/2006   jwob    Added #error for !FEATURE_TRM_API_V2
07/13/2006   jwob    Tag parameter to trm_request_and_notify and callback
07/12/2006   jwob    Removed trm_cancel_request_and_notify
05/26/2006   cab     Updated trm_extend_duration comment
05/02/2006   cab     Added ifdef's for offline compilation and testing
12/16/2005   cab     Added some comments 
12/12/2005   grl     Corrected enum to preserve backward compatibility with 
                     TRM log parsers.
11/02/2005   cab     Added BEST_POSSIBLE resource option - pre-empts lower
                     pri clients from chain 0, but takes chain 1 if higher 
                     pri clients have chain 0
09/20/2005   grl     Made the last supported reason TRM_LOW_LAT_TRAFFIC.
09/10/2005   grl     Added broadcast access/traffic reasons.
08/29/2005   grl     Added support for dynamic compatibility masks and unlock
                     immediate in the unlock callback.
06/29/2005   ajn     Added typedef for trm_init's wait callback.
05/26/2005   ajn     Code review comment changes
03/28/2005   ajn     Added GPS_TDM reason.
03/16/2005   ajn     Added get_rf_config.
03/01/2005   ajn     Timestamp to unlock callback.
01/07/2005   ajn     Removed GRANTED_0_GIVE_1.  Use trm_exchange() instead.
12/29/2004   ajn     Moved RFM_* helpers to trm_rf.h
11/12/2004   ajn     Initial AMSS version

============================================================================*/

#include "customer.h"

#ifdef FEATURE_MCS_TRM

/* This is not a true "feature," but to prevent picking up these changes
   too soon. Srch/1x, GPS, and HDR must coordinate first. */
#ifndef FEATURE_TRM_API_V2
#error "You must use FEATURE_TRM_API_V2 (see comment in trm.h)"
#endif


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#ifdef TRM_OFFLINE
  #include <stdlib.h>
  #include "trm_offline.h"
#else
  #include "customer.h"
  #include "comdef.h"
  #include "rex.h"
  #include "timetick.h"
  #include "rfcom.h"
#endif


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/


/*----------------------------------------------------------------------------
  Transceiver resource clients (who?)
    These represent the possible owners of the Rx/Tx chains.

    TRM_xxx_SECONDARY clients are intended for use with the secondary RF chain
    for diversity or for off-frequency searching without tuning the primary
    RF chain away from the active frequency.

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

----------------------------------------------------------------------------*/

typedef enum
{           
  TRM_1X,
  TRM_1X_SECONDARY,

  TRM_HDR,
  TRM_HDR_SECONDARY,

  TRM_GPS,

  TRM_UMTS,
  TRM_UMTS_SECONDARY,

  /* For internal bound-checking and array-sizing */
  TRM_LAST_CLIENT                 = TRM_UMTS_SECONDARY
}
trm_client_enum_t;



/*----------------------------------------------------------------------------
  Transceiver resource requests (what?)
    These represent what the clients can ask for.

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

    Note: Always update trm_chains array when modifying this enum

----------------------------------------------------------------------------*/

typedef enum
{
  /* Any available Rx chain */
  TRM_RX_ANY,

  /* ... with the highest sensitivity, nothing if not available */
  TRM_RX_BEST,

  /* The secondary receive chain */
  TRM_RX_SECONDARY,

  /* The receive chain associated with GPS antenna */
  TRM_RX_GPS,

  /* Any RF chain with both Rx and Tx capability */
  TRM_RXTX_ANY,

  /* The RXTX RF chain the best sensitivity, nothing if not available */
  TRM_RXTX_BEST,

  /* ... with the highest sensitivity, or lower if not available */
  TRM_RX_BEST_POSSIBLE,

  /* The RXTX RF chain the best sensitivity, or lower if not available */
  TRM_RXTX_BEST_POSSIBLE,

  /* For internal bound-checking and array-sizing */
  TRM_LAST_RESOURCE               = TRM_RXTX_BEST_POSSIBLE
}
trm_resource_enum_t;



/*----------------------------------------------------------------------------
  Transceiver resource request times (when?)
    When a client needs a resource is composed of two parts:
    1) A start time, when the resource is needed (could be ASAP).
    2) The duration the resource is needed for.
----------------------------------------------------------------------------*/

/* Timestamp type used by TRM (sclks)  See: timetick_get() */
typedef timetick_type             trm_time_t;


/* Time duration type used by TRM (sclks) */
typedef timetick_type             trm_duration_t;

/* 64 bit timestamp for increased range*/
typedef uint64                    trm_timestamp_t;

/*----------------------------------------------------------------------------
  Transceiver resource request reasons (why?)
    Is used, with the requesting client, to determine the priority of the
    request.

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

----------------------------------------------------------------------------*/

typedef enum
{
  /* Perform general access attempt */
  TRM_ACCESS,

  /* Perform access as soon as possible. TRM asks retain lock holders to
     release the lock immediately. */
  TRM_ACCESS_URGENT,

  /* Access attempt while in broadcast */
  TRM_BROADCAST_ACCESS,

  /* Determining the mobile location */
  TRM_LOCATION_FIX,

  /* In a traffic call */
  TRM_TRAFFIC,

  /* Monitoring paging channel */
  TRM_DEMOD_PAGE,

  /* Monitoring Broadcast page/data  */
  TRM_DEMOD_BROADCAST,

  /* Monitoring QPCH bits */
  TRM_DEMOD_QPCH,

  /* Demodulation of the GPS signal */
  TRM_DEMOD_GPS,

  /* Acquiring or reacquiring the system  */
  TRM_ACQUISITION,

  /* Off Frequency Search */
  TRM_OFS,

  /* Diversity */
  TRM_DIVERSITY,

  /* GPS in time-diversity-multiplex mode */
  TRM_GPS_TDM,

  /* In a traffic call during with broadcast on */
  TRM_BROADCAST_TRAFFIC,

  /* Traffic which requires low latency such as IP based video telephony */
  TRM_LOW_LAT_TRAFFIC,

  /* UMTS requests and holds the primary chain indefinitely */
  TRM_TOTAL_CONTROL,

  /* Monitoring paging channel with a small SCI */
  TRM_SMALL_SCI_PAGE,

  /* Monitoring with Max Sensitivity */
  TRM_DEMOD_PAGE_MAX_SENS,

  /* High priority BCMCS */
  TRM_HP_BROADCAST,

  /* Continuation of monitoring paging channel */
  TRM_DEMOD_PAGE_CONTINUATION,

  /* For internal bound-checking and array-sizing */
  TRM_NUM_REASONS
}
trm_reason_enum_t;



/*----------------------------------------------------------------------------
  Request results (what happened?)
    Returned by trm_request or passed as an argument to the
    trm_request_and_notify callback.

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

----------------------------------------------------------------------------*/

typedef enum
{
  /* Sorry, you can't have the requested resource. */
  TRM_DENIED,

  /* You have been granted usage of the primary RF chain */
  TRM_GRANTED_CHAIN0,

  /* You have been granted usage of the secondary RF chain */
  TRM_GRANTED_CHAIN1
}
trm_grant_event_enum_t;



/*----------------------------------------------------------------------------
  Unlock request events
    A higher priority client needs the resource you hold ...

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

----------------------------------------------------------------------------*/

typedef enum
{
  /* Unlock request cancelled.  You no-longer need to unlock. */
  TRM_UNLOCK_CANCELLED,

  /* ... at the indicated time.  You can hold the lock until then. */
  TRM_UNLOCK_BY,

  /* ... as soon as possible.  Release the lock when convenient, but soon. */
  TRM_UNLOCK_REQUIRED,

  /* ... immediately for a page response!  Give it up ASAP! */
  TRM_UNLOCK_IMMEDIATELY
}
trm_unlock_event_enum_t;



/*----------------------------------------------------------------------------
  RF device corresponding to RF chain owned by TRM lock
----------------------------------------------------------------------------*/

/* Identifer representing "no RF chain is owned" */
#define TRM_NO_DEVICE             RFCOM_MAX_DEVICES



/*----------------------------------------------------------------------------
  RF configurations

    Note: Always add to the end in order to preserve backward compatibility
          with the TRM log parsers.

----------------------------------------------------------------------------*/

typedef enum
{
  /* Single RF chain */
  TRM_RF_SINGLE,

  /* Dual RF chain - both must tune to the same frequency */
  TRM_RF_DIVERSITY,

  /* Dual RF chain - independently tunable - unequal sensitivity */
  TRM_RF_UNEQUAL_SENSITIVITY,

  /* Dual RF chain - independently tunable - equal sensitivity */
  TRM_RF_EQUAL_SENSITIVITY
}
trm_rf_enum_t;



/*----------------------------------------------------------------------------
  TRM Modes which can be enabled/disabled
----------------------------------------------------------------------------*/

typedef enum
{
  /* Simultaneous 1x idle with HDR traffic mode */
  TRM_MODE_SIMUL_1XIDLE_HDRTC
}
trm_mode_enum_t;


/*----------------------------------------------------------------------------
  TRM 1X Modes
----------------------------------------------------------------------------*/

typedef enum
{
  /* Standard 1X operation */
  TRM_1X_MODE_NORMAL,

  /* Require 1x to operate on primary antenna in SOODA operation mode  */
  TRM_1X_MODE_FORCE_SOODA,

  /* 1X should only run on the secondary chain */
  TRM_1X_MODE_SECONDARY_ONLY,

  /* Undefined operation */
  TRM_1X_MODE_UNDEFINED
}
trm_1x_mode_enum_t;



/*----------------------------------------------------------------------------
  Payload for trm_request_and_notify() / (*trm_grant_callback_t)() pair
----------------------------------------------------------------------------*/

typedef uint32                    trm_request_tag_t;


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/



/*============================================================================

CALLBACK TRM_WAIT_CALLBACK_T

DESCRIPTION
  The prototype for a rex_wait type callback function.

  The callback may add additional signals to the wait_mask, and process
  those signals before returning.  For example, the callback may watch for
  dog report timer expiry signals, and report to the dog task in response
  to that signal.

  The callback may actually be "(trm_wait_callback_t) rex_wait".
  
DEPENDENCIES
  The callback will be called by trm_init when waiting for a read from NV.
  It will be called from the context of the task calling trm_init.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_wait_callback_t)
(
  /* The signal to wait for */
  rex_sigs_type                   wait_mask
);



/*============================================================================

FUNCTION TRM_INIT

DESCRIPTION
  Initialize the Transceiver Resource Manager
  
DEPENDENCIES
  Must be called before any other TRM function.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  * tcb,

  /* Signal to wait for, for NV reading */
  rex_sigs_type                   sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  trm_wait_callback_t             wait
);



/*============================================================================

FUNCTION TRM_GET_RF_CONFIG

DESCRIPTION
  Return the RF configuration
  
DEPENDENCIES
  trm_init( ) must have been called.

RETURN VALUE
  The RF configuration in use

SIDE EFFECTS
  None

============================================================================*/

trm_rf_enum_t trm_get_rf_config( void );



/*============================================================================

FUNCTION TRM_RESERVE_AT

DESCRIPTION
  Specifies the given client needs the given transceiver resource at the given
  time, for the given duration, for the supplied reason.

  This would be used with the "trm_request()" function, below.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.

============================================================================*/

void trm_reserve_at
(
  /* The client which needs the RF resource */
  trm_client_enum_t               client,

  /* The RF resource which is being requested */
  trm_resource_enum_t             resource,

  /* When the resource will be needed (sclks timestamp) */
  trm_time_t                      when,

  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,

  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason
);



/*============================================================================

CALLBACK TRM_GRANT_CALLBACK_T

DESCRIPTION
  The prototype for event callback functions, used by the Transceiver manager
  to inform the clients of transceiver management events.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_grant_callback_t)
(
  /* The client which is being informed of an event */
  trm_client_enum_t               client,

  /* The event being sent to the client */
  trm_grant_event_enum_t          event,

  /* Anonymous payload echoed from trm_request_and_notify() */
  trm_request_tag_t               tag
);



/*============================================================================

FUNCTION TRM_REQUEST_AND_NOTIFY

DESCRIPTION
  Specifies the given client needs the given transceiver resource, for the
  given duration, for the supplied reason.

  When the resource can be granted to the client, the event callback for the
  client will be called with the result of the lock request.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.

============================================================================*/

void trm_request_and_notify
(
  /* The client which needs the transceiver resource */
  trm_client_enum_t               client,

  /* The RF resource which is being requested */
  trm_resource_enum_t             resource,

  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,

  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason,

  /* Callback to notify client when resource is granted */
  trm_grant_callback_t            grant_callback,

  /* Anonymous payload to be echoed through grant_callback() */
  trm_request_tag_t               tag
);



/*============================================================================

FUNCTION TRM_REQUEST

DESCRIPTION
  Specifies the given client needs the given transciever resource, for the
  given duration, for the supplied reason.

  The resource request is immediately evaluated, and the result returned.

  This may be used in conjunction with trm_reserve_at().
  
DEPENDENCIES
  None

RETURN VALUE
  TRM_DENIED          - the client was denied the transceiver resources.
  TRM_GRANTED_CHAIN0  - the client can now use the primary RF chain.
  TRM_GRANTED_CHAIN1  - the client can now use the secondary RF chain.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.

============================================================================*/

trm_grant_event_enum_t trm_request
(
  /* The client which needs the transceiver resource */
  trm_client_enum_t               client,

  /* The transceiver resource which is being requested */
  trm_resource_enum_t             resource,

  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,

  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason
);



/*============================================================================

FUNCTION TRM_RELEASE

DESCRIPTION
  Release the transceiver resource currently held by a client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If no client is waiting for the resource, the RF chain will be turned off.

============================================================================*/

void trm_release
(
  /* The client which is releasing the resource */
  trm_client_enum_t               client
);



/*============================================================================

FUNCTION TRM_EXCHANGE

DESCRIPTION
  Swaps the resources held by two clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_exchange
(
  /* The first client */
  trm_client_enum_t               client_1,

  /* The second client */
  trm_client_enum_t               client_2
);



/*============================================================================

CALLBACK TRM_UNLOCK_CALLBACK_T

DESCRIPTION
  The prototype for unlock event callback functions, used by the Transceiver
  Resource Manager to inform the clients of when it should unlock a resource
  it holds.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_unlock_callback_t)
(
  /* The client which is being informed of an event */
  trm_client_enum_t               client,

  /* The event being sent to the client */
  trm_unlock_event_enum_t         event,

  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk
);



/*============================================================================

FUNCTION TRM_RETAIN_LOCK

DESCRIPTION
  Informs the Transceiver Resource Manager that the client wants to hold
  the resource indefinitely.  The TRM may inform the client that it must
  give up the lock through the supplied unlock callback.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_retain_lock
(
  /* The client which is attempting to extend the lock duration */
  trm_client_enum_t               client,

  /* The required sclk extension, from "now" */
  trm_unlock_callback_t           unlock_callback
);



/*============================================================================

FUNCTION TRM_EXTEND_DURATION

DESCRIPTION
  Attempts to extend the duration a transceiver resource lock is held for.
  If the entire extension can be granted, it will be granted.
  If the entire extension cannot be granted, the lock duration remains
  unchanged, and the client should release the lock at the original lock
  expiry point.

  If the client had originally locked the resource for longer than the
  required extension, the lock will remain the original length
  and the extension will be INDICATED AS BEING GRANTED.

  The extension is all or nothing.  If a partial extension is desired, the
  trm_change_duration( ) function should be used.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  TRUE if the lock duration extends from "now" to "now + duration".
  FALSE if the lock duration is unchanged.

SIDE EFFECTS
  None

============================================================================*/

boolean trm_extend_duration
(
  /* The client which is attempting to extend the lock duration */
  trm_client_enum_t               client,

  /* The required sclk extension, from "now" */
  trm_duration_t                  duration
);



/*============================================================================

FUNCTION TRM_CHANGE_DURATION

DESCRIPTION
  Attempts to extend the duration a transceiver resource lock is held for.
  The lock will be extended as long as possible, but not greater than the
  given maximum.  If minimum is not TRM_DONT_SHORTEN, the lock duration
  may actually be shortened if a higher priority client is waiting for
  the resource.
  
DEPENDENCIES
  The client must be holding an transceiver resource lock

RETURN VALUE
  The new lock duration, from "now", in sclks.

SIDE EFFECTS
  None

============================================================================*/

/* Don't shorten the lock duration - only extend it, if possible */
#define TRM_DONT_SHORTEN          0xFFFFFFFFuL

trm_duration_t trm_change_duration
(
  /* The client which is attempting to alter the lock duration */
  trm_client_enum_t               client,

  /* The minimum time, from now, that is acceptable (or TRM_DONT_SHORTEN) */
  trm_duration_t                  minimum,

  /* The maximum duration, from now, the lock may be extended for. */
  trm_duration_t                  maximum
);



/*============================================================================

FUNCTION TRM_CHANGE_PRIORITY

DESCRIPTION
  When a client changes what it is doing, it should change the advertised
  reason for holding the transceiver resource, so its priority will change.

  Eg) A client request the transceiver resource for listening for a PAGE.  If
  it receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually change its priority to TRAFFIC.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_change_priority
(
  /* The client whose priority is to be changed */
  trm_client_enum_t               client,

  /* The new reason why the RF lock is held (used for priority decisions) */
  trm_reason_enum_t               reason
);



/*============================================================================

FUNCTION TRM_GET_GRANTED

DESCRIPTION
  Get the RF chain which the client holds a lock on.
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRM_GRANTED_CHAIN0
  TRM_GRANTED_CHAIN1
  TRM_DENIED

SIDE EFFECTS
  None

============================================================================*/

trm_grant_event_enum_t trm_get_granted
(
  /* The client who's RF chain ownership is desired */
  trm_client_enum_t               client
);




/*============================================================================

FUNCTION TRM_GET_RF_DEVICE

DESCRIPTION
  Get the RF device identifier for the given client
  
DEPENDENCIES
  None
  
RETURN VALUE
  RFCOM_TRANSCEIVER_0 - Owns primary RF chain
  RFCOM_RECEIVER_1    - Owns secondary RF chain
  
  TRM_NO_DEVICE       - No RF chain owned

SIDE EFFECTS
  None

============================================================================*/

rfcom_device_enum_type trm_get_rf_device
(
  /* The client who's RF chain ownership is desired */
  trm_client_enum_t               client
);



/*============================================================================

FUNCTION TRM_ENABLE_MODE

DESCRIPTION
  Enable the given TRM mode if possible.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_enable_mode
( 
  trm_mode_enum_t mode
);



/*============================================================================

FUNCTION TRM_DISABLE_MODE

DESCRIPTION
  Disable the given TRM mode if possible.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_disable_mode
( 
  trm_mode_enum_t mode
);

/*============================================================================

FUNCTION TRM_GET_1X_MODE

DESCRIPTION 
  Get the current 1x mode.  This is defined by the NV_ITEM
  
DEPENDENCIES
  None
  
RETURN VALUE
  The 1X mode

SIDE EFFECTS
  None

============================================================================*/

trm_1x_mode_enum_t  trm_get_1x_mode( void );

#endif /* FEATURE_MCS_TRM */

#endif /* TRM_H */

