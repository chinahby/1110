/*===========================================================================

                             R F   N V   M a n a g e r

DESCRIPTION
  This header file contains the data structure and function declarations to interface with
  the RF NV Manager.

The band class designations for current and future bands:
    BC 0 - US Cellular
    BC 1 - US PCS
    BC 2 - JTACS (I think)
    BC 3 - JCDMA
    BC 4 - Korean PCS
    BC 5 - NMT (450MHz)
    BC 6 - IMT (2.1GHz)

Copyright (c) 1997 - 2009 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfnv.c#8 $
$DateTime: 2009/02/10 17:17:01 $
$Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/09   vb      Changed rfnv_rf_config_read to return NV_RF_CONFIG_I and 
                   added NV_RF_CONFIG_I as non critical NV item
11/17/08   lp      Fixed Klocwork errors
11/07/08   vm      Fixes for BC5 Sub Block Implementation
10/06/08   vb      Corrected errors in the functions:
                   rfnv_get_nv_digital_item_ptr(); rf_set_block_mask_override
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
12/03/07   avi     Changed the return value of rf_get_rf_config() to return the 
                   value of NV_RF_HW_CONFIG (1878) instead of NV_RF_CONFIG_I (148),
                   the two iteam are identical in function and should be controled 
                   by a single NV value
11/30/07   jfc     Call rficap_init_target_dependent_features() if either 
                    FEATURE_WCDMA or FEATURE_GSM is defined
11/28/07    xw     Removed NV_RF_CONFIG_I in rfnv_cmn_mm_nv_items_list. 
                   Clear nv_data_valid_flag for each table other than 
                   rfnv_cmn_mm_nv_items_list in rfnv_supported_mode_info_table.
10/12/07   adk     Restore function-call rficap_init_target_dependent_features()
                   for QRF7600.
09/28/07   adk     Bypass function-call rficap_init_target_dependent_features()
                   for QRF7600 (for now).
07/26/07   ad      Changed  NV_RF_CONFIG_I to non-critical item and moved 
                   rfnv_rf_config_read flag to NV_RF_HW_CONFIG_I case
06/13/07   adk     Fixed a typo in FEATURE_CDMA1X
04/13/07   ad      Featurized NV_RF_BC_CONFIG_DIV_I using RF_HAS_RX_DIVERSITY
03/14/07   ad      Enabled support for diversity NV item NV_RF_BC_CONFIG_DIV_I
02/28/07   ycl     Fix compiler warnings.
02/05/07   ycl     Merge to 7200 tip.
11/29/06   jfc     Featurized loading of diversity NV items
10/17/06   ycl     Modifications for initial MSM7600 build.
01/10/06   sar     Eliminated lint errors.
12/14/05   sar     Eliminated lint errors.
11/16/05   rmd     Added support for BC11.
10/18/05   rmd     Updated code according to code review recommendations.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
07/01/05   sar     Deleted support for legacy band classes.
06/13/05   dbc     Removed support for RF_HAS_MFLO
05/02/05   dbc     Initial support for MFLO (RF_HAS_MFLO)
09/30/04   sar     Added functions rfnv_retrieve_nv_items(), rfnv_nv_read() and
                   rfnv_nv_write().
07/16/04   sar     Fixed rfnv_retrieve_nv_items() to not set the invalid flag
                   when the item mask is set to non-critial. 
05/28/04   sar     Updated feature flag names for the nv tables.
05/25/04   sar     added support for legacy cdma & pcs bands.
05/10/04    et     added GSM support by replacing NULL items in the tables
                   with various GSM table names and sizes.
03/19/04   sar     Updated to add support for new band classes.
01/30/04   zg      Fixed "#ifdef RF_HAS_GPS"
10/07/03   sar     Adapted for Jaguar. Modified structure rfnv_supported_mode_info_type
                   and updated rfnv_supported_mode_info_table for 1X. Added function
                   rfnv_get_nv_table().

===========================================================================*/
#include "task.h"
#include "rfnv.h"
#include "msg.h"
#include "rfi.h"
#include "rfcom.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_CDMA1X
#include "rfnv_cdma1x.h"

#ifdef RF_HAS_BC0_NV_TABLE
#include "rfnv_cdma1x_bc0.h"
#endif

#ifdef RF_HAS_BC1_NV_TABLE
#include "rfnv_cdma1x_bc1.h"
#endif

#ifdef RF_HAS_BC2_NV_TABLE
#include "rfnv_cdma1x_bc2.h"
#endif

#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC4_NV_TABLE
#include "rfnv_cdma1x_bc4.h"
#endif

#ifdef RF_HAS_BC5_NV_TABLE
#include "rfnv_cdma1x_bc5.h"
#endif

#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC14_NV_TABLE
#include "rfnv_cdma1x_bc14.h"
#endif

#ifdef RF_HAS_BC15_NV_TABLE
#include "rfnv_cdma1x_bc15.h"
#endif

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
#include "rfnv_cdma1x_bcx_block_0.h"
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
#include "rfnv_cdma1x_bcx_block_1.h"
#endif

#endif
/* add additional mode include file here */
extern rfnv_items_list_type rfnv_cdma1x_common_items_list[];

#ifdef RF_HAS_BC0_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc0_nv_items_list[];
#endif

#ifdef RF_HAS_BC1_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc1_nv_items_list[];
#endif

#ifdef RF_HAS_BC2_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc2_nv_items_list[];
#endif

#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC4_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc4_nv_items_list[];
#endif

#ifdef RF_HAS_BC5_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc5_nv_items_list[];
#endif

#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC14_NV_TABLE

extern rfnv_items_list_type rfnv_cdma1x_bc14_nv_items_list[];

#ifdef RF_HAS_COPY_BC1_NV_INTO_BC14
#error code not present
#endif /* RF_HAS_COPY_BC1_NV_INTO_BC14 */

#endif /* RF_HAS_BC14_NV_TABLE */

#ifdef RF_HAS_BC15_NV_TABLE
extern rfnv_items_list_type rfnv_cdma1x_bc15_nv_items_list[];
#endif /* RF_HAS_BC15_NV_TABLE */

#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif /* RF_HAS_BC11_NV_TABLE */

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif /*RF_HAS_GPS*/

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
extern rfnv_items_list_type rfnv_cdma1x_bcx_block_0_nv_items_list[];
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_0_tbl;
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
extern rfnv_items_list_type rfnv_cdma1x_bcx_block_1_nv_items_list[];
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_1_tbl;
#endif

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

