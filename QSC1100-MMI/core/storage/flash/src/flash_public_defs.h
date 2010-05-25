#ifndef __FLASH_PUBLIC_DEFS_H__
#define __FLASH_PUBLIC_DEFS_H__

/**********************************************************************
 * flash_public_defs.h
 *
 * Flash driver public definitions
 *
 * This file exposes definitions used flash driver's public interface
 *
 * Copyright (c) 2007-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_public_defs.h#8 $

   
when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-10-21   jz      Added FLASH_NAND_FACTORY_MODE_ECC to support
                     OneNAND factory image programming
2008-07-10   mh      Add MACRO for FLASH_SLC_MLC 
2008-02-07   mh      Add  FS_DEVICE_FAIL_PAGE_ERASED 
2008-02-07   mh      Add flash_dm_mode_e_type, flash_cmd_e_type, and
                     flash_dm_d_cmd_id_type
2007-09-26   op      Add support for NOR sibley
2007-07-16   mm      Moved public defines from internal header files.
2007-06-28   rt      Add support for NOR partitioning
2007-06-23   rt      Created with public definitions from flash.h

===========================================================================*/

/* Types used by flash driver routines */
typedef uint32 page_id;
typedef uint32 block_id;
typedef uint32 cluster_id;
/* Please note that the type 'cluster_id' is not used by flash driver code
 * anymore.  It has been maintained in the API for backward compatibility.*/



/* Possible result codes returned by flash drive routines */
#define FS_NO_DEVICE                              ((fs_device_t) 0)
#define FS_DEVICE_OK                              0
#define FS_DEVICE_DONE                            0
#define FS_DEVICE_FAIL                            (-1)
#define FS_DEVICE_BAD_BLOCK                       (-2)
#define FS_DEVICE_BUSY                            (-3)
#define FS_DEVICE_PARTITION_NOT_FOUND             (-4)
#define FS_DEVICE_NO_PARTITION_TABLE              (-5)
#define FS_DEVICE_INVALID_DEVICE                  (-6)
#define FS_DEVICE_INVALID_PARTITION_NAME          (-7)
#define FS_DEVICE_INVALID_FLASH                   (-8)
#define FS_DEVICE_PARTITION_TABLE_FULL            (-9)
#define FS_DEVICE_PARTITION_INVALID_NAME          (-10)
#define FS_DEVICE_PARTITION_TABLE_EXISTS          (-11)
#define FS_DEVICE_DUPLICATE_PARTITION             (-12)
#define FS_DEVICE_INVALID_PARTITION_TABLE         (-13)
#define FS_DEVICE_PARTITION_TABLE_VERSION_SKEW    (-14)
#define FS_DEVICE_NOT_SUPPORTED                   (-15)
#define FS_DEVICE_CP_READ_FAIL                    (-16)
#define FS_DEVICE_CP_WRITE_FAIL                   (-17)
#define FS_DEVICE_FAIL_PAGE_ERASED                (-18)

/* Flash Lock/Unlock operations */
#define FLASH_UNLOCK_OP                           0x1
#define FLASH_LOCK_OP                             0x2
#define FLASH_LOCKTIGHT_OP                        0x3


/* Definitions for ECC disabling/enabling. ECC can be enabled in two modes.
 * FLASH_NAND_ECC_ENABLED - In this mode ECC is enabled only on the Main Area 
 * FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED - In this mode, ECC is enabled on the
 * Main and the spare area 
 */
#define FLASH_NAND_ECC_DISABLED                   0x0
#define FLASH_NAND_ECC_DEFAULT_DISABLED           FLASH_NAND_ECC_DISABLED
#define FLASH_NAND_ECC_ENABLED                    0x1
#define FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED     0x2
/* The following mode is used for factory image programming, where,
 * for OneNAND, programming is done on Main & Spare areas with ECC enabled,
 * while for NAND, programming is done on Main & Spare areas with ECC disabled.
 */
#define FLASH_NAND_FACTORY_MODE_ECC               0x3

/* Technology: NOR/NAND/ONENAND/ORNAND/etc */
#define FLASH_TECH_MASK                           0x0F  
#define FLASH_NOR                                 0x00
#define FLASH_NAND                                0x01
#define FLASH_ONENAND                             0x02
#define FLASH_ORNAND                              0x03

/* Bits Per Cell: SLC/MLC/SLC+MLC/Mirrorbit/etc */
#define FLASH_BPC_MASK                            0xF0   
#define FLASH_SLC                                 0x00
#define FLASH_MLC                                 0x10
#define FLASH_SLC_MLC                             0x20
#define FLASH_MB                                  0x40

/* Definitions to allow NAND width detection */
#define FLASH_NAND_8BIT                           0x01
#define FLASH_NAND_16BIT                          0x02

/* Description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
typedef enum {

  /* NOR devices where arbitrary bits can be set to zero.  SLC NOR devices
   * generally fall in this category. */
  FS_DEVICE_WRITES_SIMPLE,

  /* NAND devices requiring entire pages to be written, and can only be
   * written to once.  These devices do not allow 'partial_write'. */
  FS_DEVICE_WRITES_PAGED,

  /* NOR devices where adjacent pairs of bits are written together.  A pair
   * can be written from '11' to a value, and from a value to '00'.  If
   * recovery from failed writes is needed, the only two values that should
   * be used are '11' and '00'.  After a failed write, it may not be
   * possible to write the pair to any value other than '00'. */
  FS_DEVICE_WRITES_PAIRED_BITS,

  /* Indicates a NOR device with an unsupported write style.  No
   * assumptions can be made about how the data is written.  Pages must be
   * written in their entirety, and pages that have bad powerdowns cannot
   * be used. */
  FS_DEVICE_WRITES_UNSUPPORTED = 0x1000,
} fs_device_write_style_t;

 typedef enum flash_cmd_e {
  PROBE_DEVICE,
  READ_PAGE,
  READ_PAGE_WM,
  READ_SPARE,
  READ_SPARE_WM,
  READ_MPAGE,
  WRITE_PAGE,  
  WRITE_PAGE_WM,
  WRITE_SPARE,
  WRITE_SPARE_WM,
  COPY_PAGE,
  READ_OTP_PAGE,
  READ_CUSTOM_ID,
  BLOCK_LOCK_UNLOCK,
  CHECK_BAD_BLOCK,
  ERASE_BLOCK,
  VERIFY_BLOCK,
  WAIT_FOR_INT,
  FLASH_CMD_LAST
} flash_cmd_e_type;

typedef enum flash_dm_mode_e {
  POLLING_MODE,
  INTERRUPT_MODE
} flash_dm_mode_e_type;

typedef struct flash_dm_d_cmd_id {
  flash_cmd_e_type     command;
  flash_dm_mode_e_type    mode;
} flash_dm_d_cmd_id_type;


/* Defines the minimum and maximum number of device ID bytes that can be passed
 * to APIs accessing OTP area. */ 
#define FLASH_NAND_MIN_ID_BLEN 2
#define FLASH_NAND_MAX_ID_BLEN 8

/* Defines for shortest and longest command byte sequences that are allowed.
 * Longest command sequence is determined by the driver resources. Shortest
 * comamnd sequence is determined by OTP page read algorithm and consists of
 * a 1-byte reset command, followed by 1-byte read command and terminated by
 * 1-byte reset command.*/
#define FLASH_NAND_MIN_OTP_CMD_BLEN 3
#define FLASH_NAND_MAX_OTP_CMD_BLEN 8

struct flash_otp_op_params;

typedef struct flash_otp_op_params *flash_otp_op_params_t;

struct flash_otp_op_params {
  uint8 dev_id[FLASH_NAND_MAX_ID_BLEN];         /* Flash device IDs */
  uint32 num_id_bytes;                          /* Number of ID bytes */
  uint8 otp_cmd[FLASH_NAND_MAX_OTP_CMD_BLEN];   /* OTP access cmd sequence */
  uint32 num_cmd_bytes;                         /* Num of bytes in cmd seq. */
  uint32 disable_ecc;
};

#endif /* __FLASH_PUBLIC_DEFS_H__ */
