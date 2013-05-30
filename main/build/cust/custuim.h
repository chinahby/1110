#ifndef CUSTRUIM_H
#define CUSTRUIM_H
/*===========================================================================

            " C u s t - R U I M "   H E A D E R   F I L E

DESCRIPTION
  Configuration for RUIM Feature.

  Copyright (c) 2000, 2001, 2002, 2003, 2005-2010 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/custruim.h_v   1.2   05 Mar 2002 19:48:16   ropalsky  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1100/main/latest/build/cust/custuim.h#21 $ $DateTime: 2011/03/30 01:32:46 $ $Author: amitp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/11   adp     Defining FEATURE_APP_CAT always and defining 
                   FEATURE_APP_CATAPP only for non-Mango UI builds
03/25/11   adp     Removing feature FEATURE_APP_CAT from uim, since UIM does   
                   not own the feature 
03/09/11   ssr     Fixed compialtion error
07/09/10   ssr     Enable FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND
04/23/10   ssr     Enable UIM 3GPD NV support
07/23/09   ssr     Added FEATURE_UIM_RUIM_SUPPORT_SCI
06/22/09   yb      Enable FEATURE_CAT_REL6
06/22/09   ssr     Removed FEATURE_UIM_RUIM_SUPPORT_SCI
06/15/09   rm      Enable FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES feature
04/25/09   ssr     Added Features
04/03/09   ssr     Fixed FEATURE_APP_CAT for MANGO flavor SBNGAH and SBNKAH builds.
04/01/09   ssr     Added Features
03/31/09   jk      Platform specific cust file inclusions
03/12/09   xz      Enable FEATURE_APP_CAT and FEATURE_APP_CATAPP
01/14/09   vs      Added FEATURE_UIM_USES_NEW_SLEEP_API
08/08/08   vs      Added featurization from a memory reduction standpoint
08/13/08   nk      Added Automation Related Features
07/16/08   nk      Added Icard Features
07/03/08   jk      Added features for OMH:  3GPD, LBS
06/18/08   jk      Additional changes for QSC1100 bring-up/merge to mainline
05/19/08   jk      Added features for QSC1100 bringup
02/09/08   jk      Wrapped FEATURE_UIM_SUPPORT_HRPD_AN under HDR Feature
01/30/08   jk      Added FEATURE_UIM_SUPPORT_HRPD_AN
11/13/07   vs      Added FEATURE_UIM_EUIMID
06/08/07   sun     Adding FEATURE_UIM_USES_OLD_PMIC_API back in
05/22/07   sp      Adding FEATURE_MMGSDI_USE_SMALL_HEAP and
                   FEATURE_GSTK_USE_SMALL_HEAP
07/28/06   nk      Removing FEATURE_UIM_USES_OLD_PMIC_API, UIM to use new API,
                   as 6800 picking up PMIC Dependencies
07/27/06   jk      Features to support APDU Logging
07/26/06   nk      Let UIM use the old PMIC API until the new API is picked-up.s
02/15/06   nk      Enable FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
11/30/05   pv      Added feature to comply with cards without ICCID
12/04/03   ht      Undef FEATURE_UIM_TOOLKIT and FEATURE_UIM_TOOLKIT_UTK
03/28/03   ck/jr   Added FEATURE_GSDI_OPTIMIZATION and undefined
           FEATURE_GSDI_USE_OWN_HEAP
12/16/02   cah     Add FEATURE_UI_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW for
                   6300 only.
11/11/02   cah     Added GSM features and FEATURE_NVRUIM_PREF_MODE_NOT_IN_RUIM.
09/19/02   ck      Replaced FEATURE_PRL_DISABLE_CELLULAR_CHANNEL_CHECK with
                   FEATURE_NO_CELL_GUARD_BANDS
09/17/02   ck      Added the RUN TIME RUIM Enable feature
09/06/02   ck      Added FEATURE_CDSMS_RUIM and support to include GSDI
                   features temporarily
02/19/02   ts      Organized feature switches and removed
                   FEATURE_UART_TCXO_CLK_FREQ.
01/15/02   djd     Added FEATURE_PRL_DISABLE_CELLULAR_CHANNEL_CHECK.
12/14/01   ts      Added feature switch for UASMS display mode processing.
12/13/01   ts      Added Feature switches for UIM toolkit and UTK.
12/04/01   km      added undef for FEATURE_UIM_SUPPORT_INVERSE_CONVENTION.
09/20/01   km      undef'ed FEATURE_UIM_RUIM_PRE_A1_OP_CODES.
09/17/01   km      Removed FEATURE_UIM_RUIM_VPM_OFFSET_ONE.
09/14/01   km      Added FEATURE_UIM_RUIM_VPM_OFFSET_ONE.
09/11/01   km      Updated feature names.
05/22/01   ck      Added support for R-UIM using the UIM server.
04/30/01   djd     Disabled text and number matching for SMS.
03/01/01   djd     Added FEATURE_RUIM_PHONEBOOK.
02/05/01   djd     Initial revision.

===========================================================================*/


