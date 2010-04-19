#ifndef _CUSTFSBL_H
#define _CUSTFSBL_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                           H e a d e r  F i l e

DESCRIPTION
 Contains common defintion used by external modules.
 This file is not included in any FSBL code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/custfsbl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "fsbl.h"
#include BOOT_MSG_SUPPRESS_H

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
  Get access to SBL's functionality.
===========================================================================*/
//#define SBL_KICK_WATCHDOG  FSBL_KICK_WATCHDOG
//#define SBL_VERIFY         FSBL_VERIFY

/*===========================================================================
  Perform a busy wait by programming the MSM's pause timer for tools 
===========================================================================*/
#undef clk_busy_wait
//#undef clk_pause
extern void fsbl_hw_clk_busy_wait(uint32);
#define clk_busy_wait(us)    fsbl_hw_clk_busy_wait( (us) )
//#define clk_pause(us)        fsbl_hw_clk_busy_wait( (us) )

#endif /* _CUSTFSBL_H */
