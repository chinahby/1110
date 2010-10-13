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

#include "sicd_sample.h"
#include "obj_props.h"

#ifndef DESC_GET_SET_DEVICE_DEFINED
#error DESC_GET_SET_DEVICE_DEFINED - flag need to be defined by device
#endif
#ifndef DESC_FORM_FIXED_LEN_BYTE_ARRAY
#error DESC_FORM_FIXED_LEN_BYTE_ARRAY - device limitation for fixed byte array form
#endif
#ifndef DESC_FORM_MAX_BYTE_ARR
#error DESC_FORM_MAX_BYTE_ARR - device limitation for byte array form
#endif
#ifndef DESC_FORM_MAX_LONG_STR
#error DESC_FORM_MAX_LONG_STR - device limitation for long string form
#endif

static char reg_exp_url_reference[] = "((http:)?/{0,2}([^g?#\\[\\];:&=\\+\\$,]*"
"\\.)+([^/\\?#\\[\\];:&=\\+\\$,]{2,3}))(/[^<>])*";

static char reg_exp_filename[] = "[a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~"
"][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0,7}\\.[[a-zA-Z!#\\$%&'\\(\\)"
"\\-0-9@\\^_\\`\\{\\}\\~][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0,2}]?";

typedef enum
{
    storage_id, 
    object_format,
    protection_status,
    object_size,
    association_type,
    association_desc,
    object_filename,
    date_created,
    date_modified,
    keywords,
    parent_object,
    allowed_folder_contents,
    hidden,
    system_object,
    persistent_unique_object_identifier,
    sync_id,
    property_bag,
    name,
    created_by,
    artist,
    date_authored,
    description,
    url_reference,
    language_locale,
    copyright_information,
    source,
    origin_location,
    date_added,
    non_consumable,
    corrupt_unplayable,
    producer_serial_number,
    representative_sample_format,
    representative_sample_size,
    representative_sample_height,
    representative_sample_width,
    representative_sample_duration,
    representative_sample_data,
    width,
    height,
    duration,
    rating,
    track,
    genre,
    credits,
    lyrics,
    subscription_content_id,
    produced_by,
    use_count,
    skip_count,
    last_accessed,
    parental_rating,
    meta_genre,
    composer,
    effective_rating,
    subtitle,
    original_release_date,
    album_name,
    album_artist,
    mood,
    drm_status,
    sub_description,
    is_cropped,
    is_colour_corrected,
    image_bit_depth,
    fnumber,
    exposure_time,
    exposure_index,
    total_bitrate,
    bitrate_type,
    sample_rate,
    number_of_channels,
    audio_bit_depth,
    scan_type,
    audio_wave_codec,
    audio_bitrate,
    video_fourcc_codec,
    video_bitrate,
    frames_per_thousand_seconds,
    key_frame_distance,
    buffer_size,
    encoding_quality,
    encoding_profile,
    display_name,
    body_text,
    subject,
    priority,
    given_name,
    middle_names,
    family_name,
    prefix,
    suffix,
    phonetic_given_name,
    phonetic_family_name,
    email_primary,
    email_personal_1,
    email_personal_2,
    email_business_1,
    email_business_2,
    email_others,
    phone_number_primary,
    phone_number_personal,
    phone_number_personal_2,
    phone_number_business,
    phone_number_business_2,
    phone_number_mobile,
    phone_number_mobile_2,
    fax_number_primary,
    fax_number_personal,
    fax_number_business,
    pager_number,
    phone_number_others,
    primary_web_address,
    personal_web_address,
    business_web_address,
    instant_messenger_address,
    instant_messenger_address_2,
    instant_messenger_address_3,
    postal_address_personal_full,
    postal_address_personal_line_1,
    postal_address_personal_line_2,
    postal_address_personal_city,
    postal_address_personal_region,
    postal_address_personal_postal_code,
    postal_address_personal_country,
    postal_address_business_full,
    postal_address_business_line_1,
    postal_address_business_line_2,
    postal_address_business_city,
    postal_address_business_region,
    postal_address_business_postal_code,
    postal_address_business_country,
    postal_address_other_full,
    postal_address_other_line_1,
    postal_address_other_line_2,
    postal_address_other_city,
    postal_address_other_region,
    postal_address_other_postal_code,
    postal_address_other_country,
    organization_name,
    phonetic_organization_name,
    role,
    birthdate,
    message_to,
    message_cc,
    message_bcc,
    message_read,
    message_received_time,
    message_sender,
    activity_begin_time,
    activity_end_time,
    activity_location,
    activity_required_attendees,
    activity_optional_attendees,
    activity_resources,
    activity_accepted,
    activity_tentative,
    activity_declined,
    activity_reminder_time,
    activity_owner,
    activity_status,
    owner,
    editor,
    webmaster,
    url_source,
    url_destination,
    time_bookmark,
    object_bookmark,
    byte_bookmark,
    last_build_date,
    time_to_live,
    media_guid
} desc_prop_index;

