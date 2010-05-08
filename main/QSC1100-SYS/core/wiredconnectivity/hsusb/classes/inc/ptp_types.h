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

#ifndef __PTP_TYPES_H__
#define __PTP_TYPES_H__

#include <jtypes.h>
#include "ptp_vs_types.h"

/* TODO: this should be replaced with proper 64-bit and 128-bit types
 * handling in jos */
typedef struct
{
    juint32_t    high;
    juint32_t    low;
} ptp_uint64_t;

typedef struct
{
    ptp_uint64_t        high;
    ptp_uint64_t        low;
} ptp_uint128_t;

/* Define wide char (unicode) strings */
#define PTP_MAX_STRING_LEN      255
#ifndef wchar_t
typedef juint16_t wchar_t;
#endif

/* Define PTP handles */
#define PTP_OBJH_FOLDER_ANY     0x00000000
#define PTP_OBJH_FOLDER_ROOT    0xFFFFFFFF
#define PTP_OBJH_ALL            0xFFFFFFFF
typedef juint32_t ptp_obj_handle_t;

/* Define PTP storage IDs */
#define PTP_STORAGE_ID_ALL      0xFFFFFFFF
#define PTP_STORAGE_ID_ANY      0x00000000
typedef juint32_t ptp_storage_id_t;

#define PTP_PHY_STORAGE_ID(s_id) (((s_id) >> 16) & 0xFFFF)
#define PTP_LOG_STORAGE_ID(s_id) ((s_id) & 0xFFFF)
#define PTP_MAKE_STORAGE_ID(phy,log) (((phy) << 16) | (log))


/* Data Types */
typedef enum ptp_type
{
    PTP_TYPE_UNDEF      = 0x0000,       /* Undefined */
    PTP_TYPE_INT8       = 0x0001,       /* Signed 8 bit integer */
    PTP_TYPE_UINT8      = 0x0002,       /* Unsigned 8 bit integer */
    PTP_TYPE_INT16      = 0x0003,       /* Signed 16 bit integer */
    PTP_TYPE_UINT16     = 0x0004,       /* Unsigned 16 bit integer */
    PTP_TYPE_INT32      = 0x0005,       /* Signed 32 bit integer */
    PTP_TYPE_UINT32     = 0x0006,       /* Unsigned 32 bit integer */
    PTP_TYPE_INT64      = 0x0007,       /* Signed 64 bit integer */
    PTP_TYPE_UINT64     = 0x0008,       /* Unsigned 64 bit integer */
    PTP_TYPE_INT128     = 0x0009,       /* Signed 128 bit integer */
    PTP_TYPE_UINT128    = 0x000A,       /* Unsigned 128 bit integer */
    PTP_TYPE_AINT8      = 0x4001,       /* Array of Signed 8 bit integers */
    PTP_TYPE_AUINT8     = 0x4002,       /* Array of Unsigned 8 bit integers */
    PTP_TYPE_AINT16     = 0x4003,       /* Array of Signed 16 bit integers */
    PTP_TYPE_AUINT16    = 0x4004,       /* Array of Unsigned 16 bit integers */
    PTP_TYPE_AINT32     = 0x4005,       /* Array of Signed 32 bit integers */
    PTP_TYPE_AUINT32    = 0x4006,       /* Array of Unsigned 32 bit integers */
    PTP_TYPE_AINT64     = 0x4007,       /* Array of Signed 64 bit integers */
    PTP_TYPE_AUINT64    = 0x4008,       /* Array of Unsigned 64 bit integers */
    PTP_TYPE_AINT128    = 0x4009,       /* Array of Signed 128 bit integers */
    PTP_TYPE_AUINT128   = 0x400A,       /* Array of Unsigned 128 bit integers*/
    PTP_TYPE_STR        = 0xFFFF        /* Variable-length Unicode String */
} ptp_type_t;

/* Datacodes */

typedef enum ptp_datacode_type
{
    /* Standard datacodes */
    PTP_CODE_TYPE_UNKNOWN               = 0x0000,
    PTP_CODE_TYPE_OPERATION             = 0x1000,
    PTP_CODE_TYPE_RESPONSE              = 0x2000,
    PTP_CODE_TYPE_OBJECT_FORMAT         = 0x3000,
    PTP_CODE_TYPE_EVENT                 = 0x4000,
    PTP_CODE_TYPE_DEVICE_PROP           = 0x5000,
    /* Vendor specific datacodes */
    PTP_CODE_TYPE_VS_UNKNOWN            = 0x8000,
    PTP_CODE_TYPE_VS_OPERATION          = 0x9000,
    PTP_CODE_TYPE_VS_RESPONSE           = 0xA000,
    PTP_CODE_TYPE_VS_OBJECT_FORMAT      = 0xB000,
    PTP_CODE_TYPE_VS_EVENT              = 0xC000,
    PTP_CODE_TYPE_VS_DEVICE_PROP        = 0xD000
} ptp_datacode_type_t;

