/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件:
//        OEMAnnunciator.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/
#include "customer.h"
#include "comdef.h"

#include "OEMFeatures.h"
#include "AEEComdef.h"
#include "AEE.h"       /* Standard AEE declarations */
#include "AEEShell.h"  /* AEE Shell services, IShell declarations */
#include "AEEStdLib.h" /* AEE StdLib decls */
#include "AEEDisp.h"
#include "AEE_OEM.h"
#include "AEEFile.h"
#include "OEMObjectMgr.h"
#include "AEEAnnunciator.h"

//#include "oemannunciator.brh"
#include "appscommonimages.brh"

#include "OEMDisp.h"
#include "Appscommon.h"
#ifndef WIN32
#include "disp.h"
#include "msg.h"
#include "db.h"
#else
#include "OEMHelperFuncType.h"
#endif
#include "OEMHeap.h"
#include "BREWVersion.h"
#include "AEE_OEMDispatch.h"

#include "OEMClassIDs.h"		

//#include "db.h"
#ifdef FEATURE_RANDOM_MENU_COLOR
#include "OEMConfig.h" 
#include "OEMCFGI.h"
#endif

#include "AEERUIM.h"
//#ifdef FEATURE_MDP
//#error code not present
//#endif
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
#define ANN_BLINK_TIME 1000/*320*/
#else
#define ANN_BLINK_TIME 320
#endif
/* Use a fixed size for the primary display annunciator */
#define ANNUN_PRIMARY_HEIGHT 16

#if ! MIN_BREW_VERSION(3, 0)
#define AEEFS_SHARED_DIR
#endif

//#ifdef FEATURE_MDP
//#error code not present
//#endif

#define ANN_TEXT_MAX_LEN    64

typedef struct IANNUNCore
{
   unsigned short  m_uRefs;
   IShell         *m_piShell;
   IDisplay       *m_piDisplay;
   IDIB           *m_pDDB;
   IDisplay       *m_piDisplay2;                /* secondary display */
   IDIB           *m_pDDB2;                     /* DIB for secondary display */
   boolean         m_bTimer; /* blinking timer set ? */
   uint32          m_pAnnunciators_ovId;     /* Primary Annunciator Overlay ID */
   uint32          m_pAnnunciators2_ovId;    /* Secondary Annunciator Overlay ID */
   NativeColor     m_bg;                    /* Primary Native Background Color */
   NativeColor     m_bg2;                   /* Secondary Native Background Color */
   boolean         m_bAnnunciatorOn;        /* Primary annunciators on? */
   boolean         m_bAnnunciator2On;       /* Secondary annunciators on? */
   uint16          m_annun_prim_lcd_width;   /* Primary LCD width */
   uint16          m_annun_prim_lcd_height;  /* Primary LCD height */
   boolean         cached;
   boolean         m_bActive;
   boolean         m_hasTitleText;
   uint16          m_Title[ANN_TEXT_MAX_LEN+1];
   boolean         m_btoolen;
}IANNUNCore;

struct IAnnunciator
{
   IAnnunciatorVtbl    *pvt;
   IShell              *m_piShell;
   AEECLSID            clsid;
   AEECallback         m_cbSysObj;
   void *              m_pac;
   IANNUNCore         *m_coreObj;
   unsigned short      m_usRef;
   uint32              nAnnunID;
   uint32              nState;  
};

typedef struct AnnunTimerInfo
{
   IAnnunciator *pIAnn;
   uint32 nAnnunID;
   uint32 nState;
}AnnunTimerInfo;

static uint32 IAnnunciator_AddRef(IAnnunciator *pMe);
static uint32 IAnnunciator_Release(IAnnunciator *pMe);
static int IAnnunciator_QueryInterface(IAnnunciator *pif, AEECLSID id, void **ppo);
static int IAnnunciator_GetField(IAnnunciator * pMe, uint32 nAnnunID, uint32 * pnState);
static int IAnnunciator_SetField(IAnnunciator * pMe, uint32 nAnnunID, uint32 nState);
static int IAnnunciator_SetFieldEx(IAnnunciator * pMe, uint32 nAnnunID, void * pData);
static void IAnnunciator_Delete(IAnnunciator *pMe);
static int IAnnunciator_Redraw(IAnnunciator *pMe);
static int IAnnunciator_EnableAnnunciatorBar(IAnnunciator * pIAnnunciator, AEECLSID clsid, boolean bOn);
static int IAnnunciator_GetAnnunciatorBarSize(IAnnunciator * pMe, AEECLSID clsid, AEERect *pRec);
static uint32 OEMAnnunCore_AddRef(void);
static uint32 OEMAnnunCore_Release(void);
static int DrawImageField (IAnnunciator *pMe, uint32 nAnnunID, uint32 nState);
static uint32 GetAnnunFirstState(uint32 nState);

static uint32 GetAnnunStateRank(uint32 nState);

static int ModifyAnnunStateSchedule(IAnnunciator * pMe, uint32 nAnnunID, uint32 *nState);

static int SwitchAnnunState(IAnnunciator * pMe, uint32 nAnnunID, uint32 *nState);
static int IAnnunciator_SetUnblinkTimer(IAnnunciator * pMe, uint32 nAnnunID, uint32 nState, uint32 nTimeMs);
//static int IAnnunciator_SetFieldIsActive(IAnnunciator * pMe , boolean bActive);
static int IAnnunciator_SetFieldIsActiveEx(IAnnunciator *pMe, boolean bActive);
static int IAnnunciator_SetHasTitleText(IAnnunciator *pMe, boolean bHasTitleText);
static int IAnnunciator_SetFieldText(IAnnunciator * pMe ,uint16 *cText);
static void SetFieldUnblink(IAnnunciator * pMe);

IAnnunciatorVtbl gvtIAnnunciator = {
   IAnnunciator_AddRef,
   IAnnunciator_Release,
   IAnnunciator_QueryInterface,
   IAnnunciator_GetField,
   IAnnunciator_SetField,
   IAnnunciator_SetFieldEx,
   IAnnunciator_Redraw,
   IAnnunciator_EnableAnnunciatorBar,
   IAnnunciator_GetAnnunciatorBarSize
   ,IAnnunciator_SetUnblinkTimer
   ,IAnnunciator_SetFieldIsActiveEx
   ,IAnnunciator_SetFieldText,
   IAnnunciator_SetHasTitleText
};

/* OEMAnnun_content defines the text or image that will be displayed
 * for an annunciator field.  This content may be shared between
 * the primary annunciator and the annunciator on any secondary displays.
 */
typedef struct
{
  uint32 nFieldType;             /* image or text   */
  uint32 nMaxStates;             /* max states - valid for image only */
  uint32 nCurrState;             /* current state */
  void *data;                    /* pointer to data - image or text */
} OEMAnnun_content;

/* OEMAnnun_data contains all of the data for an annunciator field. */
typedef struct {
  uint32 nFieldID;               /* Field ID        */
  uint32 x_pos;                  /* starting x-pos  */
  uint32 y_pos;                  /* starting y-pos  */
  uint32 width;                  /* width of field  */
  uint32 height;                 /* height of field */
  OEMAnnun_content *pcontent;    /* Content for the annunciator field. */
} OEMAnnun_data;


// State definitions for each field
typedef struct {
//lint -save -esym(754, nStateID)  Suppress "nStateID not referenced"
  uint32 nStateID;                  /* State ID */
  uint32 nImageResID;               /* bitmap resource ID */
  IBitmap* pBmp;                    /* pointer to image data */
} OEMState_data;


/********************************************************************
 *
 * Image data for the annunciator fields.
 *
 *******************************************************************/
 /*fields that locate at the same place are merged, and use the states ID to dicide which icon to display*/
 /*RSSI/Airplane Mode*/
 static OEMState_data rssi_image_data[]=
{
#ifndef FEATURE_USES_LOWMEM
  {ANNUN_STATE_AIR_MODE_ON, IDB_AIR_MODE, NULL},
#endif
  {ANNUN_STATE_RSSI_NO_SERV, IDB_NO_SERVICE, NULL},
  {ANNUN_STATE_RSSI_0, IDB_RSSI0, NULL},
  {ANNUN_STATE_RSSI_1, IDB_RSSI1, NULL},
  {ANNUN_STATE_RSSI_2, IDB_RSSI2, NULL},
  {ANNUN_STATE_RSSI_3, IDB_RSSI3, NULL},
  {ANNUN_STATE_RSSI_4, IDB_RSSI4, NULL},
#ifdef FEATURE_VERSION_VG68    
  {ANNUN_STATE_RSSI_5, IDB_RSSI5, NULL},
  {ANNUN_STATE_RSSI_6, IDB_RSSI6, NULL}
#endif
#ifdef FEATURE_VERSION_LM126C
  {ANNUN_STATE_RSSI_5, IDB_RSSI5, NULL},
#endif
};

//Add By zzg 2012_10_29
static OEMState_data tcard_image_data[]=
{
	{ANNUN_STATE_TCARD_ON, IDB_SD_CARD, NULL}
};
//Add End

/*1x/Wap/Roam*/
static OEMState_data wap_image_data[]=
{
#ifndef FEATURE_USES_LOWMEM
    {ANNUN_STATE_WAP_ON, IDB_WAP, NULL},
    {ANNUN_STATE_1X_ON, IDB_1X, NULL},
#endif
    {ANNUN_STATE_ROAM_ON, IDB_ROAM, NULL}
};
/*Voice Privacy/Lockstatus*/
static OEMState_data lockstatus_image_data[]=
{
    {ANNUN_STATE_LOCKSTATUS_ON, IDB_LOCKSTATUS, NULL},
    {ANNUN_STATE_VOICE_PRIV_ON, IDB_VOICE_PRIV, NULL}
};
/*Inuse/Loudspeaker/Mute/Emergency/Missed Call*/
static OEMState_data call_image_data[]=
{
    {ANNUN_STATE_CALL_EMERGENCY_ON, IDB_EMERGENCY, NULL},
    {ANNUN_STATE_CALL_MUTE_ON, IDB_MUTE, NULL},
    {ANNUN_STATE_CALL_LOUDSPEAKER_ON, IDB_LOADSPEAKER, NULL},
    {ANNUN_STATE_CALL_INUSE_ON, IDB_INUSE, NULL},
    {ANNUN_STATE_CALL_MISSEDCALL_ON, IDB_MISSED_CALL, NULL}
};
/*SMS Memory Full/VMail/SMS*/
static OEMState_data sms_image_data[]=
{
    {ANNUN_STATE_SMS_MAILFULL_ON, IDB_SMS_FULL, NULL},
    {ANNUN_STATE_SMS_VMAIL_ON, IDB_VMAIL, NULL},
    {ANNUN_STATE_SMS_SMAIL_ON, IDB_SMS, NULL}
};

