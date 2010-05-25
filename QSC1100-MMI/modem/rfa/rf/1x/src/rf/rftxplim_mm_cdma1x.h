#ifndef RFTXPLIM_MM_CDMA1X_H
#define RFTXPLIM_MM_CDMA1X_H
/* EJECT   */
/*===========================================================================

                RF Transmit Power Limit Services Header File

GENERAL DESCRIPTION
     This header file contains all the definitions necessary in order to
  interface with limit CDMA transmit power.

Copyright (c) 2007             by QUALCOMM, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rftxplim_mm_cdma1x.h#2 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/08   vm      Added suppport for PDET Waveform Compensation
06/03/07   hkk     1x, DO and temp waveform compensation
02/19/07   rmd     Initial revision.

===========================================================================*/

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*===========================================================================

FUNCTION rf_get_exp_tx_agc_vs_hdet                  EXTERNAL FUNCTION

DESCRIPTION
  This function does the following:
  1) Converts the raw HDET ADC value into a TX AGC (TX power) 
     in 10th of a dBm. 
  2) Compensates the results depending of HDET temperature and
     the radio configuration.

DEPENDENCIES
  None.

RETURN VALUE
  TX AGC (TX power) in 10th of a dBm. 

SIDE EFFECTS
  None

===========================================================================*/

#ifdef RF_HAS_HDET_UINT16
int16 rf_get_exp_tx_agc_vs_hdet(uint16 rf_hdet_read);
#else
int16 rf_get_exp_tx_agc_vs_hdet(uint8 rf_hdet_read);
#endif
/*===========================================================================

FUNCTION rf_get_exp_tx_agc_vs_hdet_for_hdr                  EXTERNAL FUNCTION

DESCRIPTION
  This function does the following:
  1) Converts the raw HDET ADC value into a TX AGC (TX power) 
     in 10th of a dBm. 
  2) Compensates the results depending of HDET temperature and
     RRI
	 
DEPENDENCIES
  None.

RETURN VALUE
  TX AGC (TX power) in 10th of a dBm. 

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rf_get_exp_tx_agc_vs_hdet_for_hdr(uint8 rf_hdet_read);

/*===========================================================================

FUNCTION rf_req_update_hdet_temp_value              EXTERNAL FUNCTION

DESCRIPTION
  This function will force HDET temperature to be read/updated from ADC 
  next time rf_get_exp_tx_agc_vs_hdet() is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_req_update_hdet_temp_value(void);
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#endif /* RFTXPLIM_MM_CDMA1X_H */
