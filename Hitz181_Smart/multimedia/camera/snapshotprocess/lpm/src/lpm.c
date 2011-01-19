/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

               L U M A    P R O C E S S I N G   M O D U L E  (LPM)
  
GENERAL DESCRIPTION
  This module contains functions to apply vignette corection, luma histogram
  collection and adaptive spatial filtering on image files.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002-2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*======*/

/* <EJECT> */
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/lpm/main/latest/src/lpm.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/19/09   chai    Fixed the compilation warning 
12/24/08   cg      Changes for Ez Tune 1.0
11/03/08   cg      Added support for Ez Tune 1.0 , 3A 1.4 and 0x203 header.
11/21/07   cg      Added support for Chromatix header 0x201.
04/09/07   dlee    In lpm_process_cb(), filter out messages that we received
                   after operation is finished.
11/7/06    ven     For multi shot, Changed ASF/ChromaSuppression/Luma 
                   Adaptation profiling will reflect the latency numbers for
                   the last captured frame.
11/03/06   ven     Added latency logging for various modules.
10/19/06   dl      Register to clk regime as JPEG APPS.
10/19/06   dl      Moved luma adaptation to service layer.
10/06/05   dl      Invalidate data cache for large exteranl memory region.
09/19/06   dl      Moved luma histogram computation from DSP to ARM.
08/24/06   dl      Resolved racing condition in luma processing.
08/07/06   dl      Fixed color artifact in luma adaptation.
08/04/06   dl      Moved lpm parameters from camsensor to chromatix.
07/09/06   dl      Check for specialized 3x3 filter.
06/20/06   dl      Added LPM profiling information.
06/16/06   jv      Fixed luma adaptation pointers in lpm_setup_parameter 
06/06/06   dl      Set working histogram for image processing.
06/04/06   dl      Added support for Luma Adaptation.
05/18/06   dl      Remove segmentation artifacts from 3x3 luma filter.
04/12/06   dl      Missing parenthesis in type casting.
03/23/06   vma     Replaced all mallocs and frees to JPEG_MALLOC's and JPEG_FREE's
03/23/06   dl      Added Chroma suppression.
03/06/06   dl      Fix dark boder line in sw luma filter.
02/28/06   dl      Resolved Lint Issues.
11/03/05   anb     Added LPM_PROCESS_START to indicate that the LPM process
                   will start.
10/28/05   dl      Does not queue another operation when aborting is in process.
10/27/05   dl      Renamed lpm_terminate to lpm_abort. Removed callback pointer
                   requirment for abort.
09/15/05   dl      Resolved coverity issue.
09/01/05   dl      Removed lint warnings.
08/26/05   dl      Removed compiler warning.
08/17/05   dl      Added software 3x3 filter supports.
07/06/05   dl      Added the following feature flags 
                   FEATURE_CAMERA_LVCM
                   FEATURE_CAMERA_5x5_ASF
05/26/05   dl      Added 5x5 asf support
05/17/05   dl      Define LPM ony if camera is defined.
04/25/05   dl      Invalidate data cachelines before and after LPM.
03/15/05   dl      Added error checking
01/26/05   dl      Initial version

=============================================================================*/
/* <EJECT> */
/*=============================================================================

                       INCLUDE FILES FOR MODULE


=============================================================================*/

  #include "target.h"             /* Target specific definitions            */
  #include "customer.h"           /* Customer specific definitions          */
  #include "comdef.h"             /* Common definitions                     */
  #include "lpm.h"
  #include "vfe.h"
  #include "clk.h"
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  #include "camera_aec.h"
  #include "jpeg_malloc.h"
  #define jpeg_file_id MALLOC_LPM


/* Turn on this flag to display tuning messages */
/* #define DEBUG_LPM_TUNING */

/* Turn on this flag to display profiling messages */
#define LPM_PROFILING

  #include "lpmqdsp.h"
  #include "stdlib.h"
  #include "msg.h"                /* Message logging/reporting services     */
  #include "graph.h"              /* Graphics task typedefs and prototype   */
  #include "camera_svcs.h"

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */
