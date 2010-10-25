/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   L I B R A R Y


GENERAL DESCRIPTION

  This file contains jcdma ruim functions

  
                        COPYRIGHT INFORMATION

Copyright (c) 2007-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_ruim_jcdma.c#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/17/08   nb      Resolved compiler warning
06/04/08   tml     Added sw to cnf header
01/24/08   nk      Added nv write if jcdma nv items are not active
11/20/07   vs      Fixed warning, lint error due to empty translation unit
11/15/07   sun     Lint Fixes
06/20/07   nk      Fixed mem free issue.
05/31/07   nk      Fixed sending and building of cnf
05/30/07   nk      Resolved compiler warning
05/24/07   nk      Resolved lint error
05/17/07   nk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
#include "gsdi.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "uimgen.h"
#include "nv_items.h"
#include "gsdi_nv.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_cnf.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================
FUNCTION MMGSDI_PROCESS_JCDMA_CARD_GET_INFO_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE:  
  MMGSDI_SUCCESS: upon succssesful processing of command.
  MMGSDI_ERROR: upon error

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_jcdma_card_get_info_cmd (
  const mmgsdi_jcdma_get_card_info_req_type *req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  
  MSG_MED("MMGSDI JCDMA GET CARD INFO", 0, 0, 0);

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("JCDMA get client request failed", 0,0,0);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index, &req_ptr->request_header,
                                                                  NULL,
                                                                  NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("JCDMA populate client request failed", 0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* Get card info */
  mmgsdi_status = mmgsdi_jcdma_card_get_attr();
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("JCDMA get card info failed", 0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp( index, MMGSDI_JCDMA_GET_CARD_INFO_REQ, MMGSDI_SUCCESS);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("JCDMA rsp queue failed", 0,0,0);
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_process_jcdma_card_get_info_cmd() */


/*===========================================================================
FUNCTION MMGSDI_JCDMA_RSP_GET_CARD_INFO

DESCRIPTION
  This function sends response 

DEPENDENCIES
  None
  
LIMITATIONS
  NONE
  
RETURN VALUE:  
   MMGSDI_SUCCESS: upon successful queueing of the conf.
   MMGSDI_ERROR: upon error
  
SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_rsp_get_card_info (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;

  MSG_HIGH("MMGSDI JCDMA GET CARD INFO RSP", 0, 0, 0);
  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(uim_rsp_ptr->rsp_data_index, 
                                    0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info( uim_rsp_ptr->rsp_data_index,
                                                             &req_header,
                                                             &uim_cmd_ptr,
                                                             &extra_param_ptr);
  
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR(
      "Error returned from mmgsdi_util_get_client_request_table_info (%d)",
      mmgsdi_status,
      0,
      0);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status, 
                                             &req_header, 
                                             NULL, 
                                             NULL,
                                             FALSE,
                                             uim_rsp_ptr->status_word);
  
  MSG_HIGH("RUIM_RSP - JCDMA GET CARD INFO  status is 0x%x", 
    report_status, 0, 0);
  return mmgsdi_status;
} /* mmgsdi_jcdma_rsp_get_card_info() */


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_COLOR_AND_LOCKS

DESCRIPTION:
  This function reads files from card and decides card color. It also reads 
  UIM lock indicator to decide if lock checks are needed. 
  Files read from card: MMGSDI_CDMA_IMSI_M  
                        MMGSDI_CDMA_IMSI_STATUS
                        MMGSDI_CDMA_UIM_VERSION 
                        MMGSDI_CDMA_RUIM_ID
                        MMGSDI_CDMA_GID1
                        MMGSDI_CDMA_GID2

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:  
  MMGSDI_SUCCESS:  Upon successfuly parsing read file data
  MMGSDI_ERROR:    Upon error 
     
SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_card_get_color_and_locks(
  mmgsdi_jcdma_card_attr_type *card_attr_ptr)
{      
   mmgsdi_return_enum_type       mmgsdi_status;
   mmgsdi_read_req_type        * read_req_ptr = NULL;
   /* response data */
   mmgsdi_read_cnf_type        * read_cnf_ptr = NULL; 

   /* JCDMA card signature */
   uint8 mmgsdi_jcdma_signature[16] = {0xAF, 0xC4, 0x59, 0xFF, 
                                       0x53, 0x70, 0xD9, 0x4E, 
                                       0xC0, 0xAF, 0x47, 0xF0, 
                                       0xBC, 0x07, 0xC1, 0xEB}; 

   MMGSDIUTIL_RETURN_IF_NULL(card_attr_ptr);

   /* Initial value for card type */
   card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;

   /* Allocate memory space */
   MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(read_req_ptr, 
                                             sizeof(mmgsdi_read_req_type));        
   
   /*-------------------------------------- 
    Read JCDMA version and signature EF 
   --------------------------------------*/
   read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
   read_req_ptr->access.file.file_enum = MMGSDI_CDMA_UIM_VERSION;
   read_req_ptr->data_len              = 0; /* read entire file */
   read_req_ptr->rec_num               = 0;
   read_req_ptr->offset                = 0;
   read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
   read_req_ptr->request_header.slot_id = MMGSDI_SLOT_1;
   mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, &read_cnf_ptr);
  
   if(read_cnf_ptr == NULL)
   {       
     MSG_ERROR("Read Cnf Ptr is NULL", 0, 0, 0);
     MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
     return MMGSDI_ERROR;
   }

   if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
        (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
   {       
      if (read_cnf_ptr->read_data.data_ptr == NULL)
      {      
         MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         return MMGSDI_ERROR;
      }

      /* Check the version. Test card is invalid */
      if (MMGSDI_JCDMA_UIM_TEST_VERSION == read_cnf_ptr->read_data.data_ptr[0])
      {      
         MSG_HIGH("Invalid. JCDMA test card", 0,0,0);
         card_attr_ptr->card_type = MMGSDI_JCDMA_UNKNOWN_CARD;
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         /* JCDMA test card. No need to proceed */
         return MMGSDI_SUCCESS;
      }

      /* Compare the signature */
      if (memcmp (&read_cnf_ptr->read_data.data_ptr[1], mmgsdi_jcdma_signature, 16)) 
      {        
         MSG_HIGH("Invalid. JCDMA signature failed", 0,0,0);
         card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         /* JCDMAsignature mismatch. No need to proceed */
         return MMGSDI_SUCCESS;
      }

      /* Check skip UIM lock */
      if (read_cnf_ptr->read_data.data_ptr[17] & MMGSDI_JCDMA_UIM_LOCK_IND) 
      {  
         /* Lock is disabled */
         card_attr_ptr->lock_enabled = FALSE;
      }
      else
      {    
         /* Lock is enabled */
        card_attr_ptr->lock_enabled = TRUE;
      }
   }
   /* handling if read on MMGSDI_CDMA_UIM_VERSION file failed */
   else
   {    
      card_attr_ptr->card_type = MMGSDI_JCDMA_NON_JCDMA_CARD;
      MSG_HIGH("Invalid EF. JCDMA signature EF read failed", 0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      /* Failed to read JCDMA signature EF */
      return MMGSDI_SUCCESS;
   }

   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
   /*-------------------------------------- 
    It is JCDMA card. Read card color 
   --------------------------------------*/
   read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
   read_req_ptr->access.file.file_enum = MMGSDI_CDMA_IMSI_M;
   read_req_ptr->data_len              = 0; /* read entire file */
   read_req_ptr->rec_num               = 0;
   read_req_ptr->offset                = 0;
   read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;

   mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, &read_cnf_ptr);

   if(read_cnf_ptr == NULL)
   {
     MSG_ERROR("Read Cnf Ptr is NULL", 0, 0, 0);
     MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
     return MMGSDI_ERROR;
   }

   if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
        (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
   {   
      if (read_cnf_ptr->read_data.data_ptr == NULL)
      {
         MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         return MMGSDI_ERROR;
      }
      /* Parse the EF data to decide card color */
      if( (read_cnf_ptr->read_data.data_ptr[7] & 
           MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED) == 0)
      {  
         card_attr_ptr->card_type = MMGSDI_JCDMA_WHITE_CARD;
         MSG_HIGH("JCDMA white card", 0,0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         /* JCDMA white card. */ 
         return MMGSDI_SUCCESS;
      }
   }
   else
   {    
      MSG_ERROR("Invalid. JCDMA imsi_m EF read failed", 0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
   }

   /* Get NW lock code -- MCC from IMSI_M (16 bits)*/
   card_attr_ptr->imsi_mcc = (read_cnf_ptr->read_data.data_ptr[9] << 
                              MMGSDI_JCDMA_8BITS) | 
                              read_cnf_ptr->read_data.data_ptr[8];

   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

   /* read JCDMA IMSI STATUS for card color */
   read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
   read_req_ptr->access.file.file_enum = MMGSDI_CDMA_IMSI_STATUS;
   read_req_ptr->data_len              = 0; /* read entire file */
   read_req_ptr->rec_num               = 0;
   read_req_ptr->offset                = 0;
   read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;

   mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, 
                                                    &read_cnf_ptr);

   if(read_cnf_ptr == NULL)
   {   
     MSG_ERROR("Read Cnf Ptr is NULL", 0, 0, 0);
     MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
     return MMGSDI_ERROR;
   }

   if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
          (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
   {   
      if (read_cnf_ptr->read_data.data_ptr == NULL)
      {
         MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
         return MMGSDI_ERROR;
      }
      /* Parse the EF data to decide card color */
      if( (read_cnf_ptr->read_data.data_ptr[0] & MMGSDI_JCDMA_UIM_IMSI_STATUS) 
          == 0)
      {  
         /* JCDMA black card. */
         card_attr_ptr->card_type = MMGSDI_JCDMA_BLACK_CARD;
         MSG_HIGH("JCDMA black card", 0,0,0);
      }
      else 
      {  
         /* JCDMA gray card. */
         card_attr_ptr->card_type = MMGSDI_JCDMA_GRAY_CARD;
         MSG_HIGH("JCDMA gray card", 0,0,0);
      }
   }
   else
   {  
      MSG_ERROR("Invalid. JCDMA imsi status EF read failed", 0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
      return MMGSDI_ERROR;
   }
   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
   MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

   /*-------------------------------------- 
    Read Lock codes from card
   --------------------------------------*/
   if (card_attr_ptr->lock_enabled)
   {    
      /* read UIM ID for line lock */
      read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
      read_req_ptr->access.file.file_enum           = MMGSDI_CDMA_RUIM_ID;
      read_req_ptr->data_len              = 0; /* read entire file */
      read_req_ptr->rec_num               = 0;
      read_req_ptr->offset                = 0;
      read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
  
      mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, &read_cnf_ptr);
  
      if(read_cnf_ptr == NULL)
      {  
         MSG_ERROR("Read Cnf Ptr is NULL", 0, 0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         return MMGSDI_ERROR;
      }
      if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
           (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
      {   
         if (read_cnf_ptr->read_data.data_ptr == NULL)
         {         
            MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
            MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
            MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
            return MMGSDI_ERROR;
         }
         else
         {  
            memcpy (&card_attr_ptr->ruim_id, 
                    &read_cnf_ptr->read_data.data_ptr[1], 
                    sizeof (card_attr_ptr->ruim_id) );
         }
      }
      else
      {    
         /* Failed to read RUIM ID. Set to defalt */
         card_attr_ptr->ruim_id = 0xFFFFFFFF;
         MSG_ERROR("JCDMA ruim id EF read failed", 0,0,0);
      }
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
  
      /* read GIB1 for SP lock     */
      read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
      read_req_ptr->access.file.file_enum = MMGSDI_CDMA_GID1;
      read_req_ptr->data_len              = 0; /* read entire file */
      read_req_ptr->rec_num               = 0;
      read_req_ptr->offset                = 0;
      read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
  
      mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, &read_cnf_ptr);
  
      if(read_cnf_ptr == NULL)
      {
         MSG_ERROR("Read Cnf Ptr is NULL", 0, 0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         return MMGSDI_ERROR;
      }

      if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
           (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
      { 
         if (read_cnf_ptr->read_data.data_ptr == NULL)
         {  
            MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
            MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
            MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
            return MMGSDI_ERROR;
         }
         else
         {   
            card_attr_ptr->sp_id = read_cnf_ptr->read_data.data_ptr[0]; 
         }
      }
      else
      {
         /* Failed to read SP code. Set to defalt */
         card_attr_ptr->sp_id = MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
         MSG_ERROR("JCDMA gid1 EF read failed", 0,0,0);
      }
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
  
      /* read GID2 for UG lock     */
      read_req_ptr->access.access_method  = MMGSDI_EF_ENUM_ACCESS;
      read_req_ptr->access.file.file_enum = MMGSDI_CDMA_GID2;
      read_req_ptr->data_len              = 0; /* read entire file */
      read_req_ptr->rec_num               = 0;
      read_req_ptr->offset                = 0;
      read_req_ptr->file_type             = MMGSDI_TRANSPARENT_FILE;
  
      mmgsdi_status = mmgsdi_uim_icc_read_transparent( read_req_ptr, TRUE, &read_cnf_ptr);
  
      if(read_cnf_ptr == NULL)
      { 
         MSG_ERROR("Read Cnf Ptr is NULL", 0, 0,0);
         MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
         return MMGSDI_ERROR;
      }

      if ( (mmgsdi_status == MMGSDI_SUCCESS) && 
           (read_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
      { 
         if (read_cnf_ptr->read_data.data_ptr == NULL)
         {
            MSG_ERROR("JCDMA get card color memory NULL", 0,0,0);
            MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
            MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
            return MMGSDI_ERROR;
         }
         else
         {
            card_attr_ptr->ug_id = read_cnf_ptr->read_data.data_ptr[0]; 
         }
      }
      else
      {  
         /* Failed to read UG code. Set to defalt */
         card_attr_ptr->ug_id = MMGSDI_JCDMA_UIM_SP_UG_DEFAULT;
         MSG_ERROR("JCDMA gid2 EF read failed", 0,0,0);
      }
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr->read_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);
   }  /* if(card_attr_ptr->lock_enabled) */

   MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
   return MMGSDI_SUCCESS;

} /* mmgsdi_jcdma_card_get_color_and_locks() */


/*===========================================================================
FUNCTION:      MMGSDI_JCDMA_CARD_GET_ATTR

DESCRIPTION:
  This function checks card color (white/gray/black), UIM lock staus, and card 
  insertion history. 
                        
DEPENDENCIES:
   PIN1 verification is done before calling this function.

LIMITATIONS:

RETURN VALUE:  
  MMGSDI_SUCCESS: If succssesful in getting card attributes
  MMGSDI_ERROR : If a error in getting card attributes
     
SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_jcdma_card_get_attr(void)
{

  mmgsdi_jcdma_card_attr_type   *card_data_ptr;
  nv_item_type                   lock_nv_data;
  nv_item_type                   history_nv_data;
  nv_stat_enum_type              nv_read_status;
  nv_stat_enum_type              nv_write_status;

  /* Allocate memory */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(card_data_ptr, 
                                           sizeof(mmgsdi_jcdma_card_attr_type));        

  /* Initialized card_info to unknown */
  mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_UNKNOWN_CARD;

  /*-----------------------------------------------------
    Read card color and lock info from card 
  -----------------------------------------------------*/
  if(MMGSDI_SUCCESS != mmgsdi_jcdma_card_get_color_and_locks(card_data_ptr))
  {    
     MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
     MSG_ERROR("JCDMA get card color failed", 0,0,0);
     return MMGSDI_ERROR;
  }

  /*-----------------------------------------------------
    Read lock info from NV
  -----------------------------------------------------*/
  nv_read_status =  gsdi_get_nv(NV_JCDMA_UIM_LOCK_I, 
                                (nv_item_type *) &lock_nv_data);
  if(nv_read_status == NV_DONE_S)
  {
     MSG_LOW("NV_JCDMA_UIM_LOCK_I read ",0,0,0);
  }
  else if (nv_read_status == NV_NOTACTIVE_S) 
  {
    /* Write Default Value */

    lock_nv_data.jcdma_uim_lock.ruim_id   = MMGSDI_JCDMA_RUIMID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.imsi_mcc1 = MMGSDI_JCDMA_IMSI_MCC1_DEFAULT;
    lock_nv_data.jcdma_uim_lock.imsi_mcc2 = MMGSDI_JCDMA_IMSI_MCC2_DEFAULT;
    lock_nv_data.jcdma_uim_lock.sp_id = MMGSDI_JCDMA_SP_ID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.ug_id = MMGSDI_JCDMA_UG_ID_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK] = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_NW_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_SP_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_SP_LOCK]   = MMGSDI_JCDMA_INDICATOR_DEFAULT;
    
    nv_write_status =  gsdi_put_nv(NV_JCDMA_UIM_LOCK_I,
                                   (nv_item_type *) &lock_nv_data);
    if (nv_write_status != NV_DONE_S) 
    {  
      MSG_ERROR("NV NV_JCDMA_UIM_LOCK_I Write Error, status returned %d", 
                nv_write_status,0,0);
      return MMGSDI_ERROR;             
    }
  }
  else
  {  
    MSG_ERROR("NV read error",0,0,0);
    return MMGSDI_ERROR;             
  }

  /*-----------------------------------------------------
     Read insertion history info from NV
  -----------------------------------------------------*/
  if(NV_DONE_S != 
     gsdi_get_nv(NV_JCDMA_RUIM_ID_I, (nv_item_type *) &history_nv_data))
  {
     MSG_ERROR("NV read error",0,0,0);
     return MMGSDI_ERROR;             
  }
  else
  {
    MSG_LOW("NV_JCDMA_RUIM_ID_I read ",0,0,0);
  }

  /*-----------------------------------------------------
    Parse the data from card 
  -----------------------------------------------------*/
  mmgsdi_data_slot1.jcdma_card_info = card_data_ptr->card_type;
  switch (card_data_ptr->card_type) {
    case MMGSDI_JCDMA_BLACK_CARD: 
      if (card_data_ptr->lock_enabled) 
      {  
         /* Check line lock enabled */
         if (lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK])
         {  
            /* Compare UIM ID */
            if ((lock_nv_data.jcdma_uim_lock.ruim_id != card_data_ptr->ruim_id) &&
                (lock_nv_data.jcdma_uim_lock.ruim_id != MMGSDI_JCDMA_UIMID_DEFAULT)) 
            {      
               mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED; 
               break;
            }
         }
         /* Check NW lock  */
         if (lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_NW_LOCK])
         {  
            /* Compare NW */
            if ((lock_nv_data.jcdma_uim_lock.imsi_mcc1 != card_data_ptr->imsi_mcc) &&
                (lock_nv_data.jcdma_uim_lock.imsi_mcc2 != card_data_ptr->imsi_mcc))
            {  
               mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NW_LOCKED;
               break;
            }
         }
         /* Check SP lock  */
         if (lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_SP_LOCK])
         {  
            /* Compare SP */
            if (lock_nv_data.jcdma_uim_lock.sp_id != card_data_ptr->sp_id)
            {      
               mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED;   
               break;
            }
        }
        /* Check UG lock  */
        if (lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_UG_LOCK])
        {  
            /* Compare UG */
            if (lock_nv_data.jcdma_uim_lock.ug_id!= card_data_ptr->ug_id)
            {
               mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED;   
               break;
            }
        }
      }
      break;
    case MMGSDI_JCDMA_GRAY_CARD:  
      if (card_data_ptr->lock_enabled) 
      {  
         /* Only need to check line lock for gray card */
         if (lock_nv_data.jcdma_uim_lock.indicator[MMGSDI_JCDMA_LINE_LOCK])
         {  
            /* Compare UIM ID and see if NV is initialized */
            if ((lock_nv_data.jcdma_uim_lock.ruim_id != card_data_ptr->ruim_id) &&
                (lock_nv_data.jcdma_uim_lock.ruim_id != MMGSDI_JCDMA_UIMID_DEFAULT)) 
            {  
                /* Only lock ME when ruim id does not match and NV is not 
                  initialized */
               mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED;  
               break;
           }
         }
      }
      break;
    default:
      break;
  } /* switch */

  /*--------------------------------------------------------------
   Check line lock code initialization and card insertion history
  --------------------------------------------------------------*/

  if (mmgsdi_data_slot1.jcdma_card_info == MMGSDI_JCDMA_BLACK_CARD)
  {      
     /* NV line lock code initialized? */
     if (lock_nv_data.jcdma_uim_lock.ruim_id == MMGSDI_JCDMA_UIMID_DEFAULT) 
     {      
        /* This is first black card for this ME */
        mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NEW_CARD;
        lock_nv_data.jcdma_uim_lock.ruim_id = card_data_ptr->ruim_id; 
        MSG_MED("JCDMA New card ", 0, 0, 0);
        if(NV_DONE_S != 
          gsdi_put_nv(NV_JCDMA_RUIM_ID_I, (nv_item_type *) &lock_nv_data))
        {  
          MSG_ERROR("NV write error",0,0,0);
          return MMGSDI_ERROR;             
        }
        else
        {
          MSG_LOW("NV_JCDMA_RUIM_ID_I write ",0,0,0);
        }
     }

     /* Compare the UIM ID with history data */
     if (history_nv_data.jcdma_ruim_id.ruim_id != card_data_ptr->ruim_id) 
     {  
        /* it is different card */
        mmgsdi_data_slot1.jcdma_card_info = MMGSDI_JCDMA_BLACK_CARD_NEW_CARD;
        /* update the history data to NV */
        history_nv_data.jcdma_ruim_id.ruim_id = card_data_ptr->ruim_id; 
        MSG_MED("JCDMA New card ", 0, 0, 0);
        if(NV_DONE_S != 
          gsdi_put_nv(NV_JCDMA_RUIM_ID_I, (nv_item_type *) &history_nv_data))
        {  
          MSG_ERROR("NV write error",0,0,0);
          return MMGSDI_ERROR;             
        }
        else
        {
          MSG_LOW("NV_JCDMA_RUIM_ID_I write ",0,0,0);
        }
     }
  }
  /* Release memory */
  MMGSDIUTIL_TMC_MEM_FREE(card_data_ptr);
  return MMGSDI_SUCCESS;             
} /* mmgsdi_jcdma_card_get_attr() */
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

