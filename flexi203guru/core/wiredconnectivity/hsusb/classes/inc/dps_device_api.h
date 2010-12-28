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
#ifndef _DPS_API_H_
#define _DPS_API_H_

#include <jtypes.h>
#include <ptp_responder_api.h>

typedef juint32_t dps_param_t;   /* Use the DPS_PARAM macros for these values */


/* Result Codes */
#define DPS_RESULT_OK                   0x10000000L
#define DPS_RESULT_UNEXECUTED_OP        0x10010000L
#define DPS_RESULT_UNSUPPORTED_OP       0x10020000L
#define DPS_RESULT_UNKNOWN_OP           0x10030000L

/* Minor result codes (for DPS_RESULT_UNSUPPORTED_OP only!) */
#define DPS_RESULT_MIN_NONE             0x0000L
#define DPS_RESULT_MIN_UNKNOWN_PARAM    0x0001L
#define DPS_RESULT_MIN_INVALID_VALUE    0x0002L
#define DPS_RESULT_MIN_MISSING_PARAM    0x0003L
#define DPS_RESULT_MIN_OVERFLOW         0x0004L

/* 0x8000-0xFFFF reserved for vendor specific minor codes */

/* MACROS for parsing and creating DPS parameter and result codes */
#define DPS_PARAM_MAJOR(res)         ((res) & 0xFFFF0000L)
#define DPS_PARAM_MINOR(res)         ((res) & 0x0000FFFFL)
#define DPS_PARAM_MAKE(major, minor) ((major) | (minor))

typedef juint32_t dps_reason_t;

/* Reason codes */

/* 0x80-0xFF reserved for vendor specific minor codes */
/* 0x80-0xFF reserved for vendor specific detail codes */

/* MACROS for parsing and creating error reasons */
#define DPS_REASON_MAJOR(res)  ((res) & 0xFFFF0000L)
#define DPS_REASON_MINOR(res)  ((res) & 0x0000FF00L)
#define DPS_REASON_DETAIL(res) ((res) & 0x000000FFL)
#define DPS_REASON_MAKE(major, minor, detail) ((major) | (minor) | (detail))

typedef void *dps_handle_t;
typedef void *dps_appctx_t;

#define DPS_MAX_VERSIONS 10
#define DPS_MAX_STRING_SIZE 64

typedef struct
{
    juint16_t major;
    juint16_t minor;
} dps_version_t;

typedef struct
{
    jint_t versions_count;
    dps_version_t dps_version[DPS_MAX_VERSIONS];
    char vendor_name[DPS_MAX_STRING_SIZE+1];
    dps_version_t vendor_specific_version;
    char product_name[DPS_MAX_STRING_SIZE+1];
    char serial_no[DPS_MAX_STRING_SIZE+1];    
} dps_req_conf_t;

typedef struct
{
    dps_param_t service_available;
    dps_req_conf_t conf;
} dps_res_conf_t;

/* Possible 'service_available' values */
#define DPS_SERVICE_UNAVAILABLE 0x30000000L
#define DPS_SERVICE_AVAILABLE 0x30010000L

typedef enum
{
    DPS_CAP_QUALITIES,
    DPS_CAP_PAPER_SIZES,
    DPS_CAP_PAPER_TYPES,        /* Optional paper_size parameter can be used */
    DPS_CAP_FILE_TYPES,
    DPS_CAP_DATE_PRINTS,
    DPS_CAP_FILENAME_PRINTS,
    DPS_CAP_IMAGE_OPTIMIZE,
    DPS_CAP_LAYOUTS,            /* Used with the paper_size parameter */
    DPS_CAP_FIXED_SIZES,
    DPS_CAP_CROPPINGS
} dps_capability_t;

/* Possible capabilities */

/* Qualities */
#define DPS_QUALITY_DEFAULT             0x50000000L
#define DPS_QUALITY_NORMAL              0x50010000L
#define DPS_QUALITY_DRAFT               0x50020000L
#define DPS_QUALITY_FINE                0x50030000L

