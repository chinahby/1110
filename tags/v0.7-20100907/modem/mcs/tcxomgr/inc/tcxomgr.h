#ifndef TCXOMGR_H
#define TCXOMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the Recent Good System values, and VCO-Temperature
  tables, for proper TCXO operation.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006 - 2009 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/inc/tcxomgr.h#17 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/01/09   ns      Feature wrap set_polarity changes
04/02/09   ns      Added tcxomgr_set_polarity
12/10/08   sb      Add a field in tcxomgr_vco_indicator_type that indicates
                   if RGS is less than 30 seconds old.
12/05/08   sst     Update for QSC11x0 to us XO therm
10/20/08   adw     Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/24/08   adw     Modified CLIENT_FREQ_EST featurization to be based on 
                   XO_TASK as well as MCS_HAS_NO_GPS.
09/15/08   sst     Featurize CLIENT_FREQ_EST under XO_TASK
08/28/08   adw     Added FEATURE_XO_CTL_1X_ONLY and FEATURE_XO_DIAG to
                   featurize for ULC.
07/11/08   scm     Extern tcxomgr_reset_rgs_and_temp_table().
06/20/08   mca     Merge from //depot
06/04/08   sg      Added TCXOMGR_SOURCE_POWER_OFF source
05/06/08   rkc     Added function tcxomgr_get_time_and_temp()
04/07/08   ljl     Supported multi-carrier AFC log.
03/13/08   nnv     Added TCXOMGR_UBM_L1 enum for tcxomgr_priority_type.
11/14/07   sst     Updated comment to proper reflect PA temp units
10/31/07   va      Export API to retrieve freq err info
10/09/07   sst     Added function tcxomgr_ft_valid()
                   Updated function tcxomgr_ft_curr() outside of featurization
10/02/07   va      Added Freq Estimator to client list
09/25/07   cab     Added tcxomgr_ft_curr to get ft val at current temp
09/12/07   sst     Added UMB L1 to client list
09/06/07   sst     Lint fixes
07/26/07   sst     Moved prototype of tcxomgr_get_temp to tcxomgr_i.h
                   Re-ordered vco_indicator_type to correct uncertainity val
06/18/07   va      Support for XO Compensation unit test
06/12/07   cab     Added back in fac cal changes
06/12/07   cab     Reverted fac cal (temporarily), added rpush_get_flag
06/08/07   sst     Created tcxomgr_temp_set_type
                   Added tcxomgr_get_temp and tcxomgr_ft_eval prototypes
                   Move PPM_*_TO_PPM macros from _cal and _xo_trim
06/06/07   sst     Create new freq err types and update comments
06/05/07   sst     Added indicator enum values for Factory Calibration modes
05/17/07   cab     Added uncertainty for passive clients
05/16/07   cab     Updated rpush_set_flag to allow multiple client request
05/07/07   cab     Added rotator callback, get_current_rot
03/15/07   cab     Added back stub function for 6800 6.x build
03/12/07   cab     Change time value in rpush to signed, include adc.h.
03/08/07   grl     Fixed win32 compilation issue.
03/02/07   nnv     Modified tcxomgr_rpush and rpush_cb function signatures
                   and changed client state OOS to NO_INFO
03/02/07   nnv     Removed unused function declaration
11/03/06   cab     Added rotator pushing functionality
                   Added client TV OUT
10/19/06   cab     Added PPM client, stubbed old APIs for build issues
10/18/06   cab     added tcxomgr_log_freq_track_mode_enum_type and
                   tcxomgr_log_gain_mode_enum_type
10/10/06   cab     Initial version for tcxomgr v3
10/12/06   cab     Added priorities for WCDMA and GSM

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#ifndef TCXOMGR_UNIT_TEST
#include "customer.h"
#include "rex.h"
#include "qw.h"
#include "timetick.h"
#endif

/*============================================================================

                        TCXO Manager client interface

============================================================================*/


/* Maximum number of frequency entries per client in AFC log packet.  */
#define TCXOMGR_MAX_FREQ_ENTRIES 8

#define TCXOMGR_MAX_NUM_CARRIERS 3
  /* The maximum number of carriers */

/*---------------------------------------------------------------------------
  Simple scalar types
---------------------------------------------------------------------------*/

