/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains generic function definitions used by the IS707
  AT command processor.
  
EXTERNALIZED FUNCTIONS
  dsat707_find_cmd_name
    This function matches the command name that gets passed in with the
    command table entry and returns the corresponding index.

  dsat707_unrec_func_ptr
    This function processess unrecognized commands, and commands with
    parameter errors which will cause a connection to the base station
    if +CXT=1.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995-2009 by QUALCOMM Incorporated.
   All Rights Reserved. 
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707util.c_v   1.11   18 Feb 2003 10:39:58   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707util.c#11 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
03/18/09   ss      Fixed the memory leak caused by item_ptr in function
                   dsat707_unrec_func_ptr.
01/28/09   ss      Klocwork Fix.
01/23/09   sn      Fixed featurization issue.
06/24/08   mga     Merged changes to support CTA timer NV item
04/09/08   bs      Fixed Klocwork errors.
02/28/08   pp      Klocwork fixes.
10/15/07 sq/mga/yz Changed ERR_FATAL to MSG_ERROR
08/09/06   az      Fixed compiler warning.
05/23/06    squ    Featurized async data
06/29/04   sy      Added bound check for the pzid hysteresis NV items.
09/17/03   sy      Added support for nv items for pzid hysteresis under
                   FEATURE_DS_PZID_HYSTERESIS.
01/28/03   rsl     Don't append 'T' to dial string because of ATCOP changes.
01/20/03   ar      Added dialstr_modifiers to ds707_async_dial_str_cb 
                   interface.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
09/30/02   ar      Accomodate changed interface to proc_dial_string
07/13/02   atp     Changed signature of dsat707_send_config() to make param
                   passed in a ptr to const byte.
07/13/02   atp     In dsat707_send_config() functions, calls 
                   ds707_async_ps_tcp_enqueue() to queue data.
4/3/01     rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "dsati.h"
#include "dsat.h"
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
#include "dsat707vendctab.h"
#include "dsat707util.h"
#include "dsatparm.h"
#include "msg.h"
#include "dstaski.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
  #include "ds707_async_mgr.h"
#endif /*FEATURE_ASYNC_DATA_NOOP*/

#include "dsatact.h"
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "err.h"

#ifdef FEATURE_JCDMA_1X
#include "ds707_jcdma_m51.h"
#endif /* FEATURE_JCDMA_1X */

#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid_hyst.h"
#else
#include "ds707_epzid.h"
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */

/*-------------------------------------------------------------------------
    Definition of strings used for verbose result codes:

    Assignment to the pointer arrays "xxx_codes" must match
    the definition of "ds_atcop_result_enum_type" in dsatcop.h
-------------------------------------------------------------------------*/

const byte res_code_string_ok []        = "OK";         /*  0  */
const byte res_code_string_con []       = "CONNECT";    /*  1  */
const byte res_code_string_ring []      = "RING";       /*  2  */
const byte res_code_string_no_car []    = "NO CARRIER"; /*  3  */
const byte res_code_string_error []     = "ERROR";      /*  4  */
const byte res_code_string_nu []        = "(not used)"; /*  5  */
const byte res_code_string_no_dial []   = "NO DIALTONE";/*  6  */
const byte res_code_string_busy []      = "BUSY";       /*  7  */
const byte res_code_string_no_ans []    = "NO ANSWER";  /*  8  */

const byte res_code_string_rng_amps  [] = "RING AMPS";  /* 10  */
const byte res_code_string_rng_async [] = "RING ASYNC"; /* 11  */
const byte res_code_string_rng_fax []   = "RING FAX";   /* 12  */
const byte res_code_string_rng_pkt   [] = "RING PACKET";/* 13  */

/*20*/
const byte res_code_string_no_amps[]    = "NO AMPS SERVICE";
/*21*/
const byte res_code_string_no_srv []    = "NO SERVICE";
/*22 */
const byte res_code_string_no_async []  = "NO ASYNC SERVICE";
/*23*/
const byte res_code_string_no_fax []    = "NO FAX SERVICE";
/*24*/
const byte res_code_string_no_pkt []    = "NO PACKET SERVICE";
/*25*/
const byte res_code_string_no_intr[]    = "BAD REQUEST";
/*26*/
const byte res_code_string_paged []     = "PAGED";
const byte res_code_string_reoder []    = "RETRY";      /* 27 */
const byte res_code_string_page_fail [] = "PAGE FAIL";  /* 28 */
const byte res_code_string_link_fail [] = "LINK FAIL";  /* 29 */

