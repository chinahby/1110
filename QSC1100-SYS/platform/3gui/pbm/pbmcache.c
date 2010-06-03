/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the cache manipulation functions used by PBM

  Copyright (c) 2002 - 2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmcache.c#41 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/01/07   clm     Fix CR130077.  Make it possible to add empty records to
                   circular caches.  This makes EFS and SIM circular record
                   files similar in behavior.
05/11/07   cvs     Fix data abort
03/12/07   cvs     Featurize 4 PB set support
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add event for phonebooks becoming ready
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
06/06/06   cvs     Fix GCF issue with sync rollover
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
03/10/06   cvs     Lint fixes
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
01/25/06   cvs     Lint fixes
11/15/05   cvs     Lint fixes
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/28/05   cvs     Fix field size for Email + SNE
09/22/05   cvs     Fix memory leak
09/14/05   cvs     USIM email fixes (broken with UCS2 changes).
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
04/12/05   AT      Allow PBM_DEFAULT when using legacy functions
                   pbm_file_info and pbm_find_name.
                   Also, only generate a USIM sync value for ADN files.
03/23/05   cvs     better handling of UIDs when regenerating
03/23/05   cvs     compilation fixes
03/22/05   PA      Do not update unique id for deletion/update.
                   Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
03/08/05   PA      Support for PBM notifications.
02/08/05   PA      Support for MSISDN
01/18/05   PA      Fixed typo in error message
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
12/14/04   PA      Update cache value before accessing it.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"

#ifdef FEATURE_PBM_TASK
#include "pbm.h"
#include "pbmcache.h"
#include "pbmutils.h"
#include "pbmuim.h"

/*===========================================================================
                       Declare data for the phone books.
===========================================================================*/

/*===========================================================================
                     Declare data for the EFS based files
===========================================================================*/
PB_CACHE                 efs_cache;            /* Mobile Equip phone */
PB_CACHE                 sdl_cache;            /* Speed Dial List */
PB_CACHE                 incoming_calls_cache; /* Received calls */
PB_CACHE                 outgoing_calls_cache; /* Dailed calls */
PB_CACHE                 missed_calls_cache;   /* Missed calls. */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
//emergency lists will be there 2 for UIMs
PB_CACHE                 ecc_sim_cache[MAX_UIMS];      /* Emergency numbers */
//Network ECC, NV ECC,Hardcoded ECC
PB_CACHE                 ecc_other_cache;      /* Emergency numbers */
#else
PB_CACHE                 emergency_cache;      /* Emergency numbers */
#endif

/*===========================================================================
                     Declare data for the UIM files
This variables will exist whether or not we have any UIMS.  If we don't have
a UIM, the write will fail, but these variables are around because the PBM
interface supports whether the file name whether or not the card is supported
in the build.
===========================================================================*/
PB_CACHE                 adn_cache[MAX_UIMS];
PB_CACHE                 sdn_cache[MAX_UIMS];
PB_CACHE                 fdn_cache[MAX_UIMS];
PB_CACHE                 lnd_cache[MAX_UIMS];
#ifdef FEATURE_PBM_USIM_SUPPORT
PB_CACHE                 oci_cache[MAX_UIMS];
PB_CACHE                 ici_cache[MAX_UIMS];
PB_CACHE                 mbdn_cache[MAX_UIMS];
#endif /* FEATURE_PBM_USIM_SUPPORT */

PB_CACHE                 msisdn_cache[MAX_UIMS];
PB_CACHE                 mbn_cache[MAX_UIMS];

#ifdef FEATURE_MMGSDI
/*===========================================================================
                     Declare data for the UIM extension records.
===========================================================================*/
pbm_ext_cache_s_type     ext1_cache[MAX_UIMS];
pbm_ext_cache_s_type     ext2_cache[MAX_UIMS];
pbm_ext_cache_s_type     ext3_cache[MAX_UIMS];
#ifdef FEATURE_PBM_USIM_SUPPORT
pbm_ext_cache_s_type     ext5_cache[MAX_UIMS];
pbm_ext_cache_s_type     ext6_cache[MAX_UIMS];

/* Master USIM cache for ADN */
PB_MASTER_USIM_CACHE     adn_m_usim_cache[MAX_UIMS][PBM_MAX_NUM_PB_SETS];

/* Right now GSDI and UIM drivers support only a single EMAIL, SNE and GRP file
 * within a USIM phonebook. It supports upto 3 ANR files.
 * PBM will also match this. */
PB_USIM_CACHE            sne_cache[MAX_UIMS][PBM_MAX_NUM_PB_SETS];
PB_USIM_CACHE            anr_cache[MAX_UIMS][PBM_MAX_NUM_SAME_USIM_FILES*(int)PBM_MAX_NUM_PB_SETS];
PB_USIM_CACHE            email_cache[MAX_UIMS][PBM_MAX_NUM_PB_SETS];
PB_USIM_CACHE            grp_cache[MAX_UIMS][PBM_MAX_NUM_PB_SETS];

PB_MASTER_SYNC_CACHE     adn_m_sync_cache[MAX_UIMS];

PB_SYNC1_CACHE           psc_cache[MAX_UIMS];
PB_SYNC1_CACHE           cc_cache[MAX_UIMS];
PB_SYNC1_CACHE           puid_cache[MAX_UIMS];
PB_SYNC2_CACHE           uid_cache[MAX_UIMS];
PB_SYNC2_CACHE           pbc_cache[MAX_UIMS];
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================*
 *              PHONE BOOK MANAGER CACHING FUNCTIONS                         *
 *                                                                           *
 * These functions will maintain the cache internally in PBM.                *
 *===========================================================================*/

