#ifndef _MIADDRESS_GENERIC_H_
#define _MIADDRESS_GENERIC_H_

/*===========================================================================

                            MULTI-IMAGE HEADER FILE

DESCRIPTION
  This header file contains declarations and type definitions for the
  Image Header Information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/mibib/miaddress_generic.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps      Added support for SECBOOT 2.0
10/01/06   anb     Created for 6260.
============================================================================*/

#define MI_MAGIC_NUM_NOR_QCSBLHD_CFGDATA                 0xEBB5735A

/*===========================================================================
  This section is used by NAND/SDRAM.
===========================================================================*/

/*===========================================================================
  The following section is used by NOR/(SDRAM/PSRAM)
===========================================================================*/
/* QCSBL in NOR */
#define MI_NOR_QCSBL_HEADER_OFFSET_FROM_START_ADDR       0x28
#define MI_NOR_QCSBL_IMG_SRC_OFFSET_FROM_START_ADDR      0x2000
/* OEMSBL in NOR */
#define MI_NOR_OEM_SBL_HEADER_OFFSET_FROM_START_ADDR     0x0
#define MI_NOR_OEM_SBL_IMG_SRC_OFFSET_FROM_START_ADDR    0x100
/* AMSS in NOR */
#define MI_NOR_AMSS_BOOT_ELF_HEADER_HASH_TBL_LENGTH      0x1000 /* NOR, CS0 */
#define MI_NOR_AMSS_BOOT_PROGRAM_HEADER_LENGTH           0x2000

/* DBL in NOR */
#define MI_NOR_DBL_HEADER_OFFSET_FROM_START_ADDR    0x0
#define MI_NOR_DBL_IMG_SRC_OFFSET_FROM_START_ADDR   0x50
/* FSBL in NOR */
#define MI_NOR_FSBL_IMG_SRC_OFFSET_FROM_START_ADDR  0x0
/* OSBL in NOR */
#define MI_NOR_OSBL_HEADER_OFFSET_FROM_START_ADDR   0x0
#define MI_NOR_OSBL_IMG_SRC_OFFSET_FROM_START_ADDR  0x40

#define MI_NOR_AMSS_BOOT_ELF_HEADER_MAX_SIZE 0x00001000UL
#endif /* _MIADDRESS_GENERIC_H_ */
