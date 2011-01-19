#ifndef HAL_SPI_PLATFORM_H
#define HAL_SPI_PLATFORM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            Serial Peripheral Interface Hardware Abstraction Layer
                      Target-specific Header File

Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/users/wxie/spi/q1110/spi/source/1100/HALspiPlatform.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
5/14/08    jdt     Released HAL
8/5/07     hj      Created
===========================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include <msm.h>

#define HAL_SPI_NUM_CORES 1
#define HAL_SPI_MAX_MX_INPUT_COUNT 4095
#define HAL_SPI_MAX_MX_OUTPUT_COUNT 4095

/* only GSBI1 is available for SPI on ULC */
#define SPI_HWIO_FMSK(instance, hwio_regsym, hwio_fldsym) HWIO_GSBI1_##hwio_regsym##_##hwio_fldsym##_BMSK
#define SPI_HWIO_IN(instance, hwio_regsym) HWIO_GSBI1_##hwio_regsym##_IN
#define SPI_HWIO_INF(instance, hwio_regsym, hwio_fldsym) \
   (HWIO_GSBI1_##hwio_regsym##_INM(HWIO_GSBI1_##hwio_regsym##_##hwio_fldsym##_BMSK) >> HWIO_GSBI1_##hwio_regsym##_##hwio_fldsym##_SHFT)
#define SPI_HWIO_OUT(instance, hwio_regsym, value) \
   HWIO_GSBI1_##hwio_regsym##_OUT(value)
#define SPI_HWIO_OUTF(instance, hwio_regsym, hwio_fldsym, value) \
   HWIO_GSBI1_##hwio_regsym##_OUTM(HWIO_GSBI1_##hwio_regsym##_##hwio_fldsym##_BMSK, \
     (value) << HWIO_GSBI1_##hwio_regsym##_##hwio_fldsym##_SHFT)

#endif /* HAL_SPI_PLATFORM_H */
