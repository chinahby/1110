/*===========================================================================

        Pacing Arbiter   F O R   Q S C 1100

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the PARB functions for the QSC1100.

REFERENCES

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/parb/parb.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/08   hvo     Removed grant table initialization, and hclk
07/31/07   hvo     Ported to QSC1100 from QSC6055
===========================================================================*/


#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "msm.h"                /* Includes for specific HW mappings    */
#include "tramp.h"              /* Error logging macro                  */
#include "err.h"                /* Error logging macro                  */

#include "parb.h"               /* Pacing Arbiter public file           */
#include "parbi.h"              /* Pacing Arbiter private file          */


/*===========================================================================

  FUNCTION parb_priority_init()

  DESCRIPTION
    Initializes the Arbiter priority registers.

  PARAMETERS
    app_bus_priority_type: Pointer to AHB APP bus priority settings configuration.
                           Use NULL pointer to leave the settings at last configuration
    sys_bus_priority_type: Pointer to AHB SYS bus priority settings configuration
                           Use NULL pointer to leave the settings at last configuration

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/

uint8 gt_sys_save[8][2];
uint32 sys_bus_priority_save;
rex_crit_sect_type parb_crit_sect;

gt_slot_info_type gt_slot_info__for_hclk_96mhz[] = {

  /* At HCLK = 96 MHz, max b/w is 6MB/s per coarse slot & 48MB/s per fine slot */
  /* { sys coarse slot, sys fine slot, app coarse slot, app fine slot} */
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      },
  {0,                     0,    0,                       0,                      }
};

gt_slot_info_type gt_slot_info__for_hclk_64mhz[] = {

  /* At HCLK = 64 MHz, max b/w is 4MB/s per coarse slot & 32MB/s per fine slot */
  /* { sys coarse slot, sys fine slot, app coarse slot, app fine slot } */
  {0,                    0,    0,8|APP_BUS_MASTER__ADSP_DME,                      },
  {0,                    0,    0,                         0,                      },
  {0,                    0,    0,8|APP_BUS_MASTER__ADSP_DME,                      },
  {0,                    0,    0,                         0,                      },
  {0,                    0,    0,8|APP_BUS_MASTER__ADSP_DME,                      },
  {0,                    0,    0,                         0,                      },
  {0,                    0,    0,8|APP_BUS_MASTER__ADSP_DME,                      },
  {0,                    0,    0,                         0,                      }
};

gt_slot_info_type *gt_slot_info_table[] = {
    /* do not change the order */
    gt_slot_info__for_hclk_96mhz,
    gt_slot_info__for_hclk_64mhz
};

/*===========================================================================

  FUNCTION parb_priority_init()

  DESCRIPTION
    Initializes the Arbiter Grant Table registers to reset values

  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void parb_priority_init(sys_bus_priority_type *sys_bus_priority,
                                        app_bus_priority_type *app_bus_priority 
                                        )
{
  /* Sanity check */
    
  rex_enter_crit_sect (&parb_crit_sect);

  if (app_bus_priority)
  {
    HWIO_OUT (AHB_APP_BUS_PRIORITY, 
      (app_bus_priority->default_master << HWIO_SHFT (AHB_APP_BUS_PRIORITY, DEFAULT_MASTER) )
      | (app_bus_priority->adsp_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, ADSP_PRIORITY) )
      | (app_bus_priority->video_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, VIDEO_PRIORITY) )
      | (app_bus_priority->bpm_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, BPM_PRIORITY) )
      | (app_bus_priority->dummy_sw_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, DUMMY_SW_PRIORITY) )
	  | (app_bus_priority->mdp_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, MDP_PRIORITY) )
	  | (app_bus_priority->usb_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, USB_PRIORITY) )
	  | (app_bus_priority->sdcc_priority << HWIO_SHFT (AHB_APP_BUS_PRIORITY, SDCC_PRIORITY) )
      );
  }
  
  
  if (sys_bus_priority)
  {
    HWIO_OUT (AHB_SYS_BUS_PRIORITY,
      (sys_bus_priority->default_master << HWIO_SHFT (AHB_SYS_BUS_PRIORITY, DEFAULT_MASTER) )
      | (sys_bus_priority->armi_priority << HWIO_SHFT (AHB_SYS_BUS_PRIORITY, ARMI_PRIORITY) )
      | (sys_bus_priority->armd_priority << HWIO_SHFT (AHB_SYS_BUS_PRIORITY, ARMD_PRIORITY) )
      | (sys_bus_priority->tic_priority << HWIO_SHFT (AHB_SYS_BUS_PRIORITY, TIC_PRIORITY) )
      );

    /* Each time AHB_SYS_BUS_PRIORITY is programmed, AHB_SYS_DUMMY0/1/2 should be
    programmed with arbitrary value */
    HWIO_OUT (AHB_SYS_DUMMY0, 0);
    HWIO_OUT (AHB_SYS_DUMMY1, 0);
    HWIO_OUT (AHB_SYS_DUMMY2, 0);

  }
  
  rex_leave_crit_sect (&parb_crit_sect);
  
}