/* VCTCXO value type
   Sensitivity: 2^-20 PPM per LSB */
typedef int16                     tcxomgr_vco_type;


/* Rotator value type
   Sensitivity: 2^-10 PPM per LSB */
typedef int16                     tcxomgr_rotator_type;

/* PPM value type
   Sensitivity: 2^-10 PPM per LSB */
typedef int16                     tcxomgr_ppm_type;

/* High Resolution PPM value type
   Sensitivity: 2^-12 PPM per LSB */
typedef int32                     tcxomgr_hr_ppm_type;

/* Frequency value type
   Sensitivity: 1 Hz per LSB */
typedef int32                     tcxomgr_hz_type;

/* Temperature value type
   Sensitivity: 2^-10 deg C per LSB */
typedef int32                     tcxomgr_temp_type;

typedef struct
{
  tcxomgr_temp_type          xo;                 /* XTAL temp
                                                    (see below for units)    */
  tcxomgr_temp_type          pmic;               /* 2^-10 C, PMIC temp       */
  tcxomgr_temp_type          pa;                 /* 1 C,     PA temp         */
} tcxomgr_temp_set_type;

#if (defined( T_QSC1100 ) || defined( T_QSC1110 ) )
  /* on QSC11x0, 
     XO temp is in units of 1 degC */
  #define TCXOMGR_XO_TEMP_TO_DEGC( temp )  ( (temp) )
#else
  /* all other targets (specifically QSC60x5), 
     XO temp is in units of 2^-10 degC */
  #define TCXOMGR_XO_TEMP_TO_DEGC( temp )  ( (temp) >> 10 )
#endif

/*---------------------------------------------------------------------------
  Client ID's
---------------------------------------------------------------------------*/

/* The number of clients must not exceed the number of bits in
   tcxomgr.rpush_mask (32). If it does, rpush_mask's size will need
   to be increased */
typedef enum
{
  TCXOMGR_CLIENT_NONE,
  #ifndef FEATURE_XO_CTL_1X_ONLY
  TCXOMGR_CLIENT_GPS,
  TCXOMGR_CLIENT_BT,
  TCXOMGR_CLIENT_MEDIA_FLO,
  TCXOMGR_CLIENT_GSM,
  TCXOMGR_CLIENT_WCDMA,
  TCXOMGR_CLIENT_CDMA_1X,
  TCXOMGR_CLIENT_CDMA_HDR,
  TCXOMGR_CLIENT_PPM,
  TCXOMGR_CLIENT_TV_OUT,
  TCXOMGR_CLIENT_UBM_L1,
  #else /* !FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_CLIENT_CDMA_1X,
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  /* REVISIT: CLIENT_FREQ_EST should really only be featurized by XO_TASK,
       however, to prevent compiler error in gps code we must ensure that
       this field is included in the enum.  The gps file mc_fee.c initializes 
       a 'static const' structure and requires the CLIENT_FREQ_EST field to be
       present.  Gps code does not distinguish library for XO vs. TCXO so for
       now we must featurize based on XO_TASK as well as MCS_HAS_NO_GPS. */
  #if( defined( FEATURE_XO_TASK ) || !defined( FEATURE_MCS_HAS_NO_GPS ) )
  TCXOMGR_CLIENT_FREQ_EST,
  #endif /* FEATURE_XO_TASK && !FEATURE_MCS_HAS_NO_GPS */
  TCXOMGR_NUM_CLIENTS
} tcxomgr_client_id_type;

/*---------------------------------------------------------------------------
  Priorities
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_NO_PRIORITY,
  TCXOMGR_DEFAULT_PRIORITY,
  #ifndef FEATURE_XO_CTL_1X_ONLY
  TCXOMGR_UBM_L1,
  TCXOMGR_1X_OL,
  TCXOMGR_HDR_OL,
  TCXOMGR_1X_CL,
  TCXOMGR_HDR_CL,
  TCXOMGR_GSM,
  TCXOMGR_WCDMA,
  #else /* !FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_1X_OL,
  TCXOMGR_1X_CL,
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_HIGHEST_PRIORITY,
  TCXOMGR_MAX_PRIORITY
} tcxomgr_priority_type;

/*---------------------------------------------------------------------------
  Source Type
---------------------------------------------------------------------------*/

