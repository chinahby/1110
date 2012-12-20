/*======================================================
FILE:  OEMRUIM.c

SERVICES:  OEM functions for supporting BREW RUIM Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW IRUIM interface.

        Copyright ? 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "OEMFeatures.h"
#ifdef FEATURE_IRUIM     // BREW Feature Define for IRUIM.

#include "comdef.h"        // Get DMSS type declarations.

#include "AEE.h"           // BREW headers.
#include "AEEComdef.h"
#include "AEEError.h"
#include "AEERUIM.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"

#include "ADDRBOOK_RUIM.bid"

// DMSS headers.
#include "rex.h"
#include "ui.h"
#include "uim.h"
#include "nvruimi.h"
#include "uimdrv.h"
#ifdef FEATURE_OEMOMH
#include "gsdi.h"
#endif
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

#ifndef UI_RUIM_SIG
#error  You must define UI_RUIM_SIG to be a unique signal in UI task.
#endif

#define BAD_PIN_CHAR 0xff
#define UIM_CDMA_FEATURE_CODE_SIZE MAX_REQUEST_LEN
#define UIM_CDMA_FEATURE_CODE_NUM_DIGI 2*2

struct IRUIM
{
   IRUIMVtbl  *pvt;
   IShell     *m_pIShell;
   uint32      m_uRefs;
   AEECLSID    m_cls;
};

// Interface function prototypes.
static uint32 OEMRUIM_AddRef(IRUIM *pMe);
static uint32 OEMRUIM_Release(IRUIM *pMe);
static int OEMRUIM_QueryInterface(IRUIM *pMe, AEECLSID id,
                                  void **ppNew);
static boolean OEMRUIM_IsCardConnected(IRUIM *pMe);
static int OEMRUIM_GetCHVStatus(IRUIM *pMe,
                                AEECHVStatus *pCHVStatus);
static boolean OEMRUIM_PINCheck(IRUIM *pMe, AEECHVType chv, const char *pPin,
                            boolean bVirtualPIN);
static int OEMRUIM_PINChange(IRUIM *pMe, AEECHVType chv,
                             const char *pPin);
static boolean OEMRUIM_CHVEnable(IRUIM *pMe, const char *pPin);
static int OEMRUIM_CHVDisable(IRUIM *pMe);
static int OEMRUIM_UnBlockCHV(IRUIM *pMe, AEECHVType chv,
                              const char *pUnblockPin,
                              const char *pPin);
static int OEMRUIM_GetPrefLang(IRUIM *pMe, int *pLang,
                               int *pEnconding);
static int OEMRUIM_GetId(IRUIM *pMe, char *pId, int *pnLen);

// Local helper function prototypes.
static void OEMRUIM_report (uim_rpt_type *report);
static void OEMRUIM_send_uim_cmd_and_wait (uim_cmd_type *uim_cmd_ptr);
static int OEMRUIM_Get_Feature_Code(IRUIM *pMe,byte *Buf,int  Lable);

static byte OEMRUIM_bcd_to_ascii(byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */
                                 byte *to_ptr   /* Convert to */);

static int OEMRUIM_Read_Svc_P_Name(IRUIM *pMe , AECHAR *svc_p_name);
static void OEMRUIM_Conversion_Uimdata_To_Spn(byte *Inputbuf,AECHAR *svc_p_name,int Endpoint);

#ifdef FEATURE_OEMOMH 
static int OEMRUIM_Get_Ecc_Code(IRUIM *pMe,byte *pBuf,int *pnNum,int step);
static byte OEMRUIM_bcd_to_ascii_forOMH(byte num_digi, byte *from_ptr, byte *to_ptr);
static boolean OEMRUIM_WriteModel(IRUIM *pMe, byte *Buf);
static int OEMRUIM_Get_AppLabels_Code(IRUIM *pMe,int nId, AECHAR *Buf);
#endif

static const VTBL(IRUIM) gOEMRUIMFuncs = {
   OEMRUIM_AddRef,
   OEMRUIM_Release,
   OEMRUIM_QueryInterface,
   OEMRUIM_IsCardConnected,
   OEMRUIM_GetCHVStatus,
   OEMRUIM_PINCheck,
   OEMRUIM_PINChange,
   OEMRUIM_CHVEnable,
   OEMRUIM_CHVDisable,
   OEMRUIM_UnBlockCHV,
   OEMRUIM_GetPrefLang,
   OEMRUIM_GetId,
   OEMRUIM_Get_Feature_Code,
   OEMRUIM_Read_Svc_P_Name
#ifdef FEATURE_OEMOMH 
  ,OEMRUIM_Get_Ecc_Code,
  OEMRUIM_WriteModel,
  OEMRUIM_Get_AppLabels_Code
#endif  
};

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/
// External global variables.
extern rex_tcb_type ui_tcb;

// Local global variables.
static char           gVirtualPin[2][UIM_MAX_CHV_DIGITS] = {
                                                  {BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR},
                                                  {BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR,
                                                   BAD_PIN_CHAR, BAD_PIN_CHAR}};
static uim_cmd_type   gUimCmd;
static uim_rpt_type   gCallBack;
#ifdef FEATURE_OEMOMH 
static char        gECC[RUIM_ECC_NUMBER][RUIM_ECC_MAXSIZE];
static int         gECCNum = 0;
static boolean     gbECCInited = FALSE;
static AECHAR      gAppLabels[RUIM_APPLABEL_NUM][RUIN_APPLABEL_SIZE+1];
static boolean     gbAppLabelsInited = FALSE;
static uint16      gwAppLabelsInd;
#endif
static boolean     gbFeatureCodeInited = FALSE;
static byte        gFeatureCode[UIM_CDMA_FEATURE_CODE_SIZE];

