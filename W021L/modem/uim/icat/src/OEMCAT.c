/*=============================================================================
FILE:  OEMCAT.c

SERVICES:  OEM functions for supporting BREW CATAPP Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ICAT interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/icat/rel/07H1_2/src/OEMCAT.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/05   tml     Linuxify fixes
05/11/05   sst     Lint fixes
09/03/04   tml     Fixed compilation issue for BREW
08/20/04   tml     Fixed compilation issue for BREW 3.0
08/18/04   tml     Did not update static global buffer for envelope response
                   and end proactive command
08/16/04   tml     Added User Activity notification
08/05/04   tml     Fixed memory free during power down
07/21/04   tml     Fixed memory allocation for object creation
07/15/04   tml     Added supports for provide local info - language
05/26/04   tml     Added supports for language notification and send DTMF
10/09/03   tml     Added handling for mode change cmd
05/29/03   AP      Added null pointer check for *req_rsp.
05/28/03   AP      Added null pointer check after retrieving shell pointer.
04/29/03   tml     Fixed potential memcopy and mem free issues
03/06/03   tml     Update enum names
02/26/03   tml     Changed gstk_evt_cb_funct_type to replace
                   gstk_evt_cb_funct_type*
02/12/03   tml     Free icon type
02/11/03   tml     Added ICAT_PopulateCardCmdBuffer and
                   ICAT_ReleaseCardCmdBuffer.  Changed ISHELL_PostEvent to
                   ISHELL_PostEventEx
02/05/03   tml     Removed CoreApp related codes and formatted
10/24/02   aaa     Initial Version
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "intconv.h"

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "OEMHeap.h"
#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers.

#include "AEEStdLib.h"
#include "AEECAT.h"      // Header file for CAT interface

#include "OEMFeatures.h"

#ifdef FEATURE_APP_CATAPP
#include "catapp.bid"
#include "catapp.h"
#endif

/* The following structure is for the object which is allocated. */

struct ICAT
{
   AEEVTBL(ICAT)             *pvt;
   IShell                    *m_pIShell;
   AEECLSID                  m_cls;
   uint32                    m_uRefs;           /* Number of references to this obj */
   AEECallback               *m_pcbExit;
   boolean                   m_user_act_notify;
};


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 OEMCAT_AddRef(ICAT *pMe);

static uint32 OEMCAT_Release(ICAT *pMe);

static gstk_client_id_type cl_id;

static int OEMCAT_QueryInterface(
              ICAT     *pMe,
              AEECLSID id,
              void     **ppNew);

static int  OEMCAT_Init(
              ICAT              *po,
              uint32            ud,
              gstk_client_type  client_type,
              uint32            evt_reg_bitmask);

static int OEMCAT_SendEnvelopeMenuSelCommand(
              ICAT                  *po,
              uint32        ud,
              byte                  ic,
              boolean               hr);

static int OEMCAT_SendTerminalResponse(
              ICAT                                             *po,
              uint32                             ud,
              uint32                                           cdri,
              uint32                                           cmd_num,
              gstk_cmd_enum_type                         term_resp_e,
              gstk_general_result_enum_type        result,
              gstk_additional_info_type              *additional_result,
              gstk_terminal_rsp_extra_param_type *other);

static int OEMCAT_SendEnvelopEvtDownloadCommand(
              ICAT                            *po,
              uint32                ud,
              gstk_evt_dl_ind_type  *evt_list);

static int OEMCAT_UpdateProfileDownloadValue(
              ICAT                                      *po,
                            uint32                        ud,
              byte                                      num_updates,
              gstk_profile_dl_support_type  *value_list);

static int OEMCAT_PopulateCardCmdBuffer(
              ICAT                     *po,
              gstk_cmd_enum_type       command_type,
              gstk_cmd_from_card_type  *old_buf,
              gstk_cmd_from_card_type  *new_buf);

static int OEMCAT_ReleaseCardCmdBuffer(
              ICAT                    *po,
              gstk_cmd_from_card_type * message_p );

static int OEMCAT_UserActivity(ICAT *po);

static const AEEVTBL(ICAT) gOEMCATFuncs = {
   OEMCAT_AddRef,
   OEMCAT_Release,
   OEMCAT_QueryInterface,
   OEMCAT_Init,
   OEMCAT_SendEnvelopeMenuSelCommand,
   OEMCAT_SendTerminalResponse,
   OEMCAT_SendEnvelopEvtDownloadCommand,
   OEMCAT_UpdateProfileDownloadValue,
   OEMCAT_PopulateCardCmdBuffer,
   OEMCAT_ReleaseCardCmdBuffer,
   OEMCAT_UserActivity
};

/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct ICAT *ICATobj = NULL;


static gstk_cmd_from_card_type gstk_cmd_from_card_buf; /* command from card */