/* List of possible vco sources */
typedef enum
{
  TCXOMGR_SOURCE_NONE,           /* no control of TCXO */
  TCXOMGR_SOURCE_OVERRIDE,       /* control of TCXO using override reg */
  TCXOMGR_SOURCE_1X_DSP,         /* control of TCXO using 1X DSP */
  #ifndef FEATURE_XO_CTL_1X_ONLY
  TCXOMGR_SOURCE_HDR_DSP,        /* control of TCXO using HDR DSP */
  TCXOMGR_SOURCE_UMTS_DSP,       /* control of TCXO using UMTS DSP */
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_SOURCE_POWER_OFF,      /* turn off TCXO */
  TCXOMGR_MAX_SOURCE
} tcxomgr_source_type;

/*---------------------------------------------------------------------------
  Restriction Type
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_RESTRICTION_NONE,    /* TCXO not restricted, anything goes */
  #ifndef FEATURE_XO_CTL_1X_ONLY
  TCXOMGR_RESTRICTION_FAST,    /* TCXO restr to fast closed loop ctrl */
  TCXOMGR_RESTRICTION_SLOW,    /* TCXO restr to slow closed loop ctrl */
  TCXOMGR_RESTRICTION_SRL,     /* TCXO restr to SRL closed loop ctrl */
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_RESTRICTION_FROZEN,  /* TCXO restr to be frozen */
  TCXOMGR_RESTRICTION_XO,      /* TCXO restr irrelevant, XO target (frozen) */
  TCXOMGR_MAX_RESTRICTION
} tcxomgr_restriction_type;


/*---------------------------------------------------------------------------
  State Type (used in rotator updates)
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_STATE_IDLE,
  TCXOMGR_STATE_TRAFFIC,
  TCXOMGR_STATE_NO_INFO,
  TCXOMGR_NUM_STATES
} tcxomgr_client_state_type;

/*----------------------------------------------------------------------*
  TCXOmgr frequency tracking modes in AFC log packet
 *----------------------------------------------------------------------*/

typedef enum
{
  #ifndef FEATURE_XO_CTL_1X_ONLY
  TCXOMGR_VCO_ONLY_TRK_MODE,
  TCXOMGR_FIXED_BW_RVAFC_TRK_MODE,
  TCXOMGR_VAR_BW_RVAFC_TRK_MODE,
  #endif /* FEATURE_XO_CTL_1X_ONLY */
  TCXOMGR_ROT_ONLY_TRK_MODE
} tcxomgr_log_freq_track_mode_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr gain modes in AFC log packet
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_ACQ_GAIN_MODE,
  TCXOMGR_TRK_GAIN_MODE
} tcxomgr_log_gain_mode_enum_type;

#ifdef FEATURE_TCXOMGR_SET_POLARITY 

/*----------------------------------------------------------------------*
  TCXOmgr TRK_LO polarity 
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_TRK_LO_POLARITY_NORMAL,
  TCXOMGR_TRK_LO_POLARITY_INVERTED
} tcxomgr_trk_lo_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr TRK_LO polarity return type 
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_TRK_LO_POLARITY_SUCCESS,    /* Polarity & override value updated */ 
  TCXOMGR_TRK_LO_POLARITY_INVALID,    /* Polarity is invalid  */ 
  TCXOMGR_TRK_LO_PDM_VALUE_UNCHANGED  /* Polarity updated, value unchanged */
} tcxomgr_trk_lo_return_enum_type;

#endif /* FEATURE_TCXOMGR_SET_POLARITY */

/*---------------------------------------------------------------------------
  Frequency Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* vco accumulator value */
  uint16   vco_acc;

  /* rotator accumulator value */
  uint16   rot_acc;

  /* delta time */
  uint16   delta_time;

  /* signal strength indicator */
  uint16   signal_strength;

} tcxomgr_log_entries_struct_type;