#endif //FEATURE_CDMA1X

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
boolean rf_block_mask_is_forced[RF_PATH_MAX] = {FALSE,FALSE}; /*Default is FALSE*/
uint32 rf_forced_block_mask_value[RF_PATH_MAX] = {0,0}; /*Default 0x0*/
rfnv_digital_items_type* rf_forced_nv_item_ptr[RF_PATH_MAX] = {NULL,NULL}; /*Default to NULL*/
static rf_card_band_type rf_parent_band_for_block[RF_BAND_MAX_BLOCKS] = {RF_MAX_BAND,RF_MAX_BAND};
#endif

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
static void rfnv_set_block_validity(rfnv_supported_mode_type mode_index,
                                    rfnv_supported_mode_info_type *mode_info_ptr,
                                    rex_tcb_type        *task_ptr,        /* Calling task's TCB pointer */
                                    rex_sigs_type       task_wait_sig,    /* Task signal to wait for when*/                                                                                
                                    void          (*task_wait_handler)( rex_sigs_type )
                                   );

#if defined(RF_HAS_BCX_BLOCK_0_NV) || defined (RF_HAS_BCX_BLOCK_1_NV)
static void rfnv_set_block_nv_ptrs(rfnv_digital_items_type* parent_band_class_nv_item_ptr,rfnv_digital_items_type* block_nv_ptr);
#endif

#endif /*RF_HAS_SUB_BLOCK_SUPPORT*/

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
const rfnv_items_list_type rfnv_cmn_mm_nv_items_list[ ] =
{
  /* RF Band mask for supported bands on primary path - Must read before RF_HW_CONFIG */
  { NV_RF_BC_CONFIG_I,     NV_ITEM_ATTRIB_CRTCL_BMSK },

  /* RF Band mask for supported bands on diversity path */
  #ifdef RF_HAS_RX_DIVERSITY
  { NV_RF_BC_CONFIG_DIV_I,  NV_ITEM_ATTRIB_CRTCL_BMSK},
  #endif

  /* RF HW type in the current target */
  { NV_RF_HW_CONFIG_I,     NV_ITEM_ATTRIB_CRTCL_BMSK },

  /* This is non critical item and is used for band class translation
     during internal testing */
  { NV_RF_CONFIG_I,        NV_ITEM_ATTRIB_NON_CRTCL_BMSK }
};

rfnv_cmn_mm_nv_tbl_type rfnv_cmn_mm_nv_tbl = 
{
  /* RF hardware configuration */
  (uint8)RF_TARGET_NONE,

  (uint8)RF_DEF_HW,

  {(uint64)RF_DEF_BC, (uint64)RF_DEF_BC}

}; /* rfnv_cmn_mm_items_type rfnv_cmn_mm_nv_tbl */

/* Initialize pointer to the default Common NV table */
rfnv_cmn_mm_nv_tbl_type *rfnv_cmn_mm_nv_tbl_ptr = &rfnv_cmn_mm_nv_tbl;


