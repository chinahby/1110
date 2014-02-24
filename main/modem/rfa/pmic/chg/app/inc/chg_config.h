#ifndef CHG_CONFIG_H
#define CHG_CONFIG_H

/*! \file 
 *  \n
 *  \brief  chg_config.h ---- CHARGER-RELATED PMIC DEFINITION
 *  \n
 *  \details This header file contains various PMIC configurations that are used 
 *  throughout the charger application
 *  \n
 *  \n &copy; Copyright 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/inc/chg_config.h#21 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Removed unnecessary header file inclusion.
08/05/09   hs      Removed AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T.
                   Changed VBATDET from 4100 to 4150.
                   Added AUTO_CHARGING_TRICKLE_T
06/05/09   hs      Added support for software controlled CV topoff.
06/04/09   hs      Added support for the charger debugger.
05/26/09   hs      Added support for charging during call.
05/05/09   hs      Added support for the improved algorithm of end-of-charge detection.
04/23/09   hs      Added support for the resume-charging detection.
04/03/09   hs      Added support for unit tests.
03/31/09   hs      Added CHG_CMD_BUF_CNT for the size charger task command buffer.
                   Increased several buffer sizes.
03/26/09   hs      Removed support for software controlled charging.
03/24/09   hs      Added definition for minimum IMAXSEL setting in autonomous charging.
03/10/09   hs      Added support for end-of-charging workaround.
02/18/09   hs      Added support for pulse charging in the autonomous charging context.
12/15/08   hs      Fixed a typo.
11/25/08   hs      Removed the definitions for overall charging time.
11/19/08   hs      Removed RSENSE_mOhms.
11/18/08   hs      Added some definitions for the revised end-of-charging algorithm
                   Removed AUTO_CHARGING_FAIL_OF_CHARGING_COUNTER_MAX
                   Removed AUTO_CHGARGING_DONE_T
11/13/08   hs      Added support for NiMH battery charging.
09/26/08   vk      Added file to fix c++ compiler warnings
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
04/07/07   hs      Created
===========================================================================*/
#ifndef T_WINNT
#include "pmapp_ads_compat.h"          /* for compiler warnings*/
#endif

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#ifndef T_WINNT
#include "pm.h"                /* For pm functions and type definitions    */
#endif

/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
/*===========================================================================

                               MACROS 

===========================================================================*/
/*! \def CHG_USB_CARKIT_IMAX_MAXIMUM
 *  \brief If we are not allowed to collapse the USB device voltage (VBUS) 
 *  then this the maximum current setting that we area allowed to use. 
 */
#define CHG_USB_CARKIT_IMAX_MAXIMUM                  400
/*! \def CHG_VMAX_TOPOFF
 *  \brief The battery alarm upper threshold (in milli volt) for the topoff State
 */
#define CHG_VMAX_TOPOFF                              4300        
/*! \def CHG_VMIN_TOPOFF
 *  \brief The battery alarm lower threshold (in milli volt) for the topoff State
 */
#define CHG_VMIN_TOPOFF                              4000        
/*! \def CHG_HEART_BEAT_FIRST_TIME
 *  \brief The charger heart beat (in milli seconds). The amount of time to check
 *  the battery and the charger status for the first time
 */
#define CHG_HEART_BEAT_FIRST_TIME                5111
/*! \def CHG_HEART_BEAT
 *  \brief The charger heart beat (in milli seconds). The amount of time to periodically
 *  check the battery and the charger status
 */
#define CHG_HEART_BEAT                           5111
/*! \def CHG_CMD_BUF_CNT 
 *  \brief The size of the charger task command queue
 */
#define CHG_CMD_BUF_CNT                          30
/*! \def CHG_IRQ_LISTENER_BUF_CNT 
 *  \brief The size of the irq event listener buffer
 */
#define CHG_IRQ_LISTENER_BUF_CNT                 15
/*! \def CHG_TIMER_LISTENER_BUF_CNT 
 *  \brief The size of the timer event listener buffer
 */
