#ifndef GPIO_INTI_H
#define GPIO_INTI_H
/*===========================================================================

          G R O U P   I N T E R R U P T   S E R V I C E S
              I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA ASIC GROUP interrupt controllers.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights
Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_inti.h#2 $ 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Created from QSC60X5

===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

/* Note this file includes gpio_inti_msm.h below */



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/* --------------------------------------------------------------------------
   Table entries for GROUP GPIO_INT control registers and shadow masks
-------------------------------------------------------------------------- */
typedef struct
{
  const uint32 clear_reg;     /* address of interrupt clear reg               */
  const uint32 clear_rmsk;    /* register mask of interrupt clear reg         */
  const uint32 mask_reg;      /* address of interrupt mask reg                */
  const uint32 mask_rmsk;     /* register mask of interrupt mask reg          */
  const uint32 pol_reg;       /* address of interrupt polarity reg            */
  const uint32 pol_rmsk;      /* register mask of interrupt polarity reg      */
  const uint32 ctl_reg;       /* address of interrupt latch control reg       */
  const uint32 ctl_rmsk;      /* register mask of interrupt latch control reg */
  const uint32 status_reg;    /* address of status register                   */
  const uint32 status_rmsk;   /* register mask of status register             */
  uint32       num_ints;      /* number of interrupts in this register        */
  uint32       offset;        /* offset to first interrupt in this register   */
  uint32       starting_bit;  /* the bit in the register corresponding to the */
                              /* first interrupt in this register             */
} gpio_int_info_type;


/* Table of interrupts */
extern const gpio_int_info_type gpio_int_table[];

/* Table of indices into the above table for group0 interrupts */
extern const uint32 gpio_int_group0_table[];

/* Table of indices into the above table for group1 interrupts */
extern const uint32 gpio_int_group1_table[];


/* Pick up MSM specific definitions */
#include "gpio_inti_msm.h"



/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif /* GPIO_INTI_H */
