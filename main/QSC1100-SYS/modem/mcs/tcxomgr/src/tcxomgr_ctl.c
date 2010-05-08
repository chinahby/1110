/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                   T C X O    M A N A G E R   C O N T R O L

                           TCXO Manager Source File

GENERAL DESCRIPTION

  This module maintains the routines for client control of the vctcxo.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2009 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/src/tcxomgr_ctl.c#16 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
05/01/09     ns      Feature wrap set_polarity changes
04/21/09     ns      Merging TRK_LO_ADJ polarity changes
03/26/09     ag      Using rex crit sect in tcxomgr_get/set_restriction only 
                     for Q6.
03/20/09     ag      Replaced rex crit sect calls with REX_ISR_* macros in 
                     tcxomgr_get/set_restriction. 
10/20/08     adw     Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/24/08     adw     Modified CLIENT_FREQ_EST featurization to be based on 
                     XO_TASK as well as MCS_HAS_NO_GPS.
09/15/08     sg      Fix lint/klocwork warnings
09/15/08     sst     Featurize CLIENT_FREQ_EST under XO_TASK
09/09/08     sg      Don't switch source for PDM clock when FTM_MODE to avoid
                       upsetting RF calibration
08/28/08     adw     Added FEATURE_XO_CTL_1X_ONLY and FEATURE_XO_DIAG to 
                     featurize for ULC.
08/12/08     sg      Don't turn off PDM clock when in FTM_MODE to avoid 
                       upsetting GSM RF Calibration
06/20/08     mca     Merge from //depot
06/04/08     sg      Added TCXOMGR_SOURCE_POWER_OFF source adn refactored out 
                       sleep mode to use this source instead
04/10/08     sg      Notify registered clients of current restriction at 
                     callback registration time
03/21/08     sg      enable/disable pdm clock regime in sync with tcxomgr
03/13/08     nnv     Added TCXOMGR_UBM_L1 for tcxomgr priority string array.
11/05/07     cab     Add in processing to notify freq_est of restr changes
10/26/07     cab     Keep restriction set to XO on XO targets
10/02/07     va      Added FREQ_EST client. Removed slope_est stuff.
09/28/07     cab     Set rpush flag before notifying
09/19/07     va      Save rot client state for slope estimation
09/12/07     sst     Added UMB L1 to client list
09/06/07     sst     Lint fixes
09/04/07     cab     Write field cal data on every field cal iteration
06/18/07     aps     Read in fac cal values tcxomgr_init()
06/18/07     va      Support for XO Compensation
06/12/07     cab     Added back in fac cal changes
06/12/07     cab     Reverted fac cal, added rpush_get_flag
06/06/07     sst     Moved xo_trim read/write and processing to _data
05/17/07     cab     Added uncertainty for passive clients
05/16/07     cab     Updated rpush_set_flag to allow multiple client request
05/07/07     cab     Added rotator callback, get_current_rot
03/20/07     cab     Featurize reference to 1x srch header file
03/12/07     cab     Change time value in rpush to signed, include adc.h.
03/02/07     nnv     Modified tcxomgr_rpush and rpush_cb functions for rpush update
03/02/07     nnv     Added code to support XO targets
01/19/07     cab     Added support for xo_trim init and write back at pwr down
11/08/06     cab     Fixed critical sections to prevent dog error
11/03/06     cab     Added rotator pushing functionality
                     Added client TV OUT
10/19/06     cab     Added PPM client
10/12/06     cab     Added priority strings for WCDMA and UMTS
10/02/06     cab     Initial version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "customer.h"

#include "adc.h"
#include "msg.h"
#include "msm_help.h"
#include "msmhwioreg.h"
#include "rex.h"
#include "rf.h"
#include "ftm.h"
#include "err.h"

#include "tcxomgr.h"
#include "tcxomgr_i.h"

#ifdef FEATURE_TCXOMGR_DIAG
#include "tcxomgr_diag.h"
#endif /* FEATURE_TCXOMGR_DIAG */

#ifdef FEATURE_SRCH_CLKREGIM_RM
#include "clkregim.h"
#include "clkrgm_rm.h"
#endif /* FEATURE_SRCH_CLKREGIM_RM */

#ifdef FEATURE_XO
#ifdef FEATURE_CDMA1X
#include "srch_mdsp.h"
#endif
#include "tcxomgr_xo_trim.h"
#endif /* FEATURE_XO */

/* Using rex critical section macros in tcxomgr_get/set_restriction only if
   the target is Q6.*/
#ifdef FEATURE_TCXOMGR_Q6 
#error code not present
#else 
  #define TCXOMGR_ENTER_CRIT_SECT(lock) INTLOCK(); 
  #define TCXOMGR_LEAVE_CRIT_SECT(lock) INTFREE();
#endif 

/*============================================================================

                           HARDWARE I/O MACROS

============================================================================*/


/*===========================================================================

MACRO           TRK_LO_ADJ_ENABLE_OUTPUT
MACRO           TRK_LO_ADJ_DISABLE_OUTPUT

DESCRIPTION     These macros enable and disable (high-impedance) the
                TRK_LO_ADJ output pin.

DEPENDENCIES    None.
RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define TRK_LO_ADJ_ENABLE_OUTPUT( ) \
  MSM_SET( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_OE )

#define TRK_LO_ADJ_DISABLE_OUTPUT( ) \
  MSM_CLEAR( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_OE )
                
/*===========================================================================

MACRO           TRK_LO_ADJ_SET_NORMAL_POLARITY

DESCRIPTION     This macro controls the polarity of the TRK_LO_ADJUST pin.
                With normal polarity, a positive frequency error will
                decrease the density of high pulse on the pin.

DEPENDENCIES    None.
RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/

#ifdef FEATURE_INVERTED_TRK_LO_POLARITY

  #define TRK_LO_ADJ_SET_NORMAL_POLARITY( ) \
    MSM_SET( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL )
  #define TRK_LO_ADJ_NORMAL_POLARITY 1

#else /* !FEATURE_INVERTED_TRK_LO_POLARITY */

  #define TRK_LO_ADJ_SET_NORMAL_POLARITY( ) \
    MSM_CLEAR( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL )
  #define TRK_LO_ADJ_NORMAL_POLARITY 0

#endif /* FEATURE_INVERTED_TRK_LO_POLARITY */

