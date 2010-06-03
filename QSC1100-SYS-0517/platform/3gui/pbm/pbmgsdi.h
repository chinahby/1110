#ifndef PBMGSDI_H
#define PBMGSDI_H

/*===========================================================================

                  INTERNAL HEADER FILE FOR PBM GSDI WRAPPERS

DESCRIPTION
  This is the internal header file that contains Phonebook Manager
  wrappers for GSDI functions.

 Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbmgsdi.h#23 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
05/18/07   cvs     Make pbm_handle_sim_init_completed() public
02/22/07   cvs     Keep track of filetype for read + write ops
07/25/06   slw     Added error reporting for pbm_create_sim_record
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
01/25/06   cvs     Lint fixes
12/13/05   cvs     Implement partial PB refresh
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/16/05   cvs     Lint Fixes
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
03/21/05   PA      Fix IAP update issue in Type 2 ANR
02/08/05   PA      Made alphabet translation table constant.
01/18/05   PA      Support for converting 7-bit default alphabet
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds
09/29/04   PA      Initial revision.
===========================================================================*/
#include "pbmcache.h"
#include "pbm.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "mmgsdilib.h"

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  gsdi_cnf_T gsdi_data;
} pbm_gsdi_resp_s_type;

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  mmgsdi_return_enum_type status;
  mmgsdi_cnf_enum_type    data_type;
  mmgsdi_cnf_type         data;
} pbm_mmgsdi_resp_s_type;

/*lint -emacro(740, PBM_GSDI_RESP_PTR) */
#define PBM_GSDI_RESP_PTR(x) ((pbm_gsdi_resp_s_type *)(x))
/*lint -emacro(740, PBM_MMGSDI_RESP_PTR) */
#define PBM_MMGSDI_RESP_PTR(x) ((pbm_mmgsdi_resp_s_type *)(x))

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  mmgsdi_refresh_file_list_type  refresh_file_list;
  mmgsdi_slot_id_enum_type       slot;
} pbm_gsdi_file_change_s_type;

/*lint -emacro(740, PBM_GSDI_FILECHANGE_PTR) */
#define PBM_GSDI_FILECHANGE_PTR(x) ((pbm_gsdi_file_change_s_type *)(x))

typedef struct {
  PBM_DEFINE_CMD_HEADER()
  mmgsdi_event_data_type   mmgsdi_event;
} pbm_mmgsdi_card_status_s_type;

/*lint -emacro(740, PBM_MMGSDI_CARDSTATUS_PTR) */
#define PBM_MMGSDI_CARDSTATUS_PTR(x) ((pbm_mmgsdi_card_status_s_type *)(x))

#define PBM_MMGSDI_SLOT_TO_PBM_SLOT(x) (((x)==MMGSDI_SLOT_1)?PBM_SLOT_1:PBM_SLOT_2)
#define PBM_PBM_SLOT_TO_MMGSDI_SLOT(x) (((x)==PBM_SLOT_1)?MMGSDI_SLOT_1:MMGSDI_SLOT_2)

typedef struct {
  boolean                  in_use;
  pbm_sim_record_s_type    record;
  byte                     extension;

#ifdef FEATURE_PBM_USIM_DEBUG
  int                      debug_num_writes;
#endif

  pbm_file_device_e_type   m_file_id;
  pbm_write_state_e_type   state;
  uint16                   findex;
  uint16                   findex_delta;
  pbm_write_record_s_type *write;
  boolean                 *caches_in_use;

  pbm_cmd_e_type           gsdi_resp_type;
  void (*proc_func)( pbm_return_type, pbm_sim_record_s_type *);
} pbm_async_gsdi_buffer;

extern pbm_async_gsdi_buffer pbm_buffers[];

extern const uint8 gsm7_to_pbm8[];
extern const uint8 pbm8_to_gsm7[];

/* Flag to be checked when PBM INIT finishes */
extern boolean             pbm_refresh_init;

/* Number of files in refresh request */
extern uint8               pbm_refresh_num_files;

/* Buffer to maintain file list in notify */
extern mmgsdi_file_enum_type pbm_refresh_file_list[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];

/* refresh file list translated into affected PBM files */
extern pbm_file_device_e_type pbm_refresh_pbm_file_devices[PBM_FILE_MAX_REQUESTS];