/*---------------------------------------------------------------------------
                            UIM and R-UIM Server Features
---------------------------------------------------------------------------*/

/* UIM Task Support
*/
#define FEATURE_UIM

/* R-UIM Support from the UIM server
*/
#ifndef FEATURE_VERSION_S600S
#define FEATURE_UIM_RUIM
#endif

/* R-UIM run time enable support by other modules.
 * Enables runtime support for R-UIM card or GSM SIM.
*/
#define FEATURE_UIM_RUN_TIME_ENABLE

/* PrefMode is read from NV rather than the RUIM card.
*/
#define FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM

/*---------------------------------------------------------------------------
                          UIM Driver Features
---------------------------------------------------------------------------*/

/* UIM Driver Support
*/
#define FEATURE_UIM_DRIVER

/* Feature to vote on the TCXO sleep
*/
#define FEATURE_UIM_TCXO_SLEEP

/* -- MULTIPLE VOLTAGE SUPPORT */
/*  1.8V SIM/USIM support */
#define FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES

/* Debug feature to continue power up with a wrong ATR ( extra bytes in ATR )
   for the UIM server */
#define FEATURE_UIM_IGNORE_UNSOLICITED_BYTES

/* Configuration switch for RUIM data and clock mux - either uses AUX PCM lines
   (FEATURE_UIM_ON_GPIO undefined) or GPIO_INT18 - GPIO_INT21
   (FEATURE_UIM_ON_GPIO defined) */
#undef FEATURE_UIM_ON_GPIO

/* Adds support for inverse convention
*/
#define FEATURE_UIM_SUPPORT_INVERSE_CONVENTION

/* Support for inverting the transmitted bits in HW
*/
#define FEATURE_UIM_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW

#ifndef FEATURE_UIM_QSC1100_LOW_MEMORY
  #define FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
  #define FEATURE_UIM_UTIL
  #define FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#endif /* FEATURE_UIM_QSC1100_LOW_MEMORY */

/* Feature to use new SLEEP APIs */
#ifdef FEATURE_NEW_SLEEP_API
  #define FEATURE_UIM_USES_NEW_SLEEP_API
#endif /* FEATURE_NEW_SLEEP_API */

/*---------------------------------------------------------------------------
                           NV access in RUIM
---------------------------------------------------------------------------*/

/* NV Access to R-UIM
*/
#define FEATURE_NV_RUIM

/* How to handle the ESN when the RUIM is not working
** Define one of the following.  If both undefined,
** NV RUIM uses the ESN from EEPROM.
*/
#undef FEATURE_RUIM_ALLOW_ESN_FAIL
#define FEATURE_RUIM_ZERO_ESN_FAIL

/* R-UIM Phone Book Support
*/
//Saber undefines this feature #define FEATURE_RUIM_PHONEBOOK

/*---------------------------------------------------------------------------
                              SMS
---------------------------------------------------------------------------*/

/* R-UIM SMS support using CDSMS
*/
#define FEATURE_CDSMS_RUIM

/* For Mobile-Originated SMS message, the user can edit the destination address,
** message text and the callback number. This feature must be undef'd if
** FEATURE_RUIM_SMS is defined.
*/
#undef FEATURE_COMPOSE_MO_SMS

/*---------------------------------------------------------------------------
                              TOOLKIT
---------------------------------------------------------------------------*/
//#ifndef FEATURE_UIM_QSC1100_LOW_MEMORY
  /* Adds support for the basic toolkit features */
  #define FEATURE_UIM_TOOLKIT

  /* This configures the Application toolkit for the Unicom Toolkit design */
 #define FEATURE_UIM_TOOLKIT_UTK
//#endif /* FEATURE_UIM_QSC1100_LOW_MEMORY */

#ifdef FEATURE_UIM_TOOLKIT
  /* Diag feature to display variable length strings */
  #define FEATURE_DIAG_MSG_STRING_ARG

  /* This adds support for the SIM data download error */
  #define FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
#endif /* FEATURE_UIM_TOOLKIT */

#if defined (FEATURE_UIM_TOOLKIT_UTK) && defined (FEATURE_GSTK)
/* Continue multiple PRL PP download without requiring user input */
#define FEATURE_UIM_DEBUG_PRL_UPDATE
#endif

#ifdef FEATURE_GSTK
#define FEATURE_CAT_REL6
#endif /* FEATURE_GSTK */
/*---------------------------------------------------------------------------
                            UASMS Features
---------------------------------------------------------------------------*/

