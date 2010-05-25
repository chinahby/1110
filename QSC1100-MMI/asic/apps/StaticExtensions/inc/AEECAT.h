#ifndef AEECAT_H
#define AEECAT_H

/*============================================================================
FILE:  AEECAT.h

SERVICES:  AEE ICAT Interface

GENERAL DESCRIPTION:
  This module supports the AEE interface for supporting CAT (Card Application
  Toolkit).

        Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEECAT.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/05   tml     Linuxify fixes
08/16/04   tml     Added User Activity notification
02/11/03   tml     Added ICAT_PopulateCardCmdBuffer and 
                   ICAT_ReleaseCardCmdBuffer. 
02/05/03   tml     Removed CoreApp related includes and formatting
11/19/02   aaa      Initial version.
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "AEE.h"
#include "AEEComdef.h"

/* CAT API definitions
*/
#include "gstk_exp.h"


/* Class ID files
*/
#include "CAT.BID"
#include "catapp.h"



/*===========================================================================

                         CLASS DEFINITIONS

===========================================================================*/


/* Forward typedef reference of the objects.
*/
typedef struct ICAT ICAT;    



/* ICAT interface class definition
*/
AEEINTERFACE(ICAT)
{
  INHERIT_IQueryInterface(ICAT);

  int    (*Init)(
                 ICAT	            *po, 
                 uint32           user_data,  
                 gstk_client_type	client_type , 
                 uint32           evt_reg_bitmask);     

  int    (*SendEnvelopeMenuSelCommand)(
                 ICAT		    *po,
                 uint32     user_data,
                 byte       item_chosen,                          
                 boolean    help_requested);

  int    (*SendTerminalResponse)(
                 ICAT                               *po,	                      
                 uint32                             user_data,                 
                 uint32                             cmd_detail_ref_id,                 
                 uint32                             cmd_num,						  
                 gstk_cmd_enum_type                 term_resp_e,						  
                 gstk_general_result_enum_type		  result,						  
                 gstk_additional_info_type			    *additional_result,						  
                 gstk_terminal_rsp_extra_param_type *other);

  int    (*SendEnvelopEvtDownloadCommand)(
                 ICAT                    *po,	
	               uint32                  user_data,
                 gstk_evt_dl_ind_type    *evt_list);

  int    (*UpdateProfileDownloadValue)(
						     ICAT                           *po,
                 uint32                         user_data,
                 byte							              num_updates,
                 gstk_profile_dl_support_type   *value_list);
  
  int    (*PopulateCardCmdBuffer)(
                 ICAT                     *po, 
                 gstk_cmd_enum_type       command_type,
                 gstk_cmd_from_card_type  *old_buf,
                 gstk_cmd_from_card_type  *new_buf);
  int    (*ReleaseCardCmdBuffer)(
                 ICAT                    *po, 
                 gstk_cmd_from_card_type * message_p );
  int    (*UserActivity)(ICAT *po);

};
/* end of ICAT interface definition */


/* Defines for accessing the ICAT class
*/
#define ICAT_AddRef(p)                 AEEGETPVTBL(p,ICAT)->AddRef(p)
#define ICAT_Release(p)                AEEGETPVTBL(p,ICAT)->Release(p)
#define ICAT_QueryInterface(p, clsid, pp)   \
          AEEGETPVTBL(p,ICAT)->QueryInterface(p,clsid,pp)

#define ICAT_Init(p,ud, clientType, evt_reg_bitmask) \
          AEEGETPVTBL(p,ICAT)->Init(p,ud, clientType, evt_reg_bitmask)

#define ICAT_SendEnvelopeMenuSelCommand(p, user_data, item_chosen, help) \
          AEEGETPVTBL(p,ICAT)->SendEnvelopeMenuSelCommand(p, user_data, item_chosen, help)

#define ICAT_SendTerminalResponse(p, user_data, cmd_detail_ref_id, cmd_num, tr_enum, result, add_result, other) \
          AEEGETPVTBL(p,ICAT)->SendTerminalResponse(p, user_data, cmd_detail_ref_id, cmd_num, tr_enum, result, add_result, other)

#define ICAT_SendEnvelopEvtDownloadCommand(p,  user_data, evt_list) \
          AEEGETPVTBL(p,ICAT)->SendEnvelopEvtDownloadCommand(p, user_data, evt_list)

#define ICAT_UpdateProfileDownloadValue(p, user_data,num_updates, value_list) \
          AEEGETPVTBL(p,ICAT)->UpdateProfileDownloadValue(p, user_data,num_updates, value_list)

#define ICAT_PopulateCardCmdBuffer(p, command_type, old_buf, new_buf) \
          AEEGETPVTBL(p,ICAT)->PopulateCardCmdBuffer(p, command_type, old_buf, new_buf)

#define ICAT_ReleaseCardCmdBuffer(p, message_pointer) \
          AEEGETPVTBL(p,ICAT)->ReleaseCardCmdBuffer(p, message_pointer)

#define ICAT_UserActivity(p) \
          AEEGETPVTBL(p,ICAT)->UserActivity(p)

#endif    /* AEECAT_H */
