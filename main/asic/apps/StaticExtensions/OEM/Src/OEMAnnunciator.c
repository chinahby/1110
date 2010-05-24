/*============================================================================

FILE:  OEMAnnunciator.c

SERVICES: Provides annunciator related services.


==============================================================================

               Copyright © 1999-2008 QUALCOMM Incorporated
                       All Rights Reserved.
                       QUALCOMM Proprietary

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMAnnunciator.c#54 $

============================================================================*/
//lint -save -e611 -e740 Suppress ptr complaints; IBitmap has lots of issues

#include "customer.h"
#include "comdef.h"

#include "OEMFeatures.h"
#include "AEEComdef.h"
#include "AEE.h"       /* Standard AEE declarations */
#include "AEEFont.h"
#include "AEEShell.h"  /* AEE Shell services, IShell declarations */
#include "AEEStdLib.h" /* AEE StdLib decls */
#include "AEEDisp.h"
#include "AEE_OEM.h"
#include "AEEFile.h"
#include "OEMObjectMgr.h"
#include "AEEAnnunciator.h"
#include "oemannunciator.brh"
#include "OEMDisp.h"
#include "disp.h"
#include "AEE_OEM.h"
#include "AEEClassIDs.h"
#include "AEE_OEMHeap.h"
#include "OEMHeap.h"
#include "msg.h"
#include "BREWVersion.h"
#include "AEE_OEMDispatch.h"

#ifdef FEATURE_MDP
#include "mdp_sys_api.h"
#endif

/* Dimensions of various display sizes.  They're defined as
 * "small dimension" and "large dimension" rather than "width" and
 * "height", since width and height vary according to landscape
 * or portrait orientation.
 */
/* Sub-QCIF Dimensions */
#define ANNUN_SQCIF_SMALL_DIM 96    //lint -save -e750 "not referenced"
#define ANNUN_SQCIF_LARGE_DIM 128   //lint -save -e750 "not referenced"
/* QCIF Dimensions */
#define ANNUN_QCIF_SMALL_DIM 176
#define ANNUN_QCIF_LARGE_DIM 220

/* Use a fixed size for the primary display annunciator */
#define ANNUN_PRIMARY_HEIGHT 40

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY

#if ! MIN_BREW_VERSION(3, 0)
#define AEEFS_SHARED_DIR
#endif

#ifdef FEATURE_MDP
#define ANNUN_ALPHA_BLEND_PCT 0x8f
#endif

typedef struct IANNUNCore
{
   unsigned short  m_uRefs;
   IShell         *m_piShell;
   IDisplay       *m_piDisplay;
   IDIB           *m_pDDB;
   uint32          m_pAnnunciators_ovId;     /* Primary Annunciator Overlay ID */
   NativeColor     m_bg;                    /* Primary Native Background Color */
   boolean         m_bAnnunciatorOn;        /* Primary annunciators on? */
   boolean         m_bAnnunciator2On;       /* Secondary annunciators on? */
   uint16          m_annun_prim_lcd_width;   /* Primary LCD width */
   uint16          m_annun_prim_lcd_height;  /* Primary LCD height */
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
   boolean         m_bTimer; /* blinking timer set ? */
   boolean         cached;
   boolean         m_bEnableBlink;
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
};


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
static void IAnnunciator_EnableBlink(IAnnunciator * pMe, boolean bEnable);
static uint32 OEMAnnunCore_AddRef(void);
static uint32 OEMAnnunCore_Release(void);
static void OEMAnnunciator_SelectFonts(IDisplay* pIDisp);

