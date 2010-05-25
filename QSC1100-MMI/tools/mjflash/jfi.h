#ifndef JFI_H
#define JFI_H
/*===========================================================================

           F L A S H   I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This file contains definitions specific to the flash programming
  operation but not specific to any particular flash device.

  Copyright (c) 2006-2007 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/jfi.h_v   1.2   20 Dec 2001 13:39:40   dionh  $
  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jfi.h#3 $ $DateTime: 2007/10/01 18:50:43 $ $Author: opham $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/07   op      rename BUILD_FOR_INTEL to BUILD_FOR_X86
02/28/07   rt      Add support for multi-image NOR targets
12/06/06   rt      Create initial version from JFLASH 2.1

===========================================================================*/

#ifdef __CC_ARM
  #define BUILD_FOR_ARM
#else
  #define BUILD_FOR_X86
#endif

#include "comdef.h"
#include "jf.h"
#include "jf_msm.h"
#include "flash_nor_device.h"
#include "jflash_msg.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

/* Defines used in probe table iteration and probe return values */
#define FLASH_NO_DEVICE ((flashi_nor_device *) 0)


/*--------------------------------------------------------------------------
** Type Declarations
**-------------------------------------------------------------------------- */

/* Enumerated type to see if device probed OK so we only probe once */
typedef enum
{
  FLASH_DEVICE_FOUND,         /* Successful probe             */
  FLASH_DEVICE_UNKNOWN,       /* Failed to probe successfully */
  FLASH_DEVICE_NOT_PROBED     /* Not yet probed               */
}
probe_code_type;


/* Return type for checks of the Intelligent ID */
typedef  enum
{
  IID_OK,     /* Intelligent ID checked successfully. */
  IID_FAILED  /* Failure of any kind while checking the IID. */
}
iid_check_type;

/* Externals used to save ID table so linker will not throw it away */
extern void save_idtable(void);

extern probe_code_type flash_dev_type;


#endif /* JFI_H */

