/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                       D S M   P O O L   P R O C E S S I N G

DESCRIPTION
  This file contains definitions for processing packets in MVS DSM pools.

REFERENCES
  None.

Copyright (c) 2002 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvsdsm.c#1 $ $DateTime: 2008/05/19 03:35:58 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/07   kan     Added AMR-WB changes 
03/18/06    ay     Cleaned up lint errors.
                   Updated Copyright.
12/14/05   ymc     Fixed compiler warning for predicted_frame_type not used.
04/28/05   ymc     Modified mvs_dsm_amr_predict_dl so that MVS will always
                   use frame type information from L1 instead of running
                   prediction automatically when CRC status is bad.
                   Featurised automatic prediction under MVS internal feature
                   MVSF_WCDMA_AMR_AUTO_PREDICT_CRC_FRAME.
           ymc     Modified mvs_dsm_amr_predict_dl so that MVS will drop the
                   first good frame if CRC error rate is too high for a moment.
                   Featurised under MVS internal feature 
                   MVSF_WCDMA_AMR_CRC_FRAME_WINDOW.
           ymc     Updated Copyright.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
06/10/04   ymc     If there are too many downlink packets in DSM queues,
                   dump the rest instead of calling ERR_FATAL.
04/17/03    sm     Added amrsup support.
            sm     Refeaturized code.
03/03/03   ymc     Cleaned up all lint errors and warnings.
01/24/03   ymc     Updated copyright.
01/16/03    sm     Removed unused variable in mvs_dsm_amr_dl_proc().
12/16/02   ymc     Changed the uplink packet loss message level from ERROR to
                   HIGH.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include <string.h>

#ifdef FEATURE_MVS
#include "msmaud.h"
#include "mvsi.h"

#if (defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_AMR_WB)) \
    && defined(FEATURE_WCDMA)
#error code not present
#endif  /* (MSMAUD_VOC_AMR || MSMAUD_VOC_AMR_WB) && FEATURE_WCDMA */
#endif  /* FEATURE_MVS */
