/*===========================================================================

                   N V   I T E M    M A N A G E R  T A S K
DESCRIPTION
  All the declarations and definitions necessary to support the table  
  lookup of mapping between an NV item and the Flash File System files
  which simulate storage in an EEPROM.  External tasks queue requests for
  this task for NV EEPROM read and write operations.  This tasks then 
  performs the necessary internal operations to read, write
  and control access to the Flash File System files which simulate storage
  in an EEPROM.   
  This task includes the following files:
    nv.h                   -  NVM Item Manager task services
    nvim.h,     nvim.c     -  NVM Item Manager item services
    nvio.h,     nvio.c     -  NVM Item Manager read/write services
    nvi.h                  -  Common definitions

EXTERNALIZED FUNCTIONS
    nv_write_err_log
      Write an error log without requiring any tasking services.
    nv_read_err_log
      Read an error log without requiring any tasking services.
    nv_cmd
      This is the applications read and write interface to this task.
    nv_task
      This is the nv task initialization entry, started up with rex_def_task.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  This task should be started up with rex_def_task.  Caution must be
  taken when invoking error services from this task.  These services invoke
  this task again (to log errors) and could cause an infinite loop.
  

Copyright (c) 2005-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //depot/asic/msmshared/services/nv1/nvim.c#22 $ $DateTime: 2008/06/17 12:17:06 $ $Author: pratapc $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/08/08   pc      Ensured mutex initialization for synchronous calls.
12/13/07   pc      Introduced kxmutexes around NV access.
06/08/07   pc      Added support for mdn based nai
12/15/06   ck      Include the header file fs_diag_access.h
06/05/05   pc      Created NV-2.
===========================================================================*/

/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/

#include "target.h"
#include "comdef.h"
#include "AEEstd.h"
#ifdef FEATURE_NV_ITEM_MGR

#include "fs.h"
#include "nv.h"
#include "nvim.h"
#include "nvi.h"
#include "msg.h"
#include "nvio.h"
#include "err.h"
#include "task.h"
#include "fs.h"
#include "dog.h"
#include "crc.h"
#include "memory.h"
#include "fs_public.h"
#include "nv_items.h"
#include "fs_diag_access.h"

#ifdef FEATURE_CDMA1X
  #include "mc.h"
#else
  #include "tmc.h"
#endif

#ifdef FEATURE_TREX
#error code not present
#endif
#include "nvim_items.h"
#ifdef FEATURE_TREX
#error code not present
#endif

#ifdef FEATURE_NV_RUIM
#include "nvruimi.h"
#endif

#ifdef FEATURE_NV_CNV
#include "fs_public.h"
#include "nv_cnv.h"
#endif

#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
#include "prl.h"
#endif

#ifdef FEATURE_MDN_BASED_NAI
#define PS_NAI_DFLT_DOMAIN "@default.com"
#include "dsmip.h"
#endif /* FEATURE_MDN_BASED_NAI */

#ifdef FEATURE_WINCE
#error code not present
#else
  #ifdef FEATURE_QUARTZ_135
    #include "quartz/KxMutex.h"
  #else
    #include <KxMutex.h>
  #endif
#endif

#include <stdlib.h> 

 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static KxMutex nv_access_op_cs;

extern rex_crit_sect_type nv_crit_sect;
extern const nvim_item_info_type nvim_item_info_table[];

#ifdef FEATURE_NV_CNV
/* File to back up the data into */
const char   *cnv_file = "/nvm/nv_cnv.bin";
#endif

boolean  nvi_task_running=FALSE;

/* Contains the roaming list "valid" indicator */
boolean  nv_prl_valid_data[NV_MAX_NAMS];

/* Contains the roaming list "prl_version" field */
word     nv_prl_version_data[NV_MAX_NAMS];

/* Buffer holding the union of all internal NVM storage types.  Use as    */
/* a working buffer when reading or writing data from or to NVM and the   */
/* data requires translation between internal format and external format. */
nvi_item_type  nvi_item;

/* Data buffer for individual writes/reads issued internally. */  
nv_item_type  local_item;  

/* Command buffer for individual writes/reads issued internally. */
nv_cmd_type   local_cmd; 

#ifdef NV_FEATURE_PRL_ITEMS
/* Globally used buffer to contain the roaming list read-from  */ 
/* or written-to NV memory.                                    */
nv_roaming_list_type   nv_pr_list; 
#endif

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

/* Dynamic pool available in EFS */
word                   nvi_memavail_data=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

rex_timer_type         nv_rpt_timer;     /* NV watchdog report timer. */
rex_timer_type         nv_write_timer;   /* NVM write timer. */
LOCAL q_type           nv_cmd_q;         /* NV task command queue. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Flag to determine if EEPROM initialization has occurred    */
static boolean                 nvi_initialized = FALSE;

/* Flag to determine if kxmutex initialization has occured */
static boolean                 nv_cs_init_flag = FALSE;

/* The total factory data block cache for peek/poke access */
byte fact_data[NVIM_FACTORY_DATA_SIZE];

static boolean          nverr_init( void );
static void             nverr_update_log( void );


/* Structure used to essentially double buffer the error log  */
struct 
{
  nv_err_log_type  external[NV_MAX_ERR_LOG];
  nv_err_log_type  internal[NV_MAX_ERR_LOG];
  boolean          update_required[NV_MAX_ERR_LOG];
  boolean          update_in_progress[NV_MAX_ERR_LOG];
  boolean          processing_required;
} nverr_log;

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
/* Run Time R-UIM Enable control.  Default: do not use the R-UIM */
#ifdef FEATURE_OEMOMH
nv_rtre_control_type nv_rtre_control_value = NV_RTRE_CONTROL_NO_RUIM;
#else
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
nv_rtre_control_type nv_rtre_control_value = NV_RTRE_CONTROL_NO_RUIM;
#else
nv_rtre_control_type nv_rtre_control_value = NV_RTRE_CONTROL_USE_RUIM;
#endif
#endif
/* Run Time R-UIM Enable polling control.  Default:  do not poll */
nv_rtre_polling_control_type nv_rtre_polling_control_value = 
  NV_RTRE_POLLING_CONTROL_NO_POLL;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
  #ifdef FEATURE_EFS2
    #include "fs_diag.h"
  #else
    #include "fsdiag.h"
  #endif

static const char *nv_access_dirs[] = {
  "nvm"
};

#endif

#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
typedef struct _nv_auto_set_type
{
    nv_items_enum_type      nv_items_id;
    char                    nv_items_value[128];
}nv_auto_set_type;

typedef struct _nv_auto_set_table_type
{
    const nv_auto_set_type* plist;
    int                     lsit_size;
}nv_auto_set_table_type;

/*---------------------------------------------------------------------------------------
                           Please add new customer code here
          We give every customer 16 version number for update it's nv in future
---------------------------------------------------------------------------------------*/
#define AUTO_NV_CUST_CODE_INVALID           0x00

#ifdef MODEL_RF_NV_H
#include MODEL_RF_NV_H
#else
#error You must define MODEL_RF_NV_H in min file and contain corresponding header file in your build directory
#endif

#ifdef CUST_SVC_NV_H
#include CUST_SVC_NV_H
#else
#error You must define CUST_SVC_NV_H in min file and contain corresponding header file in your build directory
#endif

//Add all auto set list to table
nv_auto_set_table_type nv_auto_set_table[] = 
{
    {nv_auto_set_rf_fixed_list,     sizeof(nv_auto_set_rf_fixed_list)/sizeof(nv_auto_set_type)},
    {nv_auto_set_svc_list,          sizeof(nv_auto_set_svc_list)/sizeof(nv_auto_set_type)}
};

boolean bIsResetOemNv = FALSE;

LOCAL nv_stat_enum_type nv_auto_set_need_value(void);
#endif //FEATRUE_AUTO_SET_NEED_NV_VALUE

/*-----------------------------------------------------------------------
      Backup critical nv to flash and auto resotre when nv inactive

      Rule NO.1: RF NV can be backup only 1 time.
      Rule NO.2: ESN can backup ESN_BACKUP_TIME_MAX times.
      Rule NO.3: If RF nv or ESN is backup, Header must be written
                 to FLAG_CRITICAL_NV_WRITED
      Rule NO.4: If any problem happens, Header msut be marked as 
                 FLAG_CRITICAL_NV_INVALID
      Rule NO.5: Please MAKE SURE backup area isn't overlap the 
                 bootloader and main applicaiton
-----------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Debug Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION

/*==================================================================
Function: NV_CHECK_REMOTE_ACCESS

Description: This functions returns access permissions for 
             directories specified by input parameter "path".
==================================================================*/
static boolean
nv_check_remote_access (char *path, uint32 op_type)
{
  (void) path;
  (void) op_type;
  return FALSE;
}

/*====================================================================
Function: NV_REGISTER_REMOTE_ACCESS_CHECK

Description: Registers callback function nv_check_remote_access()
             with directory listed in nv_access_dirs. On any access to
             nv_access_dirs directories, nv_check_remote_access will 
             be called to determine access permissions.
=====================================================================*/
void
nv_register_remote_access_check (void)
{
  diag_fs_register_access_check (nv_access_dirs, 1, nv_check_remote_access);
}
#endif /* FEATURE_DIAG_FS_ACCESS_VALIDATION */


/*===========================================================================

FUNCTION NV_READ_ERR_LOG

DESCRIPTION
  This function reads an error log from NVM.

DEPENDENCIES
  The function MUST NOT USE ANY TASK SERVICES, so that it can be
  called when tasks are not initialized.  The function MUST NOT
  USE ERROR SERVICES.  It returns status to the caller if there
  is a failure.

RETURN VALUE
  NV_DONE_S         if it worked
  NV_NOT_ACTIVE_S   if item was not active
  NV_FAIL_S         if item could not be read

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nv_read_err_log (
  nv_err_log_type  *nv_err_log_ptr    /* Pointer where to return read data */
)
{
  word isave;     /* Save processor flags */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!nvi_initialized) {
    return NV_DONE_S; /* can not read errors before nv built */
  }

  /* check and reject illegal request. */
  if (nv_err_log_ptr->address >= NV_MAX_ERR_LOG) {
    return NV_FAIL_S;
  }

  /* copy the shadow copy of the error log */
  INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */
  nv_err_log_ptr->err_count = nverr_log.external[nv_err_log_ptr->address].err_count;
  std_strlcpy((char *)nv_err_log_ptr->file_name,
              (char *)nverr_log.external[nv_err_log_ptr->address].file_name,
              sizeof(nv_err_log_ptr->file_name));
  nv_err_log_ptr->line_num = nverr_log.external[nv_err_log_ptr->address].line_num;
  nv_err_log_ptr->fatal = nverr_log.external[nv_err_log_ptr->address].fatal;
  INTFREE_SAV( isave );         /* Restore interrupts (PSW) */

  return NV_DONE_S;
} /* nv_read_err_log */


