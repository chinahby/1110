#ifndef PBMUIM_H
#define PBMUIM_H

/*===========================================================================

           INTERNAL HEADER FILE FOR PHONEBOOK MANAGER UIM FUNCTIONS

DESCRIPTION
  This is the internal header file for Phonebook Manager UIM related
  functions.

 Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbmuim.h#40 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
12/26/08   kcp     adding changes for ASUS CR 165600
07/10/08   sg      Add pbm_start_record_read to support SEEKing unused records
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
03/12/07   cvs     Featurize 4 PB set support
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/10/06   cvs     fix comparison in PBM_RECINDEX_TO_FILENAME()
03/23/06   cvs     Lint fixes.
02/28/06   cvs     Add support for MBDN
01/27/06   cvs     fix USE_USIM to return 0 or 1
12/13/05   cvs     Implement partial PB refresh
11/16/05   cvs     Lint Fixes
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/12/05   AT      Added UCS2 support for strings.
07/29/05   AT      Add check for NULL pointer in Macro.
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
03/22/05   PA      Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for MSISDN.
01/28/05   PA      Change GSDI_GLOBAL_PHONEBOOK to GSDI_GLOBAL_PHONEBOOK_TYPE
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds
09/29/04   PA      Initial revision.
===========================================================================*/
#ifdef FEATURE_MMGSDI
#include "uim.h"
#include "gsdi_exp.h"
#include "mmgsdilib.h"
#endif

#include "pbmlib.h"
#include "pbm.h"

#ifdef FEATURE_MMGSDI_DUAL_SLOT

  typedef enum {
    PBM_FIRST_SLOT = 0,
    PBM_SLOT_1 = PBM_FIRST_SLOT,
    PBM_SLOT_2,
    PBM_NUM_SLOTS
  } pbm_slot_type;

  /*lint -emacro(774, PBM_VALIDATE_SLOT_RET) always true */
  #define PBM_VALIDATE_SLOT_RET(x, ret)                      \
  if (((x) != PBM_SLOT_1) && ((x) != PBM_SLOT_2)) {          \
    PBM_MSG_ERR("Slot out of range slot %d num slots %d",    \
                x, MAX_UIMS, 0);                             \
    return ret;                                              \
  }
#else

  typedef enum {
    PBM_FIRST_SLOT = 0,
    PBM_SLOT_1 = PBM_FIRST_SLOT,
    PBM_SLOT_2,
    PBM_NUM_SLOTS = PBM_SLOT_2
  } pbm_slot_type;

  /*lint -emacro(774, PBM_VALIDATE_SLOT_RET) always true */
  #define PBM_VALIDATE_SLOT_RET(x, ret)                      \
  if ((x) != PBM_SLOT_1) {                                   \
    PBM_MSG_ERR("Slot out of range slot %d num slots %d",    \
                x, MAX_UIMS, 0);                             \
    return ret;                                              \
  }
#endif

typedef enum {
  PBM_USIM_PB_INVALID = -1,
  PBM_USIM_PB_ONE,
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_USIM_PB_TWO,
#ifdef FEATURE_USIM_1000_PBM
  PBM_USIM_PB_THREE,
  PBM_USIM_PB_FOUR,
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PBM_MAX_NUM_PB_SETS
} pbm_pb_set_type;

/* Note that in PBM modified mask is stored in high order byte */
#define PBM_PBC_MODIFIED_MASK 0x0100
#define PBM_PBC_HIDDEN_MASK  0x00FF

/* Number of fields in a SIM PB entry */
#define PBM_SIM_NUM_FIELDS 4
/* Number of fields in a SIM call history PB entry */
#ifdef FEATURE_PBM_USIM_SUPPORT
#define PBM_CALLHISTORY_NUM_FIELDS 6
#else
#define PBM_CALLHISTORY_NUM_FIELDS 4
#endif /* FEATURE_PBM_USIM_SUPPORT */

/* Maximum number of Type 2 USIM files within a single phonebook.
 * EMAIL, SNE, GRP, ANR, ANRA, ANRB, ANRC for now */
#define PBM_MAX_NUM_TYPE2_FILES 7

/* Maximum number of groups per phonebook */
#define PBM_MAX_NUM_GROUPS 10

