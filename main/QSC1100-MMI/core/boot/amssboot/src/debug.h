#ifndef DEBUG_H
#define DEBUG_H
/*===========================================================================

            " M S M 6 0 5 0 -  D E B U G "   H E A D E R   F I L E

DESCRIPTION
  DEBUG Configuration using MSM6050 ARM Processor.

  Copyright (c) 2003, 2004  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/debug.h_v   1.6   04 Jan 2002 10:11:46   ropalsky  $
$Header: //depot/asic/msm6800/drivers/boot/2.1/debug.h#1 $ $DateTime: 2005/06/16 11:45:28 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/04   AP      Ported for MSM6550 from MSM6500.
01/03/02    ro     Removed DEBUG_ETM_MODE.
11/30/01   dsb     Ported for MSM6050 from MSM5100.
===========================================================================*/

/*---------------------------------------------------------------------------
                            Debug Configurations
---------------------------------------------------------------------------*/
/*
** Enable JTAG or JTAG+ETM support
** NOTE: JTAG support can be disabled here to save power.
*/
#define DEBUG_ENABLE_DBG

/*
** If this feature is turned on, it prevents the CPU from sleeping and
** also overrides FEATURE_UP_CLOCK_SWITCHING.
**
** NOTE: This feature is no longer necessary (Trace32 SW fixed this long ago)
*/
#undef DEBUG_JTAG_DEBUG

#endif /*DEBUG_H*/
