/*===========================================================================

        Pacing Arbiter   F O R   Q S C 6 0 5 5

DESCRIPTION
  Test code

REFERENCES

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/parb/parb_test.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/07   fs      Initial creation (test code )
===========================================================================*/


#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "msm.h"                /* Includes for specific HW mappings    */
#include "tramp.h"              /* Error logging macro                  */
#include "err.h"                /* Error logging macro                  */

#include "clk.h"
#include "parb.h"               /* Pacing Arbiter public file           */
#include "parbi.h"              /* Pacing Arbiter private file          */

#ifdef FEATURE_SIMULATE_LACK_OF_BW_TO_GPS
#error code not present
#endif /* FEATURE_SIMULATE_LACK_OF_BW_TO_GPS */
