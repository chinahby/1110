#ifndef COREUSEREVENTS_H
#define COREUSEREVENTS_H
/*=============================================================================

FILE: CoreUserEvents.h

SERVICES: QCT UI Core BREW Application.

GENERAL DESCRIPTION:
User events header file for CoreApp

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
This should be the startup application automatically launched by BREW.

(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/core/CoreUserEvents.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/07   jas     Created
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

/*=============================================================================

                       CORE APP USER EVENTS

=============================================================================*/
/* Security events */
#define EVT_PIN_CHANGE_SUCCESS    1001
#define EVT_PUK_VERIFY_SUCCESS    1002
#define EVT_PIN_VERIFY_SUCCESS    1003
#define EVT_PIN_ENABLE_SUCCESS    1004
#define EVT_PIN_VERIFY_FAIL       1005
#define EVT_PIN_ENABLE_FAIL       1006
#define EVT_PIN_CHANGE_FAIL       1007
#define EVT_PUK_VERIFY_FAIL       1008
#define EVT_NEW_PIN_MATCH         1009
#define EVT_SECURITY_SUCCESS      1010
#define EVT_SECURITY_FAIL         1011
#define EVT_RESTRICT_UPDATE_SUCCESS     1012
#define EVT_RESTRICT_UPDATE_FAIL        1013
#define EVT_RESTRICT_UPDATE_FDN_FAIL    1014
#define EVT_RESTRICT_UPDATE_FDN_SUCCESS 1015
#define EVT_USER_SLOT_PREF_FAIL         1016
#define EVT_USER_SLOT_PREF_SUCCESS      1017
#define EVT_PERSO_DEACT_FAIL            1018
#define EVT_PERSO_DEACT_SUCCESS         1019
#define EVT_PERSO_ACT_SUCCESS           1020
#define EVT_PERSO_ACT_FAIL              1021
#define EVT_PERSO_GET_INDICATION_SUCCESS 1022
#define EVT_PERSO_GET_INDICATION_FAIL    1023
#define EVT_PERSO_SET_DATA_SUCCESS       1024
#define EVT_PERSO_SET_DATA_FAIL          1025

#ifdef  FEATURE_CPHS_EONS
#define EVT_SPN_READ_SUCCESS             1030
#define EVT_SPDI_READ_SUCCESS            1031
#define EVT_PNN_READ_SUCCESS             1032
#define EVT_ONS_READ_SUCCESS             1033
#define EVT_OPL_READ_SUCCESS             1034
#define EVT_IMSI_READ_SUCCESS            1035
#define EVT_CPHS_ONS_READ_SUCCESS        1036
#endif /* FEATURE_CPHS_EONS */

/* Menu events */
#define EVT_MESSAGE_END             1050
#define EVT_RESTORE_COMPLETED       1051
#define EVT_RDM_DONE                1052
#define EVT_RDM_NOT_ALLOWED         1053
#define EVT_RDM_DEVMAP_BUSY         1054
#define EVT_RDM_APP_BUSY            1055
#define EVT_CSP_READ_SUCCESS        1056

/* Misc events */
#define EVT_START_IDLE_DIALOG       1100
#define EVT_RING_COMPLETE           1101

/* Task related events */
#define EVT_UI_EXIT                 1150
#define EVT_ENTER_LPM               1151
#define EVT_SUBSCRIPTION_CHANGED    1152
#define EVT_SYS_PREF_ERR            1153
#define EVT_TASK_OFFLINE            1154
#define EVT_MC_PWR_DOWN             1155
#define EVT_LPM                     1156
#define EVT_SET_OPERATING_MODE      1157

/* External device events */
#define EVT_NO_DEVICE               1200
#define EVT_HEADSET_ON              1201
#define EVT_HEADSET_OFF             1202
#define EVT_HFK_ON                  1203
#define EVT_HFK_OFF                 1204
#define EVT_USB_HFK_ON              1205
#define EVT_USB_HFK_OFF             1206
#define EVT_SDAC_ON                 1207
#define EVT_SDAC_OFF                1208
#define EVT_STEREO_HEADSET_ON       1209
#define EVT_SPEAKERPHONE_TOGGLE     1210
#define EVT_VOLUME_UP               1211
#define EVT_VOLUME_DOWN             1212
#define EVT_EXT_PWR_ON              1213
#define EVT_EXT_PWR_OFF             1214

//The following is defined in AppComFunc.h.  Don't recycle.
//#define EVT_USB_GENERIC_BUTTON_PRESS     1217
//#define EVT_USB_VOICEDIAL_BUTTON_PRESS   1218

#define EVT_USB_RETURN_TO_CRADLE    1219
#define EVT_USB_REMOVE_FROM_CRADLE  1220
#define EVT_USB_STEREO_HFK_ON       1221

/* Broadcast message text*/
#define EVT_CORE_SET_IDLETEXT       1250
#define EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED 1251
#define EVT_SET_BACKLIGHT           1252

#define EVT_LCD_ON                   1253

#define EVT_SELECT_NETWORK           1254 

#define EVT_VOICE_PRIVACY              0x1000

#endif /* COREUSEREVENTS_H */