typedef enum ptp_operation
{
    PTP_OP_UNDEFINED                    = 0x1000,
    PTP_OP_GET_DEVICE_INFO              = 0x1001,
    PTP_OP_OPEN_SESSION                 = 0x1002,
    PTP_OP_CLOSE_SESSION                = 0x1003,
    PTP_OP_GET_STORAGE_IDS              = 0x1004,
    PTP_OP_GET_STORAGE_INFO             = 0x1005,
    PTP_OP_GET_NUM_OBJECTS              = 0x1006,
    PTP_OP_GET_OBJECT_HANDLES           = 0x1007,
    PTP_OP_GET_OBJECT_INFO              = 0x1008,
    PTP_OP_GET_OBJECT                   = 0x1009,
    PTP_OP_GET_THUMB                    = 0x100A,
    PTP_OP_DELETE_OBJECT                = 0x100B,
    PTP_OP_SEND_OBJECT_INFO             = 0x100C,
    PTP_OP_SEND_OBJECT                  = 0x100D,
    PTP_OP_INITIATE_CAPTURE             = 0x100E,
    PTP_OP_FORMAT_STORE                 = 0x100F,
    PTP_OP_RESET_DEVICE                 = 0x1010,
    PTP_OP_SELF_TEST                    = 0x1011,
    PTP_OP_SET_OBJECT_PROTECTION        = 0x1012,
    PTP_OP_POWER_DOWN                   = 0x1013,
    PTP_OP_GET_DEVICE_PROP_DESC         = 0x1014,
    PTP_OP_GET_DEVICE_PROP_VALUE        = 0x1015,
    PTP_OP_SET_DEVICE_PROP_VALUE        = 0x1016,
    PTP_OP_RESET_DEVICE_PROP_VALUE      = 0x1017,
    PTP_OP_TERMINATE_OPEN_CAPTURE       = 0x1018,
    PTP_OP_MOVE_OBJECT                  = 0x1019,
    PTP_OP_COPY_OBJECT                  = 0x101A,
    PTP_OP_GET_PARTIAL_OBJECT           = 0x101B,
    PTP_OP_INITIATE_OPEN_CAPTURE        = 0x101C,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_OP_GROUP,
    
    PTP_OP_LAST
} ptp_operation_t;

typedef enum ptp_response
{
    PTP_RESP_UNDEFINED                  = 0x2000,
    PTP_RESP_OK                         = 0x2001,
    PTP_RESP_GENERAL_ERROR              = 0x2002,
    PTP_RESP_SESSION_NOT_OPEN           = 0x2003,
    PTP_RESP_INVALID_TRANSACTION_ID     = 0x2004,
    PTP_RESP_OPERATION_NOT_SUPPORTED    = 0x2005,
    PTP_RESP_PARAM_NOT_SUPPORTED        = 0x2006,
    PTP_RESP_INCOMPLETE_TRANSFER        = 0x2007,
    PTP_RESP_INVALID_STORAGE_ID         = 0x2008,
    PTP_RESP_INVALID_OBJECT_HANDLE      = 0x2009,
    PTP_RESP_DEVICE_PROP_NOT_SUPPORTED  = 0x200A,
    PTP_RESP_INVALID_OBJECT_FORMAT_CODE = 0x200B,
    PTP_RESP_STORE_FULL                 = 0x200C,
    PTP_RESP_OBJECT_WRITE_PROTECTED     = 0x200D,
    PTP_RESP_STORE_READONLY             = 0x200E,
    PTP_RESP_ACCESS_DENIED              = 0x200F,
    PTP_RESP_NO_THUMBNAIL               = 0x2010,
    PTP_RESP_SELFTEST_FAILED            = 0x2011,
    PTP_RESP_PARTIAL_DELETION           = 0x2012,
    PTP_RESP_STORE_UNAVAILABLE          = 0x2013,
    PTP_RESP_SPEC_BY_FORMAT_UNSUPPORTED = 0x2014,
    PTP_RESP_NO_VALID_OBJECT_INFO       = 0x2015,
    PTP_RESP_INVALID_CODE_FORMAT        = 0x2016,
    PTP_RESP_UNKNOWN_VENDOR_CODE        = 0x2017,
    PTP_RESP_CAPTURE_ALREADY_TERMINATED = 0x2018,
    PTP_RESP_DEVICE_BUSY                = 0x2019,
    PTP_RESP_INVALID_PARENT_OBJECT      = 0x201A,
    PTP_RESP_INVALID_DEVICE_PROP_FORMAT = 0x201B,
    PTP_RESP_INVALID_DEVICE_PROP_VALUE  = 0x201C,
    PTP_RESP_INVALID_PARAM              = 0x201D,
    PTP_RESP_SESSION_ALREADY_OPEN       = 0x201E,
    PTP_RESP_TRANSACTION_CANCELLED      = 0x201F,
    PTP_RESP_SPEC_OF_DEST_UNSUPPORTED   = 0x2020,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_RESP_GROUP,
    
    PTP_RESP_LAST
} ptp_response_t;

