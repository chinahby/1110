#ifndef __FLASH_MSM_H__
#define __FLASH_MSM_H__

/**********************************************************************
 * flash_msm.h
 *
 *
 * This file implements MSM specific macros common to all flash devices
 * functions.  Every MSM will have its own version customized for
 * that particular target.
 *
 * Copyright (c) 2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/flash/MSM_FLASH.01.04/flash_msm.h#3 $
  $DateTime: 2008/04/11 09:43:00 $
  $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-03-14   sv      Fix comment and indentation
2008-01-08   op      Initial version
===========================================================================*/
#include "flash_nor_msm.h"

#define FLASH_COMPILE_NOR_ONLY

#ifdef FLASH_NOR_PARTI_TBL
  #error  Not support NOR PARTITION.
#endif

/* Maximum number of flash clients */
#ifndef FLASH_MAX_CLIENTS
  #define FLASH_MAX_CLIENTS 1
#endif

/* Maximum size for a chip select area */
#define MAX_CHIP_SELECT_SIZE 0x7FFFFFF

/* Extern declarations */
extern uint32 flash_msm_get_device_type(void);

/****************************************************************
 * Global Data Prototypes
 ***************************************************************/
/* Prototypes and macros related to multithreading support.
 */
/* The following macros are only for a multithreaded environment. 
 * The FLASH_SINGLE_THREADED feature is checked to determine the same.
 */
#ifndef FLASH_SINGLE_THREADED
#ifdef QUARTZ_BSP
#include "KxMutex.h"

#define FLASH_OPEN_INIT_LOCK()\
  do {\
    INTLOCK();\
    if (!flash_open_lock_initialized) {\
      flash_open_cs = KxMutex_Alloc("KXMUTEX_FLASH_DRV_SHARED");\
      if (!flash_open_cs) { \
        FLASH_ERR_FATAL("KxMutex_Alloc Failed",0,0,0); \
      } \
      flash_open_lock_initialized = 1;\
    }\
    INTFREE();\
  } while (0)

#define FLASH_OPEN_LOCK()\
  do {\
    if (flash_open_lock_initialized)\
      KxMutex_Lock(flash_open_cs);\
  } while (0)

#define FLASH_OPEN_UNLOCK()\
  do {\
    if (flash_open_lock_initialized)\
      KxMutex_Unlock(flash_open_cs);\
  } while (0)
#else  /* !QUARTZ_BSP */
#include "rex.h"

#define FLASH_OPEN_INIT_LOCK()\
  do {\
    if (rex_is_initialized ()) {\
      INTLOCK();\
      if (!flash_open_lock_initialized) {\
        rex_init_crit_sect (&flash_open_cs);\
        flash_open_lock_initialized = 1;\
      }\
      INTFREE();\
    }\
  } while (0)

#define FLASH_OPEN_LOCK()\
  do {\
    if (flash_open_lock_initialized)\
      rex_enter_crit_sect (&flash_open_cs);\
  } while (0)

#define FLASH_OPEN_UNLOCK()\
  do {\
    if (flash_open_lock_initialized)\
      rex_leave_crit_sect (&flash_open_cs);\
  } while (0)
#endif /* QUARTZ_BSP */
#else /* FLASH_SINGLE_THREADED */
  #define FLASH_OPEN_INIT_LOCK()
  #define FLASH_OPEN_LOCK()
  #define FLASH_OPEN_UNLOCK()
#endif

#endif /* End of __FLASH_MSM_H__ */
