/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



            G S D I   C D M A   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions that provide CDMA specific procedures
  for accessing a CDMA compliant RUIM.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdicdma.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/09   ssr     Fixed for CDMA Phase check during RUIM initialization
06/10/09   kk      Added service table check before sending down SELECT
05/22/09   js      Added gsdi support for SELECT command for RUIM
02/03/09   ssr     Removed GSDI_CDMA_POST_PIN1_INIT fucntion
11/19/08   nb      Klockwork Fixes
10/09/08   tml     Fixed compilation warnings
10/06/08   jk      Fixed compile warnings
09/05/08   ssr     Fixed typo error
03/04/08   nk      Added Support for FDN Rehabilitate
11/12/07   vs      Fixed compiler warnings
11/08/07   vs/nk   Added EUIMID feature support
04/26/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/07/07   sun     Fixed Compilation
02/21/07   sun     Remove GSDI Global PIN
01/17/07   sp      Removed pin_just_verifed flag
08/23/06   sun     Fixed Lint Errors
07/07/06   sun     Removed Refernce to mc.h
02/14/06   nk      Merged for ARR caching and by path change
11/09/05   sun     Fixed lint errors
10/18/05   tml     Added in_synch info to the cache
07/18/05   jk      Fixed Compile Issues for 1x Targets
06/10/05   tml     Added 256 MMGSDI support
05/11/05   sst     Lint fixes
03/23/05   jk      Changed gsdi_cdma_post_pin1_init so that it doesn't fail when
                   R-UIM rejects store ESN_ME command
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
12/27/04   jk      Fixed compiler warnings.
11/15/04   jar     Code cleanup to use macros & dual slot merge
10/18/04   sk      Added definition for function gsdi_cdma_proc_ruim_search.
04/02/04   tml     Fixed get pin status for slot 2 issue
03/31/04   jar     Fix for single Mode CDMA Only RUIM with NO GSM DF Available
03/12/04   jar     Added Independent Slot Initialization Support for Dual Slot
                   Targets.
03/01/04   tml     Added rec num accessed support for write
01/25/04   tml     Fixed dual slot refresh issues
01/24/04   tml     Dual slot refresh support
12/20/03   tml     Added RUIM support for FDN enabling and disabling
12/15/03   tml     Compilation fix for RUIM_W_GSM_ACCESS
12/08/03   jar     Fixed problem with CDMA DF Check in gsdi_cdma_post_pin1_init()
                   function which prevented STORE ESN ME from occurring.
                   Lint changes.
12/04/03   tml     lint
09/18/03   jar     gsdi get_pin_status fix to avoid defaulting to GSDI_PIN_
                   DISABLED for Single Mode RUIM Only Cards.
09/07/03   jar     Removed Dual Slot ERR_FATALS & Dual Slot Code Cleanup.
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
06/19/03   jar     Set the EF Category in global GSDI Data for
                   get_file_attribute requests.
06/04/03   jar     Fixed Compile error for FEATURE_GSTK Defined but
                   FEATURE_UIM_TOOLKIT_UTK Not defined.
05/20/03   tml     linted
05/17/03   jar     Added support for FEATURE_UIM_RUIM_W_GSM_ACCESS
04/30/03   tml     Added image support
04/30/03   tml     Removed uim power control when the ruim is proactive
04/21/03   tml     Properly featurized UTK support
04/15/03   tml     Evaluate ERR usage
04/10/03   jar     Added function gsdi_cdma_is_icc_cdma_df_present
                   and gsdi_cdma_is_icc_dcs1800_df_present.
02/24/03   jar     Added Missing Header/Revision Information.
02/04/03   jar     Changed RUIM_ESN_USAGE_MASK to GSDI_RUIM_ESN_USAGE_MASK
01/24/03   jar     More LTK RUIM Support Changes
01/02/03   jar     LTK RUIM Support Merge
11/26/02   jar     Renamed gsdi_cdma_proc_get_sim_capabilities to
                   gsdi_cdma_proc_get_ruim_capabilities. Added usage of
                   GSDI_RUIM_STORE_ESN_USE_UIM_ID and
                   GSDI_RUIM_STORE_ESN_LENGTH
10/07/02   tml     Added check for caching of pin status and pin id.  When
                   GSDI_PIN1 and GSDI_PIN2 have been cached, the get pin status
                   function returns the gsdi_data global data instead of going
                   all the way down to the uim driver
10/07/02   jar     Moved read of CDMA Service Table in gsdi_cdma_post_pin1_init
                   inside of FEATURE_UIM_UTK
10/05/02   jar     Vote UIM On with UIM_PROACTIVE_UIM.  Set UIM Info for
                   Proactive SIM Present.
09/25/02   tml     Added check for disable_chv1_allowed in
                   gsdi_cdma_proc_get_sim_capabilities
                   Called gsdi_cdma_proc_get_sim_capabilities in
                   gsd_cdma_post_pin1_init
09/19/02   wes     Added FEATURE_UIM_RUIM around diag_otasp_update function call
09/04/02   jar     Changed mc_get_esn() to tmc_get_stored_esn().
09/03/02   jar     Changed instances of GSDI_CDMA_SST_MASK to GSDI_CDMA_SST_
                   TOOLKIT_MASK.
08/30/02  jar/tml  Replace gs_status_T with gsdi_returns_T.  Removed usage
                   of gsdi_nvruim_set_esn_usage until full effect or zero'ing
                   ESN is determined.  Changed serval instances of sst to cst.
08/21/02  jar/tml  Add Service table read in gsdi_cdma_post_pin1_init in order
                   to populate ruim cache
08/21/02   jar     Added mc and nvruimi to #include list.  Modified gsdi_cdma_
                   post_pin1_init to support RUIM INIT Procedures and Terminal
                   profile download.  Added functions gsdi_cdma_test_for_dfs,
                   gsdi_nvruim_set_df_preset, gsdi_nvruim_set_esn_usage.
08/16/02   tml     Modifed gsdi_cdma_proc_get_pin_status to return number
                   of retries for PINs/PUKs.
07/28/02   jar     Created file

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "rex.h"
#include "memheap.h"
#include "msg.h"
#include "task.h"
#include "dsm.h"
#include "err.h"
#include "tmc.h"
#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM && FEATURE_MMGSDI_UMTS */


#include "gsdi_exp.h"
#include "gsdi.h"

#if defined (FEATURE_MMGSDI_CDMA)
#include "nvruimi.h"
#include "diag.h"
#include "mmgsdi_uim_icc.h"
#endif /* FEATURE_MMGSDI_CDMA */


#ifdef FEATURE_UIM
#include "uim.h"
#endif

#if defined (FEATURE_UIM_EUIMID)
#include "gsdi_nv.h"
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_OEMOMH
gsdi_uim_omh_cap_type gsdi_uim_omh_cap;
#endif


#if defined (FEATURE_MMGSDI_CDMA)

#include "mmgsdiutil.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static  byte file_attributes_p[sizeof(gsdi_file_attributes_T)];


