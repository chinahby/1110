/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "custfmrds.h"          /* Include global fm rds switches */
#include "comdef.h"             /* Definitions for byte, word, ... */
#include "target.h"             /* Target specific definitions */

#ifdef FEATURE_FM

#include "task.h"
#include "tmc.h"
//#include "bio.h"
#include "err.h"                /* Error logging macros */
#include "clkregim.h"
#include "dog.h"
//#include "fs.h"
#include "mc.h"
#include "queue.h"
#include "rex.h"
#include "clk.h"                /* Clock services header */
#include "clki.h"               /* Clock services internal header */
//#include "dsati.h"

//fmr specific files
//#include "fm_device.h"     // fmrds device declarations
#include "fm_api.h"          // fmrds api efinitions
#include "fm_generic.h"      // generic definitions for specific fmrds devices


#endif //FEATURE_FM

