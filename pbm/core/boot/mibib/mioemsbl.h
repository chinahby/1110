#ifndef __MIOEMSBL_H__
#define __MIOEMSBL_H__
/*===========================================================================

                MIBIB OEMSBL Page Definitions

DESCRIPTION
  This header file gives the external definition of the structures used
  to find the QC-SBL partitionn
  
  Copyright (c) 2004, 2007 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/mibib/MSM_MIBIB.02.00/mioemsbl.h#8 $ $DateTime: 2008/01/25 16:39:26 $ $Author: rtilak $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/07   rt      Updated Copyright statement; Made cosmetic changes
11/02/04   drh     Changed includes
09/06/04   drh     Created.

===========================================================================*/

#ifdef BUILD_BOOT_CHAIN
  #include "boot_comdef.h"
#else
  #include "comdef.h"
#endif

#include "miparti.h"




/*------------------------------------------------------------
 *
 *  OEMSBL Good Block Definitions
 *
 * ---------------------------------------------------------*/

/* Definitions of magic numbers used in the first page of each
 * of the OEM-SBL good blocks.  We must mark these blocks
 * with some identifier because the PBL does not have enough
 * knowledge to know how to find bad blocks.
 */
#define MI_OEMSBL_MAGIC1          0xFA0F129C
#define MI_OEMSBL_MAGIC2          0x5A8Fb6C9


/*------------------------------------------------------------
 *
 *  OEMSBL Pointer Page Definitions
 *
 * ---------------------------------------------------------*/

/* This structure is written to one page of the MIBIB and points
 * to the current OEM-SBL image, since we keep two copies.
 */
 
/* Magic numbers to identify structure */
#define MI_OEMSBL_CURRENT_PTR_MAGIC1          0xFA0F129C
#define MI_OEMSBL_CURRENT_PTR_MAGIC2          0x5A8Fb6C9


/* Must increment this version whenever structure of bad block table
 * changes.
 */
#define MI_OEMSBL_CURRENT_PTR_VERSION   0x1

struct mi_oemsbl_id;
typedef struct mi_oemsbl_id *mi_oemsbl_id_t;


/* Definition of OEM-SBL Pointer structure */
struct mi_oemsbl_id {
  uint32          magic1;      /* MAGIC1                      */
  uint32          magic2;      /* MAGIC2                      */
  uint32          version;     /* MI_OEMSBL version number    */
  char partition_name[FLASH_PART_NAME_LENGTH];
                               /* Partition name array        */
};

#endif /* __MIOEMSBL_H__ */