/*===========================================================================

MACRO           TRK_LO_ADJ_SEL_SOURCE_OVERRIDE
MACRO           TRK_LO_ADJ_SEL_SOURCE_MDSP

DESCRIPTION     TRK_LO_ADJ_SEL - This bit selects the source of the 16-bit 
                input to the TRK_LO_ADJ PDM.
                
                If cleared (0), the input to the TRK_LO_ADJ_PDM is provided by
                the ARM through bits 15:0 of the DEM1X_FREQUENCY_CTL /
                AGC_CARRIER_FREQ_ERR.
                If set (1), the input to the TRK_LO_ADJ PDM is provided by
                the mDSP through the COMB_TRK_LO_ADJ_WR register.

DEPENDENCIES    None.
RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/

#define TRK_LO_ADJ_SEL_SOURCE_OVERRIDE( ) \
  MSM_CLEAR( AGC_FREQ_COMB_CTL, CARRIER_FREQ_TRK_ORIDE_N )

#define TRK_LO_ADJ_SEL_SOURCE_MDSP( ) \
  MSM_SET( AGC_FREQ_COMB_CTL, CARRIER_FREQ_TRK_ORIDE_N )

/*===========================================================================

MACRO           TRK_LO_ADJ_SEL_SOURCE_CDMA
MACRO           TRK_LO_ADJ_SEL_SOURCE_UMTS

DESCRIPTION     TRK_LO_ADJ_SEL - This bit selects the source of the 16-bit 
                input to the TRK_LO_ADJ PDM.
                
                If cleared (0), the input to the TRK_LO_ADJ_PDM is provided by
                the ARM through bits 15:0 of the DEM1X_FREQUENCY_CTL /
                AGC_CARRIER_FREQ_ERR.
                If set (1), the input to the TRK_LO_ADJ PDM is provided by
                the mDSP through the COMB_TRK_LO_ADJ_WR register.

DEPENDENCIES    None.
RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_MSM7600
#error code not present
#else

  #define TRK_LO_ADJ_SEL_SOURCE_CDMA( )

  #define TRK_LO_ADJ_SEL_SOURCE_UMTS( )
  
#endif

/*===========================================================================

MACRO           TRK_LO_ADJ_SET_OVERRIDE_VAL

DESCRIPTION     The TRK_LO_ADJ bitfield is a micro register that can override 
                the AGC TRK_LO_PDM input. 
                
DEPENDENCIES    The TRK_LO_ADJ_SEL bit must be clear (0) to enable the override.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/

#define TRK_LO_ADJ_SET_OVERRIDE_VAL( val ) \
   if (HWIO_INF(AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL) == TRK_LO_ADJ_NORMAL_POLARITY) \
   {                                                            \
     MSM_W_MASKED( AGC_CARRIER_FREQ_ERR, FREQ_ERR, (val));      \
   }                                                            \
   else                                                         \
   {                                                            \
     MSM_W_MASKED( AGC_CARRIER_FREQ_ERR, FREQ_ERR, (-(val)));   \
   }


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*---------------------------------------------------------------------------
  TCXO Manager local variables 
---------------------------------------------------------------------------*/

tcxomgr_client_type clients[TCXOMGR_NUM_CLIENTS] =
{
  /* Client TCXOMGR_CLIENT_NONE */
  {
    TCXOMGR_CLIENT_NONE,         /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "NONE",                      /* Name */
    TCXOMGR_SOURCE_OVERRIDE,     /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  #ifndef FEATURE_XO_CTL_1X_ONLY
  /* Client TCXOMGR_CLIENT_GPS */
  {
    TCXOMGR_CLIENT_GPS,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "GPS",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_BT */
  {
    TCXOMGR_CLIENT_BT,           /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "BLUETOOTH",                 /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_MEDIA_FLO */
  {
    TCXOMGR_CLIENT_MEDIA_FLO,    /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "MEDIA FLO",                 /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_GSM */
  {
    TCXOMGR_CLIENT_GSM,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "GSM",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_WCDMA */
  {
    TCXOMGR_CLIENT_WCDMA,        /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "WCDMA",                     /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },
  #endif /* FEATURE_XO_CTL_1X_ONLY */

  /* Client TCXOMGR_CLIENT_CDMA_1X */
  {
    TCXOMGR_CLIENT_CDMA_1X,      /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "1X",                        /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  #ifndef FEATURE_XO_CTL_1X_ONLY
  /* Client TCXOMGR_CLIENT_CDMA_HDR */
  {
    TCXOMGR_CLIENT_CDMA_HDR,     /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "HDR",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_PPM */
  {
    TCXOMGR_CLIENT_PPM,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "PPM",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_TV_OUT */
  {
    TCXOMGR_CLIENT_TV_OUT,       /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "TV OUT",                    /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_UBM_L1 */
  {
    TCXOMGR_CLIENT_UBM_L1,       /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "UBM L1",                    /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },
  #endif /* FEATURE_XO_CTL_1X_ONLY */

  /* REVISIT: CLIENT_FREQ_EST should really only be featurized by XO_TASK,
       however, to prevent compiler error in gps code we must ensure that
       this field is included in the enum.  The gps file mc_fee.c initializes 
       a 'static const' structure and requires the CLIENT_FREQ_EST field to be
       present.  Gps code does not distinguish library for XO vs. TCXO so for
       now we must featurize based on XO_TASK as well as MCS_HAS_NO_GPS. */
  #if( defined( FEATURE_XO_TASK ) || !defined( FEATURE_MCS_HAS_NO_GPS ) )
  /* Client TCXOMGR_CLIENT_FREQ_EST */
  {
    TCXOMGR_CLIENT_FREQ_EST,     /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "FREQ EST",                  /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  }
  #endif /* FEATURE_XO_TASK && !FEATURE_MCS_HAS_NO_GPS */
};


/* Debug Strings */
char * source_strs[TCXOMGR_MAX_SOURCE] =
{
  "TCXOMGR_SOURCE_NONE",
  "TCXOMGR_SOURCE_OVERRIDE",
  "TCXOMGR_SOURCE_1X_DSP",
  #ifndef FEATURE_XO_CTL_1X_ONLY
  "TCXOMGR_SOURCE_HDR_DSP",
  "TCXOMGR_SOURCE_UMTS_DSP",
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  "TCXOMGR_SOURCE_POWER_OFF"
};

char * restriction_strs[TCXOMGR_MAX_RESTRICTION] =
{
  "TCXOMGR_RESTRICTION_NONE",
  #ifndef FEATURE_XO_CTL_1X_ONLY
  "TCXOMGR_RESTRICTION_FAST",
  "TCXOMGR_RESTRICTION_SLOW",
  "TCXOMGR_RESTRICTION_SRL",
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  "TCXOMGR_RESTRICTION_FROZEN",
  "TCXOMGR_RESTRICTION_XO"
};

char * priority_strs[TCXOMGR_MAX_PRIORITY] =
{
  "TCXOMGR_NO_PRIORITY",
  "TCXOMGR_DEFAULT_PRIORITY",
  #ifndef FEATURE_XO_CTL_1X_ONLY
  "TCXOMGR_UBM_L1",
  "TCXOMGR_1X_OL",
  "TCXOMGR_HDR_OL",
  "TCXOMGR_1X_CL",
  "TCXOMGR_HDR_CL",
  "TCXOMGR_GSM",
  "TCXOMGR_WCDMA",
  #else /* !FEATURE_XO_CTL_1X_ONLY */
  "TCXOMGR_1X_OL",
  "TCXOMGR_1X_CL",
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  "TCXOMGR_HIGHEST_PRIORITY"
};

/* The global storage object for tcxomgr */
tcxomgr_struct_type tcxomgr = { TCXOMGR_UNINITIALIZED };

/* Keep sections of code safe from task switching */
static rex_crit_sect_type tcxomgr_crit_sect;


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

#ifdef FEATURE_TCXOMGR_SET_POLARITY
/*============================================================================

FUNCTION TCXOMGR_SET_POLARITY

DESCRIPTION
  This function sets the polarity as requested and also sets the PDM value.
  The function should be called by the technology team (1x/HDR/WCDMA/GSM/....)
  to set the polarity required by it. It also changes the PDM value to reflect
  the new polarity. If its an XO build, it just returns success.
 
DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_trk_lo_return_enum_type which specifies if the polarity change was 
  successful, could not change the polarity or an invalid polarity was passed. 

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_trk_lo_return_enum_type tcxomgr_set_polarity
( 
  tcxomgr_trk_lo_enum_type    polarity
)
{
  tcxomgr_trk_lo_return_enum_type tcxomgr_state_flag;
  tcxomgr_trk_lo_enum_type cur_polarity;
  tcxomgr_state_flag = TCXOMGR_TRK_LO_POLARITY_SUCCESS;

#ifndef FEATURE_XO 

  /* Check if the polarity is valid */
  if( polarity != TCXOMGR_TRK_LO_POLARITY_NORMAL
      && polarity != TCXOMGR_TRK_LO_POLARITY_INVERTED )
  {
     DBG_STR1( ERROR, "Polarity invalid: %d", polarity );
     return TCXOMGR_TRK_LO_POLARITY_INVALID;
  }

  MSM_LOCK();

  /* Make sure the polarity state is consistent */
  cur_polarity = 
    ( tcxomgr_trk_lo_enum_type ) HWIO_INF(AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL);
  if ( tcxomgr.polarity != cur_polarity )
  {
    /* Tcxomgr is in a inconsistent state */
     DBG_STR2( ERROR, "Tcxomgr Polarity: %d Actual polarity: %d ", 
               tcxomgr.polarity, cur_polarity );
     /* Restore the tcxomgr state */
     tcxomgr.polarity = cur_polarity;
  }

  /* Change the polarity if required */
  if ( tcxomgr.polarity != polarity )
  {
    /* Set the requested polarity */
    if ( polarity == TCXOMGR_TRK_LO_POLARITY_NORMAL )
    {
      /* Set the polarity to Normal polarity */
      MSM_CLEAR( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL );
    } 
    else /* Error check makes sure inverted is the only other priority */
    {
      /* Set the polarity to Inverted polarity */
      MSM_SET( AGC_FREQ_COMB_CTL, TRK_LO_ADJ_POL );
    }
    /* Update the current state of polarity */
    tcxomgr.polarity = polarity;
  }

  /* Is tcxomgr in control */
  if( tcxomgr.state == TCXOMGR_IDLE )
  {
    /* Set source to override register */
    TRK_LO_ADJ_SEL_SOURCE_OVERRIDE( );
        
    /* Set the PDM value to the override val */
    TRK_LO_ADJ_SET_OVERRIDE_VAL(tcxomgr.override_val);
    /* Return value is success, the default value */
  }
  else
  {
    /* Tcxomgr not in charge, return status */
    DBG_STR1( HIGH, "Tcxomgr expected to be IDLE state, current state: %d",
              tcxomgr.state );
    tcxomgr_state_flag = TCXOMGR_TRK_LO_PDM_VALUE_UNCHANGED;
  }
  MSM_UNLOCK();

#endif  /* FEATURE_XO */

  return tcxomgr_state_flag;

} /* tcxomgr_set_polarity */
#endif /* FEATURE_TCXOMGR_SET_POLARITY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_VCO

DESCRIPTION
  This function gets the VCO value from the active client controlling 
  VCO by calling its registered callback function.  If no one is controlling
  the txcomgr override value is returned.

DEPENDENCIES
  None

RETURN VALUE
  VCO value from the active client or the frozen value if no active.

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_vco_type tcxomgr_get_current_vco( void )
{
  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control", tcxomgr.client_in_control, 0, 0 );
  }

  if ( tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE )
  {
    /* TBD:  does it matter if we are sleeping or not */
    return tcxomgr.override_val;
  }
  else
  {
    /* Callback function to the active client to update the log packet */
    return clients[tcxomgr.client_in_control].vco_cb();
  }

} /* tcxomgr_get_current_vco */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_ROT

DESCRIPTION
  This function gets the ROT value from the active client by calling its 
  registered callback function.  If no one is controlling the rgs value is 
  returned.

DEPENDENCIES
  None

RETURN VALUE
  ROT value from the active client or the rgs ROT value if no active.

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_rotator_type tcxomgr_get_current_rot( void )
{
  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control", tcxomgr.client_in_control, 0, 0 );
  }

  if ( tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE )
  {
    /* TBD:  does it matter if we are sleeping or not */
    return tcxomgr.rgs.rot;
  }
  else
  {
    /* Callback function to the active client to get the rot val */
    return clients[tcxomgr.client_in_control].rot_cb();
  }

} /* tcxomgr_get_current_rot */
#endif /* FEATURE_XO_CTL_1X_ONLY */


/*============================================================================

FUNCTION TCXOMGR_WRITE_MUX

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/
static void tcxomgr_write_mux
( 
  tcxomgr_source_type    source
)
{
  MSM_LOCK();

  if ( source == TCXOMGR_SOURCE_POWER_OFF )
  {
    if ( source != tcxomgr.vco_source ) 
    {
      DBG_0(MED, "TCXOMGR powering down...");

      /* This is a hack.  GSM TX Cal (done in FTM mode) tells GSM to
       * release the tcxo and then loads a new firmware image which
       * changes the source to the override register.  If the override
       * register is not correctly programmed or the PDM is turned
       * off, the TX Cal process fails.  This check keeps the tcxo pdm
       * on and controlled by the override register in FTM mode to
       * keep TX Cal working.
       *
       * But the correct thing to do would be to have GSM retain
       * control of the tcxo and set it in the appropriate state for
       * calibration.  This doesn't fit the current model of
       * GSM/Calibration interaction, though and would be too
       * difficult to add. */
      if ( ftm_get_mode() != FTM_MODE )
      {
#ifndef FEATURE_XO
        TRK_LO_ADJ_DISABLE_OUTPUT();
#endif  /* FEATURE_XO */

#ifdef FEATURE_SRCH_CLKREGIM_RM
        clk_regime_resource_disable( CLKRGM_CLIENT_TCXOMGR, 
                                     CLKRGM_RESOURCE_TCXO_PDM );
#endif /* FEATURE_SRCH_CLKREGIM_RM */
      }
      else
      {
        DBG_0(MED, "...But we're in FTM mode");
      #ifndef FEATURE_XO
        /* Write the desired VCO value */
        TRK_LO_ADJ_SET_OVERRIDE_VAL( tcxomgr.override_val );
        TRK_LO_ADJ_ENABLE_OUTPUT();
      #endif /* FEATURE_XO */

      #ifdef FEATURE_SRCH_CLKREGIM_RM
        clk_regime_resource_enable( CLKRGM_CLIENT_TCXOMGR, 
                                    CLKRGM_RESOURCE_TCXO_PDM );
      #endif /* FEATURE_SRCH_CLKREGIM_RM */

      }
    }
  }
  else 
  {
    if ( tcxomgr.vco_source == TCXOMGR_SOURCE_POWER_OFF ) 
    {
      tcxomgr_vco_info_type rgs_vco;

/*------------------------------------------------------------------------*/

      DBG_0(MED, "TCXOMGR powering up...");

      #ifdef FEATURE_SRCH_CLKREGIM_RM
      clk_regime_resource_enable( CLKRGM_CLIENT_TCXOMGR, 
                                  CLKRGM_RESOURCE_TCXO_PDM );
      #endif /* FEATURE_SRCH_CLKREGIM_RM */

      /* Freeze the vco with the best guess value */
      rgs_vco = tcxomgr_get_stored_vco();
      tcxomgr.override_val = rgs_vco.vco_value;

      #ifndef FEATURE_XO
      /* Write the desired VCO value */
      TRK_LO_ADJ_SET_OVERRIDE_VAL( tcxomgr.override_val );
      /* Set the pad to normal impedence and set the polarity */
      TRK_LO_ADJ_ENABLE_OUTPUT();
      #endif /* FEATURE_XO */
    }

  if ( source == TCXOMGR_SOURCE_OVERRIDE ) 
  {
    TRK_LO_ADJ_SEL_SOURCE_OVERRIDE();
    TRK_LO_ADJ_SEL_SOURCE_CDMA();
  }
  else if ( source == TCXOMGR_SOURCE_1X_DSP )
  {
    TRK_LO_ADJ_SEL_SOURCE_MDSP();
    TRK_LO_ADJ_SEL_SOURCE_CDMA();
  }
  #ifndef FEATURE_XO_CTL_1X_ONLY
  else if ( source == TCXOMGR_SOURCE_HDR_DSP )
  {
    TRK_LO_ADJ_SEL_SOURCE_MDSP();
    TRK_LO_ADJ_SEL_SOURCE_CDMA();
  }
  else if ( source == TCXOMGR_SOURCE_UMTS_DSP )
  {
    TRK_LO_ADJ_SEL_SOURCE_MDSP();
    TRK_LO_ADJ_SEL_SOURCE_UMTS();
  }
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  else
  {
      DBG_STR1( ERROR, "Request to set mux to invalid source %d", source ); 
  }
}
  tcxomgr.vco_source = source;
  MSM_UNLOCK();

} /* tcxomgr_write_mux */


/*============================================================================

FUNCTION TCXOMGR_SEND_NOTIFICATIONS

DESCRIPTION
  This function sends a notification to all clients that have registered a
  notification callback (assuming the notification level has changed).

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
static void tcxomgr_send_notifications
( 
  tcxomgr_restriction_type  restriction
)
{
  uint8 i;
  /*------------------------------------------------------------------------*/

  if ( restriction != tcxomgr.working_restr )
  {
    /* Send notification to all clients who've signed up (safe to skip NONE) */
    for (i = TCXOMGR_CLIENT_NONE + 1; i < TCXOMGR_NUM_CLIENTS; i++)
    {
      /* If client has registered to receive notification of this restr level */

      if (clients[i].notify_cb != NULL)
      {
        DBG_STR2( MED, "Client %s notification:  %s", clients[i].name, 
                  restriction_strs[restriction] );
        tcxomgr.cb_id = (tcxomgr_client_id_type)i;
        (clients[i].notify_cb)( restriction );
      }
    }

    tcxomgr.working_restr = restriction;
  }

} /* tcxomgr_send_notifications */


/*============================================================================

FUNCTION TCXOMGR_GET_RESTRICTION

DESCRIPTION

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
tcxomgr_restriction_type tcxomgr_get_restriction
( 
  tcxomgr_client_id_type    id
)
{

  #ifndef FEATURE_XO
  uint8 i;
  #endif
  tcxomgr_restriction_type restriction = TCXOMGR_RESTRICTION_NONE;  

/*--------------------------------------------------------------------------*/

  /* Start of critical section */
  /* Using ISR macro as tcxomgr_get_restriction is being called from an
     interrupt context */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  #ifdef FEATURE_XO
  /* If it is XO target, restriction is always TCXOMGR_RESTRICTION_XO */
  restriction = TCXOMGR_RESTRICTION_XO;
  #else
  for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++) 
  {
    if ( clients[i].restriction > restriction )
    {
        /* Return the highest restriction (excluding passed client) */
        if ( i != id ) 
        {
          restriction = clients[i].restriction;
        }
    }
  }
  #endif /* FEATURE_XO */

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return restriction;

}


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_GET_WORKING_RESTRICTION