const byte res_code_string_released []  = "RELEASE";    /* 30 */

/*  Stand alone */
const byte res_code_string_cerror []    = "+CERROR: ";

const byte *const res_codes [] =
      { res_code_string_ok,
        res_code_string_con,
        res_code_string_ring,
        res_code_string_no_car,
        res_code_string_error,
        res_code_string_nu,
        res_code_string_no_dial,
        res_code_string_busy,
        res_code_string_no_ans
      };

const byte *const cell_res_codes [] =
      { res_code_string_rng_amps,
        res_code_string_rng_async,
        res_code_string_rng_fax,
        res_code_string_rng_pkt
      };

const byte *const cell_error_res_codes [] =
      { res_code_string_no_amps,
        res_code_string_no_srv,
        res_code_string_no_async,
        res_code_string_no_fax,
        res_code_string_no_pkt,
        res_code_string_no_intr,
        res_code_string_paged,
        res_code_string_reoder,
        res_code_string_page_fail,
        res_code_string_link_fail,
        res_code_string_released
      };

/*-------------------------------------------------------------------------
  Dial string for Packet/QNC calls. Defaults to "#777". Note that if the
  default string is changed, the length variable should also be updated.
-------------------------------------------------------------------------*/
byte dsat707_pkt_orig_str[NV_MAX_PKT_ORIG_DIGITS] = "#777";

/*-------------------------------------------------------------------------
  Length of Packet/QNC dial string (including the terminating NULL)
-------------------------------------------------------------------------*/
byte dsat707_pkt_orig_str_len = 5;

boolean unrec_cmd_rcvd = FALSE;

/*===========================================================================

            EXTERNAL DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

LOCAL void dsat707_send_config_action
(
  const dsati_cmd_type * table,
  const unsigned int sizeof_table,
  dsm_item_type **item_ptr_ptr
) ;


/*=========================================================================

FUNCTION DSAT707_UNREC_FUNC_PTR

DESCRIPTION
  This function processess unrecognized commands, and commands with parameter
  errors which will cause a connection to the base station if +CXT=1.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_unrec_func_ptr
(
  byte* cmd_line_ptr
)
{
#ifndef FEATURE_ASYNC_DATA_NOOP
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type *revert_ptr = NULL;
  boolean digit_mode;
  byte c;
  dsm_item_type *item_ptr; /* Data item for configuration */
  unrec_cmd_rcvd = TRUE;
  /*-------------------------------------------------------------------------*/
  /* If cxt_val is 0 then no further processing is required */
  if( dsat707_cxt_val != 1 )
  {
    MSG_ERROR("dsat707_cxt_val is 0;Need no further processing",0,0,0);
    return DSAT_ERROR;
  }
  if (dsatcmdp_at_state == DSAT_CMD)
  {
    dsat_dial_modifier_info_type  dialstr_modifiers;
    memset( (void*)&dialstr_modifiers, 0, sizeof(dialstr_modifiers) );
    item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
    if (item_ptr == NULL)
    {
      MSG_ERROR("Running out of buffers",0,0,0);
      return DSAT_ERROR;
    }

  /*-------------------------------------------------------------------------
      Get an item to hold command parameters that may need reversion,
      and copy the parameters.  Command mode only.
  -------------------------------------------------------------------------*/
    revert_ptr = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);
    if (revert_ptr == NULL)
    {
      MSG_ERROR("AT CMD Reply out of memory!",0,0,0);
    }
    else
    {
      memcpy (revert_ptr->data_ptr,dsat707_fns_val,dsat707_sizeof_fns_val);
      revert_ptr->used = dsat707_sizeof_fns_val;
    }

    /*-------------------------------------------------------------------------
        Unrecognized commands, and commands with parameter errors
          will cause a connection to the base station if +CXT=1.
          In this case, revert dsat707_fns_val to its value at the
          beginning of the command line to avoid concatenating the
          value twice (once here, and once when the command is reflected).
          The reversion is benign if the there was no AT+FNS command.
          
    -------------------------------------------------------------------------*/
    digit_mode = 1;
    if (ds707_async_get_active_iface_ptr() == NULL)
    {
      ds707_async_dial_str_cb ( cmd_line_ptr,digit_mode,&dialstr_modifiers );
      result = DSAT_NO_RSP;
    }

    memcpy (dsat707_fns_val,revert_ptr->data_ptr,dsat707_sizeof_fns_val);
    item_ptr->data_ptr[item_ptr->used++] = 'A';
    item_ptr->data_ptr[item_ptr->used++] = 'T';
    while ( (c = *cmd_line_ptr++) != 0)
    {
      item_ptr->data_ptr[item_ptr->used++] = c;
    }

    item_ptr->data_ptr[item_ptr->used++] = (byte)dsat_s3_val;
    item_ptr->kind = DSM_PS_APP_PAYLOAD;
    ds707_async_ps_tcp_enqueue( &item_ptr);
    (void) dsm_free_packet ( &revert_ptr );
  }
  else
  {
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
  }
  return result;