/* ----------------------------------------------------------------------------
   FUNCTION:      GSDI_CDMA_REHAB_CDMA_FDN_PROC_FILE

   DESCRIPTION:
   This function checks if IMSI_M, IMSI_T or TMSI are invalidated if fdn is supported
   and re-habilitates them if necessary.

   DEPENDENCIES:
     Should only be called if FDN is enabled.

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   IMSI_M, IMSI_T, and TMSI are re-habilitated conditionally

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type gsdi_cdma_rehab_cdma_fdn_proc_file(
  mmgsdi_file_enum_type      sim_filename,
  mmgsdi_slot_id_enum_type   slot
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_INIT_CDMA_FILE

DESCRIPTION
  This function calls gsdi_cdma_proc_ruim_read based on the passed in file name,
  index mode, slot id and populates the result into the passed in data pointer
  This function also print out error message when the return status for the
  read is not GSDI_SUCCESS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_init_cdma_file(
    uim_items_enum_type file,
    byte index_mode,
    gsdi_slot_id_type  slot,
    byte * data)
{
    byte                   actual_data_len = 0;
    byte                   num_records;
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    gsdi_sim_read_req_T    *read_req;
    byte                   rec_num_accessed;


    if (data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("Null input", 0, 0, 0);
        return GSDI_ERROR;
    }

    read_req = mmgsdi_malloc(sizeof (gsdi_sim_read_req_T));

    if (read_req == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR ("Can't obtain memory", 0, 0, 0);
        return GSDI_ERROR;
    }

    memset(read_req, 0x00, sizeof(gsdi_sim_read_req_T));

    /* populate necessary info */
    read_req->sim_filename      = (word)file;
    read_req->index_mode        = (gsdi_index_modes_T)index_mode;
    read_req->rec_num           = 0;
    read_req->search_string_len = 0;
    read_req->required_data_len = GSDI_STANDARD_READ_LEN;
    read_req->data_offset       = 0;
    read_req->message_header.slot = slot;


    gsdi_status = gsdi_cdma_proc_ruim_read( read_req,
                                          &rec_num_accessed,
                                          &num_records,
                                          &actual_data_len,
                                          data );

    if (gsdi_status != GSDI_SUCCESS)
    {
        if ( gsdi_status == GSDI_NOT_SUPPORTED )
        {
            MSG_HIGH("NO SST BIT for CDMA file 0x%x (%d): status %d",
                      file, file,gsdi_status);
        }
        else if ( gsdi_status == GSDI_NOT_FOUND )
        {
            MSG_HIGH("CDMA file 0x%x (%d) not found: status %d",
                      file, file,gsdi_status);
        }
        else
        {
            MMGSDI_DEBUG_ERR("Invalid CDMA file 0x%x (%d) status: %d",
                file, file, gsdi_status);
        }
    }

    /* Free up the read req memory */
    MMGSDIUTIL_TMC_MEM_FREE(read_req);

    return gsdi_status;
} /* gsdi_init_cdma_file */

