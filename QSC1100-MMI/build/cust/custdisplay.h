/*===========================================================================

            " C U S T D I S P L A Y "   H E A D E R   F I L E

DESCRIPTION
  Configuration for MDP Features on Enhanced Multimedia Targets.

  Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/Display/build/mdp1/cust/main/latest/custdisplay.h#5 $ $DateTime: 2008/12/17 06:33:09 $ $Author: gauravs $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/08   gs      Defined feature for display power debug
11/18/08   rk      Defined feature for boot splash screen
10/13/07   rk      Removed compiler warning
11/07/07   dw      Add primary panel layer 1 support
10/08/07   dw      Initial version

===========================================================================*/

/* Enable MDP functionality */
#ifndef BUILD_BOOT_CHAIN
#define FEATURE_MDP

/* Enable display power profiling */
#define FEATURE_DISPLAY_POWER_DEBUG
#endif
/* Enable display task */
#define FEATURE_DISP_TASK

/* Enable IMMOverlay feature */
#define FEATURE_MMOVERLAY
#ifndef T_QSC60X5 
#error code not present
#endif
/* Enable layer 1 support for primary display */
#define FEATURE_MDP_LAYER1_PRIMARY

/* Enable support for TMD22 QVGA LCD with RODEM2 controller */
#if !defined(FEATURE_TMD_22_QVGA) || defined(FEATURE_TMD_20_QVGA)
#undef FEATURE_TMD_20_QVGA
#define FEATURE_TMD_22_QVGA
#endif



