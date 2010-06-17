/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S   M E S S A G I N G   S E R V I C E S
           -- RUIM Interface Module

DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the RUIM Interface functions.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 
2008 by QUALCOMM, Incorporated.  All Rights Reserved.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsruim.c_v   1.4   10 Jul 2002 20:18:38   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsruim.c#4 $ $DateTime: 2010/02/23 11:51:47 $ $Author: rajang $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/18/09   HN      Mainlined FEATURE_OMH_SMS
02/02/09   SJ      Set an upper bound for cfg_s_ptr->ruim_max_templates and 
                   cfg_s_ptr->ruim_max_slots to WMS_MAX_TEMPLATES and 
                   WMS_MESSAGE_LIST_MAX respectively
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
04/04/08   PMD     Feature-ized OMH logic.
04/04/08   PMD     Added support for BC-SMS and SMSCAP EFs in RUIM.
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
11/15/06   PMD     Changed wms_ts_decode_CDMA_bd function call
11/06/06   PMD     Eliminated FEATURE_SMS_LTK from Code
05/19/06   PMD     Fixed compile warning
05/03/06   PMD     Modified wms_ts_decode_CDMA_bd prototype
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
10/08/05   VK      Updated Copyright info. 
01/07/05   PMD     Added Feature FEATURE_SMS_LTK
03/08/04   HQ      Use UIM_SLOT_AUTOMATIC when accessing UIM.
02/06/04   HQ      Moved #include comdef.h outside of feature flag.
02/04/04   PMD     Cleaned up Lint Warnings in code
08/04/03   PMD     Changes to suppress / remove Lint Errors
07/01/03   PMD     Added GSDI Refresh Handling Support
06/13/03   PMD     Added watch dog kick during synchronous ruim access
                   Added some new debug (f3) messages
05/20/03   AP      When initializing, first check the tag before trying to
                   update the info cache.
04/17/03   AP      Added an additional read for a prl update before calling the
                   message info cache cb for FEATURE_UIM_TOOLKIT_UTK.
01/29/03   HQ      Added check of cdma_init_complete in wms_ruim_init().
11/14/02   HQ      Fixed compile problem; #include msg.h.
11/13/02   AP      Added support for duplicate detection.
08/26/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE.
07/10/02   HQ      Send WMS_RUIM_ACCESS_SIG to wms_tcb.
06/29/02   HQ      Changed func name to wms_cfg_do_cdma_ready_event();
                   made variables static.
06/22/02   HQ      Added processing of RUIM SMS init, sms, smsp and smss files.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/12/02   HQ      Initial version for support of Multimode WMS.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_CDSMS_RUIM
#include "wmsruim.h"
#include "wmspriv.h"
#include "wmscfg.h"
#include "wmstscdma.h"
#include "uim.h"
#include "string.h"
#include "task.h"
#include "msg.h"
#include "wmsbc.h"
#include "nvruimi.h"
#include "wmsnv.h"

#define WMS_RUIM_EF_BUF_SIZE        255
#ifdef CUST_EDITION
#include "Dstask.h"
#endif
static wms_cfg_s_type         *cfg_s_ptr;
static uint8                  ruim_data[WMS_RUIM_EF_BUF_SIZE];

static wms_OTA_message_type    cdma_ota;
static wms_tl_message_type     cdma_tl;
static wms_client_message_s_type     client_message;

/* Command buffer for each command. */
static uim_cmd_type    cmd;

/* Status reported by R-UIM for each command */
static uim_rpt_status wms_ruim_status_report = UIM_FAIL;
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
extern void wms_cacheinfo_deleteruimtemplatecache(wms_message_index_type index);
#endif
#endif // #ifdef CUST_EDITION
/*===========================================================================

FUNCTION wms_ruim_report

DESCRIPTION
  Accept status reports from R-UIM
  Set the global status and signal WMS task that status has arrived.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_ruim_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  wms_ruim_status_report = report->rpt_status;

  (void)rex_set_sigs( &wms_tcb, WMS_RUIM_ACCESS_SIG );

} /* sms_ruim_report() */


/*===========================================================================

FUNCTION wms_ruim_access

DESCRIPTION
  Access the R-UIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status wms_ruim_access
(
  uim_cmd_type *cmd_ptr       /* R-UIM command                               */
)
{
  rex_sigs_type sigs;
  cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F; /* "Access" an EF    */

  /* Indicate command completion is to be signaled:
   *    Do not signal the WMS Task
   *    Use no signal
   *    No "done" queue
   *    Status call-back function
   *    Always Report status
   */

  cmd_ptr->access_uim.hdr.cmd_hdr.task_ptr = NULL;
  cmd_ptr->access_uim.hdr.cmd_hdr.sigs = 0;
  cmd_ptr->access_uim.hdr.cmd_hdr.done_q_ptr = NULL;

  cmd_ptr->hdr.rpt_function = wms_ruim_report;
  cmd_ptr->hdr.protocol = UIM_CDMA;
  cmd_ptr->hdr.slot     = UIM_SLOT_AUTOMATIC;
  cmd_ptr->access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;

  /* Send the command to the R-UIM:
   *    Clear the "command done signal"
   *    Send the command
   *    Wait for the command to be done
   */

  (void) rex_clr_sigs( rex_self(), WMS_RUIM_ACCESS_SIG );

  uim_cmd( cmd_ptr );

  wms_kick_dog();  /* Kick the dog before rex_wait */

  while(1)
  {
    /* wait for DOG signal at the same time */
    sigs = rex_wait( WMS_RUIM_ACCESS_SIG | WMS_RPT_TIMER_SIG );

    /* Kick the dog during the wait */
    if( sigs & WMS_RPT_TIMER_SIG )
    {
      wms_kick_dog();
    }
    if( sigs & WMS_RUIM_ACCESS_SIG )
    {
      /* Done */
      (void) rex_clr_sigs( rex_self(), WMS_RUIM_ACCESS_SIG );
      break;
    }
    /* Otherwise continue to wait */
  }
  return(wms_ruim_status_report);

} /* wms_ruim_access */