/* Number of file devices in refresh request */
extern uint8               pbm_refresh_num_file_devices;

/* Relevant data when doing the SEEK operation */
typedef struct
{
  pbm_return_type status;  /* status of the SEEK operation */
  uint8 *in_use_records;   /* list of used and unused records */
  int num_of_records;      /* size of in_use_records[] */
  int record_length;       /* length of each record */
  int records_in_pb_set[PBM_MAX_NUM_PB_SETS];           /* number of records in each pb set */
  mmgsdi_file_enum_type filenames[PBM_MAX_NUM_PB_SETS]; /* filename for each pb set */
  int num_files;           /* number of files */
  int cur_file;            /* index of current file being SEEKed */
} pbm_seek_data_type;

extern pbm_seek_data_type pbm_seek_data[];

/*===========================================================================
FUNCTION PBM_ALLOC_RECORD

DESCRIPTION
  During async calls, we need to store the memory location in which the
  final answer will be stored.  This call will allocate a buffer and
  return the index.  This index is passed to and from GSDI to know where
  to place the buffers.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_alloc_record(void);

/*===========================================================================
FUNCTION PBM_FREE_RECORD

DESCRIPTION
  Mark the pbm_buffer as available for later use.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_record(int record);

/*===========================================================================
FUNCTION PBM_GSDI_INIT

DESCRIPTION
  Initialize GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_init(void);

/*===========================================================================
FUNCTION PBM_GSDI_SIM_READ

DESCRIPTION
  This function is the interface to the gsdi_sim_read function.  When we support
  multiple SIMs, then this call will call gsdi2_sim_read, if we are in a single
  SIM environment, then the old API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_gsdi_sim_read( pbm_slot_type          sim_id,
                                   mmgsdi_file_enum_type  sim_filename,
                                   mmgsdi_file_structure_enum_type sim_filetype,
                                   int                    rec_num,
                                   int                    required_data_len,
                                   uint32                 client_ref,
                                   mmgsdi_callback_type   async_cb);

/*===========================================================================
FUNCTION PBM_GSDI_UIM_WRITE

DESCRIPTION
  This function is the interface to the gsdi_sim_write function.  When we support
  multiple SIMs, then this call will call gsdi2_sim_write, if we are in a single
  SIM environment, then the old API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_uim_write (
  pbm_slot_type          sim_id,
  mmgsdi_file_enum_type  sim_filename,
  mmgsdi_file_structure_enum_type  sim_filetype,
  int                    rec_num,
  byte *                 data_p,
  int                    data_len,
  uint32                 client_ref,
  mmgsdi_callback_type   async_cb
);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_USIM_WRITE_CB

DESCRIPTION TODO: Template, needs actual code
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_usim_write_cb(gsdi_cnf_T *sim_data);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_GSDI_IS_SERVICE_ALLOWED

DESCRIPTION
  This function checks the SIM Service table to determine if the given service
  is allowed on the given SIM.  When we are in a single UIM environment, the
  old API is used, otherwise, the gsdi2 API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsdi_is_service_allowed (pbm_slot_type sim_id,
                                     gsdi_services_type service);

/*===========================================================================
FUNCTION PBM_GSDI_GET_FILE_ATTRIBUTES

DESCRIPTION
  This function is the interface to the gsdi_get_file_attributes function.
  When we support multiple SIMs, then this call will call
  gsdi2_get_file_attributes, if we are in a single SIM environment, then
  the old API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_get_file_attributes (
  pbm_slot_type           sim_id,
  mmgsdi_file_enum_type   sim_filename,
  uint32                  client_ref,
  mmgsdi_callback_type    async_cb
);


/*===========================================================================
FUNCTION PBM_GSDI_GET_APPS_AVAILABLE

DESCRIPTION
  This function calls the appropriate get_apps_available from GSDI.  If
  we have multiple SIMs, we use the gsdi2 version of the function, otherwise
  we call the original version.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_get_apps_available( pbm_slot_type sim_id,
                                  uint8 *ret_card_apps );

/*===========================================================================
FUNCTION PBM_CREATE_SIM_RECORD

DESCRIPTION
  Does the writes of the non-extension record.
  If the phone number is longer than the max for the file in question
  (e.g. >ADN_MAX_PHONE_NUMBER), the remainder of the phone number was
  stored in the extension file after it was converted into BCD.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_sim_record(pbm_slot_type sim_id,
                                      pbm_file_device_e_type file_id,
                                      pbm_num_text_record_s_type *record,
                                      int buffer_index );

/*===========================================================================
FUNCTION PBM_GSDI_PHONEBOOK_PRESENT

DESCRIPTION
  Used to determine if a global or local phonebook is present on the given
  SIM.  This function is used when we don't have the new GSDI.  Once the
  new GSDI is present, this function will not be used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsdi_phonebook_present(pbm_slot_type sim_id, gsdi_phonebook_type phonebook);

/*===========================================================================
FUNCTION PBM_SEND_READ_REQUEST

DESCRIPTION
  Populate the msg struct for GSDI including the callback function the
  call gsdi to process the read request.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_send_read_request( pbm_slot_type sim_id,
                               pbm_file_device_e_type file_id,
                               mmgsdi_file_enum_type sim_filename,
                               int rec_num,
                               int pbm_buffer_index,
                               mmgsdi_callback_type callback);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_USIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the USIM, pbm_gsdi_usim_read_cb is
  called to copy the data in. At this point we process the data from USIM into
  a generic SIM record type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_usim_read_cb( mmgsdi_return_enum_type status,
                            const mmgsdi_cnf_type  *data_ptr);

#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_MMGSDI_GSM
/*===========================================================================
FUNCTION PBM_GSDI_SIM_GET_ECC

DESCRIPTION
  This function calls the appropriate get_ecc from GSDI.  If
  we have multiple SIMs, we use the gsdi2 version of the function,
  otherwise we call the original version.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_sim_get_ecc( pbm_slot_type sim_id,
                                      gsdi_ef_ecc_data_T * sim_ecc );
#endif /* FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_UMTS
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_USIM_GET_ECC

DESCRIPTION
  This function calls the appropriate get_ecc from GSDI.  If
  we have multiple SIMs, we use the gsdi2 version of the function,
  otherwise we call the original version.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_usim_get_ecc( pbm_slot_type sim_id,
                                       gsdi_ef_ecc_data_T * usim_ecc );
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA

/*===========================================================================
FUNCTION PBM_GSDI_RUIM_GET_ECC

DESCRIPTION
  This function calls the appropriate get_ecc from GSDI.  If
  we have multiple SIMs, we use the gsdi2 version of the function,
  otherwise we call the original version.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_ruim_get_ecc( pbm_slot_type sim_id,
                                       gsdi_ef_ecc_data_T * ruim_ecc );
#endif /* FEATURE_MMGSDI_CDMA */

