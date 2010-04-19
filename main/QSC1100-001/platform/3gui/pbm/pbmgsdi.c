/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmgsdi.c#64 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files during power up.
16/03/09   kcp     resolved Lint errors
02/02/09   kcp     165174 - After deleting a contact,
                   2nd byte of EF-ANR is 00 instead of FF
09/04/08   sg      Fix the SEEK/SEARCH pattern for GRP files in pbm_get_seek_pattern()
09/03/08   sg      Add proper featurization and multiple phonebook support in
                   pbm_get_seek_pattern()
09/03/08   sg      Return a SEARCH/SEEK pattern of 0x00 for UID and PBC
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
                   and read records sequentially if the feature is not defined
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
06/13/08   cvs     New GCF field trial docs move ECC number changes to SIM inserted
04/07/08   clm     Handle MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN stage while
                   in MMGSDI_REFRESH_NAA_INIT_FCN mode for MMGSDI refresh events.
03/13/08   clm     Add notification of PBM_EVENT_PB_REFRESH_START/DONE for
                   both local and global USIM files.
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
                   Free pbm_buffer from read cb func if restart is needed.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
05/18/07   cvs     Make pbm_handle_sim_init_completed() public
03/12/07   cvs     Featurize 4 PB set support
02/23/07   cvs     Handle PIN enable/disable only as needed.
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
12/16/06   cvs     Only respond to PIN events for the provisioning app
12/15/06   cvs     Register for refresh on all files, in one call to fix refresh
11/01/06   cvs     multi pb set fixes
10/24/06   cvs     Register for refresh on all files, to fix FDN refresh issue
10/10/06   cvs     Move messages to PBM's diag msg ID
08/17/06   cvs     Fix race condition with GSDI refresh before done initializing.
07/25/06   slw     Added error reporting for pbm_create_sim_record
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/23/06   cvs     rework memory allocation macros
02/09/06   cvs     When can't encode all characters on SIM, return errors
02/07/06   cvs     better log messages
01/27/06   cvs     don't add UIM_NO_SUCH_ITEMs to refresh list,
                   make refresh list decisions based on USIM local PB
01/09/06   cvs     Lint fixes
12/13/05   cvs     Implement partial PB refresh
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/16/05   cvs     Lint Fixes
09/30/06   cvs     Fix error message
09/14/05   cvs     Properly handle empty email entries.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.  A bit of code cleanup.
07/11/05   cvs     Better error messages
06/24/05   cvs     Fix dual IMSI FDN support
06/14/05   cvs     Fix GCF failures related to emergency numbers
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications.
02/17/05   PA      Made alphabet translation table constant.
02/14/05   PA      Do not stop initialization if extension record type is
                   not recognized.
01/18/05   PA      Convert 7-bit SMS default alphabet to PBM 8-bit ascii and
                   fix compilation errors for non-MMGSDI builds
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds, code review comments
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK

#include "pbm.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdiutil.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask.h"
#include "pbm_conversion.h"
#include "pbmuim.h"
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_MMGSDI
/* <EJECT> */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/*===========================================================================
         Use the card_apps to know what is available on the SIM.
===========================================================================*/
static uint8 pbm_card_apps[MAX_UIMS]; /* SIM vs USIM */

static mmgsdi_client_id_type pbm_mmgsdi_client_id = 0;

extern uint32 ecc_has_entries;

static sim_capabilities_T pbm_sim_cap;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
static sim_capabilities_T pbm_sim_cap_2;
#endif

/* Flag to be checked when PBM INIT finishes */
boolean             pbm_refresh_init  = FALSE;

/* Number of files in refresh request */
uint8               pbm_refresh_num_files = 0;

/* Buffer to maintain file list in notify */
mmgsdi_file_enum_type pbm_refresh_file_list[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];

/* refresh file list translated into affected PBM files */
pbm_file_device_e_type pbm_refresh_pbm_file_devices[PBM_FILE_MAX_REQUESTS];

/* Number of file devices in refresh request */
uint8               pbm_refresh_num_file_devices = 0;

static boolean            refresh_pass_fail;         /* Used to indicate to
                                                        GSDI if refresh pass
                                                        0x01 -> PASS
                                                        0x00 -> FAIL
                                                     */

/* provisioning app info */
static mmgsdi_aid_type pbm_provisioning_gsdi_aid[MAX_UIMS][MMGSDI_MAX_APP_INFO];
static uint32          pbm_num_provisioning_gsdi_aid[MAX_UIMS];

static void pbm_mmgsdi_card_status_cb(const mmgsdi_event_data_type *mmgsdi_event);

static void pbm_mmgsdi_refresh_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    cnf,
                                  const mmgsdi_cnf_type  *cnf_ptr);
static void pbm_mmgsdi_init_complete_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr);
static void pbm_mmgsdi_ok_to_refresh_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr);
static boolean pbm_gsdi_process_sim_data(pbm_slot_type sim_id,
                                         pbm_file_device_e_type file_id,
                                         int index,
                                         byte *local_pbm_data_buffer,
                                         pbm_num_text_record_s_type *return_data,
                                         int *extension_index);
static boolean pbm_gsdi_process_ext_data(byte *sim_data,
                                  byte *number);
static void pbm_clear_refresh_caches(pbm_slot_type sim_id);

static boolean pbm_update_refresh_globals(
  mmgsdi_refresh_file_list_type  *refresh_file_list_p,
  mmgsdi_slot_id_enum_type       slot);

pbm_async_gsdi_buffer pbm_buffers[PBM_ASYNC_BUFFERS];

/* For storing SEEK data */
pbm_seek_data_type pbm_seek_data[MAX_UIMS];

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
int pbm_alloc_record(void)
{
  int i;
  int retval = -1;
  rex_enter_crit_sect(&pbm_crit_sect);
  for (i = 0; i < PBM_ASYNC_BUFFERS; i++)
  {
    if (!pbm_buffers[i].in_use)
    {
      memset(&pbm_buffers[i], 0, sizeof(pbm_buffers[i]));
      pbm_buffers[i].in_use = TRUE;
      retval = i;
      break;
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);

  return retval;
}

/*===========================================================================
FUNCTION PBM_FREE_RECORD

DESCRIPTION
  Mark the pbm_buffer as available for later use.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_record(int record)
{
  pbm_buffers[record].in_use = FALSE;
}

/*===========================================================================
FUNCTION PBM_MMGSDI_STATUS_CB

DESCRIPTION
  Callback function from MMGSDI to log errors

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_mmgsdi_status_cb (mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    data_type,
                                  const mmgsdi_cnf_type  *data_ptr)
{
  if (status == MMGSDI_SUCCESS)
  {
    if (data_type == MMGSDI_CLIENT_ID_REG_CNF)
    {
      PBM_CHECK_PTR_RET(data_ptr, VOID);

      pbm_mmgsdi_client_id =
                          data_ptr->client_id_reg_cnf.response_header.client_id;
    }
  }
  else
  {
    switch(data_type)
    {
      case MMGSDI_CLIENT_ID_REG_CNF:
        PBM_MSG_HIGH("MMGSDI failed registering PBM client ID.  Status %d, Confirmation %d",
                 status, data_type, 0);
        break;
      case MMGSDI_CLIENT_EVT_REG_CNF:
        PBM_MSG_HIGH("MMGSDI failed registering PBM event handler.  Status %d, Confirmation %d",
                 status, data_type, 0);
        break;
      default:
        PBM_MSG_HIGH("MMGSDI returned !SUCCESS async.  Status %d, Confirmation %d",
                 status, data_type, 0);
        break;
    }
  }

  //unblock PBM task
  (void) rex_set_sigs(&pbm_tcb, PBM_MMGSDI_WAIT_SIG);
}

/*===========================================================================
FUNCTION PBM_GSDI_INIT

DESCRIPTION
  Initialize GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_gsdi_init()
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_option_type no_option = {0};

  PBM_MSG_HIGH("Begin PBM GSDI init",0,0,0);

  memset(&pbm_fileinfo_requests[0], 0, sizeof(pbm_fileinfo_requests));
  memset(pbm_buffers, 0, sizeof(pbm_buffers));

  memset(pbm_provisioning_gsdi_aid, 0, sizeof(pbm_provisioning_gsdi_aid));
  memset(pbm_num_provisioning_gsdi_aid, 0, sizeof(pbm_num_provisioning_gsdi_aid));

  (void) rex_clr_sigs(&pbm_tcb, PBM_MMGSDI_WAIT_SIG);

  mmgsdi_status = mmgsdi_client_id_reg_set_notify(TRUE, no_option, pbm_mmgsdi_status_cb, 0);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("mmgsdi_client_id_reg failed %d", mmgsdi_status, 0, 0);
    return PBM_ERROR;
  }

  //block waiting for callback
  (void) pbm_wait (PBM_MMGSDI_WAIT_SIG);
  (void) rex_clr_sigs(&pbm_tcb, PBM_MMGSDI_WAIT_SIG);

  mmgsdi_status = mmgsdi_client_evt_reg(pbm_mmgsdi_client_id,
                                        pbm_mmgsdi_card_status_cb,
                                        pbm_mmgsdi_status_cb,
                                        0);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("mmgsdi_client_evt_reg failed %d", mmgsdi_status, 0, 0);
    return PBM_ERROR;
  }

  //block waiting for callback
  (void) pbm_wait (PBM_MMGSDI_WAIT_SIG);
  (void) rex_clr_sigs(&pbm_tcb, PBM_MMGSDI_WAIT_SIG);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_gsdi_register_fcn

DESCRIPTION
  Initialize GSDI file change notifications

DEPENDENCIES
  sim init complete and card apps available

SIDE EFFECTS
===========================================================================*/
static void pbm_gsdi_register_fcn(pbm_slot_type sim_id)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_refresh_file_list_type mmgsdi_refresh_file_list = {0, NULL};
  mmgsdi_file_enum_type         mmgsdi_file_list[ARR_SIZE(pbm_file_to_uim_file[0].uim_filename)*
                                                 ARR_SIZE(pbm_file_to_uim_file)];
  int i = 0;
  uint8 j = 0;
  uint32 fileset = 0;
  mmgsdi_slot_id_enum_type card_slot = PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id);
  mmgsdi_option_type no_option = {0};

  memset(&pbm_fileinfo_requests[0], 0, sizeof(pbm_fileinfo_requests));

  /* register for FCN on all UIM files in SIM */
  for (fileset=0; fileset<ARR_SIZE(pbm_file_to_uim_file[0].uim_filename); fileset++)
  {
    for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      if (pbm_file_to_uim_file[i].uim_filename[fileset] != MMGSDI_NO_FILE_ENUM)
      {
        // after mmgsdi is complete, these can just be set equal
        mmgsdi_file_list[j++] = pbm_file_to_uim_file[i].uim_filename[fileset];
      }
    }
  }

  mmgsdi_refresh_file_list.num_files = j;
  mmgsdi_refresh_file_list.file_list_ptr = mmgsdi_file_list;

  mmgsdi_status = mmgsdi_register_for_refresh(pbm_mmgsdi_client_id,
                                              card_slot,
                                              mmgsdi_refresh_file_list,
                                              FALSE,
                                              no_option,
                                              pbm_mmgsdi_refresh_cb,
                                              NULL);

  /* Check to see if REQ for Registration
  ** was ok
  */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("Failed to Request Refresh FCN Registration",0,0,0);
  }

  return;
}




/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_GSDI_SIM_READ

