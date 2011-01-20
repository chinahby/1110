/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY

GENERAL DESCRIPTION
  This file contains the APIs exported by PBM service which is executed as a
  library within the PBM client's task.

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmlib.c#93 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
04/01/09   kcp     fixed defect in pbm_is_init_completed
04/01/09   kcp     fixed CR146463  MS does not display alpha tags for FDN entries
09/01/09   kcp     fixed compilation errors with VU MSMSHARED_PBM.01.01.23
12/26/08   kcp     added changes for ASUS CR 165600
10/01/08   sg      In pbm_clear_phonebook() shift out all records to clear a
                   circular phonebook
02/13/08   clm     pbm_record_validate() now takes into account a leading '+' digit
                   when determining whether a field of type PBM_FT_PHONE exceeds max length.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
11/01/07   clm     Fix CR130077.  Make it possible to add empty records to
                   circular caches.  This makes EFS and SIM circular record
                   files similar in behavior.
10/24/07   clm     Fix CR125200.  Record id's from external apps should be
                   unaware of special circular cache indexing.
08/08/07   cvs     Fix abort when accessing sim PB's before SIM_INIT_COMPLETED
06/20/07   cvs     Fix invalid ptr access
05/02/07   cvs     Fix emergency category prioritization
03/16/07   cvs     Add support for error category
01/05/07   cvs     Increase allowed numbers of fields
10/10/06   cvs     Move messages to PBM's diag msg ID
08/02/06   cvs     Header file cleanup for WINCE
08/02/06   cvs     Fix problem with prior checkin
07/27/06   slw     Fix menu items displayed when adding FDNs
07/24/06   cvs     Fixes for unequal size multi pb set support
07/11/06   slw     Fix for fields being erased
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/05/06   cvs     Emergency category priority fixes
03/24/06   cvs     %x -> 0x%x in log messages
03/23/06   cvs     Lint fixes.
03/21/06   cvs     Add function to get extended phonebook info
03/10/06   cvs     Lint fixes
03/09/05   cvs     Fix clearing SIM call history
03/08/06   cvs     Fixes related to emergency categories
03/06/06   cvs     Add new fields
02/23/06   cvs     rework memory allocation macros
02/22/06   cvs     fix pbm_find_name()
02/21/06   cvs     pbm_write_lock() should always call callback if SUCCESS is returned
02/14/06   cvs     Fix num fields in emergency group
01/19/06   cvs     Add pbm_emergency_number_cat(), cleanup emergency nums
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
11/15/05   cvs     Fix pbm_find_name()
11/10/05   cvs     Add network ECC numbers
10/18/05   cvs     Add new fields
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
09/01/05   cvs     Fix memory overwrites in pbm_enum_rec_init()
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   cvs     Fix lint warning
08/26/05   AT      Added new Emergency Group for searches.
08/24/05   cvs     More optimizations of enum_rec_init(), also fix some cases where
                   we didn't release critical sections in error legs.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/19/05   cvs     Change error handling on unique IDs (more tolerant)
08/12/05   AT      Added UCS2 support for strings.
08/02/05   cvs     Fix memory leak in pbm_write()
08/01/05   cvs     rename field
07/29/05   AT      Improved some messages.
07/29/05   cvs     Fix deadlock issue by entering pbm_write_lock_crit_sect
                   before entering pbm_crit_sect.
07/27/05   AT      Adding check for maximum record size when presented
                   with a record.
07/08/05   cvs     Add new fields
07/07/05   cvs     Add error messages
06/30/05   AT      Made pbm_enum_rec_init check if the search is legal.
06/28/05   cvs     clean up critical section in error case.
06/23/05   cvs     Speed up searching, using qsort() for results
06/03/05   cvs     Multiproc cleanup, add metacomments
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
04/15/05   cvs     Fix returned values in error cases
04/12/05   AT      Allow PBM_DEFAULT when using legacy functions
                   pbm_file_info and pbm_find_name.
                   Also, only generate a USIM sync value for ADN files.
                   Various lint fixes.
04/08/05   cvs     Add fields
04/01/05   cvs     Fix infinite loop when registering the second notification.
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/22/05   PA      Do not update unique id for deletion/update.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for read lock, non-blocking write-lock,
                   support for greater than enumeration.
02/03/05   PA      Free fields in record write if phonebook id is not valid.
01/18/05   PA      Moved checks in record write affected by asynchronous
                   processing to a critical section.
                   Fixed lint errors.
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added support for distribution list
12/18/04   SUN     Get the cache from the record id in pbm_insert_in_sorted_order
12/16/04   SUN     Added a new Addr field
12/15/04   SUN     Get Fields in sorted order
12/08/04   PA      Continue over NULL records in pbm_find_number
12/08/04   PA      Fix memcpy parameters in get field info
12/08/04   PA      Allow write of complex fields.
12/03/04   PA      Record validation and field info APIs
11/18/04   PA      Support for complex fields and write lock.
09/29/04   PA      Initial revision based on uiupbm.c
===========================================================================*/

#include "customer.h"
#ifdef FEATURE_PBM_TASK

#include "tmc.h"

#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmcache.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmefs.h"
#include "pbmtask.h"
#include "stdlib.h"
#include "pbm_conversion.h"

/* <EJECT> */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/

static boolean pbm_find_ecc_cat_in_rec(uint16 rec_id,
                                       uint8 *ecc_category_ptr,
                                       pbm_field_id_e_type *found_field_ptr);

/* Below is the call back the caller to pbm_find_name wants us to call. */
static PBM_FIND_CB_FUNC pbm_find_name_user_cb;

// Maximum number of distribution lists a contact can be associated with.
#define PBM_DLISTS_MAX 50

#ifdef FEATURE_MMGSDI
/*===========================================================================
                     Declare data for DS to register with PBM
===========================================================================*/
PBM_EVENT_FUNC ds_ready_fn = NULL;
#endif

/*===========================================================================
    Declare static globals for pbm_enum_rec_init() and pbm_enum_next_rec()
===========================================================================*/
static int g_search_enumerator = 0;
static uint16 *g_search_results = NULL;
static int g_search_length = 0;
//this will store the info abt the init status of EFS and SIMs
pbm_init_info_s_type pbm_init_info = {0};

/* <EJECT> */
/*===========================================================================
                              STATIC DATA
===========================================================================*/
static pbm_device_type pbm_default_pb_ids[] = { PBM_EFS, PBM_SDL, PBM_ADN,
                                                PBM_FDN,PBM_SDN
#if (MAX_UIMS > 1)
                                                ,PBM_ADN2, PBM_FDN2, PBM_SDN2
#endif
                                               };
#ifdef FEATURE_ECC_HANDLING_AT_PBM
static pbm_device_type pbm_ecc_pb_ids[] = { PBM_ECC, PBM_ECC_OTHER,
#if (MAX_UIMS > 1)
                                                ,PBM_ECC2
#endif
                                               };
#endif


/* Note: When new fields are added to this table, update PBM_MAX_NUM_FIELDS
 * if required */

//lint -e{785} too few initializers
pbm_g_field_info_s_type g_field_info[NUM_GROUPS] =
{
  /* SIM Group */
  {
    7,
    {
      /* Field ID,   Max No of items, Max length,         Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE  },
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, TRUE  },
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE  },
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE },
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE },
    }
  },
    /* ADN Group */
  {
        11,
    {
      /* Field ID,   Max No of items, Max length,         Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE  },
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, TRUE  },
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE  },
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE },
      { PBM_FIELD_ADDITIONAL_NUMBER, 0, PBM_NUM_SIZE,     PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_GRPNAME_ID,      0, PBM_WORD_SIZE,      PBM_FT_WORD,     TRUE,  TRUE  },
      { PBM_FIELD_OTHERNAME,       0, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_EMAIL,           0, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE },
    }
  },

  /* SDL Group */
  {
    7,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_OTHER,     1, PBM_NUM_SIZE,        PBM_FT_PHONE,   TRUE,  TRUE  },
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,        PBM_FT_PHONE,   TRUE,  TRUE  },
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,       PBM_FT_WORD,    FALSE, TRUE  },
      { PBM_FIELD_VOICETAG,        1, PBM_DWORD_SIZE,      PBM_FT_DWORD,   TRUE,  TRUE  },
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,       PBM_FT_BYTE,    FALSE, FALSE },
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE,  PBM_FT_COMPLEX, FALSE, FALSE },
    }
  },

  /* EFS Group */
  {
    PBM_MAX_NUM_FIELDS,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                    1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
#ifdef CUST_EDITION
      { PBM_FIELD_PHONE_GENERIC,           1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_WORK,              1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_HOME,              1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_FAX,               1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_EMAIL,                   2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_URL,                     2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_ADDRESS,                 2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_NOTES,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_RINGTONE_AUDIO,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_GROUP,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
#else
      { PBM_FIELD_PHONE_GENERIC,          15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_WORK,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_HOME,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_FAX,              15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_VOICE,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_CELL,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_PAGER,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PHONE_OTHER,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_EMAIL,                   2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_URL,                     2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_ADDRESS,                 2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_NOTES,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_ORG,                     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_JOBTITLE,                1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_LASTNAME,                1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_FIRSTNAME,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_OTHERNAME,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_NAMEPREFIX,              1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_NAMESUFFIX,              1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHOTO,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_BINARY,   TRUE,  TRUE  },
      { PBM_FIELD_PHOTOURL,                1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_POBOX,                   1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_STREET,                  1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_ADDREXTRA,               1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_CITY,                    1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_STATE,                   1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_COUNTRY,                 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_ZIPCODE,                 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_UID,                     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PUBLICKEY,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_BINARY,   TRUE,  TRUE  },
      { PBM_FIELD_PUBLICKEYSTRING,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_GSM_TONNPI,              1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE },
      { PBM_FIELD_INDEX,                   1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE  },
      { PBM_FIELD_SPEED_DIAL,              1, PBM_WORD_SIZE,      PBM_FT_WORD,     TRUE,  TRUE  },
      { PBM_FIELD_SIP_URL,                 1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_DIAL_STRING,             1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHONE_VIDEOCALLERID,     1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PRIVATE_EMAIL1,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PRIVATE_EMAIL2,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_EMAIL1,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_EMAIL2,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_URL,            1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHONE_BUSINESS_FAX,      1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_STREET,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_CITY,           1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_STATE,          1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_COUNTRY,        1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_POSTALCODE,     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_NOTES,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_RINGTONE_AUDIO,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_RINGTONE_VIDEO,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BIRTHDAY,                1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_EVENT_UID,               1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DEFAULT_PHONE,           1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DEFAULT_EMAIL,           1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DEFAULT_ADDRESS,         1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DEFAULT_URL,             1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DEFAULT_FAX,             1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DISTRIBUTION_LIST,       1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_COMPLEX,                62, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE },
      { PBM_FIELD_PRIVATEPHONE_VOICETAG,   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_BUSINESSPHONE_VOICETAG,  1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_PRIVATEMOBILE_VOICETAG,  1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_BUSINESSMOBILE_VOICETAG, 1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_VIDEOTELEPHONY,          1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_USEDCOUNT,               1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_BCARD,                   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_GROUP,                   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_DLISTS,      PBM_DLISTS_MAX, PBM_DWORD_SIZE,    PBM_FT_DWORD,    TRUE,  TRUE  },
      { PBM_FIELD_PH_UID,                  1, PBM_DDWORD_SIZE,    PBM_FT_BINARY,   TRUE,  TRUE  },
      { PBM_FIELD_PRIVATE_ADDITIONALINFO,  1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_BUSINESS_ADDITIONALINFO, 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_PHONE_VIDEOTELEPHONY,    1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_PREFER_VT,               1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE  },
      { PBM_FIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE, 62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE  },
      { PBM_FIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE,  62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE  },
      { PBM_FIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE,62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE  },
      { PBM_FIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE, 62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE  },
      { PBM_FIELD_FIELDID_PRIMARY_PHONE, 1, PBM_WORD_SIZE, PBM_FT_WORD, TRUE,  TRUE  },
#endif
    }
  },

  /* Call History Group */
  {
    12,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,   TRUE,  TRUE  },
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,        PBM_FT_PHONE,  TRUE,  TRUE  },
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   FALSE, TRUE  },
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   TRUE,  TRUE  },
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   FALSE, FALSE },
      { PBM_FIELD_CALL_TYPE,       1, PBM_WORD_SIZE,       PBM_FT_WORD,   FALSE, TRUE  },
      { PBM_FIELD_DATE,            1, PBM_DWORD_SIZE,      PBM_FT_DWORD,  FALSE, TRUE  },
      { PBM_FIELD_DURATION,        1, PBM_DWORD_SIZE,      PBM_FT_DWORD,  FALSE, TRUE  },
      { PBM_FIELD_COMPLEX,         8, PBM_MAX_FIELD_SIZE,  PBM_FT_COMPLEX,FALSE, FALSE },
      { PBM_FIELD_CALL_METHOD,     1, PBM_WORD_SIZE,       PBM_FT_WORD,   FALSE, TRUE  },
      { PBM_FIELD_AOC,             1, PBM_MAX_FIELD_SIZE,  PBM_FT_BINARY, FALSE, TRUE  },
      { PBM_FIELD_TECH_TYPE,       1, PBM_WORD_SIZE,       PBM_FT_WORD,   TRUE,  TRUE  },
    }
  },

  /* Emergency Group */
  {
    7,
    {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                 1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,     TRUE,  TRUE  },
      { PBM_FIELD_HARDCODED_ECC,        1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_NV_ECC,               1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_SIM1_ECC,             1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_SIM2_ECC,             1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_NETWORK_ECC,          1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE  },
      { PBM_FIELD_ECC_SERVICE_CATEGORY, 1, PBM_BYTE_SIZE,       PBM_FT_BYTE,     FALSE, TRUE  }
    }
  },
};

