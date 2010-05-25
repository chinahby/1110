#ifndef DS707_DIAG_H
#define DS707_DIAG_H
/*===========================================================================

                        D S 7 0 7 _ D I A G 
GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to 1X data.

EXTERNALIZED FUNCTIONS
 ds707_diag_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2003 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_diag.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/03   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "diagcmd.h"
#include "diagpkt.h"
#include "dsrlp.h"



/*===========================================================================
                                DIAG_SUB_CMDS
===========================================================================*/

/*---------------------------------------------------------------------------
  The sub-system id's for the different commands we handle.  There is a 
  higher-level command defined in DIAG for DIAG_SUBSYS_DATA1X
---------------------------------------------------------------------------*/

#define DS707_DIAG_RLP_STATS        0


/*===========================================================================
                         DIAG RLP STATS STRUCTS
===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DS707_DIAG_RLP_STATS)
DIAGPKT_REQ_END

/*---------------------------------------------------------------------------
  Definition of response packet to DIAG.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_RSP_DEFINE(DATA1X, DS707_DIAG_RLP_STATS)
  uint16    last_pkt_so;          /* last packet so that connected         */
  uint32    rlp_rx_bytes;         /* RLP I/II rx'ed bytes                  */
  uint32    rlp_tx_bytes;         /* RLP I/II tx'ed bytes                  */
  uint32    rlp3_rx_bytes;        /* RLP III rx'ed bytes  (1x)             */
  uint32    rlp3_tx_bytes;        /* RLP III tx'ed bytes  (1x)             */
  uint32    rlp_hdr_SN_rx_bytes;  /* HDR RLP (SN) rx'ed bytes              */
  uint32    rlp_hdr_SN_tx_bytes;  /* HDR RLP (SN) tx'ed bytes)             */
  uint32    rlp_hdr_AN_rx_bytes;  /* HDR RLP (AN) rx'ed bytes              */
  uint32    rlp_hdr_AN_tx_bytes;  /* HDR RLP (AN) tx'ed bytes)             */
DIAGPKT_RSP_END



/*===========================================================================
                          EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_diag_init(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DIAG_H */

