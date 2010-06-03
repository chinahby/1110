
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
  dsat707smsi_route_config
    Sets the SMS message routing configuration for CDMA mode.

  dsat707smsi_string_to_timestamp
    Function converts string of "yy/MM/dd,hh:mm:ss" format to
    wms_timestamp_s_type struct.

  dsat707smsi_teleservice_num_to_enum
    Function  convert teleservice number to wms teleservice enum type.

   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707smsu.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/09   bs      Fixed compilation errors/warnings.
01/21/09   nc      Added support for SMS Commands.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "err.h"

/* this file should be included only for SMS for CDMA MODE */
#ifdef FEATURE_CDMA_SMS
#include <stdlib.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707smsctab.h"
#include "string.h"

/*===========================================================================
                        DEFINES AND VARIABLES
===========================================================================*/

/* For function dsat707smsi_string_to_timestamp */
#define                 TIME_TOK_NUM    6
#define                 TIME_TOK_LEN    2

/* Variable to find out what command is being processed currently */
extern sms_cmd_name_e_type dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION    DSAT707SMSMI_ROUTE_CONFIG
DESCRIPTION
  Sets the SMS message routing configuration for CDMA mode.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: wms accepts the config route command
  DSAT_ERROR:     wms rejects the config route command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707smsi_route_config
(
  dsat_num_item_type mt_val /* what type of deliver routing we are setting */
)
{
  dsat_result_enum_type result;
  wms_status_e_type status;
  wms_routes_s_type route;
  int i;

  /* leave the GW message classes untouched */
  for( i = 0 ; i < WMS_MESSAGE_CLASS_CDMA ; i++ )
  {
    route.pp_routes[i].route = WMS_ROUTE_NO_CHANGE;
    route.pp_routes[i].mem_store = WMS_MEMORY_STORE_NONE;
  }
  
  route.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_STORE_AND_NOTIFY;
  route.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = 
  dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[2], CPMS_MEM3);

/* If mt_val is 2 we Donot Store the message but route it to TE */
  if ( mt_val == 2 )
  {
    route.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_TRANSFER_AND_ACK;
  }

  
  /* Now set the routing configuration */
  status = wms_cfg_set_routes (
                               dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void *) &dsatsmsi_pres_cmd,
                               &route
                               );
  if ( status == WMS_OK_S )
  {
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}/* dsat707smsi_route_config */

/*===========================================================================

FUNCTION DSAT707SMSI_STRING_TO_TIMESTAMP

DESCRIPTION
  Function converts string of "yy/MM/dd,hh:mm:ss" format to
  wms_timestamp_s_type struct.

  If ts_ptr is NULL, the function only checks the format of the time string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : The time_str has good format.
         The ts_ptr contains the output if it is not NULL.
  FALSE: The time_str has good format.
         The ts_ptr does not change.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsat707smsi_string_to_timestamp
(
  const char            * time_str,     /* in: string of "yy/MM/dd,hh:mm:ss" 
                                         *     format. Maybe quouted or 
                                         *     unquoted                    */
  wms_timestamp_s_type  * ts_ptr        /* out                             */
)
{
  int                   str_index, tok_index, ii;
  uint8                 *time_ptr;
  
  wms_timestamp_s_type  timestamp;
  char                  tmp_tok[TIME_TOK_LEN+1];
  boolean               result;

  char    seperators[TIME_TOK_NUM+1]   = "//,::\0";

  const int     up_lim[TIME_TOK_NUM] = {99, 12, 31, 23, 59, 59};

/*-------------------------------------------------------------------------*/

  memset(&timestamp, 0, sizeof(wms_timestamp_s_type));
  time_ptr = (uint8 *) &timestamp;

  str_index = 0;
  if(time_str[str_index] == '"')
  {
    str_index++;
    /* the ...:ss is followed by '"' */
    seperators[TIME_TOK_NUM - 1] ='"';
  }

  /* check the format */
  for (str_index = 1, tok_index = 0;
       time_str[str_index] != '\0' && tok_index < TIME_TOK_NUM;
       str_index += (TIME_TOK_LEN + 1), tok_index++, time_ptr++)
  {
    if(time_str[str_index + TIME_TOK_LEN] == seperators[tok_index])
    {
      for (ii = 0; ii < TIME_TOK_LEN; ii++)
      {
        tmp_tok[ii] = time_str[str_index + ii];
      }
      tmp_tok[TIME_TOK_LEN] = '\0';

      /* write to the timestamp */
      *time_ptr = (uint8) atoi(tmp_tok);
      if( *time_ptr > up_lim[tok_index] )
      {
        /* time out of range */
        break;
      }
    }
    else
    {
      /* the separator does not match */
      break;
    }
  }

  if(tok_index == TIME_TOK_NUM)
  {
    /* all 6 token has been converted and their ranges have been checked 
     */
    result = TRUE;

    if(ts_ptr != NULL)
    {
      *ts_ptr = timestamp;
    }
  }
  else
  {
    result = FALSE;
  }

  return result;
} /* dsat707smsi_string_to_timestamp */