/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_ruim_read(
  gsdi_sim_read_req_T *  req_p,
  byte *                 rec_num_accessed_p,
  byte *                 num_records_p,
  byte *                 returned_data_len_p,
  byte *                 data_p )
{
    gsdi_returns_T       gsdi_status = GSDI_SUCCESS;
    boolean              file_is_ok_in_sst = TRUE;
    uim_items_enum_type  EF_to_read;
    boolean              pref_slot = TRUE;
    gsdi_slot_id_type    new_slot;

    MSG_MED("GSDI CDMA read proc",0,0,0);

    EF_to_read = (uim_items_enum_type) req_p->sim_filename;
    MSG_HIGH("  SIM read EF %x",EF_to_read,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref(req_p->message_header.slot, GSDI_CDMA, &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check status of EF in cached SST (SIM Service Table) */
    file_is_ok_in_sst = gsdi_util_is_file_ok_in_cst( EF_to_read, pref_slot );
    if ( file_is_ok_in_sst )
    {
        /* Attempt to read from cache */
        gsdi_status = gsdi_util_read_ruim_cache( EF_to_read,
                                                 pref_slot,
                                                 req_p->required_data_len,
                                                 req_p->data_offset,
                                                 returned_data_len_p,
                                                 data_p );
        if ( gsdi_status != GSDI_SUCCESS )
        {
            /* That didn't work, probably because the file is
               not in the cache, so read it from the SIM card */
            /* Reset status */
            gsdi_status = GSDI_SUCCESS;

            /* Read the EF */
            gsdi_status = gsdi_uim_server_cdma_read ( req_p,
                                                      rec_num_accessed_p,
                                                      num_records_p,
                                                      returned_data_len_p,
                                                      data_p );
            if (gsdi_status == GSDI_ERROR)
                MSG_HIGH("Read status %d",gsdi_status,0,0);
            else
                MSG_HIGH("Read seek status %d",gsdi_status,0,0);

            /* Copy the data into cache */
            if ( gsdi_status == GSDI_SUCCESS )
            {
                gsdi_status = gsdi_util_write_ruim_cache( EF_to_read,
                                            pref_slot,
                                            returned_data_len_p[0],
                                            req_p->data_offset,
                                            data_p,
                                            GSDI_DATA_FROM_CARD);
                if (gsdi_status != GSDI_SUCCESS) {
                  MSG_HIGH("Write RUIM Cache Failed after read", 0, 0, 0);
                  gsdi_status = GSDI_SUCCESS;
                }

            }
        }
    } /* file_ok_in_sst */
    else
    {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }

    MSG_MED("GSDI CDMA read proc status %d",gsdi_status,0,0);
    return gsdi_status;
} /* gsdi_cdma_proc_ruim_read */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_IMAGE

DESCRIPTION
  get image data in the CDMA card

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_get_image (
   gsdi_get_image_req_T *req_p,
   int32                *returned_data_length,
   uint8                *data_p
)
{
    gsdi_returns_T       gsdi_status = GSDI_SUCCESS;
    uint8                rec_num_accessed;
    uint8                num_records;
    gsdi_slot_id_type    new_slot;

    MSG_MED("GSDI cdma get image proc",0,0,0);

    /* check if the slot requested is the preferred slot */
    (void) gsdi_util_is_slot_pref(req_p->message_header.slot, GSDI_CDMA, &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Read the EF */
    gsdi_status = gsdi_uim_server_cdma_get_image (req_p,
                                                  &rec_num_accessed,
                                                  &num_records,
                                                  returned_data_length,
                                                  data_p );

    if (gsdi_status == GSDI_ERROR)
    {
        MSG_HIGH("Get Image status %d",gsdi_status,0,0);
    }

    MSG_MED("GSDI cdma get image proc status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_get_image */


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_WRITE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_ruim_write (
    gsdi_sim_write_req_T *  req_p,
    byte *                  rec_num_accessed_p
)
{
    /*lint -esym(715,rec_num_accessed_p)    */
    /*Originally put in for Seek Command    */
    /*for Seek and Write functionality,     */
    /*which is currently disabled for RUIM  */

    boolean               file_is_ok_in_sst = TRUE;
    gsdi_returns_T        gsdi_status = GSDI_SUCCESS;
    uim_items_enum_type   EF_to_write;
    boolean               pref_slot = TRUE;
    gsdi_slot_id_type     new_slot;

    MSG_MED("GSDI CDMA write proc",0,0,0);

    EF_to_write = (uim_items_enum_type) req_p->sim_filename;

    MSG_HIGH("  RUIM write EF %x",EF_to_write,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref(req_p->message_header.slot, GSDI_CDMA, &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check status of EF in cached SST (RUIM Service Table) */
    file_is_ok_in_sst = gsdi_util_is_file_ok_in_cst( EF_to_write, pref_slot );

    if ( file_is_ok_in_sst ) {

        /* Has a seek been requested ? */
 //       if ( req_p->search_string_len > 0 ) {
 //
 //           gsdi_status = gsdi_uim_server_cdma_seek( EF_to_write,
 //                                                   req_p->index_mode,
 //                                                   req_p->search_string,
 //                                                   req_p->search_string_len,
 //                                                   &rec_num_len,
 //                                                   rec_num_accessed_p);
 //           if ( gsdi_status == GSDI_SUCCESS ) {
//               req_p->rec_num = *rec_num_accessed_p;
//            }
 //       }

        /* Write to the EF */
        if ( gsdi_status == GSDI_SUCCESS ) {

            gsdi_status = gsdi_uim_server_cdma_write(req_p,
                                                     rec_num_accessed_p);

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("Write status %d",gsdi_status,0,0);
            }
        } else {
           MSG_HIGH("Write seek status %d",gsdi_status,0,0);
        }

        /* Copy the data into cache */
        if ( gsdi_status == GSDI_SUCCESS ) {

            gsdi_status = gsdi_util_write_ruim_cache( EF_to_write,
                                        pref_slot,
                                        req_p->data_len,
                                        req_p->data_offset,
                                        req_p->data_buffer,
                                        GSDI_DATA_FROM_CARD);
            if (gsdi_status != GSDI_SUCCESS) {
              MSG_HIGH("Write to cache faile after write", 0, 0, 0);
              gsdi_status = GSDI_SUCCESS;
            }

        }

    } else {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }

    MSG_MED("GSDI CDMA write proc status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_ruim_write */

/*===========================================================================
FUNCTION GSDI_CDMA_PROC_CHANGE_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_change_pin (
    gsdi_change_pin_req_T *  req_p,
    byte *                   data_p
)
{
  gsdi_returns_T  gsdi_status     = GSDI_SUCCESS;

  MSG_MED("GSDI CDMA change pin proc",0,0,0);

  gsdi_status = gsdi_uim_server_cdma_change_chv(req_p->message_header.slot,
                                                (gsdi_pin_ids_T)req_p->pin_id,
                                                req_p->old_pin,
                                                req_p->new_pin,
                                                data_p );

  MSG_MED("GSDI CDMA change pin proc status %d",gsdi_status,0,0);
  return gsdi_status;

} /* gsdi_cdma_proc_change_pin */

/*===========================================================================
FUNCTION GSDI_CDMA_PROC_DISABLE_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_disable_pin (
    gsdi_disable_pin_req_T *  req_p,
    byte *                    data_p
)
{
  gsdi_returns_T    gsdi_status      = GSDI_SUCCESS;
  byte              dummy_byte       = 0;
  byte              cst_byte         = 0;
  boolean           pref_slot        = TRUE;
  gsdi_slot_id_type new_slot         = GSDI_SLOT_NONE;

  MSG_MED("GSDI CDMA disable pin proc",0,0,0);

  /*
  ** IF PIN DISABLING IS ALLOWED BY THIS SIM CARD, DISABLE IT.
  ** CHECK THE CDMA Service Table (CST).
  */

  /* check if the slot requested is the preferred slot */
  pref_slot = gsdi_util_is_slot_pref(req_p->message_header.slot, GSDI_CDMA, &new_slot);

  gsdi_status = gsdi_util_read_ruim_cache( UIM_CDMA_CDMA_SVC_TABLE,
                                            pref_slot,
                                            1,
                                            CST_PIN_DISABLING_OFFSET,
                                            &dummy_byte,
                                            &cst_byte );

  /* Need to add this if statement for SIM/UMTS Also */
  if ( gsdi_status == GSDI_SUCCESS )
  {
    if ( ( cst_byte & CST_PIN_DISABLING_MASK ) == CST_PIN_DISABLING_MASK )
    {
      /*
      ** DISABLE PIN
      */
      gsdi_status = gsdi_uim_server_cdma_disable_chv(req_p->message_header.slot,
                                                     (gsdi_pin_ids_T)req_p->pin_id,
                                                     req_p->pin,
                                                     data_p );
    }
    else
    {
      /*
      ** THIS SIM CARD DOES NOT ALLOW PIN DISABLING,
      ** SO DON'T ATTEMPT TO DO IT
      */
      gsdi_status = GSDI_NOT_SUPPORTED;
    }
  }
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

  MSG_MED("GSDI CDMA disable pin proc status %d",gsdi_status,0,0);
  return gsdi_status;

} /* gsdi_cdma_proc_disable_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_ENABLE_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_enable_pin (
    gsdi_enable_pin_req_T *  req_p,
    byte *                   data_p
)
{
    gsdi_returns_T  gsdi_status     = GSDI_SUCCESS;

    MSG_MED("GSDI CDMA enable pin proc",0,0,0);

    gsdi_status = gsdi_uim_server_cdma_enable_chv( req_p->message_header.slot,
                                                   (gsdi_pin_ids_T)req_p->pin_id,
                                                   req_p->pin,
                                                   data_p );

    MSG_MED("GSDI cdma enable pin proc status %d",gsdi_status,0,0);
    return gsdi_status;
} /* gsdi_cdma_proc_enable_pin */



/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_ENABLE_FDN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_enable_fdn (
  gsdi_enable_fdn_req_T *  req_p
)
{
    gsdi_returns_T         gsdi_status = GSDI_SUCCESS;
    gsdi_slot_id_type      new_slot    = GSDI_SLOT_NONE;
    boolean                file_is_ok_in_cst = FALSE;
    boolean                pref_slot = TRUE;

    MSG_MED("GSDI cdma enable fdn proc",0,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref( req_p->message_header.slot,
                                        GSDI_CDMA,
                                        &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check whether ADN EF is allocated & activated in SST */
    file_is_ok_in_cst = gsdi_util_is_file_ok_in_cst( UIM_TELECOM_ADN,
                                                     pref_slot );

    if ( file_is_ok_in_cst )
    {
        /* Verify PIN-2 */
        if ( req_p->pin2_present )
        {
            gsdi_status = gsdi_uim_server_cdma_verify_chv( req_p->message_header.slot,
                                                           GSDI_PIN2,
                                                           req_p->pin2,
                                                           NULL );
        }
        /* Invalidate ADN EF */
        if ( gsdi_status == GSDI_SUCCESS )
        {
            gsdi_status = gsdi_uim_server_cdma_invalidate(req_p->message_header.slot,
                                                          UIM_TELECOM_ADN );
            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_data.sim_capability.fdn_enabled = TRUE;
                if (req_p->message_header.slot == GSDI_SLOT_1)
                {
                    if(gsdi_util_send_mmi_sim_ind( GSDI_FDN_ENABLE ) != GSDI_SUCCESS)
                        MSG_HIGH("Not able to send warning GSDI_FDN_ENABLE", 0, 0, 0);
                    else
                        MSG_HIGH("Warning indication GSDI_FDN_ENABLE",0,0,0);
                }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                else
                {
                    if(gsdi_util_send_mmi_sim_ind( GSDI_FDN_ENABLE_2 ) != GSDI_SUCCESS)
                        MSG_HIGH("Not able to send warning GSDI_FDN_ENABLE_2", 0, 0, 0);
                    else
                        MSG_HIGH("Warning indication GSDI_FDN_ENABLE_2",0,0,0);
                }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
            }
        }
    }
    else
    {
        gsdi_status = GSDI_SUCCESS;
    }

    MSG_MED("GSDI cdma enable fdn proc status %d",gsdi_status,0,0);

    return gsdi_status;


} /* gsdi_cdma_proc_enable_fdn */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_DISABLE_FDN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_disable_fdn (
  gsdi_disable_fdn_req_T *  req_p
)
{
    gsdi_returns_T           gsdi_status       = GSDI_SUCCESS;
    gsdi_slot_id_type        new_slot          = GSDI_SLOT_NONE;
    boolean                  file_is_ok_in_cst = TRUE;
    boolean                  pref_slot         = TRUE;

    MSG_MED("GSDI cdma disable fdn proc",0,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref( req_p->message_header.slot,
                                        GSDI_CDMA,
                                        &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check whether ADN EF is allocated & activated in SST */
    file_is_ok_in_cst = gsdi_util_is_file_ok_in_cst( UIM_TELECOM_ADN,
                                                     pref_slot );

    if ( file_is_ok_in_cst )
    {
        /* Verify PIN-2 */
        if ( req_p->pin2_present )
        {
            gsdi_status = gsdi_uim_server_cdma_verify_chv( req_p->message_header.slot,
                                                          GSDI_PIN2,
                                                          req_p->pin2,
                                                          NULL );
        }
        /* Rehabilitate ADN EF */
        if ( gsdi_status == GSDI_SUCCESS )
        {
            gsdi_status = gsdi_uim_server_cdma_rehabilitate( req_p->message_header.slot,
                                                            UIM_TELECOM_ADN );
            if (gsdi_status == GSDI_SUCCESS)
            {
                gsdi_data.sim_capability.fdn_enabled = FALSE;
                if (req_p->message_header.slot == (gsdi_slot_id_type)GSDI_SLOT_1)
                {
                    if(gsdi_util_send_mmi_sim_ind( GSDI_FDN_DISABLE ) != GSDI_SUCCESS)
                        MSG_HIGH("Not able to send warning GSDI_FDN_DISABLE", 0, 0, 0);
                    else
                        MSG_HIGH("Warning indication GSDI_FDN_ENABLE",0,0,0);
                }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                else
                {
                    if(gsdi_util_send_mmi_sim_ind( GSDI_FDN_DISABLE_2 ) != GSDI_SUCCESS)
                        MSG_HIGH("Not able to send warning GSDI_FDN_DISABLE_2", 0, 0, 0);
                    else
                        MSG_HIGH("Warning indication GSDI_FDN_DISABLE_2",0,0,0);
                }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
            }
        }
    }
    else
    {
        gsdi_status = GSDI_ACCESS_DENIED;
    }

    MSG_MED("GSDI cdma disable fdn proc status %d",gsdi_status,0,0);

    return gsdi_status;


} /* gsdi_cdma_proc_disable_fdn */




/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_PIN_STATUS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_get_pin_status (
    gsdi_get_pin_status_req_T *  req_p,
    gsdi_pin_status_T *          data_p
)
{
    gsdi_returns_T            gsdi_status            = GSDI_SUCCESS;
    byte                      gsdi_pin_status_offset = 0;
    byte                      gsdi_puk_status_offset = 0;
    byte                      dir_info[GSDI_DEFAULT_FILE_HEADER_LEN];
    byte                      dir_info_len           = 0;
    boolean                   pin_status_cached      = FALSE;
    gsdi_slot_id_type         slot                   = GSDI_SLOT_NONE;
    mmgsdi_app_pin_info_type *app_pin_ptr            = NULL;
    gsdi_pin_info_item_T      pin_info;


    MSG_MED("GSDI cdma get pin status proc",0,0,0);

    /* Set the Slot For Request */
    slot = req_p->message_header.slot;
    switch ( slot )
    {
      case GSDI_SLOT_1:
        app_pin_ptr = mmgsdi_util_get_sel_app_pin_ptr(req_p->pin_id);
        if(app_pin_ptr == NULL)
        {
          MSG_ERROR("PIN Pointer is null, cannot proceed",0,0,0);
          return GSDI_ERROR;
        }
        pin_info.pin_id   = req_p->pin_id;
        if(app_pin_ptr->cached)
        {
          if(mmgsdi_util_convert_to_gsdi_pin_info(&pin_info) != MMGSDI_SUCCESS)
          {
            MSG_ERROR("Could not convert to gsdi pin status, pinID= 0x%x",
                       req_p->pin_id,0,0);
            return GSDI_ERROR;
          }
          pin_status_cached = TRUE;
        }
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
          if ( gsdi_data.pin_status_info_2.pin_status_cached )
          {
             pin_status_cached = TRUE;
             switch (req_p->pin_id)
             {
               case GSDI_PIN1:
                 *data_p = gsdi_data.pin_status_info_2.pin1_status;
                 break;
               case GSDI_PIN2:
                 *data_p = gsdi_data.pin_status_info_2.pin2_status;
                 break;
               default:
                 pin_status_cached = FALSE;
                 MSG_ERROR("Unknown PIN ID 0x%x", req_p->pin_id,0,0);
                 return GSDI_ERROR;
             }
          }
          break;
#endif
      default:
          MMGSDI_DEBUG_MSG_ERROR("UNKNOWN SLOT ID %x",req_p->message_header.slot,0,0);
          break;
    }
      /* Check if pin status has been cached or not */
    if(!pin_status_cached)
    {
        /* continue to get the pin status if
        * 1. pin1 or pin2 hasn't been cached yet
        * 2. non pin1 or pin2 pin status requested */

        /* initialized user pointer to GSDI_PIN_DISABLED */
        *data_p = GSDI_PIN_DISABLED;

        /* Select CDMA DF for header info */
        gsdi_status = gsdi_uim_server_cdma_select(GSDI_SELECT_REQ,
                                                  slot,
                                                  (uim_items_enum_type)0,
                                                  UIM_DF,
                                                  GSDI_RUIM_DIR,
                                                  NULL,
                                                  &dir_info_len,
                                                  dir_info );


        /* Extract the pin status from the header info */
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
        if ( gsdi_status == GSDI_SUCCESS )
        {
            switch ( slot )
            {
              case GSDI_SLOT_1:
                break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
              case GSDI_SLOT_2:
                  /* Get number of retry left for PINs and PUKs */
                  gsdi_data.pin_status_info_2.pin1_num_retries_allowed = dir_info[18] & 0x0F;
                  gsdi_data.pin_status_info_2.puk1_num_retries_allowed = dir_info[19] & 0x0F;
                  gsdi_data.pin_status_info_2.pin2_num_retries_allowed = dir_info[20] & 0x0F;
                  gsdi_data.pin_status_info_2.puk2_num_retries_allowed = dir_info[21] & 0x0F;
                  break;
#endif
              default:
                  MMGSDI_DEBUG_MSG_ERROR("UNKNOWN SLOT ID %x",req_p->message_header.slot,0,0);
                  break;
            }

            if ( ( req_p->pin_id == GSDI_RUIM_CHV1 ) ||
                 ( req_p->pin_id == GSDI_PIN1 ) )
            {
                gsdi_pin_status_offset = GSDI_PIN1_STATUS_OFFSET;
                gsdi_puk_status_offset = GSDI_PUK1_STATUS_OFFSET;
            }
            else
            {
                gsdi_pin_status_offset = GSDI_PIN2_STATUS_OFFSET;
                gsdi_puk_status_offset = GSDI_PUK2_STATUS_OFFSET;
            }

            if ( dir_info[gsdi_pin_status_offset] & GSDI_CODE_INIT_MASK )
            {
                if ( ( dir_info[gsdi_puk_status_offset] &
                    GSDI_CODE_GUESSES_LEFT_MASK ) == 0 )
                {
                    *data_p = GSDI_PIN_PERM_BLOCKED;
                }
                else if ( ( dir_info[gsdi_pin_status_offset] &
                            GSDI_CODE_GUESSES_LEFT_MASK ) == 0 )
                {
                    *data_p = GSDI_PIN_BLOCKED;
                }
                else if ( ( ( req_p->pin_id == GSDI_RUIM_CHV1 ) ||
                            ( req_p->pin_id == GSDI_PIN1 ) ) &&
                          ( dir_info[GSDI_FILE_CHARAC_OFFSET] &
                            GSDI_CHV1_DISABLED_MASK ) )
                {
                    *data_p = GSDI_PIN_DISABLED;
                    MSG_HIGH("CDMA PIN %x DISABLED",((byte)req_p->pin_id+1),0,0);
                }
                else
                {
                    /* Case for PIN1 if Enabled */
                    /* Always the Case for PIN2 */
                    *data_p = GSDI_PIN_ENABLED;
                    MSG_HIGH("CDMA PIN %x ENABLED",((byte)req_p->pin_id+1),0,0);
                }
            }
            else
            {
                *data_p = GSDI_PIN_NOT_INITIALISED;
            }
        } /* gsdi_status == GSDI_SUCCESS */
    } /* gsdi_data.pin_status_cached &&
        (req_p->pin_id == GSDI_PIN1 || req_p->pin_id == GSDI_PIN2) */

    MSG_MED("GSDI cdma get pin status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_get_pin_status */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_UNBLOCK_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_unblock_pin (
    gsdi_unblock_pin_req_T *  req_p,
    byte*            data_p
)
{
    gsdi_returns_T    gsdi_status     = GSDI_SUCCESS;

    MSG_MED("GSDI cdma unblock pin proc",0,0,0);
    MMGSDI_RETURN_IF_NULL(data_p);

    gsdi_status = gsdi_uim_server_cdma_unblock_chv( req_p->message_header.slot,
                                                    (gsdi_pin_ids_T)req_p->pin_id,
                                                    req_p->puk,
                                                    req_p->new_pin,
                                                    data_p );

    MSG_MED("GSDI cdma unblock pin proc status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_unblock_pin */


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_VERIFY_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_verify_pin (
  gsdi_verify_pin_req_T *  req_p,
  byte *                   data_p
)
{
    gsdi_returns_T  gsdi_status     = GSDI_SUCCESS;

    MSG_MED("GSDI gsm verify pin proc",0,0,0);
    MMGSDI_RETURN_IF_NULL(data_p);

    gsdi_status = gsdi_uim_server_cdma_verify_chv(req_p->message_header.slot,
                                                  (gsdi_pin_ids_T)req_p->pin_id,
                                                  req_p->pin,
                                                  data_p);

  MSG_MED("GSDI cdma verify pin proc status %d",gsdi_status,0,0);
  return gsdi_status;

} /* gsdi_cdma_proc_verify_pin */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_FILE_ATTRIBUTES

DESCRIPTION
  Get the file header information in RUIM

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_get_file_attributes (
    gsdi_get_file_attributes_req_T *  req_p,
    byte *                            data_p
)
{
    byte                  file_header_len=0;
    byte                  file_header[GSDI_DEFAULT_FILE_HEADER_LEN];
    gsdi_returns_T        gsdi_status = GSDI_SUCCESS;
    uim_items_enum_type   EF_to_select;
    gsdi_item_category_T  EF_category;
    uim_dir_type          EF_directory=0;
    boolean               file_is_ok_in_sst = TRUE;
    gsdi_slot_id_type     new_slot;
    boolean               pref_slot = TRUE;

    MSG_MED("GSDI gsm get file attr proc",0,0,0);

    EF_to_select = (uim_items_enum_type)req_p->sim_filename;

    MSG_HIGH("  SIM select EF %x",EF_to_select,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref(req_p->message_header.slot, GSDI_CDMA, &new_slot);

    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check status of EF in cached SST (SIM Service Table) */
    file_is_ok_in_sst = gsdi_util_is_file_ok_in_cst( EF_to_select, pref_slot );

    if ( !file_is_ok_in_sst )
    {
      MSG_ERROR("EF_to_select not in CST: %x SLOT: %x",EF_to_select, pref_slot,0);
      return GSDI_NOT_SUPPORTED;
    }

    /* Set the directory of the requested EF */
    EF_category = (gsdi_item_category_T)(((word)EF_to_select >> 8) & 0x0F);

    switch(EF_category) {
      case ROOT:
          EF_directory = GSDI_MF_DIR;
          break;
      case TELECOM:
          EF_directory = GSDI_TELECOM_DIR;
          break;
      case CDMA:
          EF_directory = GSDI_RUIM_DIR;
          break;
      default:
          MMGSDI_DEBUG_MSG_ERROR("Invalid EF category - %d",EF_category,0,0);
          gsdi_status = GSDI_ERROR;
          break;
    } /* switch */

    /* Set the EF_category */
    gsdi_data.current_EF_category = EF_category;

    /* Send SELECT request to the UIM Server */
    if ( gsdi_status == GSDI_SUCCESS ) {

      gsdi_status = gsdi_uim_server_cdma_select( req_p->message_header.message_id,
                                                 req_p->message_header.slot,
                                               EF_to_select,
                                               UIM_EF,
                                               EF_directory,
                                               NULL,
                                               &file_header_len,
                                               file_header );
    }

    /* Extract the important file attributes from the file header */
    if ( gsdi_status == GSDI_SUCCESS )
    {
        gsdi_util_extract_file_attr(req_p->message_header.slot,file_header,
        file_header_len,data_p );
    }
    else
    {
        MSG_HIGH("Select status %d",gsdi_status,0,0);
    }

    MSG_MED("GSDI cdma get file attr proc status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_get_file_attributes */



/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_RUIM_CAPABILITIES

DESCRIPTION
  Return the RUIM capablities

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/


gsdi_returns_T gsdi_cdma_proc_get_ruim_capabilities (
    gsdi_slot_id_type               slot
)
{
    gsdi_returns_T                  gsdi_status = GSDI_SUCCESS;
    sim_capabilities_T              *sim_cap;
    boolean                         *disable_chv1_allowed;
    boolean                         adn_valid = FALSE;
    gsdi_get_file_attributes_req_T  attr_req;
    mmgsdi_data_type                mmgsdi_data_buf;
    mmgsdi_return_enum_type         mmgsdi_status= MMGSDI_SUCCESS;
    mmgsdi_slot_id_enum_type        mmgsdi_slot;
    mmgsdi_file_enum_type           fdn_filenames[MMGSDI_CDMA_FDN_FILES] =
                                    {MMGSDI_CDMA_IMSI_M, MMGSDI_CDMA_IMSI_T,
                                     MMGSDI_CDMA_TMSI};
    uint8                           index = 0;

    memset(&mmgsdi_data_buf, 0x00,sizeof(mmgsdi_data_type));

    MSG_HIGH("Get RUIM capabilities",0,0,0);

    switch (slot)
    {
      case GSDI_SLOT_1:
          sim_cap = &gsdi_data.sim_capability;
          disable_chv1_allowed = &gsdi_data.disable_chv1_allowed;
          break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
          sim_cap = &gsdi_data.sim_capability_2;
          disable_chv1_allowed = &gsdi_data.disable_chv1_allowed_2;
          break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
      default:
          MMGSDI_DEBUG_MSG_ERROR("Invalid Slot %d", slot, 0, 0);
          return GSDI_ERROR;
    }

    /* Reading RUM CDMA Phase file, It has 1 byte info.
       It is mandatory file and should be read */
    mmgsdi_status = mmgsdi_init_cache_binary(slot, MMGSDI_CDMA_RUIM_PHASE, 
                                             &mmgsdi_data_buf);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);    
    if (mmgsdi_status != MMGSDI_SUCCESS) 
    {
      MSG_ERROR("gsdi_cdma_proc_get_ruim_capabilities failed for 0x%x",
                 MMGSDI_CDMA_RUIM_PHASE, 0, 0);
      /* No need to return the GSDI_ERROR if this mandatory file is not present
         on the card. According to test requirement for Version Identification, 
         ME should requests the R-UIM version (EFRevision) as part of the 
         initialization, It does not say that initialization should be stop 
         in case this file is not there.
      */
      mmgsdi_status = MMGSDI_SUCCESS;
    }

    /* Read RUIM Service Table */
    mmgsdi_status = mmgsdi_init_cache_binary(slot, MMGSDI_CDMA_SVC_TABLE, &mmgsdi_data_buf);

    sim_cap->imsi_invalidated = FALSE;
    sim_cap->fdn_enabled = FALSE;

    if (mmgsdi_status != MMGSDI_SUCCESS) {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
      return GSDI_ERROR;
    }

    if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_FDN_OFFSET] & GSDI_SST_FDN_MASK ) ==
              GSDI_SST_FDN_MASK ) {

        if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_ADN_OFFSET] & GSDI_SST_ADN_MASK ) ==
              GSDI_SST_ADN_MASK ) {

            /* Read EF header of ADN */
            attr_req.sim_filename = (word)UIM_TELECOM_ADN;
            attr_req.message_header.slot = slot;
            gsdi_status = gsdi_cdma_proc_get_file_attributes( &attr_req,
                                                              file_attributes_p );
            if(gsdi_status == GSDI_SUCCESS)
            {
              adn_valid = file_attributes_p[GSDI_ATTR_VALID_FLAG_OFFSET];
            }
            if ( !adn_valid ) {
                  sim_cap->fdn_enabled = TRUE;
            } else {
                  sim_cap->fdn_enabled = FALSE;
            }

        } else {
            sim_cap->fdn_enabled = TRUE;
        }

    } else {
        sim_cap->fdn_enabled = FALSE;
    }

    if (sim_cap->fdn_enabled)
    {
      mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_slot(FALSE, &slot, &mmgsdi_slot);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDI_DEBUG_MSG_ERROR("Could not convert gsdi slot %d",
                               slot, 0, 0);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
        return GSDI_ERROR;
      }

      for (index =0; index < MMGSDI_CDMA_FDN_FILES; index++)
      {
        mmgsdi_status = gsdi_cdma_rehab_cdma_fdn_proc_file(
                          fdn_filenames[index], mmgsdi_slot);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDI_DEBUG_MSG_ERROR(
             "gsdi_cdma_rehab_cdma_fdn_proc_file failed for %d",
             fdn_filenames[index], 0, 0);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
          return GSDI_ERROR;
        }
      }
    }

    if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_ADN_OFFSET] & GSDI_SST_ADN_MASK ) == GSDI_SST_ADN_MASK )
    {
        sim_cap->sim_phone_book.present = TRUE;
        sim_cap->sim_phone_book.multiple_pbr_entries = FALSE;
        MSG_MED("SET sim_pb_present to %d ",sim_cap->sim_phone_book.present,0,0);
    }
    else
    {
        sim_cap->sim_phone_book.present = FALSE;
        sim_cap->sim_phone_book.multiple_pbr_entries = FALSE;
        MSG_MED("ADN Entry is not present, not active, or not alloc",0,0,0);
    }

    if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_CHV1_DISABLE_OFFSET] & GSDI_SST_CHV1_DISABLE_MASK ) ==
          GSDI_SST_CHV1_DISABLE_MASK)
    {
        *disable_chv1_allowed = TRUE;
    }
    else
    {
        *disable_chv1_allowed = FALSE;
    }
#ifdef FEATURE_OEMOMH    
    if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_MSG_3GPD_OFFSET] & GSDI_SST_MSG_3GPD_MASK) == GSDI_SST_MSG_3GPD_MASK)
    {
        gsdi_uim_omh_cap.omh_enabled = TRUE;
    }
    else
    {
        gsdi_uim_omh_cap.omh_enabled = FALSE;
    }
    
    if ( ( mmgsdi_data_buf.data_ptr[GSDI_SST_BREW_OFFSET] & GSDI_SST_BREW_MASK) == GSDI_SST_BREW_MASK)
    {
        gsdi_uim_omh_cap.brew_enabled = TRUE;
    }
    else
    {
        gsdi_uim_omh_cap.brew_enabled = FALSE;
    }   
