
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S 
                  U T I L I T Y )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module is used by SMS AT commands. It mainly contains utility functions
  that are used in executing the sms commands. These functions are used 
  intenally in SMS module.

EXTERNALIZED FUNCTIONS INTERNAL TO ATCOP SMS UNIT

  dsatsmsi_memory_lookup
    Converts the type of preferred memory storage into a required enum.

  dsatsmsi_send_async_indication_to_te
    If ATCOP get an unsolicited asynchronous event, This function formats 
    the data that needs to be sent to TE. It handles both SMS-DELIVERs and
    SMS-STATUS-REPORTs.

  dsatsmsi_delete_messages
    This function is used for +CMGD/$QCMGD command when a <delflag> is 
    present.
    Tries to get the list of message with that particular <delflag> in 
    memory.

  dsatsmsi_delete_msg_list
    Local function that is used to delete a message from the list of indices.
    Handles one list or lists of multiple <stat>s.

  dsatsmsi_get_mem_val
    Returns the current sms memory storage setting.
    It returns dsat_cpms_val[mem_index]/
             dsat707sms_qcpms_val[mem_index]
             depending on modes.

  dsatsmsi_get_mem_list
    Returns the current sms memory storage setting.
    It returns dsat_cpms_list[mem_index]/
               dsat707sms_qcpms_list[mem_index]
               depending on modes.

  dsatsmsi_tag_lookup
    Converts the message TAG type (wms_message_tag_e_type) to a string on
    text mode or to an integer on a pdu mode and writes the result into the
    result buffer

  dsatsmsi_str_to_addr
    Checks to see if <toda> field is specified by the user. If not, parses the 
    <da> string for address length, type of number and numbering plan.If <toda> 
    filed is specified, <toda> is split into type of number and numberign plan.

  dsatsmsi_cms_error
    Fills a response buffer with a +CMS ERROR.

  dsatsmsi_strip_quotes
    Strips out quotes from the string that is wrapped in quotes.
    Resultant string will be placed in the out pointer.
    Gives an error if the string does not begin or end with a quote

  dsatsmsi_itoa
    This function converts an integer to ASCII, using a specified
    radix. The resulting string is null terminated.

  dsatsmsi_domain_pref_to_srv
  Maps the wms_gw_domain_pref enum type to +CGSMS <service> number.

  dsatsmsi_srv_to_domain_pref
  Maps tnullhe +CGSMS <service> number to the wms_gw_domain_pref 
  enum type.


INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatsmsu.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands.


===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"

#ifdef FEATURE_DATA

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "msg.h"
#include "err.h"
#include "wms.h"
#include "dsatsmsi.h"
#include "dsm.h"

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

#define VALID_CPMS(x) if((x <= CPMS_NONE)||(x >= CPMS_MAX))\
{\
  MSG_ERROR("Invalid CPMS memory type",0,0,0);\
  return 0;\
}

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for external variables 
  that are declared else where in ATCOP SMS SUB SYSTEM
===========================================================================*/
/*--------------------------------------------------------------------------
  import variable from other files in sms module
---------------------------------------------------------------------------*/

/* Variable to find out what command is being processed currently */
extern sms_cmd_name_e_type  dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* Variable used in CMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || \
    defined(FEATURE_DSAT_FLOW_CONTROL)
LOCAL dsat_result_enum_type etsismsi_reenter_read_msg_list
(
  dsat_mode_enum_type mode,
  ds_cmd_type* cmd_ptr
);
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_DSAT_FLOW_CONTROL */

#ifdef FEATURE_DSAT_ACCDATA_ENABLED
#error code not present
#endif /* FEATURE_DSAT_ACCDATA_ENABLED */

LOCAL byte * itoa_fill
(
  word v,       /* Number to convert   */
  byte *rb_ptr  /* Destination Pointer */ 
);

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION    DSATSMSI_MEMORY_LOOKUP

