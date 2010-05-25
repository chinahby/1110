#ifndef AEEDEVICEITEMS_H
#define AEEDEVICEITEMS_H
/*===================================================================
FILE: AEEDeviceItems.h

SERVICES:  AEE Device Item and Type Definitions

DESCRIPTION:
  This header defines device items and types.

PUBLIC CLASSES AND FUNCTIONS:

Copyright © 1999,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===================================================================*/

#define AEE_DEVICEITEM_CHIP_ID               1
#define AEE_DEVICEITEM_MOBILE_ID             2     // Mobile ID

//Vocoder Support
#define AEE_DEVICEITEM_AMR_VOCODER_SUPPORT   3     // Is AMR (Adaptive Multi-Rate) Vocoder Supported ?
#define AEE_DEVICEITEM_EVRC_VOCODER_SUPPORT  4     // Is EVRC (Enhanced Variable Rate Codec) Vocoder Supported ?
#define AEE_DEVICEITEM_IS96_VOCODER_SUPPORT  5     // Is QCELP-IS96(8K)(Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
#define AEE_DEVICEITEM_IS96A_VOCODER_SUPPORT 6     // Is QCELP-IS96A(8K)(Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
#define AEE_DEVICEITEM_IS733_VOCODER_SUPPORT 7     // Is QCELP-IS733(13K)(Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
#define AEE_DEVICEITEM_SMV_VOCODER_SUPPORT   8     // Is Selectable Mode Vocoder Supported ?

//System Colors
#define AEE_DEVICEITEM_SYS_COLORS_DISP1      9
#define AEE_DEVICEITEM_SYS_COLORS_DISP2      10
#define AEE_DEVICEITEM_SYS_COLORS_DISP3      11
#define AEE_DEVICEITEM_SYS_COLORS_DISP4      12

//Display Info (AEEBitmapInfo struct)
#define AEE_DEVICEITEM_DISPINFO1             13
#define AEE_DEVICEITEM_DISPINFO2             14
#define AEE_DEVICEITEM_DISPINFO3             15
#define AEE_DEVICEITEM_DISPINFO4             16

#define AEE_DEVICEITEM_CARRIER_ID            17    // Carrier ID
#define AEE_DEVICEITEM_SIDHASH               18

//Platform credentials
#define AEE_DEVICEITEM_SSDA                  19
#define AEE_DEVICEITEM_ESN                   20
#define AEE_DEVICEITEM_BKEY                  21
#define AEE_DEVICEITEM_SID                   22
#define AEE_DEVICEITEM_DLFLAGS               23

#define AEE_DEVICEITEM_MANNER_MODE           24 // *pBuff = uint32(AEE_MANNER_MODE_XXX)

#define AEE_DEVICEITEM_MDN                   25 //Mobile Directory Number
#define AEE_DEVICEITEM_FIRMWARE_VERSION      26 //OEM defined firmware ID
#define AEE_DEVICEITEM_SOFTKEY_COUNT         27 //How many softkeys does the device have

//More platform credentials
#define AEE_DEVICEITEM_IMEI                  28 // WCDMA/GSM device ID as ascii string
#define AEE_DEVICEITEM_ADS                   29 // Application Download Server (ADS) hostname
#define AEE_DEVICEITEM_RUIMID                30 // RUIM ID for this client

#define AEE_DEVICEITEM_KEY_SUPPORT           31 // Is specified key supported ?

#define AEE_DEVICEITEM_DOWNLOAD_HEADER       32 // HTTP Header for Application Download Server (ADS)

#define AEE_DEVICEITEM_REPEAT_TIME           33 // AEEAutoRepeat *, Default: Varies

#define AEE_DEVICEITEM_ICCID                 34 // ICCID String

#define AEE_DEVICEITEM_CAMERA_INFO           35 // List of supported camera CLSIDs (Null-terminated)

#define AEE_DEVICEITEM_POSDET_PRIVACY_ALERT  36 // Is Privacy Alert is enabled for PosDet? (boolean)

#define AEE_DEVICEITEM_CARD0_INFO            37 // MMC card info - pBuff = AEECardInfo *

#define AEE_DEVICEITEM_HWID                  38 // Primary hardware ID for this device
#define AEE_DEVICEITEM_HWIDLIST              39 // list of all h/w IDs available on this device
                                                // The first element in the list is always the primary ID


#define AEE_DEVICEITEM_MANUFACTURER_NAME     40 // Manufacturer name (data type is AECHAR*)

#define AEE_DEVICEITEM_MODEL_NAME            41 // Model name (data type is AECHAR*)