/*FMRadio/Headset*/
static OEMState_data fmradio_image_data[]=
{
#ifndef FEATURE_USES_LOWMEM
    {ANNUN_STATE_FMRADIO_ON, IDB_FM_RADIO, NULL},
#ifdef FEATURE_QQ_APP         
    {ANNUN_STATE_QQ_MSG_ON, IDB_QQMSG, NULL},
    {ANNUN_STATE_QQ_ONLINE, IDB_QQ, NULL},
    {ANNUN_STATE_QQ_HIDING_ON, IDB_QQHIDING,NULL},
    {ANNUN_STATE_QQ_LEAVE_ON, IDB_QQLEAVE,NULL},    
#endif        
#endif
    {ANNUN_STATE_HEADSET_ON, IDB_HEADSET, NULL}    
};
#ifndef FEATURE_USES_LOWMEM
/*BT Trans/BT Headset/BT On*/
static OEMState_data bluetooth_image_data[]=
{
    {ANNUN_STATE_BT_TRANS_ON, IDB_BT_TRANS, NULL},
    {ANNUN_STATE_BT_HEADSET_ON, IDB_BT_HEADSET, NULL},
    {ANNUN_STATE_BT_ON, IDB_BLUETOOTH, NULL}
};
#endif
/*Timer/Alarm/Schedule*/
static OEMState_data alarm_image_data[]=
{
#ifndef FEATURE_USES_LOWMEM
    {ANNUN_STATE_TIMER_ON, IDB_TIMER, NULL},
#endif
    {ANNUN_STATE_ALARM_ON, IDB_ALARM, NULL},
#ifndef FEATURE_USES_LOWMEM
    {ANNUN_STATE_SCHEDULE_ON, IDB_SCHEDULE, NULL}
#endif
};
#ifndef FEATURE_USES_LOWMEM
/*MMS Full/MMS Unread/MMS Unreceive/Push*/
static OEMState_data mms_image_data[]=
{
    {ANNUN_MMS_FULL_ON, IDB_MMS_FULL, NULL},
    {ANNUN_MMS_UNREAD_ON, IDB_MMS_UNREAD, NULL},
    {ANNUN_MMS_UNRECEIVE_ON, IDB_MMS_UNRECEIVED, NULL},
    {ANNUN_STATE_PUSH_ON, IDB_PUSH_MSG, NULL}
};
#endif
/*Ringtone*/
static OEMState_data ringtone_image_data[]=
{
  {ANNUN_STATE_RINGTONE_ALERT, IDB_ALERT, NULL},
  {ANNUN_STATE_RINGTONE_SILENT, IDB_SILENCE, NULL},
  {ANNUN_STATE_RINGTONE_VIBRING, IDB_VIBRING, NULL},
  {ANNUN_STATE_RINGTONE_VIBRATOR, IDB_VIBRATOR, NULL}
/*  
#ifdef FEATURE_VERSION_C316
  ,{ANNUN_STATE_RINGTONE_BLUETOOTH, IDB_PROFILE_BLUETOOTH, NULL}
#endif
*/
};
/*Battery*/
static OEMState_data batt_image_data[]=
{
  {ANNUN_STATE_BATT_LOW, IDB_BATT_LOW, NULL},
  {ANNUN_STATE_BATT_1, IDB_BATT1, NULL},
  {ANNUN_STATE_BATT_2, IDB_BATT2, NULL},
  {ANNUN_STATE_BATT_3, IDB_BATT3, NULL},
  #ifdef FEATURE_VERSION_LM126C 
  {ANNUN_STATE_BATT_4, IDB_BATT4, NULL},
  #endif
  {ANNUN_STATE_BATT_FULL, IDB_BATT_FULL, NULL}
};

/*QQ
static OEMState_data qq_image_data[]=
{
  {ANNUN_STATE_QQ_MSG_ON, IDB_QQMSG, NULL},
  {ANNUN_STATE_QQ_ONLINE, IDB_QQ, NULL}
};
*/

/********************************************************************
 *
 * Content data for the annunciator fields
 *
 *******************************************************************/
 /*fields that locate at the same place are merged, and use the states ID to dicide which icon to display*/
 /*ANNUN_FIELD_RSSI*/
OEMAnnun_content rssi_content =
#ifndef FEATURE_USES_LOWMEM
#ifdef FEATURE_VERSION_VG68
     {ANNUN_TYPE_IMAGE, 9, ANNUN_STATE_RSSI_NO_SERV, (void *)rssi_image_data};
#else
#ifdef FEATURE_VERSION_LM126C
	 {ANNUN_TYPE_IMAGE, 8, ANNUN_STATE_RSSI_NO_SERV, (void *)rssi_image_data};
#else
     {ANNUN_TYPE_IMAGE, 7, ANNUN_STATE_RSSI_NO_SERV, (void *)rssi_image_data};
#endif
#endif
#else
    {ANNUN_TYPE_IMAGE, 6, ANNUN_STATE_RSSI_NO_SERV, (void *)rssi_image_data};
#endif


//Add By zzg 2012_10_29
/*ANNUN_TCARD_WAP*/
OEMAnnun_content tcard_content =
	{ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)tcard_image_data};
//Add End

/*ANNUN_FIELD_WAP*/
OEMAnnun_content wap_content =
#ifndef FEATURE_USES_LOWMEM
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)wap_image_data};
#else
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)wap_image_data};
#endif
/*ANNUN_FIELD_LOCKSTATUS*/
OEMAnnun_content lockstatus_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)lockstatus_image_data};
/*ANNUN_FIELD_CALL*/
OEMAnnun_content call_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)call_image_data};
/*ANNUN_FIELD_SMS*/
OEMAnnun_content sms_content =
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)sms_image_data};
/*ANNUN_FIELD_FMRADIO*/
OEMAnnun_content fmradio_content =
#ifndef FEATURE_USES_LOWMEM
#ifdef FEATURE_QQ_APP 
     {ANNUN_TYPE_IMAGE, 6, ANNUN_STATE_OFF, (void *)fmradio_image_data};
#else
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)fmradio_image_data};
#endif
#else
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)fmradio_image_data};
#endif
/*ANNUN_FIELD_QQ
OEMAnnun_content qq_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)qq_image_data};
*/

/*ANNUN_FIELD_BLUETOOTH*/
#ifndef FEATURE_USES_LOWMEM
OEMAnnun_content bluetooth_content =
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)bluetooth_image_data};
#endif
/*ANNUN_FIELD_ALARM*/
OEMAnnun_content alarm_content =
#ifndef FEATURE_USES_LOWMEM
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)alarm_image_data};
#else
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)alarm_image_data};
#endif
/*ANNUN_FIELD_MMS*/
#ifndef FEATURE_USES_LOWMEM
OEMAnnun_content mms_content =
     {ANNUN_TYPE_IMAGE, 4, ANNUN_STATE_OFF, (void *)mms_image_data};
#endif
/*ANNUN_FIELD_RINGTONE*/
/*
#ifdef FEATURE_VERSION_C316
OEMAnnun_content ringtone_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)ringtone_image_data};
#else
*/
OEMAnnun_content ringtone_content =
     {ANNUN_TYPE_IMAGE, 4, ANNUN_STATE_OFF, (void *)ringtone_image_data};
//#endif
/*ANNUN_FIELD_BATT*/
#ifdef FEATURE_VERSION_LM126C
OEMAnnun_content batt_content =
     {ANNUN_TYPE_IMAGE, 6, ANNUN_STATE_OFF, (void *)batt_image_data};
#else
OEMAnnun_content batt_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)batt_image_data};
#endif
#ifdef FEATURE_DISP_176X220
    #define IMG_WIDTH      16//12
    #define IMG_HEIGHT     18//13
    #define LG_IMG_WIDTH   27
    #define TEXT_HEIGHT    11
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1
#elif defined(FEATURE_DISP_128X128)
    #define IMG_WIDTH      10
    #define IMG_HEIGHT     13
    #define LG_IMG_WIDTH 20
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1
#elif defined(FEATURE_DISP_128X160)
	#define IMG_WIDTH      12
    #define IMG_HEIGHT     13
    #define LG_IMG_WIDTH   20
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 0
#elif defined(FEATURE_DISP_160X128)
    #define IMG_WIDTH      12
    #define IMG_HEIGHT     13
    #define LG_IMG_WIDTH 20
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1    
#elif defined(FEATURE_DISP_220X176)
    #define IMG_WIDTH      18
    #define IMG_HEIGHT     18
    #define LG_IMG_WIDTH   29
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1  	
#elif defined(FEATURE_DISP_128X160)
    #define IMG_WIDTH      9
    #define IMG_HEIGHT     13
    #define LG_IMG_WIDTH 20
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1 	
#elif defined(FEATURE_DISP_240X320)
    #define IMG_WIDTH        18
    #define IMG_HEIGHT       18
    #define LG_IMG_WIDTH     29
    #define TEXT_HEIGHT      11
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1 	
#elif defined(FEATURE_DISP_320X240)
    #define IMG_WIDTH      19
    #define IMG_HEIGHT     21
    #define LG_IMG_WIDTH 	32
    #define TEXT_HEIGHT    10
    #define ROW1_Y           0
    #define BETWEEN_ICON_PIXEL 1 		
#else
/* Standard image fields are 10 x 10 (pixels)    */
/* while large image fields are 20 x 10 (pixels) */
    #define IMG_WIDTH      10
    #define IMG_HEIGHT     13
    #define LG_IMG_WIDTH 20
    #define TEXT_HEIGHT    11
    #define ROW1_Y           2
    #define BETWEEN_ICON_PIXEL 1
#endif
#define ANNUN_ICON_POSITION_1      (0)
#define ANNUN_ICON_POSITION_2      (LG_IMG_WIDTH + 1*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_3      (LG_IMG_WIDTH + 1*IMG_WIDTH + 2*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_4      (LG_IMG_WIDTH + 2*IMG_WIDTH + 3*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_5      (LG_IMG_WIDTH + 3*IMG_WIDTH + 4*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_6      (LG_IMG_WIDTH + 4*IMG_WIDTH + 5*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_7      (LG_IMG_WIDTH + 5*IMG_WIDTH + 6*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_8      (LG_IMG_WIDTH + 6*IMG_WIDTH + 7*BETWEEN_ICON_PIXEL)
#define ANNUN_ICON_POSITION_9      (LG_IMG_WIDTH + 7*IMG_WIDTH + 8*BETWEEN_ICON_PIXEL)
#if defined(FEATURE_DISP_128X128)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#elif defined(FEATURE_DISP_128X160)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#elif defined(FEATURE_DISP_220X176)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#elif defined(FEATURE_DISP_176X220)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#elif defined(FEATURE_DISP_240X320)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#elif defined(FEATURE_DISP_320X240)
#define ANNUN_ICON_POSITION_10     (DISP_WIDTH - LG_IMG_WIDTH) // - IMG_WIDTH - BETWEEN_ICON_PIXEL)
#else
#define ANNUN_ICON_POSITION_10    (LG_IMG_WIDTH + 8*IMG_WIDTH + 9*BETWEEN_ICON_PIXEL)
#endif
#define ANNUN_ICON_POSITION_END  ((DISP_WIDTH) - (LG_IMG_WIDTH))

