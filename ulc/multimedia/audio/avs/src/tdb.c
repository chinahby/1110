/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Sound Hardware Routines (2nd/3rd Generation)")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                T O N E S   D A T A B A S E   R O U T I N E S

GENERAL DESCRIPTION
  These routines provide the routines for the tones database interface.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  tdb_init
    Initializes the tone database.
  tdb_get_voc_tone_freq
    Returns the vocoder/ADSP frequency values associated with the tone.
  tdb_get_ring_tone_freq
    Returns the ringer frequency values associated with the tone.

Copyright(c) 1999, 2000 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/tdb.c_v   1.0   06 Oct 2001 12:58:30   ropalsky  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/tdb.c#4 $ $DateTime: 2009/06/02 23:17:14 $ $Author: bsingh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
02/28/01    st     Removed unnecessary featurization.
12/04/00    st     New dtmf values for SND_A,_B,_C,_D.  Removed unused tables.
01/27/00    st     Added frequency definitions for HFK tones.
09/09/99    st     Removed tdb_init_from_nv() function.
08/17/99    st     Created file.

===========================================================================*/
#include "tdb.h"
#include "tdbid.h"
#include "hw.h"
#ifdef TDB_DEBUG
#include "err.h"
#endif /* TDB_DEBUG */

/* <EJECT> */
/*__________________________________________________________________________*/
/************** V O C O D E R   &   A D S P   T A B L E *****************/
/************************************************************************/
/*                                                                      */
/* This array describes all the defined tones/dual-tones that can be    */
/* played by the Vocoder and ADSP.                                      */
/*                                                                      */
/* 1. It is indexed by tdb_tone_index_type which is also the first value*/
/*    in the structure.  This allows tdb_init() to make sure the enum   */
/*    and array are in sync with each other at startup.                 */
/*                                                                      */
/* 2. The High and Low Tones appear next.  The values shown are the     */
/*    frequencies in hertz associated with the tone.                    */
/*                                                                      */
/* Note: This array is used for both vocoder and ADSP DTMFs even though */
/*       the name tdb_voc_dtmf does not reflect this.                   */
/************************************************************************/