#define PTP_OBJ_FORMAT_ALL      0x00000000
#define PTP_OBJ_FORMAT_IMAGES   0xFFFFFFFF
typedef enum ptp_object_format
{
    PTP_OBJ_FORMAT_UNDEFINED            = 0x3000,
    PTP_OBJ_FORMAT_ASSOCIATION          = 0x3001,
    PTP_OBJ_FORMAT_SCRIPT               = 0x3002,
    PTP_OBJ_FORMAT_BINARY               = 0x3003,
    PTP_OBJ_FORMAT_TEXT                 = 0x3004,
    PTP_OBJ_FORMAT_HTML                 = 0x3005,
    PTP_OBJ_FORMAT_DPOF                 = 0x3006,
    PTP_OBJ_FORMAT_AIFF                 = 0x3007,
    PTP_OBJ_FORMAT_WAV                  = 0x3008,
    PTP_OBJ_FORMAT_MP3                  = 0x3009,
    PTP_OBJ_FORMAT_AVI                  = 0x300A,
    PTP_OBJ_FORMAT_MPEG                 = 0x300B,
    PTP_OBJ_FORMAT_ASF                  = 0x300C,
    PTP_OBJ_FORMAT_QUICKTIME            = 0x300D,
    PTP_OBJ_FORMAT_XML                  = 0x300E,
    PTP_OBJ_FORMAT_IMG_UNKNOWN          = 0x3800,
    PTP_OBJ_FORMAT_IMG_JPEG             = 0x3801,
    PTP_OBJ_FORMAT_IMG_TIFF_EP          = 0x3802,
    PTP_OBJ_FORMAT_IMG_FLASHPIX         = 0x3803,
    PTP_OBJ_FORMAT_IMG_BMP              = 0x3804,
    PTP_OBJ_FORMAT_IMG_CIFF             = 0x3805,
    PTP_OBJ_FORMAT_IMG_RESERVED1        = 0x3806,
    PTP_OBJ_FORMAT_IMG_GIF              = 0x3807,
    PTP_OBJ_FORMAT_IMG_JFIF             = 0x3808,
    PTP_OBJ_FORMAT_IMG_PCD              = 0x3809,
    PTP_OBJ_FORMAT_IMG_PICT             = 0x380A,
    PTP_OBJ_FORMAT_IMG_PNG              = 0x380B,
    PTP_OBJ_FORMAT_IMG_RESERVED2        = 0x380C,
    PTP_OBJ_FORMAT_IMG_TIFF             = 0x380D,
    PTP_OBJ_FORMAT_IMG_TIFF_IT          = 0x380E,
    PTP_OBJ_FORMAT_IMG_JP2              = 0x380F,
    PTP_OBJ_FORMAT_IMG_JPX              = 0x3810,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_OBJ_FORMAT_GROUP,
    
    PTP_OBJ_FORMAT_LAST
} ptp_obj_format_t;

typedef enum ptp_event
{
    PTP_EVENT_UNDEFINED                 = 0x4000,
    PTP_EVENT_CANCEL_TRANSACTION        = 0x4001,
    PTP_EVENT_OBJECT_ADDED              = 0x4002,
    PTP_EVENT_OBJECT_REMOVED            = 0x4003,
    PTP_EVENT_STORE_ADDED               = 0x4004,
    PTP_EVENT_STORE_REMOVED             = 0x4005,
    PTP_EVENT_DEVICE_PROP_CHANGED       = 0x4006,
    PTP_EVENT_OBJECT_INFO_CHANGED       = 0x4007,
    PTP_EVENT_DEVICE_INFO_CHANGED       = 0x4008,
    PTP_EVENT_REQUEST_OBJECT_TRANSFER   = 0x4009,
    PTP_EVENT_STORE_FULL                = 0x400A,
    PTP_EVENT_DEVICE_RESET              = 0x400B,
    PTP_EVENT_STORAGE_INFO_CHANGED      = 0x400C,
    PTP_EVENT_CAPTURE_COMPLETE          = 0x400D,
    PTP_EVENT_UNREPORTED_STATUS         = 0x400E,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_EVENT_GROUP,
    
    PTP_EVENT_LAST
} ptp_event_t;

