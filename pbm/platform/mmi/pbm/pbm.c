/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the PBM write and init functions.

  Copyright (c) 2002 - 2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbm.c#56 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
12/29/08   pvphani Adding Null pointer check in pbm_list_recover function. Fix for CR: 164527
03/04/08   clm     Add logic to revert records containing USIM only fields upon
                   failure to add, delete, or update.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
01/16/07   cvs     Add event for phonebooks becoming ready
12/06/06   cvs     Fix double read of files after full card refresh
12/06/06   cvs     Remove banned APIs
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
07/12/06   cvs     Fix compile warnings
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/10/06   cvs     add debug feature to ignore WMS init
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
01/19/06   cvs     Emergency number cleanup, fix bug in network emerg nums
01/09/06   cvs     Lint fixes
01/04/06   cvs     Fix compile error without GSDI
12/14/05   cvs     Better log messages
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/15/05   cvs     Lint fixes
11/10/05   cvs     Add network ECC numbers
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/04/05   cvs     Check for committed list growth
08/01/05   cvs     Code review comments
07/29/05   AT      Added additional range checking.
07/06/05   cvs     Add 999 to emergency list if CPHS is enabled.
06/14/05   cvs     Fix GCF failures related to emergency numbers
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/23/05   cvs     Fix off-by-one error in loading ECC list
04/29/05   cvs     Fix Error Handling for USIM failures
03/31/05   AT      Created feature FEATURE_CPHS_UK to hardcode the
                   emergency number 999 in all cases.  This feature
                   causes a GCF test case to fail, but meets the CPHS
                   requirement.  Note these requirements are in conflict.
                   Also added featurization for non_MMGSDI builds.
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Added support for read lock, non-blocking write lock and
                   support for MSISDN.
01/24/05   PA      Changed NV_EMERGENCY_NUMBER_START to
                   PBM_NV_EMERGENCY_NUMBER_START
01/20/05   PA      Add only emergency numbers to ecc_cache.
01/12/05   PA      PBM USIM Support
12/09/04   PA      Set cache status to ready after reading NV items
12/03/04   PA      Fix category used in mirroring
12/02/04   PA      Add reference when new record in added to committed list.
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"

#ifdef FEATURE_PBM_TASK
#include "pbm.h"

#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmefs.h"
#include "pbmtask.h"
#include "pbm_conversion.h"

#ifdef FEATURE_MMGSDI
#include "pbmuim.h"
#include "pbmgsdi.h"
#endif

#include "cm.h"

#ifdef FEATURE_WMS
#include "wms.h"
#endif

/* <EJECT> */
/*===========================================================================
                              STATIC DATA
===========================================================================*/

/*===========================================================================
                       Declare data for the ECC.
===========================================================================*/
#ifdef FEATURE_MMGSDI
uint32 ecc_initialized = 0;
uint32 ecc_has_entries = 0;
#endif


#ifdef FEATURE_WMS
#ifdef FEATURE_PBM_IGNORE_WMS_INIT
boolean pbm_wms_init_finished = TRUE;
#else
boolean pbm_wms_init_finished = FALSE;
#endif
#endif


rex_crit_sect_type pbm_crit_sect;
rex_crit_sect_type pbm_write_lock_crit_sect;
rex_crit_sect_type pbm_read_lock_crit_sect;

/* PBM Write Lock */
pbm_lock_s_type g_lock = {NULL, PBM_LOCK_NOT_INIT, FALSE};

/* List of notification functions registered with PBM */
pbm_notify_node_s_type *pbm_notify_list = NULL;

/* This list contains the last known good record for each pending write. This is
 * used in bringing the cache to a sane state when a write operation fails. If a
 * record does not exist in this list, the existing cache entry is implied to be
 * the last committed record.
 * */
pbm_addr_node_s_type *pbm_committed_list = NULL;

/* CM stuff */
#ifdef CM_API_EMERGENCY_NUM_LIST
static cm_client_id_type pbm_cm_client_id = (cm_client_id_type) -1;

static void pbm_cm_ss_event_cb(cm_ss_event_e_type ss_event,
                               const cm_mm_ss_info_s_type *ii_info_ptr);
#endif

static pbm_return_type pbm_ecc_init(void);
static pbm_return_type pbm_cm_init(void);
static pbm_return_type pbm_wms_init( void );
pbm_return_type pbm_efs_cache_structure_init(PB_CACHE *cache,
                                             pbm_device_type pb_id,
                                             int num_of_records,
                                             boolean circular);
#ifdef FEATURE_WMS
static void pbm_wms_cfg_cb( wms_cfg_event_e_type event,
                     wms_cfg_event_info_s_type * event_info_ptr);
#endif

#ifdef FEATURE_PBM_USIM_SUPPORT
static pbm_return_type pbm_recover_usim_record(const pbm_write_record_s_type *write);
#endif // FEATURE_PBM_USIM_SUPPORT

/* <EJECT> */
/*===========================================================================*
 *              PHONE BOOK MANAGER INITIALIZATION FUNCTIONS                  *
 *                                                                           *
 * These functions will initialize the phone book managers phone books       *
 *===========================================================================*/