/*
*/
void wms_ruim_init
(
  void
)
{
  uint8         i;
  boolean       has_sms = FALSE;
  boolean       has_smsp = FALSE;
  boolean       has_smscap = FALSE;
  boolean       has_bcsms = FALSE;
  boolean       has_3gpd = FALSE;

  uim_rpt_status status; /* status of ruim operation */


  cfg_s_ptr = wms_cfg_s_ptr();

  if( cfg_s_ptr->cdma_init_complete == TRUE )
  {
    MSG_HIGH("wms_ruim_init already done",0,0,0);
    return;
  }
#ifdef CUST_EDITION
  if (!IsRunAsUIMVersion())
  {// 当前软件运行于无卡版本
      wms_cfg_do_cdma_ready_event();
      return;
  }
#endif
  MSG_MED("In wms_ruim_init module",0,0,0);

  cfg_s_ptr->ruim_sms_file_name     = UIM_CDMA_SMS;
  cfg_s_ptr->ruim_smsp_file_name    = UIM_CDMA_SMS_PARAMS;
  cfg_s_ptr->ruim_smss_file_name    = UIM_CDMA_SMS_STATUS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read CDMA Service Table                                                  */

  cmd.access_uim.num_bytes = WMS_RUIM_SERVICE_TABLE_SIZE;
  cmd.access_uim.data_ptr  = ruim_data;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_CDMA_SVC_TABLE;
  cmd.access_uim.access    = UIM_READ_F;

  status = wms_ruim_access( &cmd );

  if (status == UIM_PASS)
  {
    has_sms  = WMS_RUIM_SERVICE_TABLE_HAS_SMS(ruim_data[0]);
    has_smsp = WMS_RUIM_SERVICE_TABLE_HAS_SMSP(ruim_data[2]);
    MSG_HIGH("RUIM SMS support: %d", has_sms,0,0);
    MSG_HIGH("RUIM SMSP support: %d", has_smsp,0,0);

    has_bcsms  = WMS_RUIM_SERVICE_TABLE_HAS_BCSMS(ruim_data[3]);
    has_3gpd = WMS_RUIM_SERVICE_TABLE_HAS_3GPD(ruim_data[3]);
    MSG_HIGH("RUIM BCSMS support: %d", has_bcsms,0,0);
    MSG_HIGH("RUIM 3GPD support: %d", has_3gpd,0,0);

    has_smscap = has_sms && has_3gpd;
    MSG_HIGH("RUIM SMSCAP support: %d", has_smscap,0,0);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* Inform NVRUIM about the card's capabilities for SMS and re-read the NV
  ** items that might come from the card.
  */
  nvruim_init_wms_svc_items( has_bcsms, has_smscap );
  if( TRUE == has_bcsms )
  {
#ifdef FEATURE_CDSMS_BROADCAST
    wms_bc_init();
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  if( TRUE == has_smscap )
  {
    wms_nv_init_nvruim();
  }


  /* EFsms related init
  */
  cfg_s_ptr->ruim_max_slots = 0;
  cfg_s_ptr->ruim_max_templates = 0;

  if( has_sms )
  {
    /* Determine the number of records and record length
    */
    /* Set up cmd item */
    cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
    cmd.access_uim.data_ptr  = ruim_data;
    cmd.access_uim.offset    = 1;

    cmd.access_uim.item      = cfg_s_ptr->ruim_sms_file_name;

    cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    cmd.access_uim.access    = UIM_READ_F;
    status = wms_ruim_access( &cmd );

    if( status == UIM_PASS )
    {
      cfg_s_ptr->ruim_max_slots   = MIN(cmd.access_uim.num_records_rsp, WMS_MESSAGE_LIST_MAX);
      cfg_s_ptr->ruim_sms_rec_len = cmd.access_uim.num_bytes_rsp;
    }
  }

  for( i = 0;
       i < cfg_s_ptr->ruim_max_slots;
       i ++ )
  {
    if( wms_ruim_read_sms( i, ruim_data ) == TRUE )
    {
      cfg_s_ptr->ruim_tags[i] = (wms_message_tag_e_type)ruim_data[0];

      if(cfg_s_ptr->ruim_tags[i] != WMS_TAG_NONE)
      {
#ifndef CUST_EDITION
        wms_cfg_update_msg_info_cache( WMS_TAG_NONE,
                                       WMS_MEMORY_STORE_RUIM,
                                       i,
                                       ruim_data,
                                       (uint8)cfg_s_ptr->ruim_sms_rec_len );
#else
        wms_cfg_update_msg_info_cache( cfg_s_ptr->ruim_tags[i],
                                       WMS_MEMORY_STORE_RUIM,
                                       i,
                                       ruim_data,
                                       (uint8)cfg_s_ptr->ruim_sms_rec_len );
#endif //#ifdef CUST_EDITION
        /* Update the duplicate detection cache if message is MT */
        if(WMS_IS_MT(cfg_s_ptr->ruim_tags[i]))
        {
          /* Do the proper decoding of the ruim data to update the
          ** duplicate detection cache
          */
          client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
          client_message.msg_hdr.tag          = (wms_message_tag_e_type)ruim_data[0];
          cdma_ota.format   = WMS_FORMAT_CDMA;
          cdma_ota.data_len = ruim_data[1];
          memcpy((uint8*)cdma_ota.data, ruim_data+2, cdma_ota.data_len);
          if(wms_ts_decode_CDMA_tl(&cdma_ota, &cdma_tl,
                                   &client_message.u.cdma_message.raw_ts) != WMS_OK_S)
          {
            MSG_ERROR("Decode tl FAILED, Dup list not updated!",0,0,0);
          }
          else if(wms_ts_decode_CDMA_bd(&client_message.u.cdma_message.raw_ts, FALSE,
                                        FALSE, &cdma_tl.cl_bd) != WMS_OK_S)
          {
            MSG_ERROR("Decode bd FAILED, Dup list not updated!",0,0,0);
          }
          else
          {
            /* Update the duplicate detection list */
            wms_cfg_update_dup_info_cache(WMS_MEMORY_STORE_RUIM,
                                          i,
                                          &cdma_tl);
          }
        }
      }
    }
  }


  /* EFsmsp related init
  */
  if( has_smsp )
  {
    /* Determine the number of records and record length
    */
    /* Set up cmd item */
    cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
    cmd.access_uim.data_ptr  = ruim_data;
    cmd.access_uim.offset    = 1;

    cmd.access_uim.item      = cfg_s_ptr->ruim_smsp_file_name;

    cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
    cmd.access_uim.access    = UIM_READ_F;
    status = wms_ruim_access( &cmd );

    if( status == UIM_PASS )
    {
      cfg_s_ptr->ruim_max_templates   = MIN(cmd.access_uim.num_records_rsp, WMS_MAX_TEMPLATES);
      cfg_s_ptr->ruim_smsp_rec_len    = cmd.access_uim.num_bytes_rsp;
    }
  }

  for( i = 0;
       i < cfg_s_ptr->ruim_max_templates;
       i ++ )
  {
    if( wms_ruim_read_smsp( i, ruim_data ) == TRUE )
    {
#ifdef FEATURE_RUIM_DEBUG_SMS_NOT_STRICTFF
#error code not present
#else
      if (ruim_data[0] != WMS_RUIM_SMSP_EMPTY_CHAR)
#endif
        cfg_s_ptr->ruim_templates[i] = TRUE;

      wms_cfg_update_msg_info_cache( WMS_TAG_MO_TEMPLATE,
                                     WMS_MEMORY_STORE_RUIM,
                                     i,
                                     ruim_data,
                                     (uint8)cfg_s_ptr->ruim_smsp_rec_len );
    }
  }


  /* EFsmss related init
  */
  /* Set up cmd item */
  cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
  cmd.access_uim.data_ptr  = ruim_data;
  cmd.access_uim.offset    = 0;

  cmd.access_uim.item      = cfg_s_ptr->ruim_smss_file_name;

  //cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.access    = UIM_READ_F;
  status = wms_ruim_access( &cmd );

  if( status == UIM_PASS )
  {
    cfg_s_ptr->ruim_smss_rec_len = cmd.access_uim.num_bytes_rsp;

    cfg_s_ptr->ruim_last_message_number = (ruim_data[0] << 8) | ruim_data[1];
    cfg_s_ptr->ruim_last_wap_message_number = (ruim_data[2] << 8 ) | ruim_data[3];
    cfg_s_ptr->ruim_mem_full_flag        = (wms_sim_mem_full_flag_e_type)ruim_data[4];
  }

#ifdef FEATURE_GSTK
  if (cfg_s_ptr->refresh_in_progress)
  {
    /* Since it is called for refresh
       Send Message List Event */
    wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);

    wms_cfg_do_template_list(WMS_MEMORY_STORE_RUIM);

    wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_MO_TEMPLATE);

    wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);

    cfg_s_ptr->cdma_init_complete = TRUE;

    MSG_MED("GSDI Refresh: Returning from wms_ruim_init",0,0,0);
  }
  else
