/*=============================================================================

        R F  D r i v e r  L o g i c a l  L a y e r  I n t e r f a c e 
                           D e f i n i t i o n

GENERAL DESCRIPTION
  This file provides the logical layer definitions of the RF driver. 

EXTERNALIZED FUNCTIONS
  None  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2002  by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfll.c#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   jfc     Call rf_init() if either GSM or WCDMA bands are enabled
12/03/07   avi     Introduced a run-time check for support of WCDMA or GSM bands
                   to deternmine NV read success status.
11/26/07   rmd     For TRF6285, within rfll_init() forced nv_init_success = TRUE. 
                   This is a temporary change. I also separated the GSM and WCDMA
                   nv check (required for GSM only build).
10/12/07   adk     Restore function-call rfi_init_target_dependent_hw() for QRF7600.
10/08/07   jfc     Update WCDMA and GSM mode enums
09/28/07   adk     Bypass function-call rfi_init_target_dependent_hw()
                   for QRF7600 (for now).
08/06/07   jfc     Featurized code for EDGE-only builds
07/27/07   avi     Fix 1x compiler warnings
02/28/07   ycl     Fix compiler warnings.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial MSM7600 build.
05/27/05   sar     Added support for the device RFCOM_RECEIVER_DUAL.
04/26/05   sar     Added support for rfll_get_band_mask().
01/07/05   sar     Allowing rf_init_once_gsm() to be called only once in rfll_init
                   for RFCOM_TRANSCEIVER_0 only.
08/09/04    et     changed rf_init_gsm() to rf_init_once_gsm()
05/10/04    et     changed rf_init_gsm() call to support GSM 
                   added rfll_nv_items_read static boolean to prevent from 
                   calling retreive_nv_items more than once.
10/7/03    sar     Added featurized call to rfnv_retrieve_nv_items() to use the
                   new band specific nv driver interface.
03/11/03   sar     Added call to rfnv_get_cal_stat() in rfll_init() to incorporate
                   calibration status from cdma 1x nv driver.
03/05/03    eh     Modified rfll_init to always initialize and return device descriptor
                   pointer and update the device descriptor state based on status from
                   rf_init or rf_init_gsm. This is to support run-time switching between
                   FTM and AMSS mode.
11/15/02   sar     Removed featurization of the call to rf_gsm_init().
11/04/02   sd      Added support for intersystem measurements.
10/24/02   sar     Featurized function call to rf_gsm_init() in rfll_init(), so
                   that it is bypassed for Leopard targets. 
10/10/02   sar     Included rfnv header file and modified rfll_init function to
                   support cdma 1X mode.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

#include "rfcom.h"
#include "rfll.h"
#include "rfllcb.h"
#include "rf.h"
#include "rficap.h"

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_CDMA1X
#include "rfnv.h"
#include "sys.h"
#include "rfnv_cdma1x.h"
#endif /* FEATURE_CDMA1X */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#if defined (FEATURE_WCDMA)
#error code not present
#endif

#if defined (FEATURE_GSM)
#error code not present
#endif



/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*---------------------------------------------------------------------------
  Table of logical layer device descriptors.  One descriptor for each
  device supported by the RF driver. 