/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* Client that is logging the AFC log */
  uint8    logging_client_id;

  /* Client that is controlling the VcTcxo */
  uint8    ctrl_client_id;

  /* Reserved field */
  uint8    reserved1;

  /* system time of first entry */
  qword    timestamp;

  /* frequency tracking mode - uses  tcxomgr_log_freq_track_mode_enum_type */
  uint8    afc_mode;

  /* frequency loop gain - uses tcxomgr_log_gain_mode_enum_type */
  uint8    afc_gain;

  /* SRL indicator */
  uint8    srl_indicator;

  /* Finger cross mode */
  uint8    cross_mode;

  /* rf band class */
  uint8    band;

  /* Reserved field */
  uint8   reserved;

  /* rf cdma channel */
  uint16   chan;

  /* Carrier frequency */
  uint32   carrier_freq;

  /* vco slope */
  uint16   vco_slope;

  /* Thermistor reading degC */
  uint16   therm;

  /* number of AFC entries */
  uint32   num_freq_entry;

} tcxomgr_log_header_struct_type;

/*---------------------------------------------------------------------------
  AFC log structure
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  tcxomgr_log_header_struct_type      afc_header_info;

  tcxomgr_log_entries_struct_type     entries[TCXOMGR_MAX_FREQ_ENTRIES];

} tcxomgr_log_struct_type;

/*---------------------------------------------------------------------------
  Multi-carrier Frequency Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* vco accumulator value */
  uint16   vco_acc;

  /* delta time */
  uint16   delta_time;

  /* signal strength indicator */
  uint16   signal_strength;

  /* rotator accumulator value */
  uint16   rot_acc[TCXOMGR_MAX_NUM_CARRIERS];

} tcxomgr_mc_log_entries_struct_type;

/*---------------------------------------------------------------------------
  Carrier Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* rf band class */
  uint8    band;

  /* Reserved field */
  uint8    reserved1;

  /* rf cdma channel */
  uint16   chan;

  /* Carrier frequency */
  uint32   carrier_freq;

} tcxomgr_mc_log_carrier_struct_type;

typedef PACKED struct
{
  tcxomgr_temp_type          xo;                 /* 2^-10 C, XTAL temp       */
  tcxomgr_temp_type          pmic;               /* 2^-10 C, PMIC temp       */
  tcxomgr_temp_type          pa;                 /* 1 C,     PA temp         */
} tcxomgr_mc_log_temp_set_type;

/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{

  /* Client that is logging the AFC log */
  uint8    logging_client_id;

  /* Client that is controlling the VcTcxo */
  uint8    ctrl_client_id;

  /* Carrier mask */
  uint8    carrier_mask;

  /* system time of first entry */
  qword    timestamp;

  /* frequency tracking mode - uses  tcxomgr_log_freq_track_mode_enum_type */
  uint8    afc_mode;

  /* frequency loop gain - uses tcxomgr_log_gain_mode_enum_type */
  uint8    afc_gain;

  /* SRL indicator */
  uint8    srl_indicator;

  /* Finger cross mode */
  uint8    cross_mode;

  /* vco slope */
  uint16   vco_slope;

  /* Thermistor reading degC */
  tcxomgr_mc_log_temp_set_type  therm;

  /* number of AFC entries */
  uint32   num_freq_entry;

} tcxomgr_mc_log_header_struct_type;

/*---------------------------------------------------------------------------
  MC AFC log structure
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  tcxomgr_mc_log_header_struct_type      afc_header_info;

  /* carrier info */
  tcxomgr_mc_log_carrier_struct_type     carrier[TCXOMGR_MAX_NUM_CARRIERS];

  tcxomgr_mc_log_entries_struct_type     entries[TCXOMGR_MAX_FREQ_ENTRIES];

} tcxomgr_mc_log_struct_type;

