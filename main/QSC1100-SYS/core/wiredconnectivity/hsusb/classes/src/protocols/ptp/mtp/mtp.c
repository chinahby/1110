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


#include "mtp_types.h"
#include <ptp_utils.h>
#include <ptp.h>
#define CHECK_ADD_OR_EXIT(counter, count_func)  \
    do{                                         \
        juint32_t c = (count_func);              \
        if (!c)                                 \
            goto Error;                         \
        (counter) += c;                         \
    }while(0)

struct 
{
    mtp_obj_prop_t      code;
    ptp_type_t          type;
} mtp_obj_prop_map[] = 
{
    { /* 1 */
        MTP_OBJ_PROP_STORAGE_ID, 
        PTP_TYPE_UINT32
    },
    { /* 2 */
        MTP_OBJ_PROP_OBJECT_FORMAT,
        PTP_TYPE_UINT16
    },
    { /* 3 */
        MTP_OBJ_PROP_PROTECTION_STATUS,
        PTP_TYPE_UINT16
    },
    { /* 4 */
        MTP_OBJ_PROP_OBJECT_SIZE,
        PTP_TYPE_UINT64
    },
    { /* 5 */
        MTP_OBJ_PROP_ASSOCIATION_TYPE,
        PTP_TYPE_UINT16
    },
    { /* 6 */
        MTP_OBJ_PROP_ASSOCIATION_DESC,
        PTP_TYPE_UINT32
    },
    { /* 7 */
        MTP_OBJ_PROP_OBJECT_FILENAME,
        PTP_TYPE_STR
    },
    { /* 8 */
        MTP_OBJ_PROP_DATE_CREATED,
        PTP_TYPE_STR
    },
    { /* 9 */
        MTP_OBJ_PROP_DATE_MODIFIED,
        PTP_TYPE_STR
    },
    { /* 10 */
        MTP_OBJ_PROP_KEYWORDS,
        PTP_TYPE_STR
    },
    { /* 11 */
        MTP_OBJ_PROP_PARENT_OBJECT,
        PTP_TYPE_UINT32
    },
    { /* 12 */
        MTP_OBJ_PROP_ALLOWED_FOLDER_CONTENTS,
        PTP_TYPE_AUINT16
    },
    { /* 13 */
        MTP_OBJ_PROP_HIDDEN,
        PTP_TYPE_UINT16
    },
    { /* 14 */
        MTP_OBJ_PROP_SYSTEM_OBJECT,
        PTP_TYPE_UINT16
    },
    { /* 15 */
        MTP_OBJ_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        PTP_TYPE_UINT128
    },
    { /* 16 */
        MTP_OBJ_PROP_SYNC_ID,
        PTP_TYPE_STR
    },
    { /* 17 */
        MTP_OBJ_PROP_PROPERTY_BAG,
        PTP_TYPE_AUINT16
    },
    { /* 18 */
        MTP_OBJ_PROP_NAME,
        PTP_TYPE_STR
    },
    { /* 19 */
        MTP_OBJ_PROP_CREATED_BY,
        PTP_TYPE_STR
    },
    { /* 20 */
        MTP_OBJ_PROP_ARTIST,
        PTP_TYPE_STR
    },
    { /* 21 */
        MTP_OBJ_PROP_DATE_AUTHORED,
        PTP_TYPE_STR
    },
    { /* 22 */
        MTP_OBJ_PROP_DESCRIPTION,
        PTP_TYPE_AUINT16
    },
    { /* 23 */
        MTP_OBJ_PROP_URL_REFERENCE,
        PTP_TYPE_STR
    },
    { /* 24 */
        MTP_OBJ_PROP_LANGUAGE_LOCALE,
        PTP_TYPE_STR
    },
    { /* 25 */
        MTP_OBJ_PROP_COPYRIGHT_INFORMATION,
        PTP_TYPE_AUINT16
    },
    { /* 26 */
        MTP_OBJ_PROP_SOURCE,
        PTP_TYPE_STR
    },
    { /* 27 */
        MTP_OBJ_PROP_ORIGIN_LOCATION,
        PTP_TYPE_STR
    },
    { /* 28 */
        MTP_OBJ_PROP_DATE_ADDED,
        PTP_TYPE_STR
    },
    { /* 29 */
        MTP_OBJ_PROP_NON_CONSUMABLE,
        PTP_TYPE_UINT8
    },
    { /* 30 */
        MTP_OBJ_PROP_CORRUPT_UNPLAYABLE,
        PTP_TYPE_UINT8
    },
    { /* 31 */
        MTP_OBJ_PROP_PRODUCER_SERIAL_NUMBER,
        PTP_TYPE_STR
    },
    { /* 32 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_FORMAT,
        PTP_TYPE_UINT16
    },
    { /* 33 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_SIZE,
        PTP_TYPE_UINT32
    },
    { /* 34 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_HEIGHT,
        PTP_TYPE_UINT32
    },
    { /* 35 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_WIDTH,
        PTP_TYPE_UINT32
    },
    { /* 36 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DURATION,
        PTP_TYPE_UINT32
    },
    { /* 37 */
        MTP_OBJ_PROP_REPRESENTATIVE_SAMPLE_DATA,
        PTP_TYPE_AUINT8
    },
    { /* 38 */
        MTP_OBJ_PROP_WIDTH,
        PTP_TYPE_UINT32
    },
    { /* 39 */
        MTP_OBJ_PROP_HEIGHT,
        PTP_TYPE_UINT32
    },
    { /* 40 */
        MTP_OBJ_PROP_DURATION,
        PTP_TYPE_UINT32
    },
    { /* 41 */
        MTP_OBJ_PROP_RATING,
        PTP_TYPE_UINT16
    },
    { /* 42 */
        MTP_OBJ_PROP_TRACK,
        PTP_TYPE_UINT16
    },
    { /* 43 */
        MTP_OBJ_PROP_GENRE,
        PTP_TYPE_STR
    },
    { /* 44 */
        MTP_OBJ_PROP_CREDITS,
        PTP_TYPE_AUINT16
    },
    { /* 45 */
        MTP_OBJ_PROP_LYRICS,
        PTP_TYPE_AUINT16
    },
    { /* 46 */
        MTP_OBJ_PROP_SUBSCRIPTION_CONTENT_ID,
        PTP_TYPE_STR
    },
    { /* 47 */
        MTP_OBJ_PROP_PRODUCED_BY,
        PTP_TYPE_STR
    },
    { /* 48 */
        MTP_OBJ_PROP_USE_COUNT,
        PTP_TYPE_UINT32
    },
    { /* 49 */
        MTP_OBJ_PROP_SKIP_COUNT,
        PTP_TYPE_UINT32
    },
    { /* 50 */
        MTP_OBJ_PROP_LAST_ACCESSED,
        PTP_TYPE_STR
    },
    { /* 51 */
        MTP_OBJ_PROP_PARENTAL_RATING,
        PTP_TYPE_STR
    },
    { /* 52 */
        MTP_OBJ_PROP_META_GENRE,
        PTP_TYPE_UINT16
    },
    { /* 53 */
        MTP_OBJ_PROP_COMPOSER,
        PTP_TYPE_STR
    },
    { /* 54 */
        MTP_OBJ_PROP_EFFECTIVE_RATING,
        PTP_TYPE_UINT16
    },
    { /* 55 */
        MTP_OBJ_PROP_SUBTITLE,
        PTP_TYPE_STR
    },
    { /* 56 */
        MTP_OBJ_PROP_ORIGINAL_RELEASE_DATE,
        PTP_TYPE_STR
    },
    { /* 57 */
        MTP_OBJ_PROP_ALBUM_NAME,
        PTP_TYPE_STR
    },
    { /* 58 */
        MTP_OBJ_PROP_ALBUM_ARTIST,
        PTP_TYPE_STR
    },
    { /* 59 */
        MTP_OBJ_PROP_MOOD,
        PTP_TYPE_STR
    },
    { /* 60 */
        MTP_OBJ_PROP_DRM_STATUS,
        PTP_TYPE_UINT16
    },
    { /* 61 */
        MTP_OBJ_PROP_SUB_DESCRIPTION,
        PTP_TYPE_AUINT16
    },
    { /* 62 */
        MTP_OBJ_PROP_IS_CROPPED,
        PTP_TYPE_UINT16
    },
    { /* 63 */
        MTP_OBJ_PROP_IS_COLOUR_CORRECTED,
        PTP_TYPE_UINT16
    },
    { /* 64 */
        MTP_OBJ_PROP_IMAGE_BIT_DEPTH,
        PTP_TYPE_UINT32
    },
    { /* 65 */
        MTP_OBJ_PROP_FNUMBER,
        PTP_TYPE_UINT16
    },
    { /* 66 */
        MTP_OBJ_PROP_EXPOSURE_TIME,
        PTP_TYPE_UINT32
    },
    { /* 67 */
        MTP_OBJ_PROP_EXPOSURE_INDEX,
        PTP_TYPE_UINT16
    },
    { /* 68 */
        MTP_OBJ_PROP_TOTAL_BITRATE,
        PTP_TYPE_UINT32
    },
    { /* 69 */
        MTP_OBJ_PROP_BITRATE_TYPE,
        PTP_TYPE_UINT16
    },
    { /* 70 */
        MTP_OBJ_PROP_SAMPLE_RATE,
        PTP_TYPE_UINT32
    },
    { /* 71 */
        MTP_OBJ_PROP_NUMBER_OF_CHANNELS,
        PTP_TYPE_UINT16
    },
    { /* 72 */
        MTP_OBJ_PROP_AUDIO_BIT_DEPTH,
        PTP_TYPE_UINT32
    },
    { /* 73 */
        MTP_OBJ_PROP_SCAN_TYPE,
        PTP_TYPE_UINT16
    },
    { /* 74 */
        MTP_OBJ_PROP_AUDIO_WAVE_CODEC,
        PTP_TYPE_UINT32
    },
    { /* 75 */
        MTP_OBJ_PROP_AUDIO_BITRATE,
        PTP_TYPE_UINT32
    },
    { /* 76 */
        MTP_OBJ_PROP_VIDEO_FOURCC_CODEC,
        PTP_TYPE_UINT32
    },
    { /* 77 */
        MTP_OBJ_PROP_VIDEO_BITRATE,
        PTP_TYPE_UINT32
    },
    { /* 78 */
        MTP_OBJ_PROP_FRAMES_PER_THOUSAND_SECONDS,
        PTP_TYPE_UINT32
    },
    { /* 79 */
        MTP_OBJ_PROP_KEY_FRAME_DISTANCE,
        PTP_TYPE_UINT32
    },
    { /* 80 */
        MTP_OBJ_PROP_BUFFER_SIZE,
        PTP_TYPE_UINT32
    },
    { /* 81 */
        MTP_OBJ_PROP_ENCODING_QUALITY,
        PTP_TYPE_UINT32
    },
    { /* 82 */
        MTP_OBJ_PROP_ENCODING_PROFILE,
        PTP_TYPE_STR
    },
    { /* 83 */
        MTP_OBJ_PROP_DISPLAY_NAME,
        PTP_TYPE_STR
    },
    { /* 84 */
        MTP_OBJ_PROP_BODY_TEXT,
        PTP_TYPE_AUINT16
    },
    { /* 85 */
        MTP_OBJ_PROP_SUBJECT,
        PTP_TYPE_STR
    },
    { /* 86 */
        MTP_OBJ_PROP_PRIORITY,
        PTP_TYPE_STR
    },
    { /* 87 */
        MTP_OBJ_PROP_GIVEN_NAME,
        PTP_TYPE_STR
    },
    { /* 88 */
        MTP_OBJ_PROP_MIDDLE_NAMES,
        PTP_TYPE_STR
    },
    { /* 89 */
        MTP_OBJ_PROP_FAMILY_NAME,
        PTP_TYPE_STR
    },
    { /* 90 */
        MTP_OBJ_PROP_PREFIX,
        PTP_TYPE_STR
    },
    { /* 91 */
        MTP_OBJ_PROP_SUFFIX,
        PTP_TYPE_STR
    },
    { /* 92 */
        MTP_OBJ_PROP_PHONETIC_GIVEN_NAME,
        PTP_TYPE_STR
    },
    { /* 93 */
        MTP_OBJ_PROP_PHONETIC_FAMILY_NAME,
        PTP_TYPE_STR
    },
    { /* 94 */
        MTP_OBJ_PROP_EMAIL_PRIMARY,
        PTP_TYPE_STR
    },
    { /* 95 */
        MTP_OBJ_PROP_EMAIL_PERSONAL_1,
        PTP_TYPE_STR
    },
    { /* 96 */
        MTP_OBJ_PROP_EMAIL_PERSONAL_2,
        PTP_TYPE_STR
    },
    { /* 97 */
        MTP_OBJ_PROP_EMAIL_BUSINESS_1,
        PTP_TYPE_STR
    },
    { /* 98 */
        MTP_OBJ_PROP_EMAIL_BUSINESS_2,
        PTP_TYPE_STR
    },
    { /* 99 */
        MTP_OBJ_PROP_EMAIL_OTHERS,
        PTP_TYPE_AUINT16
    },
    { /* 100 */
        MTP_OBJ_PROP_PHONE_NUMBER_PRIMARY,
        PTP_TYPE_STR
    },
    { /* 101 */
        MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL,
        PTP_TYPE_STR
    },
    { /* 102 */
        MTP_OBJ_PROP_PHONE_NUMBER_PERSONAL_2,
        PTP_TYPE_STR
    },
    { /* 103 */
        MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS,
        PTP_TYPE_STR
    },
    { /* 104 */
        MTP_OBJ_PROP_PHONE_NUMBER_BUSINESS_2,
        PTP_TYPE_STR
    },
    { /* 105 */
        MTP_OBJ_PROP_PHONE_NUMBER_MOBILE,
        PTP_TYPE_STR
    },
    { /* 106 */
        MTP_OBJ_PROP_PHONE_NUMBER_MOBILE_2,
        PTP_TYPE_STR
    },
    { /* 107 */
        MTP_OBJ_PROP_FAX_NUMBER_PRIMARY,
        PTP_TYPE_STR
    },
    { /* 108 */
        MTP_OBJ_PROP_FAX_NUMBER_PERSONAL,
        PTP_TYPE_STR
    },
    { /* 109 */
        MTP_OBJ_PROP_FAX_NUMBER_BUSINESS,
        PTP_TYPE_STR
    },
    { /* 110 */
        MTP_OBJ_PROP_PAGER_NUMBER,
        PTP_TYPE_STR
    },
    { /* 111 */
        MTP_OBJ_PROP_PHONE_NUMBER_OTHERS,
        PTP_TYPE_AUINT16
    },
    { /* 112 */
        MTP_OBJ_PROP_PRIMARY_WEB_ADDRESS,
        PTP_TYPE_STR
    },
    { /* 113 */
        MTP_OBJ_PROP_PERSONAL_WEB_ADDRESS,
        PTP_TYPE_STR
    },
    { /* 114 */
        MTP_OBJ_PROP_BUSINESS_WEB_ADDRESS,
        PTP_TYPE_STR
    },
    { /* 115 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS,
        PTP_TYPE_STR
    },
    { /* 116 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_2,
        PTP_TYPE_STR
    },
    { /* 117 */
        MTP_OBJ_PROP_INSTANT_MESSENGER_ADDRESS_3,
        PTP_TYPE_STR
    },
    { /* 118 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_FULL,
        PTP_TYPE_AUINT16
    },
    { /* 119 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_1,
        PTP_TYPE_STR
    },
    { /* 120 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_LINE_2,
        PTP_TYPE_STR
    },
    { /* 121 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_CITY,
        PTP_TYPE_STR
    },
    { /* 122 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_REGION,
        PTP_TYPE_STR
    },
    { /* 123 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE,
        PTP_TYPE_STR
    },
    { /* 124 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_PERSONAL_COUNTRY,
        PTP_TYPE_STR
    },
    { /* 125 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_FULL,
        PTP_TYPE_AUINT16
    },
    { /* 126 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_1,
        PTP_TYPE_STR
    },
    { /* 127 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_LINE_2,
        PTP_TYPE_STR
    },
    { /* 128 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_CITY,
        PTP_TYPE_STR
    },
    { /* 129 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_REGION,
        PTP_TYPE_STR
    },
    { /* 130 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE,
        PTP_TYPE_STR
    },
    { /* 131 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_BUSINESS_COUNTRY,
        PTP_TYPE_STR
    },
    { /* 132 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_FULL,
        PTP_TYPE_AUINT16
    },
    { /* 133 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_1,
        PTP_TYPE_STR
    },
    { /* 134 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_LINE_2,
        PTP_TYPE_STR
    },
    { /* 135 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_CITY,
        PTP_TYPE_STR
    },
    { /* 136 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_REGION,
        PTP_TYPE_STR
    },
    { /* 137 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_POSTAL_CODE,
        PTP_TYPE_STR
    },
    { /* 138 */
        MTP_OBJ_PROP_POSTAL_ADDRESS_OTHER_COUNTRY,
        PTP_TYPE_STR
    },
    { /* 139 */
        MTP_OBJ_PROP_ORGANIZATION_NAME,
        PTP_TYPE_STR
    },
    { /* 140 */
        MTP_OBJ_PROP_PHONETIC_ORGANIZATION_NAME,
        PTP_TYPE_STR
    },
    { /* 141 */
        MTP_OBJ_PROP_ROLE,
        PTP_TYPE_STR
    },
    { /* 142 */
        MTP_OBJ_PROP_BIRTHDATE,
        PTP_TYPE_STR
    },
    { /* 143 */
        MTP_OBJ_PROP_MESSAGE_TO,
        PTP_TYPE_AUINT16
    },
    { /* 144 */
        MTP_OBJ_PROP_MESSAGE_CC,
        PTP_TYPE_AUINT16
    },
    { /* 145 */
        MTP_OBJ_PROP_MESSAGE_BCC,
        PTP_TYPE_AUINT16
    },
    { /* 146 */
        MTP_OBJ_PROP_MESSAGE_READ,
        PTP_TYPE_UINT16
    },
    { /* 147 */
        MTP_OBJ_PROP_MESSAGE_RECEIVED_TIME,
        PTP_TYPE_STR
    },
    { /* 148 */
        MTP_OBJ_PROP_MESSAGE_SENDER,
        PTP_TYPE_STR
    },
    { /* 149 */
        MTP_OBJ_PROP_ACTIVITY_BEGIN_TIME,
        PTP_TYPE_STR
    },
    { /* 150 */
        MTP_OBJ_PROP_ACTIVITY_END_TIME,
        PTP_TYPE_STR
    },
    { /* 151 */
        MTP_OBJ_PROP_ACTIVITY_LOCATION,
        PTP_TYPE_STR
    },
    { /* 152 */
        MTP_OBJ_PROP_ACTIVITY_REQUIRED_ATTENDEES,
        PTP_TYPE_AUINT16
    },
    { /* 153 */
        MTP_OBJ_PROP_ACTIVITY_OPTIONAL_ATTENDEES,
        PTP_TYPE_AUINT16
    },
    { /* 154 */
        MTP_OBJ_PROP_ACTIVITY_RESOURCES,
        PTP_TYPE_AUINT16
    },
    { /* 155 */
        MTP_OBJ_PROP_ACTIVITY_ACCEPTED,
        PTP_TYPE_AUINT16
    },
    { /* 156 */
        MTP_OBJ_PROP_ACTIVITY_TENTATIVE,
        PTP_TYPE_AUINT16
    },
    { /* 157 */
        MTP_OBJ_PROP_ACTIVITY_DECLINED,
        PTP_TYPE_AUINT16
    },
    { /* 158 */
        MTP_OBJ_PROP_ACTIVITY_REMINDER_TIME,
        PTP_TYPE_STR
    },
    { /* 159 */
        MTP_OBJ_PROP_ACTIVITY_OWNER,
        PTP_TYPE_STR
    },
    { /* 160 */
        MTP_OBJ_PROP_ACTIVITY_STATUS,
        PTP_TYPE_STR
    },
    { /* 161 */
        MTP_OBJ_PROP_OWNER,
        PTP_TYPE_STR
    },
    { /* 162 */
        MTP_OBJ_PROP_EDITOR,
        PTP_TYPE_STR
    },
    { /* 163 */
        MTP_OBJ_PROP_WEBMASTER,
        PTP_TYPE_STR
    },
    { /* 164 */
        MTP_OBJ_PROP_URL_SOURCE,
        PTP_TYPE_STR
    },
    { /* 165 */
        MTP_OBJ_PROP_URL_DESTINATION,
        PTP_TYPE_STR
    },
    { /* 166 */
        MTP_OBJ_PROP_TIME_BOOKMARK,
        PTP_TYPE_UINT32
    },
    { /* 167 */
        MTP_OBJ_PROP_OBJECT_BOOKMARK,
        PTP_TYPE_UINT32
    },
    { /* 168 */
        MTP_OBJ_PROP_BYTE_BOOKMARK,
        PTP_TYPE_UINT64
    },
    { /* 169 */
        MTP_OBJ_PROP_LAST_BUILD_DATE,
        PTP_TYPE_STR
    },
    { /* 170 */
        MTP_OBJ_PROP_TIME_TO_LIVE,
        PTP_TYPE_UINT64
    },
    { /* 171 */
        MTP_OBJ_PROP_MEDIA_GUID,
        PTP_TYPE_STR
    },
    { /* 172 */
        MTP_OBJ_PROP_LAST,
        PTP_TYPE_UNDEF 
    }
};