DESCRIPTION

  Return the current working restriction of the TCXO manager.

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
static tcxomgr_restriction_type tcxomgr_get_working_restriction( void )
{
  return tcxomgr.working_restr;
} /* tcxomgr_get_working_restriction */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_FIND_BEST_CLIENT

DESCRIPTION
  This function searches the list for the client with the highest service
  level.

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
static tcxomgr_client_id_type tcxomgr_find_best_client( void )
{

  uint8 i;
  tcxomgr_client_id_type best_client = TCXOMGR_CLIENT_NONE;
  tcxomgr_priority_type temp_priority = TCXOMGR_NO_PRIORITY;

/*--------------------------------------------------------------------------*/

  /* Search for those that want control, and find the one with highest pri */
  for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++) 
  {
    if ( (clients[i].source != TCXOMGR_SOURCE_NONE) && 
         (clients[i].priority > temp_priority) )
    {
        best_client   = (tcxomgr_client_id_type)i;
        temp_priority = clients[i].priority;
    }
  }

  /* If there are multiple at the same priority, give precedence to
     client in control */
  if ( (tcxomgr.client_in_control < TCXOMGR_NUM_CLIENTS) &&
       (clients[best_client].priority <= 
         clients[tcxomgr.client_in_control].priority)  )
  {
    best_client = tcxomgr.client_in_control;
  }

  return best_client;

} /* find_best_client */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_PROCESS_CHANGES

DESCRIPTION
  This function process the current list state and sends cmd to new
  client if necessary

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  Possibly a new best active client, and a command will be sent.