/*---------------------------------------------------------------------------
  Client struct
---------------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_client_id_type            id;
  tcxomgr_priority_type             priority;
  char                              name[10];
  tcxomgr_source_type               source;
  tcxomgr_restriction_type          restriction;
  boolean                           resp_req;
  tcxomgr_vco_type                (*vco_cb)        (void);
  void                            (*grant_cb)      (tcxomgr_source_type,
                                                    tcxomgr_restriction_type,
                                                    tcxomgr_vco_type);
  void                            (*change_cb)     (tcxomgr_source_type,
                                                    tcxomgr_restriction_type);
  void                            (*release_cb)    (void);
  void                            (*deny_cb)       (void);
  void                            (*notify_cb)     (tcxomgr_restriction_type);
  void                            (*rpush_flag_cb) (boolean);
  void                            (*rpush_cb)      (tcxomgr_client_id_type,
                                                    tcxomgr_vco_type,
                                                    tcxomgr_rotator_type,
                                                    uint32,
                                                    timetick_type,
                                                    timetick_type,
                                                    int32,
                                                    tcxomgr_client_state_type);
  tcxomgr_rotator_type            (*rot_cb)        (void);
} tcxomgr_client_type;

/*---------------------------------------------------------------------------
  Freq/Temp information Accumulator indicator values
---------------------------------------------------------------------------*/
  /* Enum is order from worst to best uncertianty
     NOTE: the *_NOT_CAL, *_FAC1, *_FT#, *_FAILED enum values apply when
           Factory Calibration of the Freq Temp table/curve is performed */

typedef enum
{
  TCXOMGR_FT_QUAL_FAILED,     /* Fac Cal Failed, phone should have been
                                  failed in the Factory
                                  56 PPM mode                                */
  TCXOMGR_FT_QUAL_NOT_CAL,    /* Factory Calibration has not been performed
                                  56 PPM mode                                */
  TCXOMGR_FT_QUAL_FAC1,       /* Intermediate step of Factory Calibration
                                  56 PPM mode                                */
  TCXOMGR_VCO_DEFAULT,        /* Default value was returned
                                  5 PPM mode                                 */
  TCXOMGR_FT_QUAL_DEFAULT = TCXOMGR_VCO_DEFAULT,
  TCXOMGR_FT_QUAL_FT1,        /* Value from calibrated curve with default C0
                                  5 PPM mode                                 */
  TCXOMGR_VCO_OLD_RGS,        /* Value from a not-so-recent Good System
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_OLD_RGS = TCXOMGR_VCO_OLD_RGS,
  TCXOMGR_VCO_INTERPOLATED,   /* Value was interpolated from adj value(s)
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_INTERP = TCXOMGR_VCO_INTERPOLATED,
  TCXOMGR_FT_QUAL_FT2,        /* Value from calibrated curve with known C0
                                  3 PPM mode - c0 state                      */
  TCXOMGR_VCO_EXACT,          /* Value for temp was found in freq/temp table
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_EXACT = TCXOMGR_VCO_EXACT,
  TCXOMGR_VCO_RGS,            /* Value was from the Recent Good System
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_RGS = TCXOMGR_VCO_RGS,

  TCXOMGR_VCO_RGS_RECENT,     /*  RGS, but less than 30 secs old             */
  TCXOMGR_VCO_LAST          /* last value in the enum                        */
}
tcxomgr_vco_indicator_type;


/*---------------------------------------------------------------------------
  Freq/Temp information struct
---------------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_vco_indicator_type  vco_type;
  tcxomgr_vco_type            vco_value;
  tcxomgr_rotator_type        rot_value;
} tcxomgr_vco_info_type;

/*---------------------------------------------------------------------------
  Recent Good System Values
---------------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_client_id_type      system;
  int16                       temp_diff;
  uint32                      age_secs;
  tcxomgr_vco_type            vco;
  tcxomgr_rotator_type        rot;
}
tcxomgr_rgs_vals_struct_type;

/*============================================================================

                            MACROS AND DEFINES

============================================================================*/

/* If the age in the RGS is invalid, set it to this value */
#define TCXOMGR_INVALID_RGS_AGE  (0xFFFFFFFFUL)

/* Full (1)   PPM units to 2^-10 PPM units conversion */
#define PPM_FULL_TO_PPM( x )          ( (x) << 10 )

/* Half (1/2) PPM units to 2^-10 PPM units conversion */
#define PPM_HALF_TO_PPM( x )          ( (x) << 9 )

/* Full (1)   PPM units to 2^-12 High Resolution PPM units conversion */
#define PPM_FULL_TO_HR_PPM( x )       ( (x) << 12 )

/* Half (1/2) PPM units to 2^-12 High Resolution PPM units conversion */
#define PPM_HALF_TO_HR_PPM( x )       ( (x) << 11 )