typedef enum ptp_device_prop
{
    PTP_DEV_PROP_UNDEFINED                      = 0x5000,
    PTP_DEV_PROP_BATTERY_LEVEL                  = 0x5001,
    PTP_DEV_PROP_FUNCTIONAL_MODE                = 0x5002,
    PTP_DEV_PROP_IMAGE_SIZE                     = 0x5003,
    PTP_DEV_PROP_COMPRESSION_SETTING            = 0x5004,
    PTP_DEV_PROP_WHITE_BALANCE                  = 0x5005,
    PTP_DEV_PROP_RGB_GAIN                       = 0x5006,
    PTP_DEV_PROP_F_NUMBER                       = 0x5007,
    PTP_DEV_PROP_FOCAL_LENGTH                   = 0x5008,
    PTP_DEV_PROP_FOCUS_DISTANCE                 = 0x5009,
    PTP_DEV_PROP_FOCUS_MODE                     = 0x500A,
    PTP_DEV_PROP_EXPOSURE_METERING_MODE         = 0x500B,
    PTP_DEV_PROP_FLASH_MODE                     = 0x500C,
    PTP_DEV_PROP_EXPOSURE_TIME                  = 0x500D,
    PTP_DEV_PROP_EXPOSURE_PROGRAM_MODE          = 0x500E,
    PTP_DEV_PROP_EXPOSURE_INDEX                 = 0x500F,
    PTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION     = 0x5010,
    PTP_DEV_PROP_DATE_TIME                      = 0x5011,
    PTP_DEV_PROP_CAPTURE_DELAY                  = 0x5012,
    PTP_DEV_PROP_STILL_CAPTURE_MODE             = 0x5013,
    PTP_DEV_PROP_CONTRAST                       = 0x5014,
    PTP_DEV_PROP_SHARPNESS                      = 0x5015,
    PTP_DEV_PROP_DIGITAL_ZOOM                   = 0x5016,
    PTP_DEV_PROP_EFFECT_MODE                    = 0x5017,
    PTP_DEV_PROP_BURST_NUMBER                   = 0x5018,
    PTP_DEV_PROP_BURST_INTERVAL                 = 0x5019,
    PTP_DEV_PROP_TIMELAPSE_NUMBER               = 0x501A,
    PTP_DEV_PROP_TIMELAPSE_INTERVAL             = 0x501B,
    PTP_DEV_PROP_FOCUS_METERING_MODE            = 0x501C,
    PTP_DEV_PROP_UPLOAD_URL                     = 0x501D,
    PTP_DEV_PROP_ARTIST                         = 0x501E,
    PTP_DEV_PROP_COPYRIGHT_INFO                 = 0x501F,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_DEV_PROP_GROUP,
    
    PTP_DEV_PROP_LAST
} ptp_dev_prop_t;

typedef enum ptp_functional_mode
{
    PTP_FMODE_STANDARD  = 0x0000,
    PTP_FMODE_SLEEP     = 0x0001,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_FMODE_GROUP,
    
    PTP_FMODE_LAST
} ptp_functional_mode_t;

typedef enum ptp_protection_status
{
    PTP_PROTECT_NONE            = 0x0000,
    PTP_PROTECT_READONLY        = 0x0001,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_PROTECT_GROUP,
    
    PTP_PROTECT_LAST
} ptp_protection_status_t;

typedef enum ptp_association
{
    PTP_ASSOC_UNDEFINED         = 0x0000,
    PTP_ASSOC_FOLDER            = 0x0001,
    PTP_ASSOC_ALBUM             = 0x0002,
    PTP_ASSOC_TIME_SEQUENCE     = 0x0003,
    PTP_ASSOC_H_PANORAMIC       = 0x0004,
    PTP_ASSOC_V_PANORAMIC       = 0x0005,
    PTP_ASSOC_2D_PANORAMIC      = 0x0006,
    PTP_ASSOC_ANCILLARY_DATA    = 0x0007,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_ASSOC_GROUP,
    
    PTP_ASSOC_LAST
} ptp_association_t;

