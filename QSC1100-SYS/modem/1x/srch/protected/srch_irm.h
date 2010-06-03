#ifndef SRCH_IRM_H
#define SRCH_IRM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*  

               S R C H Z Z _ I R M      H E A D E R    F I L E


GENERAL DESCRIPTION
   This file contains definitions for 1X L1 Internal Resource Manager.

  Copyright (c) 2005 - 2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_irm.h#1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
08/14/08   vks     Added FEATURE_SRCH_HAS_NO_IRM for ULC
03/07/06   rkc     Add monitoring of reserve and release functions, callback
                   function prototypes and parameter responses.
12/05/05   bt      Added SRCH_IRM_NO_RESOURCE.
12/01/05   bt      Added SRCH_IRM_SRCH4_SCHED_RESOURCE resource control.
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
10/11/05   bt      Added MFLO client and NLIST resource.
10/05/05   bt      Added srch_irm_resource_available.
10/03/05   awj     Updated srch_irm interface
09/26/05   awj     First implementation of Restricted Timeline
===========================================================================*/ 

#include "comdef.h"
#include "customer.h"

/* If FEATURE_SRCH_HAS_NO_IRM is defined, featurize out the entire file */
#ifndef FEATURE_SRCH_HAS_NO_IRM

/* Client ID to be used in resource requests */
typedef enum {
  SRCH_IRM_FIRST_CLIENT,
  SRCH_IRM_GPS_CLIENT = SRCH_IRM_FIRST_CLIENT,
  SRCH_IRM_PPM_CLIENT,
  SRCH_IRM_SYS_MEAS_CLIENT,
  SRCH_IRM_DEBUG_CLIENT,
  SRCH_IRM_DEBUG_CLIENT2,
  SRCH_IRM_MFLO_CLIENT,
  SRCH_IRM_LAST_CLIENT
} srch_irm_client_type;

/* Clients use this bitmask to request specific resources */
typedef uint32 srch_irm_resource_pri_bitmask_type;

/* Slave Resources */

#define SRCH_IRM_SLAVE_RXCX8_RESOURCE        0x1
#define SRCH_IRM_SLAVE_RTC_RESOURCE          0x2

/* This resource is available when the 1X receive chain is 
   synchronized to RTC */
#define SRCH_IRM_SLAVE_RTC_SYNC_RESOURCE     0x4

/* This resource is available when 1X is tuned to the 
   home band/channel */
#define SRCH_IRM_SLAVE_1X_SYSTEM_RESOURCE    0x8

/* This resource is availabe when 1X has received a 
   neighbor list message */
#define SRCH_IRM_SLAVE_NLIST_RESOURCE        0x10

/* REVISIT SCHED should be changed to allow the 
   MDSP services interface to deal with SRCH4 arbitration. 
   This resource is available when SRCH_SCHED is 
   not using SRCH4. */
#define SRCH_IRM_SLAVE_SRCH4_SCHED_RESOURCE  0x20

/* Master Resources */
#define SRCH_IRM_MASTER_RXCX8_RESOURCE       0x100
#define SRCH_IRM_MASTER_RTC_RESOURCE         0x200

/* This resource is available when the 1X receive chain is 
   synchronized to RTC */
#define SRCH_IRM_MASTER_RTC_SYNC_RESOURCE    0x400

/* This resource is available when 1X is tuned to the 
   home band/channel */
#define SRCH_IRM_MASTER_1X_SYSTEM_RESOURCE   0x800

/* This resource is availabe when 1X has received a 
   neighbor list message */
#define SRCH_IRM_MASTER_NLIST_RESOURCE       0x1000

/* REVISIT SCHED should be changed to allow the 
   MDSP services interface to deal with SRCH4 arbitration. 
   This resource is available when SRCH_SCHED is 
   not using SRCH4. */
#define SRCH_IRM_MASTER_SRCH4_SCHED_RESOURCE  0x2000

#define SRCH_IRM_NO_RESOURCE           0x0

#define SRCH_IRM_RXCX8_RESOURCE      ( SRCH_IRM_SLAVE_RXCX8_RESOURCE |     \
                                       SRCH_IRM_MASTER_RXCX8_RESOURCE )
#define SRCH_IRM_RTC_RESOURCE        ( SRCH_IRM_SLAVE_RTC_RESOURCE |       \
                                       SRCH_IRM_MASTER_RTC_RESOURCE )
#define SRCH_IRM_RTC_SYNC_RESOURCE   ( SRCH_IRM_SLAVE_RTC_SYNC_RESOURCE |  \
                                       SRCH_IRM_MASTER_RTC_SYNC_RESOURCE )
#define SRCH_IRM_1X_SYSTEM_RESOURCE  ( SRCH_IRM_SLAVE_1X_SYSTEM_RESOURCE | \
                                       SRCH_IRM_MASTER_1X_SYSTEM_RESOURCE )
#define SRCH_IRM_NLIST_RESOURCE      ( SRCH_IRM_SLAVE_NLIST_RESOURCE |     \
                                       SRCH_IRM_MASTER_NLIST_RESOURCE )

#define SRCH_IRM_SRCH4_SCHED_RESOURCE ( SRCH_IRM_SLAVE_SRCH4_SCHED_RESOURCE | \
                                        SRCH_IRM_MASTER_SRCH4_SCHED_RESOURCE )

