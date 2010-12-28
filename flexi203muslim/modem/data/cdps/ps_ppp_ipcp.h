#ifndef PS_PPP_IPCP_H
#define PS_PPP_IPCP_H
/*===========================================================================

                          P S _ P P P _ I P C P . H

DESCRIPTION

  Copyright 1992-1995 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

  Copyright (c) 1995, 1999 by QUALCOMM Incorporated.  All Rights Reserved.
  Copyright (c) 2001-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_ipcp.h_v   1.3   12 Feb 2003 14:30:58   mlioy  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_ipcp.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/03    mvl    Added some JCDMA timing changes.
05/23/03    atp    Reverted number of request attempts to 20.
04/24/03    mvl    Removed duplicate definition of option bits.
02/06/03    mvl    Removed prototype for ipcp_net_model_in_resync().  Reduced
                   the number of Request attemps to 10 and the number or Term
                   attempts to 3.
10/17/02    aku    Increased IPCP_TIMEOUT to 1000ms from 500ms.
09/12/02    mvl    Mainlined netmodel code, generified the network model
                   resync callcack.
07/25/02    mvl    Updates for PPP renaming.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
07/30/01    mvl    Modified prototypes to match new call signatures.
07/28/01    mvl    moved the ipcp_values data struct to pppvalues.h
07/27/01    mvl    Modified to use pstimers and shortened retry to 500ms,
                   address is now a uint32 in ipcp value struct, code cleanup
07/25/01    mvl    Added prototypes for powerup initialization and timer
                   cancellation.
10/28/99    mvl    Added code review changes
08/20/99    mvl    Added declaration of ipcp_save_work_opt()
07/28/99    mvl    changed the NAK_TRY value to 3 from 10 as 10 nak cycles
                   will take more than 3 seconds.  Avoids infinite loop.
06/18/99    mvl    removed references to two address option which is not
                   supported.
06/08/99    mvl    Changed the size of the 'slots' field in the IPCP option
                   value field to a uint8 (which is what the protocol size is
                   for this field).
05/20/99    na     Changed header for ipcp_init() for network model builds.
05/20/99    mvl    Added typedef for various structures
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_slhc.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define IPCP_REQ_TRY    20    /* REQ attempts                              */
#define IPCP_NAK_TRY     3    /* NAK attempts                              */
#define IPCP_TERM_TRY    3    /* tries on TERM REQ                         */
#define IPCP_TIMEOUT  1000    /* milliseconds to wait for response         */

#ifdef FEATURE_JCDMA_1X
#undef  IPCP_REQ_TRY
#undef  IPCP_TIMEOUT

#define IPCP_REQ_TRY    10    /* REQ attempts                              */
#define IPCP_TIMEOUT  3000    /* milliseconds to wait for response         */
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  IPCP option types
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS        2
#define IPCP_1ADDRESS        3
#define IPCP_PRIMARY_DNS     129
#define IPCP_SECONDARY_DNS   131
#define IPCP_OPTION_LIMIT    131  /* highest # we can handle */

/*---------------------------------------------------------------------------
  Size of the accepted options
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS_SIZ      4
#define IPCP_1ADDRESS_SIZ      6
#define IPCP_PRIMARY_DNS_SIZ   IPCP_1ADDRESS_SIZ
#define IPCP_SECONDARY_DNS_SIZ IPCP_1ADDRESS_SIZ

#define IPCP_SLOT_DEFAULT      16                /* Default # of slots   */
#define IPCP_SLOT_HI           SLHC_ARRAY_SIZE   /* Maximum # of slots   */
#define IPCP_SLOT_LO           1                 /* Minimum # of slots   */
#define IPCP_SLOT_COMPRESS     1                 /* May compress slot id */

/*---------------------------------------------------------------------------
  IPCP control block
---------------------------------------------------------------------------*/
struct ipcp_pdv_s
{
  struct slcompress *slhcp;  /* pointer to compression block */
};
typedef struct ipcp_pdv_s ipcp_pdv_type;

/*---------------------------------------------------------------------------
TYPEDEF NET_MODEL_RESYNC_FPTR_TYPE

DESCRIPTION
  type declaration for the net model resync function
---------------------------------------------------------------------------*/
typedef void (*net_model_resync_fptr_type)
(
  ppp_dev_enum_type device
);

/*---------------------------------------------------------------------------
  The IPCP options that are supported by the PPP stack
---------------------------------------------------------------------------*/
extern negotiate_t     ipcp_accept;
extern ipcp_value_type ipcp_default;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for IPCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ipcp_powerup_init
(
  ppp_dev_enum_type iface /* the interface to initialize */
);

/*===========================================================================
FUNCTION IPCP_INIT

DESCRIPTION
  This function will Initialize the IPCP configuration structure on the
  given interface( Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_init
(
  ppp_dev_enum_type     iface,    /* The interface to init      */
  ppp_dev_opts_type  *opts_ptr  /* options to initialize with */
);

/*===========================================================================
FUNCTION IPCP_START

DESCRIPTION
  This function will Check for PPP Network-Layer Protocol Phase

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_start
(
  struct ppp_s *ppp_p
);

/*===========================================================================
FUNCTION IPCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the IPCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  ipcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_save_work_opt
(
  struct fsm_side_s *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val                                 /* the value of the option */
);

/*===========================================================================
FUNCTION IPCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the IPCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipcp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
);

#endif /* PS_PPP_IPCP_H */