DESCRIPTION
  This function is the interface to the gsdi_sim_read function.  When we support
  multiple SIMs, then this call will call gsdi2_sim_read, if we are in a single
  SIM environment, then the old API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_gsdi_sim_read(
  pbm_slot_type         sim_id,
  mmgsdi_file_enum_type sim_filename,
  mmgsdi_file_structure_enum_type sim_filetype,
  int                   rec_num,
  int                   required_data_len,
  uint32                client_ref,
  mmgsdi_callback_type  async_cb
)
{
  mmgsdi_return_enum_type mmgsdi_retval;

  mmgsdi_slot_id_enum_type slot;
  slot = PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id);

  switch (sim_filetype) {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      mmgsdi_retval = mmgsdi_read_record (pbm_mmgsdi_client_id,
                                          slot,
                                          sim_filename,
                                          rec_num,
                                          required_data_len,
                                          async_cb,
                                          client_ref);
      break;
    case MMGSDI_TRANSPARENT_FILE:
      mmgsdi_retval = mmgsdi_read_transparent (pbm_mmgsdi_client_id,
                                               slot,
                                               sim_filename,
                                               0,
                                               required_data_len,
                                               async_cb,
                                               client_ref);
      break;
    default:
      PBM_MSG_ERR("pbm_gsdi_sim_read got invalid file type %d", sim_filetype, 0, 0);
      return PBM_ERROR;
  }

  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("mmgsdi_read_record returned %d", mmgsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}

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
)
{
  mmgsdi_return_enum_type mmgsdi_retval;
  mmgsdi_write_data_type write_data;

  mmgsdi_slot_id_enum_type slot;
  slot = PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id);

  PBM_MSG_HIGH("PBM: Writing to SIM file: %d, record %d, len %d",
           sim_filename, rec_num, data_len);
  PBM_MSG_HIGH("PBM: Writing to SIM filetype: %d",
           sim_filetype, 0, 0);
  PBM_MSG_HIGH("PBM write to sim data: 0x%x 0x%x 0x%x",
           ((data_p != NULL) && (data_len > 0)) ? data_p[0] : 0,
           ((data_p != NULL) && (data_len > 1)) ? data_p[1] : 0,
           ((data_p != NULL) && (data_len > 2)) ? data_p[2] : 0);
  PBM_MSG_HIGH("PBM write to sim data: 0x%x 0x%x 0x%x",
           ((data_p != NULL) && (data_len > 3)) ? data_p[3] : 0,
           ((data_p != NULL) && (data_len > 4)) ? data_p[4] : 0,
           ((data_p != NULL) && (data_len > 5)) ? data_p[5] : 0);

  write_data.data_len = data_len;
  write_data.data_ptr = data_p;

  switch (sim_filetype) {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      {
        mmgsdi_access_type file_struct;

        file_struct.access_method = MMGSDI_EF_ENUM_ACCESS;
        file_struct.file.file_enum = sim_filename;

        mmgsdi_retval = mmgsdi_write_record_ext (pbm_mmgsdi_client_id,
                                                 slot,
                                                 file_struct,
                                                 sim_filetype,
                                                 rec_num,
                                                 write_data,
                                                 async_cb,
                                                 client_ref);

      }
      break;
    case MMGSDI_TRANSPARENT_FILE:
      mmgsdi_retval =  mmgsdi_write_transparent (pbm_mmgsdi_client_id,
                                                 slot,
                                                 sim_filename,
                                                 0,
                                                 write_data,
                                                 async_cb,
                                                 client_ref);
      break;
    default:
      PBM_MSG_ERR("pbm_gsdi_uim_write got invalid file type %d", sim_filetype, 0, 0);
      return PBM_ERROR;
  }

  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("mmgsdi_write_record returned %d", mmgsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_GSDI_IS_SERVICE_ALLOWED

DESCRIPTION
  This function checks the SIM Service table to determine if the given service
  is allowed on the given SIM.  When we are in a single UIM environment, the
  old API is used, otherwise, the gsdi2 API is used.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{715} suppress param unused.  Can go away with MMGSDI */
boolean pbm_gsdi_is_service_allowed (pbm_slot_type sim_id,
                                     gsdi_services_type service)
{
  gsdi_svr_rsp_type           gsdi_rsp;

  memset(&gsdi_rsp, 0, sizeof(gsdi_rsp));

#if (MAX_UIMS > 1)
  {
    gsdi_slot_id_type slot;
    slot = (sim_id == PBM_SLOT_1)?GSDI_SLOT_1:GSDI_SLOT_2;
    gsdi_rsp = gsdi2_lib_is_service_available(slot, service);
  }
#else
  gsdi_rsp = gsdi_lib_is_service_available(service);
#endif

  if ( gsdi_rsp.gsdi_status == GSDI_SUCCESS )
  {
    /* Now check the Service Bit Map in   */
    /* in the gsdi_rsp */
    return ( gsdi_rsp.svr_bitmap != 0 );
  }

  return FALSE;
}


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
  pbm_slot_type          sim_id,
  mmgsdi_file_enum_type  sim_filename,
  uint32                 client_ref,
  mmgsdi_callback_type   async_cb
)
{
  mmgsdi_return_enum_type mmgsdi_retval;
  mmgsdi_access_type file_access_info;

  mmgsdi_slot_id_enum_type slot;
  slot = PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id);

  memset(&file_access_info, 0, sizeof(file_access_info));

  file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access_info.file.file_enum = sim_filename;

  mmgsdi_retval =  mmgsdi_get_file_attr (pbm_mmgsdi_client_id,
                                         slot,
                                         file_access_info,
                                         async_cb,
                                         client_ref);

  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    PBM_MSG_ERR("mmgsdi_get_file_attr returned %d", mmgsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}


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
                                  uint8 *ret_card_apps )
{
  gsdi_card_apps_T gsdi_card_apps;

  PBM_CHECK_PTR_RET(ret_card_apps, VOID);

  gsdi_get_apps_available( &gsdi_card_apps );
  switch (sim_id)
  {
    case PBM_SLOT_1:
      *ret_card_apps = gsdi_card_apps.apps_enabled;
      break;
#if (MAX_UIMS > 1)
    case PBM_SLOT_2:
      *ret_card_apps = gsdi_card_apps.apps2_enabled;
      break;
#endif
    default:
      PBM_MSG_ERR("Bad sim_id passed to function 0x%x", sim_id, 0, 0);
      break;
  }
}

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
/*lint -e{715} suppress param unused.  Can go away with MMGSDI */
pbm_return_type pbm_gsdi_sim_get_ecc(
    pbm_slot_type        sim_id,
    gsdi_ef_ecc_data_T * sim_ecc )
{
  gsdi_returns_T gsdi_retval;
#if (MAX_UIMS > 1)
  gsdi_slot_id_type slot;
  slot = (sim_id == 0)?GSDI_SLOT_1:GSDI_SLOT_2;
  gsdi_retval = gsdi2_sim_get_ecc( slot, sim_ecc );
#else
  gsdi_retval = gsdi_sim_get_ecc( sim_ecc );
#endif
  if (gsdi_retval != GSDI_SUCCESS)
  {
    PBM_MSG_ERR("gsdi_sim_get_ecc returned %d", gsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}
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
pbm_return_type pbm_gsdi_usim_get_ecc(
    pbm_slot_type        sim_id,
    gsdi_ef_ecc_data_T * usim_ecc )
{
  gsdi_returns_T gsdi_retval;
#if (MAX_UIMS > 1)
  gsdi_slot_id_type slot;
  slot = (sim_id == PBM_SLOT_1)?GSDI_SLOT_1:GSDI_SLOT_2;
  gsdi_retval = gsdi2_usim_get_ecc( slot, usim_ecc );
#else
  //lint --e{715} sim_id isn't used in this case
  gsdi_retval = gsdi_usim_get_ecc( usim_ecc );
#endif
  if (gsdi_retval != GSDI_SUCCESS)
  {
    PBM_MSG_ERR("gsdi_usim_get_ecc returned %d", gsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}
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
pbm_return_type pbm_gsdi_ruim_get_ecc(
    pbm_slot_type        sim_id,
    gsdi_ef_ecc_data_T * ruim_ecc )
{
  gsdi_returns_T gsdi_retval;
#if (MAX_UIMS > 1)
  gsdi_slot_id_type slot;
  slot = (sim_id == PBM_SLOT_1)?GSDI_SLOT_1:GSDI_SLOT_2;
  gsdi_retval = gsdi2_ruim_get_ecc( slot, ruim_ecc );
#else
  //lint --e{715} sim_id isn't used in this case
  gsdi_retval = gsdi_ruim_get_ecc( ruim_ecc );
#endif
  if (gsdi_retval != GSDI_SUCCESS)
  {
    PBM_MSG_ERR("gsdi_ruim_get_ecc returned %d", gsdi_retval, 0, 0);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}
#endif /* FEATURE_MMGSDI_CDMA */

/*===========================================================================
FUNCTION PBM_GSDI_PHONEBOOK_PRESENT

DESCRIPTION
  Used to determine if a global or local phonebook is present on the given
  SIM.  This function is used when we have the new GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsdi_phonebook_present(pbm_slot_type sim_id, gsdi_phonebook_type phonebook)
{
  gsdi_slot_id_type slot;
  slot = (sim_id == PBM_SLOT_1)?GSDI_SLOT_1:GSDI_SLOT_2;
  return gsdi2_phonebook_present(slot, phonebook);
}

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
                       const mmgsdi_cnf_type  *data_ptr)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
  uint32 client_ref;
  uint16 read_cnf_data_len = 0;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  if (!mmgsdi_resp_ptr) {
    PBM_MSG_ERR("Unable to allocate PBM command", 0, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) || (!pbm_buffers[client_ref].in_use))
  {
    PBM_MSG_ERR("Invalid client_ref found: %d\n", client_ref, 0, 0);
    pbm_cmd_free(mmgsdi_resp_ptr);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  mmgsdi_resp_ptr->cmd_type = pbm_buffers[client_ref].gsdi_resp_type;

  mmgsdi_resp_ptr->status = status;
  mmgsdi_resp_ptr->data_type = data_type;
  memcpy(&mmgsdi_resp_ptr->data, data_ptr, sizeof(mmgsdi_cnf_type));


if(PBM_MMGSDI_READ_CB == mmgsdi_resp_ptr->cmd_type)
{
  if(data_ptr->read_cnf.read_data.data_len > 0)
  {
    //here typecasting is required ti uint16 since pbm_mem_alloc accepts only uint16
    read_cnf_data_len  = (uint16)data_ptr->read_cnf.read_data.data_len;
    mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr =
                                 (byte *)pbm_mem_alloc(read_cnf_data_len);
    memcpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
           (void *)data_ptr->read_cnf.read_data.data_ptr,
           read_cnf_data_len); //deep copy
  }
  else
  {
    mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
  }
}
else if(PBM_MMGSDI_WRITE_CB != mmgsdi_resp_ptr->cmd_type)
{
  //print error
  PBM_MSG_ERR("Unexpected cmd_type %d", mmgsdi_resp_ptr->cmd_type, 0, 0);
}

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

} /*pbm_gsdi_async_cb...*/

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
                            const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  if (data_type != MMGSDI_WRITE_CNF)
  {
    PBM_MSG_ERR("Invalid response type to a GSDI write. %d",
        data_type, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) || (!pbm_buffers[client_ref].in_use))
  {
    PBM_MSG_ERR("Invalid client_ref found: %d\n", client_ref, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  switch (pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
  {
    case PBM_CACHE_TYPE_PB:
      pbm_sim_write_cb(status, data_ptr);
      break;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_CACHE_TYPE_USIM:
      pbm_usim_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_SYNC1:
    case PBM_CACHE_TYPE_SYNC2:
      pbm_sync_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_INDEX:
      pbm_index_write_cb(status, data_ptr);
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    case PBM_CACHE_TYPE_EXT:
      pbm_ext_write_cb(status, data_ptr);
      break;

    default:
      PBM_MSG_ERR("Unknown file id %d", pbm_buffers[client_ref].record.file_id,
                  0, 0);
      break;
  }
}

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
                       const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  pbm_slot_type sim_id;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;

  if (data_type != MMGSDI_READ_CNF)
  {
    PBM_MSG_ERR("Invalid response type to a GSDI read. %d",
        data_type, 0, 0);
    return;
  }

  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
              (pbm_buffers[client_ref].in_use == FALSE)) {
    PBM_MSG_ERR("Invalid client_ref found: %d\n", client_ref, 0, 0);
    return;
  }

  sim_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(data_ptr->response_header.slot_id);
  if (pbm_buffers[client_ref].record.sim_id != sim_id)
  {
    PBM_MSG_ERR("Wrong callback called for file %d and sim %d, %d",
                pbm_buffers[client_ref].record.file_id,
                pbm_buffers[client_ref].record.sim_id, sim_id);
    return;
  }

  /* Restart initialization if required */
  if (pbm_slot_restart & PBM_SIMID_TO_SLOT_MASK(sim_id))
  {
    PBM_MSG_HIGH("PBM restarting init of SIM %d", sim_id, 0, 0);
    pbm_restart_fileinfo_internal(sim_id);
    pbm_free_record((int) client_ref);
    return;
  }

  /* Stop initialization if required */
  if (!(pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(sim_id))) {
    PBM_MSG_HIGH("PBM stopping init of SIM %d", sim_id, 0, 0);
    pbm_free_record((int) client_ref);
    return;
  }

  /* GSDI data looks fine */
  switch(pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
  {
    case PBM_CACHE_TYPE_PB:
      pbm_gsdi_sim_read_cb(status, data_ptr);
      break;

#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_CACHE_TYPE_ECC:
      pbm_gsdi_ecc_read_cb(status, data_ptr);
      break;
#endif

    case PBM_CACHE_TYPE_EXT:
      pbm_gsdi_ext_read_cb(status, data_ptr);
      break;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_CACHE_TYPE_USIM:
      pbm_gsdi_usim_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_SYNC1:
    case PBM_CACHE_TYPE_SYNC2:
      pbm_gsdi_sync_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_INDEX:
      pbm_gsdi_index_read_cb(status, data_ptr);
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

    default:
      PBM_MSG_ERR("Unknown file id %d", pbm_buffers[client_ref].record.file_id, 0, 0);
      pbm_free_record((int) client_ref);
      break;
  }

  return;

} /* pbm_gsdi_read_cb...*/

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
                           const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  byte  *number;
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_field_id_e_type field_id;
#endif /* FEATURE_PBM_USIM_SUPPORT */

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;

  pbm_buffers[client_ref].record.file_id = pbm_buffers[client_ref].m_file_id;

  if (status == MMGSDI_SUCCESS)
  {
#ifdef FEATURE_PBM_USIM_SUPPORT
    field_id = pbm_file_id_to_field_id(pbm_buffers[client_ref].m_file_id);
    if (field_id == PBM_FIELD_ADDITIONAL_NUMBER)
      number = (byte *)pbm_buffers[client_ref].record.data.anr.number;
    else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      number = (byte *)pbm_buffers[client_ref].record.data.num_text.number;

    if (pbm_gsdi_process_ext_data(data_ptr->read_cnf.read_data.data_ptr, number))
    {
      if (pbm_buffers[client_ref].proc_func != NULL)
      {
        (*pbm_buffers[client_ref].proc_func) (PBM_SUCCESS,
                                              &pbm_buffers[client_ref].record);
      }
      pbm_free_record((int) client_ref);
      return;
    }
  }

  /* Read failed */
  if (pbm_buffers[client_ref].proc_func != NULL) {
    (*pbm_buffers[client_ref].proc_func) ( PBM_ERROR,
                                           &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);
  return;
} /*pbm_gsdi_ext_read_cb...*/

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_INDEX_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_index_read_cb( mmgsdi_return_enum_type status,
                             const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;

  if (status == MMGSDI_SUCCESS)
  {
    if (pbm_gsdi_process_index_data(pbm_buffers[client_ref].record.sim_id,
                                    pbm_buffers[client_ref].record.file_id,
                                    data_ptr->read_cnf.accessed_rec_num,
                                    data_ptr->read_cnf.read_data.data_ptr,
                                    &pbm_buffers[client_ref].record.data.iap) ==
                                                                PBM_SUCCESS)
    {
      if (pbm_buffers[client_ref].proc_func != NULL)
      {
        (*pbm_buffers[client_ref].proc_func) (PBM_SUCCESS,
                                              &pbm_buffers[client_ref].record);
      }
      pbm_free_record((int) client_ref);
      return;
    }
  }

  /* Read failed */
  if (pbm_buffers[client_ref].proc_func != NULL) {
    (*pbm_buffers[client_ref].proc_func) ( PBM_ERROR,
                                           &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /*pbm_gsdi_index_read_cb...*/

/*===========================================================================
FUNCTION PBM_GSDI_SYNC_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_sync_read_cb( mmgsdi_return_enum_type status,
                            const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  pbm_return_type ret = PBM_SUCCESS;
  byte *data;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;

  if (status == MMGSDI_SUCCESS)
  {
    data = data_ptr->read_cnf.read_data.data_ptr;
    switch (pbm_buffers[client_ref].record.file_id)
    {
      case PBM_FILE_PSC:
        pbm_buffers[client_ref].record.data.dword = data[0] << 24 |
                                                    data[1] << 16 |
                                                    data[2] << 8  |
                                                    data[3];
        break;

      case PBM_FILE_PUID:
      case PBM_FILE_CC:
      case PBM_FILE_PBC:
      case PBM_FILE_PBC1:
      case PBM_FILE_UID:
      case PBM_FILE_UID1:
        pbm_buffers[client_ref].record.data.word = data[0] << 8 |
                                                   data[1];
        break;

      default:
        PBM_MSG_ERR("Unexpected file_id %d",
            pbm_buffers[client_ref].record.file_id, 0, 0);
        ret = PBM_ERROR;
        break;
    }
  }

  if (pbm_buffers[client_ref].proc_func != NULL) {
    (*pbm_buffers[client_ref].proc_func) ( ret,
                                           &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /*pbm_gsdi_sync_read_cb...*/
#endif /* FEATURE_PBM_USIM_SUPPORT */

#ifdef FEATURE_ECC_HANDLING_AT_PBM
/*===========================================================================
FUNCTION PBM_GSDI_ECC_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the SIM ECC, PBM_READ_CB is
  called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_ecc_read_cb( mmgsdi_return_enum_type status,
                           const mmgsdi_cnf_type  *data_ptr)
{

  uint32 client_ref;
  PB_CACHE *cache = NULL;
  pbm_file_device_e_type file_id;
  pbm_slot_type sim_id;
  int  data_length;                 /* Length of ASCII version of ECC # */
  int  name_length = 0;
  uint16 ascii_emergency_alpha[100];    /* Emergency number Alpha Identifier. */
  byte emergency_category;            /* Emergency service category octet. */
  int ecc_offset = 0;          /* Where in the ecc_list are we */
  int record_num;
  int i;
  pbm_return_type pbm_ret = PBM_SUCCESS;



  PBM_CHECK_PTR_RET(data_ptr, VOID);

#if (MAX_UIMS > 1)
  sim_id = (sim_data->message_header.slot == GSDI_SLOT_1 ? PBM_SLOT_1 : PBM_SLOT_2);
#else
  sim_id = PBM_SLOT_1;
#endif

  client_ref = data_ptr->response_header.client_data;
  file_id = pbm_buffers[client_ref].record.file_id;

  cache = (PB_CACHE *) pbm_file_id_to_cache(sim_id, file_id);  //lint !e826 area too small

  if (!cache)
  {
    pbm_free_record((int) client_ref);
    PBM_MSG_ERR("No Cache found for sim_id %d, file_id %d\n",
                sim_id, file_id, 0);
    return;
  }

  /* If the read was successful, process the ecc data */
  if (status == MMGSDI_SUCCESS)
  {
    // we would need different processing for an USIM and the rest
    //since in USIM EC-ECC will be linear fixed and in normal SIM it will be a Transparent file
    if(USE_USIM(sim_id))
    {
      data_length = pbm_ecc_bcd_to_ascii(data_ptr->read_cnf.read_data.data_ptr,
                                             pbm_buffers[client_ref].record.data.num_text.number);

      if (data_length <= 0)
      {
        PBM_MSG_HIGH("SIM ECC record %d had 0 length", ecc_offset, 0, 0);
      }

      memset(ascii_emergency_alpha, 0, sizeof(ascii_emergency_alpha));
      //since the last byte denotes Service Category we should not consider that
      name_length = pbmconvert_sim_to_string(
                                        data_ptr->read_cnf.read_data.data_ptr + 3,
                                        (cache->record_length - 4),
                                        ascii_emergency_alpha,
                                        ARR_SIZE(ascii_emergency_alpha));

      // The last octet is the Service Category
      emergency_category = data_ptr->read_cnf.read_data.data_ptr[cache->record_length - 1];

      if (pbm_build_ecc_record(
                (sim_id==PBM_SLOT_1?PBM_FIELD_SIM1_ECC:PBM_FIELD_SIM2_ECC),
                 data_length>0?(uint32)data_length + 1:0,
                 pbm_buffers[client_ref].record.data.num_text.number,
                 sizeof(uint16) * (name_length?name_length + 1:0),
                 ascii_emergency_alpha,
                 TRUE,
                 emergency_category) != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Could not build PBM record.", 0, 0, 0);
      }
      else
      {
        PBM_SIMSLOT_SET(ecc_has_entries, sim_id);
      }
      record_num = pbm_get_next_used_record(sim_id, pbm_buffers[client_ref].record.index);
      if (record_num <= cache->num_of_records)
      {
        if((pbm_ret = pbm_sim_read(sim_id,PBM_FILE_ECC,record_num,NULL)) == PBM_SUCCESS)
        {
          pbm_free_record((int) client_ref);
          return;
        }
      }
      else
      {
        PBM_SET_PB_CACHE_READY(cache);
      }
    }
    else
    {
      //we need the below special processing since GSM and RUIM EF-ECCs are transparent files,
      //and all the ecc codes are appended together in one record
      //there exists only ecc number in this case
      for(i=0; i<cache->num_of_records; i++)
      {
        if(i < (data_ptr->read_cnf.read_data.data_len/3))
        {
        data_length = pbm_ecc_bcd_to_ascii((data_ptr->read_cnf.read_data.data_ptr + 3*i),
                                           pbm_buffers[client_ref].record.data.num_text.number);
        if (data_length <= 0)
        {
          PBM_MSG_HIGH("SIM ECC record %d had 0 length", ecc_offset, 0, 0);
        }
        if ((pbm_ret = pbm_build_ecc_record(
                    (sim_id==PBM_SLOT_1?PBM_FIELD_SIM1_ECC:PBM_FIELD_SIM2_ECC),
                     data_length>0?(uint32)data_length + 1:0,
                     pbm_buffers[client_ref].record.data.num_text.number,0,
                     NULL, FALSE, 0)) !=PBM_SUCCESS)
        {
          PBM_MSG_ERR("Could not build PBM record.", 0, 0, 0);
        }
        else
        {
          PBM_SIMSLOT_SET(ecc_has_entries, sim_id);
        }
        }//if(i < (data_ptr->read_cnf.read_data.data_len/3))
      }
      PBM_SET_PB_CACHE_READY(cache);
    }
  }
  else
  {
    switch (status)
    {
      case MMGSDI_NOT_FOUND:
        PBM_MSG_HIGH("GSDI error MMGSDI_NOT_FOUND sim 0x%x pbm file %d sim read",
                 sim_id, file_id, 0);
        break;

      case MMGSDI_ACCESS_DENIED:
        PBM_MSG_HIGH("GSDI error MMGSDI_ACCESS_DENIED sim 0x%x pbm file %d sim read",
                 sim_id, file_id, 0);
        break;

      default:
        PBM_MSG_HIGH("GSDI error code 0x%x sim 0x%x pbm file %d sim read",
                 status, sim_id, file_id);
        break;
    }
  }
  if(pbm_ret == PBM_SUCCESS)
    PBM_SIMSLOT_SET(ecc_initialized, sim_id);
  pbm_free_record((int) client_ref);

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;
  pbm_init_entries(sim_id);

} /*pbm_gsdi_ecc_read_cb(gsdi_cnf_T *sim_da...*/
#endif

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
                           const mmgsdi_cnf_type  *data_ptr)
{
  int extension_rec_num;
  mmgsdi_file_enum_type filename = MMGSDI_NO_FILE_ENUM;
  uint32 client_ref;
  PB_CACHE *cache = NULL;
  pbm_file_device_e_type file_id;
  pbm_slot_type sim_id;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

#if (MAX_UIMS > 1)
  sim_id = (sim_data->message_header.slot == GSDI_SLOT_1 ? PBM_SLOT_1 : PBM_SLOT_2);
#else
  sim_id = PBM_SLOT_1;
#endif

  client_ref = data_ptr->response_header.client_data;
  file_id = pbm_buffers[client_ref].record.file_id;

  cache = (PB_CACHE *) pbm_file_id_to_cache(sim_id, file_id);  //lint !e826 area too small

  if (!cache)
  {
    pbm_free_record((int) client_ref);
    PBM_MSG_ERR("No Cache found for sim_id %d, file_id %d\n",
                sim_id, file_id, 0);
    return;
  }

  /* If the read was successful, process the extension if needed */
  if (status == MMGSDI_SUCCESS)
  {
    if ( pbm_gsdi_process_sim_data(sim_id,
                         file_id,
                         pbm_buffers[client_ref].record.index,
                         data_ptr->read_cnf.read_data.data_ptr,
                         &pbm_buffers[client_ref].record.data.num_text,
                         &extension_rec_num))
    {
      /* Processing was complete. Life is good. */
      if (pbm_buffers[client_ref].proc_func != NULL)
      {
        (*pbm_buffers[client_ref].proc_func)( PBM_SUCCESS,
                                              &pbm_buffers[client_ref].record);
      }
      pbm_free_record((int) client_ref);
      return;
    }
    else
    {
      if (extension_rec_num && cache->extension_cache &&
          ((filename = cache->extension_cache->filename) != MMGSDI_NO_FILE_ENUM))
      {
        pbm_buffers[client_ref].m_file_id = file_id;
        pbm_buffers[client_ref].record.file_id =
                                          cache->extension_cache->file_id;
        pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
        if (pbm_send_read_request(pbm_buffers[client_ref].record.sim_id,
                                  file_id,
                                  filename,
                                  extension_rec_num, (int) client_ref,
                                  pbm_gsdi_async_cb))
        {
          return;
        }
        PBM_MSG_ERR("Could not issue read request", 0, 0, 0);
      }
      else
      {
        PBM_MSG_ERR("PBM could not find a valid extension record for %d from cache 0x%x",
            file_id, cache, 0);
      }
    }
  }
  else
  {
    switch (status)
    {
      case MMGSDI_NOT_FOUND:
        PBM_MSG_HIGH("GSDI error MMGSDI_NOT_FOUND sim 0x%x pbm file %d sim read",
                 sim_id, file_id, 0);
        break;

      case MMGSDI_ACCESS_DENIED:
        PBM_MSG_HIGH("GSDI error MMGSDI_ACCESS_DENIED sim 0x%x pbm file %d sim read",
                 sim_id, file_id, 0);
        break;

      default:
        PBM_MSG_HIGH("GSDI error code 0x%x sim 0x%x pbm file %d sim read",
                 status, sim_id, file_id);
        break;
    }
  }

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func) ( PBM_ERROR,
                                           &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);

} /*pbm_gsdi_sim_read_cb(gsdi_cnf_T *sim_da...*/

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
                            const mmgsdi_cnf_type  *data_ptr)
{
  pbm_return_type ret = PBM_SUCCESS;
  int extension_rec_num = 0;
  pbm_ext_cache_s_type *ext_cache;
  uint32 client_ref;
  PB_USIM_CACHE *usim_cache = NULL;
  pbm_file_device_e_type file_id;
  pbm_slot_type sim_id;

  PBM_CHECK_PTR_RET(data_ptr, VOID);


#if (MAX_UIMS > 1)
  sim_id = (sim_data->message_header.slot == GSDI_SLOT_1 ? PBM_SLOT_1 : PBM_SLOT_2);
#else
  sim_id = PBM_SLOT_1;
#endif
  client_ref = data_ptr->response_header.client_data;
  file_id = pbm_buffers[client_ref].record.file_id;

  usim_cache = (PB_USIM_CACHE *) pbm_file_id_to_cache(sim_id, file_id);
  if (!usim_cache)
  {
    pbm_free_record((int) client_ref);
    PBM_MSG_ERR("No cache found for sim_id %d, file_id %d\n",
                sim_id, file_id, 0);
    return;
  }

  do
  {
    if (status != MMGSDI_SUCCESS)
    {
      PBM_MSG_ERR("PBM Error %d from GSDI for file %d",
                  status, file_id, 0);
      ret = PBM_ERROR;
      break;
    }

    switch(file_id)
    {
      case PBM_FILE_EMAIL:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_SNE:
      case PBM_FILE_SNE1:
#ifdef FEATURE_USIM_1000_PBM
      case PBM_FILE_EMAIL2:
      case PBM_FILE_EMAIL3:
      case PBM_FILE_SNE2:
      case PBM_FILE_SNE3:
#endif
        ret = pbm_gsdi_process_text_data(sim_id,
                                 file_id,
                                 data_ptr->read_cnf.accessed_rec_num,
                                 data_ptr->read_cnf.read_data.data_ptr,
                                 &pbm_buffers[client_ref].record.data.text);
        break;

      case PBM_FILE_GRP:
      case PBM_FILE_GRP1:
#ifdef FEATURE_USIM_1000_PBM
      case PBM_FILE_GRP2:
      case PBM_FILE_GRP3:
#endif
        ret = pbm_gsdi_process_grp_data(sim_id,
                                 file_id,
                                 data_ptr->read_cnf.accessed_rec_num,
                                 data_ptr->read_cnf.read_data.data_ptr,
                                 &pbm_buffers[client_ref].record.data.grp);
        break;

      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
#ifdef FEATURE_USIM_1000_PBM
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
#endif
        ret = pbm_gsdi_process_anr_data(sim_id,
                                 file_id,
                                 data_ptr->read_cnf.accessed_rec_num,
                                 data_ptr->read_cnf.read_data.data_ptr,
                                 &pbm_buffers[client_ref].record.data.anr,
                                 &extension_rec_num);
        break;

      default:
        PBM_MSG_ERR("Unexpected file id %d", file_id, 0, 0);
        ret = PBM_ERROR;
        break;
    }

    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to process data from file %d", file_id, 0, 0);
      break;
    }

    /* If there are no extension records, we are done here */
    if (!extension_rec_num)
      break;

    if (pbm_file_id_to_field_id(file_id) != PBM_FIELD_ADDITIONAL_NUMBER)
    {
      PBM_MSG_ERR("File id %d is not supposed to have extension record %d",
                   file_id, extension_rec_num, 0);
      ret = PBM_ERROR;
      break;
    }

    ext_cache = &ext1_cache[sim_id];
    if (ext_cache->filename == MMGSDI_NO_FILE_ENUM)
    {
      PBM_MSG_ERR("PBM could not find a valid extension record for %d",
                  file_id, 0, 0);
      ret = PBM_ERROR;
      break;
    }

    pbm_buffers[client_ref].m_file_id = file_id;
    pbm_buffers[client_ref].record.file_id = ext_cache->file_id;
    pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
    if (!pbm_send_read_request(pbm_buffers[client_ref].record.sim_id,
                               ext_cache->file_id,
                               ext_cache->filename,
                               extension_rec_num, (int) client_ref,
                               pbm_gsdi_async_cb))
    {
      PBM_MSG_ERR("Could not issue read request for %d index %d",
                   ext_cache->filename, extension_rec_num, 0);
      ret = PBM_ERROR;
      break;
    }

    return; /* We have sent an extension request, buffer is not freed yet */

  } while (FALSE); /*lint !e717 */

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func)( ret,
                                          &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);

  return;

} /* pbm_gsdi_usim_read_cb(gsdi_cnf_T *sim_da...*/
#endif /* FEATURE_PBM_USIM_SUPPORT */

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
pbm_return_type pbm_create_sim_record(pbm_slot_type       sim_id,
                              pbm_file_device_e_type      file_id,
                              pbm_num_text_record_s_type *record,
                              int                         buffer_index )
{
  pbm_return_type           pbm_retval;
  mmgsdi_file_enum_type     filename;
  mmgsdi_file_structure_enum_type filetype;
  byte                      local_pbm_data_buffer[ GSDI_MAX_DATA_BLOCK_LEN ];
  uint32                    num_len;
  byte                      buffer[PBM_MAX_NUM_LENGTH];
  pbm_fixed_sim_rec_s_type *fixed_sim_record;
#ifdef FEATURE_PBM_USIM_SUPPORT
  clk_julian_type           julian_time;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PB_CACHE                 *cache;
  int                       index;
  uint16                    chars_converted;

  PBM_CHECK_PTR_RET(record,PBM_ERROR);

  cache = pbm_file_id_to_cache(sim_id, file_id);
  if (cache == NULL)
  {
    PBM_MSG_ERR("No cache sim_id = %d, file_id = %d", sim_id, file_id, 0);
    return PBM_ERROR;
  }

  filename = pbm_pb_record_index_to_filename(cache, record->index);
  index = pbm_pb_record_index_to_fileloc(cache, record->index);
  filetype = cache->file_type;

  if (filename == MMGSDI_NO_FILE_ENUM || index < 0)
  {
    PBM_MSG_ERR("Invalid cache/index %d, %d, %d", file_id, record->index,
                 cache->num_files);
    return PBM_ERROR_INDEX_INVALID;
  }

  /* Initialize buffer to unused. */
  memset(buffer, 0, sizeof(buffer));

  /* Set local buffer to FF's. Therefore, all unwritten fields will be 0xFF. */
  memset ( (byte *)local_pbm_data_buffer, 0xFF,
           sizeof (local_pbm_data_buffer));

  (void) pbmconvert_string_to_sim(record->text,
                                  (uint16) pbmutils_wstrlen(record->text),
                                  PBM_FT_UCS2, local_pbm_data_buffer,
                                  (uint16) cache->text_length,
                                  &chars_converted,
                                  FALSE);
  if (chars_converted != pbmutils_wstrlen(record->text))
  {
    PBM_MSG_ERR("Did not convert all characters expected %d converted %d",
                pbmutils_wstrlen(record->text), chars_converted, 0);
    return PBM_ERROR_TEXT_TOO_LONG;
  }

  /*
   * If record->number is not empty, convert it to bcd.
   */
  if ((num_len = strlen (record->number)) != 0)
  {
    if (!pbm_ascii_to_bcd((byte *)record->number, record->type, buffer))
      return PBM_ERROR_NUMBER_TOO_LONG;
  }

  //lint -e{826} struct has oci/ici, even though we may not access it
  fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                   cache->text_length);

  /* The phone number length is stored in buffer[0].  Check length
   * and copy the local buffer. */
  if (buffer[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
  {
    memcpy((byte *)&fixed_sim_record->ton_npi, &buffer[1], 1);
    memcpy((byte *)fixed_sim_record->number, &buffer[2],
        PBM_SIM_DIALING_NUMBER_LEN);
    fixed_sim_record->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
  }
  else
  {
    if (num_len != 0)
      memcpy((byte *)fixed_sim_record, buffer, buffer[0]+1);
  }
  fixed_sim_record->ext_id = pbm_buffers[buffer_index].extension;
#ifdef FEATURE_PBM_USIM_SUPPORT
  if ((cache->record_length - cache->text_length) >= OCI_KNOWN_REC_LEN)
  {
    clk_secs_to_julian(record->time, &julian_time);
    fixed_sim_record->date_time[0] = pbm_bin_to_bcd( (byte) (julian_time.year % 100) );
    fixed_sim_record->date_time[1] = pbm_bin_to_bcd( (byte) (julian_time.month) );
    fixed_sim_record->date_time[2] = pbm_bin_to_bcd( (byte) (julian_time.day) );
    fixed_sim_record->date_time[3] = pbm_bin_to_bcd( (byte) (julian_time.hour) );
    fixed_sim_record->date_time[4] = pbm_bin_to_bcd( (byte) (julian_time.minute) );
    fixed_sim_record->date_time[5] = pbm_bin_to_bcd( (byte) (julian_time.second) );
    fixed_sim_record->date_time[6] = 0xFF;  /* time zone not supported */

    fixed_sim_record->duration[0] = (byte) ((record->duration & 0xFF0000) >> 16);
    fixed_sim_record->duration[1] = (byte) ((record->duration & 0x00FF00) >> 8);
    fixed_sim_record->duration[2] = (byte) ( record->duration & 0x0000FF);

    if ((cache->record_length - cache->text_length) >= ICI_KNOWN_REC_LEN)
    {
      /* ICI status LSB = 0 if call answered, 1 if call not answered */
      fixed_sim_record->ici_oci_ending.ici_end.ici_status =
        ( record->is_answered ? 0x00 : 0x01 );
    }
  }
#endif /* FEATURE_PBM_USIM_SUPPORT */
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  index,
                                  local_pbm_data_buffer,
                                  cache->record_length,
                                  (uint32) buffer_index,
                                  pbm_gsdi_async_cb);

  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    return PBM_ERROR;
  }

  return PBM_SUCCESS;
}

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
                          mmgsdi_file_enum_type m_filename)
{
  byte *gsdi_buffer;
  pbm_slot_type sim_id = pbm_buffers[buffer_index].record.sim_id;
  int extension_record;
  uint32 extension_len;
  pbm_return_type pbm_retval;

  PBM_CHECK_PTR_RET(ext_cache, PBM_ERROR_MEM_FULL);
  PBM_CHECK_PTR_RET(num_buffer, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

  /* buffer contains the bcd in the format    */
  /* byte  0      1      2 thru buffer[0]     */
  /*     <len><ton/npi><bcd packed number>    */
  /* Note that length is the length including */
  /* the TON/NPI field.                       */

  extension_record = pbm_find_extension_record(ext_cache, m_index,
                                               m_filename);
  if (!extension_record) {
    PBM_MSG_ERR("pbm_find_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
    return PBM_ERROR_MEM_FULL;
  }


  pbm_buffers[buffer_index].extension = (byte) extension_record;
  extension_len = MIN((num_buffer[0] - 1) - PBM_SIM_DIALING_NUMBER_LEN,
                      PBM_SIM_DIALING_NUMBER_LEN);

  gsdi_buffer[0] = (byte) EXT_REC_TYPE_ADDITIONAL_DATA;
  gsdi_buffer[1] = (byte) extension_len;

  memcpy(&gsdi_buffer[2], &num_buffer[PBM_SIM_DIALING_NUMBER_LEN + 2],
         extension_len);

  pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write ( sim_id,
                                    ext_cache->filename,
                                    ext_cache->file_type,
                                    extension_record,
                                    gsdi_buffer,
                                    EXT_KNOWN_REC_LEN,
                                    (uint32) buffer_index,
                                    pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Error from pbm_gsdi_uim_write %d", pbm_retval, 0, 0);
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}

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
                                     mmgsdi_file_enum_type m_filename)
{
  byte *gsdi_buffer;
  pbm_slot_type sim_id = pbm_buffers[buffer_index].record.sim_id;
  int extension_record;
  pbm_return_type pbm_retval;

  PBM_CHECK_PTR_RET(ext_cache, PBM_ERROR_MEM_FULL);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

  /* buffer contains the bcd in the format    */
  /* byte  0      1      2 thru buffer[0]     */
  /*     <len><ton/npi><bcd packed number>    */
  /* Note that length is the length including */
  /* the TON/NPI field.                       */

  extension_record = pbm_free_extension_record(ext_cache,
                                               m_index,
                                               m_filename);

  if (!extension_record) {
    PBM_MSG_ERR("pbm_free_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
    return PBM_ERROR;
  }


  pbm_buffers[buffer_index].extension = (byte) 0xff;

  gsdi_buffer[0] = (byte) EXT_REC_TYPE_FREE;

  pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write ( sim_id,
                                    ext_cache->filename,
                                    ext_cache->file_type,
                                    extension_record,
                                    gsdi_buffer,
                                    EXT_KNOWN_REC_LEN,
                                    (uint32) buffer_index,
                                    pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Error from pbm_gsdi_uim_write %d", pbm_retval, 0, 0);
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_CREATE_ANR_RECORD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_anr_record(pbm_slot_type          sim_id,
                                      pbm_file_device_e_type file_id,
                                      pbm_anr_record_s_type *anr_record,
                                      uint32                 buffer_index )
{
  byte                  *gsdi_buffer;
  PB_USIM_CACHE         *usim_cache;
  pbm_async_gsdi_buffer *buffer = &pbm_buffers[buffer_index];
  uint16                 index;
  mmgsdi_file_enum_type  filename;
  mmgsdi_file_structure_enum_type filetype;
  pbm_return_type        pbm_retval;
  byte                   num_buffer[PBM_MAX_NUM_LENGTH];
  pbm_fixed_sim_rec_s_type *sim_rec;

  PBM_CHECK_PTR_RET(anr_record, PBM_ERROR);

  usim_cache = pbm_file_id_to_cache(sim_id, file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32) usim_cache->record_length);

  filename = usim_cache->filename;
  filetype = usim_cache->file_type;
  index = (uint16) buffer->record.index;

  gsdi_buffer[0] = 0xFF;
  //lint -e{826} struct has oci/ici, even though we may not access it
  sim_rec = (pbm_fixed_sim_rec_s_type *) &gsdi_buffer[1];

  //convert the string to ascii only if the len is more than zero
  //(fix for cr 165174) this is to fix the issue After deleting a contact,
  //2nd byte of EF-ANR is 00 instead of FF
  if ((int) strlen((char *)anr_record->number) != 0)
  {
    if (!pbm_ascii_to_bcd((byte *)anr_record->number, anr_record->type,num_buffer))
    {
      return PBM_ERROR;
    }

    if (num_buffer[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      memcpy((byte *)&sim_rec->ton_npi, (byte *)&num_buffer[1],1);
      memcpy((byte *)sim_rec->number, (byte *)&num_buffer[2],
      PBM_SIM_DIALING_NUMBER_LEN);
      sim_rec->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
    }
    else
    {
      memcpy((byte *)sim_rec, num_buffer, num_buffer[0]+1);
    }
  }

  sim_rec->ext_id = pbm_buffers[buffer_index].extension;

  if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2 &&
      pbm_buffers[buffer_index].state == PBM_WRITE_CURR_USIM)
  {
    if (pbm_fill_adn_sfi(anr_record->adn_record_id,
          &gsdi_buffer[usim_cache->record_length-2]) != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to fill ADN SFI", 0, 0, 0);
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
    }
  }

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  index,
                                  gsdi_buffer,
                                  usim_cache->record_length,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Error from pbm_gsdi_uim_write %d", pbm_retval, 0, 0);
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_MMGSDI_REFRESH_COMPLETE_CB

DESCRIPTION
  This function is called in response to gdi_refresh_complete by GSDI

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_mmgsdi_refresh_complete_cb(mmgsdi_return_enum_type status,
                                           mmgsdi_cnf_enum_type    cnf,
                                           const mmgsdi_cnf_type  *cnf_ptr)
{
  /* Do Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    PBM_MSG_ERR("Refresh Complete Ack failed %d %d",
                 status, cnf, 0);
  }
}

/* <EJECT> */
/*==============================================================================

FUNCTION NAME pbm_refresh_file_change_notification_cb

DESCRIPTION
  Function that gets called by GSDI when a REFRESH Request is made and a
  determination must be made whether to proceed or not.

  Currently we refresh all SIM phonebooks irrespective of which files were
  changed. This should actually be based on the file list that GSDI sends us.

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void pbm_refresh_file_change_notification_cb
(
  mmgsdi_refresh_file_list_type  *refresh_file_list_p, /*list of changed files*/
  mmgsdi_slot_id_enum_type       slot
)
{
  boolean                globals_updated = FALSE;
  pbm_slot_type          sim_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(slot);
  pbm_device_type        pb_id;
  int                    i, j;
  uint8                  local;     /* Local vs Global phonebook */
  mmgsdi_file_enum_type  uim_file;
  pbm_file_device_e_type pbm_file;

  PBM_CHECK_PTR_RET(refresh_file_list_p, VOID);

  /* Populate Static Globals for PBM */
  globals_updated = pbm_update_refresh_globals(refresh_file_list_p,
                                               slot);

  if ( globals_updated == TRUE)
  {
    ecc_initialized = 0;

    /* Send notification about refresh start for each phonebook that is
     * changed */
    /* For each SIM phonebook file */
    for (i = 0; i < (int)ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      pbm_file = pbm_file_to_uim_file[i].pbm_filename;

      /* Check if the phonebook was changed */
      for (j = 0; j < refresh_file_list_p->num_files; j++)
      {
        /*Check both the local and global versions*/
        for (local = 0; local < (uint8) ARR_SIZE(pbm_file_to_uim_file[i].uim_filename); local++ )
        {
          uim_file = pbm_file_to_uim_file[i].uim_filename[local];
          if (uim_file == refresh_file_list_p->file_list_ptr[j])
          {
            pb_id = pbm_file_id_to_pb_id(sim_id, pbm_file);
            if (pb_id != PBM_DEFAULT)
            {
              PBM_NOTIFY(PBM_EVENT_PB_REFRESH_START, pb_id, pb_id);
            }
          }
        }
      }
    }

    /* Clear out the PBM Caches affected */
    pbm_clear_refresh_caches(sim_id);

    /* Reinitialize the PBM Cache */
    pbm_init_fileinfo_internal(sim_id);
  }
} /* pbm_refresh_file_change_notification_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION pbm_mmgsdi_refresh_cb()

DESCRIPTION
  register for refresh mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_mmgsdi_refresh_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    cnf,
                                  const mmgsdi_cnf_type  *cnf_ptr)
{
  /* DO Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    PBM_MSG_ERR("Refresh Complete Ack failed %d %d",
                 cnf, status, 0);
  }
} /* pbm_mmgsdi_refresh_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION pbm_mmgsdi_init_complete_cb()

DESCRIPTION
  Init Complete mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_mmgsdi_init_complete_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr)
{
  /* DO Nothing */
  if ((cnf != MMGSDI_CLIENT_INIT_COMP_CNF) || (status != MMGSDI_SUCCESS))
  {
    PBM_MSG_ERR("Init Complete Ack failed %d %d",
                 cnf, status, 0);
  }
} /* pbm_mmgsdi_init_complete_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION pbm_mmgsdi_ok_to_refresh_cb()

DESCRIPTION
  Init Complete mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -e{818} suppress could be const */
static void pbm_mmgsdi_ok_to_refresh_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf,
                                        const mmgsdi_cnf_type  *cnf_ptr)
{
  /* DO Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    PBM_MSG_ERR("Ok to refresh Ack failed %d %d",
                 cnf, status, 0);
  }
} /* pbm_mmgsdi_ok_to_refresh_cb */

/*===========================================================================
FUNCTION PBM_GSDI_SIM_CAP_CB

DESCRIPTION
  Callback function to return SIM Capability.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_gsdi_sim_cap_cb(gsdi_cnf_T *sim_data)
{
  pbm_gsdi_resp_s_type *gsdi_resp_ptr;

  PBM_CHECK_PTR_RET(sim_data, VOID);

  gsdi_resp_ptr = (pbm_gsdi_resp_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_gsdi_resp_s_type));
  PBM_CHECK_PTR_RET(gsdi_resp_ptr, VOID);

  gsdi_resp_ptr->cmd_type = PBM_GSDI_SIM_CAP_CB;
  memcpy(&gsdi_resp_ptr->gsdi_data, sim_data, sizeof(gsdi_cnf_T));

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(gsdi_resp_ptr));
}

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
void pbm_sim_cap_cb(const gsdi_cnf_T *sim_data)
{
  pbm_slot_type sim_id = PBM_SLOT_1;
  sim_capabilities_T *sim_cap = &pbm_sim_cap;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  uint8 gsdi_card_apps;
  uint8 i = 0;
#endif

  PBM_CHECK_PTR_RET(sim_data, VOID);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_1)
    sim_id = 0;
  else if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_2)
    sim_id = 1;
  else {
    PBM_MSG_ERR("PBM received bad slot %d",
                 sim_data->pin_status_cnf.message_header.slot, 0, 0);
    return;
  }
  if (sim_id)
    sim_cap = &pbm_sim_cap_2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  if ((gsdi_returns_T)sim_data->get_sim_cap_cnf.message_header.gsdi_status ==
                                                                GSDI_SUCCESS)
  {
    if (sim_data->get_sim_cap_cnf.sim_capabilities.fdn_enabled)
      PBM_SIMSLOT_SET(pbm_slot_fdn_enabled, sim_id);
    else
      PBM_SIMSLOT_RESET(pbm_slot_fdn_enabled, sim_id);

    memcpy(sim_cap, &sim_data->get_sim_cap_cnf.sim_capabilities,
           sizeof(sim_capabilities_T));
  }

#ifdef FEATURE_ECC_HANDLING_AT_PBM
  pbm_gsdi_get_apps_available(sim_id, &gsdi_card_apps);
  pbm_set_card_apps(sim_id, gsdi_card_apps);
  //set the appropriate mmgsdi file for PBM_FILE_ECC
  //if it is in CDMA mode set it to MMGSDI_CDMA_ECC
  if(!USE_USIM(sim_id) && (pbm_get_card_apps(sim_id) & GSDI_RUIM_APP_MASK))
  {
    /* find this file */
    for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      if(pbm_file_to_uim_file[i].pbm_file_group[0] == PBM_FILE_ECC)
      {
        pbm_file_to_uim_file[i].uim_filename[0] = MMGSDI_CDMA_ECC;
      }
    }
  }
#endif
  /* Start SIM initialization */
  pbm_init_fileinfo_internal(sim_id);
}

/*===========================================================================
FUNCTION PBM_GSDI_PIN_STATUS_CB

DESCRIPTION
  Callback function to return SIM PIN status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_gsdi_pin_status_cb(gsdi_cnf_T *sim_data)
{
  pbm_gsdi_resp_s_type *gsdi_resp_ptr;

  PBM_CHECK_PTR_RET(sim_data, VOID);

  gsdi_resp_ptr = (pbm_gsdi_resp_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_gsdi_resp_s_type));
  PBM_CHECK_PTR_RET(gsdi_resp_ptr, VOID);

  gsdi_resp_ptr->cmd_type = PBM_GSDI_PIN_STATUS_CB;
  memcpy(&gsdi_resp_ptr->gsdi_data, sim_data, sizeof(gsdi_cnf_T));

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(gsdi_resp_ptr));
}

  /* <EJECT> */
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
void pbm_pin_status_cb(gsdi_cnf_T *sim_data)
{
  pbm_slot_type sim_id = PBM_SLOT_1;

  PBM_CHECK_PTR_RET(sim_data, VOID);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_1)
    sim_id = PBM_SLOT_1;
  else if (sim_data->pin_status_cnf.message_header.slot == GSDI_SLOT_2)
    sim_id = PBM_SLOT_2;
  else {
    PBM_MSG_ERR("PBM received bad slot %d",
                 sim_data->pin_status_cnf.message_header.slot, 0, 0);
    return;
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  switch (sim_data->pin_status_cnf.pin_id)
  {
    case GSDI_PIN1:
      break;
    default:
      ERR("Unsupported PIN %d", sim_data->pin_status_cnf.pin_id, 0, 0);
      return; /* Other pins are not supported yet. */
  }

  /* If the command succeeded, we can set things accordingly. */
  if ( (gsdi_returns_T)sim_data->message_header.gsdi_status == GSDI_SUCCESS )
  {
    switch(sim_data->pin_status_cnf.status) {
      case GSDI_PIN_ENABLED:
        if (pbm_slot_pin_waiting & PBM_SIMID_TO_SLOT_MASK(sim_id))
          pbm_clear_and_set_sim_caches(sim_id, PBM_STATUS_PIN_REQ);
        break;

      case GSDI_PIN_BLOCKED:
        if (pbm_slot_pin_waiting & PBM_SIMID_TO_SLOT_MASK(sim_id))
          pbm_clear_and_set_sim_caches(sim_id, PBM_STATUS_PUC_REQ);
        break;

      case GSDI_PIN_PERM_BLOCKED:
        if (pbm_slot_pin_waiting & PBM_SIMID_TO_SLOT_MASK(sim_id))
          pbm_clear_and_set_sim_caches(sim_id, PBM_STATUS_SIM_FAIL);
        break;

      case GSDI_PIN_DISABLED:
      case GSDI_PIN_NOT_INITIALISED:
        break;

      default:
        ERR("Unknown pin_status_cnf.status %d",
            sim_data->pin_status_cnf.status, 0, 0);
        break;
    }
  }
  else
  {
    ERR("FAILED GET PIN %d STATUS with reason %d",
        sim_data->pin_status_cnf.pin_id,
        sim_data->message_header.gsdi_status,
        0);
    return;
  }

}

/*===========================================================================
FUNCTION PBM_GSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_mmgsdi_card_status_cb(
    const mmgsdi_event_data_type   *mmgsdi_event
)
{
  pbm_mmgsdi_card_status_s_type *card_status;

  PBM_CHECK_PTR_RET(mmgsdi_event, VOID);

  card_status = (pbm_mmgsdi_card_status_s_type *)
                 pbm_cmd_alloc(sizeof(pbm_mmgsdi_card_status_s_type));
  PBM_CHECK_PTR_RET(card_status, VOID);

  card_status->cmd_type = PBM_MMGSDI_CARD_STATUS_CB;

  #ifndef T_MDM8200
  card_status->mmgsdi_event = *mmgsdi_event;
  #else
  memcpy(&card_status->mmgsdi_event, mmgsdi_event, sizeof(card_status->mmgsdi_event));
  #endif

  if (mmgsdi_event->evt == MMGSDI_REFRESH_EVT)
  {
    //Init this in case number of files is zero
    card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr = NULL;

    if ((mmgsdi_event->data.refresh.refresh_files.num_files > 0) &&
        (mmgsdi_event->data.refresh.refresh_files.file_list_ptr != NULL))
    {
      uint32 list_size = sizeof(mmgsdi_file_enum_type)*mmgsdi_event->data.refresh.refresh_files.num_files;

      //Need to copy FCN refresh list seperately.
      PBM_MEM_ALLOC(card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr,
                    list_size);
      PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr, VOID);
      memcpy(card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr,
             mmgsdi_event->data.refresh.refresh_files.file_list_ptr,list_size);
    }
  }

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(card_status));
}

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
)
{
  sim_capabilities_T dummy_sim_cap;

  PBM_VALIDATE_SLOT_RET(slot, VOID);

  /* If I'm initializing now already, continue initialization later */
  if (pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(slot))
  {
    PBM_MSG_HIGH("PBM Already init slot %d.  Continue later.\n",slot,0,0);
    PBM_SIMSLOT_SET(pbm_slot_restart, slot);
    return;
  }

  PBM_SIMSLOT_RESET(pbm_slot_pin_waiting, slot);
  PBM_SIMSLOT_RESET(pbm_slot_init_waiting, slot);
  pbm_clear_and_set_sim_caches(slot, PBM_STATUS_NOT_READY);

  pbm_gsdi_register_fcn(slot);

#ifdef FEATURE_WMS
  if (pbm_wms_init_finished)
  {
    /* Initialization happens from within pbm_gsdi_sim_cap_cb since we
     * need to check whether FDN is enabled before starting */
    if (slot == PBM_SLOT_1)  //lint !e774 always true
    {
      (void) gsdi_get_sim_capabilities(&dummy_sim_cap, 0, pbm_gsdi_sim_cap_cb);
    }
#if (MAX_UIMS > 1)
    else
    {
      (void) gsdi2_get_sim_capabilities(GSDI_SLOT_2, NULL, 0, pbm_gsdi_sim_cap_cb);
    }
#endif /* MAX_UIMS > 1 */
  }
#else
  /* no WMS, so init here */
  /* Initialization happens from within pbm_gsdi_sim_cap_cb since we
   * need to check whether FDN is enabled before starting */
  if (slot == PBM_SLOT_1)  //lint !e774 always true
  {
    (void) gsdi_get_sim_capabilities(&dummy_sim_cap, 0, pbm_gsdi_sim_cap_cb);
  }
#if (MAX_UIMS > 1)
  else
  {
    (void) gsdi2_get_sim_capabilities(GSDI_SLOT_2, NULL, 0, pbm_gsdi_sim_cap_cb);
  }
#endif /* MAX_UIMS > 1 */
#endif
}

/*===========================================================================
FUNCTION PBM_HANDLE_SIM_INSERTED

DESCRIPTION
  Function to init SIM variables, and do processing that can be done
  without access restrictions (eg. ECC).

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_handle_sim_inserted (
  pbm_slot_type slot
)
{
  gsdi_pin_status_T pin_status_dummy;
  byte num_retries_dummy;

  PBM_MSG_HIGH("PBM acting as if it received SIM inserted slot %d, num slots %d", slot, MAX_UIMS, 0);

  PBM_VALIDATE_SLOT_RET(slot, VOID);

  PBM_SIMSLOT_SET(pbm_slot_card_inserted, slot);
  PBM_SIMSLOT_SET(pbm_slot_pin_waiting, slot);
  PBM_SIMSLOT_SET(pbm_slot_init_waiting, slot);
  pbm_clear_and_set_sim_caches(slot, PBM_STATUS_NOT_AVAILABLE);
//initializing ECC will be done as part of the SIM files reading.
//hence this is not required if 167785 is TRUE
#ifndef FEATURE_ECC_HANDLING_AT_PBM
  pbm_init_ecc_internal(slot);
#endif

  pbm_ecc_set_hardcoded_eccs(TRUE, (ecc_has_entries!=0)?TRUE:FALSE);

  //move this to MMGSDI when available
  if (slot == PBM_SLOT_1) //lint !e774 always true
  {
    (void) gsdi_get_pin_status(GSDI_PIN1,
                               &pin_status_dummy,
                               &num_retries_dummy,
                               0,
                               pbm_gsdi_pin_status_cb);
  }
#if (MAX_UIMS > 1)
  else
  {
    (void) gsdi2_get_pin_status(GSDI_SLOT_2,
                                GSDI_PIN1,
                                NULL,
                                0,
                                pbm_gsdi_pin_status_cb);

  }
#endif //MAX_UIMS > 1
}
/* <EJECT> */
/*===========================================================================
FUNCTION PBM_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.
  Note:  MMGSDI returns the most recent event at registration.  So, our first
  event may be MMGSDI_CARD_INSERTED_EVT, or it may be MMGSDI_CARD_INIT_COMPLETED_EVT

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_card_status_cb(
    mmgsdi_event_data_type   mmgsdi_event
)
{
  pbm_slot_type slot;
  uint32 i,j;
  boolean found;

  switch (mmgsdi_event.evt)
  {
    case MMGSDI_SELECT_AID_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.select_aid.slot);

      PBM_MSG_HIGH("PBM received SIM inserted slot %d, num slots %d, app_id", slot, MAX_UIMS, 0);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      pbm_provisioning_gsdi_aid[slot][0] = mmgsdi_event.data.select_aid.app_data;
      pbm_num_provisioning_gsdi_aid[slot] = 1;

      break;

    case MMGSDI_CARD_INSERTED_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_inserted.slot);

      PBM_MSG_HIGH("PBM received SIM inserted slot %d, num slots %d", slot, MAX_UIMS, 0);

      pbm_handle_sim_inserted(slot);

      break;  //MMGSDI_CARD_INSERTED_EVT

    case MMGSDI_CARD_INIT_COMPLETED_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_init_completed.slot);

      PBM_MSG_HIGH("PBM received INIT completed slot %d, num slots %d",
               slot, MAX_UIMS, 0);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      pbm_num_provisioning_gsdi_aid[slot] = mmgsdi_event.data.card_init_completed.num_avail_apps;
      for (i=0; i<mmgsdi_event.data.card_init_completed.num_avail_apps; i++)
      {
        pbm_provisioning_gsdi_aid[slot][i] = mmgsdi_event.data.card_init_completed.app_info[i].app_data;
      }

      if ((pbm_slot_card_inserted & PBM_SIMID_TO_SLOT_MASK(slot)) == 0)
      {
        pbm_handle_sim_inserted(slot);
      }

      pbm_handle_sim_init_completed(slot);
      break;


    case MMGSDI_PIN1_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot);

      PBM_MSG_HIGH("PBM received PIN1 event %d slot %d, num slots %d.",
               mmgsdi_event.data.pin.pin_info.status,
               slot,
               MAX_UIMS);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      if ((pbm_slot_card_inserted & PBM_SIMID_TO_SLOT_MASK(slot)) == 0)
      {
        pbm_handle_sim_inserted(slot);
      }

      //See if this event applies to us
      found = FALSE;
      for (j=0; (j<pbm_num_provisioning_gsdi_aid[slot]) && (found == FALSE); j++)
      {
        for (i=0; i<mmgsdi_event.data.pin.num_aids; i++)
        {
          if (0 == memcmp(&mmgsdi_event.data.pin.aid_type[i],
                          &pbm_provisioning_gsdi_aid[slot][j],
                          sizeof(pbm_provisioning_gsdi_aid[slot][j])))
          {
            found = TRUE;
            break;
          }
        }
      }

      if (!found)
      {
        PBM_MSG_HIGH("PBM received PIN1 event for non-provisioning app", 0,0,0);
        return;
      }

      if (mmgsdi_event.data.pin.pin_info.status == MMGSDI_PIN_PERM_BLOCKED)
      {
        PBM_MSG_HIGH("PBM received SIM PERM blocked slot %d", slot, 0, 0);
        pbm_stop_fileinfo_internal(slot);
        pbm_clear_and_set_sim_caches(slot, PBM_STATUS_SIM_FAIL);
        if (slot == PBM_SLOT_1) //lint !e774 always true
        {
          pbm_ecc_clear_all_id(PBM_FIELD_SIM1_ECC);
        }
        else
        {
          pbm_ecc_clear_all_id(PBM_FIELD_SIM2_ECC);
        }

      }
      break;


    case MMGSDI_CARD_ERROR_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_error.slot);

      PBM_MSG_HIGH("PBM received card error slot %d, num slots %d",
               slot, MAX_UIMS, 0);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      PBM_SIMSLOT_RESET(pbm_slot_card_inserted, slot);

      pbm_stop_fileinfo_internal(slot);
      pbm_clear_and_set_sim_caches(slot, PBM_STATUS_NO_SIM);
      if (slot == PBM_SLOT_1)  //lint !e774 always true
      {
        pbm_ecc_clear_all_id(PBM_FIELD_SIM1_ECC);
      }
      else
      {
        pbm_ecc_clear_all_id(PBM_FIELD_SIM2_ECC);
      }

      break;

    case MMGSDI_CARD_REMOVED_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_removed.slot);
      PBM_MSG_HIGH("PBM received card removed slot %d, num slots %d",
               slot, MAX_UIMS, 0);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      PBM_SIMSLOT_RESET(pbm_slot_card_inserted, slot);

      pbm_stop_fileinfo_internal(slot);
      pbm_clear_and_set_sim_caches(slot, PBM_STATUS_NO_SIM);
      if (slot == PBM_SLOT_1) //lint !e774 always true
      {
        pbm_ecc_clear_all_id(PBM_FIELD_SIM1_ECC);
      }
      else
      {
        pbm_ecc_clear_all_id(PBM_FIELD_SIM2_ECC);
      }
      break;

    case MMGSDI_FDN_EVT:
      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.fdn.slot);
      PBM_MSG_HIGH("PBM received FDN event %d slot %d, num slots %d",
               mmgsdi_event.data.fdn.enabled, slot, MAX_UIMS);

      PBM_VALIDATE_SLOT_RET(slot, VOID);

      if (mmgsdi_event.data.fdn.enabled)
      {
        pbm_fdn_enable(slot);
      }
      else
      {
        pbm_fdn_disable(slot);
      }
      break;


    case MMGSDI_REFRESH_EVT:
      PBM_MSG_HIGH("PBM received REFRESH slot %d, mode %d, stage %d",
                   mmgsdi_event.data.refresh.slot,
                   mmgsdi_event.data.refresh.mode,
                   mmgsdi_event.data.refresh.stage);

      if (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_FCN)
      {
        if (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
        {
          //Kick off pbm app fcn refresh
          pbm_refresh_file_change_notification_cb(&(mmgsdi_event.data.refresh.refresh_files),
                                                mmgsdi_event.data.refresh.slot);
        }
        else if (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
        {
          //Notify MMGSDI it's okay to proceed with FCN refresh
          mmgsdi_option_type no_option = {0};

          mmgsdi_ok_to_refresh(pbm_mmgsdi_client_id,
                               mmgsdi_event.data.refresh.slot,
                               TRUE,
                               no_option,
                               pbm_mmgsdi_ok_to_refresh_cb,
                               NULL);

        }
      }
      else if ((mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN) &&
               (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN))
      {
        //Notify MMGSDI it's okay to proceed with FCN refresh
        mmgsdi_option_type no_option = {0};

        mmgsdi_ok_to_refresh(pbm_mmgsdi_client_id,
                             mmgsdi_event.data.refresh.slot,
                             TRUE,
                             no_option,
                             pbm_mmgsdi_ok_to_refresh_cb,
                             NULL);
      }

      //Clean up deep copy of file list
      PBM_MEM_FREEIF(mmgsdi_event.data.refresh.refresh_files.file_list_ptr);
      break;

    case MMGSDI_PIN2_EVT:
    case MMGSDI_SWITCH_SLOT_EVT:
    case MMGSDI_ILLEGAL_CARD_EVT:
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
    case MMGSDI_SAP_PIN_EVT:
    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
    default:
      break;
  }
}


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
                               mmgsdi_callback_type callback)
{
  int len = 0;
  boolean ret_val = FALSE;
  PB_CACHE *cache;
  pbm_ext_cache_s_type *ext_cache;
  mmgsdi_file_structure_enum_type sim_filetype = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
#ifdef FEATURE_PBM_USIM_SUPPORT
  PB_USIM_CACHE *usim_cache;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_SYNC1_CACHE *sync1_cache;
  PB_SYNC2_CACHE *sync2_cache;
#endif /* FEATURE_PBM_USIM_SUPPORT */

  switch(pbm_file_id_to_cache_type(file_id))
  {
    case PBM_CACHE_TYPE_PB:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_CACHE_TYPE_ECC:
#endif
      if ((cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = cache->record_length;
        sim_filetype = cache->file_type;
      }
      break;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_CACHE_TYPE_USIM:
      if ((usim_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = usim_cache->record_length;
        sim_filetype = usim_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_INDEX:
      if ((m_usim_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = m_usim_cache->iap_rec_len;
        sim_filetype = m_usim_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_SYNC1:
      if ((sync1_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = sync1_cache->record_length;
        sim_filetype = sync1_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_SYNC2:
      if ((sync2_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = sync2_cache->record_length;
        sim_filetype = sync2_cache->file_type;
      }
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

    case PBM_CACHE_TYPE_EXT:
      if ((ext_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) {
        len = ext_cache->ext_rec_length;
        sim_filetype = ext_cache->file_type;
      }
      break;

    default:
      PBM_MSG_ERR("Unknown file id %d", file_id, 0, 0);
      break;
  }

  if (len == 0)
  {
    PBM_MSG_ERR("Could not issue read request for filename 0x%x on SIM 0x%x",
        sim_filename, sim_id, 0);
  }
  else
  {
    pbm_return_type pbm_retval;

    /* Now issue the asynchronous command to GSDI. */
    pbm_retval = pbm_gsdi_sim_read(sim_id,            /* Sim number */
                                   sim_filename,      /* filename */
                                   sim_filetype,      /* filetype */
                                   rec_num,           /* Record number */
                                   len,               /* Required data length */
                                   (uint32) pbm_buffer_index,  /* Client Ref. */
                                   callback);         /* Asynchronous callback. */
    if (pbm_retval != PBM_SUCCESS)
    {
      PBM_MSG_ERR("PBM pbm_gsdi_sim_read returned an error 0x%x for filename 0x%x on SIM 0x%x",
          pbm_retval, sim_filename, sim_id);
    }
    else
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*===========================================================================
FUNCTION PBM_UPDATE_REFRESH_GLOBALS

DESCRIPTION
  This function is called to update the REFRESH Related Global Variables
  contained in PBM.

DEPENDENCIES
  None.

SIDE EFFECTS
  Static Globals in uiupbm.c are update:
  refresh_num_files and refresh_file_list
===========================================================================*/
static boolean pbm_update_refresh_globals(
  mmgsdi_refresh_file_list_type  *refresh_file_list_p,
  mmgsdi_slot_id_enum_type       slot)
{
  uint8 num_file_devices;
  int refresh_file;
  pbm_slot_type sim_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(slot);
  int file_group_index;
  int file_device_index;
  int i;

  pbm_file_device_e_type pbm_file_group;

  pbm_file_device_e_type affected_groups[PBM_FILE_MAX_REQUESTS];
  int num_affected_groups;

  if ( refresh_file_list_p->num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS )
  {
    PBM_MSG_ERR("PBM pbm_update_refresh_globals refresh too many files %d",
        refresh_file_list_p->num_files, 0, 0);
    return FALSE;
  }

  /* Copy Number of Files */
  pbm_refresh_num_files = refresh_file_list_p->num_files;

  if ( refresh_file_list_p->file_list_ptr == NULL )
  {
    return FALSE;
  }

  /* Copy File List */
  memcpy(pbm_refresh_file_list,
         refresh_file_list_p->file_list_ptr,
         sizeof(mmgsdi_file_enum_type) * refresh_file_list_p->num_files);
  pbm_refresh_init = TRUE;

  /* figure out which groups of files were affected */
  pbm_refresh_num_file_devices = 0;
  num_file_devices = 0;
  num_affected_groups = 0;

  memset(pbm_refresh_pbm_file_devices,0,sizeof(pbm_refresh_pbm_file_devices));
  memset(affected_groups,0,sizeof(affected_groups));

  for (refresh_file=0; refresh_file < pbm_refresh_num_files; refresh_file++)
  {
    pbm_file_group = pbm_uim_filename_to_refresh_group(pbm_refresh_file_list[refresh_file],sim_id);

    /* add it to the list, if not already there */
    if (pbm_file_group != PBM_FILE_NONE)
    {
      PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_group,
                                  affected_groups,
                                  num_affected_groups,
                                  file_group_index);
    }
  }

  /* now translate the affected groups into the list of files */

  /* go through the list of file groups */
  /* possible optimization: if PBM_FILE_ALL is found, or array becomes full, stop looking */
  for (file_group_index=0; file_group_index < num_affected_groups; file_group_index++)
  {
    /* find the files associated */
    for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      if ((affected_groups[file_group_index] ==
           pbm_file_to_uim_file[i].pbm_file_group[USE_USIM(sim_id)]) ||
          (affected_groups[file_group_index] == PBM_FILE_ALL))
      {
        /* This one is affected.  add it to the list if not already there */
        PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_to_uim_file[i].pbm_filename,
                                    pbm_refresh_pbm_file_devices,
                                    num_file_devices,
                                    file_device_index);
      }
    }
  } /* end loop over affected file groups */

  if(sim_id < MAX_UIMS) //lint check for sim_id crossing the boundary of MAX_UIMS
  {
      //Add in any files that haven't yet been read the first time
    for (i=pbm_initialize_index[sim_id]; i<(int)ARR_SIZE(pbm_initialize_list); i++)
    {
      PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_initialize_list[i],
                                  pbm_refresh_pbm_file_devices,
                                  num_file_devices,
                                  file_device_index);
    }
  }

  pbm_refresh_num_file_devices = num_file_devices;

  return TRUE;
}

/*===========================================================================
FUNCTION PBM_GSDI_REFRESH_COMPLETE

DESCRIPTION
  This function sends refresh complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_refresh_complete(pbm_slot_type sim_id)
{
  pbm_device_type        pb_id;
  int                    i, j;
  uint8                  local;     /* Local vs Global phonebook */
  mmgsdi_file_enum_type  uim_file;
  pbm_file_device_e_type pbm_file;
  uim_items_enum_type    pbm_compat_refresh_file_list[ARR_SIZE(pbm_refresh_file_list)];
  mmgsdi_option_type     no_option = {0};

  if (pbm_refresh_init == TRUE ) {
    /* Initialization is due to Refresh file
    ** change notification
    */
    refresh_pass_fail = TRUE;

    for (i=0; i<pbm_refresh_num_files; i++)
    {
      if (mmgsdi_util_convert_to_uim_items_enum(pbm_refresh_file_list[i],
                                                &pbm_compat_refresh_file_list[i])
          != MMGSDI_SUCCESS)
      {
        PBM_MSG_ERR("Could not convert item number %d to UIM file.  Data %d",
                    i, pbm_refresh_file_list[i], 0);
      }
    }

    (void) mmgsdi_refresh_complete (pbm_mmgsdi_client_id,
                                   PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id),
                                   refresh_pass_fail,
                                   no_option,
                                   pbm_mmgsdi_refresh_complete_cb,
                                   NULL);

    /* Send notification about refresh done for each phonebook that is
     * changed */
    for (i = 0; i < (int) ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      pbm_file = pbm_file_to_uim_file[i].pbm_filename;

      /* Check if the phonebook was changed */
      for (j = 0; j < pbm_refresh_num_files; j++)
      {
        /*Check both the local and global versions*/
        for (local = 0; local < (uint8) ARR_SIZE(pbm_file_to_uim_file[i].uim_filename); local++ )
        {
          uim_file = pbm_file_to_uim_file[i].uim_filename[local];
          if (uim_file == pbm_refresh_file_list[j])
          {
            pb_id = pbm_file_id_to_pb_id(sim_id, pbm_file);
            if (pb_id != PBM_DEFAULT)
            {
              PBM_NOTIFY(PBM_EVENT_PB_REFRESH_DONE, pb_id, pb_id);
            }
          }
        }
      }
    }

    pbm_refresh_init = FALSE;

  }
}

/*===========================================================================
FUNCTION PBM_GSDI_CLIENT_INIT_COMPLETE

DESCRIPTION
  This function sends client init complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_client_init_complete(pbm_slot_type sim_id)
{
  mmgsdi_option_type no_option = {0};

  (void) mmgsdi_client_init_complete (pbm_mmgsdi_client_id,
                                      PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id),
                                      no_option, pbm_mmgsdi_init_complete_cb,
                                      NULL);
}

/*===========================================================================
FUNCTION PBM_FREE_EXTENSION_RECORD

DESCRIPTION
  When we will overwrite a record, this routine will find the
  extension record associated with that record and free that in
  our cache structures. It returns the record it freed, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_free_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                              mmgsdi_file_enum_type filename)
{
  int i;
  int ext_num_of_rec;
  pbm_ext_rec_in_use_s_type *ext_rec_in_use;

  PBM_CHECK_PTR_RET(ext_cache, 0);

  ext_num_of_rec = ext_cache->num_of_rec;
  ext_rec_in_use = ext_cache->in_use_array;

  /* Now search through the extension records to see if there is
   * any available.
   */
  for (i = ext_num_of_rec; i > 0; i--)
  {
    /* Is the current record already using an extension record? */
    if ((ext_rec_in_use[i].uim_device == filename) &&
        (ext_rec_in_use[i].index == index))
    {
      /* erase */
      ext_rec_in_use[i].uim_device = MMGSDI_NO_FILE_ENUM;
      ext_rec_in_use[i].index = 0;

      return i;
    }
  }
  /* if we got here, we didn't find it */
  return 0;
}
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
int pbm_find_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                              mmgsdi_file_enum_type filename)
{
  int i;        /* Control variable for loop */
  int ext_num_of_rec;
  pbm_ext_rec_in_use_s_type *ext_rec_in_use;
  int extension_record = 0;

  PBM_CHECK_PTR_RET(ext_cache, 0);

  ext_num_of_rec = ext_cache->num_of_rec;
  ext_rec_in_use = ext_cache->in_use_array;

  /* Now search through the extension records to see if there is
   * any available.
   */
  for (i = ext_num_of_rec; i > 0; i--)
  {
    /* Is the current record already using an extension record? */
    if ((ext_rec_in_use[i].uim_device == filename) &&
        (ext_rec_in_use[i].index == index))
    {
      extension_record = i;
      break;
    }
    else /* pick a new extension record */
    {
      if (ext_rec_in_use[i].index == 0)
        extension_record = i;
    }
  }

  if (extension_record==0)
  {
    PBM_MSG_ERR("No more Extension records available %d %d", ext_num_of_rec, filename, 0);
  }

  return extension_record;
}

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
boolean pbm_check_extension_record(pbm_ext_cache_s_type *ext_cache, int index,
                                   mmgsdi_file_enum_type filename)
{
  int i;
  int ext_num_of_rec;
  pbm_ext_rec_in_use_s_type *ext_rec_in_use;

  PBM_CHECK_PTR_RET(ext_cache, FALSE);

  ext_num_of_rec = ext_cache->num_of_rec;
  ext_rec_in_use = ext_cache->in_use_array;

  /* Now search through the extension records */
  for (i = ext_num_of_rec; i > 0; i--)
  {
    /* Is the current record the right one? */
    if ((ext_rec_in_use[i].uim_device == filename) &&
        (ext_rec_in_use[i].index == index))
    {
      return TRUE;
    }
  }
  /* if we got here, we didn't find it */
  return FALSE;
}

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
                                            pbm_text_record_s_type *return_data)
{
  uint16 text_length;
  uint16 adn_rec_id;
  PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id,
                                                                   file_id);
  boolean convert = TRUE;

  pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_text_record_s_type));

  text_length = (uint16) usim_cache->record_length;
  if ((text_length > 2) && (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2))
  {
    adn_rec_id = pbm_location_to_adn_rec_id(sim_id,
                                            pb_set,
                                            data_buffer[text_length-1]);

    if ((adn_rec_id == 0) ||
        !pbm_validate_sfi(adn_rec_id, data_buffer[usim_cache->record_length-2]))
    {
      /* This is OK if the record is empty, otherwise not OK */
      if (data_buffer[0] != 0xff)
      {
        PBM_MSG_ERR("USIM record in index %d, file %d not valid",
                    index, file_id, 0);
        return PBM_ERROR;
      }
      convert = FALSE;
    }
    text_length -= 2;
  }
  else
  {
    adn_rec_id = pbm_location_to_adn_rec_id(sim_id,
                                            pb_set,
                                            (uint8)index);
  }

  if (convert) {
    return_data->adn_record_id = adn_rec_id;

    (void) pbmconvert_sim_to_string(data_buffer, text_length, return_data->text,
                                   (uint16) ARR_SIZE(return_data->text));
  } else {
    return_data->adn_record_id = 0;
  }

  return PBM_SUCCESS;
}

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
                                           int *extension_index)
{
  pbm_fixed_sim_rec_s_type *sim_rec;
  uint16 adn_rec_id;
  PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id,
                                                                    file_id);
  pbm_ext_cache_s_type *ext_cache;

  pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  PBM_CHECK_PTR4_RET(data_buffer, return_data, usim_cache, extension_index, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_anr_record_s_type));

  return_data->anr_id = data_buffer[0];

  //lint -e{826} struct has oci/ici, even though we may not access it
  sim_rec = (pbm_fixed_sim_rec_s_type *)&data_buffer[1];

  if ((sim_rec->bcd_len != 0xFF) && (sim_rec->bcd_len != 0))
  {
    if (!pbm_bcd_to_ascii(sim_rec->number,
                          sim_rec->bcd_len-1,
                          sim_rec->ton_npi,
                          (byte *)return_data->number))
    {
      PBM_MSG_ERR("USIM ERROR record %d, file %d could not be converted to ascii", index, file_id, 0);
    }
    return_data->type = sim_rec->ton_npi;
  }

  if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2)
  {
    adn_rec_id = pbm_location_to_adn_rec_id(sim_id,
                                            pb_set,
                                            data_buffer[usim_cache->record_length-1]);
    if ((adn_rec_id == 0) ||
        !pbm_validate_sfi(adn_rec_id, data_buffer[usim_cache->record_length-2]))
    {
      /* This is OK if the record is empty, otherwise not OK */
      if (data_buffer[0] != 0xff)
      {
        PBM_MSG_ERR("USIM record in index %d, file %d not valid",
                    index, file_id, 0);
        return PBM_ERROR;
      }
      adn_rec_id = 0;
    }
  }
  else
  {
    adn_rec_id = pbm_location_to_adn_rec_id(sim_id,
                                            pb_set,
                                            (uint8)index);
  }

  return_data->adn_record_id = adn_rec_id;

  if (sim_rec->ext_id == 0xFF)
  {
    *extension_index = 0;
  }
  else
  {
    ext_cache = &ext1_cache[sim_id];
    if (sim_rec->ext_id < 1 ||
        sim_rec->ext_id > (byte)ext_cache->num_of_rec)
    {
      PBM_MSG_ERR("USIM record index %d file %d wants extension record %d",
                  index, file_id, (int) sim_rec->ext_id);
    }
    else
    {
      *extension_index = (int)sim_rec->ext_id;
      ext_cache->in_use_array[*extension_index].uim_device =
                                                  usim_cache->filename;
      ext_cache->in_use_array[*extension_index].index = (uint8)index;
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_gsdi_process_grp_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
//lint -e{715} suppress unused parameters (index)
pbm_return_type pbm_gsdi_process_grp_data( pbm_slot_type sim_id,
                                           pbm_file_device_e_type file_id,
                                           int index,
                                           byte *data_buffer,
                                           pbm_grp_record_s_type *return_data)
{
  int i;
  PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id,
                                                                   file_id);
  PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_grp_record_s_type));

  for (i = 0; i < usim_cache->record_length; i++)
    if (data_buffer[i] != 0xFF)
      return_data->group_id[i] = (uint16) data_buffer[i];

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_gsdi_process_index_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
//lint -e{715} suppress unused parameters (index)
pbm_return_type pbm_gsdi_process_index_data( pbm_slot_type sim_id,
                                             pbm_file_device_e_type file_id,
                                             int index,
                                             byte *data_buffer,
                                             pbm_iap_record_s_type *return_data)
{
  int i;
  PB_MASTER_USIM_CACHE *m_usim_cache = (PB_MASTER_USIM_CACHE *)
                              pbm_file_id_to_cache(sim_id, file_id);

  PBM_CHECK_PTR3_RET(data_buffer, return_data, m_usim_cache, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_iap_record_s_type));

  for (i = 0; i < m_usim_cache->iap_rec_len; i++)
    if (data_buffer[i] != 0xFF)
      return_data->index[i] = data_buffer[i];

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_GSDI_PROCESS_SIM_DATA