#endif /* FEATURE_GSTK */
  {
    /* Notify the clients that RUIM is ready
    */
    wms_cfg_do_cdma_ready_event();

    MSG_MED("Returning from wms_ruim_init after cdma ready event",0,0,0);
  }
  
  /* done */
  return;

} /* wms_ruim_init() */

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
void wms_ruim_report_ext(uim_rpt_type *report)
{
  wms_ruim_status_report = report->rpt_status;

  (void)rex_set_sigs(&uim_tcb, UIMDATA_INIT_RPT_SIG);
} 

void wms_ruim_access_ext(uim_cmd_type *cmd_ptr)
{
    cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F; /* "Access" an EF    */

    /* Indicate command completion is to be signaled:
    *    Do not signal the WMS Task
    *    Use no signal
    *    No "done" queue
    *    Status call-back function
    *    Always Report status
    */
    cmd_ptr->access_uim.hdr.cmd_hdr.task_ptr = NULL;
    cmd_ptr->access_uim.hdr.cmd_hdr.sigs = 0;
    cmd_ptr->access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
    
    cmd_ptr->hdr.rpt_function = wms_ruim_report_ext;
    cmd_ptr->hdr.protocol = UIM_CDMA;
    cmd_ptr->hdr.slot     = UIM_SLOT_AUTOMATIC;
    cmd_ptr->access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
    
    /* Send the command to the R-UIM:
    */
    uim_cmd(cmd_ptr);
} /* wms_ruim_access_ext */

