#ifndef PBMUTILS_H
#define PBMUTILS_H

/*===========================================================================

                  INTERNAL HEADER FILE FOR PBM UTILITIES

DESCRIPTION
  This is the internal header file for Phonebook Manager utility functions.

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbmutils.h#48 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
08/02/06   cvs     Header file + macro cleanup for WINCE
07/27/06   slw     Differentiated field info for ADN and SIM groups
07/26/06   cvs     Fix compile errors
07/24/06   cvs     Fixes for unequal size multi pb set support
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
03/06/06   cvs     Add new fields
02/24/06   cvs     Move string functions to pbm_conversion.
02/23/06   cvs     rework memory allocation macros
12/13/05   cvs     Implement partial PB refresh
11/16/05   cvs     Lint Fixes
10/18/05   cvs     Add new fields
09/27/05   cvs     Move size parameters to their own file. 
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   AT      Added new Emergency Group for searches.
08/24/05   cvs     Minor optimizations to pbm_find_name(), make functions public.
08/12/05   AT      Added support for UCS2 character set.
08/12/05   cvs     When extension records go unused, write out blank record
07/08/05   cvs     Add new fields
06/70/05   AT      One more field - increment the constant.
06/23/05   cvs     Speed up searching, using qsort() for results
05/31/05   cvs     Fix return type of pbm_convert_pbm8_to_gsm7()
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/18/05   AT      Updated the string comparison functions to handle wild cards.
04/08/05   cvs     Add fields
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for re-entrant complex field enumeration.
01/20/05   PA      Moved some definitions to pbmlib.h
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
01/10/05   PA      Updated maximum number of PBM fields
12/16/04   SUN     Added a new PBM field
12/15/04   SUN     Updated maximum number of PBM fields
12/09/04   PA      Correct emergency number start location
12/08/04   PA      Updated maximum number of PBM fields.
12/03/04   PA      Record validation utilities
11/19/04   PA      Code review comments
11/18/04   PA      Support for complex fields
09/29/04   PA      Initial revision.
===========================================================================*/
#include "pbmcache.h"
#include "pbm_tuning.h"

#define PBM_STAR_BCD            0xA
#define PBM_HASH_BCD            0xB
#define PBM_TIMEPAUSE_BCD       0xC
#define PBM_WILD_BCD            0xD
#define PBM_EXPANSION_BCD       0xE

#define PBM_ASYNC_BUFFERS 5

#define ALIGNMENT 4       /* Keep fields aligned on this boundry. */
#define ALIGNED_SIZE(x)   (ALIGNMENT * (((x) + ALIGNMENT - 1)/ALIGNMENT))

#ifndef TEXT
#define TEXT(x) x
#endif

#ifdef MSG_SSID_APPS_PBM
  #define PBM_MSG_HIGH(s,p1,p2,p3)       MSG_3(MSG_SSID_APPS_PBM,MSG_LEGACY_HIGH,s,p1,p2,p3)
  #define PBM_MSG_MED(s,p1,p2,p3)        MSG_3(MSG_SSID_APPS_PBM,MSG_LEGACY_MED,s,p1,p2,p3)
  #define PBM_MSG_LOW(s,p1,p2,p3)        MSG_3(MSG_SSID_APPS_PBM,MSG_LEGACY_LOW,s,p1,p2,p3)

  #ifdef PBM_FATAL_ON_ANY_ERROR
    #define PBM_MSG_ERR(s,p1,p2,p3)  MSG_3(MSG_SSID_APPS_PBM,MSG_LEGACY_FATAL,s,p1,p2,p3)
  #elif defined (FEATURE_WINCE)
    #define PBM_MSG_ERR(s,p1,p2,p3)  ERRORMSG(1, (L"ERR: " TEXT(s) L"\r\n", p1, p2, p3))
  #else
    #define PBM_MSG_ERR(s,p1,p2,p3)  MSG_3(MSG_SSID_APPS_PBM,MSG_LEGACY_ERROR,s,p1,p2,p3)
  #endif
#else /* no specific PBM MSG ID */

  #define PBM_MSG_HIGH(s,p1,p2,p3)       MSG_HIGH(s,p1,p2,p3)
  #define PBM_MSG_MED(s,p1,p2,p3)        MSG_MED(s,p1,p2,p3)
  #define PBM_MSG_LOW(s,p1,p2,p3)        MSG_LOW(s,p1,p2,p3)

  #ifdef PBM_FATAL_ON_ANY_ERROR
    #define PBM_MSG_ERR(s,p1,p2,p3)  ERR_FATAL(s,p1,p2,p3)
  #elif defined (FEATURE_WINCE)
    #define PBM_MSG_ERR(s,p1,p2,p3)  ERRORMSG(1, (L"ERR: " TEXT(s) L"\r\n", p1, p2, p3))
  #else
    #define PBM_MSG_ERR(s,p1,p2,p3)  ERR(s,p1,p2,p3)
  #endif
#endif /* MSG_SSID_APPS_PBM */

#ifdef VOID
#undef VOID
#endif

#define VOID
#define NO_ACTION       ;

#define EXT_REC_TYPE_FREE                    0x00
#define EXT_REC_TYPE_CALLED_PARTY_SUBADDRESS 0x01
#define EXT_REC_TYPE_ADDITIONAL_DATA         0x02
#define PBM_MAX_ECC  5
#define PBM_ECC_MAX_LEN  6
#define PBM_ECC_MIN_LEN  3
#define PBM_SIM_DIALING_NUMBER_LEN  10
#define PBM_MAX_ECC_NUMBERS  50
#define ADN_KNOWN_REC_LEN   14
#define FDN_KNOWN_REC_LEN   14
#define LND_KNOWN_REC_LEN   14
#define OCI_KNOWN_REC_LEN   27
#define ICI_KNOWN_REC_LEN   28
#define EXT_KNOWN_REC_LEN   13
#define ANR_KNOWN_REC_LEN   15
#define CALL_HISTORY_SIZE   10  /* Number of history records supported by SIM.*/


#define PBM_RECID_NULL      0

#define PBM_MEM_ALLOC(dest,size) (dest)=(pbm_mem_alloc(size));
//lint -emacro(774,PBM_MEM_FREEIF) doesn't matter if the if is always true here
#define PBM_MEM_FREEIF(mem)        \
  {                                \
    if ((mem))                     \
    {                              \
      pbm_mem_free((mem));         \
      (mem) = NULL;                \
    }                              \
  }

//The checks tend to be true.
//lint -emacro(774,PBM_ALLOC_MEM_W_ERR_RET)
//lint -emacro(506,PBM_ALLOC_MEM_W_ERR_RET)
#define PBM_ALLOC_MEM_W_ERR_RET(dest, size, err_code)  \
  if ((size) > 0)                                      \
  {                                                    \
    PBM_MEM_ALLOC(dest, size)                          \
    if ((dest) == NULL)                                \
    {                                                  \
      PBM_MSG_ERR("Could not allocate %d bytes of memory for 0x%x",  (size), &(dest), 0); \
      err_code;                                        \
      return PBM_ERROR;                                \
    }                                                  \
  }

/* If ret_val is not PBM_SUCCESS, prints "str" */
#define PBM_CHECK_ERR(ret_val, str)                          \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    PBM_MSG_ERR(str " Error %d", (ret_val), 0, 0);           \
  }

/* If ret_val is not PBM_SUCCESS, prints "str" 
 * In addition, it returns with ret_val */
#define PBM_CHECK_ERR_RET(ret_val, str)                      \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    PBM_MSG_ERR(str " Error %d", (ret_val), 0, 0);           \
    return ret_val;                                          \
  }

#define PBM_CHECK_ERR_BREAK(ret_val, str)                    \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    PBM_MSG_ERR(str " Error %d", (ret_val), 0, 0);           \
    break;                                                   \
  }

