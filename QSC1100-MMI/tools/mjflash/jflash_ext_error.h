#ifndef __JFLASH_EXT_ERROR_H__
#define __JFLASH_EXT_ERROR_H__
/*===========================================================================

         Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

DESCRIPTION

  Extended Error Codes for MJFLASH CMM scripts

  Copyright (c) 2007-2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/jf.h_v   1.2   07 Nov 2001 12:30:02   dionh  $
  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jflash_ext_error.h#3 $ $DateTime: 2008/03/17 10:50:52 $ $Author: rtilak $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   sv      Modified to include ERR_BAD_DEVICE fatal code type
09/22/07   op      Add Partition too big error enum
02/28/07   rt      Create for multi-image NOR targets
===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*  The format of this enum definition is VERY critical.  It is parsed by
 *  a Perl script to create jnand_ext_error.cmm which allows verbose
 *  error reporting and prints out these human readable comments.
 *
 *  Format is:
 *
 *  ERR_something, where something is all caps
 *  no spaces, followed by =hex value, with no spaces
 *  whitespace separates enum from comment
 *  Comment must be on one line and C style
 *
 *  All enums must be explicitly defined by number, do not allow auto
 *  assignment by compiler.
 *
 *  Do not put any quotes in descriptions.
 */

typedef enum
{
  ERR_OK=0x00,              /* Success. Send an acknowledgement.            */
  ERR_BAD_CMD=0x01,         /* Failure: Not a valid command from JTAG ICD   */
  ERR_DEV_MISSING=0x02,     /* Failure: No NAND device found.               */
  ERR_BAD_DEVICE=0x03,      /* Failure: Bad Device Descriptor found         */
  ERR_OUT_OF_NAND=0x04,     /* Failure: Ran past end of NAND flash          */
  ERR_SPANS_TOO_BIG=0x05,   /* Failure: Number of spans > span buffer size  */
  ERR_PAGE_TOO_BIG=0x06,    /* Failure: Size of page > page buffer          */
  ERR_PAST_SPANS_END=0x07,  /* Failure: More spans than will fit in block   */
  ERR_PAST_NAND_END=0x08,   /* Failure: Image+ bad blocks > flash avail     */
  ERR_ERASE_FAIL=0x09,      /* Failure: Erase of good block failed          */
  ERR_WRITE_FAIL=0x0A,      /* Failure: Write of good page failed           */
  ERR_READ_FAIL=0x0B,       /* Failure: Read page failed                    */
  ERR_VERIFY_FAIL=0x0C,     /* Failure: Verify after readback failed        */
  ERR_COMPARE_FAIL=0x0D,    /* Failure: Compare failed in self test verify  */
  ERR_CRC_FAIL=0x0E,        /* Failure: CRC failed during flash verify      */
  ERR_BIBCRC_FAIL=0x0F,     /* Failure: CRC passed to selftest did not match*/
  ERR_MAGIC1_FAIL=0x10,     /* Failure: Magic1 not correct in boot_info     */
  ERR_MAGIC2_FAIL=0x11,     /* Failure: Magic2 not correct in boot_info     */
  ERR_BIBVER_FAIL=0x12,     /* Failure: VIB version not correct in boot_info*/
  ERR_OVER_CONSUME=0x13,    /* Failure: Consumed too much of input buffer   */
  ERR_PAGE_UNIT_CALC=0x14,  /* Failure: Off by one error in whole page calc */
  ERR_BIB_TOO_BIG=0x15,     /* Failure: BIB + spans too long for one block  */
  ERR_BIB_BAD=0x16,         /* Failure: BIB block was bad                   */
  ERR_SPAN_DIVIDE=0x17,     /* Failure: Partial span remainder              */
  ERR_SPAN_NOTALIGN=0x18,   /* Failure: Span not aligned on a page boundary */
  ERR_ADDR_REVERSE=0x19,    /* Failure: Current code address < previous     */
  ERR_PPI_TOO_BIG=0x1A,     /* Failure: Partial page index > page_size      */
  ERR_ECC_READ=0x1B,        /* Failure: Uncorrectable ECC error             */
  ERR_ECC=0x1C,             /* Success: Correctable ECC error               */
  ERR_ECC_SELF=0x1D,        /* Failure: Error in the ECC itself             */
  ERR_BAD_BLOCK=0x1E,       /* Failure: Block identified as bad             */
  ERR_BAD_BOOT=0x1F,        /* Failure: Boot Block identified as bad        */
  ERR_GOOD_BOOT=0x20,       /* Success: Boot Block identified as good       */
  ERR_PAST_BLOCK_END=0x21,  /* Failure: Single block image too big for block*/
  ERR_NO_MIBIB_BLOCK=0x22,  /* Failure: MIBIB block not found               */
  ERR_PARTI_NOMATCH=0x23,   /* Failure: Partition tbl mismatch, no override */
  ERR_PARTI_NOT_RCVD=0x24,  /* Failure: No partition table received         */
  ERR_PARTI_NO_QCSBL=0x25,  /* Failure: No QCSBL partition in table         */
  ERR_PARTI_NO_OEMSBL=0x26, /* Failure: No OEMSBL partition in table        */
  ERR_PARTI_NO_AMSS=0x27,   /* Failure: No AMSS partition in table          */
  ERR_PARTI_INVALID=0x28,   /* Failure: Table format not recognized         */
  ERR_SECURE_PBL=0x29,      /* Failure: PBL disallowed in Secure Boot mode  */
  ERR_SECURE_BAD_MODE=0x2A, /* Failure: Unknown Security Mode               */
  ERR_SECURE_MODE_CHG=0x2B, /* Failure: Cannot change in single invocation  */
  ERR_SEC_BAD_ORDER=0x2C,   /* Failure: Security mode must be first         */
  ERR_ALLOC_EXCEEDED=0x2D,  /* Failure: Image size exceeds allocation for it*/
  ERR_PARTI_NO_EFS2=0x2E,   /* Failure: No EFS2 partition in table          */
  ERR_PARTI_NO_FOTA=0x2F,   /* Failure: No FOTA partition in table          */
  ERR_MIBIB_DAMAGED=0x30,   /* Failure: Damaged MIBIB found                 */
  ERR_NOT_ERASED=0x31,      /* Failure: Cannot write page not erased        */
  ERR_NO_BBT_BLOCK=0x32,    /* Failure: Cannot find bad block table         */
  ERR_BBT_MAGIC1_FAIL=0x33, /* Failure: incorrect Magic1 in bad block table */
  ERR_BBT_MAGIC2_FAIL=0x34, /* Failure: incorrect Magic2 in bad block table */
  ERR_USR_PARTI_NOMATCH=0x35,/* Failure: Usr Prtn. tbl miamatch, no override */
  ERR_PARTI_CNV_FAIL=0x36,  /* Failure: Partition table conversion failed   */
  ERR_BIB_VALID_FAIL=0x37,  /* Failure: MIBIB block validation failed       */
  ERR_PARTI_NO_BVT=0x38,    /* Failure: No Boot Vec. Tbl. partiton in table */
  ERR_NO_FLASH_PARTI=0x39,  /* Failure: Partition table not found on flash  */ 
  ERR_PARTI_NO_FOTAUI=0x3A, /* Failure: No FOTA UI Bitmap partition in table*/
  ERR_PARTI_NO_FOTAPKG=0x3B,/* Failure: No FOTA package partition in table  */
  ERR_PARTI_TOO_BIG=0x3C,   /* Failure: Partition Structure too large */
} fatal_code_type;

#define SET_ERR(x) jtag_flash_param.reserved=((uint32)(x))

#endif  /* __JFLASH_EXT_ERROR_H__ */