/* <EJECT> */
/*===========================================================================
FUNCTION PBM_CACHE_RECORD_NEW

DESCRIPTION
  This function allocates a new cache record, initializes it 0 and sets the
  number of reference to 1.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_addr_cache_s_type *pbm_cache_record_new(void)
{
  pbm_addr_cache_s_type *record = NULL;

  PBM_MEM_ALLOC(record, sizeof(pbm_addr_cache_s_type));

  PBM_CHECK_PTR_RET(record, NULL);

  memset(record, 0, sizeof(pbm_addr_cache_s_type));

  record->nref = 1;

  return record;
}

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_FILL

DESCRIPTION
  This function populates a cache record with the given values.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_cache_record_fill(pbm_addr_cache_s_type *rec,
                           uint16 rec_id, uint16 uid, uint16 cat, uint16 num,
                           pbm_field_s_type *fields)
{
  PBM_CHECK_PTR_RET(rec, VOID);

  rec->unique_id = uid;
  rec->record_id = rec_id;
  rec->category = cat;
  rec->num_fields = num;
  rec->fields = fields;
}

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_GET

DESCRIPTION
  This function obtains a reference to an existing cache record

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_addr_cache_s_type *pbm_cache_record_get(uint16 rec_id)
{
  PB_CACHE *cache;
  pbm_addr_cache_s_type *record;
  uint16 location;

  cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  PBM_CHECK_PTR_RET(cache, NULL);

  location = pbm_record_id_to_location(rec_id);
  if (location == 0 || location > cache->num_of_records) {
    PBM_MSG_ERR("Invalid location %d", location, 0, 0);
    return NULL;
  }

  record = cache->pb_cache_array[location];

  if (record)
  {
    record->nref++;
  }
  else
  {
    PBM_MSG_ERR("Previous record is NULL", 0, 0, 0);
  }

  return record;
}

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_FREE

DESCRIPTION
  This function frees a cache record

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint8 pbm_cache_record_free(pbm_addr_cache_s_type *rec)
{
  if (!rec)
    return 0;

  if (--rec->nref)
    return rec->nref;

  pbm_free_fields_internal(&rec->fields, rec->num_fields);
  PBM_MEM_FREEIF(rec);

  return 0;
}

/*===========================================================================
FUNCTION PBM_CACHE_INIT

DESCRIPTION
  This function initializes all SIM based caches to zeros.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_init(void)
{
  PBM_SIM_CACHE_INIT(adn_cache, PBM_FILE_ADN);
  PBM_SIM_CACHE_INIT(sdn_cache, PBM_FILE_SDN);
  PBM_SIM_CACHE_INIT(msisdn_cache, PBM_FILE_MSISDN);
  PBM_SIM_CACHE_INIT(mbn_cache, PBM_FILE_MBN);
  PBM_SIM_CACHE_INIT(fdn_cache, PBM_FILE_FDN);
  PBM_SIM_CACHE_INIT(lnd_cache, PBM_FILE_LND);
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_SIM_CACHE_INIT(mbdn_cache, PBM_FILE_MBDN);
  PBM_SIM_CACHE_INIT(ici_cache, PBM_FILE_ICI);
  PBM_SIM_CACHE_INIT(oci_cache, PBM_FILE_OCI);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  PBM_SIM_CACHE_INIT(ecc_sim_cache, PBM_FILE_ECC);
#endif
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
  memset(adn_m_usim_cache, 0, sizeof(adn_m_usim_cache));

  memset(sne_cache, 0, sizeof(sne_cache));
  memset(anr_cache, 0, sizeof(anr_cache));
  memset(email_cache, 0, sizeof(email_cache));
  memset(grp_cache, 0, sizeof(grp_cache));

  memset(adn_m_sync_cache, 0, sizeof(adn_m_sync_cache));

  memset(psc_cache, 0, sizeof(psc_cache));
  memset(cc_cache, 0, sizeof(cc_cache));
  memset(puid_cache, 0, sizeof(puid_cache));
  memset(uid_cache, 0, sizeof(uid_cache));
  memset(pbc_cache, 0, sizeof(pbc_cache));
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

  return PBM_SUCCESS;
}

/* <EJECT> */
/*===========================================================================
FUNCTION PBM_CLEAR_CACHE

DESCRIPTION
  This function is called to remove all the entries in the cache.
  It does not change the contents of the SIM, just frees the memory
  associated with the cache.

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
  All cache items are cleared.  Phone books are marked as not initialized.
  Any search item that was conducted will no longer be able to see the
  cache values.
===========================================================================*/
void pbm_clear_cache(PB_CACHE *cache)
{
  int i;
  pbm_addr_node_s_type *curr = NULL;
  pbm_addr_node_s_type *prev = NULL;
  pbm_addr_node_s_type *tmp = NULL;
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_pb_set_type pb_set;
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif
  pbm_device_type pb_id;

  PBM_CHECK_PTR_RET(cache, VOID);

  cache->status = PBM_STATUS_NOT_INIT;
  if (cache->pb_cache_array != NULL)
  {
    /* Free all the records. */
    for (i = 1; i <= cache->num_of_records; i++)
    {
      if (cache->pb_cache_array[i] != NULL)
      {
        (void) pbm_cache_delete(cache, cache->pb_cache_array[i]->record_id);
      }
    }

    /* Free the array of cache pointers itself. */
    PBM_MEM_FREEIF(cache->pb_cache_array);
  }

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
  /* Free master USIM caches */
  for (pb_set = PBM_USIM_PB_ONE; pb_set < PBM_MAX_NUM_PB_SETS; pb_set++)
    pbm_clear_index_cache(cache->m_usim_cache[pb_set]);
  pbm_clear_sync_cache(cache->m_sync_cache);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

  // zero out cache, preserving the pb_id that may have been set
  pb_id = cache->pb_id;
  memset(cache, 0, sizeof(*cache));
  cache->pb_id = pb_id;

  /* Remove records of this cache from committed list, if present */
  curr = pbm_committed_list;
  while (curr)
  {
    if (cache == pbm_pb_id_to_cache(pbm_record_id_to_phonebook(curr->rec_id)))
    {
      (void) pbm_cache_record_free(curr->record);
      tmp = curr->next;
      PBM_LIST_REMOVE(prev, curr);
      curr = tmp;
    }
    else
    {
      prev = curr;
      curr = curr->next;
    }
  }

} /* pbm_clear_cache */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_CLEAR_USIM_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_usim_cache(PB_USIM_CACHE *cache)
{
  int i;

  PBM_CHECK_PTR_RET(cache, VOID);

  if (cache->pb_cache_array != NULL)
  {
    /* Free all the records. */
    for (i = 1; i <= cache->num_of_records; i++)
    {
      if (cache->pb_cache_array[i] != NULL)
      {
        (void) pbm_usim_cache_delete(cache, (uint16)i, TRUE);
      }
    }

    /* Free the array of cache pointers itself. */
    PBM_MEM_FREEIF(cache->pb_cache_array);
  }

  /* Master usim cache is not freed here, this is done as part of
   * clearing ADN cache */

  memset(cache, 0, sizeof(*cache));

} /* pbm_clear_usim_cache */