#else  /* FEATURE_ASYNC_DATA_NOOP */
  DSATI_ERROR_MSG();
  return DSAT_ERROR;
#endif /* FEATURE_ASYNC_DATA_NOOP*/
}

/*===========================================================================

FUNCTION DSAT707_SEND_CONFIG

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  Follow the configuration with the +CFG string (always, even if empty),
  and then the current command.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat707_send_config
(
  const byte * cur_cmd
)
{
#ifndef FEATURE_ASYNC_DATA_NOOP
  byte c;
  dsm_item_type *item_ptr; /* Data item for configuration */
  int index;
  byte cfg [] = "+CFG";

  item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
  if (item_ptr == NULL)
  {
    MSG_ERROR("Running out of buffers",0,0,0);
    return;
  }

  dsat707_send_config_action (dsat_sreg_table,
                              dsat_sreg_table_size, 
                              &item_ptr);

  dsat707_send_config_action (dsat_basic_table,
                              dsat_basic_table_size, 
                              &item_ptr);

  dsat707_send_config_action (dsat_basic_action_table,
                              dsat_basic_action_table_size, 
                              &item_ptr);

  dsat707_send_config_action (dsat_ext_table,
                              dsat_ext_table_size, 
                              &item_ptr);
  
  dsat707_send_config_action (dsat707_ext_para_table,
                              dsat707_ext_para_table_size, 
                              &item_ptr);

  dsat707_send_config_action (dsat707_ext_action_table,
                              dsat707_ext_action_table_size, 
                              &item_ptr);
  
  dsat707_send_config_action (dsat707_fax_table,
                              dsat707_fax_table_size,
                              &item_ptr);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  
#ifdef FEATURE_DS_MOBILE_IP
  dsat707_send_config_action (dsat707_mip_table,
                              dsat707_mip_table_size,
                              &item_ptr);
#endif /* FEATURE_DS_MOBILE_IP */
  
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
  
  dsat707_send_config_action (dsat707_vend_para_table,
                              dsat707_vend_para_table_size, 
                              &item_ptr);

  dsat707_send_config_action (dsat707_vend_action_table,
                              dsat707_vend_action_table_size, 
                              &item_ptr);
  if (item_ptr == NULL)
  {
    MSG_ERROR("item_ptr is NULL.Running out of buffers...",0,0,0);
    return;
  }

  if (item_ptr->used > item_ptr->size - MIN_ROOM)
  {
    item_ptr->kind = DSM_PS_APP_PAYLOAD;
    ds707_async_ps_tcp_enqueue( &item_ptr);
    item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
    if (item_ptr == NULL)
    {
      MSG_ERROR("Running out of buffers",0,0,0);
      return;
    }
  }

  item_ptr->data_ptr[item_ptr->used++] = 'A';
  item_ptr->data_ptr[item_ptr->used++] = 'T';
  index = dsat707_find_cmd_name (cfg);
  if (index != -1) 
  {
    item_ptr->used += dsatutil_fmt_string_val 
        (dsat707_ext_para_table + index,
         &item_ptr->data_ptr[item_ptr->used], 
         SET);
  }
  else
  {
    MSG_HIGH("AT+CFG not sent to IWF", 0, 0, 0);
  }
  
  item_ptr->data_ptr[item_ptr->used++]
    = (byte) dsat_s3_val;
  

  if (item_ptr->used > item_ptr->size - MIN_ROOM)
  {
    item_ptr->kind = DSM_PS_APP_PAYLOAD;
    ds707_async_ps_tcp_enqueue( &item_ptr);
    item_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
    if (item_ptr == NULL)
    {
      ERR("Running out of buffers",0,0,0);
      return;
    }
  }

  item_ptr->data_ptr[item_ptr->used++] = 'A';
  item_ptr->data_ptr[item_ptr->used++] = 'T';
  
  if ((unrec_cmd_rcvd == FALSE) && (*cur_cmd != 'A'))
  {
    item_ptr->data_ptr[item_ptr->used++] = 'D';
  }

  while ( (c = *cur_cmd++) != 0)
  {
    item_ptr->data_ptr[item_ptr->used++] = c;
  }
  
  item_ptr->data_ptr[item_ptr->used++]
    = (byte) dsat_s3_val;
  
  item_ptr->kind = DSM_PS_APP_PAYLOAD;
  ds707_async_ps_tcp_enqueue( &item_ptr);
  
