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

#ifndef __MTP_TYPES_H__
#define __MTP_TYPES_H__

#include <jtypes.h>
#include <jos.h>
#include <ptp_types.h>

/* Datacodes */

typedef enum 
{
    MTP_OBJ_PROP_STORAGE_ID                      = 0xDC01,
    MTP_OBJ_PROP_OBJECT_FORMAT                   = 0xDC02,
    MTP_OBJ_PROP_PROTECTION_STATUS               = 0xDC03,
    MTP_OBJ_PROP_OBJECT_SIZE                     = 0xDC04,
    MTP_OBJ_PROP_ASSOCIATION_TYPE                = 0xDC05,
    MTP_OBJ_PROP_ASSOCIATION_DESC                = 0xDC06,
    MTP_OBJ_PROP_OBJECT_FILENAME                 = 0xDC07,
    MTP_OBJ_PROP_DATE_CREATED                    = 0xDC08,
    MTP_OBJ_PROP_DATE_MODIFIED                   = 0xDC09,
    MTP_OBJ_PROP_KEYWORDS                        = 0xDC0A,
    MTP_OBJ_PROP_PARENT_OBJECT                   = 0xDC0B,
    MTP_OBJ_PROP_ALLOWED_FOLDER_CONTENTS         = 0xDC0C,
    MTP_OBJ_PROP_HIDDEN                          = 0xDC0D,
    MTP_OBJ_PROP_SYSTEM_OBJECT                   = 0xDC0E,
    MTP_OBJ_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER = 0xDC41,
    MTP_OBJ_PROP_SYNC_ID                         = 0xDC42,
    MTP_OBJ_PROP_PROPERTY_BAG                    = 0xDC43,
    MTP_OBJ_PROP_NAME                            = 0xDC44,
    MTP_OBJ_PROP_CREATED_BY                      = 0xDC45,
    MTP_OBJ_PROP_ARTIST                          = 0xDC46,
    MTP_OBJ_PROP_DATE_AUTHORED                   = 0xDC47,
    MTP_OBJ_PROP_DESCRIPTION                     = 0xDC48,
    MTP_OBJ_PROP_URL_REFERENCE                   = 0xDC49,
    MTP_OBJ_PROP_LANGUAGE_LOCALE                 = 0xDC4A,
    MTP_OBJ_PROP_COPYRIGHT_INFORMATION           = 0xDC4B,
    MTP_OBJ_PROP_SOURCE                          = 0xDC4C,
    MTP_OBJ_PROP_ORIGIN_LOCATION                 = 0xDC4D,
    MTP_OBJ_PROP_DATE_ADDED                      = 0xDC4E,
    MTP_OBJ_PROP_NON_CONSUMABLE                  = 0xDC4F,
    MTP_OBJ_PROP_CORRUPT_UNPLAYABLE              = 0xDC50,
    MTP_OBJ_PROP_PRODUCER_SERIAL_NUMBER          = 0xDC51,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_FORMAT    = 0xDC81,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_SIZE      = 0xDC82,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_HEIGHT    = 0xDC83,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_WIDTH     = 0xDC84,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DURATION  = 0xDC85,
    MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DATA      = 0xDC86,
    MTP_OBJ_PROP_WIDTH                           = 0xDC87,
    MTP_OBJ_PROP_HEIGHT                          = 0xDC88,
    MTP_OBJ_PROP_DURATION                        = 0xDC89,
    MTP_OBJ_PROP_RATING                          = 0xDC8A,
    MTP_OBJ_PROP_TRACK                           = 0xDC8B,
    MTP_OBJ_PROP_GENRE                           = 0xDC8C,
    MTP_OBJ_PROP_CREDITS                         = 0xDC8D,
    MTP_OBJ_PROP_LYRICS                          = 0xDC8E,
    MTP_OBJ_PROP_SUBSCRIPTION_CONTENT_ID         = 0xDC8F,
    MTP_OBJ_PROP_PRODUCED_BY                     = 0xDC90,
    MTP_OBJ_PROP_USE_COUNT                       = 0xDC91,
    MTP_OBJ_PROP_SKIP_COUNT                      = 0xDC92,
    MTP_OBJ_PROP_LAST_ACCESSED                   = 0xDC93,
    MTP_OBJ_PROP_PARENTAL_RATING                 = 0xDC94,
    MTP_OBJ_PROP_META_GENRE                      = 0xDC95,
    MTP_OBJ_PROP_COMPOSER                        = 0xDC96,
    MTP_OBJ_PROP_EFFECTIVE_RATING                = 0xDC97,
    MTP_OBJ_PROP_SUBTITLE                        = 0xDC98,
    MTP_OBJ_PROP_ORIGINAL_RELEASE_DATE           = 0xDC99,
    MTP_OBJ_PROP_ALBUM_NAME                      = 0xDC9A,
    MTP_OBJ_PROP_ALBUM_ARTIST                    = 0xDC9B,
    MTP_OBJ_PROP_MOOD                            = 0xDC9C,
    MTP_OBJ_PROP_DRM_STATUS                      = 0xDC9D,
    MTP_OBJ_PROP_SUB_DESCRIPTION                 = 0xDC9E,
    MTP_OBJ_PROP_IS_CROPPED                      = 0xDCD1,
    MTP_OBJ_PROP_IS_COLOUR_CORRECTED             = 0xDCD2,
    MTP_OBJ_PROP_IMAGE_BIT_DEPTH                 = 0xDCD3,
    MTP_OBJ_PROP_FNUMBER                         = 0xDCD4,
    MTP_OBJ_PROP_EXPOSURE_TIME                   = 0xDCD5,
    MTP_OBJ_PROP_EXPOSURE_INDEX                  = 0xDCD6,
    MTP_OBJ_PROP_TOTAL_BITRATE                   = 0xDE91,
    MTP_OBJ_PROP_BITRATE_TYPE                    = 0xDE92,
    MTP_OBJ_PROP_SAMPLE_RATE                     = 0xDE93,
    MTP_OBJ_PROP_NUMBER_OF_CHANNELS              = 0xDE94,
    MTP_OBJ_PROP_AUDIO_BIT_DEPTH                 = 0xDE95,
    MTP_OBJ_PROP_SCAN_TYPE                       = 0xDE97,
    MTP_OBJ_PROP_AUDIO_WAVE_CODEC                = 0xDE99,
    MTP_OBJ_PROP_AUDIO_BITRATE                   = 0xDE9A,
    MTP_OBJ_PROP_VIDEO_FOURCC_CODEC              = 0xDE9B,
    MTP_OBJ_PROP_VIDEO_BITRATE                   = 0xDE9C,
    MTP_OBJ_PROP_FRAMES_PER_THOUSAND_SECONDS     = 0xDE9D,
    MTP_OBJ_PROP_KEY_FRAME_DISTANCE              = 0xDE9E,
    MTP_OBJ_PROP_BUFFER_SIZE                     = 0xDE9F,
    MTP_OBJ_PROP_ENCODING_QUALITY                = 0xDEA0,
    MTP_OBJ_PROP_ENCODING_PROFILE                = 0xDEA1,
    MTP_OBJ_PROP_DISPLAY_NAME                    = 0xDCE0,
    MTP_OBJ_PROP_BODY_TEXT                       = 0xDCE1,
    MTP_OBJ_PROP_SUBJECT                         = 0xDCE2,
    MTP_OBJ_PROP_PRIORITY                        = 0xDCE3,
    MTP_OBJ_PROP_GIVEN_NAME                      = 0xDD00,
    MTP_OBJ_PROP_MIDDLE_NAMES                    = 0xDD01,
    MTP_OBJ_PROP_FAMILY_NAME                     = 0xDD02,
    MTP_OBJ_PROP_PREFIX                          = 0xDD03,
    MTP_OBJ_PROP_SUFFIX                          = 0xDD04,
    MTP_OBJ_PROP_PHONETIC_GIVEN_NAME             = 0xDD05,
    MTP_OBJ_PROP_PHONETIC_FAMILY_NAME            = 0xDD06,
    MTP_OBJ_PROP_EMAIL_PRIMARY                   = 0xDD07,
    MTP_OBJ_PROP_EMAIL_PERSONAL_1                = 0xDD08,
    MTP_OBJ_PROP_EMAIL_PERSONAL_2                = 0xDD09,
    MTP_OBJ_PROP_EMAIL_BUSINESS_1                = 0xDD0A,
    MTP_OBJ_PROP_EMAIL_BUSINESS_2                = 0xDD0B,
    MTP_OBJ_PROP_EMAIL_OTHERS                    = 0xDD0C,
    MTP_OBJ_PROP_PHONE_NUMBER_PRIMARY            = 0xDD0D,
    MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL           = 0xDD0E,
    MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL_2         = 0xDD0F,
    MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS           = 0xDD10,
    MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS_2         = 0xDD11,
    MTP_OBJ_PROP_PHONE_NUMBER_MOBILE             = 0xDD12,
    MTP_OBJ_PROP_PHONE_NUMBER_MOBILE_2           = 0xDD13,
    MTP_OBJ_PROP_FAX_NUMBER_PRIMARY              = 0xDD14,
    MTP_OBJ_PROP_FAX_NUMBER_PERSONAL             = 0xDD15,
    MTP_OBJ_PROP_FAX_NUMBER_BUSINESS             = 0xDD16,
    MTP_OBJ_PROP_PAGER_NUMBER                    = 0xDD17,
    MTP_OBJ_PROP_PHONE_NUMBER_OTHERS             = 0xDD18,
    MTP_OBJ_PROP_PRIMARY_WEB_ADDRESS             = 0xDD19,
    MTP_OBJ_PROP_PERSONAL_WEB_ADDRESS            = 0xDD1A,
    MTP_OBJ_PROP_BUSINESS_WEB_ADDRESS            = 0xDD1B,
    MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS       = 0xDD1C,
    MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_2     = 0xDD1D,
    MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_3     = 0xDD1E,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_FULL    = 0xDD1F,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_1  = 0xDD20,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_2  = 0xDD21,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_CITY    = 0xDD22,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_REGION  = 0xDD23,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE    = 0xDD24,
    MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_COUNTRY = 0xDD25,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_FULL    = 0xDD26,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_1  = 0xDD27,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_2  = 0xDD28,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_CITY    = 0xDD29,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_REGION  = 0xDD2A,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE    = 0xDD2B,
    MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_COUNTRY = 0xDD2C,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_FULL       = 0xDD2D,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_1     = 0xDD2E,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_2     = 0xDD2F,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_CITY       = 0xDD30,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_REGION     = 0xDD31,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_POSTAL_CODE       = 0xDD32,
    MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_COUNTRY    = 0xDD33,
    MTP_OBJ_PROP_ORGANIZATION_NAME               = 0xDD34,
    MTP_OBJ_PROP_PHONETIC_ORGANIZATION_NAME      = 0xDD35,
    MTP_OBJ_PROP_ROLE                            = 0xDD36,
    MTP_OBJ_PROP_BIRTHDATE                       = 0xDD37,
    MTP_OBJ_PROP_MESSAGE_TO                      = 0xDD40,
    MTP_OBJ_PROP_MESSAGE_CC                      = 0xDD41,
    MTP_OBJ_PROP_MESSAGE_BCC                     = 0xDD42,
    MTP_OBJ_PROP_MESSAGE_READ                    = 0xDD43,
    MTP_OBJ_PROP_MESSAGE_RECEIVED_TIME           = 0xDD44,
    MTP_OBJ_PROP_MESSAGE_SENDER                  = 0xDD45,
    MTP_OBJ_PROP_ACTIVITY_BEGIN_TIME             = 0xDD50,
    MTP_OBJ_PROP_ACTIVITY_END_TIME               = 0xDD51,
    MTP_OBJ_PROP_ACTIVITY_LOCATION               = 0xDD52,
    MTP_OBJ_PROP_ACTIVITY_REQUIRED_ATTENDEES     = 0xDD54,
    MTP_OBJ_PROP_ACTIVITY_OPTIONAL_ATTENDEES     = 0xDD55,
    MTP_OBJ_PROP_ACTIVITY_RESOURCES              = 0xDD56,
    MTP_OBJ_PROP_ACTIVITY_ACCEPTED               = 0xDD57,
    MTP_OBJ_PROP_ACTIVITY_TENTATIVE              = 0xDD58,
    MTP_OBJ_PROP_ACTIVITY_DECLINED               = 0xDD59,
    MTP_OBJ_PROP_ACTIVITY_REMINDER_TIME          = 0xDD5A,
    MTP_OBJ_PROP_ACTIVITY_OWNER                  = 0xDD5B,
    MTP_OBJ_PROP_ACTIVITY_STATUS                 = 0xDD5C,
    MTP_OBJ_PROP_OWNER                           = 0xDD5D,
    MTP_OBJ_PROP_EDITOR                          = 0xDD5E,
    MTP_OBJ_PROP_WEBMASTER                       = 0xDD5F,
    MTP_OBJ_PROP_URL_SOURCE                      = 0xDD60,
    MTP_OBJ_PROP_URL_DESTINATION                 = 0xDD61,
    MTP_OBJ_PROP_TIME_BOOKMARK                   = 0xDD62,
    MTP_OBJ_PROP_OBJECT_BOOKMARK                 = 0xDD63,
    MTP_OBJ_PROP_BYTE_BOOKMARK                   = 0xDD64,
    MTP_OBJ_PROP_LAST_BUILD_DATE                 = 0xDD70,
    MTP_OBJ_PROP_TIME_TO_LIVE                    = 0xDD71,
    MTP_OBJ_PROP_MEDIA_GUID                      = 0xDD72,
    MTP_OBJ_PROP_LAST
} mtp_obj_prop_t;