/*===========================================================================
FUNCTION PBM_BUILD_ECC_RECORD

DESCRIPTION
  This function takes a name, number, and category and builds the appropriate
  record for PBM_ECC.  It then adds this record to the cache.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_build_ecc_record(pbm_field_id_e_type field_id,
                                            uint32 data_length, const char *ecc_string,
                                            uint32 name_length, const uint16 *name_string,
                                            boolean usim_ecc_serv_cat_valid,
                                            uint8 usim_ecc_serv_cat)
{
  uint32 number_of_fields = 0;
  uint32 field_length[3];
  int index = 0;
  pbm_addr_cache_s_type *record;
  PB_CACHE *ecc_cache = NULL;
  uint16 location;

#ifdef FEATURE_ECC_HANDLING_AT_PBM
  switch (field_id)
  {
    case PBM_FIELD_NETWORK_ECC:
    case PBM_FIELD_HARDCODED_ECC:
    case PBM_FIELD_NV_ECC:
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC_OTHER);
      break;
    case PBM_FIELD_SIM1_ECC:
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC);
      break;
    case PBM_FIELD_SIM2_ECC:
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC2);
      break;
    default:
      PBM_MSG_HIGH("invalid ecc category", 0,0,0);
      return PBM_ERROR;
  }
#else
  ecc_cache = pbm_pb_id_to_cache(PBM_ECC);
#endif
  PBM_CHECK_PTR_RET(ecc_string, PBM_ERROR);

  if (data_length > 0)
  {

    PBM_MSG_HIGH("Adding ECC num, type 0x%x len %d", field_id, data_length, 0);
    PBM_MSG_HIGH("Adding ECC num, 0x%x 0x%x 0x%x", (data_length>0?ecc_string[0]:-1), (data_length>1?ecc_string[1]:-1), (data_length>2?ecc_string[2]:-1));
    PBM_MSG_HIGH("Adding ECC num, 0x%x 0x%x 0x%x", (data_length>3?ecc_string[3]:-1), (data_length>4?ecc_string[4]:-1), (data_length>5?ecc_string[5]:-1));
    PBM_MSG_HIGH("Adding ECC num, cat 0x%x valid %d", usim_ecc_serv_cat, usim_ecc_serv_cat_valid, 0);

    field_length[number_of_fields++] =  data_length;
    if (name_length > 0)
    {
      field_length[number_of_fields++] =  name_length;
    }
    if (usim_ecc_serv_cat_valid)
    {
      field_length[number_of_fields++] = 1;
    }

    // Next format the number for the ECC cache indicating that it is from the SIM.
    // If we ever need to figure out which application it came from, that would
    // be a different the field ID for now, each SIM has a unique ID.
    record = pbm_cache_record_new();
    PBM_CHECK_PTR_RET(record, PBM_ERROR_MEM_FULL);
    if(!pbm_allocate_fields_internal(&record->fields, number_of_fields, field_length))
    {
      PBM_MEM_FREEIF(record);
      PBM_MSG_ERR("Could not allocate fields", 0, 0, 0);
      return PBM_ERROR;
    }
    record->unique_id = 0;

    /* Allocate the location.  Return an error if it is full. */
    if ((location = pbm_get_first_available(ecc_cache, FALSE)) == 0)
    {
      pbm_free_fields_internal(&record->fields, 1);
      PBM_MEM_FREEIF(record);
      return PBM_ERROR_MEM_FULL;
    }

#ifdef FEATURE_ECC_HANDLING_AT_PBM
  switch (field_id)
  {
    case PBM_FIELD_NETWORK_ECC:
    case PBM_FIELD_HARDCODED_ECC:
    case PBM_FIELD_NV_ECC:
      record->record_id = pbm_location_to_record_id(PBM_ECC_OTHER, location);
      break;
    case PBM_FIELD_SIM1_ECC:
      record->record_id = pbm_location_to_record_id(PBM_ECC, location);
      break;
    case PBM_FIELD_SIM2_ECC:
      record->record_id = pbm_location_to_record_id(PBM_ECC2, location);
      break;
    default:
      PBM_MSG_HIGH("invalid ecc category", 0,0,0);
      return PBM_ERROR;
  }
#else
    record->record_id = pbm_location_to_record_id(PBM_ECC, location);
