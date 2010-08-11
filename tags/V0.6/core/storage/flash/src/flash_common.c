
/**********************************************************************
 * flash_common.c
 *
 * Common function across NOR and NAND
 *
 * This file implements common functionality between NOR and NAND devices. 
 * 
 * Copyright (c) 2007 - 2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_common.c#16 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-11-13   sv      Remove ERR_FATAL and set proper result code in flash_open
2009-07-08   mm      Set proper result code in flash_open.
2009-05-18   jz      Fixed compiler warnings when compiled with GCC
2008-09-10   jz      Added CS lock/unlock for device_probe() for the case
                     that the underlying CS protection is turned off (WM case)
2008-08-21   jz      Cleaned up code and fixed CR153832:
                     - Fixed issue in flash_release_device_structure()
                     - Moved flash_open_cs locking/unlocking to flash_xxx_device_structure()
                     - Moved flash_client_count range checking to proper place
2008-06-17   mh      Fix bootup failure and compiler warning/lint error
2008-05-30   mh      Add support for OS abstraction layer
2008-05-20   mm      Return failure if a partition is not found.
2008-04-18   sv      Fix klocwork error in flash_open function  
2008-03-16   sv      Add support for 0:ALL partition in non-partitioned mode 
2008-01-08   op      Featurize for compilation with NOR-only driver files
2007-11-21   rt      Clean up and rename features
2007-10-30   mh      Add WM feature flags to compile out code not used by WM
2007-07-18   mm      Updated to support multiple clients
                     - Increasing the client structure array
                     - Updated names of device types
                     - Support for clients (boot_loaders) not compiling NOR

2007-06-24   mm      KxMutex shared update
2007-02-27   mm      Initial Version

===========================================================================*/
#include "flash_msg.h"
#include "flash_msm.h"
#include "flash_os.h"

#ifndef FLASH_COMPILE_NOR_ONLY
  #include "flash_nand.h"
#endif
#ifndef FLASH_COMPILE_NAND_ONLY
  #include "flash_nor.h"
#endif
#include "mibib.h"
#include <string.h>

/****************************************************************
 * The client device structure, the client structure array and 
 * current client count 
 ***************************************************************/
struct flash_client_device_data
{ 
  struct fsi_device_data flash_device_structure;
  boolean         available;
};

static struct flash_client_device_data flash_device_clients[FLASH_MAX_CLIENTS];
static uint32 flash_client_count = 0;

/* Indicates whether the flash client structure array is initialized */
static boolean flash_device_clients_initialized = FALSE;

/****************************************************************
 * Flag for manipulating the device driver lock
 ***************************************************************/
static void *flash_open_cs;


/*===========================================================================
FUNCTION FLASH_INITIALIZE_DEVICE_STRUCTURE

DESCRIPTION
  Initialize the flash device structure.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None
===========================================================================*/
static void flash_initialize_device_structure(void)
{
  int i=0;

  /* Fix compiler warning */
  (void)flash_open_cs;
  
  FLASH_OS_CS_INIT_LOCK(&flash_open_cs);
  FLASH_OS_CS_LOCK(flash_open_cs);
  
  /* Initialization is only done on the first call */
  if (flash_device_clients_initialized == FALSE)
  {
    for (i = 0; i < FLASH_MAX_CLIENTS; i++)
    {
      flash_device_clients[i].available = TRUE;
    }
    flash_device_clients_initialized = TRUE;
  }

  FLASH_OS_CS_UNLOCK(flash_open_cs);
}

/*===========================================================================
FUNCTION FLASH_ALLOCATE_DEVICE_STRUCTURE

DESCRIPTION
  Allocate the flash device structure.
  
DEPENDENCIES
  None.

RETURN VALUE
  Available device structure. NULL if none is available.
  
SIDE EFFECTS
  None
===========================================================================*/
static fsi_device_t flash_allocate_device_structure(void)
{
  int i = 0;
  fsi_device_t available_client = NULL;

  FLASH_OS_CS_LOCK(flash_open_cs);

  /* Check to see if a free slot is available and fetch a
   * free device descriptor from the descriptor list if available.
  */
  if (flash_client_count < FLASH_MAX_CLIENTS)
  {
    for (i = 0; i < FLASH_MAX_CLIENTS; i++)
    {
      if (flash_device_clients[i].available == TRUE)
      {
        flash_device_clients[i].available = FALSE;
        available_client = &flash_device_clients[i].flash_device_structure;
        flash_client_count++;
        break;
      }
    }
  }

  FLASH_OS_CS_UNLOCK(flash_open_cs);
  return available_client;
}

/*===========================================================================
FUNCTION FLASH_RELEASE_DEVICE_STRUCTURE

DESCRIPTION
  Release the device structure.

DEPENDENCIES
  None.

RETURN VALUE
  Frees if the device structure exists.

SIDE EFFECTS
  None
===========================================================================*/
static void flash_release_device_structure(fsi_device_t device_desc)
{
  int i = 0;
  
  FLASH_OS_CS_LOCK(flash_open_cs);
  
  /* Search and free the desired device descriptor in the desciptor list
   * if atleast one client of flash is still there.
   */
  if (flash_client_count > 0)
  {
    for (i = 0; i < FLASH_MAX_CLIENTS; i++)
    {
      if (&flash_device_clients[i].flash_device_structure == device_desc &&
          flash_device_clients[i].available == FALSE)
      {
        flash_device_clients[i].available = TRUE;
        flash_client_count--;
        break;
      }
    }  
  }

  FLASH_OS_CS_UNLOCK(flash_open_cs);
  return;
}