/* If x is NULL, prints error and returns ret */
#define PBM_CHECK_PTR_RET(x, ret)                             \
  if ((x) == NULL) {                                          \
    PBM_MSG_ERR("Unexpected NULL pointer %x %x %x", 0, 0, 0); \
    return ret;                                               \
  }

/* If x, y is NULL, prints error and returns ret */
#define PBM_CHECK_PTR2_RET(x, y, ret)                             \
  if (((x) == NULL) || ((y) == NULL)) {                           \
    PBM_MSG_ERR("Unexpected NULL pointer %x %x %x", (x), (y), 0); \
    return ret;                                                   \
  }

/* If x, y, z is NULL, prints error and returns ret */
#define PBM_CHECK_PTR3_RET(x, y, z, ret)                            \
  if (((x) == NULL) || ((y) == NULL) || ((z) == NULL)) {            \
    PBM_MSG_ERR("Unexpected NULL pointer %x %x %x", (x), (y), (z)); \
    return ret;                                                     \
  }

/* If w, x, y, z is NULL, prints error and returns ret */
#define PBM_CHECK_PTR4_RET(w, x, y, z, ret)                               \
  if (((w) == NULL) || ((x) == NULL) || ((y) == NULL) || ((z) == NULL)) { \
    PBM_MSG_ERR("Unexpected NULL pointer %x %x %x", (w), (x), (y));       \
    return ret;                                                           \
  }

#define PBM_CHECK_PTR_BREAK(x, ret, val)                     \
  if ((x) == NULL) {                                         \
    PBM_MSG_ERR("Unexpected NULL pointer ", 0, 0, 0);        \
    (ret) = (val);                                           \
    break;                                                   \
  }

#define PBM_CHECK_FILE_STATUS_RET(action, str, ret)\
  if (action.status != FS_OKAY_S) {                               \
    PBM_MSG_ERR(str " Error %d",action.status, 0, 0);             \
    return ret;                                                   \
  }

#define PBM_CHECK_FILE_STATUS_RETCODE_BREAK(action, str, ret, val)\
  if (action.status != FS_OKAY_S) {                               \
    PBM_MSG_ERR(str " Error %d",action.status, 0, 0);             \
    (ret) = (val);                                                \
    break;                                                        \
  }

#define PBM_CHECK_FILE_STATUS_SIZED_RETCODE(action, size, str, ret, val)      \
  if ((action.status != FS_OKAY_S) || (action.count != (size))) {             \
    PBM_MSG_ERR(str " Error %d %d",action.status, action.count, 0);           \
    (ret) = (val);                                                            \
  }

#define PBM_CHECK_FILE_STATUS_SIZED_RETCODE_BREAK(action, size, str, ret, val)\
  if ((action.status != FS_OKAY_S) || (action.count != (size))) {             \
    PBM_MSG_ERR(str " Error %d %d",action.status, action.count, 0);           \
    (ret) = (val);                                                            \
    break;                                                                    \
  }

#define PBM_ADD_UNIQUE_ITEM_TO_LIST(item, list, list_size, loop_var)   \
          for ((loop_var)=0; (loop_var) < (list_size); (loop_var)++)   \
          {                                                            \
            if ((list)[(loop_var)] == (item))                          \
            {                                                          \
              break; /* found it in the list, exit search loop */      \
            }                                                          \
          }                                                            \
          if ((loop_var) == (list_size))                               \
          {                                                            \
            /* it's not already in the list, so add it */              \
            if ((list_size) < (int)ARR_SIZE(list))                     \
            {                                                          \
              (list)[(list_size)++] = (item);                          \
            }                                                          \
            else                                                       \
            {                                                          \
              PBM_MSG_ERR("outgrew list.  size %d max size %d",        \
                          (list_size), ARR_SIZE(list), 0);             \
            }                                                          \
          }

