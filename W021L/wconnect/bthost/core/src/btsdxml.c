/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              B L U E T O O T H    S D P    X M L    M O D U L E

GENERAL DESCRIPTION
  This contains the Bluetooth Service Discovery Protocol (SDP) XML Module

Copyright (c) 2007-2008 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsdxml.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-09-08   rs  Custom service registration to frame the DES headers as well
                   before populating the record
  2008-04-04   hm  Fixed klocwork errors

 *
 *    #6       17 Jan 2008            PG
 * Resolved Compiler Warnings
 *
 *    #5       14 Dec 2007            HM
 * Added support to parse white spaces in the XML strings. Added support
 * for to parse HID descriptor lists. Cleanup and fixes.
 *
 *    #4       05 Dec 2007            HM
 * Fixed a compile error for the 2007.11 branch
 *
 *    #3       03 Dec 2007            HM
 * Added support to parse XML
 *
 *    #2       03 Dec 2007            PN
 * Added error checking.  Splitted bt_sd_create_xml_service_record() into
 * bt_sd_create_xml_service_db() and bt_sd_create_xml_service_record().
 * Used safe string functions throughout.
 *
 *    #1       29 Nov 2007            HM
 * Initial Revision. Added support to convert an SD database into XML 
 * format.
 * 
 *
 *
 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <stdio.h>
#include <string.h>
#include "btsd.h"
#include "btsdxml.h"
#include "AEEstd.h"
#include "btmsg.h"
#include "assert.h"
#include "msg.h"

#define BT_MSG_LAYER  BT_MSG_SD   /* necessary for btmsg.h */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Declarations                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Static Functions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION
  bt_sd_xml_is_white_space

DESCRIPTION
  Returns TRUE if character is a white space character
  
