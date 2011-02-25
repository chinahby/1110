#ifndef MCCQPCH_H
#define MCCQPCH_H
/*===========================================================================

               Q U I C K   P A G I N G   C H A N N E L

GENERAL DESCRIPTION
  This module provide call processing related code to support operation of
  the quick paging channel.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccqpch.h_v   1.3   25 Sep 2002 19:51:30   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccqpch.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/06   fc      Mainlined REL A searcher interface.
03/21/03   fc      Added support for PCH BI.
02/27/03   fc      Added support for Release B.
09/25/02   fc      Added BI walsh code.
02/22/02   fc      Added support for Release A common channels.
04/11/00   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "qw.h"


/*===========================================================================

                        DEFINITONS FOR CONSTANTS

===========================================================================*/
// Walsh function definitions for Quick Paging Channel.  Defined according
// to CDMA-2000
#define QPCH_CHAN_1_WALSH  80
#define QPCH_CHAN_2_WALSH  48
#define QPCH_CHAN_3_WALSH  112

#define QPCH_CCI_WALSH     80

#if defined(FEATURE_IS2000_REL_A_CC) || defined(FEATURE_IS2000_REL_B)
#define QPCH_BI_WALSH      80
#endif /* FEATURE_IS2000_REL_A_CC  or FEATURE_IS2000_REL_B */

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
typedef enum
{
  QPCH_INVALID_CH = 0,
  QPCH_CH_1 = 1,
  QPCH_CH_2 = 2,
  QPCH_CH_3 = 3
} mccqpch_channel_type;

// Rate for quick paging channel.  Note: defined according to IS2000.
typedef enum
{
  QPCH_4800 = 0,
  QPCH_9600 = 1
} mccqpch_rate_type;

typedef enum
{
  QPCH_NOT_SUPPORTED,
  QPCH_INFO_VALID,
  QPCH_INFO_INVALID
} mccqpch_status_type;

typedef enum
{
  IMSI_M_HASHED,
  IMSI_T_HASHED
} mccqpch_hash_basis_type;

typedef enum
{
  PCH_UNKNOWN_FRAMING,
  PCH_PRE_IS95B_FRAMING,
  PCH_IS95B_FRAMING
} mccqpch_pch_framing_type;

typedef struct
{
  uint16  walsh_code;
  int8    power_level;
} mccqpch_cci_info_type;

typedef struct
{
  uint16  walsh_code;
  int8    power_level;
} mccqpch_bi_info_type;

typedef struct
{
  mccqpch_rate_type       qpch_rate;
  uint16                  pi_walsh_code;
  int8                    pi_power_level;
  boolean                 cci_supported;
  mccqpch_cci_info_type   cci;
  mccqpch_hash_basis_type hash_basis;
  boolean                 bi_supported;
  mccqpch_bi_info_type    bi; 
} mccqpch_qpch_info_type;

typedef struct
{
  uint16  pi1_pos;
  uint16  pi2_pos;
} mccqpch_pi_pos_type;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION mccqpch_get_qpch_info

DESCRIPTION
  Gets the quick paging channel information from the specified pilot.  The
  pilot should be a known pilot to the mobile (i.e. stored in bs_info).  The
  QPCH is assumed to be in the same band class.  sys_time specified when the
  information will be used.  If it is NULL (pointer), the information will not
  be checked for expiration.

DEPENDENCIES
  None.

RETURN VALUE
  UNKNOWN_PILOT       - Pilot not found in bs_info.
  QPCH_NOT_SUPPORTED  - QPCH is not support for the specified pilot
  QPCH_INFO_INVALID   - The stored QPCH information for the pilot is not valid
                        (expired or not yet received)
  QPCH_INFO_VALID     - Requested information is returned in the function
                        payload.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_status_type mccqpch_get_qpch_info
(
  uint16                  cdma_channel,
  uint16                  pilot_pn,
  qword                   sys_time,     // The time the bs info will be used
  mccqpch_qpch_info_type  *qpch_info
);

/*===========================================================================
FUNCTION mccqpch_get_qpch_pi_pos

DESCRIPTION
  Gets the paging indicator positions for the specified slot of the active
  pilot.  Note that the PI positions for both QPCH rates are returned just in
  case SRCH performs an idle handoff to a known pilot which uses a different
  QPCH rate after calling this function (saves one call to this function).

DEPENDENCIES
  Input parameter pi_pos must be array larger enough (2 members) to hold the
  PI positions.

RETURN VALUE
  QPCH_NOT_SUPPORTED  - QPCH is not support for the specified pilot
  QPCH_INFO_INVALID   - The stored QPCH information for the pilot is not valid
                        and no PI positions are generated.
  QPCH_INFO_VALID     - PI positions are returned in the function payload.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_status_type mccqpch_get_qpch_pi_pos
(
  qword               slot_sys_time,
  mccqpch_pi_pos_type pi_pos[]
);

/*===========================================================================
FUNCTION mccqpch_get_pch_framing_type

DESCRIPTION
  Gets the forward common control channel or paging channel framing type of 
  the specified pilot.  This helps searcher to determine where to wake up
  before the assigned slot.

DEPENDENCIES
  None.

RETURN VALUE
  UNKNOWN_FRAMING      -  Paging channel framing of specified pilot is unknown.
                          (because pilot is unknown).
  PRE_IS95B_FRAMING    -  IS95A or JSTD008 framing.
  IS95B_FRAMING        -  For IS95B and beyond, the frame right before the slot
                          that begins a synchronized capsule is guaranteed to
                          tail off with 0's.  Searcher can take advantage of
                          this to reduce wakeup time.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_pch_framing_type mccqpch_get_pch_framing_type
(
  uint16  cdma_channel,
  uint16  pilot_pn
);

#endif // MCCQPCH_H