/* Row layouts for annunciator, assuming 40-row annunciator region
 * Pixel rows:  First        Last        Annunciator content
 * ---------------------------------------------------------
 *                0            1         -
 *                2           11         Annunciator row 1
 *               12           13         -
 *               14           25         Annunciator row 2
 *               26           27         -
 *               28           37         Annunciator row 3
 *               38           39         -
 */

/* Primary Annunciator layout:
 * First row:  First        Last        Field
 * ---------------------------------------------------
 *               0            1         -
 *               2           11         Mode
 *              12           13         -
 *              14           23         Roam
 *              24           25         -
 *              26           45         RSSI/HDR RSSI
 *              46           47         -
 *              48           67         WLAN RSSI
 *              68           69         -
 *              70           79         In Use
 *              80           81         -
 *              82           91         Net
 *              92           93         -
 *              94          103         Domain
 *             104          105         -
 *             106          125         Batt
 *             126          127         -
  */

/* Annunciator Definitions */
//lint -save -e545  Suppress complaints about "suspicious use of &"
/*The first place is reserved for the signal icon, and the last place is reserved for the battery icon. 
    Those icons between them are arranged one by one, and their positions are now decided by macro*/
/* Q1 Annunciator layout:
 * First row: Rank               Pos                        Field(from higher priority to lower)
 * ---------------------------------------------------
 *               1    ANNUN_ICON_POSITION_1         Airplane Mode/ RSSI
 *               2    ANNUN_ICON_POSITION_2         Lockstatus/Voice Privacy
 *               3    ANNUN_ICON_POSITION_3         Wap/1x/Roam
 *               4    ANNUN_ICON_POSITION_4         Emergency/Mute/Loudspeaker/Inuse/Missed Call
 *               5    ANNUN_ICON_POSITION_5         SMS Memory Full/VMail/SMS
 *               6    ANNUN_ICON_POSITION_6         Push/Sport
 *               7    ANNUN_ICON_POSITION_7         FMRadio/Headset
 *               8    ANNUN_ICON_POSITION_8         BT Trans/BT Headset/BT On
 *               9    ANNUN_ICON_POSITION_9         Timer/Alarm/Schedule
 *              10   ANNUN_ICON_POSITION_10        SD Card
 *              11   ANNUN_ICON_POSITION_11        Ringtone
 *              12   ANNUN_ICON_POSITION_END      Battery
 */
static OEMAnnun_data Annunciators[] =
{
  {ANNUN_FIELD_RSSI,                ANNUN_ICON_POSITION_1,     ROW1_Y,  LG_IMG_WIDTH, IMG_HEIGHT,  &rssi_content}, 		
  {ANNUN_FIELD_WAP,                ANNUN_ICON_POSITION_2,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &wap_content},     
 // {ANNUN_FIELD_QQ,               ANNUN_ICON_POSITION_3,      ROW1_Y,  IMG_WIDTH,       IMG_HEIGHT,  &qq_content},  

#if (defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_C337))
  {ANNUN_FIELD_TCARD, 			  ANNUN_ICON_POSITION_3,	 ROW1_Y,  IMG_WIDTH,	  IMG_HEIGHT,  &tcard_content}, 
#else  
  {ANNUN_FIELD_LOCKSTATUS,     ANNUN_ICON_POSITION_3,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &lockstatus_content},
#endif  
  {ANNUN_FIELD_CALL,                ANNUN_ICON_POSITION_4,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &call_content}, 
  {ANNUN_FIELD_SMS,                ANNUN_ICON_POSITION_5,      ROW1_Y,  IMG_WIDTH,       IMG_HEIGHT,  &sms_content}, 
  {ANNUN_FIELD_FMRADIO,          ANNUN_ICON_POSITION_6,      ROW1_Y,  IMG_WIDTH,       IMG_HEIGHT,  &fmradio_content},
#ifndef FEATURE_USES_LOWMEM
#if defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027)
  {ANNUN_FIELD_BLUETOOTH,       ANNUN_ICON_POSITION_2,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &bluetooth_content},
#else
  {ANNUN_FIELD_BLUETOOTH,       ANNUN_ICON_POSITION_7,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &bluetooth_content},
#endif
#else
  {ANNUN_FIELD_BLUETOOTH,       ANNUN_ICON_POSITION_7,     ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  NULL},
#endif

#ifndef FEATURE_DISP_128X160
#if defined(FEATURE_DISP_176X220)
    {ANNUN_FIELD_ALARM,              ANNUN_ICON_POSITION_7,      ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &alarm_content},
    {ANNUN_FIELD_MMS,                ANNUN_ICON_POSITION_9,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &mms_content},
    {ANNUN_FIELD_RINGTONE,         ANNUN_ICON_POSITION_8,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &ringtone_content},
    {ANNUN_FIELD_BATT,             ANNUN_ICON_POSITION_END, ROW1_Y,  LG_IMG_WIDTH, IMG_HEIGHT,  &batt_content}
#else
  {ANNUN_FIELD_ALARM,               ANNUN_ICON_POSITION_8,      ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &alarm_content},
#ifndef FEATURE_USES_LOWMEM
  {ANNUN_FIELD_MMS,                ANNUN_ICON_POSITION_9,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &mms_content},
#else
  {ANNUN_FIELD_MMS,                ANNUN_ICON_POSITION_9,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  NULL},
#endif
  {ANNUN_FIELD_RINGTONE,            ANNUN_ICON_POSITION_10,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &ringtone_content},
  {ANNUN_FIELD_BATT,                ANNUN_ICON_POSITION_END, ROW1_Y,  LG_IMG_WIDTH, IMG_HEIGHT,  &batt_content}

#endif
#else
 
  {ANNUN_FIELD_ALARM,              ANNUN_ICON_POSITION_7,      ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &alarm_content},
  {ANNUN_FIELD_MMS,                ANNUN_ICON_POSITION_9,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &mms_content},
  {ANNUN_FIELD_RINGTONE,         ANNUN_ICON_POSITION_8,    ROW1_Y,  IMG_WIDTH,      IMG_HEIGHT,  &ringtone_content},
  {ANNUN_FIELD_BATT,             ANNUN_ICON_POSITION_END, ROW1_Y,  LG_IMG_WIDTH, IMG_HEIGHT,  &batt_content}
#endif

};
//lint +e545
#define ANNUN_BUFFER_BITS    (10)
#ifdef FEATURE_VERSION_VG68
#define ANNUN_MAX_STATES     (9)//(7)
#else
#ifdef FEATURE_VERSION_LM126C
#define ANNUN_MAX_STATES     (8)
#else
#define ANNUN_MAX_STATES     (7)
#endif
#endif
/* Status of bitmap cache */
static IANNUNCore *IAnnunCoreObj = NULL;

#ifdef FEATRUE_SET_ANN_FULL_SCREEN
// 用于保存待机界面墙纸顶层图标栏覆盖部分墙纸的图片数据
static IDIB *pgWallpaperBarDDB=NULL;
#endif
/*
  ||
  ||              Helper functions
  ||
  ||
 */

/*===========================================================================

FUNCTION: annun_disp_update

DESCRIPTION
        This function is to display Annunciator Image on the screen

        For FEATURE_MDP, it registeres Annunciator Image into the MDP overlay
        image list and then updates it on the screen.

        Other applications can disable or enable registered overlay images.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/

//#ifdef FEATURE_MDP
//#error code not present
//#endif

//lint -save -esym(715, src_width)
//lint -save -esym(715, src_starting_row)
//lint -save -esym(715, src_starting_column)
//lint -save -esym(715, dst_starting_row)
//lint -save -esym(715, dst_starting_column)
//lint -save -esym(715, m_bgColor)
static void annun_disp_update(
  IAnnunciator *pMe,
  IDIB *dib_ptr,
  uint32 nAnnunID)
{	
  if(NULL == pMe->m_coreObj)
  {
    return;
  }
  if (dib_ptr == pMe->m_coreObj->m_pDDB)
  {
    // We're updating the primary display
#if 0//def FEATURE_MDP
#error code not present
#else
    if(pMe->m_coreObj->m_bAnnunciatorOn)
    {
       // No overlays supported
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
       IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       Annunciators[nAnnunID].width,
                       Annunciators[nAnnunID].height,
                       IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       AEE_RO_TRANSPARENT);
#else
       IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       Annunciators[nAnnunID].width,
                       Annunciators[nAnnunID].height,
                       IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       AEE_RO_COPY);
#endif
       IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
    }
#endif
  }
  else
  {
    // Unsupported display bitmap for annunciator 
    MSG_LOW("Unsupported display bitmap for IAnnunciator.", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION: ClearField

DESCRIPTION
  Clears a field in the annunciator bar


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
static int ClearField (IAnnunciator *pMe, uint32 nAnnunID)
{
    AEERect Rect;
    int nWidth, nHeight;
    IBitmap *pIBitmap = NULL;
    db_items_value_type  need_capture;
    if (pMe == NULL) 
    {
        return EFAILED;
    }

    if(NULL == pMe->m_coreObj)
    {
        return EFAILED;
    }

    if (pMe->m_coreObj->m_pDDB == NULL) 
    {
        return EFAILED;
    }
    nWidth = (int)Annunciators[nAnnunID].width;
    nHeight = (int)Annunciators[nAnnunID].height;
    SETAEERECT(&Rect, Annunciators[nAnnunID].x_pos,
             Annunciators[nAnnunID].y_pos, nWidth, nHeight);
    if(Annunciators[nAnnunID].pcontent->nCurrState == ANNUN_STATE_OFF)
    {
        /* nothing to do */
        return SUCCESS;
    }
    db_get(DB_CAPTURE_WALLPER,&need_capture);
    if (pMe->m_coreObj->m_bAnnunciatorOn)
    {// 清除显示色备上的图标--此时需判断显示设备图标状态
        if (need_capture.b_capture == DB_CAPTURE_INIDLE)
        {
            // 待机界面下以拷贝下来的墙纸数据更新显示设备
            if (NULL != pgWallpaperBarDDB)
            {
                IBitmap *pDevBmp=NULL;
                IBitmap *pBmp =NULL;
                
                if (IDISPLAY_GetDeviceBitmap(pMe->m_coreObj->m_piDisplay, &pDevBmp) == SUCCESS)
                {
                    pBmp = IDIB_TO_IBITMAP(pgWallpaperBarDDB);
                    
                    (void)IBITMAP_BltIn(pDevBmp, 
                                        Annunciators[nAnnunID].x_pos, Annunciators[nAnnunID].y_pos, nWidth, nHeight, pBmp, 
                                        Annunciators[nAnnunID].x_pos, Annunciators[nAnnunID].y_pos, AEE_RO_COPY);
                                        
                    IBITMAP_Release(pDevBmp);
                    IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
                }
            }
        }
        else
        {
            RGBVAL nBgColor = RGB_BLACK;
#ifdef FEATURE_RANDOM_MENU_COLOR
            byte     nRandomMenu = 0;

            (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));
            if(nRandomMenu != 0)
            {
                (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, &nBgColor, sizeof(nBgColor));
            }
#endif
            // 此时应用填充色，而非背景色, 且填充的是显示设备
            IDISPLAY_FillRect(pMe->m_coreObj->m_piDisplay, &Rect, nBgColor);
            IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
        }
    }
    
    // 以图标层背景色清除图标层位图相应位置的图标数据--此时无需判断显示设配图标状态，操作必须进行
    pIBitmap = IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB);
    IBITMAP_FillRect(pIBitmap, &Rect, pMe->m_coreObj->m_bg, AEE_RO_COPY);


	IAnnunciator_Redraw(pMe);	//Add By zzg 2010_07_30
    return SUCCESS;
}