ptp_type_t get_obj_prop_type(mtp_obj_prop_t prop_code)
{
    juint_t i;

    for (i = 0 ; mtp_obj_prop_map[i].code != prop_code && 
        mtp_obj_prop_map[i].code != MTP_OBJ_PROP_LAST; i++);

    return mtp_obj_prop_map[i].type;
}

juint32_t pack_obj_prop_desc(void **buf, mtp_obj_prop_desc_t *obj_prop)
{
    /* packing datasets assume there is no dataset bigger then the buffer
     * defined as PTP_MAX_BLOCK_SIZE (in ptp_transport_api.h) */
    int i = 0;
    juint32_t count = 10; /* constant fields size of the type */
    DECLARE_FNAME("pack_prop_desc_ds");

    pack_u16(buf, (juint16_t)obj_prop->prop_desc->prop_code);
    pack_u16(buf, (juint16_t)obj_prop->prop_desc->data_type);
    pack_u8(buf, (juint8_t)obj_prop->prop_desc->writable);
    CHECK_ADD_OR_EXIT(count,
        pack_prop_value(obj_prop->prop_desc->data_type, buf, 0xffffffff,
            &obj_prop->prop_desc->default_value));
    pack_u32(buf, (juint32_t)obj_prop->prop_desc->group_code);
    pack_u8(buf, (juint8_t)obj_prop->form_flag);

    switch (obj_prop->form_flag)
    {
    case PTP_NONE_FORM:
    case MTP_DATETIME_FORM:
        /* we dont have nothing to do here */
        break;
    case PTP_ENUM_FORM:
        pack_u16(buf, (juint16_t)obj_prop->num_allowed_values);
        count += sizeof(juint16_t);
    case PTP_RANGE_FORM:
        for (i = 0 ; i < obj_prop->num_allowed_values; i++)
        {
            CHECK_ADD_OR_EXIT(count, pack_prop_value(
                obj_prop->prop_desc->data_type, buf, 0xffffffff,
                &obj_prop->allowed_values[i]));
        }
        break;
    case MTP_REGEXP_FORM:
        count += pack_str(buf, PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN), 
            obj_prop->allowed_values[0].data.string);
        break;
    case MTP_FIXED_LEN_FORM:
        pack_u16(buf, (juint16_t)obj_prop->allowed_values->data.uint16);
        count += sizeof(juint16_t);
        break;
    case MTP_BYTE_ARR_FORM:
        KASSERT((obj_prop->prop_desc->data_type == PTP_TYPE_AUINT8),
            ("wrong type for long string form\n"));
        pack_u32(buf, (juint32_t)obj_prop->allowed_values->data.uint16);
        count += sizeof(juint32_t);
        break;
    case MTP_LONG_STR_FORM:
        KASSERT((obj_prop->prop_desc->data_type == PTP_TYPE_AUINT16),
            ("wrong type for long string form\n"));
        pack_u32(buf, (juint32_t)obj_prop->allowed_values->data.uint16);
        count += sizeof(juint32_t);
        break;
    default:
        DBG_E(DPROTO_MTP, ("%s: unsupported property descriptor "
            "form flag \n",fname));
        goto Error;
    }

    return count;

