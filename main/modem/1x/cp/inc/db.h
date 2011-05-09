#ifndef DB_H
#define DB_H
/*===========================================================================

                                           Database Interface

DESCRIPTION
   This header file contains the definitions necessary to use
   the Database from other modules.

Copyright (c) 1992-2005,1996,1998 by QUALCOMM Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/db.h_v   1.3   29 Mar 2002 10:19:08   hexinw  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/db.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/08   pk      Updating to keep in line with RF changes for 8650 bringup
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
03/09/07   an      Put FEATURE_GPSONE featurization
02/13/07   pg      Make DB_LP_SEC, DB_LTM_OFF, and DB_DAYLT persistent over 
                   power cycle.
11/11/05   fc      Removed privacy_mode from db_items_value_type
11/11/05   sb      Removed voice_privacy (associated with DB_VOICE_PRIVACY).
09/29/05   sab     Removed DB_VOICE_PRIVACY and DB_PRIVACY_MODE.
03/23/05   fc      Renamed FEATURE_RPC to FEATURE_HTORPC_METACOMMENTS.
03/11/05   fc      Changes for RPC.
10/15/03   lh      Included customer.h because of FEATURE_GPSONE.
12/19/02   ph      mainlined FEATURE_QUICK_SCAN.
03/08/02   hxw     Added GPS mode to support MSBased gpsOne.
12/12/01   abh     Added support for gpsOne factory test.
06/20/01   kss     Added DB_RF_HDR to db_rf_mode_type.
08/31/00   cah     (dsb) Added DB_RF_GPS to db_rf_mode_type.
12/10/98   dna     Fix CR7143.  Add new db item for powerup scanning
02/26/99   ejv     Merged changes for FEATURE_QUICK_SCAN.  Two db_acq_mode
                   types are no longer necessary for QUICK_SCAN.
06/25/98   na      Made the following changes to db_voice_as_data_type: Added
                   DB_VOICE_AS_FAX_ONCE, DB_VOICE_AS_FAX_ALWAYS,
                   DB_VOICE_AS_MODEM_ONCE, DB_VOICE_AS_MODEM_ALWAYS. Deleted
                   DB_VOICE_AS_DATA_ONCE, DB_VOICE_AS_DATA_ALWAYS.
04/24/98   dbd     Added DB_VOICE_AS_DATA for service negotiation
02/02/98   dgy/smo Added new interfaces for CDMA pilot acquisition.
12/20/96   rdh     Changed functions to Pascal calling convention to save ROM.
12/21/95   jca     Added DB_BS_P_REV for 13K/8K service negotiation.
10/26/95   jca     Deleted DB_ORIG_MIN_INDEX.
10/04/95   rdh     Separated SLEEP_ACTIVE from SLEEP_MODE.
09/20/95   rdh     Added SLEEP_ACTIVE to tell whether or not we are sleeping.
08/10/95   cah     Added Voice Privacy active flag.
07/06/95   rah     Added 13K_VOC_AVAILABLE for 13kbps vocoder support.
06/29/95   rdh     Enums fixed for acq and sleep mode.
06/28/95   rdh     Added ACQ_MODE for setting acquisition mode.
06/21/95   rdh     Added SLEEP_MODE for disabling sleep.
12/09/94   ras     Added OFF_HOOK for hook switch state.
04/13/94   jjw     Added DB_DATA_SRVC_STATE boolean item
01/24/94   jca     Added DB_RF_900_MHZ to db_rf_mode_type
04/27/93   jai     Deleted DB_SID_NID_LIST item.
04/13/93   jjw     Added DB_SIO_INIT item
03/26/93   jca     Added DB_RF_SLEEP to db_rf_mode_type
09/16/92   ARH     Added DB_VOX
09/09/92   ARH     Added DB_HOLD_ORIG
09/04/92   ARH     Added DB_ORIG_MIN_INDEX
09/02/92   ARH     Added DB_CODE_CHANNEL and DB_PILOT_PN_OFF
07/26/92   ARH     Added DB_RF_MODE
06/20/92   ARH     Added SID_NID_LIST
03/10/92   ARH     Creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef CUST_EDITION  
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
#define INITUIMSMSMASK  0x01
#define INITUIMADDMASK  0x02
#endif
#endif /*CUST_EDITION*/
/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** Enumeration of items in the database
** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

