#ifndef __MIBIB_H__
#define __MIBIB_H__
/*===========================================================================

                Multi-Image Boot Information Block Definitions

DESCRIPTION
  This header file gives the external definition of the Boot Information
  Block, located in page 0 of the fourth and fifth non-bad blocks in NAND 
  flash for Multi-Image Boot targets.
  
  There are two copies of the data in these blocks which allow us to 
  alternate between them and always have a good copy which points to 
  valid boot loader, even if a programming operation is accidentally
  interrupted.
  
  NOTE:  Even though this block will contain various headers, each one
         of them is in its own page, so there is not a single structure
         which would encompass them all.  As each page is read in, the
         appropriate structure pointer should be applied, based on the
         structure being accessed.

  Copyright (c) 2004-2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/mibib/MSM_MIBIB.02.00/mibib.h#14 $ $DateTime: 2008/02/22 18:06:14 $ $Author: opham $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/08   op      Added macros for "0:DBL" partition names
01/08/08   op      Added macros for DSP partition names
10/21/07   rt      Add support for Secure Boot 2.0
09/22/07   op      Add support for "0:ALL" partition
02/28/07   rt      Add support for NOR partitioning
11/16/06   rt      Added macro for FTL partition name
05/12/06   rt      Bump the version number
10/27/05   rt      Added CRC page in MIBIB
10/26/05   rt      Added user partition table page in MIBIB
08/23/05   drh     Add define SIM Secure partition name
03/30/05   amw     Added additional image type defines
03/15/05   drh     Add new limit to search for MIBIB length.
11/02/04   drh     Add conditional include
08/14/04   drh     Created.

===========================================================================*/


#ifdef BUILD_BOOT_CHAIN
  #include "boot_comdef.h"
#else
  #include "comdef.h"
#endif

/*------------------------------------------------------------
 *
 *  Multi-Image Boot Block Information Definitions
 *
 * ---------------------------------------------------------*/
 
/* Magic numbers used in identifying valid MIBIB */
#define MIBIB_MAGIC1     0xFE569FAC
#define MIBIB_MAGIC2     0xCD7F127A

/* Must increment this version whenever structure of bad block table
 * changes.
 */
#ifdef FEATURE_SECBOOT_2_0
  /* SB 2.0 is supported by MIBIB version 4 structures. If target is using
   * SB 2.0 use version '4' MIBIB.
   */ 
  #define MIBIB_VERSION   0x4
#else
  #define MIBIB_VERSION   0x3
#endif

/* Before we have a partition table, use this limit */
#define MIBIB_DEFAULT_PARTI_SIZE  0x10

/* Multi-Image Boot Information block header structure */

struct mi_boot_info_block;
typedef struct mi_boot_info_block *mi_boot_info_t;

struct mi_boot_info_block {

  /* MIBIB magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* MIBIB age field
   *   NOTE:
   *   When comparing one age to another age, since this is a 32-bit
   *   number, it will be impossible to wrap, given the number
   *   of times we would have to increment the number.  A simple
   *   greater than or less than comparison will be sufficient.  No
   *   need to check for wrap around.
   */
  uint32 age;
};


/* Definitions for which pages contain which headers */

#if (MIBIB_VERSION == 4)
  #define MIBIB_PAGE_MAGIC_VERSION_PAGE    0x0
  #define MIBIB_PAGE_PARTITION_TABLE       0x1
  #define MIBIB_PAGE_USR_PARTI_TBL         0x2
  #define MIBIB_PAGE_MIBIB_BLK_CRC         0x3
