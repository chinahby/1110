#ifndef TDB_H
#define TDB_H
/*===========================================================================

            T O N E S   D A T A B A S E    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the tones database.

Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/tdb.h_v   1.0   06 Oct 2001 12:58:32   ropalsky  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/tdb.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/99   st      Removed tdb_init_from_nv() function prototype.
08/17/99  jkl/st   Created file.

===========================================================================*/

#include "comdef.h"
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                    DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/
#ifdef TDB_DEBUG
#define DEF_DTMF(i,h,l)   (i), (h), (l)
#else
#define DEF_DTMF(i,h,l)   (h), (l)
#endif

/* Frequencies and generators associated with DTMF's and single tones.
** This table is indexed by the enumeration containing all of the DTMF's
** and single tones.  Note: for some targets there is a "high" side and
** a "low" side to the DTMF generator.
*/
typedef struct {
#ifdef TDB_DEBUG
  snd_tone_type   index_tone;  /* Check Table in tdb_*_dtmf           */
#endif
  word            hi;          /* High Freq associated with DTMF      */
  word            lo;          /* Low Freq associated with DTMF       */
} tdb_dtmf_type;

/* TDB status return type */
typedef enum {
  TDB_SUCCESS,
  TDB_FAIL
} tdb_status_type;

/* <EJECT> */
/*===========================================================================
FUNCTION tdb_init

DESCRIPTION
  This routine initializes the tone database.

DEPENDENCIES
  tdb_voc_dtmf
  tdb_rng_dtmf

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdb_init( void );
/* <EJECT> */
/*===========================================================================
FUNCTION tdb_get_voc_tone_freq

DESCRIPTION
  This routine returns the vocoder/ADSP frequency values associated with
  the tone.

DEPENDENCIES
  tdb_voc_dtmf

RETURN VALUE
  DTMF pair.

SIDE EFFECTS
  None.
===========================================================================*/
extern tdb_status_type tdb_get_voc_tone_freq ( 
  snd_tone_type tone,      /* Tone index requested */
  tdb_dtmf_type *dtmf      /* DTMF returned        */
);
/* <EJECT> */
/*===========================================================================
FUNCTION tdb_get_ring_tone_freq

DESCRIPTION
  This routine returns the ringer frequency values associated with the tone.

DEPENDENCIES
  tdb_rng_dtmf

RETURN VALUE
  DTMF pair.

SIDE EFFECTS
  None.
===========================================================================*/
extern tdb_status_type tdb_get_ring_tone_freq (
  snd_tone_type tone,      /* Tone index requested */
  tdb_dtmf_type *dtmf      /* DTMF returned        */
);

#endif /* TDB_H */