/*===========================================================================

FUNCTION OEMRUIM_New
   <see OEMRUIM_New() documentation in OEMRUIM.h>

===========================================================================*/
int OEMRUIM_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   IRUIM *pMe = NULL;

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }
   if (cls != AEECLSID_RUIM) {
      return EUNSUPPORTED;
   }

   // Allocate the object.
   if ((pMe = (IRUIM*)sys_malloc(sizeof(IRUIM))) == NULL) {
      return ENOMEMORY;
   }

   pMe->pvt = (IRUIMVtbl *)&gOEMRUIMFuncs;
   pMe->m_pIShell = pIShell;
   pMe->m_cls = cls;
   pMe->m_uRefs = 1;
   
   *ppif = pMe;
   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION OEMRUIM_AddRef
   <see IRUIM_AddRef() documentation in AEERUIM.h>

===========================================================================*/
static uint32 OEMRUIM_AddRef(IRUIM *pMe)
{
   if (pMe) {
      return (++pMe->m_uRefs);
   }
   return 0;
}

/*===========================================================================

FUNCTION OEMRUIM_Release
   <see IRUIM_Release() documentation in AEERUIM.h>

===========================================================================*/
static uint32 OEMRUIM_Release(IRUIM *pMe)
{
   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   // Ref count is zero.  Release memory
   sys_free(pMe);
   return (0);
}

/*===========================================================================

FUNCTION OEMRUIM_QueryInterface
   <see IRUIM_QueryInterface() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_QueryInterface(IRUIM *pMe, AEECLSID id, void **ppNew)
{
   IShell *pIShell = NULL;
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   switch (id) {
      case AEECLSID_RUIM:
         IRUIM_AddRef(pMe);
         *ppNew = pMe;
         break;
      case AEECLSID_ADDRBOOK:
      case AEECLSID_ADDRBOOK_20:
      case AEECLSID_ADDRBOOK_21:
         ret_val = ISHELL_CreateInstance(pIShell, AEECLSID_ADDRBOOK_RUIM, (void **)ppNew);
         break;
      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }
   return ret_val;
}

/*===========================================================================

FUNCTION OEMRUIM_IsCardConnected
   <see IRUIM_IsCardConnected() documentation in AEERUIM.h>

===========================================================================*/
static boolean OEMRUIM_IsCardConnected(IRUIM *pMe)
{
   boolean bCardConnected;
   int status;

   if (!pMe) {
      return FALSE;
   }

   status = uim_get_uim_status();

   switch(status) {
      case UIM_NO_SUCH_STATUS_S:         // No such status
      case UIM_INVALID_S:                // Recd the command in an invalid State
      case UIM_UNINITIALIZED_S:          // UIM is not initialized yet
      case UIM_ERR_S:                    // UIM is faulty
      case UIM_PROTOCOL_ERR_S:           // Protocol in the command is invalid
      case UIM_MAXIMUM_S:
         bCardConnected = FALSE;
         break;
      case UIM_RECEIVED_S:               // Recd the command
      case UIM_INITIALIZED_S:            // UIM is initialized
      case UIM_POWERED_DOWN_S:           // UIM is powered down
      case UIM_CLK_OFF_S:                // UIM does not have a clock
         bCardConnected = TRUE;
         break;
      default:
         bCardConnected = FALSE;
         DBGPRINTF("Unknown status value received from uim_get_uim_status()");
         break;
      }

   return bCardConnected;
}

