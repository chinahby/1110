/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              B L U E T O O T H    S D P    D B    M O D U L E

GENERAL DESCRIPTION
  This contains the Bluetooth Service Discovery Protocol (SDP) Database
  Module

EXTERNALIZED FUNCTIONS
  bt_sd_initialize_database
  bt_sd_create_service_record
  bt_sd_create_syncml_service_record
  bt_sd_alloc_service_record
  bt_sd_free_service_record
  bt_sd_enable_service_record
  bt_sd_any_local_services_enabled
  bt_sd_update_service_connectable
  bt_sd_any_local_services_connectable
  bt_sd_find_service_record_by_handle
  bt_sd_find_service_record_by_uuid
  bt_sd_find_service_record_by_uuid128
  bt_sd_find_syncml_service_record
  bt_sd_match_service_record
  bt_sd_match_service_attribute
  bt_sd_clear_service_record_match
  bt_sd_clear_service_attribute_match
  bt_sd_find_service_attribute
  bt_sd_service_record_init_attribute
  bt_sd_service_record_add_attribute
  bt_sd_service_attribute_add_uuid
  bt_sd_service_attribute_set_uuid128
  bt_sd_service_attribute_add_proto_desc
  bt_sd_service_attribute_proto_desc_add_param
  bt_sd_service_attribute_proto_desc_uint_list_param_add_val
  bt_sd_service_attribute_add_lang_base_attr_id
  bt_sd_service_attribute_set_str
  bt_sd_service_attribute_set_value
  bt_sd_service_attribute_set_bool
  bt_sd_service_attribute_set_uint128
  bt_sd_service_attribute_add_hid_class_desc

Copyright (c) 2000 - 2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btsddb.c_v   1.9   12 Aug 2002 18:50:24   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsddb.c#2 $ $DateTime: 2009/08/10 13:09:08 $

$Log:   O:/src/asw/COMMON/vcs/btsddb.c_v  $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-08-07   gs  Updated profile descriptor list for PBAP
  2008-08-19   sa  Fixed Critical Lint warning.
  2008-07-28   hm  Fixed potential OOB array access
  2008-06-26   gs  Fixed size calc in bt_sd_service_attribute_uint_list_add_val()

 *
 *    #44        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #43        20 Mar  2007           RY
 * Updated bt_sd_match_uuid to check for 128-bit UUID in a Protocol Descriptor.
 *
 *    #42        07 Mar  2007           RY
 * Updated bt_sd_service_record_add_common_attributes to correctly add 
 * all protocol descriptors to the Bluetooth Profile Descriptor List.
 *
 *    #41        14 Nov  2006           RY
 * Added SD support for custom 128-bit UUID
 *
 *    #40        25 Jul 2006            JTL
 * Adding in a second copy of the version number for A2DP and
 * AVRCP to be spec compliant.
 *
 *    #39        21 Jul 2006            RY
 * Added bt_sd_validate_service_record.
 * 
 *    #37-38     11 Jul 2006            RY
 * Removed extraneous line feed characters.
 * 
 *    #36        23 Jun 2006            JH
 * OBEX protocol descriptor is now added in 
 *  bt_sd_update_obex_profile_service_record() in btsd.c instead of 
 *  bt_sd_create_service_record().
 *
 *    #35        15 Nov 2005            DP
 * Fixed connectable feature added for SD service registrations.
 *
 *    #34        10 Nov 2005            DP
 * Added connectable controls to SD service registration.
 *
 *    #33        18 Oct 2005            RY
 * Updated get_next_srv_rec to fix array out-of-bound problem.
 *
 *    #32        07 Sep 2005            RY
 * Added string length to the HID class descriptor definition and handling.
 *
 *    #31        02 Sep 2005            RY
 * Added support for the HID descriptor list service attribute.
 *
 *    #30        30 Aug 2005            RY
 * Added Service Discovery Server to the ServiceClassIDList attribute for
 * WOLF5 only.
 *
 *    #29        23 Jun 2005            JTL
 * Added support for registering A2DP and AVRCP; now adds protocols and profile
 * descriptors to the record.
 *
 *    #28        15 Jun 2005            RY
 * Added bt_sd_service_record_init_attribute.
 *
 *    #27        24 Jan 2005            PN
 * Added support for SIM Access profile.
 *
 *    #26        30 Dec 2004            JH
 * Added support for parsing 128-bit unsigned primitive.
 *
 *    #25        29 Dec 2004            JH
 * Added support for parsing the additional protocol descriptor lists attribute.
 * Modified bt_sd_create_service_record() to support adding L2CAP PSM to record.
 * Modified bt_sd_find_service_record_by_uuid() to support service record
 *  identification using SCN or PSM.
 *
 *    #24        23 Nov 2004            JH
 * Corrected the Network attribute of Handsfree AG SDP record.
 *
 *    #23        01 Nov 2004            JH
 * Removed bt_sd_service_attribute_proto_desc_uint_list_param_add_val since uint
 *  list within protocol descriptor is no longer stored in result database.
 *
 *    #22        29 Sep 2004            JH
 * Added bt_sd_service_attribute_proto_desc_uint_list_param_add_val to handle
 *  protocol descriptor parameters which are in the form of a data element 
 *  sequence. 
 *
 *    #21        30 Aug 2004            JH
 * 1. Modified bt_sd_create_service_record to add OBEX to protocol descriptor list
 *    if OBEX service record is being created.
 * 2. Added bt_sd_create_syncml_service_record and bt_sd_find_syncml_service_record.
 * 3. Modified bt_sd_match_service_record to support matching 128-bit SyncML UUIDs
 *    and 128-bit service IDs. 
 *
 *    #20        25 Aug 2004            DP
 * Removed featurization of BT HF.
 *
 *    #19        24 Aug 2004            JH
 * Put back missing "break" in case statement when populating BT profile 
 *  descriptor list in bt_sd_create_service_record.
 *
 *    #17        03 AUG 2004            JTL
 * Added quick support for CTP/ICP registration with SDP.
 * Modified bt_sd_create_service_record
 *
 *    #16        30 Apr PDT             RY
 * Added the support of 128-bit UUID for SyncML.
 *
 *    #15        14 Apr 2004            RY
 * Added support for creating BIP related service records.
 *
 *    #14        29 Jan 2004            DP
 * Fixed compiler warning.
 *
 *    #13        27 Jan 2004            RY
 * Added support for BT_SD_ATTR_TYPE_UUID.
 *
 *    #12        14 Jan 2004            DP
 * Registering service only affects page scanning now.
 *
 *    #11        19 Dec 2003            DP
 * Undo over error checking.
 *
 *    #10        19 Dec 2003            DP
 * Improved error handling when building/parsing SDP database records.
 *
 *    #9         9  Nov 2003            RY
 * Updated bt_sd_free_service_record to zero out just the service attribute.
 *
 *    #8        4  Sep 2003            RY
 * Updated bt_sd_free_service_record to decrement the 'enabled' and 'matched'
 * count.
 *
 *    #7        03 Jul 2003            RY
 * 1. Added the support for uint64 and unsigned integer list.
 * 2. Removed ASSERTs by handling error cases more gracefully.
 * 3. Updated bt_sd_create_service_record to support the ImagingResponder
 *    service class.
 *
 *    Rev 1.9   12 Aug 2002 18:50:24   ryuen
 * 1. Updated bt_sd_free_service_record to zero out the service record
 * 2. Removed the SERVICE_DISCOVERY_SERVER service class ID from the local
 *    SDP database
 *
 *    Rev 1.8   07 Jun 2002 10:37:08   ryuen
 * Added the GENERIC_AUDIO UUID to the AG service record.  This is enable
 * interoperability with Plantroncis
 *
 *    Rev 1.7   21 Feb 2002 17:17:30   ryuen
 * Updated bt_sd_alloc_service_record and bt_sd_service_record_add_attribute
 * to return 0 if the service record/attribute table is full, instead of
 * asserting
 *
 *    Rev 1.6   29 Aug 2001 11:57:40   propach
 * Merged certification tip onto main tip.
 *
 *    Rev 1.5.1.2   16 Jul 2001 10:25:18   ryuen
 * Updated service database
 *
 *    Rev 1.5.1.1   15 Jul 2001 14:17:50   ryuen
 * 1. Added Service Discovery Server to the ServiceClassIDList attribute
 * 2. Removed Generic Audio from the ServiceClassIDList attribute for the
 *    Headset/Audio Gateway service record
 *
 *    Rev 1.5.1.0   29 Jun 2001 15:25:40   waynelee
 * No change.
 *
 *    Rev 1.5   25 May 2001 15:41:10   ryuen
 * Renamed FEATURE_BT_MOBILE_MAP to FEATURE_BT_LPOS
 *
 *    Rev 1.4   25 May 2001 10:12:52   ryuen
 * Added support for FEATURE_BT_MOBILE_MAP
 *
 *    Rev 1.3   26 Apr 2001 23:30:38   ryuen
 * Updated bt_sd_match_service_record and bt_sd_match_service_attribute
 * to match all service records and attributes regardless of the maximum
 * attribute byte count parameter in the search pattern
 *
 *    Rev 1.2   29 Mar 2001 22:25:14   ryuen
 * Fixed bt_sd_match_service_record to perform matching on attribute ID only
 * if there's attribute ID in the search pattern
 *
 *    Rev 1.1   23 Feb 2001 12:53:52   ryuen
 * 1. Fixed the service attribute iterator to handle an empty service record
 * 2. Fixed bt_sd_initialize_database to probably initialize the SDP database
 * 3. Fixed bt_sd_match_service_record and bt_sd_match_service_attribute to
 *    terminate the match if the max_attr_byte_count is reached.  This fixes
 *    CR 16174
 *
 *    Rev 1.0   29 Jan 2001 17:10:12   propach
 * Initial revision.
 *
 *    Rev 1.11   21 Dec 2000 17:48:10   waynelee
 * Removed T_CORE_EMUL.
 *
 *    Rev 1.10   02 Nov 2000 23:39:56   ryuen
 * Changed FEATURE_BT_UNPLUGFEST_INTEROP to FEATURE_BT_TEST_SDP
 *
 *    Rev 1.9   30 Oct 2000 18:13:02   ryuen
 * Updated bt_sd_match_service_record to report a match only if all UUIDs in
 * the search pattern exist in the service record
 *
 *    Rev 1.8   05 Sep 2000 18:57:44   propach
 * Updated featurization.
 *
 *    Rev 1.7   30 Aug 2000 09:42:50   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.6   29 Aug 2000 08:26:24   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.5   08 Aug 2000 10:48:40   ryuen
 * 1. Updated the service record iterator to support both the local DB and
 *    the service search result DB
 * 2. Fixed a bug in get_next_srv_attr where we're returning service attribute
 *    records that are unused
 * 3. Updated to 32-bit service record handle
 *
 *    Rev 1.4   03 Aug 2000 14:57:28   ryuen
 * Minor bug fixes
 *
 *    Rev 1.3   02 Aug 2000 13:04:28   ryuen
 * Added support for sending SDP Service Search Attribute Request
 *
 *    Rev 1.2   02 Aug 2000 11:43:46   propach
 * Made standalone BT compile.
 *
 *    Rev 1.1   31 Jul 2000 22:23:38   ryuen
 * Fixed bt_sd_match_service_record() to calculate matched byte
 * count correctly
 *
 *    Rev 1.0   31 Jul 2000 11:55:08   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "btsd.h"
#include "btmsg.h"
#include "assert.h"
#include "msg.h"

#ifdef FEATURE_BT_EXTPF_AV
#include "btpfi.h"
#endif /* FEATURE_BT_EXTPF_AV */

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

/*
 * Definition for the internal service record iterator
 */
typedef struct
{
  int                    cur_srv_rec_index;
  bt_sd_db_enum_type     db_type;
  bt_sd_iter_enum_type   iter_type;
} bt_sd_srv_rec_iter_internal_type;