/* table of data structure and function for a each supported mode */
/* fill in this table with information for each mode */
rfnv_supported_mode_info_type rfnv_supported_mode_info_table[RFNV_SUPPORTED_MODE_NUM] =
{
  /* Common Multimode NV Items */
  { rfnv_cmn_mm_nv_items_list,
    rfnv_cmn_mm_item_table,
    rfnv_cmn_mm_list_size,
    TRUE,
    rfnv_cmn_mm_copy_nv_item
  },

  #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
  #endif

  #ifdef FEATURE_WCDMA
#error code not present
 #endif /* FEATURE_WCDMA */

 #ifdef FEATURE_GSM
#error code not present
 #endif



  #ifdef FEATURE_CDMA1X
  { rfnv_cdma1x_common_items_list,
    rfnv_cdma1x_common_item_table,
    rfnv_cdma1x_common_list_size,
    FALSE,
    rfnv_cdma1x_copy_common_nv_items
  },
  #endif

  #ifdef RF_HAS_BC0_NV_TABLE
  { rfnv_cdma1x_bc0_nv_items_list,
    rfnv_cdma1x_bc0_item_table,
    rfnv_cdma1x_bc0_list_size,
    FALSE,
    rfnv_cdma1x_bc0_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC1_NV_TABLE
  { rfnv_cdma1x_bc1_nv_items_list,
    rfnv_cdma1x_bc1_item_table,
    rfnv_cdma1x_bc1_list_size,
    FALSE,
    rfnv_cdma1x_bc1_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC2_NV_TABLE
  { rfnv_cdma1x_bc2_nv_items_list,
    rfnv_cdma1x_bc2_item_table,
    rfnv_cdma1x_bc2_list_size,
    FALSE,
    rfnv_cdma1x_bc2_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC4_NV_TABLE
  { rfnv_cdma1x_bc4_nv_items_list,
    rfnv_cdma1x_bc4_item_table,
    rfnv_cdma1x_bc4_list_size,
    FALSE,
    rfnv_cdma1x_bc4_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC5_NV_TABLE
  { rfnv_cdma1x_bc5_nv_items_list,
    rfnv_cdma1x_bc5_item_table,
    rfnv_cdma1x_bc5_list_size,
    FALSE,
    rfnv_cdma1x_bc5_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC14_NV_TABLE
  { rfnv_cdma1x_bc14_nv_items_list,
    rfnv_cdma1x_bc14_item_table,
    rfnv_cdma1x_bc14_list_size,
    FALSE,
    rfnv_cdma1x_bc14_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BC15_NV_TABLE
  { rfnv_cdma1x_bc15_nv_items_list,
    rfnv_cdma1x_bc15_item_table,
    rfnv_cdma1x_bc15_list_size,
    FALSE,
    rfnv_cdma1x_bc15_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT   

  #ifdef RF_HAS_BCX_BLOCK_0_NV
  { rfnv_cdma1x_bcx_block_0_nv_items_list,
    rfnv_cdma1x_bcx_block_0_item_table,
    rfnv_cdma1x_bcx_block_0_list_size,
    FALSE,
    rfnv_cdma1x_bcx_block_0_copy_nv_items
  },
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
   { rfnv_cdma1x_bcx_block_1_nv_items_list,
    rfnv_cdma1x_bcx_block_1_item_table,
    rfnv_cdma1x_bcx_block_1_list_size,
    FALSE,
    rfnv_cdma1x_bcx_block_1_copy_nv_items
  },
  #endif

  #endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
  /* Additional mode information here */
};


/* True, when NV_RF_HW_CONFIG_I is read. */
boolean rfnv_rf_hw_config_read = FALSE;



/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/




/*===========================================================================

FUNCTION RFNV_SET_NV_ITEM

DESCRIPTION
  This function set a specific NV item to NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
None

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  written.

===========================================================================*/
void rfnv_set_nv_item(
                     nv_items_enum_type  item_code,       /* Item to set */
                     nv_item_type        *item_ptr,       /* Pointer to the item */
                     rex_tcb_type        *task_ptr,       /* Calling task's TCB pointer */
                     rex_sigs_type       task_wait_sig,   /* Task signal to wait for when
                                                             reading NV */
                     void                (*task_wait_handler)( rex_sigs_type )
                     /* Task's wait function to be called
                        when reading NV */
                     )
{
  /* Command buffer to NV */
  static nv_cmd_type  nv_cmd_buf;   

  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd = NV_WRITE_F;               /* Write request        */
  nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
  nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  nv_cmd_buf.item = item_code;               /* Item to set          */
  nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );                       

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );                

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

} /* rfnv_set_nv_item */


/*===========================================================================

FUNCTION rfnv_get_nv_item

DESCRIPTION
  This function performs a Single NV Read operation to retrieve the requested item.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/
LOCAL nv_stat_enum_type rfnv_get_nv_item(
  nv_items_enum_type  item_code,        /* Item to get */
  nv_item_type        *item_ptr,        /* Pointer where to put the item */
  rex_tcb_type        *task_ptr,        /* Calling task's TCB pointer */
  rex_sigs_type       task_wait_sig,    /* Task signal to wait for when
                                           reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
)
{
   /* Command buffer to NV */
   static nv_cmd_type  nv_cmd_buf;

   /* Prepare command buffer to NV. */
   nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
   nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
   nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
   nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
   nv_cmd_buf.item = item_code;               /* Item to get          */
   nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

   /* Clear signal, issue the command, and wait for the respone. */
   /* Clear signal for NV  */
   (void) rex_clr_sigs(task_ptr, task_wait_sig);

   /* Issue the request    */
   nv_cmd(&nv_cmd_buf);

   /* Wait for completion  */
   task_wait_handler(task_wait_sig);

   /* clear the signal again */
   (void) rex_clr_sigs( task_ptr, task_wait_sig );

   /* return status */
   return (nv_cmd_buf.status);
}

/*===========================================================================

FUNCTION rfnv_retrieve_nv_items

DESCRIPTION
  This function retrieves all the NV items for each mode specified in the
  rfnv_supported_mode_info_table.

PARAMETER
  Caller task info. see below.

DEPENDENCIES
  NV task is started.

RETURN VALUE
  status of the NV read process.

SIDE EFFECTS
  It will block the caller task.

===========================================================================*/
rfnv_status_type rfnv_retrieve_nv_items
(
  rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
  rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                           reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
)
{
   /* NV item buffer */
   nv_item_type               nv_item_buf;

   /* NV read status */
   nv_stat_enum_type          nv_read_status;

   /* loop counter for RF NV item fetches */
   word                       nv_item_cnt;

   /* Pointer to the list of RF NV items */
   const rfnv_items_list_type        *nv_item_list;

   /* Number of items in RF NV items list */
   word                       nv_item_list_siz;

   /* Pointer to the supported mode info type */
   rfnv_supported_mode_info_type *mode_info_ptr;

   /* index to each mode */
   int16   mode_idx;

   /* status from reading NV */
   rfnv_status_type status = RFNV_READ_SUCCESS;

   /*--------------------------------------------------------------*/
   /* read NV items for each supported mode */

   #if defined(RF_HAS_BC14_NV_TABLE)
   #if defined(RF_HAS_COPY_BC1_NV_INTO_BC14)
#error code not present
   #endif  /* RF_HAS_COPY_BC1_NV_INTO_BC14 */
   #endif  /* RF_HAS_BC14_NV_TABLE */

   #if defined(RF_HAS_BC11_NV_TABLE)
#error code not present
   #endif  /* RF_HAS_BC11_NV_TABLE */
   
   for (mode_idx = 0; mode_idx < (int16) RFNV_SUPPORTED_MODE_NUM; mode_idx++)
   {
     mode_info_ptr = &rfnv_supported_mode_info_table[mode_idx];

     #ifdef RF_HAS_SUB_BLOCK_SUPPORT
     //Get the Validity of Blocks corresponding to Band Class
     rfnv_set_block_validity(mode_idx,mode_info_ptr,
                             task_ptr,task_wait_sig,
                             task_wait_handler);
     #endif
     
     if (mode_info_ptr->nv_data_valid_flag == TRUE)
     {
      /* get the pointer to NV items list for each mode */
      nv_item_list =  mode_info_ptr->nv_item_list_ptr;

      /* get the list size */
      if( mode_info_ptr->get_nv_item_list_size != NULL)
        nv_item_list_siz = (mode_info_ptr->get_nv_item_list_size)();
      else
        nv_item_list_siz = 0;

      /* for each NV item in the list, read from NV and copy into local buffer. */
      /* then copy from local buffer into RF NV table for each mode */
      for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)
      {
         /* read from NV into local buffer */
         nv_read_status = rfnv_get_nv_item( nv_item_list[nv_item_cnt].nv_item_id,
                                            &nv_item_buf,
                                            task_ptr,
                                            task_wait_sig,
                                            task_wait_handler );

         /* if read status is good */
         if (nv_read_status == NV_DONE_S)
         {
            /* copy from local buffer into RF NV table */
            mode_info_ptr->copy_nv_item( nv_item_list[nv_item_cnt].nv_item_id, &nv_item_buf );
         }
         else if (nv_read_status == NV_NOTACTIVE_S)
         {
            MSG_ERROR( "RF NV item %d for mode %d is not available", nv_item_list[nv_item_cnt].nv_item_id, mode_idx, 0 );

            if (nv_item_list[nv_item_cnt].nv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               mode_info_ptr->nv_data_valid_flag = FALSE;
               status = RFNV_INACTIVE_ITEM;
            }
         }
         else
         {
            MSG_ERROR( "NV read failed on RF NV item %d for mode %d", nv_item_list[nv_item_cnt].nv_item_id, mode_idx, 0 );
            
            if (nv_item_list[nv_item_cnt].nv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               mode_info_ptr->nv_data_valid_flag = FALSE;
               status = RFNV_READ_FAILED;
            }
         }

      } /*for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)*/
    } /* rfnv_is_nv_data_valid */
   } /*for (mode_idx = 0; mode_idx < RFNV_SUPPORTED_MODE_NUM; mode_idx++)*/

   return status;
}


#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RFNV_SET_BLOCK_VALIDITY                      Internal FUNCTION

DESCRIPTION
  This function Checks the Validity of the BCX_BLOCK and BCX_BLOCK_1 Set of NV.
  This is Called during rfnv_retrieve_nv_items() to determine if the correponding 
  Block NV loaded is Valid
  If the NV determing as to which set of Blocks the Block NV supports or to which
  Band Class the Block NV corresonds to is not populated then the nv_data_flag
  corrresponding to Block NV will be set to FALSE.
  Otherwise if Block NV loaded is valid then nv_data_flag for block NV is set to TRUE.
  If Block NV is valid then Whole of Parent Band Class NV Data Structure is memory copied 
  into the data structure for block NV as defaults for bblock NV


DEPENDENCIES
  Corresponding Band Class NV to which Block NV corresponds to must be populated

RETURN VALUE
  Sets the nv_data_valid_flag to TRUE or FALSE for Block NV

SIDE EFFECTS
  Modifies the Block Mask for Parent Band Class NV based on fact
  as to which set of Blocks the Block NV Supports


===========================================================================*/
static void rfnv_set_block_validity(rfnv_supported_mode_type mode_index,
                                rfnv_supported_mode_info_type *mode_info_ptr,
                                rex_tcb_type        *task_ptr,        /* Calling task's TCB pointer */
                                rex_sigs_type       task_wait_sig,    /* Task signal to wait for when*/                                                                                
                                void          (*task_wait_handler)( rex_sigs_type )
                               )
{ 
    #if defined(RF_HAS_BCX_BLOCK_0_NV) || defined (RF_HAS_BCX_BLOCK_1_NV)
    /* NV item buffer */
    nv_item_type               nv_item_buf_1;

        /* NV item buffer */
   /* nv_item_type               nv_item_buf_2; */

    /* NV read status */
    nv_stat_enum_type          nv_read_status_1 = NV_NOTACTIVE_S;

        /* NV read status */
   /* nv_stat_enum_type          nv_read_status_2 = NV_NOTACTIVE_S; */

    rfnv_digital_items_type* nv_digital_item_ptr = NULL;

    boolean band_class_valid = FALSE;
    #endif

    switch(mode_index)
    {
    #ifdef RF_HAS_BCX_BLOCK_0_NV    
    case RFNV_CDMA1X_BCX_BLOCK_0:
      nv_read_status_1 = rfnv_get_nv_item(NV_BCX_BLOCK_BAND_CLASS_I,
                                          &nv_item_buf_1,
                                          task_ptr,
                                          task_wait_sig,
                                          task_wait_handler );

      /*NV_BCX_BLOCK_1_SUBCLASS_MASK_I will be populated through the static QCN by CAL*/
      /*nv_read_status_2 = rfnv_get_nv_item(NV_BCX_BLOCK_SUBCLASS_MASK_I,
                                          &nv_item_buf_2,
                                          task_ptr,
                                          task_wait_sig,
                                          task_wait_handler );*/
     
      if((nv_read_status_1 == NV_DONE_S)) //&& (nv_read_status_2 == NV_DONE_S))
      {
        band_class_valid = rfnv_band_class_nv_is_valid(nv_item_buf_1.bcx_block_band_class,&nv_digital_item_ptr);

        if((band_class_valid == TRUE)&& (nv_digital_item_ptr != NULL))
        {
          memcpy((byte*)&rfnv_cdma1x_bcx_block_0_tbl,nv_digital_item_ptr,sizeof(rfnv_digital_items_type));
 
          mode_info_ptr->nv_data_valid_flag = TRUE;

          /* 'rfnv_block_mask' will be copied from the NV item NV_BCX_BLOCK_1_SUBCLASS_MASK_I*/
          //nv_digital_item_ptr->rfnv_block_mask = nv_digital_item_ptr->rfnv_block_mask & (~nv_item_buf_2.bcx_block_subclass_mask);

          rf_parent_band_for_block[RF_BAND_BLOCK_0] = (rf_card_band_type)nv_item_buf_1.bcx_block_band_class;

          rfnv_cdma1x_bcx_block_0_tbl.band_id = nv_digital_item_ptr ->band_id;

          rfnv_cdma1x_bcx_block_0_tbl.rfnv_block_mask = 0x0; //nv_item_buf_2.bcx_block_subclass_mask;

          rfnv_cdma1x_bcx_block_0_tbl->block_item_ptr = NULL;

          rfnv_set_block_nv_ptrs(nv_digital_item_ptr,&rfnv_cdma1x_bcx_block_0_tbl);
        }
        else
        {          
          mode_info_ptr->nv_data_valid_flag = FALSE;

          rfnv_cdma1x_bcx_block_0_tbl.rfnv_block_mask = 0x0;

          rfnv_cdma1x_bcx_block_0_tbl->block_item_ptr = NULL;

          MSG_ERROR("Band Class %d in Block NV Table is not valid or defined",nv_item_buf_1.bcx_block_band_class,0,0);
        }
      }
      else
      {
        MSG_ERROR("Band Class or Subclass Mask is not specified for BCX Block NV",0,0,0);

        mode_info_ptr->nv_data_valid_flag = FALSE;
        
        rfnv_cdma1x_bcx_block_0_tbl.rfnv_block_mask = 0x0;

        rfnv_cdma1x_bcx_block_0_tbl->block_item_ptr = NULL;
      }
      break;
    #endif

    #ifdef RF_HAS_BCX_BLOCK_1_NV
    case RFNV_CDMA1X_BCX_BLOCK_1:
      nv_read_status_1 = rfnv_get_nv_item(NV_BCX_BLOCK_1_BAND_CLASS_I,
                                          &nv_item_buf_1,
                                          task_ptr,
                                          task_wait_sig,
                                          task_wait_handler );

      /*NV_BCX_BLOCK_1_SUBCLASS_MASK_I will be populated through the static QCN by CAL*/
      /*nv_read_status_2 = rfnv_get_nv_item(NV_BCX_BLOCK_1_SUBCLASS_MASK_I,
                                          &nv_item_buf_2,
                                          task_ptr,
                                          task_wait_sig,
                                          task_wait_handler );*/

      if((nv_read_status_1 == NV_DONE_S))//&& (nv_read_status_2 == NV_DONE_S))
      {        
        band_class_valid = rfnv_band_class_nv_is_valid(nv_item_buf_1.bcx_block_1_band_class,&nv_digital_item_ptr);

        if((band_class_valid == TRUE) && (nv_digital_item_ptr != NULL))
        {
          memcpy((byte*)&rfnv_cdma1x_bcx_block_1_tbl,nv_digital_item_ptr,sizeof(rfnv_digital_items_type));

          mode_info_ptr->nv_data_valid_flag = TRUE;

          /* 'rfnv_block_mask' will be copied from the NV item NV_BCX_BLOCK_1_SUBCLASS_MASK_I*/
          //nv_digital_item_ptr->rfnv_block_mask = nv_digital_item_ptr->rfnv_block_mask & (~nv_item_buf_2.bcx_block_1_subclass_mask);

          rf_parent_band_for_block[RF_BAND_BLOCK_1] = (rf_card_band_type)nv_item_buf_1.bcx_block_1_band_class;

          rfnv_cdma1x_bcx_block_1_tbl.band_id = nv_digital_item_ptr ->band_id;

          rfnv_cdma1x_bcx_block_1_tbl.rfnv_block_mask = 0x0; //nv_item_buf_2.bcx_block_1_subclass_mask; 

          rfnv_cdma1x_bcx_block_1_tbl->block_item_ptr = NULL;

          rfnv_set_block_nv_ptrs(nv_digital_item_ptr,&rfnv_cdma1x_bcx_block_1_tbl);
        }
        else
        {
          MSG_ERROR("Band Class %d in Block 1 NV Table is not valid or defined",nv_item_buf_1.bcx_block_band_class,0,0);

          mode_info_ptr->nv_data_valid_flag = FALSE;

          rfnv_cdma1x_bcx_block_1_tbl.rfnv_block_mask = 0x0;

          rfnv_cdma1x_bcx_block_1_tbl->block_item_ptr = NULL;
        }
      }
      else
      {
        MSG_ERROR("Band Class or Subclass Mask is not specified for BCX Block 1 NV",0,0,0);

        mode_info_ptr->nv_data_valid_flag = FALSE;

        rfnv_cdma1x_bcx_block_1_tbl.rfnv_block_mask = 0x0;

        rfnv_cdma1x_bcx_block_1_tbl->block_item_ptr = NULL;
      }
      break;
    #endif
    }/*switch(mode_idex)*/
}

/*===========================================================================

FUNCTION RFNV_GET_PARENT_BAND_FOR_BLOCK                   Internal FUNCTION

DESCRIPTION
  This function returns the Parent band Class associated with the Block

DEPENDENCIES
 The Static QCN determining tehArent band Class for Sub Block must be loaded
 to get correct value

RETURN VALUE
  Band Type

SIDE EFFECTS

===========================================================================*/
rf_card_band_type rfnv_get_parent_band_for_block(rf_card_blocks_type block)
{
  if(block < RF_BAND_MAX_BLOCKS)
  {  
    return rf_parent_band_for_block[block];
  }
  else
  {
    return RF_MAX_BAND;
  }
}
/*===========================================================================

FUNCTION RFNV_BAND_CLASS_NV_IS_VALID                    Internal FUNCTION

DESCRIPTION
  This function retuen TRUE or FALSE depending on the fact that if 
  the Passed Band Class is Supported at compile time and corresponding
  NV for Band Class is populated by checking the nv_data_vaild_flag
  for the Band Class
  Also sets the Pointer to the NV Table of the Band Class
  thats been passed through as a parameter. If the Band Class
  Passed is not supported the pointer returned is NULL and the value returned
  is FALSE

DEPENDENCIES
  The Band Class NV must be loaded through rfnv_retrieve_nv_items
  for this function to return TRUE

RETURN VALUE
  Returns the NV validity of the Band class Passed as parameter

SIDE EFFECTS
  Should be called after the given Band Class NV is already loaded
  into internal RF data structures from NV Memory

===========================================================================*/
boolean rfnv_band_class_nv_is_valid(uint8 band_class_supported,rfnv_digital_items_type** nv_digital_item_ptr)
{  
  *nv_digital_item_ptr = rfnv_get_band_nv_ptr(band_class_supported);
  
  switch(band_class_supported)
  {
  #ifdef RF_HAS_BC0_NV_TABLE
  case RF_BC0_BAND:    
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC0].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }    
    break;
  #endif

  #ifdef RF_HAS_BC1_NV_TABLE
  case RF_BC1_BAND:    
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC1].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }
    break;
  #endif

  #ifdef RF_HAS_BC2_NV_TABLE
  case RF_BC2_BAND:    
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC2].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }
    break;
  #endif

  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC4_NV_TABLE
  case RF_BC4_BAND:    
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC4].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }
    break;
  #endif

   #ifdef RF_HAS_BC5_NV_TABLE
  case RF_BC5_BAND:    
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC5].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }
    break;
  #endif

  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC10_NV_TABLE
  case RF_BC10_BAND:
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC10].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }    
    break;
  #endif

  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC14_NV_TABLE
  case RF_BC14_BAND:
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC14].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }        
    break;
  #endif

  #ifdef RF_HAS_BC15_NV_TABLE
  case RF_BC15_BAND:
    if(rfnv_supported_mode_info_table[RFNV_CDMA1X_BC15].nv_data_valid_flag == TRUE)
    {
      return TRUE;
    }        
    break;
  #endif
  }/*switch()*/

  *nv_digital_item_ptr = NULL;
  return FALSE;
}

