#ifndef MSM_H
#define MSM_H
/*===========================================================================

               MSM5105 REGISTER ALIAS HEADER FILE

DESCRIPTION
  This file contains declarations and definitions to support the mapping
  of MSM registers in the MSM5105.

Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/msm.h_v   1.2   06 Nov 2001 09:55:48   dbrackma  $
$Header: //depot/asic/qsc1100/drivers/hw/msm.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/01   ejv     Changes for msm5100 to msm510x.
11/07/00   vlc     MSM5100 baseline based on MSM5105.  Added chip-specific file 
                   inclusions for MSM5100.
10/23/00   rmd     Moved MACRO definitions to msm5XXXio.h and register 
                   redefinitions to msm5XXXredefs.h.
10/17/00   rmd     Added msm5105bits.h which contains the bit definitions for
                   the msm5105.
09/06/00   rmd     Added _ARM_ASM_ around extern so that the compiler does not 
                   produce an error if this file is included into an assemble file.
08/31/00   jcw     Added msmhw.h include
08/25/00   rmd     Added support to MSM5105.Changed the shadow register code 
                   so that it supports 8 bit and 16 bit registers in one array
06/28/00   aks     Merged with PP3020.
05/01/00   ts      Added literals for the RUIM clock registers.  RUIM uses YAMN1.
03/30/00   aks     Added MSM3300 support, using msm33reg.h
01/14/00   mk      Deleted MSM_CLK_CTLx defines.
08/27/99   mk      Updated for MSM3100 REV2.
07/15/99   mk      Changed MSM_END for MSM3100, added new MSM_OUTHM macro
                   register indexes for MSM3100.
04/16/99   bgc     Added extra parenthesis in macros to fix lint warnings. 
                   Affected macros : MSM_END, MSM_MAX_INX, MSM_FMAX_INX

05/12/99   mk      Added initial MSM3100 support, needs msm31reg.h.
01/12/98   ak      Added #defines for second UART, which is used in the new
                   SIO.
                   version of the msm3reg.h file.
11/17/98   udp     Modified msm3rev2.h to be more generic and use msm3reg.h
10/26/98   jkl     Added support for trampm3.c
9/03/98     ss     AGC, NS and VR register definitions added.
7/18/98    udp     Use MSM3000 naming

===========================================================================*/            
            
#include "target.h"

#include "msmhw.h"        /* MSM-specific HW feature */
#include "msmbits.h"      /* Register bit definition */
#include "msmredefs.h"    /* Register redefinitions -- can and should be depricated */
#include "msmreg.h"       /* Register definitions     */
#include "msmio.h"        /* Register Input/Output -- still required for 6500 since RF VU exports old style MSMIO interfaces */
#include "msmhwio.h"      /* HWIO implementation */

#endif /* MSM_H */
