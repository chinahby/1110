#ifndef RFICAP_MM_H
#define RFICAP_MM_H

/*===========================================================================


           R F  M U L T I M O D E   D E F I N I I T I O N   F I L E

DESCRIPTION
  This file contains definitions and macros for multimode operation 

Copyright (c) 2006,2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rficap_mm.h#1 $
$DateTime: 2008/04/11 05:11:57 $
$Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/07   ems     Support for QSC1100
08/06/07   jfc     Featurized code for EDGE-only builds
03/06/07   dyc     Support for RF_HAS_QSC60X5
03/06/07   ra      Moved GPS features to rficap_mm_gps header file
02/27/07   go      Added RF_HAS_MSM7600
02/12/07   ra      Added RF_HAS_WCDMA_AND_1X
01/17/07   jfc     Added RF_HAS_LINEAR_INTERPOLATION
12/15/06   jfc     Changed RF_HAS_WCDMA_AND_1X to RF_HAS_MULTIMODE_RXF_SEL
11/15/06   ra      Initial version. Baseline from rf_mm.h header

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#if defined (FEATURE_GSM) && !defined (FEATURE_WCDMA)
#error code not present
#endif 

#ifdef FEATURE_CDMA1X
#define RF_HAS_FEATURE_CDMA1X
#endif

#if defined (RF_HAS_FEATURE_CDMA1X) && defined ( RF_HAS_FEATURE_WCDMA )
#error code not present
#endif 

/* Define for MSM7600, MSM7200A, and MSM7500A, all covered under T_MSM7600 */
#if defined(T_MSM7600)
#error code not present
#endif

/* Enable GPS if FEATURE_GPS_MODE is defined for non-Aries targets or
   FEATURE_CGPS for targets with Aries Converged GPS Engine */
#if defined (FEATURE_CGPS)|| defined (FEATURE_GPS_MODE)
#error code not present
#endif 


#ifdef T_MSM7600
#error code not present
#endif /* T_MSM7600 */

#ifdef T_QSC60X5
  #define RF_HAS_QSC60X5
#endif /* T_QSC60X5 */

#ifdef T_QSC1100
  #undef  RF_HAS_QSC60X5
  #define RF_HAS_QSC11X0
#endif /* T_QSC1100*/

#endif /* RFICAP_MM_H */

