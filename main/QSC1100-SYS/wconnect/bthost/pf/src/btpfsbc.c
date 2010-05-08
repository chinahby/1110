/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H   B I T S T R E A M   I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the Bluetooth bitstream implementation.

Copyright (c) 2005 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

*
*   #1       24 Mar 2005          JTL
* Initial revision.
*
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_AV

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"

/* Definitions for the SBC Encoder */
#define SBC_ENCODER
#undef SBC_DECODER
#undef SBC_ENHANCED
#define SBC_8SUBBANDS
#undef SBC_4SUBBANDS
/* These flags are passed as compiler flags in the original
 * implementation. */
#define PRIVATEDATADECL
#define PRIVATEDATADEF

/* Open Interface Source files */
#ifdef MAX
#undef MAX
#endif /* MAX */
#ifdef MIN
#undef MIN
#endif /* MIN */
#include "oi_bitstream.c"
#include "bitalloc.c"

#include "quant.c"
#include "encoder.c"
#include "tables-generated.c"
#include "framing.c"
#include "analysis.c"
#include "analysis-generated.c"
#include "synthesis.c"
#include "synthesis-generated.c"

#endif /* FEATURE_BT_EXTPF_AV */
#endif /* FEATURE_BT */
