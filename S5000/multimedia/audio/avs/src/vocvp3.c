/*=============================================================================

                   VOICE PATH PARAMETER PRESERVATION --- API FUNCTIONS

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

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocvp3.c#1 $ $DateTime: 2009/04/08 09:51:21 $ $Author: avangala $

when         who     what, where, why
--------     ---     ---------------------------------------------
03/10/09     sud     Fixed featurization error.
                     Updated Copyright.
23/02/09     av      Fixed link error.
21/02/09     sj      Fixed compilation issues
12/16/08     sud     Initial file version.

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"           /* Customer specific definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#include <stdlib.h>             /* declarations for malloc and free        */
#include <string.h>             /* Interface to string library             */
#include <stdio.h>              /* declarations for sprintf                */

#include "vocvp3.h"
#include "pmem.h"               /* for pmem_malloc/pmem_free               */
#include "msg.h"


/* <EJECT> */
/*=============================================================================

                     DEFINTIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     CONSTANTS AND MACROS

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     VARIABLES

=============================================================================*/

/* Instance of QARE state control structure
*/
qare_state_ctrl_type vocvp3_qare_ctrl;

/* VOCVP3 State Control Data
*/
static vocvp3_state_control_type vocvp3_ctrl;



/* <EJECT> */
/*=============================================================================

                     FUNCTION DECLARATIONS FOR MODULE

=============================================================================*/

/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_store_flag

DESCRIPTION
  This function is called to set the state of EFS store operation that will be
  subsequently used during Audio Shutdown or Power-down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void vocvp3_set_store_flag