DESCRIPTION
  Parse the data buffer read by pbm_gsdi_sim_read then populate the record
  stucture passed in by the caller.  The return value from this function
  is whether the buffer contained a complete record (TRUE), or if an
  extension record needs to be read (FALSE).  If the function returns
  FALSE, it also populates extension_index to contain the record to be
  read.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean pbm_gsdi_process_sim_data(pbm_slot_type sim_id,
                                         pbm_file_device_e_type file_id,
                                         int index,
                                         byte *local_pbm_data_buffer,
                                         pbm_num_text_record_s_type *return_data,
                                         int *extension_index)
{
  pbm_fixed_sim_rec_s_type *fixed_sim_record;
#ifdef FEATURE_PBM_USIM_SUPPORT
  clk_julian_type julian_time;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PB_CACHE *cache = (PB_CACHE *)pbm_file_id_to_cache(sim_id, file_id);

  PBM_CHECK_PTR4_RET(return_data, extension_index, local_pbm_data_buffer, cache, FALSE);

  /* Initialize return record to 0's */
  memset(return_data, 0, sizeof(pbm_num_text_record_s_type));

  /* Store the index as this index. */
  return_data->index = index;

  /* Point to the SIM record starting at the number. */
  //lint -e{826} struct has oci/ici, even though we may not access it
  fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                   cache->text_length);

  /* copy text */
  (void) pbmconvert_sim_to_string(local_pbm_data_buffer,
                                  (uint16) cache->text_length,
                                  return_data->text,
                                  (uint16) ARR_SIZE(return_data->text));

  /* If the number is not blank */
  if (fixed_sim_record->bcd_len != 0xff)
  {
    /* Convert the number to ASCII. */
    if (!pbm_bcd_to_ascii(fixed_sim_record->number,
                          fixed_sim_record->bcd_len-1,
                          fixed_sim_record->ton_npi,
                          (byte *) return_data->number ))
    {
      PBM_MSG_ERR("SIM ERROR record %d could not be converted to ascii", index, 0, 0);
    }
    return_data->type = fixed_sim_record->ton_npi;
  }