/*
 * Definition for the internal service attribute iterator
 */
typedef struct
{
  bt_sd_srv_rec_type*    cur_srv_rec_ptr;
  int                    cur_attr_index;
  bt_sd_iter_enum_type   iter_type;
} bt_sd_srv_attr_iter_internal_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                    Static Function Prototypes                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*
 * Local functions to manipulate the interal service record iterator
 */
LOCAL void init_srv_rec_iter( bt_sd_db_enum_type     db_type,
                              bt_sd_iter_enum_type   iter_type );
LOCAL const bt_sd_srv_rec_type* get_first_srv_rec( void );
LOCAL const bt_sd_srv_rec_type* get_next_srv_rec( void );
LOCAL boolean find_srv_rec_next_match( bt_sd_srv_db_type* db_ptr );

/*
 * Local functions to manipulate the interal service attribute iterator
 */
LOCAL void init_srv_attr_iter(
  const bt_sd_srv_rec_type*   srv_rec_ptr,
  bt_sd_iter_enum_type        iter_type
);
LOCAL const bt_sd_srv_attr_rec_type* get_first_srv_attr( void );
LOCAL const bt_sd_srv_attr_rec_type* get_next_srv_attr( void );
LOCAL boolean find_srv_attr_next_match( void );

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

LOCAL bt_sd_srv_db_type bt_sd_local_srv_db;
LOCAL bt_sd_srv_db_type bt_sd_srv_srch_result_db;

LOCAL bt_sd_srv_rec_iter_internal_type srv_rec_iter;
LOCAL bt_sd_srv_attr_iter_internal_type srv_attr_iter;

/*
 * Initialize the external service record iterator with the appropriate
 * function pointers
 */
const bt_sd_srv_rec_iter_type bt_sd_srv_rec_iter =
{
  init_srv_rec_iter,
  get_first_srv_rec,
  get_next_srv_rec
};

/*
 * Initialize the external service attribute iterator with the appropriate
 * function pointers
 */
const bt_sd_srv_attr_iter_type bt_sd_srv_attr_iter =
{
  init_srv_attr_iter,
  get_first_srv_attr,
  get_next_srv_attr
};

/* Lookup table based on size index */
const uint8 bt_sd_data_element_size_desc[] =
{
  1, 2, 4, 8, 16, 1, 2, 4
};

const bt_sd_uuid128_type bt_sd_syncml_server_uuid = 
                           BT_SD_SERVICE_CLASS_SYNCML_SERVER;
const bt_sd_uuid128_type bt_sd_syncml_client_uuid = 
                           BT_SD_SERVICE_CLASS_SYNCML_CLIENT;
const bt_sd_uuid128_type bt_sd_null_uuid128 = BT_SD_NULL_UUID128;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Static Functions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_sd_alloc_service_record

DESCRIPTION
  Return an unused service record

===========================================================================*/
bt_sd_srv_rec_type*  bt_sd_alloc_service_record
(
  bt_sd_db_enum_type  db_type
)
{

  bt_sd_srv_db_type*   db_ptr = NULL;
  bt_sd_srv_rec_type*  srv_rec_ptr = NULL;
  uint16               i;

  switch( db_type )
  {
    case BT_SD_DB_TYPE_LOCAL_DB:
      db_ptr = &bt_sd_local_srv_db;
      break;
    case BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB:
      db_ptr = &bt_sd_srv_srch_result_db;
      break;
    default:
      BT_ERR( "BT SD: Bad DB Type %x", db_type, 0, 0 );
      break;
  }

  if ( (db_ptr != NULL) &&
       (db_ptr->num_used_srv_rec < BT_SD_MAX_NUM_OF_SRV_REC) )
  {
    for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
    {
      if ( ! db_ptr->srv_rec[i].used_flag )
      {
        srv_rec_ptr = &( db_ptr->srv_rec[i] );
        break;
      }
    }

    if ( srv_rec_ptr != NULL )
    {
      /* Set all the appropriate fields */
      srv_rec_ptr->num_srv_attr = 0;
      srv_rec_ptr->used_flag = TRUE;
      srv_rec_ptr->enabled_flag = FALSE;
      srv_rec_ptr->connectable = FALSE;
      srv_rec_ptr->matched_flag = FALSE;
      srv_rec_ptr->matched_attr_byte_count = 0;
      srv_rec_ptr->syncml_flag = BT_SD_DB_NOT_SYNCML_V;
      
      db_ptr->num_used_srv_rec++;
    }
  }

  return ( srv_rec_ptr );

}


/*===========================================================================

FUNCTION
  bt_sd_free_service_record

DESCRIPTION
  Free up a used service record

===========================================================================*/

void bt_sd_free_service_record(
  bt_sd_srv_rec_type* srv_rec_ptr
)
{
  int i;

  if ( ! srv_rec_ptr->used_flag )
  {
    return;
  }

  if ( srv_rec_ptr->enabled_flag )
  {
    srv_rec_ptr->enabled_flag = FALSE;
    bt_sd_local_srv_db.num_enabled_srv_rec--;
  }

  if ( srv_rec_ptr->connectable != FALSE )
  {
    srv_rec_ptr->connectable = FALSE;
    bt_sd_local_srv_db.num_connectable_srv_rec--;
  }

  if ( srv_rec_ptr->matched_flag )
  {
    srv_rec_ptr->matched_flag = FALSE;
    bt_sd_local_srv_db.num_matched_srv_rec--;
  }

  srv_rec_ptr->used_flag    = FALSE;
  srv_rec_ptr->enabled_flag = FALSE;
  srv_rec_ptr->connectable  = FALSE;
  srv_rec_ptr->matched_flag = FALSE;
  srv_rec_ptr->num_srv_attr = 0;
  srv_rec_ptr->matched_attr_byte_count = 0;

  /* Zero out the service attributes */
  for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_ATTR; i++ )
  {
    memset( ( byte* ) &srv_rec_ptr->srv_attr[i],
            0,
            sizeof( bt_sd_srv_attr_rec_type ) );
  }

  bt_sd_local_srv_db.num_used_srv_rec--;
}


/*===========================================================================

FUNCTION
  bt_sd_validate_service_record

DESCRIPTION
  Validate a service record by checking for empty service record and
  invalid UUIDs

===========================================================================*/

void bt_sd_validate_service_record(
  bt_sd_srv_rec_type*   srv_rec_ptr
)
{
  boolean                    valid = TRUE;
  bt_sd_srv_attr_rec_type*   srv_attr_ptr;
  uint16                     i, j;

  /* Check for empty service record */
  if ( srv_rec_ptr->num_srv_attr == 0 )
  {
    valid = FALSE;
  }

  /* Check for invalid 128-bit UUID */
  bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_ALL );
  for ( srv_attr_ptr =
          ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
        ( srv_attr_ptr != 0 ) && valid;
        srv_attr_ptr =
          ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
  {
    switch( srv_attr_ptr->attr_type )
    {
      case BT_SD_ATTR_TYPE_UUID_LIST:
        for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid128; i++)
        {
          if ( BT_SD_UUID128_EQUAL(
                 &srv_attr_ptr->attr_value.uuid_list.uuid128[i],
                 &bt_sd_null_uuid128 ) )
          {
            valid = FALSE;
            break;
          }
        }
        for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid; i++ )
        {
          if ( srv_attr_ptr->attr_value.uuid_list.uuid[i] == BT_SD_INVALID_UUID )
          {
            valid = FALSE;
            break;
          }
        }
        break;
      case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
        for ( i = 0;
              i < srv_attr_ptr->attr_value.proto_desc_list.num_proto_desc;
              i++ )
        {
          if ( srv_attr_ptr->attr_value.proto_desc_list.proto_desc[i].uuid ==
                 BT_SD_INVALID_UUID )
          {
            valid = FALSE;
            break;
          }
        }
        break;
      case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
        for ( i = 0;
              i < BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS;
              i++ )
        {
          for ( j = 0;
                j < srv_attr_ptr->attr_value.add_proto_desc_lists[i].num_proto_desc;
                j++ )
          {
            if ( srv_attr_ptr->attr_value.add_proto_desc_lists[i].proto_desc[j].uuid ==
                   BT_SD_INVALID_UUID )
            {
              valid = FALSE;
              break;
            }
          }
        }
        break;
      case BT_SD_ATTR_TYPE_UUID:
        if ( srv_attr_ptr->attr_value.primitive_value == BT_SD_INVALID_UUID )
        {
          valid = FALSE;
        }
        break;
      case BT_SD_ATTR_TYPE_UUID128:
        if ( BT_SD_UUID128_EQUAL(
               &srv_attr_ptr->attr_value.uuid128,
               &bt_sd_null_uuid128 ) )
        {
          valid = FALSE;
        }
        break;
      default:
        break;
    }
  }

  if ( ! valid )
  {
    /* Invalidate the service record */
    BT_ERR( "BT SD: Invalid service record %ld.  Discarding",
            srv_rec_ptr->srv_rec_id, 0, 0 );

    srv_rec_ptr->used_flag    = FALSE;
    srv_rec_ptr->enabled_flag = FALSE;
    srv_rec_ptr->num_srv_attr = 0;

    /* Zero out the service attributes */
    for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_ATTR; i++ )
    {
      memset( ( byte* ) &srv_rec_ptr->srv_attr[i],
              0,
              sizeof( bt_sd_srv_attr_rec_type ) );
    }

    bt_sd_srv_srch_result_db.num_used_srv_rec--;
  }
}


/*===========================================================================

FUNCTION
  bt_sd_service_record_init_attribute

DESCRIPTION
  Initialize a service attribute

===========================================================================*/