/*==============================================================================
函数:
    wms_ruim_init_stepbystep

说明:
    初始卡上短信的回调函数。

参数:
    none

返回值:
    TRUE : 初始?瓿?
    FALSE: 初始化未完成

备注:
    注意卡上短信可能不止开机时会初始化，若支持 UTK ，则 Refresh 命令会导致卡
    上短信的重新初始化。
        
==============================================================================*/
boolean wms_ruim_init_stepbystep(void)
{
    static uint8        i=0;
    static uint8        t=0;
    static boolean      has_sms = FALSE;
    static boolean      has_smsp = FALSE;
    static boolean      bInitStart = FALSE;
    static boolean      bWaitRpt = FALSE;
    static boolean      bSvcInited = FALSE;
    uim_rpt_status      status = UIM_FAIL; /* status of ruim operation */

    cfg_s_ptr = wms_cfg_s_ptr();
    
    if (cfg_s_ptr->cdma_init_complete == TRUE)
    {
        MSG_HIGH("wms_ruim_init already done",0,0,0);
        return TRUE;
    }
    
    if (!IsRunAsUIMVersion())
    {// 当前软件运行于无卡版本
        wms_cfg_do_cdma_ready_event();
        return TRUE;
    }
    if (bInitStart == FALSE)
    {
        MSG_ERROR("wms_ruim_init start", 0, 0, 0);
        bWaitRpt = FALSE;
        bInitStart = TRUE;
        bSvcInited = FALSE;
        i = 0;
        t = 0;
        
        /* EFsms related init
        */
        cfg_s_ptr->ruim_max_slots = 0;
        cfg_s_ptr->ruim_max_templates = 0;
        
#if !defined(FEATURE_WMS_APP)
        wms_cfg_do_cdma_ready_event();
        return TRUE;
#endif
    }
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    
    /* Read CDMA Service Table                                                  */
    if ((i == 0) && (bSvcInited == FALSE))
    {
        if (bWaitRpt == FALSE)
        {
            cmd.access_uim.num_bytes = WMS_RUIM_SERVICE_TABLE_SIZE;
            cmd.access_uim.data_ptr  = ruim_data;
            cmd.access_uim.offset    = 0;
            cmd.access_uim.item      = UIM_CDMA_CDMA_SVC_TABLE;
            cmd.access_uim.access    = UIM_READ_F;
        
            bWaitRpt = TRUE;
            
            wms_ruim_access_ext(&cmd);
            return FALSE;
        }
        else 
        {
            bWaitRpt = FALSE;
            bSvcInited = TRUE;
            if (wms_ruim_status_report == UIM_PASS)
            {
                has_sms  = WMS_RUIM_SERVICE_TABLE_HAS_SMS(ruim_data[0]);
                has_smsp = WMS_RUIM_SERVICE_TABLE_HAS_SMSP(ruim_data[2]);
                
                MSG_ERROR("has_sms =%d   has_smsp =%d", has_sms, has_smsp, 0);
            }
            
            return FALSE;
        }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    
    /* EFsms related init
    */
    if (has_sms)
    {
        MSG_ERROR("wms_ruim_init i=%d", i, 0, 0);
        
        status = UIM_FAIL;
        if (i==0)
        {
            if (bWaitRpt == FALSE)
            {
                cfg_s_ptr->ruim_max_slots = 0;
                cfg_s_ptr->ruim_sms_file_name = UIM_CDMA_SMS;
                
                /* Determine the number of records and record length
                */
                /* Set up cmd item */
                cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
                cmd.access_uim.data_ptr  = ruim_data;
                cmd.access_uim.offset    = 1;
                
                cmd.access_uim.item      = cfg_s_ptr->ruim_sms_file_name;
                
                cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
                cmd.access_uim.access    = UIM_READ_F;
                
                bWaitRpt = TRUE;
                
                wms_ruim_access_ext(&cmd);
                return FALSE;
            }
            else
            {
                bWaitRpt = FALSE;
                if (wms_ruim_status_report == UIM_PASS)
                {
                    cfg_s_ptr->ruim_max_slots   = cmd.access_uim.num_records_rsp;
                    cfg_s_ptr->ruim_sms_rec_len = cmd.access_uim.num_bytes_rsp;
                    MSG_ERROR("cfg_s_ptr->ruim_max_slots=%d", cfg_s_ptr->ruim_max_slots, 0, 0);
                    if (cfg_s_ptr->ruim_max_slots == 0)
                    {
                        i++;
                        return FALSE;
                    }
                    else
                    {
                        status = UIM_PASS;
                    }
                }
                else
                {
                    i++;
                    return FALSE;
                }
            }
        }
        else if (i < cfg_s_ptr->ruim_max_slots)
        {
            if (bWaitRpt == FALSE)
            {
                /* Set up cmd item */
                cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_sms_rec_len;
                cmd.access_uim.data_ptr  = ruim_data;
                cmd.access_uim.offset    = (uint16)(i + 1);
                
                cmd.access_uim.item      = cfg_s_ptr->ruim_sms_file_name;
                
                cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
                cmd.access_uim.access    = UIM_READ_F;
                
                bWaitRpt = TRUE;
                
                wms_ruim_access_ext(&cmd);
                return FALSE;
            }
            else
            {
                bWaitRpt = FALSE;
                if (wms_ruim_status_report != UIM_PASS)
                {
                    i++;
                }
                else
                {
                    status = UIM_PASS;
                }
            }
        }
        if (status == UIM_PASS)
        {
            if ((ruim_data[0] != WMS_TAG_NONE) || (cfg_s_ptr->ruim_tags[i] != WMS_TAG_NONE))
            {
                wms_cfg_update_msg_info_cache(ruim_data[0],
                           WMS_MEMORY_STORE_RUIM,
                           i,
                           ruim_data,
                           (uint8)cfg_s_ptr->ruim_sms_rec_len );
            }
            
            cfg_s_ptr->ruim_tags[i] = (wms_message_tag_e_type)ruim_data[0];
            
            i++;
            
            return FALSE;
        }
        
        if (i < cfg_s_ptr->ruim_max_slots)
        {
            return FALSE;
        }
        has_sms = FALSE;
        bWaitRpt = FALSE;
    }

    /* EFsmsp related init
    */
    if (has_smsp)
    {
        MSG_ERROR("t=%d", t, 0, 0);
        status = UIM_FAIL;
        if (t == 0)
        {
            if (bWaitRpt == FALSE)
            {
                cfg_s_ptr->ruim_max_templates = 0;
                cfg_s_ptr->ruim_smsp_file_name = UIM_CDMA_SMS_PARAMS;
                
                /* Determine the number of records and record length
                */
                /* Set up cmd item */
                cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
                cmd.access_uim.data_ptr  = ruim_data;
                cmd.access_uim.offset    = 1;
                
                cmd.access_uim.item      = cfg_s_ptr->ruim_smsp_file_name;
                
                cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
                cmd.access_uim.access    = UIM_READ_F;
                
                bWaitRpt = TRUE;
                
                wms_ruim_access_ext(&cmd);
                return FALSE;
            }
            else
            {
                bWaitRpt = FALSE;
                if (wms_ruim_status_report != UIM_PASS)
                {
                    t++;
                    return FALSE;
                }
                else
                {
                    cfg_s_ptr->ruim_max_templates   = cmd.access_uim.num_records_rsp;
                    cfg_s_ptr->ruim_smsp_rec_len    = cmd.access_uim.num_bytes_rsp;
                    MSG_ERROR("cfg_s_ptr->ruim_max_templates=%d cfg_s_ptr->ruim_smsp_rec_len=%d", cfg_s_ptr->ruim_max_templates, cfg_s_ptr->ruim_smsp_rec_len, 0);
                    
                    if (cfg_s_ptr->ruim_max_templates == 0)
                    {
                        t++;
                        return FALSE;
                    }
                    else
                    {
                        status = UIM_PASS;
                    }
                }
            }
        }
        else if (t < cfg_s_ptr->ruim_max_templates)
        {
            if (bWaitRpt == FALSE)
            {
                /* Set up cmd item */
                cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_smsp_rec_len;
                cmd.access_uim.data_ptr  = ruim_data;
                cmd.access_uim.offset    = (uint16)(t + 1);
                
                cmd.access_uim.item      = cfg_s_ptr->ruim_smsp_file_name;
                
                cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
                cmd.access_uim.access    = UIM_READ_F;
                
                bWaitRpt = TRUE;
                
                wms_ruim_access_ext(&cmd);
                return FALSE;
            }
            else
            {
                bWaitRpt = FALSE;
                
                if (wms_ruim_status_report != UIM_PASS)
                {
                    t++;
                    return FALSE;
                }
                else
                {
                    status = UIM_PASS;
                }
            }
        }
            
        if (status == UIM_PASS)
        {
#ifdef FEATURE_RUIM_DEBUG_SMS_NOT_STRICTFF
            /* Following for invalid card work-around: */
            if ((ruim_data[0] | ruim_data[1] | ruim_data[2] | ruim_data[3]) != WMS_RUIM_SMSP_EMPTY_CHAR2)
#else
            if (ruim_data[0] != WMS_RUIM_SMSP_EMPTY_CHAR)
#endif
            {
                cfg_s_ptr->ruim_templates[t] = TRUE;
                
                wms_cfg_update_msg_info_cache(WMS_TAG_MO_TEMPLATE,
                                 WMS_MEMORY_STORE_RUIM,
                                 t,
                                 ruim_data,
                                 (uint8)cfg_s_ptr->ruim_smsp_rec_len);
            }
            else if (cfg_s_ptr->ruim_templates[t])
            {
                cfg_s_ptr->ruim_templates[t] = FALSE;
                
                // 删除可能存在的 Cache
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                wms_cacheinfo_deleteruimtemplatecache(t);
#endif
            }
                             
            t++;
            
            return FALSE;
        }
        
        if (t < cfg_s_ptr->ruim_max_templates)
        {
            return FALSE;
        }
        has_smsp = FALSE;
    }

    /* EFsmss related init
    */
    if (bWaitRpt == FALSE)
    {
        MSG_ERROR("-------------1", 0, 0, 0);
        cfg_s_ptr->ruim_smss_file_name = UIM_CDMA_SMS_STATUS;
        
        /* Set up cmd item */
        cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE;
        cmd.access_uim.data_ptr  = ruim_data;
        cmd.access_uim.offset    = 0;
        
        cmd.access_uim.item      = cfg_s_ptr->ruim_smss_file_name;
        
        //cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
        cmd.access_uim.access    = UIM_READ_F;
        
        bWaitRpt = TRUE;
        
        wms_ruim_access_ext(&cmd);
        return FALSE;
    }
    else
    {
        MSG_ERROR("-------------2", 0, 0, 0);
        bWaitRpt = FALSE;
        
        if (wms_ruim_status_report == UIM_PASS)
        {
            cfg_s_ptr->ruim_smss_rec_len = cmd.access_uim.num_bytes_rsp;
            
            cfg_s_ptr->ruim_last_message_number = (ruim_data[0] << 8) | ruim_data[1];
            cfg_s_ptr->ruim_last_wap_message_number = (ruim_data[2] << 8 ) | ruim_data[3];
            cfg_s_ptr->ruim_mem_full_flag        = (wms_sim_mem_full_flag_e_type)ruim_data[4];
        }
    }
//#ifdef FEATURE_GSTK
#if defined(FEATURE_GSTK) || defined(FEATURE_UIM_TOOLKIT)
    if (cfg_s_ptr->refresh_in_progress)
    {
        /* Since it is called for refresh
           Send Message List Event */
        wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
        
        wms_cfg_do_template_list(WMS_MEMORY_STORE_RUIM);
        
        wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_MO_TEMPLATE);
        
        wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
        
        cfg_s_ptr->cdma_init_complete = TRUE;
        
        wms_cfg_do_refresh_done_event();
        
        MSG_MED("GSDI Refresh: Returning from wms_ruim_init",0,0,0);
    }
    else
#endif /* FEATURE_GSTK */
    {
        /* Notify the clients that RUIM is ready
        */
        wms_cfg_do_cdma_ready_event();
        
        MSG_ERROR("Returning from wms_ruim_init after cdma ready event",0,0,0);
    }
    
    // 下面语句的目的是便于 UTK Refresh 命令时再次进入
    i = 0;
    bInitStart = FALSE;
    bSvcInited = FALSE;
    
    bWaitRpt = FALSE;
    
    MSG_ERROR("-------------3", 0, 0, 0);
    return TRUE;
} /* wms_ruim_init_stepbystep() */