/* The following structure is used to notify clients of CAT events */

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static void ICat_AEEExitNotify(ICAT * po)
{
  if (ICATobj != NULL) {
    if (ICATobj->m_pcbExit) {
      sys_free(ICATobj->m_pcbExit);
      ICATobj->m_pcbExit = NULL;
    }
    (void)aee_forcereleaseobj((void **)&ICATobj);
    ICATobj = NULL;
  }
} /*lint !e715 */


/*===========================================================================

FUNCTION: OEMCAT_UserActivity

DESCRIPTION
  Send down User Activity event to the Card upon receiving key events

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
static int OEMCAT_UserActivity(ICAT *po)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    user_act_ind;
  if (ICATobj) {
    if(ICATobj->m_user_act_notify) 
    {
      /* Send envelope to GSTK */
      user_act_ind.evt_type =  GSTK_USER_ACTIVITY_EVT_IND;

      client_status = gstk_send_envelope_evt_download_command(
                              cl_id,
                              0,
                              &user_act_ind);
      ICATobj->m_user_act_notify = FALSE;
      if (client_status != GSTK_SUCCESS) {
        return EFAILED;
      }
    }
  }
  return SUCCESS;
} /*lint !e715 */

/*===========================================================================

FUNCTION: OEMCAT_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
int OEMCAT_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_CAT) {
      return EUNSUPPORTED;
   }

   if (ICATobj == NULL)
   {
     // Allocate the object.
     if ((ICATobj = (ICAT*)(void*)AEE_OEM_NEWCLASSEX((AEEVTBL(IBase) *)(void*)&gOEMCATFuncs,sizeof(ICAT),TRUE)) == NULL) {
        return ENOMEMORY;
     }

     /* register for AEE Exit event */
     ICATobj->m_pcbExit = (AEECallback*)sys_malloc(sizeof(AEECallback));
     if (!ICATobj->m_pcbExit)
     {
       *ppif = NULL;
       FREE(ICATobj);
       ICATobj = NULL;
       return ENOMEMORY;
     }

     CALLBACK_Init(ICATobj->m_pcbExit, (PFNNOTIFY)ICat_AEEExitNotify, 
                     ICATobj);
#ifndef FEATURE_BREW_3_0
     AEE_RegisterSystemCallback(ICATobj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                AEE_RESUME_CB_SYS,0);
#else
     ISHELL_OnExit(pIShell, ICATobj->m_pcbExit);
#endif
     ICATobj->pvt = (ICATVtbl *)&gOEMCATFuncs;
     ICATobj->m_pIShell = pIShell;
     ICATobj->m_cls = cls;
     ICATobj->m_uRefs = 1;

     *ppif = ICATobj;

   }
   else
   {
     (void)OEMCAT_AddRef(ICATobj);
     *ppif = ICATobj;
   }

   return AEE_SUCCESS;

}
/*===========================================================================

FUNCTION: OEMCAT_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMCAT object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static uint32 OEMCAT_AddRef(ICAT *pMe)
{

   if (pMe) {
      return (++pMe->m_uRefs);
   }
   return 0;
}

/*===========================================================================

FUNCTION: OEMCAT_Release

DESCRIPTION
  Decreases the number of references to the ICAT object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static uint32 OEMCAT_Release(ICAT *pMe)
{
   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   if (ICATobj->m_pcbExit) {
     CALLBACK_Cancel(ICATobj->m_pcbExit);
     sys_free(ICATobj->m_pcbExit);
     ICATobj->m_pcbExit = NULL;
   }
   // Ref count is zero.  Release memory
   OEM_Free(pMe);
   ICATobj = NULL;
   return (0);
}

/*===========================================================================

FUNCTION: OEMCAT_QueryInterface

DESCRIPTION
  Allows applications to query if ICAT supports other interfaces and then
  returns a pointer to that new interface supported via ICAT.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/
static int OEMCAT_QueryInterface(ICAT *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   ret_val = ECLASSNOTSUPPORT;
   return ret_val;
} /*lint !e715 */


/*===========================================================================

FUNCTION: OEMCAT_PopulateCardCmdBuffer

DESCRIPTION
  Correctly allocate memory for different gstk_cmd_from_card_type structure
  and copy the information accordingly.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/
static int OEMCAT_PopulateCardCmdBuffer(ICAT                    *po,
                                        gstk_cmd_enum_type      command_type,
                                        gstk_cmd_from_card_type *old_buf,
                                        gstk_cmd_from_card_type *new_buf)
{
  int i;

  /* check memory of buffer pointers */
  if((!po) || (old_buf == NULL) || (new_buf == NULL)) {
    return EBADPARM;
  }

  /* copy the cmd_from_card_type structure first */
  (void)MEMCPY(new_buf, old_buf, sizeof(gstk_cmd_from_card_type));

  /* copy individual element within various union structure in the cmd_from_card_type
  structure if neccessary */
  switch(command_type)
  {
  case GSTK_DISPLAY_TEXT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.display_text_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.display_text_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.display_text_pro_cmd_req.text_string.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.display_text_pro_cmd_req.text_string.length);
       if(new_buf->cmd.display_text_pro_cmd_req.text_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.display_text_pro_cmd_req.text_string.text,
           old_buf->cmd.display_text_pro_cmd_req.text_string.text,
           old_buf->cmd.display_text_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.display_text_pro_cmd_req.icon.present) &&
        (old_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.display_text_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.display_text_pro_cmd_req.icon.size);
       if(new_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.display_text_pro_cmd_req.icon.data,
           old_buf->cmd.display_text_pro_cmd_req.icon.data,
           old_buf->cmd.display_text_pro_cmd_req.icon.size);
       }
     }

     break;
  case GSTK_GET_INKEY_REQ:
       /* allocate and copy text */
     if((old_buf->cmd.get_inkey_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.get_inkey_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.get_inkey_pro_cmd_req.text_string.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.get_inkey_pro_cmd_req.text_string.length);
       if(new_buf->cmd.get_inkey_pro_cmd_req.text_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.get_inkey_pro_cmd_req.text_string.text,
           old_buf->cmd.get_inkey_pro_cmd_req.text_string.text,
           old_buf->cmd.get_inkey_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.get_inkey_pro_cmd_req.icon.present) &&
        (old_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.get_inkey_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.get_inkey_pro_cmd_req.icon.size);
       if(new_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.get_inkey_pro_cmd_req.icon.data,
           old_buf->cmd.get_inkey_pro_cmd_req.icon.data,
           old_buf->cmd.get_inkey_pro_cmd_req.icon.size);
       }
     }
     break;
  case GSTK_GET_INPUT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.get_input_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.get_input_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.text_string.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.text_string.length);
       if(new_buf->cmd.get_input_pro_cmd_req.text_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.get_input_pro_cmd_req.text_string.text,
           old_buf->cmd.get_input_pro_cmd_req.text_string.text,
           old_buf->cmd.get_input_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.get_input_pro_cmd_req.icon.present) &&
        (old_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.icon.size);
       if(new_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.get_input_pro_cmd_req.icon.data,
           old_buf->cmd.get_input_pro_cmd_req.icon.data,
           old_buf->cmd.get_input_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy default text */
     if((old_buf->cmd.get_input_pro_cmd_req.default_text.length > 0) &&
        (old_buf->cmd.get_input_pro_cmd_req.default_text.text != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.default_text.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.default_text.length);
       if(new_buf->cmd.get_input_pro_cmd_req.default_text.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.get_input_pro_cmd_req.default_text.text,
           old_buf->cmd.get_input_pro_cmd_req.default_text.text,
           old_buf->cmd.get_input_pro_cmd_req.default_text.length);
       }
     }
     break;
  case GSTK_PLAY_TONE_REQ:
     /* allocate and copy alpha */
     if((old_buf->cmd.play_tone_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.play_tone_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.play_tone_pro_cmd_req.alpha.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.play_tone_pro_cmd_req.alpha.length);
       if(new_buf->cmd.play_tone_pro_cmd_req.alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.play_tone_pro_cmd_req.alpha.text,
           old_buf->cmd.play_tone_pro_cmd_req.alpha.text,
           old_buf->cmd.play_tone_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.play_tone_pro_cmd_req.icon.present) &&
        (old_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.play_tone_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.play_tone_pro_cmd_req.icon.size);
       if(new_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.play_tone_pro_cmd_req.icon.data,
           old_buf->cmd.play_tone_pro_cmd_req.icon.data,
           old_buf->cmd.play_tone_pro_cmd_req.icon.size);
       }
     }
     break;
  case GSTK_SELECT_ITEM_REQ:
    /* allocate and copy items */
     for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) {
       if((old_buf->cmd.select_item_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)) {
         new_buf->cmd.select_item_pro_cmd_req.items[i].item_text =
           (uint8*)MALLOC(sizeof(uint8) *
                    old_buf->cmd.select_item_pro_cmd_req.items[i].length);
         if(new_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)
         {
           (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.items[i].item_text,
             old_buf->cmd.select_item_pro_cmd_req.items[i].item_text,
             old_buf->cmd.select_item_pro_cmd_req.items[i].length);
         }
       }
     }
     /* allocate and copy title */
     if((old_buf->cmd.select_item_pro_cmd_req.title.length > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.title.text != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.title.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.select_item_pro_cmd_req.title.length);
       if(new_buf->cmd.select_item_pro_cmd_req.title.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.title.text,
           old_buf->cmd.select_item_pro_cmd_req.title.text,
           old_buf->cmd.select_item_pro_cmd_req.title.length);
       }
     }
     /* allocate and copy next_action_indicator */
     if((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list =
         (uint8*)MALLOC(sizeof(uint8) *
                old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
       if(new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)
       {
         (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.select_item_pro_cmd_req.icon.present) &&
        (old_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.select_item_pro_cmd_req.icon.size);
       if(new_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.icon.data,
           old_buf->cmd.select_item_pro_cmd_req.icon.data,
           old_buf->cmd.select_item_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy icon id list */
     if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
     {
       for(i = 0; i< old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) {
         new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] =
           (gstk_icon_type*)MALLOC(sizeof(gstk_icon_type));
         if((new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL) &&
            (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)) {

           (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                 sizeof(gstk_icon_type));
           if((old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data =
               (uint8*)MALLOC(sizeof(uint8) *
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size);
             if(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)
             {
               (void)MEMCPY(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size);
             }
           }
         }
       }
     }
     break;
  case GSTK_SEND_SMS_REQ:
     /* allocate and copy sms tpdu */
     if((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length > 0) &&
        (old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
       if(new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
           old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
           old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
       }
     }
     /* allocate and copy alpha */
     if((old_buf->cmd.send_sms_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_sms_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.alpha.text =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_sms_pro_cmd_req.alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_sms_pro_cmd_req.alpha.text,
           old_buf->cmd.send_sms_pro_cmd_req.alpha.text,
           old_buf->cmd.send_sms_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_sms_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_sms_pro_cmd_req.icon.data,
           old_buf->cmd.send_sms_pro_cmd_req.icon.data,
           old_buf->cmd.send_sms_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SEND_SS_REQ:
     /* allocate and copy alpha */
     if((old_buf->cmd.send_ss_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_ss_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_ss_pro_cmd_req.alpha.text =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_ss_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_ss_pro_cmd_req.alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_ss_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ss_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ss_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_ss_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_ss_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_ss_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_ss_pro_cmd_req.icon.data,
           old_buf->cmd.send_ss_pro_cmd_req.icon.data,
           old_buf->cmd.send_ss_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SEND_USSD_REQ:
     /* allocate and copy ussd string */
     if((old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length);
       if(new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text,
           old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text,
           old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length);
       }
     }
     /* allocate and copy alpha */
     if((old_buf->cmd.send_ussd_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.alpha.text =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_ussd_pro_cmd_req.alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_ussd_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ussd_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ussd_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_ussd_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_ussd_pro_cmd_req.icon.data,
           old_buf->cmd.send_ussd_pro_cmd_req.icon.data,
           old_buf->cmd.send_ussd_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SETUP_CALL_REQ:
     /* allocate and copy user_cnf_alpha */
     if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length > 0) &&
        (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
       if(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
       }
     }
     /* allocate and copy user_cnf_alpha */
     if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length > 0) &&
        (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text =
         (uint8*)MALLOC(sizeof(uint8) *
              old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
       if(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
       }
     }
     /* allocate and copy user_cnf_icon */
     if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
        (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
       if(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
       }
     }
     /* allocate and copy setup_call_display_icon */
     if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
        (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
       if(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
       }
     }
     break;
   case GSTK_SETUP_MENU_REQ:
         /* allocate and copy items */
     for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) {
       if((old_buf->cmd.setup_menu_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)) {
         new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text =
           (uint8*)MALLOC(sizeof(uint8) *
                    old_buf->cmd.setup_menu_pro_cmd_req.items[i].length);
         if(new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)
         {
           (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text,
             old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text,
             old_buf->cmd.setup_menu_pro_cmd_req.items[i].length);
         }
       }
     }
     /* allocate and copy title */
     if((old_buf->cmd.setup_menu_pro_cmd_req.title.length > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.title.text != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.title.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.setup_menu_pro_cmd_req.title.length);
       if(new_buf->cmd.setup_menu_pro_cmd_req.title.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.title.text,
           old_buf->cmd.setup_menu_pro_cmd_req.title.text,
           old_buf->cmd.setup_menu_pro_cmd_req.title.length);
       }
     }
     /* allocate and copy next_action_indicator */
     if((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list =
         (uint8*)MALLOC(sizeof(uint8) *
                old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
       if(new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.setup_menu_pro_cmd_req.icon.present) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.setup_menu_pro_cmd_req.icon.size);
       if(new_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.icon.data,
           old_buf->cmd.setup_menu_pro_cmd_req.icon.data,
           old_buf->cmd.setup_menu_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy icon id list */
     if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
     {
       for(i = 0; i< old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) {
         new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] =
           (gstk_icon_type*)MALLOC(sizeof(gstk_icon_type));
         if((new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) &&
            (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL)) {

           (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                 sizeof(gstk_icon_type));
           if((old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data =
               (uint8*)MALLOC(sizeof(uint8) *
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size);
             if(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)
             {
               (void)MEMCPY(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size);
             }
           }
         }
       }
     }
     break;
   case GSTK_MORE_TIME_REQ:
     break;
   case GSTK_SETUP_IDLE_TEXT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length);
       if(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
        (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) *
            old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size);
       if(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_END_PROACTIVE_CMD_REQ:
   case GSTK_PROVIDE_LANG_INFO_REQ:
    break;

   case GSTK_LAUNCH_BROWSER_REQ:

     /* allocate and copy url */
     if((old_buf->cmd.launch_browser_pro_cmd_req.url.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.url.url =
         (uint8*)MALLOC(sizeof(uint8) *
             old_buf->cmd.launch_browser_pro_cmd_req.url.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)
       {
         (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.url.url,
           old_buf->cmd.launch_browser_pro_cmd_req.url.url,
           old_buf->cmd.launch_browser_pro_cmd_req.url.length);
       }
     }
     /* allocate and copy bearer list */
     if((old_buf->cmd.launch_browser_pro_cmd_req.bearer.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list =
         (gstk_bearer_code_enum_type*)MALLOC(sizeof(gstk_bearer_code_enum_type) *
                  old_buf->cmd.launch_browser_pro_cmd_req.bearer.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)
       {
         (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
           old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
           old_buf->cmd.launch_browser_pro_cmd_req.bearer.length);
       }
     }
     /* allocate and copy file */
     for (i=0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) {
       if((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length > 0) &&
          (old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)) {
         new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths =
           (uint8*)MALLOC(sizeof(uint8) *
                    old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length);
         if(new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)
         {
           (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths,
             old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths,
             old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length);
         }
       }
     }
     /* allocate and copy confirm alpha */
     if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length);
       }
     }
     /* allocate and copy gateway text string */
     if((old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
           old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
           old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size);
       if(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size);
       }
     }
     break;

   case GSTK_SEND_DTMF_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text,
           old_buf->cmd.send_dtmf_pro_cmd_req.alpha.text,
           old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     
     if((old_buf->cmd.send_dtmf_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.icon.data =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.send_dtmf_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_dtmf_pro_cmd_req.icon.data,
           old_buf->cmd.send_dtmf_pro_cmd_req.icon.data,
           old_buf->cmd.send_dtmf_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy DTMF */
     if((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length > 0) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf =
         (uint8*)MALLOC(sizeof(uint8) * old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL)
       {
         (void)MEMCPY(new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
           old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
           old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length);
       }
     }

     break;
   default:
     MSG_LOW("GOT SOMETHING ELSE",0,0,0);
       break;


  }

  return SUCCESS;
}



/*===========================================================================

FUNCTION: OEMCAT_ReleaseCardCmdBuffer

DESCRIPTION
  Correctly allocate memory for different gstk_cmd_from_card_type structure
  and copy the information accordingly.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter

SIDE EFFECTS
===========================================================================*/
static int OEMCAT_ReleaseCardCmdBuffer(ICAT                    *po,
                                       gstk_cmd_from_card_type * message_p )
{
  int i;

  /* check memory of buffer pointers */
  if((!po) || (message_p == NULL)) {
    return EBADPARM;
  }

  switch(message_p->hdr_cmd.command_id) {
  case GSTK_DISPLAY_TEXT_REQ:
    if (message_p->cmd.display_text_pro_cmd_req.text_string.text != NULL) {
      FREE(message_p->cmd.display_text_pro_cmd_req.text_string.text);
      message_p->cmd.display_text_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.display_text_pro_cmd_req.icon.present) &&
      (message_p->cmd.display_text_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.display_text_pro_cmd_req.icon.data);
      message_p->cmd.display_text_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_GET_INKEY_REQ:
    if (message_p->cmd.get_inkey_pro_cmd_req.text_string.text != NULL) {
      FREE(message_p->cmd.get_inkey_pro_cmd_req.text_string.text);
      message_p->cmd.get_inkey_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.get_inkey_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.get_inkey_pro_cmd_req.icon.data);
      message_p->cmd.get_inkey_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_GET_INPUT_REQ:
    if (message_p->cmd.get_input_pro_cmd_req.text_string.text != NULL) {
      FREE(message_p->cmd.get_input_pro_cmd_req.text_string.text);
      message_p->cmd.get_input_pro_cmd_req.text_string.text = NULL;
    }
    if (message_p->cmd.get_input_pro_cmd_req.default_text.text != NULL) {
      FREE(message_p->cmd.get_input_pro_cmd_req.default_text.text);
      message_p->cmd.get_input_pro_cmd_req.default_text.text = NULL;
    }
    if ((message_p->cmd.get_input_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_input_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.get_input_pro_cmd_req.icon.data);
      message_p->cmd.get_input_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    if (message_p->cmd.launch_browser_pro_cmd_req.url.url != NULL) {
      FREE(message_p->cmd.launch_browser_pro_cmd_req.url.url);
      message_p->cmd.launch_browser_pro_cmd_req.url.url = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL) {
      FREE(message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list);
      message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list = NULL;
    }
    for (i = 0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) {
      if (message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL) {
        FREE(message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths);
        message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths = NULL;
      }
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL) {
      FREE(message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text);
      message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL) {
      FREE(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text = NULL;
    }
    if ((message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
      (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) {
      FREE(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data = NULL;
    }
    break;

  case GSTK_MORE_TIME_REQ:
    /* no dynamically allocated memory */
    break;

  case GSTK_PLAY_TONE_REQ:
    if (message_p->cmd.play_tone_pro_cmd_req.alpha.text != NULL) {
      FREE(message_p->cmd.play_tone_pro_cmd_req.alpha.text);
      message_p->cmd.play_tone_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.play_tone_pro_cmd_req.icon.present) &&
      (message_p->cmd.play_tone_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.play_tone_pro_cmd_req.icon.data);
      message_p->cmd.play_tone_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SELECT_ITEM_REQ:
    if (message_p->cmd.select_item_pro_cmd_req.title.text != NULL) {
      FREE(message_p->cmd.select_item_pro_cmd_req.title.text );
      message_p->cmd.select_item_pro_cmd_req.title.text  = NULL;
    }
    for (i = 0; i< message_p->cmd.select_item_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.select_item_pro_cmd_req.items[i].item_text != NULL) {
        FREE(message_p->cmd.select_item_pro_cmd_req.items[i].item_text );
        message_p->cmd.select_item_pro_cmd_req.items[i].item_text  = NULL;
      }
    }
    if (message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      FREE(message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< message_p->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             FREE(message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           FREE(message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
         }
      }
    }
    if ((message_p->cmd.select_item_pro_cmd_req.icon.present) &&
      (message_p->cmd.select_item_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.select_item_pro_cmd_req.icon.data);
      message_p->cmd.select_item_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SEND_SMS_REQ:
    if (message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL) {
      FREE(message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu);
      message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu = NULL;
    }
    if (message_p->cmd.send_sms_pro_cmd_req.alpha.text != NULL) {
      FREE(message_p->cmd.send_sms_pro_cmd_req.alpha.text);
      message_p->cmd.send_sms_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_sms_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_sms_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.send_sms_pro_cmd_req.icon.data);
      message_p->cmd.send_sms_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SEND_SS_REQ:
    if (message_p->cmd.send_ss_pro_cmd_req.alpha.text != NULL) {
      FREE(message_p->cmd.send_ss_pro_cmd_req.alpha.text);
      message_p->cmd.send_ss_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_ss_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ss_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.send_ss_pro_cmd_req.icon.data);
      message_p->cmd.send_ss_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SEND_USSD_REQ:
    if (message_p->cmd.send_ussd_pro_cmd_req.alpha.text != NULL) {
      FREE(message_p->cmd.send_ussd_pro_cmd_req.alpha.text);
      message_p->cmd.send_ussd_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL) {
      FREE(message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text);
      message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text = NULL;
    }
    if ((message_p->cmd.send_ussd_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.send_ussd_pro_cmd_req.icon.data);
      message_p->cmd.send_ussd_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SETUP_CALL_REQ:
    if (message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL) {
      FREE(message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text = NULL;
    }
    if (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL) {
      FREE(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text = NULL;
    }
    if ((message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
      (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) {
      FREE(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data = NULL;
    }
    break;

  case GSTK_SETUP_MENU_REQ:
    if (message_p->cmd.setup_menu_pro_cmd_req.title.text != NULL) {
      FREE(message_p->cmd.setup_menu_pro_cmd_req.title.text );
      message_p->cmd.setup_menu_pro_cmd_req.title.text  = NULL;
    }
    for (i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL) {
        FREE(message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text );
        message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text  = NULL;
      }
    }
    if (message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      FREE(message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             FREE(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           FREE(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
         }
      }
    }
    if ((message_p->cmd.setup_menu_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.setup_menu_pro_cmd_req.icon.data);
      message_p->cmd.setup_menu_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    if (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL) {
      FREE(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data = NULL;
    }
    break;

  case GSTK_SEND_DTMF_REQ:
    if (message_p->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL) {
      FREE(message_p->cmd.send_dtmf_pro_cmd_req.alpha.text);
      message_p->cmd.send_dtmf_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL) {
      FREE(message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf);
      message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf = NULL;
    }
    if ((message_p->cmd.send_dtmf_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) {
      FREE(message_p->cmd.send_dtmf_pro_cmd_req.icon.data);
      message_p->cmd.send_dtmf_pro_cmd_req.icon.data = NULL;
    }
    break;
  default:
    /* do nothing */
    break;
  }

  return SUCCESS;

} /* OEMCAT_ReleaseCardCmdBuffer*/


/*===========================================================================

FUNCTION: OEMCATGSTKevtCb

DESCRIPTION
  This is the callback function which gets called when ever GSTK has a new
  proactive command to be sent to Cat App.
  It posts an event to Cat App, passing it the proactive command

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/

void OEMCATGSTKevtCb(gstk_cmd_from_card_type *         req_rsp )
{
  boolean                            bReturnStatus = TRUE;
  gstk_general_result_enum_type      result = GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  gstk_additional_info_type          additional_info;
  gstk_terminal_rsp_extra_param_type other_info;
  gstk_cmd_enum_type                 expected_cmd_rsp = GSTK_CMD_ENUM_NOT_USE;
  int                                nReturnStatus;
  IShell                            *shell_ptr = AEE_GetShell();

  MSG_LOW("In OEMCATGSTKEvtCB",0,0,0);

  additional_info.length = 1;
  additional_info.additional_info[0] = (int32)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  other_info.present = FALSE;
  
  if((shell_ptr == NULL) || (req_rsp == NULL))
  {
    return;
  }

  /* Do not update the buffer for the following commands */  
  switch (req_rsp->hdr_cmd.command_id) {
  case GSTK_PROFILE_DL_IND_RSP:
  case GSTK_MENU_IND_RSP:                   
  case GSTK_SMS_PP_DOWNLOAD_IND_RSP:        
  case GSTK_MT_CALL_EVT_IND_RSP:            
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_MO_SMS_CTRL_IND_RSP:
  case GSTK_CC_IND_RSP:
  case GSTK_TIMER_EXPIRE_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    return;
  default:
    break;
  }

  if ((req_rsp->hdr_cmd.command_id != GSTK_END_PROACTIVE_CMD_REQ) &&
    (req_rsp->hdr_cmd.command_id != GSTK_TOOLKIT_MODE_CHANGE_REQ)) 
  {
    /* initialize to 0 */
    (void)MEMSET(&gstk_cmd_from_card_buf, 0x00, sizeof(gstk_cmd_from_card_type));
    /* allocate and memcpy the corresponding gstk_cmd_from_card_type structure elements */
    (void)OEMCAT_PopulateCardCmdBuffer(ICATobj,
      req_rsp->hdr_cmd.command_id,
      req_rsp,
      &gstk_cmd_from_card_buf);

    (void)gstk_map_proactive_req_to_cnf(
                        req_rsp->hdr_cmd.command_id,
                        &expected_cmd_rsp);
  }

  switch(req_rsp->hdr_cmd.command_id)
  {
  case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    if (ICATobj) {
      ICATobj->m_user_act_notify = gstk_cmd_from_card_buf.cmd.evt_notify_req.require_notification;
    }
    break;

  case GSTK_DISPLAY_TEXT_REQ:
     MSG_LOW("GOT DISPLAY TEXT REQUEST IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_DISPLAY_TEXT_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_GET_INKEY_REQ:
       MSG_LOW("GOT GSTK_GET_INKEY_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_GET_INKEY_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_GET_INPUT_REQ:
     MSG_LOW("GOT GSTK_GET_INPUT_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_GET_INPUT_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_PLAY_TONE_REQ:
     MSG_LOW("GOT GSTK_PLAY_TONE_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_PLAY_TONE_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_SELECT_ITEM_REQ:
     MSG_LOW("GOT GSTK_SELECT_ITEM_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SELECT_ITEM_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_PROVIDE_LANG_INFO_REQ:
      MSG_LOW("GOT GSTK_PROVIDE_LANG_INFO_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
      bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_PROVIDE_LANG_INFO_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;

    case GSTK_SEND_SMS_REQ:
     MSG_LOW("GOT GSTK_SEND_SMS_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SEND_SMS_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_SEND_SS_REQ:
     MSG_LOW("GOT GSTK_SEND_SS_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SEND_SS_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_SEND_USSD_REQ:
     MSG_LOW("GOT GSTK_SEND_USSD_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SEND_USSD_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_SETUP_CALL_REQ:
       MSG_LOW("GOT GSTK_SETUP_CALL_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SETUP_CALL_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
    case GSTK_SETUP_MENU_REQ:
     MSG_LOW("GOT GSTK_SETUP_MENU_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SETUP_MENU_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
     case GSTK_MORE_TIME_REQ:
     MSG_LOW("GOT GSTK_MORE_TIME_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_MORE_TIME_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
     case GSTK_SETUP_IDLE_TEXT_REQ:
     MSG_LOW("GOT GSTK_SETUP_IDLE_TEXT_REQ IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_UPDATE_IDLE_TEXT_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
   case GSTK_END_PROACTIVE_CMD_REQ:
     MSG_LOW("GOT EVT_CAT_END_PROACTIVE_CMD_SES IN GSTK_CALLBACK",0,0,0);
     
#ifdef FEATURE_APP_CATAPP
     if (req_rsp->cmd.end_pro_cmd_req.end_from_card) {
       bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_END_PROACTIVE_CMD_SES, 0,
                           (uint32) TRUE);
     }
     else {
       bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_END_PROACTIVE_CMD_SES, 0,
                           (uint32) FALSE);
     }
#endif
     break;
   case GSTK_TOOLKIT_MODE_CHANGE_REQ:
     MSG_LOW("GOT EVT_CAT_END_SESSION_WO_TR IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_END_PROACTIVE_CMD_SES_WITHOUT_TR, 0,
                           (uint32) 0L);
#endif
     break;
   case GSTK_LANG_NOTIFICATION_REQ:
     MSG_LOW("GOT LANG NOTIFICATION REQUEST IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_LANG_NOTIFICATION_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
   case GSTK_SEND_DTMF_REQ:
     MSG_LOW("GOT SEND DTMF REQUEST IN GSTK_CALLBACK",0,0,0);
#ifdef FEATURE_APP_CATAPP
     bReturnStatus = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CAT_SEND_DTMF_REQ, 0,
                           (uint32) &gstk_cmd_from_card_buf);
#endif
     break;
   default:
     MSG_LOW("GOT SOMETHING ELSE",0,0,0);
     break;

  }
  if(bReturnStatus == FALSE) {
    nReturnStatus = OEMCAT_SendTerminalResponse(ICATobj, 0, gstk_cmd_from_card_buf.hdr_cmd.cmd_detail_reference,
      gstk_cmd_from_card_buf.hdr_cmd.command_number,
      expected_cmd_rsp,
      result,
      &additional_info,
      &other_info);
    if (nReturnStatus != SUCCESS) {
      MSG_ERROR("Unable to send terminal response", 0, 0, 0);
    }
  }
}

/*===========================================================================

FUNCTION: OEMCAT_Init

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMCAT_Init(ICAT *po,
                       uint32 ud,
                       gstk_client_type client_type,
                       uint32 evt_reg_bitmask)
{
  gstk_status_enum_type   client_status;

  client_status = gstk_client_init(&cl_id,  client_type );

  if (client_status == GSTK_SUCCESS)
  {
    client_status = gstk_client_reg(cl_id,
                                    ud,
                                    (gstk_evt_cb_funct_type)OEMCATGSTKevtCb,
                                    evt_reg_bitmask);

    if (client_status != GSTK_SUCCESS)
    {
      ERR_FATAL("client_init error", 0,0,0);
    }
  }
  else
  {
     ERR_FATAL("client_init error", 0,0,0);
  }

  /* done */
  return SUCCESS;

} /* OEMCAT_Init() */ /*lint !e715 */


/*===========================================================================

FUNCTION: OEMCAT_SendEnvelopeMenuSelCommand

DESCRIPTION
   Sends an envelope command to GSTK

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMCAT_SendEnvelopeMenuSelCommand(ICAT          *po,
                                      uint32    ud,
                                      byte          ic,
                                      boolean       hr)
{
  gstk_status_enum_type   client_status;

  client_status = gstk_send_envelope_menu_sel_command(cl_id, ud, ic, hr);
  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */

/*===========================================================================

FUNCTION: OEMCAT_SendTerminalResponse

DESCRIPTION
   Sends a Terminal Response to GSTK

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Sending was success
  EFAILED  - Send Failure


SIDE EFFECTS
===========================================================================*/
static int OEMCAT_SendTerminalResponse(
              ICAT                                        *po,
              uint32                                      ud,
              uint32                                      cdri,
              uint32                                      cmd_num,
              gstk_cmd_enum_type                          term_resp_e,
              gstk_general_result_enum_type               result,
              gstk_additional_info_type                   *additional_result,
              gstk_terminal_rsp_extra_param_type          *other)
{
  gstk_status_enum_type   client_status;

  /* release the allocated memory */
  (void)OEMCAT_ReleaseCardCmdBuffer(po, &gstk_cmd_from_card_buf);

  client_status = gstk_send_terminal_response( cl_id,
                                               ud,
                                               cdri,
                                               uint32touint8(cmd_num),
                                               term_resp_e,
                                               result,
                                               additional_result,
                                               other);

  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
}

/*===========================================================================

FUNCTION: OEMCAT_SendEnvelopEvtDownloadCommand

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Send of envelope evt command passed
  EFAILED   - Send of envelope evt command failed

SIDE EFFECTS
===========================================================================*/
static int OEMCAT_SendEnvelopEvtDownloadCommand(
                            ICAT                                *po,
                            uint32                ud,
                            gstk_evt_dl_ind_type  *evt_list )
{
  gstk_status_enum_type   client_status;

  client_status = gstk_send_envelope_evt_download_command(
                                cl_id,
                                ud,
                                (gstk_evt_dl_ind_type*)evt_list);

  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */

/*===========================================================================
FUNCTION: OEMCAT_UpdateProfileDownloadValue,

DESCRIPTION
  Allows UI to the change profile download value

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Profile download value sent to GSTK
  EFAILED  - Profile download not sent to GSTK


SIDE EFFECTS

===========================================================================*/
static int OEMCAT_UpdateProfileDownloadValue(
                    ICAT                                          *po,
                                  uint32                          ud,
                    byte                                          num_updates,
                    gstk_profile_dl_support_type    *value_list)
 {
  gstk_status_enum_type   client_status;

  client_status = gstk_update_profile_download_value(
                            cl_id,
                            ud,
                            num_updates,
                            value_list);

  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */





