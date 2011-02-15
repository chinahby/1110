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
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

#ifndef UI_RUIM_SIG
#error  You must define UI_RUIM_SIG to be a unique signal in UI task.
#endif

#define BAD_PIN_CHAR 0xff
#define UIM_CDMA_FEATURE_CODE_SIZE 50
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
static void OEMRUIM_ExchangeByte(byte *Inputbuf,int size);
static void OEMRUIM_Conversion_Uimdata_To_Spn(byte *Inputbuf,AECHAR *svc_p_name,int Endpoint);

#ifdef FEATURE_OEMOMH 
static int OEMRUIM_Get_Ecc_Code(IRUIM *pMe,byte *Buf);
static byte OEMRUIM_bcd_to_ascii_forOMH(byte num_digi, byte *from_ptr, byte *to_ptr);
static boolean OEMRUIM_WriteModel(IRUIM *pMe, byte *Buf);
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
  OEMRUIM_WriteModel
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

#ifdef FEATURE_OEMOMH 
static int OEMRUIM_Get_Ecc_Code(IRUIM *pMe,byte *Buf)
{
    byte pData[15+2];
    int pnDataSize = 15;
    int status = EFAILED;
    DBGPRINTF("OEMRUIM_Get_Ecc_Code start");
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

    gUimCmd.access_uim.item      = UIM_CDMA_ECC;
    gUimCmd.access_uim.access    = UIM_READ_F;
    gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    gUimCmd.access_uim.num_bytes = 15;
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
        MSG_FATAL("OEMRUIM_Get_Ecc_Code EFAILED", 0, 0 ,0);
        status = EFAILED;
        return EFAILED;
    }

    //if (pData == NULL)
    //{
    //    pnDataSize = gUimCmd.access_uim.num_bytes_rsp;
    //}
    //else
    //{
    //int i=0;
    pnDataSize = MIN(pnDataSize, gUimCmd.access_uim.num_bytes_rsp);
    MSG_FATAL("OEMRUIM_Get_Ecc_Code pnDataSize=%d", pnDataSize, 0 ,0);
    
    status = SUCCESS;
    //}

    //if(status == SUCCESS)
    {
        if(30 ==OEMRUIM_bcd_to_ascii_forOMH(30,
                                        (byte *)&pData,
                                        (byte *)Buf))
        {
            Buf[UIM_CDMA_FEATURE_CODE_NUM_DIGI]='\0';
        }
    }
    MSG_FATAL("OEMRUIM_Get_Ecc_Code End", 0, 0 ,0);
    return status;
}

static byte OEMRUIM_bcd_to_ascii_forOMH(byte num_digi, /* Number of dialing digits */
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
            to_ptr++;
            from_ptr++;
            //num_digi=num_digi-2;
            i=i+2;
            k--;
        }
        else
        {
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
            from_ptr++;
        }
    } /* for */
    to_ptr[k] = '\0';

    return i;
}

static boolean OEMRUIM_WriteModel(IRUIM *pMe, byte *Buf)
{
    //byte pData[126+2];
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

    //if (pData == NULL)
    //{
    //    pnDataSize = gUimCmd.access_uim.num_bytes_rsp;
    //}
    //else
    //{
    //int i=0;
    pnDataSize = MIN(pnDataSize, gUimCmd.access_uim.num_bytes_rsp);
    MSG_FATAL("OEMRUIM_WriteModel pnDataSize=%d", pnDataSize, 0 ,0);
    
    status = SUCCESS;
    //}
#if 0
    //if(status == SUCCESS)
    {
        if(30 ==OEMRUIM_bcd_to_ascii_forOMH(30,
                                        (byte *)&pData,
                                        (byte *)Buf))
        {
            Buf[UIM_CDMA_FEATURE_CODE_NUM_DIGI]='\0';
        }
    }
#endif
    MSG_FATAL("OEMRUIM_WriteModel End", 0, 0 ,0);
    return status;
}

#endif

static int OEMRUIM_Get_Feature_Code(IRUIM *pMe,byte *Buf,int  Lable)
{
    byte pData[UIM_CDMA_FEATURE_CODE_SIZE+2];
    int pnDataSize = UIM_CDMA_FEATURE_CODE_SIZE;
    int status = EFAILED;
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
        status = EFAILED;
        return EFAILED;
    }

    //if (pData == NULL)
    //{
    //    pnDataSize = gUimCmd.access_uim.num_bytes_rsp;
    //}
    //else
    //{
    //int i=0;
    pnDataSize = MIN(pnDataSize, gUimCmd.access_uim.num_bytes_rsp);
    status = SUCCESS;
    //}

    //if(status == SUCCESS)
    {
        /*int i = 0;
        //below one of line be only for test
        for(i=0;i<pnDataSize;i++)
        {
            ERR("OEMRUIM_G.I.:: featurecodedata[%03d]:0x%02X",i,pData[i],0);
        }*/
        if(UIM_CDMA_FEATURE_CODE_NUM_DIGI==OEMRUIM_bcd_to_ascii(UIM_CDMA_FEATURE_CODE_NUM_DIGI,
                                        (byte *)&pData[Lable],
                                        (byte *)Buf))
        {
            Buf[UIM_CDMA_FEATURE_CODE_NUM_DIGI]='\0';
        }
    }
    return status;
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
    ERR("OEMRUIM_Read_Svc_P_Name",0,0,0);
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
        /*int i;
        for(i=0;i<pnDataSize;i++)
        {
            ERR("OEMRUIM_G.I.:: featurecodedata[%03d]:0x%02X",i,pData[i],0);
        }*/
        status = SUCCESS;
        OEMRUIM_Conversion_Uimdata_To_Spn(pData,svc_p_name,pnDataSize);//wszBuf中存放从UIM卡中读出的数?
    }    
    return status;
}

//对于UNICODE 编码的字符串交换高低字节
static void OEMRUIM_ExchangeByte(byte *Inputbuf,int size)
{
    int h=0;
    byte tempbuf;

    while(h<size)
    {
        tempbuf=Inputbuf[h];
        Inputbuf[h]=Inputbuf[h+1];
        Inputbuf[h+1]=tempbuf;
        h=h+2;
    }
    return;
}

//Endpoint must is 35 ,when  parsing the home service name
static void OEMRUIM_Conversion_Uimdata_To_Spn(byte *Inputbuf,AECHAR *svc_p_name,int Endpoint)
{
    int i=0;
    int k;
    int m =0;
    byte tempbuf[(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR)]={(byte)'\0'};
    if((Inputbuf[0]&0x01==0x01)&&(Inputbuf[0]!=0xFF))//TOBUF中的第一位如果是1,则显示运营商名称
    {
            for( i=3;i<Endpoint;i++)//I-3+1 表示TOBUF中的有效字符数目
            {
                if(Inputbuf[i]==0xFF)
                {
                    //当FF后面或前面不是跟的FE组成一个大小端判断串时，就表示这是一个无效字符
                    if(i > 3)
                    {
                        if(!((Inputbuf[i-1]==0xFE) || (Inputbuf[i+1]==0xFE)))
                        {
                            break;
                        }
                    }
                    else if(Inputbuf[i+1] != 0xFE)
                    {
                        break;
                    }                    
                }
            }
            ERR("number len = %d Inputbuf[1]= %x",i,Inputbuf[1],0);
            switch(Inputbuf[1]&0x1F)
            {
                case AEERUIM_LANG_ENCODING_7BIT:                  //acsii编码
                    for(k=0;k<i-3;k++)
                    {
                        tempbuf[k]=Inputbuf[k+3];
                    }
                    tempbuf[k]='\0';
                    //DBGPRINTF("tempbuf0 = %s",tempbuf);
                    STRTOWSTR((char *)tempbuf, svc_p_name,Endpoint*sizeof(AECHAR));
                    //OEMRUIM_AnsiiToUnicodeString((AECHAR *)Inputbuf,(AECHAR *)tempbuf);
                    break;

                case  AEERUIM_LANG_ENCODING_UNICODE:                   //UNICODE编码
                    if(((Inputbuf[3]&0xFF == 0xFE) || (Inputbuf[3]&0xFF == 0xFF)) &&
                        ((Inputbuf[4]&0xFF == 0xFE) || (Inputbuf[4]&0xFF == 0xFF))
                      )
                    {
                        //为TRUE则表示是BOM，要去掉前面5个标志位
                        for(k=0;k<i-5;k++)
                        {
                            tempbuf[k]=Inputbuf[k+5];
                            //ERR("tempbuf[%d] = %x",k,tempbuf[k],0);
                        }        
                        if((Inputbuf[3]&0xFF == 0xFE) && (Inputbuf[4]&0xFF == 0xFF))//(big endian 00-54 with BOM)
                        {
                            OEMRUIM_ExchangeByte(tempbuf,k);
                        }
                    }
                    else
                    {
                        for(k=0;k<i-3;k++)
                        {
                            tempbuf[k]=Inputbuf[k+3];
                            //ERR("tempbuf[%d] = %x",k,tempbuf[k],0);
                        }
                        OEMRUIM_ExchangeByte(tempbuf,k);
                    }
                    
                    //DBGPRINTF("tempbuf2 = %S",tempbuf);
                    tempbuf[k++]='\0';
                    tempbuf[k]='\0';
                    WSTRLCPY(svc_p_name,(AECHAR*)tempbuf,UIM_CDMA_HOME_SERVICE_SIZE);
                    break;

                case AEERUIM_LANG_ENCODING_LATINHEBREW:
                case AEERUIM_LANG_ENCODING_LATIN:
                case AEERUIM_LANG_ENCODING_OCTET:
                    //DBGPRINTF("tempbuf5 = %s",tempbuf);
                    for(k=0;k<i-3;k++)
                    {
                        tempbuf[m]= Inputbuf[k+3];
                        tempbuf[m+1] =0x00;// '0' -30;
                        m = m+2;
                    }
                    tempbuf[m]='\0';
                    tempbuf[m+1]='\0';
                    #if 0
                    while(1)
                    {
                        if(tempbuf[h++]!='\0')
                            ERR("OEMRUIM_G.I.:: temp data[%03d]:0x%02X::M=%d",h,tempbuf[h],m);
                        else
                            break;
                    }
                    #endif
                    MEMCPY(svc_p_name,tempbuf,UIM_CDMA_HOME_SERVICE_SIZE*sizeof(AECHAR));
                    break;
                default:
                    //ERR("tempbuf6 ",0,0,0);
                    break;

            }
    }
    else
    {
        //Inputbuf为空,表示此卡内不存储运营商名字.所以需要另外处理
        svc_p_name[0]='\0';
        svc_p_name[1]='\0';
    }
    return;
}

#endif    // FEATURE_IRUIM