#ifdef FEATURE_OMH_SMS
void wms_ruim_OMH_init(void)
{
    static          boolean bInited = FALSE;
    boolean         has_bcsms = FALSE;
    boolean         has_smscap = FALSE;
    uim_rpt_status  status; /* status of ruim operation */
    uint8           data_table[WMS_RUIM_SERVICE_TABLE_SIZE+1] = {0};
    
    if (bInited)
    {
        return;
    }
    if (!IsRunAsUIMVersion())
    {// 当前软件运行于无卡版本
        return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    
    /* Read CDMA Service Table                                                  */
    
    cmd.access_uim.num_bytes = WMS_RUIM_SERVICE_TABLE_SIZE;
    cmd.access_uim.data_ptr  = data_table;
    cmd.access_uim.offset    = 0;
    cmd.access_uim.item      = UIM_CDMA_CDMA_SVC_TABLE;
    cmd.access_uim.access    = UIM_READ_F;
    
    status = wms_ruim_access(&cmd);

    if (status == UIM_PASS)
    {
        has_bcsms  = WMS_RUIM_SERVICE_TABLE_HAS_BCSMS(data_table[3]);
        has_smscap = WMS_RUIM_SERVICE_TABLE_HAS_3GPD(data_table[3]);
        MSG_HIGH("RUIM BCSMS support: %d", has_bcsms,0,0);
        MSG_HIGH("RUIM SMSCAP support: %d", has_smscap,0,0);
    }

    /* Inform NVRUIM about the card's capabilities for SMS and re-read the NV
    ** items that might come from the card.
    */
    nvruim_init_wms_svc_items(has_bcsms, has_smscap);
    if (TRUE == has_bcsms)
    {
#ifdef FEATURE_BROADCAST_SMS
        wms_bc_init();
#endif /* FEATURE_BROADCAST_SMS */
    }
    if (TRUE == has_smscap)
    {
        wms_nv_init_nvruim();
    }
    
    bInited = TRUE;
    
    /* done */
    return;
} /* wms_ruim_init() */
#endif /* FEATURE_OMH_SMS */

#endif


/*
*/
boolean wms_ruim_read_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  uim_rpt_status status; /* status of ruim operation */

  /* Set up cmd item */
  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_sms_rec_len;
  cmd.access_uim.data_ptr  = data;
  cmd.access_uim.offset    = (uint16)(index + 1);

  cmd.access_uim.item      = cfg_s_ptr->ruim_sms_file_name;

  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.access    = UIM_READ_F;
  status = wms_ruim_access( &cmd );

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_read_sms() */

/*
*/
boolean wms_ruim_write_sms
(
  wms_message_index_type   index,
  uint8                    * data,
  boolean                  is_prl_update
)
{
  uim_rpt_status status; /* status of ruim operation */

  /* cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE; */
#ifdef CUST_EDITION
  if ((data[1]+2) <= cfg_s_ptr->ruim_sms_rec_len)
  {
     cmd.access_uim.num_bytes = data[1]+2;
  }
  else
  {
     cmd.access_uim.num_bytes = cfg_s_ptr->ruim_sms_rec_len;
  }
#endif
  cmd.access_uim.data_ptr = data;
  cmd.access_uim.offset   = (uint16)(index+1);

  cmd.access_uim.item     = cfg_s_ptr->ruim_sms_file_name;

  cmd.access_uim.rec_mode = UIM_ABSOLUTE;
  cmd.access_uim.access   = UIM_WRITE_F;
  status = wms_ruim_access( &cmd );

  if(is_prl_update && (status == UIM_PASS))
  {
    /* Must do 1 more read to get the correct user data */
    if(wms_ruim_read_sms(index, data) == FALSE)
    {
      status = UIM_FAIL;
    }
  }

#ifndef CUST_EDITION  
  if( status == UIM_PASS )
  {
    wms_cfg_update_msg_info_cache( WMS_TAG_NONE,
                                   WMS_MEMORY_STORE_RUIM,
                                   index,
                                   data,
                                   (uint8)cfg_s_ptr->ruim_sms_rec_len );
  }
#endif

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_write_sms() */

/*
*/
boolean wms_ruim_delete_sms
(
  wms_message_index_type  index
)
{
  /* Clear out the record
  */
  ruim_data[0] = WMS_TAG_NONE;
#ifndef CUST_EDITION
  memset( ruim_data+1, 0xFF, cfg_s_ptr->ruim_sms_rec_len-1 );
#else
  ruim_data[1] = 0;
#endif

  return wms_ruim_write_sms( index, ruim_data, FALSE );

} /* wms_ruim_delete_sms() */



/*
*/
boolean wms_ruim_read_smsp
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  uim_rpt_status status; /* status of ruim operation */


  /* Set up cmd item */
  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_smsp_rec_len;
  cmd.access_uim.data_ptr  = data;
  cmd.access_uim.offset    = (uint16)(index + 1);

  cmd.access_uim.item      = cfg_s_ptr->ruim_smsp_file_name;

  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.access    = UIM_READ_F;
  status = wms_ruim_access( &cmd );

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_read_smsp() */

/*
*/
boolean wms_ruim_write_smsp
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  uim_rpt_status status; /* status of ruim operation */

  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_smsp_rec_len;
  cmd.access_uim.data_ptr = data;
  cmd.access_uim.offset   = (uint16)(index+1);

  cmd.access_uim.item     = cfg_s_ptr->ruim_smsp_file_name;

  cmd.access_uim.rec_mode = UIM_ABSOLUTE;
  cmd.access_uim.access   = UIM_WRITE_F;
  status = wms_ruim_access( &cmd );

#ifndef CUST_EDITION
  if( status == UIM_PASS )
  {
    wms_cfg_update_msg_info_cache( WMS_TAG_MO_TEMPLATE,
                                   WMS_MEMORY_STORE_RUIM,
                                   index,
                                   data,
                                   (uint8)cfg_s_ptr->ruim_smsp_rec_len );
  }
#endif
  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_write_smsp() */

/*
*/
boolean wms_ruim_delete_smsp
(
  wms_message_index_type  index
)
{
  /* Clear out the record
  */
  memset( ruim_data, WMS_RUIM_SMSP_EMPTY_CHAR, cfg_s_ptr->ruim_smsp_rec_len );

  return wms_ruim_write_smsp( index, ruim_data );

} /* wms_ruim_delete_smsp() */


/*
*/
boolean wms_ruim_read_smss
(
  uint8   *data
)
{
  uim_rpt_status status; /* status of ruim operation */


  /* Set up cmd item */
  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_smss_rec_len;
  cmd.access_uim.data_ptr  = data;
  cmd.access_uim.offset    = 0;

  cmd.access_uim.item      = cfg_s_ptr->ruim_smss_file_name;

  //cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.access    = UIM_READ_F;
  status = wms_ruim_access( &cmd );

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_read_smss() */


/*
*/
boolean wms_ruim_write_smss
(
  uint8   *data
)
{
  uim_rpt_status status; /* status of ruim operation */

  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_smss_rec_len;
  cmd.access_uim.data_ptr = data;
  cmd.access_uim.offset   = 0;

  cmd.access_uim.item     = cfg_s_ptr->ruim_smss_file_name;

  //cmd.access_uim.rec_mode = UIM_ABSOLUTE;
  cmd.access_uim.access   = UIM_WRITE_F;
  status = wms_ruim_access( &cmd );

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* wms_ruim_write_smss() */

#ifdef CUST_EDITION
/*===========================================================================

FUNCTION AT_wms_ruim_report

DESCRIPTION
  Accept status reports from R-UIM
  Set the global status and signal WMS task that status has arrived.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void AT_wms_ruim_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  wms_ruim_status_report = report->rpt_status;
  //(void)rex_set_sigs( &wms_tcb, WMS_RUIM_ACCESS_SIG );
  (void) rex_set_sigs( &ds_tcb, DS_UIM_CMD_SIG);

} /* sms_ruim_report() */

/*===========================================================================

FUNCTION AT_wms_ruim_access

DESCRIPTION
  Access the R-UIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status AT_wms_ruim_access
(
  uim_cmd_type *cmd_ptr       /* R-UIM command                               */
)
{
  rex_sigs_type sigs;
  cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F; /* "Access" an EF    */

  /* Indicate command completion is to be signaled:
   *    Do not signal the WMS Task
   *    Use no signal
   *    No "done" queue
   *    Status call-back function
   *    Always Report status
   */
  cmd_ptr->access_uim.hdr.cmd_hdr.task_ptr = NULL;
  cmd_ptr->access_uim.hdr.cmd_hdr.sigs = 0;
  cmd_ptr->access_uim.hdr.cmd_hdr.done_q_ptr = NULL;

  cmd_ptr->hdr.rpt_function = AT_wms_ruim_report;
  cmd_ptr->hdr.protocol = UIM_CDMA;
  cmd_ptr->hdr.slot     = UIM_SLOT_AUTOMATIC;
  cmd_ptr->access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;

  /* Send the command to the R-UIM:
   *    Clear the "command done signal"
   *    Send the command
   *    Wait for the command to be done
   */

  //(void) rex_clr_sigs( &wms_tcb, WMS_RUIM_ACCESS_SIG );
  (void) rex_clr_sigs( &ds_tcb, DS_UIM_CMD_SIG);
  
  uim_cmd( cmd_ptr );
  wms_kick_dog();
   
  dog_report( DOG_DS_RPT );
  while(1)
  {
    /* wait for DOG signal at the same time */
    sigs = rex_wait( DS_UIM_CMD_SIG | WMS_RPT_TIMER_SIG );

    /* Kick the dog during the wait */
    if( sigs & WMS_RPT_TIMER_SIG )
    {
      wms_kick_dog();
      dog_report( DOG_DS_RPT );
    }
    if( sigs & DS_UIM_CMD_SIG )
    {
      /* Done */
      (void) rex_clr_sigs( &ds_tcb, DS_UIM_CMD_SIG);
      break;
    }
    /* Otherwise continue to wait */
  }

  return(wms_ruim_status_report);

} /* AT_wms_ruim_access */


boolean AT_wms_ruim_read_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  uim_rpt_status status; /* status of ruim operation */
  /* Set up cmd item */
  cmd.access_uim.num_bytes = (uint16)cfg_s_ptr->ruim_sms_rec_len;
  cmd.access_uim.data_ptr  = data;
  cmd.access_uim.offset    = (uint16)(index + 1);

  cmd.access_uim.item      = cfg_s_ptr->ruim_sms_file_name;

  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.access    = UIM_READ_F;
  status = AT_wms_ruim_access( &cmd );

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* AT_wms_ruim_read_sms() */


boolean AT_wms_ruim_write_sms
(
  wms_message_index_type   index,
  uint8                    * data,
  boolean                  is_prl_update
)
{
  uim_rpt_status status; /* status of ruim operation */

  /* cmd.access_uim.num_bytes = WMS_RUIM_EF_BUF_SIZE; */
  if ((data[1]+2) <= cfg_s_ptr->ruim_sms_rec_len)
  {
     cmd.access_uim.num_bytes = data[1]+2;
  }
  else
  {
     cmd.access_uim.num_bytes = cfg_s_ptr->ruim_sms_rec_len;
  }
  cmd.access_uim.data_ptr = data;
  cmd.access_uim.offset   = (uint16)(index+1);

  cmd.access_uim.item     = cfg_s_ptr->ruim_sms_file_name;

  cmd.access_uim.rec_mode = UIM_ABSOLUTE;
  cmd.access_uim.access   = UIM_WRITE_F;
  status = AT_wms_ruim_access( &cmd );

  if(is_prl_update && (status == UIM_PASS))
  {
    /* Must do 1 more read to get the correct user data */
    if(AT_wms_ruim_read_sms(index, data) == FALSE)
    {
      status = UIM_FAIL;
    }
  }

  return (status==UIM_PASS) ? TRUE : FALSE;

} /* AT_wms_ruim_write_sms() */

/*
*/
boolean AT_wms_ruim_delete_sms
(
  wms_message_index_type  index
)
{
  /* Clear out the record
  */
  ruim_data[0] = WMS_TAG_NONE;
  ruim_data[1] = 0;

  return AT_wms_ruim_write_sms( index, ruim_data, FALSE );
} /* AT_wms_ruim_delete_sms() */

#endif
#endif /* FEATURE_CDSMS_RUIM */

/* End of wmsruim.c */
