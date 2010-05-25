/*===========================================================================
FILE:  estk.c


GENERAL DESCRIPTION:ENHANCED STK layer 
  This file provides the functions required to support the Enhanced STK 
  functionality

          Copyright © 2009 QUALCOMM Incorporated.
          All Rights Reserved.
          QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/estk/rel/07H1_2/src/estk.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   xz      1) Fix Compilation Warnings
                   2) Update extra TP bits if there is no UI
03/02/09   gg      Using correct defines for ESTK registration with GSTK
11/03/08   sk      Featurized ESTK based on external interface support
09/29/08   xz      Removed "static" for estk_gstk_init_cb()
09/16/08   sk      Added lint fixes
09/08/08   tml     Fixed auto header version generation and add Send SS 
                   support
06/09/08   sk      Added support for terminal profile download and Send SMS 

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"

#if defined(FEATURE_ESTK)
#error code not present
#endif /* FEATURE_ESTK */