#ifdef FEATURE_PBM_USIM_SUPPORT
  /* These guys have the timestamp. */
  if ((file_id == PBM_FILE_ICI) || (file_id == PBM_FILE_OCI))
  {
    /* Convert timestamp */
    julian_time.year   = pbm_bcd_to_bin(fixed_sim_record->date_time[0]);

    if (julian_time.year >= 80)  /* 80 becomes 1980, 79 becomes 2079 */
      julian_time.year += 1900;
    else
      julian_time.year += 2000;

    julian_time.month  = pbm_bcd_to_bin(fixed_sim_record->date_time[1]);
    julian_time.day    = pbm_bcd_to_bin(fixed_sim_record->date_time[2]);
    julian_time.hour   = pbm_bcd_to_bin(fixed_sim_record->date_time[3]);
    julian_time.minute = pbm_bcd_to_bin(fixed_sim_record->date_time[4]);
    julian_time.second = pbm_bcd_to_bin(fixed_sim_record->date_time[5]);
    return_data->time = clk_julian_to_secs(&julian_time);

    return_data->duration = fixed_sim_record->duration[0] << 16 |
                            fixed_sim_record->duration[1] << 8  |
                            fixed_sim_record->duration[2];

    if ( return_data->duration == 0x00FFFFFF )
    {
      return_data->duration = 0;
    }
  }
