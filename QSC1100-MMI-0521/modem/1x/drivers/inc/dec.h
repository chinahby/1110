#ifndef DEC_H
#define DEC_H

#include "dec5000.h"      /* This file is replaced for MSM5000 */

/*===========================================================================

           D E C O D E R   ( S V D )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Serial Viterbi Decoder chip

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 1990, 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1996, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MSM5100/vcs/dec.h_v   1.0   03 Nov 2000 15:02:02   vlechan  $
  $Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/dec.h#1 $ $DateTime: 2009/02/13 14:29:02 $ $Author: joshuam $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/00   rmd     Removed all pre MSM5000 stuff.
01/17/00   rm      Cleaned up TEMP_HACKS_FOR_TIGER.
9/22/99    lcc     Merged in MSM5000 support from PLT archive:
           jcw     Added MSM5000 redirection to dec5000.h
Imaginary Buffer Line
04/15/99   aaj     Function prototype for dec_fcc_intr() function for MDR
02/22/99   aaj     Cosmetic changes in dec_read_tc_data to avoid cplr warns
02/08/99   aaj     Declared function to clear sup metric for standby time improv.
01/15/99   aaj     Merged MDR branch to Panther mainline
11/20/98   aaj     Added support for Medium (High) data rate sup channels
11/05/98   dlb     Added support for MSM3000.
03/02/95   jah     Added support for 14400 rate set and MSM2p decoder mods.
05/28/93   jah     Added prototype for dec_state_zero_ctl().  Removed use
                   of FPOS() macro in DEC_TC_BUF_* definitions, to appease
                   the compiler.
05/11/93   jah     Added dec_chipx8_clk_enable().
02/06/92   jah     Ported module from brassboard dec.h

===========================================================================*/
#endif  /* DEC_H */
