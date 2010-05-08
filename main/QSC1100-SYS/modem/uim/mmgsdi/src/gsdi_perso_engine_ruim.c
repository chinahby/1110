
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


      G E N E R I C   P E R S O N A L I Z A T I O N   E N G I N E


GENERAL DESCRIPTION

  This source file contains the GSDI PERSONALIZATION ENGINE which adheres to
  the personalization interface to do the personalization procedures.

            COPYRIGHT INFORMATION

Copyright (c) 2001 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
            EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso_engine_ruim.c#2 $$ $DateTime: 2009/01/16 06:47:27 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/16/09   nb      Changed gsdi_fs_name_test() to gsdi_efs_name_test()
11/12/08   ssr     Removed compiler warnings
09/05/08   ssr     Adding mmgsdiutil.h for FEATURE_PERSO_RUIM
07/24/08   sp      Modified debug messages
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/15/07   nk      Updated EFS Path depending if apps mount feature defined
08/23/06   sun     Fixed Lint Errors
05/21/06   nk      Fixed Compilation Warning
05/19/06   wli     Fixed a lint error
05/16/06   tml     Fixed compilation warning
05/11/06   wli     Fixed formatting IMSI.
04/21/06   nk      Fixed warning, fixed
                   GSDI_PERSO_ENGINE_GET_RUIM_ENCRYPTED_DATA for loop
04/13/06   sun     Removed perm_feat_ind from the file
04/06/06   sun     Initial Release
===========================================================================*/


/* <EJECT> */
/*===========================================================================

           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#include "gsdi_exp.h"
#include "gsdi.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#ifdef FEATURE_PERSO_RUIM
#include "gsdi_perso_engine.h"
#include "nvruimi.h"
#include "mmgsdiutil.h"
#ifdef FEATURE_MMGSDI_PERSO_SFS
#include "sfs_private.h"
#else
#include "gsdi_efs.h"
#endif
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#include "gsdi_perso_sec.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
#include "uim.h"
#include "tmc.h"
#include "gsdi_perso_engine_ruim.h"

#define GSDI_PERSO_HW_ESN_LENGTH 4
#define GSDI_PERSO_ESN_LAST_BYTE_MASK 0xFF
#define GSDI_PERSO_ESN_LAST_BYTE_SHIFT 8

#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
static char *mmgsdi_efs_ruim_perso_file  = "/apps/mmgsdi/perso/ruim_perso.txt";
#else
static char *mmgsdi_efs_ruim_perso_file  = "mmgsdi/perso/ruim_perso.txt";
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */

// this should come from txt file.
static  boolean gsdi_perso_ruim_use_imsi_m = TRUE;

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_READ_RUIM

DESCRIPTION
  This is the generic wrapper function used to read data from the SIM.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_ruim_read_ruim(
  gsdi_slot_id_type                      slot,
  uim_items_enum_type                    file_name,
  void *                                 data_p
)
{
  gsdi_returns_T          gsdi_status     = GSDI_ERROR;
  gsdi_sim_read_req_T *   read_req_p      = NULL;
  byte                    dummy           = 0;
  gsdi_perso_sec_sim_ele_data_type *     sim_data_p
  = (gsdi_perso_sec_sim_ele_data_type *)data_p;

  /* ------------------------------------------------------------------------
     Validate the parameters and perform basic initialization
     ----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sim_data_p);

  sim_data_p->len = 0;

  /* --------------------------------------------------------------------------
     Allocate memory for the Read Request.
     Error out of the function if memory can not be allocated.
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  MMGSDI_RETURN_IF_NULL(read_req_p);

  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));

  /* ----------------------------------------------------------------
     Build the Read Request according to
     the file provided.
     --------------------------------------------------------------*/
  read_req_p->message_header.slot = GSDI_SLOT_1;
  read_req_p->sim_filename        = (int32)file_name;
  read_req_p->index_mode          = GSDI_NO_INDEX;
  read_req_p->search_string_len   = 0x00;
  read_req_p->data_offset         = 0x00;
  read_req_p->rec_num             = 0x00;

  gsdi_status = gsdi_cdma_proc_ruim_read( read_req_p,
                                          &dummy,
                                          &dummy,
                                          (byte *)&sim_data_p->len,
                                          (byte *)sim_data_p->buffer_p);

  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("PERSO: gsdi_cdma_proc_ruim_read failed 0x%x for file 0x%x",
              gsdi_status,file_name,0);
  }
  /* --------------------------------------------------------------------------
     Free up the Request Data
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_p);

  return gsdi_status;
  /*lint -esym(715,slot)*/
}
/*===========================================================================
FUNCTION GSDI_PERSO_RUIM_READ_IMSI

DESCRIPTION
  This function will be called to read the IMSI and convert it to a usable
  format.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_ruim_read_imsi(
  gsdi_slot_id_type                    slot,
  uim_items_enum_type                  file_name,
  gsdi_perso_sec_sim_ele_data_type     *imsi
)
{
  gsdi_returns_T        gsdi_status       = GSDI_SUCCESS;
  gsdi_perso_sec_sim_ele_data_type imsi_buf;

  byte  imsi_digits[NVRUIM_NUM_MIN_DIGITS];  /* converted digits */
  word  imsi_mcc;                      /* mcc */
  byte  imsi_mnc;                      /* mnc */
  dword imsi_min1;                     /* min1 */
  word  imsi_min2;                      /* min2 */

  byte index;
  /* --------------------------------------------------------------------------
     Read the Administrative Data Elementary File
     ------------------------------------------------------------------------*/

  imsi->len = 0;

  imsi_buf.buffer_p = mmgsdi_malloc(256);
  MMGSDI_RETURN_IF_NULL(imsi_buf.buffer_p);

  /* Read IMSI from the card */
  gsdi_status = gsdi_perso_engine_ruim_read_ruim(slot,
                                                 file_name,
                                                 (void*)&imsi_buf);



  if(gsdi_status == GSDI_SUCCESS)
  {
    /* Format the coded IMSI to BCD */
    /* Contry code */
    imsi_mcc = (word) (((imsi_buf.buffer_p[9] & 0x03) << 8)
                      + imsi_buf.buffer_p[8]);
    /* S_11_12 */
    imsi_mnc = imsi_buf.buffer_p[6] & 0x7F;
    /* S1 */
    imsi_min1 = (dword) ((imsi_buf.buffer_p[5] <<16)
                         +(imsi_buf.buffer_p[4] <<8)
                         +imsi_buf.buffer_p[3]);
    /* S2 */
    imsi_min2 = (word) ((imsi_buf.buffer_p[2] <<8)
                         +imsi_buf.buffer_p[1]);


    /* Convert to decimal digits */
    if(nvruim_decode_min(&imsi_digits[0], &imsi_mcc,
                         &imsi_mnc, &imsi_min1, &imsi_min2))
    {

      /* Formatted IMSI is BCD coded:
          MCC   MNC   NW2   Last6digits
         ----   ---  ----   -----------
         B1B2    B3  B4B5    B6B7B8

    */
      /* Convert from ascii to BCD */
      imsi->buffer_p[0] = imsi_digits[0] - '0';

      for (index=1; index <8; index++) {
        imsi->buffer_p[index] = (uint8)((uint8)(imsi_digits[2*index-1] - '0') << 4)
                                        + (imsi_digits[2*index] - '0');
      }
      imsi->len = 8;
    }
    else
    {
      gsdi_status = GSDI_PERSO_INVALID_DATA;
    }
  }
  else
  {
    MSG_ERROR("PERSO: IMSI read from card failed 0x%x",
              gsdi_status,0,0);
  }

  MMGSDIUTIL_TMC_MEM_FREE(imsi_buf.buffer_p);
  return gsdi_status;
}


