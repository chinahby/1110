#ifndef SDB_H
#define SDB_H
/*===========================================================================

           S O U N D   D A T A B A S E    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the sound database.

Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/sdb.h_v   1.0   06 Oct 2001 12:58:52   ropalsky  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sdb.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/99    st     Modified after code review.
08/17/99    st     Created file.

===========================================================================*/

#include "comdef.h"
#include "snd.h"

/* Sound database return status */
typedef enum {
  SDB_SUCCESS,
  SDB_FAIL
} sdb_status_type;

/* <EJECT> */
/*===========================================================================
FUNCTION sdb_get_sound

DESCRIPTION
  This routine returns a buffer with the requested sound.

DEPENDENCIES
  sdb_sound_ptrs

RETURN VALUE
  sdb_status_type
  snd_sound_type

SIDE EFFECTS
  None.
===========================================================================*/
extern sdb_status_type sdb_get_sound(
  snd_sound_id_type    index,         /* Index of sound to retrieve */
  const snd_sound_type **sound        /* Buffer with sound returned */
);
/* <EJECT> */
/*===========================================================================
FUNCTION sdb_free_sound

DESCRIPTION
  This routine frees the memory that was allocated by sdb_get_sound().
  A ROM table implementation is currently employed, therefore there is nothing
  to do in this function.

DEPENDENCIES
  None.

RETURN VALUE
  sdb_status_type

SIDE EFFECTS
  None.
===========================================================================*/
extern sdb_status_type sdb_free_sound( 
  const snd_sound_type *buf                 /* Buffer to free */
);
#endif /* SDB_H */