#else

static int ClearField (IAnnunciator *pMe, uint32 nAnnunID)
{
    AEERect Rect;
    int nWidth, nHeight;
    IBitmap *pIBitmap = NULL;
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    db_items_value_type  need_capture;
#endif

    if (pMe == NULL) 
    {
        return EFAILED;
    }

    if(NULL == pMe->m_coreObj)
    {
        return EFAILED;
    }

    if (pMe->m_coreObj->m_pDDB == NULL) 
    {
        return EFAILED;
    }
    
    //if (nAnnunID == ANNUN_FIELD_FMRADIO)
    //{
    //    if (ANNUN_STATE_ON == Annunciators[ANNUN_FIELD_HEADSET].pcontent->nCurrState) 
    //    {
    //        /* nothing to do */
    //        return SUCCESS;
    //    }
    //}
    nWidth = (int)Annunciators[nAnnunID].width;
    nHeight = (int)Annunciators[nAnnunID].height;
    {
        SETAEERECT(&Rect, Annunciators[nAnnunID].x_pos,
                 Annunciators[nAnnunID].y_pos, nWidth, nHeight);
        // Update the annunciator on the primary display
        pIBitmap = IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB);

        IBITMAP_FillRect (pIBitmap, &Rect, pMe->m_coreObj->m_bg, AEE_RO_COPY);
        annun_disp_update (pMe, pMe->m_coreObj->m_pDDB, nAnnunID);
    }
    //annun_disp_update (pMe, pMe->m_coreObj->m_pDDB, nAnnunID);

    return SUCCESS;
}
#endif


/*===========================================================================

FUNCTION: DrawImageField

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int DrawImageField (IAnnunciator *pMe, uint32 nAnnunID, uint32 nState)
{
    OEMState_data* data_ptr;
    int nWidth, nHeight;
    IBitmap *pBmp = NULL;
    IImage  *pBackBmp = NULL;
    int i =0;
	int j;
    boolean bUpdate = TRUE;
    uint32 nFirstState = GetAnnunFirstState(nState);
    if ((pMe == NULL))
        return EFAILED;	

    if(NULL == pMe->m_coreObj)
    {
        return EFAILED;
    }

    //Add By zzg 2010_07_23
    if(IAnnunCoreObj->m_hasTitleText && IAnnunCoreObj->m_Title[0] != 0)
    {
		if ((nAnnunID > ANNUN_FIELD_RSSI) && (nAnnunID < ANNUN_FIELD_BATT))
		{
			bUpdate = FALSE;
		}
		if(IAnnunCoreObj->m_btoolen)
		{
			bUpdate = FALSE;
		}
    }
    //Add End
  
    if ((pMe->m_coreObj->m_piDisplay == NULL) || (pMe->m_piShell == NULL) ||
      (pMe->m_coreObj->m_pDDB == NULL))
        return EFAILED;

    if (nFirstState == 0) //nState
    {
        // Shouldn't be here
        return EFAILED;
    }
    //no need to clear battery field
    //else if((nAnnunID != ANNUN_FIELD_BATT) &&
    //        (nFirstState != GetAnnunFirstState(Annunciators[nAnnunID].pcontent->nCurrState)))
    {//if state changed to another one, field needed to be cleared before drawing a new one
        ClearField(pMe, nAnnunID);
    }
  
    nWidth = (int)Annunciators[nAnnunID].width;
    nHeight = (int)Annunciators[nAnnunID].height;
 
    if (!pMe->m_coreObj->cached) 
    {
#if defined(FEATURE_VERSION_C316)								
		IRUIM               *pIRUIM;
		int nRet = 0;
	    nRet = ISHELL_CreateInstance(pMe->m_piShell,
	                                 AEECLSID_RUIM,
	                                 (void **) &pIRUIM);
	    if (nRet == SUCCESS) 
	    {		
			if((pIRUIM!= NULL)  && !IRUIM_IsCardConnected(pIRUIM))
			{
				i = 1;
		        IRUIM_Release(pIRUIM);
		        pIRUIM = NULL;
			}	
	    }
#endif	    
        // Cache all the bitmaps 
        for (; i < (int)ARR_SIZE(Annunciators); i++) 
	    {
            if (Annunciators[i].pcontent->nFieldType == ANNUN_TYPE_IMAGE) 
	        {	  	
                data_ptr = (OEMState_data *) Annunciators[i].pcontent->data;
                for (j=0; j < (int)Annunciators[i].pcontent->nMaxStates; j++) 
		        {
			        //Modify by zzg 2010_10_12
			        //AEEFS_SHARED_DIR"oemannunciator.bar", 
			        // AEEFS_SYS_DIR"appscommonimages.bar",
                    data_ptr->pBmp = ISHELL_LoadResBitmap (pMe->m_piShell,                                         
                                         AEE_APPSCOMMONRES_IMAGESFILE,
                                         (uint16)(data_ptr->nImageResID));
                    data_ptr++;
                }
            }
        }
        pMe->m_coreObj->cached = TRUE;
    }

    data_ptr = (OEMState_data *) Annunciators[nAnnunID].pcontent->data;
  
    //pBmp = data_ptr[nState-1].pBmp;
     pBmp = data_ptr[nFirstState-1].pBmp;
  
    if (pBmp == NULL) 
    {
        return EFAILED;
    }
  
	// First update the primary display
	IDISPLAY_SetDestination (pMe->m_coreObj->m_piDisplay, IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB));
	IDISPLAY_BitBlt (pMe->m_coreObj->m_piDisplay,
           (int)Annunciators[nAnnunID].x_pos,
           (int)Annunciators[nAnnunID].y_pos,
           nWidth, nHeight, pBmp, 0, 0, AEE_RO_COPY);
	IDISPLAY_SetDestination(pMe->m_coreObj->m_piDisplay, NULL); // restore the destination 

    if(bUpdate)
    {
	    annun_disp_update (pMe, pMe->m_coreObj->m_pDDB, nAnnunID);
    }
  	return SUCCESS;
}

/*===========================================================================

FUNCTION: BlinkTimerCB

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void BlinkTimerCB (void *data)
{
  static boolean bDraw = TRUE;
  int i;
  IAnnunciator *pMe = (IAnnunciator *) data;

  if (pMe == NULL) 
  {
    return;
  }

  /* Clear the blinking fields on primary annunciator */
  for (i=0; i < (int)ARR_SIZE(Annunciators); i++)
  {
    if (Annunciators[i].pcontent->nCurrState & ANNUN_STATE_BLINK) 
	{
      if (bDraw) 
	  {
        if (ClearField(pMe, Annunciators[i].nFieldID) != SUCCESS)
        {
          MSG_LOW("Unable to clear field %d for annunciator blink.", i, 0, 0);
        }
      }
      else 
	  {
        if (DrawImageField (pMe, Annunciators[i].nFieldID,
                           (Annunciators[i].pcontent->nCurrState &
                            ANNUN_STATE_BLINK_UNMASK))
            != SUCCESS)
        {
          MSG_LOW("Unable to draw field %d for annunciator blink, state %d.",
                  i, Annunciators[i].pcontent->nCurrState, 0);
        }
      }
    }
  }

  /* There's no need to explicitly update the blink on the secondary display,
   * as the "draw" and "clear" routines handle both displays.
   */

  /* Toggle the static "draw" flag to make the blink work. */
  bDraw = !bDraw;
  ISHELL_SetTimer(pMe->m_piShell, ANN_BLINK_TIME, (PFNNOTIFY) BlinkTimerCB, pMe);
}