/* Paper sizes */
#define DPS_PAPER_SIZE_DEFAULT          0x51000000L
#define DPS_PAPER_SIZE_L                0x51010000L
#define DPS_PAPER_SIZE_2L               0x51020000L
#define DPS_PAPER_SIZE_HAGAKI           0x51030000L
#define DPS_PAPER_SIZE_CARD             0x51040000L
#define DPS_PAPER_SIZE_100x150          0x51050000L
#define DPS_PAPER_SIZE_4x6              0x51060000L
#define DPS_PAPER_SIZE_8x10             0x51070000L
#define DPS_PAPER_SIZE_LETTER           0x51080000L
#define DPS_PAPER_SIZE_11x17            0x510A0000L
#define DPS_PAPER_SIZE_A0               0x51100000L
#define DPS_PAPER_SIZE_A1               0x51110000L
#define DPS_PAPER_SIZE_A2               0x51120000L
#define DPS_PAPER_SIZE_A3               0x51130000L
#define DPS_PAPER_SIZE_A4               0x51140000L
#define DPS_PAPER_SIZE_A5               0x51150000L
#define DPS_PAPER_SIZE_A6               0x51160000L
#define DPS_PAPER_SIZE_A7               0x51170000L
#define DPS_PAPER_SIZE_A8               0x51180000L
#define DPS_PAPER_SIZE_A9               0x51190000L
#define DPS_PAPER_SIZE_B0               0x51200000L
#define DPS_PAPER_SIZE_B1               0x51210000L
#define DPS_PAPER_SIZE_B2               0x51220000L
#define DPS_PAPER_SIZE_B3               0x51230000L
#define DPS_PAPER_SIZE_B4               0x51240000L
#define DPS_PAPER_SIZE_B5               0x51250000L
#define DPS_PAPER_SIZE_B6               0x51260000L
#define DPS_PAPER_SIZE_B7               0x51270000L
#define DPS_PAPER_SIZE_B8               0x51280000L
#define DPS_PAPER_SIZE_B9               0x51290000L
#define DPS_PAPER_SIZE_L_ROLL           0x51810000L
#define DPS_PAPER_SIZE_2L_ROLL          0x51820000L
#define DPS_PAPER_SIZE_100_ROLL         0x51860000L
#define DPS_PAPER_SIZE_A4_ROLL          0x51940000L

/* Paper types */
#define DPS_PAPER_TYPE_DEFAULT          0x52000000L
#define DPS_PAPER_TYPE_PLAIN            0x52010000L
#define DPS_PAPER_TYPE_PHOTO            0x52020000L
#define DPS_PAPER_TYPE_FAST_PHOTO       0x52030000L

/* Paper type minor codes */
#define DPS_PAPER_TYPE_MIN_STATIONERY           0x0001L
#define DPS_PAPER_TYPE_MIN_STAT_COATED          0x0002L
#define DPS_PAPER_TYPE_MIN_STAT_INKJET          0x0003L
#define DPS_PAPER_TYPE_MIN_STAT_PREPRINTED      0x0004L
#define DPS_PAPER_TYPE_MIN_STAT_LETTERHEAD      0x0005L
#define DPS_PAPER_TYPE_MIN_STAT_PREPUNCHED      0x0006L
#define DPS_PAPER_TYPE_MIN_STAT_FINE            0x0007L
#define DPS_PAPER_TYPE_MIN_STAT_HEAVY           0x0008L
#define DPS_PAPER_TYPE_MIN_STAT_LIGHT           0x0009L
#define DPS_PAPER_TYPE_MIN_TRANSPARENT          0x000AL
#define DPS_PAPER_TYPE_MIN_ENVELOPE             0x000BL
#define DPS_PAPER_TYPE_MIN_ENV_PLAIN            0x000CL
#define DPS_PAPER_TYPE_MIN_ENV_WINDOW           0x000DL
#define DPS_PAPER_TYPE_MIN_CONTINUOUS           0x000EL
#define DPS_PAPER_TYPE_MIN_CONT_LONG            0x000FL
#define DPS_PAPER_TYPE_MIN_CONT_SHORT           0x0010L
#define DPS_PAPER_TYPE_MIN_TAB_STOCK            0x0011L
#define DPS_PAPER_TYPE_MIN_TAB_PRECUT           0x0012L
#define DPS_PAPER_TYPE_MIN_TAB_FULLCUT          0x0013L
#define DPS_PAPER_TYPE_MIN_MULTIPART_FORM       0x0014L
#define DPS_PAPER_TYPE_MIN_LABELS               0x0015L
#define DPS_PAPER_TYPE_MIN_MULTI_LAYER          0x0016L
#define DPS_PAPER_TYPE_MIN_SCREEN               0x0017L
#define DPS_PAPER_TYPE_MIN_SCR_PAGED            0x0018L
#define DPS_PAPER_TYPE_MIN_PHOTOGRAPHIC         0x0019L
#define DPS_PAPER_TYPE_MIN_PHOTO_GLOSSY         0x001AL
#define DPS_PAPER_TYPE_MIN_PHOTO_HIGH_GLOSS     0x001BL
#define DPS_PAPER_TYPE_MIN_PHOTO_SEMI_GLOSS     0x001CL
#define DPS_PAPER_TYPE_MIN_PHOTO_SATIN          0x001DL
#define DPS_PAPER_TYPE_MIN_PHOTO_MATTE          0x001EL
#define DPS_PAPER_TYPE_MIN_PHOTO_FILM           0x001FL
#define DPS_PAPER_TYPE_MIN_BACKPRINT_FILM       0x0020L
#define DPS_PAPER_TYPE_MIN_CARDSTOCK            0x0021L

