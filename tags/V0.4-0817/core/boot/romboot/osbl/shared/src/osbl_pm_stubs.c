/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              O E M S B L     P M I C       S T U B S
 
GENERAL DESCRIPTION

  This module provides stubs to the PMIC library for un-used functionality.
   
EXTERNALIZED FUNCTIONS

DEPENDENCIES
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_pm_stubs.c#1 $ 
  $DateTime: 2008/01/28 10:41:58 $ 
  $Author: wctan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/06   AMW     Added pm_variable_pa_vdd_init stub function.
02/01/06   LP      Added pm_app_diag_init stub function
06/28/05   AMW     Initial version for 7500

===========================================================================*/

#include "customer.h"
#include "pm.h"
#include "clk.h"

/* drivers/pm/pmapp.c */
void pm_vote_vreg_switch( pm_switch_cmd_type cmd,
                          pm_vreg_id_type vreg_id,
                          pm_vote_vreg_app_type app_mask )
{
}

/* drivers/pm/pmapp.c */
void pm_adc_init(void)
{
}

/* drivers/pm/pmapp.c */
void pm_app_clk_busy_wait(uint32 us)
{
    /* Halts the Microprocessor for the specified amount of usecs. */
    clk_busy_wait(us);
}

/* drivers/pm/pm.c */
void pm_app_diag_init()
{
}

/* drivers/pm/pmapp.c */
void pm_variable_pa_vdd_init(boolean dvs_enable)
{
}