/*===========================================================================

FUNCTION: SetBlinkTimer

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
//lint -save -esym(715,nAnnunID)  Suppress complaint about unused nAnnunID
static void SetBlinkTimer (IAnnunciator* pMe, uint32 nAnnunID)
{
  if (pMe == NULL) {
    return;
  }

  if(NULL == pMe->m_coreObj)
  {
    return;
  }

  if (!pMe->m_coreObj->m_bTimer) {
    ISHELL_SetTimer(pMe->m_piShell, ANN_BLINK_TIME, (PFNNOTIFY) BlinkTimerCB, pMe);
    pMe->m_coreObj->m_bTimer = TRUE;
  }
}
//lint -esym(715,nAnnunID)

/*===========================================================================

FUNCTION: CancelBlinkTimer

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void CancelBlinkTimer (IAnnunciator* pMe, uint32 nAnnunID)
{
  int i;
  boolean found = FALSE;

  if (pMe == NULL) {
    return;
  }

  if(NULL == pMe->m_coreObj)
  {
    return;
  }

  if (pMe->m_coreObj->m_bTimer) {
    for (i=0; i < (int)ARR_SIZE(Annunciators);i++) {
      if ((Annunciators[i].pcontent->nCurrState & ANNUN_STATE_BLINK) &&
          (i != (int) nAnnunID)) {
        found = TRUE;
        break;
      }
    }

    /* For now, we don't need to explicitly set anything on the secondary
     * annunciator, since the content is shared with the primary annunciator.
     */

    if (!found) {
      /* Cancel timer */
      ISHELL_CancelTimer (pMe->m_piShell, NULL, pMe);
      pMe->m_coreObj->m_bTimer = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION: GetAnnunFirstState

DESCRIPTION
  Get the state of the highest priority in the schedule.

DEPENDENCIES
  nState[in]: 

RETURN VALUE
  uint32

SIDE EFFECTS
  none
===========================================================================*/
static uint32 GetAnnunFirstState(uint32 nState)
{
    uint32 firstState = 0, tempState = (nState & ANNUN_STATE_BLINK_UNMASK);

    if((tempState & ANNUN_TURN_OFF_STATE) || (tempState == ANNUN_STATE_OFF))
    {
        firstState = 0;
    }
    else if(tempState & ANNUN_STATE_ON)
    {
        firstState = 1;
    }
    else
    {
        int i;

        for(i = 1; i < ANNUN_MAX_STATES; i++, tempState = tempState>>1)
        {
            if(tempState & ANNUN_FIRST_STATE)
            {
                break;
            }
        }

        firstState = i;
    }

    return firstState;
}

/*===========================================================================

FUNCTION: GetAnnunStateRank

DESCRIPTION
  Get the rank of a certain state.

DEPENDENCIES
  nState[in]: 

RETURN VALUE
  uint32

SIDE EFFECTS
  none
===========================================================================*/
static uint32 GetAnnunStateRank(uint32 nState)
{
    uint32 rank = 0, tempState = (nState & ANNUN_STATE_BLINK_UNMASK);

    if((tempState & ANNUN_TURN_OFF_STATE) || (tempState == ANNUN_STATE_OFF))
    {
        rank = 0;
    }
    else
    {
        int i;
        uint32 lastState = ANNUN_FIRST_STATE<<(ANNUN_MAX_STATES - 1);

        for(i = ANNUN_MAX_STATES; i > 1; i--, tempState = tempState<<1)
        {
            if(tempState & lastState)
            {
                break;
            }
        }

        rank = i;
    }

    return rank;
}

/*===========================================================================

FUNCTION: ModifyAnnunStateSchedule

DESCRIPTION
  Deal with turned off a state event

DEPENDENCIES
  pMe[in]
  nAnnunID[in]
  nState[in]

RETURN VALUE
  int

SIDE EFFECTS
  none
===========================================================================*/
static int ModifyAnnunStateSchedule(IAnnunciator * pMe, uint32 nAnnunID, uint32 *nState)
{
    if(*nState == ANNUN_STATE_OFF)
    {
        return SUCCESS;
    }
    else if(!(*nState & ANNUN_TURN_OFF_STATE))
    {
        return EFAILED;
    }
    else
    {
        uint32 tempState = 0;
#ifndef FEATURE_USES_LOWMEM
        /*deal with bluetooth close*/
        if(nAnnunID == ANNUN_FIELD_BLUETOOTH && *nState == ANNUN_STATE_BT_OFF)
        {
            *nState = ANNUN_STATE_OFF;
            return SUCCESS;
        }
#endif
        if(IAnnunciator_GetField(pMe, nAnnunID, &tempState) != SUCCESS)
        {
            return EFAILED;
        }

        /*if incoming state is going to cancel a state''s blinking but no to cancel it*/
        if(!(*nState & ANNUN_STATE_BLINK))
        {
            if(tempState & ANNUN_STATE_BLINK)
            {
                /*if object is the current blikning state, cancel the blink bit only*/
                if(!((tempState & ANNUN_STATE_BUFFERED_UNMASK) & (*nState)))
                {
                    if(tempState & ANNUN_STATE_BUFFERED_MASK)
                    {
                        tempState |= tempState>>ANNUN_BUFFER_BITS;
                    }
                    tempState &= ANNUN_STATE_BUFFERED_UNMASK;
                    *nState = tempState;
                    return SUCCESS;
                }
            }
            return EFAILED;
        }
        
        /*if buffer state exists, merge it*/
        if(tempState & ANNUN_STATE_BUFFERED_MASK)
        {/*if currently an icon blinking and the incoming state is not going to turn off this blinking state,
            current blinking state should be preserved*/
            if((tempState & ANNUN_STATE_BLINK) &&
                ((tempState & ANNUN_STATE_BUFFERED_UNMASK) & (*nState)))
            {
                *nState &= (tempState>>ANNUN_BUFFER_BITS);
                *nState = *nState<<ANNUN_BUFFER_BITS;
                *nState |= ANNUN_STATE_BUFFERED_UNMASK;
            }
            /*merge buffer bit and cancel blink bit*/
            else
            {
                tempState |= (tempState>>ANNUN_BUFFER_BITS);
                tempState &= ANNUN_STATE_BUFFERED_UNMASK;
            }
        }
        
        *nState &= tempState;
        return SUCCESS;
    }
}


/*===========================================================================

FUNCTION: SwitchAnnunState

DESCRIPTION
  Deal with turned on a state event

DEPENDENCIES
  pMe[in]
  nAnnunID[in]
  nState[in]

RETURN VALUE
  int

SIDE EFFECTS
  none
===========================================================================*/
static int SwitchAnnunState(IAnnunciator * pMe, uint32 nAnnunID, uint32 *nState)
{
    if((*nState & ANNUN_TURN_OFF_STATE) || (*nState == ANNUN_STATE_OFF))
    {
        return EFAILED;
    }
    /*handle exception field first*/
    else if((nAnnunID == ANNUN_FIELD_RINGTONE) ||
            (nAnnunID == ANNUN_FIELD_BATT))
    {
        return SUCCESS;
    }
    else
    {
        uint32 tempState = 0;

        if(IAnnunciator_GetField(pMe, nAnnunID, &tempState) != SUCCESS)
        {
            return EFAILED;
        }
        
        /*deal with signal changes*/
        if((nAnnunID == ANNUN_FIELD_RSSI) &&
            (*nState != ANNUN_STATE_AIR_MODE_ON))
        {/*if current state airplane mode on, merge it*/
            if(tempState & ANNUN_STATE_AIR_MODE_ON)
            {
                *nState |= ANNUN_STATE_AIR_MODE_ON;
            }
            return SUCCESS;
        }
        /*deal with mute and loudspeaker change at non-emergency mode*/
        else if((nAnnunID == ANNUN_FIELD_CALL) &&
                (!(tempState & ANNUN_STATE_CALL_EMERGENCY_ON)))
        {/*if mute has been turned on, current state should be stored
            in buffer bit before turn on loudspeaker*/
            if((tempState & ANNUN_STATE_CALL_MUTE_ON) && (*nState == ANNUN_STATE_CALL_LOUDSPEAKER_ON))
            {/*if buffer state exists, merge it*/
                if(tempState & ANNUN_STATE_BUFFERED_MASK)
                {
                    tempState |= (tempState>>ANNUN_BUFFER_BITS);
                    tempState &= ANNUN_STATE_BUFFERED_UNMASK;
                }
                tempState = tempState<<ANNUN_BUFFER_BITS;
            }
        }
        /*deal with a state blink event, store current state
            in buffer bit before set new blink icon on*/
        else if(*nState & ANNUN_STATE_BLINK)
        {/*if buffer state exists, merge it*/
            if(tempState & ANNUN_STATE_BUFFERED_MASK)
            {
                tempState |= (tempState>>ANNUN_BUFFER_BITS);
                tempState &= ANNUN_STATE_BUFFERED_UNMASK;
            }
            tempState = tempState<<ANNUN_BUFFER_BITS;
        }
        /*deal with turning on a state when current state blinking,
            store the incoming state in buffer bit*/
        else if(tempState & ANNUN_STATE_BLINK)
        {
            *nState = *nState<<ANNUN_BUFFER_BITS;
        }
        
        *nState |= tempState;
        return SUCCESS;
    }
}

/*===========================================================================

FUNCTION: OEM_FreeAnnunciator

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_FreeAnnunciator(IAnnunciator *pMe)
{
   if (NULL != pMe) {
    IAnnunciator_Delete(pMe);
   }
}


/*=============================================================================
FUNCTION: OEMAnnunCore_New

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static IANNUNCore* OEMAnnunCore_New(IShell* piShell)
{
  IBitmap *pDevBmp=NULL;
  IBitmap *pBmp =NULL;
#if 1//ndef FEATURE_MDP
  IBitmapCtl *pBitmapCtl;
#endif
  AEERect Rect;
  IBitmap *pConvertedBmp = NULL;
  disp_info_type dinfo;


  if (IAnnunCoreObj) {
      (void)OEMAnnunCore_AddRef();

  } else {

    IAnnunCoreObj = (IANNUNCore *)OEM_Malloc(sizeof(IANNUNCore));
    if (!IAnnunCoreObj)
       return NULL;

    IAnnunCoreObj->m_piShell = piShell;
    ISHELL_AddRef(piShell);

    IAnnunCoreObj->m_uRefs = 1;
	IAnnunCoreObj->m_bActive = TRUE;
    IAnnunCoreObj->m_hasTitleText = TRUE;
    IAnnunCoreObj->m_bTimer = FALSE;
	MEMSET(IAnnunCoreObj->m_Title,0,sizeof(IAnnunCoreObj->m_Title));

    IAnnunCoreObj->m_pAnnunciators_ovId = NULL;
    IAnnunCoreObj->m_pAnnunciators2_ovId = NULL;

    IAnnunCoreObj->m_bAnnunciatorOn = TRUE;
    IAnnunCoreObj->m_bAnnunciator2On = TRUE;
    IAnnunCoreObj->m_piDisplay = NULL;
    IAnnunCoreObj->m_pDDB = NULL;
    IAnnunCoreObj->m_piDisplay2 = NULL;
    IAnnunCoreObj->m_btoolen    = FALSE;

//
// configuring primary LCD width
#ifndef WIN32
    dinfo = disp_get_info();
    IAnnunCoreObj->m_annun_prim_lcd_width = dinfo.disp_width;
    IAnnunCoreObj->m_annun_prim_lcd_height = STATEBAR_HEIGHT; //ANNUN_PRIMARY_HEIGHT;
#else
	IAnnunCoreObj->m_annun_prim_lcd_width = DISP_WIDTH;
    IAnnunCoreObj->m_annun_prim_lcd_height = STATEBAR_HEIGHT; //ANNUN_PRIMARY_HEIGHT;
#endif
//
// configuring secondary LCD width

    if (ISHELL_CreateInstance(IAnnunCoreObj->m_piShell, AEECLSID_DISPLAY1,
                          (void **)&IAnnunCoreObj->m_piDisplay)) {

       (void)OEMAnnunCore_Release();
       return NULL;
    }

    // Get the device bitmap to create a DDB
    if (IDISPLAY_GetDeviceBitmap(IAnnunCoreObj->m_piDisplay, &pDevBmp) != SUCCESS) {
      IAnnunCoreObj->m_piDisplay = NULL;
      (void)OEMAnnunCore_Release();
      return NULL;
    }

#if 1//ndef FEATURE_MDP
    if(SUCCESS == IBITMAP_QueryInterface(pDevBmp, AEECLSID_BITMAPCTL, (void **) &pBitmapCtl))
    {
      IBITMAPCTL_Enable(pBitmapCtl, TRUE);
      IBITMAPCTL_Release(pBitmapCtl);
    }
#endif

    IBITMAP_CreateCompatibleBitmap(pDevBmp, &pBmp,
                                   (uint16)IAnnunCoreObj->m_annun_prim_lcd_width,
                                   (uint16)IAnnunCoreObj->m_annun_prim_lcd_height);
    IBITMAP_Release(pDevBmp);

    if (pBmp == NULL) {
      (void)OEMAnnunCore_Release();
      return NULL;
    }
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    IBITMAP_SetTransparencyColor(pBmp,   IBITMAP_RGBToNative(pBmp, RGB_MASK_COLOR));
#endif
    if (IBITMAP_QueryInterface (pBmp, AEECLSID_DIB, (void **) &IAnnunCoreObj->m_pDDB) !=
        SUCCESS) {
      IBITMAP_Release (pBmp);
      IDISPLAY_Release(IAnnunCoreObj->m_piDisplay);
      (void)OEMAnnunCore_Release();
      return NULL;
    }
    // clear annunciator bitmap area
    pConvertedBmp = IDIB_TO_IBITMAP(IAnnunCoreObj->m_pDDB);
    SETAEERECT(&Rect, 0,0, IAnnunCoreObj->m_annun_prim_lcd_width, IAnnunCoreObj->m_annun_prim_lcd_height);
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    // 这里的背景色对各种情况下通用，与填充色是不同概念。如待机下是图片一部分，其它应用下为单色。
    // 在重画图标栏时，拷贝整个图标位图区域到显示区域是透明背景色不会覆盖已有显示区域。
    IAnnunCoreObj->m_bg = IBITMAP_RGBToNative (pConvertedBmp, RGB_MASK_COLOR);
    IBITMAP_FillRect (pConvertedBmp, &Rect, IAnnunCoreObj->m_bg, AEE_RO_COPY);
#else    
    IAnnunCoreObj->m_bg = IBITMAP_RGBToNative (pConvertedBmp, RGB_BLACK);//RGB_WHITE);
    IBITMAP_FillRect (pConvertedBmp, &Rect, IAnnunCoreObj->m_bg, AEE_RO_COPY);
    IDISPLAY_FillRect(IAnnunCoreObj->m_piDisplay, &Rect, RGB_BLACK);
#endif    

    IBITMAP_Release (pBmp);
  }

  return IAnnunCoreObj;
}

/*=============================================================================
FUNCTION: OEMAnnunCore_AddRef

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMAnnunCore_AddRef(void)
{
  IANNUNCore *pMe = IAnnunCoreObj;

  if (pMe)
    return ++pMe->m_uRefs;
  else
    return 0;
}


/*=============================================================================
FUNCTION: OEMAnnunCore_Release

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMAnnunCore_Release()
{
  IANNUNCore *pMe = IAnnunCoreObj;
  OEMState_data *data_ptr;
  int i, j;

  if (!pMe)
    return 0;

  if (pMe->m_uRefs > 0) {
    if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
    }
  }

  if (pMe->m_bTimer) {
      ISHELL_CancelTimer(pMe->m_piShell, NULL, pMe);
      pMe->m_bTimer = FALSE;
    }

  for (i=0; i < (int)ARR_SIZE(Annunciators);i++) {
    data_ptr = (OEMState_data *)Annunciators[i].pcontent->data;
    for (j=0; j < (int)Annunciators[i].pcontent->nMaxStates; j++) {
      if (data_ptr->pBmp != NULL) {
        IBITMAP_Release(data_ptr->pBmp);
        data_ptr->pBmp = NULL;
        data_ptr++;
      }
    }
  }
  ISHELL_Release(pMe->m_piShell);

  if (pMe->m_piDisplay)
  {
    IDISPLAY_Release(pMe->m_piDisplay);
    pMe->m_piDisplay = NULL;
  }
  if (pMe->m_pDDB)
  {
    IDIB_Release(pMe->m_pDDB);
    pMe->m_pDDB = NULL;
  }

  /* For now, we don't need to explicitly release bitmaps for the
   * secondary  annunciator, since the content is shared with the
   * primary annunciator.
   */
  if (pMe->m_piDisplay2)
  {
    IDISPLAY_Release(pMe->m_piDisplay2);
    pMe->m_piDisplay2 = NULL;
  }
  if (pMe->m_pDDB2)
  {
    IDIB_Release (pMe->m_pDDB2);
    pMe->m_pDDB2 = NULL;
  }

  pMe->m_bAnnunciatorOn = FALSE;
  pMe->m_bAnnunciator2On = FALSE;
  pMe->cached = FALSE;

  OEM_Free(IAnnunCoreObj);
  IAnnunCoreObj = NULL;
  return (0);
}