#endif /* FEATURE_ASYNC_DATA_NOOP */
} /*  dsat707_send_config  */


/*===========================================================================

FUNCTION DSAT707_SEND_CONFIG_ACTION

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  This function does one parse table.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void dsat707_send_config_action
(
  const dsati_cmd_type * table,        /*  Table to send     */
  const unsigned int sizeof_table,  /*  Size of the table */
  dsm_item_type **item_ptr_ptr      /*  Output item       */

)
{
#ifndef FEATURE_ASYNC_DATA_NOOP
  unsigned int i, j, flag;
  byte c;
  const byte *q_ptr;

/*---------------------------------------------------------------------------
    Loop thru the table, looking for something to download.
---------------------------------------------------------------------------*/

  for (i = 0; i < sizeof_table; ++i)
    {
      flag = 0;
      if (table[i].attrib & DOWN_LOAD
          && ((table[i].attrib & FCLASS2P0) == 0
              || dsat_fclass_val == 2 ))
      {
          if (table[i].attrib & ALWAYS)
          {
            flag = 1;
          }
          else if (table[i].attrib & STRING)
          {
            if (* (dsat_string_item_type *)table[i].val_ptr != '\0')
            {
              flag = 1;
            }
          }
          else
          {
            for (j = 0; j < table[i].compound; ++j)
            {
              if (* ((dsat_num_item_type *)table[i].val_ptr + j)
                  != ((dflm_type *) table[i].def_lim_ptr)[j].default_v)
              {
                flag = 1;
              }
            }
          }
      }
      
      if (flag)
      {
/*---------------------------------------------------------------------------
  Item found: Generate an AT command to transmit its
  current value to the base station.
  
  If there is not enough room in the data item, queue
  it and get a new one.
  
  Format the item according to it's type. Then terminate
  with a <CR>.
  ---------------------------------------------------------------------------*/
        if ((*item_ptr_ptr)->used > (*item_ptr_ptr)->size - MIN_ROOM)
        {
          (*item_ptr_ptr)->kind = DSM_PS_APP_PAYLOAD;
          ds707_async_ps_tcp_enqueue( item_ptr_ptr );
          *item_ptr_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
          if (*item_ptr_ptr == NULL)
          {
            ERR("Running out of buffers",0,0,0);
            return;
          }
        }
        
        (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = 'A';
        (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = 'T';
        
        if (table[i].attrib & STRING)           /*  String item */
        {
          (*item_ptr_ptr)->used += dsatutil_fmt_string_val
            (table + i,
             &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used],
             SET);
        }
        else if (table[i].attrib & SIMPLE_DEC)  /*  Simple decimal item */
        {
          q_ptr = table[i].name;
          while ( (c = *q_ptr++) != 0)
          {
            (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = c;
          }
          
          (*item_ptr_ptr)->used += dsatutil_fmt_num_val
            (table + i,
             &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used]);
        }
        else                                    /*  Extended item */
        {
          (*item_ptr_ptr)->used += dsatutil_fmt_num_val_ex
            (table + i,
             &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used],
             SET);
        }
        
        (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++]
          = (byte) dsat_s3_val;
      }
    }
#endif /*FEATURE_ASYNC_DATA_NOOP*/
}

