#ifndef GPIO_INTI_MSM_H
#define GPIO_INTI_MSM_H
/*===========================================================================

          G R O U P   I N T E R R U P T   S E R V I C E S
               I N T E R N A L  H E A D E R  F I L E
                   T A R G E T   S P E C I F I C

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA ASIC GROUP interrupt controllers.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_inti_msm.h#2 $
$DateTime: 2008/02/06 11:26:09 $ $Author: vphan $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported from QSC60X5 

===========================================================================*/



/*===========================================================================

                         CAPABILITY FEATURES

===========================================================================*/

/* Always install the ISR */
#define GPIO_INT_INSTALL_ISR


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/* There are eleven (physically TWO, but we count GPIO1_GROUP and GPIO2_GROUP) 
   gpio registers: GPIO1_GROUP_0A, GPIO1_GROUP_0B, GPIO1_GROUP_0C, GPIO1_GROUP_0D, GPIO1_GROUP_0E, GPIO1_GROUP_1A
                   GPIO2_GROUP_A, GPIO2_GROUP_B, GPIO2_GROUP_C, GPIO2_GROUP_D, GPIO2_GROUP_E, 
 */
#define NUM_GROUP0_INT_REGISTERS   6 
#define NUM_GROUP1_INT_REGISTERS   5

/* First and last valid group interrupt */
#define FIRST_GROUP_INT       GPIO_INT_0
#define LAST_GROUP_INT        GPIO_INT_62

/* Distribution of GPIO_INTs in each register */
/* GPIO1 Registers are divided into six groups:
    GPIO1_GROUP_0A contains GPIO_12, 
    GPIO1_GROUP_0B contains GPIO_14 -> GPIO_30, 
    GPIO1_GROUP_0C contains GPIO_34 -> GPIO_41,
    GPIO1_GROUP_0D contains GPIO_53, 
    GPIO1_GROUP_0E contains GPIO_55 -> GPIO_59,
    GPIO1_GROUP_1A contains GPIO_60 -> GPIO_62
 */
#define OFFSET_TO_REG0_A           GPIO_INT_12
#define OFFSET_TO_REG0_B           GPIO_INT_14
#define OFFSET_TO_REG0_C           GPIO_INT_34
#define OFFSET_TO_REG0_D           GPIO_INT_53
#define OFFSET_TO_REG0_E           GPIO_INT_55
#define OFFSET_TO_REG2_A           GPIO_INT_0

/* GPIO2 Registers are divided into five groups:
    GPIO2_GROUP_A contains GPIO_0 -> GPIO_11, 
    GPIO2_GROUP_B contains GPIO_13, 
    GPIO2_GROUP_C contains GPIO_31 -> GPIO_33,
    GPIO2_GROUP_D contains GPIO_42 -> GPIO_52, 
    GPIO2_GROUP_E contains GPIO_54,          
 */ 
#define OFFSET_TO_REG1_A           GPIO_INT_0
#define OFFSET_TO_REG1_B           GPIO_INT_13
#define OFFSET_TO_REG1_C           GPIO_INT_31
#define OFFSET_TO_REG1_D           GPIO_INT_42
#define OFFSET_TO_REG1_E           GPIO_INT_54

/* Number of GPIO_INTs in each register */
#define NUM_GPIO1_INTS_IN_REG_0A    1
#define NUM_GPIO1_INTS_IN_REG_0B    17
#define NUM_GPIO1_INTS_IN_REG_0C    8
#define NUM_GPIO1_INTS_IN_REG_0D    1
#define NUM_GPIO1_INTS_IN_REG_0E    5
#define NUM_GPIO1_INTS_IN_REG_1A    3
#define NUM_GPIO2_INTS_IN_REG_A     12
#define NUM_GPIO2_INTS_IN_REG_B     1
#define NUM_GPIO2_INTS_IN_REG_C     3
#define NUM_GPIO2_INTS_IN_REG_D     11
#define NUM_GPIO2_INTS_IN_REG_E     1

/* Starting bit of GPIO_INTs in each register */
#define START_GPIO1_INT_BIT_IN_REG_0A    0
#define START_GPIO1_INT_BIT_IN_REG_0B    1
#define START_GPIO1_INT_BIT_IN_REG_0C    18
#define START_GPIO1_INT_BIT_IN_REG_0D    26
#define START_GPIO1_INT_BIT_IN_REG_0E    27
#define START_GPIO1_INT_BIT_IN_REG_1A    0
#define START_GPIO2_INT_BIT_IN_REG_A     0
#define START_GPIO2_INT_BIT_IN_REG_B     12
#define START_GPIO2_INT_BIT_IN_REG_C     13
#define START_GPIO2_INT_BIT_IN_REG_D     16
#define START_GPIO2_INT_BIT_IN_REG_E     27

#endif /* GPIO_INTI_MSM_H */
