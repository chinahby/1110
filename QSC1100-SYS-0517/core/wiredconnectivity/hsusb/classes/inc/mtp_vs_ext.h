/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef __MTP_EXT_H__
#define __MTP_EXT_H__

/* Extending ptp datacodes */

#define PTP_VS1_OP_RANGE                                \
    /* values between 0x9700 and 0x9800 are defined as  \
     * MTP Vendor Extension OperationCode */            \
                                                        \
    /* MTP Basic Operations */                          \
    MTP_OP_UNDEFINED                    = 0x9800,       \
    MTP_OP_GET_OBJECT_PROPS_SUPPORTED   = 0X9801,       \
    MTP_OP_GET_OBJECT_PROP_DESC         = 0X9802,       \
    MTP_OP_GET_OBJECT_PROP_VALUE        = 0X9803,       \
    MTP_OP_SET_OBJECT_PROP_VALUE        = 0X9804,       \
    /* Enhanced MTP Operations */                       \
    MTP_OP_GET_OBJECT_PROP_LIST         = 0x9805,       \
    MTP_OP_SET_OBJECT_PROP_LIST         = 0x9806,       \
    MTP_OP_GET_INTERDEP_PROP_DESC       = 0x9807,       \
    MTP_OP_SEND_OBJECT_PROP_LIST        = 0x9808,       \
    /* MTP Basic Operations */                          \
    MTP_OP_GET_OBJECT_REFERENCES        = 0X9810,       \
    MTP_OP_SET_OBJECT_REFERENCES        = 0X9811,       \
    MTP_OP_SKIP                         = 0X9820

 /* values between 0xA700 and 0xA800 are defined as 
  * MTP Vendor Extension ResponseCode */
#define PTP_VS1_RESP_RANGE   \
    MTP_RESP_UNDEFINED                  = 0xA800,       \
    MTP_RESP_INVALID_OBJECT_PROP_CODE   = 0xA801,       \
    MTP_RESP_INVALID_OBJECT_PROP_FORMAT = 0xA802,       \
    MTP_RESP_INVALID_OBJECT_PROP_VALUE  = 0xA803,       \
    MTP_RESP_INVALID_OBJECT_REFERENCE   = 0xA804,       \
    MTP_RESP_INVALID_DATASET            = 0xA806,       \
    MTP_RESP_SPEC_BY_GROUP_UNSUPPORTED  = 0xA807,       \
    MTP_RESP_OBJECT_TOO_LARGE           = 0xA809

/* MTP Vendor Extension Format Code */            
#define PTP_VS1_OBJ_FORMAT_RANGE                        \
    MTP_OBJ_FORMAT_UNDEF                = 0xB802,       \
    MTP_OBJ_FORMAT_WIN_IMG_FMT          = 0xB881,       \
    MTP_OBJ_FORMAT_UNDEF_AUDIO          = 0xB900,       \
    MTP_OBJ_FORMAT_WMA                  = 0xB901,       \
    MTP_OBJ_FORMAT_OGG                  = 0xB902,       \
    MTP_OBJ_FORMAT_UNDEF_VIDEO          = 0xB980,       \
    MTP_OBJ_FORMAT_WMV                  = 0xB981,       \
    MTP_OBJ_FORMAT_MP4_CONTAINER        = 0xB982,       \
    MTP_OBJ_FORMAT_UNDEF_COLLECTION     = 0xBA00,       \
    MTP_OBJ_FORMAT_ABS_M_MEDIA_ALBUM    = 0xBA01,       \
    MTP_OBJ_FORMAT_ABS_IMAGE_ALBUM      = 0xBA02,       \
    MTP_OBJ_FORMAT_ABS_AUDIO_ALBUM      = 0xBA03,       \
    MTP_OBJ_FORMAT_ABS_VIDEO_ALBUM      = 0xBA04,       \
    MTP_OBJ_FORMAT_ABS_AV_PLAYLIST      = 0xBA05,       \
    MTP_OBJ_FORMAT_ABS_CONTACT_GROUP    = 0xBA06,       \
    MTP_OBJ_FORMAT_ABS_MSG_FOLDER       = 0xBA07,       \
    MTP_OBJ_FORMAT_ABS_CHP_PRODUCTION   = 0xBA08,       \
    MTP_OBJ_FORMAT_WPL_PLAYLIST         = 0xBA10,       \
    MTP_OBJ_FORMAT_M3U_PLAYLIST         = 0xBA11,       \
    MTP_OBJ_FORMAT_MPL_PLAYLIST         = 0xBA12,       \
    MTP_OBJ_FORMAT_ASX_PLAYLIST         = 0xBA13,       \
    MTP_OBJ_FORMAT_PLS_PLAYLIST         = 0xBA14,       \
    MTP_OBJ_FORMAT_UNDEF_DOC            = 0xBA80,       \
    MTP_OBJ_FORMAT_ABS_DOC              = 0xBA81,       \
    MTP_OBJ_FORMAT_XML_DOC              = 0xBA82,       \
    MTP_OBJ_FORMAT_MS_WORD_DOC          = 0xBA83,       \
    MTP_OBJ_FORMAT_MHT_HTML_DOC         = 0xBA84,       \
    MTP_OBJ_FORMAT_MS_EXCEL_DOC         = 0xBA85,       \
    MTP_OBJ_FORMAT_MS_PPT_DOC           = 0xBA86,       \
    MTP_OBJ_FORMAT_UNDEF_MSG            = 0xBB00,       \
    MTP_OBJ_FORMAT_ABS_MSG              = 0xBB01,       \
    MTP_OBJ_FORMAT_UNDEF_CONTACT        = 0xBB80,       \
    MTP_OBJ_FORMAT_ABS_CONTACT          = 0xBB81,       \
    MTP_OBJ_FORMAT_VCARD2               = 0xBB82

