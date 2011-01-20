/*=============================================================================

FILE: OEMCall.c

SERVICES: Functions to compose response params for AMSS callmanager

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright (c) 2005-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "OEMCall.h"
#include "AEEConfig.h"
#include "OEMConfig.h"
#include "AEEStdLib.h"
#include "AEEDeviceInfoEx.h"

#if !defined(AEE_SIMULATOR)
#include "OEMCFGI.h"
#include "OEMNV.h"
#endif // AEE_SIMULATOR

/*=============================================================================
 Get CUG.
=============================================================================*/
static int get_default_fwd_cug_info(cm_forward_cug_info_s_type *fwd_cug_info_ptr)
{
   int nErr = AEE_SUCCESS;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
   fwd_cug_info_ptr->present = FALSE;
#endif

#if !defined(AEE_SIMULATOR)
   nErr = OEM_GetConfig(CFGI_CUG, fwd_cug_info_ptr,
                        sizeof(cm_forward_cug_info_s_type));
#endif // AEE_SIMULATOR

   return nErr;
}

#ifdef FEATURE_TTY
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
/*=============================================================================
 Check if tty device is connected.
=============================================================================*/
static boolean is_tty_connected(void)
{
   boolean  bIsTTY = FALSE;
   OEMTTY   tty_state = OEMNV_TTY_OFF; 

   /* if tty is not turned off */
   if (AEE_SUCCESS == OEM_GetConfig(CFGI_TTY, &tty_state, sizeof(OEMTTY))
       && OEMNV_TTY_OFF != tty_state) {
      boolean headphoneOn = FALSE;
      boolean stereoHeadphoneOn = FALSE;
      int size = sizeof(boolean);

      (void) OEM_GetDeviceInfoEx(
         AEE_DEVICESTATE_HEADPHONE_ON,
         (void *) &headphoneOn,                        
         &size
      );

      (void) OEM_GetDeviceInfoEx(
         AEE_DEVICESTATE_STEREO_HEADPHONE_ON,
         (void *) &stereoHeadphoneOn,                        
         &size
      );
      
      // If we cannot find headset is attached through the GetDeviceInfoEx(),
      // check the config items CFGI_HEADSET and CFGI_STEREO_HEADSET. These 
      // config items are currently used by 3GUI. The checking should be 
      // removed once 3GUI is no longer supported.
      if (!headphoneOn) {
         (void) OEM_GetConfig(CFGI_HEADSET, &headphoneOn, size);
      }
      
      if (!stereoHeadphoneOn) {
         (void) OEM_GetConfig(CFGI_STEREO_HEADSET, &stereoHeadphoneOn, size);
      }

      if (headphoneOn || stereoHeadphoneOn) {
         bIsTTY = TRUE;
      }
   }

   return bIsTTY;
}
#endif /* FEATURE_TTY */

#ifdef FEATURE_GSM_AMR
/*=============================================================================
 Get get device preference for AMR vocoder
=============================================================================*/
static boolean is_amr_preferred(void)
{
   boolean  bIsAMR = FALSE;
   nv_item_type               nvi;

   if (NV_DONE_S == OEMNV_Get(NV_GSM_AMR_CALL_CONFIG_I, &nvi)) {
      bIsAMR = nvi.gsm_amr_call_config;
   }
   return bIsAMR;
}
#endif // FEATURE_GSM_AMR
#endif // (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*=============================================================================
 Get the speech preferences used to setup MT, MO voice calls.
=============================================================================*/
static int get_speech_preferences(cm_bearer_capability_s_type *bearer_cap_ptr)
{
#ifdef FEATURE_GSM_AMR
   if (is_amr_preferred()) {
      bearer_cap_ptr->gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_3;
      bearer_cap_ptr->gsm_speech_ver_pref_2 = GSM_FR_SPEECH_VER_2;
      bearer_cap_ptr->gsm_speech_ver_pref_3 = GSM_FR_SPEECH_VER_1;

#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
      bearer_cap_ptr->gsm_speech_ver_pref_4 = GSM_HR_SPEECH_VER_3;
      bearer_cap_ptr->gsm_speech_ver_pref_5 = GSM_HR_SPEECH_VER_1;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
   }
   else {
      bearer_cap_ptr->gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_2;
      bearer_cap_ptr->gsm_speech_ver_pref_2 = GSM_FR_SPEECH_VER_1;
#ifdef FEATURE_GSM_HALF_RATE
      bearer_cap_ptr->gsm_speech_ver_pref_3 = GSM_HR_SPEECH_VER_1;
#else
      bearer_cap_ptr->gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;
#endif /* FEATURE_GSM_HALF_RATE */

#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
      bearer_cap_ptr->gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
      bearer_cap_ptr->gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
   }
#endif  /* FEATURE_GSM_AMR */

   return AEE_SUCCESS;
}
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/*=============================================================================
FUNCTION: OEMCall_compose_setup_params

DESCRIPTION:
  Compose call setup params suitable to accept voice calls.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCall_compose_setup_params(cm_setup_res_params_s_type *setup_params_ptr)
{

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

   (void)get_speech_preferences(&setup_params_ptr->bearer_capability_1);

#ifdef FEATURE_TTY
   if (is_tty_connected()) {
      /* set the CTM bit in the bearer capability for a TTY call*/
      setup_params_ptr->bearer_capability_1.ctm = CTM_SUPPORTED;
   }
#endif /* FEATURE_TTY */

#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

   setup_params_ptr->accept = TRUE;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCall_compose_orig_params

DESCRIPTION:
  compose call orig params suitable for voice calls.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCall_compose_orig_params(cm_gw_cs_orig_params_s_type *orig_params_ptr)
{

   (void)get_default_fwd_cug_info(&orig_params_ptr->forward_cug_info);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

   (void)get_speech_preferences(&orig_params_ptr->bearer_capability_1);

#ifdef FEATURE_TTY
   if (is_tty_connected()) {
      /* set the CTM bit in the bearer capability for a TTY call*/
      orig_params_ptr->bearer_capability_1.ctm = CTM_SUPPORTED;
   }
#endif /* FEATURE_TTY */

#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

   return AEE_SUCCESS;
}

