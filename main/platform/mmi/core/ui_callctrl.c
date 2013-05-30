/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This file contains the call control related functionality from the
  user interface.  This is used for both normal and ThinUI builds.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999 through 2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/ui_callctrl.c#27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/08   sg      Additional condition to look for before doing FDN checks
07/31/07   ypmw    Extend band classes with selected system code upto 22867
06/04/07   cvs     Copy alpha from GSTK response for GPRS.
06/04/07   cvs     Copy alpha from GSTK response.
03/23/07   cvs     Use gsdi_get_sim_capabilities asynchronously.
02/28/07   cvs     Fix FDN call control behavior.
02/23/07   cvs     Fix mem leak
11/27/06   cvs     Fix behavior for GPRS calls when CM API is defined,
                   but GSTK feature isn't
09/27/06   cvs     Fix typo in feature name
09/25/06   cvs     Only check gw_ps_cc_params for NULL if packet call
09/22/06   cvs     Fix compile warning
08/10/06   cvs     Fix featurization
07/27/06   cvs     Allow PS calls if we can't check them
07/24/06   cvs     Fix compile error
07/21/06   cvs     Fix compile error
07/17/06   cvs     Fix compile warning
07/13/06   cvs     Fixing call control featurization
07/11/06   cvs     Handle network provided APNs
06/28/06   cvs     Fix ACL check
05/24/06   cvs     Add call control for PS calls
04/24/06   cvs     fix compile warning
04/12/06   cvs     Initial revision
===========================================================================*/

/* <EJECT> */
/*
 * Secret decoder ring for features used in this file:
 * FEATURE_UI_DUALPROC_APPS   // Dual processor Apps processor
 * FEATURE_UI_DUALPROC_MDM    // Dual processor modem processor
 * FEATURE_CDMA_1900       // CDMA 1900Mhz frequencies
 * FEATURE_CDMA_800        // CDMA 800Mhz frequencies
 * FEATURE_GSM             // GSM
 * FEATURE_GSM_AMR         // Sub GSM Feature
 * FEATURE_GSM_EXT_SPEECH_PREF_LIST// Sub GSM Feature
 * FEATURE_GSM_HALF_RATE   // Sub GSM Feature
 * FEATURE_GSTK            // Supports Generic Sim Tool Kit (more functionality than GSDI)
 * FEATURE_GSTK_DUAL_SLOTS // Supports Generic Sim Tool Kit on two SIMS
 * FEATURE_MMGSDI          // We have GSDI
 * FEATURE_OTASP           // Supports OTASP calls
 * FEATURE_PLT             // Physical layer test--modem bringup
 * FEATURE_SEC_DRM         // Support Digital Rights Management
 * FEATURE_SLEEP           // Supports Sleep.
 * FEATURE_UIM             // Support the SIM/RUIM/USIM
 * FEATURE_UI_CORE_REMOVED // CoreAPP does not exist on this processor
 * FEATURE_UI_OTASP_STAR2281X      // OTASP has *228 for 1X
 * FEATURE_UI_PBM          // Phone book manager
 * FEATURE_WCDMA           // WCDMA
 */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "err.h"

#include <string.h>

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#include "ui_callctrl.h"
#include "uiutils.h"
#include "cm.h"
#include "tmc.h"

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstki.h"
#endif

#ifdef FEATURE_UI_PBM
#include "pbmlib.h"
#ifdef FEATURE_UI_DUALPROC_APPS
#include "pbm_conversion.h"
#endif
#endif


#if !defined(FEATURE_UI_DUALPROC_APPS)
#ifdef FEATURE_GSTK
void ui_envelope_command_cb(gstk_cmd_from_card_type *req_rsp);
boolean ui_gsdi2_call_control_request(gstk_slot_id_enum_type slot,
    gstk_call_enum_type cmd, byte *buf, int length,
    byte *subaddress,
#ifdef CM_API_GW_PS_CC
    cm_gw_ps_cc_params_s_type gw_ps_cc_params,
#endif
    uint32 user_data,
    gstk_evt_cb_funct_type gstk_cc_cb );
