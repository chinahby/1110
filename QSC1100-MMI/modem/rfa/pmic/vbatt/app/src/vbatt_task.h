/*! \file 
 *  \n
 *  \brief  vbatt_task.h ---- VBATT TASK RELATED DEFINITIONS
 *  \n
 * \n GENERAL DESCRIPTION:
 * \n This file includes prtotypes for the vbatt_task and it's supplementary 
 * \n functions for the new vbatt-driver, which uses Qualcomm Power Management ICs. 
 * \n The new vbatt driver will not work without this file.
 * \n
 * \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/*===========================================================================

*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         VBATT TASK IMPLEMENTATION HEADER FILE.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/src/vbatt_task.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/09   APU     Added APIs to support the new VBATT info structure and 
                   also support the CHG callback functionality.
07/26/08   APU     Added the missing comments for 
                   vbatt_interrupt_handler_lean ()
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/

/*! \file 
 *  \n
 *  \brief  vbatt_task.h ---- VBATT TASK RELATED DEFINITION
 *  \n
 *  \details This header file contains declarations of the charger task.
 *  \n
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/


/*! \def VBATT_RPT_TIMER_SIG
 *  \brief The watchdog report signal for the vbatt task
 */
#define VBATT_RPT_TIMER_SIG           0x0001     


/*! \def VBATT_CHANGED_SIG
 *  \brief The vbatt change signal for the vbatt task
 */
#define VBATT_CHANGED_SIG             0x0002    

/*! \def VBATT_TMP_ONE_TIME_BOOT_SIG      
 *  \brief The one time timer wait vbatt signal during boot-up
 */
#define VBATT_TIME_HYS_SIG            0x0004     

/*! \def VBATT_CLK_WAIT_SIG
 *  \brief For putting a time-hysteresis so we dont react to ripples.
 */

/*! \def VBATT_CHG_EVT_SIG
 *  \brief For detctecting Charger events.
 */
#define VBATT_CHG_EVT_SIG             0x0008    /* Used generic for ftm_clk_rex_wait    */ 

/*! \def VBATT_CLK_WAIT_SIG
 *  \brief For putting a time-hysteresis so we dont react to ripples.
 */
#define VBATT_CLK_WAIT_SIG            0x0010    /* Used generic for ftm_clk_rex_wait    */ 




/*===========================================================================

FUNCTION vbatt_task                                     EXTERNAL FUNCTION

DESCRIPTION
 The vbatt driver does all its dealings which include but are not limited to 
 1. Servicing clients
 2. Calling their callbacks.
 3. Setting PMIC-Comparator windows after boot-up.
 4. Servicing REX\L4 sgnals.
 in the vbatt_task.
 5. Initializing the PMIC hadware and the task during start startup.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 dword - Not Used.
 
RETURN VALUE
 None.

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()

SIDE EFFECTS

===========================================================================*/
void  
vbatt_task ( dword parm )     ; /* from REX - ignored */





/*===========================================================================

FUNCTION vbatt_task_init                                    EXTERNAL FUNCTION

DESCRIPTION
 When the tmc(task-main-controller) spawns the tasks during bootup. vbatt_task
 is one of the task. When the CPU is given to vbatt_task for the very first
 time, this function is called. vbatt_task_init initializes the task and the
 related resources.

 EXAMPLE:

INPUT PARAMETERS
 NONE.

RETURN VALUE

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()

SIDE EFFECTS

===========================================================================*/
void 
vbatt_task_init ( void ) ;




/*===========================================================================

FUNCTION pm_vbatt_deregister_irq                          EXTERNAL FUNCTION

DESCRIPTION
 The pm_vbatt_deregister_irq is used for decoupling the PMIC comparator with 
 the ISR(vbatt_interrupt_handler_lean). This is desirable when the task is 
 processing clients based on one voltage and does not want to be disturbed. 
 Also if voltage is changing very quicky, it might be ripple because of charger
 or some other reason. In such case we want o decrease our response time by 
 off interrupts for some time. This function achives this.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void 
pm_vbatt_deregister_irq ( void ) ;




/*===========================================================================

FUNCTION pm_start_vbatt_interrupt                           EXTERNAL FUNCTION

DESCRIPTION
 The pm_start_vbatt_interrupt is called after pm_vbatt_deregister_irq. This 
 function couples the PMIC Comparator's interrupt to the driver-ISR.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void 
pm_start_vbatt_interrupt ( void ) ;




/*===========================================================================

FUNCTION vbatt_interrupt_handler_lean                    EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_interrupt_handler_lean is one of the interrupt handlers that the 
 vbatt driver uses to service change in the vbatt voltage and therefore the 
 comparator signal. This is the main ISR. The other ISR used is only acive 
 during the bootup. This ISR:
 1. Clears IRQ.
 2. Sends a signal to the task.
 3. Disables the PMIC-comparator interrupt.

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/

void 
vbatt_interrupt_handler_lean ( void* vptr ) ;


/*===========================================================================

FUNCTION vbatt_notify_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_notify_chg_event is called to notify the VBATT task of a CHG event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_notify_chg_event ( void ) ;


/*===========================================================================

FUNCTION vbatt_set_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_set_chg_event is called after a CHG event occurs. It sets a flag
 to record a charger event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_set_chg_event ( void ) ;




/*===========================================================================

FUNCTION vbatt_reset_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_reset_chg_event is called after pm_service_vbatt_tables() has 
 serviced all the clients. This makes the VBATT ready to receive new events. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
void
vbatt_reset_chg_event ( void ) ;


/*===========================================================================

FUNCTION vbatt_query_is_chg_event                           EXTERNAL FUNCTION

DESCRIPTION
 The vbatt_query_is_chg_event can be called to check if the last VBATT driver 
 event was a VBATT or a CHG event. 

EXAMPLE:
 NONE.

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 None.

DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()
 3) vbatt_task_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
boolean
vbatt_query_is_chg_event ( void ) ;