/* EMAIL(1), SNE(1), ANR(4), GRP(PBM_MAX_NUM_GROUPS) */
#define PBM_MAX_USIM_FIELDS_PER_REC (6+PBM_MAX_NUM_GROUPS)

/* Maximum number of files dedicated to storing the same type of info (ANR)
 * within a single USIM phonebook */
#define PBM_MAX_NUM_SAME_USIM_FILES 4

#define PBM_USIM_SAMEFILE_INDEX(usim_pb, file_num) \
                  (((int)(usim_pb) * PBM_MAX_NUM_SAME_USIM_FILES) + (file_num))

#define USE_USIM_LOCAL(sim_id) (((pbm_get_card_apps(sim_id) & GSDI_USIM_APP_MASK)) &&   \
                        ((!pbm_gsdi_phonebook_present((sim_id), GSDI_GLOBAL_PHONEBOOK_TYPE)) &&  \
                         (pbm_gsdi_phonebook_present((sim_id), GSDI_LOCAL_PHONEBOOK_TYPE))))

#define USE_USIM(sim_id) ((pbm_get_card_apps(sim_id) & GSDI_USIM_APP_MASK)?1:0)

#define PBM_SIMID_TO_SLOT_MASK(sim_id) ((uint32)(1 << (uint32)(sim_id)))

#define PBM_SIMSLOT_SET(x, sim_id) ((x) |= PBM_SIMID_TO_SLOT_MASK((sim_id)))

#define PBM_SIMSLOT_RESET(x, sim_id) ((x) &= ~PBM_SIMID_TO_SLOT_MASK((sim_id)))

#define PBM_SIM_FILENAME_TO_INDEX(sim, file) \
        (int) (((int) PBM_FILE_MAX_REQUESTS * ((int) (sim))) + ((int) (file)))

/* The following enum is used to enumerate the files for which we can
 * request file_info.  We use this value as an index into the array below.
 * Update pbm_file_to_uim_file when this enum is modified.
 */
typedef enum {
  PBM_FILE_ADN = 0,
  PBM_FILE_FDN,
  PBM_FILE_SDN,
  PBM_FILE_LND,
  PBM_FILE_MSISDN,

  /* 5 */
  PBM_FILE_MBN,
  PBM_FILE_EXT1,
  PBM_FILE_EXT2,
  PBM_FILE_EXT3,
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_FILE_MBDN,

  /* 10 */
  PBM_FILE_OCI,
  PBM_FILE_ICI,
  PBM_FILE_EXT5,
  PBM_FILE_EXT6,
  PBM_FILE_PSC,

  /* 15 */
  PBM_FILE_PUID,
  PBM_FILE_CC,
  PBM_FILE_EMAIL,
  PBM_FILE_SNE,
  PBM_FILE_GRP,

  /* 20 */
  PBM_FILE_ANR,
  PBM_FILE_ANRA,
  PBM_FILE_ANRB,
  PBM_FILE_ANRC,
  PBM_FILE_IAP,

  /* 25 */
  PBM_FILE_UID,
  PBM_FILE_PBC,
  PBM_FILE_ADN1,
  PBM_FILE_EMAIL1,
  PBM_FILE_SNE1,

  /* 30 */
  PBM_FILE_GRP1,
  PBM_FILE_ANR1,
  PBM_FILE_ANRA1,
  PBM_FILE_ANRB1,
  PBM_FILE_ANRC1,

  /* 35 */
  PBM_FILE_IAP1,
  PBM_FILE_UID1,
  PBM_FILE_PBC1,
#ifdef FEATURE_USIM_1000_PBM
  PBM_FILE_ADN2,
  PBM_FILE_EMAIL2,

  /* 40 */
  PBM_FILE_SNE2,
  PBM_FILE_GRP2,
  PBM_FILE_ANR2,
  PBM_FILE_ANRA2,
  PBM_FILE_ANRB2,

  /* 45 */
  PBM_FILE_ANRC2,
  PBM_FILE_IAP2,
  PBM_FILE_UID2,
  PBM_FILE_PBC2,
  PBM_FILE_ADN3,

  /* 50 */
  PBM_FILE_EMAIL3,
  PBM_FILE_SNE3,
  PBM_FILE_GRP3,
  PBM_FILE_ANR3,
  PBM_FILE_ANRA3,

  /* 55 */
  PBM_FILE_ANRB3,
  PBM_FILE_ANRC3,
  PBM_FILE_IAP3,
  PBM_FILE_UID3,
  PBM_FILE_PBC3,
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
  /* 60 */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  PBM_FILE_ECC,
#endif
  PBM_FILE_MAX_REQUESTS,
  PBM_FILE_NONE,  /* NONE and ALL are special cases, and need to be */
                  /* after MAX_requests, so they don't increase the array size */
  PBM_FILE_ALL
} pbm_file_device_e_type;