#define PBM_CHECK_PBSET_RET(pb_set, ret_val)                             \
  if (((pb_set) < PBM_USIM_PB_ONE) || ((pb_set) >= PBM_MAX_NUM_PB_SETS)) \
  {                                                                      \
    PBM_MSG_ERR("PB_SET out of bounds %d num sets %d", (pb_set),         \
                PBM_MAX_NUM_PB_SETS, 0);                                 \
    return ret_val;                                                      \
  }

#define PBM_CHECK_PBSET_RETCODE_BREAK(pb_set, ret, val)                  \
  if (((pb_set) < PBM_USIM_PB_ONE) || ((pb_set) >= PBM_MAX_NUM_PB_SETS)) \
  {                                                                      \
    PBM_MSG_ERR("PB_SET out of bounds %d num sets %d", (pb_set),         \
                PBM_MAX_NUM_PB_SETS, 0);                                 \
    (ret) = (val);                                                       \
    break;                                                               \
  }

#define PBM_CHECK_PBSET_BREAK(pb_set)                                    \
  if (((pb_set) < PBM_USIM_PB_ONE) || ((pb_set) >= PBM_MAX_NUM_PB_SETS)) \
  {                                                                      \
    PBM_MSG_ERR("PB_SET out of bounds %d num sets %d", (pb_set),         \
                PBM_MAX_NUM_PB_SETS, 0);                                 \
    break;                                                               \
  }

#ifdef CUST_EDITION
#define PBM_MAX_NUM_FIELDS 20
#else
#define PBM_MAX_NUM_FIELDS 80
#endif
typedef struct {
  int num_fields;
  pbm_field_info_s_type info[PBM_MAX_NUM_FIELDS];
} pbm_g_field_info_s_type;

#define INVALID_GROUP         -1
#define SIM_GROUP              0
#define ADN_GROUP              1
#define SDL_GROUP              2
#define EFS_GROUP              3
#define CALLHISTORY_GROUP      4
#define ECC_GROUP              5
#define NUM_GROUPS             6

extern pbm_g_field_info_s_type g_field_info[];

extern char pbm_bcd_to_char_mapping[];

typedef struct {
  pbm_field_s_type *fields;
  int               num_fields;
  int               enumerator;
} pbm_complex_result_s_type;


#define PBM_NOTIFY(evt, field, field_data)                            \
  {                                                                   \
    do {                                                              \
      pbm_notify_evt_s_type *pNotify_evt_cmd = NULL;                  \
      /* Allocate PBM notify event command */                         \
      pNotify_evt_cmd = (pbm_notify_evt_s_type *)                     \
                        pbm_cmd_alloc(sizeof(pbm_notify_evt_s_type)); \
      if (!pNotify_evt_cmd)                                           \
      {                                                               \
        PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);       \
        break;                                                        \
      }                                                               \
      pNotify_evt_cmd->cmd_type = PBM_CMD_NOTIFY_EVT;                 \
      pNotify_evt_cmd->notify_data.event = (evt);                     \
      pNotify_evt_cmd->notify_data.data.##field = (field_data);       \
      pbm_notify_evt_q_put(PBM_CMD_PTR(pNotify_evt_cmd));             \
    } while (FALSE);                                                  \
  }

/*===========================================================================
FUNCTION PBM_NOTIFY

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_notify(pbm_notify_data_s_type *notify_data);

/*===========================================================================
FUNCTION pbm_heap_init

DESCRIPTION
  This function initializes the PBM heap.

RETURN VALUE:
  PBM_SUCCESS: If successful
  PBM_ERROR:   Otherwise

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_heap_init(void);

/*==============================================================================

FUNCTION NAME pbm_action_requires_sim

DESCRIPTION
   This function is called to determine if an action on a given pb_id
   requires the SIM to be fully initialized.  The purpose of this call
   is to prohibit reads and writes to memory while the SIM is still
   initializing.

RETURN VALUE
  TRUE: If the action would require the SIM.
  FALSE: The pb_id does not touch the UIM.

DEPENDENCIES
  None
==============================================================================*/
boolean pbm_action_requires_sim(pbm_device_type pb_id);

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_LOCATION_TO_ADN_REC_ID