/*===========================================================================
FUNCTION pbm_clear_index_cache

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_index_cache(PB_MASTER_USIM_CACHE *m_usim_cache)
{
  int i;

  if (m_usim_cache)
  {
    PBM_MEM_FREEIF(m_usim_cache->usim_cache_array);

    if (m_usim_cache->index_array != NULL)
    {
      for (i = 1; i <= m_usim_cache->num_caches; i++)
        PBM_MEM_FREEIF(m_usim_cache->index_array[i]);
    }
    PBM_MEM_FREEIF(m_usim_cache->index_array);
    memset(m_usim_cache, 0, sizeof(PB_MASTER_USIM_CACHE));
  }
}

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync_cache(PB_MASTER_SYNC_CACHE *m_sync_cache)
{
  if (m_sync_cache)
  {
    pbm_clear_sync1_cache(m_sync_cache->psc_cache);
    pbm_clear_sync1_cache(m_sync_cache->puid_cache);
    pbm_clear_sync1_cache(m_sync_cache->cc_cache);

    pbm_clear_sync2_cache(m_sync_cache->pbc_cache);
    pbm_clear_sync2_cache(m_sync_cache->uid_cache);

    memset(m_sync_cache, 0, sizeof(PB_MASTER_SYNC_CACHE));
  }
}

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC1_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync1_cache(PB_SYNC1_CACHE *sync1_cache)
{
  if (sync1_cache)
    memset(sync1_cache, 0, sizeof(PB_SYNC1_CACHE));
}

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC2_CACHE

DESCRIPTION
  This function will clear the sync2 cache.

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync2_cache(PB_SYNC2_CACHE *sync2_cache)
{
  if (sync2_cache)
  {
    PBM_MEM_FREEIF(sync2_cache->data_array);
    memset(sync2_cache, 0, sizeof(PB_SYNC2_CACHE));
  }
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION PBM_PB_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_pb_id_to_cache(pbm_device_type pb_id)
{
  PB_CACHE *cache = NULL;
  pbm_slot_type sim_id;

  switch (pb_id)
  {
    case PBM_EFS:
      cache = &efs_cache;
      break;
    case PBM_SDL:
      cache = &sdl_cache;
      break;
    case PBM_MISS:
      cache = &missed_calls_cache;
      break;
    case PBM_RCVD:
      cache = &incoming_calls_cache;
      break;
    case PBM_DIALED:
      cache = &outgoing_calls_cache;
      break;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC:
    case PBM_ECC2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &ecc_sim_cache[sim_id];
      break;
    case PBM_ECC_OTHER:
      cache = &ecc_other_cache;
      break;
#else
    case PBM_ECC:
      cache = &emergency_cache;
      break;
#endif
    case PBM_ADN:
    case PBM_ADN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &adn_cache[sim_id];
      break;
    case PBM_SDN:
    case PBM_SDN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &sdn_cache[sim_id];
      break;
    case PBM_FDN:
    case PBM_FDN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &fdn_cache[sim_id];
      break;
    case PBM_LND:
    case PBM_LND2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &lnd_cache[sim_id];
      break;
    case PBM_MSISDN:
    case PBM_MSISDN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &msisdn_cache[sim_id];
      break;
    case PBM_MBN:
    case PBM_MBN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &mbn_cache[sim_id];
      break;
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_ICI:
    case PBM_ICI2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &ici_cache[sim_id];
      break;
    case PBM_OCI:
    case PBM_OCI2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &oci_cache[sim_id];
      break;
    case PBM_MBDN:
    case PBM_MBDN2:
      sim_id = pbm_pb_id_to_sim_id(pb_id);
      PBM_VALIDATE_SLOT_RET(sim_id, NULL);
      cache = &mbdn_cache[sim_id];
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    default:
      return NULL;
  }
  return cache;
}

/*===========================================================================
FUNCTION PBM_GET_FIRST_AVAILABLE

DESCRIPTION
Gets the first slot that is empty.  Returns the start index
for circular caches, or returns the index of first unused
area for non-circular.  Finally, returns 0 if not available

In case of deletion of circular cache, this functions returns the last location.
This is not valid for non-circular caches.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_get_first_available(PB_CACHE *pb_cache, boolean delete)
{
  pbm_addr_cache_s_type **cache;
  int cache_size;
  uint16 location;

  PBM_CHECK_PTR_RET(pb_cache, 0);

  if (!pb_cache->circular && delete)
  {
    PBM_MSG_ERR("PBM Deletion of first available not possible for cache 0x%x",
        pb_cache, 0, 0);
    return 0;
  }

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  if (pb_cache->circular) {
    if (delete)
      return (uint16) PBM_CIRC_CACHE_LASTLOC(pb_cache);
    else
      return (uint16) PBM_CIRC_CACHE_NEWLOC(pb_cache);
  }

  for (location = 1; location <= cache_size; location++)
    if (cache[location] == NULL || cache[location]->num_fields == 0)
      break;

  if (location > cache_size)  /* No empty slot was found*/
  {
    return 0;
  }

  return location;
}


