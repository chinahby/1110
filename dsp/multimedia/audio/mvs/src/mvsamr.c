/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                    A M R   F R A M I N G   P R O C E S S I N G

DESCRIPTION
  Functions that copy AMR vocoder frames to and from logical channel data.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (c) 2002 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvsamr.c#2 $ $DateTime: 2009/06/02 23:17:14 $ $Author: bsingh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
02/26/07   kan     Added AMR-WB changes 
03/18/06    ay     Cleaned up lint errors.
                   Updated Copyright.
10/19/05   ph      removed  include of amrsup.h as its done through mvsamr.h
07/22/05   ayu     Added AMR IF1 support.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
01/27/04   ymc     Removed LOCAL declarations from conversion tables.
           ymc     Moved MVS_AMR_TO_AMRSUP_FRAME() and MVS_AMR_TO_AMRSUP_MODE()
                   definitions to mvsamr.h.
           ymc     Updated Copyright.
09/16/03   ymc     Added mvs_amr_is_subset_chan_state.
06/29/03   ymc     Added mvs_amr_frame_len.
04/17/03    sm     Added amrsup support.
            sm     Moved mvs->voc, voc->mvs tables here, added
                   mvs_amr_audfmt_framing(), mvs_amr_audfmt_de_framing().
01/24/03   ymc     Updated copyright.
01/14/03    sm     Moved AMR reorg code & tables to mvsamr_iram.c. Include
                   "mvsamri.h".
12/04/02    sm     Added support for AMR IF2.
11/14/02    sm     If no A data, clear B & C data.
05/30/02   ymc     Ported from wvsamr.c.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "customer.h"
#include "comdef.h"
#include "msmaud.h"
#include "mvsamr.h"
#include "msg.h"

/* <EJECT> */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_AMR_WB_COMMON)

#if defined(MSMAUD_VOC_AMR_WB_COMMON)
mvs_amr_frame_type voc_to_mvs_awb_frame_table[VOC_AMR_WB_FRAME_MAX] =
{
  MVS_AMR_SPEECH_GOOD,     /* VOC_AMR_WB_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* VOC_AMR_WB_SPEECH_DEGRADED */
  MVS_AMR_SPEECH_LOST,     /* VOC_AMR_WB_SPEECH_LOST */
  MVS_AMR_SPEECH_BAD,      /* VOC_AMR_WB_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* VOC_AMR_WB_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* VOC_AMR_WB_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* VOC_AMR_WB_SID_BAD */
  MVS_AMR_NO_DATA,         /* VOC_AMR_WB_NO_DATA */
  MVS_AMR_ONSET            /* VOC_AMR_WB_ONSET */
};

voc_amr_wb_frame_type mvs_to_voc_awb_frame_table[MVS_AMR_FRAME_TYPE_MAX] = 
{
  VOC_AMR_WB_SPEECH_GOOD,       /* MVS_AMR_SPEECH_GOOD */
  VOC_AMR_WB_SPEECH_DEGRADED,   /* MVS_AMR_SPEECH_DEGRADED */
  VOC_AMR_WB_ONSET,             /* MVS_AMR_ONSET */
  VOC_AMR_WB_SPEECH_BAD,        /* MVS_AMR_SPEECH_BAD */
  VOC_AMR_WB_SID_FIRST,         /* MVS_AMR_SID_FIRST */
  VOC_AMR_WB_SID_UPDATE,        /* MVS_AMR_SID_UPDATE */
  VOC_AMR_WB_SID_BAD,           /* MVS_AMR_SID_BAD */
  VOC_AMR_WB_NO_DATA,           /* MVS_AMR_NO_DATA */
  VOC_AMR_WB_SPEECH_LOST        /* MVS_AMR_SPEECH_LOST */
};