/*===========================================================================
FUNCTION FLASH_OPEN

DESCRIPTION
  Probe for the flash and open the requested partition if a partition table
  exists.

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  Device structure is returned. In case of error, the return value will be
  NULL.

SIDE EFFECTS
  None
===========================================================================*/
fs_device_t flash_open(char *partition_name, int whichflash)
{
  fsi_device_t fsi_device = NULL;
  fs_device_t fs_device = NULL;
  int result = FS_DEVICE_OK;
  uint32 flash_type = FLASH_NOR;

  if (partition_name != NULL)
  {      
    /* Initialize the client device structure array */
    flash_initialize_device_structure();

    /* Find the free slot availble */
    fsi_device = flash_allocate_device_structure();
  }
  else
  {
    result = FS_DEVICE_INVALID_PARTITION_NAME;
  }    
  /* Check the allocated device structure and continue only if
   * it is valid
   */
  if (fsi_device != NULL)
  {
    /* BOOT clients do not have support for NOR and the NOR calls need to be
     * featurised 
     */

    /* Type of device to probe */
    flash_type = flash_msm_get_device_type();

    /*
     * Probe will fill values in structure if it succeeds.
     * Currently, which_flash parameter is hard coded to 0, but in
     * future this will be dynamic.
     */
    switch (flash_type)
    {
      #if !defined(FLASH_COMPILE_NOR_ONLY)
      case FLASH_NAND:
      {
        FLASH_OS_CS_LOCK(flash_open_cs);
        result = flash_nand_device_probe (fsi_device, whichflash);
        FLASH_OS_CS_UNLOCK(flash_open_cs);
        break;
      }
      #endif

      #if !defined(FLASH_COMPILE_NAND_ONLY) 
      case FLASH_NOR:
      {
        FLASH_OS_CS_LOCK(flash_open_cs);
        result = flash_nor_device_probe (fsi_device, whichflash);
        FLASH_OS_CS_UNLOCK(flash_open_cs);
        break;
      }
      #endif

      default:  
      {
        result = FS_DEVICE_FAIL;
        flash_release_device_structure(fsi_device);
      }
    }  

    if (result == FS_DEVICE_OK)
    {
      fs_device = (fs_device_t)fsi_device;

    #if !defined(FLASH_NOR_PARTI_TBL) && !defined(FLASH_COMPILE_NAND_ONLY)
      if(flash_type == FLASH_NOR)
      {
        uint32 base_addr = (uint32)(fsi_device->nor_dev_info.base_addr);
        flash_nor_validate_structure(fsi_device, 
          (flash_ptr_type)((base_addr & ~(MAX_CHIP_SELECT_SIZE))));  
      }
    #endif

      /* Open the specified partition */
      if (fs_device->open_partition != 0)
      {
        result = fs_device->open_partition (fs_device, partition_name);
        if(result != FS_DEVICE_OK )
        {
          /* The partition is not found */
          result = FS_DEVICE_PARTITION_NOT_FOUND;
        }
      }
      else
      {
        if (flash_type == FLASH_NOR)
        {
          /* When NOR driver is used in non-partitioned mode tools or FS other 
           * than EFS need to open the complete device. Driver by default only 
           * opens up the file system partition. This code provides notion of 
           * partitions in non-partitioned mode.
           */
          if (strncmp (partition_name, MIBIB_DEVICE_ALL_PARTI_NAME,
                       FLASH_PART_NAME_LENGTH) == 0) 
          {
            /* Update the partition data for 0:ALL partition to represent the 
             * entire flash device.   
             */
            fsi_device->partition_start_baddr = 0;
            fsi_device->partition_limit_baddr = \
              1 << fsi_device->nor_dev_info.geometry->device_size;
            fsi_device->partition_blk_start = 0;
            fsi_device->partition_blk_limit = \
            fsi_device->nor_dev_info.geometry->num_total_blocks;
          }
          else if (strncmp (partition_name, MIBIB_EFS2_MODEM_PARTI_NAME,
                   FLASH_PART_NAME_LENGTH) != 0) 
          {
            /* For any partition other than 0:ALL and 0:EFS2 return partition not found */
            result = FS_DEVICE_PARTITION_NOT_FOUND;
          }
        }
        else
        {
          /* Should not come here as NAND always works in partition mode */
          result = FS_DEVICE_NO_PARTITION_TABLE;
        } /* if (flash_type == FLASH_NOR)  */

      } /* if (fs_device->open_partition != 0) */

    } /* if (result == FS_DEVICE_OK) */

    if (result != FS_DEVICE_OK) 
    {
      fs_device = FS_NO_DEVICE;
      flash_release_device_structure(fsi_device);
    }
  } /* if (fsi_device != NULL) */

  return fs_device;

}

/*===========================================================================
FUNCTION FLASH_CLOSE

DESCRIPTION
  Close the flash and free the associated device structure

DEPENDENCIES
   Should always follow a flash_open

RETURN VALUE
  FS_DEVICE_OK             - Success
  FS_DEVICE_FAILED         - An error occurred.

SIDE EFFECTS
  None
===========================================================================*/
int flash_close(fs_device_t flash_device)
{
  (void)flash_device;

  flash_release_device_structure((fsi_device_t)flash_device);

  return (int)FS_DEVICE_OK;
}
