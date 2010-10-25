#ifndef HSU_DPS_DEFS_H
#define HSU_DPS_DEFS_H
/*==============================================================================

            hsu_dps_defs   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains DPS common definitions for high speed usb.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_dps_defs.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
09/11/07  me  Creation

==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/
#define HSU_DPS_FILENAME_SIZE   24
#define HSU_DPS_DATE_SIZE       24
#define HSU_DPS_FILEPATH_SIZE   64
#define HSU_MAX_DCF_PATH_SIZE   255
#define HSU_DPS_MAX_VERSIONS    10
#define HSU_DPS_MAX_STRING_SIZE 64
#define HSU_MAX_CAPABILITIES    100

/*==============================================================================
Typedefs
==============================================================================*/
typedef struct
{
  uint16 prt_pid;
  char   file_path[HSU_MAX_DCF_PATH_SIZE+1];
  uint16 copy_id;
} hsu_dpof_params_t;

typedef struct
{
  int               job_is_dpof;
  hsu_dpof_params_t dpof_params;
  uint16            pages_done;
  uint16            total_pages;
  uint16            images_printed;
} hsu_dps_job_status_t;

typedef struct
{
  uint16 major;
  uint16 minor;
} hsu_dps_version_t;

typedef struct
{
  int               versions_count;
  hsu_dps_version_t dps_version[HSU_DPS_MAX_VERSIONS];
  char              vendor_name[HSU_DPS_MAX_STRING_SIZE+1];
  hsu_dps_version_t vendor_specific_version;
  char              product_name[HSU_DPS_MAX_STRING_SIZE+1];
  char              serial_no[HSU_DPS_MAX_STRING_SIZE+1];    
} hsu_dps_req_conf_t;

typedef struct
{
  uint32             service_available;
  hsu_dps_req_conf_t conf;
} hsu_dps_res_conf_t;

typedef struct
{
  uint16  left;
  uint16  top;
  uint16  width;
  uint16  height;
} hsu_dps_crop_area_t;

typedef struct
{
  hsu_dps_crop_area_t cropping_area;
  uint32              file_id;                   /* Mandatory */
  char                file_name[HSU_DPS_FILENAME_SIZE+1];
  char                date[HSU_DPS_DATE_SIZE+1];                 /* Arbitrary string */
  uint16              copies;
  hsu_dpof_params_t   dpof_restart;
  char                file_path[HSU_DPS_FILEPATH_SIZE+1];
} hsu_dps_job_info_t;

typedef struct
{
  uint32 quality;      
  uint32 paper_type;   
  uint32 paper_size;   
  uint32 file_type;    
  uint32 date_print;   
  uint32 filename_print;
  uint32 image_optimize;
  uint32 layout;        
  uint32 fixed_size;    
  uint32 cropping;
} hsu_dps_job_config_t;


typedef struct
{
  uint32 print_service_status;
  uint32 job_end_reason;
  uint32 error_status;
  uint32 error_reason;
  uint32 disconnect_enable;
  uint32 capability_changed;
  uint32 new_job_ok;
} hsu_dps_device_status_t;


typedef enum
{
  HSU_DPS_CAP_QUALITIES,
  HSU_DPS_CAP_PAPER_SIZES,
  HSU_DPS_CAP_PAPER_TYPES,        /* Optional paper_size parameter can be used */
  HSU_DPS_CAP_FILE_TYPES,
  HSU_DPS_CAP_DATE_PRINTS,
  HSU_DPS_CAP_FILENAME_PRINTS,
  HSU_DPS_CAP_IMAGE_OPTIMIZE,
  HSU_DPS_CAP_LAYOUTS,            /* Used with the paper_size parameter */
  HSU_DPS_CAP_FIXED_SIZES,
  HSU_DPS_CAP_CROPPINGS
} hsu_dps_capability_t;

typedef struct
{
  int32                count;
  hsu_dps_capability_t cap_type;
  uint32               capability[HSU_MAX_CAPABILITIES];
} hsu_dps_cap_array_t;

#endif /* HSU_DPS_DEFS_H */
