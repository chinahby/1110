#ifndef UIMUSB_H
#define UIMUSB_H
/*===========================================================================

                   F O R W A R D   D E C L E R A T I O N S   F O R
                      U I M   U S B   L A Y E R

DESCRIPTION
  This file contains all the forward declarations for the UIM USB layer

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimusb.h#2 $ $DateTime: 2009/02/04 21:12:01 $ $Author: viveks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/09   vs      Added access to historical bytes in ATR
10/07/08   vs      Reduced UIM_USB_MAX_NUM_DATA_BLOCK_RETRIES from 1000 to 50
09/23/08   js      Added LTK Support for USB UICC
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/15/08   vs      Added support for UIM USB ICCD pre-suspend timer
09/05/08   vs      Added support for cool down period between failed USB
                   enumeration and ISO fallback. Removed prototype for fallback
                   function that is no longer externalized
08/22/08   vs      Added functions to allow rex_sleep() removal. Added support
                   for powerup waiting time and ISO fallback
08/22/08   js      Re-designed R-APDU status word processing
08/05/08   vs      Moved declarations from uimusb source to header file. Added
                   declaration for uim_usb_is_usb_atr_received()
07/18/08   vs      Moved uimdrv.h header inclusion out of featurization
                   Added rex.h header inclusion
07/17/08   vs      Created the UIM USB module to support the UIM USB UICC
                   ICCD interface

===========================================================================*/

#include "customer.h"
/* The uimdrv.h header inclusion should be kept out side the featurization.
   This is to ensure that when FEATURE_UIM_USB_UICC is defined in target
   specific cust files it gets included */
#include "uimdrv.h"

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
#endif /* UIMUSB_H */