mvs_amr_mode_type voc_to_mvs_awb_mode_table[VOC_AMR_WB_RATE_MAX] = 
{
  MVS_AMR_MODE_0660,      /* VOC_AMR_WB_RATE_0660 */
  MVS_AMR_MODE_0885,      /* VOC_AMR_WB_RATE_0885 */
  MVS_AMR_MODE_1265,      /* VOC_AMR_WB_RATE_1265 */
  MVS_AMR_MODE_1425,      /* VOC_AMR_WB_RATE_1425 */
  MVS_AMR_MODE_1585,      /* VOC_AMR_WB_RATE_1585 */
  MVS_AMR_MODE_1825,      /* VOC_AMR_WB_RATE_1825 */
  MVS_AMR_MODE_1985,      /* VOC_AMR_WB_RATE_1985 */
  MVS_AMR_MODE_2305,      /* VOC_AMR_WB_RATE_2305 */
  MVS_AMR_MODE_2385       /* VOC_AMR_WB_RATE_2385 */
};

voc_amr_wb_frame_rate_type mvs_to_voc_awb_mode_table[MVS_AMR_MODE_MAX] =
{
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0475 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0515 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0590 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0670 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0740 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_0795 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_1020 */
  VOC_AMR_WB_RATE_MAX,     /* MVS_AMR_MODE_1220 */
  VOC_AMR_WB_RATE_0660,    /* MVS_AMR_MODE_0660 */
  VOC_AMR_WB_RATE_0885,    /* MVS_AMR_MODE_0885 */
  VOC_AMR_WB_RATE_1265,    /* MVS_AMR_MODE_1265 */
  VOC_AMR_WB_RATE_1425,    /* MVS_AMR_MODE_1425 */
  VOC_AMR_WB_RATE_1585,    /* MVS_AMR_MODE_1585 */
  VOC_AMR_WB_RATE_1825,    /* MVS_AMR_MODE_1825 */
  VOC_AMR_WB_RATE_1985,    /* MVS_AMR_MODE_1985 */
  VOC_AMR_WB_RATE_2305,    /* MVS_AMR_MODE_2305 */
  VOC_AMR_WB_RATE_2385     /* MVS_AMR_MODE_2385 */
};
#endif /*MSMAUD_VOC_AMR_WB_COMMON*/

mvs_amr_frame_type voc_to_mvs_amr_frame_table[VOC_GSM_FRAME_MAX] =
{
  MVS_AMR_FRAME_TYPE_UNDEF,  /* VOC_GSM_SID */
  MVS_AMR_FRAME_TYPE_UNDEF,  /* VOC_GSM_SPEECH_GOOD */
  MVS_AMR_FRAME_TYPE_UNDEF,  /* VOC_GSM_BFI */
  MVS_AMR_FRAME_TYPE_UNDEF,  /* VOC_GSM_INVALID_SID */
  
  MVS_AMR_SPEECH_GOOD,     /* VOC_AMR_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* VOC_AMR_SPEECH_DEGRADED */
  MVS_AMR_ONSET,           /* VOC_AMR_ONSET */
  MVS_AMR_SPEECH_BAD,      /* VOC_AMR_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* VOC_AMR_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* VOC_AMR_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* VOC_AMR_SID_BAD */
  MVS_AMR_NO_DATA          /* VOC_AMR_NO_DATA */
};

voc_gsm_frame_type mvs_to_voc_amr_frame_table[MVS_AMR_FRAME_TYPE_MAX] =
{
  VOC_AMR_SPEECH_GOOD,     /* MVS_AMR_SPEECH_GOOD */
  VOC_AMR_SPEECH_DEGRADED, /* MVS_AMR_SPEECH_DEGRADED */
  VOC_AMR_ONSET,           /* MVS_AMR_ONSET */ 
  VOC_AMR_SPEECH_BAD,      /* MVS_AMR_SPEECH_BAD */
  VOC_AMR_SID_FIRST,       /* MVS_AMR_SID_FIRST */
  VOC_AMR_SID_UPDATE,      /* MVS_AMR_SID_UPDATE */
  VOC_AMR_SID_BAD,         /* MVS_AMR_SID_BAD */
  VOC_AMR_NO_DATA,         /* MVS_AMR_NO_DATA */
  VOC_GSM_FRAME_MAX        /* MVS_AMR_SPEECH_LOST */
};

