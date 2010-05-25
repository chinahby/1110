#ifndef TRMI_H
#define TRMI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Internal Header File

GENERAL DESCRIPTION

  This file provides some common definitions for trm.cpp & trmlog.cpp


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


Copyright (c) 2005 - 2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/trm/main/latest/src/trmi.h#2 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
11/02/2005   cab     Added BEST_POSSIBLE resource option - pre-empts lower
                     pri clients from chain 0, but takes chain 1 if higher 
                     pri clients have chain 0
05/26/2005   ajn     Code review comment changes
04/08/2004   ajn     Initial AMSS version

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"

#ifdef FEATURE_MCS_TRM

extern "C"
{
  #include "trm.h"
  #include "trmcfg.h"
};



/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/


/*-----------------------------------------------------------------------------
  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  Enumerations in this file are 8-bit values that are used directly by the TRM
  log.  Changing enumerator values and adding new enumerator values will
  necessitate a new TRM log parser.
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Client lock state

    A TRM client may be inactive, waiting for an RF resource, or holding one.
    When waiting for an RF resource, it may need it at a particular time, or
    merely ASAP.  When holding an RF resource, it may need it for a specific
    length of time, or as long as possible.

    When TRM is processing a client request, it may temporarily change a
    client's state during processing, and restore the state when done, or
    change the client to a new state.
-----------------------------------------------------------------------------*/

enum trm_client_lock_state_enum
{
  /* Client does not have an RF chain, nor does it want one */
  TRM_INACTIVE,

  /* Client has a reservation for an RF chain at a particular time */
  TRM_RESERVED_AT,

  /* Client is requesting an RF chain immediately */
  TRM_REQUEST,

  /* Client requests to be notified when it can be granted an RF chain */
  TRM_REQUEST_AND_NOTIFY,

  /* Client has been given the RF chain, & will keep it until told not to */
  TRM_LOCK_RETAINED,

  /* Client has been given an RF chain until a specific time */
  TRM_LOCKED_UNTIL,


  /* The following states are TRANSIENT states.  TRM will change the client
     state to one of the above states before returning */  


  /* Client is trying to extend the "locked until" duration */
  TRM_EXTENDING,

  /* Client has been assigned an RF chain, but client hasn't been told yet */
  TRM_GRANTING
};




/*-----------------------------------------------------------------------------
  RF Chains
    The clients request resources with certain capabilites, which need to be
    converted to more specific RF chain requests.  [The second RF chain may
    not exist.  If it does exist, it may not have the same sensitivity as the
    first, and probably doesn't have an associated transmit chain.]
-----------------------------------------------------------------------------*/

enum trm_chain_enum
{
  /* The primary RF chain */
  TRM_CHAIN_0,

  /* The secondary RF chain */
  TRM_CHAIN_1,

  /* Any RF chain */
  TRM_CHAIN_ANY,

  /* The best possible RF chain */
  TRM_CHAIN_BEST_POSSIBLE,

  /* No RF chain */
  TRM_CHAIN_NONE
};



/*-----------------------------------------------------------------------------
  RF Chain Groups

    TRM will assign clients to "RF Chain Groups", or simply "groups", in the
    following manner:
  
      -1: Client has not yet been assigned to an RF chain, or
          client cannot be assigned to an RF chain.
       0: Client is assigned to the primary RF chain.
       1: Client is assigned to the secondary RF chain.
      2+: Client will be assigned to a chain, but which is not yet determined.
  
    For group "g" (g >= 0), "g xor 1" represents the group assigned to the
    other RF chain.  For example, if clients in group 2 end up being assigned
    to the secondary RF chain, group 3 (2 ^ 1) will be assigned to the primary.

-----------------------------------------------------------------------------*/

typedef int8                      trm_group;



/*-----------------------------------------------------------------------------
  Client state information

    A client may be inactive, requesting a lock, or holding one.
    If it is requesting a lock, it requests a lock on a resource, which maps
    to an RF chain (primary, secondary, or any).
    It will request or hold a lock for a specific reason, which will map to
    a particular priority.
    TRM will assign the request into a group, based on priority and conflicts.
    A client may be granted an lock on an RF chain (primary, or secondary),
    which correspond to an RF device.
    If holding a lock, it may report to the client when it must release it.
-----------------------------------------------------------------------------*/

/*
  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  This structure is a hand-packed 8-byte long record, and is used for logging.
  Changes to the size, order, or content will require an updated log parser. */

struct trm_client_state
{
  /* Lock state (inactive, requesting a lock, holding a lock, ... ) */
  trm_client_lock_state_enum      lock_state;

  /* Resource the client wants (RX_ANY, RXTX, RX_GPS, ...) */
  trm_resource_enum_t             resource;

  /* Reason the client wants the resource (Acq, Traffic, QPCH, ...) */
  trm_reason_enum_t               reason;

  /* Relative priority (based on client and reason) */
  trm_pri_t                       priority;

  /* RF Chain client wants (based on resource & h/w capability) */
  trm_chain_enum                  chain;

  /* What the client currently holds (Nothing, Chain 0, or Chain 1) */
  trm_grant_event_enum_t          granted;

  /* Internal TRM group assignment, for conflict resolution */
  trm_group                       group;

  /* When the client must give up a lock (By a certain time, now, never) */
  trm_unlock_event_enum_t         unlock_state;
};

#endif /* FEATURE_MCS_TRM */

#endif /* TRMI_H */