#endif

    record->category = (int) PBM_CAT_ECC;
    record->num_fields = (uint16) number_of_fields;
    index = 0;
    pbm_fill_field(&record->fields[index++], field_id,
                   (pbm_field_type_e_type) PBM_FT_PHONE,
                   (byte *) ecc_string, (uint16) data_length);

    if (name_length > 0)
    {
      pbm_fill_field(&record->fields[index++], PBM_FIELD_NAME,
                     (pbm_field_type_e_type) PBM_FT_UCS2,
                     (byte *) name_string, (uint16) name_length);
    }

    if (usim_ecc_serv_cat_valid)
    {
      pbm_fill_field(&record->fields[index++], PBM_FIELD_ECC_SERVICE_CATEGORY,
                     (pbm_field_type_e_type) PBM_FT_BYTE,
                     (byte *) &usim_ecc_serv_cat, 1);
    }

    // Finally, add the number to the cache.
    if (PBM_SUCCESS != pbm_cache_add(ecc_cache, record, TRUE))
    {
      pbm_free_fields_internal(&record->fields, 1);
      PBM_MEM_FREEIF(record);
      return PBM_ERROR;
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_ECC_SET_HARDCODED_ECCS

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  inserted or removed.  If uim_present indicates no SIM is present,
  then the longer hardcoded list of emergency numbers is used.  If
  the SIM is inserted, then the shorter list is used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
#ifndef FEATURE_MMGSDI
  //lint -e{715} suppress unused parameters
#endif
void pbm_ecc_set_hardcoded_eccs(boolean uim_present, boolean ecc_present)
{
  typedef struct pbm_ecc_tbl {
    char   *dial_string;
    uint32  string_length;
    boolean already_in_pbm;
  } PBM_ECC_TBL;

  struct pbm_ecc_tbl *current_list;
  int                 list_size;
  PB_CACHE *ecc_cache;
  int i;
  int j;

#ifdef FEATURE_MMGSDI
  PBM_ECC_TBL hardcoded_with_uim[] = {
    { "911",  4, FALSE },               /* 911 is ALWAYS an emergency number */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    { "112",  4, FALSE },               /* 112 is ALWAYS one if GSM/UMTS */
#endif
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    { "*911", 5, FALSE },               /* These are specific to CDMA */
    { "#911", 5, FALSE },
#endif
#if defined (FEATURE_CPHS_UK)
    { "999",  4, FALSE },               /* This is for a CPHS requirement. */
#endif
  };

  PBM_ECC_TBL hardcoded_with_uim_but_no_ecc[] = {
    { "911",  4, FALSE },
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    { "*911", 5, FALSE },
    { "#911", 5, FALSE },
#endif
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    { "112",  4, FALSE },
#endif
#if defined (FEATURE_CPHS_UK)
    { "999",  4, FALSE },               /* This is for a CPHS requirement. */
#endif
  };

#endif
  PBM_ECC_TBL hardcoded_with_no_uim[] = {
    { "911",  4, FALSE },
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    { "*911", 5, FALSE },
    { "#911", 5, FALSE },
#endif
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    { "112",  4, FALSE },
    { "000",  4, FALSE },
    { "08",   3, FALSE },
    { "110",  4, FALSE },
    { "999",  4, FALSE },
    { "118",  4, FALSE },
    { "119",  4, FALSE },
#endif
  };

  // Select the table of ECCs which is used when no SIM is inserted.
#ifdef FEATURE_MMGSDI
  if (uim_present)
  {
    if (ecc_present)
    {
      current_list = hardcoded_with_uim;
      list_size = ARR_SIZE(hardcoded_with_uim);
    }
    else
    {
      current_list = hardcoded_with_uim_but_no_ecc;
      list_size = ARR_SIZE(hardcoded_with_uim_but_no_ecc);
    }
  }
  else
#endif
  {
    current_list = hardcoded_with_no_uim;
    list_size = ARR_SIZE(hardcoded_with_no_uim);
  }

  // Search through the ECC cache for hardcoded entries
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  ecc_cache = pbm_pb_id_to_cache(PBM_ECC_OTHER);
#else
  ecc_cache = pbm_pb_id_to_cache(PBM_ECC);
#endif
  for (i = 1; i <= ecc_cache->num_of_records; i++)
  {
    PBM_MSG_HIGH("Checking ecc cache entry %d rec ptr ptr 0x%x", i, ecc_cache->pb_cache_array[i], 0);
    if (ecc_cache->pb_cache_array[i])
    {
      if (match_found(PBM_CAT_NONE, PBM_FIELD_HARDCODED_ECC, NULL, 0, 0,
                      ecc_cache->pb_cache_array[i]))
      {
        // We found an entry that is a hardcoded ECC
        // If the entry is not in the current list, remove it.
        for (j = 0; j < list_size; j++)
        {
          if (match_found(PBM_CAT_NONE, PBM_FIELD_HARDCODED_ECC,
                          current_list[j].dial_string,
                          (uint16) current_list[j].string_length,
                          PBM_SEARCHTYPE_EXACT,
                          ecc_cache->pb_cache_array[i]))
          {
            current_list[j].already_in_pbm = TRUE;
            break;
          }
        }
        if (j == list_size)
        {
          // The entry was not in the current list, so remove it. */
          if (pbm_cache_delete(ecc_cache,
                     ecc_cache->pb_cache_array[i]->record_id) != PBM_SUCCESS)
          {
            PBM_MSG_ERR("Could not remove cache item.", 0, 0, 0);
          }
        }
      }
    }
  }

  // If any entry is not in the current list, add it.
  for (i = 0; i < list_size; i++)
  {
    if (current_list[i].already_in_pbm == FALSE)
    {
      if (pbm_build_ecc_record(PBM_FIELD_HARDCODED_ECC,
            current_list[i].string_length, current_list[i].dial_string,
            0, NULL, FALSE, 0) != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Error building hardcoded record.", 0, 0, 0);
      }
    }
  }
}

/*===========================================================================
FUNCTION PBM_ECC_CLEAR_ALL_ID

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  removed.  It removes all entries in the emergency_cache that
  have the ID of the given field.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
#ifndef FEATURE_MMGSDI
//lint -e{715} suppress unused parameters
#endif
void pbm_ecc_clear_all_id(pbm_field_id_e_type field_id)
{
#ifdef FEATURE_MMGSDI
  PB_CACHE *ecc_cache;
  int       i;
  pbm_slot_type sim_id = PBM_SLOT_1;
  boolean   uses_sim = FALSE;

  switch (field_id)
  {
    case PBM_FIELD_SIM1_ECC:
      sim_id = PBM_SLOT_1;
      uses_sim = TRUE;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC);
#endif
      break;
    case PBM_FIELD_SIM2_ECC:
      sim_id = PBM_SLOT_2;
      uses_sim = TRUE;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC2);
#endif
      break;
    case PBM_FIELD_NETWORK_ECC:
      uses_sim = FALSE;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      ecc_cache = pbm_pb_id_to_cache(PBM_ECC_OTHER);
#endif
      break;
    default:
      PBM_MSG_ERR("clear_all_id only works for PBM_FIELD_SIM1/2_ECC and PBM_FIELD_NETWORK_ECC", 0, 0, 0);
      return;
  }

  /* If this ECC was not initialized, nothing to do. */
  if (!uses_sim || (ecc_has_entries & PBM_SIMID_TO_SLOT_MASK(sim_id)) != 0)
  {
    // Search through the ECC cache for hardcoded entries
#ifndef FEATURE_ECC_HANDLING_AT_PBM
    ecc_cache = pbm_pb_id_to_cache(PBM_ECC);
#endif
    for (i = 1; i <= ecc_cache->num_of_records; i++)
    {
      if (ecc_cache->pb_cache_array[i])
      {
        if (match_found(PBM_CAT_NONE, field_id, NULL, 0, 0,
                        ecc_cache->pb_cache_array[i]))
        {
          if (pbm_cache_delete(ecc_cache,
                ecc_cache->pb_cache_array[i]->record_id) != PBM_SUCCESS )
          {
            PBM_MSG_ERR("Could not delete ECC entry %d",
                ecc_cache->pb_cache_array[i]->record_id, 0, 0);
          }
        }
      }
    }
  }

  if (uses_sim)
  {
    /* Clear out the flag now. */
    PBM_SIMSLOT_RESET(ecc_has_entries, sim_id);
    PBM_SIMSLOT_RESET(ecc_initialized, sim_id);
    /* Set the hardcoded ECCs to be correct for the current UIM and ECC state. */
    pbm_ecc_set_hardcoded_eccs((ecc_initialized!=0)?TRUE:FALSE,
                               (ecc_has_entries!=0)?TRUE:FALSE);
  }

#endif
  return;
}

/*===========================================================================
FUNCTION PBM_INIT_ECC_NV

DESCRIPTION
  This function is called during boot.  It reads the ECC list as stored
  in NV.

  The idea is to always allow Emergency Calls through by keeping a list
  of valid Emergency Call Codes (ECC's) in NV in order for the ME to
  be able to call an emergency call even if no SIM is installed.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_init_ecc_nv ( void )
{
  int i,j;
  PB_CACHE *cache;
  pbm_return_type ret_val = PBM_SUCCESS;
  uint16 name[PBM_TEXT_SIZE_CHARS];
  uint16 size;

  /* Add all the hardcoded numbers. */
  PBM_MSG_HIGH("Start PBM ECC init",0,0,0);
  pbm_ecc_set_hardcoded_eccs(FALSE, FALSE);
  PBM_MSG_HIGH("Finish PBM ECC init",0,0,0);
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  cache = pbm_pb_id_to_cache(PBM_ECC_OTHER);
#else
  cache = pbm_pb_id_to_cache(PBM_ECC);
