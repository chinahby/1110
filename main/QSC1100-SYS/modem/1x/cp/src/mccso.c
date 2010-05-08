/*===========================================================================

         S E R V I C E   O P T I O N S   D E F I N I T I O N S

GENERAL DESCRIPTION
  This module contains definitions of service options other than data.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccso.c_v   1.13   01 Oct 2002 16:49:36   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccso.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/08   bb      Code changes for Voice Call Continuity (VCC) feature
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
12/14/07   an      Added Passport Mode
04/24/07   an      Changed the min P_REV for 4GV from 1 to 6
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
12/12/06   pg      Fixed Lint error.
10/12/06   pg      Added F-TDSO(SO32) and Markov Loopback(SO55) to alt_so of
                   SO = 3 under FEATURE_JCDMA_2.
11/17/05   an      Changed 4GV_WB Bitmap Pos/SO Group
11/03/05   an      Changed 4GV SO value, moved EVRC above 4GV_WB in 4GV_NB alt
                   so list. Changed certain 4GV featurizations.
08/20/05   va      Allowed SO negotiation b/w test SOs
06/13/05   fc      Merged the following from MSMSHARED:
06/09/05   an       Made EVRC the first alt so for 4GV
05/23/05   fc      Merged the following from MSMSHARED:
05/24/05   an       Added 4GV_NB as an alt so to 4GV_WB and vice versa
04/27/05   an       Added support for 4GV service option
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
12/10/04   sb      Fixed lint warning.
08/09/04   fc      Service negotiation changes for Release C and D.
08/06/04   va      Removed voc_config from here as this is anyway done
                   in MUX
10/30/03   bkm     Added SO2 as alternate SO for EVRC to support legacy
                   JCDMA test equipment.
09/29/03   bkm     Added SO2 as alternate SO for IS-96A to support legacy
                   JCDMA test equipment.
08/13/03   sb      Added OTAPA and SMS on DCCH.
08/01/03   yll     Added support for Dynamic Feature.
07/30/03   az      Added support for FEATURE_SO_8026
10/01/02   az      Mainlined FEATURE_IS2000.
08/13/02   az      Changed the minimum PREV allowed to 1 for SMV. This is due to
                   a request from Lucent interop that SMV be able to originate
                   IS-95 BS
08/13/02   HQ      Added FEATURE_CDSMS.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
04/17/02   lh      Added voc_acquire() before voc_config().
03/09/02   lh      Merged from common archive.  
  02/20/02   az      Featurized 13K voice service option
10/01/01   kk      Merge from common archive.
                   08/16/01   sj      Registered the voice service options with 
                   connection_intialization() call back.
                   Defined mccso_so_voice_con_initializer() to configure
                   vocoder. 
08/10/01   kk      Increase mux option size.
08/09/01   kk      Merge from mainline.
                   08/02/01   jrp     Added SO54, SO55.  Expanded RC list from 3 elements to 5.
                   Removed dependence on dec.h by using cai_rate_set_type.
05/17/01   fc      Merged the following from common archive:
                   05/03/01   lcc     Correct SMS and OTAPA to use NULL traffic.
                   04/10/01   lcc     Changes to accomodate P2 mode.
11/20/00   lcc     Adding disabling of SMS service options if the feature is not
                   enabled.
11/02/00   lcc     Initial release.  Contains definitions for all non-data
                   service options.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "cai.h"
#include "snm.h"
#include "voc.h"

#include "err.h"

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif /* FEATURE MVS MIGRATE */

/*===========================================================================

                 NEGOTIATION PARAMETERS FOR SERVICE OPTIONS

===========================================================================*/

/*===========================================================================

                 TEST (MARKOV AND LOOPBACK) SERVICE OPTIONS

===========================================================================*/

// CAI_SO_MARKOV_SO54
const snm_neg_par_type mccso_so54_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV,
    CAI_SO_RS1_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_NULL,
    0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD} /* WILDCARDED Maximum fwd and rev mux options */
 ,{
    5,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1
    }
  },
  {                                     /* reverse radio channel           */
    4,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};



// CAI_SO_MARKOV
const snm_neg_par_type mccso_so8002h_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS1_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_MARKOV_SO54,
    CAI_SO_NULL,
    0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS1_MARKOV