enum mtp_meta_genre
{
    META_GENRE_UNUSED               = 0x0000,
    GENERIC_MUSIC_AUDIO_FILE        = 0x0001,
    GENERIC_NON_MUSIC_AUDIO_FILE    = 0x0011,
    SPOKEN_WORD_AUDIO_BOOK_FILE     = 0x0012,
    SPOKEN_WORD_NON_AUDIO_BOOK_FILE = 0x0013,
    SPOKEN_WORD_NEWS                = 0x0014,
    SPOKEN_WORD_TALK_SHOWS          = 0x0015,
    GENERIC_MUSIC_VIDEO_FILE        = 0x0021,
    NEWS_VIDEO_FILE                 = 0x0022,
    MUSIC_VIDEO_FILE                = 0x0023,
    HOME_VIDEO_FILE                 = 0x0024,
    FEATURE_FILM_VIDEO_FILE         = 0x0025,
    TELEVISION_SHOW_VIDEO_FILE      = 0x0026,
    TRAIN_EDU_VIDEO_FILE            = 0x0027,
    PHOTO_MONTAGE_VIDEO_FILE        = 0x0028,
    GENERIC_NON_AUDIO_NON_VIDEO     = 0x0030,
    META_GENRE_LAST
};

enum mtp_drm_status
{
    EXCLUDE_DRM                     = 0x0000,
    INCLUDE_DRM                     = 0x0001,
    LAST_DRM_STATUS
};