DESCRIPTION
  Returns the ADN record id given the sim id, phonebook set and location 
  of ADN record. 

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_adn_rec_id(pbm_slot_type sim_id, 
                                  pbm_pb_set_type pb_set, 
                                  uint8 index);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_SET

DESCRIPTION
  Returns the proper pb set based on the given file id.

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_file_id_to_pb_set(pbm_file_device_e_type file_id);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache type associated with a given file id

SIDE EFFECTS
===========================================================================*/
pbm_cache_type_e_type pbm_file_id_to_cache_type(pbm_file_device_e_type file_id);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given file id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
void *pbm_file_id_to_cache(pbm_slot_type sim_id, 
                           pbm_file_device_e_type file_id);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_FIELD_ID

DESCRIPTION
  Returns the field id stored within the given file device.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_file_id_to_field_id(pbm_file_device_e_type file_id);
#endif /* FEATURE_MMGDSI */

/*===========================================================================
FUNCTION pbm_mem_alloc_init

DESCRIPTION
  Initially, the memory buffer is one big empty buffer.  Set the start of
  it to be a FREE_NODE and set the flags accordingly.  Also, initialize our
  pbm_mem_first_available and pbm_mem_last_address variables;

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_mem_alloc_init(unsigned char *buffer, unsigned long size);

/*===========================================================================
FUNCTION CHECK_PB_READY

DESCRIPTION
Checks if the phonebook is ready or not and returns the appropriate PBM
return value. Any value other than PBM_SUCCESS means the phonebook is not
available for use.

Checking for PBM_DEFAULT returns an ERROR

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type check_pb_ready(pbm_device_type pb_id);

/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_FIELDS

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure 
  passed in.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_size_from_fields( const pbm_field_s_type *fields, int num_fields);

/*===========================================================================
FUNCTION PBM_FREE_FIELDS_INTERNAL

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields_internal.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields_internal( pbm_field_s_type **fields, int num_fields);

/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS_INTERNAL

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields_internal(pbm_field_s_type **fields_buf_ptr, uint32 num_fields, const uint32 *sizes);

/*===========================================================================
FUNCTION PBM_COPY_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the destination field array
  and copies the data from the source field array.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_fields(pbm_field_s_type **dst, pbm_field_s_type const * const * src,
                                                          uint32 num_fields);

/*===========================================================================
FUNCTION pbm_mem_free

DESCRIPTION
  Takes the node and frees it.  If the following block can be merged,
  then merge that one.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_mem_free(void *block);

/*===========================================================================
FUNCTION pbm_mem_alloc

DESCRIPTION
  Search the free list for the best fit block of memory.  If no exact
  size is found, the closest sized block is split to hold the new memory.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void *pbm_mem_alloc(unsigned long size);

/*===========================================================================
FUNCTION PBM_BUILD_NUM_TEXT_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_num_text_record_s_type record 
  required for SIM writing.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_num_text_record_from_fields(pbm_field_s_type *fields, int item_count,
                                  int index, pbm_num_text_record_s_type *record);

/*===========================================================================
FUNCTION PBM_BUILD_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_record_s_type record expected by
  the old software.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_fields(pbm_field_s_type *fields, int item_count,
                                  int index, pbm_record_s_type *record);

/*===========================================================================
FUNCTION PBM_BUILD_FIELDS_FROM_RECORD

DESCRIPTION
  This function is used for the legacy API.  It takes a pbm_record_s_type
  record and builds a record that contains an array of fields expected by
  the new interface.

===========================================================================*/
pbm_addr_cache_s_type * pbm_build_fields_from_record (pbm_device_type pb_id,
                                          pbm_record_s_type *record);

