#ifndef CUSTSVG_H
#define CUSTSVG_H
/*===========================================================================

                     M S M 6 5 5 0     H E A D E R   F I L E

DESCRIPTION
    Customer file for SVG engine

  Copyright (c) 2006   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

   $Header: //source/qcom/qct/multimedia/graphics/svg/build/cust/main/latest/inc/custsvg.h#1 $
   $DateTime: 2008/06/24 02:53:37 $
   $Author: ranjhith $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/06   lip     Inital version
===========================================================================*/


// SVG
#if defined(FEATURE_ENABLE_SVG_SVGDOM)
  #define FEATURE_SVG     // SVG
  #define FEATURE_SVGDOM  // SVGDOM
  #define SVG_USES_LIBSTD // Standard Library
#endif //defined(FEATURE_ENABLE_SVG_SVGDOM)

#endif // CUSTSVG_H