#define PBM_MAX_REQUIRED_FIELDS 3
static const int g_required_fields[NUM_GROUPS][PBM_MAX_REQUIRED_FIELDS] = {
  { 0, 0, 0 },
  { 0, 0, 0 },
  { PBM_FIELD_NAME, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};

/* <EJECT> */
/*===========================================================================*
 *              PHONE BOOK MANAGER RECORD BASED API                          *
 *                                                                           *
 * These functions supply the API for the record based requests.             *
 *===========================================================================*/


/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_ID

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_fields_size_from_id( uint16 rec_id )
{
  PB_CACHE *cache;
  uint16 location;
  pbm_return_type ret;
  pbm_device_type pb_id;
  pbm_field_s_type  *fields = NULL;
  int num_fields = 0;

  /* Get fields pointer from ID */
  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return 0;

  pb_id = pbm_record_id_to_phonebook(rec_id);

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
  {
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return 0;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  do
  {
    if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
        ret != PBM_ERROR_SYNC)
    {
      break;
    }

    location = pbm_record_id_to_location(rec_id);

    if (location > cache->num_of_records)
    {
      PBM_MSG_ERR("Location %d is higher than length of cache %d.",
                  location, cache->num_of_records, 0);
      ret = PBM_ERROR_INDEX_INVALID;
      break;
    }

    if (cache->circular == TRUE)
    {
      //For circular buffers we need to so some special translation for location
      location = PBM_CIRC_CACHE_NTHLOC(cache, location);
    }

    if (location == 0)
    {
      PBM_MSG_ERR("Location cannot be zero.", 0, 0, 0);
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if ((cache->pb_cache_array == NULL) ||
        (cache->pb_cache_array[location] == NULL) ||
        (cache->pb_cache_array[location]->fields == NULL) ||
        (cache->pb_cache_array[location]->num_fields == 0))
    {
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    fields = cache->pb_cache_array[location]->fields;
    num_fields = cache->pb_cache_array[location]->num_fields;

    ret = PBM_SUCCESS;

  } while (0); /*lint !e717 */

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  if (ret != PBM_SUCCESS)
  {
    return 0;
  }

  return pbm_calculate_size_from_fields(fields, num_fields);
}

/*===========================================================================
FUNCTION PBM_RECORD_READ

DESCRIPTION
Searches the cache appropriate to the given phonebook and retrieves
information contained in a specified record, returning success/failure
indication.  Does not delete the record from the address book.  If no
record is found, PBM_ERROR_NOT_FOUND is returned.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_read(uint16            rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size)
{
  PB_CACHE *cache;
  uint16 location = 0;
  pbm_return_type ret;
  uint32 record_size;
  pbm_device_type   pb_id;

  PBM_MSG_HIGH("PBM: Attempt to read record 0x%x",rec_id, 0, 0);

  /* sanitize return values for error conditions*/
  if (num_fields_ptr)
    *num_fields_ptr = 0;

  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  pb_id = pbm_record_id_to_phonebook(rec_id);

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
  {
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  do
  {
    if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
        ret != PBM_ERROR_SYNC)
    {
      break;
    }

    location = pbm_record_id_to_location(rec_id);

    if (location > cache->num_of_records)
    {
      PBM_MSG_ERR("Location is higher than length of cache.", 0, 0, 0);
      ret = PBM_ERROR_INDEX_INVALID;
      break;
    }

    if (cache->circular == TRUE)
    {
      //For circular buffers we need to so some special translation for location
      location = PBM_CIRC_CACHE_NTHLOC(cache, location);
    }

    if (location == 0)
    {
      PBM_MSG_ERR("Location cannot be zero.", 0, 0, 0);
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if ((cache->pb_cache_array == NULL) ||
        (cache->pb_cache_array[location] == NULL) ||
        (cache->pb_cache_array[location]->fields == NULL) ||
        (cache->pb_cache_array[location]->num_fields == 0))
    {
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    record_size = (uint32) pbm_calculate_size_from_fields(
                                  cache->pb_cache_array[location]->fields,
                                  cache->pb_cache_array[location]->num_fields);
    if (data_buf_size < record_size)
    {
      PBM_MSG_ERR("Buffer too small.  Buffer %d, required %d.", data_buf_size,
                  record_size, 0);
      ret = PBM_ERROR;
      break;
    }

    if (category_ptr)
      *category_ptr   = cache->pb_cache_array[location]->category;
    if (num_fields_ptr)
      *num_fields_ptr = cache->pb_cache_array[location]->num_fields;

    if (data_buf)
    {
      memcpy(data_buf, cache->pb_cache_array[location]->fields, record_size);
    }

    ret = PBM_SUCCESS;

    PBM_MSG_HIGH("PBM: pbm_record_read(), record 0x%x, found %d fields",
             rec_id, cache->pb_cache_array[location]->num_fields, 0);

  } while (0); /*lint !e717 */

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE

DESCRIPTION
Add/updates/deletes a record in the address book (both EFS and SIM).  This is
the new interface that supports many different fields per contact.

To add a record:
1)  Set rec_id = 0xFFFF (PBM_GET_FIRST_AVAILABLE) (and rec_id_ptr=&rec_id).
2)  Set items_ptr equal to an array of fields for the new record
3)  Set item_count equal to the length of above array
    To update a record, do the same as when adding a record (above), except set
    rec_id to the record ID of the record to be updated.  Note that the fields
    given in items_list will replace all fields in the current record, i.e. the
    current fields will be deleted and replaced, not added to.

To delete a record:
1)  Set rec_id to the record ID of the record to be deleted.
2)  Set data_buf = NULL.
3)  Other fields are not used.