boolean bt_sd_service_record_init_attribute
(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint16                    attr_id,
  bt_sd_srv_attr_enum_type  attr_type
)
{
  boolean                      status = TRUE;
  uint16                       i;
  bt_sd_proto_desc_list_type*  pdl_ptr;

  memset( ( byte* ) srv_attr_ptr, 0, sizeof( bt_sd_srv_attr_rec_type ) );

    switch( attr_type )
    {
      case BT_SD_ATTR_TYPE_UUID_LIST:
      case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      case BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST:
      case BT_SD_ATTR_TYPE_UINT_LIST:
      case BT_SD_ATTR_TYPE_HID_DESC_LIST:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
        srv_attr_ptr->header.size_index = 5;
        srv_attr_ptr->header.hdr_len = 2;
        srv_attr_ptr->header.attr_value_len = 0;
        break;
      case BT_SD_ATTR_TYPE_ADD_PROTO_DESC_LISTS:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
        srv_attr_ptr->header.size_index = 5;
        srv_attr_ptr->header.hdr_len = 2;
        srv_attr_ptr->header.attr_value_len = 0;
        for ( i = 0; i < BT_SD_MAX_NUM_OF_ADD_PROTO_DESC_LISTS; i++ )
        {
          pdl_ptr = &srv_attr_ptr->attr_value.add_proto_desc_lists[i];
          pdl_ptr->header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
          pdl_ptr->header.size_index = 5;
          pdl_ptr->header.hdr_len = 2;
          pdl_ptr->header.attr_value_len = 0;
        }
        break;
      case BT_SD_ATTR_TYPE_STRING:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_TEXT_STRING;
        srv_attr_ptr->header.size_index = 5;
        srv_attr_ptr->header.hdr_len = 2;
        srv_attr_ptr->header.attr_value_len = 0;
        break;
      case BT_SD_ATTR_TYPE_UINT8:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UNSIGNED_INT;
        srv_attr_ptr->header.size_index = 0;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UINT16:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UNSIGNED_INT;
        srv_attr_ptr->header.size_index = 1;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UINT32:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UNSIGNED_INT;
        srv_attr_ptr->header.size_index = 2;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UINT64:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UNSIGNED_INT;
        srv_attr_ptr->header.size_index = 3;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_BOOL:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_BOOL;
        srv_attr_ptr->header.size_index = 0;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UUID:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UUID;
        srv_attr_ptr->header.size_index = 1;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UUID128:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UUID;
        srv_attr_ptr->header.size_index = 4;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
      case BT_SD_ATTR_TYPE_UINT128:
        srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_UNSIGNED_INT;
        srv_attr_ptr->header.size_index = 4;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len =
          bt_sd_data_element_size_desc[ srv_attr_ptr->header.size_index ];
        break;
	  #ifdef CUST_EDITION
	  case BT_SD_ATTR_TYPE_UNKNOWN:
	  	srv_attr_ptr->header.type = BT_SD_DATA_ELEMENT_NULL;
        srv_attr_ptr->header.size_index = 0;
        srv_attr_ptr->header.hdr_len = 1;
        srv_attr_ptr->header.attr_value_len = 0;
		break;
	  #endif
	  
      default:
        BT_ERR( "BT SD: Bad Attr T %x", attr_type, 0, 0 );
      status = FALSE;
        break;
    }

  if ( status != FALSE )
    {
      srv_attr_ptr->attr_id = attr_id;
      srv_attr_ptr->attr_type = attr_type;
      srv_attr_ptr->matched_flag = FALSE;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_sd_service_record_add_attribute

DESCRIPTION
  Add a service attribute to a service record

===========================================================================*/

bt_sd_srv_attr_rec_type* bt_sd_service_record_add_attribute
(
  bt_sd_srv_rec_type*       srv_rec_ptr,
  uint16                    attr_id,
  bt_sd_srv_attr_enum_type  attr_type
)
{

  bt_sd_srv_attr_rec_type*  srv_attr_ptr = NULL;

  if ( srv_rec_ptr->num_srv_attr < BT_SD_MAX_NUM_OF_SRV_ATTR )
  {
    srv_attr_ptr = &( srv_rec_ptr->srv_attr[ srv_rec_ptr->num_srv_attr ] );

  if ( ( bt_sd_service_record_init_attribute(
             srv_attr_ptr, attr_id, attr_type ) ) != FALSE )
    {
      srv_rec_ptr->num_srv_attr++;
    }
    else
    {
      srv_attr_ptr = 0;
    }
  }

  return ( srv_attr_ptr );

}


/*===========================================================================

FUNCTION
  bt_sd_service_attribute_add_uuid
  bt_sd_service_attribute_set_uuid128
  bt_sd_service_attribute_add_proto_desc
  bt_sd_service_attribute_proto_desc_add_param
  bt_sd_service_attribute_add_lang_base_attr_id
  bt_sd_service_attribute_uint_list_add_val
  bt_sd_service_attribute_set_str
  bt_sd_service_attribute_set_value
  bt_sd_service_attribute_set_uint128
  bt_sd_service_attribute_set_bool
  bt_sd_service_attribute_add_hid_class_desc

DESCRIPTION
  Add an attribute value of various types to a service attribute record

===========================================================================*/

void bt_sd_service_attribute_add_uuid(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  bt_sd_uuid_type            uuid
)
{
  uint8  count = srv_attr_ptr->attr_value.uuid_list.num_uuid;

  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID_LIST) &&
       (count < BT_SD_MAX_NUM_OF_UUID) )
  {
    srv_attr_ptr->header.attr_value_len += 3;
      /* Data element header (1) + UUID (2) */
    srv_attr_ptr->attr_value.uuid_list.uuid[count] = uuid;
    srv_attr_ptr->attr_value.uuid_list.num_uuid++;
  }
  else if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID )
  {
    srv_attr_ptr->attr_value.primitive_value = uuid;
  }
  #ifdef CUST_EDITION
  else if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID128 )
  {
	srv_attr_ptr->attr_type = BT_SD_ATTR_TYPE_UUID;
    srv_attr_ptr->attr_value.primitive_value = uuid;
  }
  #endif
  else
  {
    BT_ERR( "BT SD: Bad UUID Add T %x C %x",
            srv_attr_ptr->attr_type, count, 0 );
  }
}


void bt_sd_service_attribute_set_uuid128(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  const bt_sd_uuid128_type*  uuid128_ptr
)
{
  uint8  i;

  if ( uuid128_ptr != NULL )
  {
    if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID_LIST )
    {
      i = srv_attr_ptr->attr_value.uuid_list.num_uuid128;
      if ( i < BT_SD_MAX_NUM_OF_UUID128 )
      {
        srv_attr_ptr->header.attr_value_len += 17;
          /* Data element header (1) + UUID (16) */
        srv_attr_ptr->attr_value.uuid_list.uuid128[i] = *uuid128_ptr;
        srv_attr_ptr->attr_value.uuid_list.num_uuid128++;
      }
      else
      {
        BT_ERR( "BT SD: Bad UUID128 Add T %x C %x",
                srv_attr_ptr->attr_type, i, 0 );
      }
    }
    else if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UUID128 )
    {
      /* only Service ID should have attr_type = BT_SD_ATTR_TYPE_UUID128 */
      srv_attr_ptr->attr_value.uuid128 = *uuid128_ptr;
    }
    else
    {
      BT_ERR( "BT SD: Bad UUID Set T %x",
              srv_attr_ptr->attr_type, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT SD: Bad UUID Set - Null UUID128 pointer", 0, 0, 0 );
  }
}


bt_sd_proto_desc_type* bt_sd_service_attribute_add_proto_desc(
  bt_sd_srv_attr_rec_type*   srv_attr_ptr,
  bt_sd_uuid_type            uuid,
  const bt_sd_uuid128_type*  uuid128_ptr
)
{
  uint16 count = srv_attr_ptr->attr_value.proto_desc_list.num_proto_desc;
  bt_sd_proto_desc_type* proto_desc_ptr = NULL;

  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_PROTO_DESC_LIST) &&
       (count < BT_SD_MAX_NUM_OF_PROTO_DESC) )
  {
    proto_desc_ptr =
      &( srv_attr_ptr->attr_value.proto_desc_list.proto_desc[count] );

    proto_desc_ptr->header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
    proto_desc_ptr->header.size_index = 5; /* 2 additional bytes */
    proto_desc_ptr->srv_attr_ptr = (void*) srv_attr_ptr;
    proto_desc_ptr->num_param = 0;

    if ( uuid128_ptr == NULL ) /* Non-custom service */
    {
      srv_attr_ptr->header.attr_value_len += 5;
        /* Data element sequence header (2) +
           Data element header (1) + UUID (2) */
      proto_desc_ptr->header.attr_value_len = 3; /* UUID */
      proto_desc_ptr->is_uuid128 = FALSE;
      proto_desc_ptr->uuid = uuid;
    }
    else /* Custom service */
    {
      srv_attr_ptr->header.attr_value_len += 19;
        /* Data element sequence header (2) +
           Data element header (1) + UUID (16) */
      proto_desc_ptr->header.attr_value_len = 17; /* 128-bit UUID */
      proto_desc_ptr->is_uuid128 = TRUE;
      proto_desc_ptr->uuid128 = *uuid128_ptr;
    }

    srv_attr_ptr->attr_value.proto_desc_list.num_proto_desc++;
  }
  else
  {
    BT_ERR( "BT SD: Bad Add Proto Desc T %x C %x ",
            srv_attr_ptr->attr_type, count, 0 );
  }

  return proto_desc_ptr;
}


void bt_sd_service_attribute_proto_desc_add_param(
  bt_sd_proto_desc_type*    proto_desc_ptr,
  uint16                    param,
  uint8                     size
)
{
  uint16                     count = proto_desc_ptr->num_param;
  bt_sd_srv_attr_rec_type*   srv_attr_ptr;

  if ( count < BT_SD_MAX_NUM_OF_PROTO_PARAM )
  {
    if ( size > sizeof( uint16 ) )
    {
      BT_ERR( "BT SD: Exceed max size.  Truncating...", 0, 0, 0 );

      size = sizeof( uint16 );
    }

    srv_attr_ptr =
      (bt_sd_srv_attr_rec_type*) proto_desc_ptr->srv_attr_ptr;
    srv_attr_ptr->header.attr_value_len += size + 1;

    proto_desc_ptr->header.attr_value_len += size + 1;
      /* Data element header (1) + actual data (size) */
    proto_desc_ptr->param[count].value = param;
    proto_desc_ptr->param[count].size = size;
    proto_desc_ptr->num_param++;
  }
  else
  {
    BT_ERR( "BT SD: Exceed max # prot desc param.  Dropping...", 0, 0, 0 );
  }
}

void bt_sd_service_attribute_add_lang_base_attr_id(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint16                    lang_id,
  uint16                    char_enc_id,
  uint16                    base_attr_id
)
{
  uint16 count =
         srv_attr_ptr->attr_value.lang_base_attr_id_list.num_lang_base_rec;

  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST) &&
       (count < BT_SD_MAX_NUM_OF_LANG_BASE_REC) )
  {
    srv_attr_ptr->header.attr_value_len += 9;

    srv_attr_ptr->
      attr_value.lang_base_attr_id_list.lang_base_rec[count].lang_id = lang_id;
    srv_attr_ptr->
      attr_value.lang_base_attr_id_list.lang_base_rec[count].char_enc_id =
        char_enc_id;
    srv_attr_ptr->
      attr_value.lang_base_attr_id_list.lang_base_rec[count].base_attr_id =
        base_attr_id;
    srv_attr_ptr->
      attr_value.lang_base_attr_id_list.num_lang_base_rec++;
  }
  else
  {
    BT_ERR( "BT SD: Bad Add Lang Attr T %x C %x",
            srv_attr_ptr->attr_type, count, 0 );
  }
}


void bt_sd_service_attribute_uint_list_add_val(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint8                     size,
  uint64                    val
)
{
  uint8 count = srv_attr_ptr->attr_value.uint_list.num_val;
  uint8 size_diff;

  
  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT_LIST) &&
       (count < BT_SD_MAX_NUM_OF_UINT) )
  {
    if ( count == 0 )
    {
      srv_attr_ptr->attr_value.uint_list.size = size;
    }
    else
    {
      if ( srv_attr_ptr->attr_value.uint_list.size < size )
      {
        /* Go for the largest size element */
        size_diff = size - srv_attr_ptr->attr_value.uint_list.size;
        srv_attr_ptr->attr_value.uint_list.size = size;

        /* Fix previously sized element(s) */
        srv_attr_ptr->header.attr_value_len += count * size_diff;
      }
    }

    srv_attr_ptr->header.attr_value_len += 1 + srv_attr_ptr->attr_value.uint_list.size;
    /* Data element header (1) + actual data (size) */

    srv_attr_ptr->attr_value.uint_list.val[count] = val;
    srv_attr_ptr->attr_value.uint_list.num_val++;
  }
  else
  {
    BT_ERR( "BT SD: Bad Uint List Add T %x C %x",
            srv_attr_ptr->attr_type, count, 0 );
  }
}


void bt_sd_service_attribute_set_str(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  const char*               str
)
{
  uint16 len;

  if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_STRING )
  {
    len = strlen( str );
    if ( len > BT_SD_MAX_TEXT_STRING_LEN )
    {
      len = BT_SD_MAX_TEXT_STRING_LEN;
    }
    memcpy( srv_attr_ptr->attr_value.str, str, len );
    srv_attr_ptr->header.attr_value_len = len;
  }
  else
  {
    BT_ERR( "BT SD: Bad Serv Set Str T %x",
            srv_attr_ptr->attr_type, 0, 0 );
  }

}


void bt_sd_service_attribute_set_value(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint64                    value
)
{
  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT8 ) ||
       (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT16) ||
       (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT32) ||
       (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT64) )
  {
    srv_attr_ptr->attr_value.primitive_value = value;
  }
  else
  {
    BT_ERR( "BT SD: Bad Attr Set Val T %x",
            srv_attr_ptr->attr_type, 0, 0 );
  }
}