===========================================================================*/
boolean bt_sd_xml_is_white_space
( 
  char p
)
{
  if ( p == ' ' || p == '\t' || 
       p == '\n' || p =='\r' )
    return TRUE;
  else
    return FALSE;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_skip_white_space

DESCRIPTION
  
===========================================================================*/
void bt_sd_xml_skip_white_space
(
  char**    str,
  uint16*   len
)
{
  char *tail = NULL;
  
  tail = *str + *len;
  while( *str < tail )
  {
    if( bt_sd_xml_is_white_space( **str ) )
    {
      --*len;
      ++*str;
    } 
    else
    {
      break;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_sd_xml_check_white_space

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_check_white_space
(
  char*   str,
  uint16  len
)
{
  boolean success = TRUE;
  char    *tail = NULL;

  tail = str + len;
  while( str < tail )
  {
    if( !bt_sd_xml_is_white_space( *str++ ) )
    {
      success = FALSE;
      break;
    }
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_trim_white_space

DESCRIPTION
  
===========================================================================*/
void bt_sd_xml_trim_white_space
(
  char**    str,
  uint16*   len
)
{
  uint16 len1;

  bt_sd_xml_skip_white_space( str, len );

  len1 = *len - 1;
  while( len1 )
  {
    if( !bt_sd_xml_is_white_space( (*str)[ len1 ] ) )
    {
      break;
    }
    len1 --;
  }

  *len = len1 + 1;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_ascii_to_hex

DESCRIPTION
  Validates and converts an ascii string into hex format
  
===========================================================================*/
boolean bt_sd_xml_ascii_to_hex 
( 
  char*     str, 
  uint16    str_len,
  uint64*   value
)
{

  uint64 val;
  char* tail = NULL;

  *value = 0;
  tail = str + str_len;

  while( str < tail )
  {
    val = *str++;

    if( val >= '0' && val <= '9')
    {
      val = val - '0';
    }
    else if( val >= 'A' && val <= 'F')
    {
      val = 10 + (val - 'A');
    }
    else if( val >= 'a' && val <= 'f')
    {
      val = 10 + (val - 'a');
    }
    else
    {
      return FALSE;
    }

    *value = (*value * 16) + val;
  }
  return (TRUE);
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_bool_tag

DESCRIPTION
  Creates XML representation for boolean.
  
===========================================================================*/
void bt_sd_create_xml_bool_tag( 
  boolean   bool_flag,
  char*     str,
  uint8     str_size
)
{
  char buff[ 20 ] = "\0";

  std_strlcpy( str, BT_XML_BOOL_BEGIN, str_size );
  std_strlprintf( buff, sizeof( buff ),"%s", (bool_flag==TRUE?"true":"false") );
  std_strlcat( str, buff, str_size );
  std_strlcat( str, BT_XML_BOOL_END, str_size );
  std_strlcat( str, "\n", str_size );
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_string_tag

DESCRIPTION
  Creates XML representation for character string.
  
===========================================================================*/
void bt_sd_create_xml_string_tag( 
  const char*  source,
  char*        str,
  uint8        str_size
)
{
  std_strlcpy( str, BT_XML_STRING_BEGIN, str_size );
  std_strlcat( str, source, str_size );
  std_strlcat( str, BT_XML_STRING_END, str_size );
  std_strlcat( str, "\n", str_size );
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_uint_tag

DESCRIPTION
  Creates XML representation for unsigned integers.
  
===========================================================================*/
void bt_sd_create_xml_uint_tag( 
  uint64    value,
  uint8     size,
  char*     str,
  uint8     str_size
)
{
  char    buff[ 20 ] = "\0";

  switch( size )
  {
    case 1:
      std_strlcat( str, BT_XML_UINT8_BEGIN, str_size );
      std_strlprintf( buff, sizeof( buff ), "0x%02llX", (value & 0x000000FF) );
      std_strlcat( str, buff, str_size );
      std_strlcat( str, BT_XML_UINT8_END, str_size );
      break;

    case 2:
      std_strlcat( str, BT_XML_UINT16_BEGIN, str_size );
      std_strlprintf( buff, sizeof( buff ), "0x%04llx", (value & 0x0000FFFF) );
      std_strlcat( str, buff, str_size );
      std_strlcat( str, BT_XML_UINT16_END, str_size );
      break;

    case 4:
      std_strlcat( str, BT_XML_UINT32_BEGIN, str_size );
      std_strlprintf( buff, sizeof( buff ), "0x%08llx", (value & 0x00FFFFFF) );
      std_strlcat( str, buff, str_size );
      std_strlcat( str, BT_XML_UINT32_END, str_size );
      break;

    case 8:
      std_strlcat( str, BT_XML_UINT64_BEGIN, str_size );
      std_strlprintf( buff, sizeof( buff ), "0x%016llx", value );
      std_strlcat( str, buff, str_size );
      std_strlcat( str, BT_XML_UINT64_END, str_size );
      break;
  }
  std_strlcat( str, "\n", str_size );
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_uint128_tag

DESCRIPTION
  Creates XML representation for 128-bit unsigned int.
  
===========================================================================*/
void bt_sd_create_xml_uint128_tag( 
  const byte*  uint128_val,
  char*        str,
  uint8        str_size
)
{
  uint8   i;
  char    buff[ 5 ] = "\0";

  std_strlcat( str, BT_XML_UINT128_BEGIN, str_size );
  std_strlcat( str, "0x", str_size );
  for( i=0; i<16; i++)
  {
    buff[ 0 ] = 0;
    std_strlprintf( buff, sizeof( buff ), "%02X", uint128_val[ i ] );
    std_strlcat( str, buff, str_size );
  }
  std_strlcat( str, BT_XML_UINT128_END, str_size );
  std_strlcat( str, "\n", str_size );
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_uuid16_tag

DESCRIPTION
  Creates XML representation for 16-bit UUID.
  
===========================================================================*/
void bt_sd_create_xml_uuid16_tag( 
  bt_sd_uuid_type   uuid16,
  char*             str,
  uint8             str_size
)
{
  char buff[ 20 ] = "\0";

  std_strlcpy( str, BT_XML_UUID16_BEGIN, str_size );
  std_strlprintf( buff, sizeof( buff ), "0x%04x", uuid16 );
  std_strlcat( str, buff, str_size );
  std_strlcat( str, BT_XML_UUID16_END, str_size );
  std_strlcat( str, "\n", str_size );
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_uuid128_tag

DESCRIPTION
  Creates XML representation for 128-bit UUID.
  
===========================================================================*/
void bt_sd_create_xml_uuid128_tag( 
  const bt_sd_uuid128_type*   uuid128,
  char*                       str,
  uint8                       str_size
)
{
  uint8   i;
  char    buff[ 5 ] = "\0";

  std_strlcat( str, BT_XML_UUID128_BEGIN, str_size );
  std_strlcat( str, "0x", str_size );
  for( i=0; i<16; i++)
  {
    buff[ 0 ] = 0;
    std_strlprintf( buff, sizeof( buff ), "%02X", uuid128->uuid_byte[ i ] );
    std_strlcat( str, buff, str_size );
  }
  std_strlcat( str, BT_XML_UUID128_END, str_size );
  std_strlcat( str, "\n", str_size );
}

/*========================================================.num_uu===================

FUNCTION

  bt_sd_create_xml_uint_list

DESCRIPTION
  Creates XML representation for uint list.

  Reference: Supported format list (OPP)
  
===========================================================================*/
boolean bt_sd_create_xml_uint_list( 
  bt_sd_uint_list_type   uint_list,
  dsm_item_type**        dsm_ptr
)
{
  uint8   i;
  char    buff[ 100 ];
  boolean         keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL);

  for( i=0; i<uint_list.num_val && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    bt_sd_create_xml_uint_tag( uint_list.val[ i ], uint_list.size, 
                               (char*)buff, sizeof(buff) );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL); 
  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL); 
  }
  return keep_going;
}


/*========================================================.num_uu===================

FUNCTION

  bt_sd_create_xml_uuid_list

DESCRIPTION
  Creates XML representation for UUID list.
  
===========================================================================*/
boolean bt_sd_create_xml_uuid_list( 
  bt_sd_uuid_list_type   uuid_list,
  dsm_item_type**        dsm_ptr
)
{
  uint8   i;
  char    buff[ 100 ];
  boolean keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL); 
  for( i=0; i<uuid_list.num_uuid && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    bt_sd_create_xml_uuid16_tag( (uuid_list.uuid[ i ] & 0xFFFF),
                                 (char *)(buff + std_strlen(buff)), 
                                 (sizeof(buff) - std_strlen(buff)) );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL); 
  }

  for( i=0; i<uuid_list.num_uuid128 && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    bt_sd_create_xml_uuid128_tag( &uuid_list.uuid128[ i ], 
                                  (char *)(buff + std_strlen(buff)),
                                  (sizeof(buff) - std_strlen(buff)) );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL); 
  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL); 
  }
  return keep_going;
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_hid_list

DESCRIPTION
  Creates XML representation for HID class descriptor list.
  
===========================================================================*/
boolean bt_sd_create_xml_hid_list( 
  bt_sd_hid_desc_list_type  hid_desc_list,
  dsm_item_type**           dsm_ptr
)
{
  uint8           i,j;
  char            buff[ 50 ];
  boolean         keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL);

  for( i=0; i<hid_desc_list.num_hid_class_desc && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    std_strlcpy( buff, BT_XML_LIST_BEGIN, sizeof(buff) );


    /* Class Descriptor */
    bt_sd_create_xml_uint_tag( hid_desc_list.hid_class_desc[ i ].val,
                               sizeof(uint8), (char *)(buff + std_strlen(buff)),
                               (sizeof(buff) - std_strlen(buff)) );
    
    /* Byte string */
    std_strlcat( buff, BT_XML_BYTES_BEGIN, sizeof(buff) );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL); 

    for( j=0; j<hid_desc_list.hid_class_desc[ i ].len && keep_going; j++ )
    {
      /* Copy to dsm byte by byte because the length of the byte string 
         is variable */
      buff[ 0 ] = 0;
      std_strlprintf( buff, sizeof( buff ), "%02X", 
                      hid_desc_list.hid_class_desc[ i ].str[ j ] );
      keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                         DSM_DS_SMALL_ITEM_POOL); 
    }

    if ( keep_going )
    {
      buff[ 0 ] = 0;
      std_strlcpy( buff, BT_XML_BYTES_END, sizeof(buff) );
      std_strlcat( buff, "\n", sizeof(buff) );
      std_strlcat( buff, BT_XML_LIST_END, sizeof(buff) );
      keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                         DSM_DS_SMALL_ITEM_POOL); 
    }

  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL); 
  }
  return keep_going;
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_lang_list

DESCRIPTION
  Creates XML representation for language descriptor list.
  
===========================================================================*/
boolean bt_sd_create_xml_lang_list( 
  bt_sd_lang_base_attr_id_list_type   lang_base_attr_id_list,
  dsm_item_type**                     dsm_ptr
)
{
  uint8           i;
  char            buff[ 100 ];
  boolean         keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL);

  for( i=0; i<lang_base_attr_id_list.num_lang_base_rec && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    std_strlcpy( buff, BT_XML_LIST_BEGIN, sizeof(buff) );


    /* Language ID */
    bt_sd_create_xml_uint_tag( lang_base_attr_id_list.lang_base_rec[ i ].lang_id, 
                               sizeof(uint16), (char *)(buff + std_strlen(buff)),
                               (sizeof(buff) - std_strlen(buff)) );

    /* Character encoding */
    bt_sd_create_xml_uint_tag( lang_base_attr_id_list.lang_base_rec[ i ].char_enc_id, 
                               sizeof(uint16), (char *)(buff + std_strlen(buff)),
                               (sizeof(buff) - std_strlen(buff)) );

    /* Base attribute ID */
    bt_sd_create_xml_uint_tag( lang_base_attr_id_list.lang_base_rec[ i ].base_attr_id, 
                               sizeof(uint16), (char *)(buff + std_strlen(buff)),
                               (sizeof(buff) - std_strlen(buff)) );

    std_strlcat( buff, BT_XML_LIST_END, sizeof(buff) );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL);
  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL); 
  }
  return keep_going;
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_pd_list

DESCRIPTION
  Creates XML representation for protocol descriptor list.
  
===========================================================================*/
boolean bt_sd_create_xml_pd_list( 
  const bt_sd_proto_desc_list_type  proto_desc_list,
  dsm_item_type**                   dsm_ptr
)
{
  uint8             i,j;
  char              buff[ 100 ] = "\0";
  boolean           keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL);

  for( i=0; i<proto_desc_list.num_proto_desc && keep_going; i++ )
  {
    buff[ 0 ] = 0;
    std_strlcpy( buff, BT_XML_LIST_BEGIN, sizeof(buff) );


    /* Protocol UUID */
    if( proto_desc_list.proto_desc[ i ].is_uuid128 )
    {
      bt_sd_create_xml_uuid128_tag( &proto_desc_list.proto_desc[ i ].uuid128, 
                                    (char *)(buff + std_strlen(buff)),
                                    (sizeof(buff) - std_strlen(buff)) );
    }
    else
    {
      bt_sd_create_xml_uuid16_tag( proto_desc_list.proto_desc[ i ].uuid, 
                                   (char *)(buff + std_strlen(buff)),
                                   (sizeof(buff) - std_strlen(buff)) );
    }

    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL); 

    /* Protocol Parameters */
    for( j=0; j<proto_desc_list.proto_desc[ i ].num_param && keep_going; j++ )
    {
      buff[ 0 ] = 0;
      bt_sd_create_xml_uint_tag( proto_desc_list.proto_desc[ i ].param[ j ].value, 
                                 proto_desc_list.proto_desc[ i ].param[ j ].size, 
                                 buff, sizeof(buff) );

      keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                         DSM_DS_SMALL_ITEM_POOL); 
    }

    /* </list> */
    if ( keep_going )
    {
       keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                          std_strlen(BT_XML_LIST_END),
                                          DSM_DS_SMALL_ITEM_POOL); 
    }
  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL);
  }
  return keep_going;
}