LOCAL const tdb_dtmf_type tdb_voc_dtmf[] = {
  /*        Key              High Freq Low Freq        */

  {DEF_DTMF((word)SND_0             , 1336  , 941  ) }, 
  {DEF_DTMF((word)SND_1             , 1209  , 697  ) }, 
  {DEF_DTMF((word)SND_2             , 1336  , 697  ) }, 
  {DEF_DTMF((word)SND_3             , 1477  , 697  ) }, 
  {DEF_DTMF((word)SND_4             , 1209  , 770  ) }, 
  {DEF_DTMF((word)SND_5             , 1336  , 770  ) }, 
  {DEF_DTMF((word)SND_6             , 1477  , 770  ) }, 
  {DEF_DTMF((word)SND_7             , 1209  , 852  ) }, 
  {DEF_DTMF((word)SND_8             , 1336  , 852  ) }, 
  {DEF_DTMF((word)SND_9             , 1477  , 852  ) }, 
  {DEF_DTMF((word)SND_A             , 1633  , 697  ) }, 
  {DEF_DTMF((word)SND_B             , 1633  , 770  ) }, 
  {DEF_DTMF((word)SND_C             , 1633  , 852  ) }, 
  {DEF_DTMF((word)SND_D             , 1633  , 941  ) }, 
  {DEF_DTMF((word)SND_POUND         , 1477  , 941  ) }, 
  {DEF_DTMF((word)SND_STAR          , 1209  , 941  ) }, 
  {DEF_DTMF((word)SND_CTRL          , 852   , 852  ) }, 
  {DEF_DTMF((word)SND_2ND           , 770   , 770  ) },
  {DEF_DTMF((word)SND_WARN          , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_ERR           , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_TIME          , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_RING_A        , 2600  , 2600 ) }, 
  {DEF_DTMF((word)SND_RING_B        , 2900  , 2900 ) }, 
  {DEF_DTMF((word)SND_RING_C        , 2750  , 2750 ) }, 
  {DEF_DTMF((word)SND_RING_D        , 2750  , 2750 ) },
  {DEF_DTMF((word)SND_RING_A4       , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_RING_AS4      , 466   , 466  ) }, 
  {DEF_DTMF((word)SND_RING_B4       , 494   , 494  ) }, 
  {DEF_DTMF((word)SND_RING_C4       , 523   , 523  ) }, 
  {DEF_DTMF((word)SND_RING_CS4      , 554   , 554  ) }, 
  {DEF_DTMF((word)SND_RING_D4       , 587   , 587  ) }, 
  {DEF_DTMF((word)SND_RING_DS4      , 622   , 622  ) }, 
  {DEF_DTMF((word)SND_RING_E4       , 659   , 659  ) }, 
  {DEF_DTMF((word)SND_RING_F4       , 698   , 698  ) }, 
  {DEF_DTMF((word)SND_RING_FS4      , 740   , 740  ) }, 
  {DEF_DTMF((word)SND_RING_G4       , 784   , 784  ) }, 
  {DEF_DTMF((word)SND_RING_GS4      , 831   , 831  ) }, 
  {DEF_DTMF((word)SND_RING_A5       , 880   , 880  ) }, 
  {DEF_DTMF((word)SND_RING_AS5      , 932   , 932  ) }, 
  {DEF_DTMF((word)SND_RING_B5       , 988   , 988  ) }, 
  {DEF_DTMF((word)SND_RING_C5       , 1047  , 1047 ) }, 
  {DEF_DTMF((word)SND_RING_CS5      , 1109  , 1109 ) }, 
  {DEF_DTMF((word)SND_RING_D5       , 1175  , 1175 ) }, 
  {DEF_DTMF((word)SND_RING_DS5      , 1244  , 1244 ) }, 
  {DEF_DTMF((word)SND_RING_E5       , 1318  , 1318 ) }, 
  {DEF_DTMF((word)SND_RING_F5       , 1397  , 1397 ) }, 
  {DEF_DTMF((word)SND_RING_FS5      , 1480  , 1480 ) }, 
  {DEF_DTMF((word)SND_RING_G5       , 1568  , 1568 ) }, 
  {DEF_DTMF((word)SND_RING_GS5      , 1661  , 1661 ) }, 
  {DEF_DTMF((word)SND_RING_A6       , 1760  , 1760 ) }, 
  {DEF_DTMF((word)SND_RING_AS6      , 1865  , 1865 ) }, 
  {DEF_DTMF((word)SND_RING_B6       , 1975  , 1975 ) }, 
  {DEF_DTMF((word)SND_RING_C6       , 2093  , 2093 ) }, 
  {DEF_DTMF((word)SND_RING_CS6      , 2217  , 2217 ) }, 
  {DEF_DTMF((word)SND_RING_D6       , 2349  , 2349 ) }, 
  {DEF_DTMF((word)SND_RING_DS6      , 2489  , 2489 ) }, 
  {DEF_DTMF((word)SND_RING_E6       , 2637  , 2637 ) }, 
  {DEF_DTMF((word)SND_RING_F6       , 2794  , 2794 ) }, 
  {DEF_DTMF((word)SND_RING_FS6      , 2960  , 2960 ) }, 
  {DEF_DTMF((word)SND_RING_G6       , 3136  , 3136 ) }, 
  {DEF_DTMF((word)SND_RING_GS6      , 3322  , 3322 ) }, 
  {DEF_DTMF((word)SND_RING_A7       , 3520  , 3520 ) }, 
  {DEF_DTMF((word)SND_RBACK         , 480   , 440  ) }, 
  {DEF_DTMF((word)SND_BUSY          , 620   , 480  ) }, 
  {DEF_DTMF((word)SND_INTERCEPT_A   , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_INTERCEPT_B   , 620   , 620  ) }, 
  {DEF_DTMF((word)SND_REORDER_TONE  , 620   , 480  ) }, 
  {DEF_DTMF((word)SND_PWRUP         , 620   , 620  ) }, 
  {DEF_DTMF((word)SND_OFF_HOOK_TONE , 2600  , 1400 ) }, 
  {DEF_DTMF((word)SND_CALL_WT_TONE  , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_DIAL_TONE_TONE, 440   , 350  ) }, 
  {DEF_DTMF((word)SND_ANSWER_TONE   , 1000  , 660  ) }, 
  {DEF_DTMF((word)SND_HIGH_PITCH_A  , 3700  , 3700 ) }, 
  {DEF_DTMF((word)SND_HIGH_PITCH_B  , 4000  , 4000 ) }, 
  {DEF_DTMF((word)SND_MED_PITCH_A   , 2600  , 2600 ) }, 
  {DEF_DTMF((word)SND_MED_PITCH_B   , 2900  , 2900 ) }, 
  {DEF_DTMF((word)SND_LOW_PITCH_A   , 1300  , 1300 ) }, 
  {DEF_DTMF((word)SND_LOW_PITCH_B   , 1450  , 1450 ) }, 
  {DEF_DTMF((word)SND_TEST_ON       , 1000  , 1000 ) }, 
  {DEF_DTMF((word)SND_MSG_WAITING   , 2000  , 2000 ) },
  {DEF_DTMF((word)SND_PIP_TONE_TONE , 480   , 480  ) }, 
  {DEF_DTMF((word)SND_SPC_DT_INDIA  , 400   , 300  ) }, 
  {DEF_DTMF((word)SND_SIGNAL_INDIA  , 400   , 400  ) }, 
  {DEF_DTMF((word)SND_DT_TONE_INDIA , 400   , 250  ) },
  {DEF_DTMF((word)SND_DT_TONE_BRAZIL, 425   , 425  ) },
  {DEF_DTMF((word)SND_DT_DTACO_TONE , 350   , 350  ) },
  {DEF_DTMF((word)SND_HFK_TONE1     , 1350  , 675  ) },
  {DEF_DTMF((word)SND_HFK_TONE2     , 1800  , 900  ) }
};
/* END - Vocoder Tables */                    
/* <EJECT> */
/*__________________________________________________________________________*/
/*********************** R I N G E R  T A B L E S ***************************/
/************************************************************************/
/*                                                                      */
/* This array describes all the defined tones/dual-tones that can be    */
/* Played by the Ringer.                                                */
/*                                                                      */
/* 1. It is indexed by tdb_tone_index_type which is also the first value*/
/*    in the structure.  This allows tdb_init() to make sure the enum   */
/*    and array are in sync with each other at startup.                 */
/*                                                                      */
/* 2. The High and Low frequencies appear next.  The values shown are   */
/*    actual frequencies associated with the tone.                      */
/*                                                                      */
/************************************************************************/