/* File types */
#define DPS_FILE_TYPE_DEFAULT           0x53000000L
#define DPS_FILE_TYPE_EXIF_JPEG         0x53010000L
#define DPS_FILE_TYPE_EXIF              0x53020000L
#define DPS_FILE_TYPE_JPEG              0x53030000L
#define DPS_FILE_TYPE_TIFF_EP           0x53040000L
#define DPS_FILE_TYPE_FLASH_PIX         0x53050000L
#define DPS_FILE_TYPE_BMP               0x53060000L
#define DPS_FILE_TYPE_CIFF              0x53070000L
#define DPS_FILE_TYPE_GIF               0x53080000L
#define DPS_FILE_TYPE_JFIF              0x53090000L
#define DPS_FILE_TYPE_PCD               0x530A0000L
#define DPS_FILE_TYPE_PICT              0x530B0000L
#define DPS_FILE_TYPE_PNG               0x530C0000L
#define DPS_FILE_TYPE_TIFF              0x530D0000L
#define DPS_FILE_TYPE_TIFF_IT           0x530E0000L
#define DPS_FILE_TYPE_JP2               0x530F0000L
#define DPS_FILE_TYPE_JPX               0x53100000L
#define DPS_FILE_TYPE_UNDEFINED         0x53110000L
#define DPS_FILE_TYPE_ASSOCIATION       0x53120000L
#define DPS_FILE_TYPE_SCRIPT            0x53130000L
#define DPS_FILE_TYPE_EXECUTABLE        0x53140000L
#define DPS_FILE_TYPE_TEXT              0x53150000L
#define DPS_FILE_TYPE_HTML              0x53160000L
#define DPS_FILE_TYPE_XHTML_Print       0x53170000L
#define DPS_FILE_TYPE_DPOF              0x53180000L
#define DPS_FILE_TYPE_AIFF              0x53190000L
#define DPS_FILE_TYPE_WAV               0x531A0000L
#define DPS_FILE_TYPE_MP3               0x531B0000L
#define DPS_FILE_TYPE_AVI               0x531C0000L
#define DPS_FILE_TYPE_MPEG              0x531D0000L
#define DPS_FILE_TYPE_ASF               0x531E0000L

/* Date prints */
#define DPS_DATE_PRINT_DEFAULT          0x54000000L
#define DPS_DATE_PRINT_OFF              0x54010000L
#define DPS_DATE_PRINT_ON               0x54020000L

/* File name prints */
#define DPS_FILENAME_PRINT_DEFAULT      0x55000000L
#define DPS_FILENAME_PRINT_OFF          0x55010000L
#define DPS_FILENAME_PRINT_ON           0x55020000L

/* Image optimization */
#define DPS_IMAGE_OPTIMIZE_DEFAULT      0x56000000L
#define DPS_IMAGE_OPTIMIZE_OFF          0x56010000L
#define DPS_IMAGE_OPTIMIZE_ON           0x56020000L

