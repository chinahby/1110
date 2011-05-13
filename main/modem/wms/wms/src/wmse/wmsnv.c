/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 W I R E L E S S   M E S S A G I N G   S E R V I C E S
                 -- NV Operations

GENERAL DESCRIPTION
  This module provides the utility functions for the use by WMS for accessing
  NV items needed for SMS processing.

EXTERNALIZED FUNCTIONS
  wms_nv_read_wait
    Read an item from NV, handle non-active items

  wms_nv_write_wait
    Write an item to NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999,2000,2001,2002,2003,2004,2005,2006,2007,2008
                by QUALCOMM INCORPORATED.
  All Rights Reserved.


  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsnv.c_v   1.11   10 Jul 2002 20:18:12   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsnv.c#6 $ $DateTime: 2010/02/23 11:51:47 $ $Author: rajang $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/09   HN      Mainlined FEATURE_OMH_SMS
                   Added function wms_nv_wait()
                   Modified wms_nv_read_wait() and wms_nv_write_wait to:
                      Pass TCB pointer instead of NULL when issuing NV commands
                      Wait for REX signal when issuing NV commands                  
09/12/08   YC      Set the default value for NV_SMS_CD_PARMS_I and 
                   NV_SMS_GW_PARMS_I correctly
06/24/08   PMD     Removed Tabs from code
06/23/08   HN      Fixed Lint & Klocwork warnings
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
04/04/08   PMD     Feature-ized OMH logic.
04/04/08   PMD     Added support for BC-SMS and SMSCAP EFs in RUIM.
03/31/08   PMD     Added Null Pointer check in wms_nv_gw_msg_store_read_tags
01/31/08   PMD     Updated Copyright Information to include 2008
09/19/07   PMD     Default GW CB Config and Pref to Disallow and Deactivate
08/15/07   PY      split wmsmsg.h to form wmsmsgi.h a private header.
06/20/07   PMD     Include wmstsi.h
05/11/07   PMD     Fixed typo in wms_nv_write_bearer_prefs()
04/06/07   HQ      Check NV reading/write errors and add debug messages.
03/12/07   PMD     Changed default Max NV Messages to GCF default
03/07/07   PMD     Fixed Compile Errors due to nvi
03/05/07   HQ      Changed NV item definitions.
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP.
01/17/07   HQ      Updated copyright info to include 2007.
11/14/06   HQ      Changed prototype of wms_ts_decode_CDMA_bd().
10/06/06   NP      Fix data abort that happens when we have zero NV slots
09/28/06   PMD     Ignore GCF Flag for determining Max Number of GW NV Messages
09/25/06   PMD     Added Critical Section for WMS NV Access
                   Eliminated static nv_item_type for preventing overwrites
08/10/06   NP      Changed GW & CD SMS messages EFS storage mechanism. 
07/31/06   PMD     Removed unused feature - BCSMS_NO_NV
07/31/06   PMD     Removed unused feature - SMS_TREX
07/19/06   PMD     Changed wms_tcb to rex_self() in wms_nv_wait function.
06/26/06   PMD     Removed wms_nv_delete_cdma_params()
                   Replaces NV SMS Messages storage to EFS.
                   Removed wms_nv_delete_wait(), wms_nv_replace_wait()
05/03/06   PMD     Modified wms_ts_decode_CDMA_bd prototype
04/25/06   HQ      Fixed lint errors.
04/20/06   PMD     Mainlined FEATURE_SMS_RETRY.
02/14/06   PMD     Fixed Typo in Feature Define.
01/25/06   VK      Updated Copyright Info
01/20/06   PMD     Include wmsts.h to resolve Compile Warnings
12/22/05   VK      Removed ERR_FATALs
12/21/05   VK      Lint fixes
12/20/05   PMD     Cleaned up FEATURE_SIRIUS from Code Base.
11/18/05   VK      Cleaned up Lint warnings.
10/20/05   VK      Removed Lint Suppressions
10/20/05   VK      Lint error/warning fixes
10/19/05   VK      Added new feature FEATURE_CDSMS_DUP_DETECTION
10/19/05   PMD     Increased NV SMS Address type to uint16 under F_SIRIUS.
10/17/05   PMD     Check NV Item for Max NV SMS Slots.
10/17/05   PMD     Updated Copyright Information.
                   Added Function Headers.
10/17/05   PMD     Read SMS Retry Settings from NV
08/29/05   PMD     Added Read GCF Flag Functionality.
                   Determine Max GW NV Message count based of GCF Flag.
07/17/05   PMD     Added support for wms_bc_gw_cb_pre_init function.
09/30/04   PMD     Fixes to initialize GW CB Pref and Table Size
06/04/04   HQ      Default SMS-CB Config to Allow if not initialized.
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
02/04/04   PMD     Cleaned up Lint Warnings in code
11/06/03   AP      Assign global nv_cmd_err after nv_write_wait.
09/23/03   PMD     Changed Default Template parameters to include 1 digit in
                   SC address
07/26/03   PMD     Changes to suppress / remove Lint Errors
06/17/03   HQ      Use enum symbols instead of raw numbers in wms_nv_init().
06/02/03   PMD     Added Support for dynamic allocation of Alpha_id
                   Initialization of NV template if not present
05/30/03   HQ      Suppress MSG_ERRORs when reading NV SMS items returns
                   NV_NOTACTIVE_S.
04/21/03   HQ      Use WMS_MAX_NV_CDMA_MESSAGES and WMS_MAX_NV_GW_MESSAGES to
                   replace WMS_MAX_NV_MESSAGES.
03/14/03   HQ      Fixed compiler warnings.
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/07/03   SUN     Added Support for FEATURE_GWSMS_CACHE
12/04/02   HQ      Removed compiler warnings.
11/13/02   AP      Added support for duplicate detection, mainlined message
                   info cache support.
08/26/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE.
07/10/02   HQ      Store RUIM format to NV for CDMA SMS parameters;
                   set default CDMA SMS parameter values if NV read fails.
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
06/05/02   HQ      Use NV_SMS_CFG_ROUTING_I instead of NV_SMS_ROUTING_I.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
02/22/02   SUN     Default value is written if the nv item is not active or the
                   read for a NV item fails
02/13/02   SUN     Added nv replace and delete commands
02/07/02   HQ      Changed nv item size to NV_SMS_GW_DATA_SIZ and corrected
                   indexing problem.
01/25/02   HQ      Properly initialized NV messages.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
10/30/01   HQ      Initial version for FEATURE_GWSMS.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"
#ifndef FEATURE_ONEMSG_USE_ONEFILE

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include <memory.h>
#include <stdio.h>
#include "wmsi.h"
#include "wmsnv.h"
#include "wmscfg.h"
#include "wmsmsgi.h"
#include "wmsts.h"
#include "err.h"
#include "nv.h"
#include "task.h"
#include "tmc.h"
#include "fs_public.h"
#ifdef FEATURE_CDSMS 
#include "wmstscdma.h"
#include "wmstsi.h"
#include "wmsdc.h"
#endif /* FEATURE_CDSMS */
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*      --------------------------------------
 *      C D    E F S    F I L E    F O R M A T 
 *      --------------------------------------
 * 
 * SMS uses two files in EFS to store its messages, one to store
 * header information about storage and the other to store the actual data
 * 
 * 
 * SMS_NV_CD_MSG_STORE_MAJOR_VER
 *  - Version number of payload slot data format
 * 
 * SMS_NV_CD_MSG_STORE_DATA     
 *  - The actual SMS messages are stored in this file 
 * 
 * SMS_NV_CD_MSG_STORE_HEADER   
 *  - Contains meta information.
 *    First 4 bytes - (Major version number) Incremented
 *                    everytime we change the payload data
 *    Next 4 bytes  - (Minor version number) Corresponds to 
 *                    number of slots used to create SMS_NV_CD_MSG_DATA
 *    The rest of the header data is version dependent.
 *
 *    --------------------
 *    Version 1.xxx format
 *    --------------------
 *    First 4 bytes - 0x00000001
 *    Next 4 bytes  - Depends on value of max number of slots
 *    Next 4 bytes  - Bytes occupied by each individual slot  
 *    Next n bytes  - Where "n" is the max number of slots.
 *                    Each byte corresponds to meta information about 
 *                    the individual slot, which is the tag info.
 * 
 * SMS_NV_CD_MSG_STORE_START_TAG / SMS_NV_CD_MSG_STORE_END_TAG 
 *  - The actual data is wrapped between these tags
 *    which act as a basic protection against corruption.
 */

/* EFS Storage Version */
#define SMS_NV_CD_MSG_STORE_MAJOR_VER       0x00000001

/* Files used for storage in EFS */
#define SMS_NV_CD_MSG_STORE_DATA            "/sms/nv_cd_msg_data"
#define SMS_NV_CD_MSG_STORE_HEADER          "/sms/nv_cd_msg_header"

/* Tags around actual message in slots */
#define SMS_NV_CD_MSG_STORE_START_TAG       0xCA
#define SMS_NV_CD_MSG_STORE_END_TAG         0xFE

/* Slot consits of actual data and the start+end byte tag */
/* NV_SMS_DATA_SIZ is defined in services/nv/nv_items.h */
#define SMS_NV_CD_MSG_STORE_SLOT_SIZ        (NV_SMS_DATA_SIZ + 2)
                                           
static wms_dc_s_type                *dc_s_ptr;

#endif /* FEATURE_CDSMS */

static wms_cfg_s_type               *cfg_s_ptr;
static wms_msg_s_type               *msg_s_ptr;


/* NV command buffer */
static nv_cmd_type                   nvcmd;           

/* Critical Section for WMS accessing NV */
rex_crit_sect_type                   wms_nv_crit_sect;

#ifdef FEATURE_CDSMS
static uint8                         nv_cdma_data[NV_SMS_DATA_SIZ];
static wms_cdma_template_s_type      cdma_template;
static wms_OTA_message_type          cdma_ota;
static wms_tl_message_type           cdma_tl;
static wms_client_message_s_type     client_message;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
//TBD

                            /**************************/
                            /* Local Helper Functions */
                            /**************************/
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
static uint8 wms_nv_cd_msg_store_authenticate       ( void );
static uint8 wms_nv_cd_msg_store_is_version_valid   ( void );
static uint8 wms_nv_cd_msg_store_init               ( void );
static uint8 wms_nv_cd_msg_store_create_header_file ( void );
static uint8 wms_nv_cd_msg_store_create_data_file   ( void );
static uint8 wms_nv_cd_msg_store_update_header_file ( wms_message_index_type index, 
                                                      uint8 tag );
static uint8 wms_nv_cd_msg_store_read_tags          ( void );
#endif /* FEATURE_CDSMS */

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_WAIT

DESCRIPTION
  Wait on NV actions.  Handle signals as they come in just in case NV
  takes a while.  Only time critical watchdog signals are acknowledged.

DEPENDENCIES
  There is a command sent to NV, and this task will be signalled.  There
  are no other outstanding NV requests from this task.

RETURN VALUE
  None

SIDE EFFECT
  Some signals are delayed till we're done with NV

