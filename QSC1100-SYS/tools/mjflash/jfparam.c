/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Flash Programming Support via Lauterbach JTAG ICD

GENERAL DESCRIPTION
  This file exist so that we can explicitly place our jtag_flash_param
  data in memory since ARM doesn't allow this within a file but rather by
  file basis.

EXTERNALIZED FUNCTIONS
  None. However, jtag_flash_param is global.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  jtag_flash_param will be initialized/used by Lauterbach TRACE32 debugger

  Copyright (c) 1998,1999 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/jfparam.c_v   1.1   24 Oct 2001 18:32:54   donb  $
  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jfparam.c#1 $ $DateTime: 2007/03/02 18:20:12 $ $Author: rtilak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/99   lss     Include jfi.h
12/02/98   lss     Created.
===========================================================================*/

#include "jfi.h"
#include "jf.h"

struct type_jtag_flash_param jtag_flash_param;