mvs_amr_mode_type voc_to_mvs_amr_mode_table[VOC_GSM_RATE_MAX] =
{
  MVS_AMR_MODE_0475,       /* VOC_AMR_RATE_475 */
  MVS_AMR_MODE_0515,       /* VOC_AMR_RATE_515 */
  MVS_AMR_MODE_0590,       /* VOC_AMR_RATE_590 */
  MVS_AMR_MODE_0670,       /* VOC_AMR_RATE_670 */
  MVS_AMR_MODE_0740,       /* VOC_AMR_RATE_740 */
  MVS_AMR_MODE_0795,       /* VOC_AMR_RATE_795 */
  MVS_AMR_MODE_1020,       /* VOC_AMR_RATE_1020 */
  MVS_AMR_MODE_1220,       /* VOC_AMR_RATE_1220 */
  MVS_AMR_MODE_UNDEF,      /* VOC_GSM_FULL_RATE */
  MVS_AMR_MODE_UNDEF       /* VOC_GSM_HALF_RATE */
};

voc_gsm_frame_rate_type mvs_to_voc_amr_mode_table[MVS_AMR_MODE_MAX] =
{
  VOC_AMR_RATE_475,        /* MVS_AMR_MODE_0475 */
  VOC_AMR_RATE_515,        /* MVS_AMR_MODE_0515 */
  VOC_AMR_RATE_590,        /* MVS_AMR_MODE_0590 */
  VOC_AMR_RATE_670,        /* MVS_AMR_MODE_0670 */
  VOC_AMR_RATE_740,        /* MVS_AMR_MODE_0740 */
  VOC_AMR_RATE_795,        /* MVS_AMR_MODE_0795 */
  VOC_AMR_RATE_1020,       /* MVS_AMR_MODE_1020 */
  VOC_AMR_RATE_1220,       /* MVS_AMR_MODE_1220 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_0660 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_0885 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_1265 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_1425 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_1585 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_1825 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_1985 */
  VOC_GSM_RATE_MAX,        /* MVS_AMR_MODE_2305 */
  VOC_GSM_RATE_MAX         /* MVS_AMR_MODE_2385 */
};


/* Conversion tables & macros from MVS_AMR to AMRSUP & vice versa
*/
mvs_amr_frame_type amrsup_to_mvs_amr_frame_table[AMRSUP_FRAME_TYPE_MAX] =
{
  MVS_AMR_SPEECH_GOOD,     /* AMRSUP_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* AMRSUP_SPEECH_DEGRADED */
  MVS_AMR_ONSET,           /* AMRSUP_ONSET */
  MVS_AMR_SPEECH_BAD,      /* AMRSUP_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* AMRSUP_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* AMRSUP_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* AMRSUP_SID_BAD */
  MVS_AMR_NO_DATA,         /* AMRSUP_NO_DATA */
  MVS_AMR_SPEECH_LOST      /* AMRSUP_SPEECH_LOST */
};

amrsup_frame_type mvs_amr_to_amrsup_frame_table[MVS_AMR_FRAME_TYPE_MAX] =
{
  AMRSUP_SPEECH_GOOD,     /* MVS_AMR_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED, /* MVS_AMR_SPEECH_DEGRADED */
  AMRSUP_ONSET,           /* MVS_AMR_ONSET */ 
  AMRSUP_SPEECH_BAD,      /* MVS_AMR_SPEECH_BAD */
  AMRSUP_SID_FIRST,       /* MVS_AMR_SID_FIRST */
  AMRSUP_SID_UPDATE,      /* MVS_AMR_SID_UPDATE */
  AMRSUP_SID_BAD,         /* MVS_AMR_SID_BAD */
  AMRSUP_NO_DATA,         /* MVS_AMR_NO_DATA */
  AMRSUP_SPEECH_LOST      /* MVS_AMR_SPEECH_LOST */
};

mvs_amr_mode_type amrsup_to_mvs_amr_mode_table[AMRSUP_MODE_MAX] =
{
  MVS_AMR_MODE_0475,       /* AMRSUP_MODE_0475 */
  MVS_AMR_MODE_0515,       /* AMRSUP_MODE_0515 */
  MVS_AMR_MODE_0590,       /* AMRSUP_MODE_0590 */
  MVS_AMR_MODE_0670,       /* AMRSUP_MODE_0670 */
  MVS_AMR_MODE_0740,       /* AMRSUP_MODE_0740 */
  MVS_AMR_MODE_0795,       /* AMRSUP_MODE_0795 */
  MVS_AMR_MODE_1020,       /* AMRSUP_MODE_1020 */
  MVS_AMR_MODE_1220,       /* AMRSUP_MODE_1220 */
  MVS_AMR_MODE_0660,       /* AMRSUP_MODE_0660 */
  MVS_AMR_MODE_0885,       /* AMRSUP_MODE_0885 */
  MVS_AMR_MODE_1265,       /* AMRSUP_MODE_1265 */
  MVS_AMR_MODE_1425,       /* AMRSUP_MODE_1425 */
  MVS_AMR_MODE_1585,       /* AMRSUP_MODE_1585 */
  MVS_AMR_MODE_1825,       /* AMRSUP_MODE_1825 */
  MVS_AMR_MODE_1985,       /* AMRSUP_MODE_1985 */
  MVS_AMR_MODE_2305,       /* AMRSUP_MODE_2305 */
  MVS_AMR_MODE_2385        /* AMRSUP_MODE_2385 */
};

amrsup_mode_type mvs_amr_to_amrsup_mode_table[MVS_AMR_MODE_MAX] =
{
  AMRSUP_MODE_0475,        /* MVS_AMR_MODE_0475 */
  AMRSUP_MODE_0515,        /* MVS_AMR_MODE_0515 */
  AMRSUP_MODE_0590,        /* MVS_AMR_MODE_0590 */
  AMRSUP_MODE_0670,        /* MVS_AMR_MODE_0670 */
  AMRSUP_MODE_0740,        /* MVS_AMR_MODE_0740 */
  AMRSUP_MODE_0795,        /* MVS_AMR_MODE_0795 */
  AMRSUP_MODE_1020,        /* MVS_AMR_MODE_1020 */
  AMRSUP_MODE_1220,        /* MVS_AMR_MODE_1220 */
  AMRSUP_MODE_0660,        /* MVS_AMR_MODE_0660 */
  AMRSUP_MODE_0885,        /* MVS_AMR_MODE_0885 */
  AMRSUP_MODE_1265,        /* MVS_AMR_MODE_1265 */
  AMRSUP_MODE_1425,        /* MVS_AMR_MODE_1425 */
  AMRSUP_MODE_1585,        /* MVS_AMR_MODE_1585 */
  AMRSUP_MODE_1825,        /* MVS_AMR_MODE_1825 */
  AMRSUP_MODE_1985,        /* MVS_AMR_MODE_1985 */
  AMRSUP_MODE_2305,        /* MVS_AMR_MODE_2305 */
  AMRSUP_MODE_2385         /* MVS_AMR_MODE_2385 */
};

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING

/* expected good frame w.r.t next state */
mvs_amr_frame_type mvs_amr_state_good_frame_table[MVS_AMR_STATE_MAX] = 
{
  MVS_AMR_SPEECH_GOOD,     /* MVS_AMR_STATE_SPEECH  */
  MVS_AMR_SID_FIRST,       /* MVS_AMR_STATE_SID_F   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_1 */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_2 */
  MVS_AMR_SID_UPDATE,      /* MVS_AMR_STATE_SID_U   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_1   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_2   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_3   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_4   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_5   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_6   */
  MVS_AMR_NO_DATA          /* MVS_AMR_STATE_DTX_7   */
};