===========================================================================*/
void wms_nv_wait
(
  void
)
{
  rex_sigs_type  sigs;

  while( 1 )
  {                     /* until signal back from NV */
    sigs = rex_wait( WMS_NV_SIG | WMS_RPT_TIMER_SIG );

    if( sigs & WMS_RPT_TIMER_SIG )
    {
      wms_kick_dog(); /* right now SMS and CM are sharing the same task! */
    }

    if( sigs & WMS_NV_SIG )
    {
      (void)rex_clr_sigs( rex_self(), WMS_NV_SIG );
      break;
    }
  }

  return;

} /* end of wms_nv_wait() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_WRITE_WAIT

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr    /* pointer to data for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_WRITE_F;
  nvcmd.item       = item;             /* item to write */
  nvcmd.data_ptr   = data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd( &nvcmd );
  wms_nv_wait(); 
  
  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  if( ret_status != NV_DONE_S )
  {
    ERR( "NV Write Failed Item %d Code %d", item, ret_status, 0 );
    /* Set the global NV cmd err appropriately */
    if(ret_status == NV_FULL_S || ret_status == NV_NOMEM_S)
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
    }
    else
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
    }
  }
  else
  {
    /* Set the global NV cmd err to OK */
    cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_NONE;
  }

  return( ret_status );

} /* end of wms_nv_write_wait() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_READ_WAIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr          /* pointer to space for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_READ_F;        /* command */
  nvcmd.item       = item;             /* item to read */
  nvcmd.data_ptr   = data_ptr;         /* where to put result */

  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd( &nvcmd );
  wms_nv_wait(); 
  
  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  /* Handle a NV_NOTACTIVE_S or NV_FAIL_S status internally by replacing
  ** the random data returned with a default value of our own.
  */
  if( ret_status == NV_NOTACTIVE_S || ret_status == NV_FAIL_S )
  {
    ret_status = NV_DONE_S;

    switch( item )
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
      case NV_SMS_CD_PARMS_I:
        data_ptr->sms_cd_parms.address = 0;
        data_ptr->sms_cd_parms.len = 0;
        memset( (void*) data_ptr->sms_cd_parms.data, 0, NV_SMS_CD_PARMS_SIZE );
        ret_status = wms_nv_write_wait( NV_SMS_CD_PARMS_I, data_ptr );
        break;

#ifdef FEATURE_CDSMS_BROADCAST
      case NV_SMS_BC_CONFIG_I:
        data_ptr->sms_bc_config = (uint8) WMS_BC_CONFIG_DISALLOW;
        break;

      case NV_SMS_BC_USER_PREF_I:
        data_ptr->sms_bc_user_pref = (uint8)WMS_BC_PREF_DEACTIVATE;
        break;

      case NV_SMS_BC_SERVICE_TABLE_SIZE_I:
        data_ptr->sms_bc_service_table_size = 0 ;
        break;

      case NV_SMS_BC_SERVICE_TABLE_I:
        data_ptr->sms_bc_service_table.active_service = FALSE;
        break;
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

      case NV_SMS_MO_RETRY_INTERVAL_I:
        data_ptr->sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
        (void) wms_nv_write_wait( item, data_ptr );
        break;
      
      case NV_SMS_MO_RETRY_PERIOD_I:
        data_ptr->sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
        (void) wms_nv_write_wait( item, data_ptr );
        break;

      /* If it's an unknown item, print an info message and pass on the */
      /* NV_NOTACTIVE_S status. */
      default:
        MSG_HIGH( "Nonactive NV item 0x%x", item, 0, 0 );
        ret_status = NV_NOTACTIVE_S;
        break;
    }

#ifdef FEATURE_CDSMS_BROADCAST
    /* write back the default values to NV */
    MSG_HIGH( "Write BC default value to NV, item=%d", item,0,0 );
    if ( item == NV_SMS_BC_CONFIG_I ||
         item == NV_SMS_BC_USER_PREF_I ||
         item == NV_SMS_BC_SERVICE_TABLE_SIZE_I ||
         item == NV_SMS_BC_SERVICE_TABLE_I )
    {
      (void) wms_nv_write_wait( item, data_ptr );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  else
  {
    if( ret_status != NV_DONE_S )
    {
      ERR( "NV Read Failed Item %d Code %d", item, ret_status, 0 );
    }
  }

  return( ret_status );

} /* end of wms_nv_read_wait() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_nv_read_cdma_sms

DESCRIPTION
  Read the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int            fd = -1;
  fs_ssize_t     bytes_read = 0;
  fs_off_t       offset = 0;
  uint8          *packed_data = NULL;
  
  if (index >= WMS_MAX_NV_CDMA_MESSAGES)
  {
    MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    MSG_ERROR("ASSERT! Could not authenticate storage.  Read failed!!", 0, 0, 0);
    return FALSE;
  }

  /* Read the contents from EFS */
  /* Open message storage */
  fd =  efs_open ( SMS_NV_CD_MSG_STORE_DATA, O_RDONLY );
  
  if ( fd < 0 )
  {
    MSG_ERROR ( "SMS Read failed because sms msg store file" 
                "could not be opened for item %d", index, 0 , 0 );
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset = (fs_off_t) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);
    packed_data = (uint8*) mem_malloc ( &tmc_heap, SMS_NV_CD_MSG_STORE_SLOT_SIZ ); 
    
    if ( packed_data == NULL )
    {
      MSG_ERROR("ASSERT! Fatal Error - mem_malloc() returned NULL!", 0, 0, 0);
      (void) efs_close ( fd );
      return FALSE;
    }
    
    /* Go to index position in message store before reading ... */
    offset = efs_lseek ( fd, offset, SEEK_SET );
 
    if ( offset == -1 )
    {
      MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
      
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }
    
    /* Read data from its correct slot in file */
    bytes_read =  efs_read ( fd, packed_data, SMS_NV_CD_MSG_STORE_SLOT_SIZ );

    if ( bytes_read != (fs_ssize_t) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR("ASSERT! Unable to read msg correctly from file!", 0, 0, 0);
      
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Check if data is valid */
    if ( packed_data[0] != SMS_NV_CD_MSG_STORE_START_TAG ||
         packed_data[SMS_NV_CD_MSG_STORE_SLOT_SIZ-1] != SMS_NV_CD_MSG_STORE_END_TAG ||
         packed_data[2] == (uint8) WMS_TAG_NONE )
    {
      MSG_ERROR("ASSERT! Invalid start/end tag or TAG_NONE found in data!", 0, 0, 0);
      
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* If we got here, sms data stored in slot is good! */
    /* "Unpack" meta data before returning sms message data */
    memcpy ( data,
             packed_data + 1,
             (SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2) );
    
    mem_free ( &tmc_heap, packed_data );        
    packed_data = NULL;
    (void) efs_close ( fd );
  }

  return TRUE;

} /* wms_nv_read_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_write_cdma_sms

DESCRIPTION
  Writes the CDMA SMS to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_write_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int       fd = -1;
  fs_ssize_t bytes_written = 0;
  fs_off_t  offset = 0;
  uint8     *packed_data = NULL;
  
  if (index >= WMS_MAX_NV_CDMA_MESSAGES)
  {
    MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  /* Sanity Check */
  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    MSG_ERROR("ASSERT! Could not authenticate storage.  Write failed!!", 0, 0, 0);
    return FALSE;
  }
  
  /* Open message storage */
  fd =  efs_open ( SMS_NV_CD_MSG_STORE_DATA, O_WRONLY );
  
  if ( fd < 0 )
  {
    MSG_ERROR ( "SMS Write failed because sms msg store file" 
                "could not be opened! for item %d", index, 0 , 0 );
    return FALSE;
  }
  else /* File opened successfully for writing ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset =  (fs_off_t) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);
    
    packed_data = (uint8*) mem_malloc ( &tmc_heap, SMS_NV_CD_MSG_STORE_SLOT_SIZ ); 
    
    if ( packed_data == NULL )
    {
      MSG_ERROR("ASSERT! Fatal Error - mem_malloc() returned NULL!", 0, 0, 0);
      (void) efs_close ( fd );
      return FALSE;
    }

    /* "Pack" meta data message before writing to slot */
    packed_data[0] = SMS_NV_CD_MSG_STORE_START_TAG;
    memcpy ( packed_data + 1,
             data,
             SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2 );
    packed_data[SMS_NV_CD_MSG_STORE_SLOT_SIZ-1] = SMS_NV_CD_MSG_STORE_END_TAG;
    
    /* Go to index position in message store before writing ... */
    offset = efs_lseek ( fd, offset, SEEK_SET );
    
    if ( offset == -1 )
    {
      MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }
  
    /* Insert data into its correct slot in file */
    bytes_written =  efs_write ( fd, packed_data, SMS_NV_CD_MSG_STORE_SLOT_SIZ );

    if ( bytes_written != (fs_ssize_t) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR("ASSERT! Unable to write msg to file!", 0, 0, 0);
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Update EFS storage header also! */
    if ( ! wms_nv_cd_msg_store_update_header_file ( index, data[1] ) )
    {
      MSG_ERROR ("ERROR! Unable to update header file with updated info!!", 0, 0, 0);
      (void) efs_close ( fd );
      mem_free ( &tmc_heap, packed_data );
      packed_data = NULL;
      return FALSE;
    }
    
    mem_free ( &tmc_heap, packed_data );
    packed_data = NULL;
    (void) efs_close ( fd );
    
  }

  /* Update Message Information Cache */
  wms_cfg_update_msg_info_cache( WMS_TAG_NONE,
                                 WMS_MEMORY_STORE_NV_CDMA,
                                 index,
                                 data,
                                 WMS_MAX_LEN );
  
  return TRUE;

} /* wms_nv_write_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_replace_cdma_sms

DESCRIPTION
  Replace the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_replace_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{

  /* This call will overwrite previously written data */
  if ( ! wms_nv_write_cdma_sms ( index, data ) )
  {
    MSG_ERROR("wns_nv_replace_cd_sms failed!", 0, 0, 0);
    return FALSE; 
  }
  
  /* Update Message Information Cache */
  wms_cfg_update_msg_info_cache( WMS_TAG_NONE,
                                 WMS_MEMORY_STORE_NV_CDMA,
                                 index,
                                 data,
                                 WMS_MAX_LEN );
  
  return TRUE;

} /* wms_nv_replace_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_delete_cdma_sms

DESCRIPTION
  Deletes the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_delete_cdma_sms
(
  wms_message_index_type   index,
  const uint8            * data
)
{
  int       fd = -1;
  fs_ssize_t bytes_written = 0;
  fs_off_t  offset = 0;
  int8      *clear_data = NULL;

  (void) data;

  if (index >= WMS_MAX_NV_CDMA_MESSAGES)
  {
    MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();
  
  /* Sanity Check */
  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    MSG_ERROR("ASSERT! Could not authenticate storage.  Delete failed!!", 0, 0, 0);
    return FALSE;
  }
  
  /* Open message storage */
  fd =  efs_open ( SMS_NV_CD_MSG_STORE_DATA, O_WRONLY );
  
  if ( fd < 0 )
  {
    MSG_ERROR ( "SMS Delete failed because sms msg store file" 
                "could not be opened! for item %d", index, 0 , 0 );
    return FALSE;
  }
  else /* File opened successfully for deleting msg ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset = (fs_off_t) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);
  
    /* Go to index position in message store before writing ... */
    offset = efs_lseek ( fd, offset, SEEK_SET );
    if ( offset == -1 )
    {
      MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
      (void) efs_close ( fd );
      return FALSE;
    }
    
    /* Allocate memory for slot we want to clear */
    clear_data = (int8*) mem_malloc ( &tmc_heap, SMS_NV_CD_MSG_STORE_SLOT_SIZ );
    if ( clear_data == NULL )
    {
      MSG_ERROR("ASSERT! Fatal Error - mem_malloc() returned NULL!", 0, 0, 0);
      (void) efs_close ( fd );
      return FALSE;
    }

    memset ( clear_data, 0x00, SMS_NV_CD_MSG_STORE_SLOT_SIZ );
          
    /* Clear the slot in file */
    bytes_written =  efs_write ( fd, clear_data, SMS_NV_CD_MSG_STORE_SLOT_SIZ );

    if ( bytes_written != (fs_ssize_t) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR("ASSERT! Unable to clear msg in storage file!", 0, 0, 0);
      mem_free ( &tmc_heap, clear_data );
      clear_data = NULL;
      (void) efs_close ( fd );
      return FALSE;
    }

    /* Update the EFS storage header also */
    if ( ! wms_nv_cd_msg_store_update_header_file ( index, data[0] ) )
    {
      MSG_ERROR ("ERROR! Unable to update header file with updated info!!", 0, 0, 0);
      mem_free ( &tmc_heap, clear_data );
      clear_data = NULL;
      (void) efs_close ( fd );
      return FALSE;
    }
    
    mem_free ( &tmc_heap, clear_data );
    clear_data = NULL; 
    (void) efs_close ( fd );
  }

  return TRUE;

} /* wms_nv_delete_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_read_cdma_params

DESCRIPTION
  Read the CDMA Params from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_cdma_params
(
  wms_message_index_type    index,
  uint8                     * data
)
{
  nv_item_type nvi;
  boolean res = TRUE;
  nv_stat_enum_type   nv_st;

  nvi.sms_cd_parms.address = (uint8)index;

  if( (nv_st=wms_nv_read_wait( NV_SMS_CD_PARMS_I, & nvi )) == NV_DONE_S )
  {
    memcpy( (uint8*) data,
            (void*) nvi.sms_cd_parms.data,
            NV_SMS_CD_PARMS_SIZE );
  }
  else
  {
    if( nv_st != NV_NOTACTIVE_S )
    {
      MSG_ERROR("Read NV_SMS_CD_PARMS_I failed",0,0,0);
    }
    res = FALSE;
  }

  return res;

} /* wms_nv_read_cdma_params() */

