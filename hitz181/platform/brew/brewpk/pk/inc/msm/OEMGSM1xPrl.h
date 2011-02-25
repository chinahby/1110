#ifndef OEMGSM1X_PRL_H
#define OEMGSM1X_PRL_H

/*===========================================================================

        OEMGSM1x Preferred Roaming List

DESCRIPTION
  This file contains GSM1x Preferred Roaming List declarations for targets that
  support SD 2.0.

Copyright (c) 2003 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#ifdef FEATURE_SD20

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION gsm1x_prl_get_size

DESCRIPTION
  This function retuns the size of the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  PRL size. If prl_ptr is NULL it will return 0.

SIDE EFFECTS
  None.

===========================================================================*/
word gsm1x_prl_get_size(
    byte  *prl_ptr
);


#endif /* ! FEATURE_SD20 */


#endif /* OEMGSM1X_PRL_H */