/*===========================================================================
FUNCTION PBM_GSDI_REFRESH_COMPLETE

DESCRIPTION
  This function sends refresh complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_refresh_complete(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_GSDI_CLIENT_INIT_COMPLETE

DESCRIPTION
  This function sends client init complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_client_init_complete(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_GSDI_SIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the SIM, PBM_READ_CB is
  called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_sim_read_cb( mmgsdi_return_enum_type status,
                           const mmgsdi_cnf_type  *data_ptr);
#ifdef FEATURE_ECC_HANDLING_AT_PBM
/*===========================================================================
FUNCTION PBM_GSDI_ECC_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the SIM, PBM_READ_CB is
  called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_ecc_read_cb( mmgsdi_return_enum_type status,
                           const mmgsdi_cnf_type  *data_ptr);
#endif

/*===========================================================================
FUNCTION PBM_GSDI_EXT_READ_CB

DESCRIPTION
  After GSDI has retreived the extension record from EXT1 or EXT5,
  PBM_EXT_READ_CB is called to copy the data in to a pbm_buffer
  for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_ext_read_cb( mmgsdi_return_enum_type status,
                           const mmgsdi_cnf_type  *data_ptr);
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_INDEX_READ_CB

DESCRIPTION
  After GSDI has retreived the extension record from EXT1 or EXT5,
  PBM_EXT_READ_CB is called to copy the data in to a pbm_buffer
  for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_index_read_cb( mmgsdi_return_enum_type status,
                             const mmgsdi_cnf_type  *data_ptr);

/*===========================================================================
FUNCTION PBM_GSDI_SYNC_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_sync_read_cb( mmgsdi_return_enum_type status,
                            const mmgsdi_cnf_type  *data_ptr);
#endif /* FEATURE_PBM_USIM_SUPPORT */
/*===========================================================================
FUNCTION PBM_GSDI_ASYNC_CB

DESCRIPTION
  This function is registered with GSDI. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_async_cb(mmgsdi_return_enum_type status,
                       mmgsdi_cnf_enum_type    data_type,
                       const mmgsdi_cnf_type  *data_ptr);

/*===========================================================================
FUNCTION PBM_GSDI_UIM_WRITE_CB

DESCRIPTION
  After GSDI has written any SIM/USIM record, this function is
  called. The appropriate callback function is called based on the
  file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_uim_write_cb( mmgsdi_return_enum_type status,
                            mmgsdi_cnf_enum_type    data_type,
                            const mmgsdi_cnf_type  *data_ptr);

/*===========================================================================
FUNCTION PBM_GSDI_READ_CB

DESCRIPTION
  After GSDI has read any SIM/USIM record this function is called.
  This function makes sure that the data sent by GSDI is sane.

  The appropriate pbm read callback is called based on the file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_read_cb( mmgsdi_return_enum_type status,
                       mmgsdi_cnf_enum_type    data_type,
                       const mmgsdi_cnf_type  *data_ptr);
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_READ_PBR_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_read_pbr_cb(gsdi_cnf_T *sim_data);
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_FREE_EXTENSION_RECORD

DESCRIPTION
  When we will overwrite a record, this routine will find the
  extension record associated with that record and free that in
  our cache structures.  It returns the record it freed, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_free_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                              mmgsdi_file_enum_type filename);

/*===========================================================================
FUNCTION PBM_FIND_EXTENSION_RECORD

DESCRIPTION
  When its time to pick an extension record, this routine will find one.
  It returns the record it found, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_find_extension_record(pbm_ext_cache_s_type *cache, int index,
                              mmgsdi_file_enum_type filename);

/*===========================================================================
FUNCTION PBM_CHECK_EXTENSION_RECORD

DESCRIPTION
  This routine will return TRUE if a record is currently using an extension
  record, or FALSE if not.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_check_extension_record(pbm_ext_cache_s_type *cache, int index,
                                   mmgsdi_file_enum_type filename);

/*===========================================================================
FUNCTION PBM_GSDI_FILEINFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.  It puts a command into PBM GSDI response qeuue
  and the actual handling is done in pbm_fileinfo_cb.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_gsdi_fileinfo_cb (mmgsdi_return_enum_type status,
                           mmgsdi_cnf_enum_type    data_type,
                           const mmgsdi_cnf_type  *data_ptr);

/*==============================================================================

FUNCTION NAME pbm_refresh_file_change_notification_cb

DESCRIPTION
  Function that gets called by GSDI when a REFRESH Request is made and a
  determination must be made whether to proceed or not.

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void pbm_refresh_file_change_notification_cb
(
  mmgsdi_refresh_file_list_type  *refresh_file_list_p,
  mmgsdi_slot_id_enum_type       slot
);

/*===========================================================================
FUNCTION PBM_SIM_CAP_CB

DESCRIPTION
  Callback function to return SIM Capability.

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_sim_cap_cb(const gsdi_cnf_T *sim_data);

/*===========================================================================
FUNCTION PBM_GSDI_SIM_CAP_CB

DESCRIPTION
  Callback function to return SIM Capability.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_gsdi_sim_cap_cb(gsdi_cnf_T *sim_data);

/*===========================================================================
FUNCTION PBM_PIN_STATUS_CB

DESCRIPTION
  Callback function to return SIM PIN status.

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_pin_status_cb(gsdi_cnf_T *sim_data);

/*===========================================================================
FUNCTION PBM_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_card_status_cb(
    mmgsdi_event_data_type   gsdi_event
);

/*===========================================================================
FUNCTION PBM_HANDLE_SIM_INIT_COMPLETED

DESCRIPTION
  Function to begin initializing PBM SIM Caches, based on SIM init completed,
  or PIN unblocked.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_handle_sim_init_completed (
  pbm_slot_type slot
);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_gsdi_process_text_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_process_text_data( pbm_slot_type sim_id,
                                            pbm_file_device_e_type file_id,
                                            int index,
                                            byte *data_buffer,
                                            pbm_text_record_s_type *return_data);

/*===========================================================================
FUNCTION pbm_gsdi_process_anr_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the anr record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_process_anr_data( pbm_slot_type sim_id,
                                           pbm_file_device_e_type file_id,
                                           int index,
                                           byte *data_buffer,
                                           pbm_anr_record_s_type *return_data,
                                           int *extension_index);

/*===========================================================================
FUNCTION pbm_gsdi_process_grp_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_process_grp_data( pbm_slot_type sim_id,
                                           pbm_file_device_e_type file_id,
                                           int index,
                                           byte *data_buffer,
                                           pbm_grp_record_s_type *return_data);

/*===========================================================================
FUNCTION pbm_gsdi_process_index_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_process_index_data( pbm_slot_type sim_id,
                                             pbm_file_device_e_type file_id,
                                             int index,
                                             byte *data_buffer,
                                             pbm_iap_record_s_type *return_data);

/*===========================================================================
FUNCTION PBM_CREATE_ANR_RECORD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_anr_record(pbm_slot_type sim_id,
                                      pbm_file_device_e_type file_id,
                                      pbm_anr_record_s_type *anr_record,
                                      uint32 buffer_index );
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_CREATE_EXT_RECORD

DESCRIPTION
  This creates an extension record, and writes it to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_ext_record(uint32 buffer_index,
                          pbm_ext_cache_s_type *ext_cache,
                          const byte *num_buffer,
                          uint16 m_index,
                          mmgsdi_file_enum_type m_filename);
/*===========================================================================
FUNCTION PBM_CLEAR_EXT_RECORD

DESCRIPTION
  This finds the extension record associated with a record, and
  writes a blank record in its place to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_ext_record(uint32 buffer_index,
                          pbm_ext_cache_s_type *ext_cache,
                          uint16 m_index,
                          mmgsdi_file_enum_type m_filename);

/*===========================================================================
FUNCTION   pbm_get_card_apps

DESCRIPTION
  This function is called to get the card applications

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 pbm_get_card_apps(pbm_slot_type slot);

/*===========================================================================
FUNCTION   pbm_set_card_apps

DESCRIPTION
  This function is called to set the card applications

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_set_card_apps(pbm_slot_type slot, uint8 card_apps);

/*===========================================================================
FUNCTION PBM_INIT_SEEK_DATA

DESCRIPTION
   Initialize the members of pbm_seek_data

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_init_seek_data(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_ALLOC_SEEK_DATA

DESCRIPTION
   Allocate memory for in_use_records and set all the records to USED for
   the corresponding sim_id

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_alloc_seek_data(pbm_slot_type sim_id, int num_of_records);

/*===========================================================================
FUNCTION PBM_FREE_SEEK_DATA

DESCRIPTION
   Free memory for in_use_records in pbm_seek_data corresponding to sim_id

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_free_seek_data(pbm_slot_type sim_id);

/*===========================================================================
FUNCTION PBM_GET_NEXT_USED_RECORD

DESCRIPTION
   Returns the next used record number starting after prev_index. If the
   memory allocation for SEEK data failed or the SEEK was unsuccessful,
   returns the next record in the sequence starting from prev_index.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_get_next_used_record(pbm_slot_type sim_id, int prev_index);

/*===========================================================================
FUNCTION PBM_SEND_SEEK_REQUEST

DESCRIPTION
   This function sends the SEEK request to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_send_seek_request(pbm_slot_type sim_id, uint32 client_ref);

/*===========================================================================
FUNCTION PBM_SEEK_CB

DESCRIPTION
   This function is called in the context of the PBM task when PBM_GSDI_SEEK_CB
   command is put in the GSDI queue. This function updates the in_use_records[]
   with the list of unused records SEEKed and starts reading the USED records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_seek_cb(mmgsdi_return_enum_type status,
                 mmgsdi_cnf_enum_type    data_type,
                 const mmgsdi_cnf_type  *data_ptr);

#endif /* FEATURE_MMGSDI */
#endif /* PBMGSDI_H */