/*===========================================================================
FUNCTION wms_nv_write_cdma_params

DESCRIPTION
  Write the CDMA Params to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA params
===========================================================================*/
boolean wms_nv_write_cdma_params
(
  wms_message_index_type    index,
  const uint8                     * data
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  nvi.sms_cd_parms.address = (uint8)index;

  memcpy( (void*) nvi.sms_cd_parms.data,
          (uint8*) data,
          NV_SMS_CD_PARMS_SIZE );

  if( wms_nv_write_wait( NV_SMS_CD_PARMS_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR("Write NV_SMS_CD_PARMS_I failed",0,0,0);
    res = FALSE;
  }

  return res;

} /* wms_nv_write_cdma_params() */

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION wms_nv_read_routing

DESCRIPTION
  Read the Message Routing from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_routing
(
  wms_routes_s_type  *ptr
)
{
  int i;
  nv_item_type nvi;
  boolean res = TRUE;

  if( wms_nv_read_wait( NV_SMS_CFG_ROUTING_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR("Read NV_SMS_CFG_ROUTING_I failed",0,0,0);
    res = FALSE;
  }
  else
  {
    for( i = 0; i<NV_SMS_MAX_ROUTES; i ++ )
    {
      ptr->pp_routes[i].route     = (wms_route_e_type) nvi.sms_cfg_routing.pp_routes[i];
      ptr->pp_routes[i].mem_store = (wms_memory_store_e_type) nvi.sms_cfg_routing.pp_mem_stores[i];

      //TBD bc_routes
    }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  return res;

} /* wms_nv_read_routing() */


/*===========================================================================
FUNCTION wms_nv_write_routing

DESCRIPTION
  Write the Message Routing to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV Message Routing
===========================================================================*/
boolean wms_nv_write_routing
(
  const wms_routes_s_type  *ptr
)
{
  int i;
  nv_item_type nvi;
  boolean res = TRUE;

  for( i = 0; i<NV_SMS_MAX_ROUTES; i ++ )
  {
    nvi.sms_cfg_routing.pp_routes[i]     = (uint8) ptr->pp_routes[i].route;
    nvi.sms_cfg_routing.pp_mem_stores[i] = (uint8) ptr->pp_routes[i].mem_store;

    //TBD bc_routes
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  if( wms_nv_write_wait( NV_SMS_CFG_ROUTING_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR("Read NV_SMS_CFG_ROUTING_I failed",0,0,0);
    res = FALSE;
  }

  return res;

} /* wms_nv_write_routing() */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION WMS_NV_INIT_NVRUIM

DESCRIPTION
  Get certain NV items from the EFS or RUIM depending on NVRUIM's settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_init_nvruim
(
  void
)
{
  nv_item_type nvi;

  /* Read Retry Interval from NV */
  if (wms_nv_read_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi ) == NV_DONE_S)
  {
    msg_s_ptr->mo_retry_interval = nvi.sms_mo_retry_interval;
  }
  else
  {
    msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    nvi.sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi );
  }
  
  /* Read Retry Period from NV */
  {
    if (wms_nv_read_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) == NV_DONE_S)
    {
      msg_s_ptr->mo_retry_period = nvi.sms_mo_retry_period; /* seconds */
    }
    else
    {
      msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD; /* seconds */
      nvi.sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
      (void) wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi );
    }
  }

  MSG_HIGH("Retry Period, Interval = %d s, %d s,", msg_s_ptr->mo_retry_period, 
                                              msg_s_ptr->mo_retry_interval,0);

#ifdef FEATURE_CDSMS 

  /* Read Access or Traffic Channel from RUIM card */
  wms_nv_read_cdma_channel_setting();

  /* Read Service Option Setting from card */
  wms_nv_read_cdma_service_option();

#endif /* FEATURE_CDSMS */

} /* wms_nv_init_nvruim() */

/*===========================================================================
FUNCTION wms_nv_read_bearer_prefs

DESCRIPTION
  Read the Bearer Preferences

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean             res = TRUE;
  nv_stat_enum_type   nv_st;
  int                 i;
  nv_item_type        nvi;

  if( (nv_st=wms_nv_read_wait( NV_SMS_BEARER_PREFS_I, & nvi )) == NV_DONE_S )
  {
    for( i=0; i < WMS_BEARER_MAX; i ++ )
    {
      bearer_prefs[i] = (wms_bearer_usage_e_type) nvi.sms_bearer_prefs[i].nv_sms_bearer_usage;
    }
  }
  else
  {
    MSG_ERROR("Read NV_SMS_BEARER_PREFS_I failed: %d", nv_st, 0, 0);
    res = FALSE;
  }

  return res;

} /* wms_nv_read_bearer_prefs() */

/*===========================================================================
FUNCTION wms_nv_write_bearer_prefs

DESCRIPTION
  Write the Bearer Preferences to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean      res = TRUE;
  int          i;
  nv_item_type nvi;
 
  for( i=0; i < WMS_BEARER_MAX; i++ )
  {
    nvi.sms_bearer_prefs[i].nv_sms_bearer_usage = (uint8) bearer_prefs[i];
  }

  if( wms_nv_write_wait( NV_SMS_BEARER_PREFS_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR("Write NV_SMS_BEARER_PREFS_I failed",0,0,0);
    res = FALSE;
  }

  return res;

} /* wms_nv_write_bearer_prefs() */


/* <EJECT> */
/*===========================================================================
FUNCTION WMS_NV_INIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
void wms_nv_init
(
  void
)
{
  uint32       i;
  nv_item_type nvi;
  struct       fs_stat temp_fs_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_nv_init()",0,0,0);

  cfg_s_ptr = wms_cfg_s_ptr();

  msg_s_ptr = wms_msg_s_ptr();

#ifdef FEATURE_CDSMS
  dc_s_ptr  = wms_dc_s_ptr();
#endif

  /* Initialize WMS NV Critical Section */
  rex_init_crit_sect(&wms_nv_crit_sect);

  /* Check and Create New Directory for SMS if not present */
  if(efs_stat ("/sms",&temp_fs_buf) == -1)
  {
    if (efs_mkdir("/sms", 0777) == -1)
    {
      MSG_ERROR("Cannot make SMS Folder for SMS Messages", 0, 0, 0);
    }
  }


#ifdef FEATURE_CDSMS
  cfg_s_ptr->cdma_voice_mail_index  = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
  cfg_s_ptr->cdma_voice_mail_active = FALSE;

  cfg_s_ptr->nv_cdma_max_slots = WMS_MAX_NV_CDMA_MESSAGES;

  /* Clear the duplicate detection list */
  for(i=0; i<WMS_MESSAGE_LIST_MAX; i++)
  {
#ifdef FEATURE_CDSMS_DUP_DETECTION
    /* Initialize the mask to 0 signifying this slot is empty. */
    cfg_s_ptr->dup_detect[i].cl_mask = 0;
    cfg_s_ptr->dup_detect[i].tl_mask = 0;
#endif /* FEATURE_CDSMS_DUP_DETECTION */
  }

  if ( wms_nv_cd_msg_store_authenticate() )
  {
      /* Read meta information about slots */
      if ( wms_nv_cd_msg_store_read_tags ( ) )
      {
          for( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i ++ )
          {
            if ( cfg_s_ptr->nv_cdma_tags[i] != WMS_TAG_NONE &&
                 wms_nv_read_cdma_sms( i, nv_cdma_data ) )
            {
              /* ======= Structure =========
              ** byte 0:    format
              ** byte 1:    tag/status
              ** byte 2:    len
              ** remainder: data
              ** =========================== */

              wms_cfg_cdma_update_voice_mail_index( (wms_format_e_type) nv_cdma_data[0],  /* format */
                                                    (wms_message_tag_e_type) nv_cdma_data[1],  /* tag */
                                                    nv_cdma_data[2],  /* len */
                                                    nv_cdma_data+3,
                                                    i );
              wms_cfg_update_msg_info_cache( WMS_TAG_NONE,
                                             WMS_MEMORY_STORE_NV_CDMA,
                                             i,
                                             nv_cdma_data,
                                             WMS_MAX_LEN );

              if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
              {
                /* Do the proper decoding of the nv data to update the
                ** duplicate detection cache
                */
                client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                client_message.msg_hdr.tag          = (wms_message_tag_e_type) nv_cdma_data[1];
                cdma_ota.format   = (wms_format_e_type) nv_cdma_data[0];
                cdma_ota.data_len = nv_cdma_data[2];
                memcpy((uint8*)cdma_ota.data, nv_cdma_data+3, cdma_ota.data_len);
                if(wms_ts_decode_CDMA_tl(&cdma_ota, &cdma_tl,
                                         &client_message.u.cdma_message.raw_ts) != WMS_OK_S)
                {
                  MSG_ERROR("Decode tl FAILED, Dup list not updated!",0,0,0);
                }
                else if(wms_ts_decode_CDMA_bd(&client_message.u.cdma_message.raw_ts, FALSE, FALSE,
                                              &cdma_tl.cl_bd) != WMS_OK_S)
                {
                  MSG_ERROR("Decode bd FAILED, Dup list not updated!",0,0,0);
                }
                else
                {
                  /* Update the duplicate detection list */
                  wms_cfg_update_dup_info_cache(WMS_MEMORY_STORE_NV_CDMA,
                                                i,
                                                &cdma_tl);
                }
              }
            }
          }
      }
  }

  /* Init for NV params */
  if( wms_nv_read_cdma_params( 0, nv_cdma_data ) == FALSE ||
      wms_ts_cdma_decode_smsp( nv_cdma_data, NV_SMS_CD_PARMS_SIZE, & cdma_template ) == FALSE )
  {
    /* Write default values to NV params */
    cdma_template.teleservice  = WMS_TELESERVICE_CMT_95;
    cdma_template.mask = 0;
    (void)wms_ts_cdma_encode_smsp( & cdma_template, NV_SMS_CD_PARMS_SIZE, nv_cdma_data );
    if( wms_nv_write_cdma_params( 0, nv_cdma_data ) == FALSE )
    {
      MSG_ERROR("wms_nv_write_cdma_params failed",0,0,0);
    }
  }
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  /* Read Retry Interval from NV */
  if (wms_nv_read_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi ) == NV_DONE_S)
  {
    msg_s_ptr->mo_retry_interval = nvi.sms_mo_retry_interval;
  }
  else
  {
    msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    nvi.sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi );
  }
  
  /* Read Retry Period from NV */
  {
    boolean gcf_flag = FALSE;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    if (gcf_flag == TRUE)
    {
      msg_s_ptr->mo_retry_period = 0;
      MSG_HIGH("Retry Period not set due to GCF Flag Enabled", 0, 0, 0);
    }
    else
    {
      if (wms_nv_read_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) == NV_DONE_S)
      {
        msg_s_ptr->mo_retry_period = nvi.sms_mo_retry_period; /* seconds */
        MSG_HIGH("Read NV Retry Period=%d", nvi.sms_mo_retry_period,0,0);
      }
      else
      {
        msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD; /* seconds */
        nvi.sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
        MSG_ERROR( "Reading NV Retry Period failed, setting it to %d",
                   nvi.sms_mo_retry_period, 0, 0);
        (void) wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi );
      }
    }
    MSG_MED("Retry Period = %d s", msg_s_ptr->mo_retry_period, 0, 0);
  }
  
  /* done with NV init */
  return;

} /* end of wms_nv_init() */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GW_SMS */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_authenticate

DESCRIPTION
  Checks if the SMS storage for CD messages is present in EFS.  If not, it
  creates it.  If presents, it validates the version of the file.