#endif /* FEATURE_PBM_USIM_SUPPORT */

  /* Figure out if there is an extension record.  If not, we are done
   * return TRUE.  If there is, say that there is and return FALSE that
   * the processing is not complete.
   */
  if (fixed_sim_record->ext_id == 0xFF)
  {
    *extension_index = 0;
  }
  else  /* Extension file is needed */
  {
    if (!cache->extension_cache)
    {
      PBM_MSG_ERR("SIM ERROR, record %d no extension",
          index,  (int) fixed_sim_record->ext_id, 0);
    }
    else if (((int) fixed_sim_record->ext_id < 1) ||
         ((int) fixed_sim_record->ext_id > cache->extension_cache->num_of_rec))
    {
      PBM_MSG_ERR("SIM ERROR record %d wants extension record %d of %d",
          index, (int) fixed_sim_record->ext_id,
          cache->extension_cache->num_of_rec);
    }
    else
    {
      *extension_index = (int) fixed_sim_record->ext_id;
      cache->extension_cache->in_use_array[*extension_index].uim_device =
        pbm_pb_record_index_to_filename(cache, return_data->index);
      cache->extension_cache->in_use_array[*extension_index].index =
        (byte)pbm_pb_record_index_to_fileloc(cache, return_data->index);
      return FALSE;     /* Processing is not complete. */
    }
  }

  return TRUE;          /* Processing of this buffer is correct. */
}