============================================================================*/
static void tcxomgr_process_changes( void )
{

  tcxomgr_restriction_type restriction;
  uint16 i;

/*--------------------------------------------------------------------------*/
  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control", tcxomgr.client_in_control, 0, 0 );
  }

  tcxomgr.best_client = tcxomgr_find_best_client();

  /* Currently have a controlling client, but a new client has pre-empted */
  if ( (tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE) &&
       (tcxomgr.client_in_control != tcxomgr.best_client) ) 
  {
    if ( clients[tcxomgr.client_in_control].release_cb != NULL )
    {
      /* Set state to rel pending so other service reqs will be put on hold */
      tcxomgr.state = TCXOMGR_REPLY_PENDING;

      /* Record id of who should be acking us shortly */
      tcxomgr.ack_client = tcxomgr.client_in_control;

      DBG_STR1( MED, "Client %s release",
               clients[tcxomgr.client_in_control].name );

      /* Call the client's release callback function */
      tcxomgr.cb_id = tcxomgr.client_in_control;
      (clients[tcxomgr.client_in_control].release_cb) ();
    }
    else
    {
      DBG_STR1( ERROR, "Client %s release callback is NULL",
                clients[tcxomgr.client_in_control].name );
    }
  }

  /* Currently no controlling client, but one has requested control */
  else if ( (tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE) &&
            (tcxomgr.best_client != TCXOMGR_CLIENT_NONE) )
  {
    if ( clients[tcxomgr.best_client].grant_cb != NULL )
    {
      /* Set state to ack so other service reqs will be put on hold */
      tcxomgr.state = TCXOMGR_REPLY_PENDING;

      /* Record id of who should be acking us shortly */
      tcxomgr.ack_client = tcxomgr.best_client;

      /* Obtain highest restriction, excluding the best client */
      restriction = tcxomgr_get_restriction( tcxomgr.best_client );

      tcxomgr.curr_restr = restriction;

      DBG_STR3( MED, "Client %s grant:  %s, %s",
                clients[tcxomgr.best_client ].name, 
                source_strs[clients[tcxomgr.best_client].source],
                restriction_strs[restriction] );

      /* Call the client's grant callback function to send command */
      tcxomgr.cb_id = tcxomgr.best_client;
      (clients[tcxomgr.best_client].grant_cb)
        ( clients[tcxomgr.best_client].source, restriction, 
          tcxomgr.override_val );

      clients[tcxomgr.best_client].resp_req = FALSE;
    }
    else
    {
      DBG_STR1( ERROR, "Client %s grant callback is NULL",
                clients[tcxomgr.client_in_control].name );
    }
  }

  /* A client has changed restr level while another client is in control? */
  else if ( tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE )
  {
    /* Obtain highest restriction, excluding the controlling client */
    restriction = tcxomgr_get_restriction( tcxomgr.client_in_control ); 
    if ( restriction != tcxomgr.curr_restr )
    {
      tcxomgr.curr_restr = restriction;
      if ( clients[tcxomgr.best_client].change_cb != NULL )
      {
        /* Set state to ack so other service reqs will be put on hold */
        tcxomgr.state = TCXOMGR_REPLY_PENDING;
  
        /* Record id of who should be acking us shortly */
        tcxomgr.ack_client = tcxomgr.client_in_control;
  
        DBG_STR3( MED, "Client %s change:  %s, %s",
                  clients[tcxomgr.client_in_control ].name, 
                  source_strs[clients[tcxomgr.client_in_control].source],
                  restriction_strs[restriction] );
  
        /* Call the client's change callback function */
        tcxomgr.cb_id = tcxomgr.client_in_control;
        (clients[tcxomgr.client_in_control].change_cb)
          ( clients[tcxomgr.client_in_control].source, restriction );
      }
      else
      {
        DBG_STR1( ERROR, "Client %s change callback is NULL",
                  clients[tcxomgr.client_in_control].name );
      }
    }
  }

  /* Cycle through clients and notify denial to those that need it */
  for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++)
  {
    if ( clients[i].resp_req && (i != tcxomgr.best_client) )
    {
      DBG_STR1( MED, "Client %s denied", clients[i].name );
      (clients[i].deny_cb) ();
      clients[i].resp_req = FALSE;
    }
  }

  /* No controlling client, no client needs control, tcxomgr takes over */
  if ( (tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE) &&
       (tcxomgr.best_client == TCXOMGR_CLIENT_NONE) )
  {
    /* TODO: is this unconditionally idle? */
    tcxomgr.state = TCXOMGR_IDLE;
    restriction = tcxomgr_get_restriction( TCXOMGR_CLIENT_NONE );

    /* If highest restriction is NONE... */
    if (  restriction == TCXOMGR_RESTRICTION_NONE ) 
    {
      /* Allowed to rail the PDM */
      tcxomgr_send_notifications( TCXOMGR_RESTRICTION_NONE );

      tcxomgr_write_mux( TCXOMGR_SOURCE_POWER_OFF );
    }
    else if (  restriction != TCXOMGR_RESTRICTION_XO ) 
    {
      /* Client has requested that tcxo remain active, no sleep */
      tcxomgr_send_notifications( TCXOMGR_RESTRICTION_FROZEN );

      tcxomgr_write_mux( TCXOMGR_SOURCE_OVERRIDE );
    }
  }

  tcxomgr.restr_changed = FALSE;
  tcxomgr.pending_request = FALSE;

  #ifdef FEATURE_XO_TASK

  /* CAB - temporary - special consideration for freq_est client */
  if ( clients[TCXOMGR_CLIENT_FREQ_EST].notify_cb != NULL )
  {
    restriction = TCXOMGR_RESTRICTION_NONE;
    for ( i = 0; i < TCXOMGR_NUM_CLIENTS; i++ )
    {
      if ( clients[i].restriction != TCXOMGR_RESTRICTION_NONE )
      {
        restriction = TCXOMGR_RESTRICTION_XO;
        break;
      }
    }

    if ( (restriction == TCXOMGR_RESTRICTION_NONE) &&
         (tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE) &&
         (tcxomgr.best_client == TCXOMGR_CLIENT_NONE) )

    {
      DBG_0( MED, "Client FREQ EST notification:  TCXOMGR_RESTRICTION_NONE" );
      tcxomgr.cb_id = TCXOMGR_CLIENT_FREQ_EST;
      (clients[TCXOMGR_CLIENT_FREQ_EST].notify_cb)( TCXOMGR_RESTRICTION_NONE );
    }
    else
    {
      DBG_0( MED, "Client FREQ EST notification:  TCXOMGR_RESTRICTION_XO" );
      tcxomgr.cb_id = TCXOMGR_CLIENT_FREQ_EST;
      (clients[TCXOMGR_CLIENT_FREQ_EST].notify_cb)( TCXOMGR_RESTRICTION_XO );
    }
  }

  #endif /* FEATURE_XO_TASK */

} /* tcxomgr_process_changes */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_CALLBACKS