#define CHG_TIMER_LISTENER_BUF_CNT               15
/*! \def CHG_EVT_LISTENER_BUF_CNT 
 *  \brief The size of the charging event listener buffer
 */
#define CHG_EVT_LISTENER_BUF_CNT                 15
/*! \def CHG_SYS_EVT_LISTENER_BUF_CNT 
 *  \brief The size of the system event listener buffer
 */
#define CHG_SYS_EVT_LISTENER_BUF_CNT             15
/*! \def CHG_TIMER_CNT 
 *  \brief The size of the timer buffer
 */
#define CHG_TIMER_CNT                            10
/*! \def AUTO_CHARGING_SOURCE_DETECTION_TIME_OUT 
 *  \brief The auto charging source detection time out. (500 milli sec) 
 */
#define AUTO_CHARGING_SOURCE_DETECTION_TIME_OUT            501
/*! \def AUTO_CHARGING_SOURCE_DETECTION__UCP 
 *  \brief The auto charigng source is detected to be a UCP.
 */
#define AUTO_CHARGING_SOURCE_DETECTION__UCP                1
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
/*! \def AUTO_CHARGING_SHP_IMAX_MAXIMUM 
 *  \brief The maximum charging current that we are allowed to charge the
 *  NiMH battery when the charging source is a SHP, in automatic charigng
 */
#define AUTO_CHARGING_SHP_IMAX_MAXIMUM                     300
#else
/*! \def AUTO_CHARGING_SHP_IMAX_MAXIMUM 
 *  \brief The maximum charging current that we are allowed to charge the
 *  Li-ion battery when the charging source is a SHP, in automatic charigng
 */
#define AUTO_CHARGING_SHP_IMAX_MAXIMUM                     450
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
/*! \def AUTO_CHARGING_UCP_IMAX_MAXIMUM 
 *  \brief The maximum charging current that we are allowed to charge the
 *  Nimh battery when the charging source is a UCP, in automatic charigng
 *  \note The maximum charging current for NiMH battery should be 0.3C
 */
#define AUTO_CHARGING_UCP_IMAX_MAXIMUM                     300
#else
/*! \def AUTO_CHARGING_UCP_IMAX_MAXIMUM 
 *  \brief The maximum charging current that we are allowed to charge the
 *  Li-ion battery when the charging source is a UCP, in automatic charigng
 */
#define AUTO_CHARGING_UCP_IMAX_MAXIMUM                     500
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
/*! \def AUTO_CHARGING_IMAX_MINIMUM
 *  \brief The minimum valid IMAXSEL (in milli Amp), in automatic charigng
 */
#define AUTO_CHARGING_IMAX_MINIMUM                         50
/*! \def AUTO_CHARGING_UCP_IMAX_TX_ON
 *  \brief The maximum charging current (in milli Amp) that we are allowed to
 *  do autonomous charging when the charging source is a UCP and the system 
 *  is in a call
 */
#define AUTO_CHARGING_UCP_IMAX_TX_ON                       700
/*! \def AUTO_CHARGING_VMAXSEL
 *  \brief The VMAXSEL (in milli volt), in automatic charigng
 */
#define AUTO_CHARGING_VMAXSEL                              4200
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
/*! \def AUTO_CHARGING_VBATDET
 *  \brief The VBATDET (in milli volt), in automatic charigng
 *  \note For NiMH charging, we should set VBATDET at least 100mV higher than VMAXSEL
 */
#define AUTO_CHARGING_VBATDET                              4300
#else
/*! \def AUTO_CHARGING_VBATDET
 *  \brief The VBATDET (in milli volt), in automatic charigng
 */
#define AUTO_CHARGING_VBATDET                              4180
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
/*! \def AUTO_CHARGING_VBATDET
 *  \brief The minimum value of VBATDET (in milli volt), in automatic charigng
 */
#define AUTO_CHARGING_VBATDET_MIN                          2400
/*! \def AUTO_CHARGING_RESUME_DETECTION_VBATDET
 *  \brief The VBATDET (in milli volt) used for resume-charging detetion in automatic 
 *  charigng
 *  \details This VBATDET is used to make sure that momentary high current events do 
 *  not retrigger charging, especially on GSM systems. 
 *  \note This VBATDET setting should only be used to prevent excessive charging when 
 *  the charging is completed normally. The VBATDET should be set to this value when 
 *  the charging is determined completed. The VBATDET should be restored to its normal
 *  setting (::AUTO_CHARGING_VBATDET) when the charging is resumed or the charger is 
 *  reinserted.
 */
#define AUTO_CHARGING_RESUME_DETECTION_VBATDET             3300
/*! \def AUTO_CHARGING_TRICKLE_T
 *  \brief The maximum trickle charging time (in minutes) in the automatic charigng
 */
#define AUTO_CHARGING_TRICKLE_T                            30
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
/*! \def AUTO_CHARGING_ITERM 
 *  \brief The end-of-charging current that used to generate ICHG_END interrupt
 *  and terminate the autonomous charging.
 *  \note The valid value range is 10mA to 160mA. The default value is 20mA. 
 */
#define AUTO_CHARGING_ITERM                                10
#else
/*! \def AUTO_CHARGING_ITERM 
 *  \brief The end-of-charging current that used to generate ICHG_END interrupt
 *  and terminate the autonomous charging.
 *  \note The valid value range is 10mA to 160mA. The default value is 20mA.
 */
#define AUTO_CHARGING_ITERM                                10
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
/*! \def AUTO_CHARGING_T
 *  \brief The charging time (in min) when the battery type is NiMH
 *  \note For NiMH battery, we want to always charge it by setting this value to
 *  maximum (8 hours 32 minutes).
 */
#define AUTO_CHARGING_T                                   512
#else
/*! \def AUTO_CHARGING_T
 *  \brief The charging time (in min) when the battery type is NiMH
 *  \detail For Li-ion battery, this is the timeout used for the end-of-charge software
 *  workaround. When this timer expired, the autonomous charging will be terminated by 
 *  the PMIC hardware. The charger software will start the one-shot end-of-charging detection
 *  timer with the timrout( ::AUTO_CHARGING_END_CHARGE_DETECTION_T ). If this timer expires 
 *  before the battery voltage reaches (::AUTO_CHARGING_VBATDET), the PMIC hardware will 
 *  resume the aunotonous charging.
 */
#define AUTO_CHARGING_T                                   16
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
/*! \def AUTO_CHARGING_MAX_CHARGE_T
 *  \brief The overall fast charge timeout (in minutes) of the autonomous charger
 *  \detail This is the maximum value of the overall charge timeout for the battery to be 
 *  charged up to VMAXSEL ( ::AUTO_CHARGING_VMAXSEL ). When this timer expired, the autonomous
 *  charging should be terminated by the charger software in order to protect the battery
 *  from being charged too long. 
 *  \note The overall charge timeout can be calculated using the following formula:
 *  Max_Charge_Time = battery_Capacity/(charge_current - approx_standby_system_current) + 2hrs
 */  
#define AUTO_CHARGING_MAX_CHARGE_T                        432
/*! \def AUTO_CHARGING_END_CHARGE_DETECTION_T
 *  \brief The end-of-charging detection timeout (in milli seconds) in the autonomous charging
 *  \details This timer is used to detect if the charging is close to completion. This timer is
 *  started at the beginning of every charging cycle; when this timer expires, the software
 *  will check if the VBAT stays above VBATDET (::AUTO_CHARGING_VBATDET); if yes, it will take
 *  that the charging is close to complete. An extra charging cycle (::AUTO_CHARGING_FINAL_CYCLE_T)
 *  will be launched.
 */  
