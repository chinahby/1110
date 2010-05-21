#ifndef _FM_API_H
#define _FM_API_H

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "custfmrds.h"           /* Include global Fmrds switches */
#include "queue.h"
#include "task.h"
#include "cmd.h"
#include "msg.h"
#include "msmhwioreg_port.h"     /* Added to track RDS ISR usage */

#ifdef  FEATURE_FM

// *** functional defines ***
#define FEATURE_FM_SERVICE_AVAILABLE  // support for legacy service available command

/* FMRDS specific files */
#include "fm_device.h"        /* FMRDS device specific definitions */


#endif // FEATURE_FM

#endif