/* 2^-10 PPM units to and from 2^-12 High Resolution PPM units */
#define PPM_PPM_TO_HR_PPM( x )        ( (x) << 2 )
#define PPM_HR_PPM_TO_PPM( x )        ( (x) >> 2 )


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

#ifndef WIN32
/* Remove this function when RF removes reference to it */
inline void tcxomgr_unfreeze() {}

inline boolean tcxomgr_vco_temp_exists(void)
{
  return FALSE;
}

#endif

/*============================================================================

FUNCTION TCXOMGR_INIT

DESCRIPTION
  This function initializes the TCXO Manager

DEPENDENCIES
  The given wait function will be called when NV reads are in progress.
  "rex_wait" may be used, or some derived function which may watch for
  and respond to other signals, such as DOG report signals.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/


extern void tcxomgr_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
);



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

extern void tcxomgr_powerdown
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV writing */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
);


/*============================================================================

FUNCTION TCXOMGR_GET_VCO_SLOPE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_get_vco_slope
(
  /* vco slope */
  uint16 *vco_slope,

  /* vco range */
  uint8 *vco_range
);

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_info_type tcxomgr_get_stored_vco (void);


/*============================================================================

FUNCTION TCXOMGR_GET_UNCERTAINTY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type

SIDE EFFECTS
  None

============================================================================*/
extern tcxomgr_vco_indicator_type tcxomgr_get_uncertainty (void);


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_VCO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_type tcxomgr_get_current_vco (void);
#endif /* FEATURE_XO_CTL_1X_ONLY */


/*============================================================================

FUNCTION TCXOMGR_UPDATE_RGS_VALUES

DESCRIPTION
  This function updates the Recent Good System values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_update_rgs_values
(
  /* client doing the update */
  tcxomgr_client_id_type  id,

  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type        vco_val,

  /* Rotator accumulator (in 2^-10 ppm units) */
  tcxomgr_rotator_type    rot_val
);


/*============================================================================

FUNCTION TCXOMGR_RESET_RGS_AND_TEMP_TABLE

DESCRIPTION
  This function blows away all Recent Good System data and the entire trk_lo
  vs. temperature table.  It should be called very rarely and very carefully.
  If some RAT (GSM, WCDMA, ...) feels TCXO Manager data is bad, corrupt,
  invalid, etc., and should no longer be used, it calls this function to dump
  all the data.

  Effects of this function take place immediately in RAM cached copies.  New
  data doesn't go to NV until power down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_reset_rgs_and_temp_table
(
  /* Client resetting RGS and temperature table. */
  tcxomgr_client_id_type  client_id
);


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_PRIORITY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_set_priority
(
  tcxomgr_client_id_type    id,
  tcxomgr_priority_type     priority
);
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

extern void tcxomgr_change_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_SERVICE_LEVEL

DESCRIPTION
  This function requests a service level for a particular Active Client.
  This will generate a command that is sent to the Active Client with the
  results of the request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command may be sent to one or more active clients.

============================================================================*/

extern void tcxomgr_set_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
);
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

extern void tcxomgr_set_callbacks
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
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_NOTIFY_CALLBACK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_set_notify_callback
(
  tcxomgr_client_id_type  id,
  void (*notify_cb) (tcxomgr_restriction_type)
);
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

extern void tcxomgr_rpush_set_flag
(
  tcxomgr_client_id_type    id,
  boolean                   rpush_flag
);
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

extern boolean tcxomgr_rpush_get_flag( void );
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

extern uint32 tcxomgr_rpush_get_mask( void );
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

