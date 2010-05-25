#ifndef RFNV_H
#define RFNV_H
/*===========================================================================

                             R F   N V   M a n a g e r

DESCRIPTION
  This header file contains the data structure and function declarations to interface with
  the RF NV Manager.

Copyright (c) 1997, 1998, 1999 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfnv.h#3 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/08   vb      Corrected errors for RF_HAS_BCX_BLOCK_0_NV
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
11/29/06   jfc     Conditionally define diversity NV modes
10/17/06   ycl     Modifications for initial MSM7600 build.
09/18/06   hkk     Removed rfnv_cdma1x_bc1.h from RF_HAS_GPS_NV_TABLE
01/10/06   sar     Fixed lint errors.
11/15/05   rmd     Added support for band class 11.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
07/01/05   sar      Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
06/13/05   dbc     Removed support for MFLO (RF_HAS_MFLO)
05/02/05   dbc     Added support for MFLO (RF_HAS_MFLO)
01/07/05   sar     Updated featurization in rfnv_supported_mode_type to match 
                   that of rfnv_supported_mode_info_table.
09/30/04   sar     Added functions rfnv_retrieve_nv_items(), rfnv_nv_read() and
                   rfnv_nv_write().
05/25/04   sar     Added support for legacy cdma and pcs bands. Updated feature 
                   flags for nv tables.
03/19/04   sar     Updated to add new band classes.
01/30/04   zg      Fixed "RF_HAS_GPS"
12/04/03   sar     Fixed typo in Featurization in definition of 
                   rfnv_supported_mode_type.
10/07/03   sar     Adapted for Jaguar. Modified structure rfnv_supported_mode_info_type
                   and updated rfnv_supported_mode_info_table for 1X. Added prototype
                   for rfnv_get_nv_table().
===========================================================================*/

/*===========================================================================

                                 INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "customer.h"
#include "rficap.h"

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

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
#include "rfnv_cdma1x_bcx_block_0.h"
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
#include "rfnv_cdma1x_bcx_block_1.h"
#endif

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
#endif /* FEATURE_CDMA1X */
/*===========================================================================

                           DATA TYPE DECLARATIONS

===========================================================================*/
#define NV_ITEM_ATTRIB_CRTCL_BMSK  0x01  /* bit 0 = critical */
#define NV_ITEM_ATTRIB_NON_CRTCL_BMSK 0x00  /* bit 0 = critical */


typedef enum
{
  RFNV_READ_SUCCESS,
  RFNV_READ_FAILED,
  RFNV_WRITE_SUCCESS,
  RFNV_WRITE_FAILED,
  RFNV_INACTIVE_ITEM
} rfnv_status_type;

typedef struct
{
  /* nv item identifier */
  nv_items_enum_type   nv_item_id;

  /* flag to indicate calibration item */
  byte              nv_item_attrib;

} rfnv_items_list_type;

/* function description for extracting nv item list size              */
typedef word (*rfnv_get_list_size_type)(void);

/* function description for accessing the pointer to the nv structure */
typedef void *(*rfnv_get_nv_item_table_type)(void);

typedef struct
{
  /* pointer to an array of supported NV items for a particular mode  */
  const rfnv_items_list_type *nv_item_list_ptr;

  /* void pointer to the structure with the nv values for the band    */
  rfnv_get_nv_item_table_type get_nv_item_table;

  /* size of the array of supported NV items for a particular mode    */
  rfnv_get_list_size_type get_nv_item_list_size;

  /* flag to indicate that the NV data is valid for that mode         */
  boolean             nv_data_valid_flag;

  /* function pointer to a function to copy NV item from NV item temp
     buffer to the NV table of a particular mode                      */
  void (*copy_nv_item) ( nv_items_enum_type nv_item_id,
                         nv_item_type *nv_item_buf_ptr);

} rfnv_supported_mode_info_type;

typedef enum
{
  RFNV_COMMON_MULTIMODE = 0,

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
  RFNV_CDMA1X_COMMON,
  #endif

  #ifdef RF_HAS_BC0_NV_TABLE
  RFNV_CDMA1X_BC0,
  #endif

  #ifdef RF_HAS_BC1_NV_TABLE
  RFNV_CDMA1X_BC1,
  #endif
  
  #ifdef RF_HAS_BC2_NV_TABLE
  RFNV_CDMA1X_BC2,
  #endif
  
  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif
  
  #ifdef RF_HAS_BC4_NV_TABLE
  RFNV_CDMA1X_BC4,
  #endif
  
  #ifdef RF_HAS_BC5_NV_TABLE
  RFNV_CDMA1X_BC5,
  #endif
  
  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC14_NV_TABLE
  RFNV_CDMA1X_BC14,
  #endif
  
  #ifdef RF_HAS_BC15_NV_TABLE
  RFNV_CDMA1X_BC15,
  #endif

  #ifdef RF_HAS_GPS_NV_TABLE 
#error code not present
  #endif

  /* add additional Band Class mode enum field here */
  /* Add all the blocks field here to maintain integrity*/
  #ifdef RF_HAS_SUB_BLOCK_SUPPORT

  #ifdef RF_HAS_BCX_BLOCK_0_NV
  RFNV_CDMA1X_BCX_BLOCK_0,
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
  RFNV_CDMA1X_BCX_BLOCK_1,
  #endif

  #endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

  RFNV_SUPPORTED_MODE_NUM
  

} rfnv_supported_mode_type;

typedef struct
{

  /* The RF configuration as read from the NV Services */
  uint8 rf_config;

  uint8 rf_hw_config;

  uint64 rf_bc_config[2];

} rfnv_cmn_mm_nv_tbl_type;
 