/*===========================================================================

FUNCTION RF_SET_BLOCK_MASK_OVERRIDE                    INTERNAL FUNCTION

DESCRIPTION
Sets the Block Mask forced through FTM Mode or a given Band Class
RF Driver is tuned to.
Returns TRUE or FALSE depending on the fact that Block mask requested
is valid or not.

DEPENDENCIES
  
RETURN VALUE
Returns TRUE or FALSE depending on the fact that Block mask requested
is valid or not

SIDE EFFECTS
Global Variables rf_block_mask_is_forced and rf_forced_block_mask_value
are changed depending on what Block mask is forced.
  

===========================================================================*/
boolean rf_set_block_mask_override(rfcom_device_enum_type device,int8 index, uint32 block_mask)
{    
  rfnv_digital_items_type* rf_digital_nv_ptr;

  rf_path_enum_type path = rfcom_to_path[ device ];

  if(index == -1)
  {
    rf_digital_nv_ptr = rfnv_get_band_nv_ptr(rf_chain_status[path].rf_curr_cdma_band);    
  }
  else if(index == 0)
  {
    #ifdef RF_HAS_BCX_BLOCK_0_NV
    rf_digital_nv_ptr = &rfnv_cdma1x_bcx_block_0_tbl;
    #else
    MSG_ERROR("BCx_Block_0 Not supported",0,0,0);
    return FALSE;
    #endif

  }
  else if(index == 1)
  {
    #ifdef RF_HAS_BCX_BLOCK_1_NV
    rf_digital_nv_ptr = &rfnv_cdma1x_bcx_block_1_tbl;
    #else
    MSG_ERROR("BCx_Block_1 Not supported",0,0,0);
    return FALSE;
    #endif
  }
  else
  {
    MSG_ERROR("Incorrect Index Passed for Block Mask Override",0,0,0);

    return FALSE;
  }

  if(path < RF_PATH_MAX)
  {
    if(rfc_block_mask_is_valid(rf_chain_status[path].rf_curr_cdma_band,block_mask))
    {        
      if(((rf_chain_status[RF_PATH_1].rf_state == RF_STATE_DIVERSITY) && (path == RF_PATH_0))||(device == RFCOM_RECEIVER_DIV))
      {
        rf_block_mask_is_forced[RF_PATH_0] = rf_block_mask_is_forced[RF_PATH_1] = TRUE;
  
        rf_forced_block_mask_value[RF_PATH_0] = rf_forced_block_mask_value[RF_PATH_1]= block_mask;  
  
        rf_forced_nv_item_ptr[RF_PATH_0] = rf_forced_nv_item_ptr[RF_PATH_1] = rf_digital_nv_ptr;
              
      }
      else
      {
        rf_block_mask_is_forced[path] = TRUE;
  
        rf_forced_block_mask_value[path] = block_mask;  
  
        rf_forced_nv_item_ptr[path] = rf_digital_nv_ptr;            
      }
      rf_digital_nv_ptr->rfnv_block_mask = block_mask;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR("path >= RF_PATH_MAX", 0, 0, 0);
    rf_error_flag = TRUE;
    return FALSE;
  }
}

/*===========================================================================

FUNCTION RF_CLEAR_BLOCK_MASK_OVERRIDE                    INTERNAL FUNCTION

DESCRIPTION
Clears the forced Block mask set though rf_set_block_mask_override()
function.
This function is called whenever a new Band Class is tuned through
FTM Mode.

DEPENDENCIES
  
RETURN VALUE
None

SIDE EFFECTS
Global Variables rf_block_mask_is_forced is set to FALSE
  

===========================================================================*/
void rf_clear_block_mask_override(rfcom_device_enum_type device)
{
  rf_path_enum_type path = rfcom_to_path[ device ];

  if(path < RF_PATH_MAX)
  {
    if(((rf_chain_status[RF_PATH_1].rf_state == RF_STATE_DIVERSITY) && (path == RF_PATH_0))||(device == RFCOM_RECEIVER_DIV))
    {
      rf_block_mask_is_forced[RF_PATH_0] = rf_block_mask_is_forced[RF_PATH_1] = FALSE;      
    }
    else
    {
      rf_block_mask_is_forced[path] = FALSE;
    }
  }
  else
  {
    MSG_ERROR("Invalid RF Path", 0, 0, 0);
  }
}

#if defined(RF_HAS_BCX_BLOCK_0_NV) || defined (RF_HAS_BCX_BLOCK_1_NV)
/*===========================================================================

FUNCTION RFNV_SET_BLOCK_NV_PTRS                    INTERNAL FUNCTION

DESCRIPTION
This function creates the Linked list for the Parent Band Class NV and the Block
NV related to it. All the BLocks supported for a given Band Class are appended 
in Linked List Structure.
This takes two parameters corresponding to Parent Band Class NV Table pointer
and the Pointer to corresponding to Block NV table and append them in 
Linked list structure.

DEPENDENCIES
  The Correct Pointers to NV Table of Parent Band Class and Corresponding Block NV 
  must be valid

RETURN VALUE
None

SIDE EFFECTS
Modifies the Linked lIst structure and append another Block NV Node to it.
  

===========================================================================*/
static void rfnv_set_block_nv_ptrs(rfnv_digital_items_type* parent_band_class_nv_item_ptr,rfnv_digital_items_type* block_nv_ptr)
{
  rfnv_digital_items_type * nv_item_ptr;

  nv_item_ptr = parent_band_class_nv_item_ptr;

  while(nv_item_ptr->block_item_ptr != NULL)
  {
    nv_item_ptr = nv_item_ptr->block_item_ptr; 
  }
  nv_item_ptr->block_item_ptr = block_nv_ptr;
}
#endif
/*===========================================================================

FUNCTION RFNV_GET_NV_DIGITAL_ITEM_PTR                    INTERNAL FUNCTION

DESCRIPTION
Call  rfc_get_block() to get the Block Mask for given Band/Channel combination.
If the Block Mask is forced through FTM Mode, then it uses that Block Mask. 
Returns the pointer to the correponding either the Parent Band Class NV Table
or the NV Table of corresponding Block NV depending on the Band/Channel 
Parameters Passed

DEPENDENCIES
  Parent Band Class NV and Block NV must be populated before and
  they must have been appended in Link List structure

RETURN VALUE
Returns the pointer to the correponding either the Parent Band Class NV Table
or the NV Table of corresponding Block NV depending on the Band/Channel 
Parameters Passed


SIDE EFFECTS
Based on NV item Table Returned, whole of the NV items either corresponding
to Block or Parent Band class will be used.
  

===========================================================================*/
rfnv_digital_items_type* rfnv_get_nv_digital_item_ptr(rfcom_device_enum_type device, rf_card_band_type band, word rf_chan)
{

  uint32 block_mask;

  rfnv_digital_items_type* rf_digital_nv_ptr = NULL;
  
  rfnv_digital_items_type* digital_item_ptr = NULL;

  rf_path_enum_type path = rfcom_to_path[ device ];

  if(path < RF_PATH_MAX)
  {
    if(rf_block_mask_is_forced[path] == TRUE)
    {
      if(rf_forced_nv_item_ptr[path] == NULL)
      {
        MSG_ERROR("DEBUG_SUB_BLOCK Null Pointer Reference",0,0,0);
        
        return rfnv_get_band_nv_ptr(band);
        
      }
      else
      {    
        return rf_forced_nv_item_ptr[path];
      }
    }
    else
    {  
      block_mask = rfc_get_block(band,rf_chan);
    }
  }
  else
  {
    MSG_ERROR("Invalid RF Path", 0, 0, 0);
    rf_error_flag = TRUE;
    return FALSE;
  }
  
  rf_digital_nv_ptr = rfnv_get_band_nv_ptr(band);

  digital_item_ptr = rf_digital_nv_ptr;

  while( rf_digital_nv_ptr->block_item_ptr!= NULL)
  {
     if((rf_digital_nv_ptr->block_item_ptr->rfnv_block_mask & block_mask) != 0)
     {
       digital_item_ptr = rf_digital_nv_ptr->block_item_ptr;
       break;
     }
     else
     {
       rf_digital_nv_ptr = rf_digital_nv_ptr->block_item_ptr;
     }
  }

  if(digital_item_ptr == NULL)
   {
    // return NULL;
     MSG_ERROR( "DEBUG_SUB_BLOCK rfnv_get_band_nv_ptr() returned null for Band: %d and Chan: %d ",band, rf_chan, 0 );

     digital_item_ptr = rfnv_get_band_nv_ptr(band);

     return digital_item_ptr;

   }

  return digital_item_ptr;
}

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
 
/*===========================================================================

FUNCTION rfnv_is_nv_data_valid

DESCRIPTION
  This function check whether NV data for a particular mode is valid or not.

PARAMETER
  rfnv_supported_mode_type to indicate which mode to check.

DEPENDENCIES
  rfnv_retrieve_nv_items is called first.

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean rfnv_is_nv_data_valid(rfnv_supported_mode_type mode)
{
   return (rfnv_supported_mode_info_table[mode].nv_data_valid_flag);
}

/*===========================================================================

FUNCTION RFNV_GET_CAL_STAT

DESCRIPTION
  Returns the Calibration Status.

DEPENDENCIES
  The function rfnv_load_nv_items must have been called before this function.

RETURN VALUE
  TRUE if nv is read successfully, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfnv_get_cal_stat(void)
{
  return rfnv_rf_hw_config_read;
}

/*===========================================================================

FUNCTION rfnv_set_nv_data_valid_flag                              

DESCRIPTION
  This function sets NV data valid flag for a particular mode.

PARAMETER
  rfnv_supported_mode_type to set the mode.
  boolean flag to configure nv data valid or not. 
  
DEPENDENCIES
  
RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void rfnv_set_nv_data_valid_flag(rfnv_supported_mode_type mode, boolean flag)
{
  rfnv_supported_mode_info_table[mode].nv_data_valid_flag = flag; 

}


/*===========================================================================

FUNCTION RFNV_GET_NV_TABLE

DESCRIPTION
  This function returns a void pointer to the requested band if the requested
  band is supported. The clients must check returned value to avoid NULL pointer
  access errors.

PARAMETER
  requested mode.

DEPENDENCIES
  rfnv_retrieve_nv_items is called first.

RETURN VALUE
  A void pointer to the requested band if supported otherwise a NULL.

SIDE EFFECTS
  none

===========================================================================*/
void  *rfnv_get_nv_table(rfnv_supported_mode_type mode)
{
    if(rfnv_supported_mode_info_table[mode].get_nv_item_table == NULL)
        return NULL;
    else
        return rfnv_supported_mode_info_table[mode].get_nv_item_table();
}


#ifdef FEATURE_CDMA1X
/*===========================================================================

FUNCTION rfnv_read_nv_table

DESCRIPTION
  This function retrieves all the NV items for each specified list.

PARAMETER
  Caller task info. see below.

DEPENDENCIES
  NV task is started.

RETURN VALUE
  status of the NV read process.

SIDE EFFECTS
  It will block the caller task.

===========================================================================*/
void rfnv_read_nv_table(
    word nv_item_list_siz,
    rfnv_items_list_type *nv_item_list_ptr,
    void (*copy_nv_item)
        (nv_items_enum_type nv_item_id, nv_item_type *nv_item_buf_ptr)
)
{
    /* NV item buffer */
    nv_item_type               nv_item_buf;

    /* loop counter for RF NV item fetches */
    word                       nv_item_cnt;

    /* status from reading NV */
    nv_stat_enum_type nv_read_status = NV_DONE_S;

    /* for each NV item in the list, read from NV and copy into local buffer. */
    /* then copy from local buffer into RF NV table for each mode */
    for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)
    {
       
       /* read from NV into local buffer */
       nv_read_status = rfnv_nv_read( 
           nv_item_list_ptr[nv_item_cnt].nv_item_id, &nv_item_buf);

       /* if read status is good */
       if (nv_read_status == NV_DONE_S)
       {
          /* copy from local buffer into RF NV table */
          copy_nv_item( nv_item_list_ptr[nv_item_cnt].nv_item_id, &nv_item_buf );
       }
       else if (nv_read_status == NV_NOTACTIVE_S)
       {
          MSG_ERROR( "RF NV item %d is not available", nv_item_list_ptr[nv_item_cnt].nv_item_id, 0, 0 );

       }
       else
       {
          MSG_ERROR( "NV read failed on RF NV item %d", nv_item_list_ptr[nv_item_cnt].nv_item_id, 0, 0 );
          
       }

    } /*for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)*/
}