#ifdef FEATURE_MMGSDI
/* The next structure is used to communicate the files for which we need
 * the attributes for.  For each file, referenced by index with the name
 * above, we will indicate whether we want to get the file_info, and if
 * so, what filename to use.  Based on the index into this array, the
 * callback must know where to put the answers.
 */
typedef struct {
  boolean                do_request;
  mmgsdi_file_enum_type  filename;
  gsdi_services_type     service_available;
} pbm_fileinfo_request_t;

typedef struct {
  mmgsdi_file_enum_type uim_device;
  byte                  index;
} pbm_ext_rec_in_use_s_type;

#ifdef FEATURE_PBM_USIM_SUPPORT
/* nref should be 1 for all non-type_3 records. Field data points to the
 * content of the corresponding master ADN field.
 *
 * For type 3 records, nref can be > 1. Even when nref is 1, the USIM
 * cache will have its own copy of data since it can be shared.
 */
typedef struct {
  int               nref;        /* Number of references to this record */
  uint16            m_recid;     /* Master ADN record */
  uint16            num_fields;  /* Number of fields */
  pbm_field_s_type *fields;      /* Data */
} pbm_usim_cache_s_type;

typedef struct {
  pbm_file_device_e_type     file_id;        /* PBM file */
  mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                              /* how to read + write it */
  mmgsdi_file_enum_type      filename;       /* UIM file */
  gsdi_pbr_mapping_enum_type mapping_type;   /* record linkage type */
  byte                       order;          /* Order within IAP file and cache array */
  int                        num_of_records; /* Number of records within file */
  int                        records_in_use; /* Number of records in use */
  int                        record_length;  /* Length of each record */
  pbm_usim_cache_s_type    **pb_cache_array; /* USIM file data */
} PB_USIM_CACHE;

/* Master USIM cache structure */
typedef struct {
  pbm_file_device_e_type file_id;          /* IAP file */
  mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                              /* how to read + write it */
  mmgsdi_file_enum_type  filename;         /* UIM file for IAP */
  int                    iap_rec_len;      /* Length of IAP record */
  int                    iap_num_records;  /* Number of IAP records */

  int                    num_caches;       /* Number of usim file caches */
  PB_USIM_CACHE        **usim_cache_array; /* Caches for each file */
  /* Index within USIM cache for each ADN record.
   * For ex: index_array[1][14] will give the index corresponding to ADN
   * record 14 within USIM cache 1. Note that indexing is 1-based.
   * usim_cache_array[x] and index_array[x] can be NULL when iap
   * record length is greater than the number of files actually available.
   * So before accessing index_array[x][y], check if index_array[x] is valid.
   * The same applies to usim_cache_array[x]. */
  uint8                **index_array;

  uint8                  adn_sfi;          /* SFI corresponding to ADN file */
} PB_MASTER_USIM_CACHE;

/* For PSC, CC and PUID. Only one record per file */
typedef struct {
  pbm_file_device_e_type file_id;
  mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                              /* how to read + write it */
  mmgsdi_file_enum_type  filename;
  int                    num_of_records;
  int                    record_length;
  union {
    uint32               psc;
    uint16               cc;
    uint16               puid;
  } data;
} PB_SYNC1_CACHE;

/* For PBC, UID.
 * Same number of records as ADN */
typedef struct {
  pbm_file_device_e_type file_ids[PBM_MAX_NUM_PB_SETS];
  mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                              /* how to read + write it */
  mmgsdi_file_enum_type  filenames[PBM_MAX_NUM_PB_SETS];
  int                    records_in_pb_set[PBM_MAX_NUM_PB_SETS];
                                     /* the number of records in each PB set */
  int                    num_files;
  int                    num_of_records;
  int                    record_length;
  uint16                *data_array;
} PB_SYNC2_CACHE;