mtp_prop_desc_t prop_desc_map[MTP_PROPERTY_COUNT] = 
{
    { /* 1 */
        MTP_OBJ_PROP_STORAGE_ID, 
        PTP_TYPE_UINT32, 
        DESC_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 2 */
        MTP_OBJ_PROP_OBJECT_FORMAT,
        PTP_TYPE_UINT16, 
        DESC_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 3 */
        MTP_OBJ_PROP_PROTECTION_STATUS,
        PTP_TYPE_UINT16, 
        DESC_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 4 */
        MTP_OBJ_PROP_OBJECT_SIZE,
        PTP_TYPE_UINT64, 
        DESC_GET, 
        {PTP_TYPE_UINT64, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 5 */
        MTP_OBJ_PROP_ASSOCIATION_TYPE,
        PTP_TYPE_UINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 6 */
        MTP_OBJ_PROP_ASSOCIATION_DESC,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 7 */
        MTP_OBJ_PROP_OBJECT_FILENAME,
        PTP_TYPE_STR, 
        DESC_SET_GET,
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 8 */
        MTP_OBJ_PROP_DATE_CREATED,
        PTP_TYPE_STR, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 9 */
        MTP_OBJ_PROP_DATE_MODIFIED,
        PTP_TYPE_STR, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 10 */
        MTP_OBJ_PROP_KEYWORDS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 11 */
        MTP_OBJ_PROP_PARENT_OBJECT,
        PTP_TYPE_UINT32, 
        DESC_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 12 */
        MTP_OBJ_PROP_ALLOWED_FOLDER_CONTENTS,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 13 */
        MTP_OBJ_PROP_HIDDEN,
        PTP_TYPE_UINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 14 */
        MTP_OBJ_PROP_SYSTEM_OBJECT,
        PTP_TYPE_UINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 15 */
        MTP_OBJ_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        PTP_TYPE_UINT128, 
        DESC_GET, 
        {PTP_TYPE_UINT128, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 16 */
        MTP_OBJ_PROP_SYNC_ID,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 17 */
        MTP_OBJ_PROP_PROPERTY_BAG,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 18 */
        MTP_OBJ_PROP_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 19 */
        MTP_OBJ_PROP_CREATED_BY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 20 */
        MTP_OBJ_PROP_ARTIST,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 21 */
        MTP_OBJ_PROP_DATE_AUTHORED,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 22 */
        MTP_OBJ_PROP_DESCRIPTION,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 23 */
        MTP_OBJ_PROP_URL_REFERENCE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 24 */
        MTP_OBJ_PROP_LANGUAGE_LOCALE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 25 */
        MTP_OBJ_PROP_COPYRIGHT_INFORMATION,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 26 */
        MTP_OBJ_PROP_SOURCE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 27 */
        MTP_OBJ_PROP_ORIGIN_LOCATION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 28 */
        MTP_OBJ_PROP_DATE_ADDED,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 29 */
        MTP_OBJ_PROP_NON_CONSUMABLE,
        PTP_TYPE_UINT8, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT8, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 30 */
        MTP_OBJ_PROP_CORRUPT_UNPLAYABLE,
        PTP_TYPE_UINT8, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT8, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 31 */
        MTP_OBJ_PROP_PRODUCER_SERIAL_NUMBER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 32 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_FORMAT,
        PTP_TYPE_UINT16, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 33 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_SIZE,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 34 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_HEIGHT,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 35 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_WIDTH,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 36 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DURATION,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 37 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DATA,
        PTP_TYPE_AUINT8, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_AUINT8, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 38 */
        MTP_OBJ_PROP_WIDTH,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 39 */
        MTP_OBJ_PROP_HEIGHT,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 40 */
        MTP_OBJ_PROP_DURATION,
        PTP_TYPE_UINT32, 
        DESC_GET_SET_DEVICE_DEFINED, /* pay attention */ 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 41 */
        MTP_OBJ_PROP_RATING,
        PTP_TYPE_UINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 42 */
        MTP_OBJ_PROP_TRACK,
        PTP_TYPE_UINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 43 */
        MTP_OBJ_PROP_GENRE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 44 */
        MTP_OBJ_PROP_CREDITS,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 45 */
        MTP_OBJ_PROP_LYRICS,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 46 */
        MTP_OBJ_PROP_SUBSCRIPTION_CONTENT_ID,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 47 */
        MTP_OBJ_PROP_PRODUCED_BY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 48 */
        MTP_OBJ_PROP_USE_COUNT,
        PTP_TYPE_UINT32,
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 49 */
        MTP_OBJ_PROP_SKIP_COUNT,
        PTP_TYPE_UINT32,
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 50 */
        MTP_OBJ_PROP_LAST_ACCESSED,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 51 */
        MTP_OBJ_PROP_PARENTAL_RATING,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 52 */
        MTP_OBJ_PROP_META_GENRE,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 53 */
        MTP_OBJ_PROP_COMPOSER,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 54 */
        MTP_OBJ_PROP_EFFECTIVE_RATING,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 55 */
        MTP_OBJ_PROP_SUBTITLE,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 56 */
        MTP_OBJ_PROP_ORIGINAL_RELEASE_DATE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 57 */
        MTP_OBJ_PROP_ALBUM_NAME,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 58 */
        MTP_OBJ_PROP_ALBUM_ARTIST,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 59 */
        MTP_OBJ_PROP_MOOD,
        PTP_TYPE_STR,
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 60 */
        MTP_OBJ_PROP_DRM_STATUS,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 61 */
        MTP_OBJ_PROP_SUB_DESCRIPTION,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 62 */
        MTP_OBJ_PROP_IS_CROPPED,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 63 */
        MTP_OBJ_PROP_IS_COLOUR_CORRECTED,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 64 */
        MTP_OBJ_PROP_IMAGE_BIT_DEPTH,
        PTP_TYPE_UINT32,
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 65 */
        MTP_OBJ_PROP_FNUMBER,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 66 */
        MTP_OBJ_PROP_EXPOSURE_TIME,
        PTP_TYPE_UINT32,
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 67 */
        MTP_OBJ_PROP_EXPOSURE_INDEX,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 68 */
        MTP_OBJ_PROP_TOTAL_BITRATE,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 69 */
        MTP_OBJ_PROP_BITRATE_TYPE,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 70 */
        MTP_OBJ_PROP_SAMPLE_RATE,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 71 */
        MTP_OBJ_PROP_NUMBER_OF_CHANNELS,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 72 */
        MTP_OBJ_PROP_AUDIO_BIT_DEPTH,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 73 */
        MTP_OBJ_PROP_SCAN_TYPE,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 74 */
        MTP_OBJ_PROP_AUDIO_WAVE_CODEC,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 75 */
        MTP_OBJ_PROP_AUDIO_BITRATE,
        PTP_TYPE_UINT32, 
        DESC_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 76 */
        MTP_OBJ_PROP_VIDEO_FOURCC_CODEC,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 77 */
        MTP_OBJ_PROP_VIDEO_BITRATE,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 78 */
        MTP_OBJ_PROP_FRAMES_PER_THOUSAND_SECONDS,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 79 */
        MTP_OBJ_PROP_KEY_FRAME_DISTANCE,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 80 */
        MTP_OBJ_PROP_BUFFER_SIZE,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 81 */
        MTP_OBJ_PROP_ENCODING_QUALITY,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 82 */
        MTP_OBJ_PROP_ENCODING_PROFILE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 83 */
        MTP_OBJ_PROP_DISPLAY_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 84 */
        MTP_OBJ_PROP_BODY_TEXT,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 85 */
        MTP_OBJ_PROP_SUBJECT,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 86 */
        MTP_OBJ_PROP_PRIORITY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 87 */
        MTP_OBJ_PROP_GIVEN_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 88 */
        MTP_OBJ_PROP_MIDDLE_NAMES,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 89 */
        MTP_OBJ_PROP_FAMILY_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 90 */
        MTP_OBJ_PROP_PREFIX,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 91 */
        MTP_OBJ_PROP_SUFFIX,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 92 */
        MTP_OBJ_PROP_PHONETIC_GIVEN_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 93 */
        MTP_OBJ_PROP_PHONETIC_FAMILY_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 94 */
        MTP_OBJ_PROP_EMAIL_PRIMARY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 95 */
        MTP_OBJ_PROP_EMAIL_PERSONAL_1,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 96 */
        MTP_OBJ_PROP_EMAIL_PERSONAL_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 97 */
        MTP_OBJ_PROP_EMAIL_BUSINESS_1,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 98 */
        MTP_OBJ_PROP_EMAIL_BUSINESS_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 99 */
        MTP_OBJ_PROP_EMAIL_OTHERS,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 100 */
        MTP_OBJ_PROP_PHONE_NUMBER_PRIMARY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 101 */
        MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 102 */
        MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 103 */
        MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 104 */
        MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 105 */
        MTP_OBJ_PROP_PHONE_NUMBER_MOBILE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 106 */
        MTP_OBJ_PROP_PHONE_NUMBER_MOBILE_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 107 */
        MTP_OBJ_PROP_FAX_NUMBER_PRIMARY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 108 */
        MTP_OBJ_PROP_FAX_NUMBER_PERSONAL,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 109 */
        MTP_OBJ_PROP_FAX_NUMBER_BUSINESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 110 */
        MTP_OBJ_PROP_PAGER_NUMBER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 111 */
        MTP_OBJ_PROP_PHONE_NUMBER_OTHERS,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 112 */
        MTP_OBJ_PROP_PRIMARY_WEB_ADDRESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 113 */
        MTP_OBJ_PROP_PERSONAL_WEB_ADDRESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 114 */
        MTP_OBJ_PROP_BUSINESS_WEB_ADDRESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 115 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 116 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 117 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_3,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 118 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_FULL,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 119 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_1,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 120 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 121 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_CITY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 122 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_REGION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 123 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 124 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_COUNTRY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 125 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_FULL,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 126 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_1,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 127 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 128 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_CITY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 129 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_REGION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 130 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 131 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_COUNTRY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 132 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_FULL,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 133 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_1,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 134 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_2,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 135 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_CITY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 136 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_REGION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 137 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_POSTAL_CODE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 138 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_COUNTRY,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 139 */
        MTP_OBJ_PROP_ORGANIZATION_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 140 */
        MTP_OBJ_PROP_PHONETIC_ORGANIZATION_NAME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 141 */
        MTP_OBJ_PROP_ROLE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 142 */
        MTP_OBJ_PROP_BIRTHDATE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 143 */
        MTP_OBJ_PROP_MESSAGE_TO,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 144 */
        MTP_OBJ_PROP_MESSAGE_CC,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 145 */
        MTP_OBJ_PROP_MESSAGE_BCC,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 146 */
        MTP_OBJ_PROP_MESSAGE_READ,
        PTP_TYPE_UINT16,
        DESC_SET_GET, 
        {PTP_TYPE_UINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 147 */
        MTP_OBJ_PROP_MESSAGE_RECEIVED_TIME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 148 */
        MTP_OBJ_PROP_MESSAGE_SENDER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 149 */
        MTP_OBJ_PROP_ACTIVITY_BEGIN_TIME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 150 */
        MTP_OBJ_PROP_ACTIVITY_END_TIME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 151 */
        MTP_OBJ_PROP_ACTIVITY_LOCATION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 152 */
        MTP_OBJ_PROP_ACTIVITY_REQUIRED_ATTENDEES,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 153 */
        MTP_OBJ_PROP_ACTIVITY_OPTIONAL_ATTENDEES,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 154 */
        MTP_OBJ_PROP_ACTIVITY_RESOURCES,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 155 */
        MTP_OBJ_PROP_ACTIVITY_ACCEPTED,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 156 */
        MTP_OBJ_PROP_ACTIVITY_TENTATIVE,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 157 */
        MTP_OBJ_PROP_ACTIVITY_DECLINED,
        PTP_TYPE_AUINT16, 
        DESC_SET_GET, 
        {PTP_TYPE_AUINT16, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 158 */
        MTP_OBJ_PROP_ACTIVITY_REMINDER_TIME,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 159 */
        MTP_OBJ_PROP_ACTIVITY_OWNER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 160 */
        MTP_OBJ_PROP_ACTIVITY_STATUS,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 161 */
        MTP_OBJ_PROP_OWNER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 162 */
        MTP_OBJ_PROP_EDITOR,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 163 */
        MTP_OBJ_PROP_WEBMASTER,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 164 */
        MTP_OBJ_PROP_URL_SOURCE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 165 */
        MTP_OBJ_PROP_URL_DESTINATION,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 166 */
        MTP_OBJ_PROP_TIME_BOOKMARK,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 167 */
        MTP_OBJ_PROP_OBJECT_BOOKMARK,
        PTP_TYPE_UINT32, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT32, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 168 */
        MTP_OBJ_PROP_BYTE_BOOKMARK,
        PTP_TYPE_UINT64, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT64, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 169 */
        MTP_OBJ_PROP_LAST_BUILD_DATE,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 170 */
        MTP_OBJ_PROP_TIME_TO_LIVE,
        PTP_TYPE_UINT64, 
        DESC_SET_GET, 
        {PTP_TYPE_UINT64, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    },
    { /* 171 */
        MTP_OBJ_PROP_MEDIA_GUID,
        PTP_TYPE_STR, 
        DESC_SET_GET, 
        {PTP_TYPE_STR, {0}, 0, 0, 0},
        DESC_GROUP_CODE_DEVICE_DEFINED,
    }
};


/* default object property descriptors allowed values */

static ptp_prop_value_t protection_status_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0/*0x0000*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x0001*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x8002*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x8003*/}, 0, 0, 0 },
};

static ptp_prop_value_t non_consumable_allowed_values[] = 
{
    { PTP_TYPE_UINT8, {0/*0x00*/}, 0, 0, 0 },
    { PTP_TYPE_UINT8, {0/*0x01*/}, 0, 0, 0 },
};

static ptp_prop_value_t rating_allowed_values[] = 
{
    { PTP_TYPE_UINT8, {0/*1*/  }, 0, 0, 0 },   /* min */ 
    { PTP_TYPE_UINT8, {0/*100*/}, 0, 0, 0 },   /* max */
    { PTP_TYPE_UINT8, {0/*1*/  }, 0, 0, 0 }    /* step */
};

static ptp_prop_value_t audio_bitrate_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0/*5000*/  }, 0, 0, 0 },   /* min */ 
    { PTP_TYPE_UINT32, {0/*320000*/}, 0, 0, 0 },   /* max */
    { PTP_TYPE_UINT32, {0/*1*/     }, 0, 0, 0 }    /* step */
};