LOCAL const tdb_dtmf_type tdb_rng_dtmf[] = {
  /*        Key               High Freq Low Freq      */
  {DEF_DTMF((word)SND_0             , 1336  , 941  ) }, 
  {DEF_DTMF((word)SND_1             , 1209  , 697  ) }, 
  {DEF_DTMF((word)SND_2             , 1336  , 697  ) }, 
  {DEF_DTMF((word)SND_3             , 1477  , 697  ) }, 
  {DEF_DTMF((word)SND_4             , 1209  , 770  ) }, 
  {DEF_DTMF((word)SND_5             , 1336  , 770  ) }, 
  {DEF_DTMF((word)SND_6             , 1477  , 770  ) }, 
  {DEF_DTMF((word)SND_7             , 1209  , 852  ) }, 
  {DEF_DTMF((word)SND_8             , 1336  , 852  ) }, 
  {DEF_DTMF((word)SND_9             , 1477  , 852  ) }, 
  {DEF_DTMF((word)SND_A             , 1633  , 697  ) }, 
  {DEF_DTMF((word)SND_B             , 1633  , 770  ) }, 
  {DEF_DTMF((word)SND_C             , 1633  , 852  ) }, 
  {DEF_DTMF((word)SND_D             , 1633  , 941  ) }, 
  {DEF_DTMF((word)SND_POUND         , 1477  , 941  ) }, 
  {DEF_DTMF((word)SND_STAR          , 1209  , 941  ) }, 
  {DEF_DTMF((word)SND_CTRL          , 852   , 852  ) }, 
  {DEF_DTMF((word)SND_2ND           , 770   , 770  ) },
  {DEF_DTMF((word)SND_WARN          , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_ERR           , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_TIME          , 2000  , 2000 ) }, 
  {DEF_DTMF((word)SND_RING_A        , 2600  , 2600 ) }, 
  {DEF_DTMF((word)SND_RING_B        , 2900  , 2900 ) }, 
  {DEF_DTMF((word)SND_RING_C        , 2750  , 2750 ) }, 
  {DEF_DTMF((word)SND_RING_D        , 2750  , 2750 ) },
  {DEF_DTMF((word)SND_RING_A4       , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_RING_AS4      , 466   , 466  ) }, 
  {DEF_DTMF((word)SND_RING_B4       , 494   , 494  ) }, 
  {DEF_DTMF((word)SND_RING_C4       , 523   , 523  ) }, 
  {DEF_DTMF((word)SND_RING_CS4      , 554   , 554  ) }, 
  {DEF_DTMF((word)SND_RING_D4       , 587   , 587  ) }, 
  {DEF_DTMF((word)SND_RING_DS4      , 622   , 622  ) }, 
  {DEF_DTMF((word)SND_RING_E4       , 659   , 659  ) }, 
  {DEF_DTMF((word)SND_RING_F4       , 699   , 699  ) }, 
  {DEF_DTMF((word)SND_RING_FS4      , 740   , 740  ) }, 
  {DEF_DTMF((word)SND_RING_G4       , 784   , 784  ) }, 
  {DEF_DTMF((word)SND_RING_GS4      , 831   , 831  ) }, 
  {DEF_DTMF((word)SND_RING_A5       , 880   , 880  ) }, 
  {DEF_DTMF((word)SND_RING_AS5      , 932   , 932  ) }, 
  {DEF_DTMF((word)SND_RING_B5       , 988   , 988  ) }, 
  {DEF_DTMF((word)SND_RING_C5       , 1047  , 1047 ) }, 
  {DEF_DTMF((word)SND_RING_CS5      , 1109  , 1109 ) }, 
  {DEF_DTMF((word)SND_RING_D5       , 1175  , 1175 ) }, 
  {DEF_DTMF((word)SND_RING_DS5      , 1244  , 1244 ) }, 
  {DEF_DTMF((word)SND_RING_E5       , 1319  , 1319 ) }, 
  {DEF_DTMF((word)SND_RING_F5       , 1397  , 1397 ) }, 
  {DEF_DTMF((word)SND_RING_FS5      , 1480  , 1480 ) }, 
  {DEF_DTMF((word)SND_RING_G5       , 1568  , 1568 ) }, 
  {DEF_DTMF((word)SND_RING_GS5      , 1661  , 1661 ) }, 
  {DEF_DTMF((word)SND_RING_A6       , 1760  , 1760 ) }, 
  {DEF_DTMF((word)SND_RING_AS6      , 1865  , 1865 ) }, 
  {DEF_DTMF((word)SND_RING_B6       , 1976  , 1976 ) }, 
  {DEF_DTMF((word)SND_RING_C6       , 2093  , 2093 ) }, 
  {DEF_DTMF((word)SND_RING_CS6      , 2217  , 2217 ) }, 
  {DEF_DTMF((word)SND_RING_D6       , 2349  , 2349 ) }, 
  {DEF_DTMF((word)SND_RING_DS6      , 2489  , 2489 ) }, 
  {DEF_DTMF((word)SND_RING_E6       , 2637  , 2637 ) }, 
  {DEF_DTMF((word)SND_RING_F6       , 2794  , 2794 ) }, 
  {DEF_DTMF((word)SND_RING_FS6      , 2960  , 2960 ) }, 
  {DEF_DTMF((word)SND_RING_G6       , 3136  , 3136 ) }, 
  {DEF_DTMF((word)SND_RING_GS6      , 3322  , 3322 ) }, 
  {DEF_DTMF((word)SND_RING_A7       , 3520  , 3520 ) }, 
  {DEF_DTMF((word)SND_RBACK         , 480   , 440  ) }, 
  {DEF_DTMF((word)SND_BUSY          , 620   , 480  ) }, 
  {DEF_DTMF((word)SND_INTERCEPT_A   , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_INTERCEPT_B   , 620   , 620  ) }, 
  {DEF_DTMF((word)SND_REORDER_TONE  , 620   , 480  ) }, 
  {DEF_DTMF((word)SND_PWRUP         , 620   , 620  ) }, 
  {DEF_DTMF((word)SND_OFF_HOOK_TONE , 2600  , 1400 ) }, 
  {DEF_DTMF((word)SND_CALL_WT_TONE  , 440   , 440  ) }, 
  {DEF_DTMF((word)SND_DIAL_TONE_TONE, 440   , 350  ) }, 
  {DEF_DTMF((word)SND_ANSWER_TONE   , 1000  , 660  ) }, 
  {DEF_DTMF((word)SND_HIGH_PITCH_A  , 3900  , 3900 ) }, 
  {DEF_DTMF((word)SND_HIGH_PITCH_B  , 4350  , 4350 ) }, 
  {DEF_DTMF((word)SND_MED_PITCH_A   , 2600  , 2600 ) }, 
  {DEF_DTMF((word)SND_MED_PITCH_B   , 2900  , 2900 ) }, 
  {DEF_DTMF((word)SND_LOW_PITCH_A   , 1300  , 1300 ) }, 
  {DEF_DTMF((word)SND_LOW_PITCH_B   , 1450  , 1450 ) }, 
  {DEF_DTMF((word)SND_TEST_ON       , 1000  , 1000 ) }, 
  {DEF_DTMF((word)SND_MSG_WAITING   , 2000  , 2000 ) },
  {DEF_DTMF((word)SND_PIP_TONE_TONE , 480   , 480  ) }, 
  {DEF_DTMF((word)SND_SPC_DT_INDIA  , 400   , 300  ) }, 
  {DEF_DTMF((word)SND_SIGNAL_INDIA  , 400   , 400  ) }, 
  {DEF_DTMF((word)SND_DT_TONE_INDIA , 400   , 250  ) },
  {DEF_DTMF((word)SND_DT_TONE_BRAZIL, 425   , 425  ) },
  {DEF_DTMF((word)SND_DT_DTACO_TONE , 350   , 350  ) },
  {DEF_DTMF((word)SND_HFK_TONE1     , 1350  , 675  ) },
  {DEF_DTMF((word)SND_HFK_TONE2     , 1800  , 900  ) }
};
 