/*===========================================================================
FUNCTION MATCH_FOUND

DESCRIPTION
Local utility function used by pbm_enum_rec_init() to determine whether the
given cache entry is a match for the given search criteria. Assumes that
data is null terminated.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean match_found(pbm_cat_e_type category,
                    pbm_field_id_e_type field_id,
                    const void *data_ptr,
                    uint16 data_size,
                    uint32 flags,
                    const pbm_addr_cache_s_type *cache_entry);

/*===========================================================================

FUNCTION: pbm_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII, but checks to make sure
  we don't overwrite memory.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, if we can do this.

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_bcd_to_ascii(__packed const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number);

/*===========================================================================
FUNCTION PBM_BCD_TO_BIN

DESCRIPTION
  Converts time quantities from bcd to binary.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bcd_to_bin( byte bcd );

/*===========================================================================
FUNCTION PBM_PB_ID_TO_SIM_ID

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_slot_type pbm_pb_id_to_sim_id(pbm_device_type pb_id);

/*===========================================================================

FUNCTION: pbm_ascii_to_bcd

DESCRIPTION
  Convert the phone number from ASCII to BCD, but checks to make sure
  we don't overwrite memory. Updates number type of bcd number if the ascii
  number starts with a '+'.

DEPENDENCIES
  Boolean, if we can do this.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_ascii_to_bcd(byte *ascii_number, uint8 type, byte *bcd_number);

/*===========================================================================
FUNCTION PBM_BIN_TO_BCD

DESCRIPTION
  Converts time quantities from binary to bcd.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bin_to_bcd( byte bin );

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_INIT_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_file_id_to_pb_id(pbm_slot_type sim_id, 
                                     pbm_file_device_e_type file_id);

/*===========================================================================
FUNCTION PBM_INIT_FILE_CIRCULAR

DESCRIPTION
  This function returns TRUE if the init file is circular, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
boolean pbm_file_circular(pbm_file_device_e_type file_id);
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_CHECK

DESCRIPTION
  This function is does all the initial error checking for a record write operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_check(pbm_device_type pb_id,
                                  const uint16 *rec_id_ptr,
                                  uint16 cat,
                                  const pbm_field_s_type *fields,
                                  int num_fields,
                                  PBM_WRITE_CB_FUNC proc_func);

/*===========================================================================
FUNCTION PBM_COMPLEX_GET_SUB_FIELD

DESCRIPTION
  This function obtains the sub field for a given index. Index in this case 
  is 0-based.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_sub_field_s_type *pbm_complex_get_sub_field(pbm_complex_field_s_type *cfield,
                                                int index);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_INIT

DESCRIPTION
  This function initializes enumeration of a complex field. If there are
  complex fields within complex fields, all of them are converted into 
  individual PBM fields.

  If the input field is a simple field, the simple field is returned as enumeration.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_complex_init(const pbm_field_s_type *field, 
                                      int *num_fields,
                                      pbm_complex_result_s_type **presult);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_NEXT

DESCRIPTION
  This function obtains the next sub field within a complex field, converts it 
  to a normal PBM field and returns it. 

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_field_s_type *pbm_enum_complex_next(pbm_complex_result_s_type *result);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_FREE

DESCRIPTION
  This function frees the memory allocated to store the search result.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_enum_complex_free(pbm_complex_result_s_type *result);

/*===========================================================================
FUNCTION PBM_CAT_TO_PHONEBOOK

DESCRIPTION
  This function returns the phonebook associated with a given category.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_cat_to_phonebook(pbm_cat_e_type cat);

/*===========================================================================
FUNCTION PBM_PB_ID_TO_FIELDGROUP

DESCRIPTION
  This function returns the field info group corresponding to this phonebook.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_pb_id_to_fieldgroup(pbm_device_type pb_id);

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_FILE_ID

DESCRIPTION
  This function will find the file id corresponding to the given record id.

SIDE EFFECTS
===========================================================================*/
pbm_file_device_e_type pbm_record_id_to_file_id(uint16 rec_id);
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION PBM_IS_USIM_FIELD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_field(pbm_field_id_e_type fid);

