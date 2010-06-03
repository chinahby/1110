#ifndef PBM_H
#define PBM_H

/*===========================================================================

           P H O N E B O O K  M A N A G E R  H E A D E R  F I L E

DESCRIPTION
  This is the interface exported by phone book manager service.

 Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbm.h#22 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/02/06   cvs     Header file cleanup for WINCE
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
02/24/06   cvs     Reduce pbm_conversion footprint
02/23/06   cvs     rework memory allocation macros
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/15/05   cvs     Lint fixes
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/12/05   AT      Added UCS2 support for strings.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Added support for read lock, multiple lock types.
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
12/17/04   SUN     Fixed Compilation
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock
09/28/04   PA      Initial Revision
===========================================================================*/

#include "target.h"
#include "customer.h"

#include "err.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "mmgsdilib.h"
#endif

#define EMERGENCY_NUMBERS 10 
#include "pbmlib.h"
#include "pbmtask.h"

#define ECC_LEN 3       /* Length of an entry in NV. */
#define MAX_APPS        3

extern rex_crit_sect_type pbm_crit_sect;
extern rex_crit_sect_type pbm_write_lock_crit_sect;
extern rex_crit_sect_type pbm_read_lock_crit_sect;

typedef struct {
  rex_tcb_type         *lock_owner; /* Task that owns the lock, if set indecates that 
                                       the lock is acquired */
  pbm_lock_type_e_type  lock_type;  /* Type of lock */
  boolean               done;       /* TRUE if async processing is done. */
} pbm_lock_s_type;

extern pbm_lock_s_type g_lock;

#ifdef FEATURE_MMGSDI
extern uint32             ecc_initialized;
extern boolean            sp_ecc_enabled;
#endif

extern boolean pbm_wms_init_finished;

typedef struct {
  uint8  nref;
  uint16 unique_id; 
  uint16 record_id;
  uint16 category;
  uint16 num_fields;
  pbm_field_s_type *fields;
} pbm_addr_cache_s_type;

typedef struct pbm_addr_node_s {
  uint16 rec_id;
  pbm_addr_cache_s_type *record;
  struct pbm_addr_node_s *next;
} pbm_addr_node_s_type;

typedef struct {
  boolean update_psc;
  uint16  cc;
  uint32  psc;
} pbm_write_sync_s_type;

typedef enum {
  PBM_WRITE_NOT_INIT,
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_WRITE_SYNC,
  PBM_WRITE_PREV_USIM,
  PBM_WRITE_MASTER,
  PBM_WRITE_CURR_USIM,
  PBM_WRITE_RESETPBC,
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PBM_WRITE_DONE 
} pbm_write_state_e_type;

typedef struct pbm_notify_node_s {
  PBM_NOTIFY_FUNC           notify_func;
  void                     *user_data;
  struct pbm_notify_node_s *next;
} pbm_notify_node_s_type;

extern pbm_notify_node_s_type *pbm_notify_list;
extern pbm_addr_node_s_type *pbm_committed_list;

/* l-value of pointers should not be NULL */
#define PBM_LIST_SEARCH(recid, prev_node, curr_node, found)  \
  {                                                          \
    (found) = FALSE;                                         \
    (prev_node) = NULL;                                      \
    (curr_node) = pbm_committed_list;                        \
    while ((curr_node))                                      \
    {                                                        \
      if ((curr_node)->rec_id == (recid))                    \
      {                                                      \
        (found) = TRUE;                                      \
        break;                                               \
      }                                                      \
      (prev_node) = (curr_node);                             \
      (curr_node) = (curr_node)->next;                       \
    }                                                        \
  }

#define PBM_LIST_REMOVE(prev_node, curr_node)                \
  {                                                          \
    if ((prev_node))                                         \
      (prev_node)->next = (curr_node)->next;                 \
    else                                                     \
      pbm_committed_list = (curr_node)->next;                \
    PBM_MEM_FREEIF(curr_node);                               \
  }

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  uint16                  rec_id;
  pbm_addr_cache_s_type  *curr_rec;
  pbm_addr_cache_s_type  *prev_rec;
#ifdef FEATURE_PBM_USIM_SUPPORT
  uint8                  *curr_usim_index;
  uint8                  *prev_usim_index;
  pbm_write_sync_s_type   sync;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PBM_WRITE_CB_FUNC       proc_func;
  void                   *user_data;
} pbm_write_record_s_type;