#endif /*FEATURE_GSTK */
#endif /* !defined(FEATURE_UI_DUALPROC_APPS) */

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#if !defined(FEATURE_UI_DUALPROC_APPS)
#ifdef FEATURE_OTASP
#define STAR228 "*228"
#define STANDARD_OTASP_LENGTH 6
  /* Table specifying default hard-coded special numbers and their
  ** corresponding call type.
  **
  ** Note that for OTASP numbers the corresponding activation code
  ** is also specified.
  */
  static const struct {
      byte                     *num_str;      /* number string */
      cm_call_type_e_type      call_type;     /* associated call type */
      cm_activate_code_e_type  activate_code; /* OTASP activation code */
  } otasp_num_tbl[] = {

#ifdef CM_API_OTASP_ACTIVATE_CODE
      /* number */   /* call type */               /* OTASP activate code */
      { (byte *) "*22800",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC0_ABAND     },
      { (byte *) "*22801",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC0_BBAND     },
      { (byte *) "*22802",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_ABLOCK    },
      { (byte *) "*22803",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_BBLOCK    },
      { (byte *) "*22804",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_CBLOCK    },
      { (byte *) "*22805",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_DBLOCK    },
      { (byte *) "*22806",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_EBLOCK    },
      { (byte *) "*22807",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC1_FBLOCK    },
      { (byte *) "*22812",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC4_ABAND    },
      { (byte *) "*22813",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC4_BBAND    },
      { (byte *) "*22814",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC4_CBAND    },
      { (byte *) "*22815",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_ABAND    },
      { (byte *) "*22816",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_BBAND    },
      { (byte *) "*22817",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_CBAND    },
      { (byte *) "*22818",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_DBAND    },
      { (byte *) "*22819",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_EBAND    },
      { (byte *) "*22820",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_FBAND    },
      { (byte *) "*22821",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_GBAND    },
      { (byte *) "*22822",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC5_HBAND    },
      { (byte *) "*22823",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC6          },
      { (byte *) "*22826",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC7_CBAND    },
      { (byte *) "*22827",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC7_DBAND    },
      { (byte *) "*22828",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC8          },
      { (byte *) "*22829",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC9          },
      { (byte *) "*22830",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC10_ABAND    },
      { (byte *) "*22831",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC10_BBAND    },
      { (byte *) "*22832",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC10_CBAND    },
      { (byte *) "*22833",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC10_DBAND    },
      { (byte *) "*22834",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC10_EBAND    },
      { (byte *) "*22835",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_ABAND    },
      { (byte *) "*22836",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_BBAND    },
      { (byte *) "*22837",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_CBAND    },
      { (byte *) "*22838",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_DBAND    },
      { (byte *) "*22839",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_EBAND    },
      { (byte *) "*22840",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC11_FBAND    },
      { (byte *) "*22841",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC12_ABAND    },
      { (byte *) "*22842",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC12_BBAND    },
      { (byte *) "*22857",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC14_GBLOCK   },
      { (byte *) "*22858",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_ABAND    },
      { (byte *) "*22859",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_BBAND    },
      { (byte *) "*22860",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_CBAND    },
      { (byte *) "*22861",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_DBAND    },
      { (byte *) "*22862",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_EBAND    },
      { (byte *) "*22863",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC15_FBAND    },
      { (byte *) "*22864",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC16_ABAND    },
      { (byte *) "*22865",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC16_BBAND    },
      { (byte *) "*22866",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC16_CBAND    },
      { (byte *) "*22867",    CM_CALL_TYPE_STD_OTASP,       CM_ACTIVATE_CODE_BC16_DBAND    },
        { (byte *) "*22880", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC0_ABAND },

        { (byte *) "*22881", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC0_BBAND },

        { (byte *) "*22882", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_ABLOCK },

        { (byte *) "*22883", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_BBLOCK },

        { (byte *) "*22884", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_CBLOCK },

        { (byte *) "*22885", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_DBLOCK },

        { (byte *) "*22886", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_EBLOCK },

        { (byte *) "*22887", CM_CALL_TYPE_STD_OTASP, CM_ACTIVATE_CODE_BC1_FBLOCK },

         /* *22888 - *22899 Shall match *228 exactly - use preferred roaming list */

        { (byte *) "*22888",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22889",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22890",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22891",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22892",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22893",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22894",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22895",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22896",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22897",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22898",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

        { (byte *) "*22899",    CM_CALL_TYPE_NON_STD_OTASP,   CM_PREFERRED   },

#else /*CM_API_OTASP_ACTIVATE_CODE*/

#ifdef FEATURE_CDMA_800
      { (byte *) "*22800",    CM_CALL_TYPE_STD_OTASP,       CM_ABAND     },
      { (byte *) "*22801",    CM_CALL_TYPE_STD_OTASP,       CM_BBAND     },
#endif
#ifdef FEATURE_CDMA_1900
      { (byte *) "*22802",    CM_CALL_TYPE_STD_OTASP,       CM_ABLOCK    },
      { (byte *) "*22803",    CM_CALL_TYPE_STD_OTASP,       CM_BBLOCK    },
      { (byte *) "*22804",    CM_CALL_TYPE_STD_OTASP,       CM_CBLOCK    },
      { (byte *) "*22805",    CM_CALL_TYPE_STD_OTASP,       CM_DBLOCK    },
      { (byte *) "*22806",    CM_CALL_TYPE_STD_OTASP,       CM_EBLOCK    },
      { (byte *) "*22807",    CM_CALL_TYPE_STD_OTASP,       CM_FBLOCK    },
#endif
      { (byte *) "*22812",    CM_CALL_TYPE_STD_OTASP,       CM_ABLOCK    },
      { (byte *) "*22813",    CM_CALL_TYPE_STD_OTASP,       CM_BBLOCK    },
      { (byte *) "*22814",    CM_CALL_TYPE_STD_OTASP,       CM_CBLOCK    },
      { (byte *) "*22815",    CM_CALL_TYPE_STD_OTASP,       CM_DBLOCK    },
      { (byte *) "*22816",    CM_CALL_TYPE_STD_OTASP,       CM_EBLOCK    },
      { (byte *) "*22817",    CM_CALL_TYPE_STD_OTASP,       CM_FBLOCK    }

#endif /*CM_API_OTASP_ACTIVATE_CODE*/
};

#endif /* FEATURE_OTASP */

#if defined(FEATURE_MMGSDI)
  #ifdef FEATURE_GSTK_DUAL_SLOTS
    /* Loop constants for looking through different sources of information. */
    static cm_cc_subscription_source_e_type sub_sources[] =
      { CM_CC_SUBSCRIPTION_SOURCE_SIM_1, CM_CC_SUBSCRIPTION_SOURCE_SIM_2 };
#if defined(FEATURE_UI_PBM)
    static pbm_device_type pbm_fdns[] = { PBM_FDN, PBM_FDN2 };
#endif
    static gsdi_slot_id_type card_slots[] = { GSDI_SLOT_1, GSDI_SLOT_2 };
  #else
    /* Loop constants for looking through different sources of information. */
    static cm_cc_subscription_source_e_type sub_sources[] =
      { CM_CC_SUBSCRIPTION_SOURCE_SIM_1 };
#if defined(FEATURE_UI_PBM)
    static pbm_device_type pbm_fdns[] = { PBM_FDN };
#endif
    static gsdi_slot_id_type card_slots[] = { GSDI_SLOT_1 };
  #endif
#endif /* defined(FEATURE_UIM) && defined(FEATURE_UI_PBM) */


/* <EJECT> */
/*===========================================================================
FUNCTION   get_call_type

DESCRIPTION
  Determine call type for a origination #

DEPENDENCIES
  None

RETURNS
  None, result will be put into call_type_ptr, otasp_code_ptr, and ecc_category

SIDE EFFECTS
  None
===========================================================================*/
void get_call_type(
  const cm_num_s_type     *orig_num_ptr,    // pointer to origination #
  cm_call_type_e_type     *call_type_ptr,   // associated call type, save result
  cm_activate_code_e_type *otasp_code_ptr,  // OTASP activation code(OTASP call)
  uint8                   *ecc_category     // emergency category (emergency call)
)
/*lint -esym(715,otasp_code_ptr)*/
{
#ifdef FEATURE_OTASP
  int   i;
#endif /* FEATURE_OTASP */

  uint8 ecc_cat = 0;

  /* If this is NOT a voice call, return now  */
  if( *call_type_ptr != CM_CALL_TYPE_VOICE ) {
    return;
  }

#ifdef FEATURE_UI_PBM
  if (pbm_emergency_number_cat(orig_num_ptr->buf, orig_num_ptr->len, &ecc_cat))
  {
    *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
    *ecc_category = ecc_cat;
    MSG_HIGH("found emergency number, category %d",ecc_cat,0,0);
    return;
  }
#else
  if (OEM_IsEmergency_Number(orig_num_ptr->buf, orig_num_ptr->len))
  {
    *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
    *ecc_category = ecc_cat;
    return;
  }
#endif

#ifdef FEATURE_OTASP
  /* "*228" is checked specially. */
  if ( orig_num_ptr->len == strlen(STAR228) &&
       ! memcmp(orig_num_ptr->buf, STAR228, strlen(STAR228)) )
  {
    *call_type_ptr  = CM_CALL_TYPE_NON_STD_OTASP;
    *otasp_code_ptr = CM_PREFERRED;
    return;
  }

  /* Consult the default hard-coded otasp numbers table to
  ** determine the originating number call type.
  */
  for( i=0; i< (int)ARR_SIZE(otasp_num_tbl); i++ )
    {
      /* Check whether one of the otasp numbers match the origination #.
      */
      if( orig_num_ptr->len >= STANDARD_OTASP_LENGTH &&
        ! memcmp(orig_num_ptr->buf, otasp_num_tbl[i].num_str, STANDARD_OTASP_LENGTH) )
      {
        /* If we found a match, change the call type to the corresponding,
        ** type in the table and indicate the OTASP activation code.
        */
        *call_type_ptr  = otasp_num_tbl[i].call_type;
        *otasp_code_ptr = otasp_num_tbl[i].activate_code;
        return;
      }
    } /* end of for */

    /* Not hard coded OTASP number, check NV phone book
    */
#ifdef FEATURE_UI_PBM
    for (i = 0; i < PBM_NV_EMERGENCY_NUMBERS; i++) {
      const byte* num2_buf;
      byte  num2_len;
      nv_dial_type  ui_dial;
      ui_dial.address = (byte) (i + PBM_NV_EMERGENCY_NUMBER_START);
      (void) ui_get_nv_for_cm( NV_DIAL_I, ( nv_item_type * ) &ui_dial );
      num2_buf = (byte *) ui_dial.digits;
      num2_len = ui_dial.num_digits;

      if( pbm_number_match(orig_num_ptr->buf, orig_num_ptr->len,
                            num2_buf, num2_len) )
      {
        if( ui_dial.status == NV_OTASP_SD_NUM ||
            ui_dial.status == NV_OTASP2_SD_NUM )
        {
          /* If we found a match, change the call type to the corresponding,
          ** type in the table and indicate the OTASP activation code.
          */
          *call_type_ptr  = CM_CALL_TYPE_NON_STD_OTASP;
          *otasp_code_ptr = CM_PREFERRED;
          return;
        }
      } /*end if */
    } /* end of for */
#endif /* FEATURE_UI_PBM */
#endif /* FEATURE_OTASP */

}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_SET_RESTRICTIONS

DESCRIPTION
  This function sets the restriction_bitmask to the value from Core.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte ui_saved_restrictions_from_core;

void ui_set_restrictions(byte restriction_bitmask)
{
  ui_saved_restrictions_from_core = restriction_bitmask;
}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_RESTRICT_DIALING_TO

DESCRIPTION
  This function checks the restriction_bitmask that was set with the ui_set
  restrictions function above and returns TRUE if the PBM file given has
  the bit in the mask set which corresponds to this restriction.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_UI_PBM
boolean ui_restrict_dialing_to(pbm_device_type pbm_file)
{
  switch ( pbm_file )
  {
    case PBM_EFS:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_EFS));
    case PBM_SDL:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_SPEED_DIAL));
    case PBM_ADN:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_ADN_SLOT1));
    case PBM_ADN2:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_ADN_SLOT2));
    case PBM_FDN:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_FDN_SLOT1));
    case PBM_FDN2:
      return (0 != (ui_saved_restrictions_from_core & COREUTILS_FDN_SLOT2));
    default:
      return FALSE;
  }
}
#endif /* FEATURE_UI_PBM */