const snm_neg_par_type mccso_so801eh_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_MARKOV_SO54,
    CAI_SO_NULL,
    0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }  
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_MARKOV_13K
const snm_neg_par_type mccso_so801ch_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_MARKOV,
    CAI_SO_RS1_MARKOV,
    CAI_SO_MARKOV,
    CAI_SO_MARKOV_SO54,
    CAI_SO_NULL,
    0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }  
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS2_MARKOV
const snm_neg_par_type mccso_so801fh_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV_13K,
    CAI_SO_RS1_MARKOV,
    CAI_SO_MARKOV,
    CAI_SO_MARKOV_SO54,
    CAI_SO_NULL,
    0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_LOOPBACK_SO55
const snm_neg_par_type mccso_so55_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK_13K,
    CAI_SO_LOOPBACK,
    CAI_SO_NULL,
    0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD} /* WILDCARDED Maximum fwd and rev mux options */
 ,{
    5,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1
    }
  },
  {                                     /* reverse radio channel           */
    4,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};



// CAI_SO_LOOPBACK
const snm_neg_par_type mccso_so2_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK_13K,
    CAI_SO_LOOPBACK_SO55,
    CAI_SO_NULL,
    0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_LOOPBACK_13K
const snm_neg_par_type mccso_so9_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK,
    CAI_SO_LOOPBACK_SO55,
    CAI_SO_NULL,
    0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }

  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */

  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

/*===========================================================================

                         VOICE SERVICE OPTIONS

===========================================================================*/
// CAI_SO_VOICE_13K
const snm_neg_par_type  mccso_so8000h_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    #ifdef FEATURE_VOC_4GV
    CAI_SO_VOICE_4GV_NB,  
    CAI_SO_VOICE_4GV_WB,
    #endif /* FEATURE_VOC_4GV */
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL
    #ifndef FEATURE_VOC_4GV
    ,0,0
    #endif /* FEATURE_VOC_4GV */
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  // Since there's no bitmap position assigned for 13K, it will
  // never be reported in the bitmap, do not set the group.
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_IS96A
const snm_neg_par_type  mccso_so1_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    #ifdef FEATURE_VOC_4GV
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_4GV_WB,
    #endif /* FEATURE_VOC_4GV */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    #ifdef FEATURE_JCDMA_TEST_SUPPORT
