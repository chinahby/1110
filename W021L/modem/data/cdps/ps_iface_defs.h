#ifndef PS_IFACE_DEFS_H
#define PS_IFACE_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E _ D E F S . H

DESCRIPTION
  Header containing PS_IFACE names that need to be used in other contexts,
  but that don't required the entire ps_iface header.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_defs.h_v   1.4   07 Feb 2003 20:12:48   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface_defs.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/27/03    om     Added IPSEC_IFACE name.
05/13/03    usb    Incremented MAX_SYSTEM_IFACES for near future needs, 
                   changed iface name values to bit mask.
05/05/03    aku    Moved MAX_SYSTEM_IFACES defintion from ps_iface.h
02/07/03    usb    Added event IFACE_IPFLTR_UPDATED_EV.
01/31/03    usb    Added LO_IFACE to the iface name enum
12/24/02    aku    Moved ps_iface_event_enum_type to this file from 
                   ps_iface.h
11/15/02    mvl    Added physical link statistics.
09/17/02    mvl    created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_NAME_ENUM_TYPE

DESCRIPTION
  List of interface names and groups. 
  
  IMPORTANT NOTE: increment MAX_SYSTEM_IFACES (see below) appropriately 
  if a new iface name is added to this list.
---------------------------------------------------------------------------*/
typedef enum
{
  /* Group Names - these are bit masks (uses bottom 15 bits), msb is 0 */
  ANY_IFACE_GROUP    = 0x0000,
  ANY_DEFAULT_GROUP  = 0x0001,
  WWAN_GROUP         = 0x0002,
  RM_GROUP           = 0x0004,
  
  /* Interface Names - these are bit masks, msb always set to 1 */
  IFACE_MASK         = 0x8000,
  CDMA_SN_IFACE      = 0x8001,
  CDMA_AN_IFACE      = 0x8002,
  UMTS_IFACE         = 0x8004,
  SIO_IFACE          = 0x8008,
  IPSEC_IFACE        = 0x8400,
  LO_IFACE           = 0x8800

} ps_iface_name_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STATE_ENUM_TYPE

DESCRIPTION
  Enum for states of an interface.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_DISABLED   = 0,
  IFACE_DOWN       = 1,
  IFACE_COMING_UP  = 2,
  IFACE_ROUTEABLE  = 3,          /* packet can be routed to this interface */
  IFACE_UP         = 4,          /* data can originate over this interface */
  IFACE_GOING_DOWN = 5
} ps_iface_state_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF PHYS_LINK_STATE_TYPE

DESCRIPTION
  Physical link state enum - what is the physical link doing
---------------------------------------------------------------------------*/
typedef enum
{
  PHYS_LINK_DOWN       = 0,
  PHYS_LINK_COMING_UP  = 1,
  PHYS_LINK_UP         = 2,
  PHYS_LINK_GOING_DOWN = 3

} phys_link_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_ENUM_TYPE

DESCRIPTION
  List of all the possible events on an interface that modules can register
  callbacks for.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_MIN_EV            =  0,
  IFACE_ENABLED_EV        =  0,
  IFACE_DISABLED_EV       =  1,
  IFACE_DOWN_EV           =  2,
  IFACE_ROUTEABLE_EV      =  3,
  IFACE_UP_EV             =  4,
  IFACE_PHYS_LINK_DOWN_EV =  5,
  IFACE_PHYS_LINK_UP_EV   =  6,
  IFACE_FLOW_ENABLED_EV   =  7,
  IFACE_FLOW_DISABLED_EV  =  8,
  IFACE_ADDR_CHANGED_EV   =  9,
  IFACE_MTU_CHANGED_EV    = 10,
  IFACE_DELETE_EV         = 11,
  IFACE_IPFLTR_UPDATED_EV = 12,
  IFACE_EVENT_MAX

} ps_iface_event_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STATS_TYPE
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    physical link statistics - bytes RX/TX by physical layer (e.g. RLP/RLC)
  -------------------------------------------------------------------------*/
  struct
  {
    uint32 rx_bytes;
    uint32 tx_bytes;
  } phys_link;

} ps_iface_stats_type;


/*---------------------------------------------------------------------------
  MAX_SYSTEM_IFACES - the maximum number of interfaces in the system.
  2 - CDMA SN IFACE (1 for pkt, 1 for async)
  1 - CDMA AN IFACE  
  1 - UMTS IFACE
  2 - SIO IFACE (1 for CDMA, 1 for UMTS)  
  1 - LO IFACE  
  2 - Miscellaneous
---------------------------------------------------------------------------*/
#define MAX_SYSTEM_IFACES 9

#endif /* FEATURE_DATA */
#endif /* PS_IFACE_DEFS_H */
