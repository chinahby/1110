#ifndef MVSDSM_H
#define MVSDSM_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                       D S M   P O O L   P R O C E S S I N G

DESCRIPTION
  This header file defines interfaces for processing packets in MVS DSM pools.

REFERENCES
  None.

Copyright (c) 2002 - 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvsdsm.h#1 $ $DateTime: 2008/05/19 03:35:58 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/05   ymc     Removed MSMAUD dependencies.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
04/17/03    sm     Added amrsup support.
            sm     Refeaturized code.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_MVS

#if defined(FEATURE_WCDMA)
#error code not present
#endif  /* FEATURE_WCDMA */
#endif  /* FEATURE_MVS */
#endif  /* MVSDSM_H */

