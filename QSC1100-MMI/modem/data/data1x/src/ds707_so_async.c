/*===========================================================================

                         D S 7 0 7 _ A S Y N C _ S O

DESCRIPTION
  This file contains definitions of data structures, functions, etc
  related to configuring service options for IS-707 circuit switched data
  calls (async data).

EXTERNALIZED FUNCTIONS
  ds707_so_async_recal
    Re-does the async SO states, based on current settings.

  ds707_so_async_init
    Any power-up initialization that needs to be performed for
    async data service options should be done here.

  ds707_so_async_get_orig
    Returns the SO to originate with.

  ds707_so_async_so_is_async
     Returns TRUE if passed in SO is an async SO; else returns FALSE.

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_async.c_v   1.4   06 Nov 2002 14:40:16   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_so_async.c#5 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $
  
when       who    what, where, why
--------   ---    ----------------------------------------------------------
01/23/09   sn     Featurized code under FEATURE_ASYNC_DATA_NOOP.
06/17/08   ms     Compiler warning fix
02/07/08   sn     Featurized code related to FEATURE_IS2000_SCH and 
                  FEATURE_SNM_ENHANCEMENTS.
10/28/03   ak     Now update page response when recalculating SO info.
10/15/02   ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_1X
10/01/02   atp    Cleanup.
10/10/02   ak     Updated for JCDMA M51/1X by making so_list public and
                  moving #define of number async SO's into .h.
02/20/02   ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "cai.h"
#include "dec5000.h"
#include "dsat707vendctab.h"
#include "ds707.h"
#include "ds707_so_async.h"
#include "ds707_socm.h"
#include "err.h"
#include "snm.h"

#ifdef FEATURE_JCDMA_1X
#include "ds707_so_jcdma.h"
#endif

/*===========================================================================
                             TYPEDEFS
===========================================================================*/
typedef struct
{
  const snm_config_type   *default_cfg;        /* Default configuration    */
  const snm_neg_par_type  *neg_par;            /* Negotiation parameters   */
} ds707_so_async_param_type;

/*===========================================================================
                       ASYNC SERVICE OPTIONS
===========================================================================*/
const ds707_so_type ds707_so_async_list[] =
{
  CAI_SO_ASYNC_DATA_PRE707,             /* SO 4      = Pre-707, RS1        */
  CAI_SO_ASYNC_DATA_IS707,              /* SO 4100   = IS-707, RS1         */
  CAI_SO_ASYNC_DATA_13K_PRE707,         /* SO 12     = Pre-707, RS2        */
  CAI_SO_ASYNC_DATA_13K_PROPTRY         /* SO 0x8021 = QC Proprietary, RS2 */
};

/*---------------------------------------------------------------------------
   Table indicating if the SO is enabled/disabled under certain settings.
---------------------------------------------------------------------------*/
LOCAL const boolean ds707_so_async_tbl [DS_SO_SET_COUNT]
                                       [DS707_SO_ASYNC_NUM] =
{
/*                 4      4100    12      0x8021                           */
/* QCSO = 0 */    TRUE,   FALSE,  TRUE,   FALSE,
/* QCSO = 1 */    TRUE,   FALSE,  FALSE,  TRUE,
/* QCSO = 2 */    FALSE,  TRUE,   TRUE,   FALSE
};

/*---------------------------------------------------------------------------
  Async SO to originate with.
---------------------------------------------------------------------------*/
LOCAL const ds707_so_type ds707_so_async_orig_tbl [DS_SO_SET_COUNT]
                                                  [DS_RATESET_COUNT] =
{
  /*    RATE SET 1                         RATE SET 2                      */
  CAI_SO_ASYNC_DATA_PRE707,    CAI_SO_ASYNC_DATA_13K_PRE707,   /* QCSO = 0 */
  CAI_SO_ASYNC_DATA_PRE707,    CAI_SO_ASYNC_DATA_13K_PROPTRY,  /* QCSO = 1 */
  CAI_SO_ASYNC_DATA_IS707,     CAI_SO_ASYNC_DATA_13K_PRE707    /* QCSO = 2 */
};

/*---------------------------------------------------------------------------
  The Default Configuration and the Negotiation Parameters for each defined
  Async Data service option (from the async_so_list[]) are defined below.
  These data structures are used by the Service Negotiation Manager (SNM).
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Definitions for Service Option 4, 9.6 kbps Async Data (IS-99).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_async4_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
#ifdef FEATURE_JCDMA_1X
    CAI_RS1_ALL,                   /* All transmission rates used          */
#else
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
#endif /* FEATURE_JCDMA_1X */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
#ifdef FEATURE_JCDMA_1X
    CAI_RS1_ALL,                   /* All transmission rates used          */
