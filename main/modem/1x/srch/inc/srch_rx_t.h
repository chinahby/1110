#ifndef SRCH_RX_T_H
#define SRCH_RX_T_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ R X  ---  H E A D E R   F I L E 

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_rx.

EXTERNALIZED FUNCTIONS

  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2005 - 2010
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_rx_t.h#2 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
04/05/10   pk      Added invalid field to chain_name and freq_range enum types.
09/17/08   vks     Added FEATURE_SRCH_HIGHER_BAND_CLASSES_SUPPORTED_BY_RF to
                   cover band classes 16 through 19.
09/09/08   aps     Converted RF chains to enum type
09/03/08   aps     Corrected checkin error
09/03/08   aps     Corrected enum mapping for srch_rx_band_type
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
                   Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
06/20/08   mca     Merge from //depot
06/11/08   adw     Added Band Class 16, 17, 18, 19
09/25/06   tjc     Added Band Class 15
03/29/06   kwo     Added RX_REQUESTED to enum
01/16/06   awj     Lint cleanup
11/14/05   awj     Added Band Class 14 mapping
09/13/05   grl     Added RX_TUNING enum.
07/19/05   bt      Added srch_rx_bypass_rtc_sync_type and 
                   srch_rx_flush_samp_ram_type
07/19/05   grl     Added RX_TUNED enum to srch_rx_status_type. 
06/24/05   sfm     Moved NO_CHAIN define to srch_rx.c
06/09/05   sfm     more naming changes and coding standards changes 
06/03/05   sfm     srch_rx naming changes
05/31/05   rng     Merged from SHDR Sandbox.
05/31/05   sar     Removed definition of SRCH_RX_BC13 from srch_rx_band_enum_type.
01/20/05   sst     Implementation, first cut.
===========================================================================*/ 

#include "comdef.h"
#include "customer.h"
#include "rfm.h"

#ifndef FEATURE_SRCH_HAS_NO_TRM
#include "trm.h"
#endif /* FEATURE_SRCH_HAS_NO_TRM */

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#ifdef FEATURE_SRCH_SINGLE_ANTENNA
#define SRCH_RX_NUM_CHAINS 1
#else /* !FEATURE_SRCH_SINGLE_ANTENNA */
#define SRCH_RX_NUM_CHAINS 2
#endif /* FEATURE_SRCH_SINGLE_ANTENNA */

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
typedef enum
{
  NO_CHAIN = -1,   /* no chain for the specified client (error) */
  DEMOD_CHAIN = 0,
  #ifndef FEATURE_SRCH_SINGLE_ANTENNA
  ALT_CHAIN,
  #endif  /* FEATURE_SRCH_SINGLE_ANTENNA */
  MAX_CHAINS
} srch_rx_chain_name_type;

typedef enum
{
  SRCH_RX_INVALID_FREQ_RANGE = -1,
  SRCH_RX_450MHZ_FREQ_RANGE,  
  SRCH_RX_800MHZ_FREQ_RANGE,  
  SRCH_RX_2GHZ_FREQ_RANGE,
  SRCH_RX_MAX_FREQ_RANGE         /* must be last */

} srch_rx_freq_range_type;

typedef enum 
{
  RX_UNLOCKED,             /* 1X does not have the chain */
  RX_REQUESTED,            /* TRM request has been sent, but not received */ 
  RX_LOCKED,               /* 1X has the chain but it's not ready yet */
  RX_TUNING,               /* Receiver in the process of being tuned. */
  RX_WAITING_FOR_COMPLETE, /* Receiver tuned but AGCs not running. */
  RX_READY                 /* sample RAM is flushed and the chain is tuned */

} srch_rx_status_type;

/* REVISIT Update when rfcom.h is updated for the new CDMA2000 Band Clases 
   FEATURE_SRCH_HIGHER_BAND_CLASSES_SUPPORTED_BY_RF to be mainlined when
   rfcom.h is updated */
typedef enum
{
  SRCH_RX_BC0          = RF_BC0_BAND,
  SRCH_RX_BC1          = RF_BC1_BAND,
  SRCH_RX_BC2          = RF_BC2_BAND,
  SRCH_RX_BC3          = RF_BC3_BAND,
  SRCH_RX_BC4          = RF_BC4_BAND,
  SRCH_RX_BC5          = RF_BC5_BAND,
  SRCH_RX_BC6          = RF_BC6_BAND,
  SRCH_RX_BC7          = RF_BC7_BAND,
  SRCH_RX_BC8          = RF_BC8_BAND,
  SRCH_RX_BC9          = RF_BC9_BAND,
  SRCH_RX_BC10         = RF_BC10_BAND,
  SRCH_RX_BC11         = RF_BC11_BAND,
  SRCH_RX_BC12         = RF_BC12_BAND,
  SRCH_RX_BC13         = RF_BC13_BAND,
  SRCH_RX_BC14         = RF_BC14_BAND,
  SRCH_RX_BC15         = RF_BC15_BAND,
  #ifdef FEATURE_SRCH_HIGHER_BAND_CLASSES_SUPPORTED_BY_RF
  SRCH_RX_BC16         = RF_MAX_BAND,
  SRCH_RX_BC17         = RF_MAX_BAND,
  SRCH_RX_BC18         = RF_MAX_BAND,
  SRCH_RX_BC19         = RF_MAX_BAND,
  #endif /* FEATURE_SRCH_HIGHER_BAND_CLASSES_SUPPORTED_BY_RF */
  SRCH_RX_GPS          = RF_GPS_BAND,
  SRCH_RX_FM           = RF_FM_BAND,
  SRCH_RX_MAX          = RF_MAX_BAND,
  SRCH_RX_NO_TUNE_BAND = 0xFF

} srch_rx_band_type;

typedef uint16 srch_rx_chan_type;         /* Channel */

typedef enum
{
  SRCH_RX_ENABLE_BYPASS_RTC_SYNC,
  SRCH_RX_DISABLE_BYPASS_RTC_SYNC

} srch_rx_bypass_rtc_sync_type;

typedef enum
{
  SRCH_RX_FLUSH_SAMPLE_RAM,
  SRCH_RX_DO_NOT_FLUSH_SAMPLE_RAM

} srch_rx_flush_samp_ram_type;


#endif /* SRCH_RX_T_H */