DESCRIPTION
  Converts the type of preferred memory storage into a required enum.
  
DEPENDENCIES
  None

RETURN VALUE
  returns the memory type enum:
  WMS_MEMORY_STORE_NV_GW        (ETSI only)
  WMS_MEMORY_STORE_SIM          (ETSI only)
  WMS_MEMORY_STORE_RUIM         (CDMA only)
  WMS_MEMORY_STORE_NV_CDMA      (CDMA only)

SIDE EFFECTS
  None

===========================================================================*/

wms_memory_store_e_type dsatsmsi_memory_lookup
( 
 int mem_val,                   /* value of the mem index to be mapped */ 
 cpms_mem_e_type mem_type       /* which memory (<mem[1-3]>) to mapped */
)
{
  wms_memory_store_e_type return_val = WMS_MEMORY_STORE_NONE;
  const dsat_string_item_type * mem_valstr;
  int index;

typedef struct {
  wms_memory_store_e_type     wms_memory;
  char                        mem_str[3];
} smsu_wms_mem_table_type;

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
  static const smsu_wms_mem_table_type 
    wms_memory_is707_table[4] = 
  {
    { WMS_MEMORY_STORE_NV_CDMA, "ME" },
    { WMS_MEMORY_STORE_NV_CDMA, "MT" },
    { WMS_MEMORY_STORE_RUIM,    "SM" },
    { WMS_MEMORY_STORE_RUIM,    "SR" },
  };
#endif /* FEATURE_CDMA_SMS */

  const smsu_wms_mem_table_type * wms_memory_table;
  dsati_operating_cmd_mode_type op_mode;
  const def_list_type           * mem_def;

  op_mode = dsatcmdp_get_operating_cmd_mode();
  
  switch(op_mode)
  {
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
    case CDMA_CMD_MODE:
      wms_memory_table = wms_memory_is707_table;
      break;
#endif /* FEATURE_CDMA_SMS */
      
    default:
      /* please compiler */
      wms_memory_table = NULL;
      ERR_FATAL("wrong op_mode %d",(int)op_mode,0,0);
  }

  /* get the right mem_valstr */
  mem_def = dsatsmsi_get_mem_list(mem_type);

  /* sanity check */
  if ( mem_def == NULL )
  {
    /* Error fatal already happened, klocwork doesn't know this */
    return return_val;
  }

  mem_valstr = mem_def->list_v[mem_val];

  /* look for the wms_memory by mem_str */
  for (index = 0; index < 4; index++)
  {
    if(strcmp(wms_memory_table[index].mem_str, 
              (char *) mem_valstr) == 0)
    {
      return_val = wms_memory_table[index].wms_memory;
      break;
    }
  }

  return return_val;
} /* dsatsmsi_memory_lookup */