/*===========================================================================

FUNCTION: OEMAnnunciator_New

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEMAnnunciator_New(IShell *piShell, AEECLSID clsid, void **pp)
{
   IAnnunciator *pMe=NULL;
   *pp = 0;

   if (AEECLSID_ANNUNCIATOR != clsid)
   {
      return EUNSUPPORTED;
   }

//lint -save -e826  Suppress "Suspicious ptr-to-ptr conversion; space too small"
   pMe = (IAnnunciator *)AEE_NewClass((IBaseVtbl*)&gvtIAnnunciator,
                                      sizeof(IAnnunciator));
//lint -restore
   if (pMe == NULL)
   {
      return ENOMEMORY;
   }

   pMe->m_piShell = piShell;
   (void)ISHELL_AddRef(piShell);

   pMe->clsid = clsid;
   pMe->m_coreObj = OEMAnnunCore_New(piShell);

   if (pMe->m_coreObj == NULL)
   {
      IAnnunciator_Delete(pMe);
      return EFAILED;
   }
   pMe->m_pac = AEE_GetAppContext();

   /* Force the use of fonts that fit our field sizes. */
   //OEMAnnunciator_SelectFonts(pMe->m_coreObj->m_piDisplay);

   /* The following check is a workaround for a BREW bug.  If it's
    * a BREW-internal thing creating the annunciator, as indicated 
    * by the context class being the shell's class, then don't
    * set up the callback.
    */
   if (AEE_GetAppContextCls(pMe->m_pac) != AEECLSID_SHELL)
   {
      CALLBACK_Init(&pMe->m_cbSysObj, OEM_FreeAnnunciator, pMe);
      AEE_LinkSysObject(&pMe->m_cbSysObj);
   }
   *pp = pMe;
   return SUCCESS;
}
/*===========================================================================

FUNCTION: IAnnunciator_Delete

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void IAnnunciator_Delete(IAnnunciator *pMe)
{
  if (pMe) {
    ISHELL_CancelTimer(pMe->m_piShell, NULL, pMe);
    if(pMe->m_coreObj)
    {
      (void)OEMAnnunCore_Release();
      pMe->m_coreObj = NULL;
    }
    CALLBACK_Cancel(&pMe->m_cbSysObj);
    ISHELL_Release(pMe->m_piShell);
    FREEIF(pMe);
  }
}
/*===========================================================================

FUNCTION: IAnnunciator_AddRef

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 IAnnunciator_AddRef(IAnnunciator *pMe)
{
  if (pMe) {
    return ++pMe->m_usRef;
  }
  return 0;
}
/*===========================================================================

FUNCTION: IAnnunciator_Release

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static uint32 IAnnunciator_Release(IAnnunciator *pMe)
{
  if (pMe) {
    unsigned short us = --pMe->m_usRef;
    if (us == 0) {
      IAnnunciator_Delete(pMe);
    }
    return (uint32)us;
  }
  return 0;
}

/*===========================================================================

FUNCTION: IAnnunciator_QueryInterface

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int IAnnunciator_QueryInterface(IAnnunciator *pMe, AEECLSID clsid, void **ppo)
{
  if (pMe == NULL) {
    return EFAILED;
  }
  if (0 != (*ppo = IQI_SELF(pMe,clsid,ppo,pMe->clsid))) {
    // We don't need the updated ref count, so just (void) cast the function
    (void)IAnnunciator_AddRef(pMe);
    return SUCCESS;
  } else {
    return ECLASSNOTSUPPORT;
  }
}

/*===========================================================================

FUNCTION: IAnnunciator_GetField

DESCRIPTION
  Retrieves the state for a particular annunciator field.

DEPENDENCIES
  none

RETURN VALUE
  int - SUCCESS if successful
        EFAILED otherwise

SIDE EFFECTS
  none
===========================================================================*/
static int IAnnunciator_GetField(IAnnunciator * pMe, uint32 nAnnunID, uint32 * pnState)
{
  if (pMe == NULL) {
    return EFAILED;
  }
  if (nAnnunID >= ARR_SIZE(Annunciators))
    return EFAILED;
  if (Annunciators[nAnnunID].pcontent->nFieldType != ANNUN_TYPE_IMAGE)
    return EFAILED;
  *pnState = Annunciators[nAnnunID].pcontent->nCurrState;
  return SUCCESS;
}
/*===========================================================================
FUNCTION:IAnnunciator_SetFieldIsActiveEx

=============================================================================*/
static int IAnnunciator_SetFieldIsActiveEx(IAnnunciator * pMe,boolean bActive)
{		
	if (pMe == NULL) 
    {
      return EFAILED;
    }
	
	IAnnunCoreObj->m_bActive = bActive;
	return SUCCESS;
}

static int IAnnunciator_SetHasTitleText(IAnnunciator *pMe, boolean bHasTitleText)
{	
	if (pMe == NULL) 
    {
      return EFAILED;
    }
	IAnnunCoreObj->m_hasTitleText = bHasTitleText;
	return SUCCESS;

}