typedef enum {
  DB_IN_USE,               /* Call in progress indication                  */
  DB_NO_SVC,               /* Cannot find cellular service indication      */
  DB_DIGITAL,              /* Indicates Digital service                    */
  DB_CONV,                 /* Indicates Conversation is taking place       */
  DB_ROAM,                 /* Roaming status indication                    */
  DB_CHANNEL,              /* Current channel mobile is tuned to           */
  DB_CODE_CHANNEL,         /* Current code-channel mobile is tuned to      */
  DB_PILOT_PN_OFF,         /* Current pilot PN offset                      */
  DB_SID,                  /* Current System ID                            */
  DB_LOCAID,               /* Current Location ID                          */
  DB_NID,                  /* Current Network ID                           */
  DB_LP_SEC,               /* Leap Seconds since start of System Time      */
  DB_LTM_OFF,              /* Offset local time from System Time           */
  DB_DAYLT,                /* Daylight savings time indicator              */
  DB_DMSS_STATE,           /* DMSS System State                            */
  DB_POWER,                /* Current mobile output power level            */
  DB_RSSI,                 /* Current FM RSSI level                        */
  DB_CDMA_RSSI,            /* Current CDMA RSSI level                      */
  DB_BATTERY_LEVEL,        /* Current Battery level                        */
  DB_TEMPERATURE,          /* Current Temperature                          */
  DB_RF_MODE,              /* Current RF mode (analog, CDMA, PCN)          */
  DB_HOLD_ORIG,            /* Hold incoming origination cmds               */
  DB_POWERUP_SCANNING,     /* Hold origination on powerup                  */
  DB_ON_HOOK,              /* Is the subscriber unit on hook?              */
  DB_VOX,                  /* Current VOX mode                             */
  DB_DATA_SRVC_STATE,      /* Current state of data services task          */
  DB_SLEEP_MODE,           /* Is sleep enabled?                            */
  DB_SLEEP_ACTIVE,         /* Is sleep occurring?                          */
  DB_ACQ_MODE,             /* Type of CDMA acquisition desired             */
  DB_13K_VOC_AVAILABLE,    /* Vocoder 13k availability flag                */
  #ifdef CUST_EDITION  
  DB_VOICE_PRIVACY,        /* Voice Privacy                                */
  #endif /*CUST_EDITION*/
  DB_BS_P_REV,             /* Base Station's protocol revision level       */
  DB_VOICE_AS_DATA,        /* Answer next call as data                     */
  #ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */
#ifdef CUST_EDITION  
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
  DB_UIMINIT_MASK,         /* 卡上数据初始化掩码，由UI设置, UIM 负责清除   */
  DB_UIMSMSINIT,           /* 卡上短信是否初始化完毕                       */
  DB_UIMADDINIT,           /* 卡上电话本是否初始化完毕                     */
#endif
  DB_POWERUPTYPE,          /* 开机方式                                     */
  DB_CAPTURE_WALLPER,
#endif /*CUST_EDITION*/
  DB_LAST_ITEM             /* Internal Use Only                            */
} db_items_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** The following are the types associated with items in the database
** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef CUST_EDITION  
typedef enum 
{
    DB_POWERUP_NONE,
    DB_POWERUP_BYKEY,
    DB_POWERUP_BYEXTERNALPWR,
    DB_POWERUP_BYRTCTIMER,
    DB_POWERUP_BYRESET
} db_powerup_type;
#endif /*CUST_EDITION*/
/*
** Type for DB_ROAM item
*/
typedef enum {
  DB_ROAM_OFF,
  DB_ROAM_ON,
  DB_ROAM_BLINK
} db_roam_type;


/*
** Type for DB_RF_MODE item
*/