void bt_sd_service_attribute_set_uint128(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  const byte*               uint128_ptr
)
{
  if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UINT128 )
  { 
    memcpy( (void*) srv_attr_ptr->attr_value.uint128_value,
            (void*) uint128_ptr,
            16 );
  }
  else
  {
    BT_ERR( "BT SD: Bad Attr Set Uint128 T %x",
            srv_attr_ptr->attr_type, 0, 0 );
  }
}

void bt_sd_service_attribute_set_bool(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  boolean                   bool_flag
)
{
  if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_BOOL )
  {
    srv_attr_ptr->attr_value.bool_flag = bool_flag;
  }
  else
  {
    BT_ERR( "BT SD: Bad Attr Set Bool T %x",
            srv_attr_ptr->attr_type, 0, 0 );
  }
}

void bt_sd_service_attribute_add_hid_class_desc(
  bt_sd_srv_attr_rec_type*  srv_attr_ptr,
  uint8                     val,
  uint8                     len,
  const byte*               str
)
{
  bt_sd_hid_class_desc_type*  hid_class_desc_ptr;
  uint16 count = srv_attr_ptr->attr_value.hid_desc_list.num_hid_class_desc;

  if ( (srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_HID_DESC_LIST) &&
       (count < BT_SD_MAX_NUM_OF_HID_CLASS_DESC) )
  {
    hid_class_desc_ptr =
      &( srv_attr_ptr->attr_value.hid_desc_list.hid_class_desc[count] );

    if ( len > BT_SD_MAX_HID_CLASS_DESC_STRING_LEN )
    {
      len = BT_SD_MAX_HID_CLASS_DESC_STRING_LEN;
    }

    srv_attr_ptr->header.attr_value_len += 6 + len;
      /* Data element sequence header, i.e. 0x35 0x?? (2) + 
           uint8 data element header, i.e. 0x8 + uint8 (2) +
           string data element header, i.e. 0xA 0x?? + string (2+variable) */

    hid_class_desc_ptr->header.type = BT_SD_DATA_ELEMENT_SEQUENCE;
    hid_class_desc_ptr->header.size_index = 5;
    hid_class_desc_ptr->header.attr_value_len = 4 + len;

    hid_class_desc_ptr->val = val;
    hid_class_desc_ptr->len = len;
    memcpy( hid_class_desc_ptr->str, str, len );

    srv_attr_ptr->attr_value.hid_desc_list.num_hid_class_desc++;
  }
  else
  {
    BT_ERR( "BT SD: Bad HID Class Descriptor T %x C %x ",
            srv_attr_ptr->attr_type, count, 0 );
  }
}

#ifdef CUST_EDITION
boolean bt_sd_service_attribute_set_custom_attr_value( bt_sd_srv_attr_rec_type *srv_attr_ptr, uint8	*pData )
{
	boolean bSuccess = TRUE;
	
	if ( ( srv_attr_ptr == NULL ) || ( pData == NULL ) )
		 return FALSE;
	
	if ( srv_attr_ptr->attr_type == BT_SD_ATTR_TYPE_UNKNOWN )
	{
		 uint8 size_index = 0;

	     const uint8 DataElementSizeDesc[] = { 1, 2, 4, 8, 16, 1, 2, 4 };

		 bt_sd_data_element_enum_type DataFieldType = BT_SD_DATA_ELEMENT_NULL;

		 uint32 DataFieldSize = 0;

		 uint16 HeaderFieldSize = 0;

	     // The Upper 5 Bits are Type Descriptor
	     DataFieldType = ( bt_sd_data_element_enum_type )( ( ( *pData ) >> 3 ) & 0x1F );

	     // The Lower 3 Bits are Size Index
	     size_index = ( *pData ) & 0x07;

		 switch ( DataFieldType )
		 {
		 case BT_SD_DATA_ELEMENT_NULL:
			  {
				  if ( size_index == 0 )
				  {
				 	   DataFieldSize = 0;
				 	   HeaderFieldSize = 1;
				  }
				  else
				  {
					   bSuccess = FALSE;
				  }
			  }
			  break;
			 
		 case BT_SD_DATA_ELEMENT_BOOL:
			  {
				  if ( size_index == 0 )
				  {
				 	   DataFieldSize = 1;
				 	   HeaderFieldSize = 1;
				  }
				  else
				  {
					   bSuccess = FALSE;
				  }
			  }
			  break;

		 case BT_SD_DATA_ELEMENT_UNSIGNED_INT:
		 case BT_SD_DATA_ELEMENT_SIGNED_INT:
			  {
				  if ( size_index <= 4 )
				  {
				 	   DataFieldSize = DataElementSizeDesc[ size_index ];
				 	   HeaderFieldSize = 1;
				  }
				  else
				  {
					   bSuccess = FALSE;
				  }
			  }
			  break;
			 
		 case BT_SD_DATA_ELEMENT_UUID:
			  {
				  if ( ( size_index == 1 ) || ( size_index == 2 ) || ( size_index == 4 ) )
				  {
				 	   DataFieldSize = DataElementSizeDesc[ size_index ];
				 	   HeaderFieldSize = 1;
				  }
				  else
				  {
					   bSuccess = FALSE;
				  }
			  }
			  break;

		 case BT_SD_DATA_ELEMENT_TEXT_STRING:
		 case BT_SD_DATA_ELEMENT_SEQUENCE:
		 case BT_SD_DATA_ELEMENT_ALTERNATIVE:
		 case BT_SD_DATA_ELEMENT_URL:
			  {
				  if ( size_index >= 5 )
				  {
				 	   HeaderFieldSize = 2;

					   switch ( DataElementSizeDesc[ size_index ] )
					   {
					   case 1:
						    {
							    DataFieldSize = *( pData + 1 );
						    }
						    break;

					   case 2:
						    {
							    uint16 value = 0;

							    memcpy( ( void * )&value, ( void * )( pData + 1 ), sizeof( uint16 ) );
							   
							    DataFieldSize = SWAB16( value );
						    }
						    break;
						  
					   case 4:
					  	    {
							    uint32 value = 0;

							    memcpy( ( void * )&value, ( void * )( pData + 1 ), sizeof( uint32 ) );
							   
							    DataFieldSize = SWAB32( value );
						    }
						    break;

					   default:
					   	    {
								bSuccess = FALSE;
					   	    }
						    break;
					   }
				  }
				  else
				  {
					   bSuccess = FALSE;
				  }
			  }
			  break;
			  
		 default:
		 	  {
				  bSuccess = FALSE;
		 	  }
		      break;
		 }

		 BT_MSG_DEBUG( "BT SD: Set Custom Attr Value, Type = %d, HeaderSize = %d, DataSize = %d",
		 			DataFieldType, HeaderFieldSize, DataFieldSize );

		 if ( bSuccess == TRUE )
		 {
			  if ( HeaderFieldSize + DataFieldSize > BT_SD_MAX_CUSTOM_ATTR_VALUE_LENGTH )
			  	   return FALSE;
			  
			  srv_attr_ptr->header.type = DataFieldType;
			  srv_attr_ptr->header.size_index = size_index;
			  srv_attr_ptr->header.hdr_len = ( uint8 )HeaderFieldSize;
			  srv_attr_ptr->header.attr_value_len = DataFieldSize;

			  memset( ( void * )&srv_attr_ptr->attr_value.custom_attr_value, 0, sizeof( bt_sd_custom_attribute_value ) );

			  memcpy( ( void * )( srv_attr_ptr->attr_value.custom_attr_value.Data ), ( void * )pData, ( HeaderFieldSize + DataFieldSize ) * sizeof( uint8 ) );

			  return TRUE;
		 }
		 else
		 	  return FALSE;
	}
	else
	{
		 BT_ERR( "BT SD: Bad Attr Set Custom Attr Value T %x", srv_attr_ptr->attr_type, 0, 0 );
		 return FALSE;
	}
}
#endif
/*===========================================================================

FUNCTION
  init_srv_rec_iter
  get_first_srv_rec
  get_next_srv_rec
  find_srv_rec_next_match

DESCRIPTION
  Internal functions to manipulate the service record iterator

===========================================================================*/

void init_srv_rec_iter(
  bt_sd_db_enum_type     db_type,
  bt_sd_iter_enum_type   iter_type
)
{
  srv_rec_iter.cur_srv_rec_index = 0;
  srv_rec_iter.db_type = db_type;
  srv_rec_iter.iter_type = iter_type;
}


const bt_sd_srv_rec_type* get_first_srv_rec( void )
{
  bt_sd_srv_db_type*   db_ptr = NULL;
  bt_sd_srv_rec_type*  sr_ptr = NULL;

  switch( srv_rec_iter.db_type )
  {
    case BT_SD_DB_TYPE_LOCAL_DB:
      db_ptr = &bt_sd_local_srv_db;
      break;
    case BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB:
      db_ptr = &bt_sd_srv_srch_result_db;
      break;
    default:
      BT_ERR( "BT SD: Bad DB Type %x",
              srv_rec_iter.db_type, 0, 0 );
      break;
  }

  if ( db_ptr != NULL )
  {
    srv_rec_iter.cur_srv_rec_index = 0;

    if ( find_srv_rec_next_match( db_ptr ) != FALSE )
    {
      sr_ptr = &( db_ptr->srv_rec[ srv_rec_iter.cur_srv_rec_index ] );
    }
  }
  
  return ( sr_ptr );

}


const bt_sd_srv_rec_type* get_next_srv_rec( void )
{
  bt_sd_srv_db_type*   db_ptr = NULL;
  bt_sd_srv_rec_type*  sr_ptr = NULL;

  switch( srv_rec_iter.db_type )
  {
    case BT_SD_DB_TYPE_LOCAL_DB:
      db_ptr = &bt_sd_local_srv_db;
      break;
    case BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB:
      db_ptr = &bt_sd_srv_srch_result_db;
      break;
    default:
      BT_ERR( "BT SD: Bad DB Type %x",
              srv_rec_iter.db_type, 0, 0 );
      break;
  }
  
  if ( (db_ptr != NULL) &&
       (++srv_rec_iter.cur_srv_rec_index < BT_SD_MAX_NUM_OF_SRV_REC) )
  {
    if ( find_srv_rec_next_match( db_ptr ) != FALSE )
    {
      sr_ptr = &( db_ptr->srv_rec[ srv_rec_iter.cur_srv_rec_index ] );
    }
  }
  else
  {
    BT_ERR( "BT SD: Bad Next Serv Rec DBP %x RI %x",
            db_ptr, srv_rec_iter.cur_srv_rec_index, 0 );
  }

  return ( sr_ptr );

}