/*===========================================================================

FUNCTION    DSATSMSI_SEND_ASYNC_INDICATION_TO_TE

DESCRIPTION
  If ATCOP get an unsolicited asynchronous event, This function formats 
  the data that needs to be sent to TE. It handles both SMS-DELIVERs and
  SMS-STATUS-REPORTs.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns number of bytes that are used in formatting the result buffer

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatsmsi_send_async_indication_to_te
(
 sms_mt_msg_e_type       msg_type,   /* Type of message       */
 wms_message_index_type  msg_index,  /* Index Number          */
 wms_memory_store_e_type mem_store   /* Type of Memory Store  */
)
{
  char *data_ptr;
  dsm_item_type * res_buff;
  dsm_item_type * formatted_res_buff;

#ifdef FEATURE_DSAT_ACCDATA_ENABLED
#error code not present
#endif /* FEATURE_DSAT_ACCDATA_ENABLED */

  if( (res_buff = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL )) == NULL)
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }
  else
  {
    data_ptr = (char *)res_buff->data_ptr;

    if ( msg_type == MT_DELIVER )
    {
      if ( dsatcmdp_get_operating_cmd_mode() == CDMA_CMD_MODE )
      {
        res_buff->used += (word)std_strlprintf((char*)data_ptr,
                                               res_buff->size - res_buff->used,
                                               "$QCMTI: ");
      }
      else
      {
       res_buff->used += (word)std_strlprintf((char*)data_ptr,
                                               res_buff->size - res_buff->used,
                                               "+CMTI: ");
       }
    }
    else
    {
      res_buff->used += (word)std_strlprintf((char*)data_ptr,
                                             res_buff->size - res_buff->used,
                                             "+CDSI: ");
    }
    data_ptr = (char *) res_buff->data_ptr + res_buff->used;

    if ( msg_type == MT_STATUS_REPORT )
    {
      res_buff->used += (word) std_strlprintf ((char*)data_ptr,
                                               res_buff->size -  res_buff->used,
                                               "\"SR\"");
    }
    else
    {
      if ( dsatcmdp_get_operating_cmd_mode() == CDMA_CMD_MODE )
      {
        const def_list_type * list_ptr = dsatsmsi_get_mem_list(CPMS_MEM3);
        if (list_ptr != NULL )
        {
          res_buff->used += (word) std_strlprintf(data_ptr,
                           res_buff->size - res_buff->used  ,"\"%s\"",
                           list_ptr->list_v[dsatsmsi_get_mem_val(CPMS_MEM3)]);
        }
      }
      else
      {
        const def_list_type * list_ptr = dsatsmsi_get_mem_list(CPMS_MEM3);
        if (list_ptr != NULL )
        {
          res_buff->used += (word) std_strlprintf(data_ptr,
                           res_buff->size - res_buff->used  ,"\"%s\"",
                           list_ptr->list_v[dsatsmsi_get_mem_val(CPMS_MEM3)]);
        }
      }
    }
    data_ptr = (char *)res_buff->data_ptr + res_buff->used;

    *data_ptr++ = ',';
    res_buff->used++;

    res_buff->used += (word)std_strlprintf(data_ptr,
                                           res_buff->size - res_buff->used,
                                           "%d",(int)msg_index);
    data_ptr = (char *) res_buff->data_ptr + res_buff->used;

    *data_ptr = '\0';
  }
  
  /* We are done with filling up raw response buffer */
  /* Format it and send it to TE                     */
  if( (formatted_res_buff = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL )) 
       == NULL)
  {
    ERR_FATAL( "Could not get a new DSM item", 0, 0, 0);
  }
  /* send the response to the user */
  dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                     &formatted_res_buff );
  
  if ( formatted_res_buff == NULL )
  {
    ERR_FATAL( "Running out of buffers",0,0,0 );
  }
  
  /* Flush the queue to the Rm interface. */
  dsatcmdp_send_queued_cmd_results( );

  /* Check the formatted response data item. If it is not empty, send it
     to the Rm interface. */
  if ( formatted_res_buff->used != 0 )
  {
#ifdef FEATURE_DSAT_ACCDATA_ENABLED
#error code not present
#endif /* FEATURE_DSAT_ACCDATA_ENABLED */

#ifdef FEATURE_DATA_SERIALIZER
    /* Tell SIOLIB that this is SMS and should be sent to Modem port */
    ds3g_siolib_set_tx_type 
      ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT );
#endif /* FEATURE_DATA_SERIALIZER */

    dsatrsp_send_response( formatted_res_buff, TRUE );

#ifdef FEATURE_DSAT_ACCDATA_ENABLED
#error code not present
#endif /* FEATURE_DSAT_ACCDATA_ENABLED */

  }
  else
  {
    (void) dsm_free_buffer( formatted_res_buff );
  }

  return;
} /* dsatsmsi_send_async_indication_to_te */


