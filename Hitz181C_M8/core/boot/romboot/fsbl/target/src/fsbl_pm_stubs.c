/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F S B L     P M I C       S T U B S
 
GENERAL DESCRIPTION

  This module provides stubs to the PMIC library for un-used functionality.
   
EXTERNALIZED FUNCTIONS

DEPENDENCIES
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_pm_stubs.c#2 $ 
  $DateTime: 2008/03/31 15:36:24 $ 
  $Author: psanghvi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
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

