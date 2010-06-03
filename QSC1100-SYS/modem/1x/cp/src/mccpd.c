/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M A I N  C O N T R O L
                          
        P O S I T I O N   D E T E R M I N A T I O N   S E R V I C E S 

GENERAL DESCRIPTION

  This file contains the support for Position Determination services.

EXTERNALIZED FUNCTIONS
  mcc_pd_init - Register the position determination service options with
                service negotiation manager (SNM).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  mcc_pd_init needs to be called during the powerup.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccpd.c_v   1.14   02 Oct 2002 13:40:22   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccpd.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
12/14/07   an      Added Passport Mode
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
12/12/06   pg      Fixed Lint error.
10/15/06   fc      Code review comments.
07/17/06   fc      Optimize the implementation of pseudo bs.
07/10/06   fc      Added support for pseudo bs.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
12/10/04   sb       Fixed lint error.
01/10/05   va      Merged lint fixes from REL_B_C archive
09/27/04   va      Merged following from REL B/C archive
07/08/04   bkm      Correct setting of so_bitmap for SO23 (RS1 PD).
07/01/04   va       Corrected lint errors
06/18/04   ank      Mainlined FEATURE_GPSONE_DBM.
06/30/04   bkm     Lint cleanup.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged the following from msmshared:
                   03/05/04   bkm     Lint cleanup.
10/14/03   is      Added task lock on critical data in FEATURE_GPSONE_BS_INFO
10/10/03   ht      Included ts.h for ts_get().
08/01/03   yll     Added Dynamic Feature support.
11/25/02   sk      Merged force dormancy changes from MSM archive,  new logic
                   for deciding the anchors.
10/02/02   az      Mainlined FEATURE_IS2000
03/09/02   lh      Merged from common archive.
  02/07/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper 
                     Base Station ID.                    
02/22/02   fc      Added support for Release A common channels.
02/10/02   yll     Merged from common archive CP VU 116
                   01/31/02   hxw     
                   Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper 
                   Base Station ID.                    
10/01/01   kk      Merge from common archive CP VU 96.
                   08/14/01   jrp     Changed references to DEC_RATE... 
                   to CAI_RATE...
08/16/01   kk      Merge from common archive CP VU 89.
                   08/10/01   ph    Include CHIND_LIST in neg pars for SO0x23 
                   and 0x24.
08/10/01   kk      SO initialization modified.
06/26/01   sb      Merged following from common archive:
                   05/31/01   abh     Send the SID/NID from paging channel.
04/19/01   kk      Fixed typo
03/14/01   kk      IS2000 related initialization.
02/27/00   kk      SO initialization added for group/bitmap info.
11/30/00   yll     Changed the min_p_rev for SO 35/36.
10/06/00   yll     Fixed the compiler warning.
06/28/00   yll     Initial revision.
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */
