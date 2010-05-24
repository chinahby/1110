/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S   M E S S A G I N G   S E R V I C E S
           -- SIM Interface Module

DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the SIM Interface functions.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 
2008 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmssim.c_v   1.9   26 Jun 2002 16:00:12   suberoy  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmssim.c#4 $ $DateTime: 2010/02/23 11:51:47 $ $Author: rajang $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   PMD     Removed Tabs from code
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
05/15/08   PMD     Fixed mmgsdi_client_init_complete() for slots
02/07/08   PMD     Added support for MMGSDI Init
01/31/08   PMD     Updated Copyright Information to include 2008
01/14/07   PMD     Added support for MO SMS FDN Control 
02/05/07   PMD     Added support for GSDI_REFRESH_APP_RESET and 
                   GSDI_REFRESH_3G_SESSION_RESET events from GSDI
01/17/07   HQ      Updated copyright info to include 2007.
12/13/06   NP      Initialize cfg_s_ptr during wms_sim_gsdi_init() so it is
                   valid, rather than wait till later on which was causing
                   a crash since this was junk.
12/06/06   PMD     Added Low Memory Feature for certain Compile Issues
12/01/06   PMD     Added support for Entire SIM Refresh
                   Updated SMSR flags for rescan on SIM Refresh
11/06/06   PMD     Eliminated FEATURE_SMS_LTK from Code
08/30/06   PMD     Fixed GSDI Card Apps Enabled Masks for Slot 2 as well
08/28/06   PMD     Fixed GSDI Card Apps Enabled Masks 
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
07/31/06   PMD     Removed unused feature - SMS_TREX
05/23/06   HQ      Fixed compile error for RUIM builds.
05/19/06   PMD     Fixed another Compile Warning
05/19/06   PMD     Added FEATURE_IWMSDIAG to fix compile warning
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/24/06   HQ      Fixed lint errors.
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/08/05   VK      Updated Copyright info. 
01/26/05   PMD     Added support for initializing CB before reading other SIM Files.
01/07/05   PMD     Added Feature FEATURE_SMS_LTK
08/30/04   PMD     Added CBMID Support
06/30/04   PMD     Added Debug Statements for CBMI, CBMIR and CBMID Initialization
                   Modified CBMID Initialization
06/22/04   PMD     Fix for WMS Dog Timeout
02/14/04   PMD     Initialized CBMI, CBMIR Record Lengths
01/27/04   HQ      Added support for dual-slot initialization.
12/15/03   PMD     Added Ix Broadcast Support in Multimode Broadcast API
12/02/03   HQ      Resolved some Lint errors.
08/04/03   PMD     Changes to suppress / remove Lint Errors
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/22/03   PMD     Fixed Status Report Deletion
06/27/03   PMD     Added Support for GSDI Refresh
06/13/03   PMD     Added some new debug (f3) messages
06/04/03   HQ      Check sim_file_attributes.rec_len==0 before the divisions.
05/30/03   PMD     Support for gsdi_sim_read API change
05/20/03   SUN     Check for in_progress flag before doing sim_init
04/24/03   SUN     Check the validity of the status report before saving it
04/11/03   SUN     SIM SMSR Initialization flag should be set if SMSR is not enabled
02/07/03   SUN     Added Support for Status Reports and Commands
                   Added Support for FEATURE_GWSMS_CACHE
                   Mainlined  FEATURE_GWSMS_ASYNC_INIT
01/30/03   SUN     Max Templates should be limited to WMS_MAX_TEMPLATES
01/29/03   HQ      Added check of gw_init_complete in wms_sim_init().
11/14/02   HQ      Resolved warnings for GW targets.
10/11/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
09/13/02   HQ      MM GSDI changes.
06/26/02   SUN     Template could not be deleted - Fixed
06/10/02   HQ      Resolved watch dog timeout problem.
06/03/02   SUN     Removed SMMA in sim_init.
                   Added wrapper function for reading smss
05/16/02   SUN     Fixed Template Read/Write Error
05/14/02   SUN     Fixed SMMA Issue
03/12/02   HQ      Added support of Multimode WMS.
02/11/02   HQ      Kick the watch dog in the loops during init.
02/07/02   HQ      Send CFG_EVENT_READY when SIM init is done.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/19/01   HQ      Added support of USIM SMS files.
10/30/01   HQ      Initial version for WCDMA & GSM SMS.

===========================================================================*/
#include "comdef.h"
#include "wmssim.h"
#include "wmscfg.h"
#include "wmsbc.h"
#include "msg.h"

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_CDSMS_RUIM
#include "wmsruim.h"
#endif /* FEATURE_CDSMS_RUIM */

#if (defined(FEATURE_GWSMS) || defined(FEATURE_MMGSDI))
static wms_cfg_s_type         * cfg_s_ptr;
#endif

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION WMS_SIM_MMGSDI_STATUS_CB

DESCRIPTION
  Callback function from MMGSDI to log errors

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void wms_sim_mmgsdi_status_cb (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    data_type,
                                      const mmgsdi_cnf_type  *data_ptr)
{
  if (MMGSDI_SUCCESS == status)
  {
    if (MMGSDI_CLIENT_ID_REG_CNF == data_type)
    {
      if (NULL != data_ptr)
      {
        MSG_MED("Received MMGSDI Client ID Reg Success",0,0,0);
        cfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_INIT_COMPLETE;
        cfg_s_ptr->wms_sim_mmgsdi_client = 
           data_ptr->client_id_reg_cnf.response_header.client_id;
        
      }
    }
    else if (MMGSDI_CLIENT_INIT_COMP_CNF == data_type)
    {
      MSG_MED("Received MMGSDI Client Init Complete Success",0,0,0);
    }
  }
  else
  {
    switch(data_type)
    {
      case MMGSDI_CLIENT_ID_REG_CNF:
        MSG_HIGH("MMGSDI failed registering WMS client ID.  Status %d, Confirmation %d", 
                 status, data_type, 0);
        break;

      case MMGSDI_CLIENT_INIT_COMP_CNF:
        MSG_HIGH("MMGSDI failed WMS Complete Init Status %d, Confirmation %d", 
                 status, data_type, 0);
        break;

      default:
        MSG_HIGH("MMGSDI returned !SUCCESS async.  Status %d, Confirmation %d", 
                 status, data_type, 0);
        break;
    }
  }
}
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION WMS_SIM_GSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_sim_gsdi_card_status_cb (gsdi_sim_events_T   gsdi_event)
{
  /* Send the command to WMS task */
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (gsdi_event)
  {
    case GSDI_SIM_INIT_COMPLETED:
    case GSDI_SIM_INIT_COMPLETED_2:
    /* FDN Related Events */
    case GSDI_FDN_ENABLE:
    case GSDI_FDN_DISABLE:
    case GSDI_FDN_ENABLE_2:
    case GSDI_FDN_DISABLE_2:
    /* Added SIM Refresh Commands as well for WMS Re-Initialization */
#ifndef FEATURE_LOW_MEMORY_USAGE
    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_SIM_INIT:
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_REFRESH_SIM_INIT_2:
    case GSDI_REFRESH_SIM_INIT_FCN:
    case GSDI_REFRESH_SIM_INIT_FCN_2:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
#endif /* !FEATURE_LOW_MEMORY_USAGE */
      cmd_ptr = wms_get_cmd_buf();

      if( cmd_ptr != NULL)
      {
        cmd_ptr->hdr.cmd_id      = WMS_CMD_GSDI_CARD_STATUS_CB;
        cmd_ptr->hdr.client_id   = WMS_CLIENT_TYPE_INTERNAL;
        cmd_ptr->hdr.cmd_cb      = NULL;
        cmd_ptr->hdr.user_data   = NULL;

        cmd_ptr->cmd.gsdi_card_status_cb.gsdi_event = gsdi_event;

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        MSG_ERROR("Out of cmd buf for card status cb",0,0,0);
      }
      break;

    default:
      break;
  }
  return;
}

/*===========================================================================
FUNCTION WMS_SIM_GSDI_CARD_STATUS_CB_PROC

DESCRIPTION
  Card Status Processing Function in WMS.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_sim_gsdi_card_status_cb_proc (wms_cmd_type   * cmd_ptr)
{
  gsdi_card_apps_T card_apps;

  cfg_s_ptr = wms_cfg_s_ptr();

  switch (cmd_ptr->cmd.gsdi_card_status_cb.gsdi_event)
  {
    case GSDI_SIM_INIT_COMPLETED:
    {
      gsdi_get_apps_available(&card_apps);

      if (card_apps.apps_enabled & GSDI_RUIM_APP_MASK)
      {
  #ifdef FEATURE_CDSMS
        /* Initialize 1x SMS */
        if (cfg_s_ptr->cdma_init_complete == FALSE)
        {
  #ifdef FEATURE_CDSMS_RUIM
          wms_ruim_init();
  #else /* FEATURE_CDSMS_RUIM */
          wms_cfg_do_cdma_ready_event();
  #endif /* FEATURE_CDSMS_RUIM */

  #ifdef FEATURE_CDSMS_BROADCAST
          wms_bc_online_proc();
  #endif /* FEATURE_CDSMS_BROADCAST */
        }
  #endif /* FEATURE_CDSMS */
      }
      else if ((card_apps.apps_enabled & GSDI_GSM_SIM_MASK)
              ||(card_apps.apps_enabled & GSDI_USIM_APP_MASK))
      {
  #ifdef FEATURE_GWSMS
#error code not present
  #endif /* FEATURE_GWSMS */
      }
      else
      {
        MSG_HIGH("Error in gsdi_get_apps_available!",0,0,0);
      }
      break;
    }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SIM_INIT_COMPLETED_2:
    {

      gsdi_get_apps_available(&card_apps);

      if (card_apps.apps2_enabled & GSDI_RUIM_APP_MASK)
      {
  #ifdef FEATURE_CDSMS
        /* Initialize 1x SMS */
        if (cfg_s_ptr->cdma_init_complete == FALSE)
        {
  #ifdef FEATURE_CDSMS_RUIM
          wms_ruim_init();
  #else /* FEATURE_CDSMS_RUIM */
          wms_cfg_do_cdma_ready_event();
  #endif /* FEATURE_CDSMS_RUIM */

  #ifdef FEATURE_CDSMS_BROADCAST
          wms_bc_online_proc();
  #endif /* FEATURE_CDSMS_BROADCAST */
        }
  #endif /* FEATURE_CDSMS */
      }
      else if ((card_apps.apps_enabled & GSDI_GSM_SIM_MASK)
              ||(card_apps.apps_enabled & GSDI_USIM_APP_MASK))
      {
  #ifdef FEATURE_GWSMS
#error code not present
  #endif /* FEATURE_GWSMS */
      }
      else
      {
        MSG_HIGH("Error in gsdi_get_apps_available!",0,0,0);
      }
      break;
    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    /* MO SMS FDN Event Handling */
    case GSDI_FDN_ENABLE:
      cfg_s_ptr->is_fdn_enabled_slot_1 = TRUE;
      break;

    case GSDI_FDN_DISABLE:
      cfg_s_ptr->is_fdn_enabled_slot_1 = FALSE;
      break;

    case GSDI_FDN_ENABLE_2:
      cfg_s_ptr->is_fdn_enabled_slot_2 = TRUE;
      break;

    case GSDI_FDN_DISABLE_2:
      cfg_s_ptr->is_fdn_enabled_slot_2 = FALSE;
      break;

    default:
      MSG_HIGH("Unhandled GSDI Event in wms_sim_gsdi_card_status_cb_proc!",0,0,0);
      break;

  } /* End switch (cmd_ptr->cmd.gsdi_card_status_cb.gsdi_event) */
  return;
}

/*===========================================================================
FUNCTION WMS_SIM_GSDI_INIT

DESCRIPTION
  Initialize GSDI and register a callback to get SIM card status.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_sim_gsdi_init(void)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  boolean notify_init = FALSE;

  /* Initialize cfg_s_ptr */
  cfg_s_ptr = wms_cfg_s_ptr();

  /* Register the GSDI event callback function */
  gsdi_status = gsdi_reg_callback ( wms_sim_gsdi_card_status_cb );

  /* This should not happen, raise an error */
  if (gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Could not register callback function with GSDI", 0, 0, 0 );
  }
  
  /* Initialize mmgsdi to default values */
  cfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_NOT_INIT;

  {
    mmgsdi_option_type mmgsdi_option;
    mmgsdi_option.tlv_num = 0;
    mmgsdi_option.tlvs_ptr = NULL;
#ifdef FEATURE_GWSMS
#error code not present
#endif     
    mmgsdi_status = mmgsdi_client_id_reg_set_notify (notify_init, mmgsdi_option, 
                                                     wms_sim_mmgsdi_status_cb, 0);
    if (MMGSDI_SUCCESS != mmgsdi_status)
    {
      MSG_ERROR("mmgsdi_client_id_reg failed %d", mmgsdi_status, 0, 0);
    }
  }

  /* Set MMGSDI Client Init State to In Progress */
  cfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_INIT_IN_PROGRESS;
}
#endif /* FEATURE_MMGSDI */
