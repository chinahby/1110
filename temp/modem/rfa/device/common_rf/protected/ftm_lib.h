#ifndef FTM_LIB_H
#define FTM_LIB_H

/*===========================================================================

                      External FTM Library Header File

DESCRIPTION
  External interface to the FTM library.


Copyright (c) 2004      by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/rfa/device/common_rf/main/qsc1100/protected/ftm_lib.h#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/06   vm      Added the prototypes of new APIs for new IM2 cal algorithm
03/20/05   bmg     Added ftm_calibrate_intelliceiver API
10/31/04   ra      Added support for ftm_get_im2_with_suppression
07/01/04   bmg     Created.  Collected library APIs into one file.
===========================================================================*/

#include "rfcom.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  byte i_dac;
  byte q_dac;
  byte transconductor;
  int2 suppression_level;
} im2_type;

/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION FTM_GET_IM2

DESCRIPTION  
   Library entry point for new IM2 calibration algorithm based on independent 
   I and Q DAC Codes.Vectors to the desired IM2 search algorithim.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   Sets the best I and Q values in the im2CalData structure
===========================================================================*/
void ftm_get_im2(rfcom_device_enum_type device, im2_type *im2CalData);

/*===========================================================================
FUNCTION FTM_GET_LEGACY_IM2

DESCRIPTION  
   Library entry point for legacy IM2 calibration algorithm.Vectors to the desired IM2
   search algorithim.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   Sets the best I and Q values in the im2CalData structure
===========================================================================*/
void ftm_get_legacy_im2(rfcom_device_enum_type device, im2_type *im2CalData);

/*===========================================================================

FUNCTION ftm_get_CtoN

DESCRIPTION  
   Measures the ratio of the strongest in-band signal to the noise, and
   returns the result in 1/10th of a dB resolution.

DEPENDENCIES
   RF mode and channel must be set before calling.

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
int ftm_get_CtoN(rfcom_device_enum_type device);

/*===========================================================================

FUNCTION FTM_GET_IM2_WITH_SUPPRESSION

DESCRIPTION  
   Library entry point for new IM2 calibration routine which not only
   returns the best IDAC and QDAC Codes but also returns the improvement 
   between the un-calibrated (IM2 circuitry disabled) and the calibrated 
   in terms of dB

DEPENDENCIES
   None

RETURN VALUE
   Boolean

SIDE EFFECTS
   Sets the best I and Q values in the im2CalData structure, and reports
   the suppression level.It uses the new IM2 calibration algorithm based on 
   assumption of independence of QDAC and QDAC codes.

===========================================================================*/
boolean ftm_get_im2_with_suppression(rfcom_device_enum_type device, im2_type *im2_cal_data);

/*===========================================================================

FUNCTION FTM_GET_LEGACY_IM2_WITH_SUPPRESSION

DESCRIPTION  
   Library entry point for legacy IM2 calibration routine which not only
   returns the best IDAC and QDAC Codes but also returns the improvement 
   between the un-calibrated (IM2 circuitry disabled) and the calibrated 
   in terms of dB

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   Sets the best IDAC and QDAc values in the im2CalData structure, and reports
   the suppression level.It uses the legacy IM2 calibration algorithm to calculate
   best DAC codes.

===========================================================================*/
boolean ftm_get_legacy_im2_with_suppression(rfcom_device_enum_type device, im2_type *im2_cal_data);

/*===========================================================================

FUNCTION FTM_CALIBRATE_INTELLICEIVER

DESCRIPTION  
   Calibrate chip dependent IntelliCeiver parameters

DEPENDENCIES
   RF mode and channel must be set, as well as the specified IntelliCeiver
   calibration signal present.

RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
boolean ftm_calibrate_intelliceiver(rfcom_device_enum_type device,
                                    int2 power, PACKED void *data, int length);


#endif /* FTM_LIB_H */
