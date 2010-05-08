#ifndef TLMM_H
#define TLMM_H
/*===========================================================================

          T O P  L E V E L  M O D E  M U X

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the TLMM functions.

REFERENCES

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/tlmm.h_v   1.3   30 Nov 2001 10:52:54   dbrackma  $
$Header: //depot/asic/qsc1100/drivers/tlmm/tlmm.h#1 $ $DateTime: 2007/10/15 16:59:03 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/04   ap       Added CPLD_MSM_H.
02/19/03   ems      Created.

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include TLMM_MSM_H     /* MSM specific header file */

#ifdef CPLD_MSM_H
#include CPLD_MSM_H
#endif

#ifndef _ARM_ASM_
typedef enum
{
    TLMM_USB,
    TLMM_UART2,

    TLMM_NUM_CONFIGS
} tlmm_type;


extern void tlmm_init ( void );
extern void tlmm_write ( tlmm_type feature, boolean onoff);
#endif

#endif /* TLMM_H */