/* END - Ringer Tables */         
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void tdb_init() {
#ifdef TDB_DEBUG

  snd_tone_type       tone;

// >>> Vocoder initialization
  /* Check that all the enums in tdb_tone_index_type properly index their */
  /* entries in tone_voc_dtmf[].  Additionally, make sure that the high   */
  /* and low tones of the DTMF are not swapped.                           */

  for (tone=SND_FIRST_TONE + 1;tone < SND_LAST_TONE;tone++) {
    if (tdb_voc_dtmf[tone - SND_FIRST_TONE - 1].index_tone != tone) {
      ERR_FATAL("Tone Index Error in tdb_voc_dtmf[] Table",0,0,0);
    }
    if (tdb_voc_dtmf[tone - SND_FIRST_TONE - 1].hi <
        tdb_voc_dtmf[tone - SND_FIRST_TONE - 1].lo) {
      ERR_FATAL("High/Low Tone Error in tdb_voc_dtmf[] Table",0,0,0);
    }
  }

// >>> Ringer initialization
  /* Check that all the enums in tdb_tone_index_type properly index their */
  /* entries in tone_rng_dtmf[].  Additionally, make sure that the high   */
  /* and low tones of the DTMF are not swapped.                           */

  for (tone=SND_FIRST_TONE + 1;tone < SND_LAST_TONE;tone++) {
    if (tdb_rng_dtmf[tone - SND_FIRST_TONE - 1].index_tone != tone) {
      ERR_FATAL("Tone Index Error in tdb_rng_dtmf[] Table",0,0,0);
    }
    if (tdb_rng_dtmf[tone - SND_FIRST_TONE - 1].hi <
        tdb_rng_dtmf[tone - SND_FIRST_TONE - 1].lo) {
      ERR_FATAL("High/Low Tone Error in tdb_rng_dtmf[] Table",0,0,0);
    }
  }

#endif /* TDB_DEBUG */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
tdb_status_type tdb_get_voc_tone_freq
(
  snd_tone_type tone,         /* Tone index requested */
  tdb_dtmf_type *dtmf         /* DTMF returned        */
)
{
  if(tone > SND_FIRST_TONE && tone < SND_LAST_TONE) {
    *dtmf = tdb_voc_dtmf[tone - SND_FIRST_TONE - 1];
    return (TDB_SUCCESS);
  } else {
    return (TDB_FAIL);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
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
tdb_status_type tdb_get_ring_tone_freq
(
  snd_tone_type tone,         /* Tone index requested */
  tdb_dtmf_type *dtmf         /* DTMF returned        */
)
{
  if(tone > SND_FIRST_TONE && tone < SND_LAST_TONE) {
    *dtmf = tdb_rng_dtmf[tone - SND_FIRST_TONE - 1];
    return (TDB_SUCCESS);
  } else {
    return (TDB_FAIL);
  }
}
