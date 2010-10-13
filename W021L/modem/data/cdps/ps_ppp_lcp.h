#ifndef PS_PPP_LCP_H
#define PS_PPP_LCP_H
/*===========================================================================

                           P S _ P P P _ L C P . H

DESCRIPTION

  Copyright 1992-1995 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

  Copyright (c) 1995, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
  Copyright (c) 1999-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_lcp.h_v   1.2   12 Feb 2003 14:31:50   mlioy  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_lcp.h#1 $
  $Author: nsivakum $ $DateTime: 2007/11/05 03:25:37 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/03    mvl    Added some JCDMA timing changes.
05/23/03    atp    Reverted number of request attempts to 20.
04/29/03    jd     Reduce term-req tries to 2 from 10.  Added multiplier for
                   term-req timeout to extend for dormant reorigination case.
                   Fixed compile error with SHOW_STAT off.
04/24/03    mvl    Removed duplicate definition of option bits.
03/26/03    mvl    removed authentication featurization
02/11/03    mvl    Reduced the number of Request attemps to 10 and the number
                   or Term attempts to 3.
08/02/02    mvl    moved the auth definitions to either the .c files or defs
07/25/02    mvl    removed netmodel dependency.  Updates for PPP renaming.
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
07/30/01    mvl    Modified prototypes to match new call signatures.  Only
                   defining LCP_N_AP to its non-zero value if an auth
                   protocol is supported.
07/28/01    mvl    moved the lcp_values data struct to pppvalues.h
07/27/01    mvl    Modified to use pstimers and shortened retry to 1000ms
07/25/01    mvl    Added prototypes for powerup initialization and timer
                   cancellation.
04/24/00    snn    Increased the value of Max_passwd_len from 14 to 16,
                   because during resync, the CHAP password used has a
                   length of 16.
03/10/00    snn    added CHAP support
10/28/99    mvl    Added code review changes
09/27/99    snn    Changed ppp_auth_info_type to a typedef to follow coding
                   standards. Fixed commenting styles. Added comments for
                   the PAP prototypes.
08/25/99    snn    Updated the sizes of the arrays in PAP data structure
08/19/99    mvl    Added header for lcp_save_work_opt()
08/09/99    snn    Added support for PAP timeouts.
07/26/99    mvl    Changed the NAK_TRY value to 3 from 10 as 10 nak cycles
                   will take more than 3 seconds.  Avoids infinite loop.
07/26/99    mvl    removed references to quality reporting period which is
                   not supported.
07/26/99    na     Changed header for lcp_init() for network model builds.
07/26/99    mvl    Added typedef for various structures
07/09/99    snn    Added support for PAP implementation
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define LCP_REQ_TRY    20     /* REQ attempts                              */
#define LCP_NAK_TRY     3     /* NAK attempts                              */
#define LCP_TERM_TRY  2     /* pseudo-tries on TERM REQ - code actually 
                               only retries once, but timeout is this value
                               times the standard c-req timeout to special 
                               case the fsm_timeout code .                 */
#define LCP_TERM_MULT 3     /* multiplier for LCP TERM-REQ retry timeout   */
#define LCP_TIMEOUT  1000     /* milliseconds to wait for response         */

#ifdef FEATURE_JCDMA_1X
#undef LCP_TIMEOUT
#undef LCP_REQ_TRY

#define LCP_TIMEOUT   3000   /* milliseconds to wait for response          */
#define LCP_TERM_TO    500   /* timeout for sending term requests          */
#define LCP_REQ_TRY     10   /* the number of retries to attempt           */
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  LCP option types
---------------------------------------------------------------------------*/
#define LCP_MRU           1
#define LCP_ACCM          2
#define LCP_AP            3
#define LCP_QMP           4
#define LCP_MAGIC         5
#define LCP_PFC           7
#define LCP_ACFC          8
#define LCP_OPTION_LIMIT  8     /* highest # we can handle                 */

/*---------------------------------------------------------------------------
  Other configuration option values
---------------------------------------------------------------------------*/
#define LCP_ACCM_DEFAULT  0xffffffffL
#define LCP_MRU_DEFAULT   1500
#define LCP_MRU_HI        4096
#define LCP_MRU_LO        128

/*---------------------------------------------------------------------------
  LCP control block
---------------------------------------------------------------------------*/
struct lcp_pdv_s
{
  int dummy;
};

/*---------------------------------------------------------------------------
  The options that are supported by the PPP stack
---------------------------------------------------------------------------*/
extern negotiate_t    lcp_accept;
extern lcp_value_type lcp_default;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION LCP_POWERUP_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  FSM for LCP on the interface specified, this includes getting a timer.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lcp_powerup_init
(
  ppp_dev_enum_type iface /* the interface to initialize */
);


/*===========================================================================
FUNCTION LCP_INIT()

DESCRIPTION
  This function will initialize the LCP configuration structure for the given
  interface (Rm or Um).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_init
(
  ppp_dev_enum_type     iface,   /* The interface to init      */
  ppp_dev_opts_type    *opts_ptr /* options to initialize with */
);

/*===========================================================================
FUNCTION LCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the LCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void lcp_save_work_opt
(
  struct fsm_side_s *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val                                 /* the value of the option */
);

#endif /* _PS_PPP_LCP_H */