#elif (MIBIB_VERSION == 3)
  #define MIBIB_PAGE_MAGIC_VERSION_PAGE    0x0
  #define MIBIB_PAGE_PARTITION_TABLE       0x1
  #define MIBIB_PAGE_OEMSBL_CURRENT        0x2
  #define MIBIB_PAGE_OEMSBL_HEADER         0x3
  #define MIBIB_PAGE_AMSS_HEADER           0x4
  #define MIBIB_PAGE_APPSBL_HEADER         0x5
  #define MIBIB_PAGE_APPS_HEADER           0x6
  #define MIBIB_PAGE_PROGRESSIVE_BOOT      0x7
  #define MIBIB_PAGE_APPS_PROGRESSIVE_BOOT 0x8
  #define MIBIB_PAGE_USR_PARTI_TBL         0x9
  #define MIBIB_PAGE_MIBIB_BLK_CRC         0xA
#else
  #error Incompatible MIBIB version number
#endif


/* Define the last page number in the MIBIB.  This is used when
 * copying forward the information from the old MIBIB to the
 * new MIBIB.  
 *
 * WARNING:  If you add a new page to the MIBIB, you must
 *           update this define to use the last defined page.
 */
#define MIBIB_PAGE_LAST_PAGE  (MIBIB_PAGE_MIBIB_BLK_CRC + 1)


/* Definition for the longest span of blocks we will scan through
 * looking for a valid MIBIB.  This is way overkill, but will allow
 * for a large number of bad blocks to be encountered.  We determine
 * this fudge factor in the following way.  An 4096 block flash is allowed
 * to have about 2% bad blocks.  They can all cluster in the MIBIB
 * area.  If so, then we need to search through at least 82 blocks
 * plus the number of good blocks we are requiring in the MIBIB area.
 * That number is 16 blocks.  Adding together, we get 98, so for
 * good measure, we will use a round 96 (0x60) blocks.  This will have a
 * speed impact when the flash is completely erased, but when there
 * are actually MIBIB images in the flash we will stop looking when
 * we find the MIBIB.
 */
#define MIBIB_BLOCK_SEARCH_MAX          0x40


/* Defines for Partition names.  Used both in partition table
 * C file and in JNAND code.  These must not be changed without
 * close investigation of the code.
 */
#define MIBIB_PARTI_NAME               "0:MIBIB"
#define MIBIB_SIMSECURE_PARTI_NAME     "0:SIM_SECURE"
#define MIBIB_QCSBL_PARTI_NAME         "0:QCSBL"
#define MIBIB_OEMSBL1_PARTI_NAME       "0:OEMSBL1"
#define MIBIB_OEMSBL2_PARTI_NAME       "0:OEMSBL2"
#define MIBIB_AMSS_PARTI_NAME          "0:AMSS"
#define MIBIB_APPS_PARTI_NAME          "0:APPS"
#define MIBIB_APPSBL_PARTI_NAME        "0:APPSBL"
#define MIBIB_FOTA_PARTI_NAME          "0:FOTA"
#define MIBIB_EFS2_MODEM_PARTI_NAME    "0:EFS2"
#define MIBIB_EFS2_APPS_PARTI_NAME     "0:EFS2APPS"
#define MIBIB_FTL_APPS_PARTI_NAME      "0:FTL"
#define MIBIB_DSP1_PARTI_NAME          "0:DSP1"
#define MIBIB_DSP2_PARTI_NAME          "0:DSP2"

/* This is a special partiton name used by flash driver to represent
 * complete device. Note that this partition name is reserved for system
 * use and cannot be used to name any other user defined partition.
 */  
#define MIBIB_DEVICE_ALL_PARTI_NAME     "0:ALL"

/* Partition names used by NOR multi-image boot architecture */
#define MIBIB_NOR_BOOT_PARTI_NAME       "0:BOOTVECT"
#define MIBIB_NOR_PARTI_INFO_PARTI_NAME "0:MIPIB"
#define MIBIB_OEMSBL_PARTI_NAME         "0:OEMSBL"

/* Partition names used by Secure Boot architecture 2.0 */
#define MIBIB_DBL_PARTI_NAME            "0:DBL"
#define MIBIB_FSBL_PARTI_NAME           "0:FSBL"
#define MIBIB_OSBL_PARTI_NAME           "0:OSBL"

#endif /* __MIBIB_H__ */