/*===========================================================================

FUNCTION    DSATSMSI_DELETE_MESSAGES

DESCRIPTION
  This function is used for +CMGD/$QCMGD command when a <delflag> is present.
  Tries to get the list of message with that particular <delflag> in memory.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_messages
(
  const byte * del_flag                  /* pointer to the delete flag */
)
{
  wms_memory_store_e_type mem_store;
  dsat_result_enum_type result;
  wms_status_e_type msg_status;

  switch (*del_flag)
  {
    case '1':
      dsatsmsi_cmd_sm.num_of_stat = 1;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      break;
    case '2':
      dsatsmsi_cmd_sm.num_of_stat = 2;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      break;
    case '3':
      dsatsmsi_cmd_sm.num_of_stat = 3;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_NOT_SENT;
      break;
    case '4':
      dsatsmsi_cmd_sm.num_of_stat = 4;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_NOT_SENT;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MT_NOT_READ;
      break;
    default:
      return DSAT_ERROR;
  }
  
  /* We have to get a list */
  /* Seperate it from just one index */
  dsatsmsi_cmd_sm.one_index = 0;
  dsatsmsi_cmd_sm.is_test =  0;

  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1), 
                                     CPMS_MEM1);
  /* Get the message list first and then delete from the list */
  msg_status = wms_cfg_get_message_list(
                                          dsatsmsi_client_id,
                                          dsatsmsi_cmd_cb_func,
                                          (void *)&dsatsmsi_pres_cmd,
                                          mem_store,
                                          WMS_TAG_MT_READ
                                          );
  if ( msg_status == WMS_OK_S )
  {
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result ;
} /* dsatsmsi_delete_messages */


/*===========================================================================

FUNCTION DSATSMSI_DELETE_MSG_LIST

DESCRIPTION
  Local function that is used to delete a message from the list of indices.
  Handles one list or lists of multiple <stat>s.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_msg_list(void )
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  dsat_result_enum_type result;

  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1), 
                                     CPMS_MEM1);
  if ( dsatsmsi_cmd_sm.num_of_indices != 0 )
  {
    msg_status = wms_msg_delete(
                                  dsatsmsi_client_id,
                                  dsatsmsi_cmd_cb_func,
                                  (void*)&dsatsmsi_pres_cmd,
                                  mem_store,
                                  dsatsmsi_cmd_sm.indices[0]
                                 );
    /* Always read from the tip */
    dsatsmsi_cmd_sm.num_of_indices--;
    dsatsmsi_cmd_sm.present_index = 0;
    if (msg_status == WMS_OK_S)
    {
      /* still more to come */
      /* tell ATCOP about that */
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error      */
      /* return with an error */ 
      result = DSAT_CMD_ERR_RSP;
    }
  }
  else if ( dsatsmsi_cmd_sm.num_of_stat > 1 )
  {
    if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
    {
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
      dsatsmsi_cmd_sm.present_stat++;
      msg_status = wms_cfg_get_message_list(
                                              dsatsmsi_client_id,
                                              dsatsmsi_cmd_cb_func,
                                              (void *)&dsatsmsi_pres_cmd,
                                              mem_store,
                                              tag
                                             );
      /* Tell ATCOP that there is mode to come */
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* We are finally done */
      /* tell ATCOP we are done */
      result = DSAT_OK;
    }
  }
  else 
  {
    /* this is a case of just single index delete or */
    /* we are done deleting messages                 */
    result = DSAT_OK;
  }

  return result;

}/* dsatsmsi_delete_msg_list */


/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_VAL

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_val[mem_index]/
             dsat707sms_qcpms_val[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a dsat_num_item_type.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatsmsi_get_mem_val
(
 cpms_mem_e_type mem_index   /* which memory (mem[1-3]) setting we are 
                                looking for                             */
)
{
  dsat_num_item_type            mem_val;
  dsati_operating_cmd_mode_type cmd_mode;

  VALID_CPMS(mem_index);
  cmd_mode = dsatcmdp_get_operating_cmd_mode();

#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    mem_val = dsat707sms_qcpms_val[mem_index];
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    ERR("We are in wrong mode: %d",cmd_mode,0,0);
    return 0;
  }

  return mem_val;

} /* dsatsmsi_get_mem_val */


