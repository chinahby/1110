#ifndef PS_LO_H
#define PS_LO_H
/*===========================================================================

              D M S S   L O O P B A C K   I N T E R F A C E

          I N I T I A L I Z A T I O N   F U N C T I O N   A N D

      A C C E S S   C O N T R O L   L I S T   D E C L A R A T I O N S
 
GENERAL DESCRIPTION
  The DMSS loopback interface allows applications on the phone to
  communicate with each other via DS sockets.
  The loopback interface is implemented as a ps_iface, i.e. the
  loopback traffic traverses the complete IP stack.

EXTERNALIZED FUNCTIONS
  lo_init()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  lo_init() should be called as part of the PS initialization.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/03    om     Externalized the /dev/lo ACL.
02/13/03    ss     Included ps_aclrules.h
11/01/02    om     Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_meta_info.h"
#include "ps_iface.h"


/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  The loopback interface's ACL
---------------------------------------------------------------------------*/
extern acl_type lo_acl;


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION LO_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface, bring it up, and add it's route.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_init( void );

/*===========================================================================
FUNCTION LO_TX_CMD()

DESCRIPTION
  This function is used as transmit command for the loopback interface.
  It discards any meta info and send the IP datagramm back to the stack.

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr     - Pointer to transmitting interface
  pkt_chain_ptr      - Reference pointer to IP datagramm
  meta_info_ref_ptr  - Reference pointer to packet's meta info
  tx_cmd_info        - User data pointer (not used)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int lo_tx_cmd
(
  ps_iface_type*       this_iface_ptr,
  dsm_item_type**      pkt_chain_ptr,
  ps_meta_info_type**  meta_info_ref_ptr,
  void*                tx_cmd_info
);

/*===========================================================================

                    ACCESS CONTROL LIST DEFINITION

===========================================================================*/

ACL_DEF( lo_rt_acl );

#endif /* PS_LO_H */