typedef enum ptp_storage_type
{
    PTP_STOR_TYPE_UNDEFINED     = 0x0000,
    PTP_STOR_TYPE_FIXED_ROM     = 0x0001,
    PTP_STOR_TYPE_REMOVABLE_ROM = 0x0002,
    PTP_STOR_TYPE_FIXED_RAM     = 0x0003,
    PTP_STOR_TYPE_REMOVABLE_RAM = 0x0004,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_STOR_GROUP,
    
    PTP_STOR_TYPE_LAST
} ptp_storage_type_t;

typedef enum ptp_filesystem_type
{
    PTP_FS_UNDEFINED            = 0x0000,
    PTP_FS_GENERIC_FLAT         = 0x0001,
    PTP_FS_GENERIC_HIERARCHICAL = 0x0002,
    PTP_FS_DCF                  = 0x0003,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_FS_GROUP,
    
    PTP_FS_LAST
} ptp_filesystem_type_t;

typedef enum ptp_access_capability
{
    PTP_ACCESS_READ_WRITE       = 0x0000,
    PTP_ACCESS_READ_ONLY        = 0x0001,
    PTP_ACCESS_READ_DELETE      = 0x0002,
    
    /* Maps to a group of vendor specific extensions */
    PTP_VS_EXT_ACCESS_GROUP,
    
    PTP_ACCESS_LAST
} ptp_access_capability_t;

/* Datasets */

typedef struct _ptp_device_info
{
    ptp_functional_mode_t       func_mode;
    ptp_operation_t             *supported_ops;
    ptp_event_t                 *supported_events;
    ptp_dev_prop_t              *supported_props;
    ptp_obj_format_t            *capture_formats;
    ptp_obj_format_t            *image_formats;
    wchar_t                     *manufacturer;
    wchar_t                     *model;
    wchar_t                     *dev_ver;
    wchar_t                     *serial_num;
} ptp_dev_info_t;

typedef struct _ptp_object_info
{
    ptp_storage_id_t            storage_id;
    ptp_protection_status_t     prot_status;
    ptp_obj_format_t            format;
    juint32_t                    compressed_size;
    juint32_t                    width;             /* In pixels */
    juint32_t                    height;            /* In pixels */
    juint32_t                    bit_depth;
    ptp_obj_format_t            thumb_format;
    juint32_t                    thumb_compressed_size;
    juint32_t                    thumb_width;       /* In pixels */
    juint32_t                    thumb_height;      /* In pixels */
    ptp_obj_handle_t            parent_object;
    ptp_association_t           association_type;
    juint32_t                    association_desc;
    juint32_t                    sequence_number;
    wchar_t                     *filename;
    wchar_t                     *capture_date;
    wchar_t                     *modification_date;
    wchar_t                     *keywords;
} ptp_obj_info_t;

typedef struct _ptp_storage_info
{
    ptp_storage_type_t          type;
    ptp_filesystem_type_t       fs_type;
    ptp_access_capability_t     access;
    ptp_uint64_t                max_capacity;   /* In bytes */
    ptp_uint64_t                free_space;     /* In bytes */
    juint32_t                    free_images;    /* In images */
    wchar_t                     *desc;
    wchar_t                     *label;    
} ptp_storage_info_t;

typedef struct
{
    ptp_type_t          data_type;

    union 
    {
        /* fields representing single values */
        juint8_t         uint8;
        juint16_t        uint16;
        juint32_t        uint32;
        ptp_uint64_t    uint64;
        ptp_uint128_t   uint128;
        wchar_t         *string;
        
        /* fields representing arrays of values */
        juint8_t         *a_uint8;
        juint16_t        *a_uint16;
        juint32_t        *a_uint32;
        ptp_uint64_t    *a_uint64;
        ptp_uint128_t   *a_uint128;
    } data;

    /* the fields below are valid only for arrays */
    juint32_t     total_elems;
    juint32_t     num_elems;
    juint32_t     elem_offset;
} ptp_prop_value_t;

typedef struct _ptp_device_prop_desc
{
    ptp_dev_prop_t              prop_code;
    ptp_type_t                  data_type;
    jbool_t                      writable;
    ptp_prop_value_t            *default_value;
    ptp_prop_value_t            *current_value;
#define PTP_NONE_FORM           0x00
#define PTP_RANGE_FORM          0x01
#define PTP_ENUM_FORM           0x02
    juint8_t                     form_flag;
    juint16_t                    num_allowed_values;
    ptp_prop_value_t            *allowed_values;
} ptp_dev_prop_desc_t;


/*
 * Non-PTP, implementation specific codes for general I/O completion callbacks
 */
typedef enum ptp_io_status
{
    PTP_IO_STATUS_OK,
    PTP_IO_STATUS_ERROR,
    PTP_IO_STATUS_CANCELLED
} ptp_io_status_t;

#endif /*__PTP_TYPES_H__*/