boolean find_srv_rec_next_match(
  bt_sd_srv_db_type*   db_ptr
)
{
  int   i;

  for ( i = srv_rec_iter.cur_srv_rec_index; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
  {
    if ( db_ptr->srv_rec[i].used_flag )
    {
      if ( srv_rec_iter.iter_type == BT_SD_ITER_MATCH_ONLY )
      {
        if ( db_ptr->srv_rec[i].matched_flag )
        {
          srv_rec_iter.cur_srv_rec_index = i;
          return TRUE;
        }
      }
      else
      {
        srv_rec_iter.cur_srv_rec_index = i;
        return TRUE;
      }
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION
  init_srv_attr_iter
  get_first_srv_attr
  get_next_srv_attr
  find_srv_attr_next_match

DESCRIPTION
  Internal functions to manipulate the service attribute iterator

===========================================================================*/

void init_srv_attr_iter(
  const bt_sd_srv_rec_type*   srv_rec_ptr,
  bt_sd_iter_enum_type        iter_type
)
{
  if ( srv_rec_ptr != NULL )
  {
    srv_attr_iter.cur_srv_rec_ptr = ( bt_sd_srv_rec_type* ) srv_rec_ptr;
    srv_attr_iter.cur_attr_index = 0;
    srv_attr_iter.iter_type = iter_type;
  }
  else
  {
    BT_ERR( "BT SD: Bad DB Type %x",
            srv_rec_iter.db_type, 0, 0 );
  }
}


const bt_sd_srv_attr_rec_type* get_first_srv_attr( void )
{
  if ( srv_attr_iter.cur_srv_rec_ptr->num_srv_attr == 0 )
  {
    return 0;
  }

  srv_attr_iter.cur_attr_index = 0;

  if ( srv_attr_iter.iter_type == BT_SD_ITER_MATCH_ONLY )
  {
    if ( ! find_srv_attr_next_match() )
    {
      return 0;
    }
  }

  return &(
    srv_attr_iter.cur_srv_rec_ptr->srv_attr[ srv_attr_iter.cur_attr_index ] );
}


const bt_sd_srv_attr_rec_type* get_next_srv_attr( void )
{
  if ( srv_attr_iter.cur_srv_rec_ptr->num_srv_attr == 0 )
  {
    return 0;
  }

  srv_attr_iter.cur_attr_index++;

  if ( srv_attr_iter.cur_attr_index >=
         srv_attr_iter.cur_srv_rec_ptr->num_srv_attr )
  {
    return 0;
  }

  if ( srv_attr_iter.iter_type == BT_SD_ITER_MATCH_ONLY )
  {
    if ( ! find_srv_attr_next_match() )
    {
      return 0;
    }
  }

  return &(
    srv_attr_iter.cur_srv_rec_ptr->srv_attr[ srv_attr_iter.cur_attr_index ] );
}


boolean find_srv_attr_next_match( void )
{
  int i;

  for ( i = srv_attr_iter.cur_attr_index;
        i < srv_attr_iter.cur_srv_rec_ptr->num_srv_attr;
        i++ )
  {
    if ( srv_attr_iter.cur_srv_rec_ptr->srv_attr[i].matched_flag )
    {
      srv_attr_iter.cur_attr_index = i;
      return TRUE;
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION
  bt_sd_match_uuid
  bt_sd_match_uuid128
  bt_sd_match_attr_id

DESCRIPTION
  Internal functions to match a given UUID/attribute ID with a SDP service
  search pattern

===========================================================================*/

boolean bt_sd_match_uuid(
  bt_sd_uuid_type            uuid,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint16                  i;
  boolean                 found_match = FALSE;
  bt_sd_proto_desc_type*  pd_ptr;

  switch( srv_attr_ptr->attr_type )
  {
    case BT_SD_ATTR_TYPE_UUID_LIST:
      for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid; i++ )
      {
        if ( uuid == srv_attr_ptr->attr_value.uuid_list.uuid[i] )
        {
          found_match = TRUE;
          break;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      for ( i = 0;
            i < srv_attr_ptr->attr_value.proto_desc_list.num_proto_desc;
            i++ )
      {
        pd_ptr = &( srv_attr_ptr->attr_value.proto_desc_list.proto_desc[i] );
        if ( ( pd_ptr->is_uuid128 == FALSE ) && ( uuid == pd_ptr->uuid ) )
        {
          found_match = TRUE;
          break;
        }
      }
      break;
    default:
      /* ignore */
      break;
  } /* switch */

  return found_match;
}


boolean bt_sd_match_uuid128(
  const bt_sd_uuid128_type*  uuid128_ptr,
  bt_sd_srv_attr_rec_type*   srv_attr_ptr
)
{
  uint16                  i;
  boolean                 found_match = FALSE;
  bt_sd_proto_desc_type*  pd_ptr;

  switch( srv_attr_ptr->attr_type )
  {
    case BT_SD_ATTR_TYPE_UUID_LIST:
      for ( i = 0; i < srv_attr_ptr->attr_value.uuid_list.num_uuid128; i++ )
      {
        if ( BT_SD_UUID128_EQUAL(
               uuid128_ptr, 
               &srv_attr_ptr->attr_value.uuid_list.uuid128[i] ) )
        {
          found_match = TRUE;
          break;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_PROTO_DESC_LIST:
      for ( i = 0;
            i < srv_attr_ptr->attr_value.proto_desc_list.num_proto_desc;
            i++ )
      {
        pd_ptr = &srv_attr_ptr->attr_value.proto_desc_list.proto_desc[i];

        if ( pd_ptr->is_uuid128 &&
             BT_SD_UUID128_EQUAL( uuid128_ptr, &pd_ptr->uuid128 ) )
        {
          found_match = TRUE;
          break;
        }
      }
      break;
    case BT_SD_ATTR_TYPE_UUID128:
      if ( BT_SD_UUID128_EQUAL(
        uuid128_ptr,
        &srv_attr_ptr->attr_value.uuid128 ) )
      {
        found_match = TRUE;
      }
      break;
    default:
      /* ignore */
      break;
  } /* switch */

  return found_match;
}


boolean bt_sd_match_attr_id(
  uint16                          attr_id,
  const bt_sd_srv_srch_pat_type*  srch_pat_ptr
)
{
  int       i;
  uint16    high, low;
    /* Upper and lower limit of an attribute ID range */

  for ( i = 0; i < srch_pat_ptr->num_attr_id; i++ )
  {
    if ( srch_pat_ptr->attr_id[i].is_range )
    {
      low = ( srch_pat_ptr->attr_id[i].value & 0xFFFF0000 ) >> 16;
      high = srch_pat_ptr->attr_id[i].value & 0x0000FFFF;
      if ( ( attr_id >= low ) && ( attr_id <= high ) )
      {
        return TRUE;
      }
      else if ( attr_id < low )
      {
        return FALSE;
      }
    }
    else
    {
      if ( attr_id == srch_pat_ptr->attr_id[i].value )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_sd_initialize_database

DESCRIPTION
  Initialize the SDP database

===========================================================================*/

void bt_sd_initialize_database(
  bt_sd_db_enum_type         db_type
)
{
  uint16               i;
  bt_sd_srv_db_type*   db_ptr = NULL;

  switch( db_type )
  {
    case BT_SD_DB_TYPE_LOCAL_DB:
      db_ptr = &bt_sd_local_srv_db;
      break;
    case BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB:
      db_ptr = &bt_sd_srv_srch_result_db;
      break;
    default:
      BT_ERR( "BT SD: Bad DB Type %x", db_type, 0, 0 );
      break;
  }

  if ( db_ptr != NULL )
  {
    /* Zero out the database */
    memset( ( byte* ) db_ptr, 0, sizeof( bt_sd_srv_db_type ) );

    db_ptr->num_used_srv_rec = 0;
    db_ptr->num_enabled_srv_rec = 0;
    db_ptr->num_connectable_srv_rec = 0;
    db_ptr->num_matched_srv_rec = 0;

    for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
    {
      db_ptr->srv_rec[i].srv_rec_id =
        ( ( (uint32) i ) & 0x0000FFFF ) | 0x00010000;
      db_ptr->srv_rec[i].used_flag = FALSE;
      db_ptr->srv_rec[i].enabled_flag = FALSE;
      db_ptr->srv_rec[i].connectable = FALSE;
      db_ptr->srv_rec[i].matched_flag = FALSE;
      db_ptr->srv_rec[i].num_srv_attr = 0;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_sd_service_record_add_common_attributes
  bt_sd_create_service_record
  bt_sd_create_custom_service_record

DESCRIPTION
  Create a service record

===========================================================================*/
void bt_sd_service_record_add_common_attributes
(
  bt_sd_srv_rec_type*    sr_ptr,
  bt_sd_uuid_list_type*  uuid_list_ptr,
  uint8                  rfc_scn,
  uint16                 l2_psm,
  uint16                 pvn,
  const char*            name
)
{
  
  bt_sd_srv_attr_rec_type*  sa_ptr = NULL;
  bt_sd_proto_desc_type*    pd_ptr = NULL;
  boolean                   pvn_added = FALSE;
  uint16                    i;

  /* Attribute ID = 0x0000 (ServiceRecordHandle)
     Attribute Value = { uint32 }
   */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE,
                   BT_SD_ATTR_TYPE_UINT32 )) != NULL )
  {
    bt_sd_service_attribute_set_value( sa_ptr, sr_ptr->srv_rec_id );
  }
    
  /* Attribute ID = 0x0001 (ServiceClassIDList)
     Attribute Value = { <UUID1>, <UUID2>, ... }
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST,
                   BT_SD_ATTR_TYPE_UUID_LIST )) != NULL )
  {
    for ( i = 0; 
          i < uuid_list_ptr->num_uuid128 && i < BT_SD_MAX_NUM_OF_UUID128;
          i++ )
    {
      bt_sd_service_attribute_set_uuid128( sa_ptr, &uuid_list_ptr->uuid128[i] );
    }
    for ( i = 0;
          i < uuid_list_ptr->num_uuid && i < BT_SD_MAX_NUM_OF_UUID;
          i++ )
    {
      bt_sd_service_attribute_add_uuid( sa_ptr, uuid_list_ptr->uuid[i] );
    }
#ifdef WOLF_5
#error code not present
#endif
  }

#ifdef BT_TEST_SDP
  /* Attribute ID = 0x0003 (ServiceID)
     Attribute Value = <UUID>
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_SERVICE_SERVICE_ID,
                   BT_SD_ATTR_TYPE_UUID )) != NULL )
  {
    bt_sd_service_attribute_add_uuid( sa_ptr, uuid );
  }
#endif /* BT_TEST_SDP */

  /* Attribute ID = 0x0004 (ProtocolDescriptorList)
     Attribute Value = { { 0x0100 (L2CAP UUID) },
                         { 0x0003 (RFCOMM UUID), <Server Channel #> } }
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST,
                   BT_SD_ATTR_TYPE_PROTO_DESC_LIST )) != NULL )
  {
    pd_ptr = bt_sd_service_attribute_add_proto_desc(
               sa_ptr, BT_SD_PROTOCOL_L2CAP, NULL );

    if ( ( pd_ptr != NULL ) && ( l2_psm != BT_L2_PSM_INVALID ) )
    {
      bt_sd_service_attribute_proto_desc_add_param(
        pd_ptr, l2_psm, 2 );
    }

    for ( i = 0;
          i < uuid_list_ptr->num_uuid && i < BT_SD_MAX_NUM_OF_UUID;
          i++ )
    {
#ifdef FEATURE_BT_EXTPF_CTP
      if ( ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY ) ||
           ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_INTERCOM ) )
      {
        (void*) bt_sd_service_attribute_add_proto_desc(
                  sa_ptr, BT_SD_PROTOCOL_TCS_BIN, NULL );
      }
#endif /* FEATURE_BT_EXTPF_CTP */

#ifdef FEATURE_BT_EXTPF_AV
      if ( ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_AUDIO_SOURCE ) ||
           ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_AUDIO_SINK ) )
      {
        pd_ptr = bt_sd_service_attribute_add_proto_desc(
                   sa_ptr, BT_SD_PROTOCOL_AVDTP, NULL );
        if ( pd_ptr != NULL )
        {
          bt_sd_service_attribute_proto_desc_add_param(
            pd_ptr, (uint16)pvn, 2 );
        }
      }

      if ( ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET ) ||
           ( uuid_list_ptr->uuid[i] == BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL ) )
      {
        pd_ptr = bt_sd_service_attribute_add_proto_desc(
                   sa_ptr, BT_SD_PROTOCOL_AVCTP, NULL );
        if ( pd_ptr != NULL )
        {
          bt_sd_service_attribute_proto_desc_add_param(
            pd_ptr, (uint16)pvn, 2 );
        }
      }
#endif /* FEATURE_BT_EXTPF_AV */
    } /* for each 16-bit UUID */

    if ( rfc_scn != BT_RC_SCN_NOT_SPECIFIED )
    {
      pd_ptr = bt_sd_service_attribute_add_proto_desc(
                 sa_ptr, BT_SD_PROTOCOL_RFCOMM, NULL );

      if ( pd_ptr != NULL )
      {
        bt_sd_service_attribute_proto_desc_add_param(
          pd_ptr, (uint16)rfc_scn, 1 );
      }
    }

  }

  /* Attribute ID = 0x0005 (BrowseGroupList)
     Attribute Value = { 0x1002 (PublicBrowseRoot UUID) }
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_BROWSE_GROUP_LIST,
                   BT_SD_ATTR_TYPE_UUID_LIST )) != NULL )
  {
    bt_sd_service_attribute_add_uuid(
      sa_ptr, BT_SD_SERVICE_CLASS_PUBLIC_BROWSE_GROUP );
  }

  /* Attribute ID = 0x0006 (LanguageBaseAttributeIDList)
     Attribute Value = { { 0x656E, 0x006A, 0x0100 } }
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST,
                   BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST )) != NULL )
  {
    bt_sd_service_attribute_add_lang_base_attr_id(
      sa_ptr, 0x656E, 0x0006A, 0x0100 );
  }

  /* Attribute ID = 0x0009 (BluetoothProfileDescriptorList)
     Attribute Value = { { 0x1108 (Headset UUID), <Profile Version #> } }
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST,
                   BT_SD_ATTR_TYPE_PROTO_DESC_LIST )) != NULL )
  {
    for ( i = 0; 
          i < uuid_list_ptr->num_uuid128 && i < BT_SD_MAX_NUM_OF_UUID128;
          i++ )
    {
      pd_ptr = bt_sd_service_attribute_add_proto_desc(
                 sa_ptr, 0, &uuid_list_ptr->uuid128[i] );
      
      /* Add the profile version number for the most specific UUID */
      if ( ( pd_ptr != NULL ) && ( !pvn_added ) )
      {
        bt_sd_service_attribute_proto_desc_add_param(
          pd_ptr, pvn, 2 );
        pvn_added = TRUE;
      }
    }
    
    for ( i = 0;
          i < uuid_list_ptr->num_uuid && i < BT_SD_MAX_NUM_OF_UUID;
          i++ )
    {
      switch ( uuid_list_ptr->uuid[i] )
      {
        case BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_HEADSET, NULL );
          break;

        case BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_HANDSFREE, NULL );
          break;

        case BT_SD_SERVICE_CLASS_IMAGING_RESPONDER:
        case BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE:
        case BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_IMAGING, NULL );
          break;

        case BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_BASIC_PRINTING, NULL );
          break;

        case BT_SD_SERVICE_CLASS_AUDIO_SOURCE:
        case BT_SD_SERVICE_CLASS_AUDIO_SINK:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr,
                     BT_SD_SERVICE_CLASS_ADVANCED_AUDIO_DISTRIBUTION, NULL );
          break;

        case BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET:
        case BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL, NULL );
          break;

        case BT_SD_SERVICE_CLASS_PBAP_PCE:
        case BT_SD_SERVICE_CLASS_PBAP_PSE:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, BT_SD_SERVICE_CLASS_PBAP, NULL );
          break;

        default:
          pd_ptr = bt_sd_service_attribute_add_proto_desc(
                     sa_ptr, uuid_list_ptr->uuid[i], NULL );
      }

      /* Add the profile version number for the most specific UUID */
      if ( ( pd_ptr != NULL ) && ( !pvn_added ) )
      {
        bt_sd_service_attribute_proto_desc_add_param(
          pd_ptr, pvn, 2 );
        pvn_added = TRUE;
      }
    }
  }

  /* Attribute ID = 0x0100 (Service Name)
     Attribute Value = "<Service Name>"
  */
  if ( (sa_ptr = bt_sd_service_record_add_attribute(
                   sr_ptr, BT_SD_ATTR_ID_SERVICE_NAME,
                   BT_SD_ATTR_TYPE_STRING )) != NULL )
  {
    bt_sd_service_attribute_set_str( sa_ptr, name );
  }

  /* Add the service class specific attributes */
  if ( uuid_list_ptr->num_uuid128 == 0 ) /* For non-custom service record */
  {
    switch( uuid_list_ptr->uuid[0] )
    {
#ifdef FEATURE_BT_AG
      case BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY:
        if ( (sa_ptr =
                bt_sd_find_service_attribute(
                  sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_uuid(
            sa_ptr, BT_SD_SERVICE_CLASS_GENERIC_AUDIO );
        }
        break;

      case BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY:
        if ( (sa_ptr =
                bt_sd_find_service_attribute(
                  sr_ptr,BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_uuid(
            sa_ptr, BT_SD_SERVICE_CLASS_GENERIC_AUDIO );
        }

        /* Attribute ID = 0x0301 (Network)
           Attribute Value = 0 (means "no ability to reject a call")
        */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
                         sr_ptr, BT_SD_ATTR_ID_NETWORK,
                         BT_SD_ATTR_TYPE_UINT8 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x01 );
        }

        /* Attribute ID = 0x0311 (SupportedFeatures)
           Attribute Value = bitmap of supported features
           Note: this must be the same as the features_bitmap field in
           bt_ag_data_type (btag.c)
        */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
                         sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FEATURES,
                         BT_SD_ATTR_TYPE_UINT16 )) != NULL )
        {
          bt_sd_service_attribute_set_value(
            sa_ptr, bt_ag_current_supported_features() );
        }
        break;
#endif /* FEATURE_BT_AG */

      case BT_SD_SERVICE_CLASS_AUDIO_SOURCE:
        /* Attribute ID = 0x0311 (SupportedFeatures)
           Attribute Value = bitmap of supported features
           Note: this is currently hard coded to supporting only Player
           (bit0)
        */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
                         sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FEATURES,
                         BT_SD_ATTR_TYPE_UINT16 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x01 );
        }
        break;