/* The following ENUM is used to determine if call control is required for
 * a given technology.  Call control and the SIM from which the subscription
 * is obtained are now linked.
 */
typedef enum {
  CC_NOT_REQUIRED,              /* Either not a SIM subscription or the SIM
                                   doesn't require Call Control */
  CC_REQUIRED,                  /* Call control is required for this
                                   subscription */
  CC_COMPLETED                  /* Call control was required, and has been
                                   completed. */
} uixcm_cc_status_e_type;

/* The following structure is used when issueing a call control command
 * to keep track of what is required, what has been completed, and the
 * results.
 */
typedef struct {
  uixcm_cc_status_e_type sim1_cc_required;      /* What to do about SIM 1 */
  uixcm_cc_status_e_type sim2_cc_required;      /* What to do about SIM 2 */
  cm_nc_call_control_complete_cb_f_type *cb_func;       /* Callback to use */
  void  *user_data;                             /* User data supplied. */
  cm_call_id_type call_id;                      /* Original Call ID. */
  cm_cc_status_e_type status;                   /* Determined status. */
  cm_call_type_e_type call_type;                /* Determined call type */
  cm_orig_call_control_s_type original_origination_request; /* Original orig. */
  cm_orig_call_control_s_type sim1_cc_result;   /* Result of first SIM CC */
  cm_orig_call_control_s_type sim2_cc_result;   /* Result of second SIM CC */
  cm_orig_call_control_s_type *cdma_result_ptr; /* Pointer to above. */
  cm_orig_call_control_s_type *gw_result_ptr; /* Pointer to above. */
#ifdef FEATURE_MMGSDI
  sim_capabilities_T sim_capabilities[ARR_SIZE(card_slots)];
  boolean            sim_capabilities_valid[ARR_SIZE(card_slots)];
#endif

  /* The following are params from CM that we can't fully use immediately */
  cm_cc_subscription_source_e_type      cdma_subscription_source;
  cm_cc_subscription_source_e_type      gw_subscription_source;

#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
  cm_cc_generic_params_s_type           cc_generic_params;
#endif
} CC_REQUEST;

/* <EJECT> */
/*===========================================================================
FUNCTION UI_SEND_CC_COMMAND

DESCRIPTION
  Look at the request structure and determine which request to do next.
  This function will then call ui_gsdi2_call_control_request to issue
  the request on the correct slot.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  request_ptr may be modified if the request fails.

===========================================================================*/
void ui_send_cc_command(CC_REQUEST *request_ptr)
{
#if defined(FEATURE_GSTK)
  gstk_call_enum_type cc_type = GSTK_VOICE;

  if (request_ptr == NULL)
  {
    ERR("NULL Request ptr",0,0,0);
  }

#if defined (CM_API_GW_PS_CC)
  if (request_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    //If GSTK will accept this, set up the type, otherwise, don't send to GSTK
#if defined (FEATURE_GPRS_CALLCONTROL)

    cc_type = GSTK_PDP_CONTEXT_ACT;

#else

    request_ptr->sim1_cc_required = CC_NOT_REQUIRED;
#ifdef FEATURE_GSTK_DUAL_SLOTS
    request_ptr->sim2_cc_required = CC_NOT_REQUIRED;
#endif //FEATURE_GSTK_DUAL_SLOTS

#endif //FEATURE_GPRS_CALLCONTROL
  }
#endif //CM_API_GW_PS_CC

  /* If we are required to send the command to SLOT 1. */
  if (request_ptr->sim1_cc_required == CC_REQUIRED)
  {
    if (ui_gsdi2_call_control_request(GSTK_SLOT_1, cc_type,
        request_ptr->original_origination_request.num.buf,
        request_ptr->original_origination_request.num.len,
        request_ptr->original_origination_request.num.subaddr.extension_bit?
          &request_ptr->original_origination_request.num.subaddr.num_char:NULL,
#ifdef CM_API_GW_PS_CC
        request_ptr->original_origination_request.gw_ps_cc_params,
#endif
        (uint32) request_ptr, ui_envelope_command_cb))
      return;
    else
    {
      request_ptr->sim1_cc_required = CC_NOT_REQUIRED;
      request_ptr->status = CM_CC_STATUS_ERROR;
      request_ptr->sim1_cc_result.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
    }
  }
#ifdef FEATURE_GSTK_DUAL_SLOTS
  /* Slot 1 is complete, now check if we are required to send
   * the command to SLOT 2. */
  if (request_ptr->sim2_cc_required == CC_REQUIRED)
  {
    if (ui_gsdi2_call_control_request(GSTK_SLOT_2, cc_type,
        request_ptr->original_origination_request.num.buf,
        request_ptr->original_origination_request.num.len,
        request_ptr->original_origination_request.num.subaddr.extension_bit?
          &request_ptr->original_origination_request.num.subaddr.num_char:NULL,
#ifdef CM_API_GW_PS_CC
        request_ptr->original_origination_request.gw_ps_cc_params,
#endif
        (uint32) request_ptr, ui_envelope_command_cb))
      return;
    else
    {
      request_ptr->sim2_cc_required = CC_NOT_REQUIRED;
      request_ptr->status = CM_CC_STATUS_ERROR;
      request_ptr->sim2_cc_result.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
    }
  }
#endif
#endif /* FEATURE_GSTK */

  /* If we reach here, both SLOT 1 and SLOT 2 is complete, this will
   * send the command to CM. */
  (*request_ptr->cb_func)( request_ptr->call_id,
                           request_ptr->status,
                           request_ptr->call_type,
                           request_ptr->cdma_result_ptr,
                           request_ptr->gw_result_ptr,
                           (void*) request_ptr->user_data);

  /* This sequence is complete, so free the memory. */
  mem_free(&tmc_heap, request_ptr);
} //void send_cc_command(CC_REQUES...

#if defined(FEATURE_GSTK)
/*===========================================================================
FUNCTION uixgsdi_ascii_to_gstk

DESCRIPTION
  The GSTK wants digits with the decimal value in each byte.  We need to
  the character string convert this array of integers.

RETURN VALUE
  None.

DEPENDENCIES
  None.
===========================================================================*/
void uixgsdi_ascii_to_gstk(byte *buf, uint32 len, gstk_address_type *address)
{
   int i = 0;   /* Loop variable though buf array. */
   int j = 0;   /* Loop variable for addresses array of decimal bytes. */

   address->TON = GSTK_UNKNOWN_TON;
   address->NPI = GSTK_ISDN_TELEPHONY_NPI;

   if (buf[i] == '+')
   {
     address->TON = GSTK_INTERNATIONAL_NUM;
     i++;
   }

   for (; i < len; i++)
   {
     switch (buf[i])
     {
       case '0': case '1': case '2': case '3': case '4':
       case '5': case '6': case '7': case '8': case '9':
         address->address[j++] = buf[i] - '0';
         break;
       case '*':
         address->address[j++] = 0x0a;
         break;
       case '#':
         address->address[j++] = 0x0b;
         break;
       default:
         ERR("Unexpected value for buf[%d], %x", i, buf[i], 0);
         break;
     } //     switch (buf[i])...
   } //   for (j = 0, i = 0; i < len;...

   address->length = j;                     /* Save the length. */
   return;
} //void uixgsdi_ascii_to_gstk(byt...

/* <EJECT> */
/*===========================================================================

FUNCTION ui_gsdi2_call_control_request

DESCRIPTION
  Take the SS command and sends the envelope command to GSTK.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean ui_gsdi2_call_control_request(gstk_slot_id_enum_type slot,
    gstk_call_enum_type cmd, byte *buf, int length,
    byte *subaddress,
#ifdef CM_API_GW_PS_CC
    cm_gw_ps_cc_params_s_type gw_ps_cc_params,
#endif
    uint32 user_data,
    gstk_evt_cb_funct_type gstk_cc_cb )
{
  gstk_cc_ind_type command_data;
  gstk_status_enum_type status;

  memset(&command_data, 0, sizeof(command_data));

  command_data.call_enum_type = cmd;
  switch (cmd)
  {
    case GSTK_SS:
    {
      command_data.gstk_address_string.ss_string.TON = GSTK_RFU_TON;
      command_data.gstk_address_string.ss_string.NPI = GSTK_RFU_NPI;
      command_data.gstk_address_string.ss_string.length = length;
      memcpy(command_data.gstk_address_string.ss_string.address, buf, length);
      break;
    }
    case GSTK_USSD:
    {
      command_data.gstk_address_string.ussd_string.dcs = 0; /* DCS from cell broadcast. */
      command_data.gstk_address_string.ussd_string.length = length;
      command_data.gstk_address_string.ussd_string.text = buf;
      break;
    }
    case GSTK_VOICE:
    {
      uixgsdi_ascii_to_gstk(buf, length,
          &command_data.gstk_address_string.voice.address);

      if ((subaddress != NULL) &&
          (*subaddress <
           sizeof(command_data.gstk_address_string.voice.subaddress.subaddress))
         )
      {
        command_data.gstk_address_string.voice.subaddress.length = *subaddress;
        memcpy(command_data.gstk_address_string.voice.subaddress.subaddress,
            &subaddress[1], subaddress[0]);
      }

      command_data.gstk_address_string.voice.ccp1.length = 0;
      command_data.gstk_address_string.voice.ccp2.length = 0;
      break;
    }
#if defined (CM_API_GW_PS_CC) && defined (FEATURE_GPRS_CALLCONTROL)
    case GSTK_PDP_CONTEXT_ACT:
    {
      command_data.gstk_address_string.pdp_context.length =
        MIN(gw_ps_cc_params.length,
            sizeof(command_data.gstk_address_string.pdp_context.pdp_act_param));

      memcpy(command_data.gstk_address_string.pdp_context.pdp_act_param,
             gw_ps_cc_params.pdp_act_mesg,
             command_data.gstk_address_string.pdp_context.length);
      break;
    }
#endif
    default:
      ERR("Unexpected cmd to ui_gsdi_call_control_ss_request %d", cmd, 0, 0);
      return FALSE;
  }
  /* build the envelope command and send it to the GSTK; */
  status = gstk_slot_send_envelope_cc_command (slot, (uint32) user_data, &command_data,
                  gstk_cc_cb);
  return (GSTK_SUCCESS == status);
}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_GSTK_TO_CM_TON

DESCRIPTION
  GSTK has their own constants for the type of number.  This function will
  map a GSTK Type of number to the CM type of number.

DEPENDENCIES
  None

RETURN VALUE
  CM Type of Number.

SIDE EFFECTS
  None

===========================================================================*/
uint8 ui_gstk_to_cm_ton(gstk_ton_enum_type ton)
{
  switch (ton)
  {
    case GSTK_UNKNOWN_TON: return CM_TON_UNKNOWN;
    case GSTK_INTERNATIONAL_NUM: return CM_TON_INTERNATIONAL;
    case GSTK_NATIONAL_NUM: return CM_TON_NATIONAL;
    case GSTK_NETWORK_SPECIFIC_NUM: return CM_TON_NETWORK_SPECIFIC;
    case GSTK_RFU_TON: return CM_TON_RESERVED_5;
    default: return CM_TON_UNKNOWN;
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION UI_GSTK_TO_CM_NPI

DESCRIPTION
  GSTK has their own constants for the number plan indication.  This function will
  map a GSTK number plan to the CM number plan.

DEPENDENCIES
  None

RETURN VALUE
  CM Number Plan.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ui_gstk_to_cm_npi(gstk_npi_enum_type npi)
{
  switch (npi)
  {
    case GSTK_UNKNOWN_NPI: return CM_NPI_UNKNOWN;
    case GSTK_ISDN_TELEPHONY_NPI: return CM_NPI_ISDN;
    case GSTK_DATA_NPI: return CM_NPI_DATA;
    case GSTK_TELEX_NPI: return CM_NPI_TELEX;
    case GSTK_PRIVATE_NPI: return CM_NPI_PRIVATE;
    case GSTK_EXT_RESERVED_NPI: return CM_NPI_RESERVED_EXTENSION;
    case GSTK_RFU_NPI: return CM_NPI_UNKNOWN;
  }
  return CM_NPI_UNKNOWN;
}

/* <EJECT> */
/*===========================================================================
FUNCTION ui_envelope_command_cb

DESCRIPTION
  When the envelope command for call control is completed, this function
  is called by GSDI/GSTK to communicate the results.   The user_data should
  be the CC_REQUEST structure we were working with.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ui_envelope_command_cb(gstk_cmd_from_card_type *req_rsp)
{
  /* Hacks for now. */
  CC_REQUEST *request_ptr = (CC_REQUEST *) req_rsp->hdr_cmd.user_data;
  cm_orig_call_control_s_type *sim_cc_result_ptr = NULL;

  if (request_ptr->sim1_cc_required == CC_REQUIRED)
  {
    request_ptr->sim1_cc_required = CC_COMPLETED;
    sim_cc_result_ptr = &request_ptr->sim1_cc_result;
  }
#ifdef FEATURE_GSTK_DUAL_SLOTS
  else if (request_ptr->sim2_cc_required == CC_REQUIRED)
  {
    request_ptr->sim2_cc_required = CC_COMPLETED;
    sim_cc_result_ptr = &request_ptr->sim2_cc_result;
  }
#endif
  else
  {
    ERR("Unexpected callback.  No request pending.", 0, 0, 0);
  }

  /* If the data looks good, process the response. */
  if (sim_cc_result_ptr)
  {
    /* Save the results of the CC command in the structure. */
    if (req_rsp->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS)
    {
      /* Call control can say the call is allowed, but with a modification, */
      /* the call is allowed as is, or, the call is not allowed. */
      switch(req_rsp->cmd.cc_rsp.call_ctrl_result)
      {
        case GSTK_ALLOWED_BUT_MOD:
          /* They have given us a modification to the original command. */
          switch (req_rsp->cmd.cc_rsp.cc_req_action.call_type)
          {
            case GSTK_VOICE:
              /* Apply the origination if its still an origination. */
              sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE;
              sim_cc_result_ptr->num.len = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.length;
              (void) gstk_address_to_ascii(&req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address,
                                           sim_cc_result_ptr->num.buf, CM_MAX_NUMBER_CHARS);
              sim_cc_result_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
              sim_cc_result_ptr->num.number_type = ui_gstk_to_cm_ton(req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.TON);
              sim_cc_result_ptr->num.number_plan = ui_gstk_to_cm_npi(req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.NPI);
              sim_cc_result_ptr->num.pi = 0;
              sim_cc_result_ptr->num.si = 0;

              memcpy(sim_cc_result_ptr->alpha.buf,
                     req_rsp->cmd.cc_rsp.cc_req_action.alpha.text,
                     req_rsp->cmd.cc_rsp.cc_req_action.alpha.length);

              sim_cc_result_ptr->alpha.len = req_rsp->cmd.cc_rsp.cc_req_action.alpha.length;

              /* per spec, 112 is the only emergency number */
              if ((sim_cc_result_ptr->num.len == 3) &&
                (sim_cc_result_ptr->num.buf[0] == '1')  &&
                (sim_cc_result_ptr->num.buf[1] == '1')  &&
                (sim_cc_result_ptr->num.buf[2] == '2') ) {
                request_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
                sim_cc_result_ptr->num.emerg_srv_categ = 0;
#endif
              }

              break;
            case GSTK_SS:
              sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS;
              sim_cc_result_ptr->num.len = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string.length;
              /* the SS needs to be parsed, and sim_cc_result_ptr->sups_cmd needs to be filled in.  */
              /* This needs to work in ThinUI as well, so calling ISuppServ won't work here */
              (void) gstk_address_to_ascii(&req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string,
                                           sim_cc_result_ptr->num.buf, CM_MAX_NUMBER_CHARS);
              break;
            case GSTK_USSD:
              {
                int length = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.length;

                if (length > sizeof(sim_cc_result_ptr->num.buf))
                  length = sizeof(sim_cc_result_ptr->num.buf);

                sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD;
                sim_cc_result_ptr->num.len = length;
                sim_cc_result_ptr->sups_cmd = CM_SUPS_CMD_PROCESS_USS;
                memcpy(sim_cc_result_ptr->num.buf,
                       req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text,
                       length);
              }
              break;
#if defined (CM_API_GW_PS_CC) && defined (FEATURE_GPRS_CALLCONTROL)
            case GSTK_PDP_CONTEXT_ACT:
              {
                sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED;
                sim_cc_result_ptr->gw_ps_cc_params.length = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.pdp_context_param.length;

                memcpy(sim_cc_result_ptr->gw_ps_cc_params.pdp_act_mesg,
                       req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.pdp_context_param.pdp_act_param,
                       MIN(sizeof(sim_cc_result_ptr->gw_ps_cc_params.pdp_act_mesg),
                           sim_cc_result_ptr->gw_ps_cc_params.length));

                memcpy(sim_cc_result_ptr->alpha.buf,
                       req_rsp->cmd.cc_rsp.cc_req_action.alpha.text,
                       req_rsp->cmd.cc_rsp.cc_req_action.alpha.length);

                sim_cc_result_ptr->alpha.len = req_rsp->cmd.cc_rsp.cc_req_action.alpha.length;

              }
              break;
#endif
            default:
              ERR("Unexpected call type returned by GSTK %d", req_rsp->cmd.cc_rsp.cc_req_action.call_type, 0, 0);
              break;
          }
          break;
        case GSTK_NOT_ALLOWED:
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
          break;
        case GSTK_ALLOWED_NO_MOD:
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
          break;
        default:
          /* We got an error from GSTK.  Should we allow the call or not? */
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
          ERR("Unexected response from GSTK. %d",
              req_rsp->cmd.cc_rsp.general_resp, 0, 0);
          break;
      }
    }
    else
    {
      /* We got an error from GSTK.  Should we allow the call or not? */
      ERR("GSTK returned an error. %d", req_rsp->cmd.cc_rsp.general_resp, 0, 0);
      sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
  }


  /* Call Send CC command to send the next or report we are done. */
  ui_send_cc_command(request_ptr);
}

/*===========================================================================
FUNCTION ui_gsdi_acl_read_cb

DESCRIPTION
  This is the read callback for reading ACL.  This is used in call
  control for packet data calls.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ui_gsdi_acl_read_cb(gsdi_cnf_T* gsdi_cnf)
{
  CC_REQUEST *cc_request = NULL;
  boolean pass = FALSE;

  do
  {
    if (gsdi_cnf == NULL)
    {
      MSG_ERROR("Null pointer from GSDI read", 0, 0, 0);
      break;
    }

    cc_request = (CC_REQUEST*)gsdi_cnf->read_cnf.message_header.client_ref;

    if (cc_request == NULL)
    {
      MSG_ERROR("Null userdata pointer from GSDI read", 0, 0, 0);
      break;
    }

    if (gsdi_cnf->read_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      /* we got ACL file.  See if it's applicable */
      int sim_data_index = 0;
      int num_apns = 0;
      int apn_len;
      int gsdi_file_len;

      gsdi_file_len = gsdi_cnf->read_cnf.returned_data_len;

      if (gsdi_file_len < 1)
      {
        MSG_ERROR("ACL read failed, length %d", gsdi_file_len, 0, 0);
        break;
      }
      num_apns = gsdi_cnf->read_cnf.data[sim_data_index];
      sim_data_index++;

      if (num_apns == 0)
      {
        pass = TRUE;
        MSG_HIGH("APN file has 0 entries",0,0,0);
        break;
      }

      while ((num_apns > 0) && (sim_data_index < gsdi_file_len))
      {
        //read tag
        if (gsdi_cnf->read_cnf.data[sim_data_index] != 0xdd)
        {
          MSG_HIGH("APN file has invalid tag entry 0x%x at index %d",
                   gsdi_cnf->read_cnf.data[sim_data_index],sim_data_index,0);
          break;
        }
        sim_data_index++;

        //read len
        apn_len = gsdi_cnf->read_cnf.data[sim_data_index];
        if (apn_len + sim_data_index >= gsdi_file_len)
        {
          MSG_HIGH("APN file has invalid length 0x%x at index %d.  File len %d",
                   apn_len,sim_data_index,gsdi_file_len);
          break;
        }
        sim_data_index++;


        //value is now apn_len bytes, beginning at sim_data_index
        //apn matches if the lengths are equal, and the data is the same

#if defined(CM_API_GW_PS_CC) && (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
        //handle normal APNs
        if (cc_request->original_origination_request.gw_ps_cc_params.apn_name.valid &&
            (cc_request->original_origination_request.gw_ps_cc_params.apn_name.apn_addr_len == apn_len) &&
            (0 == memcmp(cc_request->original_origination_request.gw_ps_cc_params.apn_name.address,
                         &gsdi_cnf->read_cnf.data[sim_data_index],
                         apn_len)))
        {
          pass = TRUE;
          break;
        }

        //handle network provided APNs
        if (!cc_request->original_origination_request.gw_ps_cc_params.apn_name.valid &&
            (0 == apn_len))
        {
          pass = TRUE;
          break;
        }

        num_apns--;
        sim_data_index += apn_len;
#else
        //if we aren't getting the APN from CM, assume it's allowed.
        MSG_HIGH("ACL checking required, but not supported.  Allowing call",0,0,0);
        pass = TRUE;
        break;
#endif
      }
    }
    else
    { /* GSDI returned !success.  file not found */
      pass = TRUE;
    }

  } while ( 0 );

  if (pass)
  {
    /*pass to envelope command */
    ui_send_cc_command(cc_request);
  }
  else
  {
    /* fail call control */
    cc_request->original_origination_request.call_control_result =
      CM_CC_RESULT_NOT_ALLOWED;
    cc_request->gw_result_ptr   = &cc_request->original_origination_request;
    (*cc_request->cb_func)(cc_request->call_id,
                           CM_CC_STATUS_SUCCESS,
                           cc_request->call_type,
                           cc_request->cdma_result_ptr,
                           cc_request->gw_result_ptr,
                           (void*) cc_request->user_data);
    /* This sequence is complete, so free the memory. */
    mem_free(&tmc_heap, cc_request);
  }
}
#endif /* defined(FEATURE_GSTK)   */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION ui_gsdi_is_service_enabled

DESCRIPTION
 Looks at info received from GSDI to see if a given service is enabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ui_gsdi_is_service_enabled (CC_REQUEST *cc_request, gsdi_slot_id_type slot, gsdi_services_type service)
{
  int i;

  for (i=0; i<ARR_SIZE(card_slots); i++)
  {
    if (slot == card_slots[i])
    {
      if (cc_request->sim_capabilities_valid[i])
      {
        switch (service)
        {
          case GSDI_FDN:
            return cc_request->sim_capabilities[i].fdn_enabled;
          case GSDI_BDN:
            return cc_request->sim_capabilities[i].bdn_enabled;
          case GSDI_ACL:
            return cc_request->sim_capabilities[i].acl_enabled;
          default:
            ERR("Unknown service check",0,0,0);
            return FALSE;
        }
      }
    }
  }
  return FALSE;
}
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION ui_call_ctrl_after_sim_capabilities

DESCRIPTION
 Performs call control operations that depend on having SIM capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ui_call_ctrl_after_sim_capabilities(CC_REQUEST *cc_request)
{
  int i;
  boolean call_allowed_cdma = TRUE;     /* Is call still allowed on CDMA? */
  boolean call_allowed_gw = TRUE;       /* Is call still allowed on GW? */

#if defined(FEATURE_GSTK)
  boolean (*cc_required_fn)(gstk_slot_id_enum_type) = NULL;
#endif


  if (CM_CC_SUBSCRIPTION_SOURCE_NONE == cc_request->cdma_subscription_source)
  {
    call_allowed_cdma = FALSE;
    cc_request->cdma_result_ptr = NULL;
  }

  if (CM_CC_SUBSCRIPTION_SOURCE_NONE == cc_request->gw_subscription_source)
  {
    call_allowed_gw = FALSE;
    cc_request->gw_result_ptr = NULL;
  }

  /* Next, the SIM has required checks of the FDN.  If the FDN is
   * enabled, these must be checked.
   * Limit calls to SIM1 FDN
   * Limit calls to SIM2 FDN
   */

#if defined(FEATURE_MMGSDI) && defined(FEATURE_UI_PBM)
  for (i = 0; i < ARR_SIZE(sub_sources); i++)
  {
    if ((cc_request->cdma_subscription_source == sub_sources[i]) ||
        (cc_request->gw_subscription_source   == sub_sources[i]))
    {
      gsdi_slot_id_type slot;

      slot = card_slots[i];

      /* If FDN is enabled, make sure we check the FDN. */
      if (
#if defined(FEATURE_GSTK_FDN_CC_SUPPORT)
          (cc_request->cc_generic_params.is_fdn_to_check == TRUE) &&
#endif
          ui_gsdi_is_service_enabled(cc_request, slot, GSDI_FDN)
         )
      {
        if (cc_request->call_type == CM_CALL_TYPE_PS_DATA)
        {
#ifdef FEATURE_PACKET_CALL_FDN_CHECK
          if (PBM_SUCCESS != pbm_find_number(pbm_fdns[i], (const byte *)UI_PACKET_CALL_FDN_STR,
                                             strlen(UI_PACKET_CALL_FDN_STR), NULL, NULL))
          {
            if (cc_request->cdma_subscription_source == sub_sources[i])
            {
              call_allowed_cdma = FALSE;
              cc_request->cdma_result_ptr = NULL;
            }
            if (cc_request->gw_subscription_source == sub_sources[i])
            {
              call_allowed_gw = FALSE;
              cc_request->gw_result_ptr = NULL;
            }
          } /* pbm_find_number */
#endif /* FEATURE_PACKET_CALL_FDN_CHECK */
        } /* ps data */
        else
        {
          /* voice or CS data */
          if (PBM_SUCCESS != pbm_find_number(pbm_fdns[i],
                                             cc_request->original_origination_request.num.buf,
                                             cc_request->original_origination_request.num.len,
                                             NULL, NULL))
          {
            if (cc_request->cdma_subscription_source == sub_sources[i])
            {
              call_allowed_cdma = FALSE;
              cc_request->cdma_result_ptr = NULL;
            }
            if (cc_request->gw_subscription_source == sub_sources[i])
            {
              call_allowed_gw = FALSE;
              cc_request->gw_result_ptr = NULL;
            }
          } /* pbm_find_number() */
        } /* voice or CS data */

      } /* ui_restrict_dialling_to() */
    } /* source == sub_sources[i] */
  } /* for (sub_sources) */
  /* Now the call has passed the UIM required checks for being allowed,  */
#endif /* defined(FEATURE_MMGSDI) && defined(FEATURE_UI_PBM) */

#ifdef FEATURE_UI_PBM
  /* Now check for the restrictions enforced by the UI itself.
   * Specifically:
   * Limit calls to numbers in the NV phone book(s)
   * Limit calls to SIM1 ADN
   * Limit calls to SIM2 ADN
   * If any of these limitations are set and the number does not match these
   * numbers, Return the call is not allowed on any subscription source.
   */
  if (call_allowed_cdma || call_allowed_gw)
  {
    boolean passes_optional_checks = FALSE;
    boolean has_optional_restriction = FALSE;
    pbm_device_type optional_list[] = {PBM_EFS, PBM_SDL, PBM_ADN, PBM_ADN2};

    for (i = 0; (!passes_optional_checks && (i < ARR_SIZE(optional_list))); i++)
    {
      if (ui_restrict_dialing_to(optional_list[i]))
      {
        has_optional_restriction = TRUE;
        if (PBM_SUCCESS == pbm_find_number(optional_list[i],
                                           cc_request->original_origination_request.num.buf,
                                           cc_request->original_origination_request.num.len,
                                           NULL, NULL) )
        {
          passes_optional_checks = TRUE;
          break;
        }
      }
    }

    if (has_optional_restriction && !passes_optional_checks)
    {
      call_allowed_cdma = call_allowed_gw = FALSE;
      cc_request->cdma_result_ptr = NULL;
      cc_request->gw_result_ptr = NULL;
    }
  } //  if (call_allowed_cdma || cal...
#endif /* defined(FEATURE_UI_PBM) */

  if (!call_allowed_cdma && !call_allowed_gw)
  {
    /* The call is not allowed on either system, so say no to both. */
    cc_request->original_origination_request.call_control_result =
      CM_CC_RESULT_NOT_ALLOWED;
    (*(cc_request->cb_func))(cc_request->call_id,
                             CM_CC_STATUS_SUCCESS,
                             cc_request->call_type,
                             cc_request->cdma_result_ptr,
                             cc_request->gw_result_ptr,
                             (void*) cc_request->user_data);
    mem_free(&tmc_heap, cc_request);
    return;
  }

/* build the envelope command info */
#if defined(FEATURE_GSTK)
  if (cc_request->call_type == CM_CALL_TYPE_PS_DATA)
  {
#ifdef FEATURE_GPRS_CALLCONTROL
    cc_required_fn = gstk_is_gprs_cc_required;
#else
    cc_required_fn = NULL;
#endif
  }
  else
  {
    cc_required_fn = gstk_slot_is_cc_required;
  }

  /* If either subscription comes from SIM1, */
  if ((call_allowed_cdma && (cc_request->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)) ||
      (call_allowed_gw && (cc_request->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)))
  {
    memcpy(&cc_request->sim1_cc_result,
           &cc_request->original_origination_request,
           sizeof(cm_orig_call_control_s_type));

    if (call_allowed_gw && (cc_request->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1) )
      cc_request->gw_result_ptr = &cc_request->sim1_cc_result;
    if (call_allowed_cdma && (cc_request->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1) )
      cc_request->cdma_result_ptr = &cc_request->sim1_cc_result;

    /* Check if CC is required from SIM1.
     * If so, set a variable to remember that CC is required
     * for those subscriptions.
     */
    if ((cc_required_fn != NULL) && cc_required_fn(GSTK_SLOT_1))
    {
      cc_request->sim1_cc_required = CC_REQUIRED;
    }
  }
#if defined (FEATURE_GSTK_DUAL_SLOTS)
  /* If either subscription comes from SIM2, */
  if ((call_allowed_cdma && (cc_request->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)) ||
      (call_allowed_gw   && (cc_request->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)))
  {
    memcpy(&cc_request->sim2_cc_result,
           &cc_request->original_origination_request,
           sizeof(cm_orig_call_control_s_type));
    if (call_allowed_gw   && (cc_request->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))
      cc_request->gw_result_ptr = &cc_request->sim2_cc_result;
    if (call_allowed_cdma && (cc_request->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))
      cc_request->cdma_result_ptr = &cc_request->sim2_cc_result;

    /* Check if CC is required from SIM2.
     * If so, set a variable to remember that CC is required
     * for those subscriptions.
     */
    if ((cc_required_fn != NULL) && cc_required_fn(GSTK_SLOT_2))
    {
      cc_request->sim2_cc_required = CC_REQUIRED;
    }
  }
#endif /* defined(FEATURE_GSTK_DUAL_SLOTS) */

  if (cc_request->call_type == CM_CALL_TYPE_PS_DATA)
  {
    /* read ACL from GW subscription source if applicable */
    gsdi_slot_id_type slot;
    slot = (cc_request->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)?GSDI_SLOT_1:GSDI_SLOT_2;

    if (ui_gsdi_is_service_enabled(cc_request, slot, GSDI_ACL))
    {
      gsdi_returns_T gsdi_retval;

      if (cc_request->gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
      {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        gsdi_retval = gsdi2_sim_read(
          slot,
          UIM_USIM_ACL,
          GSDI_NO_INDEX,     /* index_mode */
          0,
          0,
          0,                 /* Data offset. */
          NULL,              /* GSDI Application ID */
          (uint32)cc_request,
          ui_gsdi_acl_read_cb
        );
#else
        gsdi_retval = gsdi_sim_read (
          UIM_USIM_ACL,
          GSDI_NO_INDEX,     /* index_mode */
          0,
          NULL,
          0,
          NULL,
          0,
          0,                 /* Data offset. */
          NULL,
          (uint32)cc_request,
          ui_gsdi_acl_read_cb
        );
#endif //FEATURE_MMGSDI_DUAL_SLOT

        if (gsdi_retval != GSDI_SUCCESS)
        {
          MSG_HIGH("GSDI_SIM_WRITE returned %d", gsdi_retval, 0, 0);
          /* can't read ACL.  May not exist.  Continue with envelope command */
          ui_send_cc_command(cc_request);
        }
      }
      else
      {
        /*no GS sub source, so just call envelope command */
        ui_send_cc_command(cc_request);
      }
    }
    else
    {
      /* If ACL is not applicable, still need to send CC command */
      ui_send_cc_command(cc_request);
    }
  }
  else
  {
    /* Call the function that sends CC requests based on the above structure. */
    ui_send_cc_command(cc_request);
  }
#else
  /* Call the function that sends CC requests based on the above structure. */
  ui_send_cc_command(cc_request);
#endif /* defined(FEATURE_GSTK)  */
}

#if defined (FEATURE_MMGSDI)
/*===========================================================================
FUNCTION ui_sim_cap_cb

DESCRIPTION
 Callback for gsdi_get_sim_capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ui_sim_cap_cb(gsdi_cnf_T* gsdi_cnf)
{
  gsdi_slot_id_type slot;
  CC_REQUEST *cc_request;
  int i;

  if (gsdi_cnf == NULL)
  {
    MSG_ERROR("Null pointer from GSDI sim capabilities", 0, 0, 0);
    return;
  }

  slot = gsdi_cnf->get_sim_cap_cnf.message_header.slot;
  cc_request = (CC_REQUEST*)gsdi_cnf->get_sim_cap_cnf.message_header.client_ref;

  for (i=0; i<ARR_SIZE(card_slots); i++)
  {
    if (slot == card_slots[i])
    {
      cc_request->sim_capabilities[i] = gsdi_cnf->get_sim_cap_cnf.sim_capabilities;
      cc_request->sim_capabilities_valid[i] = TRUE;
      break;
    }
  }

  /* Make sure that we got something valid before trying again */
  if (i != ARR_SIZE(card_slots))
  {
    for (i=0; i<ARR_SIZE(cc_request->sim_capabilities_valid); i++)
    {
      if (cc_request->sim_capabilities_valid[i] == FALSE)
      {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        gsdi2_get_sim_capabilities(card_slot[i],
                                   &cc_request->sim_capabilities[i],
                                   (uint32)cc_request,
                                   ui_sim_cap_cb);
#else
        gsdi_get_sim_capabilities(&cc_request->sim_capabilities[i],
                                  (uint32)cc_request,
                                  ui_sim_cap_cb);
#endif
        return;
      }
    }
  }
  ui_call_ctrl_after_sim_capabilities(cc_request);
}
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION ui_populate_sim_capabilities

DESCRIPTION
 Initiates the process of getting SIM capabilities.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ui_populate_sim_capabilities(CC_REQUEST *cc_request)
{
#ifdef FEATURE_MMGSDI
  int i;

  for (i=0; i<ARR_SIZE(cc_request->sim_capabilities_valid); i++)
  {
    cc_request->sim_capabilities_valid[i] = FALSE;
  }

  #ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi2_get_sim_capabilities(card_slot[0],
                               &cc_request->sim_capabilities[i],
                               (uint32)cc_request,
                               ui_sim_cap_cb);
  #else
    gsdi_get_sim_capabilities(&cc_request->sim_capabilities[i],
                              (uint32)cc_request,
                              ui_sim_cap_cb);
  #endif
#else
  //no MMGSDI, so just carry on
  ui_call_ctrl_after_sim_capabilities(cc_request);
#endif /* FEATURE_MMGSDI */

}

/* <EJECT> */
/*===========================================================================
FUNCTION ui_nc_type_allowed_and_callcontrol_cb

DESCRIPTION
  The new NC function that checks all information about a number, including call
  control will have the following design.

  First, check if its an emergency call.
   If Emergency, return that the call is an emergency number, and CC not required.

  Second, the UI has non-required by specification user limits, specifically:
    Limit calls to numbers in the NV phone book(s)
    Limit calls to SIM1 ADN
    Limit calls to SIM2 ADN
  If any of these limitations are set and the number does not match these numbers,
    Return the call is not allowed on either SIM.

  Third if the call is still allowed
   If either subscription uses SIM1:
     Do FDN check on SIM1
       If the call is not allowed on SIM1, mark the call as not allowed for modes whose subscription comes from SIM1.

   If either subscription uses SIM2
      Do FDN check on SIM2
         If the call is not allowed on SIM2, mark the call as not allowed for modes whose subscription comes from SIM2.

  If the call is not allowed now on either mode,
    Return the call is not allowed on either SIM.

 Fourth Call the following do_cc function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ui_nc_type_allowed_and_callcontrol_cb (
  cm_call_id_type                       call_id, /* call id */
  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */
  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */
  cm_call_type_e_type                   call_type,
    /* call type */
  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */
  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */
  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func
    /* function to be called when processing is complete */