/*===========================================================================

  FUNCTION parb_grant_table_reset()

  DESCRIPTION
    Initializes the Arbiter Grant Table registers to reset values

  PARAMETERS
    Slot size
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/

void parb_grant_table_reset(gt_slot_size_type slot_size)
{
  int i;
  
  rex_enter_crit_sect (&parb_crit_sect);
  
  for (i=0; i<8; ++i)
  {
    HWIO_OUTI (SYS_COARSE_TABLEn, i, 0);
    HWIO_OUTI (SYS_FINE_TABLEn, i, 0);

    HWIO_OUTI (APP_COARSE_TABLEn, i, 0);    
    HWIO_OUTI (APP_FINE_TABLEn, i, 0);
  }
  
  /* Set the slot size */
  HWIO_OUT (GT_SLOT_SIZE, 
    (slot_size << HWIO_SHFT (GT_SLOT_SIZE, SYS_SLOT_SIZE) )
    | (slot_size << HWIO_SHFT (GT_SLOT_SIZE, APP_SLOT_SIZE) ));

  rex_leave_crit_sect (&parb_crit_sect);

}


/*===========================================================================

  FUNCTION parb_init

  DESCRIPTION
    Initialize the priority for all masters on SYS/APP AHB. Priority 0 won't
	give any access to bus.
  PARAMETERS
    None
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void parb_init(void)
{
#if 0 // No need
  int hclk_div;
#endif
  app_bus_priority_type app_bus_priority;
  sys_bus_priority_type sys_bus_priority;

#if 0 // No need
  hclk_div = HWIO_IN (HCLK_DIV);
#endif

  rex_init_crit_sect (&parb_crit_sect);
  
  /* App AHB default master */
  app_bus_priority.default_master = APP_BUS_MASTER__VIDEO;

  /* App AHB master priority, they are pre-determine, need to review */
  app_bus_priority.adsp_priority = APP_AHB_ADSP_PRIORITY;
  app_bus_priority.video_priority = APP_AHB_VIDEO_PRIORITY;
  app_bus_priority.bpm_priority = APP_AHB_BPM_PRIORITY;
  app_bus_priority.dummy_sw_priority = APP_AHB_DUMMY_SW_PRIORITY;
  app_bus_priority.mdp_priority = APP_AHB_MDP_PRIORITY;
  app_bus_priority.usb_priority = APP_AHB_USB_PRIORITY;
  app_bus_priority.sdcc_priority = APP_AHB_SDCC_PRIORITY;
  
  /* Sys AHB default master */
  sys_bus_priority.default_master = SYS_BUS_MASTER__ARMD;

  /* Sys AHB masters priority, they are pre-determined */
  sys_bus_priority.armi_priority = SYS_AHB_ARMDI_PRIORITY;
  sys_bus_priority.armd_priority = SYS_AHB_ARMD_PRIORITY;
  sys_bus_priority.tic_priority = SYS_AHB_TIC_PRIORITY;

#if 0 // No need
  if (2 == hclk_div)
  {
    /* For 64 MHz, Nor, implement special settings to work around MDM2 
       latency problem */
    app_bus_priority.adsp_priority = 0;
  }
#endif
  parb_priority_init (&sys_bus_priority, &app_bus_priority);

  #ifdef PARB_INT_INSTALL_ISR

    tramp_set_isr (TRAMP_ARBITER_SYS_ISR, parb_interrupt_sys_isr);
	tramp_set_isr (TRAMP_ARBITER_APP_ISR, parb_interrupt_app_isr);
  
  #endif /* PARB_INT_INSTALL_ISR */

#if 0 // No need
  /* Hack - Find the HCLK and set the arbiter slots accordingly.  Final implementation
     should have a way to query clk module to find correct HCLK. */
  if (1 == hclk_div)
  {
    parb_grant_table_reset (GT_SLOT_SIZE__8);
    parb_grant_bandwidth (HCLK_96mhz);
  }
  else if (2 == hclk_div)
  {
    parb_grant_table_reset (GT_SLOT_SIZE__16);
    parb_grant_bandwidth (HCLK_64mhz);
  }
#endif
}


/*===========================================================================

  FUNCTION parb_grant_bandwidth

  DESCRIPTION
    Fill in the grant table entries with masters which need a certain time
	slots. These timeslots must be predictable.
  PARAMETERS

  DEPENDENCIES
    AHB clock rate

  RETURN VALUE
    0: Success

  SIDE EFFECTS
    None.
===========================================================================*/
int parb_grant_bandwidth(gt_slot_info_table_index_type hclk)
{
  int i;

  gt_slot_info_type *slot_info = gt_slot_info_table[hclk];

  rex_enter_crit_sect (&parb_crit_sect);
  
  for (i=0; i<8; ++i)
  {
    HWIO_OUTI (SYS_COARSE_TABLEn, i, slot_info[i].sys_coarse_slot);
    HWIO_OUTI (SYS_FINE_TABLEn, i, slot_info[i].sys_fine_slot);

    HWIO_OUTI (APP_COARSE_TABLEn, i, slot_info[i].app_coarse_slot);    
    HWIO_OUTI (APP_FINE_TABLEn, i, slot_info[i].app_fine_slot);
  }

  rex_leave_crit_sect (&parb_crit_sect);
  
  return 0;
}