IAnnunciatorVtbl gvtIAnnunciator = {
   IAnnunciator_AddRef,
   IAnnunciator_Release,
   IAnnunciator_QueryInterface,
   IAnnunciator_GetField,
   IAnnunciator_SetField,
   IAnnunciator_SetFieldEx,
   IAnnunciator_Redraw,
   IAnnunciator_EnableAnnunciatorBar,
   IAnnunciator_GetAnnunciatorBarSize,
   IAnnunciator_EnableBlink
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
static OEMState_data mode_image_data[] =
{
  {ANNUN_STATE_ANALOG, IDB_ANALOG, NULL},
  {ANNUN_STATE_CDMA, IDB_CDMA, NULL},
  {ANNUN_STATE_GSM, IDB_GSM, NULL},
  {ANNUN_STATE_WCDMA, IDB_WCDMA, NULL},
  {ANNUN_STATE_HDR, IDB_HDR, NULL},
  {ANNUN_STATE_HYBRID, IDB_HYBRID, NULL},
  {ANNUN_STATE_HDR_REV_A, IDB_HDR_REV_A, NULL},
  {ANNUN_STATE_HYBRID_REV_A, IDB_HYBRID_REV_A, NULL},
  {ANNUN_STATE_SIMULTANEOUS, IDB_SIMULTANEOUS, NULL},
  {ANNUN_STATE_GPS, IDB_GPS, NULL},
  {ANNUN_STATE_DIGITAL, IDB_DIGITAL, NULL}
};

// Image data for Roaming
static OEMState_data roam_image_data[] = {ANNUN_STATE_ON, IDB_ROAM, NULL};

// Image data for Domain
static OEMState_data domain_image_data[] =
{
  {ANNUN_STATE_CS, IDB_CS, NULL},
  {ANNUN_STATE_PS, IDB_PS, NULL},
  {ANNUN_STATE_CS_PS, IDB_CSPS, NULL}
};

// Image data for RSSI */
static OEMState_data rssi_image_data[] =
{
  {ANNUN_STATE_RSSI_0, IDB_RSSI0, NULL},
  {ANNUN_STATE_RSSI_1, IDB_RSSI1, NULL},
  {ANNUN_STATE_RSSI_2, IDB_RSSI2, NULL},
  {ANNUN_STATE_RSSI_3, IDB_RSSI3, NULL},
  {ANNUN_STATE_RSSI_4, IDB_RSSI4, NULL}
};

// Image data for HDR RSSI */
static OEMState_data hdr_rssi_image_data[] =
{
  {ANNUN_STATE_RSSI_0, IDB_HDR_RSSI0, NULL},
  {ANNUN_STATE_RSSI_1, IDB_HDR_RSSI1, NULL},
  {ANNUN_STATE_RSSI_2, IDB_HDR_RSSI2, NULL},
  {ANNUN_STATE_RSSI_3, IDB_HDR_RSSI3, NULL},
  {ANNUN_STATE_RSSI_4, IDB_HDR_RSSI4, NULL}
};

// Image data for WIFI RSSI */
static OEMState_data wlan_rssi_image_data[] =
{
  {ANNUN_STATE_RSSI_0, IDB_WIFI_RSSI0, NULL},
  {ANNUN_STATE_RSSI_1, IDB_WIFI_RSSI1, NULL},
  {ANNUN_STATE_RSSI_2, IDB_WIFI_RSSI2, NULL},
  {ANNUN_STATE_RSSI_3, IDB_WIFI_RSSI3, NULL},
  {ANNUN_STATE_RSSI_4, IDB_WIFI_RSSI4, NULL}
};

// Image data for In-Use */
static OEMState_data in_use_image_data[] =
{
  {ANNUN_STATE_IN_USE, IDB_INUSE, NULL},
  {ANNUN_STATE_NO_SVC, IDB_NOSVC, NULL}
};

// Image data for Data-call
static OEMState_data datacall_image_data[] =
{
  {ANNUN_STATE_DORMANT, IDB_DORMANT, NULL},
  {ANNUN_STATE_ACTIVE, IDB_NETWORK, NULL}
};


// Image data for Alert
static OEMState_data alert_image_data[] =
{
  {ANNUN_STATE_ALERT_ON, IDB_ALERT, NULL},
  {ANNUN_STATE_SILENCE, IDB_SILENCE, NULL}
};

// Image data for Messages
static OEMState_data msg_image_data[] = {ANNUN_STATE_ON, IDB_MSG, NULL};

static OEMState_data vt_image_data[] = {ANNUN_STATE_ON, IDB_VT, NULL};

// Image data for MMS
static OEMState_data mms_image_data[] = {ANNUN_STATE_ON, IDB_MBOX, NULL};

// Image data for battery
static OEMState_data batt_image_data[] =
{
  {ANNUN_STATE_BATT_LOW, IDB_NO_BATT, NULL},
  {ANNUN_STATE_BATT_1, IDB_BATT1, NULL},
  {ANNUN_STATE_BATT_2, IDB_BATT2, NULL},
  {ANNUN_STATE_BATT_3, IDB_BATT3, NULL},
  {ANNUN_STATE_BATT_FULL, IDB_FULLBATT, NULL}
};

// Image data for security
static OEMState_data security_image_data[] = {ANNUN_STATE_ON, IDB_LOCK, NULL};

// Image data for logo (reserved)
static OEMState_data reserved_image_data[] = {ANNUN_STATE_ON, IDB_LOGO, NULL};

// Image data for HSDPA/HSUPA service indicator
static OEMState_data hsdpa_image_data[] =
{
  {ANNUN_STATE_HSDPA, IDB_HSDPA, NULL},
  {ANNUN_STATE_HSUPA, IDB_HSUPA, NULL},
  {ANNUN_STATE_HSDPA_HSUPA, IDB_HSDPAHSUPA, NULL}
};

// Image data for BCMCS annunciator
static OEMState_data bcmcs_image_data[] = {ANNUN_STATE_ON, IDB_BCMCS, NULL};

// Text data
#define ANNUN_TEXT_SIZE 25
static AECHAR pszTime[ANNUN_TEXT_SIZE];
static AECHAR pszBanner[ANNUN_TEXT_SIZE];

/********************************************************************
 *
 * Content data for the annunciator fields
 *
 *******************************************************************/
OEMAnnun_content reserved_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)reserved_image_data};

OEMAnnun_content msg_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)msg_image_data};

OEMAnnun_content datacall_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)datacall_image_data};

OEMAnnun_content alert_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)alert_image_data};

OEMAnnun_content security_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)security_image_data};

OEMAnnun_content mode_content =
     {ANNUN_TYPE_IMAGE, 9, ANNUN_STATE_OFF, (void *)mode_image_data};

OEMAnnun_content roam_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)roam_image_data};

OEMAnnun_content domain_content =
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)domain_image_data};