#define AUTO_CHARGING_END_CHARGE_DETECTION_T              60000
/*! \def AUTO_CHARGING_RESUME_CHARGE_DETECTION_T
 *  \brief The resume-charging detection timeout (in milli seconds) in the autonomous charging
 *  \details This timer is used to collect the ADC readings for the battery voltage, in order
 *  to decide if it is necessary to resume autonomous charging.
 *  \note This timer should only be started when the charging is completed normally and the 
 *  VBATDET is lowered to (::AUTO_CHARGING_RESUME_DETECTION_VBATDET). This timer should be stopped
 *  whenever the charging is resumed or the charger is removed from the phone.
 */  
#define AUTO_CHARGING_RESUME_CHARGE_DETECTION_T           61111
/*! \def AUTO_CHARGING_FINAL_CYCLE_T
 *  \brief The charging time (in minutes) for the last charging cycle before the autonomous
 *  charging is considered complete
 *  \details This final charging cycle is used to make sure that the battery is fully charged. 
 */  
#define AUTO_CHARGING_FINAL_CYCLE_T                       32
/*! \def AUTO_CHARGING_RESUME_CHARGE_DETECTION_COUNTER
 *  \brief This value is used in the resume-charging detection routine to decide if the charigng
 *  should resume or not: if 5 consecutive readings of the battery voltage is below
 *  (::AUTO_CHARGING_VBATDET), the charging will resume
 *  \sa AutoChargingManager::EventFired() 
 */  
#define AUTO_CHARGING_RESUME_CHARGE_DETECTION_COUNTER     5
/*! \def CHG_ENABLE_DEBUGGER
 *  \brief This value is used to enable/disable debugger in the charger applicatiojn. 
 *  \details If the value is 1, enable debugger; otherwise, disable debugger. When the debugger is
 *  enabled, charger related F3 messages will be output through QXDM. These debug messages can 
 *  help engineers understand/debug the charging application. 
 *  It is recommented that OEMs turn off the debugger once they finish developing and start launching
 *  the phone in market.
 */  
#define CHG_ENABLE_DEBUGGER                               1
#if defined( FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(T_WINNT)
/*! \def AUTO_CHARGING_PULSE_VMAXSEL
 *  \brief The pulse charger vmaxsel (in mv) in the autonomous charging
 */  
#define AUTO_CHARGING_PULSE_VMAXSEL                       4250
/*! \def AUTO_CHARGING_PULSE_VBATDET
 *  \brief The pulse charger vbatdet (in mv) in the autonomous charging
 */  
#define AUTO_CHARGING_PULSE_VBATDET                       4180
#if defined(FEATURE_PMIC_HIGH_SIDE_PULSE_CHARGING)
/*! \def CHG_TOPOFF_PULSE_TYPE
 *  \brief The pulse charger type during the topoff state
 */  
#define CHG_TOPOFF_PULSE_TYPE                    PM_PULSE_CHG_Q__HIGH_SIDE
#else
#define CHG_TOPOFF_PULSE_TYPE                    PM_PULSE_CHG_Q__LOW_SIDE
#endif
/*! \def CHG_TOPOFF_PULSE_TDONE
 *  \brief The pulse charger t_done during the topoff state
 */  
#define CHG_TOPOFF_PULSE_TDONE                   PM_PULSE_CHG_TDONE__TOFF_X_16
/*! \def CHG_TOPOFF_PULSE_TON
 *  \brief The pulse charger t_on during the topoff state
 */  
#define CHG_TOPOFF_PULSE_TON                     PM_PULSE_CHG_TON__250ms
/*! \def CHG_TOPOFF_PULSE_TOFF
 *  \brief The pulse charger t_off during the topoff state
 */  
#define CHG_TOPOFF_PULSE_TOFF                    PM_PULSE_CHG_TOFF__125ms
#if defined(FEATURE_PMIC_HIGH_SIDE_PULSE_CHARGING) 
/*! \def CHG_MAINT_PULSE_TYPE
 *  \brief The pulse charger type during the maintenance state
 */  