/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_LIST

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_list[mem_index]/
             dsat707sms_qcpms_list[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a def_list_type array.

SIDE EFFECTS
  None

===========================================================================*/
const def_list_type * dsatsmsi_get_mem_list
(
 cpms_mem_e_type mem_index   /* which memory (<mem[1-3]) setting we are 
                                looking for                             */
)
{
  const def_list_type           * mem_list;
  dsati_operating_cmd_mode_type cmd_mode;

  VALID_CPMS(mem_index);
  cmd_mode = dsatcmdp_get_operating_cmd_mode();

#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    mem_list = &dsat707sms_qcpms_list[mem_index];
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    ERR("We are in wrong mode: %d",cmd_mode,0,0);
    return 0;
  }

  return mem_list;

} /* dsatsmsi_get_mem_list */


/*===========================================================================

FUNCTION    DSATSMSI_TAG_LOOKUP

DESCRIPTION
  Converts the message TAG type (wms_message_tag_e_type) to a string on
  text mode or to an integer on a pdu mode and writes the result into the
  result buffer
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None

===========================================================================*/
byte * dsatsmsi_tag_lookup
( 
 wms_message_tag_e_type tag,   /* TAG enum             */
 boolean mode,                 /* mode: PDU or TEXT    */
 byte * res_ptr                /* ptr to result buffer */
)
{
  char tag_table[5][11] =
  {"REC UNREAD","REC READ","STO UNSENT","STO SENT","ALL"};
  byte i = 0, j = 0;
  wms_message_tag_e_type tag_array[4] = 
  { 
    WMS_TAG_MT_NOT_READ, 
    WMS_TAG_MT_READ, 
    WMS_TAG_MO_NOT_SENT, 
    WMS_TAG_MO_SENT 
  };

  while ( i < 4 )
  {
    if ( tag == tag_array[i] )
    {
#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */
      {
        /* mode FALSE: TEXT mode */
        *res_ptr++ = '"';
        while(tag_table[i][j] != '\0')
        {
          *res_ptr++ =  tag_table[i][j++];
        }
        *res_ptr++ = '"';
      }

      return res_ptr;
    }
    else
    {
      i ++;
    }
  }

  return res_ptr;

}/* dsatsmsi_tag_lookup */