/*===========================================================================
FUNCTION pbm_is_usim_required

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_required(uint16 rec_id, const pbm_field_s_type *fields, 
                             int num_fields);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GROUP_FIELDS

DESCRIPTION
  Group fields together. Currently we care only about grouping group ids
  together.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_group_fields(pbm_addr_cache_s_type *record);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given ADN index (ie, with 2 sets, 10 records 
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_pb_record_index_to_pb_set(const PB_CACHE *cache, int index);

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given SYNC2 index (ie, with 2 sets, 10 records 
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_sync2_record_index_to_pb_set(const PB_SYNC2_CACHE *cache, int index);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records 
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
mmgsdi_file_enum_type pbm_pb_record_index_to_filename(const PB_CACHE *cache, int index);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records 
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
mmgsdi_file_enum_type pbm_sync2_record_index_to_filename(const PB_SYNC2_CACHE *cache, int index);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif //FEATURE_MMGSDI

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records 
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_pb_record_index_to_fileloc(const PB_CACHE *cache, int index);

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records 
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_sync2_record_index_to_fileloc(const PB_SYNC2_CACHE *cache, int index);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif //FEATURE_MMGSDI

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PB_SET

DESCRIPTION
  Map a record ID to a phonebook set
  
DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_record_id_to_pb_set(uint16 rec_id);

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_INDEX

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_fileloc(uint16 rec_id);

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_fill_adn_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_fill_adn_sfi(uint16 rec_id, byte *buffer);

/*===========================================================================
FUNCTION pbm_build_record_from_usim_fields

DESCRIPTION
  This function adds ato a record from USIM fields.  It expects to either be
  sent 1 field, or multiple consecutive group ID fields.


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_usim_fields(uint16 adn_rec_id, 
                                       const pbm_field_s_type *fields, 
                                       int num_fields,
                                       pbm_sim_record_s_type *record);
#endif /* FEATURE_PBM_USIM_SUPPORT */
/*===========================================================================
FUNCTION pbm_allocate_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_index(uint16 rec_id, const pbm_field_s_type *fields,
                                   int num_fields, uint8 **prev_index,
                                   uint8 **curr_index);

#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION pbm_free_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_index(uint8 *prev_index, uint8 *curr_index);

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_copy_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_usim_index(uint16 rec_id, 
                         uint8 *index_array);

/*===========================================================================
FUNCTION pbm_get_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_get_usim_index(uint16 rec_id, 
                              const pbm_field_s_type *fields, int num_fields,
                              uint8 *index_array);

/*===========================================================================
FUNCTION PBM_GET_NEXT_USIM_CACHE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
PB_USIM_CACHE *pbm_get_next_usim_cache(uint16 rec_id,
                                  pbm_field_id_e_type field_id, 
                                  int *index);

/*===========================================================================
FUNCTION pbm_copy_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsm_modified(uint16 rec_id);

/*===========================================================================
FUNCTION pbm_validate_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_validate_sfi(uint16 rec_id, uint8 sfi);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION pbm_check_write_lock

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_check_write_lock(void);

/*===========================================================================
FUNCTION pbm_check_read_lock

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_check_read_lock(void);

/*===========================================================================
FUNCTION pbm_wstrncpy

DESCRIPTION
  uint16 *pbm_wstrncpy(uint16 *dest, const uint16 *src, int size) 

  dest: [out]  The resulting string is placed in dest.
  src:  [in]   The string to copy is placed here.
  size: [in]   The number of UCS2 characters of src to at most copy.

  This is the UCS2 version of the strncpy function.  It will copy
  src to dest until either a NULL character is encountered or
  size characters have been copied.  The resulting string is
  NULL terminated if there was one encountered within the first
  size characters.  Otherwise, the resulting string is not NULL terminated.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
uint16 *pbm_wstrncpy(uint16 *dest, const uint16 *src, int size);

/*===========================================================================
FUNCTION pbm_wstrncat

DESCRIPTION
  uint16 *pbm_wstrncat(uint16 *dest, uint16 *src, int size)

  dest: [out]  The resulting string is placed in dest.
  src:  [in]   The string to copy is placed here.
  size: [in]   The number of UCS2 characters of src to at most copy.

  This is the UCS2 version of the strncat function.  It will append
  src to dest until either a NULL character is encountered or
  size characters have been copied.  The resulting string is
  NULL terminated if there was one encountered within the first
  size characters.  Otherwise, the resulting string is not NULL terminated.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
uint16 *pbm_wstrncat(uint16 *dest, const uint16 *src, int size);

/*===========================================================================
FUNCTION PBM_GET_NAME

DESCRIPTION
Extracts the name field from the cache based on a priority.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_name(const pbm_addr_cache_s_type *cache, 
                             boolean pbm_sort_last_first, uint16 *nameBuff, 
                             int buffSize);

/*===========================================================================
FUNCTION PBM_APPEND_PHONE_UID_FIELD

DESCRIPTION
  This function is used to append the field PBM_FIELD_PH_UID to a field list
  with the given phone unique ID.

DEPENDENCIES
  None.

SIDE EFFECTS
  Frees the existing field list. New memory is allocated for the appended field list, and the pointer
  to it is returned in the parameter.

SEE ALSO
==========================================================================*/
pbm_return_type pbm_append_phone_uid_field(uint64 ph_unique_id, pbm_field_s_type **ppfields,
   uint16 *pnum_fields);

