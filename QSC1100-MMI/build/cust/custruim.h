#ifndef CUSTRUIM_H
#define CUSTRUIM_H
/*===========================================================================

            " C u s t - R U I M "   H E A D E R   F I L E

DESCRIPTION
  Configuration for RUIM Feature.

  Copyright (c) 2000 - 2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/custruim.h_v   1.2   05 Mar 2002 19:48:16   ropalsky  $
$Header: //depot/asic/qsc60x0/build/cust/custruim.h#17 $ $DateTime: 2008/04/01 18:41:38 $ $Author: jkomenda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   jk      Added Multiple SIP Profiles Feature for OMH Phase 1B
01/22/08   nk      Added CDMA Refresh Feature
10/12/07   jk      Added 3GPD Authentication Support for Open Market Handset
10/02/07   vs      Added FEATURE_ICARDSESSION
07/10/07   nk      Added FEATURE_UIM_EUIMID
12/05/06   mp      defining features FEATURE_MMGSDI_PERSONALIZATION, FEATURE_MMGSDI_PERSONALIZATION_ENGINE
                   and FEATURE_PERSO_RUIM for 6010 GSTK builds
10/19/06   sp      Added FEATURE_MMGSDI_NV_ME_CONFIG
08/09/06   pv      Add features to remove UIM dependencies for 6100
                   on NV and PMIC VU's
10/21/04   jar     Added FEATURE_UIM_INTERNAL_VERIFY_CHV2
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
#define FEATURE_UIM_RUIM

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

/* UIM clock regime with UART control
*/
#define FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME

/*---------------------------------------------------------------------------
                      Pre-Addendum R-UIM Card Version Support
---------------------------------------------------------------------------*/

/* The Offset for VPM/CMEA Key generation is one
*/
#undef FEATURE_UIM_RUIM_VPM_OFFSET_ONE

/* Support for Pre-Addendum Op codes that clash with ISO-7816 commands
*/
#undef FEATURE_UIM_RUIM_PRE_A1_OP_CODES

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
#define FEATURE_RUIM_PHONEBOOK

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

/* Adds support for the basic toolkit features */
#define FEATURE_UIM_TOOLKIT

/* This configures the Application toolkit for the Unicom Toolkit design */
#define FEATURE_UIM_TOOLKIT_UTK
#error FEATURE_UIM_TOOLKIT_UTK
/* This adds support for the SIM data download error */
#define FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR

/* Diag feature to display variable length strings */
#ifdef FEATURE_UIM_TOOLKIT
  #define FEATURE_DIAG_MSG_STRING_ARG
#endif /* FEATURE_UIM_TOOLKIT */

#if defined (FEATURE_UIM_TOOLKIT_UTK) && defined (FEATURE_GSTK)
/* Continue multiple PRL PP download without requiring user input */
#define FEATURE_UIM_DEBUG_PRL_UPDATE
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define FEATURE_UTK2
#endif //FEATURE_UIM_TOOLKIT
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
#undef FEATURE_SECOND_UART
/* Allows the HFK to operate whenever the second UART is not open.
*/
#undef FEATURE_SDEVMAP_SHARE_HFK

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
#ifndef WIN32
#define FEATURE_MMGSDI
#endif
/* CDMA part of MMGSDI */
#define FEATURE_MMGSDI_CDMA

/* test automation */
#define FEATURE_MMGSDI_TEST_AUTOMATION

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

/* Allows for Card OEM Layer (ICARD) to Regiser for Card Events */
#define FEATURE_ICARD_GSDI_REGISTER_CALLBACK

#undef FEATURE_UIM_UICC
#undef FEATURE_ICARD_CHANNEL_MANAGEMENT_ADDITIONAL_FILTERING_PROCESS
#define FEATURE_UIM_UTIL
#undef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#define FEATURE_ICARD_NO_UI_BASE
#define FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED

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

/* Debug feature to log the APDU transactions
*/
#define FEATURE_UIM_DEBUG_LOG

/* Debug feature for UI menu item to enable/disable UIM clock
*/
#define FEATURE_UIM_DEBUG_CLOCK_TOGGLE

/* Debug feature to test proactive command CDMA Send SMS
*/
#ifdef FEATURE_UIM_TOOLKIT

  #define FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS

#endif /* FEATURE_UIM_TOOLKIT */

/* Disables programming of rx waterlevel in the UART*/
#define T_UIM_NO_RX_WATERLEVEL

/* Support for T=1 asynchronous block protocol */
#define FEATURE_UIM_T_1_SUPPORT

/* Support SIM Lock Features */
#define FEATURE_MMGSDI_PERSONALIZATION
#define FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#define FEATURE_PERSO_RUIM

#if !defined(T_QSC6010) && !defined(FEATURE_OEM_SPECIAL_6020_TARGET)
/* Support SIM Lock Features */
#define FEATURE_ICARD_NO_UI_BASE

#define FEATURE_CCAT
#define FEATURE_CAT_REL6
#define FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED

#define FEATURE_MMGSDI_GEN_OPTIMIZATION
#define FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
#endif /*T_QSC6010*/

#ifndef T_QSC6010
/* Features to support APDU Logging */
#define FEATURE_UIM_LOG_TO_EFS
#define FEATURE_UIM_LOG_APDU_TO_EFS
#endif /* T_QSC6010 */

#ifdef T_QSC6010
/* 6010 does not want to have dependencies on NV or PMIC */
#define FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
#endif /*T_QSC6010*/

/* Read me support of fdn, bdn, acl from nv*/
#define FEATURE_MMGSDI_NV_ME_CONFIG

/* Support for SF EUIMID */
#define FEATURE_UIM_EUIMID

/* Support for 3GPD Simple and Mobile IP Authentication*/
#define FEATURE_UIM_SUPPORT_3GPD
#define FEATURE_UIM_3GPD_MOBILE_IP
#define FEATURE_UIM_3GPD_FALLBACK
#define FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES

/* Support for ICardSession */
#define FEATURE_ICARDSESSION

/* Support CDMA Refresh */
#define FEATURE_RUIM_CDMA_REFRESH
#endif /* CUSTRUIM_H */