#endif
    MSG_MED("GSDI gsm get sim cap proc status %d",gsdi_status,0,0);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);
    return GSDI_SUCCESS;

} /* gsdi_cdma_proc_get_ruim_capabilities */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_REHABILITATE

DESCRIPTION
  call to the gsdi_uim_server_cdma_rehabilitate function for uim driver
  result

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_rehabilitate (
    gsdi_rehabilitate_req_T *  req_p
)
{
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

    MSG_MED("GSDI cdma rehabilitate proc",0,0,0);

    gsdi_status = gsdi_uim_server_cdma_rehabilitate(req_p->message_header.slot,
                                                    req_p->sim_filename);

    MSG_MED("GSDI cdma rehabilitate proc status %d",gsdi_status,0,0);

    return gsdi_status;
} /*gsdi_cdma_proc_rehabilitate */

/*===========================================================================
FUNCTION GSDI_CDMA_TEST_FOR_DFS

DESCRIPTION
  Function used to determine which DFs are present on RUIM

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_test_for_dfs(
    byte * dfs_present_p,
    byte * data_p )
{
    gsdi_returns_T gsdi_status;
    byte           select_len;

    /* First Select the CDMA DF */
    gsdi_status = gsdi_uim_server_cdma_select( GSDI_SELECT_REQ,
                                               GSDI_SLOT_1,
                                               (uim_items_enum_type)0,
                                               UIM_DF,
                                               GSDI_RUIM_DIR,
                                               NULL,
                                               &select_len,
                                               data_p );

    if ( gsdi_status == GSDI_SUCCESS )
    {
        /* CDMA DF Present */
        dfs_present_p[0] |= GSDI_CDMA_DF_PRESENT;
    }

    /* Second Select the GSM DF */
    gsdi_status = gsdi_uim_server_cdma_select( GSDI_SELECT_REQ,
                                               GSDI_SLOT_1,
                                               (uim_items_enum_type)0,
                                               UIM_DF,
                                               GSDI_GSM_DIR,
                                               NULL,
                                               &select_len,
                                               data_p );
    if ( gsdi_status == GSDI_SUCCESS )
    {
        /* GSM DF Present */
        dfs_present_p[0] |= GSDI_GSM_DF_PRESENT;
    }

    /* Third Select the DSC 1800 DF */
    gsdi_status = gsdi_uim_server_cdma_select( GSDI_SELECT_REQ,
                                               GSDI_SLOT_1,
                                               (uim_items_enum_type)0,
                                               UIM_DF,
                                               GSDI_DCS1800_DIR,
                                               NULL,
                                               &select_len,
                                               data_p );

    if ( gsdi_status == GSDI_SUCCESS )
    {
        /* GSM DF Present */
        dfs_present_p[0] |= GSDI_DCS1800_DF_PRESENT;
    }

    return GSDI_SUCCESS;

} /* gsdi_cdam_test_for_dfs */