/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_RUIM_GET_RUIM_DATA

DESCRIPTION:
  Will gather all of the information from the SIM/USIM/RUIM inserted.
  It will retrieve:
  EF-IMSI
  EF-CNL
  EF-GID1
  EF-GID2

DEPENDENCIES:
  Card must be inserted before being called and the PIN Must be verified.

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_ruim_data(
  gsdi_slot_id_type                          slot,
  gsdi_perso_sec_sim_data_type             * sim_data_p,
  const gsdi_perso_engine_mem_pointer_type * mem_pointer_p
)
{
  gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Perform Validation Checks
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p->memory_curr);

  /* --------------------------------------------------------------------------
     Assign the IMSI Pointer to the current mmgsdi_malloc'ed memory
     block available
     ------------------------------------------------------------------------*/
  sim_data_p->imsi.buffer_p = mem_pointer_p->memory_curr;

  /* --------------------------------------------------------------------------
     Get the IMSI information from the RUIM
     If retrieval of the IMSI information fails,
     the sec_data_p->buffer_p memory allocated for
     this request will already be freed.
     ------------------------------------------------------------------------*/
  if(gsdi_perso_ruim_use_imsi_m)
  {
    /* Read IMSI_M */
    gsdi_status = gsdi_perso_engine_ruim_read_imsi(slot,
                                                   UIM_CDMA_IMSI_M,
                                                   (void*)&sim_data_p->imsi);
  }
  else
  {
    /* Read IMSI_T */
    gsdi_status = gsdi_perso_engine_ruim_read_imsi(slot,
                                                   UIM_CDMA_IMSI_T,
                                                   (void*)&sim_data_p->imsi);
  }

  if(gsdi_status != GSDI_SUCCESS)
    return GSDI_ERROR;

  sim_data_p->cnl.len = 0;
  sim_data_p->gid1.len = 0;
  sim_data_p->gid2.len = 0;
  sim_data_p->dck.len  = 0;

   /* ------------------------------------------------------------------------
   The read operation was successful.  Need to move the current pointer
   of the mem_pointer_p provided to ensure it doesn't overwrite IMSI
   as a result of reading additional data
   ----------------------------------------------------------------------*/
  if(( mem_pointer_p->memory_curr + sim_data_p->imsi.len ) >
     mem_pointer_p->memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY USAGE ERROR",0,0,0);
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_get_sim_data */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CALC_RUIM_DATA_LEN

DESCRIPTION
  This function is called to calcuate the total memory required for the
  SIM Items to be retrieved.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_ruim_data_len(
  int32 * len_p
)
{
  /* ------------------------------------------------------------------------
   Validate all parameters
   ----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(len_p);
  /* ------------------------------------------------------------------------
   Calculate the total memory required.
   ----------------------------------------------------------------------*/
  *len_p = GSDI_PERSO_ENGINE_IMSI_LEN;

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_calc_sim_data_len */


/*===========================================================================
FUNCTION gsdi_perso_engine_set_ruim_mem_data

DESCRIPTION
  This function is used to create all of the relevant Personalization Files
  used for storing information in EFS.

  This procedure is ran every time the mobile is powered up, but will not
  recreate any files already created.

  If all personalization files are not created properly or upon entering
  this function again, only some of the files exist...then an Error Fatal
  will occur.  This could be evidence of tampering.

  It will be acceptable for the file to be empty.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_set_ruim_mem_data(
  gsdi_perso_sec_me_data_type *enc_data
)
{
  gsdi_perso_engine_mem_pointer_type mem_pointer;
  uint32 total_len;

  MMGSDI_RETURN_IF_NULL(enc_data);

  memset(enc_data, 0x00, sizeof(gsdi_perso_sec_me_data_type));

  /*------------------------------------------------------------------------
  Feature Inds
  --------------------------------------------------------------------------*/
  enc_data->feat_inds.len = GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS * 2;
  total_len = enc_data->feat_inds.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
  SIM Lock Codes
  --------------------------------------------------------------------------*/
  enc_data->sim_lock_codes.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE)
                                     * GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  total_len += enc_data->sim_lock_codes.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
  DCK
  --------------------------------------------------------------------------*/
  enc_data->dcks.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE) *
                                 GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  enc_data->dcks.len += 8 * 2* GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  total_len +=  enc_data->dcks.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
  DCK Counter
  --------------------------------------------------------------------------*/
  enc_data->dck_counts.len = GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS *2;
  total_len += enc_data->dck_counts.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /*------------------------------------------------------------------------
  UNBLOCK CK
  --------------------------------------------------------------------------*/
  enc_data->unblock_cks.len = (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE) *
                                 GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  enc_data->unblock_cks.len += 8 * 2* GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  total_len +=  enc_data->unblock_cks.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);


  /*------------------------------------------------------------------------
  UNBLOCK CK Counter
  --------------------------------------------------------------------------*/
  enc_data->unblock_ck_counts.len = GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS *2;
  total_len += enc_data->unblock_ck_counts.len;
  total_len += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  /* ---------------------------------------------------------------------------
    Allocate the entire buffer required for the encrypted data structure
    Memset it to 0x00 when successful
    Set memory_pointer.memory_curr = memory_pointer.memory_start
    Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
   -------------------------------------------------------------------------*/
  mem_pointer.memory_start = mmgsdi_malloc(total_len);
  mem_pointer.memory_curr  = mem_pointer.memory_start;
  mem_pointer.memory_end   = mem_pointer.memory_start + total_len;

  MMGSDI_RETURN_IF_NULL(mem_pointer.memory_start);
  memset(mem_pointer.memory_start, 0x00, total_len);

  /*-------------------------------------------------------------------------
  Set the Feat Ind Buffer Pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);

  enc_data->feat_inds.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->feat_inds.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(  mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->feat_inds.len;

  /*-------------------------------------------------------------------------
  Set the SIM Lock Code Buffer Pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->sim_lock_codes.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->sim_lock_codes.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(  mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->sim_lock_codes.len;

  /*-------------------------------------------------------------------------
  Set the DCK Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->dcks.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->dcks.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(  mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->dcks.len;

  /*-------------------------------------------------------------------------
  Set the DCK Counter Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->dck_counts.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->dck_counts.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->dck_counts.len;


  /*-------------------------------------------------------------------------
  Set the UNblock CK Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->unblock_cks.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->unblock_cks.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->unblock_cks.len;


  /*-------------------------------------------------------------------------
  Set the Unblock CK Counter Memory Buffer pointer
  -------------------------------------------------------------------------*/
  mem_pointer.memory_curr += (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE);
  enc_data->unblock_ck_counts.buffer_p = mem_pointer.memory_curr;

  if((mem_pointer.memory_curr + enc_data->unblock_ck_counts.len) > mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }
  mem_pointer.memory_curr += enc_data->unblock_ck_counts.len;


  if(mem_pointer.memory_curr != mem_pointer.memory_end)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Memory Corruption!",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE( mem_pointer.memory_start);
    return GSDI_ERROR;
  }


  enc_data->data_mod.update_fs = TRUE;
  return GSDI_SUCCESS;
}