/*===========================================================================

FUNCTION    DSATSMSI_STR_TO_ADDR

DESCRIPTION
  Checks to see if <toda> field is specified by the user.
  If not, parses the <da> string for address length, type of number and 
  numbering plan.
  If <toda> filed is specified, <toda> is split into type of number
  and numberign plan.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_str_to_addr
(
 wms_address_s_type      *addr,   /* Pointer to address structure to 
                                     be filled                             */
 byte                    * da,    /* Pointer to destination address        */
 const byte              * toda,  /* Pointer to type of address            */
 dsatsms_fmt_mode_e_type sms_mode /* SMS mode Enum                         */
)
{
  uint8 loop=0, toa=0, toda_type;
  byte * s;
  byte c;
  dsat_num_item_type type_of_address;
  boolean result = TRUE;

  /*check to see if optional <da> and <toda> are supplied by the user*/
  /* new <da>: no type of address */
  if ((toda == NULL) || (*toda == NULL))
  {
    /* if <da> begins with '+' type-of-address is 145 */
    if ( *da == '+' )
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
      da++;
    }
    else /* default type-of-address is 129 */
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_UNKNOWN;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    }
  }
  /* We got a type of address */
  else
  {
    if ( sms_mode == DSATSMS_TEXT_MODE )
    {
      /* It is a decimal number if we are in text mode */
      toda_type = 10;
    }
    else
    {
      /* It is a hex number if we are in pdu mode */
      toda_type = 16;
    }

    if( dsatutil_atoi(&type_of_address, toda, toda_type ) != ATOI_OK)
    {
      /* Bad type of address */
      return FALSE;
    }

    if( type_of_address > 255 )
    {
      /* Over flow */
      return FALSE;
    }

    toa = (uint8)type_of_address;
    addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
    addr->number_type = (wms_number_type_e_type)((toa & 0x70)>>4);
    addr->number_plan = (wms_number_plan_e_type)(toa & 0x0F);

    /* Ignore the '+' */
    if ( *da == '+' )
    {
      da++;
      /* Needs to be International */
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
    }
  }
  
  s = da;
  /* Initializing the number mode*/
  addr->number_mode = WMS_NUMBER_MODE_DATA_NETWORK_MAX32;

  while((c = *s++) != '\0')
  { 
    dsat_dial_val_e_type vresult = dsatutil_validate_dial_char(&c);

    if (DSAT_DIAL_DIGIT == vresult)
    {
      c -= '0';
    }
    else if (DSAT_DIAL_PLUS == vresult)
    {
      return FALSE;
    }
    else
    {
      if ( sms_mode == DSATSMS_TEXT_MODE )
      {
        switch(vresult)
        {
          case DSAT_DIAL_ASCII_ABC:
            c -= 53;  /* 'A' - 12 */
            break;
          case DSAT_DIAL_STAR:
            c = 10;
            break;
          case DSAT_DIAL_POUND:
            c = 11;
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }
      else /* SMS_PDU_MODE */
      {
        /* Mapping in PDU mode: */
        /*   'A' --> '*'        */
        /*   'B' --> '#'        */
        /*   'C' --> 'A'        */
        /*   'D' --> 'B'        */
        /*   'E' --> 'C'        */
        switch(vresult)
        {
          case DSAT_DIAL_ASCII_ABC:
          case DSAT_DIAL_ASCII_D:
          case DSAT_DIAL_ASCII_E:
            c -= 55;  /* 'A' - 10 */
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }/* else */
    }
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS */
    loop++;
    if(loop > 20)
    {
      return FALSE;
    }
  }/* while "" */

#ifdef FEATURE_CDMA_SMS
  da[loop] = '\0';
  if(wms_ts_ascii2dtmf((const char *) da,(uint8 *) &addr->digits[0])== 0)
  {
    MSG_ERROR("Unsupported Address character",0,0,0);
    return FALSE;
  }
#endif
  
  addr->number_of_digits = loop;
  if(!addr->number_of_digits)
  {
    MSG_HIGH("No Number available",0,0,0);
    return FALSE;
  }
  
  return result;
}/* dsatsmsi_str_to_addr */


/*===========================================================================

FUNCTION    DSATSMSI_CMS_ERROR

DESCRIPTION
  Fills a response buffer with a +CMS ERROR.
  
DEPENDENCIES
  None

RETURN VALUE
  returns number of bytes that are used in filling up the response buffer.

SIDE EFFECTS
  None

===========================================================================*/
unsigned int dsatsmsi_cms_error
(
  const dsm_item_type *res_buff_ptr,             /*  Place to put response    */
  int                  error                     /*  Error Number             */
)
{
  byte * w_ptr;
  uint16 buff_len = 0;
  
  w_ptr = res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;


  w_ptr += (word)std_strlprintf((char*)w_ptr, buff_len, "+CMS ERROR: ");

  w_ptr = dsatutil_itoa((uint32)error, w_ptr, 10);

  return (unsigned int) (w_ptr - (res_buff_ptr->data_ptr + res_buff_ptr->used));
}/* dsatsmsi_cms_error */


/*===========================================================================

FUNCTION    DSATSMSI_STRIP_QUOTES

DESCRIPTION
  Strips out quotes from the string that is wrapped in quotes.
  Resultant string will be placed in the out pointer.
  Gives an error if the string does not begin or end with a quote
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean : 
    TRUE : if successful in stripping the quotes out
    FALSE : if there is any error

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_strip_quotes
(
  const byte * in_ptr,   /* Pointer to the string to be processed  */
  byte * out_ptr         /* Pointer to the resultant string buffer */
) 
{
  if(*in_ptr != '\0' )
  {
    if( *in_ptr++ == '"' )
    {
      while ( *in_ptr != '"' && *in_ptr != '\0' )
      {
        *out_ptr++ = *in_ptr++;
      }
      /* Check to see if the strring ends with a null */
      if( *in_ptr == '\0' )
      {
        /* We got a string without ending quotes */
        return FALSE;
      }
      else
      {
        /* Everything is happy */
        *out_ptr = '\0';
        return TRUE;
      }
    }
    else
    {
      /* We got a string with out quotes */
      return FALSE;
    }    
  }
  else
  {
    return FALSE;
  }
} /* dsatsmsi_strip_quotes */