(
  boolean flag
)
{
  vocvp3_ctrl.store_flag_is_set = flag;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_get_store_flag

DESCRIPTION
  This function is used to get the state of the EFS write operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

boolean vocvp3_get_store_flag
(
  void
)
{
  return vocvp3_ctrl.store_flag_is_set;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_update_flag

DESCRIPTION
  This function is called to set update mask in vocvp3 control structure. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
boolean vocvp3_set_update_flag
(
  vocvp3_buf_id_type buf_id
)
{
  boolean status = TRUE;

  if (buf_id < VOCVP3_BUF_IDMAX)
  {
    vocvp3_ctrl.update_mask |= ((uint32)1 << buf_id);
  }
  else
  {
    status = FALSE;
  }
  return status;
}


/* <EJECT> */
/*===========================================================================

FUNCTION vocvp3_get_buf_address

DESCRIPTION
  This function returns the BUF address (offset) for a given BUF_ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void* vocvp3_get_buf_address
(
  vocvp3_buf_id_type buf_id
)
{
  void* buf_ptr = NULL;
  
  if (buf_id < VOCVP3_BUF_IDMAX)
  {
    buf_ptr=vocvp3_ctrl.start_addresses[buf_id];
  }
  return buf_ptr;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_init

DESCRIPTION
  Call this function to initialize QARE and initialize all the control/status 
  structure of vocvp3_ctrl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void vocvp3_init
(
  unsigned int* buf_sizes,
  uint16* buf_defaults[]
)
{
  int index=0;
  int total_size=0;
  int start_address=0;

  if ((NULL==buf_sizes)|| (NULL==buf_defaults))
  {
    return;
  }

  /* Initialize the size and defalut values for each buffer (BUF_ID)
  */
  for(index=0;index<VOCVP3_BUF_IDMAX;++index)
  {
    vocvp3_ctrl.sizes[index]= buf_sizes[index];
    vocvp3_ctrl.defaults[index]=buf_defaults[index];
    total_size+=buf_sizes[index];
  }

  /* Initialize Qualcomm Audio Registry
  */
  qare_init(&vocvp3_qare_ctrl, vocvp3_ctrl.sizes);

  /* Initialize the remaining members of vocvp3 control structure
  */
  vocvp3_ctrl.restore_flag_is_set = TRUE;
  vocvp3_ctrl.store_flag_is_set=FALSE;
  vocvp3_ctrl.restore_mask = 0;
  vocvp3_ctrl.store_mask = 0;  
  vocvp3_ctrl.update_mask = 0;

  /* Allocate PMEM memory
  */
  vocvp3_ctrl.buf_ptr = pmem_malloc(total_size, PMEM_UNCACHED_ID);
  if(vocvp3_ctrl.buf_ptr == NULL)
  {
    vocvp3_ctrl.restore_flag_is_set = FALSE;
    return;
  }

  /* Now compute the start addressess for each buffer.
  */
  for(index=0;index<VOCVP3_BUF_IDMAX;++index)
  {
    vocvp3_ctrl.start_addresses[index]=(char*)vocvp3_ctrl.buf_ptr+start_address;
    start_address+=vocvp3_ctrl.sizes[index];
  }

}  /* end of vocvp3_init() */



/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_read_from_efs

DESCRIPTION
  This function read the VP3 data from EFS and write to PMEM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void vocvp3_read_from_efs
(
  void
)
{
  qare_error_code_type read_status = QARE_ERR_SUCCESS;
  qare_error_code_type write_status = QARE_ERR_SUCCESS;
  vocvp3_buf_id_type buf_id = (vocvp3_buf_id_type)0;

  if (vocvp3_ctrl.restore_flag_is_set)
  {
    for (buf_id = (vocvp3_buf_id_type)0; buf_id < VOCVP3_BUF_IDMAX; ++buf_id)
    {
      read_status = qare_read_data(&vocvp3_qare_ctrl, (qare_file_id_type)buf_id,
                                  (char*)(vocvp3_ctrl.start_addresses[buf_id]));

      if (read_status != QARE_ERR_SUCCESS)
      {
        if (read_status == QARE_ERR_FILE_NONEXIST)
        {
          /* Update both PMEM and EFS with default data very first time
          */
          memcpy(vocvp3_ctrl.start_addresses[buf_id],
                      vocvp3_ctrl.defaults[buf_id],vocvp3_ctrl.sizes[buf_id]);
                                                                                 
          write_status  = qare_write_data(&vocvp3_qare_ctrl,
                (qare_file_id_type)buf_id, (char*)vocvp3_ctrl.defaults[buf_id]);
          if (write_status != QARE_ERR_SUCCESS)
          {
            vocvp3_ctrl.restore_mask |= ((uint32)1 << buf_id);
          }
        }
        else 
        {
          /* If read failed for some other reason, write PMEM with default data.
             Next time the EFS wil be updated with valid data.
          */
          memcpy(vocvp3_ctrl.start_addresses[buf_id],
                        vocvp3_ctrl.defaults[buf_id],vocvp3_ctrl.sizes[buf_id]);
          vocvp3_ctrl.restore_mask |= ((uint32)1 << buf_id);
        }  /* end of if(read_status == QARE_ERR_FILE_NONEXIST) */
      }  /* end of if(read_status != QARE_ERR_SUCCESS) */
    }  /* end of for loop*/
  }  /* end of if(vocvp3_ctrl.restore_flag_is_set) */
}  /* end of vocvp3_read_from_efs() */


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_write_to_efs

DESCRIPTION
  This function write the PMEM data to EFS. Usually this function will be called
  during Audio Shutdown or Phone shut down

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void vocvp3_write_to_efs
(
  void
)
{
  vocvp3_buf_id_type buf_id = (vocvp3_buf_id_type)0;
  
  if (vocvp3_get_store_flag()) 
  {
    for (buf_id = (vocvp3_buf_id_type)0; buf_id < VOCVP3_BUF_IDMAX; ++buf_id)
    {
      if ( (vocvp3_ctrl.update_mask & ((uint32)1 << buf_id)) != 0)
      {
        if ( qare_write_data(&vocvp3_qare_ctrl, (qare_file_id_type)buf_id,
             (char*)(vocvp3_ctrl.start_addresses[buf_id]) ) != QARE_ERR_SUCCESS)
        {
          vocvp3_ctrl.store_mask |= ((uint32)1 << buf_id);
          MSG_HIGH("File restoring failed for BUF_ID %d", buf_id,0,0);
        }
        else 
        {
          vocvp3_ctrl.update_mask &= ~((uint32)1 << buf_id);
          MSG_HIGH("File restoring success for BUF_ID %d",buf_id,0,0);
        }
      }  
    }  /* end of for loop */
    /* Clear the flag at the end of update
    */
    vocvp3_set_store_flag(FALSE);
  } /* end of if (vocvp3_get_store_flag()) */ 
  else
  {
    MSG_HIGH("VP3: Nothing to store in EFS this session",0,0,0);
  } 
}  /* end of vocvp3_write_to_efs() */

/* End of vocvp3.c */