/*===========================================================================

  FUNCTION ahb_bus_lock()

  DESCRIPTION
    Lock out APP bus.  Allow only ARM to access SYS bus.
    Obtains critical section lock but doesnt release it.
    Also see ahb_bus_unlock()  [releases critical section lock]
    
  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    0: Success

  SIDE EFFECTS
    None.
===========================================================================*/
int ahb_bus_lock(void)
{

    int i, rt_status=0;

    rex_enter_crit_sect (&parb_crit_sect);

    /* Acquire APP bus */
    HWIO_OUT (AHB_BUS_REQUEST,1 << HWIO_SHFT (AHB_BUS_REQUEST, APP_HBUSREQ));
  
    /* Wait for APP */
    while (HWIO_IN (AHB_BUS_MASTERS) & (HWIO_FMSK (AHB_BUS_MASTERS, APP_HMASTER)))
    {
        KICK_WATCHDOG ();
    }


    /* Prevent TIC from getting on SYS */
    
    for (i=0; i<8; ++i)
    {
      gt_sys_save[i][0] = HWIO_INI (SYS_COARSE_TABLEn, i);
      gt_sys_save[i][1] = HWIO_INI (SYS_FINE_TABLEn, i);
      HWIO_OUTI (SYS_COARSE_TABLEn, i, 0);
      HWIO_OUTI (SYS_FINE_TABLEn, i, 0);
    }

    /* Save SYS priority settings */
    sys_bus_priority_save = HWIO_IN (AHB_SYS_BUS_PRIORITY);
    
    /* Assign zero priority to TIC */
    HWIO_OUTM (AHB_SYS_BUS_PRIORITY, HWIO_FMSK (AHB_SYS_BUS_PRIORITY, TIC_PRIORITY),0  );

    /* do not leave critical section, let the bus_unlock() function release the critical section */

    return rt_status;

}

/*===========================================================================

  FUNCTION ahb_bus_unlock()

  DESCRIPTION
    Release APP bus.  Restore SYS bus to settings that preceded ahb_bus_lock() call
    Releases the critical section lock obtained by ahb_bus_lock()
    Also see ahb_bus_lock()

  PARAMETERS

  DEPENDENCIES
    Should be preceded by ahb_bus_lock()

  RETURN VALUE
    0: Success

  SIDE EFFECTS
    None.
===========================================================================*/
int ahb_bus_unlock(void)
{

    int i, rt_status=0;

    /* already entered critical section in bus_lock() */
    
    /* Restore GT setting */
    
    for (i=0; i<8; ++i)
    {
      HWIO_OUTI (SYS_COARSE_TABLEn, i, gt_sys_save[i][0]);
      HWIO_OUTI (SYS_FINE_TABLEn, i, gt_sys_save[i][1]);
    }

    /* Release APP */
    HWIO_OUT (AHB_BUS_REQUEST,0 << HWIO_SHFT (AHB_BUS_REQUEST, APP_HBUSREQ));

    /* Restore SYS bus priority settings */
    
    HWIO_OUT (AHB_SYS_BUS_PRIORITY, sys_bus_priority_save);

    rex_leave_crit_sect (&parb_crit_sect);

    return rt_status;
}



#ifdef PARB_INT_INSTALL_ISR

parb_isr_counter_type parb_isr_counter = {0, 0};

/*===========================================================================

  FUNCTION parb_interrupt_sys_isr()

  DESCRIPTION
    Handle SYS-AHB pointers wrap (SSP and TSP)
  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void parb_interrupt_sys_isr(void)
{
    /* Tramp cleared the sys_wrap interrupt bit, so update the counter, 
	   or throw fatal error because one of the masters is hogging the bus  */
#if 0
	// Tramp already cleared
    HWIO_OUTM(IRQ_CLEAR_1,HWIO_FMSK(IRQ_CLEAR_1,SYS_POINTER_WRAP) ); 
#endif
	parb_isr_counter.sys_bus += 1;
  
}

/*===========================================================================

  FUNCTION parb_interrupt_app_isr()

  DESCRIPTION
    Handle APP-AHB pointers wrap (SSP and TSP)
  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void parb_interrupt_app_isr(void)
{
	/* Tramp cleared the sys_wrap interrupt bit, so update the counter, 
	   or throw fatal error because one of the masters is hogging the bus  */
#if 0
    // Tramp already cleared
    HWIO_OUTM(IRQ_CLEAR_1,HWIO_FMSK(IRQ_CLEAR_1,APP_POINTER_WRAP) ); 
#endif
	parb_isr_counter.app_bus += 1;
}
#endif /* PARB_INT_INSTALL_ISR */
