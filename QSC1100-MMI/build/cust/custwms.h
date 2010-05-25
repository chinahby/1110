#ifndef CUSTWMS_H
#define CUSTWMS_H
/*===========================================================================

DESCRIPTION
  Configuration for WMS

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/wms/chipset/QSC1100/main/latest/custwms.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/08   HN      Enable FEATURE_CDSMS_DUP_DETECTION for CDMA
10/20/08   HN      Disable FEATURE_IWMSDIAG if WMSAPP is not enabled
06/23/08   PMD     Removed featurizations for mainlined / removed code
11/16/07   NP      File updated and customized for ULC
01/16/07   HQ      Added support for WMS_BC_MM for 1x Broadcast SMS.
06/22/04   HQ      Added support for SMS-CB in GSM mode.
                   Enabled FEATURE_IWMSDIAG.
06/22/04   HQ      Some clean-ups.
11/11/02   cah     Added GSM features.
10/30/02
08/23/02   HQ      Initial version.
===========================================================================*/

/*---------------------------------------------------------------------------
       Wireless Messaging Services (SMS/EMS/CB-SMS)
---------------------------------------------------------------------------*/

/* Wireless Messaging Services (WMS)
*/

/* Main feature flags
*/

/* Enable CDMA/1x SMS
*/
#define FEATURE_CDSMS

#if defined(FEATURE_GSM)
#error code not present
#endif

/* ------------------------------------------------ */
/* Common feature flags between CDMA and GSM SMS    */
/* ------------------------------------------------ */
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

  /* FEATURE_WMS is defined when either of the following flags is defined. This is convenient sometimes for developers to check if WMS API is enabled or not regardless of the modes.
  */
  #define FEATURE_WMS

 /* Enable IWmsDiag static extension
  */
#if (!defined(FEATURE_UI_CORE_REMOVED) && defined(FEATURE_WMS_APP))
  #define FEATURE_IWMSDIAG
#endif /* do not define for ThinUI  */

#endif /*defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)*/



/* ------------------------------------------------ */
/* CDMA/1x SMS Specific features                    */
/* ------------------------------------------------ */
#ifdef FEATURE_CDSMS

  /* Broadcast SMS support
  */
  #define FEATURE_BROADCAST_SMS

  /* WMS_BC_MM API for 1x Broadcast SMS
  */
  #define FEATURE_CDSMS_BROADCAST

  /* Diag Support */
  #define FEATURE_WMS_DIAG

  #ifdef FEATURE_BUILD_JCDMA
    /* CDMA SMS JCDMA support
    */
    #define FEATURE_CDSMS_JCDMA
  #endif /*FEATURE_BUILD_JCDMA*/

  #define FEATURE_CDSMS_DUP_DETECTION

#endif /* FEATURE_CDSMS */



/* ------------------------------------------------ */
/* GSM/GPRS/UMTS SMS Specific features              */
/* ------------------------------------------------ */
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


/* Enable Multimode Broadcast SMS
*/
#if (defined(FEATURE_GWSMS_BROADCAST)||defined(FEATURE_CDSMS_BROADCAST))
  #define FEATURE_BROADCAST_SMS_MULTIMODE
#endif

#endif /* CUSTWMS_H */