OEMAnnun_content rssi_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)rssi_image_data};

OEMAnnun_content in_use_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)in_use_image_data};

OEMAnnun_content batt_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)batt_image_data};

OEMAnnun_content time_content =
     {ANNUN_TYPE_TEXT, 0, 0, (void *)pszTime};

OEMAnnun_content banner_content =
     {ANNUN_TYPE_TEXT, 0, 0, (void *)pszBanner};

OEMAnnun_content mms_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)mms_image_data};

OEMAnnun_content hdr_rssi_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)hdr_rssi_image_data};

OEMAnnun_content wlan_rssi_content =
     {ANNUN_TYPE_IMAGE, 5, ANNUN_STATE_OFF, (void *)wlan_rssi_image_data};

OEMAnnun_content vt_content =
     {ANNUN_TYPE_IMAGE, 2, ANNUN_STATE_OFF, (void *)vt_image_data};

OEMAnnun_content hsdpa_content =
     {ANNUN_TYPE_IMAGE, 3, ANNUN_STATE_OFF, (void *)hsdpa_image_data};

OEMAnnun_content bcmcs_content =
     {ANNUN_TYPE_IMAGE, 1, ANNUN_STATE_OFF, (void *)bcmcs_image_data};

/* Standard image fields are 10 x 10 (pixels)    */
/* while large image fields are 20 x 10 (pixels) */

#define IMG_WIDTH  10
#define IMG_HEIGHT 10
#define LG_IMG_WIDTH 20
#define TEXT_HEIGHT 11

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
#define ROW1_Y  2
#define ROW2_Y 14
#define ROW3_Y 28

/* Primary Annunciator layout:
 * First row:  First        Last        Field
 * ---------------------------------------------------
 *               0            1         -
 *               2           11         Mode
 *              12           14         -
 *              15           24         Roam
 *              25           27         -
 *              28           47         RSSI/HDR RSSI
 *              48           50         -
 *              51           70         WLAN RSSI
 *              71           73         -
 *              74           83         In Use
 *              84           86         -
 *              87           96         Net
 *              97           99         -
 *             100          109         Domain
 *             110          111         -
 *             112          121         HSDPA/HSUPA Service
 *             122          141         -
 *             142          151         BCMCS
 *             152          153         -
 *             154          173         Batt
 *             174          175         -
 *
 * Second row: First        Last        Field
 * ---------------------------------------------------
 *               0            1         -
 *               2           11         Msg
 *              12           14         -
 *              25           34         MMS
 *              35           37         -
 *              38           47         Alarm
 *              48           50         -
 *              51           60         VT
 *              61          163         -
 *             164          173         Security
 *             174          175         -
 *
 * Third row: First        Last        Field
 * ---------------------------------------------------
 *               0            1         -
 *               2           51         Time
 *              52           54         -
 *              55          124         Banner
 *             125          174         Operator
 *             175          175         -
 */

/* Annunciator Definitions */
//lint -save -e545  Suppress complaints about "suspicious use of &"
static OEMAnnun_data Annunciators[] =
{
  {ANNUN_FIELD_OPERATOR, 125, ROW3_Y, 50, IMG_HEIGHT, &reserved_content},
  {ANNUN_FIELD_MSG,   2, ROW2_Y, IMG_WIDTH, IMG_HEIGHT, &msg_content},
  {ANNUN_FIELD_NET,    87, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &datacall_content},
  {ANNUN_FIELD_ALARM, 38, ROW2_Y, IMG_WIDTH, IMG_HEIGHT, &alert_content},
  {ANNUN_FIELD_SECURITY, 164, ROW2_Y, IMG_WIDTH, IMG_HEIGHT, &security_content},
  {ANNUN_FIELD_MODE,    2, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &mode_content},
  {ANNUN_FIELD_ROAM,   15, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &roam_content},
  {ANNUN_FIELD_DOMAIN, 100, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &domain_content},
  {ANNUN_FIELD_RSSI,   28, ROW1_Y, LG_IMG_WIDTH, IMG_HEIGHT, &rssi_content},
  {ANNUN_FIELD_INUSE,  74, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &in_use_content},
  {ANNUN_FIELD_BATT,  154, ROW1_Y, LG_IMG_WIDTH, IMG_HEIGHT, &batt_content},
  {ANNUN_FIELD_TIME, 2, ROW3_Y, 50, TEXT_HEIGHT, &time_content},
  {ANNUN_FIELD_BANNER,   55, ROW3_Y, 70, TEXT_HEIGHT, &banner_content},
  {ANNUN_FIELD_MMS, 25, ROW2_Y, IMG_WIDTH, IMG_HEIGHT, &mms_content},
  {ANNUN_FIELD_HDR_RSSI, 28, ROW1_Y, LG_IMG_WIDTH, IMG_HEIGHT, &hdr_rssi_content},
  {ANNUN_FIELD_WLAN_RSSI, 51, ROW1_Y, LG_IMG_WIDTH, IMG_HEIGHT, &wlan_rssi_content},
  {ANNUN_FIELD_VT, 51, ROW2_Y, IMG_WIDTH, IMG_HEIGHT, &vt_content},
  {ANNUN_FIELD_HSDPA, 112, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &hsdpa_content},
  {ANNUN_FIELD_BCMCS, 142, ROW1_Y, IMG_WIDTH, IMG_HEIGHT, &bcmcs_content}
};

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY
//lint +e545

