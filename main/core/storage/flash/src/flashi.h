#ifndef __FLASHI_H__
#define __FLASHI_H__

/**********************************************************************
 * flashi.h
 *
 * Flash driver internal interface
 *
 * This file implements the generic flash driver's internal interface
 *
 * Copyright (c) 2004, 2006-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flashi.h#18 $ $DateTime: 2008/09/16 10:38:42 $ $Author: mhossain $

   
when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-08-16   mh      Add SS_4b_ECC_IN_SPARE_UDATA enum for 4-bit ECC used by 
                     FlexOneNAND
2008-07-01   jz      Added FLASH_SAMSUNG_FAMILY to flash_family_type
2008-03-20   eo      Add return status to set_ecc_state API
2008-03-11   sv      Add new APIs to NOR ops table for M18 half-density mode
2008-01-16   rt      Removed a TAB, clean up superflous ',' in enum definitions
2007-10-30   dp      Increase max ctlr specific flags
2007-09-22   op      Add Flash size 128MB, and Flash buffer 1024 bytes
2007-06-19   mm      Moved device types to external header file flash.h
2007-05-10   rt      Add support for reading pages from OTP area.
2007-02-28   rt      Add support for NOR partitioning
2006-06-27   dp      Adding Flash API updates for copy page & spare data bytes
2006-05-04   dp      Adding controller abstraction layer
2004-08-09   drh     Added support for ECC manipulation
2004-06-09   drh     Defines for preventing 8/16 bit NAND mis-detection
2004-03-26   drh     Change FSI_NO_DEVICE to an integer value from a pointer
                     in support of new API for flash family probe functions.
2004-02-23   drh     Created

===========================================================================*/
#include "flash.h"
#include "miparti.h"    /* To include the flash partition attribute enum */

/* Private flash device interface */

#define FSI_NO_DEVICE (-1)

/* Maximum controller specific dword flags */
#define FLASH_MAX_CTLR_FLAGS   25

/*   Defines for NOR */
#define MAX_FLASH_BANKS       16
#define MAX_FLASH_REGIONS     5

/* Device Control opts for half-density mode for MLC NOR devices */
#define FLASHI_DEV_CTRL_OPTS_HD_MODE      0x01

typedef enum
{
  NO_ECC_TYPE_SET       = 0,
  HW_ECC_IN_SPARE_UDATA = 1,
  SW_ECC_IN_SPARE_UDATA = 2
} spare_ecc_type_enum;

typedef enum
{
  NO_ECC_IN_SPARE_UDATA  = 0,
  HC_ECC_IN_SPARE_UDATA  = 1,
  SS_4b_ECC_IN_SPARE_UDATA = 2 /* 4-bit ECC used by SS FlexOneNAND */
} spare_ecc_algo_enum;


/* Status return enums */
typedef enum
{
 FLASH_SUCCESS          = 0x0,
 FLASH_FAILURE          = 0x1,
 FLASH_INVALID_LEN      = 0x3,
 FLASH_TIMEOUT          = 0x7,
 FLASH_VERIFY_FAILED    = 0xA,
 FLASH_OP_NOT_COMPLETE  = 0x15    // same value as FS_GC_IN_PROGRESS_S enum
} flash_status;

/* Flash family - Intel, AMD, Spansion, etc. */
typedef enum
{
  FLASH_SPANSION_FAMILY, 
  FLASH_INTEL_FAMILY,
  FLASH_SAMSUNG_FAMILY, 
  FLASH_UNKNOWN_FAMILY,
  FLASH_MAX_FAMILIES =   FLASH_UNKNOWN_FAMILY
} flash_family_type;

/* Flash size enum in terms of x, where flash size = 2^x */
typedef enum
{
  FLASH_SIZE_UNKNOWN = 0,
  FLASH_SIZE_4MB     = 22,
  FLASH_SIZE_8MB, 
  FLASH_SIZE_16MB,
  FLASH_SIZE_32MB,
  FLASH_SIZE_64MB,
  FLASH_SIZE_128MB 
} flash_size_type;

/* Flash device width interface capability enum */
typedef enum
{
  FLASH_XIFACE_8 = 0,
  FLASH_XIFACE_16,
  FLASH_XIFACE_32,
  FLASH_XIFACE_64
} flash_xiface_type;

/* Write buffer size if any in bytes */
typedef enum
{
  FLASH_WBUF_NOT_PRESENT = 0,
  FLASH_WBUF_2,
  FLASH_WBUF_4,
  FLASH_WBUF_8,  
  FLASH_WBUF_16,  
  FLASH_WBUF_32,    
  FLASH_WBUF_64,    
  FLASH_WBUF_128,    
  FLASH_WBUF_1024 = 10
} flash_wbuf_size_type;



/* The Flash device is, of course, memory mapped, so the compiler might
 * find the various device manipulations to be nonsense, and optimize
 * it all away.  To prevent this, we use pointers to volatile to let
 * the compiler know that we're dealing with a special device and that
 * it is to leave the operations as written. 
 */
typedef  volatile word *flash_ptr_type;


/* Structure for describing a template of the block map of a 
 * particular part flash part.  This is used in building the run time
 * block map.  We no longer keep static expanded tables for each part.
 */
typedef struct {
  int numblocks;
  int blocksize_bytes;
} block_info;



/* Structure for storing a table of sector boundaries representing bank 
 * organization of the flash device.
 */
typedef struct
{
  word start_block;
  word num_blocks;
  word end_block;
  word is_read_only;            /* Flag: TRUE if bank is used to store code */
} flash_bank_info;


/* Structure to store the pertinent Flash device geometry information 
 */
typedef struct
{
  flash_family_type     family_type;                     /* AMD/Spansion/Intel 
                                                            family type       */
  flash_size_type       device_size;                     /* n such that actual 
                                                            device size = 2^  */
  flash_xiface_type     x_iface;                         /* flash device width*/
  flash_wbuf_size_type  write_buf_size;                  /* n such that max 
                                                            write buffer size */
  word                  num_erase_blk_region;            /* How many different 
                                                            sizes             */
  block_info            blk_regions[MAX_FLASH_REGIONS];  /* Block layout      */
  word                  num_total_blocks;                /* Total Number of 
                                                            blocks            */
  word                  num_banks;                       /* Total Number of 
                                                            Banks             */
  flash_bank_info       bank_info[MAX_FLASH_BANKS];      /* Bank Information:
                                                            Number of sectors */
} flash_geometry_info;


struct flashi_nor_dev_ops;  /* Forward declaration */

typedef struct {
  const char                  *name;             /* Textual name of device. */
  word                        num_ids;           /* Number of codes to match */
  word                        codes[4];          /* Manufacture's codes. */
  word                        dev_ctrl_opts;     /* Any device specific
                                                    differentiation */
  fs_device_write_style_t     write_style;       /* Write Grouping Style      */
  volatile word               *base_addr;        /* Base Address of the device*/
  word                        pg_size;           /* Page size of the device   */
  flash_geometry_info         *geometry;         /* User returned info. */
  flash_geometry_info         *handcoded_geometry; /* Geometry info for tools */
  struct flashi_nor_dev_ops   *ops;              /* Flash operations */
} flashi_nor_device;

/* Individual device's data used while doing open_partition() and
 * some is copied into client instance for use while later accessing
 * device during use.
 */
struct flash_device_config_data;

typedef struct flash_device_config_data *flash_device_config_t;

struct flash_device_config_data {

  /*------------------------------
   * Device specific data 
   *----------------------------*/
  char     *dev_name;          /* Device name string                      */
  uint32   block_count;        /* Number of blocks in actual device       */
  uint32   page_count;         /* Number of pages in block                */
  uint32   page_size;          /* Number of bytes not including spare     */
  uint32   total_page_size;    /* Number of bytes including spare         */
  int      maker_id;           /* Maker Identification                    */
  int      device_id;          /* Device Identification                   */
  int      device_width;       /* Device width for NAND checking          */
  uint32   device_type;        /* NAND/OneNAND/ORNAND/etc                 */
  uint32   dev_specfic_flag1 ; /* Extra device specific data per instance */
  uint32   dev_specfic_flag2 ; /* Extra device specific data per instance */
};

struct spare_layout
{
  uint8 unit_bsize;       /* Number of bytes of spare layout units (eg: 16)*/
  uint8 num_cw;           /* Total number of code word units (4 for 2K)    */
  uint8 bb_boffset;       /* Offset to Bad block marker in spare in bytes  */
  uint8 bb_blen;          /* Number of bytes for bad block marker          */
  uint8 lsn_boffset;      /* Offset to LSN data in spare in bytes          */
  uint8 lsn_blen;         /* Number of LSN data bytes in spare             */
  uint8 udata_boffset;    /* Offset to user data in spare in bytes         */
  uint8 udata_blen;       /* Number of user data bytes in spare            */
  uint8 eccm_boffset;     /* Offset to ECC of Main Area in spare in bytes  */
  uint8 eccm_blen;        /* Number of ECC bytes for Main Area             */
  uint8 eccs_boffset;     /* Offset to ECC of Spare Area in spare in bytes */
  uint8 eccs_blen;        /* Number of ECC bytes for Spare Area            */
};

struct spare_info_data
{
  const struct spare_layout *spare_layout_ptr; /* Spare Layout */
  spare_ecc_type_enum ecc_type;                /* HW/SW ECC    */
  spare_ecc_algo_enum ecc_algo;                /* HC/RS/BCH    */
  uint8 total_spare_user_bytes;                /* Total user bytes in spare */
  uint8 total_spare_user_ecc_bytes;            /* Total ECC for spare user bytes */
  uint8 corrected_user_bytes;                  /* Correctable bytes by ECC */

  /* Encodes given spare_udate_bytes using ECC algorithm used and returns
     encoded data along with its ECC */
  void *(*sw_encode_data)(struct spare_info_data *spare_info, 
    void *spare_udata_bytes, uint32 blen);

  /* Decodes given spare_udata_ecc_bytes to check for any error and returns
     status as to no error, correctable error or uncorrectable error */
  int (*sw_decode_data)(struct spare_info_data *spare_info, 
    void *spare_udata_and_ecc_bytes);  
};

struct flash_controller {

  /*
   * Controller specific place holders
   */
  uint32 ctlr_specific_flags[FLASH_MAX_CTLR_FLAGS];

  /*
   * Spare Layout information place holder 
   */
  struct spare_info_data spare_info;

  /*
   * Page based operations 
   */

  /* Read the data into a buffer.  Can be used if the above returns NULL. */
  int (*read_page) (fs_device_t self, page_id page, void *data);

  /* Read len bytes from spare area starting from spare_offset of given page.
     Data is read with ECC turned OFF */
  int (*read_spare_bytes) (fs_device_t self, page_id page, uint32 spare_offset, 
      uint32 len, void *data);

  /* Read multiple pages within a block */
  int (*read_mpages) (fs_device_t self, page_id start_page, page_id num_pages, 
      void *data, void (*wdog_kick_fn_ptr)(void));

  /* Read only the user data in spare location of given page with ECC
     correction/detection */
  int (*read_spare_udata) (fs_device_t self, page_id page, 
      void *spare_udata, uint32 spare_udata_blen);

  /* Read the data and Spare bytes into buffers. */
  int (*read_page_and_udata) (fs_device_t self, page_id page, void *data,
      void *spare, uint32 spare_udata_blen);

  /* Read user data and spare bytes from a page in OTP block */
  int (*read_otp_page_and_spare) (fs_device_t self, page_id page,
      void *data, void *spare, uint32 spare_blen, void *dev_otp_params);

  /* Write operations. Same API also supports write page data alone or
     page data with spare data. The latter would use HW/SW ECC
     to cover the spare user bytes */
  int (*write_page) (fs_device_t self, page_id page, void *page_data, 
      void *spare_udata, uint32 spare_udata_blen);

  /* Copy src_page to dest_page */
  int (*copy_page) (fs_device_t self, page_id src_page, page_id dest_page);

  /* Determine erase status of page */
  int (*is_page_erased) (fs_device_t self, page_id page);  

  /*
   * Block based operations 
   */

  /* Mark given block as bad */
  int (*mark_block_bad) (fs_device_t self, block_id block);

  /* A full erase.  Eventually, will implement NOR-style partial erasing. */
  int (*erase_block) (fs_device_t self,
      block_id block);

  /* Determine erase status of block */
  int (*is_block_erased) (fs_device_t self, block_id block);  