/* values between 0xC000 and 0xC800 are defined as 
 * MTP Vendor Extension EventCode */
#define PTP_VS1_EVENT_RANGE                             \
    MTP_EVENT_UNDEFINED                 = 0xC800,       \
    MTP_EVENT_OBJECT_PROP_CHANGED       = 0xC801,       \
    MTP_EVENT_OBJECT_PROP_DESC_CHANGED  = 0xC802,       \
    MTP_EVENT_OBJECT_REFERENCES_CHANGED = 0xC803

/* MTP Vendor Extension Device Property */
#define PTP_VS1_DEV_PROP_RANGE                                   \
    MTP_DEV_PROP_SYNCHRONIZATION_PARTNER         = 0xD401,       \
    MTP_DEV_PROP_DEVICE_FRIENDLY_NAME            = 0xD402,       \
    MTP_DEV_PROP_VOLUME                          = 0xD403,       \
    MTP_DEV_PROP_SUPPORTED_FORMATS_ORDERED       = 0xD404,       \
    MTP_DEV_PROP_DEVICE_ICON                     = 0xD405,       \
    MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO  = 0xD406,       \
    MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE           = 0xD407,       \
    MTP_DEV_PROP_PLAYBACK_RATE                   = 0xD410,       \
    MTP_DEV_PROP_PLAYBACK_OBJECT                 = 0xD411,       \
    MTP_DEV_PROP_PLAYBACK_CONTAINER              = 0xD412

/* Additional Extended Datacodes */

#define PTP_VS1_FMODE_RANGE                                   \
    MTP_FMODE_NON_RESPONSIVE_PLAYBACK         = 0xC001,       \
    MTP_FMODE_RESPONSIVE_PLAYBACK             = 0xC002

#define PTP_VS1_PROTECT_RANGE                           \
    MTP_PROTECT_READONLY_DATA           = 0x8002,       \
    MTP_PROTECT_NON_TRANSFER            = 0x8002

#define PTP_VS1_ASSOC_RANGE                    \
    MTP_ASSOC_UNDEF                     = 0xC000

#define PTP_STORAGE_TYPE_VS_EXT_RANGE           MTP_EMPTY_STORAGE_RANGE

#define PTP_VS1_FS_RANGE                                                \
    MTP_FS_VENDOR_EXT                   = 0x8000, /* and higher */      \
    MTP_FS_DEFINED                      = 0xC000  /* and higher */

#define PTP_ACCESS_CAPABILITY_VS_EXT_RANGE      MTP_EMPTY_ACCESS_CAP_RANGE

#endif /*__MTP_EXT_H__*/