/* When defined, don't look through the RUIM Phone Book to
** map numbers to names.
*/
#define FEATURE_UASMS_DISABLE_NUMBER_MATCH

/* When defined, don't look through stored messages to match duplicates.
*/
#define FEATURE_UASMS_DISABLE_TEXT_MATCH

/* This configures the SMS processing to enable the display mode processing. */
#ifdef FEATURE_UIM_TOOLKIT_UTK
  #define FEATURE_UASMS_DISPLAY_MODE
#endif /* FEATURE_UIM_TOOLKIT_UTK */

/*---------------------------------------------------------------------------
                                SIO
---------------------------------------------------------------------------*/

/* Disables second UART for RUIM.
*/
#ifdef FEATURE_UIM1
#undef FEATURE_SECOND_UART
/* Allows the HFK to operate whenever the second UART is not open.
*/
  #undef FEATURE_SDEVMAP_SHARE_HFK
#endif

/*---------------------------------------------------------------------------
                                PRL
---------------------------------------------------------------------------*/

/* Enable the feature to include guard band channels as valid channels
*/
#ifndef FEATURE_NO_CELL_GUARD_BANDS
  #define FEATURE_NO_CELL_GUARD_BANDS
#endif
/*---------------------------------------------------------------------------
                                MMGSDI
---------------------------------------------------------------------------*/
/* MMGSDI Task support */
/* Enables MMGSDI, which supports multiple clients.
*/
#define FEATURE_GSDI
/* Used by TMC to activate the MMGSDI.
*/
#define FEATURE_MMGSDI

/* CDMA part of MMGSDI */
#define FEATURE_MMGSDI_CDMA

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* Needs to be removed by 04/01/03 */
#define FEATURE_DOG

/* MMGSDI uses it's own heap */
#undef FEATURE_MMGSDI_OWN_HEAP

/* MMGSDI supports multiple clients */
#define FEATURE_GSDI_MULTICLIENT_SUPPORT

/* Allow for Synchronous Cache Accesses to GSDI */
#define FEATURE_GSDI_OPTIMIZATION

#if defined (FEATURE_GSDI) && defined (FEATURE_GSDI_CDMA)
/* Allows for Softkey UI to Register for Card Events */
#define FEATURE_UI_GSDI_REGISTER_CALLBACK
#endif
#ifndef CUST_EDITION
/* Allows for Card OEM Layer (ICARD) to Regiser for Card Events */
#define FEATURE_ICARD_GSDI_REGISTER_CALLBACK
#endif
/*---------------------------------------------------------------------------
                            Debug Features
---------------------------------------------------------------------------*/

/* Debug features related to Driver Time test
*/
#undef FEATURE_UIM_DRIVER_TEST
#undef FEATURE_UIM_DRIVER_TIME_TEST

/* Debug feature to not do PPS for the UIM server
*/
#undef FEATURE_UIM_DEBUG_NO_PPS

/* Debug features to force NV to return valid status and values
*/
#undef FEATURE_NVRUIM_DEBUG_FORCE_MCC
#undef FEATURE_NVRUIM_DEBUG_FORCE_ADDR_NUM
#undef FEATURE_NVRUIM_DEBUG_FORCE_DIR_NUM

/* Debug feature to flip the byte ordering of ESN in RUN CAVE command specific
   to the R-UIM module */
#undef FEATURE_RUIM_DEBUG_CAVE_ESN
#ifndef CUST_EDITION
/* Debug feature to log the APDU transactions
*/
#define FEATURE_UIM_DEBUG_LOG

/* Debug feature for UI menu item to enable/disable UIM clock
*/
#define FEATURE_UIM_DEBUG_CLOCK_TOGGLE
#endif
/* Debug feature to test proactive command CDMA Send SMS
*/
#ifdef FEATURE_UIM_TOOLKIT
#ifndef CUST_EDITION
  #define FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS
#endif  
#endif /* FEATURE_UIM_TOOLKIT */

#ifdef CUST_EDITION
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define FEATURE_UTK2
#endif //FEATURE_UIM_TOOLKIT
#endif  

/* Disables programming of rx waterlevel in the UART*/
#define T_UIM_NO_RX_WATERLEVEL

/* Support for T=1 asynchronous block protocol */
//#define FEATURE_UIM_T_1_SUPPORT
#ifndef FEATURE_UIM_QSC1100_LOW_MEMORY
  #define FEATURE_UIM_UICC
#endif /* FEATURE_UIM_QSC1100_LOW_MEMORY */

/*---------------------------------------------------------------------------
                       GSM SIM support
---------------------------------------------------------------------------*/
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* Feature to work with cards without ICCID ef */
#define FEATURE_UIM_SUPPORT_NO_ICCID