typedef struct {
  PB_SYNC2_CACHE        *pbc_cache;
  boolean                reset_pbc;

  boolean                sync_present;
  PB_SYNC1_CACHE        *psc_cache;
  PB_SYNC1_CACHE        *cc_cache;
  PB_SYNC1_CACHE        *puid_cache;
  PB_SYNC2_CACHE        *uid_cache;
} PB_MASTER_SYNC_CACHE;

/* Index Administration Phonebook (IAP) record.
 * IAP record contains the indices of each USIM record correponding to an
 * ADN record. There is a one-to-one mapping between ADN records and IAP
 * records. The order of this index within an IAP record is same as the
 * order in which the USIM file appear within PBR record. In case of PBM, this
 * order is stored in usim_cache->order. */
typedef struct {
  uint8 index[PBM_MAX_NUM_TYPE2_FILES]; /* Record number within USIM file */
} pbm_iap_record_s_type;

typedef struct {
  uint16 group_id[PBM_MAX_NUM_GROUPS]; /* Group Id */
} pbm_grp_record_s_type;

typedef struct {
  uint8  anr_id;               /* Additional number id */
  char   number[PBM_NUM_SIZE]; /* Additional Number */
  uint8  type;
  uint16 adn_record_id;        /* ADN record id */
} pbm_anr_record_s_type;

#endif /* FEATURE_PBM_USIM_SUPPORT */

typedef struct {
  uint16 text[PBM_TEXT_SIZE_CHARS]; /* Alpha Text Stored as UCS2. */
  uint16 adn_record_id;             /* ADN record id */
} pbm_text_record_s_type;

typedef union {
  pbm_num_text_record_s_type num_text; /* Text + Num (ex: ADN) */
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_iap_record_s_type      iap;      /* IAP record */
  pbm_grp_record_s_type      grp;      /* Group record */
  pbm_anr_record_s_type      anr;      /* ANR record */
#endif /* FEATURE_PBM_USIM_SUPPORT */
  pbm_text_record_s_type     text;     /* eX: SNE, EMAIL */
  uint32                     dword;
  uint16                     word;
} pbm_generic_sim_data_s_type;

typedef struct {
  int                         index;
  pbm_slot_type               sim_id;
  pbm_file_device_e_type      file_id;
  pbm_generic_sim_data_s_type data;
} pbm_sim_record_s_type;

/* This struct is only used to show the ending of the ICI file.  */
typedef PACKED struct
{
  byte ici_status; /* status: LSB 0 = answered, LSB 1 = not answered  */
  byte pb_link[3]; /* 0xFFFFFF if not used */
} ICI_ENDING;

/* This struct is only used to show the ending of the OCI file.  */
typedef PACKED struct
{
  byte pb_link[3]; /* 0xFFFFFF if not used */
} OCI_ENDING;

/* This struct is used when writing entries to the SIM to make life a bit
 * easier.
 **/
typedef PACKED struct {
  byte bcd_len;
  byte ton_npi;
  byte number[10];      /* BCD coding */
  byte cap_config_id;   /* 0xFF if not used */
  byte ext_id;          /* 0xFF if not used */

  /* Then on ICI/OCI file, we also have */
  byte date_time[7];    /* year, month, day, hour, minute, second, time zone */
  byte duration[3];

  PACKED union {
    ICI_ENDING ici_end;
    OCI_ENDING oci_end;
  } ici_oci_ending;
} pbm_fixed_sim_rec_s_type;

typedef struct {
  pbm_file_device_e_type pbm_filename;
  mmgsdi_file_enum_type  uim_filename[2]; /* 0 for global, 1 for USIM local */
  pbm_file_device_e_type pbm_file_group[2]; /* 0 for SIM, 1 for USIM */
} pbm_uim_file_entry;

extern uint32 pbm_slot_init_waiting;    /* Waiting for sim init completed */
extern uint32 pbm_slot_pin_waiting;     /* Waiting for pin */
extern uint32 pbm_slot_fdn_enabled;     /* FDN enabled */