/*===========================================================================

FUNCTION
  bt_sd_create_xml_add_pd_list

DESCRIPTION
  Creates XML representation for additional protocol descriptor lists.
  
===========================================================================*/
boolean bt_sd_create_xml_add_pd_list( 
  const bt_sd_proto_desc_list_type* add_proto_desc_list,
  dsm_item_type**                   dsm_ptr
)
{
  uint8    i;
  boolean  keep_going = TRUE;

  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_BEGIN, 
                                     std_strlen(BT_XML_LIST_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL);

  for( i=0; i<BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS && keep_going; i++ )
  {
    /* Create the Protocol Descriptor list */
    keep_going = bt_sd_create_xml_pd_list( add_proto_desc_list[ i ], dsm_ptr);
  }

  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_LIST_END, 
                                        std_strlen(BT_XML_LIST_END),
                                        DSM_DS_SMALL_ITEM_POOL);
  }
  return keep_going;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION
  bt_sd_create_xml_service_record

DESCRIPTION
  Creates an XML representation of a service record.

===========================================================================*/
boolean bt_sd_create_xml_service_record(
  const bt_sd_srv_rec_type*   srv_rec_ptr,
  dsm_item_type**             dsm_ptr
)
{
  boolean keep_going = TRUE;
  const bt_sd_srv_attr_rec_type* srv_attr_ptr;
  char                           buff[ 50 ] = "\0";

  BT_MSG_DEBUG( "BT SD: Create XML REC h=%x", srv_rec_ptr->srv_rec_id, 0, 0);

  /* <srecord> */
  keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_SREC_BEGIN, 
                                     std_strlen(BT_XML_SREC_BEGIN),
                                     DSM_DS_SMALL_ITEM_POOL); 

  bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_ALL );
  for ( srv_attr_ptr = bt_sd_srv_attr_iter.first();
        srv_attr_ptr != 0 && keep_going;
        srv_attr_ptr = bt_sd_srv_attr_iter.next() )
  {

    /* <attribute> */
    buff[ 0 ] = 0;
    std_strlprintf( buff, sizeof( buff ), "<attr id=\"0x%04X\">\n", 
                    srv_attr_ptr->attr_id );
    keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                       DSM_DS_SMALL_ITEM_POOL );
    if (!keep_going)
    {
       break;
    }

    buff[ 0 ] = 0;

    switch( srv_attr_ptr->attr_type )
    {
      case BT_SD_ATTR_TYPE_UNKNOWN:
        break;

      case BT_SD_ATTR_TYPE_STRING:
        bt_sd_create_xml_string_tag( srv_attr_ptr->attr_value.str, buff, 
                                     sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UINT8:
        bt_sd_create_xml_uint_tag( srv_attr_ptr->attr_value.primitive_value, 
                                   sizeof(uint8), buff, sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UINT16:
        bt_sd_create_xml_uint_tag( srv_attr_ptr->attr_value.primitive_value, 
                                   sizeof(uint16), buff, sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UINT32:
        bt_sd_create_xml_uint_tag( srv_attr_ptr->attr_value.primitive_value, 
                                   sizeof(uint32), buff, sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UINT64:
        bt_sd_create_xml_uint_tag( srv_attr_ptr->attr_value.primitive_value, 
                                   sizeof(uint64), buff, sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UINT128:
        bt_sd_create_xml_uint128_tag( srv_attr_ptr->attr_value.uint128_value, 
                                      buff, sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_BOOL:
        bt_sd_create_xml_bool_tag( srv_attr_ptr->attr_value.bool_flag, buff, 
                                   sizeof(buff)  );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UUID:
        bt_sd_create_xml_uuid16_tag( 
          (srv_attr_ptr->attr_value.primitive_value & 0xFFFF),
          (char *)buff, sizeof(buff)  );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UUID128:
        bt_sd_create_xml_uuid128_tag( &srv_attr_ptr->attr_value.uuid128, buff, 
                                      sizeof(buff) );
        keep_going = bt_dsm_pushdown_tail( dsm_ptr, buff, std_strlen(buff),
                                           DSM_DS_SMALL_ITEM_POOL); 
        break;

      case BT_SD_ATTR_TYPE_UUID_LIST:
        keep_going = bt_sd_create_xml_uuid_list( 
                       srv_attr_ptr->attr_value.uuid_list, dsm_ptr);
        break;

      case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
        keep_going = bt_sd_create_xml_pd_list( 
                       srv_attr_ptr->attr_value.proto_desc_list, dsm_ptr);
        break;

      case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
        keep_going = bt_sd_create_xml_lang_list( 
                       srv_attr_ptr->attr_value.lang_base_attr_id_list, dsm_ptr);
        break;

      case BT_SD_ATTR_TYPE_UINT_LIST:
        keep_going = bt_sd_create_xml_uint_list( 
                       srv_attr_ptr->attr_value.uint_list, dsm_ptr );
        break;

      case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
        keep_going = bt_sd_create_xml_add_pd_list( 
                       srv_attr_ptr->attr_value.add_proto_desc_lists, dsm_ptr );
        break;

      case BT_SD_ATTR_TYPE_HID_DESC_LIST:
        keep_going = bt_sd_create_xml_hid_list( 
                       srv_attr_ptr->attr_value.hid_desc_list, dsm_ptr);
        break;
    }

    /* </attribute> */
    if ( keep_going )
    {
       keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_ATTR_END, 
                                          std_strlen(BT_XML_ATTR_END),
                                          DSM_DS_SMALL_ITEM_POOL); 
    }
  }

  /* </srecord> */
  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_SREC_END, 
                                        std_strlen(BT_XML_SREC_END),
                                        DSM_DS_SMALL_ITEM_POOL );
  }
  return keep_going;
}

/*===========================================================================

FUNCTION
  bt_sd_create_xml_service_db

DESCRIPTION
  Creats an XML representation of the SD database

  Following is a sample 
  <sdpdb>
    <srec>
      <attr id="0x0001">
      ..
      </attr>
      <attr id="0x0002">
      ..
      </attr>
      ..
    </srec>
    <srec>
      ..
    </srec>
    ..
  </sdpdb>
  
===========================================================================*/
boolean bt_sd_create_xml_service_db(
  bt_sd_db_enum_type         db_type,
  dsm_item_type**            dsm_ptr
)
{
  const bt_sd_srv_rec_type*   srv_rec_ptr;
  bt_sd_srv_rec_type          tmp_srv_rec;
  boolean                     keep_going = TRUE;

  BT_MSG_DEBUG( "BT SD: Create XML DB" ,0, 0, 0);

  /* <?xml version */
  bt_dsm_pushdown_tail( dsm_ptr, BT_XML_VER, std_strlen(BT_XML_VER),
                        DSM_DS_SMALL_ITEM_POOL); 

  /* <sdpdb> */
  bt_dsm_pushdown_tail( dsm_ptr, BT_XML_SDPDB_BEGIN, 
                        std_strlen( BT_XML_SDPDB_BEGIN ),
                        DSM_DS_SMALL_ITEM_POOL); 

  bt_sd_srv_rec_iter.init( db_type, BT_SD_ITER_ALL );
  for ( srv_rec_ptr = bt_sd_srv_rec_iter.first();
        srv_rec_ptr != 0 && keep_going;
        srv_rec_ptr = bt_sd_srv_rec_iter.next() )
  {
    TASKLOCK();
    tmp_srv_rec = *srv_rec_ptr;
    TASKFREE();
    keep_going = bt_sd_create_xml_service_record( &tmp_srv_rec, dsm_ptr );
  }

  /* </sdpdb> */
  if ( keep_going )
  {
     keep_going = bt_dsm_pushdown_tail( dsm_ptr, BT_XML_SDPDB_END, 
                                        std_strlen(BT_XML_SDPDB_END),
                                        DSM_DS_SMALL_ITEM_POOL); 
  }
  return keep_going;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_find_next_close_tag

DESCRIPTION
  Returns pointer to next XML close tag
  
===========================================================================*/
char* bt_sd_xml_find_next_close_tag
( 
  char *p, 
  char *ctag 
)
{
  return ( strstr(p, ctag) );
}


/*===========================================================================

FUNCTION
  bt_sd_xml_find_last_close_tag

DESCRIPTION
  Returns pointer to last XML close tag in the string
  
===========================================================================*/
char* bt_sd_xml_find_last_close_tag 
( 
  char *p1, 
  char *p2, 
  char *ctag 
)
{
  while( p2 > p1 )
  {
    if( bt_sd_xml_is_white_space( *p2 ) )
    {
      --p2;
    }
    else
    {
      break;
    }
  }

  p2 = p2 - strlen(ctag) + 1;
  if( !strncmp(p2, ctag, strlen(ctag)) )
  {
    return (p2);
  }

  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_get_num_tags

DESCRIPTION
  
===========================================================================*/
int bt_sd_xml_get_num_tags
(
 char *s1, 
 char *s2, 
 char *tag
)
{
  char *temp = NULL;
  int n = 0;

  while ( s1 < s2 )
  {
    temp = strstr( s1, tag );
    if( temp == NULL || 
        (temp + strlen(tag) - 1) > s2 )
    {
      break;
    }
    n++;
    s1 = temp + strlen(tag);
  }
  return (n);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_find_close_tag

DESCRIPTION
  
===========================================================================*/
char* bt_sd_xml_find_close_tag
(
  char *p1, 
  char *p2, 
  char *otag,
  char *ctag 
)
{
  char* s1 = NULL;
  char* s2 = NULL;
  int n1, n2;

  s1 = p1;
  s2 = NULL;

  while( s1 < p2 )
  {
    s2 = strstr( s1, ctag );
    if( s2 == NULL )
    {
      break;
    }

    n1 = bt_sd_xml_get_num_tags( p1, s2, otag );
    n2 = bt_sd_xml_get_num_tags( p1, s2, ctag );
    if( n1 == n2 )
    {
      break;
    }

    s1 = s2 + strlen(ctag);
    s2 = NULL;
  }

  return s2;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_find_open_tag

DESCRIPTION
  Returns pointer to XML open tag in string
  
===========================================================================*/
char* bt_sd_xml_find_open_tag 
( 
  char *p1, 
  char *p2, 
  char *otag 
)
{
  uint16 len;

  len = p2 - p1;
  bt_sd_xml_skip_white_space( &p1, &len );

  if( len >= strlen(otag) )
  {
    if( !strncmp(p1, otag, strlen(otag)) )
    {
      return (p1);
    }
  }

  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_get_nested_element

DESCRIPTION
  Returns pointer to a nested XML element. 
  The tags in the beginning and the end of 'xmlstring' should match

  Input: 
  xmlstring   - string to parse 
  elem_len    - length of string to parse
  otag, ctag  - opening and closing tags

  For the following string - 
  "<list>ABCD</list>"
  function will return pointer to 'ABCD' and a length of 4.
  
===========================================================================*/
char* bt_sd_xml_get_nested_element
(
  char*   xmlstring,
  uint16* elem_len,
  char*   otag,
  char*   ctag
)
{
  char* s1 = NULL;
  char* s2 = NULL;
  uint16 min_length;

  min_length = strlen(otag) + strlen(ctag);

  if( *elem_len < min_length )
  {
    return NULL;
  }

  /* Match opening tag */
  s1 = bt_sd_xml_find_open_tag( xmlstring, xmlstring + (*elem_len - min_length), otag );
  if( s1 != NULL )
  {
    s2 = bt_sd_xml_find_last_close_tag( s1, xmlstring + (*elem_len - 1), ctag );
    if( s1 != NULL )
    {
      s1 = strchr( s1, '>' );
      s1 += 1;
      *elem_len = s2 - s1;
      return (s1);
    }
  }
  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_get_next_element

DESCRIPTION
  Returns pointer to a the next XML element
  Modifies xmlstring to point to beyond the XML element returned

  Input: 
  xmlstring       - string to parse 
  xmlstring_len   - length of string to parse
  elem_len        - length of returned string
  otag, ctag      - opening and closing tags

  For the following string - 
  "<uint8>0x11</uint8><uint8>0x22</uint8>"
    - returns pointer to '0x11' and a length of 4
    - xmlstring points to '<uint8>0x22</uint8>'
  
===========================================================================*/
char* bt_sd_xml_get_next_element
(
  char**  xmlstring, 
  uint16* xmlstring_length,
  uint16* elem_len,
  char*   otag,
  char*   ctag
)
{
  char* s1 = NULL;
  char* s2 = NULL;
  char* temp = NULL;

  /* todo - if element is not found but only white space is found dont return error */

  *elem_len = 0;

  s1 = bt_sd_xml_find_open_tag( *xmlstring, *xmlstring + (*xmlstring_length), otag );
  if( s1 != NULL )
  {
    s1 += strlen(otag);
    s2 = bt_sd_xml_find_close_tag( s1, *xmlstring + (*xmlstring_length), 
                                   otag, ctag );
    if( s2 != NULL ) 
    {
      temp = *xmlstring;
      *xmlstring = s2 + strlen(ctag);
      *xmlstring_length = *xmlstring_length - (*xmlstring - temp);
      *elem_len = s2 - s1;
      return (s1);
    }
  }

  return NULL;
}


/*===========================================================================

FUNCTION
  bt_sd_xml_get_next_attribute

DESCRIPTION
  Returns pointer to next XML attribute
  
===========================================================================*/
char* bt_sd_xml_get_next_attribute
(
  char**  xmlstring, 
  uint16* xmlstring_length,
  uint16* elem_length
)
{
  char* s1 = NULL;
  char* s2 = NULL;

  s1 = bt_sd_xml_find_open_tag( *xmlstring, *xmlstring + (*xmlstring_length), BT_XML_ATTR_BEGIN );
  if( s1 != NULL )
  {
    s2 = bt_sd_xml_find_next_close_tag( *xmlstring, BT_XML_ATTR_END );
    if( s2 != NULL ) 
    {
      s2 += strlen(BT_XML_ATTR_END);
      *xmlstring_length = *xmlstring_length - (s2 - *xmlstring);
      *elem_length = s2 - s1;
      *xmlstring = s2;
      return (s1);
    }
  }
  return NULL;
}



/*===========================================================================

FUNCTION
  bt_sd_xml_get_next_element_type

DESCRIPTION
  
===========================================================================*/
bt_sd_xml_elem_type bt_sd_xml_get_next_element_type 
(
  char**  xmlstring, 
  char**  elem,
  uint16* xmlstring_length,
  uint16* elem_length
)
{
  bt_sd_xml_elem_type elem_type = BT_SD_XML_ELEM_UNKNOWN;

  if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                           BT_XML_LIST_BEGIN, BT_XML_LIST_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_LIST;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UUID16_BEGIN, BT_XML_UUID16_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UUID16;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UUID128_BEGIN, BT_XML_UUID128_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UUID128;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UINT8_BEGIN, BT_XML_UINT8_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UINT8;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UINT16_BEGIN, BT_XML_UINT16_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UINT16;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UINT32_BEGIN, BT_XML_UINT32_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UINT32;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UINT64_BEGIN, BT_XML_UINT64_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UINT64;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_UINT128_BEGIN, BT_XML_UINT128_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_UINT64;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_BOOL_BEGIN, BT_XML_BOOL_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_BOOL;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_STRING_BEGIN, BT_XML_STRING_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_STRING;
  }
  else if( (*elem = bt_sd_xml_get_next_element( xmlstring, xmlstring_length, elem_length, 
                                                BT_XML_BYTES_BEGIN, BT_XML_BYTES_END )) != NULL )
  {
    elem_type = BT_SD_XML_ELEM_BYTES;
  }

  return (elem_type);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_bytes

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_bytes
(
  char*                       str_bytes,
  uint16                      len,
  bt_sd_hid_class_desc_type*  hid_class_desc
)
{
  uint64  num;
  boolean success = FALSE;
  int     i = 0;

  bt_sd_xml_trim_white_space( &str_bytes, &len );
  if( len <= BT_SD_MAX_HID_CLASS_DESC_STRING_LEN )
  {
    if( !(len % 2) )
    {
      while( len )
      {
        if ( bt_sd_xml_ascii_to_hex( str_bytes, 2, &num ) )
        {
          hid_class_desc->str[ i++ ] = (byte)num;
          str_bytes += 2;
          len -= 2;
        }
        else
        {
          break;
        }
      }
      if( len == 0 )
      {
        hid_class_desc->len = i;
        success = TRUE;
      }
    }
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_bool

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_bool
(
  char*     str_bool,
  uint16    len,
  boolean*  bool_value
)
{
  boolean success = FALSE;

  bt_sd_xml_skip_white_space( &str_bool, &len );

  if( !strncmp( str_bool, BT_SD_XML_BOOL_VAL_TRUE, 
                strlen(BT_SD_XML_BOOL_VAL_TRUE) ) )
  {
    *bool_value = TRUE;
    str_bool += strlen( BT_SD_XML_BOOL_VAL_TRUE );
    len -= strlen( BT_SD_XML_BOOL_VAL_TRUE );
    success = bt_sd_xml_check_white_space( str_bool, len );
  }
  else if( !strncmp( str_bool, BT_SD_XML_BOOL_VAL_FALSE,
                     strlen(BT_SD_XML_BOOL_VAL_FALSE) ) )
  {
    *bool_value = FALSE;
    str_bool += strlen( BT_SD_XML_BOOL_VAL_FALSE );
    len -= strlen( BT_SD_XML_BOOL_VAL_FALSE );
    success = bt_sd_xml_check_white_space( str_bool, len );
  }

  return (success);
}
  


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uint

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uint
(
  char*    str_uint,
  uint16   len,
  uint64*  num,
  uint8    size_bytes
)
{
  boolean success = FALSE;
  uint16  size;

  size = size_bytes * 2;

  bt_sd_xml_skip_white_space( &str_uint, &len );

  if( !strncmp( str_uint, "0x", 2 ) )
  {
    str_uint += 2;
    len -= 2;
    if ( bt_sd_xml_ascii_to_hex( str_uint, size, num ) )
    {
      /* The rest of it should only have white space */
      str_uint += size;
      len -= size;
      success = bt_sd_xml_check_white_space( str_uint, len );
    }
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uint128

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uint128
(
  char*    str_uint128,
  uint16   len,
  byte     uint128_value[]
)
{
  uint64    num;
  int       i;
  boolean   success = FALSE;

  bt_sd_xml_skip_white_space( &str_uint128, &len );

  if( !strncmp( str_uint128, "0x", 2 ) )
  {
    str_uint128 += 2;
    len -= 2;
    for( i=0; i<16; i++ )
    {
      if ( bt_sd_xml_ascii_to_hex( str_uint128, 2, &num ) )
      {
        uint128_value[ i ] = (byte)num;
        str_uint128 += 2;
        len -= 2;
      }
      else
      {
        break;
      }
    }
    if(i == 16)
    {
      success = bt_sd_xml_check_white_space( str_uint128, len );
    }
  }
  return (success);
}

 
/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uuid128

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uuid128
(
  char*               str_uuid,
  uint16               len,
  bt_sd_uuid128_type* uuid128
)
{
  boolean   success = FALSE;
  byte      uint128_value[ 16 ];

  if( (success = bt_sd_xml_parse_uint128( str_uuid, len, 
                                          uint128_value )) == TRUE )
  {
    memcpy( (void *)uuid128->uuid_byte, 
            (void *)uint128_value, sizeof(uint128_value) );
  }
  return (success);
}



/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uuid16

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uuid16
(
  char*             uuid,
  uint16             len,
  bt_sd_uuid_type*  bt_sd_uuid
)
{
  boolean   success = FALSE;
  uint64    num;

  if ( (success = bt_sd_xml_parse_uint( uuid, len, 
                                        &num, 2 )) == TRUE )
  {
    *bt_sd_uuid = (bt_sd_uuid_type)num;
  }
  return (success);
}



/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uuid_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uuid_list
(
  char*                   xmllist,
  uint16                  xmllist_length,
  bt_sd_uuid_list_type*   uuid_list
)
{
  char*                 head = NULL;
  char*                 tail = NULL;
  char*                 elem = NULL;
  uint16                elem_length;
  uint16                num;
  bt_sd_uuid128_type    uuid128;
  bt_sd_xml_elem_type   xml_elem;
  boolean               success = TRUE;
  
  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );
  uuid_list->num_uuid = 0;
  uuid_list->num_uuid128 = 0;

  head = xmllist;
  tail = xmllist + xmllist_length;

  while( head < tail )
  {
    xml_elem = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem == BT_SD_XML_ELEM_UUID16 )
    {
      if( uuid_list->num_uuid >= BT_SD_MAX_NUM_OF_UUID )
      {
        success = FALSE;
        break;
      }
      if( (success = bt_sd_xml_parse_uuid16( elem, elem_length, &num )) == TRUE )
      {
        uuid_list->uuid[ uuid_list->num_uuid ] = num;
        uuid_list->num_uuid ++;
      }
    }
    else if( xml_elem == BT_SD_XML_ELEM_UUID128 )
    {
      if( (success = bt_sd_xml_parse_uuid128( elem, elem_length, &uuid128 )) == TRUE )
      {
        if( uuid_list->num_uuid128 >= BT_SD_MAX_NUM_OF_UUID128 )
        {
          success = FALSE;
          break;
        }
        memcpy( (void *)&uuid_list->uuid128[ uuid_list->num_uuid128 ], 
                (void *)&uuid128, sizeof(bt_sd_uuid128_type) );
        uuid_list->num_uuid128 ++;
      }
    }
    else
    {
      success = FALSE;
      break;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_proto_desc

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_proto_desc
(
  char*                    xmllist,
  bt_sd_proto_desc_type*   proto_desc,
  uint16                   xmllist_length
)
{
  char*                 head = NULL;
  char*                 tail = NULL;
  char*                 elem = NULL;
  uint16                elem_length;
  uint64                num;
  bt_sd_uuid_type       uuid;
  bt_sd_uuid128_type    uuid128;
  bt_sd_xml_elem_type   xml_elem_type;
  boolean               success = TRUE;
  uint8                 size = 0;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );
  head = xmllist;
  tail = head + xmllist_length;

  /* Extract protocol UUID */
  xml_elem_type = bt_sd_xml_get_next_element_type( &xmllist, &elem, &xmllist_length, &elem_length );
  if( xml_elem_type == BT_SD_XML_ELEM_UUID16 )
  {
    if( (success = bt_sd_xml_parse_uuid16( elem, elem_length, &uuid )) == TRUE )
    {
      proto_desc->uuid = uuid;
      proto_desc->is_uuid128 = FALSE;
    }     
  }
  else if( xml_elem_type == BT_SD_XML_ELEM_UUID128 )
  {
    if( (success = bt_sd_xml_parse_uuid128( elem, elem_length, &uuid128 )) == TRUE )
    {
      proto_desc->is_uuid128 = TRUE;
      memcpy( (void *)&proto_desc->uuid128,
              (void *)&uuid128, sizeof(bt_sd_uuid128_type) );
    }     
  }
  else
  {
    success = FALSE;
  }

  /* Extract protocol parameters */
  head = xmllist;
  proto_desc->num_param = 0;

  while( head < tail && 
         success == TRUE )
  {
    if( proto_desc->num_param >= BT_SD_MAX_NUM_OF_PROTO_PARAM )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_UINT8 )
    {
      size = 1;
    }
    else if( xml_elem_type == BT_SD_XML_ELEM_UINT16 )
    {
      size = 2;
    }
    else if( xml_elem_type == BT_SD_XML_ELEM_UINT32 )
    {
      size = 4;
    }
    else
    {
      success = FALSE;
    }

    if( success == TRUE &&
        bt_sd_xml_parse_uint( elem, elem_length, &num, size ) )
    {
      proto_desc->param[ proto_desc->num_param ].value = (uint32)num;
      proto_desc->param[ proto_desc->num_param ].size = size;
      proto_desc->num_param ++;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }
        
  return (success);
}



/*===========================================================================

FUNCTION
  bt_sd_xml_parse_proto_desc_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_proto_desc_list
(
  char*                         xmllist,
  uint16                        xmllist_length,
  bt_sd_proto_desc_list_type*   proto_desc_list
)
{
  char*                   head = NULL;
  char*                   tail = NULL;
  char*                   elem = NULL;
  uint16                  elem_length;
  bt_sd_xml_elem_type     xml_elem_type;
  bt_sd_proto_desc_type   proto_desc;
  boolean                 success = TRUE;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  proto_desc_list->num_proto_desc = 0;
  head = xmllist;
  tail = xmllist + xmllist_length;

  /* Initializing to make klocwork happy */
  memset( &proto_desc, 0, sizeof(bt_sd_proto_desc_type) );

  while( head < tail && 
         success == TRUE )
  {
    if( proto_desc_list->num_proto_desc >= BT_SD_MAX_NUM_OF_PROTO_DESC )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_LIST )
    {
      if ( (success = bt_sd_xml_parse_proto_desc( elem, &proto_desc, 
                                                  elem_length )) == TRUE )
      {
        memcpy( &proto_desc_list->proto_desc[ proto_desc_list->num_proto_desc ], 
                &proto_desc, sizeof(bt_sd_proto_desc_type) );
        proto_desc_list->num_proto_desc ++;
      }
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_lang_base_rec

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_lang_base_rec
(
  char*                       xmllist,
  bt_sd_lang_base_rec_type*   lang_base_rec,
  uint16                      xmllist_length 
)
{
  char*               head = NULL;
  char*               tail = NULL;
  char*               elem = NULL;
  int                 iter = 0;
  uint64              num;
  uint16              elem_length;
  bt_sd_xml_elem_type xml_elem_type;
  boolean             success = TRUE;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  head = xmllist;
  tail = head + xmllist_length;

  while( head < tail &&
         success == TRUE )
  {
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, 
                                                     &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_UINT16 )
    {
      if( (success = bt_sd_xml_parse_uint( elem, elem_length, 
                                           &num, 2 )) == TRUE )
      {
        if( iter == 0 )
        {
          lang_base_rec->lang_id = (uint16)num;
        }
        else if( iter == 1 )
        {
          lang_base_rec->char_enc_id = (uint16)num;
        }
        else if( iter == 2 )
        {
          lang_base_rec->base_attr_id = (uint16)num;
        }
        else
        {
          /* Should never get here */
          success = FALSE;
        }
        iter = iter + 1;
      }
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }

  return (success);
}



/*===========================================================================

FUNCTION
  bt_sd_xml_parse_lang_base_attrid_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_lang_base_attrid_list
(
  char*                                 xmllist,
  uint16                                xmllist_length,
  bt_sd_lang_base_attr_id_list_type*    lang_base_attr_id_list
)
{
  char*                       head = NULL;
  char*                       tail = NULL;
  char*                       elem = NULL;
  uint16                      elem_length;
  bt_sd_xml_elem_type         xml_elem_type;
  bt_sd_lang_base_rec_type    lang_base_rec;
  boolean                     success = TRUE;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  head = xmllist;
  tail = xmllist + xmllist_length;

  lang_base_attr_id_list->num_lang_base_rec = 0;

  while( head < tail &&
         success == TRUE )
  {
    if( lang_base_attr_id_list->num_lang_base_rec >= BT_SD_MAX_NUM_OF_LANG_BASE_REC )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_LIST )
    {
      if( (success = bt_sd_xml_parse_lang_base_rec( elem, &lang_base_rec, 
                                                    elem_length )) == TRUE )
      {
        memcpy( &lang_base_attr_id_list->lang_base_rec[ lang_base_attr_id_list->num_lang_base_rec ], 
                &lang_base_rec, sizeof(bt_sd_lang_base_rec_type) );
        lang_base_attr_id_list->num_lang_base_rec ++;
      }
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_uint_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_uint_list
(
  char*                    xmllist,
  uint16                   xmllist_length,
  bt_sd_uint_list_type*    uint_list
)
{
  char*                 head = NULL;
  char*                 tail = NULL;
  char*                 elem = NULL;
  uint64                num;
  uint16                elem_length;
  uint8                 size = 0;
  bt_sd_xml_elem_type   xml_elem_type = BT_SD_XML_ELEM_UNKNOWN;
  bt_sd_uint_list_type  temp_uint_list;
  boolean               success = TRUE;

  /* Initializing to make klocwork happy */
  memset( &temp_uint_list, 0, sizeof(bt_sd_uint_list_type) );

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  head = xmllist;
  tail = xmllist + xmllist_length;

  temp_uint_list.num_val = 0;

  while( head < tail &&
         success == TRUE )
  {
    if( temp_uint_list.num_val >= BT_SD_MAX_NUM_OF_UINT )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_UINT8 &&
        ( size == 0 || size == 1 ) )
    {
      size = 1;
    }
    else if( xml_elem_type == BT_SD_XML_ELEM_UINT16 &&
             ( size == 0 || size == 2 ) )
    {
      size = 2;
    }
    else if( xml_elem_type == BT_SD_XML_ELEM_UINT32 &&
             ( size == 0 || size == 4 ) )
    {
      size = 4;
    }
    else if( xml_elem_type == BT_SD_XML_ELEM_UINT64 && 
             ( size == 0 || size == 8 ) )
    {
      size = 8;
    }
    else
    {
      success = FALSE;
    }

    if( success == TRUE &&
        bt_sd_xml_parse_uint( elem, elem_length, 
                              &num, size ) )
    {
      temp_uint_list.size = size;
      temp_uint_list.val[ temp_uint_list.num_val ] = num;
      temp_uint_list.num_val ++;
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }

  if( success == TRUE )
  {
    memcpy( (void *)uint_list,  
            (void *)&temp_uint_list, 
            sizeof(bt_sd_uint_list_type) );
  }
  return (success);
}



/*===========================================================================

FUNCTION
  bt_sd_xml_parse_add_proto_desc_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_add_proto_desc_list
(
  char*                       xmllist,
  uint16                      xmllist_length,
  bt_sd_proto_desc_list_type  add_proto_desc_lists[]
)
{
  char*                         head = NULL;
  char*                         tail = NULL;
  char*                         elem = NULL;
  uint16                        elem_length;
  int                           cnt = 0;
  bt_sd_xml_elem_type           xml_elem_type;
  bt_sd_proto_desc_list_type    proto_desc_list;
  boolean                       success = TRUE;

  /* Initializing to make klocwork happy */
  memset( &proto_desc_list, 0, sizeof(bt_sd_proto_desc_list_type) );

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  head = xmllist;
  tail = xmllist + xmllist_length;

  while( head < tail && 
         success == TRUE )
  {
    if( cnt >= BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_LIST )
    {
      if( (success = bt_sd_xml_parse_proto_desc_list( elem, elem_length, 
                                                      &proto_desc_list )) == TRUE )
      {
        memcpy( (void *)&add_proto_desc_lists[ cnt ],
                (void *)&proto_desc_list, sizeof(bt_sd_proto_desc_list_type) );
        cnt = cnt + 1;
      }
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_hid_desc

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_hid_desc
(
  char*                       xmllist,
  bt_sd_hid_class_desc_type*  hid_class_desc,
  uint16                      xmllist_length 
)
{
  char*                         elem = NULL;
  uint16                        elem_length;
  bt_sd_xml_elem_type           xml_elem_type;
  uint64                        num;
  boolean                       success = TRUE;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  xml_elem_type = bt_sd_xml_get_next_element_type( &xmllist, &elem, 
                                                   &xmllist_length, &elem_length );
  if( xml_elem_type == BT_SD_XML_ELEM_UINT8 )
  {
    if( (success = bt_sd_xml_parse_uint( elem, elem_length, 
                                         &num, 1 )) == TRUE )
    {
      xml_elem_type = bt_sd_xml_get_next_element_type( &xmllist, &elem, 
                                                     &xmllist_length, &elem_length );
      if( xml_elem_type == BT_SD_XML_ELEM_BYTES )
      {
        hid_class_desc->val = (uint8)num;
        success = bt_sd_xml_parse_bytes( elem, elem_length, hid_class_desc );
      }
    }
  }
  else
  {
    success = FALSE;
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_hid_desc_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_hid_desc_list
(
  char*                       xmllist,
  uint16                      xmllist_length,
  bt_sd_hid_desc_list_type*   hid_desc_list
)
{
  char*                         head = NULL;
  char*                         tail = NULL;
  char*                         elem = NULL;
  uint16                        elem_length;
  bt_sd_xml_elem_type           xml_elem_type;
  bt_sd_hid_class_desc_type     hid_class_desc;
  boolean                       success = TRUE;

  /* Initializing to make klocwork happy */
  memset( &hid_class_desc, 0, sizeof(bt_sd_hid_class_desc_type) );

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  head = xmllist;
  tail = xmllist + xmllist_length;

  hid_desc_list->num_hid_class_desc = 0;

  while( head < tail && 
         success == TRUE )
  {
    if( hid_desc_list->num_hid_class_desc >= BT_SD_MAX_NUM_OF_HID_CLASS_DESC )
    {
      success = FALSE;
      break;
    }
    xml_elem_type = bt_sd_xml_get_next_element_type( &head, &elem, &xmllist_length, &elem_length );
    if( xml_elem_type == BT_SD_XML_ELEM_LIST )
    {
      if( (success = bt_sd_xml_parse_hid_desc( elem, &hid_class_desc, 
                                               elem_length )) == TRUE )
      {
        memcpy( &hid_desc_list->hid_class_desc[ hid_desc_list->num_hid_class_desc ], 
                &hid_class_desc, sizeof(bt_sd_hid_class_desc_type) );
        hid_desc_list->num_hid_class_desc ++;
      }
    }
    else
    {
      success = FALSE;
    }
    bt_sd_xml_skip_white_space( &head, &xmllist_length );
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_list

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_list
(
  char*                     xmllist,
  uint16                    xmllist_length,
  bt_sd_srv_attr_rec_type*  ptr_srv_attr_rec
)
{
  char*                             elem = NULL;
  bt_sd_proto_desc_list_type        proto_desc_list;
  bt_sd_uuid_list_type              uuid_list;
  bt_sd_lang_base_attr_id_list_type lang_base_attr_id_list;
  bt_sd_uint_list_type              uint_list;
  bt_sd_proto_desc_list_type        add_proto_desc_lists[ 
                                        BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS ];
  bt_sd_hid_desc_list_type          hid_desc_list;
  boolean                           success = TRUE;
  bt_sd_proto_desc_type             *pd_ptr;
  int                               proto_desc_cnt, param_cnt;
  int                               num_lang_base_cnt;

  /* Initializing to make klocwork happy */
  memset( &proto_desc_list, 0, sizeof(bt_sd_proto_desc_list_type) );

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );

  elem = bt_sd_xml_get_nested_element( xmllist, &xmllist_length, BT_XML_LIST_BEGIN, 
                                       BT_XML_LIST_END );

  if( elem == NULL )
  {
    return FALSE;
  }

  switch( ptr_srv_attr_rec->attr_type )
  {
    case BT_SD_ATTR_TYPE_UUID_LIST:
      if( (success = bt_sd_xml_parse_uuid_list( elem, xmllist_length, 
                                                &uuid_list )) == TRUE )
      {
        memcpy( (void *)&ptr_srv_attr_rec->attr_value.uuid_list, 
                (void *)&uuid_list, sizeof(bt_sd_uuid_list_type) );
      }
      break;
    case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      if( (success = bt_sd_xml_parse_proto_desc_list( elem, xmllist_length, 
                                                      &proto_desc_list )) == TRUE )
      {
        /* Update the header along with the protocol des */
        ptr_srv_attr_rec->attr_value.proto_desc_list.num_proto_desc = 0;

        if ( ( bt_sd_service_record_init_attribute(
                   ptr_srv_attr_rec, BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST, 
                   BT_SD_ATTR_TYPE_PROTO_DESC_LIST ) ) != FALSE )
        {
            for(proto_desc_cnt = 0; proto_desc_cnt < proto_desc_list.num_proto_desc; proto_desc_cnt++)
            {
              if (!proto_desc_list.proto_desc[proto_desc_cnt].is_uuid128) 
              {
                  pd_ptr = bt_sd_service_attribute_add_proto_desc(
                             ptr_srv_attr_rec, 
                             proto_desc_list.proto_desc[proto_desc_cnt].uuid,
                             NULL );
              }
              else
              {
                  pd_ptr = bt_sd_service_attribute_add_proto_desc(
                             ptr_srv_attr_rec, 
                             proto_desc_list.proto_desc[proto_desc_cnt].uuid,
                             &proto_desc_list.proto_desc[proto_desc_cnt].uuid128 );
              }
              if ( pd_ptr != NULL )
              {
                  for (param_cnt = 0; param_cnt < proto_desc_list.proto_desc[proto_desc_cnt].num_param; param_cnt++) 
                  {
                      bt_sd_service_attribute_proto_desc_add_param(
                        pd_ptr, 
                        proto_desc_list.proto_desc[proto_desc_cnt].param[param_cnt].value,
                        proto_desc_list.proto_desc[proto_desc_cnt].param[param_cnt].size );
                  }
              }
            }
        }
        else
        {
          /* custom registration has failed */
          success = FALSE;
          break;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
      if( (success = bt_sd_xml_parse_lang_base_attrid_list( elem, xmllist_length, 
                                                            &lang_base_attr_id_list )) == TRUE )
      {
          if ( ( bt_sd_service_record_init_attribute(
                     ptr_srv_attr_rec, BT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST,
                     BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST ) ) != FALSE )
          {
              for (num_lang_base_cnt = 0; num_lang_base_cnt < lang_base_attr_id_list.num_lang_base_rec; num_lang_base_cnt++) 
              {
                  bt_sd_service_attribute_add_lang_base_attr_id(
                    ptr_srv_attr_rec, 
                    lang_base_attr_id_list.lang_base_rec[num_lang_base_cnt].lang_id,
                    lang_base_attr_id_list.lang_base_rec[num_lang_base_cnt].char_enc_id,
                    lang_base_attr_id_list.lang_base_rec[num_lang_base_cnt].base_attr_id);
              }
          }
      }
      break;
    case BT_SD_ATTR_TYPE_UINT_LIST:
      if( (success = bt_sd_xml_parse_uint_list( elem, xmllist_length, 
                                                &uint_list )) == TRUE )
      {
        memcpy( (void *)&ptr_srv_attr_rec->attr_value.uint_list,
                (void *)&uint_list, sizeof(bt_sd_uint_list_type) );
      }
      break;
    case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
      if( (success = bt_sd_xml_parse_add_proto_desc_list( elem, xmllist_length, 
                                                          add_proto_desc_lists )) == TRUE )
      {
          memcpy( (void *)ptr_srv_attr_rec->attr_value.add_proto_desc_lists,
                (void *)add_proto_desc_lists, sizeof(bt_sd_proto_desc_list_type) );
      }
      break;
    case BT_SD_ATTR_TYPE_HID_DESC_LIST:
      if( (success = bt_sd_xml_parse_hid_desc_list( elem, xmllist_length, 
                                                    &hid_desc_list )) == TRUE )
      {
        memcpy( (void *)&ptr_srv_attr_rec->attr_value.hid_desc_list,
                (void *)&hid_desc_list, sizeof(bt_sd_hid_desc_list_type) );
      }
      break;
  }

  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_primitive

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_primitive
(
  char*                     xmllist,
  uint16                    xmllist_length,
  bt_sd_srv_attr_rec_type*  ptr_srv_attr_rec
)
{
  char*                   elem = NULL;
  uint16                  elem_length;
  bt_sd_xml_elem_type     xml_elem_type;
  boolean                 found = FALSE;

  byte                    uint128_value[ 16 ];
  boolean                 bool_flag;
  uint64                  num = 0;
  bt_sd_uuid128_type      uuid128;
  int                     i;

  bt_sd_xml_trim_white_space( &xmllist, &xmllist_length );
  xml_elem_type = bt_sd_xml_get_next_element_type( &xmllist, &elem,
                                                   &xmllist_length, &elem_length );

  switch( ptr_srv_attr_rec->attr_type )
  {
    case BT_SD_ATTR_TYPE_STRING:
      if( xml_elem_type == BT_SD_XML_ELEM_STRING )
      {
        bt_sd_xml_trim_white_space( &elem, &elem_length );
        if( elem_length < BT_SD_MAX_TEXT_STRING_LEN )
        {
          /* std_strlcpy will not work here as the source 
             string is not null terminated */
          for( i=0; i<elem_length; i++ )
          {
            ptr_srv_attr_rec->attr_value.str[ i ] = elem[ i ];
          }
          ptr_srv_attr_rec->attr_value.str[ elem_length ] = '\0';
          found = TRUE;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_BOOL:
      if( xml_elem_type == BT_SD_XML_ELEM_BOOL &&
          bt_sd_xml_parse_bool( elem, elem_length, &bool_flag ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.bool_flag = bool_flag;
      }
      break;
    case BT_SD_ATTR_TYPE_UINT8:
      if( xml_elem_type == BT_SD_XML_ELEM_UINT8 &&
          bt_sd_xml_parse_uint( elem, elem_length, &num, 1 ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.primitive_value = num;
      }
      break;
    case BT_SD_ATTR_TYPE_UINT16:
      if( xml_elem_type == BT_SD_XML_ELEM_UINT16 &&
          bt_sd_xml_parse_uint( elem, elem_length, &num, 2 ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.primitive_value = num;
      }
      break;
    case BT_SD_ATTR_TYPE_UINT32:
      if( xml_elem_type == BT_SD_XML_ELEM_UINT32 &&
          bt_sd_xml_parse_uint( elem, elem_length, &num, 4 ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.primitive_value = num;
      }
      break;
    case BT_SD_ATTR_TYPE_UINT64:
      if( xml_elem_type == BT_SD_XML_ELEM_UINT64 &&
          bt_sd_xml_parse_uint( elem, elem_length, &num, 8 ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.primitive_value = num;
      }
      break;
    case BT_SD_ATTR_TYPE_UUID:
      if( xml_elem_type == BT_SD_XML_ELEM_UUID16 &&
          bt_sd_xml_parse_uuid16( elem, elem_length, (bt_sd_uuid_type *)&num ) )
      {
        found = TRUE;
        ptr_srv_attr_rec->attr_value.primitive_value = num;
      }
      break;
    case BT_SD_ATTR_TYPE_UUID128:
      if( xml_elem_type == BT_SD_XML_ELEM_UUID128 &&
          bt_sd_xml_parse_uuid128( elem, elem_length, &uuid128 ) )
      {
        found = TRUE;
        memcpy( (void *)&ptr_srv_attr_rec->attr_value.uuid128,
                (void *)&uuid128, sizeof(bt_sd_uuid_type) );
      }
      break;
    case BT_SD_ATTR_TYPE_UINT128:
      if( xml_elem_type == BT_SD_XML_ELEM_UINT128 &&
          bt_sd_xml_parse_uint128( elem, elem_length, uint128_value ) )
      {
        found = TRUE;
        memcpy( ptr_srv_attr_rec->attr_value.uint128_value,
                uint128_value, 16*sizeof(byte) );
      }
      break;
    default:
      found = FALSE;
      break;
  }
  return (found);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_attribute

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_attribute 
(
  char*                      xmlstring,
  uint16                     xmlstring_length,
  bt_sd_srv_attr_rec_type*   srv_attr_rec
)
{
  char*   s1 = NULL;
  char*   elem = NULL;
  uint64  num;
  boolean success = TRUE;

  BT_MSG_DEBUG( "BT SD: Parse XML attr: %u", xmlstring_length, 0, 0);

  bt_sd_xml_trim_white_space( &xmlstring, &xmlstring_length );

  elem = bt_sd_xml_get_nested_element( xmlstring, &xmlstring_length, 
                                       BT_XML_ATTR_BEGIN, BT_XML_ATTR_END );
  if( elem == NULL )
  {
    return FALSE;
  }
                     
  /* Extract attr id */
  s1 = strstr( xmlstring, BT_XML_ATTR_BEGIN );
  if( s1 == NULL )
  {
    return FALSE;
  }

  success = bt_sd_xml_ascii_to_hex( s1 + strlen(BT_XML_ATTR_BEGIN), 4, &num );

  srv_attr_rec->attr_id = (uint16)num;
  srv_attr_rec->attr_type = bt_sd_get_attr_type( srv_attr_rec->attr_id );

  if( success == TRUE )
  {
    switch( srv_attr_rec->attr_type )
    {
      case BT_SD_ATTR_TYPE_UUID_LIST:
      case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
      case BT_SD_ATTR_TYPE_UINT_LIST:
      case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
      case BT_SD_ATTR_TYPE_HID_DESC_LIST:
        success = bt_sd_xml_parse_list( elem, xmlstring_length, srv_attr_rec );
        break;

      case BT_SD_ATTR_TYPE_STRING:
      case BT_SD_ATTR_TYPE_UINT8:
      case BT_SD_ATTR_TYPE_UINT16:
      case BT_SD_ATTR_TYPE_UINT32:
      case BT_SD_ATTR_TYPE_UINT64:
      case BT_SD_ATTR_TYPE_BOOL:
      case BT_SD_ATTR_TYPE_UUID:
      case BT_SD_ATTR_TYPE_UUID128:
      case BT_SD_ATTR_TYPE_UINT128:
        success = bt_sd_xml_parse_primitive( elem, xmlstring_length, srv_attr_rec );
        break;

      case BT_SD_ATTR_TYPE_UNKNOWN:
      default:
        success = FALSE;
        break;

    }
  }
  return (success);
}


/*===========================================================================

FUNCTION
  bt_sd_xml_parse_service_record

DESCRIPTION
  
===========================================================================*/
boolean bt_sd_xml_parse_service_record
(
  char*                 xmlstring,
  uint16                xmlstring_length,
  bt_sd_srv_rec_type*   srv_rec
)
{
  char*                      head = NULL;
  char*                      tail = NULL;
  char*                      elem = NULL;
  char*                      attr = NULL;
  uint16                     attr_len;
  bt_sd_srv_attr_rec_type    srv_attr_rec;
  boolean                    success = TRUE;
  uint16                     elem_length;

  BT_MSG_DEBUG( "BT SD: Parse XML rec: %u", xmlstring_length, 0, 0);

  bt_sd_xml_trim_white_space( &xmlstring, &xmlstring_length );

  elem_length = xmlstring_length;
  elem = bt_sd_xml_get_nested_element( xmlstring, &elem_length, 
                                       BT_XML_SREC_BEGIN, BT_XML_SREC_END );
  if( elem == NULL )
  {
    return FALSE;
  }

  head = elem;
  tail = elem + elem_length;

  srv_rec->num_srv_attr = 0;

  while( head < tail &&
         success == TRUE )
  {
     attr = bt_sd_xml_get_next_attribute( &head, &elem_length, &attr_len );
     if( attr != NULL )
     {
       /* Initializing to reset the values set in the privious iteration */
       memset( &srv_attr_rec, 0, sizeof(bt_sd_srv_attr_rec_type) );

       if( (success = bt_sd_xml_parse_attribute( attr, attr_len, 
                                                 &srv_attr_rec )) == TRUE )
       {
         memcpy( (void *)&srv_rec->srv_attr[ srv_rec->num_srv_attr ], 
                 (void *)&srv_attr_rec, sizeof( bt_sd_srv_attr_rec_type) );
         srv_rec->num_srv_attr ++;
       }
     }
     else
     {
       success = FALSE;
     }
  }
  return (success);
}

#endif /* FEATURE_BT */
