#ifndef FTMICAP_H
#define FTMICAP_H

/*===========================================================================

             F T M   C a p a b i l i t i e s   H e a d e r   F i l e

DESCRIPTION
  This header file that defines FTM capabilities.  
  This header file is internal to the FTM unit.

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007, 2008             by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rficap.h_v   1.1   09 Apr 2002 19:54:24   ehandojo  $
$Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftmicap.h#8 $ $DateTime: 2008/10/13 19:21:44 $ $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/08   vm      Merged from //depot/asic/qsc1100/modem/ftm/core/inc/ftmicap.h#1
2008-07-07 as      Updated with FEATURE_FTM_FMRDS
06/16/08   vb      Changed the #define from T_QSC11x0 to T_QSC1100
05/05/08   vb      Corrected the HW resolution for Beta Prime
04/18/08   vb      Added support for Beta Prime HW resolution for QSC11X0 & QSC60X5
11/28/07   ems     Support For QSC1100
11/21/07   vm      Added more support in FTM for CGPS targets
11/08/07   vm      Added support for FTM API supporting CDMA Waveform, 
                   CW Waveform and Offset CW Waveform for Tx
11/08/07   ans     Added BC10 support for FTM.
10/03/07   bmg     Introduced target-dependant ftm capabilities file.  If 
                   FTMICAP_HEADER is defined in the builds file it will be
                   used instead of the body of this header.
08/13/07   dyc     Added FTM_HAS_VREG_VOTE for QSC60x5
08/01/07   kg      featurization changes for EDGE only support
08/02/07   dyc     Added WLAN support for QSC60x5
07/13/07   anr     Added support for XO cal feature.
05/11/07   dp      Add support for FTM single ended BER.
04/10/07   vm      Added teh support for Tx Tone for SC2X
03/06/07   aak     Define FTM_HAS_AGC_CARRIER_FREQ_ERR for MSM6800B
01/16/07   tws     Make FTM_HAS_MBP dependent on MBP_HW.
01/10/07   aak     Featurize the FTM Hard Handoff feature (FTM_HAS_SRCH_HHO)
12/11/06   anb     Added QFUSE programming support for 6800B.
12/11/06   dbc     Chnage FTM_MFLO_SUPPORTS_FLO_DEMOD to FTM_MFLO_HAS_NON_SIGNALING
12/07/06   bt      Added support or MBP (multi broadcast platform).
11/07/06   jtn(rs) Do not define FTM_HAS_AUDIO for 7200 THIN_UI build.
10/26/06   dbc     Support FTM_MFLO_SUPPORTS_FLO_DEMOD for T_MSM6280 when
                   FEATURE_MFLO is defined
10/20/06   dbc     Added FTM_MFLO_SUPPORTS_FLO_DEMOD for FEATURE_MFLO
09/26/06   hkk     Added support for BC15 featurization
07/06/06   anb     Added support to blow fuses via FTM commands.
07/20/06   et      moved #define FTM_HAS_GSM to end of file
07/13/06   xw      Added 7200 support. 
06/26/06   et      GSM quadband support
06/26/06   dbc     Added support for FTM_HAS_MFLO on UMTS Targets
06/13/06   ra      Added FTM_HAS_WLAN on UMTS Targets
05/23/06   vm      Added feature FTM_HAS_HDR_ENHANCED_ACCESS_CHANNEL to support 
                   enhanced access channel in HDR non signaling mode
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/12/06   ra      Add support for FTM_HAS_WLAN on 1x Targets
04/10/06   vm      Added new feature FTM_USES_NEW_IM2_SEARCH_METHOD to support
                   new IM2 Calibration Algorithm.
03/15/06   zg      Defined ZIF_LIBRARY_SUPPORTS_IM2_API for pre MSM6800 targets.
01/10/06   lp      Added FTM_HAS_AGC_CARRIER_FREQ_ERR to adjust
                   TRK_LO_ADJ_VCTCXO PDM.
11/18/05   lp      For MSM7500, enabled FTM_HAS_TX_DACC_CONTROL and
                   FEATURE_FTM_CONFIG_TX_SWEEP_CAL defines and removed
                   ZIF_LIBRARY_SUPPORTS_IM2_API.
11/14/05   dyc     Updates to allow this file to be picked up for MSM6500
11/14/05   zg      Added ZIF_LIBRARY_SUPPORTS_IM2_API for MSM6500
10/24/05   ra      Added FTM_HAS_TX_DACC_CONTROL for relevant target
09/05/05   ra      Create UMTS and 1x sections.
                   Enable FTM_HAS_HDR_REV_A based on FEATURE_HDR_REVA_L1
09/05/05   ra      Define FEATURE_FTM_CONFIG_TX_SWEEP_CAL. This will enable FTM_HAS_CFG_TX_SWP_CAL. 
                   The user may choose remove FEATURE_FTM_CONFIG_TX_SWEEP_CAL which will
                   enable FTM_LEGACY_TX_SWEEP_CAL
06/13/05   ra      Added FTM_HAS_CFG_TX_SWP_CAL 
05/16/05   et      added FTM_HAS_I2C under FEATURE_FTM_I2C under MSM6500
05/02/05   dbc     Added FTM_HAS_MFLO, predicated by FEATURE_MFLO.
03/11/05   ka      Added FTM_HAS_GSM_EXTENDED for Raven.
02/09/05   rsr     Added FTM_HAS_MULTI_SLOT for raven
12/07/04   ra      Removed obsolete FTM_HAS_AGC_LOGGING definition from 1x projects.
                   This feature has been mainlined.
11/23/04   ra      Removed I2C support from 1x projects
10/13/04   lp      Removed FEATURE_CAMERA_V7 to fix MSM6550 FTM camera 
                   compile errors.
10/12/04   jac     Removed unneeded FTM_HAS_ASYNC_PKT definitions.
10/12/04   lp      Turned off FTM_HAS_CAMERA feature for MSM6550 when
                   FEATURE_CAMERA_V7 is defined.
10/06/04   lp      Added FTM_HAS_CAMERA
08/22/04   ra      Merge 1X and UMTS
08/20/04   xw      Removed FEATURE_FTM_BT and added FTM_HAS_I2C. 
10/10/03   sw      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target definitions   */
#include "customer.h"   /* Customer definitions */