extern uint32 pbm_slot_card_inserted;   /* True if a card is inserted in the slot */
extern uint32 pbm_slot_initializing;    /* Initialization is going on */
extern uint32 pbm_slot_restart;         /* Somebody wants to restart */

extern pbm_fileinfo_request_t pbm_fileinfo_requests[PBM_FILE_MAX_REQUESTS * MAX_UIMS];

#define PBM_NON_USIM_INDEX 0
#define PBM_USIM_GLOBAL_INDEX 0
#define PBM_USIM_LOCAL_INDEX 1
#define PBM_USIM_COMMON_INDEX 1

extern pbm_uim_file_entry pbm_file_to_uim_file[PBM_FILE_MAX_REQUESTS];
#ifdef FEATURE_PBM_USIM_SUPPORT
#define PBM_NUM_SIM_PB 9
#else
#define PBM_NUM_SIM_PB 7
#endif /* FEATURE_PBM_USIM_SUPPORT */
extern pbm_file_device_e_type pbm_sim_pb_files[PBM_NUM_SIM_PB];

extern int pbm_initialize_index[MAX_UIMS];
//PBM_SIZE_INITIALIZE_LIST is the number of pbm file ids that will be in
//pbm_initialize_list array.
#ifdef FEATURE_ECC_HANDLING_AT_PBM
#ifdef FEATURE_PBM_USIM_SUPPORT
#ifdef FEATURE_USIM_1000_PBM
  #define PBM_SIZE_INITIALIZE_LIST 47
#else
  #define PBM_SIZE_INITIALIZE_LIST 31
#endif
#else
  #define PBM_SIZE_INITIALIZE_LIST 7
#endif /* FEATURE_PBM_USIM_SUPPORT */
#else /* FEATURE_ECC_HANDLING_AT_PBM */
#ifdef FEATURE_PBM_USIM_SUPPORT
#ifdef FEATURE_USIM_1000_PBM
  #define PBM_SIZE_INITIALIZE_LIST 46
#else
  #define PBM_SIZE_INITIALIZE_LIST 30
#endif
#else
  #define PBM_SIZE_INITIALIZE_LIST 6
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_ECC_HANDLING_AT_PBM */

extern pbm_file_device_e_type pbm_initialize_list[PBM_SIZE_INITIALIZE_LIST];


#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_usim_read_pbr_cb

DESCRIPTION
  Read the PBR structure from GSDI.

PARAMETERS
   None.

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_pbr_cb(const gsdi_cnf_T *sim_data);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_SIM_READ

DESCRIPTION
  Reads from the given phone book.  The parameters for this function will
  be passed by value rather than as pointers to prevent other applications
  from changing values before being processed.

  If proc_func = NULL this is a SYNC read.  If it's !NULL this is an ASYNC read.

  Data returned in pbm_data_record.


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_read (pbm_slot_type sim_id,
                              pbm_file_device_e_type file_id,
                              int record_num,
                              void (*proc_func) ( pbm_return_type,
                                                  pbm_sim_record_s_type *));

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_READ

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only USIM files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_read (pbm_slot_type sim_id,
                               pbm_file_device_e_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *));

/*===========================================================================
FUNCTION PBM_INDEX_READ

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only USIM files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_index_read(pbm_slot_type sim_id,
                               pbm_file_device_e_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *));
/*===========================================================================
FUNCTION pbm_sync_read

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only synchronization files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_read(pbm_slot_type sim_id,
                              pbm_file_device_e_type file_id,
                              int record_num,
                              void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *));

/*===========================================================================
FUNCTION PBM_USIM_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record);

/*===========================================================================
FUNCTION PBM_INDEX_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record);
/*===========================================================================
FUNCTION PBM_SYNC1_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync1_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record);

/*===========================================================================
FUNCTION PBM_SYNC2_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync2_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_RESTART_FILEINFO_INTERNAL

DESCRIPTION
  This function is called from a callback to restart an intialization
  chain.

DEPENDENCIES
  Not reentrant. This function should always be called from GSDI task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_restart_fileinfo_internal(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_INIT_ENTRIES

DESCRIPTION
  This function is called after PIN is verified and GSDI is ready for a flood
  of UI requests.

DEPENDENCIES
   This is now an internal function and should not be used by other subsystems.

SIDE EFFECTS
===========================================================================*/
void pbm_init_entries(pbm_slot_type sim_id);


