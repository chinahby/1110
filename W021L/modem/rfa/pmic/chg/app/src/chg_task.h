#ifndef CHG_TASK_H
#define CHG_TASK_H
/*! \file 
 *  \n
 *  \brief  chg_task.h ---- CHARGER TASK RELATED DEFINITION
 *  \n
 *  \details This header file contains declarations of the charger task.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/*! \mainpage Charger Task
 *  \section Introduction
 *  The newly designed charger software is task based. It was written in C++ while 
 *  it has the interface for modules written in C. It supports both the autonomous 
 *  charger as well as software-controlled charger.
 *
 * The charger software uses the observer pattern: whatever happens that is charging 
 * related, a proper charging event will be created and will be fired, by a proper
 * charging event generator, and will then be handled by a proper charging event listener.
 * For more information please go to:
 * \htmlonly <a href="http://www.research.ibm.com/designpatterns/example.htm">
 * http://www.research.ibm.com/designpatterns/example.htm</a>
 * \endhtmlonly
 *
 * There are two major categories of the charging events:
 * \li Events that happen in the interrupt context, such as PMIC interrupts, timer 
 * expirations and etc. When such an event happens, a charger task command will be 
 * generated and will be pushed into the charger task command queue. The command will 
 * be executed later when the charger task is scheduled to run. In this way, the charger
 * task will minimize the time it operates in interrupt context.
 * \li Events that happen in the task context, such as battery conditions, charger 
 * conditions, and etc. When such events happen, a charging event will be created and
 * will be fired by a ChargingEventGenerator and then will be propagated to various 
 * ChargingEventListeners. All ChargingEventListener that have registered to listen for
 * this event, will be notified and given a chance to respond to the event.
 *
 * \section Flowchart 
 * 
 * For the flowchart of the charger task, please refer to \ref page1; for the flowcharts 
 * of the automonous charging, please refer to \ref page2 and \ref page3
 * \page page1 "Flowchart - Charger Task"
 * \image html charger_task_flowchart.jpg
 * \page page2 "Flowchart - Autonomous Charging Using Wall Charger"
 * \image html Auto_charging_wall_flowchart.jpg
 * \page page3 "Flowchart - Autonomous Charging Using USB Device"
 * \image html Auto_charging_usb_flowchart.jpg
 */


 
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_task.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Removed unnecessary header file inclusion.
05/07/09   hs      Removed unnecessary header file inclusion.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
11/18/08   hs      Updated the documentation.
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
07/12/07   hs      Created.

========================================================================== */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* end __cplusplus */

#include "chg_common.h"        /* For chg_system_event_id_type definition  */
#include "queue.h"             /* for q_link_type                          */ 

/*! \def CHG_RPT_TIMER_SIG
 *  \brief The watchdog report signal for the charger task
 */
#define CHG_RPT_TIMER_SIG      0x0001     
/*! \def CHG_CMD_Q_SIG
 *  \brief The command queue signal for the charger task
 */
#define CHG_CMD_Q_SIG          0x0002    
/*! \def CHG_CLK_WAIT_SIG
 *  \brief The generic timer
 */
#define CHG_CLK_WAIT_SIG       0x0008    

/*! \enum chg_task_cmd_id_type
 *  \brief This enum contains commands/messages sent among charger application 
*/
typedef enum
{
    /*! Timer expired.                               */
    CHG_CMD__TIMER_EXPIRED,     
    /*! IRQ triggered.                               */
    CHG_CMD__IRQ_TRIGGERED,  
    /*! System status changed.                       */
    CHG_CMD__SYSTEM_STATUS_CHANGED,     
    /*! UI status changed.                           */
    CHG_CMD__UI_STATUS_CHANGED,     
    /*! Debug the auto charging.                     */
    CHG_CMD__AUTO_CHARGING_DEBUG,
    /*! Invalid event.                               */
    CHG_CMD__INVALID                         
}chg_task_cmd_id_type;
/*! \struct chg_task_cmd_hdr_type
 *  \brief This enum defines the format of the charger task command header 
 */
typedef struct
{
    /*! Queue link.                                  */
    q_link_type                     link;    
} chg_task_cmd_hdr_type;
/*! \struct chg_task_cmd_type
 *  \brief This enum defines the format of the charger task command 
 */
typedef struct
{
    chg_task_cmd_hdr_type        hdr;
    chg_task_cmd_id_type         cmd_id;
    void                        *cmd_data_ptr;
    unsigned int                 timer_token;
    pm_irq_hdl_type              irq_id;
    chg_system_event_id_type     system_event_id;
    chg_tx_wait_voter_type       tx_voter_id;
} chg_task_cmd_type;
/*===========================================================================

                        VARIABLE DEFINITIONS 

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*! \brief This function is the charger task
 *  \details This is the charger task, created via rex_def_task from the Main
    Control task. After processing task start procedure, the CHG task enters
    an infiniten loop, awaiting commands received on its command queue. Each
    command received is processed to completion before the next command is
    processed. While in the main loop the task kicks the watchdog periodically.
 *  \param parm The parameter is used to comply with the REX interface.
 *  \return void
 */ 
extern void  chg_task(dword parm);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */


#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_TASK_H */

