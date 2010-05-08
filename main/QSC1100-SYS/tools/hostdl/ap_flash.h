#ifndef AP_FLASH_H
#define AP_FLASH_H
/*===========================================================================

           FLASH PROGRAMMER PUBLIC INTERFACE HEADER FILE

DESCRIPTION
   This file contains the interface to all flash devices and is not
   flash type specific (NOR or NAND).  It is the high level "public"
   interface to the flash device driver layer.
      
  Copyright (c) 1997-2005, 2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_flash.h#2 $ $DateTime: 2008/01/28 17:30:17 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_flash.h#3 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
01/22/08   op      Pass partition table length to flash programmer
12/05/05   dp      NOR flash driver unification
06/16/05   dng     Added support for Spansion flash. 
11/25/04   drh     Support for multi-image boot
09/23/03   drh     Extensive rewrite to make drivers usable in a variety
                    of flash tools.
                    Previous history removed because it no longer applies.

============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------
#include "comdef.h"

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Type Declarations
//--------------------------------------------------------------------------

/* Enumerated type to see if device probed OK so we only probe once */
typedef enum
{
  FLASH_DEVICE_FOUND,         /* Successful probe             */
  FLASH_DEVICE_UNKNOWN,       /* Failed to probe successfully */
  FLASH_DEVICE_NOT_PROBED     /* Not yet probed               */
}
probe_code_type;


//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------
/* enum to indicate status of probe */
extern probe_code_type flash_dev_type;


//--------------------------------------------------------------------------
// Function Definitions
//--------------------------------------------------------------------------

/*===========================================================================

FUNCTION do_simple_write

DESCRIPTION
  This function writes a provided string of bytes into a specified
  block of memory.

  If the block is in RAM space, the only restriction is that the
  write is not permitted to overlay this program.

  If the block is in Flash space, the write will be attempted.  The
  updated memory will then be verified to confirm that the write
  operation succeeded.  It might fail because the area had not been
  erased, or because it has worn out, or because it is in the
  hardware-lockable boot block.

DEPENDENCIES
  Address checks assume that the maximum allowed block size
  (MAX_WRITE_SIZE) is smaller than the block of memory reserved
  for this program.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  This operation can take a bit of time, around 5ms per kilobyte.

===========================================================================*/

extern response_code_type do_simple_write (byte * buf,
/* Pointer to the string of bytes to write */
                                    dword addr,
/* physical address of the first byte of memory to write */
                                    word len
/* Count of bytes to write */
  );


/*===========================================================================

FUNCTION do_stream_write

DESCRIPTION
  This function writes a provided string of bytes into a specified
  block of memory and handles erase of any blocks as necessary
  before writing.


  If the block is in Flash space, the write will be attempted.  The
  updated memory will then be verified to confirm that the write
  operation succeeded.  It might fail because the area had not been
  erased, or because it has worn out, or because it is in the
  hardware-lockable boot block.

DEPENDENCIES
  Address checks assume that the maximum allowed block size
  (MAX_WRITE_SIZE) is smaller than the block of memory reserved
  for this program.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  This operation can take a bit of time, around 5ms per kilobyte.

===========================================================================*/

extern response_code_type do_stream_write (byte * buf,
/* Pointer to the string of bytes to write */
                                    dword addr,
/* physical address of the first byte of memory to write */
                                    word len
/* Count of bytes to write */
  );





/*===========================================================================

FUNCTION do_read

DESCRIPTION
  This function reads string of bytes into a specified
  block of memory.

DEPENDENCIES

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/

extern response_code_type do_read (byte * buf,
/* Pointer to the destination buffer */
                                    dword addr,
/* physical address of the first byte of memory to read */
                                    word len
/* Count of bytes to read */
  );

#ifdef ADD_LATER_FOR_COMMON_WITH_JFLASH
/*===========================================================================

FUNCTION do_erase

DESCRIPTION
  This function erases a specified block of flash memory.

  If the block is in RAM space, the only restriction is that the
  erased block is not permitted to overlay this program.  The RAM
  is cleared to 0.

  The memory address and block size must exactly match the address 
  and size of one of the blocks in the Flash memory device.  The Flash 
  memory is erased to a device-specific value, in this case 0xFFFF.

DEPENDENCIES
  Address checks assume that the maximum allowed block size
  (MAX_WRITE_SIZE) is smaller than the block allocated for this
  program.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  This operation can take quite a bit of time when erasing a block of
  Flash memory.  According to the data book, up to 17 seconds for one
  of the main blocks, and up to 8.6 seconds for the boot block
  or a parameter block.  Typical values are much quicker.

  The watchdog is reset.

===========================================================================*/

extern response_code_type do_erase (dword addr,
/* 20-bit linear physical address of the first byte of memory to erase */
                                    dword len
/* Count of bytes to erase */
  );
#endif


/*===========================================================================

FUNCTION flash_device_size

DESCRIPTION
  This function returns a code indicating how large the Flash device is.

DEPENDENCIES
  Uses the dev_type regional variable.

RETURN VALUE
  Byte indicating the size of the installed Flash device, according to
  the table in the Download Protocol spec

SIDE EFFECTS
  May call check_iid() to set dev_type.

===========================================================================*/

extern byte flash_device_size (void);


/*===========================================================================

FUNCTION flash_device_name

DESCRIPTION
  This function returns a pointer to a string containing the flash
  device name

DEPENDENCIES
  Uses the dev_type regional variable.

RETURN VALUE
  string pointer

SIDE EFFECTS
  May call check_iid() to set dev_type.

===========================================================================*/

extern char * flash_device_name (void);


/*===========================================================================

FUNCTION flash_device_sectors

DESCRIPTION
  This function fills the indicated handle_hello response buffer with the 
  number of sectors, and a length for each sector.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the number of bytes added to the indicated buffer

SIDE EFFECTS
  None

===========================================================================*/

extern int flash_device_sectors (uint8 *buf);



/*===========================================================================

FUNCTION flash_device_init_data

DESCRIPTION
  This function does one time initialization of data structures used in
  flash device access

DEPENDENCIES
  Must be called from main before any other flash access functions.

RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/

extern void flash_device_init_data (void);


/*===========================================================================

FUNCTION flash_device_probe

DESCRIPTION
  This function calls the flash device specific function to probe the
  flash

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

extern void flash_device_probe (void);


/*===========================================================================

FUNCTION do_open

DESCRIPTION
  This function is a stub for a function needed in the next revision of
  the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/

extern response_code_type do_open (byte mode);


/*===========================================================================

FUNCTION do_close

DESCRIPTION
  This function is a stub for a function needed in the next revision of
  the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/

extern response_code_type do_close (void);

/*===========================================================================

FUNCTION do_security_mode

DESCRIPTION
  This function Sends the security mode to the layer below
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/
extern response_code_type do_security_mode (byte mode);


/*===========================================================================

FUNCTION do_partition_table

DESCRIPTION
  This function Sends the partition table to the layer below
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/
extern response_code_type do_partition_table (byte *data, byte override,
		                              word len);


/*===========================================================================

FUNCTION do_open_multi

DESCRIPTION
  This function handles the open for all modes.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/
extern response_code_type do_open_multi (byte *data, byte mode, uint32 length);


#endif /* AP_FLASH_H */