#ifdef FEATURE_BT_EXTPF_AV
      case BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL:
      case BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET:
        /* Attribute ID = 0x0311 (SupportedFeatures)
           Attribute Value = bitmap of supported features
           Note: this is set by app when registering with AVRCP
        */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
                         sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FEATURES,
                         BT_SD_ATTR_TYPE_UINT16 )) != NULL )
        {
          bt_sd_service_attribute_set_value( 
            sa_ptr, bt_pf_avrcp_current_supported_features() );
        }
        break;
#endif /* FEATURE_BT_EXTPF_AV */

      case BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY:
      case BT_SD_SERVICE_CLASS_INTERCOM:
      case BT_SD_SERVICE_CLASS_SIM_ACCESS:
        if ( (sa_ptr = bt_sd_find_service_attribute(
          sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_uuid(
            sa_ptr, BT_SD_SERVICE_CLASS_GENERIC_TELEPHONY );
        }
        break;

#ifdef BT_TEST_SDP
      case BT_SD_SERVICE_CLASS_DIALUP_NETWORKING:
        if ( (sa_ptr =
              bt_sd_find_service_attribute(
                sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_uuid(
            sa_ptr, BT_SD_SERVICE_CLASS_GENERIC_NETWORKING );
        }
        break;
      case BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH:
        if ( (sa_ptr =
              bt_sd_find_service_attribute(
                sr_ptr, BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_proto_desc(
            sa_ptr, BT_SD_PROTOCOL_OBEX, NULL );
        }

        /* Add the Supported Formats List service attribute */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
          sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST,
          BT_SD_ATTR_TYPE_UINT_LIST )) != NULL )
        {
          bt_sd_service_attribute_uint_list_add_val(
            sa_ptr,
  /*        sizeof( bt_opp_supported_format_type ), */
            sizeof( uint32 ),
            BT_SD_OPP_SUPPORTED_FORMAT_VCARD_2_1 );
          bt_sd_service_attribute_uint_list_add_val(
            sa_ptr,
  /*        sizeof( bt_opp_supported_format_type ), */
            sizeof( uint32 ),
            BT_SD_OPP_SUPPORTED_FORMAT_VCARD_3_0 );
        }
        break;
      case BT_SD_SERVICE_CLASS_FAX:
        if ( (sa_ptr =
              bt_sd_find_service_attribute(
                sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST )) != NULL )
        {
          bt_sd_service_attribute_add_uuid(
            sa_ptr, BT_SD_SERVICE_CLASS_GENERIC_TELEPHONY );
        }
        break;
      case BT_SD_SERVICE_CLASS_IMAGING_RESPONDER:
        /* Add the Supported Capabilities service attribute */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
          sr_ptr, BT_SD_ATTR_ID_SUPPORTED_CAPABILITIES,
          BT_SD_ATTR_TYPE_UINT8 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x07 );
        }

        /* Add the Supported Features service attribute */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
          sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FEATURES,
          BT_SD_ATTR_TYPE_UINT16 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x01FF );
        }

        /* Add the Supported Functions service attribute */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
          sr_ptr, BT_SD_ATTR_ID_SUPPORTED_FUNCTIONS,
          BT_SD_ATTR_TYPE_UINT32 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x00016FFF );
        }

        /* Add the Total Imaging Data Capacity service attribute */
        if ( (sa_ptr = bt_sd_service_record_add_attribute(
          sr_ptr, BT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY,
          BT_SD_ATTR_TYPE_UINT64 )) != NULL )
        {
          bt_sd_service_attribute_set_value( sa_ptr, 0x00016FFF00016FFFULL );
        }
        break;
#endif /* BT_TEST_SDP */

#ifdef FEATURE_BT_LPOS
#error code not present
#endif /* FEATURE_BT_LPOS */
      default:
        break;
    }
  }

}

bt_sd_srv_rec_type* bt_sd_create_service_record
(
  bt_sd_uuid_type  uuid,    /* Service Class UUID                 */
  uint8            rfc_scn, /* RFCOMM Server Channel Number       */
  uint16           l2_psm,  /* L2CAP Protocol/Service Multiplexer */
  uint16           pvn,     /* Profile Version Number             */
  const char*      name     /* Service Name                       */
)
{

  bt_sd_srv_rec_type*   sr_ptr = NULL;
  bt_sd_uuid_list_type  uuid_list;

  if ( (sr_ptr =
          bt_sd_alloc_service_record( BT_SD_DB_TYPE_LOCAL_DB )) != NULL )
  {
    sr_ptr->is_custom_srv = FALSE;
    sr_ptr->srv_uuid = uuid;

    uuid_list.num_uuid128 = 0; /* To indicate non-custom service record  */
    uuid_list.num_uuid = 1;
    uuid_list.uuid[ 0 ] = uuid;

    /* Add all the common attributes */
    bt_sd_service_record_add_common_attributes(
      sr_ptr, &uuid_list, rfc_scn, l2_psm, pvn, name );
  }

  return ( sr_ptr );
}

bt_sd_srv_rec_type* bt_sd_create_custom_service_record
(
  bt_sd_uuid_list_type*  uuid_list_ptr,  /* Service Class UUID List       */
  uint8                  rfc_scn,   /* RFCOMM Server Channel Number       */
  uint16                 l2_psm,    /* L2CAP Protocol/Service Multiplexer */
  uint16                 pvn,       /* Profile Version Number             */
  const char*            name       /* Service Name                       */
)
{

  bt_sd_srv_rec_type*  sr_ptr = NULL;

  if ( (sr_ptr =
          bt_sd_alloc_service_record( BT_SD_DB_TYPE_LOCAL_DB )) != NULL )
  {
    sr_ptr->is_custom_srv = TRUE;
    sr_ptr->srv_uuid128 = uuid_list_ptr->uuid128[0];

    /* Add all the common attributes */
    bt_sd_service_record_add_common_attributes(
      sr_ptr, uuid_list_ptr, rfc_scn, l2_psm, pvn, name );
  }

  return ( sr_ptr );
}