/* Status of bitmap cache */
static IANNUNCore *IAnnunCoreObj = NULL;

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
static void annun_disp_update(
  IAnnunciator *pMe,
  const IDIB *dib_ptr,
  uint32 nAnnunID)
{

  if(NULL == pMe->m_coreObj)
  {
    return;
  }
  if (dib_ptr == pMe->m_coreObj->m_pDDB)
  {
    // We're updating the primary display
#ifdef FEATURE_MDP
    if(pMe->m_coreObj->m_bAnnunciatorOn)
    {
       MDPIMG ovImg;

       /* register Annunciator bitmap */
       if (mdp_is_ovimg_valid(pMe->m_coreObj->m_pAnnunciators_ovId) != TRUE)
       {
         ovImg.dest = PRIMARY_LCD_TYPE;
         ovImg.imgType = MDP_RGB565;
         ovImg.bmy_addr = (uint32 *)(void *)(dib_ptr->pBmp);
         ovImg.width = (uint16)pMe->m_coreObj->m_annun_prim_lcd_width;
         ovImg.cwin.x1 = 0;
         ovImg.cwin.x2 = pMe->m_coreObj->m_annun_prim_lcd_width-1;
         ovImg.cwin.y1 = 0;
         ovImg.cwin.y2 = pMe->m_coreObj->m_annun_prim_lcd_height-1;
         ovImg.lcd_x = 0- BREW_OFFSET_X;
         ovImg.lcd_y = 0- BREW_OFFSET_Y;
         ovImg.mdpOp = MDPOP_ALPHAB | MDPOP_TRANSP | MDPOP_VSYNC_OFF;
         ovImg.alpha = ANNUN_ALPHA_BLEND_PCT;
         ovImg.tpVal = (uint32) pMe->m_coreObj->m_bg;
         ovImg.layer = MDP_TOP_MOST_LAYER;

         pMe->m_coreObj->m_pAnnunciators_ovId = mdp_register_ovimg(&ovImg);
         /* lock this overlay icon to protect from being removed by Apps */
         if (mdp_lock_ovimg(pMe->m_coreObj->m_pAnnunciators_ovId, TRUE) !=
             MDP_SUCCESSFUL)
         {
            MSG_LOW("Unable to lock overlay image (primary).", 0, 0, 0);
         }

       }

       (void)mdp_update_ovimg(pMe->m_coreObj->m_pAnnunciators_ovId);
    }
#else
    if(pMe->m_coreObj->m_bAnnunciatorOn)
    {
       // No overlays supported
       IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       Annunciators[nAnnunID].width,
                       Annunciators[nAnnunID].height,
                       IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                       Annunciators[nAnnunID].x_pos,
                       Annunciators[nAnnunID].y_pos,
                       AEE_RO_COPY);
       IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
    }
#endif
  }
#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY
  else
  {
    /* Unsupported display bitmap for annunciator */
    MSG_LOW("Unsupported display bitmap for IAnnunciator.", 0, 0, 0);
  }