#define AEE_DEVICEITEM_FRIENDLY_NAME         42 // Device friendly name (data type is AECHAR*)

#define AEE_DEVICEITEM_MEIDS                 43 // MEID as ascii string 

#define AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY 44 // Are PosDet requests restricted to emergency services?

#define AEE_DEVICEITEM_TAPI_SMS_ENCODING     45 // Encoding of data stream provided to ITAPI_SendSMS()

#define AEE_DEVICEITEM_ENCODING              46
#define AEE_DEVICEITEM_RAM                   47
#define AEE_DEVICEITEM_TEXT_SCROLL_TIMER     48
#define AEE_DEVICEITEM_IMAGE_DELAY_TIME      49
#define AEE_DEVICEITEM_SCROLL_BAR_LENGTH     50
#define AEE_DEVICEITEM_CLOSE_APP_KEY         51
#define AEE_DEVICEITEM_CLOSE_ALL_KEY         52
#define AEE_DEVICEITEM_MAX_FILE_NAME         53
#define AEE_DEVICEITEM_PROMPT_PROPERTIES     54
#define AEE_DEVICEITEM_NET_LINGER_TIME       55
#define AEE_DEVICEITEM_SLEEP_DEFER_TIME      56
#define AEE_DEVICEITEM_VOICE_RECOGNITION_SUPPORTED    57
#define AEE_DEVICEITEM_POSITION_LOCATION_SUPPORTED    58
#define AEE_DEVICEITEM_MIDI_SUPPORTED        59
#define AEE_DEVICEITEM_CMX_SUPPORTED         60
#define AEE_DEVICEITEM_PEN_SUPPORTED         61

// OEM Device Keys.
#define AEE_DEVICEITEM_OEM_ENCRYPTION_KEY     62
#define AEE_DEVICEITEM_OEM_AUTHENTICATION_KEY 63

#define AEE_DEVICEITEM_FSSPACE_CARD0         64 // Free and Total Space on AEEFS_CARD0_DIR filesystem
#define AEE_DEVICEITEM_FSSPACE_ROOT          65 // Free and Total Space on main internal flash filesystem
// AEE_DEVICEITEM_FSSPACE_USBHMS1 and AEE_DEVICEITEM_FSSPACE_USBHMS2
// are supported only from BREW 3.1.5 onwards
#define AEE_DEVICEITEM_FSSPACE_USBHMS1       66 // Free and Total Space on AEEFS_USB_HOST_MASS_STORAGE1
#define AEE_DEVICEITEM_FSSPACE_USBHMS2       67 // Free and Total Space on AEEFS_USB_HOST_MASS_STORAGE2

#define AEE_DEVICESTATE_FIRST                0x4fff
#define AEE_DEVICESTATE_KEYS_PRESSED         (AEE_DEVICESTATE_FIRST + 1)
#define AEE_DEVICESTATE_FLIP_OPEN            (AEE_DEVICESTATE_FIRST + 2)
#define AEE_DEVICESTATE_HEADPHONE_ON         (AEE_DEVICESTATE_FIRST + 3)
#define AEE_DEVICESTATE_KEYGUARD_ON          (AEE_DEVICESTATE_FIRST + 4)
#define AEE_DEVICESTATE_SPEAKER_ON           (AEE_DEVICESTATE_FIRST + 5)
#define AEE_DEVICESTATE_VIBRATOR_ON          (AEE_DEVICESTATE_FIRST + 6)
#define AEE_DEVICESTATE_SCR_ORIENTATION      (AEE_DEVICESTATE_FIRST + 7)
#define AEE_DEVICESTATE_KEY_INFO             (AEE_DEVICESTATE_FIRST + 8)
#define AEE_DEVICESTATE_TESTSIG_MOD          (AEE_DEVICESTATE_FIRST + 9)

// Carrier Defined Items Range
#define AEE_DEVICEITEM_CARRIER_BEGIN         0x58EF // = AEE_DEVICEITEM_USER - 10000
#define AEE_DEVICEITEM_CARRIER_END           0x7ffe // = AEE_DEVICEITEM_USER - 1

// User Defined or OEM Defined Items Range
#define AEE_DEVICEITEM_USER  0x7fff
#define AEE_DEVICEITEM_USER_BEGIN            0x7fff // = AEE_DEVICEITEM_USER
#define AEE_DEVICEITEM_USER_END              0x9fff // = AEE_DEVICEITEM_USER + 8192

#endif  /* end ifndef AEEDEVICEITEMS_H */