enum mtp_crop
{
    NOT_CROPPED                     = 0x0000,
    CROPPED                         = 0x0001,
    MUST_NOT_CROP                   = 0x0002,
    LAST_MTP_CROP
};

enum mtp_colour_correct
{
    NOT_COLOR_CORRECTED             = 0x0000,
    COLOR_CORRECTED                 = 0x0001,
    MUST_NOT_COLOR_CORRECT          = 0x0002,
    LAST_MTP_COLOUR_CORRECT
};

enum mtp_bitrate_type
{
    BITRATE_TYPE_UNUSED             = 0x0000,
    BITRATE_TYPE_DISCRETE           = 0x0001,
    BITRATE_TYPE_VARIABLE           = 0x0002,
    BITRATE_TYPE_FREE               = 0x0003,
    LAST_MTP_BITRATE_TYPE
};

enum mtp_channels_count
{
    NUMBER_OF_CHANNELS_UNUSED      = 0x0000,
    NUMBER_OF_CHANNELS_1           = 0x0001,
    NUMBER_OF_CHANNELS_2           = 0x0002,
    NUMBER_OF_CHANNELS_2_1         = 0x0003,
    NUMBER_OF_CHANNELS_3           = 0x0004,
    NUMBER_OF_CHANNELS_3_1         = 0x0005,
    NUMBER_OF_CHANNELS_4           = 0x0006,
    NUMBER_OF_CHANNELS_4_1         = 0x0007,
    NUMBER_OF_CHANNELS_5           = 0x0008,
    NUMBER_OF_CHANNELS_5_1         = 0x0009,
    NUMBER_OF_CHANNELS_6           = 0x000A,
    NUMBER_OF_CHANNELS_6_1         = 0x000B,
    NUMBER_OF_CHANNELS_7           = 0x000C,
    NUMBER_OF_CHANNELS_7_1         = 0x000D,
    NUMBER_OF_CHANNELS_8           = 0x000E,
    NUMBER_OF_CHANNELS_8_1         = 0x000F,
    NUMBER_OF_CHANNELS_9           = 0x0010,
    NUMBER_OF_CHANNELS_9_1         = 0x0011,
    MTP_LAST_CHANNEL_COUNT
};