/*===========================================================================

FUNCTION NV_WRITE_ERR_LOG

DESCRIPTION
  This function writes the boot error log to NVM.

DEPENDENCIES
  The function MUST NOT USE ANY TASK SERVICES, so that it can be
  called when tasks are not initialized.  The function MUST NOT
  USE ERROR SERVICES.  It returns status to the caller if there
  is a failure.

RETURN VALUE
  NV_DONE_S - if it worked
  NV_FAIL_S - if item could not be written

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nv_write_err_log(
  nv_err_log_type  *nv_err_log_ptr    /* Pointer where to get write data */
)
{
   static boolean nv_err_fataling = FALSE;
   nv_stat_enum_type status;       /* NV read status */
   word isave;
   
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if (nv_err_fataling) return NV_FAIL_S;

   if (!nvi_initialized) {
      return NV_DONE_S; /* can not write errors before nv built */
   }

   /* Check if file system is error fataling  */
   if ((rex_self() == &fs_tcb) && (nv_err_log_ptr->fatal)) {
      nv_err_fataling = TRUE; /* NV is not usable once FS err fatals */
      return NV_DONE_S; /* can not write this error */
   }
   
   /* check and reject illegal request. */

   if (nv_err_log_ptr->address >= NV_MAX_ERR_LOG) {
      return NV_FAIL_S;
   }
   /* If the task is running we queue the write to the nv part as to not
    * interrupt a write in progress. This is a somewhat device dependend
    * concept in that we assume a write can not be safely interrupted during
    * a write, which is not true of all nv devices. 
    * Note that error_fatals always take the direct route.
    */
   if (nvi_task_running && !(nv_err_log_ptr->fatal)) {
      /* put error into shadow copy of nv error record */
      INTLOCK_SAV(isave);
      nverr_log.processing_required = TRUE;
      (void) memcpy((void *)&(nverr_log.external[nv_err_log_ptr->address]), 
                    (void *)nv_err_log_ptr,
                    sizeof(nv_err_log_type));          /* data buffer size */
      nverr_log.update_required[nv_err_log_ptr->address] = TRUE;
      INTFREE_SAV(isave);

      /* Signal the nv task that there is a buffer to process */
      (void) rex_set_sigs(&nv_tcb, NV_ERR_LOG_SIG);
      return NV_DONE_S;
   }
   else {
      if ((nv_err_log_ptr->fatal)) {
         nv_err_fataling = TRUE;
         /* allow lower levels to run with ints disabled */
         nvi_task_running = FALSE; 
      }

      status = nvio_write_item
               ( NV_ERR_LOG_I, 
                 nv_err_log_ptr->address, /* Item index within its array */
                 (void *)&nv_err_log_ptr->err_count,         /* data buffer */ 
                 nvim_op_get_size(NV_ERR_LOG_I)
               );

      INTLOCK_SAV(isave);
      if (!(nv_err_log_ptr->fatal)) {
         (void) memcpy((void *)&(nverr_log.external[nv_err_log_ptr->address]), 
                       (void *)nv_err_log_ptr,
                       sizeof(nv_err_log_type));          /* data buffer size */
      }
      INTFREE_SAV(isave);

      return status;
   }
} /* nv_write_err_log */

/*===========================================================================

FUNCTION NVIM_OP_GET_PRESENCE

DESCRIPTION
  This function determines whether an entry is present.

DEPENDENCIES
  None.

RETURN VALUE
  0 : NVIM_NOT_PRESENT : Not present
  1 : NVIM_PRESENT : Present
  2 : NVIM_WRITE_ONCE : Present and write once only

SIDE EFFECTS

===========================================================================*/
boolean 
nvim_op_get_presence (
  nv_items_enum_type item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE)
  {
    return nvim_item_info_table[item].is_present;
  }
  else
  {
    ERR("Item code %d out of range", item, 0, 0);
    return 0;
  }

} /* nvim_op_get_presence */


/*===========================================================================

FUNCTION NVIM_OP_GET_ARRAY_SIZE

DESCRIPTION
  This function returns the array size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Array size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint16 
nvim_op_get_array_size (
  nv_items_enum_type   item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE) {
    return nvim_item_info_table[item].array_size;
  }
  else {
    ERR("Item %d out of range", item, 0, 0);
    return 0;
  }
} /* nvim_op_get_array_size */



/*===========================================================================

FUNCTION NVIM_OP_GET_SIZE

DESCRIPTION
  This function returns the size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Item size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint32 
nvim_op_get_size (
  nv_items_enum_type   item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE)
  {
    return (nvim_item_info_table[item].item_size);
  }
  else
  {
    ERR("Item %d out of range", item, 0, 0);
    return 0;
  }
} /* nvim_op_get_size */



/*===========================================================================

FUNCTION NV_DETERMINE_ESN_VALIDITY

DESCRIPTION
  This function is determines if the current ESN has been programmed with the
  status of this check returned.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the current ESN is non-zero with proper checksums, else FALSE

SIDE EFFECTS

===========================================================================*/

boolean 
nv_determine_esn_validity( void)
{
  /* This is gross, but there is not enough stack space
   *  for these next two items so we declare them as the appear in the enum
   */
  dword             nv_item1_esn;        /* tmp buffer for esn */
  dword             nv_item2_esn_chksum; /* tmp buffer for esn checksum */
  nv_stat_enum_type status;              /* to get status of NV operations */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by reading the ESN and returning FALSE if it is now -1        */

  status = nvio_read_item(NV_ESN_I,/* file handle */
                          0,       /* file position */
                          &nv_item1_esn,          /* data ptr */
                          sizeof(nv_item1_esn)); /* data count */

  if( (status != NV_DONE_S) ||
      (nv_item1_esn == 0xFFFFFFFF) )  {
    /* ------------------------- */
    /* The ESN has not been set, */
    /* return FALSE              */
    /* ------------------------- */
    ERR( "ESN has not been set", 0, 0, 0);
    return FALSE;
  }

  if( nv_item1_esn == 0 )  {
    MSG_HIGH("ESN is zero for factory test",0,0,0);
  }

  /* --------------------------------------------------------------------- */
  /* To get here the ESN has been set. Determine if the checksums are      */
  /* valid. Do this be comparing calculated checksums against stored sums  */
  /* --------------------------------------------------------------------- */
 
  status = nvio_read_item(NV_ESN_CHKSUM_I,/* file handle */
                          0,       /* file position */
                          &nv_item2_esn_chksum,          /* data ptr */
                          sizeof(nv_item2_esn_chksum)); /* data count */

  /* --------------------------------------------------------------------- */
  /* To get here the ESN is valid and this is not a pre-Version 5.      */
  /* Check for proper checksums and return TRUE if so, else return FALSE   */
  /* --------------------------------------------------------------------- */

  if( crc_30_calc( (byte *)&nv_item1_esn, sizeof(nv_item1_esn) * 8) !=
      nv_item2_esn_chksum)  {
    /* ------------------------- */
    /* The ESN checksum is not   */
    /* valid, return FALSE       */
    /* ------------------------- */
    ERR( "Stored ESN CHKSUM bad: %u", nv_item2_esn_chksum, 0, 0);
    return FALSE;
  } else {
    return TRUE;
  }
}


#ifdef FEATURE_NV_CNV
/*======================================================================
FUNCTION NVI_CNV_CHECK

DESCRIPTION
  This function checks for the presence of a CNV file in EFS.
  If it is found, the items are inserted into the current nv.

DEPENDENCIES
  EFS must be initialized.  The item manager must also have completed
  nv_init.  This routine expects to be called at the end of NV_INIT.

RETURN VALUE
  None

SIDE EFFECTS
  NV items could be updated if a valid CNV fileis present.

======================================================================*/
void nvim_cnv_check(void)
{
  int       file_handle = NULL;
  struct    fs_stat sbuf;
  fs_off_t  offset;
  fs_size_t size;
  word      crc16;
  static    byte buffer[32];
  uint32    count;
  uint32    size_left;
  uint8     pad_size;


  cnv_item_hdr_type item_head;
  static nv_item_type      item;
  nv_cmd_type       cmd;

  /* Open the file */
  file_handle = efs_open(cnv_file, O_RDWR);
  if (file_handle < 0)
  {
    MSG_HIGH ("Cannot open CNV file", 0, 0, 0);
    return;
  }

  /* Stat to get the file size */
  if (efs_fstat(file_handle, &sbuf) < 0)
  {
    MSG_HIGH ("Error on fstat of CNV file",0,0,0);
    return;
  }

  /* Cycle thru the contents to check CRC */
  crc16 = (word) CRC_16_STEP_SEED;

  size_left = sbuf.st_size;
  while (size_left > 0) {
    count = size_left;
    if (count > sizeof (buffer))
      count = sizeof (buffer);

    size = efs_read(file_handle, buffer, count);
    if (size != count)
    {
      MSG_MED ("Unable to read CNV data", 0, 0, 0);
      return;
    }

    crc16 = crc_16_step (crc16, buffer, count);
    size_left -= count;
  }

  if (crc16 != CRC_16_OK) {
    MSG_HIGH ("Invalid CRC for CNV data", 0, 0, 0);
    return;
  }

  MSG_MED ("Valid CNV file detected", 0, 0, 0);

  /* Rewind the file back to the first item and ignore the header
     for now */
  offset = efs_lseek (file_handle, sizeof(cnv_header_type), SEEK_SET);

  if (offset < 0)
  {
    MSG_HIGH ("Cannot seek to the beginning of the file",0,0,0);
    return;
  }

  size_left = sbuf.st_size - (sizeof(cnv_header_type) + sizeof(crc16)
                                          + sizeof(cnv_item_hdr_type));

  while (size_left > 0)
  {
    /* Read in an item. */
    size = efs_read (file_handle, (void *) &item_head, sizeof (item_head));
    if (size != sizeof (item_head))
    {
      MSG_MED ("Unable to read handover item header", 0, 0, 0);
      return;
    }

    size_left -= sizeof (item_head);

    MSG_MED ("CNV item: %d, size %d", item_head.nv_item_number,
               item_head.nv_item_size, 0);

    /* Read in the raw data. */
    size = efs_read (file_handle, (void *) &item, item_head.nv_item_size);
    if (size != item_head.nv_item_size)
    {
      MSG_MED ("Unable to read CNV item", 0, 0, 0);
      return;
    }

    /* Write the item to NV. */
    cmd.item     = (nv_items_enum_type) item_head.nv_item_number;
    cmd.cmd      = NV_WRITE_F;
    cmd.data_ptr = &item;

    if (nvio_write (&cmd) != NV_DONE_S)
    {
      MSG_HIGH ("Unable to write handover item: %d, size %d",
                 item_head.nv_item_number, item_head.nv_item_size, 0);
    }

    pad_size = 0;
    if ((item_head.nv_item_size % 4) != 0)
    {
      pad_size = 4 - (item_head.nv_item_size % 4);

      offset = efs_lseek(file_handle, pad_size, SEEK_CUR);
      if (offset < 0)
      {
        MSG_HIGH ("Cannot seek forward to the next item",0,0,0);
        return;
      }
    }
    size_left -= (item_head.nv_item_size + pad_size);
  }

  /* Close the file */
  efs_close (file_handle);

  /* Remove the file since we are done updating the NV items */
  if (efs_unlink (cnv_file) < 0)
  {
    MSG_HIGH ("Unable to remove CNV file", 0, 0, 0);
  }
} /* nvi_cnv_check */