/* pointer to CMN NV table */
extern rfnv_cmn_mm_nv_tbl_type *rfnv_cmn_mm_nv_tbl_ptr;
extern rfnv_cmn_mm_nv_tbl_type  rfnv_cmn_mm_nv_tbl;

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
extern const rfnv_items_list_type rfnv_cmn_mm_nv_items_list[ ];

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION rfnv_retrieve_nv_items

DESCRIPTION
  This function retrieves NV items for each mode specified in the
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
extern rfnv_status_type rfnv_retrieve_nv_items
(
  rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
  rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                           reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
);

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
extern boolean rfnv_is_nv_data_valid(rfnv_supported_mode_type mode);

/*===========================================================================

MACRO RFNV_SATURATE

DESCRIPTION
  This macro returns the input (val) limited to the range specified by (min)
  and (max).  This is used for clipping the results of adjustment
  calculations where the newly calculated value may exceed the legal range
  for the control.  Since these calculations are typically closed loop
  integrators, this macro prevents "integrator windup" and/or "control
  overflow/underflow".

PARAMETERS
  val   the value to be limited (saturated) interpreted as a signed integer
  min   the minimum allowed value (e.g. 0)
  max   the maximum allowed value (e.g. 2^n-1)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RFNV_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

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
  a void pointer to the requested band.

SIDE EFFECTS
  none

===========================================================================*/
void  *rfnv_get_nv_table(rfnv_supported_mode_type mode);

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
);
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
);

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
);
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
);
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
extern void rfnv_set_nv_item(
                            nv_items_enum_type  item_code,       /* Item to set */
                            nv_item_type        *item_ptr,       /* Pointer to the item */
                            rex_tcb_type        *task_ptr,       /* Calling task's TCB pointer */
                            rex_sigs_type       task_wait_sig,   /* Task signal to wait for when
                                                                    reading NV */
                            void                (*task_wait_handler)( rex_sigs_type )
                            /* Task's wait function to be called
                               when reading NV */
                            );

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
void rfnv_set_nv_data_valid_flag(rfnv_supported_mode_type mode, boolean flag);

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
boolean rfnv_get_cal_stat(void);

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
);

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
rfnv_cmn_mm_nv_tbl_type *rfnv_cmn_mm_get_nv_tbl_ptr ( void );

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
uint16 rfnv_cmn_mm_list_size( void );

/*===========================================================================

FUNCTION RFNV_CMN_MM_ITEM_TABLE                            EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cmn_mm_item_table(void);


#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RFNV_BAND_CLASS_NV_IS_VALID                    Internal FUNCTION

DESCRIPTION
  This function retuen TRUE or FalSE depending on the fact that if 
  the Passed Band Calss is Supported at complie time and corresponding
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
boolean rfnv_band_class_nv_is_valid(uint8 band_class_supported,rfnv_digital_items_type** nv_digital_item_ptr);

/*===========================================================================

FUNCTION RFNV_SET_BLOCK_MASK_OVERRIDE                    INTERNAL FUNCTION

DESCRIPTION
Sets the Block Mask forced through FTM Mode or a given Band Class
RF Driver is tuned to.
Returns TRUE or FALSE depending on the fact that Block mask requeste
is Valid or not

DEPENDENCIES
  
RETURN VALUE
  Returns TRUE or FALSE depending on the fact that Block mask requeste
  is valid or not

SIDE EFFECTS
Global Variables rf_block_mask_is_forced and rf_forced_block_mask_value
are changed depending on what Block mask is forced.
  

===========================================================================*/
boolean rf_set_block_mask_override(rfcom_device_enum_type device, int8 index, uint32 block_mask);

/*===========================================================================

FUNCTION RF_CLEAR_BLOCK_MASK_OVERRIDE                    INTERNAL FUNCTION

DESCRIPTION
Clears the forced Block mask set though rf_set_block_mask_override()
function.
This function is called whenver a new Band Class is tuned through
FTM Mode

DEPENDENCIES
  
RETURN VALUE
None

SIDE EFFECTS
Global Variables rf_block_mask_is_forced is set to FALSE
  

===========================================================================*/
void rf_clear_block_mask_override(rfcom_device_enum_type);

/*===========================================================================

FUNCTION RFNV_GET_NV_DIGITAL_ITEM_PTR                    INTERNAL FUNCTION

DESCRIPTION
Call  rfc_get_block() to get the Block Mask for given Band/Channel combination 
If the Block Mask is forced through FTM Mode, then it uses that Block Mask 
Returns the pointer to the correponding either the Parent Band Class NV Table
or the NV Table of corresponding Block NV depending on the Band/Channel 
Parameters Passed

DEPENDENCIES
  Parent Band Class NV and Block NV must be populated before and
  they must have been appended in Link List structure

RETURN VALUE
Returns the pointer to the correponding eitehr the the Parent Band Class NV Table
or the NV Table of corresponding Block NV depending on the Band/Channel 
Parameters Passed


SIDE EFFECTS
Based on NV Item Table Returned, whole of the NV items either corresponding
to Block or Parent Band class will be used.
  

===========================================================================*/
rfnv_digital_items_type* rfnv_get_nv_digital_item_ptr(rfcom_device_enum_type device,rf_card_band_type band, word rf_chan);


/*===========================================================================

FUNCTION RF_GET_PARENT_BAND_FOR_BLOCK                   Internal FUNCTION

DESCRIPTION
  This function returns the Parent band Class associated with the Block

DEPENDENCIES
 The Static QCN determining tehArent band Class for Sub Block must be loaded
 to get correct value

RETURN VALUE
  Band Type

SIDE EFFECTS

===========================================================================*/
rf_card_band_type rfnv_get_parent_band_for_block(rf_card_blocks_type block);
#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
#endif /* RFNV_H */

