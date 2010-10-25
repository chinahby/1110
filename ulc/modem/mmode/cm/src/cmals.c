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

  Currently ALS custom logic supports default and Orange-specific
  custom logic functions.
  All other vendor-specific ALS custom logic functions would need to be
  placed in this module as well.


EXTERNALIZED FUNCTIONS

  All functions

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited. 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmals.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
08/07/07   ic      Lint clean up
09/05/06   ic      If we're in USIM mode and failed to write USIM ALS file, 
                   switch to its corresponding GSM file (CR#100334)
08/08/06   ic      If we're in USIM mode and failed to read USIM ALS file, 
                   switch to its corresponding GSM file (CR#97222)
07/31/06   ic      Lint fixes
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/13/05   ic      Fixed compiler warning "..\..\services\cm\cmals.c", 
                   Warning: C2871W: static 'cmals_dummy' declared but not used
12/11/05   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
11/12/04   ic      Fixed RVCT 2.1 compiler warnings / Lint clean up
11/11/04   ic      Lint cleanup 
09/30/04   ic      Fixed ADS 1.2 compiler warnings: removed als_ptr variables
                   that were initialized but never used afterwards.
                   Fixed RVCT 2.1 compiler warnings: "#188-D: enumerated type
                   mixed with another type": used UIM_NO_SUCH_ITEM to initialize
                   filename instead of 0x00
09/27/04   ic      Added cmals_dummy() to get rid of ADS 1.2 compiler warning
                   "Warning: C3040E: no external declaration in translation
                   unit" - apparently it got lost in the merge
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/04/04   ic      Removed target.h as it's not needed by the file.
                   Added cmals_dummy() to get rid of ADS 1.2 compiler warning
                   "Warning: C3040E: no external declaration in translation unit"
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/02/04   ic      Merged in bug fixes for problems found during integration
                   tests
09/02/04   ic      Synced up to MSMSHARED_CM.02.00.19.00.07 and its unit test
                   (to help with FEATURE_ALS integration on M6250CTMQG4230)
09/01/04   ic      Changes to FEATURE_ALS.
                   Added support for FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
07/12/04   ic      Initial release

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h)*/
#include "customer.h"  /* Customer configuration file */

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#else /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

/*===========================================================================
FUNCTION cmals_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmals_dummy( void )
/*lint -esym(714,cmals_dummy) */
/*lint -esym(528,cmals_dummy) */
/*lint -esym(765,cmals_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */





