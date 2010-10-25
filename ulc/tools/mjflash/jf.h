#ifndef JF_H
#define JF_H
/*===========================================================================

         Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

DESCRIPTION
  This header file contains the struct that is used to communicate 
  between our plug-in and Lauterbach TRACE32 Debugger. Also contains
  generic types.

  Copyright (c) 2006-2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jf.h#7 $ $DateTime: 2008/04/23 20:17:46 $ $Author: c_surven $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/08   sv      Add macros for buffer sizes 
03/24/08   sv      Add JFLASH return codes macros
01/03/08   op      Add support for single-image NOR targets
09/22/07   op      clean up file
02/28/07   rt      Add support for multi-image NOR targets
12/06/06   rt      Create initial version from JFLASH 2.1
===========================================================================*/

#define JFLASH_JTAG_COMM_BUFFER          0x20000
/* 512 bytes each for System and User partition tables */
#define JFLASH_PARTI_BSIZE               (512 * 2) 
#define JFLASH_MAX_FNAME_BSIZE           40
#define JFLASH_MAX_PATH_BSIZE            1024

// This structure is used to communcate between TRACE32 Debugger and
// our FLASH plug-in.
struct type_jtag_flash_param {
   uint32 flashaddr;
   uint32 flashwidth;
   uint32 width;
   uint32 offset;      
   uint32 addr;        // in: address of the flash to write/erase
   uint32 size;        // in: size of data to write/erase
   uint32 reserved;
   uint32 status;      // in: 1=prog., 2=erase, 5=erase, out: 0=o.k., 0x100=fail
   byte   data[JFLASH_JTAG_COMM_BUFFER];
                       // give 128K worth of buffer
                       // this is where TRACE32 will store data that
                       // we'll be writing to the FLASH
};

/* This structure is used to communcate from our program to T32
 * in an extended fashion for post processing statistics.  It
 * overlays the data area of jtag_flash_param and presents
 * a different interface */
struct type_jtag_report {
   uint32 input_gap_size;
   uint32 bad_block_gap_size;
   uint32 image_size;
   uint32 reserved1;      
   uint32 reserved2;
   uint32 reserved3;
   uint32 reserved4;
   uint32 status;           // out: 0=o.k., 0x100=fail
};



 /*===========================================================================

                     MACRO DEFINITIONS FOR MODULE

===========================================================================*/

/* command code from type_jtag_flash_param.status */

/* Please NOTE that all these commands are not used for programming NOR flash
 * but are produced here for completeness.
 */
#define FLASH_PROGRAM                 0x00000001
#define FLASH_ERASE                   0x00000002
#define FLASH_ERASE5                  0x00000005
#define FLASH_INITIALIZE              0x00000009
#define FLASH_FINALIZE                0x0000000A
#define FLASH_SELFTEST                0x0000000B
#define FLASH_REPORT                  0x0000000C
#define FLASH_VERIFY                  0x0000000D
#define FLASH_INIT_BOOT               0x0000000E
#define FLASH_ERASE_CHIP              0x00000020
#define FLASH_RETRIEVE_ID             0x00000021
#define FLASH_VALID_BOOT              0x00000022
#define FLASH_REPORT_BAD              0x00000023
#define FLASH_STRESS_TEST             0x00000024
#define FLASH_FS_DUMP_INIT            0x00000025
#define FLASH_FS_DUMP_BLOCK           0x00000026
#define FLASH_MARK_BLOCK_BAD          0x00000027
#define FLASH_MARK_BLOCK_GOOD         0x00000028
#define FLASH_READ_PAGE               0x00000029

/* Set ECC state */
#define FLASH_SET_ECC_STATE           0x0000002A

/* Raw block erase from blockutil */
#define FLASH_ERASE_BLOCK             0x0000002B

/* Fill page with value */
#define FLASH_FILL_PAGE               0x0000002E
#define FLASH_FILL_PAGE_ORD           0x0000002F
#define FLASH_NAND_DUMP_INIT          0x00000030

/* Multi-Image only */
#define FLASH_USR_PARTI_TBL           0x00000040
#define FLASH_INIT_PBL                0x00000041
#define FLASH_INIT_QCSBLHD            0x00000042
#define FLASH_INIT_QCSBL              0x00000043
#define FLASH_INIT_OEMSBL             0x00000044
#define FLASH_INIT_AMSS               0x00000045
#define FLASH_PRINT_MIBIB             0x00000046
#define FLASH_PRINT_LAYOUT            0x00000047
#define FLASH_SECURITY_MODE           0x00000048

#define FLASH_INIT_OBL                0x00000049

/* APPS processor support on MSM7500 */
#define FLASH_INIT_APPSBL             0x0000004A
#define FLASH_INIT_APPS               0x0000004B

#define FLASH_INIT_CEFS               0x0000004C
#define FLASH_SYS_PARTI_TBL           0x0000004D
#define FLASH_INIT_BOOT_VEC_TBL       0x0000004E
#define FLASH_INIT_FOTAUI             0x0000004F
#define FLASH_INIT_FOTAPKG            0x00000050
#define FLASH_INIT_ALL                0x00000051

#define FLASH_PARTI_NO_OVERRRIDE      0x0
#define FLASH_PARTI_OVERRRIDE         0x1

/* return code for type_jtag_flash_param.status */
#define JFLASH_SUCCESS   0
#define JFLASH_FAIL      0x00000100

extern const char *id_tbl[];


#endif  /* JF_H */