/*===========================================================================
FUNCTION GSDI_NVRUIM_SET_DFS_PRESENT

DESCRIPTION
  Function used to notify CDMA Stack where to retrieve ESN From

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_nvruim_set_df_present(
    byte ruim_dfs_present )
{
#ifndef FEATURE_VIRTUAL_SIM
    nvruim_set_uim_dir_present(ruim_dfs_present);
#endif

}/* gsdi_nvruim_set_df_present */


/*===========================================================================
FUNCTION GSDI_NVRUIM_SET_ESN_USAGE

DESCRIPTION
  Function used to notify CDMA Stack where to retrieve ESN From

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_nvruim_set_esn_usage(
    byte esn_usage )
{
#ifndef FEATURE_VIRTUAL_SIM
    /* Notify NVRUIM of ESN USAGE */
    nvruim_set_esn_usage((nvruim_esn_usage_type)esn_usage);
#endif
}

/*===========================================================================
FUNCTION GSDI_CDMA_IS_ICC_CDMA_DF_PRESENT

DESCRIPTION
  This function will test for DF CDMA in the Card Inserted.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_is_icc_cdma_df_present ( byte * data_p )
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    byte           select_len  = 0;

    /* First Select the CDMA DF */
    gsdi_status = gsdi_uim_server_cdma_select( GSDI_SELECT_REQ,
                                               GSDI_SLOT_1,
                                               (uim_items_enum_type)0,
                                               UIM_DF,
                                               GSDI_RUIM_DIR,
                                               NULL,
                                               &select_len,
                                               data_p );
    return gsdi_status;

} /* gsdi_cdma_is_icc_cdma_df_present */