/*===========================================================================
FUNCTION PBM_FIND_PHONE_UID_FIELD

DESCRIPTION
  This function locates the PH UID field from a field list and returns the
  index to the field. Returns TRUE on found, FALSE if not found.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/
boolean pbm_find_phone_uid_field(const pbm_field_s_type *fields, int num_fields, int *index);

/*===========================================================================
FUNCTION PBM_CONVERT_PBM8_TO_GSM7

DESCRIPTION
  This function converts a string from PBM8 alphabet to GSM7 alphabet
  
DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/
void pbm_convert_pbm8_to_gsm7(char *dest, char *src, int len);

/*===========================================================================
FUNCTION PBM_NUMBER_MATCH_INT

DESCRIPTION
  Does an exact match of a phone number.  Wild card characters match
  any digits.  All searches end at the end of either string, or a pause
  character in the SIM string.  These matches require the dialed string
  does not contain a wild card or pause character - these routines are
  meant to validate a string which was given to call manager to see if
  the call is allowed.  If fdn_comparison is TRUE, we do a starts with
  match (extra digits are allowed).

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_number_match_int(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len,
                             boolean fdn_comparison);

/*===========================================================================
FUNCTION pbm_fields_convert_str_to_ucs2

DESCRIPTION
  boolean pbm_fields_convert_str_to_ucs2(uint8   **to_fields_buf_ptr,
                                         int     *to_fields_buf_size,
                                         const   uint8  *from_fields_buf_ptr,
                                         uint32  num_fields)

  to_fields_buf_ptr:   [out]   newly allocated record with all
                               PBM_FT_STRING values stored as PBM_FT_UCS2 type
  to_fields_buf_size:  [out]   size of the newly allocated record
  from_fields_buf_ptr: [in]    record which needs to be converted
  num_fields:          [in]    number of fields
                               
  This function takes a given record and copies over the fields to a
  new buffer. If any field of type PBM_FT_STRING is encountered, they
  are copied over as PBM_FT_UCS2. If no PBM_FT_STRING type is found,
  no new buffer is allocated. The calling function is responsible for
  freeing the memory allocated for the new buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if conversion was successful

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbm_fields_convert_str_to_ucs2(uint8          **to_fields_buf_ptr,
                                       uint32         *to_fields_buf_size,
                                       const   uint8  *from_fields_buf_ptr,
                                       uint32         num_fields);

#endif /* PBMUTILS_H */