RETURN VALUE
  TRUE successful case
  FAIL failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_authenticate( void )
{
  struct    fs_stat temp_fs_buf;
  uint8     result = TRUE;
  
  MSG_MED ( "wms_nv_cd_msg_store_authenticate()", 0, 0, 0 );
  
  /* Create SMS Message Store if necessary */
  if ( efs_stat ( SMS_NV_CD_MSG_STORE_HEADER, &temp_fs_buf ) == -1  ||
       efs_stat ( SMS_NV_CD_MSG_STORE_DATA, &temp_fs_buf ) == -1 )
  {
    MSG_MED(" SMS CD storage file(s) not found.  Creating them...", 0, 0, 0);
    
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_HEADER);
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_DATA);
    
    result = wms_nv_cd_msg_store_init ();
  }
  /* msg store exists, just validate version information */
  else if ( !wms_nv_cd_msg_store_is_version_valid() )
  {
    MSG_MED(" Version mismatch.  Delete and re-create current version", 0, 0, 0);
    /* TODO: "Convert" versions in the future */
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_HEADER);
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_DATA);

    result = wms_nv_cd_msg_store_init ();
  }
  else
  {
    MSG_MED("SMS_NV_CD_MSG_STORE_DATA authenticated successfully!", 0, 0, 0);
  }

  return result;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_is_version_valid

DESCRIPTION
  Reads the information from the storage file and validates it.

RETURN VALUE
  TRUE/FALSE

===========================================================================*/
static uint8 wms_nv_cd_msg_store_is_version_valid( void )
{
  int   fd = -1;
  uint8 version_info[8];
  fs_ssize_t bytes_read = 0;
  uint32 major_ver = 0;
  uint32 minor_ver = 0;
  
  MSG_MED ( "wms_nv_cd_msg_store_is_version_valid()", 0, 0, 0 );
  
  /* Open header */
  fd =  efs_open ( SMS_NV_CD_MSG_STORE_HEADER, O_RDONLY );
  
  if ( fd < 0 )
  {
    MSG_ERROR ( "Could not open header file", 0, 0 , 0 );
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    bytes_read =  efs_read ( fd, version_info, sizeof ( version_info ) );

    if ( bytes_read != (fs_ssize_t) sizeof ( version_info ) )
    {
      MSG_ERROR("ASSERT! Unable to read version correctly", 0, 0, 0);
      
      (void) efs_close ( fd );
      return FALSE;
    }
    
    major_ver = (version_info[0] << 24) |
                (version_info[1] << 16) |
                (version_info[2] << 8)  |
                 version_info[3];
    
    minor_ver = (version_info[4] << 24) |
                (version_info[5] << 16) |
                (version_info[6] << 8)  |
                 version_info[7];
                
    /* In this implementation, if either version is wrong, return error */
    if ( major_ver != SMS_NV_CD_MSG_STORE_MAJOR_VER ||
         minor_ver != cfg_s_ptr->nv_cdma_max_slots )
    {
      (void) efs_close ( fd );
      return FALSE;

    }
    (void) efs_close ( fd );
  }

  return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_init

DESCRIPTION
  Creates the storage file and initializes all the slots.
  Also creats the header file

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_init ( void )
{
  
  MSG_MED ( "wms_nv_cd_msg_store_init()", 0, 0, 0 );
  
  if ( !wms_nv_cd_msg_store_create_header_file() )
  {
    MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_HEADER);
    return FALSE;
  }
  
  if ( !wms_nv_cd_msg_store_create_data_file() )
  {
    MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
    
    /* Ensure the corrupt files are deleted before returning! */
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_HEADER);
    (void) efs_unlink (SMS_NV_CD_MSG_STORE_DATA); 
    
    return FALSE;
  }

  return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_cd_msg_store_create_header_file

DESCRIPTION
  Creates the storage header file.

RETURN VALUE
  1 successful case
  0 failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_create_header_file ( void )
{
    int       fd = -1;
    uint8     *data = NULL;
    fs_size_t data_size = 0;
    fs_ssize_t bytes_written = 0;
  
    MSG_MED ( "wms_nv_cd_msg_store_create_header_file()", 0, 0, 0 );

    fd = efs_open ( SMS_NV_CD_MSG_STORE_HEADER, O_WRONLY | O_CREAT | O_TRUNC );
    
    if ( fd < 0 )
    {
      /* 
       * Unfortunately cant do much at this point...
       * We will try creating the header before every write operation 
       * so hopefully it will eventually succeed..! 
       */
      MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
      return FALSE;
    }
    else
    {
      /* Calculate file size */
      data_size += 4; /* Major Version */
      data_size += 4; /* Minor Version (number of slots) */
      data_size += 4; /* Individual Slot length */
      data_size += (cfg_s_ptr->nv_cdma_max_slots); /* Meta info. about each slot */
      
      data = (uint8*) mem_malloc ( &tmc_heap, data_size );

      if ( data == NULL )
      {
        MSG_ERROR("ASSERT!  FATAL Error - mem_malloc() returned NULL!", 0, 0, 0);
        return FALSE;
      }

      memset ( data, 0x00, data_size );
    
      /* Set major version */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [0]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0xff000000) >> 24);
      data [1]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x00ff0000) >> 16);
      data [2]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [3]  = (uint8) ((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x000000ff);
      
      /* Set minor version (number of slots) */
      data [4]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0xff000000) >> 24);
      data [5]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0x00ff0000) >> 16);
      data [6]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0x0000ff00) >> 8);
      data [7]  = (uint8) ((cfg_s_ptr->nv_cdma_max_slots) & 0x000000ff);

      /* Set slot length */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [8]  = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0xff000000) >> 24);
      data [9]  = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x00ff0000) >> 16);
      data [10] = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [11] = (uint8) ((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x000000ff);
      
      /* Masks init */
      memset ( data + 12, (int) WMS_TAG_NONE, (cfg_s_ptr->nv_cdma_max_slots) ); 
      
      bytes_written = efs_write ( fd, data, data_size );
        
      if ( bytes_written != (fs_ssize_t) data_size )
      {
         MSG_ERROR("ASSERT! Unable to write correctly to header file!!", 0, 0, 0);
         mem_free ( &tmc_heap, data );
         data = NULL;
         (void) efs_close ( fd );
         return FALSE;
      }
      
      mem_free ( &tmc_heap, data );
      data = NULL;
      (void) efs_close ( fd );
    }
    
    return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_cd_msg_store_create_data_file