/*===========================================================================

FUNCTION DSATSMSI_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFF and a radix of 2,
  which returns 17 bytes (16 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatsmsi_itoa
(
  uint32 v,       /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  word r          /*  Conversion radix              */
)
{
  byte buf[17], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      MSG_HIGH ("OVERFLOW", 0, 0, 0);
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  dsatsmsi_itoa */


#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_PS
#error code not present
#endif /* FEATURE_ETSI_SMS_PS */

/*===========================================================================

FUNCTION DSATSMSI_READ_MSG_LIST

DESCRIPTION
  Local function that is used to read a message from the list of indices.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the request to wms to read the msg is accepted
       or if the function quits because of ds to sio watermake high.
  FALSE if the request to wms to read the msg is rejected


SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_read_msg_list(void )
{
  wms_memory_store_e_type mem_store;
  wms_status_e_type msg_status;
  boolean result;
  wms_message_index_type index;
  const dsat_string_item_type * mem_valstr;
  const def_list_type * list_ptr;
  dsat_num_item_type mem_val;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  mem_val = dsatsmsi_get_mem_val(CPMS_MEM1);
  mem_store = dsatsmsi_memory_lookup((int)mem_val,
                                     CPMS_MEM1);
  index = dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index];

#ifdef FEATURE_DSAT_FLOW_CONTROL
  if(dsati_tx_wm_high)
  {
    dsati_to_sio_low_handler = etsismsi_reenter_read_msg_list;
    return TRUE;
  }
#endif /* FEATURE_DSAT_FLOW_CONTROL */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  list_ptr = dsatsmsi_get_mem_list(CPMS_MEM1);
  if(list_ptr == NULL)
  {
    MSG_ERROR("SMS memory settings unavailable",0,0,0);
    return FALSE;
  }
  mem_valstr = list_ptr->list_v[mem_val];
  /* Check if the CMGL is for Status report */
  if ( strcmp ("SR",(char *)mem_valstr) == 0)
  {
    msg_status = wms_msg_read_sts_report(
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                mem_store,
                                index
                                );
  }
  else
  {
    msg_status = wms_msg_read(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              mem_store,
                              index
                              );
  }
  dsatsmsi_cmd_sm.present_index++;

  /* Check to see if we got an error */
  if (msg_status == WMS_OK_S)
  {
    result = TRUE;
  }
  else 
  {
    /* we got an error */
    result = FALSE;
  }

  return result;
}/* dsatetsismsi_read_msg_list */

/*===========================================================================

FUNCTION DSATSMSI_GET_MSG_LIST

DESCRIPTION
  Local function that is used to get a message list.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  Boolean.
  TRUE: If the next message list was asked successfully
  FALSE: If there is an error or if we are done reading the lists

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_get_msg_list(void )
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  boolean result = FALSE;

  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1), 
                                     CPMS_MEM1);

  if ( dsatsmsi_cmd_sm.num_of_stat > 1 )
  {
    if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
    {
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
      dsatsmsi_cmd_sm.present_stat++;
      msg_status = wms_cfg_get_message_list(
                                              dsatsmsi_client_id,
                                              dsatsmsi_cmd_cb_func,
                                              (void *)&dsatsmsi_pres_cmd,
                                              mem_store,
                                              tag
                                             );
      if( msg_status == WMS_OK_S )
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
    else
    {
      result = FALSE;
    }
  }

  return result;
}/* dsatsmsi_get_msg_list */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || \
    defined(FEATURE_DSAT_FLOW_CONTROL)