Error: /* exit in case of an error - 0 on return value represent an error */
    return 0;
}

#define BUF_SIZE_CALC(size, x)  ((size)/(x) + ((size)%(x) ? 1 : 0))
juint32_t calc_max_type(mtp_obj_prop_t prop_code, juint32_t buf_size/*in bytes*/)
{
    juint32_t count = 0;

    switch (get_obj_prop_type(prop_code))
    {
    case PTP_TYPE_AINT8:
    case PTP_TYPE_AUINT8:
    case PTP_TYPE_INT8:
    case PTP_TYPE_UINT8:
        count = buf_size;
        break;
    case PTP_TYPE_AINT16:
    case PTP_TYPE_AUINT16:
    case PTP_TYPE_INT16:
    case PTP_TYPE_UINT16:
        count = BUF_SIZE_CALC(buf_size, 2);
        break;
    case PTP_TYPE_AINT32:
    case PTP_TYPE_AUINT32:
    case PTP_TYPE_INT32:
    case PTP_TYPE_UINT32:
        count = BUF_SIZE_CALC(buf_size, 4);
        break;
    case PTP_TYPE_AINT64:
    case PTP_TYPE_AUINT64:
    case PTP_TYPE_INT64:
    case PTP_TYPE_UINT64:
        count = BUF_SIZE_CALC(buf_size, 8);
        break;
    case PTP_TYPE_AINT128:
    case PTP_TYPE_AUINT128:
    case PTP_TYPE_INT128:
    case PTP_TYPE_UINT128:
        count = BUF_SIZE_CALC(buf_size, 16);
        break;
    case PTP_TYPE_STR:
        count = 1; /* only 1 string */
        break;
    default: /* including PTP_TYPE_UNDEF */
        DBG_E(DPROTO_MTP, ("unsupported type \n"));
        count = 0;
        break;
    }

    return count;
}


