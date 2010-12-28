#ifndef CMALS_H
#define CMALS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

C A L L   M A N A G E R   A L S   M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager ALS custom logic module
  (FEATURE_ALS - Alternate Line Service).

  ALS custom logic is responsible for mapping CM's generic LINE_N /
  line selection disabled/enabled indication into a set of fields in GSM LL
  messages and SIM that need to be read or written to convey the information as
  to what ALS setting they represent and vice versa.

  On MO call / SIM write, CM will request ALS custom logic to do the mapping
  of CM enums into GSM LL message / SIM fields and then send the request down
  to GSM stack / GSDI or MMGSDI.

  On MT call / SIM read, CM will request ALS custom logic to do the mapping
  of GSM LL message / SIM fields into CM ALS enums and then pass those to
  clients if needed.

  Currently ALS custom logic supports Orange-specific custom logic functions.
  All other vendor-specific ALS custom logic functions would need to be
  placed in this module as well.


EXTERNALIZED FUNCTIONS

  All functions listed in cmals.h

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmals.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
07/31/06   ic      Lint fixes
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   jar     Changed CMALS_CSP_ALS_ALLOWED_MASK from 0x40 to 0x80
09/02/04   ic      Synced up to MSMSHARED_CM.02.00.19.00.07 and its unit test
                   (to help with FEATURE_ALS integration on M6250CTMQG4230)
09/01/04   ic      Changes to FEATURE_ALS.
                   Added support for FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
07/12/04   ic      Initial release

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cm.h"
#include "cmi.h"
#include "comdef.h"
#include "sys.h"
#include "cmdbg.h"
#include "cmtask.h"
#include "cmclient.h"

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */
#endif /* CMALS_H */