/*===========================================================================

FUNCTION DSAT707_FIND_CMD_NAME

DESCRIPTION
This function matches the command name that gets passed in with the command
table entry and returns the corresponding index.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns the table index that matches.  Returns -1 if no match is found.

SIDE EFFECTS
  None
  
===========================================================================*/
int dsat707_find_cmd_name
(
 byte *s1
)
{
 int result = -1;
 unsigned int index = 0;
 int no_match = 1;

 for ( index = 0; index < dsat707_ext_para_table_size && no_match; index++ )
 {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_ext_para_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    MSG_LOW("%s found in ext_table", s1, 0, 0);
 }

#ifndef FEATURE_ASYNC_DATA_NOOP
 for ( index = 0; index < dsat707_fax_table_size && no_match; index++ )
 {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_fax_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    MSG_LOW("%s found in fax_table", s1, 0, 0);
 }
#endif /* FEATURE_ASYNC_DATA_NOOP */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MOBILE_IP
 for ( index = 0; index < dsat707_mip_table_size && no_match; index++ )
 {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_mip_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    MSG_LOW("%s found in mip_table", s1, 0, 0);
 }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

 for ( index = 0; index < dsat707_vend_para_table_size && no_match; index++ )
 {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_vend_para_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    MSG_LOW("%s found in vend_table", s1, 0, 0);
 }

 if (no_match) 
 {
  return -1;
 }
 else
 {
  return result;
 }
}
/*===========================================================================

FUNCTION DSAT_IS_STATUS

DESCRIPTION
  This function returns TRUE if the argument string begins with
  the numeric character (1-8)  or the word corresponding to the
  result specified by the second arg.  Trailing characters are ignored.
  Other wise if returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
===========================================================================*/

boolean dsat_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
)
{
  byte c;
  const byte *c_ptr;

  if (*a_ptr == '0' + result)
    {
    return TRUE;
    }

  if((result >= DSAT_MAX_BASIC)||(result < DSAT_OK))
  {
    /* Applicable till 8 only! */
    return FALSE;
  }
  c_ptr = res_codes [result];

  while ( (c = *c_ptr++) != '\0')
    {
    if (c != *a_ptr++)
      {
      return FALSE;
      }
    }
  return TRUE;
}
/*===========================================================================
FUNCTION      DSAT707_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsat707_nv_sync(void)
{
  nv_stat_enum_type status;                         /* status from NV call */
