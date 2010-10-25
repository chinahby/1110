#ifndef APWRX_H
#define APWRX_H

#include "target.h"
#if (defined( FEATURE_DH) && defined(FEATURE_DH_EXP))

/*===========================================================================

DESCRIPTION
  This file contains defines for integer size independence

Copyright (c) 1999-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/apwrx.h_v   1.0.2.0   30 Nov 2001 16:44:34   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/apwrx.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/00   ks      Added casts to LOW and MSG macros to get rid of compiler
                   warnings.
01/20/99   ck      Checked in the file.

===========================================================================*/

/*=========================================================================*/


#define WORD long

#define BITCT (8*sizeof(unsigned WORD))
#define LOW ((unsigned WORD)(((unsigned WORD)1L << (BITCT/2)) - 1))
#define MSB ((unsigned WORD)((unsigned WORD)1L << (BITCT-1)))

/* set Nmax equal to the maximum number of bits divided by BITCT */

#define MaxBits 512
#define Nmax ((MaxBits+BITCT-1)/BITCT)

int apwrxN(int n, unsigned WORD *a, unsigned WORD *x, unsigned WORD *ax,
       unsigned WORD *p);

void InitMont(unsigned WORD *m,   /* storage for -1/p */
        unsigned WORD *p);  /* modulus */

#endif /* FEATURE_DH and FEATURE_DH_EXP */
#endif /* APWRX_H */