DESCRIPTION
  Creates the storage data file and initializes all the slots.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_create_data_file ( void )
{
    int       fd = -1;
    uint32    i = 0;
    int8      *data = NULL;
    fs_size_t data_size = 0;
    fs_ssize_t bytes_written = 0;
 
    MSG_MED ( "wms_nv_cd_msg_store_create_data_file()", 0, 0, 0 );
    
    fd = efs_open ( SMS_NV_CD_MSG_STORE_DATA, O_WRONLY | O_CREAT | O_TRUNC );
    if ( fd < 0 )
    {
      /* 
       * Unfortunately cant do much at this point...
       * We will try creating the storage before every write operation 
       * so hopefully it will eventually succeed..! 
       */
      MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage", 0, 0, 0);
      return FALSE;
      
    }
    else
    {
      data_size = SMS_NV_CD_MSG_STORE_SLOT_SIZ;
      
      data = (int8*) mem_malloc ( &tmc_heap, data_size );

      if ( data == NULL )
      {
        MSG_ERROR("ASSERT! Fatal Error - mem_malloc() returned NULL!", 0, 0, 0);
        (void) efs_close ( fd );
        return FALSE;
      }
      
      memset ( data, (int) WMS_TAG_NONE, data_size );

      for ( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
      {
          bytes_written = efs_write ( fd, data, data_size );
            
          if ( bytes_written != (fs_ssize_t) data_size )
          {
            /* Bad, bad, bad...! */
            /* Somehow we were not able to allocate memory correctly for the storage file! */
            MSG_ERROR("ASSERT! Fatal Error in reserving space for sms storage", 0, 0, 0);
            mem_free ( &tmc_heap, data );
            data = NULL;
            (void) efs_close ( fd );

            return FALSE;
          }
      }
      
      mem_free ( &tmc_heap, data );
      data = NULL;
      (void) efs_close ( fd );

      return TRUE;
    }
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_update_header_file

DESCRIPTION
    Updates the EFS header file with the value of the passed in tag.
    To be called whenever the tag associated with the slot changes.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_update_header_file ( wms_message_index_type index, uint8 tag )
{
    int       fd = -1;
    fs_ssize_t bytes_written = 0;
    fs_off_t  offset = 0;
 
    MSG_MED ( "wms_nv_cd_msg_store_update_header_file()", 0, 0, 0 );
    
    fd = efs_open ( SMS_NV_CD_MSG_STORE_HEADER, O_WRONLY );
    
    if ( fd < 0 )
    {
      /* 
       * Unfortunately cant do much at this point...
       */
      MSG_ERROR("ASSERT! Updating header file failed! ", 0, 0, 0);
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4 /* Slot size */
                 + (fs_off_t) index;
        
        /* Go to index position in message store before writing ... */
        offset = efs_lseek ( fd, offset, SEEK_SET );
        if ( offset == -1 )
        {
          MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
          (void) efs_close ( fd );
          return FALSE;
        }
        
        /* Update tag */
        bytes_written =  efs_write ( fd, &tag, sizeof(tag) );
        
         if ( bytes_written != (fs_ssize_t) sizeof(tag) )
         {
           MSG_ERROR("ASSERT! Unable to update mask!", 0, 0, 0);
           (void) efs_close ( fd );
           return FALSE;
         }

         (void) efs_close ( fd );
    }
    
    return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_read_tags

DESCRIPTION
    Reads in the meta tag values associated with the slots into
    the *global* buffer used to store this for in RAM
    
RETURN VALUE
    TRUE if successful
    FALSE if failed

===========================================================================*/
static uint8 wms_nv_cd_msg_store_read_tags ( void )
{
    int       fd = -1;
    fs_ssize_t bytes_read = 0;
    fs_off_t  offset = 0;
    uint8*    tags = NULL;
    uint32    i;

    MSG_MED ( "wms_nv_cd_msg_store_read_tags()", 0, 0, 0 );

    /* Check for zero slots */
    if (cfg_s_ptr->nv_cdma_max_slots == 0)
    {
      MSG_LOW("Zero NV slots, no further processing needed for CDMA! ", 0, 0, 0);
      return TRUE;
    }
    
    fd = efs_open ( SMS_NV_CD_MSG_STORE_HEADER, O_RDONLY );
    
    if ( fd < 0 )
    {
      MSG_ERROR("ASSERT! Read failed! ", 0, 0, 0);
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4; /* Slot size */
        
        /* Go to start of meta tag information ... */
        offset = efs_lseek ( fd, offset, SEEK_SET );
        if ( offset == -1 )
        {
           MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
           (void) efs_close ( fd );
           return FALSE;
        }
        
        tags = (uint8*) mem_malloc ( &tmc_heap, cfg_s_ptr->nv_cdma_max_slots );
        if (NULL == tags)
        {
           return FALSE;
        }
        memset ( tags, (int) WMS_TAG_NONE, sizeof (tags) );
                
        bytes_read =  efs_read ( fd, tags, cfg_s_ptr->nv_cdma_max_slots );
        
        if ( bytes_read != (fs_ssize_t) cfg_s_ptr->nv_cdma_max_slots )
        {
           MSG_ERROR("ASSERT! Unable read slot meta data correctly!", 0, 0, 0);
           mem_free ( &tmc_heap, tags );
           (void) efs_close ( fd );
           return FALSE;
        }
       
        /* Set the tags correctly in the "global" buffer */
        for( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
        {
            cfg_s_ptr->nv_cdma_tags[i] = (wms_message_tag_e_type) tags[i];
        }
        
        mem_free ( &tmc_heap, tags );
        (void) efs_close ( fd );
    }

    return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_read_cdma_service_option

DESCRIPTION
    Reads NV_SMS_SERVICE_OPTION_I from NV items or RUIM card and map those 
    value to WMS format.
    
RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_service_option(void)
{
   nv_item_type nvi;

  /* Read Service Option from NV */
  if (NV_DONE_S == wms_nv_read_wait( NV_SMS_SERVICE_OPTION_I, &nvi))
  {
     switch(nvi.sms_service_option)
     {
        case 0: 
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
           break;
        case 1:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_6;
           break;
        case 2:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_14;
           break;
        default:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
     }
  }
  else
  {
    /* Write By Default value WMS_DC_SO_6 to NV*/
    dc_s_ptr->default_so_from_nv = WMS_DC_SO_6; 
    nvi.sms_service_option =  1; /* 1 correspond to WMS_DC_SO_6 */
    (void) wms_nv_write_wait( NV_SMS_SERVICE_OPTION_I, &nvi );
  }

  MSG_HIGH("Service Option = %d ",dc_s_ptr->default_so_from_nv,0,0); 
}

/*===========================================================================
FUNCTION wms_nv_read_cdma_channel_setting

DESCRIPTION
    Reads NV_SMS_MO_ON_ACCESS_CHANNEL_I and NV_SMS_MO_ON_TRAFFIC_CHANNEL_I
    from NV items or RUIM card
    
RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_channel_setting(void)
{
   nv_item_type nvi;

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi))
   {
      dc_s_ptr->is_access_channel_enabled = nvi.sms_mo_on_access_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_access_channel_enabled = 1;      
      nvi.sms_mo_on_access_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi );

      MSG_HIGH("Access Channel = %d ",dc_s_ptr->is_access_channel_enabled,0,0);
   }

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi))
   {
      dc_s_ptr->is_traffic_channel_enabled = nvi.sms_mo_on_traffic_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_traffic_channel_enabled = 1;
      nvi.sms_mo_on_traffic_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi );

      MSG_HIGH("Traffic Channel = %d ",dc_s_ptr->is_traffic_channel_enabled,0,0);
   }

   MSG_HIGH("Access Channel = %d & Traffic Channel = %d ",dc_s_ptr->is_access_channel_enabled,
                                                          dc_s_ptr->is_traffic_channel_enabled,0);
}

#endif /* FEATURE_CDSMS */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */


#else // FEATURE_ONEMSG_USE_ONEFILE

/*==============================================================================
//           W I R E L E S S   M E S S A G I N G   S E R V I C E S
//           --- EFS Operations
// 文件:
//        wmsnv.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//                 
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include <memory.h>
#include <stdio.h>
#include "wmsi.h"
#include "wmsnv.h"
#include "wmscfg.h"
#include "wmsmsg.h"
#include "wmsts.h"
#include "err.h"
#include "nv.h"
#include "task.h"
#include "tmc.h"
#include "fs_public.h"
#include "wmstscdma.h"
#include "OEMWMS_CacheInfo.h"
#include "wmsmsgi.h"
#include "wmstsi.h"
#include "wmsdc.h"

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
// 相关文件格式请见 《短信存取设计方案2.ppt》
#define WMS_NV_MSG_STORE_VER                0x04

#define SMS_NV_CD_MSG_STORE_DIR             "/wms"
// Files used for storage in EFS
#define SMS_NV_CD_MSG_STORE_HEADER          "/wms/msgheader"
#define SMS_NV_CD_MSG_STORE_DATA            "/wms/msgdata"

#define SMS_NV_STORE_MSGFILEFMT             "/wms/msg%03d"
/*==============================================================================
                                 
                                 本地全局变量定义
                                 
==============================================================================*/
    
#ifdef FEATURE_CDSMS
static wms_dc_s_type                *dc_s_ptr;
#endif /* FEATURE_CDSMS */

static wms_cfg_s_type               *cfg_s_ptr;
static wms_msg_s_type               *msg_s_ptr;

/* Critical Section for WMS accessing NV */
rex_crit_sect_type                  wms_nv_crit_sect;

static uint8                        nv_cdma_data[NV_SMS_DATA_SIZ];
static wms_cdma_template_s_type     cdma_template;
static wms_OTA_message_type         cdma_ota;
static wms_tl_message_type          cdma_tl;
static wms_client_message_s_type    client_message;

// NV command buffer
static nv_cmd_type                   nvcmd;           
/*==============================================================================

                                 函数声明
                                 
==============================================================================*/
static boolean wms_nv_cd_msg_store_authenticate(void);
static boolean wms_nv_cd_msg_store_is_version_valid(void);
static boolean wms_nv_cd_msg_store_init(void);
static uint8 wms_nv_cd_msg_store_create_header_file(void);
static boolean wms_nv_cd_msg_store_create_data_file(void);
static uint8 wms_nv_cd_msg_store_update_header_file(wms_message_index_type index, uint8 tag);
static uint8 wms_nv_cd_msg_store_read_tags(void);


/*==============================================================================

                                 函数定义
                                 
==============================================================================*/
/*==============================================================================
FUNCTION 
    WMS_NV_WAIT

DESCRIPTION
    Wait on NV actions.  Handle signals as they come in just in case NV
    takes a while.  Only time critical watchdog signals are acknowledged.

DEPENDENCIES
    There is a command sent to NV, and this task will be signalled.  There
    are no other outstanding NV requests from this task.

RETURN VALUE
    None

SIDE EFFECT
    Some signals are delayed till we're done with NV

==============================================================================*/
void wms_nv_wait(void)
{
    rex_sigs_type  sigs;

    while (1)
    {   /* until signal back from NV */
        sigs = rex_wait( WMS_NV_SIG | WMS_RPT_TIMER_SIG );

        if (sigs & WMS_RPT_TIMER_SIG)
        {
            /* right now SMS and CM are sharing the same task! */
            wms_kick_dog(); 
        }

        if (sigs & WMS_NV_SIG)
        {
            (void)rex_clr_sigs(rex_self(), WMS_NV_SIG);
            break;
        }
    }

    return;
} /* end of wms_nv_wait() */


/*==============================================================================
FUNCTION 
    WMS_NV_WRITE_WAIT

DESCRIPTION
    Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
    The NV Return Code

DEPENDENCIES
    This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
    till we're done, and it's only called from one task.
    
==============================================================================*/
nv_stat_enum_type wms_nv_write_wait(nv_items_enum_type item,  
                                    nv_item_type       *data_ptr)
{
    nv_stat_enum_type  ret_status;
    
    /* Entering WMS NV Critical Section */
    rex_enter_crit_sect(&wms_nv_crit_sect);
    
    /* set up NV item */
    nvcmd.tcb_ptr    = rex_self();        /* Notify this task when done */
    nvcmd.sigs       = WMS_NV_SIG;
    nvcmd.done_q_ptr = NULL;
    
    nvcmd.cmd        = NV_WRITE_F;
    nvcmd.item       = item;             /* item to write */
    nvcmd.data_ptr   = data_ptr;        /* the data to write */
    
    
    /* Clear the signal, call NV, wait for it to finish */
    (void) rex_clr_sigs(rex_self(), WMS_NV_SIG);
    nv_cmd(&nvcmd);
    wms_nv_wait();
    
    ret_status = nvcmd.status;
    
    /* Leaving WMS NV Critical Section */
    rex_leave_crit_sect(&wms_nv_crit_sect);

    if (ret_status != NV_DONE_S)
    {
        ERR("NV Write Failed Item %d Code %d", item, ret_status, 0);
        /* Set the global NV cmd err appropriately */
        if (ret_status == NV_FULL_S || ret_status == NV_NOMEM_S)
        {
            cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
        }
        else
        {
            cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
    }
    else
    {
        /* Set the global NV cmd err to OK */
        cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_NONE;
    }
    
    return( ret_status );
} /* end of wms_nv_write_wait() */

/*==============================================================================
FUNCTION 
    WMS_NV_READ_WAIT

DESCRIPTION
    Get an item from the nonvolatile memory.  Handles nonactive items by
    providing a default value.

RETURN VALUE
    The NV return code, except for NV_NOTACTIVE_S, which is handled
    internally.

DEPENDENCIES
    This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
    till we're done, and it's only called from one task.
==============================================================================*/
nv_stat_enum_type wms_nv_read_wait(nv_items_enum_type item, 
                                   nv_item_type       *data_ptr)
{
    nv_stat_enum_type  ret_status;
    
    /* Entering WMS NV Critical Section */
    rex_enter_crit_sect(&wms_nv_crit_sect);
    
    /* set up NV item */
    nvcmd.tcb_ptr    = rex_self();       /* notify this task when done */
    nvcmd.sigs       = WMS_NV_SIG;
    nvcmd.done_q_ptr = NULL;
    
    nvcmd.cmd        = NV_READ_F;        /* command */
    nvcmd.item       = item;             /* item to read */
    nvcmd.data_ptr   = data_ptr;         /* where to put result */
    
    /* Clear the return signal, call NV, and wait for a response */
    (void) rex_clr_sigs(rex_self(), WMS_NV_SIG);
    nv_cmd( &nvcmd );
    wms_nv_wait();
    
    ret_status = nvcmd.status;
    
    /* Leaving WMS NV Critical Section */
    rex_leave_crit_sect(&wms_nv_crit_sect);
    
    /* Handle a NV_NOTACTIVE_S or NV_FAIL_S status internally by replacing
    ** the random data returned with a default value of our own.
    */
    if (ret_status == NV_NOTACTIVE_S || ret_status == NV_FAIL_S)
    {
        nv_item_type nvi; 
        
        ret_status = NV_DONE_S;
        
        switch (item)
        {
            case NV_SMS_CD_PARMS_I:
                memset((void*) nvi.sms_cd_parms.data, 0, NV_SMS_CD_PARMS_SIZE);
                ret_status = wms_nv_write_wait(NV_SMS_CD_PARMS_I, & nvi);
                break;

#ifdef FEATURE_BROADCAST_SMS
            case NV_SMS_BC_CONFIG_I:
                data_ptr->sms_bc_config = (uint8) WMS_BC_CONFIG_DISALLOW;
                break;

            case NV_SMS_BC_USER_PREF_I:
                data_ptr->sms_bc_user_pref = (uint8)WMS_BC_PREF_DEACTIVATE;
                break;

            case NV_SMS_BC_SERVICE_TABLE_SIZE_I:
                data_ptr->sms_bc_service_table_size = 0 ;
                break;

            case NV_SMS_BC_SERVICE_TABLE_I:
                data_ptr->sms_bc_service_table.active_service = FALSE;
                break;
#endif /* FEATURE_BROADCAST_SMS */

            case NV_SMS_MO_RETRY_INTERVAL_I:
                data_ptr->sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
                (void) wms_nv_write_wait( item, data_ptr );
                break;
      
            case NV_SMS_MO_RETRY_PERIOD_I:
                data_ptr->sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
                (void) wms_nv_write_wait( item, data_ptr );
                break;

            /* If it's an unknown item, print an info message and pass on the */
            /* NV_NOTACTIVE_S status. */
            default:
                MSG_HIGH("Nonactive NV item 0x%x", item, 0, 0);
                ret_status = NV_NOTACTIVE_S;
                break;
        }

#ifdef FEATURE_BROADCAST_SMS
        /* write back the default values to NV */
        MSG_HIGH("Write BC default value to NV, item=%d", item,0,0);
        if (item == NV_SMS_BC_CONFIG_I ||
            item == NV_SMS_BC_USER_PREF_I ||
            item == NV_SMS_BC_SERVICE_TABLE_SIZE_I ||
            item == NV_SMS_BC_SERVICE_TABLE_I)
        {
            (void) wms_nv_write_wait( item, data_ptr );
        }
#endif /* FEATURE_BROADCAST_SMS */
    }
    else
    {
        if (ret_status != NV_DONE_S)
        {
            ERR("NV Read Failed Item %d Code %d", item, ret_status, 0);
        }
    }
    
    return(ret_status);
} /* end of wms_nv_read_wait() */

/*==============================================================================
FUNCTION 
    wms_nv_read_cdma_sms

DESCRIPTION
    Read the CDMA SMS from NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    None
==============================================================================*/
boolean wms_nv_read_cdma_sms(wms_message_index_type  index, uint8  *data)
{
    int             fd = -1;
    fs_ssize_t      bytes_read = 0;
    fs_off_t        offset = 0;
    uint8           *packed_data = NULL;
    
    if (index >= WMS_MAX_NV_CDMA_MESSAGES)
    {
        MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
        return FALSE;
    }
    
    /* Kick the Watchdog timer */
    wms_kick_dog();
    
    if (!wms_nv_cd_msg_store_authenticate())
    {
        MSG_ERROR("ASSERT! Could not authenticate storage.  Read failed!!", 0, 0, 0);
        return FALSE;
    }    
    
    /* Read the contents from EFS. Open message storage file*/
    fd =  efs_open(SMS_NV_CD_MSG_STORE_DATA, O_RDONLY);
  
    if (fd < 0)
    {
        MSG_ERROR ("open data file failed!", index, 0 , 0);
        goto read_error;
    }
    else /* File opened successfully for reading ... */
    {
        offset = (fs_off_t) index*(NV_SMS_DATA_SIZ);
        packed_data = (uint8*) mem_malloc(&tmc_heap, NV_SMS_DATA_SIZ); 
        
        if (packed_data == NULL)
        {
            MSG_ERROR("ASSERT! Fatal Error - mem_malloc() returned NULL!", 0, 0, 0);
            (void) efs_close(fd);
            goto read_error;
        }
    
        /* Go to index position in message store before reading ... */
        offset = efs_lseek(fd, offset, SEEK_SET);
 
        if (offset == -1)
        {
            MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
            
            (void) efs_close(fd);
            goto read_error;
        }
    
        /* Read data from its correct slot in file */
        bytes_read =  efs_read(fd, packed_data, NV_SMS_DATA_SIZ);

        if (bytes_read != (fs_ssize_t)NV_SMS_DATA_SIZ)
        {
            MSG_ERROR("ASSERT! Unable to read msg correctly from file!", 0, 0, 0);
            
            (void) efs_close(fd);
            goto read_error;
        }

        /* Check if data is valid */
        if (packed_data[1] == (uint8) WMS_TAG_NONE)
        {
            MSG_ERROR("ASSERT! Invalid start/end tag or TAG_NONE found in data!", 0, 0, 0);
            
            (void) efs_close(fd);
            goto read_error;
        }

        /* If we got here, sms data stored in slot is good! */
        /* "Unpack" meta data before returning sms message data */
        memcpy(data, packed_data, NV_SMS_DATA_SIZ);
    
        (void) efs_close(fd);
    }

    mem_free(&tmc_heap, packed_data);
    return TRUE;
  
read_error:
    if (NULL != packed_data)
    {
        mem_free(&tmc_heap, packed_data);
    }
    
    return FALSE;
} /* wms_nv_read_cdma_sms() */

/*==============================================================================
FUNCTION 
    wms_nv_write_cdma_sms

DESCRIPTION
    Writes the CDMA SMS to NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    Modified NV CDMA SMS
==============================================================================*/
boolean wms_nv_write_cdma_sms(wms_message_index_type   index, uint8 *data)
{
    int         fd = -1;
    fs_ssize_t  bytes_written = 0;
    fs_off_t    offset = 0;
    
    if (index >= WMS_MAX_NV_CDMA_MESSAGES)
    {
        MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
        return FALSE;
    }

    /* Kick the Watchdog timer */
    wms_kick_dog();

    /* Sanity Check */
    if (!wms_nv_cd_msg_store_authenticate())
    {
        MSG_ERROR("ASSERT! Could not authenticate storage.  Write failed!!", 0, 0, 0);
        return FALSE;
    }
    
    /* Open message storage file*/
    fd =  efs_open(SMS_NV_CD_MSG_STORE_DATA, O_WRONLY);
  
    if (fd < 0)
    {
        MSG_ERROR ( "Message store file could not be opened!", 0, 0 , 0);
        goto write_error;
    }
    else /* File opened successfully for writing ... */
    {
        offset =  (fs_off_t) index * (NV_SMS_DATA_SIZ);
        
        /* Go to index position in message store before writing ... */
        offset = efs_lseek(fd, offset, SEEK_SET);
    
        if (offset == -1)
        {
            MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
            (void) efs_close(fd);
            goto write_error;
        }
  
        // Write message
        bytes_written =  efs_write(fd, data, NV_SMS_DATA_SIZ);

        if (bytes_written != (fs_ssize_t) NV_SMS_DATA_SIZ)
        {
            MSG_ERROR("ASSERT! Unable to write msg to file!", 0, 0, 0);
            (void) efs_close(fd);
            goto write_error;
        }

        /* Update EFS storage header also! */
        if (!wms_nv_cd_msg_store_update_header_file(index, data[1]))
        {
            MSG_ERROR ("ERROR! Unable to update header file with updated info!!", 0, 0, 0);
            (void) efs_close(fd);
            goto write_error;
        }
    
        (void) efs_close(fd);
    }

#if 0    
    /* Update Message Information Cache */
    wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                  WMS_MEMORY_STORE_NV_CDMA,
                                  index,
                                  data,
                                  WMS_MAX_LEN);
#endif
  
    return TRUE;

write_error:
    return FALSE;
} /* wms_nv_write_cdma_sms() */

/*==============================================================================
FUNCTION 
    wms_nv_replace_cdma_sms

DESCRIPTION
    Replace the CDMA SMS from NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    Modified NV CDMA SMS
==============================================================================*/
boolean wms_nv_replace_cdma_sms(wms_message_index_type   index, uint8 *data)
{
  /* This call will overwrite previously written data */
    if (!wms_nv_write_cdma_sms(index, data))
    {
        MSG_ERROR("wns_nv_replace_cd_sms failed!", 0, 0, 0);
        return FALSE; 
    }
  
    return TRUE;
} /* wms_nv_replace_cdma_sms() */

/*==============================================================================
FUNCTION 
    wms_nv_delete_cdma_sms

DESCRIPTION
    Deletes the CDMA SMS from NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    Modified NV CDMA SMS
    不实际删除短信存储文件，只需更新短信的头文件内相应 tag
==============================================================================*/
boolean wms_nv_delete_cdma_sms(wms_message_index_type  index, const uint8 *data)
{
    if (index >= WMS_MAX_NV_CDMA_MESSAGES)
    {
        MSG_ERROR("Message Index %d Out of Bounds", index, 0, 0);
        return FALSE;
    }

    /* Kick the Watchdog timer */
    wms_kick_dog();
  
    /* Sanity Check */
    if (!wms_nv_cd_msg_store_authenticate())
    {
        MSG_ERROR("ASSERT! Could not authenticate storage.  Delete failed!!", 0, 0, 0);
        return FALSE;
    }
    
    /* Update the EFS storage header also */
    if (!wms_nv_cd_msg_store_update_header_file(index, data[1]))
    {
        return FALSE;
    }
    
    return TRUE;
} /* wms_nv_delete_cdma_sms() */

/*==============================================================================
FUNCTION 
    wms_nv_read_cdma_params

DESCRIPTION
    Read the CDMA Params from NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    None
==============================================================================*/
boolean wms_nv_read_cdma_params(wms_message_index_type    index, uint8  * data)
{
    nv_item_type nvi;
    boolean res = TRUE;
    nv_stat_enum_type   nv_st;
    
    nvi.sms_cd_parms.address = (uint8)index;

    if ((nv_st=wms_nv_read_wait(NV_SMS_CD_PARMS_I, & nvi)) == NV_DONE_S)
    {
        memcpy((uint8*) data, (void*) nvi.sms_cd_parms.data, NV_SMS_CD_PARMS_SIZE);
    }
    else
    {
        if (nv_st != NV_NOTACTIVE_S)
        {
            MSG_ERROR("Read NV_SMS_CD_PARMS_I failed",0,0,0);
        }
        
        res = FALSE;
    }
    
    return res;
} /* wms_nv_read_cdma_params() */

/*==============================================================================
FUNCTION 
    wms_nv_write_cdma_params

DESCRIPTION
    Write the CDMA Params to NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    Modified NV CDMA params
==============================================================================*/
boolean wms_nv_write_cdma_params(wms_message_index_type index, const uint8 *data)
{
    nv_item_type nvi;
    boolean res = TRUE;
    
    nvi.sms_cd_parms.address = (uint8)index;
    
    memcpy((void*) nvi.sms_cd_parms.data, (uint8*)data, NV_SMS_CD_PARMS_SIZE);
    
    if (wms_nv_write_wait( NV_SMS_CD_PARMS_I, & nvi) != NV_DONE_S)
    {
        MSG_ERROR("Write NV_SMS_CD_PARMS_I failed",0,0,0);
        res = FALSE;
    }
    
    return res;
} /* wms_nv_write_cdma_params() */


/*==============================================================================
FUNCTION 
    wms_nv_read_routing

DESCRIPTION
    Read the Message Routing from NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    None
==============================================================================*/
boolean wms_nv_read_routing(wms_routes_s_type  *ptr)
{
    int i;
    nv_item_type nvi;
    boolean res = TRUE;
    
    if (wms_nv_read_wait(NV_SMS_CFG_ROUTING_I, & nvi) != NV_DONE_S)
    {
        MSG_ERROR("Read NV_SMS_CFG_ROUTING_I failed",0,0,0);
        res = FALSE;
    }
    else
    {
        for (i = 0; i<NV_SMS_MAX_ROUTES; i ++)
        {
            ptr->pp_routes[i].route     = (wms_route_e_type) nvi.sms_cfg_routing.pp_routes[i];
            ptr->pp_routes[i].mem_store = (wms_memory_store_e_type) nvi.sms_cfg_routing.pp_mem_stores[i];
            
            //TBD bc_routes
        }
    }
    
    return res;
} /* wms_nv_read_routing() */


/*==============================================================================
FUNCTION 
    wms_nv_write_routing

DESCRIPTION
    Write the Message Routing to NV

RETURN VALUE
    TRUE / FALSE

DEPENDENCIES
    Modified NV Message Routing
==============================================================================*/
boolean wms_nv_write_routing(const wms_routes_s_type  *ptr)
{
    int i;
    nv_item_type nvi;
    boolean res = TRUE;
    
    for (i = 0; i<NV_SMS_MAX_ROUTES; i ++)
    {
        nvi.sms_cfg_routing.pp_routes[i]     = (uint8) ptr->pp_routes[i].route;
        nvi.sms_cfg_routing.pp_mem_stores[i] = (uint8) ptr->pp_routes[i].mem_store;
        
        //TBD bc_routes
    }
    
    if (wms_nv_write_wait(NV_SMS_CFG_ROUTING_I, & nvi ) != NV_DONE_S)
    {
        MSG_ERROR("Read NV_SMS_CFG_ROUTING_I failed",0,0,0);
        res = FALSE;
    }
    
    return res;
} /* wms_nv_write_routing() */

#ifdef FEATURE_OMH_SMS
/*===========================================================================
FUNCTION WMS_NV_INIT_NVRUIM

DESCRIPTION
  Get certain NV items from the EFS or RUIM depending on NVRUIM's settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_init_nvruim
(
  void
)
{
  nv_item_type nvi;

  /* Read Retry Interval from NV */
  if (wms_nv_read_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi ) == NV_DONE_S)
  {
    msg_s_ptr->mo_retry_interval = nvi.sms_mo_retry_interval;
  }
  else
  {
    msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    nvi.sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi );
  }
  
  /* Read Retry Period from NV */
  {
    if (wms_nv_read_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) == NV_DONE_S)
    {
      msg_s_ptr->mo_retry_period = nvi.sms_mo_retry_period; /* seconds */
    }
    else
    {
      msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD; /* seconds */
      nvi.sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
      (void) wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi );
    }
  }
  MSG_HIGH("Retry Period = %d s", msg_s_ptr->mo_retry_period, 0, 0);
#ifdef FEATURE_CDSMS 
  
  /* Read Access or Traffic Channel from RUIM card */
  wms_nv_read_cdma_channel_setting();
  
  /* Read Service Option Setting from card */
  wms_nv_read_cdma_service_option();
  
#endif /* FEATURE_CDSMS */
} /* wms_nv_init_nvruim() */
#endif /* FEATURE_OMH_SMS */