When making an asynchronous call, a success return value indicates that
the command has been successfully queued. The actual result of the operation
is given in the first argument to the callback function proc_func(). The
resulting record id is returned synchronously within rec_id_ptr and along
with the proc_func callback.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write( pbm_device_type         pb_id,
                                  uint16                 *rec_id_ptr,
                                  uint16                  cat,
                                  int                     num_fields,
                                  const uint8            *data_buf,
                                  int                     data_buf_size,
                                  PBM_WRITE_CB_FUNC       proc_func,
                                  void                   *user_data)
{
  pbm_write_record_s_type *write_cmd = NULL;
  PB_CACHE                *cache = pbm_pb_id_to_cache(pb_id);
  pbm_return_type          ret;
  uint16                   rec_id;
  pbm_addr_cache_s_type   *prev_rec = NULL;
  pbm_addr_cache_s_type   *record = NULL;
  uint16                   unique_id;
  boolean                  deletion;
  boolean                  new = FALSE;
  int                      location;
  uint8                   *curr_index = NULL;
  uint8                   *prev_index = NULL;
  uint64                   ph_unique_id;
  pbm_field_s_type        *my_data_buf = NULL;

  if (!cache)
  {
    PBM_MSG_ERR("Invalid phonebook id %d", pb_id, 0, 0);
    return PBM_ERROR;
  }

  PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

  PBM_MSG_HIGH("PBM: Attempt to write %d fields to record 0x%x",
           num_fields, *rec_id_ptr, 0);

  /* Check for write lock */
  if ((ret = pbm_check_write_lock()) != PBM_SUCCESS)
  {
    return ret;
  }

  /* make a copy of the data_buf.  data_buf belogs to the caller, and
   * my_data_buf belongs to PBM.  If data_buf is NULL, this is probably a
   * deletion.  my_data_buf is already NULL, and that's handled later */
  if (data_buf != NULL) {
    //lint -e{740} incompatible indirect types
    if (!pbm_copy_fields(&my_data_buf,
                         (pbm_field_s_type**) &data_buf, (uint32) num_fields))
    {
      PBM_MSG_ERR("Can't copy fields", 0, 0, 0);
      rex_leave_crit_sect(&pbm_write_lock_crit_sect);
      return PBM_ERROR_MEM_FULL;
    }
  }

  do
  {
    rex_enter_crit_sect(&pbm_crit_sect);

    /* Parameter check */
    ret = pbm_record_write_check(pb_id, rec_id_ptr, cat, my_data_buf,
                                 num_fields, proc_func);
    PBM_CHECK_ERR_BREAK(ret, "Parameter check failed");

    /* Check if the record itself is valid */
    ret = pbm_record_validate(pb_id, *rec_id_ptr,
                              (pbm_cat_e_type)cat, (uint8*)my_data_buf,
                              data_buf_size, num_fields);
    PBM_CHECK_ERR_BREAK(ret, "Record validation failed");

    /* Deletion is not allowed on circular pb's. Best we can do is
       write empty records in to shift out all old records. */
    deletion = (cache->circular)?(FALSE):(num_fields == 0 ? TRUE : FALSE);

    /* Find location of the entry within cache */
    if (*rec_id_ptr == PBM_GET_FIRST_AVAILABLE)
    {
      /* Give me the first available slot */
      if ((location = pbm_get_first_available(cache, deletion)) == 0)
      {
        PBM_MSG_HIGH("Could not find first available for cache %d %d.",
                 cache->num_of_records, cache->records_in_use, 0);
        ret = PBM_ERROR_MEM_FULL;
        break;
      }

      new = TRUE;
      rec_id = pbm_location_to_record_id(pb_id, location);
    }
    else
    {
      if (cache->circular == TRUE)
      {
        /* Cannot to write to anything but the PBM_GET_FIRST_AVAILABLE location
           in a circular file */
        ret = PBM_ERROR_NOT_SUPPORTED;
        break;
      }
      location = pbm_record_id_to_location(*rec_id_ptr);
      rec_id = *rec_id_ptr;
    }

    PBM_MSG_HIGH("PBM: pbm_record_write writing %d fields to record 0x%x",
             num_fields, rec_id, 0);

    if (location > cache->num_of_records)
    {
      PBM_MSG_ERR("Write: index is outside cache.", 0, 0, 0);
      ret = PBM_ERROR_INDEX_INVALID;
      break;
    }

    /* If it is deletion and record is not present, return error */
    if (deletion &&
        (!cache->pb_cache_array || !cache->pb_cache_array[location] ||
         !cache->pb_cache_array[location]->num_fields))
    {
      PBM_MSG_ERR("Write: Attempt to delete empty record pb_id %d, location %d",
                   pb_id, location, 0);
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    /* Allocate index array if USIM fields are present in previous or current
     * record and fill it */
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
    /* There's a race condition here.  If you write 2 records back to back
       that both require allocating USIM fields, the second one will get the
       same copy of the IAP, and will overwrite the first record's fields */
    if (!pbm_allocate_index(rec_id, my_data_buf, num_fields,
                            &prev_index, &curr_index) ||
        (curr_index &&
         !pbm_get_usim_index(rec_id, my_data_buf, num_fields, curr_index)) ||
        (prev_index &&
         !pbm_copy_usim_index(rec_id, prev_index)))
    {
      PBM_MSG_ERR("No space available for USIM fields record %d %d %d",
                  rec_id, prev_index, curr_index);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */


    /* Allocate PBM write command */
    write_cmd = (pbm_write_record_s_type *)
                    pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
    if (!write_cmd)
    {
      PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Save previous record to commit list if required */
    ret = pbm_list_backup(rec_id);
    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to save previous entry (rec_id: %d) to commit list %d",
                   rec_id, ret, 0);
      break;
    }

    /* Get a reference to previous record */
    prev_rec = pbm_cache_record_get(rec_id);
    if (!prev_rec)
    {
      PBM_MSG_ERR("Unexpected NULL previous record 0x%x", rec_id, 0, 0);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Allocate new cache record */
    record = pbm_cache_record_new();
    if (!record)
    {
      PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Generate unique id */
    if (pbm_action_requires_sim(pb_id))
    {
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (pb_id == PBM_ADN || pb_id == PBM_ADN2)
      {
        if (prev_rec->num_fields) /* deletion or update */
        {
          (void) pbm_uim_generate_sync_values(cache, &write_cmd->sync, FALSE);
          unique_id = prev_rec->unique_id;
          /* we didn't generate a new UID, so we don't care about wrap around */
        }
        else
        {
          unique_id = pbm_uim_generate_sync_values(cache, &write_cmd->sync, TRUE);

          if (!unique_id)
          {
            /* Do not allow unique id to wrap around */
            PBM_MSG_ERR("Unable to allocate SIM unique id", 0, 0, 0);

            /* This could return an error as well, but we don't use SIM UID */
            /* anywhere, so let's just keep going */
            /* This really shouldn't happen, as we regerate Sim UIDs when we */
            /* detect rollover inside pbm_uim_generate_sync_values() */
          }
        }
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        unique_id = 0xFFFF;
      }
    }
    else
    {
      unique_id = pbm_efs_generate_unique_id();

      if (!unique_id)
      {
        /* Do not allow unique id to wrap around */
        PBM_MSG_ERR("Unable to allocate PBM unique id", 0, 0, 0);
        ret = PBM_ERROR;
        break;
      }
    }

    /* Update cache using the new entry */
    if (deletion)
    {
      pbm_cache_record_fill(record, rec_id, unique_id, cat, 0, 0);

      /* Delete record in cache */
      ret = pbm_cache_delete(cache, rec_id);
      if (ret != PBM_SUCCESS)
      {
        PBM_MSG_ERR("PBM cache delete failed with error %d.", ret, 0, 0);
        break;
      }

      cache->pb_cache_array[location] = record; /* Dummy record */
    }
    else
    {
      /* If adding a new record to the EFS phone-book, update the field
       * list to append a new phone unique ID field. The validation check
       * performed earlier already checked that the field did not exist. */
      if (pb_id == PBM_EFS)
      {
        uint16 new_num_fields = (uint16)num_fields;

        if (new)
        {
          ret = pbm_get_next_phone_unique_id(&ph_unique_id);
          PBM_CHECK_ERR_BREAK(ret, "Get next unique ID failed");

          ret = pbm_append_phone_uid_field(ph_unique_id, &my_data_buf, &new_num_fields);
          PBM_CHECK_ERR_BREAK(ret, "Assign phone uid failed");

          num_fields = (int)new_num_fields;
        }
        else
        {
          if (!pbm_find_phone_uid_field(my_data_buf, num_fields, NULL))
          {
            /* Find current PH_UID field value from cache, and append it to
             * the update field list. */
            ret = pbm_get_phone_unique_id_from_record(prev_rec, &ph_unique_id);
            if (ret != PBM_SUCCESS)
            {
              PBM_MSG_ERR("Failed locating phone uid for existing record %d",
                   rec_id, 0,0);

              /* This is a pathological situation since the PBM EFS initialization
               * should have assigned unique IDs to all old entries. */
              ret = pbm_get_next_phone_unique_id(&ph_unique_id);
              PBM_CHECK_ERR_BREAK(ret, "Get next unique ID failed");
            }

            ret = pbm_append_phone_uid_field(ph_unique_id, &my_data_buf, &new_num_fields);
            PBM_CHECK_ERR_BREAK(ret, "Assign phone uid failed");

            num_fields = (int)new_num_fields;
          }
        }
      }

      /* Add record to cache, existing record is overwritten */
      pbm_cache_record_fill(record, rec_id, unique_id, cat,
                            (uint16) num_fields, my_data_buf);

      ret = pbm_cache_add(cache, record, new);
      if (ret != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Cache add failed with error %d.", ret, 0, 0);
        break;
      }
    }

  /* Allocate index array if USIM fields are present in previous or current
   * record and fill it */
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
    if (pbm_master_usim_cache_update(rec_id, prev_index, curr_index)
                                                            != PBM_SUCCESS)
    {
      (void) pbm_cache_delete(cache, rec_id);
      PBM_MSG_ERR("Unable to update master usim cache for %d", rec_id, 0, 0);
      ret = PBM_ERROR;
      break;
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MGSDI */

    rex_leave_crit_sect(&pbm_crit_sect);

    /* Send PBM command */
    write_cmd->cmd_type   = PBM_CMD_WRITE;
    write_cmd->rec_id     = rec_id;
    write_cmd->prev_rec   = prev_rec;
    write_cmd->proc_func  = proc_func;
    write_cmd->user_data  = user_data;

    write_cmd->curr_rec        = pbm_cache_record_get(rec_id);
#ifdef FEATURE_PBM_USIM_SUPPORT
    write_cmd->curr_usim_index = curr_index;
    write_cmd->prev_usim_index = prev_index;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

    if (cache->circular == TRUE)
    {
      //Go back to hiding cache location details for circular cache types
      location = PBM_CIRC_CACHE_LOCTOINDEX(cache, location);
      rec_id = pbm_location_to_record_id(pb_id, location);
    }
    *rec_id_ptr = rec_id;

    ret = PBM_SUCCESS;

  } while (0); /*lint !e717 */

  /* Error Handling */
  if (ret != PBM_SUCCESS)
  {
    rex_leave_crit_sect(&pbm_crit_sect);

    if (my_data_buf)
      pbm_free_fields_internal(&my_data_buf, num_fields);
    pbm_free_index(prev_index, curr_index);
    if (write_cmd)
      pbm_cmd_free(PBM_CMD_PTR(write_cmd));
    if (prev_rec)
      (void) pbm_cache_record_free(prev_rec);
    if (record)
      (void) pbm_cache_record_free(record);
  }

  rex_leave_crit_sect(&pbm_write_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_COMPARE_RECORDS

DESCRIPTION
This function is a comparison function that can be called by
qsort().  This uses the order flag from the search list.

DEPENDENCIES
pbm_get_name

SIDE EFFECTS

===========================================================================*/
static int pbm_compare_records(const void *pRecid1, const void *pRecid2, boolean bSearchLastFirst)
{
  static uint16 compareName1[PBM_TEXT_SIZE_CHARS];
  static uint16 compareName2[PBM_TEXT_SIZE_CHARS];
  uint32 name1_len;
  uint32 name2_len;
  int index;
  PB_CACHE *pb_cache = NULL;
  pbm_return_type ret;


  if (pRecid1 == NULL || pRecid2 == NULL)
  {
    PBM_MSG_ERR("Bad param to sort comparison fcn %d %d", pRecid1, pRecid2, 0);
    return 0;
  }

  //Get the last name
  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(*(uint16*)pRecid1));
  PBM_CHECK_PTR_RET(pb_cache, 0);

  index = pbm_record_id_to_location(*(uint16*)pRecid1);

  if (pb_cache->circular == TRUE)
  {
    //For circular buffers we need to so some special translation for index
    index = PBM_CIRC_CACHE_NTHLOC(pb_cache, index);
  }

  if (index == 0)
  {
    PBM_MSG_ERR("Index cannot be zero.", 0, 0, 0);
    return 0;
  }


  ret = pbm_get_name(pb_cache->pb_cache_array[index],
                     bSearchLastFirst, compareName1, ARR_SIZE(compareName1));

  if (ret != PBM_SUCCESS)
    return 0;

  name1_len = pbmutils_wstrlen(compareName1);

  //Get the last name
  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(*(uint16*)pRecid2));
  PBM_CHECK_PTR_RET(pb_cache, 0);

  index = pbm_record_id_to_location(*(uint16*)pRecid2);

  if (pb_cache->circular == TRUE)
  {
    //For circular buffers we need to so some special translation for index
    index = PBM_CIRC_CACHE_NTHLOC(pb_cache, index);
  }

  if (index == 0)
  {
    PBM_MSG_ERR("Index cannot be zero.", 0, 0, 0);
    return 0;
  }

  ret = pbm_get_name(pb_cache->pb_cache_array[index],
                     bSearchLastFirst, compareName2, ARR_SIZE(compareName2));

  if (ret != PBM_SUCCESS)
    return 0;

  name2_len = pbmutils_wstrlen(compareName2);

  return (pbm_strncmp(compareName1, compareName2,
                      MAX(name1_len,name2_len), FALSE, TRUE, TRUE));
}

static int pbm_compare_records_lastnamefirst(const void *pRecid1, const void *pRecid2)
{
  return pbm_compare_records(pRecid1, pRecid2, TRUE);
}

static int pbm_compare_records_notlastnamefirst(const void *pRecid1, const void *pRecid2)
{
  return pbm_compare_records(pRecid1, pRecid2, FALSE);
}


/*===========================================================================
FUNCTION PBM_ENUM_REC_INIT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_enum_next_rec(), which
iterates through the records that match the given criteria.  Search results
are stored in static variables and are overwritten by the next call to
pbm_rec_init().

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE.
          Bit 0:    0   - PBM_CASE_INSENSITIVE
          Bit 0:    1   - PBM_CASE_SENSITIVE
          Bit 1-2:  0   - PBM_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - PBM_SEARCHTYPE_EXACT
          Bit 1-2:  10  - PBM_SEARCHTYPE_CONTAINS
          Bit 3:    0   - PBM_DATATYPE_STR
          Bit 3:    1   - PBM_DATATYPE_BYTE
          Bit 4:    0   - PBM_FIELDID_NONE
          Bit 4:    1   - PBM_FIELDID_PHONE (Any type of phone)
          Bit 5:    0   - PBM_CALLHISTORY_NONE
          Bit 5:    1   - PBM_CALLHISTORY_ALL (incoming, outgoing, missed)
          Bit 6:    0   - PBM_ENUMERATE_ALL
          Bit 6:    1   - PBM_ENUMERATE_AVAILABLE
          Bit 7:    0   - PBM_MATCHING_NORMAL
          Bit 7:    1   - PBM_MATCHING_GREATER (Not supported with SEARCHTYPE_CONTAINS).
          Bit 8:    0   - PBM_SORT_LASTNAME_FIRST
          Bit 8:    1   - PBM_SORT_LASTNAME_SECOND

  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE | PBM_SEARCHTYPE_STARTSWITH)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_rec_init(pbm_device_type pb_id,
                               uint16 category,
                               pbm_field_id_e_type field_id,
                               const uint8 *data_ptr,
                               uint16 data_size,
                               uint32 flags)
{
  pbm_return_type ret = PBM_SUCCESS;
  uint16 *temp_results = NULL; /* Will point to array of record IDs */
  int result_index;
  int size = 0;
  int num_caches = 0;
  int caches_used = 0;
  int i;
  int cidx;
  int field_group;
  pbm_cat_e_type my_cat = (pbm_cat_e_type)category;

  boolean avail_flag = (((flags & PBM_ENUMERATE_MASK) ==
                          PBM_ENUMERATE_AVAILABLE) ? TRUE : FALSE);

  PB_CACHE *pb_cache_list[PBM_RESERVED] = {NULL,}; /* Cache list. */
  pbm_device_type *pb_ids = NULL;                  /* Phonebook list */

  PB_CACHE *pb_cache = NULL;    /* Used when list is one big. */
  pbm_device_type ch_pb_ids[] = {PBM_DIALED, PBM_RCVD, PBM_MISS};

  /* First make sure the category is valid for the search. */
  if (my_cat != PBM_CAT_NONE && pb_id != pbm_cat_to_phonebook(my_cat))
  {
    PBM_MSG_ERR("Category %d and phonebook %d does not match",
                    my_cat, pb_id, 0);
    return PBM_ERROR_CAT_NOT_SUPPORTED;
  }

  /* Get the field grouping based on pb_id. */
  field_group = pbm_pb_id_to_fieldgroup(pb_id);
  if (field_group < 0)
  {
    PBM_MSG_ERR("Invalid group: phonebook %d and category %d", pb_id,
                my_cat, 0);
    return PBM_ERROR;
  }

  /* Now if they are searching for a field, make sure they can. */
  if (field_id != PBM_FIELD_NONE)
  {
    for (i = 0; i < g_field_info[field_group].num_fields; i++)
    {
      if (field_id == g_field_info[field_group].info[i].field_id)
      {
        if (((my_cat == PBM_CAT_NONE) && !g_field_info[field_group].info[i].field_search) ||
            ((my_cat != PBM_CAT_NONE) && !g_field_info[field_group].info[i].field_cat_search))
        {
          return PBM_ERROR_NOT_SUPPORTED;
        }
        break;
      }
    }
  }

  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  PBM_MEM_FREEIF(g_search_results);

  rex_enter_crit_sect(&pbm_crit_sect);

  /* When all call history phonebooks are enumerated, pb_id
   * could be PBM_DEFAULT */
  if (pb_id == PBM_DEFAULT)
  {
    if ((flags & PBM_CALLHISTORY_MASK) == PBM_CALLHISTORY_ALL)
    {
      pb_ids = ch_pb_ids;
      num_caches = ARR_SIZE(ch_pb_ids);
    }
    else
    {
      pb_ids = pbm_default_pb_ids;
      num_caches = ARR_SIZE(pbm_default_pb_ids);
    }
    /* Check if the caches are ready.
     * For PBM_DEFAULT, no SIM and no SIM file should not be returned as an
     * error */
    for (cidx = 0; cidx < num_caches; ++cidx)
    {
      if ((ret = check_pb_ready(pb_ids[cidx])) == PBM_SUCCESS)
      {
        pb_cache_list[caches_used++] = pbm_pb_id_to_cache(pb_ids[cidx]);
      }
      else
      {
        PBM_MSG_ERR("Cache for PB %d not available %d", (pb_ids?pb_ids[cidx]:pb_id), ret, 0);
        //(ret != PBM_ERROR_RESTRICTED)check is to skip if particular cache have restricted access CR146463
        //ex - some SIMs may restrict access to ADN records when FDN is disabled
        if (!avail_flag &&
            ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
        {
          rex_leave_crit_sect(&pbm_crit_sect);
          rex_leave_crit_sect(&pbm_read_lock_crit_sect);
          return ret;
        }
      }
    }
  } /*  if (pb_id == PBM_DEFAULT)... */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  //process all the ECC caches
  else if (pb_id == PBM_ECC)
  {
    num_caches = ARR_SIZE(pbm_ecc_pb_ids);
    for (cidx = 0; cidx < num_caches; ++cidx)
    {
      if ((ret = check_pb_ready(pbm_ecc_pb_ids[cidx])) == PBM_SUCCESS)
      {
        pb_cache_list[caches_used++] = pbm_pb_id_to_cache(pbm_ecc_pb_ids[cidx]);
      }
    }
  }
#endif
  else
  {
    /* Check if the cache is ready */
    if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
    {
      rex_leave_crit_sect(&pbm_crit_sect);
      rex_leave_crit_sect(&pbm_read_lock_crit_sect);
      return ret;
    }

    pb_cache_list[0] = pbm_pb_id_to_cache(pb_id);
    caches_used = 1;
  }


  /* Allocate the maximum number of possible results. */
  for (cidx = 0; cidx < caches_used; ++cidx) {
    size += pb_cache_list[cidx]->records_in_use;
  }

  if (size == 0)
  {
    g_search_length = 0;
    g_search_enumerator = 0;
    rex_leave_crit_sect(&pbm_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_SUCCESS;
  }

  /* Allocate srch results array*/
  temp_results = mem_malloc(&tmc_heap, (uint32)size * sizeof(uint16));

  if (temp_results == NULL)
  {
    g_search_length = 0;
    g_search_enumerator = 0;
    rex_leave_crit_sect(&pbm_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR;
  }

  result_index = 0;

  /* All call history phonebooks */
  /* pb_id could be PBM_DEFAULT or any one of the call history phonebooks */
  for (cidx = 0; cidx < caches_used; ++cidx) {
    pb_cache = pb_cache_list[cidx];

    if (!pb_cache)
    {
      PBM_MSG_ERR("Cache was NULL for index %d", cidx, 0, 0);
      continue;
    }

    for ( i = 1; i <= pb_cache->num_of_records; i++ )
    {
      int index;

      if (pb_cache->circular)
        index = PBM_CIRC_CACHE_NTHLOC(pb_cache, i);
      else
        index = i;

      if (pb_cache->pb_cache_array[index])
      {
        if (match_found(my_cat, field_id, data_ptr, data_size, flags,
                        pb_cache->pb_cache_array[index]))
        {
          if (result_index >= size)
          {
            PBM_MSG_ERR("Allocated %d records, index %d", size, result_index, 0);
            break;
          }
          if (pb_cache->circular)
          {
            //Hide the location shifting details for circular files from outside apps.
            temp_results[result_index++] = pbm_location_to_record_id(pb_cache->pb_id, i);
          }
          else
          {
            temp_results[result_index++] = pb_cache->pb_cache_array[index]->record_id;
          }
        }
      }
    }
  }

  if ((flags & PBM_SORT_ORDER_MASK) == PBM_SORT_LASTNAME_FIRST)
  {
    qsort(temp_results, (uint32)result_index, sizeof(temp_results[0]), pbm_compare_records_lastnamefirst);
  }
  else
  {
    qsort(temp_results, (uint32)result_index, sizeof(temp_results[0]), pbm_compare_records_notlastnamefirst);
  }

  /* Allocate g_search_results[] and copy results in*/
  do
  {
    if (result_index == 0)
    {
       g_search_results = NULL;
       g_search_length = 0;
    }
    else
    {
      PBM_MEM_ALLOC(g_search_results, (uint32)result_index * sizeof(g_search_results[0]));
      PBM_CHECK_PTR_BREAK(g_search_results, ret, PBM_ERROR_MEM_FULL);

      /* g_search_results is a 0-based array*/
      for (i=0; i<result_index; i++)
      {
        g_search_results[i] = temp_results[i];
      }

      g_search_length = (int) result_index;
    }
  } while ( 0 ); /*lint !e717 */

  /* clean up the temp results */
  mem_free(&tmc_heap,(void *)temp_results);

  /* Initialize enumerator*/
  g_search_enumerator = 0;

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_ENUM_NEXT_REC_ID

DESCRIPTION
Retrieves the next record ID that matches the search criteria
specified in the most recent call to pbm_enum_rec_init().

Only one search at a time is supported.  This can allow situations where
one application has called pbm_enum_rec_init() and is in the process of
calling pbm_enum_next_rec_id() a few times.  Then another application calls
pbm_enum_rec_init() and changes the search criteria and resets the search
before the first application is done.

When the end of the enumeration is reached, the index is not reset to the
beginning of the enumeration.  All subsequent calls to this function must
continue to return PBM_EOF.  The caller must call pbm_enum_rec_init() to
reinitialize the search criteria.

DEPENDENCIES
pbm_enum_rec_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_next_rec_id(uint16 *rec_id_ptr)
{
  pbm_return_type ret;
  PB_CACHE *cache;
  uint16 location;
  pbm_device_type pb_id;

  PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  /* Empty list?*/
  if (!g_search_results)
  {
    *rec_id_ptr = PBM_RECID_NULL;
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR_NOT_FOUND;
  }

  /* Skip over blank entries*/
  /* (Blank because they have been deleted from the cache)*/
  while (g_search_enumerator < g_search_length)
  {
    *rec_id_ptr = g_search_results[g_search_enumerator];

    /* see if the entry is blank */
    pb_id = pbm_record_id_to_phonebook(*rec_id_ptr);

    if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    {
      *rec_id_ptr = PBM_RECID_NULL;
      rex_leave_crit_sect(&pbm_read_lock_crit_sect);
      return PBM_ERROR_NOT_FOUND;
    }

    rex_enter_crit_sect(&pbm_crit_sect);

    do
    {
      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
          ret != PBM_ERROR_SYNC)
      {
        break;
      }

      location = pbm_record_id_to_location(*rec_id_ptr);

      if (location > cache->num_of_records)
      {
        PBM_MSG_ERR("Location is higher than length of cache.", 0, 0, 0);
        ret = PBM_ERROR_INDEX_INVALID;
        break;
      }

      if (cache->circular == TRUE)
      {
        //For circular buffers we need to so some special translation for location
        location = PBM_CIRC_CACHE_NTHLOC(cache, location);
      }

      if (location == 0)
      {
        PBM_MSG_ERR("Location cannot be zero.", 0, 0, 0);
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if ((cache->pb_cache_array == NULL) ||
          (cache->pb_cache_array[location] == NULL) ||
          (cache->pb_cache_array[location]->fields == NULL) ||
          (cache->pb_cache_array[location]->num_fields == 0))
      {
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }
    } while (0); /*lint !e717 */

    rex_leave_crit_sect(&pbm_crit_sect);

    if (ret == PBM_SUCCESS)
    {
      break;
    }

    g_search_enumerator++;
  }

  if (g_search_enumerator >= g_search_length)
  {
    *rec_id_ptr = PBM_RECID_NULL;
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_EOF;
  }

  g_search_enumerator++;

  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  //Area of improvement:  Double check our search result, and make sure it
  //still matches the search criteria (or probably do it in record write?)

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_CLEAR_CALL_HISTORY

DESCRIPTION
  Clears all call history entries.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_call_history()
{
  uint32 i;
  int j;
  uint16 rec_id;
  pbm_return_type ret = PBM_SUCCESS;
  pbm_return_type write_retval = PBM_SUCCESS;
  PB_CACHE *cache;

  pbm_device_type devices[] =
    { PBM_RCVD, PBM_DIALED, PBM_MISS
#ifdef FEATURE_MMGSDI
      , PBM_LND, PBM_LND2
#ifdef FEATURE_PBM_USIM_SUPPORT
      , PBM_OCI, PBM_OCI2, PBM_ICI, PBM_ICI2
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif
     };

  /* Check for write lock */
  if ((ret = pbm_check_write_lock()) != PBM_SUCCESS)
  {
    return ret;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  for (i = 0; i < sizeof(devices)/sizeof(devices[0]); i++)
  {
    if ((cache = pbm_pb_id_to_cache(devices[i])) != NULL)
    {
      if ((cache->status == PBM_STATUS_READY) && (cache->records_in_use > 0))
      {
        /* Need to shift in empty records to clear these pb's */
        for (j = cache->num_of_records; j >= 1; j--)
        {
          rec_id = PBM_GET_FIRST_AVAILABLE;
          write_retval = pbm_record_write(devices[i], &rec_id, 0, 0, NULL, 0,
                                          NULL, NULL);
          if (write_retval != PBM_SUCCESS)
          {
            PBM_MSG_ERR("Failure to delete 0x%x", rec_id, 0, 0);
            ret = PBM_ERROR;
          }
        }
      }
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_write_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_CLEAR_PHONEBOOK

DESCRIPTION
  Clears all entries within a given phonebook

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_phonebook(pbm_device_type pb_id)
{
  int i;
  uint16 rec_id;
  pbm_return_type ret = PBM_SUCCESS;
  pbm_return_type write_retval = PBM_SUCCESS;
  PB_CACHE *cache;

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
  {
    PBM_MSG_ERR("Invalid phonebook %d", pb_id, 0, 0);
    return PBM_ERROR;
  }

  /* Check for write lock */
  if ((ret = pbm_check_write_lock()) != PBM_SUCCESS)
  {
    return ret;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  if ((cache->status == PBM_STATUS_READY) && (cache->records_in_use > 0))
  {
    if (cache->circular)
    {
      /* Need to shift in empty records to clear these pb's */
      for (i = cache->num_of_records; i >= 1; i--)
      {
        rec_id = PBM_GET_FIRST_AVAILABLE;
        write_retval = pbm_record_write(pb_id, &rec_id, 0, 0, NULL, 0,
                                        NULL, NULL);
        if (write_retval != PBM_SUCCESS)
        {
          PBM_MSG_ERR("Failure to delete 0x%x", rec_id, 0, 0);
          ret = PBM_ERROR;
        }
      }
    }
    else
    {
      for (i = 1; i <= cache->num_of_records; i++)
      {
        if (!cache->pb_cache_array[i])
          continue;
        rec_id = pbm_location_to_record_id(pb_id, i);
        write_retval = pbm_record_write(pb_id, &rec_id, 0, 0, NULL, 0,
                                        NULL, NULL);
        if (write_retval != PBM_SUCCESS)
        {
          PBM_MSG_ERR("Failure to delete 0x%x", rec_id, 0, 0);
          ret = PBM_ERROR;
        }
      }
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_write_lock_crit_sect);

  return ret;
}

/* <EJECT> */
/*===========================================================================*
 *              PHONE BOOK MANAGER LEGACY BASED API                          *
 *                                                                           *
 * These functions supply the API for the legacy based requests.             *
 *===========================================================================*/

static pbm_return_type pbm_get_fileinfo_internal(pbm_device_type pb_id,
                                                 pbm_extended_fileinfo_s_type *info)
{
  PB_CACHE *cache;
  pbm_return_type ret;
  uint32 i;
#ifdef FEATURE_MMGSDI
  int j;
#endif

  PBM_CHECK_PTR_RET(info, PBM_ERROR);

  rex_enter_crit_sect(&pbm_crit_sect);


  if (pb_id == PBM_DEFAULT)
  {
    for (i = 0; i < ARR_SIZE(pbm_default_pb_ids); ++i)
    {
      if ((cache = pbm_pb_id_to_cache(pbm_default_pb_ids[i])) == NULL)
      {
        rex_leave_crit_sect(&pbm_crit_sect);
        return PBM_ERROR;
      }
      if (cache->status == PBM_STATUS_READY)
      {
        info->records_used +=  cache->records_in_use;
        info->number_of_records +=  cache->num_of_records;
        info->text_len = MAX(cache->text_length, info->text_len);
        info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
      }
    }
  }
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  //process all the ECC caches
  else if (pb_id == PBM_ECC)
  {
    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids); ++i)
    {
      if ((ret = check_pb_ready(pbm_ecc_pb_ids[i])) == PBM_SUCCESS)
      {
        if ((cache = pbm_pb_id_to_cache(pbm_ecc_pb_ids[i])) != NULL)
        {
          info->records_used +=  cache->records_in_use;
          info->number_of_records +=  cache->num_of_records;
          info->text_len = MAX(cache->text_length, info->text_len);
          info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
        }
      }
    }
  }
#endif
  else
  {
    if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
    {
      rex_leave_crit_sect(&pbm_crit_sect);
      return ret;
    }
    if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    {
      rex_leave_crit_sect(&pbm_crit_sect);
      return PBM_ERROR;
    }

    info->number_of_records = cache->num_of_records;
    info->text_len          = cache->text_length;
    info->records_used      = cache->records_in_use;


    if (pbm_action_requires_sim(pb_id))
    {
      /* assume extension records not available */
      info->number_len = PBM_MAX_NUM_LENGTH_NO_EXT_RECORD;

#ifdef FEATURE_MMGSDI
      /* see if we have extension records available */
      if (cache->extension_cache != NULL)
      {
        /* Now search through the extension records to see if there is
         * any available.
         */
        for (j = cache->extension_cache->num_of_rec; j > 0; j--)
        {
          if (cache->extension_cache->in_use_array[j].index == 0)
          {
            /* there's at least one available */
            info->number_len = PBM_MAX_NUM_LENGTH;
            break;
          }
        }
      }
#else
      PBM_MSG_ERR("Requested info on a SIM phonebook without SIM support.  Results may be unexpected.", 0, 0, 0);
#endif /* FEATURE_MMGSDI */
    }
    else
    {
      /* EFS phonebook */
      info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len */
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, and the text length for the specified phonebook.

  Data returned in proc_func.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_file_info (pbm_device_type pb_id,
                               PBM_FILE_INFO_CB_FUNC proc_func)
{
  pbm_extended_fileinfo_s_type info;
  pbm_return_type ret;

  PBM_CHECK_PTR_RET(proc_func, PBM_ERROR);

  ret = pbm_get_fileinfo_internal(pb_id, &info);

  (* proc_func)(ret,
                pb_id,
                info.records_used,
                info.number_of_records,
                info.text_len);

  return ret;
}

/*===========================================================================
FUNCTION PBM_EXTENDED_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, the text length, and the max phone number length for the specified phonebook.

  Data returned in proc_func.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_extended_file_info (pbm_device_type pb_id,
                                        PBM_EXTENDED_FILE_INFO_CB_FUNC proc_func)
{
  pbm_extended_fileinfo_s_type info;
  pbm_return_type ret;

  PBM_CHECK_PTR_RET(proc_func, PBM_ERROR);

  ret = pbm_get_fileinfo_internal(pb_id, &info);

  (* proc_func)(ret,
                pb_id,
                &info);

  return ret;
}



/*===========================================================================
FUNCTION PBM_GET_NUM_RECS

DESCRIPTION
Gets the total number of records in the given phone book.

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
uint16 pbm_get_num_recs(pbm_device_type pb_id)
{
  uint16 count = 0;
  PB_CACHE *cache;
  unsigned int i;

  rex_enter_crit_sect(&pbm_crit_sect);

  if (pb_id == PBM_DEFAULT)
  {
    for (i = 0; i < ARR_SIZE(pbm_default_pb_ids); ++i)
    {
      if ((cache = pbm_pb_id_to_cache(pbm_default_pb_ids[i])) != NULL)
        if (cache->status == PBM_STATUS_READY)
          count += (uint16) cache->records_in_use;
    }
  }
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  //process all the ECC caches
  else if (pb_id == PBM_ECC)
  {
    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids); ++i)
    {
      if ((cache = pbm_pb_id_to_cache(pbm_ecc_pb_ids[i])) != NULL)
      {
        if (cache->status == PBM_STATUS_READY)
        {
          count += (uint16) cache->records_in_use;
        }
      }
    }
  }
#endif
  else
  {
    if ((cache = pbm_pb_id_to_cache(pb_id)) != NULL)
      if (cache->status == PBM_STATUS_READY)
        count = (uint16) cache->records_in_use;
  }

  rex_leave_crit_sect(&pbm_crit_sect);

  return count;
}

/*===========================================================================
FUNCTION PBM_REG_EVENT_CB

DESCRIPTION
  Allows the user to initialize the ds_ready_fn to be called during init.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_reg_event_cb ( PBM_EVENT_FUNC user_fn )
{
  PBM_CHECK_PTR_RET(user_fn, PBM_ERROR);

#ifdef FEATURE_MMGSDI
  ds_ready_fn = user_fn;
#endif
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_FIND_LOCATION

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_find_location (pbm_device_type pb_id,
                                   int indx,
                                   pbm_record_s_type * data,
                                   PBM_FIND_CB_FUNC proc_func)
{
  PB_CACHE *cache;
  int index = indx;
  pbm_return_type ret;
  pbm_record_s_type *record;

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    return PBM_ERROR;

  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  rex_enter_crit_sect(&pbm_crit_sect);

    do
    {

      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
      {
        break;
      }

      /* The Phonebook cache has no entries or they asked for one beyond the
       * range of this phone book, so fail. */
      if ((cache->records_in_use == 0) || (cache->num_of_records < index))
      {
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if (cache->circular)
        index = PBM_CIRC_CACHE_NTHLOC(cache, indx);

      if ((cache->pb_cache_array[index] != NULL) &&
          (cache->pb_cache_array[index]->fields != NULL) &&
          (cache->pb_cache_array[index]->num_fields > 0))
      {
        if (data)
        {
          pbm_build_record_from_fields(cache->pb_cache_array[index]->fields,
                                       cache->pb_cache_array[index]->num_fields,
                                       indx, data);
        }

        if (proc_func != NULL)
        {
          PBM_MEM_ALLOC(record, sizeof(pbm_record_s_type));
          if (!record)
          {
            (*proc_func) (PBM_ERROR, NULL );
          }
          else
          {
            pbm_build_record_from_fields(cache->pb_cache_array[index]->fields,
              cache->pb_cache_array[index]->num_fields, indx, record);

            (*proc_func) (PBM_SUCCESS, record);

            PBM_MEM_FREEIF(record);
          }
        }

        ret = PBM_SUCCESS;
      }
      else
      {
        ret = PBM_ERROR_NOT_FOUND;
      }

    } while (0); /*lint !e717 */

  /* Send callback if record was not found (for backward compaitbility). */
  if (ret == PBM_ERROR_NOT_FOUND &&
      proc_func != NULL)
  {
    /* Get a buffer so the caller knows what we are failing. */
    PBM_MEM_ALLOC(record, sizeof(pbm_record_s_type));
    if (!record)
    {
      (*proc_func) (PBM_ERROR, NULL );
    }
    else
    {
      memset(record, 0, sizeof(pbm_record_s_type));
      record->index = index;
      (*proc_func) (PBM_ERROR_NOT_FOUND, record);
      PBM_MEM_FREEIF(record);
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_FIND_NAME_NEXT

DESCRIPTION
  Continues reading from linked list to determine if additional calls
  to the SIM are needed.  If so, call pbm_sim_read again.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_find_name_next()
{
  uint16 rec_id;
  uint16 category;
  int item_count;
  pbm_return_type ret;
  PB_CACHE *pb_cache;
  pbm_record_s_type *record = NULL;
  int index;

  pbm_field_s_type  *fields_buf = NULL;
  uint32             fields_size;


  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  if (pbm_find_name_user_cb == NULL)
  {
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  ret = pbm_enum_next_rec_id(&rec_id);

  switch (ret)
  {
    case PBM_SUCCESS:
    {
      fields_size = (uint32) pbm_calculate_fields_size_from_id(rec_id);
      PBM_MEM_ALLOC(fields_buf, fields_size);

      if (fields_buf == NULL)
      {
        (*pbm_find_name_user_cb) (PBM_ERROR_MEM_FULL, NULL);
        ret = PBM_ERROR_MEM_FULL;
        break;
      }

      ret = pbm_record_read(rec_id, &category, &item_count,
                            (uint8*)fields_buf, fields_size);
      if (ret != PBM_SUCCESS)
      {
        (*pbm_find_name_user_cb) (PBM_ERROR, NULL);
        ret = PBM_ERROR;
        break;
      }

      PBM_MEM_ALLOC(record, sizeof(pbm_record_s_type));
      if (!record)
      {
        (*pbm_find_name_user_cb) (PBM_ERROR, NULL);
        ret = PBM_ERROR;
        break;
      }

      index = pbm_record_id_to_location(rec_id);
      pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
      if (pb_cache == NULL)
      {
        PBM_MSG_ERR("pb_cache was NULL for record id 0x%x", rec_id, 0, 0);
        (*pbm_find_name_user_cb) (PBM_ERROR, NULL);
        rex_leave_crit_sect(&pbm_crit_sect);
        rex_leave_crit_sect(&pbm_read_lock_crit_sect);
        ret = PBM_ERROR;
        break;
      }

      /* Fill in the fields into the record structure. */
      pbm_build_record_from_fields(fields_buf, item_count, index, record);

      /* Call the call back. */
      (*pbm_find_name_user_cb) (PBM_SUCCESS, record);

      break;
    }
    case PBM_EOF:
    case PBM_ERROR_NOT_FOUND:
    default:
    {
      (*pbm_find_name_user_cb)(ret, NULL);
      ret = PBM_SUCCESS;
      break;
    }
  }

  PBM_MEM_FREEIF(record);
  PBM_MEM_FREEIF(fields_buf);

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_FIND_NAME

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "name".  If no match is found PBM_ERROR_NOT_FOUND is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_find_name (pbm_device_type pb_id,
                            char *name,
                            pbm_record_s_type * data,
                            PBM_FIND_CB_FUNC proc_func)

{
  uint16 rec_id;
  uint16 category;
  int item_count;
  pbm_return_type ret = PBM_SUCCESS;
  PB_CACHE *pb_cache;
  int index;
  uint16 cat;
  uint16 length;

  pbm_field_s_type  *fields_buf = NULL;
  uint32             fields_size;

  PBM_CHECK_PTR_RET(name, PBM_ERROR);

  if ((ret = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret;

  rex_enter_crit_sect(&pbm_crit_sect);

  do
  {
    if (data == NULL)
    {
      if (proc_func == NULL)
      {
        ret = PBM_ERROR;
        break;
      }
      else
        pbm_find_name_user_cb = proc_func;
    }

    length = (uint16) strlen(name);

    if (pb_id == PBM_SDL)
      cat = (uint16)PBM_CAT_SDL;
    else
      cat = (uint16)PBM_CAT_NONE;

    if (length == 0)
    {
      ret = pbm_enum_rec_init(pb_id, cat, PBM_FIELD_NONE,
                              NULL, 0, 0);
    }
    else
    {
      uint16 wstr_name[PBM_TEXT_SIZE_CHARS];

      if (!pbmconvert_pbm8_to_ucs2((uint8 *) name, length,
                                 wstr_name, ARR_SIZE(wstr_name)))
      {
        PBM_MSG_ERR("Could not convert name to WSTR: %d %d", length,
                    ARR_SIZE(wstr_name), 0);
      }

      ret = pbm_enum_rec_init(pb_id, cat, PBM_FIELD_NAME,
                              (void *)wstr_name, (uint16)(sizeof(uint16)*(length+1)),
                              PBM_CASE_INSENSITIVE |
                              PBM_SEARCHTYPE_STARTSWITH |
                              PBM_STRTYPE_UCS2);
    }
    if (ret != PBM_SUCCESS)
      break;

    if (proc_func != NULL) {
      /* Asynchronous call, use callback function */
      ret = pbm_find_name_next();
    } else {
      /* Return data immediately */
      ret = pbm_enum_next_rec_id(&rec_id);
      if (ret == PBM_SUCCESS) {
        fields_size = (uint32) pbm_calculate_fields_size_from_id(rec_id);
        PBM_MEM_ALLOC(fields_buf, fields_size);

        if (fields_buf == NULL)
        {
          (*pbm_find_name_user_cb) (PBM_ERROR_MEM_FULL, NULL);
          ret = PBM_ERROR_MEM_FULL;
          break;
        }

        ret = pbm_record_read(rec_id, &category, &item_count,
                              (uint8*)fields_buf, fields_size);
        if (ret == PBM_SUCCESS)
        {
          index = pbm_record_id_to_location(rec_id);
          /* In the case of a circular cache, the index does not
           * necessarily correspond with the "current" index.  Check
           * if its a circular cache, and then update index accordingly
           * if it is.
           */
          if (pb_id != PBM_DEFAULT) // PBM_DEFAULT is never circular.
          {
            /* Not PBM_DEFAULT, so we can get the cache itself. */
            pb_cache = pbm_pb_id_to_cache(pb_id);
            if (pb_cache == NULL)
            {
              PBM_MSG_ERR("pb_cache was NULL for phonebook id 0x%x", pb_id, 0, 0);
              ret = PBM_ERROR;
              break;
            }
          }

          /* Fill in the fields into the record structure. */
          pbm_build_record_from_fields(fields_buf, item_count,
                                          index, data);
        }
        else
        {
          PBM_MSG_ERR("pbm_record_read failed with return %d", ret, 0, 0);
          ret = PBM_ERROR;
          break;
        }
      }
    }
  } while (0); /*lint !e717 */

  PBM_MEM_FREEIF(fields_buf);

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return ret;
}

/*===========================================================================
FUNCTION PBM_FIND_NUMBER

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_find_number (pbm_device_type pb_id,
                              const byte *number, int loc_number_len,
                              pbm_record_s_type * record,
                              PBM_FIND_CB_FUNC proc_func)

{
  boolean (*match_fn)(const byte *, byte, const byte *, byte);
  PB_CACHE *cache;
  pbm_field_s_type      *field_ptr;
  pbm_field_s_type      *sfield;
  pbm_addr_cache_s_type *record_ptr;
  int i, j;
  int index;
  pbm_return_type ret_val;
  int records_in_use = 0;
  pbm_device_type *l_pb_ids;
  uint8 l_pb_ids_size = 0;


  PBM_CHECK_PTR_RET(number, PBM_ERROR);

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
  {
    return PBM_ERROR;
  }

  if ((ret_val = pbm_check_read_lock()) != PBM_SUCCESS)
    return ret_val;

  rex_enter_crit_sect(&pbm_crit_sect);

  if ((ret_val = check_pb_ready(pb_id)) != PBM_SUCCESS)
  {
    rex_leave_crit_sect(&pbm_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return ret_val;
  }

  if ((pb_id == PBM_FDN) || (pb_id == PBM_FDN2))
  {
    match_fn = pbm_fdn_number_match;
  }
  else
  {
    match_fn = pbm_number_match;
  }

#ifdef FEATURE_ECC_HANDLING_AT_PBM
  //if the pb_id is PBM_ECC then we should process the three caches PBM_ECC,PBM_ECC2 &PBM_ECC_OTHER
  if (pb_id == PBM_ECC)
  {

    l_pb_ids = pbm_ecc_pb_ids;
    l_pb_ids_size = ARR_SIZE(pbm_ecc_pb_ids);
  }
  else
#endif
  {
    l_pb_ids = &pb_id;
    l_pb_ids_size = 1;
  }

  for (j = 0; j<l_pb_ids_size; j++)
  {
    cache = pbm_pb_id_to_cache(l_pb_ids[j]);
    records_in_use += cache->records_in_use;
  }

  /* The Phonebook cache has no entries so fail. */
  if (records_in_use == 0)
  {
    rex_leave_crit_sect(&pbm_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR_NOT_FOUND;
  }




  for (j = 0; j<l_pb_ids_size; j++)
  {
    cache = pbm_pb_id_to_cache(l_pb_ids[j]);

    for (i = 1; i <= cache->num_of_records; i++)
    {
      if ((record_ptr = cache->pb_cache_array[i]) != NULL)
      {

        field_ptr = record_ptr->fields;

        for (j = 0; j < record_ptr->num_fields; j++)
        {
          pbm_complex_result_s_type *result = NULL;
          ret_val = pbm_enum_complex_init(&field_ptr[j], NULL, &result);
          if (ret_val != PBM_SUCCESS)
          {
            PBM_MSG_ERR("Failed complex field enumeration %d for %d",
            ret_val, record_ptr->record_id, 0);
            rex_leave_crit_sect(&pbm_crit_sect);
            rex_leave_crit_sect(&pbm_read_lock_crit_sect);
            return PBM_ERROR;
          }

          while ((sfield = pbm_enum_complex_next(result)) != NULL)
          {

            if ((sfield->field_type == PBM_FT_PHONE) &&
              ((*match_fn)((byte *) pbm_get_field_data_ptr(sfield,0),
                                                 (byte) sfield->data_len - 1,
                                                                (byte *) number,      (byte) loc_number_len)))
            {
              index = pbm_record_id_to_location(record_ptr->record_id);
              if (cache->circular)
              index = PBM_CIRC_CACHE_LOCTOINDEX(cache, index);

              if (record != NULL)
              {
                /* This builds a record from the fields, however, if there are
                * multiple numbers in the cache, the number returned in record is
                * the highest number field.  This will not affect current code,
                * and this is for the legacy UI, so this caveot should not be
                * an issue.
                */
                pbm_build_record_from_fields(field_ptr, record_ptr->num_fields,
                index, record);
              }

              /* Clients expect asynchrounous call to have memory of its own.
              * The input record pointer can be NULL */
              if (proc_func != NULL)
              {
                pbm_record_s_type *async_record;
                PBM_MEM_ALLOC(async_record, sizeof(pbm_record_s_type));
                if (!async_record)
                {
                  (*proc_func) (PBM_ERROR, NULL);
                }
                else
                {
                /* This builds a record from the fields, however, if there are
                * multiple numbers in the cache, the number returned in record is
                * the highest number field.  This will not affect current code,
                * and this is for the legacy UI, so this caveot should not be
                * an issue.
                */
                pbm_build_record_from_fields(field_ptr, record_ptr->num_fields,
                index, async_record);

                (*proc_func) (PBM_SUCCESS, async_record);

                PBM_MEM_FREEIF(async_record);
                }
              }
              pbm_enum_complex_free(result);
              rex_leave_crit_sect(&pbm_crit_sect);
              rex_leave_crit_sect(&pbm_read_lock_crit_sect);
              return PBM_SUCCESS;
            } /* if matching phone number */
          } /* for each subfield */
          pbm_enum_complex_free(result);
        } /* for each field */
      } /* if record not NULL */
    } /* for each record */
  }/*for each pb_id in l_pb_ids */

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return PBM_ERROR_NOT_FOUND;
}

/*===========================================================================
FUNCTION PBM_WRITE

DESCRIPTION
  Writes the given record to the phone book specified.  If proc_func is not NULL
  that is called when the write is complete.

  Note: Now this is just a wrapper for pbm_record_write() that preserves the legacy
  interface of one callback function with one argument to it.

PARAMETERS:
  record  if record.index == -1, write to the first empty record
          if record.number is NULL, delete entry record.index

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_write_compat_cb_translator (pbm_writecb_data_s_type *cb_data)
{
  PBM_WRITE_COMPAT_CB_FUNC pfCompatCB;

  PBM_CHECK_PTR_RET(cb_data, VOID);

  /*lint -e{611} suspicious cast from data to function*/
  pfCompatCB = (PBM_WRITE_COMPAT_CB_FUNC) cb_data->user_data;

  if (pfCompatCB != NULL)
  {
    pfCompatCB(cb_data->ret);
  }
}

/*===========================================================================
FUNCTION PBM_WRITE

DESCRIPTION
  Writes the given record to the phone book specified.  If proc_func is not NULL
  that is called when the write is complete.

  Note: Now this is just a wrapper for pbm_record_write() that preserves the legacy
  interface of one callback function with one argument to it.

PARAMETERS:
  record  if record.index == -1, write to the first empty record
          if record.number is NULL, delete entry record.index

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write ( pbm_device_type pb_id,
                         pbm_record_s_type * record,
                         PBM_WRITE_COMPAT_CB_FUNC proc_func)
{
  pbm_addr_cache_s_type *cache_fields = NULL;
  pbm_return_type ret_val;
  uint16 rec_id;
  uint16 location;
  PB_CACHE *cache;
  boolean deletion = FALSE;

  PBM_CHECK_PTR_RET(record, PBM_ERROR);

  /* Check for write lock */
  if ((ret_val = pbm_check_write_lock()) != PBM_SUCCESS)
  {
    return ret_val;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  cache_fields = pbm_build_fields_from_record(pb_id, record);

  if (cache_fields == NULL)
  {
    rex_leave_crit_sect(&pbm_crit_sect);
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    return PBM_ERROR;
  }

  if (record->number[0] == '\0' && record->text[0] == '\0')
    deletion = TRUE;

  rec_id = cache_fields->record_id;
  ret_val = (pbm_return_type) pbm_record_write(
                                pb_id,
                                &rec_id,
                                cache_fields->category,
                                deletion ? 0 : cache_fields->num_fields,
                                deletion ? NULL : (uint8*)cache_fields->fields,
                                deletion ? 0 :
                                    pbm_calculate_size_from_fields(
                                                     cache_fields->fields,
                                                     cache_fields->num_fields),
                                pbm_write_compat_cb_translator,
                                (void*)proc_func);  //lint !e611 suspicious cast

  if (ret_val == PBM_SUCCESS && record->index == -1)
  {
    cache = pbm_pb_id_to_cache(pb_id);
    location = pbm_record_id_to_location(rec_id);

    if (cache)
    {
      record->index = location;
    }
    else
    {
      PBM_MSG_ERR("cache was NULL", 0, 0, 0);
      ret_val = PBM_ERROR;
    }
  }

  PBM_MEM_FREEIF(cache_fields->fields); /* pbm_record_write() copied the fields */
  PBM_MEM_FREEIF(cache_fields);

  rex_leave_crit_sect(&pbm_crit_sect);
  rex_leave_crit_sect(&pbm_write_lock_crit_sect);

  return ret_val;
}

/*===========================================================================
FUNCTION PBM_WRITE_LOCK

DESCRIPTION
  This function is used to lock further writes to PBM. Returns error if unable
  to acquire a non-blocking lock and block if trying to acquire a blocking lock.
  If lock type is blocking, other tasks are blocked till the task who owns the
  lock unlocks pbm write. If the lock type is unblocking, writes from other tasks
  will result in an error.

  The lock operation is complete only after the asynchronous callback function
  is called. This is to make sure that there are no pending commands in the PBM
  command queue which could affect the PBM cache later.

  Note that a read lock always implies a write lock as well.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write_lock(pbm_lock_type_e_type type,
                               void *user_data,
                               PBM_WRITE_CB_FUNC func)

{
  pbm_write_lock_s_type *cmd;
  boolean read_lock = FALSE;
  rex_tcb_type *self = rex_self();

  if (type == PBM_LOCK_NOT_INIT || !func)
    return PBM_ERROR;

  /* If lock is already locked in blocking way by someone else, we will
   * block at line below irrespective of the type of lock we are trying
   * to acquire */
  if (type == PBM_RW_LOCK_BLOCKING ||
      type == PBM_RW_LOCK_NON_BLOCKING)
  {
    read_lock = TRUE;
    rex_enter_crit_sect(&pbm_read_lock_crit_sect);
  }
  rex_enter_crit_sect(&pbm_write_lock_crit_sect);

  /* Check if we can allow this lock operation */
  if (g_lock.lock_owner)
  {
    /* Somebody already owns the lock */
    if (g_lock.lock_owner != self)
    {
      /* Not locked by me */
      PBM_MSG_HIGH("Lock is already acquired by 0x%x", g_lock.lock_owner, 0, 0);
      rex_leave_crit_sect(&pbm_write_lock_crit_sect);
      if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);
      return PBM_ERROR_LOCKED;
    }
    else
    {
      /* Locked by me */
      if (g_lock.lock_type != type)
      {
        /* Trying to change lock type */
        rex_leave_crit_sect(&pbm_write_lock_crit_sect);
        if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);
        PBM_MSG_HIGH("Tring to change lock type %d to %d",
                 g_lock.lock_type, type, 0);
        return PBM_ERROR_LOCKED;
      }
      else
      {
        /* Trying to lock again with same type, not a normal thing to do but not
         * an error either */
        pbm_writecb_data_s_type cb_data;

        PBM_MSG_HIGH("Trying to acquire lock twice", 0, 0, 0);
        rex_leave_crit_sect(&pbm_write_lock_crit_sect);
        if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);

        /* since we're going to return success since they have already locked it,
          we need to call their callback */
        cb_data.ret = PBM_SUCCESS;
        cb_data.rec_id = PBM_GET_FIRST_AVAILABLE;
        cb_data.user_data = user_data;
        func(&cb_data);

        return PBM_SUCCESS;
      }
    }
  }
  else
  {
    /* Not locked by anyone */
    if (g_lock.lock_type != PBM_LOCK_NOT_INIT)
    {
      PBM_MSG_ERR("Lock variables are inconsistent", 0, 0, 0);
      rex_leave_crit_sect(&pbm_write_lock_crit_sect);
      if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);
      return PBM_ERROR;
    }
  }

  /* If we reach here, the write lock is allowed. */
  /* Get some memory for the command. */
  cmd = (pbm_write_lock_s_type *)pbm_cmd_alloc(sizeof(pbm_write_lock_s_type));
  if (!cmd)
  {
    PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR_MEM_FULL;
  }

  /* Set our globals as well as the details of the command. */
  g_lock.lock_owner = self;
  g_lock.lock_type = type;
  g_lock.done = FALSE;

  cmd->cmd_type   = PBM_CMD_WRITE_LOCK;
  cmd->lock_type  = type;
  cmd->proc_func  = func;
  cmd->user_data  = user_data;

  /* Write the command to the pbm_task. */
  pbm_client_cmd_q_put(PBM_CMD_PTR(cmd));

  /* If the type is non-blocking, free the critical section. */
  /* Blocking requests will keep all others waiting. */
  if (type == PBM_W_LOCK_NON_BLOCKING ||
      type == PBM_RW_LOCK_NON_BLOCKING)
  {
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    if (read_lock) rex_leave_crit_sect(&pbm_read_lock_crit_sect);
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_WRITE_UNLOCK

DESCRIPTION
  This function is used to unlock PBM writes.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write_unlock(void)
{
  pbm_lock_type_e_type lock_type;
  pbm_write_lock_s_type *cmd;

  rex_enter_crit_sect(&pbm_read_lock_crit_sect);
  rex_enter_crit_sect(&pbm_write_lock_crit_sect);

  if (g_lock.lock_owner != rex_self() || !g_lock.done)
  {
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    PBM_MSG_HIGH("Invalid unlock attempt", 0, 0, 0);
    return PBM_ERROR;
  }

  /* In case of blocking locks, we have to leave the lock */
  if (g_lock.lock_type == PBM_RW_LOCK_BLOCKING ||
      g_lock.lock_type == PBM_W_LOCK_BLOCKING)
  {
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    if (g_lock.lock_type == PBM_RW_LOCK_BLOCKING)
      rex_leave_crit_sect(&pbm_read_lock_crit_sect);
  }

  lock_type = g_lock.lock_type;

  g_lock.lock_owner = NULL;
  g_lock.lock_type = PBM_LOCK_NOT_INIT;
  g_lock.done = FALSE;

  /* Allocate PBM command */
  cmd = (pbm_write_lock_s_type *)pbm_cmd_alloc(sizeof(pbm_write_lock_s_type));
  if (!cmd)
  {
    PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR_MEM_FULL;
  }

  /* The command is used only to make sure that notifications happen from PBM
   * task. Otherwise unlock notifications can reach clients before lock
   * notifications */
  cmd->cmd_type   = PBM_CMD_WRITE_UNLOCK;
  cmd->lock_type  = lock_type;
  cmd->proc_func  = NULL;
  cmd->user_data  = NULL;

  /* Write command to pbm_task. */
  pbm_client_cmd_q_put(PBM_CMD_PTR(cmd));

  rex_leave_crit_sect(&pbm_write_lock_crit_sect);
  rex_leave_crit_sect(&pbm_read_lock_crit_sect);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_GET_FIELD_INFO_COUNT

DESCRIPTION
  This function returns the number of PBM fields supported by the specified
  PBM phonebook or category.

PROTOTYPE:
  pbm_return_type pbm_get_field_info_count(pbm_device_type pb_id, pbm_cat_e_type cat,
                                           int *num)
PARAMETERS:
  pb_id       [in]: Phonebook device.
  cat         [in]: Category for which information is requested.
  num        [out]: Number of fields supported by this category.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_field_info_count(pbm_device_type pb_id, pbm_cat_e_type cat,
                                         int *num)
{
  int field_group;

  PBM_CHECK_PTR_RET(num, PBM_ERROR);

  if (cat != PBM_CAT_NONE && pb_id != pbm_cat_to_phonebook(cat))
  {
    PBM_MSG_ERR("Category %d and phonebook %d does not match", cat, pb_id, 0);
    return PBM_ERROR;
  }

  field_group = pbm_pb_id_to_fieldgroup(pb_id);
  if (field_group < 0)
  {
    PBM_MSG_ERR("Invalid group: phonebook %d and category %d", pb_id, cat, 0);
    return PBM_ERROR;
  }

  *num = g_field_info[field_group].num_fields;

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_GET_FIELD_INFO

DESCRIPTION
  This function returns field information associated with each PBM field
  supported by the specified category

PROTOTYPE:
  pbm_return_type pbm_get_field_info(pbm_device_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size)
PARAMETERS:
  pb_id       [in]: The phonebook device for which information is requested
  cat         [in]: Category for which information is requested.
  pf         [out]: Array of field info which is filled up by PBM
  size        [in]: Size of pf array in bytes.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_field_info(pbm_device_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size)
{
  int field_group;

  PBM_CHECK_PTR_RET(pf, PBM_ERROR);

  if (cat != PBM_CAT_NONE && pb_id != pbm_cat_to_phonebook(cat))
  {
    PBM_MSG_ERR("Category %d and phonebook %d does not match", cat, pb_id, 0);
    return PBM_ERROR;
  }

  field_group = pbm_pb_id_to_fieldgroup(pb_id);
  if (field_group < 0)
  {
    PBM_MSG_ERR("Invalid group: phonebook %d and category %d", pb_id, cat, 0);
    return PBM_ERROR;
  }

  memcpy(pf, (void *)g_field_info[field_group].info,
         (uint16) MIN(size, g_field_info[field_group].num_fields*
                      (uint16)sizeof(pbm_field_info_s_type)));

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION compare_field_types

DESCRIPTION
  This function compares two field types to determine if they are
  the same or not.  If PBM accepts a UCS2 string, it can also accept
  a PBM8 string.

PROTOTYPE:
  boolean compare_field_types(pbm_field_type_e_type type_provided,
                              pbm_field_type_e_type type_accepted)

PARAMETERS:
  type_provided [in]: The type of the record being checked.
  type_accepted [in]: The type PBM will accepts.

RETURN VALUE:
  TRUE:  The types are the same or the accepted type is UCS2 and
         the provided type is either UCS2 or STRING.
  FALSE: There are differences.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static boolean compare_field_types(pbm_field_type_e_type type_provided,
                            pbm_field_type_e_type type_accepted)
{
  switch (type_accepted)
  {
    case PBM_FT_UCS2:
      return ((type_provided == PBM_FT_STRING) ||
              (type_provided == PBM_FT_UCS2));
    default:
      break;
  }

  return (type_provided == type_accepted);
}

/*===========================================================================
FUNCTION PBM_RECORD_VALIDATE

DESCRIPTION
  This function checks whether a given record is valid.

  The checks include the following.
    - Check whether fields that are required are included. For ADN or FDN phonebooks
      there should be a non-NULL number string. For other phonebooks there should be
      a non-NULL number OR non-NULL name string.
    - Check whether the maximum number of a particular field or the maximum size of
      a particular field or the maximum size of the entire record is exceeded.
    - Check whether all the fields are supported for this phonebook and whether the
      types of fields match the field id.

PROTOTYPE:
  pbm_return_type pbm_record_validate(pbm_device_type pb_id, uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *pfields, int num_fields);
PARAMETERS:
  pb_id       [in]: Phonebook device
  rec_id      [in]: Record id.
  cat         [in]: Category of the record
  pfields     [in]: buffer with Array of fields containing data (same struct as
                    pbm_record_read() and pbm_record_write()
  num_fields  [in]: Number of fields.

RETURN VALUE:
  PBM_ERROR_INDEX_INVALID   : If the record contains an INDEX field and if it is
                              invalid or the record id is not valid.
  PBM_ERROR_TEXT_TOO_LONG   : If name is too long.
  PBM_ERROR_NUMBER_TOO_LONG : If number is too long.
  PBM_ERROR_NOT_READY       : If the phone book requested has not been initialized.
  PBM_ERROR_NO_SIM          : If the corresponding UIM is not present.
  PBM_ERROR_NOT_AVAILABLE   : If the corresponding UIM file is not available
  PBM_ERROR_RESTRICTED      : If UIM file is not available due to restriction
  PBM_ERROR_INVALID_REC     : If the record is not valid for this phonebook due
                              to other reasons.
  PBM_SUCCESS               : If record is valid


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_validate(pbm_device_type pb_id,
                                    uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields)
{
  int group;
  int i;
  int j;
  int counts[PBM_MAX_NUM_FIELDS];
  boolean record_required_fields[PBM_MAX_REQUIRED_FIELDS];
  pbm_device_type old_pb_id;
  pbm_addr_cache_s_type *record = NULL;
  pbm_field_s_type *ptyped_fields = (pbm_field_s_type *)data_buf;  //lint !e826 unusual cast
  uint16 pbm_cache_rec_id = rec_id;

  if (rec_id != PBM_GET_FIRST_AVAILABLE)
  {
    PB_CACHE *pb_cache = NULL;
    pb_cache = pbm_pb_id_to_cache(pb_id);

    if (pb_cache->circular == TRUE)
    {
      //Get the true cache rec_id for circular caches
      uint16 location = pbm_record_id_to_location(rec_id);

      location = PBM_CIRC_CACHE_NTHLOC(pb_cache, location);
      pbm_cache_rec_id = pbm_location_to_record_id(pb_id,location);
    }
  }

  memset(counts, 0, sizeof(counts));


  /* Check if pfields is empty */
  if (num_fields)
  {
    int calculated_size;

    if (!ptyped_fields)
    {
      PBM_MSG_ERR("Invalid Record (have %d fields, null data)", num_fields, 0, 0);
      return PBM_ERROR_INVALID_REC;
    }

    calculated_size = pbm_calculate_size_from_fields((pbm_field_s_type*)data_buf,num_fields);  //lint !e826 unusual cast

    if ((data_buf_size < calculated_size) || (calculated_size > PBM_MAX_RECORD_SIZE))
    {
      if (calculated_size > PBM_MAX_RECORD_SIZE)
      {
        PBM_MSG_ERR("Invalid Record (have %d fields, calculated size %d beyond max needed %d)",
                    num_fields, calculated_size, PBM_MAX_RECORD_SIZE );
      }
      else
      {
        PBM_MSG_ERR("Invalid Record (have %d fields, data given %d needed %d)",
                    num_fields, data_buf_size, calculated_size );
      }
      return PBM_ERROR_INVALID_REC;
    }
    else if (calculated_size > 1024)
    {
      PBM_MSG_HIGH("Record of unusual size.  %d", calculated_size, 0, 0);
    }
  }
  else
  {
    /* Deletion */
    if (ptyped_fields)
    {
      PBM_MSG_ERR("Invalid Record (deletion, non-null data)", 0, 0, 0);
      return PBM_ERROR_INVALID_REC;
    }
    else
      return PBM_SUCCESS; /* we are done */
  }

  if ((cat == PBM_CAT_ERROR) || (cat != PBM_CAT_NONE && pb_id != pbm_cat_to_phonebook(cat)))
  {
    PBM_MSG_ERR("Category %d and phonebook %d does not match", cat, pb_id, 0);
    return PBM_ERROR_INVALID_REC;
  }

  group = pbm_pb_id_to_fieldgroup(pb_id);
  if (group == INVALID_GROUP)
    return PBM_ERROR_NOT_SUPPORTED;

  // If there are no errors, check the category/record ID pair.
  if (pbm_cache_rec_id != PBM_GET_FIRST_AVAILABLE)
  {
    old_pb_id = pbm_record_id_to_phonebook(pbm_cache_rec_id);

    if (pb_id != old_pb_id)
    {
      PBM_MSG_ERR("Can not move record %d from one phonebook %d to another %d",
                  pbm_cache_rec_id, old_pb_id, pb_id);
      return PBM_ERROR_INVALID_REC;
    }
  }

  /* Validate phone unique ID field as follows -
    * If adding a new record: check that the PH_UID field does not exist.
    * If updating an existing record: check that either PH_UID field does not exist,
    * or that it PH_UID exists and matches exactly what is already in the cache.
    *
    * Note that later checks will verify that the PH_UID field exists only once.
    */

  do
  {
    /* Only for EFS phone book. */
    if (pb_id != PBM_EFS)
    {
      break;
    }

    /* Locate PH_UID field in update field list. */
    for (i = 0; i < num_fields; i++)
    {
      if (ptyped_fields[i].field_id == PBM_FIELD_PH_UID)
      {
        break;
      }
    }

    /* If adding a new record... */
    if (pbm_cache_rec_id == PBM_GET_FIRST_AVAILABLE)
    {
      /* Not permitted to have the PH_UID field set. */
      if (i != num_fields)
      {
        PBM_MSG_ERR("Have PH_UID field in GET_FIRST_AVAILABLE %d %d",
                    i, num_fields, 0);
        return PBM_ERROR_INVALID_REC;
      }
      break; /* All OK so far, done with these checks ... */
    }

    /* If updating an existing record... */
    if (i == num_fields)
    {
      /* Did not find the PH_UID field. */
      break; /* All OK so far, done with these checks... */
    }

    /* Check that the PH_UID field matches what is in cache. */
    record = pbm_cache_record_get(pbm_cache_rec_id);
    PBM_CHECK_PTR_RET(record, PBM_ERROR)

    /* Locate unique ID field in cache record. */
    for (j = 0; j < record->num_fields; j++)
    {
      if (record->fields[j].field_id == PBM_FIELD_PH_UID)
      {
        void *data_ptr1;
        void *data_ptr2;

        /* Found, compare against the update field. */

        if (ptyped_fields[i].data_len != record->fields[j].data_len)
        {
          (void) pbm_cache_record_free(record);
          return PBM_ERROR_UNIQUE_IDS_CHANGED;
        }

        data_ptr1 = pbm_get_field_data_ptr(ptyped_fields,i);
        data_ptr2 = pbm_get_field_data_ptr(record->fields,j);

        if (!data_ptr1 || !data_ptr2)
        {
          PBM_MSG_ERR("Null ptr 0x%x 0x%x", data_ptr1, data_ptr2, 0);
          (void) pbm_cache_record_free(record);
          return PBM_ERROR;
        }

        if (memcmp(data_ptr1, data_ptr2, ptyped_fields[i].data_len) != 0)
        {
          (void) pbm_cache_record_free(record);
          return PBM_ERROR_UNIQUE_IDS_CHANGED;
        }
        break;
      }
    }

    if (j == record->num_fields)
    {
      PBM_MSG_ERR("Failed locating PH_UID field for cache record %d",
          pbm_cache_rec_id, 0, 0);
    }

    (void) pbm_cache_record_free(record);
  }
  while (FALSE); /*lint !e717 */

  /* If there are still no errors, check that the field can exist
   * for this category and that there are no missing required fields.
   */

  /* Build the list of required fields. */
  for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
  {
    if (g_required_fields[group][j] != 0)
      record_required_fields[j] = TRUE;
    else
      record_required_fields[j] = FALSE;
  }

  /* For each field... */
  for (i = 0; i < num_fields; i++)
  {
    /* Check to see if this field is a required field.  If so,
     * mark it as no longer required since we have one. */
    for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
    {
      if (g_required_fields[group][j] == ptyped_fields[i].field_id)
      {
        record_required_fields[j] = FALSE;
        break;
      }
    }

    /* Now check to make sure the field type and the number
     * of occurrances match what we need. */
    for (j = 0; j < g_field_info[group].num_fields; j++)
    {
      if (ptyped_fields[i].field_id == g_field_info[group].info[j].field_id)
      {
        counts[j]++;
        if ((!compare_field_types(ptyped_fields[i].field_type,
                                 g_field_info[group].info[j].field_type)) ||
            (((g_field_info[group].info[j].num_items > 0) &&
              (counts[j] > g_field_info[group].info[j].num_items))))
        {
          PBM_MSG_ERR("Bad Field Value: ID=0x%X Type=0x%x Count=%d", ptyped_fields[i].field_id,
              ptyped_fields[i].field_type, counts[j] );
          PBM_MSG_ERR("for record %d, phonebook %d", pbm_cache_rec_id, pb_id, 0);
          return PBM_ERROR_INVALID_REC;
        }

        if (ptyped_fields[i].data_len > g_field_info[group].info[j].max_length)
        {
          if (ptyped_fields[i].field_type == PBM_FT_PHONE)
            return PBM_ERROR_NUMBER_TOO_LONG;
          else
            return PBM_ERROR_TEXT_TOO_LONG;
        }

        /* A phone number can only be size PBM_NUM_SIZE if
           '+' dial character is the first digit*/
        if ((ptyped_fields[i].field_type == PBM_FT_PHONE) &&
            (ptyped_fields[i].data_len == g_field_info[group].info[j].max_length) &&
            (((char*)(pbm_get_field_data_ptr(ptyped_fields, i)))[0] != '+'))
        {
          return PBM_ERROR_NUMBER_TOO_LONG;
        }

        break;
      }
    }

    if (j == g_field_info[group].num_fields)
    {
      PBM_MSG_ERR("Bad Field ID: ID=0x%X for record %d, phonebook %d",
                  ptyped_fields[i].field_id, pbm_cache_rec_id, pb_id);
      return PBM_ERROR_INVALID_REC;
    }

    if (ptyped_fields[i].field_id == PBM_FIELD_INDEX)
    {
      uint16 index;
      void *data_ptr = pbm_get_field_data_ptr(ptyped_fields,i);

      PBM_CHECK_PTR_RET(data_ptr, PBM_ERROR);

      index = *(uint16 *)(data_ptr);
      if (index != pbm_record_id_to_location(pbm_cache_rec_id))
      {
        PBM_MSG_ERR("Index within record %d does not match record id %d, phonebook %d",
                    index, pbm_cache_rec_id, pb_id);
        return PBM_ERROR_INDEX_INVALID;
      }
    }

  } /* for each field */

  for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
  {
    if (record_required_fields[j])
    {
      PBM_MSG_ERR("Missing required field ID 0x%x, phonebook %d",
                  g_field_info[group].info[j].field_id, pb_id, 0 );
      return PBM_ERROR_INVALID_REC;
    }
  }

  /* OK looks like this record is clean */
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_NOTIFY_REGISTER

DESCRIPTION
  This function is used to register a callback function that is used to notify
  PBM events. The user data given as input will be passed on when the
  notification callback is called.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_unregister
===========================================================================*/
pbm_return_type pbm_notify_register(PBM_NOTIFY_FUNC func, void *user_data)
{
  pbm_notify_node_s_type *node;
  pbm_notify_node_s_type *curr;

  PBM_CHECK_PTR_RET(func, PBM_ERROR);

  /* Allocate notification node */
  PBM_MEM_ALLOC(node, sizeof(pbm_notify_node_s_type));
  PBM_CHECK_PTR_RET(node, PBM_ERROR);

  rex_enter_crit_sect(&pbm_crit_sect);

  /* Return error if func/user_data pair is already registered */
  curr = pbm_notify_list;
  while (curr)
  {
    if ((curr->notify_func == func) && (curr->user_data == user_data))
    {
      PBM_MSG_ERR("Callback %p:0x%x already registered", func, user_data, 0);
      PBM_MEM_FREEIF(node);
      rex_leave_crit_sect(&pbm_crit_sect);
      return PBM_ERROR;
    }
    curr = curr->next;
  }

  /* Add node to the head of notification list */
  node->next = pbm_notify_list;
  node->notify_func = func;
  node->user_data = user_data;

  pbm_notify_list = node;

  rex_leave_crit_sect(&pbm_crit_sect);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_NOTIFY_UNREGISTER

DESCRIPTION
  This function is used to cancel the notification function registered with PBM.

  The function pointer and user data provided as input should match the
  registered function pointer and user data.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_register
===========================================================================*/
pbm_return_type pbm_notify_unregister(PBM_NOTIFY_FUNC func, void *user_data)
{
  pbm_notify_node_s_type *curr;
  pbm_notify_node_s_type *prev;
  pbm_return_type ret = PBM_ERROR;

  PBM_CHECK_PTR_RET(func, PBM_ERROR);

  rex_enter_crit_sect(&pbm_crit_sect);

  if (!pbm_notify_list)
  {
    PBM_MSG_ERR("Notification list is empty", 0, 0, 0);
    rex_leave_crit_sect(&pbm_crit_sect);
    return PBM_ERROR;
  }

  prev = NULL;
  curr = pbm_notify_list;

  while (curr)
  {
    if ((curr->notify_func == func) && (curr->user_data == user_data))
    {
      /* Found node, delete it from the list */
      if (prev)
        prev->next = curr->next;
      else
        pbm_notify_list = curr->next;

      PBM_MEM_FREEIF(curr);
      ret = PBM_SUCCESS;
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  rex_leave_crit_sect(&pbm_crit_sect);

  if (ret != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Could not find notification function %p:0x%x",
                 func, user_data, 0);
  }

  return ret;
}

/*===========================================================================
FUNCTION PBM_EMERGENCY_NUMBER

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.

  Length does not include the trailing NULL.

DEPENDENCIES
  uipbm.h
  nv.h

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_emergency_number(const byte *num, byte len)
{
  PBM_CHECK_PTR_RET(num, FALSE);

  PBM_MSG_HIGH("Checking num as emergency.  Len: %d.  Num: 0x%x 0x%x", len, (len>0?num[0]:-1), (len>1?num[1]:-1));
  PBM_MSG_HIGH("emergency 0x%x 0x%x 0x%x", (len>2?num[2]:-1), (len>3?num[3]:-1), (len>4?num[4]:-1));

  if (len == 0) {
    return FALSE;
  }

  /* Try the numbers in ECC phonebook. */
  if (pbm_find_number(PBM_ECC, num, len, NULL, NULL) == PBM_SUCCESS)
  {
    PBM_MSG_HIGH("Emergency number found",0,0,0);
    return TRUE;
  }

  PBM_MSG_HIGH("Emergency number not found",0,0,0);
  return FALSE;
}

/*===========================================================================
FUNCTION PBM_EMERGENCY_NUMBER_CAT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.  If it is, the
  category is returned in ecc_category_ptr.  If the number has no category, it is
  set to 0.

  Length does not include the trailing NULL.

DEPENDENCIES
  uipbm.h
  nv.h

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_emergency_number_cat(const byte *num, byte len, uint8 *ecc_category_ptr)
{
  uint16 rec_id;
  pbm_return_type ret_val;
  boolean is_emergency_num = FALSE;
  pbm_field_id_e_type found_field;  //field found in the current record
  uint32 best_match_index;          //index into priority array of best category so far
  uint32 i;
  uint8 found_category;             //category found in the current record

  /* specs say to prefer categories in SIM over network, and network over others */
  /* NV aren't spec'd, so they go to the bottom */
  pbm_field_id_e_type ecc_order_preference[] =
  {
    PBM_FIELD_SIM1_ECC, PBM_FIELD_SIM2_ECC,
    PBM_FIELD_NETWORK_ECC, PBM_FIELD_HARDCODED_ECC,
    PBM_FIELD_NV_ECC
  };

  byte my_ph_num[PBM_MAX_NUM_LENGTH+1]; //plus null char

  PBM_CHECK_PTR_RET(num, FALSE);
  PBM_CHECK_PTR_RET(ecc_category_ptr, FALSE);

  PBM_MSG_HIGH("Checking num as emergency.  Len: %d.  Num: 0x%x 0x%x", len, (len>0?num[0]:-1), (len>1?num[1]:-1));
  PBM_MSG_HIGH("emergency 0x%x 0x%x 0x%x", (len>2?num[2]:-1), (len>3?num[3]:-1), (len>4?num[4]:-1));

  if (len == 0 || len > PBM_MAX_NUM_LENGTH) {
    PBM_MSG_ERR("search for emerg num invalid length %d %d %d",len,0,PBM_MAX_NUM_LENGTH);
    return FALSE;
  }

  memset(my_ph_num,0,sizeof(my_ph_num));
  memcpy(my_ph_num,num,len);

  ret_val = pbm_enum_rec_init(PBM_ECC, (uint16)PBM_CAT_ECC, PBM_FIELD_NONE,
                              my_ph_num, (uint16)strlen((char*)my_ph_num)+1,
                              PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR |
                              PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8);
  if (PBM_SUCCESS == ret_val)
  {
    while(PBM_SUCCESS == pbm_enum_next_rec_id(&rec_id))
    {
      *ecc_category_ptr = 0; //in case we find nothing better
      best_match_index = ARR_SIZE(ecc_order_preference);

      /* we have a match */
      is_emergency_num = TRUE;

      //see if we can find a category
      if (pbm_find_ecc_cat_in_rec(rec_id, &found_category, &found_field))
      {
        //we found a category
        //Now, see if it's better than any category we may have.
        for (i=0; i<ARR_SIZE(ecc_order_preference); i++)
        {
          if (ecc_order_preference[i] == found_field)
          {
            if (i < best_match_index)
            {
              best_match_index = i;
              *ecc_category_ptr = found_category;
              PBM_MSG_HIGH("Picking emergency category 0x%x based on field 0x%x record 0x%x",
                       *ecc_category_ptr, found_field, rec_id);
            }
          }
        }
      }
    }
  }
  else
  {
    PBM_MSG_ERR("pbm_enum_rec_init failed %d", ret_val, 0, 0);
  }

  PBM_MSG_HIGH("Emergency number status %d cat %d",is_emergency_num,*ecc_category_ptr,0);

  return is_emergency_num;
}

/*===========================================================================
FUNCTION PBM_FIND_ECC_CAT_IN_REC

DESCRIPTION
  This function searches a given record id, assumed to be in the ECC
  phonebook for the ecc category.

  If it finds the field, it returns TRUE, and fills in ecc_category_ptr and
  found_field_ptr.

DEPENDENCIES

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
static boolean pbm_find_ecc_cat_in_rec(uint16 rec_id,
                                       uint8 *ecc_category_ptr,
                                       pbm_field_id_e_type *found_field_ptr)
{
  int i;
  pbm_return_type pbm_ret_val;
  boolean ret = FALSE;

  pbm_field_s_type      *fields_buf;
  uint32                fields_size;
  uint16 category;

  int item_count;
  void *data_ptr;

  PBM_CHECK_PTR_RET(ecc_category_ptr, FALSE);
  PBM_CHECK_PTR_RET(found_field_ptr, FALSE);

  //see if we can find a category
  fields_size = (uint32)pbm_calculate_fields_size_from_id(rec_id);
  if (fields_size == 0)
  {
    PBM_MSG_ERR("invalid record passed to pbm_find_ecc_cat_in_rec %d", rec_id, 0, 0);
    return FALSE;
  }

  fields_buf = mem_malloc(&tmc_heap, fields_size);
  PBM_CHECK_PTR_RET(fields_buf, FALSE);

  do
  {
    pbm_ret_val = pbm_record_read(rec_id, &category, &item_count,
                                  (uint8*)fields_buf, fields_size);
    if (pbm_ret_val != PBM_SUCCESS)
    {
      PBM_MSG_ERR("pbm_record_read() failed with error %d.", pbm_ret_val, 0, 0);
      ret = FALSE;
      break;
    }

    //see if we can find the ecc cat field
    for (i = 0; i < item_count; i++)
    {
      if (fields_buf[i].field_id == PBM_FIELD_ECC_SERVICE_CATEGORY)
      {
        //found it
        data_ptr = pbm_get_field_data_ptr(fields_buf,i);
        PBM_CHECK_PTR_BREAK(data_ptr, ret, FALSE);

        *ecc_category_ptr = *(uint8*)data_ptr;
        ret = TRUE;
        break;
      }
    }

    //for logging help, and to get field ID
    if (ret)
    {
      for (i = 0; i < item_count; i++)
      {
        switch (fields_buf[i].field_id)
        {
          case PBM_FIELD_NETWORK_ECC:
          case PBM_FIELD_HARDCODED_ECC:
          case PBM_FIELD_NV_ECC:
          case PBM_FIELD_SIM1_ECC:
          case PBM_FIELD_SIM2_ECC:
            *found_field_ptr = fields_buf[i].field_id;
            PBM_MSG_HIGH("Found ECC cat %d in record with field type 0x%x",
                     *ecc_category_ptr, *found_field_ptr, 0);
            break;
          default:
            break;
        }
      }
    }
  } while ( 0 ); /*lint !e717 */

  mem_free(&tmc_heap, fields_buf);
  return ret;
}


/*===========================================================================
FUNCTION PBM_FDN_NUMBER_MATCH

DESCRIPTION
  The rules for FDN is that all the SIM digits must match, but
  extra digits are allowed at the end.
  The following examples show a SIM string, a dialed String,
  and what the function should return.
  { "12345",       "12345",        TRUE },
  { "12345",       "22345",        FALSE },
  { "12345",       "123456",       TRUE },
  { "12345",       "1234",         FALSE },
  { "12345H1234",  "12345",        TRUE },
  { "123456H1234", "12345",        FALSE },
  { "123456H1234", "123456",       TRUE },
  { "12345T1234",  "1234561234",   TRUE },
  { "12345T1234",  "12345612345",  TRUE },
  { "123456T1234", "12345",        FALSE },
  { "12345?1234",  "12345",        FALSE },
  { "123456?1234", "12345",        FALSE },
  { "123456?1234", "12345671234",  TRUE },
  { "123456?1234", "123456712345", TRUE },
  { "123456?1234", "1234567123",   FALSE },
  { "??????????",  "123456789",    FALSE },
  { "??????????",  "1234567890",   TRUE },
  { "??????????",  "12345678901",  TRUE },
  { "?????H????",  "12345678901",  TRUE },
  { "?????H????",  "12345678901",  TRUE },
  { "?????T????",  "12345",        TRUE },
  { "?????T????",  "1234",         FALSE },

DEPENDENCIES
  None.

SIDE EFFECTS
===========================================================================*/
boolean pbm_fdn_number_match(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len)
{
  return pbm_number_match_int(sim_num, sim_num_len, dialed_num, dialed_num_len, TRUE);
}

/*===========================================================================
FUNCTION PBM_NUMBER_MATCH

DESCRIPTION
  The rules for non-FDN is that the digits in the SIM must match.  No
  extra characters are allow.  The following examples show a SIM string,
  a dialed String, and what the function should return.
  { "12345",       "12345",        TRUE },
  { "12345",       "22345",        FALSE },
  { "12345",       "123456",       FALSE },
  { "12345",       "1234",         FALSE },
  { "12345H1234",  "12345",        TRUE },
  { "123456H1234", "12345",        FALSE },
  { "123456H1234", "123456",       TRUE },
  { "12345T1234",  "1234561234",   FALSE },
  { "12345T1234",  "12345612345",  FALSE },
  { "123456T1234", "12345",        FALSE },
  { "12345?1234",  "12345",        FALSE },
  { "123456?1234", "12345",        FALSE },
  { "123456?1234", "12345671234",  TRUE },
  { "123456?1234", "123456712345", FALSE },
  { "123456?1234", "1234567123",   FALSE },
  { "??????????",  "123456789",    FALSE },
  { "??????????",  "1234567890",   TRUE },
  { "??????????",  "12345678901",  FALSE },
  { "?????H????",  "12345678901",  FALSE },
  { "?????H????",  "12345678901",  FALSE },
  { "?????T????",  "12345",        TRUE },
  { "?????T????",  "1234",         FALSE },
  { "911",         "911",          TRUE },
  { "911",         "9111",         FALSE },

DEPENDENCIES
  None.

SIDE EFFECTS
===========================================================================*/
boolean pbm_number_match(const byte *sim_num,    byte sim_num_len,
                         const byte *dialed_num, byte dialed_num_len)
{

  return pbm_number_match_int(sim_num, sim_num_len, dialed_num, dialed_num_len, FALSE);
}

/*===========================================================================
FUNCTION PBM_IS_INIT_COMPLETED

DESCRIPTION
will let the client know whether the init process of PBM is comlpeted or not


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_init_completed()
{
  boolean init_complete = FALSE;
  uint8 i;

  PBM_MSG_HIGH("pbm_is_init_completed",0,0,0);


#ifdef FEATURE_MMGSDI //always put any UIM code under FEATURE_MMGSDI
  for(i=0;i<MAX_UIMS;i++)
  {
    //check for the initialisation of inserted sim slots only
    if((pbm_slot_card_inserted & PBM_SIMID_TO_SLOT_MASK(i)) != 0)
    {
      if(pbm_init_info.pbm_sim_init_done[i] == TRUE)
      {
        init_complete = TRUE;
      }
      else
        {
        //return as soon as you find one SIM that is not initialised
        break;
      }
        }
  }
#endif
  return init_complete;
}


#endif /* FEATURE_PBM_TASK. */