extern void tcxomgr_rpush
(
  tcxomgr_client_id_type    id,
  tcxomgr_rotator_type      rot_val,
  uint32                    num_samples,
  timetick_type             begin_acq_time,
  timetick_type             end_acq_time,
  tcxomgr_client_state_type state
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_ACK

DESCRIPTION
  This function acknowledges a recieved command.
  This may generate a command that is sent to the Active Client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_ack
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_RELEASE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_release
(
  tcxomgr_client_id_type    id,
  tcxomgr_vco_type          curr_vco_val
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_REPORT

DESCRIPTION
  This function updates an ack to a new restriction level

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_report
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_SET_RESTRICTION

DESCRIPTION
  This function requests a restriction level for a particular Passive Client.
  This will generate a command that is sent to the Active Client with the
  results of the request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_set_restriction
(
  tcxomgr_client_id_type    id,
  tcxomgr_restriction_type  restriction
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


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
extern tcxomgr_restriction_type tcxomgr_get_restriction
(
  tcxomgr_client_id_type    id
);


#ifndef FEATURE_XO_CTL_1X_ONLY
/*============================================================================

FUNCTION TCXOMGR_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

extern void tcxomgr_flush_client_log
(
  tcxomgr_log_struct_type     *afc_log_ptr
);
#endif /* FEATURE_XO_CTL_1X_ONLY */


#if ( defined( FEATURE_TCXOMGR_DIAG ) && !defined( FEATURE_XO_CTL_1X_ONLY ) )
/*============================================================================

FUNCTION TCXOMGR_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

extern void tcxomgr_log_entry_updated
(
  tcxomgr_log_struct_type     *afc_log_ptr
);
#endif /* FEATURE_TCXOMGR_DIAG && !FEATURE_XO_CTL_1X_ONLY */


#ifdef FEATURE_TCXOMGR_DIAG
/*============================================================================

FUNCTION TCXOMGR_MC_FLUSH_CLIENT_LOG

DESCRIPTION

  This function sends the mulitcarrier AFC log to diag if the log
  buffer is full or if logging client wanted it to be flushed because
  of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

extern void tcxomgr_mc_flush_client_log
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
);
#endif /* FEATURE_TCXOMGR_DIAG */

#ifdef FEATURE_TCXOMGR_DIAG
/*============================================================================

FUNCTION TCXOMGR_MC_LOG_ENTRY_UPDATED

DESCRIPTION

  This function is used by clients to indicate that they have updated
  one frequency entry to their multicarrier AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

extern void tcxomgr_mc_log_entry_updated
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
);
#endif /* FEATURE_TCXOMGR_DIAG */


#ifdef FEATURE_XO_FACTORY_CAL
/*============================================================================

FUNCTION TCXOMGR_FT_EVAL

DESCRIPTION
  This function calculates the frequency error in PPM expected based on a
  given set of temperatures. This function is not supported outside of the
  FEATURE_XO_FACTORY_CAL define

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_eval
(
  tcxomgr_temp_set_type temp
);
#endif /* FEATURE_XO_FACTORY_CAL */

/*============================================================================

FUNCTION TCXOMGR_FT_CURR

DESCRIPTION
  This function calculates the frequency error in PPM expected at the current
  temperature

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_curr( void );

/*============================================================================

FUNCTION TCXOMGR_FT_VALID

DESCRIPTION
  This function returns TRUE if the FT curve has been calibrated and is valid

DEPENDENCIES
  None

RETURN VALUE
  True/False depending on FT cal state

SIDE EFFECTS
  None

============================================================================*/
extern boolean tcxomgr_ft_valid( void );

/*============================================================================

FUNCTION tcxomgr_get_slope_err

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  boolean, TRUE if estimate available, FALSE if not

SIDE EFFECTS
  None

============================================================================*/

boolean tcxomgr_get_slope_err
(
  int32 *slope,
  int32 *outer_accum,
  timetick_type *timestamp
);

/*============================================================================

FUNCTION TCXOMGR_GET_TIME_AND_TEMP

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_time_and_temp
( 
  uint32 *time, 
  tcxomgr_temp_set_type *temp 
);

#ifdef FEATURE_TCXOMGR_SET_POLARITY 

/*============================================================================

FUNCTION TCXOMGR_SET_POLARITY

DESCRIPTION
  This function sets the polarity as requested and also sets the PDM value.
  The function should be called by the technology team (1x/HDR/WCDMA/GSM/....)
  to set the polarity required by it. It also changes the PDM value to reflect
  the new polarity.
 
DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_trk_lo_return_enum_type which specifies if the polarity change was 
  successful, could not change the polarity or an invalid polarity was passed. 

SIDE EFFECTS
  None.

============================================================================*/

extern tcxomgr_trk_lo_return_enum_type tcxomgr_set_polarity
( 
  tcxomgr_trk_lo_enum_type    polarity
);

#endif /* FEATURE_TCXOMGR_SET_POLARITY */

#endif /* !TCXOMGR_H */