static ptp_prop_value_t sample_rate_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0/*8000*/ }, 0, 0, 0 }, /* min */ 
    { PTP_TYPE_UINT32, {0/*48000*/}, 0, 0, 0 }, /* max */
    { PTP_TYPE_UINT32, {0/*50*/   }, 0, 0, 0 }  /* step */
};

static ptp_prop_value_t number_of_channels_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0/*0x0001*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x0002*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x0003*/}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0/*0x0004*/}, 0, 0, 0 },
};

static ptp_prop_value_t duration_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0/*60*/  }, 0, 0, 0 }, /* min */ 
    { PTP_TYPE_UINT32, {0/*6000*/}, 0, 0, 0 }, /* max */
    { PTP_TYPE_UINT32, {0/*1*/   }, 0, 0, 0 }  /* step */
};

static ptp_prop_value_t audio_wav_codec_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0/*0*/}, 0, 0, 0 },
    { PTP_TYPE_UINT32, {0/*1*/}, 0, 0, 0 },
    { PTP_TYPE_UINT32, {0/*2*/}, 0, 0, 0 },
    { PTP_TYPE_UINT32, {0/*85*/}, 0, 0, 0 }
};

static ptp_prop_value_t jpg_object_filename_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

static ptp_prop_value_t jpg_url_reference_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