DESCRIPTION
  Sets the callback functions for the client indicated by id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs callback functions into the clients[] data array

============================================================================*/

void tcxomgr_set_callbacks
(
  tcxomgr_client_id_type  id,
  tcxomgr_vco_type (*vco_cb)    (void),
  void             (*grant_cb)  (tcxomgr_source_type, tcxomgr_restriction_type,
                                 tcxomgr_vco_type),
  void             (*change_cb) (tcxomgr_source_type, tcxomgr_restriction_type),
  void             (*release_cb)(void),
  void             (*deny_cb)   (void),
  void             (*notify_cb) (tcxomgr_restriction_type),
  void             (*rpush_flag_cb) (boolean),
  void             (*rpush_cb)      (tcxomgr_client_id_type,
                                     tcxomgr_vco_type,
                                     tcxomgr_rotator_type,
                                     uint32,
                                     timetick_type,
                                     timetick_type,
                                     int32,
                                     tcxomgr_client_state_type),
  tcxomgr_rotator_type (*rot_cb) (void)
)
{
  clients[id].vco_cb          = vco_cb;
  clients[id].grant_cb        = grant_cb;
  clients[id].change_cb       = change_cb;
  clients[id].release_cb      = release_cb;
  clients[id].deny_cb         = deny_cb;
  clients[id].notify_cb       = notify_cb;
  clients[id].rpush_flag_cb   = rpush_flag_cb;
  clients[id].rpush_cb        = rpush_cb;
  clients[id].rot_cb          = rot_cb;

  /* Inform the client of the current state */
  if (clients[id].notify_cb != NULL)
  {
    tcxomgr_restriction_type restriction = tcxomgr_get_working_restriction();
    DBG_STR2( MED, "Client %s notification:  %s", clients[id].name,
              restriction_strs[restriction] );
    tcxomgr.cb_id = id;
    (clients[id].notify_cb)( restriction );
  }

} /* tcxomgr_set_callbacks */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_CHANGE_SOURCE

DESCRIPTION
  For the client specified by id, who must be in control, it indicates a
  desire to change to a new tcxo control source (i.e. switch from DSP
  closed loop control to open loop control via the override register).

DEPENDENCIES
  Caller must have control of the tcxo

RETURN VALUE
  None

SIDE EFFECTS
  Changes the clients[].source data field

============================================================================*/

void tcxomgr_change_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
)
{
  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( tcxomgr.client_in_control == id )
  {
    DBG_STR2( MED, "Client %s, change to source %s",
              clients[id].name, source_strs[source] );

    /* Switch the mux to the new source */
    tcxomgr_write_mux(source);
  }
  else
  {
    DBG_STR1( ERROR, "Client %s, attempt to change source when not in control", 
              clients[id].name );
  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );
} /* tcxomgr_change_source */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_SOURCE

DESCRIPTION
  For the client specified by id, it indicates to tcxomgr that the client
  wants to control the tcxo with the source specified.  If the tcxomgr is
  currently involved in an operation with another client, the request is
  flagged and handled at a later time.  If the current client in control
  want to change source, the tcxomgr_change_source API should be used, not
  this one.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  Installs source into the clients[].source data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
)
{

  if ( (source >= TCXOMGR_MAX_SOURCE) || (source == TCXOMGR_SOURCE_NONE) )
  {
    DBG_STR1( ERROR, "Client %s, source invalid", clients[id].name );
    return;
  }

  /* Client must have vco_cb, so vco value can be retrieved by other clients */
  if ( clients[id].vco_cb == NULL )
  {
    DBG_STR1( ERROR, "Client %s, no vco_cb", clients[id].name );
    return;
  }

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  /* Make sure the requested service level is from a non-controlling client
     and is a change from the previous source request */
  if ( (id != tcxomgr.client_in_control) && (clients[id].source != source) )
  {
    DBG_STR2( MED, "Client %s request and notify, source %s",
              clients[id].name, source_strs[source] );

    /* Set the source and response flag for the client */
    clients[id].source = source;
    clients[id].resp_req = TRUE;

    if ( tcxomgr.state != TCXOMGR_REPLY_PENDING ) 
    {
      /* Now crunch the client list for any needed changes */
      tcxomgr_process_changes();
    }
    else
    {
      /* If in the middle of an ack, wait until ack comes back */
      tcxomgr.pending_request = TRUE;
    }

  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );

} /* tcxomgr_set_source */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_PRIORITY

DESCRIPTION
  Allows client specified by id to set his priority in relation to the other
  tcxomgr clients.  The priority is used by tcxomgr to determine who should
  get control of tcxomgr when more than one client requests it at the same
  time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs priority into the clients[].priority data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_priority
(
  tcxomgr_client_id_type    id,
  tcxomgr_priority_type     priority
)
{

  if ( priority >= TCXOMGR_MAX_PRIORITY )
  {
    DBG_STR1( ERROR, "Client %s, priority invalid", clients[id].name );
    return;
  }

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( clients[id].priority != priority )
  {
    /* Store the requested priority for this client */
    clients[id].priority = priority;

    DBG_STR2( MED, "Client %s, priority set to %s",
              clients[id].name, priority_strs[priority] );

    /* If someone is in control process the change */
    if ( tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE )
    {
      if ( tcxomgr.state != TCXOMGR_REPLY_PENDING ) 
      {
        /* Now crunch the client list for any needed changes */
        tcxomgr_process_changes();
      }
      else
      {
        /* If in the middle of an ack, wait until ack comes back */
        tcxomgr.pending_request = TRUE;
      }
    }
  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );

} /* tcxomgr_set_priority */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_RESTRICTION

DESCRIPTION
  This function sets the desired restriction for the client specified by
  id.  The restriction is a request to any controlling client to limit the
  rate of change of tcxo.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs restriction into the clients[].restriction data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_restriction
