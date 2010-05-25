#ifndef __FLASH_OS_H__
#define __FLASH_OS_H__
/**********************************************************************
 * flash_os.h
 *
 * OS interface.
 *
 * This file file includes OS interfaces APIs
 * Copyright (c) 2008-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_os.h#7 $ $DateTime: 2009/04/01 09:54:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-27   jmm     Fix prototype for flash_os_mem_free
2008-09-12   jmm     Use OALVAtoPA for WM7 BLDR
2008-08-01   mh      Add FLASH_OS_WM6_WINCE_BOOT_MODE and FLASH_OS_WM7_BOOT_MODE
2008-07-30   mh      Remove flash_msg.h
2008-07-17   mh      Remove rex.h for WM6 builds
2008-06-17   mh      Add GET_PHYS_ADDR
2008-05-30   mh      File created

===========================================================================*/
#ifdef FEATURE_WINCE 
#error code not present
#endif   /* #ifdef (FEATURE_WINCE)   */

#ifndef GET_PHYS_ADDR
  #define GET_PHYS_ADDR(addr)   ((uint32)addr)
#endif

#ifdef FLASH_USES_DM
  #include "flash.h"
#endif

#define FLASH_OS_MAX_CS  0x05

typedef enum flash_os_mode_e {
     FLASH_OS_NONE_MODE,
     FLASH_OS_REX_MODE,
     FLASH_OS_QUARTZ_MODE,
     FLASH_OS_WM6_WINCE_MODE,
     FLASH_OS_WM7_MODE,
     FLASH_OS_WM6_WINCE_BOOT_MODE,
     FLASH_OS_WM7_BOOT_MODE,
     FLASH_OS_MAX
}flash_os_mode_e_type;


 /****************************************************************
 * Global Data Prototypes
 ***************************************************************/

 /* Macros related to multithreading support.
  * Use OS critical section macros if not in single threaded enviroment 
  */
 #ifdef FLASH_SINGLE_THREADED

   #define FLASH_OS_CS_INIT_LOCK(cs)  
   #define FLASH_OS_CS_LOCK(cs)      
   #define FLASH_OS_CS_UNLOCK(cs)   

 #else

   #define FLASH_OS_CS_INIT_LOCK(cs)           flash_os_init_lock(cs)
   #define FLASH_OS_CS_LOCK(cs)                flash_os_lock(cs)
   #define FLASH_OS_CS_UNLOCK(cs)              flash_os_unlock(cs)

 #endif /* !defined(FLASH_SINGLE_THREADED) */


/*===========================================================================
 *  FUNCTIONS
===========================================================================*/

#ifdef FLASH_USES_DM
/*===========================================================================
DM transfer APIs
===========================================================================*/
int flash_dm_transfer_os_rex(uint32 * cmd_ptr, flash_cmd_e_type cmd_id);
int flash_dm_transfer_os_wm(uint32 * cmd_ptr, flash_cmd_e_type cmd_id);
int flash_dm_transfer_os_none(uint32 * cmd_ptr, flash_cmd_e_type cmd_id);

#endif

/*===========================================================================
FUNCTION flash_os_get_type

DESCRIPTION
    This API is used to report the OS type

DEPENDENCIES
  None

RETURN VALUE
  OS type.

SIDE EFFECTS
  None
===========================================================================*/
flash_os_mode_e_type flash_os_get_type(void);

/*===========================================================================
FUNCTION flash_os_init_lock

DESCRIPTION
  Initilaizes a critical section structure for the OS. 

DEPENDENCIES
  None

INPUT
  Address of the void pointer (masking as a pointer to flash_os_cs_type).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void flash_os_init_lock(void**  flash_os_cs);


/*===========================================================================
FUNCTION flash_os_lock

DESCRIPTION
  Enter critical section if critical section is initialized

DEPENDENCIES
  flash_os_init_lock must have been called and flash_os_lock_initialized set 

RETURN VALUE
  None .

SIDE EFFECTS
  None

===========================================================================*/
 void flash_os_lock(void* flash_os_cs);

/*===========================================================================
FUNCTION flash_os_unlock

DESCRIPTION
  Exit critical section if critical section is initialized

DEPENDENCIES
  flash_os_lock must have been called and flash_os_lock_initialized set 

RETURN VALUE
  None .

SIDE EFFECTS
  None

===========================================================================*/
 void flash_os_unlock(void*  flash_os_cs);

/*===========================================================================
FUNCTION flash_os_mem_malloc

DESCRIPTION
  Allocates a block of size bytes from the heap that matches the requested
  flags/attributes. 

DEPENDENCIES
  None

RETURN VALUE
  void* - A pointer to the newly allocated block, or NULL if the block
  could not be allocated. The pointer is 1k aligned.

SIDE EFFECTS
  None

===========================================================================*/
void * flash_os_mem_malloc (unsigned int size,  unsigned int id);


/*===========================================================================
FUNCTION flash_os_mem_free

DESCRIPTION
  Deallocates a block of memory and returns it to the right heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void flash_os_mem_free(void *ptr );

#endif /*__FLASH_OS_H__ */

