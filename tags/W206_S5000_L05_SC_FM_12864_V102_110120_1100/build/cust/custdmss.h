#ifndef CUSTDMSS_H
#define CUSTDMSS_H
/*===========================================================================

DESCRIPTION
  Configuration for general DMSS features

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custdmss.h#3 $ $DateTime: 2009/03/20 04:24:59 $ $Author: kishorep $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/08   mp      Cleanup for QSC1100
02/27/03   wd      Enable FTM Asynchronous packets
05/01/02   jct     Created
===========================================================================*/

/* EVRC service option supported
*/
#define FEATURE_EVRC

/* Enable Factory Test Mode
*/
#define FEATURE_FACTORY_TESTMODE

/* Enable RF-Cal via Factory Test Mode
*/
#define FEATURE_FTM_RF

/* Enable FTM Task
*/
#define FEATURE_FTM_TASK

/* Enable FTM Asynchronous packets
*/
#define FEATURE_FTM_ASYNC_PKT

/* Put 8 bits and 16 bits shadow registers in one array.
*/
#define FEATURE_16BIT_SHADOW_REG

/* Search generated clock uptime to prevent losing time - affects deep sleep
*/
#define T_ALT_UPTIME

/* Define features of interest in field test and validation
*/
#undef  TMOBILE

/* Tell MC to use clk_uptime() for some of its timers.
*/
#define FEATURE_MC_TIMER_FIX

/*  Display register contents on Arm Mode and Reentrant Data Aborts
*/
#define FEATURE_BOOT_DATA_ABORT_DEBUG

/* Enable GPIO group interrupt
*/
#define FEATURE_GPIO_GROUP_INT

/* Removes the reference of self pointer and queue pointer in Queue elements.
** Saves 8 Bytes of RAM per queue element.
*/
#define FEATURE_Q_NO_SELF_QPTR

/* Links queue elements as Singly-link list. Saves 4 Bytes of RAM per queue element.*/
#define FEATURE_Q_SINGLE_LINK

/* Fast Interrupt*/
#define FEATURE_INT_125_FIQ
#define FEATURE_TRAMP_QUEUED_CALLS
#define FEATURE_2ND_LVL_INTR
#define FEATURE_INTR_TRIG_CFG


/* Test Automation*/
#define FEATURE_PORTMAP_TEST_AUTOMATION
#ifndef FEATURE_MANGO_UI
#define FEATURE_MEDIAPLAYER_TEST_AUTOMATION
#endif
#undef FEATURE_QCAMERA_TEST_AUTOMATION
#define FEATURE_AUDIOSETTINGS_TEST_AUTOMATION

#define FEATURE_XMLLIB

/* Turn on ETM*/
#define FEATURE_ETM

#define FEATURE_OEMSBL_USB_BATT_CHG 
              
/* Fix for a USB charger bug */
#define FEATURE_BATTERY_CHARGER__DISABLE_USB_CHARGER

#ifndef BUILD_BOOT_CHAIN
  #define FEATURE_TIMER_TASK
#endif 
#if !defined(BUILD_BOOT_CHAIN) 
   #define FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
#endif 
#endif /* CUSTDMSS_H */

