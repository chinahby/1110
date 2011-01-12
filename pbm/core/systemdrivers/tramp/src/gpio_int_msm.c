/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          G R O U P   I N T E R R U P T   S E R V I C E S
         T A R G E T   S P E C I F I C   I M P L E M E N T A T I O N

GENERAL DESCRIPTION
   This module contains services for the GPIO Group Interrupt Block.  All 
   GPIO's can be configured as interrupts.  

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_int_msm.c#4 $
$DateTime: 2008/03/01 14:10:48 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported from QSC60X5.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "msm.h"
#include "gpio_int.h"
#include "gpio_inti.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
   Table entries for GROUP GPIO_INT control registers and shadow masks
-------------------------------------------------------------------------- */

const gpio_int_info_type gpio_int_table[NUM_GROUP_INT_REGISTERS] =
{
  /* GPIO2_GROUP_A: GPIO_INT_0 --> GPIO_INT_11 */
  {HWIO_ADDR(MSM_GPIO2_INT_CLEAR),      HWIO_RMSK(MSM_GPIO2_INT_CLEAR),
   HWIO_ADDR(MSM_GPIO2_INT_EN),         HWIO_RMSK(MSM_GPIO2_INT_EN),
   HWIO_ADDR(MSM_GPIO2_INT_POLARITY),   HWIO_RMSK(MSM_GPIO2_INT_POLARITY), 
   HWIO_ADDR(MSM_GPIO2_DETECT_CTL),     HWIO_RMSK(MSM_GPIO2_DETECT_CTL),
   HWIO_ADDR(MSM_GPIO2_INT_STATUS),     HWIO_RMSK(MSM_GPIO2_INT_STATUS),
   NUM_GPIO2_INTS_IN_REG_A,
   OFFSET_TO_REG1_A,
   START_GPIO2_INT_BIT_IN_REG_A },

  /* GPIO1_GROUP_0A: GPIO_INT_12 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_0),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_0),
   HWIO_ADDR(MSM_GPIO1_INT_EN_0),       HWIO_RMSK(MSM_GPIO1_INT_EN_0),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_0), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_0), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_0),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_0),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_0),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_0),
   NUM_GPIO1_INTS_IN_REG_0A,
   OFFSET_TO_REG0_A,
   START_GPIO1_INT_BIT_IN_REG_0A },

  /* GPIO2_GROUP_B: GPIO_INT_13 */
  {HWIO_ADDR(MSM_GPIO2_INT_CLEAR),      HWIO_RMSK(MSM_GPIO2_INT_CLEAR),
   HWIO_ADDR(MSM_GPIO2_INT_EN),         HWIO_RMSK(MSM_GPIO2_INT_EN),
   HWIO_ADDR(MSM_GPIO2_INT_POLARITY),   HWIO_RMSK(MSM_GPIO2_INT_POLARITY), 
   HWIO_ADDR(MSM_GPIO2_DETECT_CTL),     HWIO_RMSK(MSM_GPIO2_DETECT_CTL),
   HWIO_ADDR(MSM_GPIO2_INT_STATUS),     HWIO_RMSK(MSM_GPIO2_INT_STATUS),
   NUM_GPIO2_INTS_IN_REG_B,
   OFFSET_TO_REG1_B,
   START_GPIO2_INT_BIT_IN_REG_B},

  /* GPIO1_GROUP_0B: GPIO_INT_14  --> GPIO_INT_30 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_0),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_0),
   HWIO_ADDR(MSM_GPIO1_INT_EN_0),       HWIO_RMSK(MSM_GPIO1_INT_EN_0),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_0), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_0), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_0),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_0),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_0),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_0),
   NUM_GPIO1_INTS_IN_REG_0B,
   OFFSET_TO_REG0_B,
   START_GPIO1_INT_BIT_IN_REG_0B },

  /* GPIO2_GROUP_C: GPIO_INT_31 --> GPIO_INT_33 */
  {HWIO_ADDR(MSM_GPIO2_INT_CLEAR),      HWIO_RMSK(MSM_GPIO2_INT_CLEAR),
   HWIO_ADDR(MSM_GPIO2_INT_EN),         HWIO_RMSK(MSM_GPIO2_INT_EN),
   HWIO_ADDR(MSM_GPIO2_INT_POLARITY),   HWIO_RMSK(MSM_GPIO2_INT_POLARITY), 
   HWIO_ADDR(MSM_GPIO2_DETECT_CTL),     HWIO_RMSK(MSM_GPIO2_DETECT_CTL),
   HWIO_ADDR(MSM_GPIO2_INT_STATUS),     HWIO_RMSK(MSM_GPIO2_INT_STATUS),
   NUM_GPIO2_INTS_IN_REG_C,
   OFFSET_TO_REG1_C,
   START_GPIO2_INT_BIT_IN_REG_C },

  /* GPIO1_GROUP_0C: GPIO_INT_34  --> GPIO_INT_41 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_0),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_0),
   HWIO_ADDR(MSM_GPIO1_INT_EN_0),       HWIO_RMSK(MSM_GPIO1_INT_EN_0),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_0), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_0), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_0),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_0),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_0),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_0),
   NUM_GPIO1_INTS_IN_REG_0C,
   OFFSET_TO_REG0_C,
   START_GPIO1_INT_BIT_IN_REG_0C },

  /* GPIO2_GROUP_D: GPIO_INT_42 --> GPIO_INT_52 */
  {HWIO_ADDR(MSM_GPIO2_INT_CLEAR),      HWIO_RMSK(MSM_GPIO2_INT_CLEAR),
   HWIO_ADDR(MSM_GPIO2_INT_EN),         HWIO_RMSK(MSM_GPIO2_INT_EN),
   HWIO_ADDR(MSM_GPIO2_INT_POLARITY),   HWIO_RMSK(MSM_GPIO2_INT_POLARITY), 
   HWIO_ADDR(MSM_GPIO2_DETECT_CTL),     HWIO_RMSK(MSM_GPIO2_DETECT_CTL),
   HWIO_ADDR(MSM_GPIO2_INT_STATUS),     HWIO_RMSK(MSM_GPIO2_INT_STATUS),
   NUM_GPIO2_INTS_IN_REG_D,
   OFFSET_TO_REG1_D,
   START_GPIO2_INT_BIT_IN_REG_D },

  /* GPIO1_GROUP_0D: GPIO_INT_53 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_0),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_0),
   HWIO_ADDR(MSM_GPIO1_INT_EN_0),       HWIO_RMSK(MSM_GPIO1_INT_EN_0),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_0), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_0), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_0),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_0),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_0),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_0),
   NUM_GPIO1_INTS_IN_REG_0D,
   OFFSET_TO_REG0_D,
   START_GPIO1_INT_BIT_IN_REG_0D },

  /* GPIO2_GROUP_E: GPIO_INT_54 */
  {HWIO_ADDR(MSM_GPIO2_INT_CLEAR),      HWIO_RMSK(MSM_GPIO2_INT_CLEAR),
   HWIO_ADDR(MSM_GPIO2_INT_EN),         HWIO_RMSK(MSM_GPIO2_INT_EN),
   HWIO_ADDR(MSM_GPIO2_INT_POLARITY),   HWIO_RMSK(MSM_GPIO2_INT_POLARITY), 
   HWIO_ADDR(MSM_GPIO2_DETECT_CTL),     HWIO_RMSK(MSM_GPIO2_DETECT_CTL),
   HWIO_ADDR(MSM_GPIO2_INT_STATUS),     HWIO_RMSK(MSM_GPIO2_INT_STATUS),
   NUM_GPIO2_INTS_IN_REG_E,
   OFFSET_TO_REG1_E,
   START_GPIO2_INT_BIT_IN_REG_E },

  /* GPIO1_GROUP_0E: GPIO_INT_55  --> GPIO_INT_59 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_0),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_0),
   HWIO_ADDR(MSM_GPIO1_INT_EN_0),       HWIO_RMSK(MSM_GPIO1_INT_EN_0),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_0), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_0), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_0),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_0),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_0),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_0),
   NUM_GPIO1_INTS_IN_REG_0E,
   OFFSET_TO_REG0_E,
   START_GPIO1_INT_BIT_IN_REG_0E },
   
  /* GPIO1_GROUP_1A: GPIO_INT_60  --> GPIO_INT_62 */
  {HWIO_ADDR(MSM_GPIO1_INT_CLEAR_1),    HWIO_RMSK(MSM_GPIO1_INT_CLEAR_1),
   HWIO_ADDR(MSM_GPIO1_INT_EN_1),       HWIO_RMSK(MSM_GPIO1_INT_EN_1),
   HWIO_ADDR(MSM_GPIO1_INT_POLARITY_1), HWIO_RMSK(MSM_GPIO1_INT_POLARITY_1), 
   HWIO_ADDR(MSM_GPIO1_DETECT_CTL_1),   HWIO_RMSK(MSM_GPIO1_DETECT_CTL_1),
   HWIO_ADDR(MSM_GPIO1_INT_STATUS_1),   HWIO_RMSK(MSM_GPIO1_INT_STATUS_1),
   NUM_GPIO1_INTS_IN_REG_1A,
   OFFSET_TO_REG2_A,
   START_GPIO1_INT_BIT_IN_REG_1A }
};

/* Table of indices into the above table for group1 interrupts */
const uint32 gpio_int_group0_table[NUM_GROUP0_INT_REGISTERS] =
{
  1, 3, 5, 7, 9, 10
};
const uint32 gpio_int_group1_table[NUM_GROUP1_INT_REGISTERS] =
{
  0, 2, 4, 6, 8
};


/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