  /* Support Unlock, lock and locktight if needed */
  int (*lock_ops) (fs_device_t self, page_id start_block, page_id end_block,
    uint32 opcode);

  /*
   * Misc operations
   */
  /* Clear any controller specific operation bits if needed*/
  void (*clear_op_status_bit) (void);

  /* Set controller ECC state */
  int (*set_ecc_state) (fs_device_t self, int state);

  /* Issue customized <cmd><addr> ID commands and read n bytes directly from
     device, if supported */
  int (*read_custom_ids) (fs_device_t self, uint32 id_cmd, uint32 id_addr,
      void *buffer, uint32 num_bytes);

  /* Reconfigure if necessary based on partition attribute */
  int (*reconfigure) (fs_device_t self, uint32 partition_attrib);

};

/* Forward declaration of Composite device structure and a pointer type to it */
struct fsi_device_data;
typedef struct fsi_device_data *fsi_device_t;


struct flashi_nor_dev_ops {
  flash_status (*configure) (fsi_device_t self,
                             flash_ptr_type baseaddr);
                                /* Perform any needed initialization. */
  flash_status (*read) (fs_device_t self, byte *buffer, dword offset,
                        dword count);
                                /* Device independent read operation. */
  flash_status (*write) (byte *buffer, flash_ptr_type baseaddr,
                         dword offset, dword count);
                                /* Device write operation. */
  flash_status (*erase_start) (flash_ptr_type baseaddr, dword offset);
                                /* Device erase start operation. */
  flash_status (*erase_status) (flashi_nor_device *nor_device, 
                                flash_ptr_type eraseaddr);
                                /* Progress of erase operation. */
  flash_status (*suspend_erase) (flash_ptr_type eraseaddr);
                                /* Suspend an erase operation. */
  flash_status (*resume_erase) (flash_ptr_type eraseaddr);
                                /* Resume a suspended erase. */
  flash_status (*read_dev) (flash_ptr_type read_addr, byte *buffer, dword count);
                                /* Device specific read operation. Used by
                                 * device independent read if defined */ 
  void * (*read_ptr) (flash_ptr_type page_addr, dword page_size);
                                /* Device specific page read. */
  flash_status (*is_erased) (flash_ptr_type page_address, dword page_size);
                                /* Device specific page erased check */
};


/* Composite device structure */
struct fsi_device_data {

  /*----------------------------------- 
   * Device Data public dispatch table 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Callers will
   * need to allocate an fsi_device_data structure, but will use it
   * via a fs_device_data structure pointer which will allow them only
   * to access the public functions.
    */
  struct fs_device_data pub_dev;


  /*------------------------------
   * NAND Device specific data
   *----------------------------*/
  struct flash_device_config_data dev_info;

  struct flash_controller  ctlr;

  /*------------------------------
   * NOR Device specific data
   *----------------------------*/
  flashi_nor_device nor_dev_info;

  /* Numeric ID of flash part (first = 0, second = 1) */
  uint32 which_flash;

  /*------------------------------
   * Partition specific data 
   *----------------------------*/
  uint32 partition_start_baddr;      /* NOR partition start byte address */
  uint32 partition_limit_baddr;      /* NOR partition end byte address */
  uint32 partition_attributes;       /* Partition attributes */
  uint32 partition_blk_start;        /* Partition physical start block*/
  uint32 partition_blk_limit;        /* Partition physical end address */
  uint32 partition_blk_cnt;          /* Partition length in blocks */
  uint32 partition_blk_bsize;        /* Block size in bytes */ 
  uint32 partition_last_pg_id;       /* Last page ID in NOR partition */


  /* Variable to store log to base 2 of pages in the block. This is
   * used when translating block number to page number by left shift
   * operation. It is set by flash_nand_device_probe() function.
   */
  uint32 block_size_shift;

  /* Variable to store log to base 2 of page size in bytes */
  uint32 page_size_shift;

  /* Variable to Logical page size in bytes. Note the notion of pages
   * is not native to NOR flash devices. It is provided to make file
   * system design uniform across NOR and NAND devices */
  uint32 page_bsize;                 /* Logical Page size in bytes */ 
};

#endif /* __FLASHI_H__ */