/*===========================================================================

FUNCTION rfnv_nv_read

DESCRIPTION
  read an item from nv.
  
  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the read operation.
  During this time, it kicks the watch dog periodically.

DEPENDENCIES
  None

RETURN VALUE
  nv_stat_enum_type - nv read status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
nv_stat_enum_type rfnv_nv_read 
(
   nv_items_enum_type nv_item,
   nv_item_type *data_ptr
)
{
   nv_cmd_type rf_nv_cmd;
   int dog_autokick;

   /* Prepare the NV read command buffer. */
   rf_nv_cmd.item       = nv_item;            /* item to read */
   rf_nv_cmd.cmd        = NV_READ_F;          /* NV operation */
   rf_nv_cmd.data_ptr   = data_ptr;           /* buffer to store read data  */
   rf_nv_cmd.tcb_ptr    = rex_self();         /* Notify this task when done */
   rf_nv_cmd.sigs       = FS_OP_COMPLETE_SIG; /* Signal with this sig when done */
   rf_nv_cmd.done_q_ptr = NULL;               /* Return cmd to NO Q when done */

   /* 
   * Read from NV by:
   *
   * 1. Clearing the NV signal.
   * 2. Queuing the read command to the NV task.
   * 3. Waiting for the NV task to finish servicing the read command. Ensure that
   *    watchdog is being hit during this synchronous operation. 
   */
   (void) rex_clr_sigs( rex_self(), rf_nv_cmd.sigs );
   nv_cmd( &rf_nv_cmd ); 

   dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
   
   (void) rex_wait(FS_OP_COMPLETE_SIG);

   if (!dog_autokick) 
   {
     (void) rex_autodog_disable();
   }

   return rf_nv_cmd.status;

} /* rfnv_nv_read */
/*===========================================================================

FUNCTION rf_rfnv_write

DESCRIPTION
  Write a item to nv.
  
  Note that the NV write is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.
  During this time, it kicks the watch dog periodically.

DEPENDENCIES
  None

RETURN VALUE
  nv_stat_enum_type - nv write status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
nv_stat_enum_type rfnv_nv_write 
(
   nv_items_enum_type nv_item,
   nv_item_type *data_ptr
)
{
   nv_cmd_type rf_nv_cmd;
   int dog_autokick;

   /* Prepare the NV write command buffer. */
   rf_nv_cmd.item       = nv_item;            /* item to write */
   rf_nv_cmd.cmd        = NV_WRITE_F;         /* NV operation */
   rf_nv_cmd.data_ptr   = data_ptr;           /* buffer to store write data  */
   rf_nv_cmd.tcb_ptr    = rex_self();         /* Notify this task when done */
   rf_nv_cmd.sigs       = FS_OP_COMPLETE_SIG; /* Signal with this sig when done */
   rf_nv_cmd.done_q_ptr = NULL;               /* Return cmd to NO Q when done */

   /* 
   * Write to NV by:
   *
   * 1. Clearing the NV signal.
   * 2. Queuing the write command to the NV task.
   * 3. Waiting for the NV task to finish servicing the write command. Ensure that
   *    watchdog is being hit during this synchronous operation. 
   */
   (void) rex_clr_sigs( rex_self(), rf_nv_cmd.sigs );
   nv_cmd( &rf_nv_cmd ); 

   dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
   (void) rex_wait(FS_OP_COMPLETE_SIG);

   if (!dog_autokick) 
   {
    (void) rex_autodog_disable();
   }

   return rf_nv_cmd.status;

} /* rf_nv_write */
#endif //FEATURE_CDMA1X