#ifdef FEATURE_BT_EXTPF_SML
/*===========================================================================

FUNCTION
  bt_sd_create_syncml_service_record

DESCRIPTION
  Create a SyncML client or server service record

===========================================================================*/
bt_sd_srv_rec_type* bt_sd_create_syncml_service_record
(
  boolean          client,  /* SyncML client or server      */
  uint8            rfc_scn, /* RFCOMM Server Channel Number */
  const char*      name     /* Service Name                 */
)
{
  bt_sd_uuid128_type        uuid;
  bt_sd_srv_rec_type*       sr_ptr = NULL;
  bt_sd_srv_attr_rec_type*  sa_ptr = NULL;
  bt_sd_proto_desc_type*    pd_ptr = NULL;

  uuid = client ? bt_sd_syncml_client_uuid : bt_sd_syncml_server_uuid;

  if ( (sr_ptr =
          bt_sd_alloc_service_record( BT_SD_DB_TYPE_LOCAL_DB )) != NULL )
  {
    sr_ptr->srv_uuid = 0;
    sr_ptr->syncml_flag = client ? 
                          BT_SD_DB_SYNCML_CLIENT_B : BT_SD_DB_SYNCML_SERVER_B;

    /* Attribute ID = 0x0000 (ServiceRecordHandle)
       Attribute Value = { uint32 }
    */
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE,
                     BT_SD_ATTR_TYPE_UINT32 )) != NULL )
    {
      bt_sd_service_attribute_set_value( sa_ptr, sr_ptr->srv_rec_id );
    }
    
    /* Attribute ID = 0x0001 (ServiceClassIDList)
       Attribute Value = { <UUID1>, <UUID2>, ... }
    */  
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST,
                     BT_SD_ATTR_TYPE_UUID_LIST )) != NULL )
    {
      bt_sd_service_attribute_set_uuid128( sa_ptr, &uuid );
    }                                      

    /* Attribute ID = 0x0004 (ProtocolDescriptorList)
       Attribute Value = { { 0x0100 (L2CAP UUID) },
                           { 0x0003 (RFCOMM UUID), <Server Channel #> } }
    */
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST,
                     BT_SD_ATTR_TYPE_PROTO_DESC_LIST )) != NULL )
    {
      bt_sd_service_attribute_add_proto_desc(
        sa_ptr, BT_SD_PROTOCOL_L2CAP, NULL );
 
      pd_ptr = bt_sd_service_attribute_add_proto_desc(
                 sa_ptr, BT_SD_PROTOCOL_RFCOMM, NULL );

      if ( pd_ptr != NULL )
      {
        bt_sd_service_attribute_proto_desc_add_param(
          pd_ptr, (uint16)rfc_scn, 1 );
      }

      bt_sd_service_attribute_add_proto_desc(
        sa_ptr, BT_SD_PROTOCOL_OBEX, NULL );
    }

    /* Attribute ID = 0x0005 (BrowseGroupList)
       Attribute Value = { 0x1002 (PublicBrowseRoot UUID) }
    */
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_BROWSE_GROUP_LIST,
                     BT_SD_ATTR_TYPE_UUID_LIST )) != NULL )
    {
      bt_sd_service_attribute_add_uuid(
        sa_ptr, BT_SD_SERVICE_CLASS_PUBLIC_BROWSE_GROUP );
    }

    /* Attribute ID = 0x0006 (LanguageBaseAttributeIDList)
       Attribute Value = { { 0x656E, 0x006A, 0x0100 } }
    */
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST,
                     BT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST )) != NULL )
    {
      bt_sd_service_attribute_add_lang_base_attr_id(
        sa_ptr, 0x656E, 0x0006A, 0x0100 );
    }

    /* Attribute ID = 0x0100 (Service Name)
       Attribute Value = "<Service Name>"
    */
    if ( (sa_ptr = bt_sd_service_record_add_attribute(
                     sr_ptr, BT_SD_ATTR_ID_SERVICE_NAME,
                     BT_SD_ATTR_TYPE_STRING )) != NULL )
    {
      bt_sd_service_attribute_set_str( sa_ptr, name );
    }
  }

  return ( sr_ptr );
}
#endif /* FEATURE_BT_EXTPF_SML */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_sd_eir_update_rm_uuid

DESCRIPTION
  Sends the 

===========================================================================*/
void bt_sd_eir_update_rm_uuid(
  bt_uuid_type   uuid_type
)
{
  uint8               i, j;
  bt_sd_uuid_type     uuid_16bit[ BT_MAX_EIR_UUID16 ];
  bt_sd_uuid128_type  uuid_128bit[ BT_MAX_EIR_UUID128 ];
  boolean             uuid_complete = TRUE;

  j = 0;

  /* Determine the size of the data */
  switch( uuid_type )
  {                  
    case BT_UUID_16BIT_TYPE:
      /* All 32 bit UUIDs are internally stored as 16-bit UUIDs */
      for( i = 0; 
           (i < bt_sd_local_srv_db.num_enabled_srv_rec && j < BT_SD_MAX_NUM_OF_SRV_REC);
           i++ )
                
      {
        if( bt_sd_local_srv_db.srv_rec[ i ].is_custom_srv != TRUE &&
            bt_sd_local_srv_db.srv_rec[ i ].enabled_flag == TRUE )
        {
          uuid_16bit[ j ] = bt_sd_local_srv_db.srv_rec[ j ].srv_uuid;
          j ++;
        }
        if( j >= BT_MAX_EIR_UUID16 )
        {
          uuid_complete = FALSE;
          break;
        }
      }

      bt_cmd_rm_wr_eir_uuid( bt_sd_app_id, uuid_type, uuid_complete,
                             j * sizeof(bt_sd_uuid_type) , (uint8*)uuid_16bit );
      break;

    case BT_UUID_128BIT_TYPE:
      for( i = 0; 
           (i < bt_sd_local_srv_db.num_enabled_srv_rec && j < BT_SD_MAX_NUM_OF_SRV_REC);
           i++ )
      {
        if( bt_sd_local_srv_db.srv_rec[ i ].is_custom_srv == TRUE &&
            bt_sd_local_srv_db.srv_rec[ i ].enabled_flag == TRUE )
        {
            if( i < BT_SD_MAX_NUM_OF_SRV_REC && j < BT_MAX_EIR_UUID128 )
	        {
          	     memcpy( (void *)&uuid_128bit[ j ], 
                  	   (void *)&bt_sd_local_srv_db.srv_rec[ i ].srv_uuid128,
                  	    sizeof( bt_sd_uuid128_type ) );
          	     j ++;
            }
        }
        if( j >= BT_MAX_EIR_UUID128 )
        {
          uuid_complete = FALSE;
          break;
        }
      }
      bt_cmd_rm_wr_eir_uuid( bt_sd_app_id, uuid_type, uuid_complete,
                             j * sizeof(bt_sd_uuid128_type), (uint8*)uuid_128bit );
      break;

    default:
      /* Error Message */
      BT_MSG_DEBUG( "BT SD: UNKN UUID Ty: %u", uuid_type, 0, 0);
      break;

  }
}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_sd_enable_service_record

DESCRIPTION
  Enable/disable a service record

===========================================================================*/

void bt_sd_enable_service_record(
  bt_sd_srv_rec_type*   srv_rec_ptr,
  boolean               enabled_flag
)
{
  if ( srv_rec_ptr->enabled_flag != enabled_flag )
  {
    srv_rec_ptr->enabled_flag = enabled_flag;
    if ( enabled_flag != FALSE )
    {
      bt_sd_local_srv_db.num_enabled_srv_rec++;
    }
    else
    {
      bt_sd_local_srv_db.num_enabled_srv_rec--;
    }
  }
}

/*===========================================================================

FUNCTION
  bt_sd_update_service_connectable

DESCRIPTION
  Updates the service record connectable field as specified.

===========================================================================*/

void bt_sd_update_service_connectable(
  bt_sd_srv_rec_type*   srv_rec_ptr,
  boolean               connectable
)
{
  if ( srv_rec_ptr->connectable != connectable )
  {
    srv_rec_ptr->connectable = connectable;
    if ( srv_rec_ptr->connectable != FALSE )
    {
      bt_sd_local_srv_db.num_connectable_srv_rec++;
    }
    else
    {
      bt_sd_local_srv_db.num_connectable_srv_rec--;
    }
  }
}

/*===========================================================================

FUNCTION
  bt_sd_match_service_record

DESCRIPTION
  Match service records with a given search pattern {UUID List,Attr ID List}

===========================================================================*/

uint16 bt_sd_match_service_record(
  const bt_sd_srv_srch_pat_type*   srch_pat_ptr
)
{
  uint16                     i;
  uint16                     matched_byte_count = 0;
  bt_sd_srv_rec_type*        srv_rec_ptr;
  bt_sd_srv_attr_rec_type*   srv_attr_ptr;
  boolean                    found_match;

  bt_sd_clear_service_record_match();

  bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_ALL );
  for ( srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        srv_rec_ptr != 0;
        srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  {
    /* Make sure the service record is enabled */
    if ( srv_rec_ptr->enabled_flag )
    {
      srv_rec_ptr->matched_flag = TRUE;

      for ( i = 0; i < srch_pat_ptr->num_uuid128; i++ )
      {
        found_match = FALSE;

        /*
         * Match 128-bit UUID
         * This takes care of the service ID match as well
         */
        bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_ALL );
        for ( srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
              srv_attr_ptr != 0;
              srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
        {
          if ( bt_sd_match_uuid128( &srch_pat_ptr->uuid128[i], srv_attr_ptr ) )
          {
            found_match = TRUE;
            break;
          }
        } /* foreach service attribute */

        if ( ! found_match )
        {
          srv_rec_ptr->matched_flag = FALSE;
          break;
        }
      } /* foreach 128-bit UUID in the search pattern */

      /* Match 16-bit UUID */
      for ( i = 0; i < srch_pat_ptr->num_uuid; i++ )
      {
        found_match = FALSE;

        bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_ALL );
        for ( srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
              srv_attr_ptr != 0;
              srv_attr_ptr =
                ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
        {
          if ( bt_sd_match_uuid( srch_pat_ptr->uuid[i], srv_attr_ptr ) )
          {
            found_match = TRUE;
            break;
          }
        } /* foreach service attribute */

        if ( ! found_match )
        {
          srv_rec_ptr->matched_flag = FALSE;
          break;
        }
      } /* foreach 16-bit UUID in the search pattern */

      /* if no match found, check if there is a SyncML match */
      if ( !srv_rec_ptr->matched_flag &&                 
           ( ( srch_pat_ptr->syncml_flag & srv_rec_ptr->syncml_flag ) != 0 ) )
      {
        srv_rec_ptr->matched_flag = TRUE;
      }

      /* Found a matching service record */
      if ( srv_rec_ptr->matched_flag )
      {
        if ( srch_pat_ptr->num_attr_id == 0 )
        {
          /* Add 4 bytes for the service record handle */
          matched_byte_count += 4;
        }
        else
        {
          srv_rec_ptr->matched_attr_byte_count =
            bt_sd_match_service_attribute( srv_rec_ptr, srch_pat_ptr );

          if ( srv_rec_ptr->matched_attr_byte_count != 0 )
          {
            matched_byte_count += 2 /* All matched attributes of a given
                                     * service record are treated
                                     * collectively as a data element
                                     * sequence, so add the length of a
                                     * data element header */
                               +  srv_rec_ptr->matched_attr_byte_count;
          }
        }

        if ( srv_rec_ptr->is_custom_srv )
        {
          BT_MSG_UUID128( MSG_HIGH, "BT SD: Found match",
                          &srv_rec_ptr->srv_uuid128 );
        }
        else
        {
          BT_MSG_HIGH( "BT SD: Found match UUID16 %x",
                       srv_rec_ptr->srv_uuid, 0, 0 );
        }
        BT_MSG_HIGH( "BT SD: Found match count %d",
                     matched_byte_count, 0, 0 );
        
        bt_sd_local_srv_db.num_matched_srv_rec++;

        if ( ( bt_sd_local_srv_db.num_matched_srv_rec ==
                 srch_pat_ptr->max_srv_rec_count ) ||
             ( bt_sd_local_srv_db.num_matched_srv_rec ==
                 bt_sd_local_srv_db.num_enabled_srv_rec ) )
        {
          break;
        }
      } /* if match */
    } /* if enable */
  } /* foreach service record */

  return matched_byte_count;
}