#endif /*FEATURE_NV_CNV*/


/*===========================================================================

FUNCTION NVIM_BUILD_SEC

DESCRIPTION
  This function builds all NVM variables.

DEPENDENCIES

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if a variable could not be initialized.

SIDE EFFECTS

===========================================================================*/

static nv_stat_enum_type 
nvim_build_sec(void)
{
   nv_stat_enum_type  status;    /* Status to return to calling procedure */
   word               cnt;       /* Counter for attempts to write variable */
   
   /* Initialize the lock code to the default value (0000)   */
   for( cnt=0; cnt<NV_LOCK_CODE_SIZE; cnt++)  
   {
     local_item.lock_code.digits[ cnt] = '0';
   }
   
   local_cmd.item       = NV_LOCK_CODE_I;
   local_cmd.tcb_ptr    = NULL;
   local_cmd.sigs       = 0;
   local_cmd.done_q_ptr = NULL;
   local_cmd.cmd        = NV_WRITE_F;
   local_cmd.data_ptr   = &local_item;
   status = nvio_write( &local_cmd);
   if(status != NV_DONE_S)
   {
      return status;
   }
   
   /* Initialize the sec code to the default value (0000)   */
   for( cnt=0; cnt<NV_SEC_CODE_SIZE; cnt++)  {
     local_item.sec_code.digits[ cnt] = '0';
   }
   local_cmd.item       = NV_SEC_CODE_I;
   local_cmd.tcb_ptr    = NULL;
   local_cmd.sigs       = 0;
   local_cmd.done_q_ptr = NULL;
   local_cmd.cmd        = NV_WRITE_F;
   local_cmd.data_ptr   = &local_item;
   status = nvio_write( &local_cmd);

   return status;
}


/*===========================================================================

FUNCTION NVIM_CLEAR_CRCS

DESCRIPTION
  This function is called to clear CRCs in NV when the NV is built. The function 
  clears the NAM checksums, the MIN checksums within each NAM, and, if 
  the ESN and IMEI have not already been written (assigned), the ESN, the IMEI,
  the ESN checksum, the IMEI checksum.  This action lets higher level tasks 
  know that these parameters are not valid and that service programming is 
  required.

DEPENDENCIES
  NVM must have been built.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  NV parameters identified above are zeroed.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_clear_crcs (void)
{
  byte                nam_idx;       /* NAM counter */
  byte                min_idx;       /* MIN counter */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Setup command buffer parameters. */

  local_cmd.cmd = NV_WRITE_F;
  local_cmd.tcb_ptr = NULL;
  local_cmd.sigs = 0;
  local_cmd.done_q_ptr = NULL;
  local_cmd.data_ptr = &local_item;

  /* Clear all NAMs checksums and the MIN checksums for each NAM. */
  
  for (nam_idx = 0; nam_idx < NV_MAX_NAMS; nam_idx++) { 
    local_item.nam_chksum.nam = nam_idx;
    local_item.nam_chksum.chksum = 0;
    local_cmd.item = NV_NAM_CHKSUM_I;
    local_cmd.status = nvio_write(&local_cmd);

    MSG_LOW("NAM %d NAM checksum zero status %d", nam_idx, local_cmd.status, 0);
    if (local_cmd.status != NV_DONE_S) {
      return (local_cmd.status);
    }
 
    local_item.min_chksum.nam = nam_idx;
    for (min_idx = 0; min_idx < NV_MAX_MINS; min_idx++) {
      local_item.min_chksum.chksum[min_idx] = 0;
    }
    local_cmd.item = NV_MIN_CHKSUM_I;
    local_cmd.status = nvio_write(&local_cmd);

    MSG_LOW("NAM %d MIN checksum zero status %d", nam_idx, local_cmd.status, 0);
    if (local_cmd.status != NV_DONE_S) {
      return (local_cmd.status);
    }
  } /* for */

  /* --------------------------------------------------------------------- */
  /* Next we determine if the ESN is now proper and if not then clear the  */
  /* ESN,the ESN checksum, and the ESN valid flag.                         */
  /* --------------------------------------------------------------------- */
  if( nv_determine_esn_validity() == FALSE) {
    /* ------------------------------------------------------------------- */
    /* The ESN has not yet been assigned/written. Zero associated items    */
    /* Note that to get here status == NV_DONE_S                           */
    /* ------------------------------------------------------------------- */
    local_item.esn.esn = 0;
    local_cmd.item = NV_ESN_I;
    local_cmd.status = nvio_write(&local_cmd);
    
    MSG_LOW("ESN zero status %d", local_cmd.status, 0, 0);
    if (local_cmd.status != NV_DONE_S) {
      return local_cmd.status;
    }
  }
  return local_cmd.status;
} /* nvim_clear_crcs */


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_TIMER

DESCRIPTION
  This function reads the rental timer item structure out of NVRAM and
  initializes the "nv_rental_item_image" state data item.  It also
  validates the data structure and recovers from a previous bad powerdown.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_timer (void) 
{

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif
  return NV_DONE_S;
}


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_CNT

DESCRIPTION
  This function reads the rental count item from NV and initializes 
  the "nv_rental_cnt_image" state data item.  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_cnt (void) {
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#else
  return NV_DONE_S;
#endif
}


/*===========================================================================

FUNCTION NVIM_INIT_PRL_DATA

DESCRIPTION
  This function initializes the "prl_valid_data" and "prl_version_data"
  state data items.  These are maintained as NV state data so the functions
  "nv_prl_is_valid()" and "nvim_prl_version()" can return an immediate 
  response.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void
nvim_init_prl_data (void) 
{
  byte                         nam;
  nv_roaming_list_header_type  prl_header;
  nv_stat_enum_type            status;

  prl_header.nam =  FALSE;
  prl_header.prl_version = 0;
  prl_header.valid = FALSE;

  for (nam=0; nam<NV_MAX_NAMS; nam++) {
    /* For each nam, read the prl header */
    status = nvio_read_prl_item(nam,
                                &prl_header,               /* data ptr */
                                NV_ROAMING_LIST_HEADER_SIZE);/* data count */
    /* Set state data items -- note that the item's active flag */
    /* occupies the "nam" field in the external type.           */

	
    if ((status == NV_DONE_S) && ((boolean)prl_header.nam == TRUE)) {
      nv_prl_version_data[nam] = prl_header.prl_version;
      nv_prl_valid_data[nam] = prl_header.valid;
    }
    else {
      nv_prl_version_data[nam] = NV_PRL_VERSION_INVALID;
      nv_prl_valid_data[nam] = FALSE;
    }


  }
  return;
}


/*===========================================================================

FUNCTION NVIM_REMOVE_ITEM

DESCRIPTION
  This procedure processes NVM remove requests for a particular item.

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_item (
  nv_cmd_type  *cmd_ptr           /* Pointer to Command block */
)
{
  char file_name[20];
  nv_stat_enum_type status;

  if ((cmd_ptr->item != NV_SMS_I) && (cmd_ptr->item != NV_SMS_GW_I)) 
    return NV_BADPARM_S;

#ifdef FEATURE_NV_RUIM
  if ( nvruim_free(cmd_ptr, &status) == NV_RUIM_SUPPORTS_ITEM )
      return status;
#endif
  status = NV_DONE_S;

  if (cmd_ptr->item >= NV_MAX_I) {
   status = NV_BADPARM_S;
  }
  else {
    switch(cmd_ptr->item) {
      case NV_SMS_I:
      case NV_SMS_DM_I:
        if(cmd_ptr->data_ptr->sms.address >= NVI_MAX_SMS_ADDR)
          status = NV_BADPARM_S;
        else{
          (void) snprintf(file_name, sizeof(file_name), "/nvm/sms_%05d",
                                            cmd_ptr->data_ptr->sms.address);
          efs_unlink(file_name);
        }
        break;
      case NV_SMS_GW_I:
        if(cmd_ptr->data_ptr->sms_gw.address >= NVI_MAX_SMS_ADDR) 
          status = NV_BADPARM_S;
        else {
          (void) snprintf(file_name, sizeof(file_name), "/nvm/sms_gw_%05d",
                                          cmd_ptr->data_ptr->sms_gw.address);
          efs_unlink(file_name);
        }
        break;
        /* Creating file name for array item*/
    }/*switch*/
  }/* else */
  return status;
}/*nvim_remove_item*/


/*===========================================================================

FUNCTION NVIM_REMOVE_ALL

DESCRIPTION
  This removes all files under the NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_all ( void )
{
  struct fs_dirent         *dirent;
  EFSDIR *dir;

  dir = efs_opendir("nvm/sms");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/sms");

  dir = efs_opendir("nvm/prl");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/prl");

  dir = efs_opendir("nvm/nvm");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/nvm");

  dir = efs_opendir("nvm");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);

  return NV_DONE_S;
}/*nvim_remove_all*/


/*===========================================================================

FUNCTION NVIM_MIGRATION_NEEDED

DESCRIPTION
  This function will decide if there is a need to unlink the nvm directory 
  during a transition of older NV services to NV-2

DEPENDENCIES
  None.

RETURN VALUE
  0 if migration is not required
  1 if migratuion is required
  
SIDE EFFECTS
  None.

===========================================================================*/

LOCAL int 
nvim_migration_needed(void) 
{
  struct fs_stat temp_buf;
  /* Make sure that the change to efs_get is made so that it returns 
  correct value */
  if(efs_stat("nvm/num/0",&temp_buf) == -1)
    return 1;
  else 
    return 0;
}


/*===========================================================================

FUNCTION NV_INIT

DESCRIPTION
  This function checks the major version number of the the NV task
  as stored at edit time, against the major version number of the
  NV EEPROM as read from NVM.  If the numbers are different then the
  function completely rebuilds the NV EEPROM.  If the major version
  number has not changed then the function checks if the number of
  items defined in NVI.H has increased from the internal NVM stored
  items count.  If so then only those new items are built.  Once the
  NV EEPROM has been initialized the function updates the stored
  major version number to the one contained in NVI.H.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  Rebuilding the NVM takes a significant amount of time, measured in
  seconds on serial eeprom devices, if NVM is rebuilt. Startup time 
  will vary with the type of EEPROM.

===========================================================================*/

LOCAL nv_stat_enum_type  
nv_init (void)
{
  nv_stat_enum_type  status;          /* Status to return to caller */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!nv_cs_init_flag)
    nv_access_op_cs_init();

  NV_GLOBAL_LOCK_INIT();
  if (nvim_migration_needed()) {
    status = nvim_remove_all();
    if ((status = nvim_build_sec()) == NV_DONE_S) {
      status = nvim_clear_crcs();
    }
  } 
  else {
    status = NV_DONE_S;
  }