#endif
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  /* Read in the numbers from NV. */
  for ( i=0; i < PBM_NV_EMERGENCY_NUMBERS; i++ )
  {
    nv_dial_type   dial_nv;

    memset ((byte *) &dial_nv, 0, sizeof(nv_dial_type));

    dial_nv.address = (byte) ( i + PBM_NV_EMERGENCY_NUMBER_START ) ;
    (void) pbm_get_nv(NV_DIAL_I, (nv_item_type *) &dial_nv);  //lint !e826 for the suspicious ptr convert

    if (dial_nv.num_digits > PBM_MAX_NUM_LENGTH)
      dial_nv.num_digits = PBM_MAX_NUM_LENGTH;

    if (dial_nv.num_digits > 0 && dial_nv.status == NV_EMERGENCY_SD_NUM)
    {
      //Need to add a NULL to the end of dial_nv.
      char *dial_string;
      PBM_MEM_ALLOC(dial_string, dial_nv.num_digits + 1);
      PBM_CHECK_PTR_RET(dial_string, PBM_ERROR_MEM_FULL);

      for (j=0; j<dial_nv.num_digits; j++)
      {
        dial_string[j] = dial_nv.digits[j];
      }
      dial_string[j] = '\0'; //add NULL

      size = (uint16) strlen((char *) dial_nv.letters);
      if (!pbmconvert_pbm8_to_ucs2((uint8 *) dial_nv.letters, size,
                                   name, ARR_SIZE(name)))
      {
        PBM_MSG_ERR("Could not convert all %d octets of the name", size, 0, 0);
      }

      if (pbm_build_ecc_record(PBM_FIELD_NV_ECC, dial_nv.num_digits + 1,
                              dial_string,
                              size, name, FALSE, 0) != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Could not build ECC record", 0, 0, 0);
      }

      PBM_MEM_FREEIF(dial_string);
    }
  }

  PBM_SET_PB_CACHE_READY(cache);

  return ret_val;
}

/*===========================================================================
FUNCTION PBM_INIT

DESCRIPTION
  This function is called during PBM task startup after the task related
  initialization is done.

  - Initialize Caches
  - Register functions with GSDI
  - Initialize EFS based phonebooks
  - Initialize Emergency Call Codes

  Note that UIM based phonebook is read after SIM initialization is complete.

DEPENDENCIES
  pbm_task_init has been called before.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init(void)
{
  pbm_return_type ret_val;
  uint8 i;

  pbm_init_info.pbm_efs_init_done = FALSE;

  for(i=0;i<MAX_UIMS;i++)
    pbm_init_info.pbm_sim_init_done[i] = FALSE;

  /* Initialize caches to zero */
  ret_val = pbm_cache_init();
  PBM_CHECK_ERR_RET(ret_val, "Cache init failed");

  /* Initialize GSDI
   * UIM is initialized based on events we receive from GSDI */
#ifdef FEATURE_MMGSDI
  ret_val = pbm_gsdi_init();
  PBM_CHECK_ERR(ret_val, "GSDI init failed");
#endif

  /* Initialize WMS client */
  ret_val = pbm_wms_init();
  PBM_CHECK_ERR(ret_val, "PBM WMS init failed");

  /* Initialize EFS based phonebook */
  ret_val = pbm_efs_init();
  pbm_init_info.pbm_efs_init_done = TRUE;
  PBM_CHECK_ERR(ret_val, "EFS init failed");

  /* Initialize Emergency Call Codes */
  ret_val = pbm_ecc_init();
  PBM_CHECK_ERR(ret_val, "ECC init failed");

  /* Initialize CM client */
  ret_val = pbm_cm_init();
  PBM_CHECK_ERR(ret_val, "PBM CM init failed");

  return PBM_SUCCESS;
}

#ifdef FEATURE_STA_PBM
/*===========================================================================
FUNCTION PBM_INIT_ECC

DESCRIPTION
  Deprecated legacy function, currently only called from STA.
  This function should not be called from outside PBM.

  The new ECC initialzation function is pbm_ecc_init() and it is called during
  PBM task startup.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init_ecc(void)
{
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_GET_LIST

DESCRIPTION
  Deprecated legacy function, currently only called from STA.
  This function should not be called from outside PBM.

  Data returned in proc_func.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
//lint -e{715} suppress unused parameters
pbm_return_T pbm_get_list(pbm_device_type pb_id,  boolean **isdial,
    int *num_recs)
{
  return PBM_ERROR;
}
#endif /* FEATURE_STA_PBM */

/*===========================================================================
FUNCTION PBM_ECC_INIT

DESCRIPTION
  Initialize Emergency Call Codes.

  The idea is to always allow Emergency Calls through by keeping a list
  of valid Emergency Call Codes (ECC's) in NV in order for the ME to
  be able to call an emergency call even if no SIM is installed.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_ecc_init( void )
{
  /* Clear out the SIM ECC variable. */
#ifdef FEATURE_MMGSDI
  pbm_init_uim_ecc_list();
#endif

  /* Initialize Emergency Call Codes from NV */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&ecc_other_cache,
                                                  PBM_ECC_OTHER,
                                                  PBM_MAX_ECC_NUMBERS,
                                                  FALSE))
#else
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&emergency_cache,
                                                  PBM_ECC,
                                                  PBM_MAX_ECC_NUMBERS,
                                                  FALSE))
#endif
  {
    PBM_MSG_ERR("Error initializing emergency numbers", 0, 0, 0);
  }
  if (pbm_init_ecc_nv() != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Error loading emergency numbers from NV", 0, 0, 0);
  }

  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_INT_RECORD_WRITE