typedef enum {
  DB_RF_ANALOG,                   /* RF is in analog (FM) mode of operation */
  DB_RF_CDMA,                     /* RF is in CDMA mode of operation        */
  DB_RF_PCN,                      /* RF is in PCN mode of operation         */
  DB_RF_900MHZ,                   /* RF is in 900 MHz mode of operation     */
  DB_RF_SLEEP,                    /* RF is in sleep mode of operation       */
  DB_RF_GPS,                      /* RF is in GPS mode of operation         */
  DB_RF_HDR,                       /* RF is in HDR mode of operation         */
  DB_RF_HDR_SC = DB_RF_HDR,
  DB_RF_HDR_MC
} db_rf_mode_type;



/*
** Type for DB_DMSS_STATE: System state enumeration.
*/

typedef enum {
  DB_DMSS_POWERUP_S,
  DB_DMSS_ONLINE_ANALOG_S,
  DB_DMSS_ONLINE_DIGITAL_S,
  DB_DMSS_OFFLINE_ANALOG_S,
  DB_DMSS_OFFLINE_DIGITAL_S,
  DB_DMSS_RESET_S,
  DB_DMSS_POWERDOWN_S,
  DB_DMSS_GPS_S
} db_dmss_state_type;


/*
** Type for DB_SLEEP_MODE items
*/

typedef enum {
  DB_SLEEP_ENABLED,               /* All sleep enabled                      */
  DB_SLEEP_ZZ_OFF,                /* Idle sleep disabled                    */
  DB_SLEEP_DEEP_OFF,              /* Deep sleep disabled                    */
  DB_SLEEP_DISABLED               /* All sleep disabled                     */
} db_sleep_mode_type;


/*
** Type for DB_SLEEP_ACTIVE items
*/

typedef enum {
  DB_SLEEP_ON,                    /* We are asleep                          */
  DB_SLEEP_OFF                    /* We are awake                           */
} db_sleep_active_type;


/*
** Type for DB_ACQ_MODE item
*/

typedef enum {
  DB_ACQ_FULL,                    /* Full acquisition desired               */
  DB_ACQ_MINI,                    /* Mini acquisition desired               */
  DB_ACQ_MICRO                    /* Micro acquisition desired              */
} db_acq_mode_type;

/*
** Type for DB_VOICE_AS_DATA item
*/