#ifdef FTMICAP_HEADER
  #include FTMICAP_HEADER
#else

/*===========================================================================

                         UMTS Projects

===========================================================================*/
#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#if defined (FTM_HAS_WCDMA) || defined (FTM_HAS_GSM)
#error code not present
#endif

#if defined (T_MSM6250)
#error code not present
#endif 

#if defined (T_MSM6275)
#error code not present
#endif

#if defined (T_MSM6280)
#error code not present
#endif

#if defined(FEATURE_MBP_HW)
  #define FTM_HAS_MBP
#endif
/*===========================================================================

                         1X Projects

===========================================================================*/

#if defined(T_MSM6500) && !defined(T_MSM7200)
  #define FTM_HAS_1X
  #define FTM_HAS_1X_CAL_V2
  #define FTM_HAS_LOGGING  
  #define FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS    
  //#define FTM_HAS_AUDIO 
  #define FTM_USES_NEW_IM2_SEARCH_METHOD
  
  #if defined(FEATURE_BT)
    //#define FTM_HAS_BT
  #endif  

  #if defined(FEATURE_WLAN_FTM)
#error code not present
  #endif
  
  #if defined(FEATURE_CAMERA)
    //#define FTM_HAS_CAMERA
  #endif  
    
  #if defined(FEATURE_MFLO)
#error code not present
  #endif

  #if defined(FEATURE_FTM_I2C)
    #define FTM_HAS_I2C
  #endif

  /* Disabled for MSM6500 but enable for all later targets */
  #if defined(T_MSM6550) || defined(T_MSM7500)
    #define FEATURE_FTM_CONFIG_TX_SWEEP_CAL
  #endif

  #if defined(FEATURE_FTM_CONFIG_TX_SWEEP_CAL)
    #define FTM_HAS_CFG_TX_SWP_CAL
  #else
    #define FTM_LEGACY_TX_SWEEP_CAL
  #endif
  
  #if defined( FEATURE_HDR_REVA_L1)
#error code not present
  #endif
  
#endif /* T_MSM6500 && !T_MSM7200 */

#if defined (T_MSM6800)
  #define FTM_HAS_TX_DACC_CONTROL
#endif 

#if !defined(T_MSM6800)
  #define ZIF_LIBRARY_SUPPORTS_IM2_API
#endif

#if defined (T_MSM7500B) || defined (T_MSM6800B)
  #define FTM_HAS_AGC_CARRIER_FREQ_ERR
#endif

#if defined(FEATURE_DSPDIAG) 
  #define FTM_HAS_DSPDIAG
#endif  

#if defined(T_MSM6800B)
  #define FTM_HAS_HDR_ENHANCED_ACCESS_CHANNEL
  #define FTM_HAS_SUPPORT_FOR_DAST
  #define FTM_HAS_SUPPORT_FOR_BC10
  #define FTM_HAS_SUPPORT_FOR_BC15
  #define FTM_HAS_SRCH_HHO
  #if defined (FEATURE_QFUSE_PROGRAMMING)
    #define FTM_HAS_QFUSE
  #endif  
#endif

#if defined (T_QSC60X5)
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif

#if defined (T_QSC1100)
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif
#undef  FTM_HAS_SUPPORT_FOR_BC10
#undef  FTM_HAS_SUPPORT_FOR_BC15
#endif

#if defined(FEATURE_WLAN_FTM)
#error code not present
#endif
#endif /* T_QSC60X5 */

#ifdef FEATURE_GSM
#error code not present
#endif

#if defined (FEATURE_CGPS)
#error code not present
#endif

#if defined (FEATURE_XO_FACTORY_CAL)
#if defined (T_QSC60X5)
    #if !defined (T_QSC60X5REV1_0)
        #define FTM_HAS_XO_FACTORY_CAL
    #endif
#endif /* T_QSC60X5 */
#endif /* FEATURE_XO_FACTORY_CAL */

#if defined (T_QSC1100)
  #define FTM_BETA_PRIME_HW_RESOLUTION 4096
#elif defined (T_QSC60X5)
  #define FTM_BETA_PRIME_HW_RESOLUTION 256
#else
#endif

#if defined(T_MSM7600)
#error code not present
#elif  defined(T_MSM7500) 
#error code not present
#elif  defined(T_MSM6800B) && (!defined(T_QSC60X5))
#error code not present
#else
/*Dont Define anything*/
#endif
#endif /* else FTMICAP_HEADER */
#ifdef FEATURE_FM
#define FEATURE_FTM_FMRDS
#endif
#endif /* FTMICAP_H */