static ptp_prop_value_t mp3_object_filename_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

static ptp_prop_value_t wav_object_filename_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

static ptp_prop_value_t wma_object_filename_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

static ptp_prop_value_t ogg_object_filename_allowed_values[] =
{
    { PTP_TYPE_STR, {0}, 0, 0, 0 }
};

mtp_obj_prop_desc_t jpg_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        jpg_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        2
    },
    {
        &prop_desc_map[url_reference],
        MTP_REGEXP_FORM,
        jpg_url_reference_allowed_values,
        1
    }
};

mtp_obj_prop_desc_t undefined_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        jpg_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[non_consumable],
        PTP_ENUM_FORM,
        non_consumable_allowed_values,
        2
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        2
    }
};

mtp_obj_prop_desc_t mp3_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        mp3_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[non_consumable],
        PTP_ENUM_FORM,
        non_consumable_allowed_values,
        2
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        4
    },
    {
        &prop_desc_map[track],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[audio_bitrate],
        PTP_RANGE_FORM, /* can be PTP_ENUM_FORM */
        audio_bitrate_allowed_values,
        3
    },
    {
        &prop_desc_map[artist],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[genre],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[sample_rate],
        PTP_RANGE_FORM,
        sample_rate_allowed_values,
        3
    },
    {
        &prop_desc_map[number_of_channels],
        PTP_ENUM_FORM,
        number_of_channels_allowed_values,
        4
    },
    {
        &prop_desc_map[album_name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[duration],
        PTP_RANGE_FORM,
        duration_allowed_values,
        3
    },
    {
        &prop_desc_map[audio_wave_codec],
        PTP_ENUM_FORM,
        audio_wav_codec_allowed_values,
        4
    },
    {
        &prop_desc_map[original_release_date],
        MTP_DATETIME_FORM,
        NULL,
        0
    }
};