---------------------------------------------------------------------------*/
rfll_device_desc_type rfll_device_desc_table[RFCOM_MAX_DEVICES] =
{
  /*---------------------------------------------
    RFLL_TRANSCEIVER_0: Device 0 descriptor table.
  ---------------------------------------------*/
  {
    RFCOM_TRANSCEIVER_0,                /* Device identifier */
    RFCOM_PARKED_MODE,                  /* Device operating mode */
    RFCOM_PARKED_MODE,                  /* Device intersystem mode */
    RFLL_PARKED_STATE,                  /* Device RF state */
    RF_PATH_0,                          /* Device physical path */
  },

  /*---------------------------------------------
    RFLL_RECEIVER_1: Device 1 descriptor table.
  ---------------------------------------------*/
  {
    RFCOM_RECEIVER_1,                   /* Device identifier */
    RFCOM_PARKED_MODE,                  /* Device operating mode */
    RFCOM_PARKED_MODE,                  /* Device intersystem mode */
    RFLL_PARKED_STATE,                  /* Device RF state */
    RF_PATH_1,                          /* Device physical path */
  },

  /*---------------------------------------------
    RFLL_RECEIVER_DIV: Device 2 descriptor table.
  ---------------------------------------------*/
  {
    RFCOM_RECEIVER_DIV,                 /* Device identifier */
    RFCOM_PARKED_MODE,                  /* Device operating mode */
    RFCOM_PARKED_MODE,                  /* Device intersystem mode */
    RFLL_PARKED_STATE,                  /* Device RF state */
    RF_PATH_1,                          /* Device physical path */
  },

  /*---------------------------------------------
    RFLL_RECEIVER_DUAL: Device 3 descriptor table.
  ---------------------------------------------*/
  {
    RFCOM_RECEIVER_DUAL,                /* Device identifier */
    RFCOM_PARKED_MODE,                  /* Device operating mode */
    RFCOM_PARKED_MODE,                  /* Device intersystem mode */
    RFLL_PARKED_STATE,                  /* Device RF state */
    RF_PATH_1,                          /* Device physical path */
  }

};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  RFLL_INIT

DESCRIPTION
  This function performs the common RF driver initialization for dual
  mode GSM and WCDMA operation.  It reads common and mode specific RF NV
  parameters from NV and initializes common and mode specific NV tables.
  It also initializes common hardware and mode specific RF hardware for
  GSM and WCDMA modes of operation.  RF driver is placed in parked/sleep
  state.

DEPENDENCIES
  This function must be called before any other RF driver function.
  
RETURN VALUE
  Logical layer device descriptor if initialization successful, otherwise
  NULL.

SIDE EFFECTS
  None

===========================================================================*/
rfll_device_desc_type* rfll_init
(
  rfcom_device_enum_type device,        /* RF device */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
)
{
  rfll_device_desc_type *desc_ptr;
 
  static boolean nv_init_success = FALSE;
  static boolean nv_items_read = FALSE;

  #ifdef FEATURE_CDMA1X
  boolean cdma1x_init_success = FALSE;
  #endif  /* FEATURE_CDMA1X */
  
  #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
  #endif  /* FEATURE_GSM */

  boolean all_success = TRUE;


  /* initialize device descriptor. */
  desc_ptr                    = &rfll_device_desc_table[device];
  //desc_ptr->device            = device;
  desc_ptr->mode              = RFCOM_PARKED_MODE;
  desc_ptr->intersystem_mode  = RFCOM_PARKED_MODE;
  desc_ptr->state             = RFLL_PARKED_STATE;

  switch (device)
  {
  case RFCOM_TRANSCEIVER_0:
    if (!nv_items_read)
    {
      if (rfnv_retrieve_nv_items(caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr) == RFNV_READ_SUCCESS)
      {
        /* Not missing critical NV items */
        nv_init_success = TRUE;
      }
      /* Remember that we've read the NV. */
      nv_items_read = TRUE;
    }
    #ifdef FEATURE_CDMA1X
    cdma1x_init_success = rfnv_get_cal_stat();
		#endif /* FEATURE_CDMA1X */

    #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */
    break;

  case RFCOM_RECEIVER_DIV:
  case RFCOM_RECEIVER_1:
  case RFCOM_RECEIVER_DUAL:
    MSG_HIGH("Initializing Second Chain (%d)", device, 0, 0);
    break;
  
  default:
    MSG_ERROR("Unrecognized chain (%d)", device, 0, 0);
    break;
  }

  /* Initialize the RF driver call back service */
  rfllcb_init();

  #if defined(FEATURE_TRF6285)
#error code not present
  #endif


  #ifdef FEATURE_CDMA1X
  all_success &= (cdma1x_init_success & nv_init_success);
  #endif

  #if defined (FEATURE_WCDMA)
#error code not present
  #endif

  #if defined (FEATURE_GSM)
#error code not present
  #endif

  if ( (device == RFCOM_TRANSCEIVER_0) && (!all_success) )
  {
    /* RF initialization failed */
    desc_ptr->state = RFLL_NV_INVALID_STATE;
    rfll_device_desc_table[RFCOM_RECEIVER_DIV].state = RFLL_NV_INVALID_STATE;
    rfll_device_desc_table[RFCOM_RECEIVER_1].state = RFLL_NV_INVALID_STATE;
    rfll_device_desc_table[RFCOM_RECEIVER_DUAL].state = RFLL_NV_INVALID_STATE;
  }

  return (desc_ptr);

} /* rfll_init */




/*===========================================================================

FUNCTION  RFLL_PARK

DESCRIPTION
  This function places the device to parked mode.

DEPENDENCIES
  None  

RETURN VALUE
  Returns 0 if operation successful otherwise a non-zero.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfll_park
(
  rfll_device_desc_type *dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
)
{

#ifdef FEATURE_CDMA1X
    rfll_device_desc_type *desc_ptr;
    
    desc_ptr = &rfll_device_desc_table[dev_desc->device];
  

    switch(desc_ptr->mode){
    case RFCOM_PARKED_MODE:     /* Parked/sleep mode */
        /* Already in park mode, nothing to do */
        break;
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM   */
#ifdef FEATURE_CDMA1X
    case RFCOM_1X_MODE:         /* CDMA 1X */
        desc_ptr->mode   = RFCOM_PARKED_MODE;
        desc_ptr->state  = RFLL_PARKED_STATE;
        rf_1x_inact(desc_ptr->device); /* Turn off 1X */
        break;
#endif /* FEATURE_CDMA1X */
    default:
        MSG_ERROR("Illegal Mode in rfll_park",0,0,0);
        break;
    }
#endif //FEATURE_CDMA1X
    
    return 0;
  
} /* rfll_park() */



/*===========================================================================

FUNCTION RFLL_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None

===========================================================================*/
uint64 rfll_get_band_mask(void )
{
  #ifdef T_QUASAR_X
#error code not present
  #elif defined(FEATURE_WCDMA) || defined(FEATURE_CDMA1X) || defined(FEATURE_GSM)
  //return (rfnv_get_band_mask( ));
  return rf_get_rf_bc_config(RF_PATH_0);

  #else
  rfnv_status_type    nv_read_status = RFNV_READ_SUCCESS;
  nv_item_type        nv_item_buf;
  sys_band_mask_type  band_mask = RF_DEF_BC;
  
  if((nv_read_status = rfnv_nv_read(NV_RF_BC_CONFIG_I,&nv_item_buf))== NV_DONE_S)
  {
    band_mask = nv_item_buf.rf_bc_config;
  }
  
  return band_mask;
  #endif

} /* rfll_init() */

/*===========================================================================

FUNCTION RFLL_TURN_ON_RF_LDO             

DESCRIPTION
  This function turns on RF related LDOs. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfll_turn_on_rf_ldo( uint16 delay )
{
  #ifdef FEATURE_WCDMA    
#error code not present
  #endif
}

/*===========================================================================

FUNCTION RFLLUMTS_IOCTL             

DESCRIPTION
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
rfcom_ioctl_return_type rfllpark_ioctl
(
  rfll_device_desc_type * dev_desc,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
)
{
  rfcom_ioctl_return_type err = RFCOM_IOCTL_NO_ERROR;

  if (pbuf == NULL)
  {
    err = RFCOM_IOCTL_NULL_BUFFER;
  }
  else
  {
    switch (request)
    {
    case RFCOM_IOCTL_GET_DBG_SCR_SUPPORTED_HW:
      *(uint8 *)pbuf = rf_get_rf_hw_config();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_NV_BAND:
      *(uint8 *)pbuf = 0;
      break;

    default:
      err = RFCOM_IOCTL_INVALID_REQUEST;
      break;
    }
  }

  return err;
}