#ifdef CM_API_GW_PS_CC
  ,
  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
    /* Call control parameters for Packet data call
    */
#endif

#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
  ,
  cm_cc_generic_params_s_type           *cc_generic_params
    /* Call control generic parameters for each call
    */
#endif
)
{
  CC_REQUEST *cc_request = NULL;        /* Call control request structure. */
  uint8 ecc_category;                   /* emergency call category */

  /* Check incoming parameters. */
  if ((orig_num_ptr == NULL) ||
      (cm_call_control_complete_cb_func == NULL)
#if defined (CM_API_GW_PS_CC)
      || ((call_type == CM_CALL_TYPE_PS_DATA) && (gw_ps_cc_params == NULL))
#endif
     )
  {
#ifdef CM_API_GW_PS_CC
    ERR("Bad parameter from CM %x %x %x",
        orig_num_ptr,
        cm_call_control_complete_cb_func,
        gw_ps_cc_params);
#else
    ERR("Bad parameter from CM %x %x %x",
        orig_num_ptr,
        cm_call_control_complete_cb_func,
        0);
#endif
    return FALSE;
  }

  /* This memory is freed after we issue the callback to CM. */
  if ((cc_request = (CC_REQUEST *)mem_malloc(&(tmc_heap),
                                             sizeof(CC_REQUEST))) == NULL)
  {
    ERR("Could not allocate response structure.", 0, 0, 0);
    return FALSE;
  }
  memset(cc_request, 0, sizeof(CC_REQUEST));
  cc_request->sim1_cc_required = CC_NOT_REQUIRED;
  cc_request->sim2_cc_required = CC_NOT_REQUIRED;
  cc_request->call_id = call_id;
  cc_request->user_data = user_data;
  cc_request->cb_func = cm_call_control_complete_cb_func;
  cc_request->status = CM_CC_STATUS_SUCCESS;
  cc_request->call_type = call_type;
  cc_request->cdma_subscription_source = cdma_subscription_source;
  cc_request->gw_subscription_source = gw_subscription_source;

#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
 (void) MEMCPY (&(cc_request->cc_generic_params), cc_generic_params,
		       sizeof (cm_cc_generic_params_s_type));
#endif

#ifdef CM_API_GW_PS_CC
  memset(&(cc_request->original_origination_request.gw_ps_cc_params),
         0,
         sizeof(cc_request->original_origination_request.gw_ps_cc_params));
  if (call_type == CM_CALL_TYPE_PS_DATA) {
    cc_request->original_origination_request.gw_ps_cc_params = *gw_ps_cc_params;
  }
#endif
  cc_request->cdma_result_ptr = &cc_request->original_origination_request;
  cc_request->gw_result_ptr   = &cc_request->original_origination_request;
  cc_request->original_origination_request.call_control_result =
                                                CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

  cc_request->original_origination_request.alpha.len = 0;
  memset(cc_request->original_origination_request.alpha.buf,
      0, sizeof(cc_request->original_origination_request.alpha.buf));
  cc_request->original_origination_request.num = *orig_num_ptr;

  /* Get the call type.  */
  get_call_type(orig_num_ptr, &(cc_request->call_type),
                &(cc_request->original_origination_request.activate_code),
                &ecc_category);

  /* The following check will ensure that we allow emergency calls
   * without further checking.  Also, if it is OTASP, or any other
   * call that typically does not have a number,
   * we will tell CM to allow this call.
   */
  if ((cc_request->call_type != CM_CALL_TYPE_VOICE) &&
      (cc_request->call_type != CM_CALL_TYPE_CS_DATA) &&
      (cc_request->call_type != CM_CALL_TYPE_PS_DATA))
  {
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
    cc_request->gw_result_ptr->num.emerg_srv_categ = ecc_category;
#endif
    MSG_HIGH("skipping further call control. type: %d, cat %d",
             cc_request->call_type, ecc_category, 0);
    (*cm_call_control_complete_cb_func)(call_id,
                                      CM_CC_STATUS_SUCCESS,
                                      cc_request->call_type,
                                      cc_request->cdma_result_ptr,
                                      cc_request->gw_result_ptr,
                                      (void*) user_data);
    mem_free(&tmc_heap, cc_request);
    return TRUE;
  }

  /* populate sim capabilities, required by rest of call control */
  ui_populate_sim_capabilities(cc_request);

  return TRUE;
} //void start_call_control()...


/* <EJECT> */
/*===========================================================================
FUNCTION ui_nc_call_control_preferences

DESCRIPTION
 Type for call control preferences function to be registered by a client.  CM will
 call this function after call control is complete to mediate between which modes
 to use in the case where the call is allowed on both modes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
cm_cc_mode_pref_e_type ui_nc_call_control_preferences (

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */

)
{
 return CM_CC_MODE_PREF_ANY;
}
#endif /* !defined(FEATURE_UI_DUALPROC_APPS) */