mtp_obj_prop_desc_t wav_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        wav_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[non_consumable],
        PTP_ENUM_FORM,
        non_consumable_allowed_values,
        2
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        4
    },
    {
        &prop_desc_map[track],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[audio_bitrate],
        PTP_RANGE_FORM, /* can be PTP_ENUM_FORM */
        audio_bitrate_allowed_values,
        3
    },
    {
        &prop_desc_map[artist],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[genre],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[sample_rate],
        PTP_RANGE_FORM,
        sample_rate_allowed_values,
        3
    },
    {
        &prop_desc_map[number_of_channels],
        PTP_ENUM_FORM,
        number_of_channels_allowed_values,
        2
    },
    {
        &prop_desc_map[album_name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[duration],
        PTP_RANGE_FORM,
        duration_allowed_values,
        3
    },
    {
        &prop_desc_map[audio_wave_codec],
        PTP_ENUM_FORM,
        audio_wav_codec_allowed_values,
        4
    },
    {
        &prop_desc_map[original_release_date],
        MTP_DATETIME_FORM,
        NULL,
        0
    }
};

mtp_obj_prop_desc_t wma_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        wma_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[non_consumable],
        PTP_ENUM_FORM,
        non_consumable_allowed_values,
        2
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        4
    },
    {
        &prop_desc_map[track],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[audio_bitrate],
        PTP_RANGE_FORM, /* can be PTP_ENUM_FORM */
        audio_bitrate_allowed_values,
        3
    },
    {
        &prop_desc_map[artist],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[genre],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[sample_rate],
        PTP_RANGE_FORM,
        sample_rate_allowed_values,
        3
    },
    {
        &prop_desc_map[number_of_channels],
        PTP_ENUM_FORM,
        number_of_channels_allowed_values,
        2
    },
    {
        &prop_desc_map[album_name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[duration],
        PTP_RANGE_FORM,
        duration_allowed_values,
        3
    },
    {
        &prop_desc_map[audio_wave_codec],
        PTP_ENUM_FORM,
        audio_wav_codec_allowed_values,
        4
    },
    {
        &prop_desc_map[original_release_date],
        MTP_DATETIME_FORM,
        NULL,
        0
    }
};