//lint -save -esym(818, pMe)  Suppress "pMe could be const"
//lint -save -esym(715, nAnnunID)  Suppress "nAnnunID not referenced"
}
//lint -restore
//lint -restore

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
static int ClearField (IAnnunciator *pMe, uint32 nAnnunID)
{
  AEERect Rect;
  int nWidth, nHeight;
  IBitmap *pIBitmap = NULL;

  if (pMe == NULL) {
    return EFAILED;
  }

  if(NULL == pMe->m_coreObj)
  {
    return EFAILED;
  }

  if (pMe->m_coreObj->m_pDDB == NULL) {
    return EFAILED;
  }

  nWidth = (int)Annunciators[nAnnunID].width;
  nHeight = (int)Annunciators[nAnnunID].height;

  // Update the annunciator on the primary display
  pIBitmap = IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB);

  SETAEERECT(&Rect, Annunciators[nAnnunID].x_pos,
             Annunciators[nAnnunID].y_pos, nWidth, nHeight);

  (void)IBITMAP_FillRect (pIBitmap, &Rect, pMe->m_coreObj->m_bg, AEE_RO_COPY);

  annun_disp_update (pMe, pMe->m_coreObj->m_pDDB, nAnnunID);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY

  return SUCCESS;
}

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
  int i,j;

  if ((pMe == NULL))
    return EFAILED;

  if(NULL == pMe->m_coreObj)
  {
    return EFAILED;
  }

  if ((pMe->m_coreObj->m_piDisplay == NULL) || (pMe->m_piShell == NULL) ||
      (pMe->m_coreObj->m_pDDB == NULL))
    return EFAILED;

  // Since we have HDR RSSI and normal RSSI at the same location,
  // even if the new state is same as previous state,
  // it needs to be redrawn.
  if ((nState == Annunciators[nAnnunID].pcontent->nCurrState) &&
      (nAnnunID != ANNUN_FIELD_RSSI) &&
      (nAnnunID != ANNUN_FIELD_HDR_RSSI)) {
    /* nothing to do */
    return SUCCESS;
  }

  if (nState == 0) {
    /* Shouldn't be here */
    return EFAILED;
  }

  nWidth = (int)Annunciators[nAnnunID].width;
  nHeight = (int)Annunciators[nAnnunID].height;

  if (!pMe->m_coreObj->cached) {
    /* Cache all the bitmaps */
    for (i=0; i < (int)ARR_SIZE(Annunciators); i++) {
      if (Annunciators[i].pcontent->nFieldType == ANNUN_TYPE_IMAGE) {
        data_ptr = (OEMState_data *) Annunciators[i].pcontent->data;
        for (j=0; j < (int)Annunciators[i].pcontent->nMaxStates; j++) {
          data_ptr->pBmp = ISHELL_LoadResBitmap (pMe->m_piShell,
                                         AEEFS_SHARED_DIR"oemannunciator.bar",
                                         (uint16)(data_ptr->nImageResID));
          data_ptr++;
        }
      }
    }
    pMe->m_coreObj->cached = TRUE;
  }

  data_ptr = (OEMState_data *) Annunciators[nAnnunID].pcontent->data;
  pBmp = data_ptr[nState-1].pBmp;

  if (pBmp == NULL) {
    return EFAILED;
  }

  // First update the primary display
  (void)IDISPLAY_SetDestination (pMe->m_coreObj->m_piDisplay, IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB));
  IDISPLAY_BitBlt (pMe->m_coreObj->m_piDisplay,
                   (int)Annunciators[nAnnunID].x_pos,
                   (int)Annunciators[nAnnunID].y_pos,
                   nWidth, nHeight, pBmp, 0, 0, AEE_RO_COPY);
  (void)IDISPLAY_SetDestination(pMe->m_coreObj->m_piDisplay, NULL); /* restore the destination */

  annun_disp_update (pMe, pMe->m_coreObj->m_pDDB, nAnnunID);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY

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

  if (pMe == NULL) {
    return;
  }

  /* Clear the blinking fields on primary annunciator */
  for (i=0; i < (int)ARR_SIZE(Annunciators); i++) {
    if (Annunciators[i].pcontent->nCurrState & ANNUN_STATE_BLINK) {
      if (bDraw) {
        if (ClearField(pMe, Annunciators[i].nFieldID) != SUCCESS)
        {
          MSG_LOW("Unable to clear field %d for annunciator blink.", i, 0, 0);
        }
      }
      else {
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
  (void)ISHELL_SetTimer(pMe->m_piShell, 320, (PFNNOTIFY) BlinkTimerCB, pMe);
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

  if (pMe->m_coreObj->m_bEnableBlink && !pMe->m_coreObj->m_bTimer) {
    (void)ISHELL_SetTimer(pMe->m_piShell, 320, (PFNNOTIFY) BlinkTimerCB, pMe);
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
      (void)ISHELL_CancelTimer (pMe->m_piShell, NULL, pMe);
      pMe->m_coreObj->m_bTimer = FALSE;
    }
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
#ifndef FEATURE_MDP
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
    (void)ISHELL_AddRef(piShell);

    IAnnunCoreObj->m_uRefs = 1;

    IAnnunCoreObj->m_bTimer = FALSE;

    IAnnunCoreObj->m_pAnnunciators_ovId = NULL;

    IAnnunCoreObj->m_bAnnunciatorOn = TRUE;
    IAnnunCoreObj->m_piDisplay = NULL;
    IAnnunCoreObj->m_pDDB = NULL;

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
    IAnnunCoreObj->m_bEnableBlink = TRUE;

//
// configuring primary LCD width

    dinfo = disp_get_info();
    IAnnunCoreObj->m_annun_prim_lcd_width = dinfo.disp_width;
    IAnnunCoreObj->m_annun_prim_lcd_height = ANNUN_PRIMARY_HEIGHT;
//
// configuring secondary LCD width

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif // #ifdef FEATURE_SECONDARY_DISPLAY

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

#ifndef FEATURE_MDP
    if(SUCCESS == IBITMAP_QueryInterface(pDevBmp, AEECLSID_BITMAPCTL, (void **) &pBitmapCtl))
    {
      IBITMAPCTL_Enable(pBitmapCtl, TRUE);
      (void)IBITMAPCTL_Release(pBitmapCtl);
    }
#endif

    (void)IBITMAP_CreateCompatibleBitmap(pDevBmp, &pBmp,
                                   (uint16)IAnnunCoreObj->m_annun_prim_lcd_width,
                                   (uint16)IAnnunCoreObj->m_annun_prim_lcd_height);
    (void)IBITMAP_Release(pDevBmp);

    if (pBmp == NULL) {
      (void)OEMAnnunCore_Release();
      return NULL;
    }

    if (IBITMAP_QueryInterface (pBmp, AEECLSID_DIB, (void **) &IAnnunCoreObj->m_pDDB) !=
        SUCCESS) {
      (void)IBITMAP_Release (pBmp);
      (void)IDISPLAY_Release(IAnnunCoreObj->m_piDisplay);
      (void)OEMAnnunCore_Release();
      return NULL;
    }
    // clear annunciator bitmap area
    pConvertedBmp = IDIB_TO_IBITMAP(IAnnunCoreObj->m_pDDB);
    IAnnunCoreObj->m_bg = IBITMAP_RGBToNative (pConvertedBmp, RGB_WHITE);
    SETAEERECT(&Rect, 0,0, IAnnunCoreObj->m_annun_prim_lcd_width, IAnnunCoreObj->m_annun_prim_lcd_height);
    (void)IBITMAP_FillRect (pConvertedBmp, &Rect, IAnnunCoreObj->m_bg, AEE_RO_COPY);

    (void)IBITMAP_Release (pBmp);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY
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
      (void)ISHELL_CancelTimer(pMe->m_piShell, NULL, pMe);
      pMe->m_bTimer = FALSE;
    }

  for (i=0; i < (int)ARR_SIZE(Annunciators);i++) {
    data_ptr = (OEMState_data *)Annunciators[i].pcontent->data;
    for (j=0; j < (int)Annunciators[i].pcontent->nMaxStates; j++) {
      if (data_ptr->pBmp != NULL) {
        (void)IBITMAP_Release(data_ptr->pBmp);
        data_ptr->pBmp = NULL;
        data_ptr++;
      }
    }
  }
  (void)ISHELL_Release(pMe->m_piShell);

  if (pMe->m_piDisplay)
  {
    (void)IDISPLAY_Release(pMe->m_piDisplay);
    pMe->m_piDisplay = NULL;
  }
  if (pMe->m_pDDB)
  {
    (void)IDIB_Release(pMe->m_pDDB);
    pMe->m_pDDB = NULL;
  }

  /* For now, we don't need to explicitly release bitmaps for the
   * secondary  annunciator, since the content is shared with the
   * primary annunciator.
   */
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif

  pMe->m_bAnnunciatorOn = FALSE;
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
   OEMAnnunciator_SelectFonts(pMe->m_coreObj->m_piDisplay);

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
    if(pMe->m_coreObj)
    {
      (void)OEMAnnunCore_Release();
      pMe->m_coreObj = NULL;
    }
    CALLBACK_Cancel(&pMe->m_cbSysObj);
    (void)ISHELL_Release(pMe->m_piShell);
    FREE(pMe);
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

  if (nAnnunID >= ARR_SIZE(Annunciators))
    return EFAILED;
  if (Annunciators[nAnnunID].pcontent->nFieldType != ANNUN_TYPE_IMAGE)
    return EFAILED;
  if (Annunciators[nAnnunID].pcontent->nMaxStates < (nState &
                                                     ANNUN_STATE_BLINK_UNMASK))
    return EFAILED;
  if (Annunciators[nAnnunID].pcontent->data == NULL)
    return EFAILED;
  if (pMe == NULL) {
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
      SetBlinkTimer(pMe, nAnnunID);
    }
    else {
      CancelBlinkTimer (pMe, nAnnunID);
    }

    /* Image field */
    if (DrawImageField(pMe, nAnnunID, (nState & ANNUN_STATE_BLINK_UNMASK)) !=
        SUCCESS)
    {
      MSG_LOW("Unable to draw field %d for annunciator set, state %d.",
              nAnnunID, Annunciators[nAnnunID].pcontent->nCurrState, 0);
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
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
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
  if ((Annunciators[nAnnunID].pcontent->nFieldType != ANNUN_TYPE_TEXT) &&
       (nAnnunID != ANNUN_FIELD_OPERATOR))
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

  (void)IBITMAP_FillRect (pIBitmap, &Rect, pMe->m_coreObj->m_bg, AEE_RO_COPY);
  (void)IDISPLAY_SetDestination (pMe->m_coreObj->m_piDisplay, pIBitmap);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY

  pszText = (AECHAR *) pData;

  if(nAnnunID != ANNUN_FIELD_OPERATOR) // Reserved field is image.
  {
    (void)WSTRLCPY((AECHAR *) Annunciators[nAnnunID].pcontent->data, pszText, ANNUN_TEXT_SIZE);
  }
  if (pszText != NULL) {
    (void)IDISPLAY_DrawText(pMe->m_coreObj->m_piDisplay, AEE_FONT_BOLD, pszText, -1, 0, 0,
                            &Rect, IDF_ALIGN_TOP|IDF_ALIGN_LEFT|IDF_ALIGN_FILL);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY
  }

  /* restore the destination */
  (void)IDISPLAY_SetDestination(pMe->m_coreObj->m_piDisplay, NULL);
  annun_disp_update( pMe, pMe->m_coreObj->m_pDDB, nAnnunID);

#if defined(FEATURE_SECONDARY_DISPLAY)
#error code not present
#endif // FEATURE_SECONDARY_DISPLAY


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
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
#ifndef FEATURE_MDP
  IBitmap        *pBmp;                        // BMP for primary display
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
   AEEBitmapInfo   sBmpInfo;                    // Bmp info struct
   AEERect         sBmpRect;                    // Rectangle to invalidate
#endif // ! FEATURE_MDP
   int             nRetVal = EFAILED;           // Return code
   PACONTEXT       pac;                         // Calling context

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
#ifdef FEATURE_MDP
      if (mdp_is_ovimg_valid(pMe->m_coreObj->m_pAnnunciators_ovId) == TRUE)
      {
         /* Redraw all annunciator items */
         (void)mdp_update_ovimg(pMe->m_coreObj->m_pAnnunciators_ovId);
      }
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
      IDISPLAY_BitBlt(pMe->m_coreObj->m_piDisplay,
                        0,
                        0,
                        pMe->m_coreObj->m_annun_prim_lcd_width,
                        pMe->m_coreObj->m_annun_prim_lcd_height,
                        IDIB_TO_IBITMAP(pMe->m_coreObj->m_pDDB),
                        0,
                        0,
                        AEE_RO_COPY);
      IDISPLAY_Update(pMe->m_coreObj->m_piDisplay);
#endif // ! FEATURE_MDP

   }
   else
   {
      bIsBmpInvalidated = TRUE;
   }

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#else
   if (bIsBmpInvalidated)
   {
      nRetVal = SUCCESS;
   }
#endif

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
#ifdef FEATURE_MDP
  uint32  ovId = NULL;
  PACONTEXT pacLast;
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
#ifdef FEATURE_MDP
        ovId = pMe->m_coreObj->m_pAnnunciators_ovId;
#endif /* FEATURE_MDP */
        break;

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif

     default:
        return ECLASSNOTSUPPORT;
  }

  if(bOn != bLastState)
  {
#ifdef FEATURE_MDP
      boolean bSuccess = TRUE;
      pacLast = AEE_EnterAppContext(NULL);
      if (mdp_is_ovimg_valid(ovId) == TRUE)
      {
         // Unlock the annunciator bar
         if (mdp_lock_ovimg(ovId, FALSE) == MDP_SUCCESSFUL)
         {
            if (mdp_enable_ovimg(ovId, bOn) != (int32)MDP_SUCCESSFUL)
            {
              bSuccess = FALSE;
              MSG_LOW("Unable to set overlay %d state to %d", ovId, bOn, 0);
            }
         }
         else
         {
            bSuccess = FALSE;
            MSG_LOW("Unable to lock overlay, id %d", ovId, 0, 0);
         }
         if (mdp_lock_ovimg(ovId, TRUE) != MDP_SUCCESSFUL)
         {
            bSuccess = FALSE;
            MSG_LOW("Unable to unlock overlay, id %d", ovId, 0, 0);
         }
      }
      else
      {
         bSuccess = FALSE;
         MSG_LOW("Invalid overlay id, %d", ovId, 0, 0);
      }

      AEE_LeaveAppContext(pacLast);
      if (bSuccess != TRUE)
      {
        return EFAILED;
      }
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

#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif

     default:
        return ECLASSNOTSUPPORT;
  }

  return SUCCESS;
}

/* =======================================================================

IANNUNCIATOR_EnableBlink()

Description:
    This function enables or disables the blinking of annunciator images.

Prototype:
    int IANNUNCIATOR_EnableBlink(IAnnunciator * pIAnnunciator, boolean bEnable)

Parameters:
    pIAnnunciator : Pointer to the IAnnunciator Interface object
    bOn           : TRUE/Enable annunciator blink feature.

Return Value:

Comments:
    None

Side Effects:

See Also:


======================================================================= */
static void IAnnunciator_EnableBlink(IAnnunciator * pMe, boolean bEnable)
{
  //Error check
  if (NULL == pMe)
  {
    return;
  }

  if (NULL == pMe->m_coreObj)
  {
    return;
  }

  if (bEnable && !pMe->m_coreObj->m_bTimer)
  {
    //Enable blink and timer isn't currently active
    int i;
    boolean found = FALSE;

    //Search for annunciator with blink state set to active.
    for (i=0; i < (int)ARR_SIZE(Annunciators);i++) {
      if (Annunciators[i].pcontent->nCurrState & ANNUN_STATE_BLINK) {
        found = TRUE;
        break;
      }
    }
    if (found) {
      (void)ISHELL_SetTimer(pMe->m_piShell, 320, (PFNNOTIFY) BlinkTimerCB, pMe);
      pMe->m_coreObj->m_bTimer = TRUE;
    }
  } 
  else if (!bEnable && pMe->m_coreObj->m_bTimer)
  {
    //Cancel any active blink timer.
    (void)ISHELL_CancelTimer (pMe->m_piShell, NULL, pMe);
    pMe->m_coreObj->m_bTimer = FALSE;
  }

  pMe->m_coreObj->m_bEnableBlink = bEnable;
}

