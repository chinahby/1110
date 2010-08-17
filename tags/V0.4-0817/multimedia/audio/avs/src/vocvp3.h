#ifndef VOCVP3_H 
#define VOCVP3_H 
/*=============================================================================

                   VOICE PATH PARAMETER PRESERVATION --- HEADER FILE

GENERAL DESCRIPTION
  This file contains declarations associated with VP3.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2008 - 2009 by QUALCOMM, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================*/

/* <EJECT> */
/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocvp3.h#1 $ $DateTime: 2009/04/08 09:51:21 $ $Author: avangala $

when         who     what, where, why
--------     ---     ---------------------------------------------
03/10/09     sud     Fixed featurization error.
                     Updated Copyright.
21/02/09     sj      Fixed compilation issues
12/16/08     sud     Initial file version.

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"   /* Customer specific definitions                   */
#include "target.h"     /* Target specific definitions                     */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "qare.h"       /* Qualcomm Audio Registry defintions              */

/* <EJECT> */
/*=============================================================================

                     CONSTANTS

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     MACROS

=============================================================================*/
#define VOCVP3_BUF_IDMAX QARE_FILEID_MAX


/* <EJECT> */
/*=============================================================================

                     TYPEDEFS

=============================================================================*/

typedef qare_file_id_type vocvp3_buf_id_type;


typedef struct vocvp3_state_control_struct
{
  /* flag that indicates PMEM alloc status during power up
  */
  boolean restore_flag_is_set;
  /* flag that indicates EFS update pending
  */
  boolean store_flag_is_set;
  /* EFS file restore status during power up   
  */
  uint32 restore_mask;               
  /* EFS file store status - power down and audio shutdown
  */
  uint32 store_mask; 
  /* Flag that indicates the updates recieved from DSP            
  */
  uint32 update_mask;                
  /*Sizes for each of the buffers (BUF_ID)             
  */
  unsigned int sizes[VOCVP3_BUF_IDMAX];    
  /* Default values of each of the buffers (BUF_ID)
  */
  uint16* defaults[VOCVP3_BUF_IDMAX];     
  /* Initializes with buffer addresses during power up 
  */
  void* start_addresses[VOCVP3_BUF_IDMAX];
  /* Pointer to allocated PMEM memory          
  */
  void* buf_ptr;                          
} vocvp3_state_control_type;



/* <EJECT> */
/*=============================================================================

                     FUNCTIONS

=============================================================================*/


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_store_flag

DESCRIPTION
  This function is used to set the vocvp3_store_flag_is_set to TRUE or FALSE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

extern void vocvp3_set_store_flag
(
  boolean flag
);

/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_get_store_flag

DESCRIPTION
  This function is used to read the value of flag vocvp3_store_flag_is_set 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

extern boolean vocvp3_get_store_flag
(
  void
);


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_update_flag

DESCRIPTION
  This function is called to set update flag in vocvp3 control structure. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern boolean vocvp3_set_update_flag
(
  vocvp3_buf_id_type buf_id
);


/* <EJECT> */
/*===========================================================================

FUNCTION vocvp3_get_buf_address

DESCRIPTION
  This function returns the BUF address for a given BUF_ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void* vocvp3_get_buf_address
(
  vocvp3_buf_id_type buf_id
);



/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_init

DESCRIPTION
  Call this function to initialize qare and allocate memory (PMEM) for VP3 
	buf_sizes should be an array of size VOCVP3_BUF_IDMAX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void vocvp3_init
(
  unsigned int* buf_sizes, 
  uint16* buf_defaults[]
);


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_read_from_efs

DESCRIPTION
  Call this function to read the VP3 data from EFS and write to PMEM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void vocvp3_read_from_efs
(
  void
);


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_write_to_efs

DESCRIPTION
  Call this function during power down to write the PMEM data to EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void vocvp3_write_to_efs
(
  void
);


#endif   /* VOCVP3_H  */

