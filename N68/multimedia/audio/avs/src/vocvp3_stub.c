
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

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocvp3_stub.c#1 $ $DateTime: 2009/04/08 09:51:21 $ $Author: avangala $

when         who     what, where, why
--------     ---     ---------------------------------------------
03/20/09      av     Fixed compiler warning and removed the featurization.
03/12/09      sj     Reverted sud's change
03/10/09     sud     Fixed featurization error.
                     Updated Copyright.
21/02/09     sj      Fixed compilation issues
12/11/08     sud     Initial file with stub functions.

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



/* <EJECT> */
/*=============================================================================

                     DEFINTIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
typedef enum 
{
  VOCVP3_STUB_BUF_ID = 0
}  vocvp3_buf_id_type;

/* <EJECT> */
/*=============================================================================

                     CONSTANTS AND MACROS

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     VARIABLES

=============================================================================*/


/* <EJECT> */
/*=============================================================================

                     FUNCTION DECLARATIONS FOR MODULE

=============================================================================*/

/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_store_flag

DESCRIPTION

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
  return;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_get_store_flag

DESCRIPTION

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
  return FALSE;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_set_update_flag

DESCRIPTION

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
  return FALSE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION vocvp3_get_buf_address

DESCRIPTION

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
  return NULL;
}


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_init

DESCRIPTION

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
  return;
}  /* end of vocvp3_init() */



/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_read_from_efs

DESCRIPTION

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
  return;
}  /* end of vocvp3_read_from_efs() */


/* <EJECT> */
/*=============================================================================

FUNCTION vocvp3_write_to_efs

DESCRIPTION

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
  return;
}  /* end of vocvp3_write_to_efs() */

/* End of vocvp3.c */