#define SRCH_IRM_SLAVE_RESOURCES   ( SRCH_IRM_SLAVE_RXCX8_RESOURCE |     \
                                     SRCH_IRM_SLAVE_RTC_RESOURCE |       \
                                     SRCH_IRM_SLAVE_RTC_SYNC_RESOURCE |  \
                                     SRCH_IRM_SLAVE_1X_SYSTEM_RESOURCE | \
                                     SRCH_IRM_SLAVE_NLIST_RESOURCE )

#define SRCH_IRM_MASTER_RESOURCES  ( SRCH_IRM_MASTER_RXCX8_RESOURCE |     \
                                     SRCH_IRM_MASTER_RTC_RESOURCE |       \
                                     SRCH_IRM_MASTER_RTC_SYNC_RESOURCE |  \
                                     SRCH_IRM_MASTER_1X_SYSTEM_RESOURCE | \
                                     SRCH_IRM_MASTER_NLIST_RESOURCE)

#define SRCH_IRM_1X_RESOURCES      ( SRCH_IRM_SLAVE_RESOURCES |  \
                                     SRCH_IRM_MASTER_RESOURCES )

/* Overloaded callback response for both reserve and release requests */
typedef enum {
  /* If the resource has been available since last use, notify client */
  SRCH_IRM_PERSISTENT,
  SRCH_IRM_DEACTIVATED,

  /* If the release callback is being called due to the client yielding, 
     notify the client. */
  SRCH_IRM_CLIENT_RELEASE,
  SRCH_IRM_1X_FORCED_RELEASE

} srch_irm_cb_response_type;

/* Callback function type for client resource requests */
typedef void (*srch_irm_cb_type)(srch_irm_cb_response_type);

/* Reserve and release resource monitoring responses */
typedef enum {
    /* If resource(s) is released */
    SRCH_IRM_RESOURCES_RELEASED_IMMED,
    SRCH_IRM_RESOURCES_RELEASED,

    /* If resource(s) reservation requested */
    SRCH_IRM_RESOURCES_RESERVED_IMMED,
    SRCH_IRM_RESOURCES_RESERVED

} srch_irm_monitor_cb_response_type;

/* Callback function type for monitoring resource reservation and release */
typedef void (*srch_irm_monitor_cb_type)(srch_irm_monitor_cb_response_type,
                                         srch_irm_resource_pri_bitmask_type);

/* Resource availability for updates to the IRM */
typedef enum{
  SRCH_IRM_RESOURCE_AVAILABLE,
  SRCH_IRM_RESOURCE_UNAVAILABLE
} srch_irm_resource_update_type;

/*-------------------------------------------------------------------------
      Exported Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       SRCH_IRM_CLIENT_RESERVE_RESOURCE

DESCRIPTION    Sets a client's request to active.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Registers a callback and potentially begins a transfer to 
               the RTL.  Prevents sleep for specified resources.

===========================================================================*/
void srch_irm_client_reserve_resource
(  
  srch_irm_client_type client, 
  srch_irm_resource_pri_bitmask_type mask, 
  srch_irm_cb_type request_cb,
  srch_irm_cb_type release_cb
);

/*===========================================================================

FUNCTION       SRCH_IRM_CLIENT_RELEASE_RESOURCE

DESCRIPTION    Clears the client's request for resources.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Removes restrictions on which resources may be disabled for
               sleep.  Last one out, hit the lights.

===========================================================================*/
extern void srch_irm_client_release_resource
(  
  srch_irm_client_type client
);

/*===========================================================================

FUNCTION       SRCH_IRM_RESOURCE_REQUESTED

DESCRIPTION    Checks the request bit vector.

DEPENDENCIES   None

RETURN VALUE   Returns TRUE if a client has requested reservation of at least
               one of the specified resources.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_irm_resource_requested
(
  srch_irm_resource_pri_bitmask_type
);

/*===========================================================================

FUNCTION       SRCH_IRM_RESOURCE_UPDATE

DESCRIPTION    Inform the IRM of new resource status.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_irm_resource_update
( 
  srch_irm_resource_pri_bitmask_type,
  srch_irm_resource_update_type 
);

/*===========================================================================

FUNCTION       SRCH_IRM_RESOURCES_AVAILABLE

DESCRIPTION    Checks if resources are available

DEPENDENCIES   None

RETURN VALUE   Returns TRUE if all resources specified are available.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_irm_resource_available
(
  srch_irm_resource_pri_bitmask_type resources
);

/*===========================================================================

FUNCTION       SRCH_IRM_MONITOR_RESERVE_RESOURCE

DESCRIPTION    Registers a callback to be called when specified resources
               have requested reservation.

               To clear the monitoring, this function should be called with
               NULL callback function as the parameter.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_irm_monitor_reserve_resource
(  
  srch_irm_resource_pri_bitmask_type   resources, 
  srch_irm_monitor_cb_type             reserve_cb
);

/*===========================================================================

FUNCTION       SRCH_IRM_MONITOR_RELEASE_RESOURCE

DESCRIPTION    Registers a callback to be called when specified resource(s)
               are released.

               To clear the monitoring, this function should be called with
               NULL callback function as the parameter.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_irm_monitor_release_resource
(  
  srch_irm_resource_pri_bitmask_type   resources, 
  srch_irm_monitor_cb_type             release_cb
);

#endif /* FEATURE_SRCH_HAS_NO_IRM */

#endif /* SRCH_IRM_H */
