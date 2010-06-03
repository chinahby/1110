#ifndef CMWSUPS_H
#define CMWSUPS_H

/*===========================================================================

       C A L L   M A N A G E R

              S U P P L E M E N T A R Y   S E R V I C E S

                     H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface with
  the Call Manager Sups module (CMWSUPS.C)

Copyright (c) 2001 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmwsups.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   mp      Updated SUPS event notification in SUSPEND state
11/08/05   sk      Separated CM reports and commands.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
02/06/04   ws      Initial jaguar/mainline merge.
04/11/03   prk     Removed references to FEATURE_SUPS.
01/21/03   ws      Updated copyright information for 2003
05/07/02   PRK     Renamed to cmwsups.h.
09/25/01   RI      Initial release.

===========================================================================*/
#include "cmi.h"

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

#endif /* CMWSUPS_H */