typedef enum {
  DB_VOICE_AS_DATA_NEVER,         /* Never answer voice call as a data call */
  DB_VOICE_AS_FAX_ONCE,           /* Answer voice as fax once               */
  DB_VOICE_AS_FAX_ALWAYS,         /* Always answer voice as fax             */
  DB_VOICE_AS_MODEM_ONCE,         /* Answer voice as modem once             */
  DB_VOICE_AS_MODEM_ALWAYS        /* Always answer voice as modem           */
} db_voice_as_data_type;
/*DB_CAPTURE_WALLPER*/
#ifdef CUST_EDITION  
typedef enum 
{
    DB_CAPTURE_NONE = 0x00,
    DB_CAPTURE_NEED,
    DB_CAPTURE_INIDLE,
    DB_CAPTURE_MAX
} db_capture_type;
#endif /*CUST_EDITION*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** The following is the union of all types
** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
typedef union {
  boolean               in_use;
  boolean               no_svc;
  boolean               digital;
  boolean               conv;
  db_roam_type          roam;
  word                  channel;
  byte                  code_channel;
  word                  pilot_pn_off;
  word                  sid;
  word                  locaid;
  word                  nid;
  byte                  lp_sec;
  byte                  ltm_off;
  boolean               daylt;
  db_dmss_state_type    dmss_state;
  byte                  power;
  word                  rssi;
  word                  cdma_rssi;
  word                  battery_level;
  word                  temperature;
  db_rf_mode_type       rf_mode;
  boolean               hold_orig;
  boolean               powerup_scanning;
  boolean               on_hook;
  boolean               vox;
  boolean               data_srvc_enabled;
  db_sleep_mode_type    sleep_mode;
  db_sleep_active_type  sleep_active;
  db_acq_mode_type      acq_mode;
  boolean               voc13k_available;
  byte                  bs_p_rev;
  db_voice_as_data_type voice_as_data;
#ifdef FEATURE_GPSONE
#error code not present
#endif
#ifdef CUST_EDITION  
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
  byte                  db_uiminitmask;
  boolean               db_uimsmsinited;
  boolean               db_uimaddinited;
  boolean               db_uimsmsadd_init_done;
#endif
  db_powerup_type       db_poweruptype;
  db_capture_type       b_capture;
#endif /*CUST_EDITION*/
} db_items_value_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE DB_IN_USE            db_items_value_type.in_use */
/*~ CASE DB_NO_SVC            db_items_value_type.no_svc */
/*~ CASE DB_DIGITAL           db_items_value_type.digital */
/*~ CASE DB_CONV              db_items_value_type.conv */
/*~ CASE DB_ROAM              db_items_value_type.roam */
/*~ CASE DB_CHANNEL           db_items_value_type.channel */
/*~ CASE DB_CODE_CHANNEL      db_items_value_type.code_channel */
/*~ CASE DB_PILOT_PN_OFF      db_items_value_type.pilot_pn_off */
/*~ CASE DB_SID               db_items_value_type.sid */
/*~ CASE DB_LOCAID            db_items_value_type.locaid */
/*~ CASE DB_NID               db_items_value_type.nid */
/*~ CASE DB_LP_SEC            db_items_value_type.lp_sec */
/*~ CASE DB_LTM_OFF           db_items_value_type.ltm_off */
/*~ CASE DB_DAYLT             db_items_value_type.daylt */
/*~ CASE DB_DMSS_STATE        db_items_value_type.dmss_state */
/*~ CASE DB_POWER             db_items_value_type.power */
/*~ CASE DB_RSSI              db_items_value_type.rssi */
/*~ CASE DB_CDMA_RSSI         db_items_value_type.cdma_rssi */
/*~ CASE DB_BATTERY_LEVEL     db_items_value_type.battery_level */
/*~ CASE DB_TEMPERATURE       db_items_value_type.temperature */
/*~ CASE DB_RF_MODE           db_items_value_type.rf_mode */
/*~ CASE DB_HOLD_ORIG         db_items_value_type.hold_orig */
/*~ CASE DB_POWERUP_SCANNING  db_items_value_type.powerup_scanning */
/*~ CASE DB_ON_HOOK           db_items_value_type.on_hook */
/*~ CASE DB_VOX               db_items_value_type.vox */
/*~ CASE DB_DATA_SRVC_STATE   db_items_value_type.data_srvc_enabled */
/*~ CASE DB_SLEEP_MODE        db_items_value_type.sleep_mode */
/*~ CASE DB_SLEEP_ACTIVE      db_items_value_type.sleep_active */
/*~ CASE DB_ACQ_MODE          db_items_value_type.acq_mode */
/*~ CASE DB_13K_VOC_AVAILABLE db_items_value_type.voc13k_available */
/*~ CASE DB_BS_P_REV          db_items_value_type.bs_p_rev */
/*~ CASE DB_VOICE_AS_DATA     db_items_value_type.voice_as_data */
/*~ CASE DB_GPS_FACT_TEST     db_items_value_type.gps_fact_test_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION db_put

DESCRIPTION
  Changes the value of the item specified. The tasks that need to know that
  the value of the item has changed are notified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void db_put
(
  db_items_type        item,        /* Item to be changed                  */
  db_items_value_type  *value_ptr   /* Value to change the item to         */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION db_put */
/*~ PARAM value_ptr POINTER DISC item */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* <EJECT> */
/*===========================================================================

FUNCTION db_get

DESCRIPTION
  Retrieve the value of an item from the database


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void db_get
(
  db_items_type         item,     /* Item to get value from                */
  db_items_value_type  *value_ptr /* Buffer to retrieve item into          */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION db_get */
/*~ PARAM OUT value_ptr POINTER DISC item */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION db_init

DESCRIPTION
  Initializes the Database to default values. No notifications are made
  when setting the values to default.


DEPENDENCIES
  None

RETURN VALUE
  None. All values of the database are initialized to a known default
  (Check DB.C for default values)

SIDE EFFECTS
  None

===========================================================================*/
extern void db_init
(
  void
);
#ifdef CUST_EDITION  
extern boolean PhoneBookCache_IsIninited(void);

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
void db_setuiminitmask(byte mask);
byte db_getuiminitmask(void);

// 注意此函数只能由 uim task 调用
void db_removeuiminitmask(byte mask);
#endif
#endif /*CUST_EDITION*/

#endif

