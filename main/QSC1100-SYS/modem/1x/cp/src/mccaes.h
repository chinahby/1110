#ifndef MCCAES_H
#define MCCAES_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
                    
                       ADVANCED ENCRYPTION STANDARD
   
GENERAL DESCRIPTION

  This is the module for Advanced Encryption Standard.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccaes.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/06   fh      Moved CRC calculation function into mccsup.c
09/15/06   fh      Added a function to set LAC fields of TC messages
09/02/06   fh      Miscellaneous code review changes
08/04/06   fh      Cleaned up the usage of AES capability functions
07/26/06   fh      Added a new feature FEATURE_IS2000_REL_A_AES_ECK to keep 
                   the key schedule (expanded CK) in buffer for future use 
                   until the key is invalidated or regenerated.
                   Removed the middle layer function of mccaes_privacy_key.
07/24/06   fh      Added back the special validation function to check encrypt_mode
                   for UHDM after the code review. Also added a new function to
                   check ui_encrypt_mode of connection records in UHDM.
07/07/06   fh      Cleaned up AES capability helper functions, and removed the 
                   UHDM AES field validating function
07/06/06   fh      Added a function to specially check encrypt_mode for UHDM
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/17/06   fh      Merged in new development of AKA/AES features
01/26/06   fh      create this module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "comdef.h"


#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */
#endif /* MCCAES_H */