#error code not present
    #else   
    CAI_SO_NULL
    #endif
    #ifndef FEATURE_VOC_4GV
    ,CAI_SO_NULL,0
    #endif /* FEATURE_VOC_4GV */
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_IS96A
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_13K_IS733
const snm_neg_par_type  mccso_so17_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    #ifdef FEATURE_VOC_4GV
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_4GV_WB,
    #endif /* FEATURE_VOC_4GV */
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL
    #ifndef FEATURE_VOC_4GV
    ,0,0
    #endif /* FEATURE_VOC_4GV */
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  , { 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  , { 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_13K_IS733
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_EVRC
const snm_neg_par_type  mccso_so3_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
  #ifdef FEATURE_VOC_4GV
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_4GV_WB,
  #endif /* FEATURE_VOC_4GV */
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
  #ifdef FEATURE_PASSPORT_MODE
    CAI_SO_FULL_TDSO,
    CAI_SO_LOOPBACK_SO55,
  #endif /* FEATURE_PASSPORT_MODE */
  #ifdef FEATURE_JCDMA_TEST_SUPPORT
#error code not present
  #else
    CAI_SO_NULL
  #endif
  #ifndef FEATURE_VOC_4GV
    ,CAI_SO_NULL,0
  #endif /* FEATURE_VOC_4GV */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_EVRC 
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_SMV
const snm_neg_par_type  mccso_so56_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    #ifdef FEATURE_VOC_4GV
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_4GV_WB,                                  
    #endif /* FEATURE_VOC_4GV */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL
    #ifndef FEATURE_VOC_4GV
    ,0,0
    #endif /* FEATURE_VOC_4GV */
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_SMV 
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_4GV_NB
const snm_neg_par_type  mccso_so4gv_nb_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {
    CAI_SO_VOICE_EVRC,                  /* Alternate Service Options       */
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_13K,                   
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  { 
    CAI_SO_NULL,                        /* Compatible service options      */
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_4GV_NB
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_4GV_WB
const snm_neg_par_type  mccso_so4gv_wb_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {            
    CAI_SO_VOICE_4GV_NB,                /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_13K,                   
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  { 
    CAI_SO_NULL,                        /* Compatible service options      */
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */

  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED } 
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_4GV_WB
  #endif /* FEATURE_IS2000_REL_A */
};

/*===========================================================================

                         DATA BURST SERVICE OPTIONS

===========================================================================*/
// CAI_SO_RS2_SMS
const snm_neg_par_type  mccso_so14_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_SMS,
    CAI_SO_NULL,
    0,0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_SMS
  , CAI_SO_BITMAP_SO_RS2_SMS 
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_SMS
const snm_neg_par_type  mccso_so6_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_SMS,
    CAI_SO_NULL,
    0,0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_SMS
  , CAI_SO_BITMAP_SO_SMS 
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS2_OTAPA
const snm_neg_par_type  mccso_so19_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_RS1_OTAPA,
    CAI_SO_NULL,
    0,0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_OTAPA
  , CAI_SO_BITMAP_SO_RS2_OTAPA 
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS1_OTAPA
const snm_neg_par_type  mccso_so18_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_OTAPA,
    CAI_SO_NULL,
    0,0,0,0,0
    #ifdef FEATURE_PASSPORT_MODE
    ,0,0
    #endif /* FEATURE_PASSPORT_MODE */
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  #ifdef FEATURE_IS2000_SCH
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  #endif /* FEATURE_IS2000_SCH */
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_OTAPA
  , CAI_SO_BITMAP_SO_RS1_OTAPA 
  #endif /* FEATURE_IS2000_REL_A */
};

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */




/*===========================================================================

                 DEFAULT CONFIGURATIONS SERVICE OPTIONS

===========================================================================*/
// Default configuration shared by some RS 1 service options
const snm_config_type  mccso_rs1_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

// Default configuration shared by some RS 2 service options
const snm_config_type  mccso_rs2_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

// Default configuration shared by RS 1 signaling service options
const snm_config_type  mccso_rs1_sig_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  }
};

// Default configuration shared by RS 2 signaling service options
const snm_config_type  mccso_rs2_sig_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  }
};

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif /* FEATURE_MVS_MIGRATE */

/*===========================================================================

FUNCTION  mccso_so_voice_con_initializer

DESCRIPTION
  Called when a voice call is connected.  Goes ahead and configures up the
  vocoder capability.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mccso_so_voice_con_initializer
(
  const caii_srv_cfg_type*  cfg_info,
  word                      so_index
)
{

  /* Acquire vocoder if we haven't done so */
#ifdef FEATURE_MVS_MIGRATE
#error code not present
#else
  voc_acquire( VOC_ACQ_CDMA, NULL );
#endif

} /* mccso_so_voice_con_initializer */ /*lint !e715*/

/*===========================================================================

FUNCTION mccso_initialize

DESCRIPTION
  This function initializes all the service options defined in this file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccso_initialize( void )
{
  // Test service options
  (void)snm_add_so( CAI_SO_MARKOV_SO54, (snm_neg_par_type*)&mccso_so54_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_MARKOV, (snm_neg_par_type*)&mccso_so8002h_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_MARKOV_13K, (snm_neg_par_type*)&mccso_so801ch_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS1_MARKOV, (snm_neg_par_type*)&mccso_so801eh_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_MARKOV, (snm_neg_par_type*)&mccso_so801fh_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK_SO55, (snm_neg_par_type*)&mccso_so55_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK, (snm_neg_par_type*)&mccso_so2_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK_13K, (snm_neg_par_type*)&mccso_so9_neg_par, &mccso_rs2_so_default_cfg );

  // Data burst service options
  (void)snm_add_so( CAI_SO_SMS, (snm_neg_par_type*)&mccso_so6_neg_par, &mccso_rs1_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_SMS, (snm_neg_par_type*)&mccso_so14_neg_par, &mccso_rs2_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS1_OTAPA, (snm_neg_par_type*)&mccso_so18_neg_par, &mccso_rs1_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_OTAPA, (snm_neg_par_type*)&mccso_so19_neg_par, &mccso_rs2_sig_so_default_cfg );
#ifdef FEATURE_SO_8026
#error code not present
#endif

  // Voice Service Options
  (void)snm_add_so( CAI_SO_VOICE_13K, (snm_neg_par_type*)&mccso_so8000h_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_13K_IS733, (snm_neg_par_type*)&mccso_so17_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_IS96A, (snm_neg_par_type*)&mccso_so1_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_EVRC, (snm_neg_par_type*)&mccso_so3_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_SMV, (snm_neg_par_type*)&mccso_so56_neg_par, &mccso_rs1_so_default_cfg );

  (void)snm_add_so( CAI_SO_VOICE_4GV_NB, (snm_neg_par_type*)&mccso_so4gv_nb_neg_par, &mccso_rs1_so_default_cfg);
  (void)snm_add_so( CAI_SO_VOICE_4GV_WB, (snm_neg_par_type*)&mccso_so4gv_wb_neg_par, &mccso_rs1_so_default_cfg);

  /*  Register the connection_initializer call back function for the 
      CAI_SO_VOICE* service options to configure the vocoder on service 
      connect
  */
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_13K,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_13K_IS733,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_IS96A,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_EVRC,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_SMV,
                                         mccso_so_voice_con_initializer);