/* Layouts */
#define DPS_LAYOUT_DEFAULT              0x57000000L
#define DPS_LAYOUT_X_UP(x)              (DPS_LAYOUT_DEFAULT+((x)<<16))
                                        /* 1-up to 250-up */
#define DPS_LAYOUT_INDEX                0x57FE0000L
#define DPS_LAYOUT_1_UP_BORDERLESS      0x57FF0000L

/* Fixed sizes */
#define DPS_FIXED_SIZE_DEFAULT          0x58000000L
#define DPS_FIXED_SIZE_2_5x3_25         0x58010000L
#define DPS_FIXED_SIZE_3_5x_5           0x58020000L
#define DPS_FIXED_SIZE_4x6              0x58030000L
#define DPS_FIXED_SIZE_5x7              0x58040000L
#define DPS_FIXED_SIZE_8x10             0x58050000L
#define DPS_FIXED_SIZE_254x178          0x58060000L
#define DPS_FIXED_SIZE_110x74           0x58070000L
#define DPS_FIXED_SIZE_89x55            0x58080000L
#define DPS_FIXED_SIZE_100x148          0x58090000L
#define DPS_FIXED_SIZE_60x80            0x580A0000L
#define DPS_FIXED_SIZE_70x100           0x580B0000L
#define DPS_FIXED_SIZE_90x130           0x580C0000L
#define DPS_FIXED_SIZE_100x150          0x580D0000L
#define DPS_FIXED_SIZE_130x180          0x580E0000L
#define DPS_FIXED_SIZE_150x210          0x580F0000L
#define DPS_FIXED_SIZE_180x240          0x58100000L
#define DPS_FIXED_SIZE_A4               0x58110000L
#define DPS_FIXED_SIZE_LETTER           0x58120000L

/* Croppings */
#define DPS_CROPPING_DEFAULT            0x59000000L
#define DPS_CROPPING_OFF                0x59010000L
#define DPS_CROPPING_ON                 0x59020000L

#define MAX_CAPABILITIES 100
typedef struct
{
    jint32_t count;
    dps_capability_t cap_type;  /* DK : capability type */
    dps_param_t capability[MAX_CAPABILITIES];
} dps_cap_array_t;

typedef struct
{
    juint16_t left;
    juint16_t top;
    juint16_t width;
    juint16_t height;
} dps_crop_area_t;

#define MAX_DCF_PATH_SIZE 255
typedef struct
{
    juint16_t prt_pid;
    char file_path[MAX_DCF_PATH_SIZE+1];
    juint16_t copy_id;
} dpof_params_t;

typedef struct
{
    int job_is_dpof;
    dpof_params_t dpof_params;

    juint16_t pages_done;
    juint16_t total_pages;

    juint16_t images_printed;    /* Invalid for DPOF jobs */
} dps_job_status_t;

typedef struct
{
    dps_param_t print_service_status;
    dps_param_t job_end_reason;
    dps_param_t error_status;
    dps_reason_t error_reason;
    dps_param_t disconnect_enable;
    dps_param_t capability_changed;
    dps_param_t new_job_ok;
} dps_device_status_t;

/* Possible print_service_status values */
#define DPS_DEV_STAT_PRINTING           0x70000000L
#define DPS_DEV_STAT_IDLE               0x70010000L
#define DPS_DEV_STAT_PAUSED             0x70020000L

/* Possible job_end_reason values */
#define DPS_DEV_STAT_JOB_NOT_ENDED      0x71000000L
#define DPS_DEV_STAT_JOB_ENDED          0x71010000L
#define DPS_DEV_STAT_JOB_TERMINATED     0x71020000L /* Abort style 
                                                       'immediate' */
#define DPS_DEV_STAT_JOB_ABORTED        0x71030000L /* Abort style 
                                                       'complete current' */
#define DPS_DEV_STAT_JOB_ENDED_OTHER    0x71040000L

/* Possible error_status values */
#define DPS_DEV_STAT_ERR_NONE           0x72000000L
#define DPS_DEV_STAT_ERR_WARNING        0x72010000L     /* Recoverable error */
#define DPS_DEV_STAT_ERR_FATAL          0x72020000L /* Unrecoverable error */

