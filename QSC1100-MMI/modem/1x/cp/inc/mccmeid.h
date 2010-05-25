#ifndef MCCMEID_H
#define MCCMEID_H
/*==========================================================================

      C D M A   M A I N   C O N T R O L   FOR   MEID   SUPPORT

DESCRIPTION
  This header file contains the functions that supports MEID.


Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccmeid.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/06   fc      Added meta comments.
09/22/04   ht      Created file.    

===========================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "qw.h"


/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
 
/* <EJECT> */
/*===========================================================================

FUNCTION MCC_ESN_PROGRAMMING_IS_ALLOWED_WITH_MEID_SUPPORT

DESCRIPTION
  This function checks to see if ESN programming is allowed when the MEID 
  is supported.

DEPENDENCIES

RETURN VALUE
    Return TRUE - Mobile may be programmed with a true ESN (not PESN) for
                  pre-PREV 11 mobiles.
           FALSE - the MEID and PESN is used. The mobile shall not accept
                  ESN programming

SIDE EFFECTS

===========================================================================*/
boolean mcc_esn_programming_is_allowed_with_meid_support (void);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mcc_esn_programming_is_allowed_with_meid_support */
#endif


/*===========================================================================

FUNCTION MCC_MEID_CREATE_SHA1_DIGEST

DESCRIPTION
  This function computes the msg digest (160 bits) using the SHA-1 algorithm.

DEPENDENCIES

RETURN VALUE
    TRUE - Success
    FALSE - Fail

SIDE EFFECTS

===========================================================================*/
boolean mcc_meid_create_sha1_digest(qword, unsigned long *);


#define  MCC_PESN_PREFIX       0x80000000UL     /* Pseudo-ESN Prefix */
#define  MCC_PESN_PREFIX_MASK  0xFF000000UL     /* Pseudo-ESN Prefix mask */

#define  MCC_GET_PSEUDO_ESN   (((meidhash[4]) & (~MCC_PESN_PREFIX_MASK)) | (MCC_PESN_PREFIX))

/*===========================================================================

FUNCTION MCC_COMPUTE_PSEUDO_ESN

DESCRIPTION
  This function creates the Pseudo-ESN (PESN) from
  the Mobile Equipment IDentifier (MEID).
  
  This function is provided for DIAG use to compute the Pseudo-ESN
  from a MEID.  
  
DEPENDENCIES
  None.

RETURN VALUE    
  Value of pesn

SIDE EFFECTS
  None.
 
===========================================================================*/
dword mcc_compute_pseudo_esn(qword);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mcc_compute_pseudo_esn */
#endif


#endif /* MCCMEID_H */