#ifndef FEATURE_MMGSDI_PERSO_SFS
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_RUIM_FILE_TEST

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_ruim_fs_file_test(void)
{
  gsdi_returns_T gsdi_status;
  gsdi_status = gsdi_efs_name_test(mmgsdi_efs_ruim_perso_file);
  if(GSDI_SUCCESS != gsdi_status)
  {
    MMGSDI_DEBUG_ERR("PERSO: EFS PERSO_FILE MISSING",0,0,0);
    return GSDI_NOT_FOUND;
  }
  return GSDI_SUCCESS;
}


/*===========================================================================
FUNCTION GSDI_PERSO_CREATE_RUIM_FILE

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_create_ruim_file(void)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  /* --------------------------------------------------------------------------
   Create the Perso File
   ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_ruim_perso_file);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("PERSO: Could not create Perso File",0,0,0);
    return gsdi_status;
  }
  return gsdi_status;
}
#endif /* !FEATURE_MMGSDI_PERSO_SFS */

/* ============================================================================
FUNCTION: GSDI_PERSO_ENGINE_WRITE_BACK_RUIM_FS

DESCRIPTION:
  Function will write back to FS any data indicated to be modified.  The
  indication is set by the Security Library.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

============================================================================ */
static gsdi_returns_T gsdi_perso_engine_write_back_ruim_fs
(
  gsdi_perso_sec_me_data_type * encrypted_me_data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint32          len         = 0;
  uint8 *        buffer_p    = NULL;
  gsdi_perso_sec_data_type   *sec_data_p;
  int32          i           = 0;
#ifdef FEATURE_MMGSDI_PERSO_SFS
  int total;
#endif
  /* --------------------------------------------------------------------------
   Validate the Paramaters provided
   ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_me_data_p);


  /* If update fs is not required, exist out of the function */
  if(!encrypted_me_data_p->data_mod.update_fs)
  {
    return GSDI_SUCCESS;
  }
  /* Update to FS is required, continue with the process */
  len = encrypted_me_data_p->feat_inds.len +
        encrypted_me_data_p->sim_lock_codes.len +
        encrypted_me_data_p->dcks.len +
        encrypted_me_data_p->dck_counts.len +
        encrypted_me_data_p->unblock_cks.len +
        encrypted_me_data_p->unblock_ck_counts.len +
        (GSDI_PERSO_ENGINE_TAG_SIZE + GSDI_PERSO_ENGINE_LEN_SIZE) * GSDI_PERSO_ENGINE_NUMBER_OF_TAGS;

  buffer_p= mmgsdi_malloc(len);
  /*Write the Feature Inds*/

  MMGSDI_RETURN_IF_NULL(buffer_p);

  len =0;

  for(i =1;i<=GSDI_PERSO_ENGINE_NUMBER_OF_TAGS;i++)
  {
    switch(i)
    {
      case GSDI_PERSO_ENGINE_TAG_FEATURE_INDS:
        sec_data_p = &encrypted_me_data_p->feat_inds;
        break;
      case GSDI_PERSO_ENGINE_TAG_PERM_FEATURE_INDS:
        continue;
      case GSDI_PERSO_ENGINE_TAG_SIM_LOCK_CODE:
        sec_data_p = &encrypted_me_data_p->sim_lock_codes;
        break;
      case GSDI_PERSO_ENGINE_TAG_DEPERSO_CK:
        sec_data_p = &encrypted_me_data_p->dcks;
        break;
      case GSDI_PERSO_ENGINE_TAG_DCK_COUNTER:
        sec_data_p = &encrypted_me_data_p->dck_counts;
        break;
      case GSDI_PERSO_ENGINE_TAG_UNBLOCK:
        sec_data_p = &encrypted_me_data_p->unblock_cks;
        break;
      case GSDI_PERSO_ENGINE_TAG_UNBLOCK_COUNTER:
        sec_data_p = &encrypted_me_data_p->unblock_ck_counts;
        break;
      default:
        MSG_ERROR("PERSO: Incorrect tag",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(buffer_p);
        return GSDI_ERROR;
    }

    buffer_p[len]   = (uint8)i;
    len            += GSDI_PERSO_ENGINE_TAG_SIZE;
    buffer_p[len]   = (uint8)(sec_data_p->len/255);
    buffer_p[len+1] = (uint8)(sec_data_p->len%255);
    len            += GSDI_PERSO_ENGINE_LEN_SIZE;

    memcpy(buffer_p + len, sec_data_p->buffer_p, sec_data_p->len);
    len += sec_data_p->len;

  }
#ifdef FEATURE_MMGSDI_PERSO_SFS
  total = sfs_priv_simloc_write(buffer_p, len);
  if(total != len)
#else
  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_ruim_perso_file,
                                    (int32)len,
                                    buffer_p);
  if(gsdi_status != GSDI_SUCCESS)
#endif /*FEATURE_MMGSDI_PERSO_SFS*/
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to write: %s", mmgsdi_efs_ruim_perso_file,0,0);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(buffer_p);

  return gsdi_status;
} /* gsdi_perso_engine_write_back_ruim_fs */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CREATE_RUIM_DATA