/* Possible error_reason values (dps_reason_t / use REASON MACROS) */
#define DPS_DEV_STAT_ERR_REASON_NONE            0x73000000L
#define DPS_DEV_STAT_ERR_REASON_PAPER           0x73010000L
/* Paper minor values */
#define         DPS_DEV_STAT_ERR_REASON_PAPER_DEFAULT   0x0000L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_EMPTY     0x0100L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_LOAD      0x0200L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_EJECT     0x0300L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_MEDIA     0x0400L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_JAMMED    0x0500L
#define         DPS_DEV_STAT_ERR_REASON_PAPER_LOW       0x0600L /* almost
                                                                   empty */
#define         DPS_DEV_STAT_ERR_REASON_PAPER_MISMATCH  0x0700L

#define DPS_DEV_STAT_ERR_REASON_INK             0x73020000L
/* Ink minor values */
#define         DPS_DEV_STAT_ERR_REASON_INK_DEFAULT     0x0000L
#define         DPS_DEV_STAT_ERR_REASON_INK_EMPTY       0x0100L
#define         DPS_DEV_STAT_ERR_REASON_INK_LOW         0x0200L
#define         DPS_DEV_STAT_ERR_REASON_INK_WASTE_FULL  0x0300L

#define DPS_DEV_STAT_ERR_REASON_HARDWARE        0x73030000L
/* HW minor values */
#define         DPS_DEV_STAT_ERR_REASON_HW_DEFAULT      0x0000L
#define         DPS_DEV_STAT_ERR_REASON_HW_FATAL        0x0100L
#define         DPS_DEV_STAT_ERR_REASON_HW_SRVCALL      0x0200L
#define         DPS_DEV_STAT_ERR_REASON_HW_UNAVAIL      0x0300L
#define         DPS_DEV_STAT_ERR_REASON_HW_BUSY         0x0400L
#define         DPS_DEV_STAT_ERR_REASON_HW_LEVER        0x0500L
#define         DPS_DEV_STAT_ERR_REASON_HW_COVER_OPEN   0x0600L
#define         DPS_DEV_STAT_ERR_REASON_HW_NO_MRK_AGENT 0x0700L
#define         DPS_DEV_STAT_ERR_REASON_HW_INK_COVER    0x0800L
#define         DPS_DEV_STAT_ERR_REASON_HW_NO_CARTRIDGE 0x0900L

#define DPS_DEV_STAT_ERR_REASON_FILE            0x73040000L
/* File minor values */
#define         DPS_DEV_STAT_ERR_REASON_FILE_DEFAULT    0x0000L
#define         DPS_DEV_STAT_ERR_REASON_FILE_INFO       0x0100L
#define         DPS_DEV_STAT_ERR_REASON_FILE_FORMAT     0x0200L

/* Possible disconnect_enable values */
#define DPS_DEV_STAT_DISCONN_ENABLED    0x74000000L
#define DPS_DEV_STAT_DISCONN_DISABLED   0x74010000L

/* Possible capability_changed values */
#define DPS_DEV_STAT_CAP_CHNG_FALSE     0x75000000L
#define DPS_DEV_STAT_CAP_CHNG_TRUE      0x75010000L

/* Possible new_job_ok values */
#define DPS_DEV_STAT_NEW_JOB_NOT_OK     0x76000000L
#define DPS_DEV_STAT_NEW_JOB_OK         0x76010000L

typedef struct
{
    dps_param_t quality;        /* Use DPS_QUALITY_* values */
    dps_param_t paper_type;     /* Use DPS_PAPER_TYPE_* values */
    dps_param_t paper_size;     /* Use DPS_PAPER_SIZE_* values */
    dps_param_t file_type;      /* Use DPS_FILE_TYPE_* values */
    dps_param_t date_print;     /* Use DPS_DATE_PRINT_* values */
    dps_param_t filename_print; /* Use DPS_FILENAME_PRINT_* values */
    dps_param_t image_optimize; /* Use DPS_IMAGE_OPTIMIZE_* values */
    dps_param_t layout;         /* Use DPS_LAYOUT_* values */
    dps_param_t fixed_size;     /* Use DPS_FIXED_SIZE_* values */
                                /*   - Cannot be specified with layout */
    dps_param_t cropping;       /* Use DPS_CROPPING_* values */
} dps_job_config_t;

