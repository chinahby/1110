#ifndef _FMRDS_DEVICE_H
#define _FMRDS_DEVICE_H

/*============================================================================
 @file fmrds_device.h

 File defining specific hardware device used for FM/RDS reception and (if
 supported) transmission.


 Copyright (c) 2008 QUALCOMM Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
 ============================================================================*/
 
/*=============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order. Please
 use ISO format for dates.

 $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/inc/fmrds_device.h#1 $$DateTime: 2008/07/02 11:52:20 $$Author: rmortime $

 when       who	  what where, why
 ---------- ---   -------------------------------------------------------------
 2008-07-02 rjm	  Modded for initial 4703 release.
 2008-05-22 rjm	  Initial release.

=============================================================================*/

// note: fmrds_device.h not currently used to determine physical fm/rds device
// - placemarker file

//
// potential physical fm devices
//

// *** qualcomm soc ***
#undef  TAVARUA
#define TAVARUA   0


// *** silabs ***
#undef  SI4703
#define SI4703    1

#undef  SI4721
#define SI4721    0

// *** nxp ***
#undef  TEA5764
#define TEA5764   0

#undef  TEA5766
#define TEA5766   0

// Flag to run TEA5766 code on TEA5764 asic
#undef  RUN_5766_CODE_ON_5764
#define RUN_5766_CODE_ON_5764   0


#endif /* #ifndef _FMRDS_DEVICE_H */