/*===========================================================================
FUNCTION PBM_INIT_FILEINFO_INTERNAL

DESCRIPTION
  This function will be called when GSDI gives us the SIM init completed
  event. If there is an initialization in progress for this SIM,
  that initialization is stopped and restarted from the beginning.

  Only one sequence of initializations can happen for each SIM.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from GSDI task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_init_fileinfo_internal(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_STOP_FILEINFO_INTERNAL

DESCRIPTION
  This function is called to stop an initialization chain when SIM is
  removed.

DEPENDENCIES
  Not reentrant. This function should always be called from GSDI task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_stop_fileinfo_internal(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_FDN_DISABLE

DESCRIPTION
  This function is called is response to a GSDI_FDN_DISABLED event.
  It re-initializes FDN and if ADN is readable now, it reinitalizes
  ADN too.

DEPENDENCIES
  Not reentrant. This function should always be called from GSDI task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fdn_disable(pbm_slot_type sim_id);


/*===========================================================================
FUNCTION PBM_FDN_ENABLE

DESCRIPTION
  This function is called is response to a GSDI_FDN_ENABLED event.
  It re-initializes FDN. If ADN is restricted when FDN is enabled for this
  SIM, then ADN cache is also updated.

DEPENDENCIES
  Not reentrant. This function should always be called from GSDI task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fdn_enable(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_UIM_WRITE_CB

DESCRIPTION
  This function is called in response to a pbm_uim_write function after
  all processing is done. The input parameter 'ret' specifes whether the
  operation succeeded or not.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_uim_write_cb(uint32 buffer_index, pbm_return_type ret);

/*===========================================================================
FUNCTION PBM_SIM_WRITE_CB

DESCRIPTION
  This function is called in response to a pbm_sim_write function after
  all processing is done. The input parameter 'ret' specifes whether the
  operation succeeded or not.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_sim_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr);

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_SIM

DESCRIPTION
Add/updates/deletes a record in the address book in the SIM.  This is
the new interface that supports many different fields per contact.

To add a record:
1)  Set rec_id = 0xFFFF (and rec_id_ptr=&rec_id).  After a synchronous call has
    completed, look here for the assigned record ID for the new record.
2)  Set items_ptr equal to an array of fields for the new record
3)  Set item_count equal to the length of above array
    To update a record, do the same as when adding a record (above), except set
    rec_id to the record ID of the record to be updated.  Note that the fields
    given in items_list will replace all fields in the current record, i.e. the
    current fields will be deleted and replaced, not added to.

To delete a record:
1)  Set rec_id to the record ID of the record to be deleted.
2)  Set items_ptr = NULL.
3)  Other fields are not used.

When making an asynchronous call, rec_id_ptr is not used as an output parameter,
and success/failure indication is not given by the return value.  Instead,
success always returned immediately by the function, and then success/failure
indication is given in the first argument to the callback function proc_func().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_sim(pbm_write_record_s_type *write);

/*===========================================================================
FUNCTION PBM_FILEINFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.   It takes the request, looks at the
  client_ref to determine for which type of file the request was made,
  and stores the results.  It then calls the make_request function to
  get the next set of file attributes.  This way, this are never more
  than one PBM request to GSDI outstanding.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fileinfo_cb(mmgsdi_return_enum_type status,
                     mmgsdi_cnf_enum_type    data_type,
                     const mmgsdi_cnf_type  *data_ptr);


/*===========================================================================
FUNCTION PBM_GSDI_EXT_WRITE_CB

DESCRIPTION
  After GSDI has written the extension record, PBM_EXT_WRITE_CB

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_ext_write_cb(gsdi_cnf_T *sim_data);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_INDEX_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_index_write_cb(gsdi_cnf_T *sim_data);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_UIM_WRITE

DESCRIPTION

PARAMETERS:

DEPENDENCIES
  Cache recovery should be done outside this function if write fails.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_uim_write (uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_WRITE_MASTER

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_write_master(uint32 client_ref, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_WRITE_SYNC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_write_sync(uint32 client_ref, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_WRITE_PREV

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_write_prev(uint32 client_ref, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_WRITE_CURR

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_write_curr(uint32 client_ref, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_SIM_WRITE

DESCRIPTION
  Writes the given record to the phone book specified.  If proc_func is not NULL
  that is called when the write is complete.

  This supports two different callback function signatures.  proc_func1 supports
  the legacy pbm_write() interface.  proc_func2 supports the new pbm_record_write()
  interface.  Note:  Only *one* of the two functions should be provided by the
  calling routine.  The other function pointer argument should be NULL.

PARAMETERS:
  record  if record.index == -1, write to the first empty record
          if record.number is NULL, delete entry record.index

DEPENDENCIES
  Cache recovery should be done outside this function if write fails.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_write (uint32 buffer_index);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_DELETE

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_delete(uint32 buffer_index, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_USIM_DELETE_IAP

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_delete_iap(uint32 buffer_index, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_USIM_WRITE

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_write(uint32 buffer_index, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_SYNC1

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync1(uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_UID

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync2(uint16 data, uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_USIM_WRITE_CB

DESCRIPTION TODO: Template, needs actual code
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr);

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_EXT_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_ext_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_INDEX_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_write_cb( mmgsdi_return_enum_type status,
                         const mmgsdi_cnf_type  *data_ptr);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_WRITE_MASTER

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_master(uint32 buffer_index, pbm_return_type *ret);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_WRITE_SYNC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_sync(uint32 buffer_index, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_WRITE_CURR

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_usim(uint32 buffer_index, pbm_return_type *ret);

/*===========================================================================
FUNCTION PBM_IAP_WRITE

DESCRIPTION

PARAMETERS
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_iap_write(uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_USIM_WRITE_TEXT

DESCRIPTION
  This function fills in the text field of a USIM record.

PARAMETERS
  buffer_index: [in] The index containing the data to write.
  gsm7_only: [in] Can we encode as UCS2, or only as GSM7

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_text(uint32 buffer_index, boolean gsm7_only);

/*===========================================================================
FUNCTION PBM_USIM_WRITE_ANR

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_anr(uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_USIM_WRITE_GRP_ID

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_grp_id(uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_USIM_WRITE_IAP

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_iap(uint32 buffer_index);

/*===========================================================================
FUNCTION PBM_DELETE_USIM_FIELDS

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_delete_usim_fields(uint16 rec_id,
                                       PBM_WRITE_CB_FUNC proc_func);

/*===========================================================================
FUNCTION PBM_RESET_PBC

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization

SIDE EFFECTS
===========================================================================*/
void pbm_reset_pbc(pbm_slot_type sim_id, int index);