mtp_obj_prop_desc_t ogg_obj_prop_desc_list[] = 
{
    {
        &prop_desc_map[effective_rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[rating],
        PTP_RANGE_FORM,
        rating_allowed_values,
        3
    },
    {
        &prop_desc_map[name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_format],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[storage_id],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_size],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[object_filename],
        MTP_REGEXP_FORM, /* can be PTP_NONE_FORM */
        ogg_object_filename_allowed_values,
        1
    },
    {
        &prop_desc_map[date_modified],
        MTP_DATETIME_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[non_consumable],
        PTP_ENUM_FORM,
        non_consumable_allowed_values,
        2
    },
    {
        &prop_desc_map[parent_object],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[persistent_unique_object_identifier],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[protection_status],
        PTP_ENUM_FORM,
        protection_status_allowed_values,
        4
    },
    {
        &prop_desc_map[track],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[audio_bitrate],
        PTP_RANGE_FORM, /* can be PTP_ENUM_FORM */
        audio_bitrate_allowed_values,
        3
    },
    {
        &prop_desc_map[artist],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[genre],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[sample_rate],
        PTP_RANGE_FORM,
        sample_rate_allowed_values,
        3
    },
    {
        &prop_desc_map[number_of_channels],
        PTP_ENUM_FORM,
        number_of_channels_allowed_values,
        2
    },
    {
        &prop_desc_map[album_name],
        PTP_NONE_FORM,
        NULL,
        0
    },
    {
        &prop_desc_map[duration],
        PTP_RANGE_FORM,
        duration_allowed_values,
        3
    },
    {
        &prop_desc_map[audio_wave_codec],
        PTP_ENUM_FORM,
        audio_wav_codec_allowed_values,
        4
    },
    {
        &prop_desc_map[original_release_date],
        MTP_DATETIME_FORM,
        NULL,
        0
    }
};