/*===========================================================================
FUNCTION GSDI_CDMA_IS_ICC_DCS1800_DF_PRESENT

DESCRIPTION
  This function will test for DF DCS1800 in the Card Inserted.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_is_icc_dcs1800_df_present ( byte * data_p )
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    byte           select_len  = 0;

    gsdi_status = gsdi_uim_server_cdma_select( GSDI_SELECT_REQ,
                                               GSDI_SLOT_1,
                                               (uim_items_enum_type)0,
                                               UIM_DF,
                                               GSDI_DCS1800_DIR,
                                               NULL,
                                               &select_len,
                                               data_p );
    return gsdi_status;
} /*  gsdi_cdma_is_icc_dcs1800_df_present */


#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_SEARCH

DESCRIPTION
  Function is called by gsdi_proc_handle_client_req to handle a
  GSDI_SIM_SEARCH_REQ.

INPUT
  gsdi_sim_search_req_T *  req_p
  byte *  rec_num_accessed_p

OUTPUT
  Invokes the appropriate uim server fucntion to handle the request.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_ruim_search (
    gsdi_sim_search_req_T *  req_p,
    byte *  rec_num_accessed_p
)
{

    gsdi_returns_T      gsdi_status       = GSDI_SUCCESS;
    gsdi_slot_id_type   new_slot          = GSDI_SLOT_NONE;
    uim_items_enum_type EF_to_search      = UIM_ICCID;
    boolean             file_is_ok_in_cst = TRUE;
    byte                rec_num_len       = 0;
    boolean             pref_slot         = TRUE;

    /* ----------------------------------------------------------------
    Perform Paramate checking
    1. Check the search_mode.
    2. Check the search direction
    ----------------------------------------------------------------*/
    if((req_p->search_mode      != (byte)GSDI_SIMPLE_SRCH) ||
       (req_p->search_direction != (byte)GSDI_SRCH_FORWARD)
      )
    {
      return(GSDI_INCORRECT_PARAMS);
    }

    MSG_MED("GSDI cdma search proc",0,0,0);

    EF_to_search = (uim_items_enum_type)req_p->sim_filename;

    MSG_HIGH("  RUIM search EF %x",EF_to_search,0,0);

    /* check if the slot requested is the preferred slot */
    pref_slot = gsdi_util_is_slot_pref( req_p->message_header.slot,
                                        GSDI_CDMA,
                                        &new_slot);
    /* update the slot in case the original slot was AUTOMATIC */
    req_p->message_header.slot = new_slot;

    /* Check status of EF in cached CST (SIM Service Table) */
    file_is_ok_in_cst = gsdi_util_is_file_ok_in_cst( EF_to_search,
                                                     pref_slot );

    if ( file_is_ok_in_cst )
    {
        if ( req_p->search_string_len > 0 )
        {
            gsdi_status = gsdi_uim_server_cdma_seek( req_p->message_header.slot,
                                                     EF_to_search,
                                                     req_p->index_mode,
                                                     req_p->search_string,
                                                     req_p->search_string_len,
                                                     &rec_num_len,
                                                     rec_num_accessed_p);
            if ( gsdi_status != GSDI_SUCCESS)
               MSG_HIGH("Seek status %d",gsdi_status,0,0);
        }
    }
    else
    {
       gsdi_status = GSDI_NOT_SUPPORTED;
    }

    MSG_MED("GSDI cdma search proc status %d",gsdi_status,0,0);

    return gsdi_status;

} /* gsdi_cdma_proc_ruim_search */