DESCRIPTION
  This function performs the actual write of the record to UIM or EFS.
  It is always called from within PBM Task. The address record is already
  added to cache from within the library function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_int_record_write(pbm_write_record_s_type *write)
{
  pbm_device_type pb_id;
  uint16 rec_id;
  pbm_return_type ret_val;
#ifdef FEATURE_MMGSDI
  pbm_addr_cache_s_type *record;
  int fields_size;
  int index;
  pbm_device_type outgoing_call_mirror[] = { PBM_LND, PBM_LND2, PBM_OCI, PBM_OCI2 };
  pbm_device_type incoming_call_mirror[] = { PBM_ICI, PBM_ICI2 };
  pbm_device_type *mirror = NULL;
  int mirror_size = 0;
  PB_CACHE *cache;
#endif

  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  rec_id = write->rec_id;
  pb_id = pbm_record_id_to_phonebook(rec_id);

  /* This can happen when GSDI sends a refresh */
  if ((ret_val = check_pb_ready(pb_id)) != PBM_SUCCESS)
  {
    pbm_write_notify(write, ret_val);
    pbm_write_cmd_free(write);
  }

  /* Mirror callhistory entry to SIM if it is not a deletion.
   * Because sizes may be different between the SIM and
   * EFS versions, deletions in one file do not necessarily mean
   * a record will be deleted from the mirror file.  Thus, if the
   * SIM file must be deleted, that needs to be done seperately.
   */
#ifdef FEATURE_MMGSDI
  record = write->curr_rec;
  if (write->curr_rec->num_fields)
  {
    switch(pb_id)
    {
      case PBM_DIALED:
        mirror = outgoing_call_mirror;
        mirror_size = ARR_SIZE(outgoing_call_mirror);
        break;
      case PBM_RCVD:
      case PBM_MISS:
        mirror = incoming_call_mirror;
        mirror_size = ARR_SIZE(incoming_call_mirror);
        break;
      default:
        mirror = NULL;
        mirror_size = 0;
        break;
    } /*  switch(pb_id)... */

    if (mirror != NULL)
    {
      for (index = 0; index < mirror_size; index++)
      {
        cache = pbm_pb_id_to_cache(mirror[index]);
        if (!cache)
          continue;

        if (cache->num_of_records > 0 )
        {
          rec_id = PBM_GET_FIRST_AVAILABLE;

          /* Write the fields to SIM cache too.  pbm_record_write() makes a copy */
          fields_size = pbm_calculate_size_from_fields(record->fields, record->num_fields);

          if ( pbm_record_write(mirror[index], &rec_id, (uint16) PBM_CAT_NONE,
                                (uint32) record->num_fields,
                                (uint8*) record->fields, fields_size,
                                NULL, NULL) != PBM_SUCCESS)
          {
            PBM_MSG_ERR("Mirroring of call history entry to SIM %d file failed",
                        mirror[index], 0, 0);
          }
        }
      }
    }
  }
#endif

  /* Now do actual write */
  switch(pb_id)
  {
    case PBM_DIALED:
    case PBM_RCVD:
    case PBM_MISS:
    case PBM_EFS:
    case PBM_SDL:
      ret_val = pbm_record_write_to_efs(write);
      break;

#ifdef FEATURE_MMGSDI
    case PBM_ADN:
    case PBM_SDN:
    case PBM_FDN:
    case PBM_LND:
    case PBM_MSISDN:
    case PBM_MBN:
    case PBM_ADN2:
    case PBM_SDN2:
    case PBM_FDN2:
    case PBM_LND2:
    case PBM_MSISDN2:
    case PBM_MBN2:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_OCI:
    case PBM_ICI:
    case PBM_OCI2:
    case PBM_ICI2:
    case PBM_MBDN:
    case PBM_MBDN2:
#endif /* FEATURE_PBM_USIM_SUPPORT*/
      ret_val = pbm_record_write_to_sim(write);
      break;
#endif /* FEATURE_MMGSDI */

  default:
    PBM_MSG_ERR("Unrecognized phonebook for write, %d.", pb_id, 0, 0);
    ret_val = PBM_ERROR;
    break;
  } /* end switch */

  if (ret_val != PBM_SUCCESS)
  {
    pbm_write_notify(write, ret_val);
    pbm_write_cmd_free(write);
  }
  return ret_val;

}

/*===========================================================================
FUNCTION PBM_WRITE_CMD_FREE

DESCRIPTION
  This function frees any references to cache records and frees
  the command structure itself. It also signals the PBM task to go
  ahead and process the next command.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_cmd_free(pbm_write_record_s_type *write)
{
  PBM_CHECK_PTR_RET(write, VOID);

  if (write->prev_rec)
    (void) pbm_cache_record_free(write->prev_rec);

  if (write->curr_rec)
    (void) pbm_cache_record_free(write->curr_rec);

#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_MEM_FREEIF(write->curr_usim_index);
  PBM_MEM_FREEIF(write->prev_usim_index);
#endif /* FEATURE_PBM_USIM_SUPPORT */

  pbm_cmd_free(PBM_CMD_PTR(write));

  (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
}

/*===========================================================================
FUNCTION PBM_WRITE_NOTIFY

DESCRIPTION
  This function sends the asynchronous result of a PBM write operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_notify(const pbm_write_record_s_type *write, pbm_return_type ret_val)
{
  pbm_notify_event_e_type event;
  pbm_writecb_data_s_type cb_data;

  PBM_CHECK_PTR_RET(write, VOID);

  /* notify async callback passed in with write cmd */
  cb_data.ret = ret_val;
  cb_data.rec_id = write->rec_id;
  cb_data.user_data = write->user_data;

  if (write->proc_func)
    (*write->proc_func)(&cb_data);

  /* Send notifications using registered functions */
  if (ret_val == PBM_SUCCESS)
  {
    /* Check # fields to find if there is a record.
     * If both current and previous records are present, this is an update.
     * If current record is present and previous record is not present,
     * this is an add operation. Otherwise this is a deletion.
     */
    if (write->curr_rec && write->curr_rec->num_fields)
    {
      if (write->prev_rec && write->prev_rec->num_fields)
        event = PBM_EVENT_REC_UPDATE;
      else
        event = PBM_EVENT_REC_ADD;
    }
    else
    {
      event = PBM_EVENT_REC_DELETE;
    }
  }
  else
  {
    event = PBM_EVENT_REC_FAILED;
  }

  PBM_NOTIFY(event, rec_id, write->rec_id);
}