/*===========================================================================
FUNCTION wms_nv_read_bearer_prefs

DESCRIPTION
  Read the Bearer Preferences

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean             res = TRUE;
  nv_stat_enum_type   nv_st;
  int                 i;
  nv_item_type        nvi;

  if( (nv_st=wms_nv_read_wait( NV_SMS_BEARER_PREFS_I, & nvi )) == NV_DONE_S )
  {
    for( i=0; i < WMS_BEARER_MAX; i ++ )
    {
      bearer_prefs[i] = (wms_bearer_usage_e_type) nvi.sms_bearer_prefs[i].nv_sms_bearer_usage;
    }
  }
  else
  {
    MSG_ERROR("Read NV_SMS_BEARER_PREFS_I failed: %d", nv_st, 0, 0);
    res = FALSE;
  }

  return res;

} /* wms_nv_read_bearer_prefs() */

/*===========================================================================
FUNCTION wms_nv_write_bearer_prefs

DESCRIPTION
  Write the Bearer Preferences to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean      res = TRUE;
  int          i;
  nv_item_type nvi;
 
  for( i=0; i < WMS_BEARER_MAX; i++ )
  {
    nvi.sms_bearer_prefs[i].nv_sms_bearer_usage = (uint8) bearer_prefs[i];
  }

  if( wms_nv_write_wait( NV_SMS_BEARER_PREFS_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR("Write NV_SMS_BEARER_PREFS_I failed",0,0,0);
    res = FALSE;
  }

  return res;

} /* wms_nv_write_bearer_prefs() */

