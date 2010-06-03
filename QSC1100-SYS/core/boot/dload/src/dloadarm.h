#ifndef DLOADARM_H
#define DLOADARM_H
/*===========================================================================

          ARM Boot Downloader Header File

DESCRIPTION
  This file contains global declarations and external references
  for the boot block downloader facility specific to the ARM.

Copyright (c) 1990,1991,1992 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1993,1994,1995 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1996,1997,1998 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1999,2002      by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloadarm.h_v   1.2   13 Mar 2002 12:58:06   rprenove  $
$Header: //depot/asic/msmshared/services/dload/1_0/dloadarm.h#3 $ $DateTime: 2005/10/26 07:42:08 $ $Author: tgold $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/05   tbg     Added FEATURE_DLOAD_MEM_DEBUG
11/12/02   rp      Removed dload_usb_init().
                   Moved dload_uart_init(), and all uart_xxxx() declarations
                   to dloaduart.h.
03/06/02   rp      Added dload_usb_init() under FEATURE_USB.
08/11/99   kk      Merge from SS baseline.
03/17/99   ms      Renamed bb_uart_init to dload_uart_init
                   due to Virtual Boot Block changes.
12/15/98   jc      Merge from et0400
11/06/98   jtg     Added PACKED to enum for ARM port into mainline.
09/13/98   hcg     Removed some #defines - Moved to another file.
07/30/98   hcg     Implemented security feature.
07/26/98   hcg     Revised for coding standard, removed unused code
06/01/98   hcg     Ported from Flashprg by ptw

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Status Code Enumeration
   This lists the status result codes passed around in the program.

   This enum is used to index a table of response packets, so these
   values map exactly to possible responses. */

typedef enum
  {
  ACK,                    /* Success. Send an acknowledgement.           */
  NAK_INVALID_FCS,        /* Failure: invalid frame check sequence.      */
  NAK_INVALID_DEST,       /* Failure: destination address is invalid.    */
  NAK_INVALID_LEN,        /* Failure: operation length is invalid.       */
  NAK_EARLY_END,          /* Failure: packet was too short for this cmd. */
  NAK_TOO_LARGE,          /* Failure: packet was too long for my buffer. */
  NAK_INVALID_CMD,        /* Failure: packet command code was unknown.   */
  NAK_FAILED,             /* Failure: operation did not succeed.         */
  NAK_WRONG_IID,          /* Failure: intelligent ID code was wrong.     */
  NAK_BAD_VPP,            /* Failure: programming voltage out of spec    */
  NAK_OP_NOT_PERMITTED,   /* Failure: memory dump not permitted          */
#ifdef FEATURE_DLOAD_MEM_DEBUG
  NAK_INVALID_ADDR,       /* Failure: invalid address for a memory read  */
#endif
  NAK_VERIFY_FAILED,      /* Failure: readback verify did not match      */
  NAK_NO_SEC_CODE,        /* Failure: not permitted without unlock       */
  NAK_BAD_SEC_CODE        /* Failure: invalid security code              */
  } response_code_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern void boot_error(int error);
extern void boot_hw_powerdown(void);
extern void boot_powerdown_entry(void);

/*===========================================================================

FUNCTION process_packets

DESCRIPTION
  This function is the main loop implementing the DMSS Async Download
  Protocol.  It loops forever, processing packets as they arrive.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this function.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_packets(void);

/*===========================================================================

FUNCTION do_write

DESCRIPTION
  This function writes a provided string of bytes into a specified
  block of RAM memory.

DEPENDENCIES
  Address checks assume that the maximum allowed block size
  (MAX_WRITE_SIZE) is smaller than the block of memory reserved
  for this program.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None.

===========================================================================*/

extern response_code_type do_write
(
  byte  *buf,
    /* Pointer to the string of bytes to write */

  dword addr,
    /* 20-bit linear physical address of the first byte of memory to write */

  word  len
    /* Count of bytes to write */
);

/*===========================================================================

FUNCTION do_erase

DESCRIPTION
  This function erases a specified block of RAM memory.

DEPENDENCIES
  Address checks assume that the maximum allowed block size
  (MAX_WRITE_SIZE) is smaller than the block allocated for this
  program.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None.

===========================================================================*/
extern response_code_type do_erase
(
  dword addr,
    /* 20-bit linear physical address of the first byte of memory to erase */

  dword len
    /* Count of bytes to erase */
);

#endif /* DLOADARM_H */