/* For GSDI Get file Attribute PIN security caching */
#define FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES

#define FEATURE_UIM_RUNTIME_ENABLE

/* Features to support APDU Logging */
/* Support for 3GPD Simple and Mobile IP Authentication*/
#define FEATURE_UIM_SUPPORT_3GPD
#define FEATURE_UIM_3GPD_MOBILE_IP
#define FEATURE_UIM_3GPD_FALLBACK
#define FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
#define FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
/* 3GPD NV support for non legacy cdma card */
#define FEATURE_UIM_SUPPORT_3GPD_NV

/* Feature to support LBS on RUIM */
#define FEATURE_UIM_SUPPORT_LBS
#define FEATURE_UIM_RUIM_SUPPORT_SCI

/* Support for ICardSession */
#ifdef FEATURE_OEMOMH
#define FEATURE_ICARDSESSION
#endif
#ifndef CUST_EDITION
#ifdef T_QSC1110
#define FEATURE_CCAT
#endif /* T_QSC1110 */
#endif
/* Please leave the definition below as is at the end of the file
   as it depends on another feature being defined */
#if defined(FEATURE_UIM_SUPPORT_3GPD)
  #define FEATURE_DATA_PS_INTERNAL_AUTH
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#define FEATURE_ICARD_IMODEL

/* Features to support Code Reduction */
#define FEATURE_UIM_MEMORY_REDUCTION
#define FEATURE_GSTK_MEMORY_REDUCTION
#define FEATURE_MMGSDI_MEMORY_REDUCTION

/* Feature to use tmc small heap */
#define FEATURE_MMGSDI_USE_SMALL_HEAP

/* Feature to use tmc_small_heap for gstk */
#define FEATURE_GSTK_USE_SMALL_HEAP

#define FEATURE_UIM_USES_OLD_PMIC_API

/* Feature EUIMID */
#define FEATURE_UIM_EUIMID

/* Feature for HRPD/HDR Authentication */
#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif
#ifdef FEATURE_OEMOMH
/* ICard Extension */
#define FEATURE_ICARD_IMODEL
#define FEATURE_ICARD
/* Feature for model support */
#define FEATURE_ICARDSESSION_IMODEL
#endif
/* MMGSDI DIAG Automation */
#ifndef FEATURE_UIM_QSC1100_LOW_MEMORY
  #define FEATURE_MMGSDI_TEST_AUTOMATION
  #define FEATURE_MMGSDI_ATCSIM
  #define FEATURE_UIM_SUPPORT_CSIM
#endif /* FEATURE_UIM_QSC1100_LOW_MEMORY */

/* CARD APPLICATION TOOLKIT  SUPPORT */
#if defined(FEATURE_GSTK) && !(defined(FEATURE_MANGO_UI))
  #define FEATURE_APP_CAT
  #define FEATURE_APP_CATAPP
#endif /* FEATURE_GSTK */

  /*************************************************
   PLATFORM SPECIFIC CUST FILE INCLUSIONS ONLY
   *************************************************/

  /* Platform specific custuim file inclusions */
  #ifdef T_MSM6290
  #include "custuim_hw_6290.h"
  #endif /* T_MSM6290 */

  /* Platform specific custuim file inclusions */
  #if defined(T_QSC6270) || defined(T_QSC6240)
    #include "custuim_hw_6240_6270.h"
  #endif /* T_QSC6270 || T_QSC6240 */

  /* Platform specific custuim file inclusions */
  #ifdef T_MSM6246
  #include "custuim_hw_6246.h"
  #endif /* T_MSM6246 */

  /* Platform specific custuim file inclusions */
  #if defined(T_QSC60X5) && !defined(T_QSC1100)
  #include "custuim_hw_qsc60x5.h"
  #endif /* defined(T_QSC60X5) && !defined(T_QSC1100) */

  /* Platform specific custuim file inclusions */
  #if defined(T_MSM7625) && !defined(T_MSM76XXT)
  #include "custuim_hw_7225.h"
  #elif defined(T_MSM76XXT)
#error code not present
  #endif /* T_MSM7625 T_MSM76XXT */

  #ifdef T_QSC1100
  #include "custuim_hw_qsc11x0.h"
  #endif /* T_QSC1100 */

  #ifdef T_MSM8650A
  #include "custuim_hw_8x50.h"
  #endif /* T_MSM8650A */

  /* RUIM Perso Lock */
  #if defined(FEATURE_RUIM) && !(defined(FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM))
  #define FEATURE_PERSO_RUIM
  #endif /* FEATURE_RUIM !(FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM)*/


#endif /* CUSTRUIM_H */