/*==============================================================================
FUNCTION 
    WMS_NV_INIT

DESCRIPTION
    Get an item from the nonvolatile memory.  Handles nonactive items by
    providing a default value.

RETURN VALUE
    The NV return code, except for NV_NOTACTIVE_S, which is handled
    internally.

DEPENDENCIES
    This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
    till we're done, and it's only called from one task.
==============================================================================*/
void wms_nv_init(void)
{
    uint32       i;
    nv_item_type nvi;
    struct       fs_stat temp_fs_buf;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    MSG_HIGH("wms_nv_init()",0,0,0);

    cfg_s_ptr = wms_cfg_s_ptr();

    msg_s_ptr = wms_msg_s_ptr();
    
#ifdef FEATURE_CDSMS
    dc_s_ptr  = wms_dc_s_ptr();
#endif
    /* Initialize WMS NV Critical Section */
    rex_init_crit_sect(&wms_nv_crit_sect);

    /* Check and Create New Directory for SMS if not present */
    if (efs_stat(SMS_NV_CD_MSG_STORE_DIR,&temp_fs_buf) == -1)
    {
        if (efs_mkdir(SMS_NV_CD_MSG_STORE_DIR, 0777) == -1)
        {
            MSG_ERROR("Cannot make wms Folder for WMS Messages", 0, 0, 0);
        }
    }


#ifdef FEATURE_CDSMS
    cfg_s_ptr->cdma_voice_mail_index  = (wms_message_index_type)WMS_VM_MESSAGE_INDEX;
    //cfg_s_ptr->cdma_voice_mail_index  = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    cfg_s_ptr->cdma_voice_mail_active = FALSE;

    cfg_s_ptr->nv_cdma_max_slots = WMS_MAX_NV_CDMA_MESSAGES;

    /* Clear the duplicate detection list */
#ifdef FEATURE_CDSMS_DUP_DETECTION
    for (i=0; i<WMS_MESSAGE_LIST_MAX; i++)
    {
        /* Initialize the mask to 0 signifying this slot is empty. */
        cfg_s_ptr->dup_detect[i].cl_mask = 0;
        cfg_s_ptr->dup_detect[i].tl_mask = 0;
    }
#endif /* FEATURE_CDSMS_DUP_DETECTION */

    if (wms_nv_cd_msg_store_authenticate())
    {
        /* Read meta information about slots */
        if (wms_nv_cd_msg_store_read_tags())
        {
            for (i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++)
            {
                if ((cfg_s_ptr->nv_cdma_tags[i] != WMS_TAG_NONE) &&
                    wms_nv_read_cdma_sms(i, nv_cdma_data))
                {
                    /* ======= Structure =========
                    ** byte 0:    format
                    ** byte 1:    tag/status
                    ** byte 2:    len
                    ** remainder: data
                    ** =========================== */
                    if (i==0)
                    {// 第一条记录固定为语音短信
                        wms_cfg_cdma_update_voice_mail_index((wms_format_e_type) nv_cdma_data[0],
                                                             (wms_message_tag_e_type) nv_cdma_data[1],  /* tag */
                                                             nv_cdma_data[2],  /* len */
                                                             &nv_cdma_data[3],
                                                             i);
                    }
                    //else
                    {
                        wms_cfg_update_msg_info_cache((wms_message_tag_e_type) nv_cdma_data[1],
                                                      WMS_MEMORY_STORE_NV_CDMA,
                                                      i,
                                                      nv_cdma_data,
                                                      WMS_MAX_LEN);
                    }

                    if (WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
                    {
                        /* Do the proper decoding of the nv data to update the
                        ** duplicate detection cache
                        */
                        client_message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                        client_message.msg_hdr.tag          = (wms_message_tag_e_type) nv_cdma_data[1];
                        cdma_ota.format   = (wms_format_e_type) nv_cdma_data[0];
                        cdma_ota.data_len = nv_cdma_data[2];
                        memcpy((uint8*)cdma_ota.data, nv_cdma_data+3, cdma_ota.data_len);
                        
                        if (wms_ts_decode_CDMA_tl(&cdma_ota, &cdma_tl,
                                        &client_message.u.cdma_message.raw_ts) != WMS_OK_S)
                        {
                            MSG_ERROR("Decode tl FAILED, Dup list not updated!",0,0,0);
                        }
                        else if (wms_ts_decode_CDMA_bd(&client_message.u.cdma_message.raw_ts, FALSE, FALSE,
                                        &cdma_tl.cl_bd) != WMS_OK_S)
                        {
                            MSG_ERROR("Decode bd FAILED, Dup list not updated!",0,0,0);
                        }
                        else
                        {
                            /* Update the duplicate detection list */
                            wms_cfg_update_dup_info_cache(WMS_MEMORY_STORE_NV_CDMA, i, &cdma_tl);
                        }
                    }
                }
            }
        }
    }

    /* Init for NV params */
    if ((wms_nv_read_cdma_params(0, nv_cdma_data ) == FALSE) ||
        (wms_ts_cdma_decode_smsp(nv_cdma_data, NV_SMS_CD_PARMS_SIZE, & cdma_template) == FALSE))
    {
        /* Write default values to NV params */
        cdma_template.teleservice  = WMS_TELESERVICE_CMT_95;
        cdma_template.mask = 0;
        (void)wms_ts_cdma_encode_smsp(&cdma_template, NV_SMS_CD_PARMS_SIZE, nv_cdma_data);
        if (wms_nv_write_cdma_params(0, nv_cdma_data) == FALSE)
        {
            MSG_ERROR("wms_nv_write_cdma_params failed",0,0,0);
        }
    }
#endif /* FEATURE_CDSMS */

    /* Read Retry Interval from NV */
    if (wms_nv_read_wait(NV_SMS_MO_RETRY_INTERVAL_I, &nvi) == NV_DONE_S)
    {
        msg_s_ptr->mo_retry_interval = nvi.sms_mo_retry_interval;
    }
    else
    {
        msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
        nvi.sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
        (void) wms_nv_write_wait(NV_SMS_MO_RETRY_INTERVAL_I, &nvi);
    }

    /* Read Retry Period from NV */
    {
        boolean gcf_flag = FALSE;

        if (gcf_flag == TRUE)
        {
            msg_s_ptr->mo_retry_period = 0;
            MSG_HIGH("Retry Period not set due to GCF Flag Enabled", 0, 0, 0);
        }
        else
        {
            if (wms_nv_read_wait(NV_SMS_MO_RETRY_PERIOD_I, &nvi) == NV_DONE_S)
            {
                msg_s_ptr->mo_retry_period = nvi.sms_mo_retry_period; /* seconds */
            }
            else
            {
                msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD; /* seconds */
                nvi.sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
                (void) wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi);
            }
        }
        MSG_MED("Retry Period = %d s", msg_s_ptr->mo_retry_period, 0, 0);
    }

    /* done with NV init */
    return;

} /* end of wms_nv_init() */