/* expected bad frame (CRC fails or multiple CRCs pass) w.r.t. next state */
mvs_amr_frame_type mvs_amr_state_bad_frame_table[MVS_AMR_STATE_MAX] = 
{
  MVS_AMR_SPEECH_BAD,      /* MVS_AMR_STATE_SPEECH  */
  MVS_AMR_SID_BAD,         /* MVS_AMR_STATE_SID_F   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_1 */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_2 */
  MVS_AMR_SID_BAD,         /* MVS_AMR_STATE_SID_U   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_1   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_2   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_3   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_4   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_5   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_6   */
  MVS_AMR_NO_DATA          /* MVS_AMR_STATE_DTX_7   */
};

/* CRC status type w.r.t. AMR frame type */
mvs_amr_crc_status_type mvs_amr_frame_crc_table[MVS_AMR_FRAME_TYPE_MAX] = 
{
  MVS_AMR_CRC_SPEECH_ONLY, /* MVS_AMR_SPEECH_GOOD     */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SPEECH_DEGRADED */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_ONSET           */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SPEECH_BAD      */
  MVS_AMR_CRC_SID_F_ONLY,  /* MVS_AMR_SID_FIRST       */
  MVS_AMR_CRC_SID_U_ONLY,  /* MVS_AMR_SID_UPDATE      */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SID_BAD         */
  MVS_AMR_CRC_DTX_ONLY,    /* MVS_AMR_NO_DATA         */
  MVS_AMR_CRC_SPEECH_ONLY  /* MVS_AMR_SPEECH_LOST     */
};

#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

mvs_amr_frame_type_index_type
  amrsup_to_mvs_amr_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX] =
{
  MVS_AMR_FRAME_TYPE_INDEX_0475,
  MVS_AMR_FRAME_TYPE_INDEX_0515,
  MVS_AMR_FRAME_TYPE_INDEX_0590,
  MVS_AMR_FRAME_TYPE_INDEX_0670,
  MVS_AMR_FRAME_TYPE_INDEX_0740,
  MVS_AMR_FRAME_TYPE_INDEX_0795,
  MVS_AMR_FRAME_TYPE_INDEX_1020,
  MVS_AMR_FRAME_TYPE_INDEX_1220,
  MVS_AMR_FRAME_TYPE_INDEX_AMR_SID,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_NO_DATA
};

amrsup_frame_type_index_type
  mvs_amr_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX] =
{
  AMRSUP_FRAME_TYPE_INDEX_0475,
  AMRSUP_FRAME_TYPE_INDEX_0515,
  AMRSUP_FRAME_TYPE_INDEX_0590,
  AMRSUP_FRAME_TYPE_INDEX_0670,
  AMRSUP_FRAME_TYPE_INDEX_0740,
  AMRSUP_FRAME_TYPE_INDEX_0795,
  AMRSUP_FRAME_TYPE_INDEX_1020,
  AMRSUP_FRAME_TYPE_INDEX_1220,
  AMRSUP_FRAME_TYPE_INDEX_AMR_SID,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA
};

#if defined(MSMAUD_VOC_AMR_WB_COMMON)
mvs_amr_frame_type_index_type
  amrsup_to_mvs_awb_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX] =
{
  MVS_AMR_FRAME_TYPE_INDEX_0660,
  MVS_AMR_FRAME_TYPE_INDEX_0885,
  MVS_AMR_FRAME_TYPE_INDEX_1265,
  MVS_AMR_FRAME_TYPE_INDEX_1425,
  MVS_AMR_FRAME_TYPE_INDEX_1585,
  MVS_AMR_FRAME_TYPE_INDEX_1825,
  MVS_AMR_FRAME_TYPE_INDEX_1985,
  MVS_AMR_FRAME_TYPE_INDEX_2305,
  MVS_AMR_FRAME_TYPE_INDEX_2385,
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SID,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SPL,
  MVS_AMR_FRAME_TYPE_INDEX_NO_DATA
};

