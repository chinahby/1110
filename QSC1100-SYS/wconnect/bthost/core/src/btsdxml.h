#ifndef _BTSDXML_H
#define _BTSDXML_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    S D P    M O D U L E

DESCRIPTION
  This file contains declarations external to the Bluetooth SDP XML module.

Copyright (c) 2007 by Qualcomm Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsdxml.h#1 $ $DateTime

$Log$
 *
 *    #4       14 Dec 2007            HM
 * Added BT_SD_XML_ELEM_BYTES in enum bt_sd_xml_elem_type
 *
 *    #3       03 Dec 2007            HM
 * Added support to parse XML
 *
 *    #2       03 Dec 2007            PN
 * Added extern declarations for bt_sd_create_xml_service_db() and 
 * bt_sd_create_xml_service_record().
 *
 *    #1         29 Nov 2007         HM
 * Initial Revision.
 *
 *

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Type Definitions                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* XML Data Constructs */

#define BT_XML_LT             "<"
#define BT_XML_LT_CLOSE       "</"
#define BT_XML_GT             ">"

#define BT_XML_VER            "<?xml version=\"1.0\"?>\n"

#define BT_XML_SDP_DB         "sdpdb"
#define BT_XML_SERVICE_REC    "srec"
#define BT_XML_ATTRIBUTE      "attr"

#define BT_XML_SDPDB_BEGIN      "<sdpdb>\n"
#define BT_XML_SDPDB_END        "</sdpdb>\n"

#define BT_XML_SREC_BEGIN      "<srec>"
#define BT_XML_SREC_END        "</srec>"
#define BT_XML_SREC_BEGIN_NL   "<srec>\n"
#define BT_XML_SREC_END_NL     "</srec>\n"

#define BT_XML_ATTR_BEGIN      "<attr id=\"0x"
#define BT_XML_ATTR_END        "</attr>"
#define BT_XML_ATTR_BEGIN_NL   "<attr>\n"
#define BT_XML_ATTR_END_NL     "</attr>\n"

#define BT_XML_LIST_BEGIN      "<list>"
#define BT_XML_LIST_END        "</list>"
#define BT_XML_LIST_BEGIN_NL   "<list>\n"
#define BT_XML_LIST_END_NL     "</list>\n"


/* UINTs */
#define BT_XML_UINT8_BEGIN      "<uint8>"
#define BT_XML_UINT8_END        "</uint8>"
#define BT_XML_UINT16_BEGIN     "<uint16>"
#define BT_XML_UINT16_END       "</uint16>"
#define BT_XML_UINT32_BEGIN     "<uint32>"
#define BT_XML_UINT32_END       "</uint32>"
#define BT_XML_UINT64_BEGIN     "<uint64>"
#define BT_XML_UINT64_END       "</uint64>"
#define BT_XML_UINT128_BEGIN    "<uint128>"
#define BT_XML_UINT128_END      "</uint128>"

/* UUIDs */
#define BT_XML_UUID16_BEGIN     "<uuid16>"
#define BT_XML_UUID16_END       "</uuid16>"
#define BT_XML_UUID128_BEGIN    "<uuid128>"
#define BT_XML_UUID128_END      "</uuid128>"

/* Other types */
#define BT_XML_STRING_BEGIN     "<string>"
#define BT_XML_STRING_END       "</string>"
#define BT_XML_BOOL_BEGIN       "<bool>"
#define BT_XML_BOOL_END         "</bool>"
#define BT_XML_BYTES_BEGIN      "<bytes>"
#define BT_XML_BYTES_END        "</bytes>"

#define BT_SD_XML_BOOL_VAL_TRUE    "true"
#define BT_SD_XML_BOOL_VAL_FALSE   "false"

typedef enum
{
  BT_SD_XML_ELEM_UNKNOWN,
  BT_SD_XML_ELEM_ATTR,
  BT_SD_XML_ELEM_LIST,
  BT_SD_XML_ELEM_UUID16,
  BT_SD_XML_ELEM_UUID128,
  BT_SD_XML_ELEM_UINT8,
  BT_SD_XML_ELEM_UINT16,
  BT_SD_XML_ELEM_UINT32,
  BT_SD_XML_ELEM_UINT64,
  BT_SD_XML_ELEM_UINT128,
  BT_SD_XML_ELEM_BOOL,
  BT_SD_XML_ELEM_STRING,
  BT_SD_XML_ELEM_BYTES
} bt_sd_xml_elem_type;


extern boolean bt_sd_create_xml_service_record(
  const bt_sd_srv_rec_type*  srv_rec_ptr,
  dsm_item_type**            dsm_ptr
);

extern boolean bt_sd_create_xml_service_db(
  const bt_sd_db_enum_type   db_type,
  dsm_item_type**            dsm_ptr
);

extern boolean bt_sd_xml_parse_service_record
(
  char*                 xmlstring,
  uint16                xmlstring_length,
  bt_sd_srv_rec_type*   srv_rec
);

extern boolean bt_sd_xml_parse_attribute 
(
  char*                      xmlstring,
  uint16                     xmlstring_length,
  bt_sd_srv_attr_rec_type*   srv_attr_rec
);

#endif /* FEATURE_BT */

#endif  /*  _BTSDXML_H  */
