#ifndef SLEEPMOD_KEYPRESSI_H
#define SLEEPMOD_KEYPRESSI_H
/*===========================================================================

              S L E E P M O D    K E Y P R E S S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Modem.
  This module is used in the portable only.

                             Copyright (c) 2008 
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepmod_keypressi.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   clm     1H08 sleep re-work.  Initial version.

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SLEEP_INSTALL_KEYPRESS_ISR

DESCRIPTION
  This installs the keypress ISR for targets that have keyboards

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleep_install_keypress_isr( void );

#endif /* SLEEPMOD_KEYPRESSI_H */