#else
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
#endif /* FEATURE_JCDMA_1X */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_async4_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_ASYNC_DATA_13K_PRE707,       /* SO 12                           */
    CAI_SO_ASYNC_DATA_13K_PROPTRY,      /* SO 0x8021                       */
    CAI_SO_ASYNC_DATA_IS707,            /* SO 4100                         */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1X, RS = 1 (preferred)          */
      CAI_RC_4,                         /* 1X, RS = 1                      */
      CAI_RC_1                          /* IS-95, RS = 1                   */ 
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* 1X, RS = 1                      */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      (cai_radio_config_type)0
    }
  }
#ifdef FEATURE_IS2000_SCH
  ,{
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  }
#endif /* FEATURE_IS2000_SCH */
  ,{SNM_FCH, SNM_RESERVED, SNM_RESERVED} /* CH_IND allowed for this SO    */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_LOW_SPEED_ASYNC_DATA
  , CAI_SO_BITMAP_SO_ASYNC_DATA_PRE707
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Definitions for Service Option 4100, 9.6 kbps Async Data (IS-707-A.3).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_async4100_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_9600 | CAI_RS1_4800 | CAI_RS1_1200, /* Use Rate 1, 1/2 & 1/8   */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_async4100_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_ASYNC_DATA_13K_PRE707,       /* SO 12                           */
    CAI_SO_ASYNC_DATA_13K_PROPTRY,      /* SO 0x8021                       */
    CAI_SO_ASYNC_DATA_PRE707,           /* SO 4                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    3,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1X, RS = 1 (preferred)          */
      CAI_RC_4,                         /* 1X, RS = 1                      */
      CAI_RC_1                          /* IS-95, RS = 1                   */ 
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,                         /* 1X, RS = 1                      */
      CAI_RC_1,                         /* IS-95, RS = 1                   */ 
      (cai_radio_config_type)0
    }
  }
#ifdef FEATURE_IS2000_SCH
  ,{
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  }
#endif /* FEATURE_IS2000_SCH */
  ,{SNM_FCH, SNM_RESERVED, SNM_RESERVED} /* CH_IND allowed for this SO    */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_LOW_SPEED_ASYNC_DATA
  , CAI_SO_BITMAP_SO_ASYNC_DATA_IS707
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Definitions for Service Option 12, 14.4 kbps Async Data (IS-707-A.3).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_async12_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_async12_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_ASYNC_DATA_13K_PROPTRY,      /* SO 0x8021                       */
    CAI_SO_ASYNC_DATA_IS707,            /* SO 4100                         */
    CAI_SO_ASYNC_DATA_PRE707,           /* SO 4                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0
    }
  },
 {                                      /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0
    }
  }
#ifdef FEATURE_IS2000_SCH
  ,{
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  }
#endif /* FEATURE_IS2000_SCH */
  ,{SNM_FCH, SNM_RESERVED, SNM_RESERVED} /* CH_IND allowed for this SO    */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_LOW_SPEED_ASYNC_DATA
  , CAI_SO_BITMAP_SO_ASYNC_DATA_13K_PRE707
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Definitions for Service Option 0x8021, 14.4 kbps Async Data (Proprietary).
---------------------------------------------------------------------------*/
LOCAL const snm_config_type  ds707_so_async8021h_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All transmission rates used          */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

LOCAL const snm_neg_par_type  ds707_so_async8021h_neg_par = 
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_ASYNC_DATA_13K_PRE707,       /* SO 12                           */
    CAI_SO_ASYNC_DATA_IS707,            /* SO 4100                         */
    CAI_SO_ASYNC_DATA_PRE707,           /* SO 4                            */
    CAI_SO_NULL,
    0,
    0,
    0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, 14,400 bps             */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* 1X, RS = 2                      */
      CAI_RC_2,                         /* IS-95, RS = 2                   */ 
      (cai_radio_config_type)0
    }
  }
#ifdef FEATURE_IS2000_SCH
  ,{
    (byte)0,                              /* # of mux options for fwd sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for fwd sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  },
  {
    (byte)0,                              /* # of mux options for rev sch  */
    {
      (word)0, (word)0, (word)0, (word)0, /* preferences for rev sch mos   */
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0, (word)0,
      (word)0, (word)0, (word)0
    }
  }
#endif /* FEATURE_IS2000_SCH */
  ,{SNM_FCH, SNM_RESERVED, SNM_RESERVED} /* CH_IND allowed for this SO    */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
#endif /* FEATURE_IS2000_REL_A */
};


/*---------------------------------------------------------------------------
  Array to store the default config and negotiation parameter data structures
  for each async data service option (from the async_so_list[]).
  NOTE: The order and number of entries in this table must correspond to the
        order in which the service options are listed in the async_so_list[],
        and the number of entries in the async_so_list[], respectively.
---------------------------------------------------------------------------*/
LOCAL ds707_so_async_param_type ds707_so_async_param[DS707_SO_ASYNC_NUM] =
{
  {&ds707_so_async4_default_cfg,     &ds707_so_async4_neg_par},   /* 4     */ 
  {&ds707_so_async4100_default_cfg,  &ds707_so_async4100_neg_par},/* 4100  */ 
  {&ds707_so_async12_default_cfg,    &ds707_so_async12_neg_par},  /* 12    */
  {&ds707_so_async8021h_default_cfg, &ds707_so_async8021h_neg_par}/* 0x8021*/
};

