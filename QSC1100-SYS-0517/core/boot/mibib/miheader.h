#ifndef MIHEADER_H
#define MIHEADER_H

/*===========================================================================

                          MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/mibib/miheader.h#8 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/09   dk      Fixing the bootup issues seen due to memory overlap.
                   Shared segment got enabled and code size increased by 2k.
04/01/09   dk      MMU enabled in Boot stage and secure boot build increases the code size
                   and to avoid overlap issue, relocate the elf header address start.
02/04/08   PS      Initial revision

============================================================================*/

#include "miaddress.h"
#include "miimageheader.h"
#include "miaddress_generic.h"
#include "customer.h"
/* Error checking */ 

#ifndef SCL_DBL_CODE_BASE_ADDR
#error Need to define SCL_DBL_CODE_BASE_ADDR
#endif /* SCL_DBL_CODE_BASE_ADDR */

#ifdef SCL_FSBL_XIP
#ifndef SCL_FSBL_DATA_BASE_ADDR 
#error If FSBL is XIP, SCL_FSBL_DATA_BASE_ADDR needs to be defined
#endif /* SCL_FSBL_DATA_BASE_ADDR */
#ifdef SCL_FSBL_CODE_BASE_ADDR
#error If FSBL is XIP, SCL_FSBL_CODE_BASE_ADDR should not to be defined
#endif /* SCL_FSBL_CODE_BASE_ADDR */
#else /* SCL_FSBL_XIP */
#ifdef SCL_FSBL_DATA_BASE_ADDR 
#error If FSBL is not XIP, SCL_FSBL_DATA_BASE_ADDR should not be defined
#endif /* SCL_FSBL_DATA_BASE_ADDR */
#ifndef SCL_FSBL_CODE_BASE_ADDR
#error If FSBL is not XIP, SCL_FSBL_CODE_BASE_ADDR should be defined
#endif /* SCL_FSBL_CODE_BASE_ADDR */
#endif /* SCL_FSBL_XIP */

#ifdef SCL_OSBL_XIP
#ifndef SCL_OSBL_DATA_BASE_ADDR 
#error If OSBL is XIP, SCL_OSBL_DATA_BASE_ADDR needs to be defined
#endif /* SCL_OSBL_DATA_BASE_ADDR */
#ifdef SCL_OSBL_CODE_BASE_ADDR
#error If OSBL is XIP, SCL_OSBL_CODE_BASE_ADDR should not to be defined
#endif /* SCL_OSBL_CODE_BASE_ADDR */
#else /* SCL_OSBL_XIP */
#ifdef SCL_OSBL_DATA_BASE_ADDR 
#error If OSBL is not XIP, SCL_OSBL_DATA_BASE_ADDR should not be defined
#endif /* SCL_OSBL_DATA_BASE_ADDR */
#ifndef SCL_OSBL_CODE_BASE_ADDR
#error If OSBL is not XIP, SCL_OSBL_CODE_BASE_ADDR should be defined
#endif /* SCL_FSBL_CODE_BASE_ADDR */
#endif /* SCL_OSBL_XIP */

#ifndef SCL_OSBL_STACK_BASE_ADDR
#error Need to define SCL_OSBL_STACK_BASE_ADDR
#endif  /* SCL_OSBL_STACK_BASE_ADDR */

#ifndef SCL_AMSS_DATA_BASE_ADDR
#error Need to define SCL_AMSS_DATA_BASE_ADDR
#endif /* SCL_AMSS_DATA_BASE_ADDR */

/* End Error checking */


#define MI_NOR_DBL_IMG_SRC_ADDR 0x78
/* Dummy definitions. This would be overridden by definitions in miaddress.h 
   file 
 */
/* Definitions for FSBL */

#ifndef MI_NOR_FSBL_IMG_SRC_ADDR
#define MI_NOR_FSBL_IMG_SRC_ADDR     0xDEADBEEF
#endif
/*Definitions for OSBL*/
#ifndef MI_NOR_OSBL_IMG_SRC_ADDR
#define MI_NOR_OSBL_IMG_SRC_ADDR 0xDEADBEEF
#endif /* MI_NOR_OSBL_IMG_SRC_ADDR */

/* Definitions for DBL */

#define MI_NOR_DBL_HEADER_SRC_ADDR                0x0 
#define MI_NOR_DBL_IMG_DEST_ADDR                  SCL_DBL_CODE_BASE_ADDR


#ifdef SCL_FSBL_XIP
#define MI_NOR_FSBL_IMG_DEST_ADDR    MI_NOR_FSBL_IMG_SRC_ADDR 
#define MI_NOR_FSBL_IMG_DATA_ADDR    SCL_FSBL_DATA_BASE_ADDR
#else  /* SCL_FSBL_XIP */
#define MI_NOR_FSBL_IMG_DEST_ADDR    SCL_FSBL_CODE_BASE_ADDR 
#define MI_NOR_FSBL_IMG_DATA_ADDR    +0x0
#endif /* SCL_FSBL_XIP */

/* Definitions for OSBL */
#define MI_NOR_OSBL_HEADER_SRC_ADDR 0x50

#ifdef SCL_OSBL_XIP
#define MI_NOR_OSBL_IMG_DEST_ADDR    MI_NOR_OSBL_IMG_SRC_ADDR 
#define MI_NOR_OSBL_IMG_DATA_ADDR    SCL_OSBL_DATA_BASE_ADDR
#else  /* SCL_OSBL_XIP */
#define MI_NOR_OSBL_IMG_DEST_ADDR    SCL_OSBL_CODE_BASE_ADDR 
#define MI_NOR_OSBL_IMG_DATA_ADDR    +0x0
#endif /* SCL_OSBL_XIP */
#define MI_NOR_OSBL_IMG_STACK_ADDR   SCL_OSBL_STACK_BASE_ADDR

/* Definitions for AMSS */
#define MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR 0x22000
/* #define MI_NOR_AMSS_BOOT_PROGRAM_HEADER_SRC_ADDR MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR+MI_NOR_AMSS_BOOT_ELF_HEADER_MAX_SIZE */
#define MI_NOR_AMSS_BOOT_PROGRAM_HEADER_SRC_ADDR 0x24000
/* #define MI_NOR_AMSS_HASH_START_ADDR MI_NOR_AMSS_BOOT_PROGRAM_HEADER_SRC_ADDR+MI_NOR_AMSS_BOOT_PROGRAM_HEADER_LENGTH */
#define MI_NOR_AMSS_HASH_START_ADDR 0x25000
#define MI_NOR_AMSS_IMG_DATA_ADDR    SCL_AMSS_DATA_BASE_ADDR

/* Dummy . Get back to it later */
/* ARMPRG download location in SDRAM */
#ifndef SCL_HOSTDL_BASE
#error Need to define SCL_HOSTDL_BASE
#else
#define MI_NOR_HOSTDL_IMG_DEST_ADDR     SCL_HOSTDL_BASE /* SDRAM/PSRAM */
#endif 

#define MI_NOR_AMSS_HEADER_SRC_ADDR                  0xFFFFFFFF /* header is 40 bytes */
#define MI_NOR_AMSS_IMG_SRC_ADDR                     0x0
#define MI_NOR_AMSS_IMG_DEST_ADDR                    0x0

#define MI_NOR_BASE_ADDR                0x00000000UL
#define MI_NOR_IMG_SIZE                 0x02000000UL


/*===========================================================================
  This section is used by NAND/SDRAM.
  These are dummy addresses used by image_header.c
===========================================================================*/
#define MI_NAND_DBL_IMG_DEST_ADDR                    0xFFFFFFFF 
#define MI_NAND_FSBL_IMG_DEST_ADDR                   0xFFFFFFFF 
#define MI_NAND_OSBL_IMG_DEST_ADDR                   0xFFFFFFFF 
#define MI_NAND_OSBL_IMG_STACK_ADDR                  0xFFFFFFFF 
#define MI_NAND_AMSS_IMG_DEST_ADDR                   0xFFFFFFFF 
#define MI_NAND_HOSTDL_IMG_DEST_ADDR                 0xFFFFFFFF 


#endif  /* MIHEADER_H */