#ifdef FEATURE_CDSMS
/*==============================================================================
FUNCTION wms_nv_cd_msg_store_authenticate

DESCRIPTION
  Checks if the SMS storage for CD messages is present in EFS.  If not, it
  creates it.  If presents, it validates the version of the file.

RETURN VALUE
  TRUE successful case
  FAIL failure case

==============================================================================*/
static boolean wms_nv_cd_msg_store_authenticate(void)
{
    struct    fs_stat temp_fs_buf;
    boolean   result = TRUE;
    
    MSG_MED ("wms_nv_cd_msg_store_authenticate()", 0, 0, 0);
    
    /* Create SMS Message Store if necessary */
    if ((efs_stat(SMS_NV_CD_MSG_STORE_HEADER, &temp_fs_buf) == -1) ||
        (efs_stat(SMS_NV_CD_MSG_STORE_DATA, &temp_fs_buf) == -1))
    {
        MSG_MED(" WMS CD storage file(s) not found.  Creating them...", 0, 0, 0);
        
        (void)efs_unlink(SMS_NV_CD_MSG_STORE_HEADER);
        (void)efs_unlink(SMS_NV_CD_MSG_STORE_DATA);
        
        (void) efs_unlink ("/sms/nv_cd_msg_header");
        (void) efs_unlink ("/sms/nv_cd_msg_data");
        (void) efs_rmdir ("/sms");
        
        result = wms_nv_cd_msg_store_init();
    }
    /* msg store exists, just validate version information */
    else if (!wms_nv_cd_msg_store_is_version_valid())
    {
        MSG_MED(" Version mismatch.  Delete and re-create current version", 0, 0, 0);
        /* TODO: "Convert" versions in the future */
        (void) efs_unlink (SMS_NV_CD_MSG_STORE_HEADER);
        (void)efs_unlink(SMS_NV_CD_MSG_STORE_DATA);
        
        (void) efs_unlink ("/sms/nv_cd_msg_header");
        (void) efs_unlink ("/sms/nv_cd_msg_data");
        (void) efs_rmdir ("/sms");
        
        result = wms_nv_cd_msg_store_init();
    }
    else
    {
        MSG_MED("SMS_NV_CD_MSG_STORE_DATA authenticated successfully!", 0, 0, 0);
    }
    
    return result;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_is_version_valid

DESCRIPTION
    Reads the information from the storage file and validates it.

RETURN VALUE
    TRUE/FALSE

==============================================================================*/
static boolean wms_nv_cd_msg_store_is_version_valid(void)
{
    int         fd = -1;
    uint8       version_info;
    fs_ssize_t  bytes_read = 0;
    
    MSG_MED("wms_nv_cd_msg_store_is_version_valid()", 0, 0, 0);
    
    /* Open header */
    fd =  efs_open(SMS_NV_CD_MSG_STORE_HEADER, O_RDONLY);
  
    if (fd<0)
    {
        MSG_ERROR("Could not open header file", 0, 0 , 0);
        return FALSE;
    }
    else /* File opened successfully for reading ... */
    {
        bytes_read =  efs_read(fd, &version_info, sizeof(version_info));
        
        if (bytes_read != (fs_ssize_t) sizeof(version_info))
        {
            MSG_ERROR("ASSERT! Unable to read version correctly", 0, 0, 0);
            
            (void) efs_close(fd);
            return FALSE;
        }
    
        /* In this implementation, if either version is wrong, return error */
        if (WMS_NV_MSG_STORE_VER != version_info)
        {
            (void) efs_close(fd);
            return FALSE;
        }
        (void) efs_close(fd);
    }
    
    return TRUE;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_init

DESCRIPTION
    Creates the storage file and initializes all the slots.
    Also creats the header file

RETURN VALUE
    TRUE successful case
    FALSE failure case

==============================================================================*/
static boolean wms_nv_cd_msg_store_init(void)
{
    MSG_MED("wms_nv_cd_msg_store_init()", 0, 0, 0);
    
    if (!wms_nv_cd_msg_store_create_header_file())
    {
        MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
        (void) efs_unlink(SMS_NV_CD_MSG_STORE_HEADER);
        return FALSE;
    }
    
    if (!wms_nv_cd_msg_store_create_data_file())
    {
        MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
        (void) efs_unlink(SMS_NV_CD_MSG_STORE_HEADER);
        (void) efs_unlink(SMS_NV_CD_MSG_STORE_DATA);
        return FALSE;
    }
    
    return TRUE;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_create_header_file

DESCRIPTION
    Creates the storage header file.

RETURN VALUE
    1 successful case
    0 failure case

==============================================================================*/
static uint8 wms_nv_cd_msg_store_create_header_file(void)
{
    int         fd = -1;
    uint8       *data = NULL;
    fs_size_t   data_size = 0;
    fs_ssize_t  bytes_written = 0;
    uint16      nMaxItems = WMS_MAX_NV_CDMA_MESSAGES;
    boolean     bRet = FALSE;
  
    MSG_MED ("wms_nv_cd_msg_store_create_header_file()", 0, 0, 0);

    fd = efs_open (SMS_NV_CD_MSG_STORE_HEADER, O_WRONLY | O_CREAT | O_TRUNC);
    
    if (fd < 0)
    {
        MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage Header", 0, 0, 0);
        return bRet;
    }
    else
    {
        data_size = nMaxItems*sizeof(wms_message_tag_e_type)+1+sizeof(uint16);
        
        data = (uint8*) mem_malloc(&tmc_heap, data_size);
        
        if (data == NULL)
        {
            MSG_ERROR("ASSERT!  FATAL Error - mem_malloc() returned NULL!", 0, 0, 0);
            return bRet;
        }
        
        memset(data, 0x00, data_size);
        data[0] = WMS_NV_MSG_STORE_VER;
        memcpy(&data[1],&nMaxItems,sizeof(nMaxItems));
        
        // 写文件
        bytes_written = efs_write(fd, data, data_size);
    
        if (data_size != bytes_written)
        {
            MSG_ERROR("ASSERT! Unable to write correctly to header file!!", 0, 0, 0);
        }
        else
        {
            bRet = TRUE;
        }
      
        mem_free (&tmc_heap, data);
        data = NULL;
        (void) efs_close (fd);
    }
    
    return bRet;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_create_data_file

DESCRIPTION
    Creates the storage header file.

RETURN VALUE
    TRUE successful case
    FALSE failure case

==============================================================================*/
static boolean wms_nv_cd_msg_store_create_data_file(void)
{
    int         fd = -1;
    int         i = 0;
    uint8       *data = NULL;
    fs_ssize_t  bytes_written = 0;
  
    MSG_MED ("wms_nv_cd_msg_store_create_data_file()", 0, 0, 0);

    fd = efs_open (SMS_NV_CD_MSG_STORE_DATA, O_WRONLY | O_CREAT | O_TRUNC);
    
    if (fd < 0)
    {
        MSG_ERROR("ASSERT!  Fatal Error...cannot make SMS Message Storage", 0, 0, 0);
        return FALSE;
    }
    else
    {
        data = (uint8*) mem_malloc(&tmc_heap, NV_SMS_DATA_SIZ);
        
        if (data == NULL)
        {
            MSG_ERROR("ASSERT!  FATAL Error - mem_malloc() returned NULL!", 0, 0, 0);
            return FALSE;
        }
        
        memset(data, 0x00, NV_SMS_DATA_SIZ);
        
        // 写空白数据文件
        for (i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++)
        {
            bytes_written = efs_write(fd, data, NV_SMS_DATA_SIZ);
        
            if (bytes_written != NV_SMS_DATA_SIZ)
            {
                MSG_ERROR("ASSERT! Unable to write correctly to data file!!", 0, 0, 0);
                mem_free (&tmc_heap, data);
                (void) efs_close (fd);
                
                return FALSE;
            }
        }
        
        mem_free (&tmc_heap, data);
        data = NULL;
        (void) efs_close (fd);
    }
    
    return TRUE;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_update_header_file

DESCRIPTION
    Updates the EFS header file with the value of the passed in tag.
    To be called whenever the tag associated with the slot changes.

RETURN VALUE
    TRUE successful case
    FALSE failure case

==============================================================================*/
static uint8 wms_nv_cd_msg_store_update_header_file(wms_message_index_type index, uint8 tag)
{
    int         fd = -1;
    fs_ssize_t  bytes_written = 0;
    fs_off_t    offset = 0;
 
    MSG_MED("wms_nv_cd_msg_store_update_header_file()", 0, 0, 0);
    
    fd = efs_open(SMS_NV_CD_MSG_STORE_HEADER, O_WRONLY);
    
    if (fd<0)
    {
        MSG_ERROR("ASSERT! Updating header file failed! ", 0, 0, 0);
        return FALSE;
    }
    else
    {
        offset = 1   /* Version Size */
                 + sizeof(uint16) /* Max Items */
                 + (fs_off_t) index*sizeof(wms_message_tag_e_type);
        
        /* Go to index position in message store before writing ... */
        offset = efs_lseek(fd, offset, SEEK_SET);
        if (offset == -1) 
        {
            MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
            (void) efs_close(fd);
            return FALSE;
        }
        
        /* Update tag */
        bytes_written =  efs_write(fd, &tag, sizeof(tag));
        
        if (bytes_written != (fs_ssize_t)sizeof(tag))
        {
            MSG_ERROR("ASSERT! Unable to update mask!", 0, 0, 0);
            (void) efs_close(fd);
            return FALSE;
        }
        
        (void) efs_close(fd);
    }
    
    return TRUE;
}

/*==============================================================================
FUNCTION 
    wms_nv_cd_msg_store_read_tags

DESCRIPTION
    Reads in the meta tag values associated with the slots into
    the *global* buffer used to store this for in RAM
    
RETURN VALUE
    TRUE if successful
    FALSE if failed

==============================================================================*/
static uint8 wms_nv_cd_msg_store_read_tags(void)
{
    int         fd = -1;
    fs_ssize_t  bytes_read = 0;
    fs_off_t    offset = 0;
    uint8       *tags = NULL;
    uint32      i;
    
    MSG_MED("wms_nv_cd_msg_store_read_tags()", 0, 0, 0);
    
    /* Check for zero slots */
    if (cfg_s_ptr->nv_cdma_max_slots == 0)
    {
        MSG_LOW("Zero NV slots, no further processing needed for CDMA! ", 0, 0, 0);
        return TRUE;
    }
    
    fd = efs_open(SMS_NV_CD_MSG_STORE_HEADER, O_RDONLY);
    
    if (fd<0)
    {
        MSG_ERROR("ASSERT! Read failed! ", 0, 0, 0);
        return FALSE;
    }
    else
    {
        offset = 1   /* Version Size */
                 + sizeof(uint16); /* Max Items */

        /* Go to start of meta tag information ... */
        offset = efs_lseek(fd, offset, SEEK_SET);
        if (offset == -1)
        {
            MSG_ERROR("ASSERT! efs_lseek() failed!", 0, 0, 0);
            (void) efs_close(fd);
            return FALSE;
        }

        tags = (uint8*)mem_malloc(&tmc_heap, cfg_s_ptr->nv_cdma_max_slots);
        memset(tags, 0, cfg_s_ptr->nv_cdma_max_slots);

        bytes_read = efs_read(fd, tags, cfg_s_ptr->nv_cdma_max_slots);

        if (bytes_read != (fs_ssize_t)cfg_s_ptr->nv_cdma_max_slots)
        {
            MSG_ERROR("ASSERT! Unable read slot meta data correctly!", 0, 0, 0);
            mem_free(&tmc_heap, tags);
            (void) efs_close(fd);
            return FALSE;
        }

        /* Set the tags correctly in the "global" buffer */
        for (i = 0; i<cfg_s_ptr->nv_cdma_max_slots; i++)
        {
            cfg_s_ptr->nv_cdma_tags[i] = (wms_message_tag_e_type)tags[i];
        }

        mem_free (&tmc_heap, tags);
        (void) efs_close(fd);
    }

    return TRUE;
}


/*===========================================================================
FUNCTION wms_nv_read_cdma_service_option

DESCRIPTION
    Reads NV_SMS_SERVICE_OPTION_I from NV items or RUIM card and map those 
    value to WMS format.
    
RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_service_option(void)
{
   nv_item_type nvi;

  /* Read Service Option from NV */
  if (NV_DONE_S == wms_nv_read_wait( NV_SMS_SERVICE_OPTION_I, &nvi))
  {
     switch(nvi.sms_service_option)
     {
        case 0: 
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
           break;
        case 1:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_6;
           break;
        case 2:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_14;
           break;
        default:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
     }
  }
  else
  {
    /* Write By Default value WMS_DC_SO_6 to NV*/
    dc_s_ptr->default_so_from_nv = WMS_DC_SO_6; 
    nvi.sms_service_option =  1; /* 1 correspond to WMS_DC_SO_6 */
    (void) wms_nv_write_wait( NV_SMS_SERVICE_OPTION_I, &nvi );
  }

  MSG_HIGH("Service Option = %d ",dc_s_ptr->default_so_from_nv,0,0); 
}

#ifdef FEATURE_OEMOMH
#include "gsdi.h"
/*===========================================================================
FUNCTION gstk_is_sms_pp_supported

DESCRIPTION
  Utility function to check if SMS PP Download is allowed/supported

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean wms_is_sms_supported(void)
{
    gsdi_svr_rsp_type                srv_available = {0};
    
    /* Check if SIM Service Table included SMS-PP service */
    srv_available = gsdi_lib_is_service_available(GSDI_SMS);

    if (GSDI_SUCCESS == srv_available.gsdi_status &&
        0 != srv_available.svr_bitmap)
    {
      MSG_FATAL("wms_is_sms_supported TRUE",0,0,0);
      return TRUE;
    }
    MSG_FATAL("wms_is_sms_supported FALSE",0,0,0);
    return FALSE;
} /* gstk_is_sms_pp_supported */
#endif

/*===========================================================================
FUNCTION wms_nv_read_cdma_channel_setting

DESCRIPTION
    Reads NV_SMS_MO_ON_ACCESS_CHANNEL_I and NV_SMS_MO_ON_TRAFFIC_CHANNEL_I
    from NV items or RUIM card
    
RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_channel_setting(void)
{
   nv_item_type nvi;
   
#ifdef FEATURE_OEMOMH
   if(!wms_is_sms_supported())
   {
      dc_s_ptr->is_access_channel_enabled  = 1;
      dc_s_ptr->is_traffic_channel_enabled = 1;
      return;
   }
#endif

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi))
   {
#ifndef FEATURE_OEMOMH
   	  if (nvi.sms_mo_on_access_channel!=1)
	  {
		/* Set to Default Value & Write to NV items */
		dc_s_ptr->is_access_channel_enabled = 1;      
		nvi.sms_mo_on_access_channel = 1;

		(void) wms_nv_write_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi );

		MSG_HIGH("Access Channel = %d ",dc_s_ptr->is_access_channel_enabled,0,0);
	  }
#endif
      dc_s_ptr->is_access_channel_enabled = nvi.sms_mo_on_access_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_access_channel_enabled = 1;      
      nvi.sms_mo_on_access_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi );

      MSG_HIGH("Access Channel = %d ",dc_s_ptr->is_access_channel_enabled,0,0);
   }

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi))
   {
#ifndef FEATURE_OEMOMH
   	  if (nvi.sms_mo_on_traffic_channel!=1)
	  {
		  dc_s_ptr->is_traffic_channel_enabled = 1;
		  nvi.sms_mo_on_traffic_channel = 1;
		  
		  (void) wms_nv_write_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi );
		  
		  MSG_HIGH("Traffic Channel = %d ",dc_s_ptr->is_traffic_channel_enabled,0,0);

	  }
#endif
      dc_s_ptr->is_traffic_channel_enabled = nvi.sms_mo_on_traffic_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_traffic_channel_enabled = 1;
      nvi.sms_mo_on_traffic_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi );

      MSG_HIGH("Traffic Channel = %d ",dc_s_ptr->is_traffic_channel_enabled,0,0);
   }
   
   MSG_HIGH("Access Channel = %d & Traffic Channel = %d ",dc_s_ptr->is_access_channel_enabled,
                                                          dc_s_ptr->is_traffic_channel_enabled,0);
}

#endif /* FEATURE_CDSMS */

#endif // FEATURE_ONEMSG_USE_ONEFILE