obj_format_prop_desc_t obj_format_prop_desc_map[] =
{
    { PTP_OBJ_FORMAT_IMG_JPEG , jpg_obj_prop_desc_list, 12 },
    { PTP_OBJ_FORMAT_UNDEFINED , undefined_obj_prop_desc_list, 12 },
    { PTP_OBJ_FORMAT_MP3      , mp3_obj_prop_desc_list, 22 },
    { PTP_OBJ_FORMAT_WAV      , wav_obj_prop_desc_list, 22 },
    { MTP_OBJ_FORMAT_WMA      , wma_obj_prop_desc_list, 22 },
    { MTP_OBJ_FORMAT_OGG      , ogg_obj_prop_desc_list, 22 },
    { PTP_OBJ_FORMAT_LAST     , NULL                  , 0 }
};

void objprops_init_descs()
{
    juint_t i,j;
    obj_format_prop_desc_t *map = obj_format_prop_desc_map;

    for (i = 0 ; map[i].obj_format != PTP_OBJ_FORMAT_LAST ; i++)
    {
        for (j = 0; j < map[i].obj_prop_desc_count ; j++)
        {
            mtp_obj_prop_desc_t *desc = &map[i].obj_prop_desc[j];

            switch (desc->prop_desc->prop_code)
            {
            case MTP_OBJ_PROP_PROTECTION_STATUS:
                desc->allowed_values[0].data.uint16 = 0x0000;
                desc->allowed_values[1].data.uint16 = 0x0001;
                desc->allowed_values[2].data.uint16 = 0x8002;
                desc->allowed_values[3].data.uint16 = 0x8003;
                break;
            case MTP_OBJ_PROP_NON_CONSUMABLE:
                desc->allowed_values[0].data.uint8 = 0x00;
                desc->allowed_values[1].data.uint8 = 0x01;
                break;
            case MTP_OBJ_PROP_EFFECTIVE_RATING:
            case MTP_OBJ_PROP_RATING:
                desc->allowed_values[0].data.uint16 = 0;
                desc->allowed_values[1].data.uint16 = 100;
                desc->allowed_values[2].data.uint16 = 1;
                break;
            case MTP_OBJ_PROP_AUDIO_BITRATE:
                desc->allowed_values[0].data.uint32 = 5000;
                desc->allowed_values[1].data.uint32 = 320000;
                desc->allowed_values[2].data.uint32 = 1;
                break;
            case MTP_OBJ_PROP_SAMPLE_RATE:
                desc->allowed_values[0].data.uint32 = 8000;
                desc->allowed_values[1].data.uint32 = 48000;
                desc->allowed_values[2].data.uint32 = 50;
                break;
            case MTP_OBJ_PROP_NUMBER_OF_CHANNELS:
                desc->allowed_values[0].data.uint16 = 0x0001;
                desc->allowed_values[1].data.uint16 = 0x0002;

                if (map[i].obj_format != PTP_OBJ_FORMAT_MP3)
                {
                    desc->allowed_values[2].data.uint16 = 0x0003;
                    desc->allowed_values[3].data.uint16 = 0x0004;
                }
                break;
            case MTP_OBJ_PROP_DURATION:
                desc->allowed_values[0].data.uint32 = 60;
                desc->allowed_values[1].data.uint32 = 6000;
                desc->allowed_values[2].data.uint32 = 1;
                break;
            case MTP_OBJ_PROP_AUDIO_WAVE_CODEC:
                desc->allowed_values[0].data.uint32 = 0;
                desc->allowed_values[1].data.uint32 = 1;
                desc->allowed_values[2].data.uint32 = 2;
                desc->allowed_values[3].data.uint32 = 85;       /* MP3 */
                break;
            case MTP_OBJ_PROP_OBJECT_FILENAME:
                if (desc->form_flag == MTP_REGEXP_FORM &&
                    desc->allowed_values)
                {
                    desc->allowed_values[0].data_type = PTP_TYPE_STR;
                    desc->allowed_values[0].data.string = 
                        ptp_string_atow(reg_exp_filename);
                    if (!desc->allowed_values[0].data.string)
                    {
                        DBG_E(DSLAVE_MTP_API, ("objprops_init_descs: failed "
                            "to allocate object filename regexp\n"));
                    }
                }
                break;
            case MTP_OBJ_PROP_URL_REFERENCE:
                if (desc->form_flag == MTP_REGEXP_FORM &&
                    desc->allowed_values)
                {
                    desc->allowed_values[0].data_type = PTP_TYPE_STR;
                    desc->allowed_values[0].data.string = 
                        ptp_string_atow(reg_exp_url_reference);
                    if (!desc->allowed_values[0].data.string)
                    {
                        DBG_E(DSLAVE_MTP_API, ("objprops_init_descs: failed "
                            "to allocate url reference regexp\n"));
                    }
                }
                break;
            default:
                /* nothing to do for now */
                break;
            }
        }
    }
}