/*===========================================================================

FUNCTION ETSISMSI_REENTER_READ_MSG_LIST

DESCRIPTION
  Local function that is used to reenter reading a message from the list 
  of indices: it's called to reenter this processing following ATCOP to SIOLIB
  flow control or after waiting for DSM items to be freed by USB when listing
  many SMSes.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_ASYNC_CMD: if the request to wms to read the msg is accepted
  DSAT_ERROR    : if the request to wms to read the msg is rejected

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsismsi_reenter_read_msg_list
(
  dsat_mode_enum_type mode,
  ds_cmd_type* cmd_ptr
)
{
  boolean read_msg_result;
  dsat_result_enum_type result;

  /* Reset handler function pointers */
#ifdef FEATURE_DSAT_FLOW_CONTROL
  dsati_to_sio_low_handler = NULL;
#endif /* FEATURE_DSAT_FLOW_CONTROL */
  dsati_handle_free_item_timer_expiry = NULL;
  
  read_msg_result = dsatsmsi_read_msg_list();

  if(read_msg_result)
  {
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* etsismsi_reenter_read_msg_list */

#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_DSAT_FLOW_CONTROL */

/*===========================================================================

FUNCTION    ITOA_FILL

DESCRIPTION
  Converts an integer to a string. With leading zeros. Assumes that it is a
  2 digit number.
  
DEPENDENCIES
  None

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL byte * itoa_fill
(
  word v,       /* Number to convert   */
  byte *rb_ptr  /* Destination Pointer */ 
)
{
  int n;
  byte c, *ptr;
  if( rb_ptr == NULL )
  {
      ERR_FATAL("Null rb_ptr",0,0,0);
  }
  ptr = rb_ptr + 2;
  *ptr = '\0';

  for (n = 0; n < 2; ++n)
  {
    c = (byte) (v % 10);
    v /= 10;
    *--ptr = (c + '0');
  }
  
  return rb_ptr + 2;
} /*  itoa_fill */


/*===========================================================================

FUNCTION    DSATSMSI_TIMESTAMP

DESCRIPTION
  Converts the message SC timestamp type (wms_timestamp_s_type) to a 
  string. It converts it into "yy/MM/dd,hh:mm:ss+/-zz" format.
  writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatsmsi_timestamp
( 
 wms_timestamp_s_type  time,  /* Pointer to Time Stamp        */
 byte * res_ptr                /* Pointer to the result buffer */
)
{
  uint8 tmp;
  if( res_ptr == NULL )
  {
      ERR_FATAL("Null res_ptr",0,0,0);
  }
  /* Convert BCDs to strings */
  *res_ptr++ = '"';
  tmp = time.year;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10), 
                       res_ptr ); 
  *res_ptr++ = '/';
  tmp = time.month;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                       res_ptr );
  *res_ptr++ = '/';
  tmp = time.day;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                       res_ptr);
  *res_ptr++ = ',';
  tmp = time.hour;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                       res_ptr);
  *res_ptr++ = ':';
  tmp = time.minute;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                       res_ptr);
  *res_ptr++ = ':';
  tmp = time.second;
  res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                       res_ptr);

  if( time.timezone < 0 )
  {
    *res_ptr++ = '-';
    tmp = (uint8)(time.timezone * (-1));
  }
  else
  {
    *res_ptr++ = '+';
    tmp = (uint8)time.timezone;
  }
  res_ptr = itoa_fill( tmp, res_ptr);
  *res_ptr++ = '"';

return res_ptr;
}/* dsatsmsi_timestamp */


#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#endif /* FEATURE_DATA */