/*===========================================================================

FUNCTION DSAT707SMSI_TELESERVICE_NUM_TO_ENUM

DESCRIPTION
  Function  convert teleservice number to wms teleservice enum type.

DEPENDENCIES
  None

RETURN VALUE
  wms teleservice enum.

SIDE EFFECTS
  None

===========================================================================*/
wms_teleservice_e_type dsat707smsi_teleservice_num_to_enum
(
 char * num_str
)
{
  /* the function is used to convert qcsmp_val's first argument.
     The arguments are guarrenteed to be convertable.
     So, no error checking is done here.
  */
  return (wms_teleservice_e_type) atoi(num_str);
}

/*===========================================================================

FUNCTION    DSAT707SMSI_ADDR_TO_STR

DESCRIPTION
  Converts the TPDU address type (wms_address_s_type) to a 
  string. writes the result into the result buffer.
    
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of the result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsat707smsi_addr_to_str
( 
 wms_address_s_type addr,  /* Address data structure           */
 uint8 * type_of_addr,     /* pointer to type of address field */
 byte * res_ptr           /* pointer to result buffer         */
)
{
  byte bcd_idx=0;
  uint8 temp = 0;
  

  /* Address as BCD is first converted to IRA in this buffer */
  char addr_ira[WMS_ADDRESS_MAX + 1];
  char *addr_ira_ptr = &addr_ira[0];
  
  if ( ( res_ptr == NULL ) || ( type_of_addr == NULL ) )
  {
    ERR_FATAL("Null res_ptr",0,0,0);
  }

  /* Over-running array element but not entire array:
     disable Lint error */
  memset( &addr_ira[0], 0, sizeof(addr_ira) ); /*lint !e419*/

  temp = (uint8)((uint32)addr.number_type & 0x07);
  *type_of_addr = (uint8)(( *type_of_addr | temp ) << 4);
  temp = (uint8)((uint32)addr.number_plan & 0x07);
  *type_of_addr = *type_of_addr | temp ;
  *type_of_addr = *type_of_addr | 0x80;

  *res_ptr++ = '"';

  /* Address as BCD is first converted to NULL terminated IRA into 
     addr_ira[] buffer. */
  /* Add '+' if it is a international number */
  /* Do it only if we are in text mode       */
  if ( *type_of_addr == 145 )
  {
    *addr_ira_ptr++ = '+';
  }
  /* As per Spec GSM 04.08, In the case where a semi-octet represented 
  field comprises an odd number of digits, the bits with bit numbers 4 to 7
  within the last octet are fill bits and shall always be set to "1111".
  If a mobile receives an address field containing non-integer information
  in the semi-octets other than "1111" (e.g. 1110)it shall display the 
  semi-octet as the representation given in GSM 04.08 under "called BCD number", 
  viz 1010="*",1011="#", 1100="a", 1101="b", 1110="c". 
  In the event of a discrepancy between the values quoted here and the values
  specified in GSM 04.08 then GSM 04.08 shall take precedence.  */
  
  while(bcd_idx < addr.number_of_digits)
  {

      switch(addr.digits[bcd_idx])
      {
          case 12:
            *addr_ira_ptr++ = 'A';
            break;
          case 13:
            *addr_ira_ptr++ = 'B';
            break;
          case 14:
            *addr_ira_ptr++ = 'C';
            break;
          case 10:
            *addr_ira_ptr++ = '*';
            break;
          case 11:
            *addr_ira_ptr++ = '#';
            break;
          default:
            addr_ira_ptr = 
              (char *)dsatutil_itoa( (uint32) addr.digits[bcd_idx], 
                                     (byte *)addr_ira_ptr, 
                                     10 );
            break;
      }/* switch */

      *addr_ira_ptr = '\0';
    
    bcd_idx++;
  }
  for( temp =0 ;temp < (uint8)(addr_ira_ptr - addr_ira) ;temp ++)
  {
    *res_ptr++  = (byte)addr_ira[temp] ;
  }

  /* sanity check before updating */
  if ( res_ptr != NULL )
  {
    *res_ptr++ = '"';
  }

  return res_ptr;

} /* dsat707smsi_addr_to_str */

#endif /* FEATURE_CDMA_SMS */

#endif /* FEATURE_DATA */