#ifdef FEATURE_VOC_4GV
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_4GV_NB,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_4GV_WB,
                                         mccso_so_voice_con_initializer);
#endif /* FEATURE_VOC_4GV */


  // MVS feature doesn't support half rate vocoder rates
  // (IS96A) so disable IS96A vocoder rate when MVS migrate
  // feature is enabled
#ifndef FEATURE_MVS_MIGRATE
  // Now, disable service options not support for this target
  // First voice service options
  if ( !voc_capability_supported(VOC_CAP_IS96A) )
#endif
  {
    (void)snm_disable_so( CAI_SO_VOICE_IS96A );
  }


  #ifdef FEATURE_EVRC
  #ifdef FEATURE_MVS_MIGRATE
#error code not present
  #else
  if ( !voc_capability_supported(VOC_CAP_IS127) )
  #endif /* FEATURE_MVS_MIGRATE */
  #endif /* FEATURE_EVRC */
  {
    (void)snm_disable_so( CAI_SO_VOICE_EVRC );
  }

 // MVS feature doesn't support SMV vocoder rate
 // so disable SMV vocoder rate when MVS migrate
 // feature is enabled
#ifndef FEATURE_MVS_MIGRATE
  #ifdef FEATURE_SO_VOICE_SMV
  if ( !voc_capability_supported(VOC_CAP_SMV) )
  #endif // FEATURE_SO_VOICE_SMV
#endif // FEATURE_MVS_MIGRATE
  {
    (void)snm_disable_so( CAI_SO_VOICE_SMV );
  }

  #ifdef FEATURE_VOC_4GV
  #ifdef FEATURE_MVS_MIGRATE
#error code not present
  #else
  if ( !voc_capability_supported(VOC_CAP_4GV_NB))
  #endif /* FEATURE_MVS_MIGRATE */
  #endif /* FEATURE_VOC_4GV */    
  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_NB );
  }
  #ifdef FEATURE_VOC_4GV
  #ifdef FEATURE_MVS_MIGRATE
#error code not present
  #else
  if ( !voc_capability_supported(VOC_CAP_4GV_WB))
  #endif /* FEATURE_MVS_MIGRATE */
  #endif /* FEATURE_VOC_4GV */      
  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_WB );
  }


#ifndef FEATURE_NO_13K_VOICE_SUPPORT
  #ifdef FEATURE_MVS_MIGRATE
#error code not present
  #else
  if ( !voc_capability_supported(VOC_CAP_IS733) )
  #endif /* FEATURE_MVS_MIGRATE */
#endif
  {

    (void)snm_disable_so( CAI_SO_VOICE_13K );
    (void)snm_disable_so( CAI_SO_VOICE_13K_IS733 );
  }

  // Now SMS service options
  #ifndef FEATURE_CDSMS
  (void)snm_disable_so( CAI_SO_SMS );
  (void)snm_disable_so( CAI_SO_RS2_SMS );
  #endif  // !FEATURE_CDSMS

} /* mccso_initialize */