amrsup_frame_type_index_type
  mvs_awb_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX] =
{
  AMRSUP_FRAME_TYPE_INDEX_0660,
  AMRSUP_FRAME_TYPE_INDEX_0885,
  AMRSUP_FRAME_TYPE_INDEX_1265,
  AMRSUP_FRAME_TYPE_INDEX_1425,
  AMRSUP_FRAME_TYPE_INDEX_1585,
  AMRSUP_FRAME_TYPE_INDEX_1825,
  AMRSUP_FRAME_TYPE_INDEX_1985,
  AMRSUP_FRAME_TYPE_INDEX_2305,
  AMRSUP_FRAME_TYPE_INDEX_2385,
  AMRSUP_FRAME_TYPE_INDEX_AWB_SID,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_AWB_SPL,
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA
};
#endif /*MSMAUD_VOC_AMR_WB_COMMON*/
#endif  /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_AMR_WB_COMMON*/


#ifdef FEATURE_MVS
#if (defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_AMR_WB_COMMON)) \
    && defined(FEATURE_WCDMA)
#error code not present
#endif  /* (MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_AMR_WB_COMMON) && FEATURE_WCDMA */


/* <EJECT> */
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_AMR_WB_COMMON)
/*===========================================================================

FUNCTION mvs_amr_class_A_bits

DESCRIPTION
  This function will determine number of bits of AMR class A data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class A bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
int mvs_amr_class_A_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type), 
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode), 
                                 AMRSUP_CLASS_A);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_class_B_bits

DESCRIPTION
  This function will determine number of bits of AMR class B data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class B bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
int mvs_amr_class_B_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type), 
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode), 
                                 AMRSUP_CLASS_B);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_class_C_bits

DESCRIPTION
  This function will determine number of bits of AMR class C data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class C bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
int mvs_amr_class_C_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type), 
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode), 
                                 AMRSUP_CLASS_C);
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_frame_len

DESCRIPTION
  This function will determine number of bytes of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
int mvs_amr_frame_len(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_len(MVS_AMR_TO_AMRSUP_FRAME(frame_type), 
                          MVS_AMR_TO_AMRSUP_MODE(amr_mode));
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_frame_len_bits

DESCRIPTION
  This function will determine number of bits of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
int mvs_amr_frame_len_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_len_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type), 
                               MVS_AMR_TO_AMRSUP_MODE(amr_mode));
}


/* <EJECT> */
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
/*===========================================================================

FUNCTION mvs_amr_state_roll

DESCRIPTION
  This function runs a state machine to predict the next possible AMR frame
based on the previous frames.  Next state could be peeked without update.
Undefined CRC status with update option is used to reset the state machine.
Undefined CRC status without update option indicates no state changes,
predicted frame will be based on good crc status in this case.

DEPENDENCIES
  None.

RETURN VALUE
  Next AMR frame type predicted.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
mvs_amr_frame_type mvs_amr_state_roll(
  mvs_amr_crc_status_type crc_status,       /* CRC status of next frame */
  boolean update                            /* update state or not */
)
{
  static mvs_amr_state_type state = MVS_AMR_STATE_UNDEF;
  mvs_amr_state_type next_state = MVS_AMR_STATE_UNDEF;
  mvs_amr_frame_type ret_frame = MVS_AMR_FRAME_TYPE_UNDEF;


  if (crc_status==MVS_AMR_CRC_UNDEF)
  {
    /* reset the state machine */
    if (update==TRUE)
    {
      state = MVS_AMR_STATE_UNDEF;
      MSG_MED("Reset AMR state machine", 0, 0, 0);
      return MVS_AMR_FRAME_TYPE_UNDEF;
    }


    /* no state change */
    if (state==MVS_AMR_STATE_UNDEF)
    {
      ret_frame = MVS_AMR_FRAME_TYPE_UNDEF;
    }
    else
    {
      ret_frame = mvs_amr_state_good_frame_table[state];
    }

    MSG_MED("Current AMR state %d %d", state, ret_frame, 0);
    return ret_frame;
  }


  switch (crc_status)
  {
    case MVS_AMR_CRC_SPEECH_ONLY :
      next_state = MVS_AMR_STATE_SPEECH;
      break;

    case MVS_AMR_CRC_SID_F_ONLY :
      next_state = MVS_AMR_STATE_SID_F;

      ret_frame = mvs_amr_state_roll(MVS_AMR_CRC_SID_ONLY, FALSE);
      if (ret_frame!=MVS_AMR_SID_FIRST)
      {
        MSG_HIGH("Incompatible SID_FIRST prediction %d", ret_frame, 0, 0);
      }

      break;

    case MVS_AMR_CRC_SID_U_ONLY :
      next_state = MVS_AMR_STATE_SID_U;

      ret_frame = mvs_amr_state_roll(MVS_AMR_CRC_SID_ONLY, FALSE);
      if (ret_frame!=MVS_AMR_SID_UPDATE)
      {
        MSG_HIGH("Incompatible SID_UPDATE prediction %d", ret_frame, 0, 0);
      }

      break;

    case MVS_AMR_CRC_SID_ONLY :
      switch (state)
      {
        case MVS_AMR_STATE_UNDEF :
        case MVS_AMR_STATE_SPEECH :
          next_state = MVS_AMR_STATE_SID_F;
          break;

        case MVS_AMR_STATE_F_DTX_2 :
        case MVS_AMR_STATE_DTX_7 :
          next_state = MVS_AMR_STATE_SID_U;
          break;

        case MVS_AMR_STATE_DTX_2 :
          next_state = MVS_AMR_STATE_SID_U;
          MSG_HIGH("Incompatible old state %d", state, 0, 0);
          break;

        default :
          next_state = MVS_AMR_STATE_SID_F;
          MSG_HIGH("Incompatible old state %d", state, 0, 0);
          break;
      }
      
      break;
        
    case MVS_AMR_CRC_DTX_ONLY :
      switch (state)
      {
        case MVS_AMR_STATE_F_DTX_2 :
          next_state = MVS_AMR_STATE_DTX_3;
          MSG_HIGH("Incompatible old state %d", state, 0, 0);
          break;

        case MVS_AMR_STATE_SPEECH :
        case MVS_AMR_STATE_DTX_7 :
          MSG_HIGH("Incompatible old state %d", state, 0, 0);
          /*lint -fallthrough */
        case MVS_AMR_STATE_UNDEF :
          next_state = MVS_AMR_STATE_F_DTX_1;
          break;
 
        default :
          next_state = ((mvs_amr_state_type) ((int) state + 1));
          break;
      }

      break;

    case MVS_AMR_CRC_MULTIPLE :
    case MVS_AMR_CRC_FAIL :
      switch (state)
      {
        case MVS_AMR_STATE_UNDEF :
        case MVS_AMR_STATE_SPEECH :
          next_state = MVS_AMR_STATE_SPEECH;
          break;
        
        case MVS_AMR_STATE_DTX_7 :
          next_state = MVS_AMR_STATE_SID_U;
          break;

        default :
          next_state = ((mvs_amr_state_type) ((int) state + 1));
          break;
      }

      break;

    default :
      MSG_ERROR("Unknown CRC status %d", crc_status, 0, 0);
      return MVS_AMR_FRAME_TYPE_UNDEF;
  }      
    
  
  if ((crc_status==MVS_AMR_CRC_MULTIPLE) || (crc_status==MVS_AMR_CRC_FAIL))
  {
    ret_frame = mvs_amr_state_bad_frame_table[next_state];
  }
  else
  {
    ret_frame = mvs_amr_state_good_frame_table[next_state];
  }


  MSG_MED("AMR state old %d new %d crc %d", state, next_state, crc_status);

  if (update==TRUE)
  {
    state = next_state;
  }


  return ret_frame;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* <EJECT> */
#ifdef FEATURE_WCDMA
#error code not present
#endif  /* FEATURE_WCDMA */

#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
#endif  /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_AMR_WB_COMMON */
#endif  /* FEATURE_MVS */