/*lint -emacro(740, PBM_WRITE_CMD_PTR) */
#define PBM_WRITE_CMD_PTR(x) ((pbm_write_record_s_type *)(x))

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  pbm_lock_type_e_type lock_type;
  PBM_WRITE_CB_FUNC proc_func;
  void *user_data;
} pbm_write_lock_s_type;

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  pbm_notify_data_s_type notify_data;
} pbm_notify_evt_s_type;

typedef struct {
  int     index; /* Logical index across multiple phonebook sets */
  char    number[PBM_NUM_SIZE]; /* + and null for string */
  uint8   type; /* type of address octet in integer format */
  uint16  text[PBM_TEXT_SIZE_CHARS]; /* null terminated UCS2 string */
#ifdef FEATURE_PBM_USIM_SUPPORT
  /* Following fields added to support USIM */
  uint32  time;  /* Date and time in clock seconds */
  uint32  duration;  /* in seconds, USIM supports up to 0x00FFFFFF */
  boolean is_answered;  /* TRUE = answered, FALSE = not answered */ 
#endif /* FEATURE_PBM_USIM_SUPPORT */
} pbm_num_text_record_s_type;

/* A PFSOURCE_FN function gets the next input character as UCS2 and moves the
 * parameter pointer to the next character. */
typedef uint16  (*PFSOURCE_FN)(const void **in_str_ptr);

/*lint -emacro(740, PBM_WRITE_LOCK_CMD_PTR) */
#define PBM_WRITE_LOCK_CMD_PTR(x) ((pbm_write_lock_s_type *)(x))

/*===========================================================================
FUNCTION PBM_INIT

DESCRIPTION
  This function is called after PIN is verified.  It performs these primary
  functions.

  1) Read ADN into memory
  2) Read EXT file attributes
  3) Set up linked list from ADN.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init ( void );

/*===========================================================================
FUNCTION PBM_INIT_FILE_INFO

DESCRIPTION
  No longer used.  Remains in build until other APPs remove references to
  this.

DEPENDENCIES
  None.  Note used.

SIDE EFFECTS
  None.  Note used.
===========================================================================*/
void pbm_init_file_info(void);


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
                                            uint8 usim_ecc_serv_cat);

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
void pbm_ecc_clear_all_id(pbm_field_id_e_type field_id);

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
void pbm_ecc_set_hardcoded_eccs(boolean uim_present, boolean ecc_present);

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_GET_ECC_LIST

DESCRIPTION
  This function returns the ECC data corresponding to given slot and app.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
gsdi_ef_ecc_data_T *pbm_get_ecc_list(uint8 slot, uint8 app);
#endif /* FEATURE_MMGDI */

/*===========================================================================
FUNCTION PBM_WRITE_NOTIFY

DESCRIPTION
  This function sends the asynchronous result of a PBM write operation

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_notify(const pbm_write_record_s_type *write, pbm_return_type ret_val);

/*===========================================================================
FUNCTION PBM_INT_RECORD_WRITE

DESCRIPTION
  This function performs the actual write of the record to UIM or EFS.
  It is always called from within PBM Task. The address record is already 
  added to cache from within the library function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_int_record_write(pbm_write_record_s_type *write);

/*===========================================================================
FUNCTION PBM_WRITE_CMD_FREE

DESCRIPTION
  This function frees any references to cache records and frees
  the command structure itself. 

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_cmd_free(pbm_write_record_s_type *write);

#ifdef FEATURE_STA_PBM
/*===========================================================================
FUNCTION PBM_INIT_ECC

DESCRIPTION
  Legacy function, currently only called from STA. This function should not be 
  called from outside PBM. 

  The new ECC initialzation function is pbm_ecc_init() and it is called during
  PBM task startup.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init_ecc(void);
#endif /* FEATURE_STA_PBM */

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
  for an Update operation on EFS.

  In both cases above, we no longer need the node in commiitted list after this
  and hence the node is removed. Also, if the entry in cache is a dummy record, 
  it is converted to a NULL.

  In case of UIM, delete and add is the same operation. In this case, this 
  function should always be called with 'partial_update' set to FALSE.

DEPENDENCIES
  None. No need to be in critical section to call this function.

SIDE EFFECTS
===========================================================================*/
void pbm_list_recover(const pbm_write_record_s_type *write, boolean delete_succeed);

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
pbm_return_type pbm_list_backup(uint16 rec_id);

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
void pbm_list_update(const pbm_write_record_s_type *write);

#endif /* ifndef PBM_H */