/*===========================================================================

FUNCTION OEMRUIM_GetCHVStatus
   <see IRUIM_GetCHVStatus() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_GetCHVStatus(IRUIM *pMe, AEECHVStatus *pCHVStatus)
{
   int status;

   if ((!pMe) || (!pCHVStatus)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   // Prepare to read the id EF.
   gUimCmd.status.hdr.command            = UIM_STATUS_F;
   gUimCmd.status.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.status.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.status.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.status.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.status.hdr.protocol           = UIM_CDMA;
   gUimCmd.status.hdr.rpt_function       = OEMRUIM_report;

   // From nvruim_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   if ((gCallBack.rpt_type != UIM_STATUS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      status = EFAILED;
   } else {
      if (gCallBack.rpt.status.num_bytes >= 22) {
         pCHVStatus->chv1_enabled      = (gCallBack.rpt.status.data[13] & 0x80)?FALSE:TRUE; // Gemsea Add
         pCHVStatus->chv1CountRemain   = gCallBack.rpt.status.data[18] & 0xf;
         pCHVStatus->chv1UnblockRemain = gCallBack.rpt.status.data[19] & 0xf;
         pCHVStatus->chv2CountRemain   = gCallBack.rpt.status.data[20] & 0xf;
         pCHVStatus->chv2UnblockRemain = gCallBack.rpt.status.data[21] & 0xf;
         status = SUCCESS;
      } else {
         status = EFAILED;
      }
   }

   return (status);
}

/*===========================================================================

FUNCTION OEMRUIM_PINCheck
   <see IRUIM_PINCheck()/IRUIM_VirtualPINCheck() documentation in AEERUIM.h>

===========================================================================*/
static boolean OEMRUIM_PINCheck(IRUIM *pMe, AEECHVType chv, const char *pPin,
                                                            boolean bVirtualPIN)
{
   if ((!pMe) || (!pPin)) {
      return FALSE;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return FALSE;
   }

   if (!bVirtualPIN) {
      // Create & send the command.
      gUimCmd.verify_chv.hdr.command            = UIM_VERIFY_CHV_F;
      gUimCmd.verify_chv.hdr.cmd_hdr.task_ptr   = NULL;
      gUimCmd.verify_chv.hdr.cmd_hdr.sigs       = NULL;
      gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
      gUimCmd.verify_chv.hdr.options            = UIM_OPTION_ALWAYS_RPT;
      gUimCmd.verify_chv.hdr.protocol           = UIM_CDMA;
      gUimCmd.verify_chv.hdr.rpt_function       = OEMRUIM_report;

      gUimCmd.verify_chv.chv = chv;
      MEMCPY((byte *)gUimCmd.verify_chv.chv_digits, pPin, UIM_MAX_CHV_DIGITS);

      OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

      // Review the status from the callback.
      if ((gCallBack.rpt_type == UIM_VERIFY_CHV_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
         MEMCPY (gVirtualPin[chv-1], pPin, UIM_MAX_CHV_DIGITS); // Copy for
                                                                // future ref.
         return TRUE;
      } else {
         MEMSET (gVirtualPin[chv-1], BAD_PIN_CHAR, UIM_MAX_CHV_DIGITS);// Reset.
         return FALSE;
      }
   } else { // Virtual PIN check.
      if (MEMCMP(gVirtualPin[chv-1], pPin, UIM_MAX_CHV_DIGITS) == 0) {
         return TRUE;
      } else {
         return FALSE;
      }
   }
}

/*===========================================================================

FUNCTION OEMRUIM_PINChange
   <see IRUIM_PINChange() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_PINChange(IRUIM *pMe, AEECHVType chv, const char *pPin)
{
   if ((!pMe) || (!pPin)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   // Create & send the command.
   gUimCmd.change_chv.hdr.command            = UIM_CHANGE_CHV_F;
   gUimCmd.change_chv.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.change_chv.hdr.cmd_hdr.sigs       = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.change_chv.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.change_chv.hdr.protocol           = UIM_CDMA;
   gUimCmd.change_chv.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.change_chv.chv = chv;
   MEMCPY((byte *)gUimCmd.change_chv.chv_digits, gVirtualPin[chv-1],
                                                            UIM_MAX_CHV_DIGITS);
   MEMCPY((byte *)gUimCmd.change_chv.new_chv_digits, pPin, UIM_MAX_CHV_DIGITS);

   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   // Review the status from the callback.
   if ((gCallBack.rpt_type == UIM_CHANGE_CHV_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      MEMCPY (gVirtualPin[chv-1], pPin, UIM_MAX_CHV_DIGITS); // Copy for future
                                                             // ref.
      return AEE_SUCCESS;
   }
 
   return EFAILED;
}

/*===========================================================================

FUNCTION OEMRUIM_CHVEnable
   <see IRUIM_CHVEnable() documentation in AEERUIM.h>

===========================================================================*/
static boolean OEMRUIM_CHVEnable(IRUIM *pMe, const char *pPin)
{
   if (!pMe) {
      return FALSE;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return FALSE;
   }

   // Create & send the command.
   gUimCmd.enable_chv.hdr.command            = UIM_ENABLE_CHV_F;
   gUimCmd.enable_chv.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.enable_chv.hdr.cmd_hdr.sigs       = NULL;
   gUimCmd.enable_chv.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.enable_chv.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.enable_chv.hdr.protocol           = UIM_CDMA;
   gUimCmd.enable_chv.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.enable_chv.chv = IRUIM_CHV1;
   if (pPin) {
      MEMCPY((byte *)gUimCmd.enable_chv.chv_digits, pPin, UIM_MAX_CHV_DIGITS);
   } else {
      MEMCPY((byte *)gUimCmd.enable_chv.chv_digits, gVirtualPin[0],
                                                            UIM_MAX_CHV_DIGITS);
   }

   OEMRUIM_send_uim_cmd_and_wait(&gUimCmd);

   // Review the status from the callback.
   if ((gCallBack.rpt_type == UIM_ENABLE_CHV_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      if (pPin) {                            // Copy for future reference.
         MEMCPY (gVirtualPin[0], pPin, UIM_MAX_CHV_DIGITS);
      }
      return TRUE;
   }

   return FALSE;
}

boolean OEM_IsCDMASVCSupport(uim_cdma_svc_table_enum_type eType)
{
#ifdef FEATURE_OEMOMH 
    byte uim_cdma_svc_table_buffer[UIM_CDMA_SVC_TABLE_SIZE];
    uim_svc_table_return_type svc;            /* service allocated and activated*/
	
	MSG_FATAL("OEM_IsCDMASVCSupport %d",eType,0,0);
    gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
    gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
    gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
    gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
    gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
    gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
    gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

    gUimCmd.access_uim.item      = UIM_CDMA_CDMA_SVC_TABLE;
    gUimCmd.access_uim.access    = UIM_READ_F;
    gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    gUimCmd.access_uim.num_bytes = UIM_CDMA_SVC_TABLE_SIZE;
    gUimCmd.access_uim.offset    = 0;
    gUimCmd.access_uim.data_ptr  = uim_cdma_svc_table_buffer;

    // From nvruim_access():  Access an EF, do not signal any task, use no
    // signal, no done queue, use a callback, always report status.

    // Send the command to the R-UIM:
    OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);
    
    if ((gCallBack.rpt_type == UIM_ACCESS_R) &&
        (gCallBack.rpt_status == UIM_PASS))
    {
        /* Check to see if UIM is Pro-Active */
        svc = uim_return_cdma_svc_availabililty ( eType, uim_cdma_svc_table_buffer);
        MSG_FATAL("OEM_IsCDMASVCSupport %d %d",svc.allocated,svc.activated,0);
        if(!svc.allocated || !svc.activated)
        {
            return FALSE;
        }
    }
#endif
    return TRUE;
}

/*===========================================================================

FUNCTION OEMRUIM_CHVDisable
   <see IRUIM_CHVDisable() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_CHVDisable(IRUIM *pMe)
{
   if (!pMe) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }
   
   if(!OEM_IsCDMASVCSupport(UIM_CDMA_SVC_CHV_DISABLE))
   {
      return EFAILED;
   }
   
   // Create & send the command.
   gUimCmd.disable_chv.hdr.command            = UIM_DISABLE_CHV_F;
   gUimCmd.disable_chv.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.disable_chv.hdr.cmd_hdr.sigs       = NULL;
   gUimCmd.disable_chv.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.disable_chv.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.disable_chv.hdr.protocol           = UIM_CDMA;
   gUimCmd.disable_chv.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.disable_chv.chv = UIM_CHV1;
   MEMCPY((byte *)gUimCmd.disable_chv.chv_digits, gVirtualPin[0],
                                                            UIM_MAX_CHV_DIGITS);

   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   // Review the status from the callback.
   if ((gCallBack.rpt_type == UIM_DISABLE_CHV_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      return AEE_SUCCESS;
   }
 
   return EFAILED;
}

/*===========================================================================

FUNCTION OEMRUIM_UnBlockCHV
   <see IRUIM_UnBlockCHV() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_UnBlockCHV(IRUIM *pMe, AEECHVType chv,
                                      const char *pUnblockPin, const char *pPin)
{
   if ((!pMe) || (!pPin) || (!pUnblockPin)) {
      return EFAILED;
   }

   // Check the priv. level.
   if (!ISHELL_CheckPrivLevel(pMe->m_pIShell, PL_SYSTEM, FALSE)) {
      return EPRIVLEVEL;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   // Create & send the command.
   gUimCmd.unblk_chv.hdr.command            = UIM_UNBLOCK_CHV_F;
   gUimCmd.unblk_chv.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.unblk_chv.hdr.cmd_hdr.sigs       = NULL;
   gUimCmd.unblk_chv.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.unblk_chv.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.unblk_chv.hdr.protocol           = UIM_CDMA;
   gUimCmd.unblk_chv.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.unblk_chv.chv = chv;
   MEMCPY((byte *)gUimCmd.unblk_chv.chv_digits, pUnblockPin,
                                                            UIM_MAX_CHV_DIGITS);
   MEMCPY((byte *)gUimCmd.unblk_chv.new_chv_digits, pPin, UIM_MAX_CHV_DIGITS);

   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   // Review the status from the callback.
   if ((gCallBack.rpt_type == UIM_UNBLOCK_CHV_R)
                                        && (gCallBack.rpt_status == UIM_PASS)) {
      MEMCPY (gVirtualPin[chv-1], pPin, UIM_MAX_CHV_DIGITS); // Copy for future
                                                             // ref.
      return AEE_SUCCESS;
   }
 
   return EFAILED;
}

/*===========================================================================

FUNCTION OEMRUIM_GetPrefLang
   <see IRUIM_GetPrefLang() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_GetPrefLang(IRUIM *pMe, int *plang, int *pencoding)
{
   byte *ruim_buffer;
   int status = EFAILED;

   if ((!pMe) || (!plang) || (!pencoding)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   ruim_buffer = MALLOC(NVRUIM_EF_BUF_SIZE);
   if (!ruim_buffer) {
      return EFAILED;
   }

   // Prepare to read the id EF.
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_CDMA_PREF_LANG;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = 1;
   gUimCmd.access_uim.offset    = 1;
   gUimCmd.access_uim.data_ptr  = ruim_buffer;

   // From nvruim_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   if ((gCallBack.rpt_type != UIM_ACCESS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      status = EFAILED;
   } else {
      *plang = ruim_buffer[0];
      *pencoding = 0;
      status = AEE_SUCCESS;
   }

   FREE (ruim_buffer);
   return status;
}

/*===========================================================================

FUNCTION OEMRUIM_GetId
   <see IRUIM_GetId() documentation in AEERUIM.h>

===========================================================================*/
static int OEMRUIM_GetId(IRUIM *pMe, char *pId, int *pnLen)
{
#define RUIM_ID_SIZE ((word)(sizeof(((nv_item_type *) 0)-> esn.esn )))

   byte *ruim_buffer;

   if ((!pMe) || (!pnLen)) {
      return EFAILED;
   }

   // Check to see if the card is connected.
   if (!IRUIM_IsCardConnected (pMe)) {
      return EFAILED;
   }

   ruim_buffer = MALLOC(RUIM_ID_SIZE);
   if (!ruim_buffer) {
      return EFAILED;
   }

   // Prepare to read the id EF.
   gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
   gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
   gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
   gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
   gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
   gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
   gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

   gUimCmd.access_uim.item      = UIM_CDMA_RUIM_ID;
   gUimCmd.access_uim.access    = UIM_READ_F;
   gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
   gUimCmd.access_uim.num_bytes = RUIM_ID_SIZE;
   gUimCmd.access_uim.offset    = 1;
   gUimCmd.access_uim.data_ptr  = ruim_buffer;

   // From nvruim_access():  Access an EF, do not signal any task, use no
   // signal, no done queue, use a callback, always report status.

   // Send the command to the R-UIM:
   OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

   if ((gCallBack.rpt_type != UIM_ACCESS_R)
                                        || (gCallBack.rpt_status != UIM_PASS)) {
      FREE(ruim_buffer);
      return EFAILED;
   }

   if (pId == NULL) {
      *pnLen = gUimCmd.access_uim.num_bytes_rsp;
   } else {
      *pnLen = MIN(*pnLen, gUimCmd.access_uim.num_bytes_rsp);
      MEMCPY(pId, ruim_buffer, *pnLen);
   }

   FREE(ruim_buffer);
   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION OEMRUIM_report

===========================================================================*/
static void OEMRUIM_report (uim_rpt_type *report)
{
   MEMCPY(&gCallBack, report, sizeof(uim_rpt_type));
   rex_set_sigs( &ui_tcb, UI_RUIM_SIG);
}

/*===========================================================================

FUNCTION OEMRUIM_send_uim_cmd_and_wait

===========================================================================*/
static void OEMRUIM_send_uim_cmd_and_wait (uim_cmd_type *uim_cmd_ptr)
{
   (void) rex_clr_sigs( &ui_tcb, UI_RUIM_SIG);

   // Send the command.
   uim_cmd (uim_cmd_ptr);

   (void) rex_wait( UI_RUIM_SIG);
}

static void OEMRUIM_Conversion_Uimdata_To_WStr(byte *Inputbuf, int nInputSize, byte ecd_ind, byte lng_ind, AECHAR *AppLabel,int Endpoint)
{
    int i=0;
    int wStrLen;
    boolean bBigEndian = FALSE;
    
    MSG_FATAL("OEMRUIM_Conversion_Uimdata_To_WStr Start", 0, 0, 0);
    if(!AppLabel || !Inputbuf)
    {
        return;
    }
    
    switch(ecd_ind){
    case  AEERUIM_LANG_ENCODING_UNICODE:                   //UNICODE编码
        wStrLen = MIN(nInputSize/2, Endpoint);
        MSG_FATAL("OEMRUIM_Conversion_Uimdata_To_WStr 1 wStrLen=%d", wStrLen, 0, 0);
        if((Inputbuf[0] == 0xFF) && (Inputbuf[1] == 0xFE))//(Little endian 00-54 with BOM)
        {
            // Remove BOM
            wStrLen-=1;
            Inputbuf+=2;
            bBigEndian = FALSE;
        }
        else if((Inputbuf[0] == 0xFE) && (Inputbuf[1] == 0xFF)) // Big Endian
        {
            // Remove BOM
            wStrLen-=1;
            Inputbuf+=2;
            bBigEndian = TRUE;
        }
        else
        {
            // without BOM
            if(((lng_ind == 0x01) && (Inputbuf[0] != 0x00)))
            {
                bBigEndian = FALSE;
            }
            else
            {
                //lng_ind为01表示是英文
                //AppLabel第一位接着为00表示大端
                //我们的手机是小端，大端的要交换高低位
                //默认RUIM卡应该是bigendian
                bBigEndian = TRUE;
            }
        }
        MSG_FATAL("OEMRUIM_Conversion_Uimdata_To_WStr 2", 0, 0, 0);
        if(bBigEndian)
        {
            MSG_FATAL("is BigEndian wStrLen=%d", wStrLen, 0, 0);
            for(i=0; i<wStrLen; i++)
            {
                if(Inputbuf[i] != 0xFF && Inputbuf[i+1] != 0xFF)
                {
                    AppLabel[i]=(AECHAR)(Inputbuf[2*i]<<8)|Inputbuf[(2*i)+1];
                }
                else
                {
                    break;
                }
            }
            MSG_FATAL("is BigEndian i=%d", i, 0, 0);
            AppLabel[i]='\0';
        }
        else
        {
            MSG_FATAL("not BigEndian wStrLen=%d", wStrLen, 0, 0);
            for(i=0; i<wStrLen; i++)
            {
                if(Inputbuf[i] != 0xFF && Inputbuf[i+1] != 0xFF)
                {
                    AppLabel[i]=(AECHAR)(Inputbuf[(2*i)+1]<<8)|Inputbuf[2*i];
                }
                else
                {
                    break;
                }
            }
            AppLabel[i]='\0';
            MSG_FATAL("is not BigEndian i=%d", i, 0, 0);
        }
        break;
        
    case AEERUIM_LANG_ENCODING_7BIT:                  //acsii编码
    case AEERUIM_LANG_ENCODING_LATINHEBREW:
    case AEERUIM_LANG_ENCODING_LATIN:
    case AEERUIM_LANG_ENCODING_OCTET:
    default:
        wStrLen = MIN(nInputSize, Endpoint);
        MSG_FATAL("OEMRUIM_Conversion_Uimdata_To_WStr 3 wStrLen=%d", wStrLen, 0, 0);
        for(i=0; i<wStrLen; i++)
        {
            if(Inputbuf[i] != 0xFF)
            {
                AppLabel[i]=(AECHAR)Inputbuf[i];
            }
            else
            {
                AppLabel[i]='\0';
                break;
            }
        }
        break;
    }
}

#ifdef FEATURE_OEMOMH 
static int OEMRUIM_Get_Ecc_Code(IRUIM *pMe,byte *pOutBuf,int *pnNum, int step)
{
    int i;
    DBGPRINTF("OEMRUIM_Get_Ecc_Code start");
    if ((!pMe) || (!pOutBuf))
    {
        return EFAILED;
    }
    
    // Check to see if the card is connected.
    if (!IRUIM_IsCardConnected (pMe))
    {
        return EFAILED;
    }
    
    if(!gbECCInited)
    {
        byte pData[RUIM_ECC_NUMBER*RUIM_ECC_BCDSIZE];
        int  nDataSize = RUIM_ECC_NUMBER*RUIM_ECC_BCDSIZE;
        byte *pBuf = pData;
        byte nLen;
        
        if(rex_self() != &ui_tcb)
        {
            return EFAILED;
        }
        
        gbECCInited = TRUE;
        
        gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
        gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
        gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
        gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
        gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
        gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
        gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

        gUimCmd.access_uim.item      = UIM_CDMA_ECC;
        gUimCmd.access_uim.access    = UIM_READ_F;
        gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
        gUimCmd.access_uim.num_bytes = RUIM_ECC_NUMBER*RUIM_ECC_BCDSIZE;
        gUimCmd.access_uim.offset    = 0;
        gUimCmd.access_uim.data_ptr  = pData;

        // From nvruim_access():  Access an EF, do not signal any task, use no
        // signal, no done queue, use a callback, always report status.

        // Send the command to the R-UIM:
        OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

        if (   (gCallBack.rpt_type != UIM_ACCESS_R)
            || (gCallBack.rpt_status != UIM_PASS)
            )
        {
            MSG_FATAL("OEMRUIM_Get_Ecc_Code EFAILED %d %d", gCallBack.rpt_type, gCallBack.rpt_status ,0);
            return EFAILED;
        }
        
        nDataSize = MIN(nDataSize, gUimCmd.access_uim.num_bytes_rsp);
        MSG_FATAL("OEMRUIM_Get_Ecc_Code pnDataSize=%d", nDataSize, 0 ,0);
        gECCNum = 0;
        for(i=0;i<RUIM_ECC_NUMBER;i++)
        {
            if(nDataSize <= 0)
            {
                break;
            }
            MSG_FATAL("OEMRUIM_Get_Ecc_Code 0x%x 0x%x 0x%x", pBuf[0], pBuf[1] ,pBuf[2]);
            nLen = OEMRUIM_bcd_to_ascii_forOMH(RUIM_ECC_MAXSIZE,
                                               (byte *)pBuf,
                                               (byte *)&gECC[gECCNum][0]);
            MSG_FATAL("OEMRUIM_Get_Ecc_Code 0x%x 0x%x 0x%x", gECC[gECCNum][0], gECC[gECCNum][1] ,gECC[gECCNum][2]);
            if(nLen) gECCNum++;
            pBuf += RUIM_ECC_BCDSIZE;
            nDataSize -= RUIM_ECC_BCDSIZE;
        }
    }
    
    for(i=0;i<gECCNum;i++)
    {
        MEMCPY(pOutBuf,&gECC[i][0],RUIM_ECC_MAXSIZE);
        pOutBuf += step;
    }
    
    if(pnNum)
    {
        *pnNum = gECCNum;
    }
    MSG_FATAL("OEMRUIM_Get_Ecc_Code %d End", gECCNum, 0 ,0);
    return SUCCESS;
}

static int OEMRUIM_Get_AppLabels_Code(IRUIM *pMe,int nId, AECHAR *Buf)
{
    int  status = EFAILED;
    DBGPRINTF("OEMRUIM_Get_AppLabels_Code start");
    if ((!pMe) || (!Buf))
    {
        return EFAILED;
    }
    DBGPRINTF("OEMRUIM_Get_AppLabels_Code 1");
    // Check to see if the card is connected.
    if (!IRUIM_IsCardConnected (pMe))
    {
        return EFAILED;
    }
    DBGPRINTF("OEMRUIM_Get_AppLabels_Code 2");
    if(!gbAppLabelsInited)
    {
        byte pData[RUIN_APPLABEL_SIZE*RUIM_APPLABEL_NUM+4];
        int  nDataSize = RUIN_APPLABEL_SIZE*RUIM_APPLABEL_NUM+4;
        byte encoding_ind;
        byte lang_ind;
        byte *pBuf = pData;
        DBGPRINTF("OEMRUIM_Get_AppLabels_Code 3");
        gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
        gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
        gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
        gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
        gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
        gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
        gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

        gUimCmd.access_uim.item      = UIM_CDMA_APP_LABELS;
        gUimCmd.access_uim.access    = UIM_READ_F;
        gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
        gUimCmd.access_uim.num_bytes = nDataSize;
        gUimCmd.access_uim.offset    = 0;
        gUimCmd.access_uim.data_ptr  = pData;
        
        OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

        if (   (gCallBack.rpt_type != UIM_ACCESS_R)
            || (gCallBack.rpt_status != UIM_PASS)
            )
        {
            MSG_FATAL("OEMRUIM_Get_AppLabels_Code EFAILED", 0, 0 ,0);
            return EFAILED;
        }
        
        nDataSize = MIN(nDataSize, gUimCmd.access_uim.num_bytes_rsp);
        MSG_FATAL("OEMRUIM_Get_AppLabels_Code nDataSize=%d", nDataSize, 0 ,0);
        MSG_FATAL("pData length=%d", STRLEN((char*)pData), 0 ,0);
        gbAppLabelsInited = TRUE;
        gwAppLabelsInd = 0;
        if(nDataSize > 4) // 最小要大于Header
        {
            int nIdx = 0;
            encoding_ind = pBuf[0]&0x1F;
            lang_ind = pBuf[1];
            gwAppLabelsInd = (pBuf[2]<<8)|pBuf[3];
            pBuf += 4;
            nDataSize -= 4;
            while(nDataSize > 0 && nIdx<RUIM_APPLABEL_NUM)
            {
                if(gwAppLabelsInd&(0x1<<nIdx))
                {
                    OEMRUIM_Conversion_Uimdata_To_WStr(pBuf,RUIN_APPLABEL_SIZE,encoding_ind,lang_ind,&gAppLabels[nIdx][0],RUIN_APPLABEL_SIZE);
                }
                nDataSize-=RUIN_APPLABEL_SIZE;
                pBuf += RUIN_APPLABEL_SIZE;
                nIdx++;
            }
        }
        
    }
    
    if(gwAppLabelsInd&(0x1<<nId))
    {
        WSTRCPY(Buf,&gAppLabels[nId][0]);
        if(WSTRLEN(Buf) == 0)
        {
           status = EFAILED; 
        }
        else
        {
            status = SUCCESS;
        }
    }
    
    MSG_FATAL("OEMRUIM_Get_AppLabels_Code len=%d End", WSTRLEN(Buf), 0 ,0);
    return status;
}

static byte OEMRUIM_bcd_to_ascii_forOMH(byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */
                                 byte *to_ptr   /* Convert to */)
{
    byte lower_nibble, upper_nibble;
    byte i = 0;
    
    while (i < num_digi )
    {
        if(*from_ptr==0xFF)
        {
            *to_ptr = 0;
            break;
        }
        else
        {
            lower_nibble = *from_ptr & 0x0F;
            if(lower_nibble == 0x0F)
            {
                *to_ptr = 0;
                break;
            }
            switch (lower_nibble)
            {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x08:
                case 0x09:
                    /* Digits 0 - 9 */
                    *to_ptr = lower_nibble + '0';
                    break;
                    
                default:
                    /* Impossible to come here */
                    break;
            }
            i++;
            to_ptr++;
            upper_nibble = (*from_ptr & 0xF0) >> 4;
            if(upper_nibble == 0x0F)
            {
                *to_ptr = 0;
                break;
            }
            switch (upper_nibble)
            {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x08:
                case 0x09:
                    /* Digits 0 - 9 */
                    *to_ptr = upper_nibble + '0';
                    break;
                default:
                    /* Impossible to come here */
                    break;
            } /* switch upper_nibble */
            i++;
            to_ptr++;
            from_ptr++;
        }
    } /* for */

    return i;

}

static boolean OEMRUIM_WriteModel(IRUIM *pMe, byte *Buf)
{
    int pnDataSize = 126;
    int status = EFAILED;
    DBGPRINTF("OEMRUIM_WriteModel start");
    DBGPRINTF("Buf =%s", Buf);
    if ((!pMe) || (!Buf))
    {
        return EFAILED;
    }
    // Check to see if the card is connected.
    if (!IRUIM_IsCardConnected (pMe))
    {
        return EFAILED;
    }
    gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
    gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
    gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
    gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
    gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
    gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
    gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

    gUimCmd.access_uim.item      = UIM_CDMA_MODEL;
    gUimCmd.access_uim.access    = UIM_WRITE_F;
    gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    gUimCmd.access_uim.num_bytes = 126;
    gUimCmd.access_uim.offset    = 0;
    gUimCmd.access_uim.data_ptr  = Buf;

    // From nvruim_access():  Access an EF, do not signal any task, use no
    // signal, no done queue, use a callback, always report status.

    // Send the command to the R-UIM:
    OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

    if (   (gCallBack.rpt_type != UIM_ACCESS_R)
        || (gCallBack.rpt_status != UIM_PASS)
        )
    {
        MSG_FATAL("OEMRUIM_WriteModel EFAILED", 0, 0 ,0);
        status = EFAILED;
        return EFAILED;
    }
    pnDataSize = MIN(pnDataSize, gUimCmd.access_uim.num_bytes_rsp);
    MSG_FATAL("OEMRUIM_WriteModel pnDataSize=%d", pnDataSize, 0 ,0);
    
    status = SUCCESS;
    MSG_FATAL("OEMRUIM_WriteModel End", 0, 0 ,0);
    return status;
}

#endif

static int OEMRUIM_Get_Feature_Code(IRUIM *pMe,byte *Buf,int Lable)
{
    byte pData[UIM_CDMA_FEATURE_CODE_SIZE+2];
    int pnDataSize = UIM_CDMA_FEATURE_CODE_SIZE;
    MSG_FATAL("OEMRUIM_Get_Feature_Code Start %d",Lable,0,0);
    if ((!pMe) || (!Buf))
    {
        MSG_FATAL("OEMRUIM_Get_Feature_Code %x %x",pMe,Buf,0);
        return EFAILED;
    }

    if(!gbFeatureCodeInited)
    {
        // Check to see if the card is connected.
        if (!IRUIM_IsCardConnected (pMe))
        {
            MSG_FATAL("OEMRUIM_Get_Feature_Code NO CARD",0,0,0);
            return EFAILED;
        }
        gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
        gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
        gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
        gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
        gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
        gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
        gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

        gUimCmd.access_uim.item      = UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE;
        gUimCmd.access_uim.access    = UIM_READ_F;
        gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
        gUimCmd.access_uim.num_bytes = UIM_CDMA_FEATURE_CODE_SIZE;
        gUimCmd.access_uim.offset    = 0;
        gUimCmd.access_uim.data_ptr  = pData;

        // From nvruim_access():  Access an EF, do not signal any task, use no
        // signal, no done queue, use a callback, always report status.

        // Send the command to the R-UIM:
        OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

        if (   (gCallBack.rpt_type != UIM_ACCESS_R)
            || (gCallBack.rpt_status != UIM_PASS)
            )
        {
            MSG_FATAL("OEMRUIM_Get_Feature_Code %d %d",gCallBack.rpt_status,gCallBack.rpt_type,0);
            return EFAILED;
        }

        pnDataSize = MIN(pnDataSize, gUimCmd.access_uim.num_bytes_rsp);
        MEMCPY(gFeatureCode,pData,pnDataSize);
        gbFeatureCodeInited = TRUE;
    }
    else
    {
        pnDataSize = UIM_CDMA_FEATURE_CODE_SIZE;
        MEMCPY(pData,gFeatureCode,pnDataSize);
    }
    
    MSG_FATAL("OEMRUIM_Get_Feature_Code 0x%x 0x%x",pData[Lable],pData[Lable+1],0);
    if(UIM_CDMA_FEATURE_CODE_NUM_DIGI==OEMRUIM_bcd_to_ascii(UIM_CDMA_FEATURE_CODE_NUM_DIGI,
                                    (byte *)&pData[Lable],
                                    (byte *)Buf))
    {
        Buf[UIM_CDMA_FEATURE_CODE_NUM_DIGI]='\0';
    }
    
    MSG_FATAL("OEMRUIM_Get_Feature_Code %d %d END",pnDataSize,Lable,0);
    return SUCCESS;
}

static byte OEMRUIM_bcd_to_ascii(byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */
                                 byte *to_ptr   /* Convert to */)
{
    byte lower_nibble, upper_nibble;
    byte i = 0;
    byte  k=num_digi;
    while (i < num_digi )
    {
        if(*from_ptr==0xFF)
        {
            *to_ptr = '*';
            to_ptr++;
            from_ptr++;
            //num_digi=num_digi-2;
            i=i+2;
            k--;
        }
        else
        {
            upper_nibble = (*from_ptr & 0xF0) >> 4;
            switch (upper_nibble)
            {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x08:
                case 0x09:
                    /* Digits 0 - 9 */
                    *to_ptr = upper_nibble + '0';
                    break;
                case 0x0F:
                    *to_ptr = '*';
                    break;
                default:
                    /* Impossible to come here */
                    break;
            } /* switch upper_nibble */
            i++;
            to_ptr++;
            lower_nibble = *from_ptr & 0x0F;
            switch (lower_nibble)
            {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x08:
                case 0x09:
                    /* Digits 0 - 9 */
                    *to_ptr = lower_nibble + '0';
                    break;


                case 0x0F:
                    *to_ptr = '*';
                    break;

                default:
                    /* Impossible to come here */
                    break;
            }
            i++;
            to_ptr++;
            from_ptr++;
        }
    } /* for */
    to_ptr[k] = '\0';
    return i;
}

static int OEMRUIM_Read_Svc_P_Name(IRUIM *pMe , AECHAR *svc_p_name)
{
    int    pnDataSize = UIM_CDMA_HOME_SERVICE_SIZE;
    byte pData[UIM_CDMA_HOME_SERVICE_SIZE+2];
    int    status = EFAILED;
    
    MSG_FATAL("OEMRUIM_Read_Svc_P_Name",0,0,0);   
    // Check to see if the card is connected.
    if (!IRUIM_IsCardConnected (pMe))
    {
        return EFAILED;
    }
    gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
    gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
    gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
    gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
    gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
    gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
    gUimCmd.access_uim.hdr.rpt_function       = OEMRUIM_report;

    gUimCmd.access_uim.item      = UIM_CDMA_HOME_SVC_PVDR_NAME;
    gUimCmd.access_uim.access    = UIM_READ_F;
    gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    gUimCmd.access_uim.num_bytes = UIM_CDMA_HOME_SERVICE_SIZE;
    gUimCmd.access_uim.offset    = 0;
    gUimCmd.access_uim.data_ptr  = pData;

    // From nvruim_access():  Access an EF, do not signal any task, use no
    // signal, no done queue, use a callback, always report status.

    // Send the command to the R-UIM:
    OEMRUIM_send_uim_cmd_and_wait (&gUimCmd);

    if (   (gCallBack.rpt_type != UIM_ACCESS_R) || (gCallBack.rpt_status != UIM_PASS) )
    {
        status = EFAILED;
    }
    else      
    {
        status = SUCCESS;
        OEMRUIM_Conversion_Uimdata_To_Spn(pData,svc_p_name,pnDataSize);//wszBuf中存放从UIM卡中读出的数?
        DBGPRINTF("svc_p_name = %S, Length=%d",svc_p_name, WSTRLEN(svc_p_name));
    }    
    return status;
}

//Endpoint must is 35 ,when  parsing the home service name
static void OEMRUIM_Conversion_Uimdata_To_Spn(byte *Inputbuf,AECHAR *svc_p_name,int Endpoint)
{
    if((Inputbuf[0]&0x01==0x01)&&(Inputbuf[0]!=0xFF))//TOBUF中的第一位如果是1,则显示运营商名称
    {
        MSG_FATAL("number Inputbuf[1]= %x %x",Inputbuf[1],Inputbuf[2],0);
        OEMRUIM_Conversion_Uimdata_To_WStr((Inputbuf+3),UIM_CDMA_HOME_SERVICE_SIZE,Inputbuf[1]&0x1F,Inputbuf[2],svc_p_name,Endpoint);
    }
    else
    {
        MSG_FATAL("Inputbuf == NULL", 0, 0, 0);
        //Inputbuf为空,表示此卡内不存储运营商名字.所以需要另外处理
        svc_p_name[0]='\0';
        svc_p_name[1]='\0';
    }
}
#endif    // FEATURE_IRUIM