/*===========================================================================
FUNCTION PBM_LIST_BACKUP

DESCRIPTION
  This function makes sure that there is a valid entry in the committed list for
  this record id. If there are no pending writes on this record, the current
  cache entry is saved to committed list. If the previous entry is a NULL, we
  replace it with a dummy entry.

DEPENDENCIES
  Cache should be initialized before. Record id should be valid.
  Should be called from within pbm_crit_sect.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_list_backup(uint16 rec_id)
{
  PB_CACHE *cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  uint16 location = pbm_record_id_to_location(rec_id);
  pbm_addr_node_s_type *curr_node = NULL;
  pbm_addr_node_s_type *prev_node = NULL;
  pbm_addr_node_s_type *node = NULL;
  pbm_addr_cache_s_type *dummy = NULL;
  int committed_list_size = 0;

  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  if (rec_id == PBM_GET_FIRST_AVAILABLE)
    return PBM_ERROR;

  /* Search for a node with given record id or go till the end */
  curr_node = pbm_committed_list;
  while (curr_node)
  {
    if (curr_node->rec_id == rec_id)
      return PBM_SUCCESS; /* We are done */
    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  /* Allocate a new node */
  PBM_MEM_ALLOC(node, sizeof(pbm_addr_node_s_type));
  PBM_CHECK_PTR_RET(node, PBM_ERROR_MEM_FULL);

  /* If entry is NULL, add a dummy entry in cache for future comparison */
  if (!cache->pb_cache_array[location])
  {
    dummy = pbm_cache_record_new();
    if (!dummy)
    {
      PBM_MSG_ERR("Unable to allocate dummy record for 0x%x", rec_id, 0, 0);
      PBM_MEM_FREEIF(node);
      return PBM_ERROR_MEM_FULL;
    }
    pbm_cache_record_fill(dummy, rec_id, 0, 0, 0, NULL);
    cache->pb_cache_array[location] = dummy;
  }

  node->rec_id = rec_id;
  node->record = pbm_cache_record_get(rec_id);
  node->next = NULL;

  /* Add node to committed list (end of list) */
  if (prev_node)
    prev_node->next = node;
  else
    pbm_committed_list = node;

  /* check committed list size */
  node = pbm_committed_list;
  committed_list_size = 0;
  while (node != NULL)
  {
    committed_list_size++;
    node = node->next;
  }
  if (committed_list_size > 10)
  {
    PBM_MSG_ERR("Committed list of unusual size %d", committed_list_size, 0, 0);
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_LIST_UPDATE

DESCRIPTION
  This function updates the PBM committed list after a successful write.

  If the cache entry is same as current write record, there are no other
  pending writes. So we delete the corresponding node from within PBM
  committed list. A dummy record wthin cache is also relaplced with the
  original NULL entry.

  If cache entry is different, we update the corresponding node within
  committed list with the latest record.

DEPENDENCIES
  No critical section dependencies.

SIDE EFFECTS
===========================================================================*/
void pbm_list_update(const pbm_write_record_s_type *write)
{
  pbm_addr_cache_s_type *cache_rec;
  PB_CACHE *cache;
  pbm_addr_node_s_type *curr_node = NULL;
  pbm_addr_node_s_type *prev_node = NULL;
  boolean found = FALSE;
  uint16 location;

  PBM_CHECK_PTR_RET(write, VOID);

  cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(write->rec_id));
  PBM_CHECK_PTR_RET(cache, VOID);

  location = pbm_record_id_to_location(write->rec_id);
  if (location > cache->num_of_records)
  {
    PBM_MSG_HIGH("Location for record ID out of bounds %d of %d", write->rec_id,
             location, cache->num_of_records);
    return;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  cache_rec = cache->pb_cache_array[location];

  if (!cache_rec)
  {
    PBM_MSG_HIGH("NULL cache entry found for record id %d", write->rec_id, 0, 0);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  /* Search for the record id within committed list */
  PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);

  if (!found || !curr_node)
  {
    PBM_MSG_HIGH("Unable to find an entry in committed list for record 0x%x, node 0x%x",
                 write->rec_id, curr_node, 0);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  if (write->curr_rec == cache_rec)
  {
    /* There are no more pending writes.
     * Remove node from committed list. */
    (void) pbm_cache_record_free(curr_node->record);
    PBM_LIST_REMOVE(prev_node, curr_node);

    /* Remove dummy record */
    if (!cache_rec->num_fields)
    {
      (void) pbm_cache_record_free(cache_rec);
      cache->pb_cache_array[location] = NULL;
    }
  }
  else
  {
    /* Update node with new record */
    (void) pbm_cache_record_free(curr_node->record);
    curr_node->record = write->curr_rec;
    curr_node->record->nref++;
  }

  rex_leave_crit_sect(&pbm_crit_sect);
}

/*===========================================================================
FUNCTION PBM_LIST_RECOVER

DESCRIPTION
  This function is called to recover a cache entry in response to a write error.
  The entry is obtained from the PBM committed list.

  If cache entry is same as current write record, there are no other pending
  writes. In this case we have to do the recovery operation. In the normal case
  we take the record from the corresponding node in commit list and add it to
  the cache.

  If the current operation was partially successful ie deletion was successful
  and add failed, we delete the current cache entry. This can happen only
  for an Update operation on EFS or while adding USIM entries.

  In both cases above, we no longer need the node in commiitted list after this
  and hence the node is removed. Also, if the entry in cache is a dummy record,
  it is converted to a NULL.

DEPENDENCIES
  None. No need to be in critical section to call this function.

SIDE EFFECTS
===========================================================================*/
void pbm_list_recover(const pbm_write_record_s_type *write, boolean partial_success)
{
  PB_CACHE *cache;
  pbm_addr_cache_s_type *cache_rec;
  pbm_addr_node_s_type *curr_node = NULL;
  pbm_addr_node_s_type *prev_node = NULL;
  uint16 location;
  boolean found;
  pbm_return_type ret;

  PBM_CHECK_PTR_RET(write, VOID);

  cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(write->rec_id));
  PBM_CHECK_PTR_RET(cache, VOID);

  /* Fix for CR: 164527. Adding Null pointer check for cache->pb_cache_array */
  PBM_CHECK_PTR_RET(cache->pb_cache_array, VOID);

  location = pbm_record_id_to_location(write->rec_id);

  rex_enter_crit_sect(&pbm_crit_sect);

  cache_rec = cache->pb_cache_array[location];
  if (!cache_rec)
  {
    PBM_MSG_HIGH("NULL cache entry found for record id %d", write->rec_id, 0, 0);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  /* make sure there isn't another write to the same record pending */
  if (write->curr_rec == cache_rec)
  {
    /* Search for the record id within committed list */
    PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);
    if (!found || !curr_node)
    {
      PBM_MSG_HIGH("Unable to find an entry in committed list for record 0x%x, node 0x%x",
                   write->rec_id, curr_node, 0);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
    }

    if (partial_success)
    {
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
      if ((write->curr_rec && pbm_is_usim_required(write->rec_id,
                                                   write->curr_rec->fields,
                                                   write->curr_rec->num_fields)) ||
          (write->prev_rec && pbm_is_usim_required(write->rec_id,
                                                   write->prev_rec->fields,
                                                   write->prev_rec->num_fields)))
      {
        /* Recover USIM record when Add, Delete or Update operation failed.*/
        ret = pbm_recover_usim_record(write);
        PBM_CHECK_ERR(ret, "USIM Record Recovery after error failed");
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MGSDI */
      {
        /* Deletion was successful and Add failed for EFS entries.
         * We ignore the commit list in this case. */

        ret = pbm_cache_delete(cache, write->rec_id);
        PBM_CHECK_ERR(ret, "Recovery after partial update failed");
      }
    }
    else
    {
      /* Normal recovery case when Add, Delete or Update operation failed.
       * In this case we add entry from commit list to the cache. */
      ret = pbm_cache_add(cache, curr_node->record, FALSE);
      PBM_CHECK_ERR(ret, "Recovery after error failed");
      curr_node->record->nref++;
    }

    /* we are done with the node in committed list, so it can be removed */
    (void) pbm_cache_record_free(curr_node->record);
    PBM_LIST_REMOVE(prev_node, curr_node);


    /* Remove dummy record, if present */
    if (cache->pb_cache_array[location] &&
        !cache->pb_cache_array[location]->num_fields)
    {
      (void) pbm_cache_record_free(cache->pb_cache_array[location]);
      cache->pb_cache_array[location] = NULL;
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);

}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_RECOVER_USIM_RECORD

DESCRIPTION
  Restores pb cache to previous entry.  Creates a write command for restoring
  data written to SIM.  Backup list mechanism is not used, so if this write
  command request fails, then we give up on trying to fix it.
DEPENDENCIES
  Should be called only from PBM task and from pbm_list_recover.  Only used when
  failure adding, deleting, or updating fails on a current or previous record
  entry that contains a USIM field.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_recover_usim_record(const pbm_write_record_s_type *write)
{
  pbm_write_record_s_type *write_cmd = NULL;
  pbm_device_type          pb_id;
  PB_CACHE                *cache = NULL;
  pbm_return_type          ret;
  pbm_addr_cache_s_type   *prev_rec = NULL;
  pbm_addr_cache_s_type   *record   = NULL;
  uint16                   unique_id;
  uint8                   *curr_index = NULL;
  uint8                   *prev_index = NULL;

  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  pb_id = pbm_record_id_to_phonebook(write->rec_id);

  cache = pbm_pb_id_to_cache(pb_id);
  if (!cache)
  {
    PBM_MSG_ERR("Invalid phonebook id %d", pb_id, 0, 0);
    return PBM_ERROR;
  }

  do
  {
    /* Allocate index array if USIM fields are present in previous or current
     * record and fill it */
    /* There's a race condition here.  If you write 2 records back to back
       that both require allocating USIM fields, the second one will get the
       same copy of the IAP, and will overwrite the first record's fields */
    if (!pbm_allocate_index(write->rec_id, write->prev_rec->fields, write->prev_rec->num_fields,
                            &prev_index, &curr_index) ||
        (curr_index &&
         !pbm_get_usim_index(write->rec_id, write->prev_rec->fields, write->prev_rec->num_fields,
                             curr_index)) ||
        (prev_index &&
         !pbm_copy_usim_index(write->rec_id, prev_index)))
    {
      PBM_MSG_ERR("No space available for USIM fields record %d %d %d",
                  write->rec_id, prev_index, curr_index);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Allocate PBM write command */
    write_cmd = (pbm_write_record_s_type *)
                pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
    if (!write_cmd)
    {
      PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Get a reference to failed current record.
       This will now be our previous record entry for clean up purposes*/
    prev_rec = pbm_cache_record_get(write->rec_id);
    if (!prev_rec)
    {
      PBM_MSG_ERR("Unexpected NULL previous record 0x%x", write->rec_id, 0, 0);
      ret = PBM_ERROR;
      break;
    }

    /* Use our old record as our restoring record */
    record = write->prev_rec;
    if (!record)
    {
      PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
      ret = PBM_ERROR;
      break;
    }

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
    {
      unique_id = 0xFFFF;
    }
    record->unique_id = unique_id;

    /* Restore prevoius record into pbm cache */
    ret = pbm_cache_add(cache, record, FALSE);
    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Cache add failed with error %d.", ret, 0, 0);
      break;
    }
    /* pbm_cache_add doesn't do this, so we need to do this here. */
    record->nref++;

    /* Allocate index array if USIM fields are present in previous or current
     * record and fill it */
    if (pbm_master_usim_cache_update(write->rec_id, prev_index, curr_index)
        != PBM_SUCCESS)
    {
      (void) pbm_cache_delete(cache, write->rec_id);
      PBM_MSG_ERR("Unable to update master usim cache for %d", write->rec_id, 0, 0);
      ret = PBM_ERROR;
      break;
    }

    /* Build and Send restoring PBM command */
    write_cmd->cmd_type   = PBM_CMD_WRITE;
    write_cmd->rec_id     = write->rec_id;
    write_cmd->prev_rec   = prev_rec;
    write_cmd->proc_func  = NULL;
    write_cmd->user_data  = NULL;

    write_cmd->curr_rec        = pbm_cache_record_get(write->rec_id);
    write_cmd->curr_usim_index = curr_index;
    write_cmd->prev_usim_index = prev_index;
    pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

    ret = PBM_SUCCESS;

  } while (0); /*lint !e717 */

  /* Error Handling */
  if (ret != PBM_SUCCESS)
  {
    pbm_free_index(prev_index, curr_index);
    if (write_cmd)
      pbm_cmd_free(PBM_CMD_PTR(write_cmd));
    if (prev_rec)
      (void) pbm_cache_record_free(prev_rec);
    if (record)
      (void) pbm_cache_record_free(record);
  }
  return ret;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_CM_INIT

DESCRIPTION
  Initialize CM Client, for network emergency numbers.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_cm_init( void )
{

#ifdef CM_API_EMERGENCY_NUM_LIST
  cm_client_status_e_type cm_status;

  cm_status = cm_client_init(CM_CLIENT_TYPE_PBM, &pbm_cm_client_id);

  if (cm_status != CM_CLIENT_OK)
  {
    PBM_MSG_ERR("cm_client_init failed, %d %d",cm_status,pbm_cm_client_id,0);
    return PBM_ERROR;
  }

  cm_status = cm_mm_client_ss_reg(pbm_cm_client_id, pbm_cm_ss_event_cb,
                                  CM_CLIENT_EVENT_REG, CM_SS_EVENT_EMERG_NUM_LIST,
                                  CM_SS_EVENT_EMERG_NUM_LIST, NULL);

  if (cm_status != CM_CLIENT_OK)
  {
    PBM_MSG_ERR("cm_mm_client_ss_reg failed, %d %d",cm_status,pbm_cm_client_id,0);
    return PBM_ERROR;
  }

  cm_status = cm_client_act( pbm_cm_client_id );
  if (cm_status != CM_CLIENT_OK)
  {
    PBM_MSG_ERR("cm_client_act failed, %d %d",cm_status,pbm_cm_client_id,0);
    return PBM_ERROR;
  }

#endif

  return PBM_SUCCESS;
}

#ifdef CM_API_EMERGENCY_NUM_LIST
/*===========================================================================
FUNCTION   pbm_cm_ss_event_cb

DESCRIPTION
  This function is called when we get an SS event.
  This will only look at the emergency number event, and add those numbers to
  the emergency number phonebook.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_cm_ss_event_cb(cm_ss_event_e_type ss_event,
                               const cm_mm_ss_info_s_type *ii_info_ptr)
{
  int i;

  switch (ss_event)
  {
  case CM_SS_EVENT_EMERG_NUM_LIST:
    PBM_CHECK_PTR_RET(ii_info_ptr, VOID);

    PBM_MSG_HIGH("PBM got CM_SS_EVENT_EMERG_NUM_LIST, len %d",
             ii_info_ptr->emerg_num_list.num_list_len,0,0);

    /* clear out old network emergency numbers */
    pbm_ecc_clear_all_id(PBM_FIELD_NETWORK_ECC);

    /* add new ones */
    for (i=0; i<ii_info_ptr->emerg_num_list.num_list_len; i++)
    {
      char   emerg_num[sizeof(ii_info_ptr->emerg_num_list.num_list[i].num.buf) + 1];
      uint16 emerg_num_len = ii_info_ptr->emerg_num_list.num_list[i].num.len;

      if (emerg_num_len == 0) {
        PBM_MSG_ERR("emergency number length is 0 for item %d of %d", i, ii_info_ptr->emerg_num_list.num_list_len, 0);
        continue;
      }

      if (emerg_num_len >= sizeof(emerg_num)) {
        PBM_MSG_ERR("Emergency num len from cm is too large, allow %d, reported %d", sizeof(emerg_num)-1, emerg_num_len, 0);
        emerg_num_len = sizeof(emerg_num) - 1;
      }

      memset(emerg_num, 0, sizeof(emerg_num));
      (void)memcpy(emerg_num,
                    (char*)ii_info_ptr->emerg_num_list.num_list[i].num.buf,
                    emerg_num_len);

      if (pbm_build_ecc_record(PBM_FIELD_NETWORK_ECC,
                               emerg_num_len + 1, //Add 1 for NULL
                               emerg_num,
                               0,
                               NULL,
                               TRUE,
                               (uint8)ii_info_ptr->emerg_num_list.num_list[i].num_type) != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Could not build network ECC record.", 0, 0, 0);
      }
    }

    break;

  default:
    PBM_MSG_ERR("Received unknown CM SS event %d", ss_event, 0, 0);
  }
}
#endif /* CM_API_EMERGENCY_NUM_LIST */

/*===========================================================================
FUNCTION PBM_WMS_INIT

DESCRIPTION
  Initialize WMS Client, for serializing SIM access at startup.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_wms_init( void )
{
#ifdef FEATURE_WMS
  wms_client_id_type client_id;
  wms_client_err_e_type wms_result;

  wms_result = wms_client_init( WMS_CLIENT_TYPE_PBM, &client_id );
  if (wms_result != WMS_CLIENT_ERR_NONE)
  {
    PBM_MSG_ERR("wms_client_init failed, %d %d",wms_result,client_id,0);
    return PBM_ERROR;
  }

  wms_result = wms_client_reg_cfg_cb( client_id, pbm_wms_cfg_cb );
  if (wms_result != WMS_CLIENT_ERR_NONE)
  {
    PBM_MSG_ERR("wms_client_reg_cfg_cb failed, %d %d",wms_result,client_id,0);
    return PBM_ERROR;
  }

  wms_result = wms_client_activate( client_id );
  if (wms_result != WMS_CLIENT_ERR_NONE)
  {
    PBM_MSG_ERR("wms_client_activate failed, %d %d",wms_result,client_id,0);
    return PBM_ERROR;
  }
#endif

  return PBM_SUCCESS;
}

#ifdef FEATURE_WMS
/*===========================================================================
FUNCTION pbm_wms_cfg_cb

DESCRIPTION
  WMS callback.  Used to start reading from SIM.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} -e{715} suppress param could be const, param not referenced */
static void pbm_wms_cfg_cb (wms_cfg_event_e_type event,
                            wms_cfg_event_info_s_type * event_info_ptr)
{
#ifdef FEATURE_MMGSDI
  sim_capabilities_T dummy_sim_cap;
#endif /* FEATURE_MMGSDI */

  switch( event ) /* ignore event_info_ptr */
  {
    case WMS_CFG_EVENT_GW_READY:
    case WMS_CFG_EVENT_CDMA_READY:
      if (!pbm_wms_init_finished)
      {
        pbm_wms_init_finished = TRUE;

#ifdef FEATURE_MMGSDI
        if (((pbm_slot_card_inserted & PBM_SIMID_TO_SLOT_MASK(PBM_SLOT_1)) != 0) &&
            ((pbm_slot_init_waiting & PBM_SIMID_TO_SLOT_MASK(PBM_SLOT_1)) == 0))
        {
          /* Initialization happens from within pbm_gsdi_sim_cap_cb since we
           * need to check whether FDN is enabled before starting */
          (void) gsdi_get_sim_capabilities(&dummy_sim_cap, 0, pbm_gsdi_sim_cap_cb);
        }
#if (MAX_UIMS > 1)
        if (((pbm_slot_card_inserted & PBM_SIMID_TO_SLOT_MASK(PBM_SLOT_2)) != 0) &&
            ((pbm_slot_init_waiting & PBM_SIMID_TO_SLOT_MASK(PBM_SLOT_2)) == 0))
        {
          /* Initialization happens from within pbm_gsdi_sim_cap_cb since we
           * need to check whether FDN is enabled before starting */
          (void) gsdi2_get_sim_capabilities(GSDI_SLOT_2, NULL, 0, pbm_gsdi_sim_cap_cb);
        }
#endif /* MAX_UIMS */
#endif /* FEATURE_MMGSDI */
      }

      break;

    default:
      break;
  }
}
#endif /*FEATURE_WMS*/

#endif /* FEATURE_PBM_TASK */