/* ----------------------------------------------------------------------------
   FUNCTION:      GSDI_CDMA_REHAB_CDMA_FDN_PROC_FILE

   DESCRIPTION:
   This function checks if IMSI_M, IMSI_T or TMSI are invalidated if fdn is supported
   and re-habilitates them if necessary.

   DEPENDENCIES:
     Should only be called if FDN is enabled.

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
   IMSI_M, IMSI_T, and TMSI are re-habilitated conditionally

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type gsdi_cdma_rehab_cdma_fdn_proc_file(
  mmgsdi_file_enum_type      sim_filename,
  mmgsdi_slot_id_enum_type   slot
)
{
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *attr_req_ptr      = NULL;
  mmgsdi_get_file_attr_cnf_type      *attr_cnf_ptr      = NULL;
  mmgsdi_transparent_file_info_type  fdn_file_attr;
  mmgsdi_rehabilitate_cnf_type       *rehab_cnf_ptr     = NULL;
  mmgsdi_request_header_type         fdn_request_header;
  mmgsdi_access_type                 fdn_access;
  mmgsdi_rehabilitate_req_type       *rehab_req_ptr     = NULL;


  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                            attr_req_ptr,
                            sizeof(mmgsdi_get_file_attr_req_type));

  memset(&fdn_request_header, 0x00, sizeof(mmgsdi_request_header_type));
  memset(&fdn_access, 0x00, sizeof(mmgsdi_access_type));

  fdn_access.file.file_enum = sim_filename;
  fdn_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  fdn_request_header.slot_id = MMGSDI_SLOT_1;

  attr_req_ptr->access         = fdn_access;
  attr_req_ptr->request_header = fdn_request_header;

  /* Get the file attributes for the requested rehabilitate file */
  mmgsdi_status = mmgsdi_uim_icc_select (attr_req_ptr, TRUE, &attr_cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(attr_req_ptr);

  if ( mmgsdi_status == MMGSDI_SUCCESS )
  {
    /* We have successfully got the file attributes for CDMA File*/

    if (attr_cnf_ptr == NULL)
    {
      MSG_ERROR("attr_cnf_ptr from mmgsdi_uim_icc_select is NULL",0,0,0);
      return MMGSDI_ERROR;
    }

    fdn_file_attr = attr_cnf_ptr->file_attrib.file_info.transparent_file;

    /* Check if the FDN Procedures should continue if the permissions
       to rehabilitate the file is not CHV1 then the card is not provisioned
       properly and do not continue with FDN procedures but do not send an
       error as these cards should power up barring anyother issues. */
    if (fdn_file_attr.file_security.rehabilitate_activate.protection_method ==
        MMGSDI_SINGLE_ALLOWED &&
        fdn_file_attr.file_security.rehabilitate_activate.protection_pin_ptr[0] ==
        MMGSDI_PIN1)
    {
      /* file_invalidated == 0 => file is invalidated
         file_invalidated == 1 => file is not invalidated */
      if (!fdn_file_attr.file_status.file_invalidated)
      {
        /* Free  cnf pointer not needed */
        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)attr_cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(attr_cnf_ptr);

        /* File is invalidated */
        MSG_HIGH("CDMA File %x Invalidated - Rehabilitating", sim_filename, 0, 0);

        /* Rehabilitate File */
        MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                            rehab_req_ptr,
                            sizeof(mmgsdi_rehabilitate_req_type));

        rehab_req_ptr->access         = fdn_access;
        rehab_req_ptr->request_header = fdn_request_header;

        mmgsdi_status = mmgsdi_uim_icc_rehabilitate(rehab_req_ptr, TRUE, &rehab_cnf_ptr);

        MMGSDIUTIL_TMC_MEM_FREE(rehab_req_ptr);

        /* Check the status if the rehabilitate command. */
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Check SW1 and SW2 from card response if failure is because of auth
             failure then we have a card that does not support rehabiliation so
             continue */
          /* sw1=>90*/
          if (rehab_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
          {
            MSG_HIGH(
              "File %x Rehabilitate Success", sim_filename, 0,0);
          }
          /* sw1=>0x98 sw2=>0x04*/
          else if (rehab_cnf_ptr->response_header.mmgsdi_status ==
                   MMGSDI_ACCESS_DENIED)
          {
            MSG_HIGH(
              "File %x Rehabilitate Failed, Access Condition Not Fulfilled",
              sim_filename, 0,0);
          }
          else
          {
            MSG_ERROR("File %x Rehabilitate Failed mmgsdi status: %x",
                      sim_filename,
                      rehab_cnf_ptr->response_header.mmgsdi_status,0);
            mmgsdi_status = MMGSDI_ERROR;
          } /* End Check for Rehabilitate Command SW1 and SW2 */
        } /* if(mmgsdi_internal_synch_uim_rpt_rsp != NULL) */

        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)rehab_cnf_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rehab_cnf_ptr);
      } /* if ( !file_invalidated ) */
      else
      {
        MSG_HIGH("File: %x Not Invalidated",sim_filename,0,0);
      }
    } /* if check for file rehabilitate permissions */
    else
    {
      MSG_HIGH("Cannot continue with FDN procdeures, Rehabilitate permissions not CHV1",
               0,0,0);
      mmgsdi_status = MMGSDI_SUCCESS;

    }

  } /* get file attribute success */
  else
  {
    MSG_ERROR(
        "mmgsdi_uim_icc_select returned with error status %x",
        mmgsdi_status,0,0);
  }
  return mmgsdi_status;
}


