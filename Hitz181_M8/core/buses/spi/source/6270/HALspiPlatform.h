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

  $Header: //depot/asic/msmshared/hal/spi/source/6270/HALspiPlatform.h#2 $

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

#define SPI_HWIO_FMSK(instance, hwio_regsym, hwio_fldsym) HWIO_FMSK(hwio_regsym, hwio_fldsym) 
#define SPI_HWIO_IN(instance, hwio_regsym) HWIO_IN(hwio_regsym)
#define SPI_HWIO_INF(instance, hwio_regsym, hwio_fldsym) HWIO_INF(hwio_regsym, hwio_fldsym)
#define SPI_HWIO_OUT(instance, hwio_regsym, value) HWIO_OUT(hwio_regsym, value)
#define SPI_HWIO_OUTF(instance, hwio_regsym, hwio_fldsym, value) HWIO_OUTF(hwio_regsym, hwio_fldsym, value)

#endif /* HAL_SPI_PLATFORM_H */