/*===========================================================================
FUNCTION PBM_GSDI_PROCESS_EXT_DATA

DESCRIPTION
  If the phone number is longer than ADN_MAX_PHONE_NUMBER, the
  remainder of the phone number was stored in the extension record
  in the EXT file.  After the extension data is read from GSDI, this
  function is called with the return_data which was filled in by
  pbm_gsdi_process_sim_data and this finishes where that function
  left off.

DEPENDENCIES
  return_data should be filled in from the main gsdi record first.

SIDE EFFECTS
===========================================================================*/
static boolean pbm_gsdi_process_ext_data(byte *sim_data,
                                  byte *number)
{
  int bcd_index;
  int extension_length;
  byte asc_1;
  byte asc_2;
  uint32 ascii_index = 20;

  PBM_CHECK_PTR2_RET(sim_data, number, FALSE);

  /* Is this an International call? */
  if (number[0] == '+')
    ascii_index++;

  /* Check the type. */
  if (sim_data[0] != EXT_REC_TYPE_ADDITIONAL_DATA)
  {
    PBM_MSG_HIGH("Extension record type %d not supported", sim_data[0], 0, 0);
    return TRUE;
  }

  /* Check the extension_length. Log any errors, but continue processing. */
  extension_length=sim_data[1];
  if (extension_length > ((PBM_MAX_NUM_LENGTH - (2*PBM_SIM_DIALING_NUMBER_LEN))/2))
  {
    PBM_MSG_HIGH("Extension data longer than supported: %d", extension_length, 0, 0);
    extension_length = ((PBM_MAX_NUM_LENGTH - (2*PBM_SIM_DIALING_NUMBER_LEN))/2);
  }

  memset(&number[ascii_index], 0,
       (PBM_NUM_SIZE*sizeof(char)-ascii_index));

  /* Flip the digits around for BCD numbers */
  for( bcd_index = 2; bcd_index < (2+extension_length); bcd_index++)
  {
    asc_1 = (sim_data[bcd_index] & 0x0F);
    asc_2 = (sim_data[bcd_index] & 0xF0) >> 4;

    /* Check first digit for special characters */
    number[ascii_index++] = pbm_bcd_to_char_mapping[asc_1];

    /* Check second digit for special characters */
    number[ascii_index++] = pbm_bcd_to_char_mapping[asc_2];
  }

  return TRUE;  /* We read this record successfully. */
}