/*===========================================================================
FUNCTION:IAnnunciator_SetFieldText

=============================================================================*/
static int IAnnunciator_SetFieldText(IAnnunciator * pMe ,uint16 *cText)
{
	if (pMe == NULL) 
	{
      return EFAILED;
    }

    if(cText)
    {
	    WSTRLCPY(IAnnunCoreObj->m_Title, cText, ANN_TEXT_MAX_LEN);
    }
    else
    {
        IAnnunCoreObj->m_Title[0] = 0;
    }
	
	IAnnunciator_Redraw(pMe);	
	
	return SUCCESS;
}

/*===========================================================================

FUNCTION: IAnnunciator_SetField

DESCRIPTION
  Sets the state for a particular annunciator field.

DEPENDENCIES
  none

RETURN VALUE
  int - SUCCESS if successful
        EFAILED otherwise

SIDE EFFECTS
  none

===========================================================================*/
static int IAnnunciator_SetField(IAnnunciator * pMe, uint32 nAnnunID,
                                 uint32 nState)
{
  PACONTEXT pac;
  uint32 nStateRank = GetAnnunStateRank(nState);
  //MSG_FATAL("IAnnunciator_SetField %d 0x%x",nAnnunID,nState,0);
  
  
  if (nAnnunID >= ARR_SIZE(Annunciators))
  {
    return EFAILED;
  }
  if (Annunciators[nAnnunID].pcontent->nFieldType != ANNUN_TYPE_IMAGE)
  {
    return EFAILED;
  }
  /*if (Annunciators[nAnnunID].pcontent->nMaxStates < (nState &
                                                     ANNUN_STATE_BLINK_UNMASK))*/
  if(nStateRank == 0)
  {/*to turn off a state*/
    if(ModifyAnnunStateSchedule(pMe, nAnnunID, &nState) != SUCCESS)
    {
      return EFAILED;
    }
  }
  else if(Annunciators[nAnnunID].pcontent->nMaxStates < nStateRank)
  {
    return EFAILED;
  }
  else
  {/*deal with state that need to switch*/
    if(SwitchAnnunState(pMe, nAnnunID, &nState) != SUCCESS)
    {
      return EFAILED;
    }
  }
  
  if (Annunciators[nAnnunID].pcontent->data == NULL)
  {
    return EFAILED;
  }
  if (pMe == NULL)
  {
    return EFAILED;
  }
  
  /* Run in the system context */
#if MIN_BREW_VERSION(3, 0)
  pac = AEE_EnterAppContext(NULL);
#else
  pac = AEE_SetAppContext(NULL);
#endif // MIN_BREW_VERSION(3, 0)
  if (nState == ANNUN_STATE_OFF) {
    /* clear the field */
    if (ClearField (pMe, nAnnunID) != SUCCESS)
    {
      /* Change back to the App Context */
#if MIN_BREW_VERSION(3, 0)
      AEE_LeaveAppContext(pac);
#else
      AEE_SetAppContext(pac);
#endif // MIN_BREW_VERSION(3, 0)
      return EFAILED;
    }

    /* Cancel timer if present */
    CancelBlinkTimer(pMe, nAnnunID);
  }
  else {
    /* Blinking */
    if (nState & ANNUN_STATE_BLINK) {
      //nState = nState|ANNUN_STATE_ON;
      SetBlinkTimer(pMe, nAnnunID);
    }
    else {
      CancelBlinkTimer (pMe, nAnnunID);
    }

    /* Image field */
    if (DrawImageField(pMe, nAnnunID, (nState & ANNUN_STATE_BLINK_UNMASK)) !=
        SUCCESS)
    {
#ifndef WIN32
 //     MSG_FATAL("Unable to draw field %d for annunciator set, state %d.", i,
 //             Annunciators[i].pcontent->nCurrState, 0);
#endif
    }

  }
  Annunciators[nAnnunID].pcontent->nCurrState = nState;

  /* Change back to App context */
#if MIN_BREW_VERSION(3, 0)
  AEE_LeaveAppContext(pac);
#else
  AEE_SetAppContext(pac);
#endif // MIN_BREW_VERSION(3, 0)
  return SUCCESS;
}

/*===========================================================================

FUNCTION: OEMANNUNCIATOR_SetFieldEx

DESCRIPTION
    This function sets the value of an annunciator field to the specified
    data (e.g. text).

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS if successful
  EFAILED otherwise

SIDE EFFECTS
  none

===========================================================================*/
static int IAnnunciator_SetFieldEx(IAnnunciator * pMe, uint32 nAnnunID,
                                   void * pData)
{
  int nWidth, nHeight;
  AEERect Rect;
  AECHAR* pszText=NULL;
  PACONTEXT pac;
  IBitmap *pIBitmap = NULL;

  if (nAnnunID >= ARR_SIZE(Annunciators))
    return EFAILED;

  if(NULL == pMe || NULL == pMe->m_coreObj)
  {
    return EFAILED;
  }

  //ANNUN_FIELD_OPERATOR is a special case which can be image/text
  // We display the PLMN at this location for GSM/WCDMA
  if ((Annunciators[nAnnunID].pcontent->nFieldType != ANNUN_TYPE_TEXT))
    return EFAILED;

  /* Run in the system context */
#if MIN_BREW_VERSION(3, 0)
  pac = AEE_EnterAppContext(NULL);
#else
  pac = AEE_SetAppContext(NULL);
#endif // MIN_BREW_VERSION(3, 0)
  nWidth = (int)Annunciators[nAnnunID].width;
  nHeight = (int)Annunciators[nAnnunID].height;

  pIBitmap = IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB);
  SETAEERECT(&Rect, Annunciators[nAnnunID].x_pos,
             Annunciators[nAnnunID].y_pos, nWidth, nHeight);

  IBITMAP_FillRect (pIBitmap, &Rect, pMe->m_coreObj->m_bg, AEE_RO_COPY);
  IDISPLAY_SetDestination (pMe->m_coreObj->m_piDisplay, pIBitmap);

  pszText = (AECHAR *) pData;

//  if(nAnnunID != ANNUN_FIELD_OPERATOR) // Reserved field is image.
  {
    WSTRCPY ((AECHAR *) Annunciators[nAnnunID].pcontent->data, pszText);
  }
  
  if (pszText != NULL) {
    IDISPLAY_DrawText (pMe->m_coreObj->m_piDisplay, AEE_FONT_BOLD, pszText, -1, 0, 0,
                       &Rect, IDF_ALIGN_TOP|IDF_ALIGN_LEFT|IDF_ALIGN_FILL);
  }
  IDISPLAY_SetDestination(pMe->m_coreObj->m_piDisplay, NULL); /* restore the destination */
  annun_disp_update ( pMe, pMe->m_coreObj->m_pDDB, nAnnunID);

  /* Change back to App context */
#if MIN_BREW_VERSION(3, 0)
  AEE_LeaveAppContext(pac);
#else
  AEE_SetAppContext(pac);
#endif // MIN_BREW_VERSION(3, 0)

  return SUCCESS;

}

/*=======================================================================

FUNCTION: OEMANNUNCIATOR_Redraw

DESCRIPTION
  This function forces a redraw of the annunciator on all displays
  where it appears.  This is achieved by invalidating the annunciator's
  bitmap on that display.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS : If successful
  EFAILED : If unsuccessful
  EBADPARM: If an argument was invalid

SIDE EFFECTS
  none

======================================================================= */
//lint -save -esym(529, nAnnIdx)
static int IAnnunciator_Redraw(IAnnunciator *pMe)
{
   boolean         bIsBmpInvalidated = FALSE;   // Primary bmp invalidated?
#if 1//ndef FEATURE_MDP
   IBitmap        *pBmp;                        // BMP for primary display
   AEEBitmapInfo   sBmpInfo;                    // Bmp info struct
   AEERect         sBmpRect;                    // Rectangle to invalidate
#endif // ! FEATURE_MDP
   int             nRetVal = EFAILED;           // Return code
   PACONTEXT       pac;                         // Calling context
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
   db_items_value_type  need_capture;
#endif
   //IImage * pBackBmp = NULL;
   AEERect rc = {0};
   uint32      dwFlags;
   if (NULL == pMe)
   {
      MSG_LOW("Null object ptr in annunciator redraw.", 0, 0, 0);
      return EBADPARM;
   }
   else if (NULL == pMe->m_coreObj)
   {
      MSG_LOW("Null core object ptr in annunciator redraw.", 0, 0, 0);
      return EFAILED;
   }   
	
   /* Run in the system context */
#if MIN_BREW_VERSION(3, 0)
   pac = AEE_EnterAppContext(NULL);
#else
   pac = AEE_SetAppContext(NULL);
#endif // MIN_BREW_VERSION(3, 0)	

   if(pMe->m_coreObj->m_bAnnunciatorOn)
   {
#if 0//def FEATURE_MDP
#error code not present
#else

      /* Invalidate the entire annunciator bitmap, to ensure that the
       * whole thing redraws.
       */
      pBmp = IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB);
      if (pBmp)
      {
         if (SUCCESS == IBITMAP_GetInfo(pBmp, &sBmpInfo, sizeof(sBmpInfo)))
         {
            sBmpRect.x  = 0;
            sBmpRect.y  = 0;
            sBmpRect.dx = (int16)(sBmpInfo.cx);
            sBmpRect.dy = (int16)(sBmpInfo.cy);
            if (SUCCESS == IBITMAP_Invalidate(pBmp, &sBmpRect))
            {
               bIsBmpInvalidated = TRUE;
            }
         }
      }

      /* Now update the display */
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
      db_get(DB_CAPTURE_WALLPER,&need_capture);
      {
         AEERect Rect;
    
         SETAEERECT(&Rect,
                       0,
                       0,
                       pMe->m_coreObj->m_annun_prim_lcd_width, 
                       pMe->m_coreObj->m_annun_prim_lcd_height);
            
            
         if (need_capture.b_capture == DB_CAPTURE_INIDLE)
         {
            // 待机重绘图标栏时可不必再在显示设备上绘制底部图片，因这之前整个墙纸绘制已覆盖此区域。如此可避免因绘制底图带来的闪屏
         }
         else
         {
            // 非待机下显示单色背景，目前为黑色
            RGBVAL nBgColor = RGB_BLACK;
#ifdef FEATURE_RANDOM_MENU_COLOR
            byte     nRandomMenu = 0;
            
            (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));

            if(nRandomMenu != 0)
            {
                (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, &nBgColor, sizeof(nBgColor));
            }
#endif
            IDISPLAY_FillRect(pMe->m_coreObj->m_piDisplay, &Rect, nBgColor);
         }
         //IDISPLAY_FillRect(pMe->m_coreObj->m_piDisplay, &Rect, RGB_BLACK);
      }
      IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                        0,
                        0,
                        pMe->m_coreObj->m_annun_prim_lcd_width,
                        pMe->m_coreObj->m_annun_prim_lcd_height,
                        IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                        0,
                        0,
                        AEE_RO_TRANSPARENT);