#define CHG_MAINT_PULSE_TYPE                     PM_PULSE_CHG_Q__HIGH_SIDE
#else
#define CHG_MAINT_PULSE_TYPE                     PM_PULSE_CHG_Q__LOW_SIDE
#endif
/*! \def CHG_MAINT_PULSE_TDONE
 *  \brief The pulse charger t_done during the maintenance state
 */  
#define CHG_MAINT_PULSE_TDONE                    PM_PULSE_CHG_TDONE__TOFF_X_128
/*! \def CHG_MAINT_PULSE_TON
 *  \brief The pulse charger t_on during the maintenace state
 */  
#define CHG_MAINT_PULSE_TON                      PM_PULSE_CHG_TON__125ms
/*! \def CHG_MAINT_PULSE_TOFF
 *  \brief The pulse charger t_off during the maintenance state
 */  
#define CHG_MAINT_PULSE_TOFF                     PM_PULSE_CHG_TOFF__500ms
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF */
#if defined( FEATURE_CHG_TASK_USE_CV_TOPOFF ) || defined (T_WINNT)
/*! \def AUTO_CHARGING_END_CHARGE_DETECTION_T
 *  \brief The software controlled end-of-CV(Constant Voltage)-topoff detection timeout 
 *  (in milli seconds) in the autonomous charging (5 minutes)
 *  \details This timer is used to coarsely detect if the software controlled CV (Constant Voltage) 
 *  topoff is close to completion or not. This timer is started at the begging of the CV topoff
 *  procedure; when this timer expires, the software will meature the charging currents before  
 *  and after opennning the battery FET; if the difference of the two measurements is less than 
 *  ::AUTO_CHARGING_END_CV_TOPOFF_I_DELTA, it will start listening for another end-of-CV-topoff 
 *  detection timeout :: AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T to furthure make sure that
 *  the CV topoff is complete.
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T                 300111
/*! \def AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T
 *  \brief The software controlled end-of-CV(Constant Voltage)-topoff final detection timeout 
 *  (in milli seconds) in the autonomous charging (10 sec)
 *  \details This timer is used to detect if the software controlled CV (Constant Voltage) topoff
 *  is truely complete. This timer is started when the difference of the two measurements
 *  (before and after openning  the battery FET) is less than ::AUTO_CHARGING_END_CV_TOPOFF_I_DELTA.
 *  When this timer expires, the software will meature the charging currents before and after 
 *  opennning the battery FET two more times; if the difference of the two measurements is less than 
 *  ::AUTO_CHARGING_END_CV_TOPOFF_I_DELTA, it will decide that the CV topoff is complete.
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T                 10111
/*! \def AUTO_CHARGING_END_CV_TOPOFF_I_DELTA
 *  \brief The difference of the charging currents measured before and after opening the battery FET.
 *  \details This value is used to determine if the CV topoff is complete.
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_I_DELTA                       47
/*! \def AUTO_CHARGING_END_CV_TOPOFF_VCP_CHECK_WAIT
 *  \brief The peoriod that the software has to wait, after enabling the VCP IRQ, before it can
 *  check the RT status of the IRQ.
 *  \details This value is used to make sure that the software will get the correct status of
 *  the VCP IRQ.
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_VCP_CHECK_WAIT                300
/*! \def AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX
 *  \brief How many measurements of the charging current the software will do
 *  \details This value is used to make sure that the software will get the accurate measurement of 
 *  the charging current.
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX            30
/*! \def AUTO_CHARGING_END_CV_TOPOFF_DETECTION_COUNTER_MAX
 *  \brief How many consecutive small diffs of the current measuremenets 
 *  \details This value is used to make sure that the CV topoff is truly ending: ff the charging
 *  current measurements had three consecutive small diffs (less than ::AUTO_CHARGING_END_CV_TOPOFF_I_DELTA
 *  between the current measurements before and after opening the battery FET), it is considered that
 * the CV top-off is complete
 */  
#define AUTO_CHARGING_END_CV_TOPOFF_DETECTION_COUNTER_MAX         2
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */
#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_CONFIG_H */