/*===========================================================================
FUNCTION PBM_GSDI_FILE_INFO_CB

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
                           const mmgsdi_cnf_type  *data_ptr)
{
  pbm_slot_type sim_id;
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
  uint32 client_ref;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
  {
    PBM_MSG_ERR("Invalid client_ref found: %d\n", client_ref, 0, 0);
    return;
  }

  sim_id = (pbm_slot_type)((int) client_ref / (int) PBM_FILE_MAX_REQUESTS);

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_FILE_INFO_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memcpy(&mmgsdi_resp_ptr->data, data_ptr, sizeof(mmgsdi_cnf_type));

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_GSDI_READ_PBR_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/
void pbm_gsdi_read_pbr_cb(gsdi_cnf_T *sim_data)
{
  pbm_gsdi_resp_s_type *gsdi_resp_ptr;

  PBM_CHECK_PTR_RET(sim_data, VOID);

  MSG_MED("Received PBR info for GSDI slot %d", sim_data->message_header.slot,
          0, 0);

  gsdi_resp_ptr = (pbm_gsdi_resp_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_gsdi_resp_s_type));
  PBM_CHECK_PTR_RET(gsdi_resp_ptr, VOID);

  gsdi_resp_ptr->cmd_type = PBM_GSDI_READ_PBR_CB;
  memcpy(&gsdi_resp_ptr->gsdi_data, sim_data, sizeof(gsdi_cnf_T));

  pbm_gsdi_resp_q_put(PBM_CMD_PTR(gsdi_resp_ptr));
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_CLEAR_REFRESH_CACHES

DESCRIPTION
  This function will clear the caches associated with the global refresh file
  list.

PARAMETERS

SIDE EFFECTS
===========================================================================*/
static void pbm_clear_refresh_caches(pbm_slot_type sim_id)
{
  int i;
  pbm_file_device_e_type file_id;
  PB_CACHE *cache;
  pbm_ext_cache_s_type *ext_cache;
#ifdef FEATURE_PBM_USIM_SUPPORT
  PB_USIM_CACHE *usim_cache;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_SYNC1_CACHE *sync1_cache;
  PB_SYNC2_CACHE *sync2_cache;
#endif /* FEATURE_PBM_USIM_SUPPORT */

  /* go through the list of files that you need to clear out */
  /* find the files associated */
  for (i=0; i<pbm_refresh_num_file_devices; i++)
  {
    /* clear the cache */
    file_id = pbm_refresh_pbm_file_devices[i];
    switch(pbm_file_id_to_cache_type(file_id))
    {
      case PBM_CACHE_TYPE_PB:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      case PBM_CACHE_TYPE_ECC:
#endif
        if ((cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) //lint !e826 area too small
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_cache(cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_EXT:
        if ((ext_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          /*for ext caches, you can just clear the in use array */
          if (ext_cache->num_of_rec)
          {
            memset(ext_cache->in_use_array, 0, (uint32)(ext_cache->num_of_rec + 1) *
                   sizeof(pbm_ext_rec_in_use_s_type));
          }
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_CACHE_TYPE_USIM:
        if ((usim_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) //lint !e826 area too small
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_usim_cache(usim_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_INDEX:
        if ((m_usim_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) //lint !e826 area too small
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_index_cache(m_usim_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_SYNC1:
        if ((sync1_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) //lint !e826 area too small
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_sync1_cache(sync1_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_SYNC2:
        if ((sync2_cache = pbm_file_id_to_cache(sim_id, file_id)) != NULL) //lint !e826 area too small
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_sync2_cache(sync2_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

      default:
        PBM_MSG_ERR("Unknown file id %d", file_id, 0, 0);
        break;
    }

#ifdef FEATURE_PBM_USIM_SUPPORT
    /* if we're doing the ADN group, we need to clear the USIM caches, too. */
    if (file_id == PBM_FILE_ADN)
    {
      pbm_clear_usim_caches(sim_id);
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
  }
}

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
uint8 pbm_get_card_apps(pbm_slot_type slot)
{
  PBM_VALIDATE_SLOT_RET(slot, 0);

  return pbm_card_apps[slot];
}

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
void pbm_set_card_apps(pbm_slot_type slot, uint8 card_apps)
{
  PBM_VALIDATE_SLOT_RET(slot, VOID);

  pbm_card_apps[slot] = card_apps;

  PBM_MSG_HIGH("Setting card apps for slot %d to 0x%x", slot, card_apps, 0);
}

/*===========================================================================
FUNCTION PBM_INIT_SEEK_DATA

DESCRIPTION
   Initialize the members of pbm_seek_data for the sim_id

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_init_seek_data(pbm_slot_type sim_id)
{
   int i;

   pbm_seek_data[sim_id].status = PBM_ERROR;
   pbm_seek_data[sim_id].in_use_records = NULL;
   pbm_seek_data[sim_id].num_of_records = 0;
   pbm_seek_data[sim_id].record_length = 0;
   pbm_seek_data[sim_id].num_files = 0;
   pbm_seek_data[sim_id].cur_file = 0;

   for (i = 0; i < PBM_MAX_NUM_PB_SETS; ++i)
   {
      pbm_seek_data[sim_id].records_in_pb_set[i] = 0;
      pbm_seek_data[sim_id].filenames[i] = MMGSDI_NO_FILE_ENUM;
   }
}

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
void pbm_alloc_seek_data(pbm_slot_type sim_id, int num_of_records)
{
   if (num_of_records > 0)
   {
      PBM_MEM_ALLOC(pbm_seek_data[sim_id].in_use_records, \
                    (pbm_seek_data[sim_id].num_of_records+1)*sizeof(uint8));

      /* Mark all records as used */
      if (pbm_seek_data[sim_id].in_use_records != NULL)
      {
         memset(pbm_seek_data[sim_id].in_use_records, TRUE,
                (uint32) (pbm_seek_data[sim_id].num_of_records+1)*sizeof(uint8));
      }
      else
      {
         PBM_MSG_ERR("Could not allocate memory for SEEK data", 0, 0, 0);
      }
   }
}

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
void pbm_free_seek_data(pbm_slot_type sim_id)
{
   PBM_MEM_FREEIF(pbm_seek_data[sim_id].in_use_records);
}

#ifdef FEATURE_MMGSDI_ENHANCED_SEARCH
/*===========================================================================
FUNCTION PBM_MMGSDI_SEEK_CB

DESCRIPTION
   Call back for the mmgsdi_seek(). Enqueues a PBM_GSDI_SEEK_CB command for
   PBM task to process the SEEKed data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_mmgsdi_seek_cb(mmgsdi_return_enum_type status,
                               mmgsdi_cnf_enum_type    data_type,
                               const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint16 read_cnf_data_len = 0;


   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                      pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

   mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_SEEK_CB;

   mmgsdi_resp_ptr->data_type = data_type;
   mmgsdi_resp_ptr->status = status;

   memcpy(&mmgsdi_resp_ptr->data, data_ptr, sizeof(mmgsdi_cnf_type));


   read_cnf_data_len  = (uint16)data_ptr->search_cnf.searched_record_nums.data_len;
   if(read_cnf_data_len > 0)
   {
     mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr =
                                (byte *)pbm_mem_alloc(read_cnf_data_len);
     memcpy((void *)mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr,
            (void *)data_ptr->search_cnf.searched_record_nums.data_ptr,
            read_cnf_data_len);  //deep copy
   }
   else
   {
     mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr = NULL;
   }


   pbm_gsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}

/*===========================================================================
FUNCTION PBM_GET_SEEK_PATTERN

DESCRIPTION
   This function returns the SEEK pattern to use for different files

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint8 pbm_get_seek_pattern(uint32 client_ref)
{
   pbm_file_device_e_type file_id;
   uint8 pattern;

   file_id = (pbm_file_device_e_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

   switch (file_id)
   {
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_FILE_UID:
      case PBM_FILE_PBC:
      case PBM_FILE_GRP:
      case PBM_FILE_UID1:
      case PBM_FILE_PBC1:
      case PBM_FILE_GRP1:

#ifdef FEATURE_USIM_1000_PBM
      case PBM_FILE_UID2:
      case PBM_FILE_PBC2:
      case PBM_FILE_GRP2:
      case PBM_FILE_UID3:
      case PBM_FILE_PBC3:
      case PBM_FILE_GRP3:
#endif /* FEATURE_USIM_1000_PBM */
         pattern = 0x00;
         break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

      default:
         pattern = 0xFFU;
         break;
   }

   return pattern;
}

#endif /* FEATURE_MMGSDI_ENHANCED_SEARCH */


/*===========================================================================
FUNCTION PBM_SEND_SEEK_REQUEST

DESCRIPTION
   This function sends the SEEK request to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_send_seek_request(pbm_slot_type sim_id,
                                      uint32 client_ref)
{
   pbm_return_type ret_val = PBM_ERROR;

#ifdef FEATURE_MMGSDI_ENHANCED_SEARCH
   mmgsdi_option_type dummy_option;
   mmgsdi_slot_id_enum_type slot;
   mmgsdi_access_type file_access;
   mmgsdi_seek_data_type seek_pattern;
   mmgsdi_return_enum_type seek_ret;

   memset(&dummy_option, 0, sizeof(dummy_option));

   slot = PBM_PBM_SLOT_TO_MMGSDI_SLOT(sim_id);

   seek_pattern.data_len = pbm_seek_data[sim_id].record_length;
   seek_pattern.data_ptr = PBM_MEM_ALLOC(seek_pattern.data_ptr, seek_pattern.data_len);

   if (seek_pattern.data_ptr != NULL)
   {
      /* Initialize the SEEK pattern */
      uint8 seek_byte = pbm_get_seek_pattern(client_ref);

      memset(seek_pattern.data_ptr, seek_byte, seek_pattern.data_len);

      file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
      file_access.file.file_enum = pbm_seek_data[sim_id].filenames[pbm_seek_data[sim_id].cur_file];

      if (USE_USIM(sim_id))
      {
         mmgsdi_search_offset_data_type dummy_offset;

         memset(&dummy_offset, 0, sizeof(dummy_offset));

         if ((seek_ret = mmgsdi_search(pbm_mmgsdi_client_id, slot, file_access,
                                       MMGSDI_UICC_SIMPLE_SEARCH, 1, MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                       dummy_offset, seek_pattern,
                                       dummy_option, pbm_mmgsdi_seek_cb, client_ref)) == MMGSDI_SUCCESS)
         {
            ret_val = PBM_SUCCESS;
         }
         else
         {
            PBM_MSG_ERR("mmgsdi_search returned %d", seek_ret, 0, 0);
         }
      }
      else if ((seek_ret = mmgsdi_seek(pbm_mmgsdi_client_id, slot, file_access,
                                       MMGSDI_SEEK_BEGINNING_FORWARDS, seek_pattern,
                                       dummy_option, pbm_mmgsdi_seek_cb, client_ref)) == MMGSDI_SUCCESS)
      {
         ret_val = PBM_SUCCESS;
      }
      else
      {
         PBM_MSG_ERR("mmgsdi_seek returned %d", seek_ret, 0, 0);
      }
   }

   /* Free the memory allocated for the SEEK pattern */
   PBM_MEM_FREEIF(seek_pattern.data_ptr);

#endif /* FEATURE_MMGSDI_ENHANCED_SEARCH */

   return ret_val;
}

#ifdef FEATURE_MMGSDI_ENHANCED_SEARCH
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
                 const mmgsdi_cnf_type  *data_ptr)
{
   pbm_slot_type sim_id;
   pbm_file_device_e_type file_id;
   uint32 client_ref;

   client_ref = data_ptr->response_header.client_data;

   sim_id  = (pbm_slot_type)((int) client_ref / (int) PBM_FILE_MAX_REQUESTS);
   file_id = (pbm_file_device_e_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

   if (PBM_MMGSDI_SLOT_TO_PBM_SLOT(data_ptr->response_header.slot_id) != sim_id)
   {
     PBM_MSG_ERR("Wrong callback called for sim %d, %d",
                 sim_id, PBM_MMGSDI_SLOT_TO_PBM_SLOT(data_ptr->response_header.slot_id), 0);
     return;
   }

   /* Restart initialization if required */
   if (pbm_slot_restart & PBM_SIMID_TO_SLOT_MASK(sim_id))
   {
     PBM_MSG_HIGH("PBM restarting init of SIM %d", sim_id, 0, 0);
     pbm_restart_fileinfo_internal(sim_id);
     return;
   }

   /* Stop initialization if required */
   if (!(pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(sim_id))) {
     PBM_MSG_HIGH("PBM stopping init of SIM %d", sim_id, 0, 0);
     return;
   }

   if (status == MMGSDI_SUCCESS)
   {
      if ((data_type == MMGSDI_SEARCH_CNF) &&
          (data_ptr != NULL) &&
          (pbm_seek_data[sim_id].in_use_records != NULL))
      {
         const mmgsdi_seek_data_type *seek_data;
         int i;
         int start;  /* start index for this pb set in in_use_records[] */
         int file_index = pbm_seek_data[sim_id].cur_file;

         /* Calculate the start index in in_use_records[] of this file */
         for (i = 0, start = 0; i < file_index; ++i)
         {
            start += pbm_seek_data[sim_id].records_in_pb_set[i];
         }

         seek_data = &data_ptr->search_cnf.searched_record_nums;

         for (i = 0; i < seek_data->data_len; ++i)
         {
            /* Check that the record index is in range */
            if ((seek_data->data_ptr[i] > 0) &&
                (seek_data->data_ptr[i] <= pbm_seek_data[sim_id].records_in_pb_set[file_index]) &&
                (start + seek_data->data_ptr[i] <= pbm_seek_data[sim_id].num_of_records))
            {
               pbm_seek_data[sim_id].in_use_records[start + seek_data->data_ptr[i]] = FALSE;
            }
            else
            {
               PBM_MSG_ERR("Invalid record number returned in pbm_gsdi_seek_cb", 0, 0, 0);
            }
         }

         /* See if we need to SEEK unused records for the next file in the pb set */
         if (++pbm_seek_data[sim_id].cur_file < pbm_seek_data[sim_id].num_files)
         {
            if (pbm_send_seek_request(sim_id, client_ref) == PBM_SUCCESS)
               return;

            PBM_MSG_ERR("PBM SEEK failed of PBM file: sim %d, client ref %d", sim_id, client_ref, 0);
         }

         /* The SEEK data is valid */
         pbm_seek_data[sim_id].status = PBM_SUCCESS;
      }
   }
   else
   {
      PBM_MSG_ERR("PBM SEEK callback returned %d, sim %d, file id %d", status, sim_id, file_id);
   }

   /* Start reading the records for this file. Read atleast one record (even for empty files)
      so that the cache could be set to ready in the read callback */
   if (PBM_SUCCESS == pbm_start_record_read(sim_id, file_id, 1))
   {
      return;
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[sim_id]++;

   pbm_init_entries(sim_id);
}
#endif /* FEATURE_MMGSDI_ENHANCED_SEARCH */

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
int pbm_get_next_used_record(pbm_slot_type sim_id, int prev_index)
{
   int num_records = pbm_seek_data[sim_id].num_of_records;
   int next_index = prev_index + 1;

   /* If memory allocation failed or if SEEK was unsuccessful, read all
      the records in sequence */
   if (pbm_seek_data[sim_id].in_use_records != NULL &&
       pbm_seek_data[sim_id].status == PBM_SUCCESS)
   {
      for (; next_index <= num_records; ++next_index)
      {
         if (pbm_seek_data[sim_id].in_use_records[next_index] == TRUE)
         {
            break;
         }
      }
   }

   return (next_index <= num_records) ? next_index : num_records+1;
}

#endif /* FEATURE_MMGSDI */

#endif /* FEATURE_PBM_TASK */