/*===========================================================================
FUNCTION PBM_RESET_PBC_CB

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization

SIDE EFFECTS
===========================================================================*/
void pbm_reset_pbc_cb(pbm_writecb_data_s_type *cb_data);

/*===========================================================================
FUNCTION PBM_UIM_WRITE_PBC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_pbc(uint32 buffer_index, pbm_return_type *ret);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_UIM_FILENAME_TO_REFRESH_GROUP

DESCRIPTION
  This function will return the refresh file group corresponding to the
  UIM filename

SIDE EFFECTS
===========================================================================*/
pbm_file_device_e_type pbm_uim_filename_to_refresh_group(mmgsdi_file_enum_type uim_file,
                                                         pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_INIT_UIM_ECC_LIST

DESCRIPTION
  This function is called at init to set up UIM ecc list

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_uim_ecc_list (void);

/*===========================================================================
FUNCTION PBM_INIT_ECC_INTERNAL

DESCRIPTION
  This function is called after a SIM_INSERTED event is received from GSDI.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_ecc_internal (pbm_slot_type sim_id);

/*===========================================================================
FUNCTION pbm_ecc_bcd_to_ascii

DESCRIPTION
  This function converts an ECC number into an ASCII string.  Note that
  the input string must be at least 7 characters in length.  This function
  will always set the first 7 characters.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_ecc_bcd_to_ascii(const byte *ecc_number, char *ascii_out );

/*===========================================================================
FUNCTION PBM_START_RECORD_READ

DESCRIPTION
   This function is called after the SEEK operation is complete to start
   reading records

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_start_record_read(pbm_slot_type sim_id,
                                      pbm_file_device_e_type file_id,
                                      int record_num);
#endif /* FEATURE_MMGSDI */

#endif /* PBMUIM_H */