#else
      IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                        0,
                        0,
                        pMe->m_coreObj->m_annun_prim_lcd_width,
                        pMe->m_coreObj->m_annun_prim_lcd_height,
                        IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                        0,
                        0,
                        AEE_RO_COPY);
#endif
        
        
	    if(!IAnnunCoreObj->m_bActive)
		{			
			/*
			//Add By zzg 2012_11_12
			if (IAnnunCoreObj->m_Title[0] == 0)
			{
#ifdef FEATURE_VERSION_C337
					
				IImage      *pBarImg = NULL;

				pBarImg = ISHELL_LoadResImage(pMe->m_piShell,
	                                          AEE_APPSCOMMONRES_IMAGESFILE,
	                                          IDI_TITLEBAR);

				if (NULL != pBarImg)
		        {
		            IIMAGE_Draw(pBarImg, 0, 0);
		            IIMAGE_Release(pBarImg);
		            pBarImg = NULL;
		        }
				
				IDISPLAY_UpdateEx(pMe->m_coreObj->m_piDisplay, TRUE);					
#endif
			}
			//Add End
			*/
			
            if(IAnnunCoreObj->m_hasTitleText && IAnnunCoreObj->m_Title[0] != 0)
            {            	
            	if (need_capture.b_capture != DB_CAPTURE_INIDLE)	//Add By zzg 2010_08_05
            	{
            	    //IIMAGE_Draw(pBackBmp,20, 0);
            	    AEERect bgRect;
                    int titleLen = IDISPLAY_MeasureText(pMe->m_coreObj->m_piDisplay,AEE_FONT_NORMAL, (const AECHAR*)IAnnunCoreObj->m_Title);

					//MSG_FATAL("***zzg OEMAnnunciator titleLen=%d***", titleLen, 0, 0);
					
                    bgRect.y = 0;
                    bgRect.dy = STATEBAR_HEIGHT;
#if defined(FEATURE_DISP_128X128)
					bgRect.x = 20;
                    if(titleLen > 88)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 128;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 88;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }
#elif defined(FEATURE_DISP_160X128)
					bgRect.x = 20;
                    if(titleLen > 120)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 160;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 120;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }
#elif defined(FEATURE_DISP_220X176)
					bgRect.x = 28;
                    //if(titleLen > 180)
                    if(titleLen > 164)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 220;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 164;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }					
#elif defined(FEATURE_DISP_128X160)
					bgRect.x = 20;
                    if(1)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 128;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 88;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }
#elif defined(FEATURE_DISP_176X220)
					bgRect.x = 30;
                    //if(titleLen > 116)
                    if(1)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 176;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 116;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }	
#elif defined(FEATURE_DISP_240X320)
					bgRect.x = 40;
                    if(titleLen >=150)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 240;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                        bgRect.dx = 200;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }		
#elif defined(FEATURE_DISP_320X240)
					bgRect.x = 40;
                    if(titleLen > 256)
                    {
                        bgRect.x = 0;
                        bgRect.dx = 320;
                        IAnnunCoreObj->m_btoolen = TRUE;
                    }
                    else
                    {
                    	bgRect.x = 32;
                        bgRect.dx = 256;
                        IAnnunCoreObj->m_btoolen = FALSE;
                    }						
#else
                    bgRect.dx = 120;
                    IAnnunCoreObj->m_btoolen = FALSE;
#endif


					//DBGPRINTF("***zzg bgRect:%d,%d,%d,%d***", bgRect.x, bgRect.y, bgRect.dx, bgRect.dy);

					
					#ifdef FEATURE_VERSION_C337
					{
						IImage      *pBarImg = NULL;

						pBarImg = ISHELL_LoadResImage(pMe->m_piShell,
			                                          AEE_APPSCOMMONRES_IMAGESFILE,
			                                          IDI_TITLEBAR);

						if (NULL != pBarImg)
				        {
				            IIMAGE_Draw(pBarImg, 0, 0);
				            IIMAGE_Release(pBarImg);
				            pBarImg = NULL;
				        }
					}
					#else
					
                    IDISPLAY_DrawRect(pMe->m_coreObj->m_piDisplay,
                                      &bgRect,
                                      RGB_NONE,
                                      MAKE_RGB(0,0,0),
                                      IDF_RECT_FILL);
					#endif

	                dwFlags =  IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
	                rc.x = 0;
	                rc.y = 0;
	                rc.dx = SCREEN_WIDTH;
	                rc.dy = STATEBAR_HEIGHT;

					#ifdef FEATURE_VERSION_C337
					IDISPLAY_SetColor(pMe->m_coreObj->m_piDisplay, CLR_USER_TEXT, RGB_BLACK);
					#else
					IDISPLAY_SetColor(pMe->m_coreObj->m_piDisplay, CLR_USER_TEXT, RGB_WHITE);
					#endif					
	                
	                // 绘制标题文本
	                //DBGPRINTF("IAnnunCoreObj->m_Title:::::::::::::::::%s",IAnnunCoreObj->m_Title);
	                (void) IDISPLAY_DrawText(pMe->m_coreObj->m_piDisplay, 
	                AEE_FONT_NORMAL, 
	                (const AECHAR*)IAnnunCoreObj->m_Title, 
	                -1, 
	                0, 
	                0, 
	                &rc, 
	                dwFlags);					
	                (void)IDISPLAY_SetColor(pMe->m_coreObj->m_piDisplay, CLR_USER_TEXT, RGB_BLACK);					
					
	                IDISPLAY_UpdateEx(pMe->m_coreObj->m_piDisplay, TRUE);
            	}
            }
           
		  // IIMAGE_Release( pBackBmp);
		}
		
      // 待机界面下不必跟新显示，待机界面绘制完显示信息后再统一更新显示，如此可避免进入待机界面的闪屏
      if (need_capture.b_capture != DB_CAPTURE_INIDLE)
      {
          IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
      }
#endif // ! FEATURE_MDP

   }
   else
   {
      bIsBmpInvalidated = TRUE;
   }

   /* Change back to App context */
#if MIN_BREW_VERSION(3, 0)
   AEE_LeaveAppContext(pac);
#else
   AEE_SetAppContext(pac);
#endif // MIN_BREW_VERSION(3, 0)
   return nRetVal;
}

/*=======================================================================

IAnnunciator_EnableAnnunciatorBar()

Description:
    This function enables or disables the drawing of the annunciator bar to the specified screen.

Prototype:
    int IAnnunciator_EnableAnnunciatorBar(IAnnunciator * pIAnnunciator, AEECLSID clsid, boolean bOn)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    clsid         : Class ID of the Screen to change the state for
    bOn           : TRUE/Enable annunciator bar, FALSE/Disable annunciator bar.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    When reenabling annunciator bar, the annunciator bar bitmap will be copied to the display

See Also:

======================================================================= */
static int IAnnunciator_EnableAnnunciatorBar(IAnnunciator * pMe, AEECLSID clsid, boolean bOn)
{
  boolean bLastState;
#if 0//def FEATURE_MDP
#error code not present
#endif /* FEATURE_MDP */

  if(!pMe)
  {
     return EFAILED;
  }

  if(NULL == pMe->m_coreObj)
  {
    return EFAILED;
  }

  switch(clsid)
  {
     case AEECLSID_DISPLAY1:
        bLastState = pMe->m_coreObj->m_bAnnunciatorOn;
        pMe->m_coreObj->m_bAnnunciatorOn = bOn;
#if 0//def FEATURE_MDP
#error code not present
#endif /* FEATURE_MDP */
        break;

     default:
        return ECLASSNOTSUPPORT;
  }
  
  if(bOn != bLastState)
  {
#if 0//def FEATURE_MDP
#error code not present
#endif /* FEATURE_MDP */
     
     if(bOn)
     {
        (void) IANNUNCIATOR_Redraw(pMe);
     }
  }

  return SUCCESS;
}

/*=======================================================================

IAnnunciator_GetAnnunciatorBarSize()

Description:
    This function retrieves the size of the annunciator bar for the specified screen.

Prototype:
    int IAnnunciator_GetAnnunciatorBarSize(IAnnunciator * pMe, AEECLSID clsid, AEERect *pRec)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    clsid         : Class ID of the Screen to get the size of the annunicator bar for
    prc           : Pointer the rectangle describing the size of the annunciator bar.

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments:
    None

Side Effects:
    None

See Also:


======================================================================= */
static int IAnnunciator_GetAnnunciatorBarSize(IAnnunciator * pMe, AEECLSID clsid, AEERect *prc)
{
  if(!pMe || !prc)
  {
     return EFAILED;
  }

  switch(clsid)
  {
     case AEECLSID_DISPLAY1:
        SETAEERECT(prc, 0, 0, pMe->m_coreObj->m_annun_prim_lcd_width, pMe->m_coreObj->m_annun_prim_lcd_height);
        break;
        
     default:
        return ECLASSNOTSUPPORT;
  }

  return SUCCESS;
}

#ifdef FEATRUE_SET_ANN_FULL_SCREEN
// 用于保存待机界面墙纸顶层图标栏覆盖部分墙纸的图片数据
IDIB ** OEMANNUNCIATOR_GetWallpaperBarDDB(void)
{
    if (NULL != pgWallpaperBarDDB)
    {
        IDIB_Release(pgWallpaperBarDDB);
        pgWallpaperBarDDB = NULL;
    }
    
    return (&pgWallpaperBarDDB);
}
#endif
// 设置取消闪烁的定时器
static int IAnnunciator_SetUnblinkTimer(IAnnunciator * pMe, uint32 nAnnunID, uint32 nState, uint32 nTimeMs)
{
    //AnnunTimerInfo AnnTimerInfo = {0};
    
    if(NULL == pMe || nTimeMs == 0)
    {
        return EFAILED;
    }

    //AnnTimerInfo.pIAnn= pMe;
    pMe->nAnnunID= nAnnunID;
    pMe->nState= nState&ANNUN_STATE_UNBLINK;
    
    ISHELL_SetTimer(pMe->m_piShell, nTimeMs, (PFNNOTIFY)SetFieldUnblink, pMe);
    return SUCCESS;
}

static void SetFieldUnblink(IAnnunciator * pMe)
{
    //AnnunTimerInfo *pAnnTimerInfo = (AnnunTimerInfo *)pAnnunTimerInfo;
    IAnnunciator_SetField(pMe, pMe->nAnnunID, pMe->nState);
}