#if MIN_BREW_VERSIONEx(3,1,4)
/*===========================================================================
FUNCTION OEMAnnunciator_CleanupFontData

DESCRIPTION
  Restores the original display font settings and releases resources
  that were allocated for setting up the fonts.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void OEMAnnunciator_CleanupFontData(IDisplay* pIDisp,
                                           IFont** ppOldNormalFont,
                                           IFont** ppOldBoldFont,
                                           IFont** ppOldLargeFont)
{
   IFont  *pIFont;
   IFont* pOldNormalFont = NULL;
   IFont* pOldBoldFont = NULL;
   IFont* pOldLargeFont = NULL;

   if(ppOldNormalFont != NULL)
   {
      pOldNormalFont = *ppOldNormalFont;
   }
   if(ppOldBoldFont != NULL)
   {
      pOldBoldFont   = *ppOldBoldFont;
   }
   if(ppOldLargeFont != NULL)
   {
      pOldLargeFont  = *ppOldLargeFont;
   }

   /* For each of the fonts, if we were able to set it (i.e. if the "old font"
    * pointer is not NULL, we need to revert to the old font and release
    * the font we created.
    */
   if (pOldNormalFont != NULL)
   {
      pIFont = NULL;
      pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_NORMAL, pOldNormalFont);
      if (pIFont == NULL)
      {
         MSG_MED("Set font failed.", 0, 0, 0);
      }
      else
      {
         (void)IFONT_Release(pIFont);
      }
   }
      
   if (pOldBoldFont != NULL)
   {
      pIFont = NULL;
      pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_BOLD, pOldBoldFont);
      if (pIFont == NULL)
      {
         MSG_MED("Set font failed.", 0, 0, 0);
      }
      else
      {
         (void)IFONT_Release(pIFont);
      }
   }

   if (pOldLargeFont != NULL)
   {
      pIFont = NULL;
      pIFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_LARGE, pOldLargeFont);
      if (pIFont == NULL)
      {
         MSG_MED("Set font failed.", 0, 0, 0);
      }
      else
      {
         (void)IFONT_Release(pIFont);
      }
   }
 
   *ppOldNormalFont = NULL;
   *ppOldBoldFont = NULL;
   *ppOldLargeFont = NULL;
}
#endif // MIN_BREW_VERSIONEx(3,1,4)