#ifdef CUST_EDITION
#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
    status = nv_auto_set_need_value();
    if(NV_DONE_S != status)
    {
        return status;
    }
#endif   /*FEATRUE_AUTO_SET_NEED_NV_VALUE*/
#endif /*CUST_EDITION*/


#ifdef FEATURE_NV_CNV
  /* If the file was dropped in as part of a CEFS image, then restore
     the NV items */
  nvim_cnv_check();

  /* Remove the file since we are done updating the NV items and just
     in case the file is left hanging around because of error conditions */
  if (efs_unlink (cnv_file) < 0) {
    MSG_HIGH ("Unable to remove CNV file", 0, 0, 0);
  }
#endif

  (void)nverr_init();      /* cannot init err logging until nv is built */
  (void)nvim_init_rental_timer();  /* validate the rental timer item */
  (void)nvim_init_rental_cnt();    /* validate the rental count item */

#if !defined( FEATURE_NV_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  nvim_init_prl_data();      /* initialize prl state data items */
#endif /* FEATURE_RUIM */

#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
  nv_register_remote_access_check();
#endif

  return status;
} /* nv_init */

#ifdef FEATURE_MDN_BASED_NAI

/*===========================================================================

FUNCTION NV_BCD_TO_CHAR

DESCRIPTION
  Functions to convert MDN (BCD form) into ASCII characters.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the characters for the numeric values between 1 - 9.
  Returns 0 for 10, * for 11 and # for 12.

SIDE EFFECTS
  None.

===========================================================================*/
char nv_bcd_to_char
(
  word num_to_convert
)
{
  switch(num_to_convert)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      return '0' + num_to_convert;
    case 10:
      return '0';
    case 11:
      return '*';
    case 12:
      return '#';
    default:
      MSG_ERROR ("Invalid BCD digit (%d)", num_to_convert, 0, 0);
      return ' ';
  }
} /* nv_bcd_to_char */


/*===========================================================================

FUNCTION NV_NAI_FROM_MDN

DESCRIPTION
  Functions form NAI based on given MDN and domain .

DEPENDENCIES
  None.

RETURN VALUE
  TRUE    if it worked
  FALSE   Failure

SIDE EFFECTS
  None.

===========================================================================*/

boolean nv_nai_from_mdn
(
  char *                  nai_to_write,
  word *                  len,   //  len should be set to MAX_NAI_LENGTH
  nv_mob_dir_data_type *  mob_dir,
  char *                  domain  // must be null terminated
)
{
  int  i, cpylen;

  ASSERT (nai_to_write);
  ASSERT (len);
  ASSERT (mob_dir);
  ASSERT (domain);

  if (*len < mob_dir->n_digits + 1)
  {
    MSG_ERROR ("Output nai buffer too small for MDN",0,0,0);
    return FALSE;
  }

  for (i = 0; i < mob_dir->n_digits; i++)
  {
    nai_to_write[i] = nv_bcd_to_char(mob_dir->digitn[i]);
    if (nai_to_write[i] == ' ')
    {
      MSG_ERROR ("Invalid MDN when converting to NAI",0,0,0);
      return FALSE;  // item_to_be_written = FALSE;
    }
  }

  nai_to_write[i] = 0;
 
  cpylen = strlen(domain);
  if (cpylen > *len - i - 1)
  {
    MSG_ERROR ("Output nai buffer too small for domain",0,0,0);
    return FALSE;  // item_to_be_written = FALSE;
  }

  std_strlcat(&(nai_to_write[i]), domain, *len - i - 1);
  nai_to_write[i+cpylen] = 0;  // NULL terminate
  *len = i + cpylen;
  return TRUE;
} /* nv_nai_from_mdn */

/*===========================================================================

FUNCTION NV_GET_DFLT_MIP_PROFILE

DESCRIPTION
  Sets the current profile with the parameters for the default MIP profile.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void nv_get_dflt_mip_profile
(
  nv_ds_mip_gen_user_prof_type * prof
)
{
  ASSERT (prof);

  prof->index             = MIP_DLFT_PROFILE_NUM;
  prof->nai_length        = 0;
  prof->nai[0]            = 0;
  prof->mn_ha_spi_set     = TRUE;
  prof->mn_ha_spi         = MIP_MD5_SPI;
  prof->mn_aaa_spi_set    = TRUE;
  prof->mn_aaa_spi        = MIP_CHAP_SPI;
  prof->rev_tun_pref      = FALSE;
  prof->home_addr         = DYNAMIC_HOME_ADDR;
  prof->primary_ha_addr   = DYNAMIC_HA_ADDR;
  prof->secondary_ha_addr = UNSET_HA_ADDR;
} /* nv_get_dflt_mip_profile */
#endif /* FEATURE_MDN_BASED_NAI */