DESCRIPTION
  This function is called to calcuate the total memory required for the
  SIM Items to be retrieved.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_create_ruim_data(int32       * returned_data_len_p,
                                                  const uint8 * data_p)
{
  gsdi_perso_engine_mem_pointer_type mem_pointer;
  gsdi_perso_sec_me_data_type encrypted_data;
  gsdi_returns_T              gsdi_status;

  /* --------------------------------------------------------------------------
    The Perso Engine Has an Interface to allow the Personalization
    Engine to determine how much memory needs to be allocated.
  ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_engine_set_ruim_mem_data(&encrypted_data);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
    return GSDI_ERROR;
  }
  memset( &mem_pointer, 0, sizeof(gsdi_perso_engine_mem_pointer_type) );
  mem_pointer.memory_start = encrypted_data.feat_inds.buffer_p;
  mem_pointer.memory_start -= (GSDI_PERSO_ENGINE_TAG_SIZE +  GSDI_PERSO_ENGINE_LEN_SIZE);

  gsdi_status = gsdi_perso_ruim_security_create_data(&encrypted_data,
                                                      data_p,
                                                      returned_data_len_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("PERSO: Could not encrypt data",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
    return gsdi_status;
  }

#ifndef FEATURE_MMGSDI_PERSO_SFS
  gsdi_status = gsdi_perso_engine_create_ruim_file();
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("PERSO: Could not create dirs or files",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
    return gsdi_status;
  }
#endif /*FEATURE_MMGSDI_PERSO_SFS*/



  gsdi_status = gsdi_perso_engine_write_back_ruim_fs(&encrypted_data);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not write back FS",0,0,0);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mem_pointer.memory_start);
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINGE_GET_RUIM_ESN_SIZE

DESCRIPTION
  This function is called to calcuate the total memory required for the
  ESN Buffer

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_get_ruim_esn_size(
  uint32 * len_p
)
{
  MMGSDI_RETURN_IF_NULL(len_p);
  *len_p = GSDI_PERSO_HW_ESN_LENGTH;
  return GSDI_SUCCESS;
} /* gsdi_perso_engine_get_ruim_esn_size */

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_CALC_ENC_DATA_LEN

DESCRIPTION
  This function is called to calcuate the total memory required for a
  particular item.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_calc_ruim_enc_data_len(
  gsdi_perso_engine_proc_enum_type      feature_ind,
  const void                           *req_p,
  int32                                *len_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  int32          len         = 0;
#ifdef FEATURE_MMGSDI_PERSO_SFS
  uint8 *data_buf;
#endif

  /* -----------------------------------------------------------------------
   Do the parameter checking
  ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(len_p);

  /* -----------------------------------------------------------------------
   Retrieve the File Size from FS
   ---------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_PERSO_SFS
  data_buf = mmgsdi_malloc(1000);
  len= sfs_priv_simloc_read(data_buf, 1000);

  if(len == 0)
#else
  gsdi_status = gsdi_fs_get_file_size(mmgsdi_efs_ruim_perso_file, &len);

  if(gsdi_status != GSDI_SUCCESS ||
     len == 0)
#endif
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: FILE Status: 0x%x File Size: 0x%x",
                           gsdi_status,len,0);
    return GSDI_ERROR;
  }

  *len_p = len;

  /*Also cater for the new data that may be added*/
  if(feature_ind == GSDI_PERSO_ENGINE_SET_DATA_FEATURE)
  {
    MMGSDI_RETURN_IF_NULL(req_p);
    *len_p += ((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes *2;
  }
  /* Check to see if new SIM Lock Codes have to be generated */
  else if(feature_ind == GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK)
  {
    *len_p += (int32)(GSDI_PERSO_BUFFER_LEN -
                      ((int32)GSDI_PERSO_ENGINE_TAG_SIZE * GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS));
  }

  /* ------------------------------------------------------------------------
    Get the total size of the Mobile's ESN
    -----------------------------------------------------------------------*/
  len =0;
  gsdi_status = gsdi_perso_engine_get_ruim_esn_size((uint32*)&len);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    *len_p = 0;
    return gsdi_status;
  }

  /* Accumulate the length of the Deperso keys */
  *len_p = *len_p + len;

  return GSDI_SUCCESS;

} /* gsdi_perso_engine_calc_enc_data_len */

/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_RUIM_ENCRYPTED_DATA

DESCRIPTION:
  Will gather all of the information required to activate the feature
  indication provided.

  Information gather is as follows:

  1)  Secured / Encrypted Data - Personalization Keys
  2)  Secured / Encrypted Data - Feature Indicators
  3)  Secured / Encrypted Data - Depersonalization Key Status
  4)  Get the IMEI Value
  5)  Get the Mobile Serial Number