/*===========================================================================
                       LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SO_ASYNCI_REGISTER

DESCRIPTION   Register each defined async data service option (from the 
              async_so_list[]) with the Service Negotiation Manager, and
              check that the registration was successful.

DEPENDENCIES  An instance of snm_config_type (default configuration) and
              snm_neg_par_type (negotiation parameters) should have been
              defined for each service option being registered, and should
              have been initialized with meaningful values prior to calling
              this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_asynci_register(void)
{
  byte  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Add each service option in the async_so_list[] to the SNM.
  -------------------------------------------------------------------------*/
  for( i = 0; i < DS707_SO_ASYNC_NUM; i++ )
  {
    if( snm_add_so((word)ds707_so_async_list[i],
                   (snm_neg_par_type*)(ds707_so_async_param[i].neg_par),
                   ds707_so_async_param[i].default_cfg) == SNM_SUCCESS )
    {
      /*---------------------------------------------------------------------
        The service option was added successfully, register the service
        option control message handler (the same handler is used for all
        packet data service options).
      ---------------------------------------------------------------------*/
      snm_set_cont_msg_handler((word)ds707_so_async_list[i],
                               ds707_socm_parse_socm);
    }
    else
    {
      /*---------------------------------------------------------------------
        The service option registration failed.
      ---------------------------------------------------------------------*/
      ERR_FATAL("Couldn't add SO %d to SNM", ds707_so_async_list[i], 0, 0);
    }
  }
} /* ds707_so_asynci_register () */

/*===========================================================================
FUNCTION      DS707_SO_ASYNCI_SET_STATE

DESCRIPTION   Lookup the state of each async data service option based on the
              current service option set in use. Enable or disable each async
              data service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_so_asynci_set_state(void)
{
  ds707_so_type  async_so;
  byte  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for( i = 0; i < DS707_SO_ASYNC_NUM; i++ )
  {
    async_so = ds707_so_async_list[i];

    if (ds707_so_async_tbl[dsat707_qcso_val][i] == TRUE)
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to enable this service option.
      ---------------------------------------------------------------------*/
      snm_enable_so((word)async_so);
    }
    else
    {
      /*---------------------------------------------------------------------
        Invoke the SNM function to disable this service option.
      ---------------------------------------------------------------------*/
      snm_disable_so((word)async_so);
    }
  } /* for i = 0 to DS707_SO_ASYNC_NUM */
} /* ds707_so_asynci_set_state() */



/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_ASYNC_RECAL
              
DESCRIPTION   Re-does the async SO states, based on current settings.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_async_recal(void)
{
#ifdef FEATURE_JCDMA_1X
  ds707_so_jcdma_async_set_state();
  ds707_so_jcdma_async_set_page_rsp();
#else
  ds707_so_asynci_set_state();
#endif /* FEATURE_JCDMA_1X */
}


/*===========================================================================
FUNCTION      DS707_SO_ASYNC_INIT
              
DESCRIPTION   Wrapper function for register_async_so() and 
              set_async_so_state() functions, in order to simplify the
              external interface. Any power-up initialization that needs to
              be performed for async data service options should be done here.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_async_init(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register all the defined async data service options with the service
    negotiation manager.
  -------------------------------------------------------------------------*/
  ds707_so_asynci_register();

  /*-------------------------------------------------------------------------
    Enable/disable the appropriate async data service options as per the
    current operating mode.
  -------------------------------------------------------------------------*/
  ds707_so_async_recal();
} /* ds707_so_async_init() */


/*===========================================================================
FUNCTION       DS707_SO_ASYNC_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_async_get_orig
(
  ds707_rateset_type  rateset
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_JCDMA_1X
  return(ds707_so_jcdma_async_get_orig(rateset));
#else
  return(ds707_so_async_orig_tbl[dsat707_qcso_val]
                                [rateset]);
#endif
} /* ds707_so_async_get_orig() */



/*===========================================================================
FUNCTION       DS707_SO_ASYNC_SO_IS_ASYNC

DESCRIPTION    Returns TRUE if passed in SO is an async SO.  Else returns
               FALSE.

DEPENDENCIES   NONE

RETURN VALUE   See DESCRIPTION

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_so_async_so_is_async
(
  ds707_so_type    so
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < DS707_SO_ASYNC_NUM; i++)
  {
    if (ds707_so_async_list[i] == so)
    {
      return(TRUE);
    }
  }

  return(FALSE);
} /* ds707_so_async_so_is_async() */
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */
