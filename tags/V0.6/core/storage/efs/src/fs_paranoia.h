/***********************************************************************
 * fs_paranoia.h
 *
 * Check on the EFS private data structures for any data corruption.
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * This feature does run-time checks to verify that EFS private data
 * structures are not corrupted while EFS is not executing.
 *
 * When releasing the lock, we compute and store a hash of all private
 * data.  When taking the global lock, we compare the data structures
 * to the hashed values and announce any discrepancies.  Clearly, this
 * takes some processing time, so it should be enabled only as needed.
 * *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_paranoia.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-12-05   rp    Create

===========================================================================*/

#ifndef __FS_PARANOIA_H__
#define __FS_PARANOIA_H__

#include "fs_config.h"
#include "comdef.h"


/*=============================================================================
  FUNCTION      fs_paranoia_init

  DESCRIPTION
    Initilaize the paranoia features internal data structures. Should be
    called at the time of efs initialization.

  PARAMETERS    None.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_init (void);

/*=============================================================================
  FUNCTION      fs_paranoia_register

  DESCRIPTION
    This function is used to register with paranoia to start monitoring a
    data region.

  PARAMETERS
    data_ptr  ->  [IN]
        Start address of the data that should be watched.

    data_size ->  [IN]
        How many bytes from the Start address that should be watched.

    name ->  [IN]
        This argument is not watched by the paranoia, In the event of any
        crc mismatch, we could use this name to easily locate which data
        got scribbled.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_register (void *data_ptr, int32 data_size, char *name);

/*=============================================================================
  FUNCTION      fs_paranoia_unregister

  DESCRIPTION
    This function is used to unregister with paranoia the previously registered
    start-address using the fs_paranoia_register() function.

  PARAMETERS
    data_ptr  ->  [IN]
        Start address of the data that was passed to the register function.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_unregister (void *data_ptr);

/*=============================================================================
  FUNCTION      fs_paranoia_set_skip_check

  DESCRIPTION
    This function is used to turn On/Off the paranoia check temporarily at
    run-time. It is ideal for any off-target test files that want to modify
    any internal efs-structures, In this case the paranoia-check could be
    turned off until desired.

  PARAMETERS
    skip_check  ->  [IN]
        1 -> Means turn Off paranoia check; 0 -> Means turn On paranoia check.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_set_skip_check (uint8 skip_check);


/*=============================================================================
  FUNCTION      fs_paranoia_record

  DESCRIPTION
    This function should get called from FS_GLOBAL_UNLOCK(). In this function
    we compute and store the CRC on all of the data-regions that we are
    watching.

  PARAMETERS    None.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_record (void);


/*=============================================================================
  FUNCTION      fs_paranoia_check

  DESCRIPTION
    This function should get called from FS_GLOBAL_LOCK(). In this function
    we compute the CRC on all of the data-regions that we are watching and
    compare it against the stored CRC which we would have computed while
    releasing the FS_GLOBAL_UNLOCK(). The computed CRC's should match otherwise
    we assert.

  PARAMETERS    None.

  RETURN VALUE  None.
=============================================================================*/
void fs_paranoia_check (void);

#endif /* not __FS_PARANOIA_H__ */