DEPENDENCIES:
  Based on the Personalization State and Self Personalization State.
  gsdi_perso_self_perso_state_enum_type
  gsdi_perso_state_enum_type

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
static gsdi_returns_T gsdi_perso_engine_get_ruim_encrypted_data(
  gsdi_perso_sec_me_data_type        * sec_data_p,
  gsdi_perso_engine_mem_pointer_type * mem_pointer_p,
  gsdi_perso_engine_proc_enum_type     feature_type,
  const void                         * req_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint32 file_size =0;
  uint8 *file_buffer_p = NULL;
  uint8 *orig_file_buffer_p = NULL;
  int i;

  /* --------------------------------------------------------------------------
   Perform Paramter checking
   ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sec_data_p);
  MMGSDI_RETURN_IF_NULL(mem_pointer_p);

  memset(&sec_data_p->feat_inds,      0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->sim_lock_codes ,0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->dcks,           0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->dck_counts,     0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->unblock_cks,    0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->unblock_ck_counts, 0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->me_imei,        0x00 , sizeof(gsdi_perso_sec_data_type));
  memset(&sec_data_p->me_sn,          0x00 , sizeof(gsdi_perso_sec_data_type));

  /* --------------------------------------------------------------------------
   Using the Secure FS Interface, get the Perso Data.
   ------------------------------------------------------------------------*/


#ifdef FEATURE_MMGSDI_PERSO_SFS
  file_buffer_p = mmgsdi_malloc(1000);
  MMGSDI_RETURN_IF_NULL(file_buffer_p);
  file_size = sfs_priv_simloc_read((uint8*)file_buffer_p, 1000);
  if(file_size   == 0)
#else
  gsdi_status = gsdi_fs_get_file_size(mmgsdi_efs_ruim_perso_file,(int32*)&file_size);

  if(gsdi_status != GSDI_SUCCESS ||
     file_size == 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: FILE Status: 0x%x File Size: 0x%x",
                           gsdi_status,file_size,0);
    return GSDI_ERROR;
  }

  file_buffer_p = mmgsdi_malloc(file_size);
  MMGSDI_RETURN_IF_NULL(file_buffer_p);

  gsdi_status =  gsdi_efs_read_file(mmgsdi_efs_ruim_perso_file,
                                    (int32*)&file_size,
                                    file_buffer_p);

  if(gsdi_status != GSDI_SUCCESS ||
     file_size   == 0)
#endif
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: FS Status: 0x%x Read Size 0x%x Expected Size 0x%x",
                           gsdi_status,file_size,file_buffer_p);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(file_buffer_p);
    return gsdi_status;
  }

  orig_file_buffer_p = file_buffer_p;
  for(i=0;i<GSDI_PERSO_ENGINE_NUMBER_OF_TAGS-1;i++)
  {
    /*Switch on the Tags*/
    switch(*file_buffer_p)
    {
      case GSDI_PERSO_ENGINE_TAG_FEATURE_INDS:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->feat_inds.buffer_p  = mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->feat_inds.len      =  (*file_buffer_p) + *(file_buffer_p +1);
        memcpy(sec_data_p->feat_inds.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->feat_inds.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->feat_inds.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->feat_inds.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_SIM_LOCK_CODE:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->sim_lock_codes.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->sim_lock_codes.len  =  (*file_buffer_p + *(file_buffer_p +1));
        memcpy(sec_data_p->sim_lock_codes.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->sim_lock_codes.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->sim_lock_codes.len;

        /* Check to see if new SIM Lock Codes have to be generated*/
        if(feature_type == GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK && sec_data_p->sim_lock_codes.len == 0x13)
        {
          mem_pointer_p->memory_curr += GSDI_PERSO_BUFFER_LEN -
                                 (GSDI_PERSO_ENGINE_TAG_SIZE * GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS);
          /* The sec_adta_p->dck is set to memory_curr within

            gsdi_perso_engine_get_enc_dcks */
        }
        else if(feature_type == GSDI_PERSO_ENGINE_SET_DATA_FEATURE)
        {
          MMGSDI_RETURN_IF_NULL(req_p);
          mem_pointer_p->memory_curr += ((gsdi_perso_set_feature_data_req_T *)req_p)->num_bytes *2;
        }
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->sim_lock_codes.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_DEPERSO_CK:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->dcks.buffer_p =  mem_pointer_p->memory_curr  + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->dcks.len      =  (*file_buffer_p) + *(file_buffer_p +1);
        memcpy(sec_data_p->dcks.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->dcks.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dcks.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dcks.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_DCK_COUNTER:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->dck_counts.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->dck_counts.len      =  (*file_buffer_p) + *(file_buffer_p +1);
        memcpy(sec_data_p->dck_counts.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->dck_counts.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dck_counts.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->dck_counts.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_UNBLOCK:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->unblock_cks.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->unblock_cks.len      =  (*file_buffer_p) +*(file_buffer_p +1);
        memcpy(sec_data_p->unblock_cks.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->unblock_cks.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_cks.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_cks.len;
        break;

      case GSDI_PERSO_ENGINE_TAG_UNBLOCK_COUNTER:
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_TAG_SIZE;
        file_buffer_p += GSDI_PERSO_ENGINE_TAG_SIZE;
        sec_data_p->unblock_ck_counts.buffer_p =  mem_pointer_p->memory_curr + GSDI_PERSO_ENGINE_LEN_SIZE;

        sec_data_p->unblock_ck_counts.len      =  (*file_buffer_p) + *(file_buffer_p +1);
        memcpy(sec_data_p->unblock_ck_counts.buffer_p, file_buffer_p + GSDI_PERSO_ENGINE_LEN_SIZE, (uint32)sec_data_p->unblock_ck_counts.len);
        mem_pointer_p->memory_curr += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_ck_counts.len;
        file_buffer_p += GSDI_PERSO_ENGINE_LEN_SIZE + sec_data_p->unblock_ck_counts.len;
        break;
      default:
        MSG_ERROR("PERSO: Invalid Tag",0,0,0);
        return GSDI_ERROR;
    }


    /* --------------------------------------------------------------------------
     The operation was successfull.
     Need to move the current Pointer over so that the data it points to isn't
     accidentally overwritten.  Ensure memory_curr does not exceed
     memory_end.
     ------------------------------------------------------------------------*/
    if(( mem_pointer_p->memory_curr ) > mem_pointer_p->memory_end)
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: MEMORY POINTER USAGE ERROR",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(orig_file_buffer_p);
      return GSDI_ERROR;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(orig_file_buffer_p);
  /* --------------------------------------------------------------------------
  Using the NV Items Get ESN Value (and stores it in the IMEI buffer).
  The ESN comes as a DWORD.  The Most significant byte is stored in the first
  byte of the imei buffer, the second most significant byte is stoerd in the
  second buffer byte, etc.  The DWORD is always 4 bytes (unless an error occured
  in which case there should be three).
  ------------------------------------------------------------------------*/
  sec_data_p->me_imei.buffer_p = mem_pointer_p->memory_curr;

  gsdi_status = gsdi_perso_engine_get_esn(
                       sec_data_p->me_imei.buffer_p,
                       &sec_data_p->me_imei.len);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: ENGINE Get ESN Failure: 0x%x",
               gsdi_status,0,0);
  }
  return gsdi_status;
}
/*=============================================================================
FUNCTION: GSDI_PERSO_ENGINE_GET_ESN

DESCRIPTION:
  Will retrieve the ESN number from NV.

DEPENDENCIES:
  None

LIMITATIONS:
  The memory allocated in this function will need to be released by the
  caller of this function.

RETURN TYPE:
  gsdi_returns_T

================================================================================*/
gsdi_returns_T gsdi_perso_engine_get_esn (
  uint8 *  esn_data_p,
  uint32 * esn_len_p
)
{
  dword              cached_esn = 0;
  int32                i = 0;

  if(esn_data_p == NULL || esn_len_p == NULL)
  {
    MSG_ERROR("PERSO: NULL pointer in gsdi_perso_engine_get_esn",0,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }
  /* ------------------------------------------------------------------------
  Is the ESN Provided a valid ESN
  ----------------------------------------------------------------------*/
  cached_esn = tmc_get_stored_esn_me();

  if ( cached_esn == 0 )
  {
    memset(esn_data_p,0x0,GSDI_PERSO_HW_ESN_LENGTH);
    *esn_len_p = 0;
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not get ESN",0,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  /*---------------------------------------------------------------------------
  ESN comes as a dword.  We put the Most significant byte is put into first byte
  of esn_data_array, second most significant byte is put into second array byte, etc.
  ---------------------------------------------------------------------------*/

  *esn_len_p = GSDI_PERSO_HW_ESN_LENGTH;
  i = GSDI_PERSO_HW_ESN_LENGTH - 1;
  while(i >= 0)
  {
    esn_data_p[i] = (byte)(cached_esn & GSDI_PERSO_ESN_LAST_BYTE_MASK);
    cached_esn = cached_esn >> GSDI_PERSO_ESN_LAST_BYTE_SHIFT;
    --i;
  }

  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_RUIM_CALC_MEM_REQUIRED

DESCRIPTION
  This function is called to calcuate the total memory required for a
  particular item.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_engine_ruim_calc_mem_required(
  gsdi_perso_eng_calc_req_mem_enum_type calc_type,
  gsdi_perso_engine_proc_enum_type      feature_ind,
  const void                          * req_p,
  int32                               * required_len_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_SUCCESS;
  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(required_len_p);
  MMGSDI_RETURN_IF_NULL(req_p);

  switch(calc_type)
  {
    case GSDI_PERSO_ENG_CALC_RUIM_ENC_DATA:
     /* ----------------------------------------------------------------------
       The total memory required is the sum of the following:
        Buffer size for the Feature Indicators
        Buffer size for the Depersonalization Keys
        Buffer size for the Depersonalization Counters
        Buffer size for the Mobile's IMEI
        Buffer size for the Mobile's Serial Number
       --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_calc_ruim_enc_data_len(feature_ind,
                                                             req_p,
                                                             required_len_p);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return gsdi_status;
      }
      break;

    case GSDI_PERSO_ENG_CALC_RUIM_DATA:
      /* ----------------------------------------------------------------------
       The total memory required is the sum of the following:
        Buffer size for the IMSI
        Buffer size for the CNL
        Buffer size for the GID1
        Buffer size for the GID2
       --------------------------------------------------------------------*/
      gsdi_status = gsdi_perso_engine_calc_ruim_data_len(required_len_p);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return gsdi_status;
      }
      break;
    default:
      gsdi_status = GSDI_ERROR;
      MSG_ERROR("PERSO: Wrong calc_type 0x%x",calc_type,0,0); 
      return gsdi_status;
   }
   return GSDI_SUCCESS;
}


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_HANDLE_RUIM_REQUEST

DESCRIPTION
  This function is the main handler for all the perso requests.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_handle_ruim_request(
                                      gsdi_perso_engine_proc_enum_type feature_type,
                                      gsdi_perso_sec_client_req_type  *client_data,
                                      const void *                     req_p,
                                      uint8 *                          ret_data_p,
                                      int32*                           ret_data_len_p,
                                      gsdi_perso_event_enum_type       *perso_event_p)
{
   gsdi_perso_engine_mem_pointer_type ruim_mem_pointer;
   gsdi_perso_sec_sim_data_type       ruim_data;
   gsdi_returns_T                     gsdi_status;
   uint32                             total_mem_req      = 0x00;
   gsdi_perso_engine_mem_pointer_type enc_mem_pointer;
   gsdi_perso_sec_me_data_type        encrypted_data;

   /* --------------------------------------------------------------------------
     Initialize Data as necessary
     ------------------------------------------------------------------------*/
   memset(&enc_mem_pointer,0x00, sizeof(gsdi_perso_engine_mem_pointer_type));
   memset(&encrypted_data.data_mod, 0, sizeof(gsdi_perso_sec_data_mod_inds_type));
   memset(&ruim_mem_pointer,0x00, sizeof(gsdi_perso_engine_mem_pointer_type));

  /* --------------------------------------------------------------------------
     The Perso Engine Has an Interface to allow the Personalization
     Engine to determine how much memory needs to be allocated.
     ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_engine_ruim_calc_mem_required(GSDI_PERSO_ENG_CALC_RUIM_ENC_DATA,
                                                          feature_type,
                                                          (void *)req_p,
                                                          (int32*)&total_mem_req);

   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
     return gsdi_status;
   }

   /* ---------------------------------------------------------------------------
     Allocate the entire buffer required for the encrypted data strcture
     Memset it to 0x00 when successful and the length required is greater
     than 0.
     Set memory_pointer.memory_curr = memory_pointer.memory_start
     Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
    -------------------------------------------------------------------------*/
   if (total_mem_req > 0)
   {
     enc_mem_pointer.memory_start = mmgsdi_malloc(total_mem_req);
     MMGSDI_RETURN_IF_NULL(enc_mem_pointer.memory_start);
     memset(enc_mem_pointer.memory_start, 0x00, total_mem_req);

     enc_mem_pointer.memory_curr = enc_mem_pointer.memory_start;
     enc_mem_pointer.memory_end  = enc_mem_pointer.memory_start + total_mem_req;
   /* ---------------------------------------------------------------------------
      Populate the encrypted data into the encrypted_data structure.
      The memory pointer will be used to provide memory for the
      encrypted_data structure buffers
      -------------------------------------------------------------------------*/
     gsdi_status = gsdi_perso_engine_get_ruim_encrypted_data(&encrypted_data,
                                                             &enc_mem_pointer,
                                                             feature_type,
                                                             (void *)req_p);

     if(gsdi_status != GSDI_SUCCESS)
     {
        /* An error occurred getting the Encrypted
         information from FS. All memory will be freed.
        */
       MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
       return gsdi_status;
     }
   }
   else
   {
      MSG_MED("PERSO: Encrypted Data Not Required from GSDI ENGINE",0,0,0);
   }


  /* --------------------------------------------------------------------------
     The Perso Engine Has an Interface to allow the Personalization
     Engine to determine how much memory needs to be allocated.
     ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_engine_ruim_calc_mem_required(GSDI_PERSO_ENG_CALC_RUIM_DATA,
                                                         feature_type,
                                                         (void *)req_p,
                                                         (int32*)&total_mem_req);

   if(gsdi_status != GSDI_SUCCESS)
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get total memory required",0,0,0);
     MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
     return gsdi_status;
   }

   /* ---------------------------------------------------------------------------
     Allocate the entire buffer required for the sim data structure
     Memset it to 0x00 when successful
     Set memory_pointer.memory_curr = memory_pointer.memory_start
     Set memory_pointer.memory_end  = memory_pointer.memory_start + total_mem_req
     -------------------------------------------------------------------------*/
   ruim_mem_pointer.memory_start = mmgsdi_malloc(total_mem_req);
   if(ruim_mem_pointer.memory_start == NULL)
   {
     MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
     MSG_ERROR("PERSO: NULL Pointer in gsdi_perso_engine_handle_ruim_request",0,0,0);
     return GSDI_ERROR;
   }
   memset(ruim_mem_pointer.memory_start, 0x00, total_mem_req);

   ruim_mem_pointer.memory_curr = ruim_mem_pointer.memory_start;
   ruim_mem_pointer.memory_end  = ruim_mem_pointer.memory_start + total_mem_req;

   /* --------------------------------------------------------------------------
    Get the required SIM Data in the case that the SIM Lock State is in
    AUTOLOCK for the Feature Indicated.
    ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_engine_get_ruim_data(GSDI_SLOT_1,
                                                 &ruim_data,
                                                 &ruim_mem_pointer);
   if ( gsdi_status != GSDI_SUCCESS )
   {
     /* An error occurred getting the  data
        All memory will be freed.
      */
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Failed to get SIM Data",0,0,0);
     MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
     MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ruim_mem_pointer.memory_start);
     return gsdi_status;
   }

   gsdi_status = gsdi_perso_security_handle_request(feature_type,
                                                    client_data,
                                                    &encrypted_data,
                                                    &ruim_data,
                                                    ret_data_p,
                                                    ret_data_len_p,
                                                    perso_event_p);
   if ( gsdi_status == GSDI_SUCCESS )
   {
     MSG_HIGH("PERSO SECURITY 0x%x:  Success",feature_type,0,0);
   }
   else
   {
     MSG_ERROR("PERSO SECURITY 0x%x: FAILURE 0x%x",feature_type,gsdi_status,0);
   }

   if(GSDI_SUCCESS !=  gsdi_perso_engine_write_back_ruim_fs(&encrypted_data))
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not write back FS",0,0,0);
     gsdi_status =  GSDI_ERROR;
   }

   MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(enc_mem_pointer.memory_start);
   MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ruim_mem_pointer.memory_start);
   return gsdi_status;

  }

#endif /*FEATURE_PERSO_RUIM*/
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