/*===========================================================================  
FUNCTION OEMAnnunciator_SelectFonts

DESCRIPTION
  Forces the annunciator to use fonts that work with the annunciator
  field sizes.  These are also the default fonts on displays smaller
  than VGA.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
static void OEMAnnunciator_SelectFonts(IDisplay* pIDisp)
{
   // Only targets with brew 3.1.4 or greater have these capabilities
#if MIN_BREW_VERSIONEx(3,1,4)
   IFont    *pIFont;
   int      nErrCode;
   boolean  bFontsWereSet = FALSE;
   IFont    *pOldNormalFont = NULL;
   IFont    *pOldBoldFont = NULL;
   IFont    *pOldLargeFont = NULL;
   IShell   *pIShell = NULL;    ACONTEXT *pacRestore;

   /* We want to set the fonts for the entire display.  If that fails,
    * we'll revert back to the old font values.
    */

   if (pIDisp == NULL)
   {
      MSG_MED("NULL IDisplay pointer.", 0, 0, 0);
      return;
   }
   pIShell = (IShell*)AEE_GetShell();   if (pIShell == NULL)   {      MSG_HIGH("AEE_GetShell failed.", 0, 0, 0);      return;   }
   pacRestore = AEE_EnterAppContext(NULL);
   do
   {
      pIFont = NULL;
      nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_BASIC11,
                                       (void **)&pIFont);
      if ((nErrCode != SUCCESS) || (pIFont == NULL))
      {
         MSG_MED("Failed to create a font.", 0, 0, 0);
         break;
      }
      pOldNormalFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_NORMAL, pIFont);
      if (pOldNormalFont == NULL)
      {
         (void)IFONT_Release(pIFont);
         MSG_MED("Failed to set a font.", 0, 0, 0);
         break;
      }

      pIFont = NULL;
      nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_BASIC11B,
                                       (void **)&pIFont);
      if ((nErrCode != SUCCESS) || (pIFont == NULL))
      {
         MSG_MED("Failed to create a font.", 0, 0, 0);
         break;
      }
      pOldBoldFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_BOLD, pIFont);
      if (pOldBoldFont == NULL)
      {
         (void)IFONT_Release(pIFont);
         MSG_MED("Failed to set a font.", 0, 0, 0);
         break;
      }
    
      pIFont = NULL;
      nErrCode = ISHELL_CreateInstance(pIShell, AEECLSID_FONT_BASIC14,
                                       (void **)&pIFont);
      if ((nErrCode != SUCCESS) || (pIFont == NULL))
      {
         MSG_MED("Failed to create a font.", 0, 0, 0);
         break;
      }
      pOldLargeFont = IDISPLAY_SetFont(pIDisp, AEE_FONT_LARGE, pIFont);
      if (pOldLargeFont == NULL)
      {
         (void)IFONT_Release(pIFont);
         MSG_MED("Failed to set a font.", 0, 0, 0);
         break;
      }

      bFontsWereSet = TRUE;

   } while (0); /*----  lint -save -e506 "constant value boolean" */
   AEE_LeaveAppContext(pacRestore);

   if (bFontsWereSet == FALSE)
   {
      OEMAnnunciator_CleanupFontData(pIDisp, &pOldNormalFont,
                                     &pOldBoldFont, &pOldLargeFont);
   }
   else
   {
      /* If successful, release all the old fonts, as no one has access
       * to them anymore.
       */
      (void)IFONT_Release(pOldNormalFont);
      (void)IFONT_Release(pOldBoldFont);
      (void)IFONT_Release(pOldLargeFont);
   }

#else  
   return;
#endif
}


//lint -esym(754, IANNUNCore::m_bg2)  Suppress "not referenced"
//lint -esym(754, IAnnunciator::pvt)  Suppress "not referenced"