/*===========================================================================

FUNCTION rfnv_cmn_mm_copy_nv_item                              

DESCRIPTION
  This function copies one RF NV data item, as specified by "item_code/identifier" 
  into an element of the RF NV item table. 
  This function must be called during initialization for each and every RF NV item.

DEPENDENCIES
  Note that there must NOT be a dependency on the read order of the NV
  calibration items.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_cmn_mm_copy_nv_item
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
)
{
  #ifdef FEATURE_CDMA1X
  static boolean init_card_def = FALSE;

  if(init_card_def == FALSE)
  {
    init_card_def = TRUE;
    rfnv_set_card_defaults((rf_hw_type) RF_DEF_HW);
  }
  #endif /* FEATURE_CDMA1X */

  /* read RF NV item based on its identifier/item code */
  switch ( item_code )
  {
  case NV_RF_CONFIG_I:
    rfnv_cmn_mm_nv_tbl.rf_config = item_ptr->rf_config;
    
    /* For targets that do not use RF_CONFIG NV item and for
    ** some reason have bogus values in RF_CONFIG NV entry,
    ** read RF_CONFIG here.  It will be overwritten later
    ** with value from RF ID bits.  The reason why it is
    ** read at all is that some WLL targets read RF ID bits
    ** and some do not, and if RF_CONFIG NV item is to be
    ** read, the logical place would be where all other NV
    ** items are read.
    */      
    break;

  case NV_WCDMA_VBATT_I:
    break;

  case NV_RF_BC_CONFIG_I:
    rfnv_cmn_mm_nv_tbl.rf_bc_config[RF_PATH_0] = item_ptr->rf_bc_config;    
    break;
    
  case NV_RF_BC_CONFIG_DIV_I:    
    rfnv_cmn_mm_nv_tbl.rf_bc_config[RF_PATH_1] = item_ptr->rf_bc_config_div;
    break;
    
  case NV_RF_HW_CONFIG_I:
    rfnv_cmn_mm_nv_tbl.rf_hw_config = item_ptr->rf_hw_config;

    #ifdef FEATURE_CDMA1X
    //if((byte)RF_DEF_HW != rfnv_cmn_mm_nv_tbl.rf_hw_config)
    //{
      rfnv_set_card_defaults((rf_hw_type)rf_get_rf_hw_config());
    //}
    #endif /* FEATURE_CDMA1X */

    #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    /* Enabling this flag would mean NV_RF_HW_CONFIG_I was read successfully*/
    rfnv_rf_hw_config_read = TRUE;
    break;

  default:
    MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

  } /* switch ( item_code )  */

} /* end of rfnv_cmn_mm_copy_nv_item */

/*===========================================================================

FUNCTION rfnv_cmn_mm_get_nv_tbl_ptr                                 

DESCRIPTION
  Returns pointer to Common MM RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to common MM RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_cmn_mm_nv_tbl_type *rfnv_cmn_mm_get_nv_tbl_ptr( void)
{
  return( &rfnv_cmn_mm_nv_tbl);
} 

/*===========================================================================

FUNCTION RFNV_CMN_MM_LIST_SIZE                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF Common MM items NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfnv_cmn_mm_list_size( void )
{
  return( ARR_SIZE( rfnv_cmn_mm_nv_items_list ) );
} 

/*===========================================================================

FUNCTION RFNV_CMN_ITEM_TABLE                            EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cmn_mm_item_table(void)
{
  return((void *) &rfnv_cmn_mm_nv_tbl);
}

/*===========================================================================

FUNCTION rf_get_rf_config

DESCRIPTION
  This function returns the RF Configuration flag value.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rf_get_rf_config(void)
{
  return rfnv_cmn_mm_nv_tbl.rf_config;
}

/*===========================================================================

FUNCTION rf_get_rf_hw_config

DESCRIPTION
  This function returns the RF HW Configuration flag value.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_HW_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rf_get_rf_hw_config(void)
{
  return rfnv_cmn_mm_nv_tbl.rf_hw_config;
}

/*===========================================================================

FUNCTION rf_get_rf_bc_config

DESCRIPTION
  This function returns the RF BC Configuration flag value for either 
  primary or diversity chain.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_BC_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/
uint64 rf_get_rf_bc_config(rf_path_enum_type path)
{
  uint64 config = 0;

  if ( (path == RF_PATH_0) || (path == RF_PATH_1) )
  {
    config = rfnv_cmn_mm_nv_tbl.rf_bc_config[path];
  }
  else
  {
    MSG_ERROR("Unrecognized path in rf_get_rf_bc_config(): %d",
              path, 0, 0);
  }

  return config;
}