/*===========================================================================
 FUNCTION GSDI_CDMA_PROC_SELECT

 DESCRIPTION
    populate parameters for gsdi_uim_server_cdma_select() for the select
     process, EFs are being checked against cdma service table to ensure
    that they are avaliable before sending down to the uim driver
 
  DEPENDENCIES
    gsdi_util_is_file_ok_in_cst
 
  RETURN VALUE
    GSDI_SUCCESS on success
    GSDI_NOT_SUPPORTED if operation is not permitted
    GSDI_ERROR general errors.
 ===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_select (
  gsdi_select_req_T *  req_p,
  byte*                file_len_p,
  byte*                file_data_p
)
{
  gsdi_returns_T       gsdi_status       = GSDI_SUCCESS;
  gsdi_item_category_T EF_category       = ROOT;
  gsdi_slot_id_type    new_slot          = GSDI_SLOT_NONE;
  uim_items_enum_type  EF_to_select      = UIM_NO_SUCH_ITEM;
  uim_file_type        file_type         = UIM_NO_SUCH_FILE;
  uim_dir_type         directory_type    = (uim_dir_type)UIM_NO_SUCH_FILE;
  word *               path_array_p      = NULL;
  boolean              file_is_ok_in_cst = TRUE;
  boolean              pref_slot         = TRUE;

  if((req_p == NULL) || (file_len_p == NULL) || (file_data_p == NULL))
  {
    MSG_ERROR("Null input ptr", 0, 0, 0);
    return GSDI_ERROR;
  }

  MSG_HIGH(" Processing gsdi_cdma_proc_select",0,0,0);

  path_array_p = &req_p->sim_filename[0];

  /* check if the slot requested is the preferred slot */
  pref_slot = gsdi_util_is_slot_pref( req_p->message_header.slot,
                                      GSDI_CDMA,
                                      &new_slot);
  /* update the slot in case the original slot was AUTOMATIC */
  req_p->message_header.slot = new_slot;

  switch(req_p->file_type)
  {
    case UIM_MF:
      EF_to_select = UIM_NO_SUCH_ITEM;
      file_type = UIM_MF; /* uim server uses this info only */
      directory_type = (uim_dir_type)UIM_MF;
      break;

    case UIM_DF:
      file_type = UIM_DF;
      /* uim server uses UIM_NO_FILE_SEL = 0xFFFF to distinguish */
      if (req_p->path_len == GSDI_FIRST_LVL_DF)
      {
        /* first level path */
        /* uim server uses directory_type */
        EF_to_select = UIM_NO_SUCH_ITEM;
        directory_type = req_p->sim_filename[0];
      }
      else
      {
        /* select other level DF with path provided */
        /* uim server uses path */
        EF_to_select = UIM_NO_SUCH_ITEM;
        directory_type = 0xFFFF;
      }
      break;

    case UIM_EF:
      file_type = UIM_EF;
      directory_type =  (uim_dir_type)UIM_NO_SUCH_FILE;
      if (req_p->path_len == 0)
      {
        EF_to_select = (uim_items_enum_type)req_p->sim_filename[0];
        MSG_HIGH("RUIM select uim EF enum 0x%x",EF_to_select,0,0);

        /* Does the CST (CDMA Service Table) allow this EF. */

        file_is_ok_in_cst = gsdi_util_is_file_ok_in_cst( EF_to_select,
                                                          pref_slot );
        if (!file_is_ok_in_cst)
        {
          gsdi_status = GSDI_NOT_SUPPORTED;
          break;
        }

        /* Set the directory of the requested EF */
        EF_category = (gsdi_item_category_T)(((uint32)EF_to_select >> 8) & 0x0F);

        switch(EF_category)
        {
          case ROOT:
            directory_type = GSDI_MF_DIR;
            break;
          case CDMA:
            directory_type = GSDI_RUIM_DIR;
            break;
          case TELECOM:
            directory_type = GSDI_TELECOM_DIR;
            break;
          case GSM:
            directory_type = GSDI_GSM_DIR;
            break;
          default:
            MSG_ERROR("Invalid EF category - 0x%x",EF_category,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        } /* switch */
      }
      else
      {
        EF_to_select = UIM_EF_BY_PATH;
      }
      break;

    case UIM_ADF:
      MSG_ERROR(" ADF not supported in CDMA/RUIM ", 0, 0, 0);
      gsdi_status = GSDI_NOT_SUPPORTED;
      break;

    default:
      MSG_ERROR(" Unknown file type 0x%x", req_p->file_type,0, 0);
      gsdi_status = GSDI_ERROR;
      break;
  }

  /* Send select request to the UIM server */
  if (gsdi_status == GSDI_SUCCESS)
  {
    MSG_HIGH("CDMA SELECT: EF=0x%x, dir=0x%x, filetype=0x%x",
              EF_to_select, directory_type, file_type);

    gsdi_status = gsdi_uim_server_cdma_select(req_p->message_header.message_id,
                                              req_p->message_header.slot,
                                              EF_to_select,
                                              file_type,
                                              directory_type,
                                              path_array_p,
                                              file_len_p,
                                              file_data_p);
  }
  else
  {
    MSG_ERROR("Select failed status 0x%x",gsdi_status,0,0);
  }
  return gsdi_status;
} /* gsdi_cdma_proc_select */


#endif /* FEATURE_MMGSDI_CDMA */