void objprops_free_descs()
{
    juint_t i,j;
    obj_format_prop_desc_t *map = obj_format_prop_desc_map;

    for (i = 0 ; map[i].obj_format != PTP_OBJ_FORMAT_LAST ; i++)
    {
        for (j = 0; j < map[i].obj_prop_desc_count ; j++)
        {
            mtp_obj_prop_desc_t *desc = &map[i].obj_prop_desc[j];

            switch (desc->prop_desc->prop_code)
            {
            case MTP_OBJ_PROP_OBJECT_FILENAME:
            case MTP_OBJ_PROP_URL_REFERENCE:
                if (desc->form_flag == MTP_REGEXP_FORM &&
                    desc->allowed_values)
                {
                    ptp_prop_value_clear(&desc->allowed_values[0]);
                }
                break;
            default:
                /* nothing to do for now */
                break;
            }
        }
    }
}

jresult_t objprops_setup_obj(sicd_object_t *obj)
{
    juint_t i, j;
    obj_format_prop_desc_t *desc_map = obj_format_prop_desc_map;
    
    /* check if there are any relevant formats in the desc map */
    for (i = 0;
        desc_map[i].obj_format != PTP_OBJ_FORMAT_LAST &&
        desc_map[i].obj_format != obj->info.format;
        i++)
        ;

    /* if there aren't any, we're done */
    if (desc_map[i].obj_format == PTP_OBJ_FORMAT_LAST)
        return JENOENT;

    obj->obj_props_count = desc_map[i].obj_prop_desc_count;

    obj->obj_props = jmalloc(sizeof(obj_proper_t) * 
        desc_map[i].obj_prop_desc_count, M_ZERO);
    if (!obj->obj_props)
        goto Error;

    for (j = 0 ; j < desc_map[i].obj_prop_desc_count ; j++)
    {
        obj->obj_props[j].desc = &desc_map[i].obj_prop_desc[j];
        obj->obj_props[j].prop_code = obj->obj_props[j].desc->prop_desc->prop_code;
        obj->obj_props[j].current_value = NULL;
    }

    return 0;

Error:
    if (obj->obj_props)
    {
        jfree(obj->obj_props);
        obj->obj_props = NULL;
    }

    obj->obj_props_count = 0;
    
    return JENOMEM;
}

void objprops_free(sicd_object_t *obj)
{
    juint_t i;

    if (obj->obj_props)
    {
        for (i = 0 ; i < obj->obj_props_count ; i++)
        {
            if (obj->obj_props[i].current_value)
            {
                ptp_prop_value_clear(obj->obj_props[i].current_value);
                jfree(obj->obj_props[i].current_value);
            }
        }

        jfree(obj->obj_props);
    }
}