/*===========================================================================

FUNCTION NV_OTASP_COMMIT

DESCRIPTION
  This function performs the OTASP "commit" operation, which updates
  a number of OTASP-related nv items at once.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  Others       Failure for internal call

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nv_otasp_commit (
  nv_cmd_type  *cmd_ptr          /* Command block */
)
{
#ifdef FEATURE_NV_OTASP

/* Define constants for the switch statement below */
#define NV_COMMIT_DIR_NUM                      0
#define NV_COMMIT_IMSI_ADDR                    1
#define NV_COMMIT_IMSI_MCC                     2
#define NV_COMMIT_IMSI_11_12                   3
#define NV_COMMIT_MIN1                         4
#define NV_COMMIT_MIN2                         5
#define NV_COMMIT_ACCOLC                       6
#define NV_COMMIT_MOB_HOME                     7
#define NV_COMMIT_MOB_SID                      8
#define NV_COMMIT_MOB_NID                      9
#define NV_COMMIT_SID_NID                     10
#define NV_COMMIT_FIRSTCHP                    11
#define NV_COMMIT_HOME_SID                    12
#define NV_COMMIT_ROAM_LIST                   13
#define NV_COMMIT_SPC                         14
#define NV_COMMIT_IMSI_T_S1                   15
#define NV_COMMIT_IMSI_T_S2                   16
#define NV_COMMIT_IMSI_T_11_12                17
#define NV_COMMIT_IMSI_T_MCC                  18
#define NV_COMMIT_IMSI_T_ADDR_NUM             19
#define NV_COMMIT_NAM_LOCK                    20
#define NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN   21
#define NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN   22
#define NV_COMMIT_AN_NAI_AFTER_MDN            23

#define NV_COMMIT_DONE            255

  word         nv_task_pri;        /* REX priority of the NV task */
  boolean      item_to_be_written; /* Flag controls whether write occurs */
  byte         i, commit_state=0;  /* Index variables */

#ifdef FEATURE_MDN_BASED_NAI

/* use the largest of the source nai fields (MIP, SIP, AN) */
#define MAX_NAI_LENGTH NV_MAX_PPP_USER_ID_LENGTH

  char    nai_to_write[MAX_NAI_LENGTH]; // max for MIP, check SIP
  char    domain[MAX_NAI_LENGTH + 1];
  char *  domain_ptr;
  word    nai_length;
#endif /* FEATURE_MDN_BASED_NAI */

  MSG_HIGH ("NV Starting OTASP commit", 0, 0, 0);

  /* Raise NV task priority for OTASP commit operation (and save */
  /* original priority so it can be restored at end of commit)   */
  nv_task_pri = rex_set_pri(SRCH_PRI+3);

  MSG_HIGH ("NV Priority raised for OTASP", 0, 0, 0);
  /* Set up generic command buffer parameters */
  local_cmd.cmd         = NV_WRITE_F;
  local_cmd.tcb_ptr     = NULL;
  local_cmd.sigs        = 0;
  local_cmd.done_q_ptr  = NULL;
  local_cmd.data_ptr    = &local_item;
  local_cmd.status      = NV_DONE_S;

  while (commit_state != NV_COMMIT_DONE) { 

    item_to_be_written = FALSE;
    switch (commit_state) {
  
      case NV_COMMIT_DIR_NUM:
    
        /* Read control flag to determine if item should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_mob_dir) {

          /* Write NV_DIR_NUMBER_PCS_I */
          item_to_be_written = TRUE;
          local_item.mob_dir_number.nam = 
              cmd_ptr->data_ptr->otasp_commit->nam;
          local_item.mob_dir_number.n_digits = 
              cmd_ptr->data_ptr->otasp_commit->mob_dir.n_digits;
          for (i=0; i < NV_DIR_NUMB_PCS_SIZ; ++i) {
            local_item.mob_dir_number.digitn[i] = 
                cmd_ptr->data_ptr->otasp_commit->mob_dir.digitn[i];
          }      
          local_cmd.item = NV_DIR_NUMBER_PCS_I;
        }

        /* Set next state */
#ifdef FEATURE_MDN_BASED_NAI
        if (item_to_be_written) 
        {
          commit_state = NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN;
          break;
        }
#endif /* FEATURE_MDN_BASED_NAI */
        commit_state = NV_COMMIT_IMSI_ADDR;
        break;

      case NV_COMMIT_IMSI_ADDR:

        /* Read control flag to determine if items should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_ftc_cdma) {
        
          /* Write NV_IMSI_ADDR_NUM_I */
          item_to_be_written = TRUE;
          local_item.imsi_addr_num.nam = 
              cmd_ptr->data_ptr->otasp_commit->nam;
          local_item.imsi_addr_num.num = 
              cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_addr_num;
          local_cmd.item = NV_IMSI_ADDR_NUM_I;
          /* Set commit_state to write the additional items of this group */
          commit_state = NV_COMMIT_IMSI_MCC;
        }
        else {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_FIRSTCHP;
        }
        break;

      case NV_COMMIT_IMSI_MCC:

        /* Write NV_IMSI_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_mcc.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_mcc.imsi_mcc = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.mcc;
        local_cmd.item = NV_IMSI_MCC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_11_12:

        /* Write NV_IMSI_11_12_I */
        item_to_be_written = TRUE;
        local_item.imsi_11_12.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_11_12.imsi_11_12 = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_11_12;
        local_cmd.item = NV_IMSI_11_12_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MIN1:

        /* Write NV_MIN1_I */
        item_to_be_written = TRUE;
        local_item.min1.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.min1.min1[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_s1;
        /* Set both the CDMA min and the analog min to this same value */
        local_item.min1.min1[NV_ANALOG_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_s1;
        local_cmd.item = NV_MIN1_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MIN2:

        /* Write NV_MIN2_I */
        item_to_be_written = TRUE;
        local_item.min2.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.min2.min2[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_s2;
        /* Set both the CDMA min and the analog min to this same value */
        local_item.min2.min2[NV_ANALOG_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.imsi_s2;
        local_cmd.item = NV_MIN2_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_ACCOLC:

        /* Write NV_ACCOLC_I */
        item_to_be_written = TRUE;
        local_item.accolc.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.accolc;
        local_cmd.item = NV_ACCOLC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_HOME:

        /* Write NV_MOB_TERM_HOME_I */
        item_to_be_written = TRUE;
        local_item.mob_term_home.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.mob_term_home.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.mob_term_home;
        local_cmd.item = NV_MOB_TERM_HOME_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_SID:

        /* Write NV_MOB_TERM_FOR_SID_I */
        item_to_be_written = TRUE;
        local_item.mob_term_for_sid.nam = 
            cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.mob_term_for_sid;
        local_cmd.item = NV_MOB_TERM_FOR_SID_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_NID:

        /* Write NV_MOB_TERM_FOR_NID_I */
        item_to_be_written = TRUE;
        local_item.mob_term_for_nid.nam = 
            cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->ftc_cdma.mob_term_for_nid;
        local_cmd.item = NV_MOB_TERM_FOR_NID_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_SID_NID:

        /* Write NV_SID_NID_I */
        item_to_be_written = TRUE;
        /* All the targets which have SSPR Enhancements use NV item HOME SID-NID */
#if (defined (NV_FEATURE_SSPR_ENHANCEMENTS) || defined (NV_FEATURE_TRIMODE_ITEMS))
        local_item.home_sid_nid.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        for(i=0;i<NV_MAX_HOME_SID_NID;i++)
        {
          local_item.home_sid_nid.pair[i].sid = 
              cmd_ptr->data_ptr->otasp_commit->ftc_cdma.pair[i].sid;
          local_item.home_sid_nid.pair[i].nid = 
              cmd_ptr->data_ptr->otasp_commit->ftc_cdma.pair[i].nid;
        } 
        local_cmd.item = NV_HOME_SID_NID_I;
#else
        local_item.sid_nid.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        for(i=0;i<NV_MAX_SID_NID;i++)
        {
          local_item.sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid = 
              cmd_ptr->data_ptr->otasp_commit->ftc_cdma.pair[i].sid;
          local_item.sid_nid.pair[NV_CDMA_MIN_INDEX][i].nid = 
              cmd_ptr->data_ptr->otasp_commit->ftc_cdma.pair[i].nid;
        }
        local_cmd.item = NV_SID_NID_I;
#endif
        /* Set next state */
        commit_state = NV_COMMIT_FIRSTCHP;
        break;

      case NV_COMMIT_FIRSTCHP:

        /* Read control flag to determine if items should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_ftc_amps) {

          /* Write NV_ANALOG_FIRSTCHP_I */
          item_to_be_written = TRUE;
          local_item.analog_firstchp.nam = 
              cmd_ptr->data_ptr->otasp_commit->nam;
          local_item.analog_firstchp.channel = 
              cmd_ptr->data_ptr->otasp_commit->ftc_amps.firstchp;
          local_cmd.item = NV_ANALOG_FIRSTCHP_I;
          /* Set commit_state to write the next item of this group */
          commit_state = NV_COMMIT_HOME_SID;
        }
        else {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_SPC;
        }
        break;

      case NV_COMMIT_HOME_SID:

        /* Write NV_ANALOG_HOME_SID_I */
        item_to_be_written = TRUE;
        local_item.analog_home_sid.nam = 
            cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.analog_home_sid.sid = 
            cmd_ptr->data_ptr->otasp_commit->ftc_amps.home_sid;
        local_cmd.item = NV_ANALOG_HOME_SID_I;
        /* Set next state */
        commit_state = NV_COMMIT_SPC;
        break;

      case NV_COMMIT_SPC:

        /* Read control flag to determine if item should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_spc) {

          /* Write NV_SEC_CODE_I */
          item_to_be_written = TRUE;
          local_item.sec_code = cmd_ptr->data_ptr->otasp_commit->spc;
          local_cmd.item = NV_SEC_CODE_I;
        }
        /* Set next state */
        commit_state = NV_COMMIT_IMSI_T_S1;
        break;

      case NV_COMMIT_IMSI_T_S1:

        /* Read control flag to determine if items should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_imsi_t) {

          /* Write NV_IMSI_T_S1 */
          item_to_be_written = TRUE;
          local_item.imsi_t_s1.nam = cmd_ptr->data_ptr->otasp_commit->nam;
          local_item.imsi_t_s1.min1[NV_CDMA_MIN_INDEX] = 
            cmd_ptr->data_ptr->otasp_commit->imsi_t.s1;
          local_cmd.item = NV_IMSI_T_S1_I;
          /* Set commit_state to write the next item of this group */
          commit_state++;
        }
        else
        {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_ROAM_LIST;
        }
        break;

      case NV_COMMIT_IMSI_T_S2:

        /* Write NV_IMSI_T_S2_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_s2.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_t_s2.min2[NV_CDMA_MIN_INDEX] = 
          cmd_ptr->data_ptr->otasp_commit->imsi_t.s2;
        local_cmd.item = NV_IMSI_T_S2_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_T_11_12:

        /* Write NV_IMSI_T_11_12_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_11_12.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_t_11_12.imsi_11_12 = 
          cmd_ptr->data_ptr->otasp_commit->imsi_t.imsi_t_11_12;
        local_cmd.item = NV_IMSI_T_11_12_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;
      
      case NV_COMMIT_IMSI_T_MCC:

        /* Write NV_IMSI_T_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_mcc.nam = cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_t_mcc.imsi_mcc = 
          cmd_ptr->data_ptr->otasp_commit->imsi_t.mcc;
        local_cmd.item = NV_IMSI_T_MCC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_T_ADDR_NUM:

        /* Write NV_IMSI_T_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_addr_num.nam = 
          cmd_ptr->data_ptr->otasp_commit->nam;
        local_item.imsi_t_addr_num.num = 
          cmd_ptr->data_ptr->otasp_commit->imsi_t.addr_num;
        local_cmd.item = NV_IMSI_T_ADDR_NUM_I;
        /* Set next state */
        commit_state = NV_COMMIT_ROAM_LIST;
        break;

      case NV_COMMIT_ROAM_LIST:

        /* Commit to the Roaming List MUST be the last commit to be performed  */
        /* This is, because the local_cmd.data_ptr gets assigned to pointer to */
        /* nv_pr_list during this sequence of operations. If one must add any  */
        /* other NV item in the commit switch after Roaming List at all, then  */
        /* (s)he should make sure to reassign this to local_item               */ 

        /* Read control flag to determine if item should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_roam_list) {

          /* Write NV_ROAMING_LIST_I */
          item_to_be_written = TRUE;
          local_cmd.data_ptr = (nv_item_type *)&nv_pr_list;
          #ifdef NV_FEATURE_IS683A_PRL
            local_cmd.item = NV_ROAMING_LIST_683_I;
          #else
            local_cmd.item = NV_ROAMING_LIST_I;
          #endif
        }
        /* Set next state */
        commit_state = NV_COMMIT_NAM_LOCK;
        break;

      case NV_COMMIT_NAM_LOCK:
        /* Read control flag to determine if items should be written */
        if (cmd_ptr->data_ptr->otasp_commit->commit_nam_lock) 
        {
        #ifdef FEATURE_OTASP_OTAPA
          /* Write NV_NAM_LOCK_I */
          item_to_be_written = TRUE;
          local_item.nam_lock.nam = cmd_ptr->data_ptr->otasp_commit->nam;
          local_item.nam_lock.enabled = 
            cmd_ptr->data_ptr->otasp_commit->nam_lock.enabled;
          local_cmd.item = NV_NAM_LOCK_I;
        #else
          ERR("Attempted to write NV_NAM_LOCK_I when FEATURE_OTASP_OTAPA not on",
              0, 0, 0);
        #endif
        }
        /* Set commit_state to signify we've finished committing */
        commit_state = NV_COMMIT_DONE;
      
        break;

#ifdef FEATURE_MDN_BASED_NAI
      case NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN:

        /*-------------------------------------------------------------------
          Note: this defaults to read/write profile at index 0.
                Customer should use whichever index they assign
                to the active MIP profile.
        -------------------------------------------------------------------*/
        local_item.ds_mip_gen_user_prof.index = MIP_DLFT_PROFILE_NUM;
        local_cmd.cmd                         = NV_READ_F;
        local_cmd.item                        = NV_DS_MIP_GEN_USER_PROF_I;
        local_cmd.status                      = nvio_read(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          nv_get_dflt_mip_profile( &(local_item.ds_mip_gen_user_prof) );
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          MSG_ERROR ("Error %d reading NV",local_cmd.status,0,0);
          commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
          break;
        }

        if(local_item.ds_mip_gen_user_prof.nai_length < NV_MAX_NAI_LENGTH)
          local_item.ds_mip_gen_user_prof.nai[
            local_item.ds_mip_gen_user_prof.nai_length] = 0;
        domain_ptr =
          std_strchr( (char*) local_item.ds_mip_gen_user_prof.nai, '@' );
        if (domain_ptr == NULL)
        {
          MSG_HIGH("Using default domain",0,0,0);
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        std_strlcpy(domain,domain_ptr,NV_MAX_NAI_LENGTH);
        domain[NV_MAX_NAI_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_NAI_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ptr->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd = NV_WRITE_F;
        local_item.ds_mip_gen_user_prof.nai_length = nai_length;
        memcpy( (char*) local_item.ds_mip_gen_user_prof.nai,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
        break;

      case NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN:

        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_PPP_USER_ID_I;
        local_cmd.status = nvio_read(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          MSG_ERROR ("Error %d reading NV",local_cmd.status,0,0);
          commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.ppp_user_id.user_id[local_item.ppp_user_id.user_id_len] = 0;
        domain_ptr = std_strchr( (char*) local_item.ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          MSG_HIGH("Using default domain for SIP nai",0,0,0);
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        std_strlcpy(domain,domain_ptr,NV_MAX_PPP_USER_ID_LENGTH);
        domain[NV_MAX_PPP_USER_ID_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ptr->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_PPP_USER_ID_I;
        local_item.ppp_user_id.user_id_len = nai_length;
        memcpy( (char*) local_item.ppp_user_id.user_id,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
        break;

      case NV_COMMIT_AN_NAI_AFTER_MDN:
        
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_HDR_AN_AUTH_USER_ID_LONG_I;
        local_cmd.status = nvio_read(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.hdr_an_ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          MSG_ERROR ("Error %d reading NV",local_cmd.status,0,0);
          commit_state = NV_COMMIT_IMSI_ADDR;
          break;
        }
 
        // copy the old domain from the user profile, or the default domain
        local_item.hdr_an_ppp_user_id.user_id[
          local_item.hdr_an_ppp_user_id.user_id_len] = 0;
        domain_ptr =
          std_strchr( (char*) local_item.hdr_an_ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          MSG_HIGH("Using default domain for AN nai",0,0,0);
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        std_strlcpy(domain,domain_ptr,NV_MAX_AN_PPP_USER_ID_LENGTH);
        domain[NV_MAX_AN_PPP_USER_ID_LENGTH] = 0;  // NULL terminate
 
        nai_length = NV_MAX_AN_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ptr->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_IMSI_ADDR;
          break;
        }
 
        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_HDR_AN_AUTH_USER_ID_LONG_I;
        local_item.hdr_an_ppp_user_id.user_id_len = nai_length;
        memcpy( (char*) local_item.hdr_an_ppp_user_id.user_id,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_IMSI_ADDR;
        break;
#endif /* FEATURE_MDN_BASED_NAI */ 


      default:
        /* Restore original NV task priority before returning */
        (void)rex_set_pri(nv_task_pri);
 
        MSG_HIGH ("NV Priority Returned to NORMAL", 0, 0, 0);
        return(cmd_ptr->status = NV_BADCMD_S);
      
    } /* switch */

    if (item_to_be_written == TRUE) {
      local_cmd.status = nvio_write(&local_cmd);
      if (local_cmd.status != NV_DONE_S) {
        /* Restore original NV task priority before returning */
        (void)rex_set_pri(nv_task_pri);
       
        MSG_HIGH ("NV Priority Returned to NORMAL", 0, 0, 0);
        return (local_cmd.status);
      }
    }

    /* Reset data_ptr for next item */
    local_cmd.data_ptr = &local_item;
  } /* while */  

  /* Restore original NV task priority before returning */
  (void)rex_set_pri(nv_task_pri);

  MSG_HIGH ("NV Priority Returned to NORMAL", 0, 0, 0);
  return (local_cmd.status);

#else
  return NV_BADTG_S;

#endif /* FEATURE_NV_OTASP */
} /* nv_otasp_commit */


/*===========================================================================

FUNCTION NV_PRL_IS_VALID

DESCRIPTION
  This function returns the "valid" indicator from the specified NAM's
  roaming list.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE         if the roaming list "valid" field is TRUE 
  FALSE        if the roaming list "valid" field is FALSE or if the
                 specified NAM is invalid or if the roaming list
                 is currently NOT_ACTIVE

SIDE EFFECTS
  None.  
  
===========================================================================*/

boolean  nv_prl_is_valid
(
  byte   nam          /* Which NAM the request is for */
)
{
  /* Check for illegal NAM */
  if (nam >= NV_MAX_NAMS) return FALSE;

  /* Note that valid field is set FALSE if prl is NOT_ACTIVE */
  return nv_prl_valid_data[nam];
}


/*===========================================================================

FUNCTION NV_PRL_VERSION

DESCRIPTION
  This function returns the "prl_version" indicator from the specified NAM's
  roaming list.  

DEPENDENCIES
  None.

RETURN VALUE
  prl_version             if the roaming list for the specified NAM is
                            currently ACTIVE 
  NV_PRL_VERSION_INVALID  if the specified NAM is invalid or if the 
                            roaming list for the specified NAM is 
                            currently NOT_ACTIVE

SIDE EFFECTS
  None.

===========================================================================*/
word  nv_prl_version
(
  byte   nam          /* Which NAM the request is for */
)
{
  /* Check for illegal NAM */
  if (nam >= NV_MAX_NAMS) return NV_PRL_VERSION_INVALID;

  /* Note that prl_version field is set to NV_PRL_VERSION_INVALID if   */
  /* the roaming list is NOT_ACTIVE                                    */

	
  return nv_prl_version_data[nam];
}


/*===========================================================================

FUNCTION NVIMNV_TASK_START

DESCRIPTION
  This function performs NV task initialization.  The function
  first sets the acknowledge signal back to MC to acknowledge that it has
  been created.  It then waits for the start signal back to it.  While
  waiting for the start signal and keeps kicking the watchdog timer
  everytime its interval timer has expired.  When it gets the start
  signal it then performs file initialization.
  Once initialization completes, it then  acknowledges it back to MC 
  and returns to the caller.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/

static void nvimnv_task_start
(
  rex_sigs_type    rpt_sig,        /* report timer signal for task */
  dog_report_type  dog_rpt,        /* Watchdog report for the task */
  rex_timer_type   *rpt_timer_ptr  /* pointer to report timer */
)
{

  rex_sigs_type sigs;               /* Signals returned by rex_timed_wait. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Acknowledge task creation. */

  (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);

  /* Continue to set watchdog timer while waiting for start signal from MC. */

  for (;;)
  {
    /* Set a timed wait for OR of signals and a watchdog report timer. */

    sigs = rex_timed_wait(TASK_START_SIG | rpt_sig, rpt_timer_ptr, DOG_MC_RPT_TIME);

    /* If start signal is set then perform file initialization and  
       signal back to MC.     */

    if ((sigs & TASK_START_SIG) != 0)
    {
       (void) rex_clr_sigs(&nv_tcb, TASK_START_SIG);
       
       /* Initialize the EFS files. */
       if (nv_init() != NV_DONE_S) 
       {
          ERR_FATAL("Failed NVIM EFS initialization", 0, 0, 0);
       }
       
       MSG_MED("NVIM initialization successfull", 0, 0, 0);
       nvi_initialized = TRUE;
      
       /* Signal back to TMC acknowledgement.  */
       (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
       break;
    }
    /* Otherwise timer has expired.  Kick the watchdog and loop.   */
    else
    {
       (void) rex_clr_sigs(&nv_tcb, rpt_sig);
       dog_report(dog_rpt);
    }
  } /* for */
} /* nvimnv_task_start */


/*===========================================================================

FUNCTION NV_FRONT_OP

DESCRIPTION
  This is the function called by both nv_cmd and nv_task. This is required
  in order to use kxmutexes for both synchronous and asynchronous accesses via
  NV.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void nv_front_op(
  nv_cmd_type  *cmd_ptr                   /* Pointer to NV command buffer */
  )
{

  (void)KxMutex_Lock(&nv_access_op_cs);   

  switch (cmd_ptr->cmd) {
    case NV_READ_F:
      cmd_ptr->status = nvio_read(cmd_ptr);
    break;

    case NV_WRITE_F:
      cmd_ptr->status = nvio_write(cmd_ptr);
    break;

    case NV_PEEK_F:
      cmd_ptr->status = NV_FAIL_S;
    break;

    case NV_POKE_F:
      cmd_ptr->status = NV_FAIL_S;
    break;

    case NV_FREE_F:
      cmd_ptr->status = nvim_remove_item(cmd_ptr);
    break;

    case NV_CHKPNT_ENA_F:
      break;

    case NV_CHKPNT_DIS_F:
      break;

    case NV_OTASP_COMMIT_F:
      cmd_ptr->status = nv_otasp_commit(cmd_ptr);
    break;

    case NV_REPLACE_F:
      cmd_ptr->status = nvio_write(cmd_ptr);
    break;

    case NV_INCREMENT_F:
      cmd_ptr->status = nvim_increment(cmd_ptr);
    break;
           
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
    case NV_RTRE_OP_CONFIG_F:
      cmd_ptr->status = 
      nvruim_process_rtre_configuration(cmd_ptr, 
                                        &nv_rtre_control_value,
                                        &nv_rtre_polling_control_value);
    break;
#endif           

    default:
      cmd_ptr->status = NV_BADCMD_S;
    break;

  }/*switch*/

  (void)KxMutex_Unlock(&nv_access_op_cs);
}


/*===========================================================================

FUNCTION NV_TASK

DESCRIPTION
  This is the NV task, created via rex_def_task from the Main Control task.
  The NV task sets MC_ACK_SIG when it is done with initialization and it
  then waits for NV_START_SIG before starting its normal operations.  The
  NV task then enters a main loop, awaiting for commands received on its
  input queue.  Each command received is processed and the loop continues.
  While in the main loop the task kicks the watchdog periodically.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  nv_task
( 
  dword   parm          /* from REX - ignored */
)
{
  nv_cmd_type     *cmd_ptr;   /* Pointer to command received on nv_cmd_q */
  rex_sigs_type   rex_sigs;   /* REX signals word */
  rex_tcb_type    *task_ptr;  /* Task to signal back on request completion. */
  rex_sigs_type   task_sigs;  /* Signals to set back to requesting task */

/*-------------------------------------------------------------------------*/

/* Keep Lint happy */

#ifdef _lint
  parm = parm;
#endif

  MSG_MED("NV task created", 0, 0, 0);
  
  /* Initialize the watchdog report timer and NV write timer. */

  rex_def_timer(&nv_rpt_timer,  &nv_tcb, NV_RPT_TIMER_SIG);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the NV command queue. */
  (void) q_init(&nv_cmd_q);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Wait for start signal. */

  nvimnv_task_start (NV_RPT_TIMER_SIG, DOG_NV_RPT, &nv_rpt_timer);


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Kick the wathcdog and start task operations. */

  KICK_WATCHDOG();

  /* signal nv err log operations to queue writes to nv */
  nvi_task_running = TRUE;   

  for (;;) { /*  while (TRUE) and Lint happy */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set REX wait with mask of all possible input signals. */

    rex_sigs = rex_wait(  NV_RPT_TIMER_SIG      /* Watchdog timer elapsed */
                        | NV_CMD_Q_SIG          /* Input of command queue */
                        | NV_ERR_LOG_SIG        /* Process the nverr record */
                        | TASK_STOP_SIG         /* Stop the NV task */
                        | TASK_OFFLINE_SIG);    /* Go offline */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Kick watchdog and start the watchdog report timer.   */
    /* Note that the watchdog is kicked after every wakeup, */
    /* following input of any signal to the NV task.        */

    KICK_WATCHDOG();


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         COMMAND QUEUE SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    if ((rex_sigs & NV_RPT_TIMER_SIG) != 0) {
      MSG_LOW("NV_RPT_TIMER_SIG received", 0, 0, 0);
      /* write out any old ERR() if required */
      nverr_update_log();
    }

    if ((rex_sigs & NV_ERR_LOG_SIG) != 0) {
      MSG_LOW("NV_ERR_LOG_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&nv_tcb, NV_ERR_LOG_SIG);
      /* write out any old ERR() if required */
      nverr_update_log();
    }
    /* Check if command queue signal was set. */
    /* If set then clear signal and proceeed. */

    if ((rex_sigs & NV_CMD_Q_SIG) != 0) {
      MSG_LOW("NV_CMD_Q_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&nv_tcb, NV_CMD_Q_SIG);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Enter a loop where the command queue is checked and */
      while ((cmd_ptr = (nv_cmd_type *) q_get(&nv_cmd_q)) != NULL) 
      {
        nv_front_op(cmd_ptr);  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Save for a moment the requesting task TCB and signal to set. */
  
        task_ptr = cmd_ptr->tcb_ptr;
        task_sigs = cmd_ptr->sigs;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* If specified in request then return buffer to specified queue. */

        if (cmd_ptr->done_q_ptr != NULL)
          q_put(cmd_ptr->done_q_ptr, &cmd_ptr->link);
  
        /* If specified in request then signal the requesting task. */
  
        if (task_ptr != NULL)
          (void) rex_set_sigs(task_ptr, task_sigs);
  
        /* At the end of the loop kick the watchdog timer. */
  
        KICK_WATCHDOG();
  
      } /* while */
    } /* if command queue signal */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                       OFFLINE COMMAND SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    /* Check if offline command signal was set. If set then clear signal, */
    /* send ack back to MC, and proceeed.                                 */

    if ((rex_sigs & TASK_OFFLINE_SIG) != 0) {
      MSG_MED("TASK_OFFLINE_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&nv_tcb, TASK_OFFLINE_SIG);
      (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG);
      /* write out any old ERR() if required */
      nverr_update_log();
    } /* if offline command signal */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     POWERDOWN (STOP) COMMAND SIGNAL                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    /* Check if powerdown command signal was set. */
    /* If set then clear signal and proceeed.     */

    if ((rex_sigs & TASK_STOP_SIG) != 0) {
      /* write out any old ERR() if required */
      nverr_update_log();
      MSG_MED("TASK_STOP_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&nv_tcb, TASK_STOP_SIG);

      nvi_task_running=FALSE;

      task_stop ();
      /* Send ack back to MC. */
      /* (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG); */
    } /* if stop command signal */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     WATCHDOG REPORT TIMER SIGNAL                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* PC debug only.                                                          */

#ifdef FEATURE_NV_UNIT_TEST
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* for (;;) */
} /* nv_task */



/*===========================================================================
FUNCTION NVERR_INIT

DESCRIPTION
  Initialize nv error queue for external tasks
  
DEPENDENCIES
  Called once, by NV.C to init buffers

RETURN VALUE
  False if init conflict.

SIDE EFFECTS
  None.

===========================================================================*/
boolean 
nverr_init( void ) {
  int i;
  static boolean nv_error_initialized = FALSE;
  static boolean nv_error_init_in_progress = FALSE;

  /* We have allready visited this routine */
  if(nv_error_initialized) {
    nv_error_init_in_progress = FALSE;
    return TRUE;
  }

  /* Flag incase multiple paths initing nv at same time. */
  if(nv_error_init_in_progress) return FALSE;
  nv_error_init_in_progress = TRUE;

  /* read up shadow copy of error log */
  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    (void) nvio_read_item (
        NV_ERR_LOG_I,
        (byte) i,                          /* Item index within its array */
        (byte*)&(nverr_log.external[i].err_count),          /* data buffer */
        nvim_op_get_size(NV_ERR_LOG_I)
    );
    nverr_log.update_required[i] = FALSE;
  }
  nverr_log.processing_required = FALSE;

  nv_error_initialized = TRUE;
  nv_error_init_in_progress = FALSE;
  return TRUE;
}


/*===========================================================================
FUNCTION NVERR_UPDATE_LOG

DESCRIPTION
  write dirty error log items out to nv.
  
DEPENDENCIES
  nverr_init must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void 
nverr_update_log( void ) {
  int  i;      /* loop counter */
  word isave;  /* save flags for interrupt diasble/enable */

  /* If no errors waiting to be saved, get out of here */
  if(!nverr_log.processing_required) return;

  KICK_WATCHDOG();

  /* update internal copy of err_log to write out from */
  INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */
  (void) memcpy((void *) nverr_log.internal,
         (void *) nverr_log.external, 
         sizeof(nverr_log.internal));   /* data buffer size */

  (void) memcpy((void *) nverr_log.update_in_progress,
         (void *) nverr_log.update_required, 
         sizeof(nverr_log.update_in_progress));   /* data buffer size */

  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    nverr_log.update_required[i] = FALSE;
  }
  nverr_log.processing_required = FALSE;
  INTFREE_SAV( isave );         /* Restore interrupts (PSW) */

  /* write out internal shadow copy of error log */
  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    if(nverr_log.update_in_progress[i]) {
      MSG_MED("nverr_log entry %d update in progress ", i, 0, 0);
      KICK_WATCHDOG();
      nvio_write_item
             (
                 NV_ERR_LOG_I,
                 (byte) i,                          /* Item index within its array */
                 (byte *)&(nverr_log.internal[i].err_count),    /* data buffer */ 
                 nvim_op_get_size(NV_ERR_LOG_I)
             );
    }
  }
}


/*===========================================================================

FUNCTION NV_MAX_SIZE_OF_ROAMING_LIST

DESCRIPTION
  This function returns the maximum number of bytes available for
  storage of a roaming list (per NAM).  The value returned depends 
  on the total amount of NVRAM installed in the phone and the total 
  number of NAMs defined.  

DEPENDENCIES
  The function cannot be called until NV is initialized.  A
  premature call will raise err_fatal.

RETURN VALUE
  The maximum number of bytes available for storage of a roaming list
  (per NAM).

SIDE EFFECTS
  None.

===========================================================================*/

word  
nv_max_size_of_roaming_list( void ) {
  if (!nvi_initialized)
  {
    ERR_FATAL("Illegal task start-up order", 0, 0, 0);
  }

  #if !defined( FEATURE_NV_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available */
    if (NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
    #ifdef NV_FEATURE_PRL_ITEMS
       return NV_ROAMING_LIST_MAX_SIZE;
    #else
       return (word)0;
    #endif
    }
  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else {
      #ifdef FEATURE_NVRUIM_ADM_ACCESS
      /* Return a max number here and check the size in nvruim before writing */
      return NV_ROAMING_LIST_MAX_SIZE;
      #else
      #ifdef FEATURE_DISABLE_UIMPRL //Gemsea Add
      return NV_ROAMING_LIST_MAX_SIZE;
      #else
      return (word)0;
      #endif
      #endif  /* FEATURE_RUIM_ADM_ACCESS */
    }
  #endif /*( FEATURE_UIM_RUN_TIME_ENABLE ) */

  #else
    return (word)0;
  #endif /* !FEATURE_NV_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* nv_max_size_of_roaming_list */


/*===========================================================================

FUNCTION NV_MAX_SIZE_OF_WEB_CACHE

DESCRIPTION
  This function returns the maximum number of bytes currently available 
  for storage of the Unwired Planet cache.  At this time, the cache
  size is always zero (it is not currently used).
  
DEPENDENCIES
  The function cannot be called until NV is initialized.  A
  premature call will raise err_fatal.

RETURN VALUE
  The maximum number of bytes currently available for storage of the
  Unwired Planet cache.

SIDE EFFECTS
  None.

===========================================================================*/

word  
nv_max_size_of_web_cache( void ) {
  if (!nvi_initialized) {
    ERR_FATAL("Illegal task start-up order", 0, 0, 0);
  }
  return (word)0;
} /* nv_max_size_of_web_cache */


/*===========================================================================

FUNCTION NV_BUILT

DESCRIPTION
  This function will return TRUE once the files are available that simulate
  NV storage.

DEPENDENCIES
  This is a special use function, normally called by error services
  to allow early access to NV, and before the NV task has been started.
  The NV Item Manager allows access once file initialization is complete.

RETURN VALUE
  TRUE      - The NV has been built and direct read/write is allowed
  FALSE     - The NV has not been built and access is not allowed

SIDE EFFECTS
  None.

===========================================================================*/

boolean nv_built (void)

{
  return nvi_initialized;
  
} /* nv_built */


/*===========================================================================

FUNCTION NV_CMD

DESCRIPTION
  This is the outside world's interface to the non volatile memory task
  this function takes an already filled out nv_cmd_type and place it on
  the nv queue.  The function returns to the caller after setting the
  status to busy indicaton.  This does _not_ mean that the command has
  been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void  
nv_cmd (
  nv_cmd_type  *cmd_ptr                   /* Pointer to NV command buffer */
)
{

  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */
  if (cmd_ptr->tcb_ptr != NULL) 
  {
    cmd_ptr->status = NV_BUSY_S;
    (void) q_link(cmd_ptr, &cmd_ptr->link);
    q_put(&nv_cmd_q, &cmd_ptr->link);
    (void) rex_set_sigs(&nv_tcb, NV_CMD_Q_SIG);
  }
  else
  {

    /* For synchronous calls, we need to ensure mutexes are initiliazed */
    if(!nv_cs_init_flag)
      nv_access_op_cs_init();
  
    nv_front_op(cmd_ptr);
  } /* if command has no tcb  */
} /* nv_cmd */

#ifdef FEATURE_RPC
#error code not present
#endif /* FEATURE_RPC */

#ifdef FEATURE_NV_RPC_SUPPORT
/*===========================================================================

FUNCTION NV_CMD_REMOTE

DESCRIPTION
  This is the outside world's interface to the non volatile memory task.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_type must be already set.

RETURN VALUE

  NV_DONE_S,      -  Request completed okay
  NV_BUSY_S,      -  Request is queued
  NV_BADCMD_S,    -  Unrecognizable command field
  NV_FULL_S,      -  The NVM is full
  NV_FAIL_S,      -  Command failed, reason other than NVM was full
  NV_NOTACTIVE_S, -  Variable was not active
  NV_BADPARM_S,   -  Bad parameter in command block
  NV_READONLY_S,  -  Parameter is write-protected and thus read only
  NV_BADTG_S,     -  Item not valid for Target
  NV_NOMEM_S,     -  free memory exhausted
  NV_NOTALLOC_S   -  address is not a valid allocation

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed!

===========================================================================*/

nv_stat_enum_type 
nv_cmd_remote (
  nv_func_enum_type cmd,
  nv_items_enum_type item,
  nv_item_type *data_ptr
)
{
  nv_cmd_type nv_rpc_cmd;

  nv_rpc_cmd.status = NV_BUSY_S;
  (void) q_link(&nv_rpc_cmd, &nv_rpc_cmd.link);
  nv_rpc_cmd.tcb_ptr = rex_self();
  nv_rpc_cmd.sigs = NV_QIDS_RPC_SIG;
  nv_rpc_cmd.cmd = cmd;
  nv_rpc_cmd.item = item;
  nv_rpc_cmd.data_ptr = data_ptr;
  nv_rpc_cmd.done_q_ptr = NULL;

  nv_cmd( &nv_rpc_cmd );
  rex_wait( NV_QIDS_RPC_SIG ); /* this signal needs to be reserved for all worker task*/

  rex_clr_sigs( nv_rpc_cmd.tcb_ptr, NV_QIDS_RPC_SIG );

  return nv_rpc_cmd.status;

}
#endif /*FEATURE_NV_RPC_SUPPORT */

/*===========================================================================

FUNCTION NV_RTRE_USE_RUIM_FOR_NAM

DESCRIPTION
  This function determines if the R-UIM is to be used (based on RTRE control)
  and the NAM is the R-UIM NAM.
  
DEPENDENCIES
  The return value has a default setting and is updated when NV receives
  a NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  The RTRE control indicates to use the R-UIM and the NAM matches the R-UIM 
  NAM.

SIDE EFFECTS
  This function uses the current value of the RTRE control.  It may
  change.

===========================================================================*/

boolean 
nv_rtre_use_ruim_for_nam (
  byte nam
)
{	
  #ifdef FEATURE_UIM_RUN_TIME_ENABLE
    /* Determine if the current RTRE control is for the R-UIM */
   // return ((NV_RTRE_CONTROL_USE_RUIM == nv_rtre_control()) &&

   //Add By zzg 2010_09_08
   return (((NV_RTRE_CONTROL_USE_RUIM == nv_rtre_control()) || (NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control())) &&
   //Add End
   
    /* and the NAM matches the R-UIM NAM */
            (NV_RUIM_NAM == nam));
  #else
    (void)nam;
    return FALSE;
  #endif /*FEATURE_UIM_RUN_TIME_ENABLE*/
}

/*==================================================================
Function: NV_ACCESS_OP_CS_INIT

Description: This functions initialises kxmutex.
==================================================================*/

void nv_access_op_cs_init(void)
{
  if (!(KxMutex_Init(&nv_access_op_cs,"KXMUTEX_NV_ACCESS_OP_CS"))) 
  {
    ERR_FATAL("nv_access_op_cs KxMutex_Alloc Failed",0,0,0);
  }
  nv_cs_init_flag = TRUE;
}

#ifdef CUST_EDITION
#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE

extern void lcd_debug_message( char * message);


/*===========================================================================

Function nv_auto_set_prl()

DESCRIPTION
  Auot set the pre-defined prl to nv.

PARAMETERS
  None

DEPENDENCIES
  Must be called after NV task initialized or be called in nv_auto_set_need_value

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type nv_auto_set_prl()
{
    dword               dwPrlSize = 0;
#ifdef FEATURE_SD20
    word                prl_version = PRL_DEFAULT_VER;
#endif    
    boolean             bIsNVTaskIni = nvi_initialized;
#ifdef FEATURE_EXTENDED_PRL
    nv_item_type  nv_item;  /* NV Item buffer for NV interface */
    prl_sspr_p_rev_e_type sspr_p_rev; /* PRL protocol revision */
#endif

    dwPrlSize = sizeof(pr_list_data)/sizeof(byte);

    local_cmd.tcb_ptr    = NULL;
    local_cmd.sigs       = 0;
    local_cmd.done_q_ptr = NULL;
    
    //set it to TRUE and set back to it's own value when exit
    nvi_initialized = TRUE;
    
    if(dwPrlSize > nv_max_size_of_roaming_list())
    {
        ERR("Pre-defined PRL size is invalid!!!", 0, 0, 0);  
        
        nvi_initialized = bIsNVTaskIni;
        return NV_NOTACTIVE_S;
    }
    
    /* We always only pre-set NAM 1 */
    nv_pr_list.nam = 0;
    nv_pr_list.size = dwPrlSize * 8;
    
    memcpy((void*)nv_pr_list.roaming_list, (void*)pr_list_data, dwPrlSize);
    
#ifdef FEATURE_SD20
    #ifdef FEATURE_EXTENDED_PRL
    local_cmd.cmd           = NV_READ_F;
    local_cmd.data_ptr      = (nv_item_type*) &nv_item;
    local_cmd.item          = NV_SSPR_P_REV_I;
    nvio_read(&local_cmd);
    /* Get PRL protocol revision from NV. Validate PRL revision from nv first
    ** then with all other available protocol revisions next.
    ** Update revision in nv if returned revision does not match that from nv.
    */
    if ( local_cmd.status != NV_DONE_S ) {
       MSG_HIGH("Failed to read PRL protocol revision", 0, 0, 0);
       nv_item.sspr_p_rev = PRL_SSPR_P_REV_INVALID;
       sspr_p_rev = PRL_SSPR_P_REV_1;
    }
    else{
       sspr_p_rev = (prl_sspr_p_rev_e_type)nv_item.sspr_p_rev;
    }

    nv_pr_list.valid = FALSE;
    if( prl_validate((byte*) nv_pr_list.roaming_list, &prl_version, &sspr_p_rev, TRUE) == PRL_VALID)
    {
      if ( sspr_p_rev != nv_item.sspr_p_rev) {
        nv_item.sspr_p_rev = sspr_p_rev;
        local_cmd.item      = NV_SSPR_P_REV_I;
        local_cmd.cmd       = NV_WRITE_F;
        local_cmd.data_ptr  = (nv_item_type*) &nv_item;
        nvio_write(&local_cmd);
      }
      nv_pr_list.valid = TRUE;
    }
    #else /* !FEATURE_EXTENDED_PRL*/
    if( prl_validate((byte*) nv_pr_list.roaming_list, &prl_version) == PRL_VALID)
    {
      nv_pr_list.valid = TRUE;
    }
    #endif /* FEATURE_EXTENDED_PRL */
#else
    if (mcsys_validate_roaming_list((byte *)nv_pr_list.roaming_list) == PRL_VALID)
    {
      nv_pr_list.valid = TRUE;
    }
#endif


    /* Set the PRL version */
#ifdef FEATURE_SD20
    nv_pr_list.prl_version = prl_version;
#else
    nv_pr_list.prl_version = mcsys_get_prl_version();
#endif
    
#ifdef FEATURE_IS683A_PRL
    local_cmd.item = NV_ROAMING_LIST_683_I;
#else
    local_cmd.item = NV_ROAMING_LIST_I;
#endif    
    
    local_cmd.cmd        = NV_WRITE_F;
    local_cmd.data_ptr   = (nv_item_type*) &nv_pr_list;
    nvio_write(&local_cmd);
    
    nvi_initialized = bIsNVTaskIni;
    return local_cmd.status;
}
    
/*===========================================================================

Function nv_auto_set_need_value()

DESCRIPTION
  Auot set needed nv item when fresh build.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type nv_auto_set_need_value(void)
{
    int                             table_index, table_size, list_index;
    dword                           auto_nv_item_version = AUTO_NV_CUST_CODE_INVALID;
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
    extern boolean                  nvruim_rtre_config_initialized;

    nv_rtre_control_type            saved_rtre_control_value = nv_rtre_control_value;
    nv_rtre_polling_control_type    saved_rtre_polling_control_type = nv_rtre_polling_control_value;
    boolean                         saved_rtre_config_initialized = nvruim_rtre_config_initialized;    
#endif
    nv_stat_enum_type               nvOpStatus;
    boolean                         bIsFreshBuild = TRUE;
    
    //Check if this is a fresh start phone.
    //if the phone is fresh start, the NV_STATUS of NV_PCB_STATUS_I must be NV_INACTIVE
    local_cmd.tcb_ptr    = NULL;
    local_cmd.sigs       = 0;
    local_cmd.done_q_ptr = NULL;
    local_cmd.cmd        = NV_READ_F;
    local_cmd.item       = NV_AUTO_SET_ITEM_VERSION_I;
    local_cmd.data_ptr   = (nv_item_type*) &auto_nv_item_version;

    //Read It
    local_cmd.status = nvio_read(&local_cmd);
    
    if(NV_DONE_S == local_cmd.status)
    {
        if(AUTO_NV_ITME_VERSION == auto_nv_item_version)	
        {
            //OK, it not a fresh start phone and version is same as this bin file
            //skip the follow operations.
            return NV_DONE_S;
        }
        
        //If version doesen't equal we need to do follow operations.        
           
        //make it as not fresh start.
        bIsFreshBuild = FALSE;  
    }
    //OK, Fresh Build of NV VERSION change, I must restore OEM NVs.
    bIsResetOemNv = TRUE;
    
    //OK, need to restore, make the version equal to BIN file's.
	auto_nv_item_version = AUTO_NV_ITME_VERSION;   
    lcd_debug_message ("Initialize necessary nv, please waiting.");
    
    
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
//If phone supports UIM RTRE, we must config RTRE mode to NV Only, then we can write some SERVICE NVs.
//After NVs set done, we MUST restore those RTRE Control value.
    nv_rtre_control_value = NV_RTRE_CONTROL_NO_RUIM;
    nv_rtre_polling_control_value = NV_RTRE_POLLING_CONTROL_NO_POLL;
    nvruim_rtre_config_initialized = TRUE;
#endif           

    local_cmd.cmd = NV_WRITE_F;

    /*--------------------------------------------------------------
                       Write Needed Items to NV
    --------------------------------------------------------------*/
    table_index = 0;
    table_size = sizeof(nv_auto_set_table)/sizeof(nv_auto_set_table_type);
    while(table_index < table_size)
    {
        list_index = 0;
        while(list_index < nv_auto_set_table[table_index].lsit_size)
        {
            local_cmd.item = nv_auto_set_table[table_index].plist[list_index].nv_items_id;
            local_cmd.data_ptr = (nv_item_type*)nv_auto_set_table[table_index].plist[list_index].nv_items_value;
            local_cmd.status = nvio_write(&local_cmd);

            //if(NV_DONE_S != local_cmd.status)
            //{
            //    ERR_FATAL("Auto set NV failed, item = %d", local_cmd.item, 0, 0);
            //    return local_cmd.status;                            
            //}
            
            list_index++;
        }
        table_index++;
    }

    /*--------------------------------------------------------------
                       Write Pre-Defined PRL to NV
    --------------------------------------------------------------*/
    nvOpStatus = nv_auto_set_prl();    
    //if(NV_DONE_S != nvOpStatus &&  NV_NOTACTIVE_S != nvOpStatus)
    //{
    //    ERR_FATAL("Auto set PRL failed, item = %d", local_cmd.item, 0, 0);   
    //    return local_cmd.status;         
    //}
    
    //Only restore Pre-Defined RF calibration items and
    //Critical Items when phone's fresh build
    if(bIsFreshBuild)
    {
    /*--------------------------------------------------------------
                Write Pre-Defined RF calibration items to NV
    --------------------------------------------------------------*/
        list_index = 0;
        while(list_index < sizeof(nv_auto_set_rf_cal_list)/sizeof(nv_auto_set_type))
        {
            local_cmd.item = nv_auto_set_rf_cal_list[list_index].nv_items_id;
            local_cmd.data_ptr = (nv_item_type*)nv_auto_set_rf_cal_list[list_index].nv_items_value;
            local_cmd.status = nvio_write(&local_cmd);

            //if(NV_DONE_S != local_cmd.status)
            //{
            //    ERR_FATAL("Auto set NV failed, item = %d", local_cmd.item, 0, 0);      
            //    return local_cmd.status;                            
            //}
            list_index++;
        }
    } //if(bIsFreshBuild)

    local_cmd.item       = NV_AUTO_SET_ITEM_VERSION_I;
    local_cmd.data_ptr   = (nv_item_type*) &auto_nv_item_version;

    //Write NV
    local_cmd.status = nvio_write(&local_cmd);

    if(NV_DONE_S != local_cmd.status)
    {
        ERR_FATAL("Set auto nv item version failed, status = %d", local_cmd.status, 0, 0);            

#ifdef FEATURE_UIM_RUN_TIME_ENABLE    
        nv_rtre_control_value = saved_rtre_control_value;
        nv_rtre_polling_control_value = saved_rtre_polling_control_type;
        nvruim_rtre_config_initialized = saved_rtre_config_initialized;
#endif      
        return local_cmd.status;                            
    }

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
    nv_rtre_control_value = saved_rtre_control_value;
    nv_rtre_polling_control_value = saved_rtre_polling_control_type;
    nvruim_rtre_config_initialized = saved_rtre_config_initialized;
#endif  
    
    return NV_DONE_S;
}
#endif /*FEATRUE_AUTO_SET_NEED_NV_VALUE*/
#endif /*CUST_EDITION*/

#endif /* FEATURE_NV_ITEM_MGR */