#define DPS_FILENAME_SIZE 24
#define DPS_DATE_SIZE 24
#define DPS_FILEPATH_SIZE 64 /* TODO : figure out right size */
typedef struct
{
    dps_crop_area_t cropping_area;
    ptp_obj_handle_t file_id;                   /* Mandatory */
    char file_name[DPS_FILENAME_SIZE+1];
    char date[DPS_DATE_SIZE+1];                 /* Arbitrary string */
    juint16_t copies;
    dpof_params_t dpof_restart;
    char file_path[DPS_FILEPATH_SIZE+1];
} dps_job_info_t;


/***********************************
 *    Print Client (DSC) API       *
 ***********************************/


/* Print Client Operations */
jresult_t dps_configure_print_service(dps_handle_t handle,
    dps_req_conf_t *dsc_config);

jresult_t dps_start_job(dps_handle_t handle, dps_job_config_t *config,
    dps_job_info_t *info, jint_t count);

#define DPS_ABORT_STYLE_IMMEDIATE               0x90000000L
#define DPS_ABORT_STYLE_COMPLETE_CURRENT        0x90010000L
jresult_t dps_abort_job(dps_handle_t handle, dps_param_t abort_style);

jresult_t dps_continue_job(dps_handle_t handle);

jresult_t dps_get_capability(dps_handle_t handle,
    dps_capability_t cap_type, dps_param_t paper_size);

jresult_t dps_get_job_status(dps_handle_t handle);

jresult_t dps_get_device_status(dps_handle_t handle);

/* Print Client Operation Completion Callbacks */
typedef void (*dps_config_print_service_cb)(dps_appctx_t appctx,
    dps_param_t result, dps_res_conf_t *printer_cfg);

typedef void (*dps_complete_cb)(dps_appctx_t appctx, dps_param_t result);

typedef void (*dps_get_capability_cb)(dps_appctx_t appctx, dps_param_t result,
    dps_cap_array_t *cap_list);

typedef void (*dps_get_job_status_cb)(dps_appctx_t appctx, dps_param_t result,
    dps_job_status_t *status);

typedef void (*dps_get_device_status_cb)(dps_appctx_t appctx,
    dps_param_t result, dps_device_status_t *status);
    
/* Print Client Event Handlers */
typedef dps_param_t (*dps_notify_job_status_cb)(dps_appctx_t appctx,
    dps_job_status_t *status);

typedef dps_param_t (*dps_notify_device_status_cb)(dps_appctx_t appctx,
    dps_device_status_t *status);

/* Storage Service Handlers */
typedef dps_param_t (*dps_get_file_id_cb)(dps_appctx_t appctx,
    ptp_obj_handle_t autprint_mrk_file_id, char *dcf_file_path,
    ptp_obj_handle_t *id);

/* DPS Discovery Callbacks */
typedef jint_t (*dps_printer_connect_cb)(dps_appctx_t appctx);
typedef void (*dps_printer_estab_cb)(dps_appctx_t appctx);
typedef void (*dps_printer_disconnect_cb)(dps_appctx_t appctx);


/**************************************
 *  PictBridge Initialization API  *
 **************************************/

typedef struct
{
    /* DPS detection */
    dps_printer_connect_cb      printer_connect_h;
    dps_printer_estab_cb        printer_estab_h;
    dps_printer_disconnect_cb   printer_disconnect_h;
    
    /* Print client action completion callbacks */
    dps_config_print_service_cb config_print_service_complete;
    dps_complete_cb             start_job_complete;
    dps_complete_cb             abort_job_complete;
    dps_complete_cb             continue_job_complete;
    dps_get_capability_cb       get_capability_complete;
    dps_get_job_status_cb       get_job_status_complete;
    dps_get_device_status_cb    get_device_status_complete;
    
    /* Print client event handlers */
    dps_notify_job_status_cb    notify_job_status_h;
    dps_notify_device_status_cb notify_device_status_h;

    /* DPOF translation handler */
    dps_get_file_id_cb          get_file_id_h;
} dps_callbacks_t;

jresult_t dps_init(ptp_instanceh_t insth, dps_callbacks_t *cbs,
    dps_appctx_t appctx, dps_handle_t *dpsh);

#endif/*_DPS_API_H_*/