(
  tcxomgr_client_id_type    id,
  tcxomgr_restriction_type  restriction
)
{

  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_STR1( ERROR, "Client %s, restriction invalid", clients[id].name );
    return;
  }

  /* Start of critical section */
  /* Using ISR macro as tcxomgr_get_restriction is being called from an
     interrupt context */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Store the requested restriction for this client */
  clients[id].restriction = restriction;

  DBG_STR2( MED, "Client %s, restriction set to %s",
            clients[id].name, restriction_strs[restriction] );

  if ( tcxomgr.state != TCXOMGR_REPLY_PENDING ) 
  {
    /* Now crunch the client list for any needed changes */
    tcxomgr_process_changes();
  }
  else
  {
    /* If in the middle of an ack, wait until ack comes back */
    tcxomgr.pending_request = TRUE;
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_set_restriction */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RPUSH_SET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

void tcxomgr_rpush_set_flag
(
  tcxomgr_client_id_type    id,
  boolean                   rpush_flag
)
{
  uint8 i;

  uint32 rpush_mask = tcxomgr.rpush_mask;

  uint32 old_rpush_mask = tcxomgr.rpush_mask;

/*--------------------------------------------------------------------------*/
  
  if ( id >= TCXOMGR_NUM_CLIENTS )
  {
    DBG_0( ERROR, "Invalid Client %d" );
    return;
  }

  if (rpush_flag)
  {
    rpush_mask |= ((uint32)0x1 << id);
  }
  else
  {
    rpush_mask &= ~((uint32)0x1 << id);
  }

  tcxomgr.rpush_mask = rpush_mask;
  DBG_STR2( LOW, "Client %s, set rpush flag to %d", 
            clients[id].name, rpush_flag );

  /* If rpush was disabled, but this client is requesting enable */
  if (rpush_mask && !old_rpush_mask)
  {
    for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++)
    {
      if (clients[i].rpush_flag_cb != NULL)
      {
        clients[i].rpush_flag_cb(TRUE);
      }
    }
  }

  /* If rpush was enabled, and no more clients are requesting */
  else if (!rpush_mask && old_rpush_mask)
  {
    for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++)
    {
      if (clients[i].rpush_flag_cb != NULL)
      {
        clients[i].rpush_flag_cb(FALSE);
      }
    }
  }

} /* tcxomgr_rpush_set_flag */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

boolean tcxomgr_rpush_get_flag( void )
{
  if ( tcxomgr.rpush_mask != 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* tcxomgr_rpush_get_flag */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_MASK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

uint32 tcxomgr_rpush_get_mask( void )
{
  return tcxomgr.rpush_mask;
} /* tcxomgr_rpush_get_mask */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RPUSH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

void tcxomgr_rpush
(
  tcxomgr_client_id_type    id,
  tcxomgr_rotator_type      rot_val,
  uint32                    num_samples,
  timetick_type             begin_acq_time,
  timetick_type             end_acq_time,
  tcxomgr_client_state_type state
)
{
  int i;                        /* loop variable */
  int32 temp = 0;               /* temperature value */

  /*--------------------------------------------------------------------------*/

  /* Range check */
  if ( state >= TCXOMGR_NUM_STATES )
  {
    DBG_STR2( ERROR, "Client %s, rot push state invalid:  %d", 
              clients[id].name, state );
    return;
  }

  /* Only send the value on if rotator push is enabled */
  if (tcxomgr.rpush_mask)
  {
    tcxomgr_vco_type vco_val = tcxomgr_get_current_vco();

    /* get temperature in degree centigrade */
    temp = (int32)adc_read(ADC_PA_THERM_DEGC);

    /* Cycle through and pass the values to those with registered rpush_cb */
    for (i = 0; i < TCXOMGR_NUM_CLIENTS; i++)
    {
      if (clients[i].rpush_cb != NULL)
      {
        clients[i].rpush_cb(id, vco_val, rot_val, num_samples, begin_acq_time, end_acq_time, temp, state);
      }
    }
  }
  else
  {
    DBG_STR1( LOW, "Client %s, rot push attempted when disabled", 
              clients[id].name );
  }

} /* tcxomgr_rpush */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_ACK

DESCRIPTION
  This function is used by client id to acknowledge a recieved grant.  Once
  the acknowledgement is received, tcxomgr sets the mux to the client source
  and notifies all other clients of the restriction being adhered to.  The
  restriction may be different than that sent in the original command.  If new
  data has arrived from another client while waiting on this ack, the new
  request will be processed withing this function context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Hardware muxes will be set to client source
  Command may be sent to one or more active clients.

============================================================================*/
void tcxomgr_ack
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
)
{

  /* Range check */
  if ( source >= TCXOMGR_MAX_SOURCE )
  {
    DBG_STR1( ERROR, "Client %s, ack service level invalid", 
              clients[id].name );
    return;
  }

  /* Range check */
  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_STR1( ERROR, "Client %s, ack restriction invalid", 
              clients[id].name );
    return;
  }

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( id != tcxomgr.ack_client )
  {
    DBG_STR1( ERROR, "Client %s, ack received when not granted", 
              clients[id].name );
  }
  else
  {
    DBG_STR3( MED, "Client %s ack:  %s, %s",
              clients[id].name, 
              source_strs[source],
              restriction_strs[restriction] );

    /* Client officially has control of the vco */
    tcxomgr_write_mux( source );
    tcxomgr.client_in_control = id;
    tcxomgr.ack_client = TCXOMGR_CLIENT_NONE;
    tcxomgr.state = TCXOMGR_CONTROLLED;

    #ifdef FEATURE_XO
    /* If HDR gets the control then HDR will control the NMd counter hardware */
    if ( id == TCXOMGR_CLIENT_CDMA_HDR )
    {
      srch_mdsp_mnd_ctrl( FALSE );

      DBG_STR1( MED, "Client %s disabled 1x MND Control", clients[id].name );
    }

    if ( restriction != TCXOMGR_RESTRICTION_XO )
    {
      DBG_STR1( MED, "Client %s, not adhering to RESTRICTION_XO on XO target", 
              clients[id].name );
    }
    #endif /* FEATURE_XO */

    /* Notify other clients of new service */
    tcxomgr_send_notifications(restriction);

    /* If a new request/restriction has arrived, process it */
    if ( tcxomgr.pending_request ) 
    {
      tcxomgr_process_changes();
    }
  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );

} /* tcxomgr_ack */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RELEASE

DESCRIPTION
  This function is used by client id to release control of the tcxo.  If the
  client voluntarily does this, then other lower priority clients may be
  waiting for control.  If it has been commanded to release (i.e. 
  pre-emption), the new higher priority client is waiting to be granted
  service.  In either case, need to process changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command may be sent to one or more active clients.
  vco is frozen in hardware and the override register is in control

============================================================================*/
void tcxomgr_release
(
  tcxomgr_client_id_type    id,
  tcxomgr_vco_type          curr_vco_val
)
{
  tcxomgr_vco_info_type  rgs_vco;

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( id != tcxomgr.client_in_control )
  {
    /* If client is not in control, may be cancelling a prior request */
    clients[id].source = TCXOMGR_SOURCE_NONE;
    DBG_STR1( MED, "Client %s cancelled request", clients[id].name );
  }
  else
  {
    #ifdef FEATURE_XO
    /* If HDR releases control then 1x will control the NMd hardware */
    if ( id == TCXOMGR_CLIENT_CDMA_HDR )
    {
      srch_mdsp_mnd_ctrl( TRUE );

      DBG_STR1( MED, "Client %s enabled 1x MND Control", 
                clients[id].name );
    }
    #endif /* FEATURE_XO */

    /* If no restriction is in place, jump to rgs/temp/default value */
    /* If there is a restriction, keep the vco value that the last client used */
    if ( tcxomgr_get_restriction( TCXOMGR_CLIENT_NONE ) == 
         TCXOMGR_RESTRICTION_NONE ) 
    {
      rgs_vco = tcxomgr_get_stored_vco();
      tcxomgr.override_val = rgs_vco.vco_value;
    }
    else
    {
      tcxomgr.override_val = curr_vco_val;
    }

    /* Freeze the vco for now */
    TRK_LO_ADJ_SET_OVERRIDE_VAL( tcxomgr.override_val );
    tcxomgr_write_mux( TCXOMGR_SOURCE_OVERRIDE );

    DBG_STR2( MED, "Client %s released, vco frozen to: %d",
              clients[id].name, tcxomgr.override_val );

    clients[id].source = TCXOMGR_SOURCE_NONE;
    tcxomgr.client_in_control = TCXOMGR_CLIENT_NONE;
    tcxomgr.state = TCXOMGR_IDLE;

    /* Check for any new changes, esp. for pre-empting client */
    tcxomgr_process_changes();
  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );

} /* tcxomgr_release */
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_REPORT