enum mtp_scan_type
{
    SCAN_TYPE_UNUSED                = 0x0000,
    PROGRESSIVE                     = 0x0001,
    FIELD_INTERLEAVED_UPPER_FIRST   = 0x0002,
    FIELD_INTERLEAVED_LOWER_FIRST   = 0x0003,
    FIELD_SINGLE_UPPER_FIRST        = 0x0004,
    FIELD_SINGLE_LOWER_FIRST        = 0x0005,
    MIXED_INTERLACE                 = 0x0006,
    MIXED_INTERLACE_AND_PROGRESSIVE = 0x0007,
    MTP_LAST_SCAN_TYPE
};

/* Property group codes */
typedef enum group_code
{
    GROUP_CODE_NONE           = 0x00000000,
    GROUP_CODE_SYNC_PROPS     = 0x00000001,
    GROUP_CODE_UI_PROPS       = 0x00000002,
    GROUP_CODE_OBJ_INFO       = 0x00000004,
    GROUP_CODE_OFTEN_USED     = 0x00000008,
    GROUP_CODE_SUPPLEMENTAL   = 0x00000100,
    GROUP_CODE_UNKNOWN_PROP   = 0x00010000,
    GROUP_CODE_SLOW           = 0x00FF0000,
    GROUP_CODE_ALL            = 0xFFFFFFFF
} mtp_obj_prop_group_t;

/* MTP Datasets */

/* MTP object description Dataset */
typedef struct
{
    mtp_obj_prop_t               prop_code;
    ptp_type_t                   data_type;
    jbool_t                       writable;
    ptp_prop_value_t             default_value;
    mtp_obj_prop_group_t         group_code;
} mtp_prop_desc_t;

#define MTP_DATETIME_FORM        0x03
#define MTP_FIXED_LEN_FORM       0x04
#define MTP_REGEXP_FORM          0x05
#define MTP_BYTE_ARR_FORM        0x06
#define MTP_LONG_STR_FORM        0xFF
typedef struct 
{
    mtp_prop_desc_t             *prop_desc;
    juint8_t                      form_flag;
    ptp_prop_value_t            *allowed_values;
    juint16_t                     num_allowed_values;
} mtp_obj_prop_desc_t;

typedef struct _mtp_obj_prop_value_elem
{
    ptp_obj_handle_t             obj_handle;
    mtp_obj_prop_t               prop_code;
    ptp_datacode_type_t          prop_data_type;
    ptp_prop_value_t             value;
} mtp_obj_prop_elem_t;

typedef struct 
{
    juint16_t                     num_of_prop_descs;
    mtp_obj_prop_desc_t         *obj_prop_desc;
} interdep_obj_prop_t;

#endif /*__MTP_TYPES_H__*/