/*===========================================================================

FUNCTION
  bt_sd_match_service_attribute

DESCRIPTION
  Match the service attributes of a given service record

===========================================================================*/

uint32 bt_sd_match_service_attribute(
  bt_sd_srv_rec_type*             srv_rec_ptr,
  const bt_sd_srv_srch_pat_type*  srch_pat_ptr
)
{
  bt_sd_srv_attr_rec_type*   srv_attr_ptr;
  uint16                     tot_attr_byte_count = 0;

  bt_sd_clear_service_attribute_match( srv_rec_ptr );

  if ( srch_pat_ptr->num_attr_id > 0 )
  {
    bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_ALL );

    for ( srv_attr_ptr =
            ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
          srv_attr_ptr != 0;
          srv_attr_ptr =
            ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
    {
      if ( bt_sd_match_attr_id( srv_attr_ptr->attr_id, srch_pat_ptr ) )
      {
        srv_attr_ptr->matched_flag = TRUE;

        tot_attr_byte_count += 3 /* Number of bytes for the attribute ID */
                            +  srv_attr_ptr->header.hdr_len
                                 /* Since each attribute value is treated as
                                    a data element, add numbers of bytes for
                                    data element header */
                            +  srv_attr_ptr->header.attr_value_len;
                                 /* Attribute value */
      }
    }
  }

  return tot_attr_byte_count;
}


/*===========================================================================

FUNCTION
  bt_sd_clear_service_record_match

DESCRIPTION
  Clear all service records and attributes which are previously matched

===========================================================================*/

void bt_sd_clear_service_record_match( void )
{
  bt_sd_srv_rec_type*   srv_rec_ptr;

  bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_MATCH_ONLY );
  for ( srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        srv_rec_ptr != 0;
        srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                       /* Explicitly cast away the constness */
  {
    srv_rec_ptr->matched_flag = FALSE;
    srv_rec_ptr->matched_attr_byte_count = 0;
    bt_sd_clear_service_attribute_match( srv_rec_ptr );
  }

  bt_sd_local_srv_db.num_matched_srv_rec = 0;
}


/*===========================================================================

FUNCTION
  bt_sd_clear_service_attribute_match

DESCRIPTION
  Clear all service records and attributes which are previously matched

===========================================================================*/

void bt_sd_clear_service_attribute_match(
  bt_sd_srv_rec_type*   srv_rec_ptr
)
{
  bt_sd_srv_attr_rec_type*   srv_attr_ptr;

  bt_sd_srv_attr_iter.init( srv_rec_ptr, BT_SD_ITER_MATCH_ONLY );
  for ( srv_attr_ptr =
          ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.first();
        srv_attr_ptr != 0;
        srv_attr_ptr =
          ( bt_sd_srv_attr_rec_type* ) bt_sd_srv_attr_iter.next() )
  {
    srv_attr_ptr->matched_flag = FALSE;
  }
}


/*===========================================================================

FUNCTION
  bt_sd_find_service_record_by_handle

DESCRIPTION
  Return a service record given a service record handle

===========================================================================*/

bt_sd_srv_rec_type* bt_sd_find_service_record_by_handle(
  uint32   srv_rec_handle
)
{
  uint16 i;

  for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
  {
    if ( ( bt_sd_local_srv_db.srv_rec[ i ].used_flag ) &&
         ( bt_sd_local_srv_db.srv_rec[ i ].enabled_flag ) &&
         ( bt_sd_local_srv_db.srv_rec[ i ].srv_rec_id == srv_rec_handle ) )
    {
      return &( bt_sd_local_srv_db.srv_rec[ i ] );
    }
  }

  return 0;
}


/*===========================================================================

FUNCTION
  bt_sd_match_service_attribute_by_scn_or_psm
  bt_sd_find_service_record_by_uuid
  bt_sd_find_service_record_by_uuid128

DESCRIPTION
  Return a service record given a service class UUID and optionally a
  RFCOMM server channel number OR a L2CAP protocol/service multiplexer.

===========================================================================*/


boolean bt_sd_match_service_attribute_by_scn_or_psm(
  bt_sd_srv_rec_type*       sr_ptr,
  uint8                     scn,
  uint16                    psm
)
{
  bt_sd_srv_attr_rec_type*  sa_ptr;
  boolean                   match = FALSE;
  uint16                    i;

  if ( ( sa_ptr = bt_sd_find_service_attribute( 
                    sr_ptr, BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST ) ) != NULL )
  {
    for ( i = 0; i < sa_ptr->attr_value.proto_desc_list.num_proto_desc; i++ )
    {
      if ( scn != BT_RC_SCN_NOT_SPECIFIED )
      {
        if ( ( sa_ptr->attr_value.proto_desc_list.proto_desc[ i ].
               uuid == BT_SD_PROTOCOL_RFCOMM ) &&
             ( sa_ptr->attr_value.proto_desc_list.proto_desc[ i ].
               num_param > 0 ) ) 
        {
          if ( scn == (uint8)(sa_ptr->attr_value.proto_desc_list.
                              proto_desc[ i ].param[ 0 ].value) )
          {
            match = TRUE;
            break;
          }   
        }
      }
      else if ( psm != BT_L2_PSM_INVALID )
      {
        if ( ( sa_ptr->attr_value.proto_desc_list.proto_desc[ i ].
               uuid == BT_SD_PROTOCOL_L2CAP ) &&
             ( sa_ptr->attr_value.proto_desc_list.proto_desc[ i ].
               num_param > 0 ) )
        {
          if ( psm == (uint16)(sa_ptr->attr_value.proto_desc_list.
                               proto_desc[ i ].param[ 0 ].value) )
          {
            match = TRUE;
            break;
          }
        }
      }
    } /* end for each proto descriptor */
  }
  else /* no protocol descriptor list in service record */
  {
    match = TRUE; /* ignore SCN/PSM */
  }

  return match;
}

bt_sd_srv_rec_type* bt_sd_find_service_record_by_uuid(
  bt_sd_db_enum_type         db_type,
  bt_sd_uuid_type            uuid,
  uint8                      scn,
  uint16                     psm
)
{
  bt_sd_srv_rec_type*       sr_ptr;
  boolean                   match = FALSE;

  bt_sd_srv_rec_iter.init( db_type, BT_SD_ITER_ALL );
  for ( sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        sr_ptr != 0;
        sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  { 
    if ( sr_ptr->srv_uuid == uuid )
    {
      if ( ( scn == BT_RC_SCN_NOT_SPECIFIED ) && ( psm == BT_L2_PSM_INVALID ) )
      {                                   
        match = TRUE;
      }
      else
      {
        match =  bt_sd_match_service_attribute_by_scn_or_psm( sr_ptr, scn, psm );
      }
    } /* end if uuid matches */

    if ( match )
    {
      return sr_ptr;
    }
  }

  return 0;
}

bt_sd_srv_rec_type* bt_sd_find_service_record_by_uuid128(
  bt_sd_db_enum_type         db_type,
  const bt_sd_uuid128_type*        uuid128_ptr,
  uint8                      scn,
  uint16                     psm
)
{
  bt_sd_srv_rec_type*       sr_ptr;
  boolean                   match = FALSE;

  bt_sd_srv_rec_iter.init( db_type, BT_SD_ITER_ALL );
  for ( sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        sr_ptr != 0;

        sr_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  {
    if (  sr_ptr->is_custom_srv &&
          BT_SD_UUID128_EQUAL( &sr_ptr->srv_uuid128, uuid128_ptr ) )
    {
      if ( ( scn == BT_RC_SCN_NOT_SPECIFIED ) && ( psm == BT_L2_PSM_INVALID ) )
      {                                   
        match = TRUE;
      }
      else
      {
        match = bt_sd_match_service_attribute_by_scn_or_psm( sr_ptr, scn, psm );
      }
    } /* end if uuid matches */
    
    if ( match )
    {
      return sr_ptr;
    }
  }

  return 0;
}

#ifdef FEATURE_BT_EXTPF_SML
/*===========================================================================

FUNCTION
  bt_sd_find_syncml_service_record

DESCRIPTION
  Return the service record of a SyncML server or SyncML client

===========================================================================*/

bt_sd_srv_rec_type* bt_sd_find_syncml_service_record(
  bt_sd_db_enum_type         db_type,
  boolean                    client
)
{
  bt_sd_srv_rec_type* srv_rec_ptr;
  uint8               syncml_flag;

  syncml_flag = client ? BT_SD_DB_SYNCML_CLIENT_B : BT_SD_DB_SYNCML_SERVER_B;

  bt_sd_srv_rec_iter.init( db_type, BT_SD_ITER_ALL );
  for ( srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.first();
                                       /* Explicitly cast away the constness */
        srv_rec_ptr != 0;
        srv_rec_ptr = ( bt_sd_srv_rec_type* ) bt_sd_srv_rec_iter.next() )
                                      /* Explicitly cast away the constness */
  {
    if ( srv_rec_ptr->syncml_flag == syncml_flag )
    {
      return srv_rec_ptr;
    }
  }

  return 0;
}
#endif /* FEATURE_BT_EXTPF_SML */


/*===========================================================================

FUNCTION
  bt_sd_find_service_attribute

DESCRIPTION
  Find a service attribute with a given attribute ID

===========================================================================*/

bt_sd_srv_attr_rec_type* bt_sd_find_service_attribute(
  bt_sd_srv_rec_type*         srv_rec_ptr,
  uint16                      srv_attr_id
)
{
  int i;

  for ( i = 0; i < srv_rec_ptr->num_srv_attr; i++ )
  {
    if ( srv_rec_ptr->srv_attr[i].attr_id == srv_attr_id )
    {
      return &( srv_rec_ptr->srv_attr[i] );
    }
  }

  return ( NULL );
}


/*===========================================================================

FUNCTION
  bt_sd_get_num_service_record

DESCRIPTION
  Return the number of used service record

===========================================================================*/

uint16 bt_sd_get_num_service_record(
  bt_sd_db_enum_type         db_type
)
{
  uint16 num_srv_rec = 0;

  switch( db_type )
  {
    case BT_SD_DB_TYPE_LOCAL_DB:
      num_srv_rec = bt_sd_local_srv_db.num_used_srv_rec;
      break;
    case BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB:
      num_srv_rec = bt_sd_srv_srch_result_db.num_used_srv_rec;
      break;
    default:
      BT_ERR( "BT SD: Bad DB Type %x", db_type, 0, 0 );
      break;
  }

  return num_srv_rec;
}


/*===========================================================================

FUNCTION
  bt_sd_any_local_services_enabled

DESCRIPTION
  Returns whether any local service records are enabled.

===========================================================================*/
boolean bt_sd_any_local_services_enabled
(
  void
)
{

  boolean  any_local_services_enabled;

  if ( bt_sd_local_srv_db.num_enabled_srv_rec > 0 )
  {
    any_local_services_enabled = TRUE;
  }
  else
  {
    any_local_services_enabled = FALSE;
  }

  return ( any_local_services_enabled );

}


/*===========================================================================

FUNCTION
  bt_sd_any_local_services_connectable

DESCRIPTION
  Returns whether any local service records are connectable.

===========================================================================*/
boolean bt_sd_any_local_services_connectable
(
  void
)
{

  boolean  any_local_services_connectable;

  if ( bt_sd_local_srv_db.num_connectable_srv_rec > 0 )
  {
    any_local_services_connectable = TRUE;
  }
  else
  {
    any_local_services_connectable = FALSE;
  }

  return ( any_local_services_connectable );

}

#endif /* FEATURE_BT */