DESCRIPTION
  The client specified in id issues a report when the restriction to which
  that client is adhering changes for whatever reason.  Whent he client
  requires operation under a new restriction, he issues a report which in
  turn issues a notification to all other client of the change in rate by
  which tcxo is changing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Notification may be sent to one or more active clients.

============================================================================*/
void tcxomgr_report
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
)
{

  if ( source >= TCXOMGR_MAX_SOURCE )
  {
    DBG_STR1( ERROR, "Client %s, report invalid", 
              clients[id].name );
    return;
  }

  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_STR1( ERROR, "Client %s, ack restriction invalid", 
              clients[id].name );
    return;
  }

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( id != tcxomgr.client_in_control )
  {
    DBG_STR1( ERROR, "Client %s, report recieved when not in control", 
              clients[id].name );
  }
  else
  {
    DBG_STR3( MED, "Client %s report:  %s, %s",
              clients[id].name, 
              source_strs[source],
              restriction_strs[restriction] );

    /* Notify other clients of new service */
    tcxomgr_send_notifications( restriction );
  }

  /* End of critical section */
  rex_leave_crit_sect( &tcxomgr_crit_sect );

} /* tcxomgr_report */
#endif /* FEATURE_XO_CTL_1X_ONLY */


/*============================================================================

FUNCTION TCXOMGR_INIT

DESCRIPTION
  This function initializes the TCXO Manager

DEPENDENCIES
  The given wait function will be called when NV reads are in progress.
  "rex_wait" may be used, or some derived function which may watch for 
  and respond to other signals, such as DOG report signals.

RETURN VALUE
  TRUE if initialization is complete
  FALSE if initialization is being performed by another task, but is not
    yet complete.

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
)
{

  /* For this first time only, use intlock to create thread safe code */
  INTLOCK();

  if ( tcxomgr.state == TCXOMGR_UNINITIALIZED )
  {
    /* Prevent re-initialization */
    tcxomgr.state = TCXOMGR_INITIALIZING;

    /* create a crit section */
    rex_init_crit_sect( &tcxomgr_crit_sect );
  }

  /* End of critical section */
  INTFREE();

  /* Start of critical section */
  rex_enter_crit_sect( &tcxomgr_crit_sect );

  if ( tcxomgr.state < TCXOMGR_IDLE )
  {
    #ifdef FEATURE_TCXOMGR_DIAG
    /* Initialize diag */
    tcxomgr_diag_init();
    #endif /* FEATURE_TCXOMGR_DIAG */

    /* Setup the TCXO manager's NV item read/write buffer */
    tcxomgr_nv_init( tcb_ptr, sig, wait );

    /* Attempt to read VCO data */
    tcxomgr_nv_vco_read();

    /* Attempt to read recent-good-system values */
    tcxomgr_nv_rgs_read();

    #ifdef FEATURE_XO
    /* Attempt to read and initialize cap switching vals */
    tcxomgr_xo_trim_read();
    #endif

    #ifdef FEATURE_XO_FACTORY_CAL
    tcxomgr_nv_cal_data_read();
    #endif

    /* start with best uncertainty because get_stored_value
       will decrease the uncertainty to the appropriate value */
    tcxomgr.uncertainty = TCXOMGR_VCO_RGS;
    (void) tcxomgr_get_stored_vco();

    /* Put the hardware to sleep */
    tcxomgr.pending_request = FALSE;
    tcxomgr.restr_changed = FALSE;
    tcxomgr.rpush_mask = 0;
    tcxomgr.state = TCXOMGR_IDLE;
    tcxomgr.vco_source = TCXOMGR_SOURCE_NONE;
    tcxomgr_write_mux( TCXOMGR_SOURCE_POWER_OFF );

    /* Initialize the working restriction and notify any clients that
     * happened to register prior to tcxomgr_init being invoked. */
    tcxomgr_send_notifications( tcxomgr_get_restriction( TCXOMGR_CLIENT_NONE ) );

#ifndef FEATURE_XO 
    TRK_LO_ADJ_SET_NORMAL_POLARITY();

    #ifdef FEATURE_TCXOMGR_SET_POLARITY
    /* Zero is normal, One is inverted */
    if ( TRK_LO_ADJ_NORMAL_POLARITY == 0 )
    {
      tcxomgr.polarity = TCXOMGR_TRK_LO_POLARITY_NORMAL;
    }
    else
    {
      tcxomgr.polarity = TCXOMGR_TRK_LO_POLARITY_INVERTED;
    }
    #endif /* FEATURE_TCXOMGR_SET_POLARITY */

#endif

    DBG_0( MED, "TCXOMGR initialized" );
  }

  rex_leave_crit_sect( &tcxomgr_crit_sect );

#ifdef FEATURE_XO
  srch_mdsp_mnd_ctrl( TRUE );
  DBG_0(MED, "1x MND control enabled");
#endif /* FEATURE_XO */

#ifdef TCXOMGR_UNIT_TEST
  tcxomgr_test_init();
#endif
} /* tcxomgr_init */


/*============================================================================

FUNCTION TCXOMGR_POWERDOWN

DESCRIPTION
  This function commits RAM values to NV memory

DEPENDENCIES
  The given wait function will be called when NV reads are in progress.
  "rex_wait" may be used, or some derived function which may watch for
  and respond to other signals, such as DOG report signals.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_powerdown
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV writing */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
)
{

/*--------------------------------------------------------------------------*/

  if ( tcxomgr.state != TCXOMGR_UNINITIALIZED )
  {
    /* Setup the TCXO manager's NV item read/write buffer */
    tcxomgr_nv_init( tcb_ptr, sig, wait );

    /* Attempt to read VCO temperature-frequency table */
    tcxomgr_nv_vco_write();

    /* Attempt to read recent-good-system values */
    tcxomgr_nv_rgs_write();

    /* Attempt to write the current xo_trim values */
    #ifdef FEATURE_XO
    tcxomgr_xo_trim_write();
    #endif /* FEATURE_XO */
  }

} /* tcxomgr_powerdown */
