/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       C L O C K   S E R V I C E S

               J U L I A N   T I M E   C O N V E R S I O N

GENERAL DESCRIPTION
  This module contains procedures to convert seconds to and from Julian
  calendar units.

  IMPORTANT NOTE: The time calculations in this module are only valid for
  dates and times from 6 Jan 1988, 00:00:00 to 28 Feb 2100, 23:59:59.  This
  is due to the fact that this module assumes that all years evenly divisible
  by 4 are leap years. Unfortunately, century years (e.g., 1900, 2000, 2100)
  must also be evenly divisible by 400 in order to be leap years, and so the
  year 2100 does not qualify as a leap year.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1994, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800/drivers/clk/clkjul.c#1 $ $DateTime: 2004/11/23 18:27:36 $ $Author: tkarra $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/03   ajn     Deleting - functionality moved to Time Service Subsystem
10/09/00   dsb     Mainlined T_ARM.
03/17/99    ms     Changes for Virtual Boot Block.
                     Removed bb_ prefixes
                     Not including bbjmpout.h, bbmisc.h.
                     Including misc.h
10/04/98   scdb    Merged ARM changes into the mainline
09/01/98   scdb    Added _cdecl to some functions to save ROM
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/29/96   dna     Moved extern of bb_div4x2 into its own .h file
02/28/96   dna     Prepended bb_ to function names to put this module in the
                   Boot Block.  Added function clk_ulmul to multiply unsigned
                   long integers without the use of a library function.
04/01/94   jah     Moved code here from clk.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Common Definitions and types              */
#include "target.h"     /* Target specific definitions               */



/***** Implementation moved to the Time Service Subsystem *****/

