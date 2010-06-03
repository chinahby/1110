#ifndef MCCKEY_H
#define MCCKEY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
                    
                       AKA/AES KEY MODULE
   
GENERAL DESCRIPTION

  This is the module for key management for AKA/AES

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.h_v   1.7   27 Jun 2002 19:49:36   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mcckey.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/06   fh      Removed FEATURE_IS2000_REL_A_AES_ECK
07/26/06   fh      Added a new feature FEATURE_IS2000_REL_A_AES_ECK to keep 
                   the key schedule (expanded CK) in buffer for future use 
                   until the key is invalidated or regenerated.
07/24/06   fh      Cleaned up unused functions after code review
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/17/06   fh      Merged in new development of AKA/AES features
05/26/06   fh      create this module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "comdef.h"


#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
#endif /* MCCKEY_H */