#ifdef FEATURE_DS_MOBILE_IP
  dsat707_mip_result_type mip_status;
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
   Get MN-HA authentication method from NV.
  -------------------------------------------------------------------------*/
  status = dsatutil_get_nv_item( NV_DS_MIP_2002BIS_MN_HA_AUTH_I, &ds_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /* At time of first boot, the NV item may not have been initialized
       yet.  Initialize it from statically-initialized default. */
    ds_nv_item.ds_mip_2002bis_mn_ha_auth = dsat707_qcmipt_dflm[0].default_v;
    (void) dsatutil_put_nv_item( NV_DS_MIP_2002BIS_MN_HA_AUTH_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR( "Bad NV read status %d for MH-HA auth method", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
  dsat707_qcmipt_dflm[0].default_v = ds_nv_item.ds_mip_2002bis_mn_ha_auth;

  /*-------------------------------------------------------------------------
   Get Mobile IP preference setting from NV.
  -------------------------------------------------------------------------*/
  status = dsatutil_get_nv_item( NV_DS_QCMIP_I, &ds_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /* At time of first boot, the NV item may not have been initialized
       yet.  Initialize it from statically-initialized default. */
    ds_nv_item.ds_qcmip = dsat707_qcmip_dflm[0].default_v;
    (void) dsatutil_put_nv_item( NV_DS_QCMIP_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR("Bad NV read status %d for DS QCMIP", status, 0, 0 );
  }
#ifdef CUST_EDITION
  ds_nv_item.ds_qcmip = 1;
#endif
  /*-------------------------------------------------------------------------
    Store $qcmip val retrieved from NV in default field, which will
    later (in init_table) be put in dsat707_qcmip_val.
  -------------------------------------------------------------------------*/
  dsat707_qcmip_dflm[0].default_v = ds_nv_item.ds_qcmip;

  /*-------------------------------------------------------------------------
   Get Mobile IP active profile from NV.
  -------------------------------------------------------------------------*/
  status = dsatutil_get_nv_item( NV_DS_MIP_ACTIVE_PROF_I, &ds_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /* At time of first boot, the NV item may not have been initialized
       yet.  Initialize it from statically-initialized default. */
    ds_nv_item.ds_mip_active_prof = dsat707_qcmipp_dflm[0].default_v;
    (void) dsatutil_put_nv_item( NV_DS_MIP_ACTIVE_PROF_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR( "Bad NV read status %d for MIP active profile", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
  dsat707_qcmipp_dflm[0].default_v = ds_nv_item.ds_mip_active_prof;

  /*-------------------------------------------------------------------------
     Enable or disable  profile?
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I, &ds_nv_item);
   if(status == NV_NOTACTIVE_S)
   {
     /*----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized
       yet.Initialize it from statically-initialized default.
     ----------------------------------------------------------------------*/
     ds_nv_item.ds_mip_enable_prof[0]
                          = dsat707_qcmipep_dflm[0].default_v;
     (void) dsatutil_put_nv_item( NV_DS_MIP_ENABLE_PROF_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for MIP enable", status, 0, 0 );
   }
   /*-------------------------------------------------------------------------
     Store number retrieved from NV.
    -------------------------------------------------------------------------*/
   dsat707_qcmipep_dflm[0].default_v = 
     ds_nv_item.ds_mip_enable_prof[dsat707_qcmipp_dflm[0].default_v];
  
    /*-------------------------------------------------------------------------
      Retreive the user profile from NV
    -------------------------------------------------------------------------*/
   mip_status = dsat707_init_mip_prof();
   if(mip_status != INIT_MIP_PROF_OK)
   {
     MSG_ERROR( "Bad NV read status %d from init_mip_prof", status, 0, 0 );
   }
   ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_dflm[0].default_v;
   dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);
 
  /*------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcmiprt_dflm[0].default_v =
     ds_nv_item.ds_mip_gen_user_prof.rev_tun_pref;
   dsat707_qcmiprt_dflm[1].default_v = 1;
   dsat707_qcmipmaspi_dflm[0].default_v = 
     ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi;
   dsat707_qcmipmaspi_dflm[1].default_v = 1;
   dsat707_qcmipmhspi_dflm[0].default_v = 
     ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi;
   dsat707_qcmipmhspi_dflm[1].default_v = 1; 

#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_QNC
  /*-------------------------------------------------------------------------
    Get QNC enable flag from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_qnc_enabled = dsat707_qcqnc_dflm[0].default_v;
     (void) dsatutil_put_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for QNC enable flag", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcqnc_dflm[0].default_v = ds_nv_item.data_qnc_enabled;

   dsat707_qcqnc_val = ds_nv_item.data_qnc_enabled;
#endif /* FEATURE_DS_QNC */

#ifdef FEATURE_DS_CHOOSE_SO
  /*-------------------------------------------------------------------------
    Get the service option selection from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_DATA_SO_SET_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S)
   {
    /*-----------------------------------------------------------------------
      At time of first boot, the NV item may not have been initialized yet.
      Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_so_set = dsat707_qcso_dflm[0].default_v;
     (void) dsatutil_put_nv_item( NV_DATA_SO_SET_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for service option", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcso_dflm[0].default_v = ds_nv_item.data_so_set;

#ifndef FEATURE_DS_AT_TEST_ONLY
  /*-------------------------------------------------------------------------
    The type of SO is decided by the value of dsat707_qcso_val. If AT
    command cannot modify the SO value(i.e. if FEATURE_DS_AT_TEST_ONLY is
    not defined), then this variable will not be updated, because it is not
    present in the parse table. So initialize the variable with the value
    read from the NV.
  -------------------------------------------------------------------------*/
   dsat707_qcso_val = ds_nv_item.data_so_set;

#endif /* FEATURE_DS_AT_TEST_ONLY */

#endif /* FEATURE_DS_CHOOSE_SO */


  /*-------------------------------------------------------------------------
    Get the MDR Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_MDR_MODE_I, &ds_nv_item);

   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
      At the time of first boot, the NV item may not have been initialized
      yet. Initialize it from the statically initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_mdr_mode = dsat707_qcmdr_dflm[0].default_v;
     dsatutil_put_nv_item(NV_DATA_MDR_MODE_I, &ds_nv_item);
   }
   else if( status != NV_DONE_S )
   {
     MSG_ERROR("Bad NV read status %d for MDR mode", status, 0, 0);
   }

  /*-------------------------------------------------------------------------
    Store the value retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcmdr_dflm[0].default_v = ds_nv_item.data_mdr_mode;

  /*-------------------------------------------------------------------------
    If AT command cannot modify the MDR mode (i.e. if FEATURE_DS_AT_TEST_ONLY
    is not defined), then dsat707_qcmdr_val variable will not be updated,
    because it is not present in the parse table. So initialize the variable
    with the value read from the NV.
  -------------------------------------------------------------------------*/
   dsat707_qcmdr_val = ds_nv_item.data_mdr_mode;

  /*-------------------------------------------------------------------------
    Get the Packet/QNC dial string from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_PKT_ORIG_STR_I, &ds_nv_item);

   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
      At the time of first boot, the NV item may not have been initialized
      yet. Initialize it from the statically initialized default. Note that
      num_digits is the length of the dial string, including the terminating
      NULL character.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_pkt_orig_str.num_digits = dsat707_pkt_orig_str_len;
     memcpy((void *)ds_nv_item.data_pkt_orig_str.digits,
            (void *)dsat707_pkt_orig_str,
            dsat707_pkt_orig_str_len
            );
     dsatutil_put_nv_item(NV_DATA_PKT_ORIG_STR_I, &ds_nv_item);
   }
   else if( status != NV_DONE_S )
   {
     MSG_ERROR("Bad NV read status %d for Pkt dial string", status, 0, 0);
   }

  /*-------------------------------------------------------------------------
    Store the value retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_pkt_orig_str_len = ds_nv_item.data_pkt_orig_str.num_digits;
   memcpy((void *)dsat707_pkt_orig_str,
          (void *)ds_nv_item.data_pkt_orig_str.digits,
          ds_nv_item.data_pkt_orig_str.num_digits );

#if defined(FEATURE_IS2000_R_SCH)
  /*-------------------------------------------------------------------------
    Get the SCRM Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_SCRM_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
      At time of first boot, the NV item may not have been initialized yet.
      Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_scrm_enabled = dsat707_qcscrm_dflm[0].default_v;
     (void) dsatutil_put_nv_item( NV_DATA_SCRM_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for SCRM mode", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcscrm_dflm[0].default_v = ds_nv_item.data_scrm_enabled;

#ifndef FEATURE_DS_AT_TEST_ONLY
  /*-------------------------------------------------------------------------
    If AT command cannot modify the SCRM mode(i.e. if FEATURE_DS_AT_TEST_ONLY
    is not defined), then dsat707_qcscrm_val variable will not be updated,
    because it is not present in the parse table. So initialize the variable
    with the value read from the NV.
  -------------------------------------------------------------------------*/
   dsat707_qcscrm_val = ds_nv_item.data_scrm_enabled;
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_IS2000_R_SCH    */

#if defined(FEATURE_IS2000_R_SCH)
  /*-------------------------------------------------------------------------
    Get the R-SCH throttle (QCTRTL) Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_TRTL_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_trtl_enabled = dsat707_qctrtl_dflm[0].default_v;
     (void) dsatutil_put_nv_item( NV_DATA_TRTL_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for QCTRTL mode", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qctrtl_dflm[0].default_v = ds_nv_item.data_trtl_enabled;

#ifndef FEATURE_DS_AT_TEST_ONLY
  /*-------------------------------------------------------------------------
    If AT command cannot modify the TRTL mode(i.e. if FEATURE_DS_AT_TEST_ONLY
    is not defined), then dsat707_qctrtl_val variable will not be updated,
    because it is not present in the parse table. So initialize the variable
    with the value read from the NV.
  -------------------------------------------------------------------------*/
   dsat707_qctrtl_val = ds_nv_item.data_trtl_enabled;
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_JCDMA_1X
  /*-------------------------------------------------------------------------
    M51   1
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_JCDMA_M511_MODE_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.jcdma_m511_mode = DS707_JCDMA_M511_DEF;
     (void) dsatutil_put_nv_item( NV_JCDMA_M511_MODE_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for M51 1", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   ds707_jcdma_m511_val = ds_nv_item.jcdma_m511_mode;

  /*-------------------------------------------------------------------------
    M51   2
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_JCDMA_M512_MODE_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.jcdma_m512_mode = DS707_JCDMA_M512_DEF;
     (void) dsatutil_put_nv_item( NV_JCDMA_M512_MODE_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for M51 2", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   ds707_jcdma_m512_val = ds_nv_item.jcdma_m512_mode;

  /*-------------------------------------------------------------------------
    M51   3
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_JCDMA_M513_MODE_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.jcdma_m513_mode = DS707_JCDMA_M513_DEF;
     (void) dsatutil_put_nv_item( NV_JCDMA_M513_MODE_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for M51 3", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   ds707_jcdma_m513_val = ds_nv_item.jcdma_m513_mode;
#endif /* FEATURE_JCDMA_1X */
#if defined(FEATURE_DS_PZID_HYSTERESIS) || defined(FEATURE_DS_EPZID_HYSTERESIS)
  /*------------------------------------------------------------------------
    Process the NV items to see if HAT/ HT changed. If they are
    not set in QXDM then use the default values of 30sec for HAT and 
    60sec for HT
  --------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_HYSTERISIS_ACTIVATION_TIMER_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
#ifdef FEATURE_DS_PZID_HYSTERESIS
    ds_nv_item.hysterisis_activation_timer = PZID_HYSTERESIS_HAT_TIMER_IN_SEC;
#else
    ds_nv_item.hysterisis_activation_timer = EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
#endif /* FEATURE_DS_PZID_HYSTERESIS */
    (void) dsi_put_nv_item( NV_HYSTERISIS_ACTIVATION_TIMER_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S) 
  {
    MSG_ERROR( "Bad NV read status %d for HAT timer", status, 0, 0 );
  }

  /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
  if(ds_nv_item.hysterisis_activation_timer > PZID_HYSTERESIS_HAT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_activation_timer = PZID_HYSTERESIS_HAT_TIMER_IN_SEC;
  }
  ds707_pzid_hysteresis_hat_timer_in_sec = ds_nv_item.hysterisis_activation_timer;
#else
  if(ds_nv_item.hysterisis_activation_timer > EPZID_HYSTERESIS_HAT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_activation_timer = EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
  }
  ds707_epzid_hysteresis_hat_timer_in_sec = ds_nv_item.hysterisis_activation_timer;
#endif /* FEATURE_DS_PZID_HYSTERESIS */

  status = dsi_get_nv_item( NV_HYSTERISIS_TIMER_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
#ifdef FEATURE_DS_PZID_HYSTERESIS
     ds_nv_item.hysterisis_timer = PZID_HYSTERESIS_HT_TIMER_IN_SEC;
#else
     ds_nv_item.hysterisis_timer = EPZID_HYSTERESIS_HT_TIMER_IN_SEC;
#endif /* FEATURE_DATA_PZID_HYSTERESIS */
    (void) dsi_put_nv_item( NV_HYSTERISIS_TIMER_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S) {
    MSG_ERROR( "Bad NV read status %d for HT timer", status, 0, 0 );
  }
  
  /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
  if(ds_nv_item.hysterisis_timer > PZID_HYSTERESIS_HT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_timer = PZID_HYSTERESIS_HT_TIMER_IN_SEC;
  }
  ds707_pzid_hysteresis_ht_timer_in_sec = ds_nv_item.hysterisis_timer;
#else
  if(ds_nv_item.hysterisis_timer > EPZID_HYSTERESIS_HT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_timer = EPZID_HYSTERESIS_HT_TIMER_IN_SEC;
  }
  ds707_epzid_hysteresis_ht_timer_in_sec = ds_nv_item.hysterisis_timer;
#endif /* FEATURE_DS_PZID_HYSTERESIS */

#ifdef FEATURE_DATA_EPZID 
  status = dsi_get_nv_item( NV_ACTIVE_EPZID_TYPE_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
     ds_nv_item.active_epzid_type = DEFAULT_ACTIVE_EPZID_TYPE;
    (void) dsi_put_nv_item( NV_ACTIVE_EPZID_TYPE_I, &ds_nv_item );
  }
  else if( status == NV_DONE_S )
  {
  /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
    if(ds_nv_item.active_epzid_type > NV_EPZID_PZID_SID_NID)
    {
      ds_nv_item.active_epzid_type = DEFAULT_ACTIVE_EPZID_TYPE;
    }
    ds707_active_epzid_type = ds_nv_item.active_epzid_type;
  }
  else
  {
    MSG_ERROR( "Bad NV read status %d for ACTIVE_EPZID_TYPE", status, 0, 0 );
  }
#endif /* FEATURE_DATA_EPZID */
#endif /* FEATURE_DS_PZID_HYSTERESIS || FEATURE_DS_EPZID_HYSTERESIS */
  
  status = dsi_get_nv_item( NV_DSAT707_CTA_TIMER_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
    ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;  
  }
  else if( status != NV_DONE_S) 
  {
    MSG_ERROR( "Bad NV read status %d for CTA timer", status, 0, 0 );
    ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;
  }

  if( ds_nv_item.dsat707_cta_timer > DS707_CTA_UPPER_LIMIT )
  {
    ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;   
  }
  dsat707_cta_val = ds_nv_item.dsat707_cta_timer;
  dsat707_cta_dflm[0].default_v = dsat707_cta_val ;
 
   return;
} /* ds707_nv_sync */

/*=========================================================================*/

#endif /* FEATURE_DATA_IS707 */

