#ifndef CLKREGIM_H
#define CLKREGIM_H
/*===========================================================================

   C L O C K   R E G I M E    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions used by the
  DMSS clock regime services.

  Copyright (c) 1999--2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkregim.h#3 $
$DateTime: 2007/11/13 18:39:35 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/07    bc     Initial port from QSC6055v3202.
==========================================================================*/ 

/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

/* Set some features that this clock regime supports.  Careful adding
 * features that are used in clock regime itself as not every file includes
 * clkregim.h right now.
 */

/* Clock regime split into modem and MSM sides */
#define FEATURE_DUAL_CLK_RGM

/* Old definition still used in some places - should be mainlined */
#define FEATURE_HARDWARE_CONTROLLED_CLK_REGIMES

/* Old definition meaning clk_regime_enable takes a clock type instead of
   a direct mask - should be mainlined */
#define FEATURE_CLKREGIM_HEIRARCHICAL

/* Supports clk_regime_config_xxx_clocks functions for all modes. */
#define FEATURE_CLKREGIM_HAS_CONFIG_MODEM


/*=========================================================================
      Includes
==========================================================================*/

#include "clkrgm_msm.h" 

#include "clkrgm_modem.h"

#ifdef FEATURE_CLKREGIM_RM
#include "clkrgm_rm.h"            /* Resource manager API */
#endif

#include "clkrgm_devman.h"        /* Device manager API */

#include "clkrgm_client.h"

#endif /* CLKREGIM_H */
