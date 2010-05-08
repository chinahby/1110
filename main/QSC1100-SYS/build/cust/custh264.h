#ifndef CUSTH264_H
#define CUSTH264_H
/*===========================================================================

            " M S M 6 0 5 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target using MSM6050 ARM Processor.
  This is a Trimode build with IS2000.
  This particular build supports EFS, H264 and AAC

  Copyright (c) 2001,2002   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/qtv/build/cust/rel/ULC/1.0/custh264.h#2 $ $DateTime: 2008/07/14 04:45:12 $ $Author: kediya $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/1/02    anr     Initial creation
===========================================================================*/

/* Enable H264 decoder
*/
#define FEATURE_H264_DECODER
#define FEATURE_QTV_H264
#define FEATURE_H264D_DECODER
#define FEATURE_QTV_QCIF_ENABLE

#endif /* CUSTH264_H */