#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_GET_FIRST_AVAILABLE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_usim_get_first_available(PB_USIM_CACHE *usim_cache)
{
  pbm_usim_cache_s_type **cache;
  int cache_size;
  uint16 location;

  PBM_CHECK_PTR_RET(usim_cache, 0);

  cache = usim_cache->pb_cache_array;
  cache_size = usim_cache->num_of_records;

  for (location = 1; location <= cache_size; location++)
    if (cache[location] == NULL)
      break;

  if (location > cache_size)  /* No empty slot was found*/
    return 0;

  return location;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION PBM_CACHE_ADD

DESCRIPTION
Adds the given record to the given cache.  First allocates a new record
and copies over the info. from the given record.

If a record already exists at the location specified by the record ID, then
the existing record is first deleted before the new record is added at that
same location.

If the "location" part of the record ID is 0, then the record is placed in
the first available location and the record ID is updated accordingly.  (This
is a 1-based array.)

new_flag is used to support updating existing records.

DEPENDENCIES
  Must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_add(PB_CACHE *pb_cache,
                           pbm_addr_cache_s_type *record, boolean new_flag)
{
  pbm_addr_cache_s_type **cache;
  int cache_size;
  uint16 location;

  PBM_CHECK_PTR2_RET(pb_cache, record, PBM_ERROR);

  if (record->nref == 0)
  {
    PBM_MSG_ERR("Trying to add invalid record %d", record->record_id, 0, 0);
    return PBM_ERROR;
  }

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  /* Where is it stored? */
  location = pbm_record_id_to_location(record->record_id);

  if (location > cache_size || location == 0)
  {
    PBM_MSG_ERR("PBM: Invalid record ID %d for this phonebook.", location, 0, 0);
    return PBM_ERROR_INDEX_INVALID;
  }

  /* If that location is in use, delete entry before overwriting*/
  if (cache[location] != NULL)
  {
    if ( pbm_cache_delete(pb_cache, record->record_id) != PBM_SUCCESS )
    {
      return PBM_ERROR;
    }
  }

  /* Save the record in the cache location. */
  cache[location] = record;

  /* Update records in use only if we are deleting a non-dummy record */
  if (cache[location]->num_fields)
  {
    pb_cache->records_in_use++;
  }

  /* If new_flag is false, that means we are modifying an existing entry. */
  /* BREW supports this odd thing, but this flag makes it work. */
  if (pb_cache->circular && new_flag)
  {
    PBM_CIRC_CACHE_ADDSTART(pb_cache);       /* new_flag is true, so this
                                                   changes where we point. */
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_CACHE_DELETE

DESCRIPTION
  Deletes the record with the given record ID from the given cache.  The record
  is also free'ed.

DEPENDENCIES
  Must be called from within pbm critical section.

  Make sure that USIM fields are overwritten/deleted before the
  corresponding ADN fields. The field data resides as part of ADN record
  except after init.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_delete(PB_CACHE *pb_cache, uint16 record_id)
{
  pbm_addr_cache_s_type **cache;
  int cache_size;
  int location;
  uint8 ref;

  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  cache = pb_cache->pb_cache_array;
  cache_size = pb_cache->num_of_records;

  location = pbm_record_id_to_location(record_id);

  if ( (location > cache_size) || (location < 1) )
  {
    PBM_MSG_ERR("Could not delete a location from record_id 0x%x",record_id,0,0);
    return PBM_ERROR_INDEX_INVALID;
  }

  if (!cache[location])
  {
    PBM_MSG_ERR("Record not found at location from record_id 0x%x",record_id,0,0);
    return PBM_ERROR_NOT_FOUND;
  }

  /* Update records in use only if we are deleting a non-dummy record */
  if (cache[location]->num_fields)
  {
    /* If we are deleting the firt entry in a circular cache, "decrement" value
     * of start. Currently this can happen only while recovering from an Add
     * failure on first available location. */
    if (pb_cache->circular && location == PBM_CIRC_CACHE_FIRSTLOC(pb_cache))
      PBM_CIRC_CACHE_DELSTART(pb_cache);

    pb_cache->records_in_use--;
  }

  ref = pbm_cache_record_free(cache[location]);
  if (ref != 0) {
    MSG_MED("%d references remaining in record %d", ref, record_id, 0);
  }

  cache[location] = NULL;

  return PBM_SUCCESS;
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_CACHE_ADD

DESCRIPTION

DEPENDENCIES
  Must be called only from PBM task
  and it must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_add(PB_USIM_CACHE *usim_cache, uint16 location,
                                   pbm_usim_cache_s_type *record, boolean init)
{
  pbm_usim_cache_s_type **cache_array;
  pbm_return_type ret;

  PBM_CHECK_PTR2_RET(usim_cache, record, PBM_ERROR);

  if (location > usim_cache->num_of_records || location == 0)
  {
    PBM_MSG_ERR("Invalid location %d for USIM filename %d, num of records %d", location, usim_cache->filename, usim_cache->num_of_records);
    return PBM_ERROR_INDEX_INVALID;
  }

  /* Update Master USIM cache */
  if (init)
  {
    ret = pbm_master_usim_cache_set(usim_cache, record->m_recid,
                                       (uint8)location);
    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to update master cache for ADN %d, USIM %d, loc %d",
                  record->m_recid, usim_cache->filename, location);
      return ret;
    }
  }

  /* Update USIM cache */
  cache_array = usim_cache->pb_cache_array;
  if (cache_array[location] != NULL)
  {
    PBM_MSG_HIGH("Found a record in location %d for USIM %d", location,
              usim_cache->file_id, 0);
    if ( pbm_usim_cache_delete(usim_cache, location, init) != PBM_SUCCESS )
    {
      if (init)
         (void) pbm_master_usim_cache_set(usim_cache, record->m_recid, 0);

      return PBM_ERROR;
    }
  }

  if (init)
    usim_cache->records_in_use++;
  cache_array[location] = record;

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_USIM_CACHE_DELETE

DESCRIPTION


DEPENDENCIES
  Must be called from within pbm critical section and in the
  PBM task.

  Make sure that USIM fields are overwritten/deleted before the
  corresponding ADN fields. The field data resides as part of ADN record
  except after init.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_delete(PB_USIM_CACHE *usim_cache,
                                      uint16 location, boolean init)
{
  pbm_usim_cache_s_type **cache_array;
  pbm_return_type ret;

  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  if (location == 0 || location > usim_cache->num_of_records)
  {
    PBM_MSG_ERR("Could not delete entry from location %d, USIM %d",
                 location, usim_cache->filename, 0);
    return PBM_ERROR_INDEX_INVALID;
  }

  cache_array = usim_cache->pb_cache_array;
  if (!cache_array[location])
  {
    PBM_MSG_ERR("Record not found at location %d, USIM %d", location,
                usim_cache->filename, 0);
    return PBM_ERROR_NOT_FOUND;
  }

  /* Update Master USIM cache */
  if (init)
  {
    ret = pbm_master_usim_cache_set(usim_cache,
                                       cache_array[location]->m_recid, 0);
    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to update master cache for ADN %d, USIM %d loc %d",
                  cache_array[location]->m_recid, usim_cache->filename,
                  location);
      return ret;
    }
  }

  PBM_MEM_FREEIF(cache_array[location]->fields);
  PBM_MEM_FREEIF(cache_array[location]);
  if (init)
    usim_cache->records_in_use--;

  cache_array[location] = NULL;


  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_master_usim_cache_set

DESCRIPTION


DEPENDENCIES
  Must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_master_usim_cache_set(PB_USIM_CACHE *usim_cache,
                                             uint16 adn_recid,
                                             uint8 location)
{
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_CACHE *pb_cache;

  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);
  if (!usim_cache->num_of_records)
  {
    PBM_MSG_ERR("No entries in usim cache 0x%x", usim_cache, 0, 0);
    return PBM_ERROR;
  }

  /* Get master record */
  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(adn_recid));
  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  /* Update Master cache */
  m_usim_cache = pb_cache->m_usim_cache[pbm_record_id_to_pb_set(adn_recid)];
  PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

  if (!usim_cache->order || usim_cache->order > m_usim_cache->num_caches)
  {
    PBM_MSG_ERR("Invalid order in USIM cache %d, %d", usim_cache->order,
                m_usim_cache->num_caches, 0);
    return PBM_ERROR;
  }

  m_usim_cache->index_array[usim_cache->order]
                           [pbm_record_id_to_fileloc(adn_recid)] = location;

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_CLEAR_AND_SET_SIM_CACHES

DESCRIPTION
  This function is called to clear all the SIM caches and set the cache status
  to the one specified.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_clear_and_set_sim_caches(pbm_slot_type sim_id,
                                  pbm_status_e_type status)
{
  int i;
  PB_CACHE *cache;

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  rex_enter_crit_sect(&pbm_crit_sect);

  for (i = 0; i < (int)ARR_SIZE(pbm_sim_pb_files); i++) {
    cache = (PB_CACHE *) pbm_file_id_to_cache(sim_id, pbm_sim_pb_files[i]);
    if (cache == NULL)
    {
      PBM_MSG_ERR("NULL cache", 0, 0, 0);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
    }
    pbm_clear_cache(cache);
    cache[sim_id].status = status;
  }
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_clear_usim_caches(sim_id);
#endif /* FEATURE_PBM_USIM_SUPPORT */

  rex_leave_crit_sect(&pbm_crit_sect);
}

/*===========================================================================
FUNCTION PBM_CLEAR_SIM_CACHES

DESCRIPTION
  This function is called to remove all the entries in the cache
  related to the SIM.  It does not change the contents of the SIM,
  just frees the memory associated with the cache.

DEPENDENCIES
  None.

SIDE EFFECTS
  All cache items are cleared.  Phone books are marked as not initialized.
  Any search item that was conducted will no longer be able to see the
  cache values.
===========================================================================*/
void pbm_clear_sim_caches(void)
{
  unsigned int i;
  pbm_slot_type sim_id;
  PB_CACHE *cache;

  for (i = 0; i < ARR_SIZE(pbm_sim_pb_files); i++)
  {
    for (sim_id = PBM_FIRST_SLOT; sim_id < PBM_NUM_SLOTS; sim_id++)
    {
      cache = (PB_CACHE *) pbm_file_id_to_cache(sim_id,
                                                pbm_sim_pb_files[i]);
      PBM_CHECK_PTR_RET(cache, VOID);
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_cache(cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }
  }

#ifdef FEATURE_PBM_USIM_SUPPORT
  for (sim_id = PBM_FIRST_SLOT; sim_id < PBM_NUM_SLOTS; sim_id++)
    pbm_clear_usim_caches(sim_id);
#endif /* FEATURE_PBM_USIM_SUPPORT */


} /* pbm_clear_sim_caches */

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_CLEAR_USIM_CACHES

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_clear_usim_caches(pbm_slot_type sim_id)
{
  int i;
  pbm_pb_set_type pb_set;

  pbm_file_device_e_type usim_files[] = {
    PBM_FILE_SNE,   PBM_FILE_SNE1,
    PBM_FILE_GRP,   PBM_FILE_GRP1,
    PBM_FILE_ANR,   PBM_FILE_ANR1,
    PBM_FILE_ANRA,  PBM_FILE_ANRA1,
    PBM_FILE_ANRB,  PBM_FILE_ANRB1,
    PBM_FILE_ANRC,  PBM_FILE_ANRC1,
    PBM_FILE_EMAIL, PBM_FILE_EMAIL1,
#ifdef FEATURE_USIM_1000_PBM
    PBM_FILE_SNE2,   PBM_FILE_SNE3,
    PBM_FILE_GRP2,   PBM_FILE_GRP3,
    PBM_FILE_ANR2,   PBM_FILE_ANR3,
    PBM_FILE_ANRA2,  PBM_FILE_ANRA3,
    PBM_FILE_ANRB2,  PBM_FILE_ANRB3,
    PBM_FILE_ANRC2,  PBM_FILE_ANRC3,
    PBM_FILE_EMAIL2, PBM_FILE_EMAIL3,
#endif
  };

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  for (i = 0; i < (int)ARR_SIZE(usim_files); i++)
  {
    PB_USIM_CACHE *usim_cache = pbm_file_id_to_cache(sim_id, usim_files[i]);
    if (usim_cache)
    {
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_usim_cache(usim_cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  for (pb_set = PBM_USIM_PB_ONE; pb_set < PBM_MAX_NUM_PB_SETS; pb_set++)
  {
    pbm_clear_index_cache(&adn_m_usim_cache[sim_id][pb_set]);
  }
  pbm_clear_sync_cache(&adn_m_sync_cache[sim_id]);

  rex_leave_crit_sect(&pbm_crit_sect);
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
/*===========================================================================
FUNCTION PBM_CACHE_RECORD_ADD

DESCRIPTION
  Takes in an ADN record, adds USIM fields corresponding to it and adds it to
  cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_cache_record_add( pbm_device_type pb_id,
                              const pbm_num_text_record_s_type *record,
                              int location )
{
  int                           num_fields = 0;
  int                           i = 0;
#ifdef FEATURE_PBM_USIM_SUPPORT
  int                           j, k;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  boolean                       call_history = FALSE;
  uint32                       *field_lengths = NULL;
  PB_CACHE                     *pb_cache = pbm_pb_id_to_cache(pb_id);
  pbm_addr_cache_s_type        *node = NULL;
  boolean                       ret = TRUE;
  uint16                        rec_id;
  pbm_pb_set_type               pb_set;
#ifdef FEATURE_PBM_USIM_SUPPORT
  int                           usim_index;
  int                           num_usim_fields = 0;
  PB_USIM_CACHE                *usim_cache = NULL;
  pbm_usim_cache_s_type        *usim_rec = NULL;
  PB_MASTER_USIM_CACHE         *m_usim_cache = NULL;
  pbm_field_s_type            **usim_fields = NULL;
  pbm_usim_cache_s_type       **usim_recs = NULL;
  int                           num_usim_recs = 0;
#endif /* FEATURE_PBM_USIM_SUPPORT */

  PBM_CHECK_PTR_RET(pb_cache, FALSE);
  PBM_CHECK_PTR_RET(record, FALSE);

  if (location == 0 || location > pb_cache->num_of_records)
  {
    PBM_MSG_ERR("Invalid record index %d in phonebook %d", location,
                pb_id, 0);
    return FALSE;
  }

  if (( record->number[0] == '\0' ) && (record->text[0] == 0x0000))
    return TRUE;

  rec_id = pbm_location_to_record_id(pb_id, location);

  do
  {
    node = pbm_cache_record_new();
    PBM_CHECK_PTR_BREAK(node, ret, FALSE);

    pb_set = pbm_pb_record_index_to_pb_set(pb_cache,location);
    PBM_CHECK_PBSET_RETCODE_BREAK(pb_set, ret, FALSE);

#ifdef FEATURE_PBM_USIM_SUPPORT
    m_usim_cache = pb_cache->m_usim_cache[pb_set];
    if (m_usim_cache && !pbm_gsm_modified(rec_id))
    {
      PBM_MEM_ALLOC(usim_fields, PBM_MAX_USIM_FIELDS_PER_REC *
                                 sizeof(pbm_field_s_type *));
      PBM_CHECK_PTR_BREAK(usim_fields, ret, FALSE);

      PBM_MEM_ALLOC(usim_recs, PBM_MAX_USIM_FIELDS_PER_REC *
                                 sizeof(pbm_usim_cache_s_type *));
      PBM_CHECK_PTR_BREAK(usim_recs, ret, FALSE);

      /* Obtain number of USIM fields, field pointers, number of
       * USIM records and USIM record pointers */
      for (i = 1; i <= m_usim_cache->num_caches; i++)
      {
        usim_cache = m_usim_cache->usim_cache_array[i];
        if (!usim_cache)
          continue;

        usim_index = m_usim_cache->
                    index_array[i][pbm_pb_record_index_to_fileloc(pb_cache, location)];
        if (usim_index)
        {
          boolean added_rec = FALSE;

          usim_rec = usim_cache->pb_cache_array[usim_index];
          if (usim_rec)
          {
            for (j = 0; j < usim_rec->num_fields; j++)
            {
              usim_fields[num_usim_fields++] = &usim_rec->fields[j];
              added_rec = TRUE;
            }
            if (added_rec)
              usim_recs[num_usim_recs++] = usim_rec;
          }
          else
          {
            PBM_MSG_ERR("Invalid record found in USIM %d index %d",
                         usim_cache->file_id, usim_index, 0);
          }
        }
      }
    }

    num_fields = num_usim_fields;
#endif /* FEATURE_PBM_USIM_SUPPORT */

    /* Obtain number of SIM fields */
    switch (pb_id)
    {
      case PBM_ADN:
        node->category = (uint16) PBM_CAT_SIM_ADN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_ADN2:
        node->category = (uint16) PBM_CAT_SIM_ADN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;

      case PBM_SDN:
        node->category = (uint16) PBM_CAT_SIM_SDN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_SDN2:
        node->category = (uint16) PBM_CAT_SIM_SDN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;

      case PBM_FDN:
        node->category = (uint16) PBM_CAT_SIM_FDN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_FDN2:
        node->category = (uint16) PBM_CAT_SIM_FDN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;

      case PBM_LND:
      case PBM_LND2:
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_OCI:
      case PBM_OCI2:
#endif /* FEATURE_PBM_USIM_SUPPORT */
        node->category = (uint16) PBM_CAT_OUTGOING;
        num_fields += PBM_CALLHISTORY_NUM_FIELDS;
        call_history = TRUE;
        break;

      case PBM_MSISDN:
        node->category = (uint16) PBM_CAT_SIM_MSISDN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MSISDN2:
        node->category = (uint16) PBM_CAT_SIM_MSISDN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;

      case PBM_MBN:
        node->category = (uint16) PBM_CAT_SIM_MBN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MBN2:
        node->category = (uint16) PBM_CAT_SIM_MBN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC:
    case PBM_ECC2:
        node->category = (uint16) PBM_CAT_ECC;
        num_fields += PBM_SIM_NUM_FIELDS;//this needs to checked whether it is required or not
        break;
#endif
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_ICI:
      case PBM_ICI2:
        node->category = (uint16) PBM_CAT_INCOMING;
        num_fields += PBM_CALLHISTORY_NUM_FIELDS;
        call_history = TRUE;
        break;
      case PBM_MBDN:
        node->category = (uint16) PBM_CAT_SIM_MBDN;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MBDN2:
        node->category = (uint16) PBM_CAT_SIM_MBDN2;
        num_fields += PBM_SIM_NUM_FIELDS;
        break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

      default:
        PBM_MSG_ERR("Unknown category.  Searches will be interesting. %d", pb_id, 0, 0);
        node->category = (uint16) PBM_CAT_NONE;
        break;
    }

    /* We have the number of fields now.
     * Allocate field_lengths array and fill in data sizes */
    PBM_MEM_ALLOC(field_lengths, (uint32)num_fields*sizeof(uint32));
    PBM_CHECK_PTR_BREAK(field_lengths, ret, FALSE);

#ifdef FEATURE_PBM_USIM_SUPPORT
    for (i = 0; i < num_usim_fields; i++) {
      if (usim_fields != NULL) {
          field_lengths[i] = usim_fields[i]->data_len;
      }
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */

    /* Allocate 4 fields for an ADN record.
     * 1 for the Name.          ( Type string. )
     * 1 for the GSM_NUMBER     ( Type string. )
     * 1 for the GSM_TON_NPI    ( Type Byte. )
     * 1 for Index              ( Type Word. )
     */
    if ((field_lengths[i] = pbmutils_wstrlen(record->text)) > 0)
    {
      field_lengths[i] = (MIN(field_lengths[i], PBM_MAX_TEXT_LENGTH) + 1);
      field_lengths[i] *= sizeof(uint16);
      i++;
    }

    if ((field_lengths[i] = strlen(record->number)) > 0)
    {
      field_lengths[i] = (MIN(field_lengths[i], PBM_MAX_NUM_LENGTH) + 1);
      i++;
    }

    field_lengths[i++] = 1;       /* GSM_TON_NPI is 1 byte always. */
    field_lengths[i++] = 2;       /* Index */

    /* Get two more fields for call history */
    if (call_history)
    {
      field_lengths[i++] = 4;       /* The DATE is stored as a DWORD */
      field_lengths[i++] = 4;       /* The DURATION is also stored as a DWORD */
    }

    num_fields = i;

    /* Allocate PBM fields to put everything in */
    if (!pbm_allocate_fields_internal(&node->fields, (uint32)num_fields, field_lengths))
    {
      ret = FALSE;
      break;
    }

    //Re-init i to start filling in field info
    i = 0;

#ifdef FEATURE_PBM_USIM_SUPPORT
    /* Fill in USIM fields */
    for (i = 0; i < num_usim_fields; i++) {
      if (usim_fields != NULL) {
        pbm_fill_field(&node->fields[i], usim_fields[i]->field_id,
                       usim_fields[i]->field_type, pbm_get_field_data_ptr(usim_fields[i],0),
                       (uint16) field_lengths[i]);
      }
    }

    /* Delete copy of USIM fields in USIM cache */
    for (j = 0, k = 0; j < num_usim_recs; j++) {
      if (usim_recs != NULL) {
        k += pbm_usim_update_fields(usim_recs[j], &node->fields[k]);
      }
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */

    /* Fill in SIM fields */
    if (pbmutils_wstrlen(record->text))
    {
      pbm_fill_field(&node->fields[i], PBM_FIELD_NAME, PBM_FT_UCS2,
          (byte *) record->text, (uint16) field_lengths[i]);
      i++;
    }

    if (strlen(record->number))
    {
      pbm_fill_field(&node->fields[i], PBM_FIELD_PHONE_GENERIC,
          PBM_FT_PHONE, (byte *) record->number, (uint16) field_lengths[i]);
      i++;
    }

    pbm_fill_field(&node->fields[i], PBM_FIELD_GSM_TONNPI,
        PBM_FT_BYTE, (byte *) &record->type, (uint16) field_lengths[i]);
    i++;
    pbm_fill_field(&node->fields[i], PBM_FIELD_INDEX,
        PBM_FT_WORD, (byte *) &location, (uint16) field_lengths[i]);
    i++;
#ifdef FEATURE_PBM_USIM_SUPPORT
    if (call_history)
    {
      pbm_fill_field(&node->fields[i], PBM_FIELD_DATE, PBM_FT_DWORD,
          (byte *) &record->time, (uint16) field_lengths[i]);
      i++;
      pbm_fill_field(&node->fields[i], PBM_FIELD_DURATION, PBM_FT_DWORD,
          (byte *) &record->duration, (uint16) field_lengths[i]);
      i++;
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
    /* Add node to cache */
    node->num_fields = (uint16) num_fields;
    node->record_id = rec_id;

    node->unique_id = 0xFFFF;

#ifdef FEATURE_PBM_USIM_SUPPORT
    if (pb_cache && pb_cache->m_sync_cache &&
        pb_cache->m_sync_cache->sync_present)
    {
      PB_SYNC2_CACHE *sync_cache = pb_cache->m_sync_cache->uid_cache;
      if (sync_cache && sync_cache->data_array)
      {
        node->unique_id = sync_cache->data_array[location];
      }
      else
      {
        PBM_MSG_ERR("Unexpected NULL pointer in ADN SYNC cache", 0, 0, 0);
      }
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */

    if (pbm_cache_add(pb_cache, node, FALSE) != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to add record %d to cache", node->record_id, 0, 0);
      ret = FALSE;
      break;
    }

  } while(FALSE); /*lint !e717 */

  PBM_MEM_FREEIF(field_lengths);
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_MEM_FREEIF(usim_fields);
  PBM_MEM_FREEIF(usim_recs);
#endif /* FEATURE_PBM_USIM_SUPPORT */
  if (!ret)
  {
    if (node)
    {
      PBM_MEM_FREEIF(node->fields);
      PBM_MEM_FREEIF(node);
    }
  }

  return ret;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_CACHE_RECORD_ADD

DESCRIPTION
  Takes in a generic SIM record, converts it to a USIM field and adds it to the
  appropriate cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_cache_record_add(PB_USIM_CACHE *usim_cache,
                                  const pbm_sim_record_s_type *record)
{
  pbm_field_id_e_type field_id;
  pbm_usim_cache_s_type *node;
  uint32 field_lengths[PBM_MAX_USIM_FIELDS_PER_REC];
  int num_fields = 0;
  int i, j;

  PBM_CHECK_PTR_RET(usim_cache, FALSE);
  PBM_CHECK_PTR_RET(record, FALSE);

  memset(field_lengths, 0, sizeof(field_lengths));

  field_id = pbm_file_id_to_field_id(record->file_id);
  if (field_id != pbm_file_id_to_field_id(usim_cache->file_id))
  {
    PBM_MSG_ERR("Field id stored within file %d does not match %d",
                 record->file_id, pbm_file_id_to_field_id(usim_cache->file_id), 0);
    return FALSE;
  }

  if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_3)
  {
    PBM_MSG_ERR("Type 3 records are not currently supported %d",
                usim_cache->filename, 0, 0);
    return FALSE;
  }

  /* Get field lengths */
  switch (field_id)
  {
    case PBM_FIELD_EMAIL:
    case PBM_FIELD_OTHERNAME:
      if ((field_lengths[0] = pbmutils_wstrlen(record->data.text.text)) != 0)
      {
        field_lengths[0] = (MIN(field_lengths[0], PBM_MAX_TEXT_LENGTH) + 1);
        field_lengths[0] *= sizeof(uint16);
        num_fields++;
      }
      else
        return TRUE;
      break;

    case PBM_FIELD_ADDITIONAL_NUMBER:
      if ((field_lengths[0] = strlen(record->data.anr.number)) != 0)
      {
        field_lengths[0] = (MIN(field_lengths[0], PBM_MAX_NUM_LENGTH) + 1);
        num_fields++;
      }
      else
        return TRUE;
      break;

    case PBM_FIELD_GRPNAME_ID:
      for (i = 0; i < PBM_MAX_NUM_GROUPS; i++)
        if (record->data.grp.group_id[i] != 0)
          field_lengths[num_fields++] = PBM_WORD_SIZE;
      if (!num_fields)
        return TRUE;

      if (usim_cache->mapping_type != GSDI_MAPPING_TYPE_1)
      {
        PBM_MSG_ERR("GRP can only be a type 1 record %d",
                    usim_cache->mapping_type, 0, 0);
        return FALSE;
      }
      break;

    default:
      PBM_MSG_ERR("Unknown field id %d", field_id, 0, 0);
      return FALSE;
  }

  /* Allocate node and fields */
  PBM_MEM_ALLOC(node, sizeof(pbm_usim_cache_s_type));
  PBM_CHECK_PTR_RET(node, FALSE);

  if (!pbm_allocate_fields_internal(&node->fields, (uint32) num_fields, field_lengths))
  {
    PBM_MEM_FREEIF(node);
    return FALSE;
  }

  /* Fill values */
  switch (field_id)
  {
    case PBM_FIELD_EMAIL:
    case PBM_FIELD_OTHERNAME:
      pbm_fill_field(&node->fields[0], field_id, PBM_FT_UCS2,
                     (byte *)record->data.text.text, (uint16) field_lengths[0]);
      node->m_recid = record->data.text.adn_record_id;
      break;

    case PBM_FIELD_ADDITIONAL_NUMBER:
      pbm_fill_field(&node->fields[0], field_id, PBM_FT_PHONE,
                     (byte *)record->data.anr.number,
                     (uint16) field_lengths[0]);
      node->m_recid = record->data.anr.adn_record_id;
      break;

    case PBM_FIELD_GRPNAME_ID:
      for (i = 0, j = 0; i < PBM_MAX_NUM_GROUPS && j < num_fields; i++)
      {
        if (record->data.grp.group_id[i] != 0)
        {
          pbm_fill_field(&node->fields[j], field_id, PBM_FT_WORD,
                         (byte *)&record->data.grp.group_id[i],
                         (uint16) field_lengths[j]);
          j++;
        }
      }
      break;

    default:
      PBM_MSG_ERR("Unknown field id %d",
                  pbm_file_id_to_field_id(usim_cache->file_id), 0, 0);
      PBM_MEM_FREEIF(node->fields);
      PBM_MEM_FREEIF(node);
      return FALSE;
  }

  /* Update master record id */
  if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_1)
  {
    pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(record->file_id);
    PBM_CHECK_PBSET_RET(pb_set, FALSE);

    node->m_recid = pbm_location_to_adn_rec_id(record->sim_id,
                                    pb_set,
                                    (uint8)record->index);
  }

  /* Add node to cache */
  node->num_fields = (uint16)num_fields;
  node->nref = 1;
  if (pbm_usim_cache_add(usim_cache, (uint16)record->index, node, TRUE) !=
                                                                PBM_SUCCESS)
  {
    PBM_MSG_ERR("Unable to add USIM record to location %d, file %d, sim %d",
                record->index, record->file_id, record->sim_id);
    PBM_MEM_FREEIF(node->fields);
    PBM_MEM_FREEIF(node);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION PBM_USIM_FIELDS_ADD

DESCRIPTION
  Takes in a generic SIM record, converts it to a USIM field and adds it to the
  appropriate cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_fields_add(PB_USIM_CACHE *usim_cache,
                                          uint16 location,
                                          uint16 adn_recid,
                                          const pbm_field_s_type *fields,
                                          int num_fields)
{
  pbm_usim_cache_s_type *node;

  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);
  PBM_CHECK_PTR_RET(fields, PBM_ERROR);

  if (num_fields <= 0)
  {
    PBM_MSG_ERR("Invalid number of fields %d", num_fields, 0, 0);
    return PBM_ERROR;
  }

  PBM_MEM_ALLOC(node, sizeof(pbm_usim_cache_s_type));
  if (!node)
  {
    PBM_MSG_ERR("Unable to allocate memeory for USIM node", 0, 0, 0);
    return PBM_ERROR_MEM_FULL;
  }

  node->nref = 1;
  node->num_fields = (uint16)num_fields;
  node->m_recid = adn_recid;

  if (!pbm_copy_fields(&node->fields, &fields, (uint32)num_fields))
  {
    PBM_MSG_ERR("Unable to copy fields", 0, 0, 0);
    PBM_MEM_FREEIF(node);
    return PBM_ERROR_MEM_FULL;
  }

  if (pbm_usim_cache_add(usim_cache, location, node, FALSE) != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Unable to add USIM record %d to cache of file %d", location,
                 usim_cache->file_id, 0);
    PBM_MEM_FREEIF(node);
    return PBM_ERROR;
  }

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_UIM_GENERATE_SYNC_VALUES

DESCRIPTION
  Generates a new unique id if generate_uid is TRUE.  Also generates other
  sync values ( change counter and psc) for the UIM cache entry regardless.
  Generation of new unique id is done based on previously used unique id.
  If generate_uid is FALSE, the function returns 0.

  Note: Unique ids does not have to be contiguous. So if addition of this
  record fails, we do not have to set puid back to previous value.

RETURN VALUE
  unique id

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
#ifndef FEATURE_MMGSDI
//lint -e{715} -e{818} suppress unused parameters, param could be const
#endif
uint16 pbm_uim_generate_sync_values(PB_CACHE *cache,
                                    pbm_write_sync_s_type *sync,
                                    boolean generate_uid)
{
  uint16 uid = 1;
#ifdef FEATURE_MMGSDI
  PB_MASTER_SYNC_CACHE *m_sync_cache;
  int i;
#endif

  PBM_CHECK_PTR_RET(cache, 0);
  PBM_CHECK_PTR_RET(sync, 0);

#ifdef FEATURE_MMGSDI
  m_sync_cache = cache->m_sync_cache;
  if (m_sync_cache && m_sync_cache->sync_present)
  {
    uint16 cc;

    PBM_CHECK_PTR_RET(m_sync_cache->cc_cache, 0);
    PBM_CHECK_PTR_RET(m_sync_cache->puid_cache, 0);
    PBM_CHECK_PTR_RET(m_sync_cache->psc_cache, 0);

    if (cache->file_ids[0] != PBM_FILE_ADN)
    {
      PBM_MSG_ERR("Invalid file ID for generation of unique ID %d",
          cache->file_ids[0], 0, 0);
      uid = 0;
    }
    else
    {
      /* Generate change counter */
      cc = ++m_sync_cache->cc_cache->data.cc;

      /* Generate unique id if required */
      if (generate_uid)
        uid = ++m_sync_cache->puid_cache->data.puid;
      else
        uid = 0;

      /* Increment PSC if uid or cc wraps around and regenerate all uids */
      if (cc == 0 || ((uid == 0) && generate_uid))
      {
        sync->update_psc = TRUE;
        m_sync_cache->psc_cache->data.psc++;
        m_sync_cache->psc_cache->data.psc %= 0xFFFFFFFF;
        sync->psc = m_sync_cache->psc_cache->data.psc;
        uid = 1;
        for (i = 1; i <= cache->num_of_records; i++)
        {
          if (cache->pb_cache_array[i] && cache->pb_cache_array[i]->num_fields)
            cache->pb_cache_array[i]->unique_id = uid++;
        }

        /* We've counted a UID for the new record, so if we */
        /* aren't generating one, back off that UID, and force return 0 */
        if (generate_uid)
        {
          m_sync_cache->puid_cache->data.puid = uid;
        }
        else
        {
          m_sync_cache->puid_cache->data.puid = uid - 1;
          uid = 0;
        }

        if (cc == 0)
          m_sync_cache->cc_cache->data.cc = 1;
        cache->status = PBM_STATUS_SYNC;
      }
      else
      {
        sync->update_psc = FALSE;
      }

      sync->cc = m_sync_cache->cc_cache->data.cc;
    }
  }
#endif

  return uid;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_usim_update_fields

DESCRIPTION
  Update the field pointers within cache to point to the specified fields
  array. Returns number of fields updated.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_usim_update_fields(pbm_usim_cache_s_type *cache,
                              const pbm_field_s_type *fields)
{
  int i;

  PBM_CHECK_PTR_RET(cache, 0);
  PBM_CHECK_PTR_RET(fields, 0);

  PBM_MEM_FREEIF(cache->fields);

  PBM_MEM_ALLOC(cache->fields, cache->num_fields *
                               sizeof(pbm_field_s_type));
  if (!cache->fields)
  {
    PBM_MSG_ERR("Unable to allocate memory for new fields array", 0, 0, 0);
    return cache->num_fields = 0;
  }

  for (i = 0; i < cache->num_fields; i++)
    cache->fields[i] = fields[i];

  return cache->num_fields;
}

/*===========================================================================
FUNCTION PBM_MASTER_USIM_CACHE_UPDATE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_master_usim_cache_update(
                                  uint16 rec_id,
                                  const uint8 *prev_index,
                                  const uint8 *curr_index)
{
  int i;
  pbm_device_type pb_id = pbm_record_id_to_phonebook(rec_id);
  PB_USIM_CACHE *usim_cache;
  PB_CACHE *pb_cache = pbm_pb_id_to_cache(pb_id);
  uint16 adn_index;
  PB_MASTER_USIM_CACHE *m_usim_cache;

  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  if (!pbm_action_requires_sim(pb_id))
    return PBM_SUCCESS;

  m_usim_cache = pb_cache->m_usim_cache[pbm_record_id_to_pb_set(rec_id)];

  adn_index = pbm_record_id_to_fileloc(rec_id);

  if (prev_index)
  {
    PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

    for ( i = 1; i <= m_usim_cache->num_caches; i++)
    {
      if (prev_index[i])
      {
        usim_cache = m_usim_cache->usim_cache_array[i];
        if (!usim_cache || usim_cache->order != i)
        {
          PBM_MSG_ERR("No cache found for cache %d index %d",
                       i, prev_index[i], 0);
          return PBM_ERROR;
        }
        usim_cache->records_in_use--;
        m_usim_cache->index_array[i][adn_index] = 0;
      }
    }
  }

  if (curr_index)
  {
    PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

    for ( i = 1; i <= m_usim_cache->num_caches; i++)
    {
      if (curr_index[i])
      {
        usim_cache = m_usim_cache->usim_cache_array[i];
        if (!usim_cache || usim_cache->order != i)
        {
          PBM_MSG_ERR("No cache found for cache %d index %d",
                       i, curr_index[i], 0);
          return PBM_ERROR;
        }
        usim_cache->records_in_use++;
        m_usim_cache->index_array[i][adn_index] = curr_index[i];
      }
    }
  }

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_PBM_TASK */
