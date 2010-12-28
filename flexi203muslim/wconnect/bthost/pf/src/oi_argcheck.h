#ifndef _ARGCHECK_H
#define _ARGCHECK_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_argcheck.h#1 $ 

$Log: $

===========================================================================*/
/**
@file

This file provides a macro for validating function arguments.
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_assert.h"
#include "oi_status.h"

/**
 * The ARGCHECK macro is to be used for checking function arguments in
 * public interfaces. It takes a conditional as an argument and
 * succeeds if the conditional is TRUE. If the conditional is FALSE,
 * an assertion is perfomed, and OI_STATUS_INVALID_PARAMETERS is returned.
 * Assertions can be disabled by not defining OI_DEBUG.
 *
 * If OI_SUPPRESS_ARGCHECK is defined, no argument checking is performed.
 * This allows us to use strong argument checking when the caller may
 * not be known (e.g., under Windows) and use assertions for debugging,
 * but leave the code out of the release build, when the stack and
 * application are tightly coupled (e.g., a deeply embedded system).
 *
 * Note that the documentation for BLUEmagic 3.0 software assumes that
 * OI_SUPPRESS_ARGCHECK is NOT defined. To leave argument checking code
 * out of build, pass -DOI_SUPPRESS_ARGCHECK on your compiler command line.
 */

#ifndef OI_SUPPRESS_ARGCHECK
#define ARGCHECK(x) {OI_ASSERT((x)); if (!(x)) return OI_STATUS_INVALID_PARAMETERS; }
#else
#define ARGCHECK(x)
#endif

#endif /* _ARGCHECK_H */
