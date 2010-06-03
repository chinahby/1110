#ifndef TCXOMGR_CAL_H
#define TCXOMGR_CAL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

             T C X O    M A N A G E R    C A L I B R A T I O N

               TCXO Manager Calibration Header File

GENERAL DESCRIPTION

  This module contains the functions that are used for factory calibration
  and field calibration to determine the coefficients for the calibrated
  XO freq vs. Temp curve

EXTERNALIZED FUNCTIONS



REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/inc/tcxomgr_cal.h#2 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/16/07   va      Remove TCXO Diag cal functions
10/18/07   sst     Updated calculations for nominal C2/C3
09/06/07   sst     Sync enum of ft_qual btw NV and TCXOMGR
                   Undefine TCXOMGR_CAL_DEBUG
                   Lint fixes
08/13/07   sst     Change field data member timestamp to uint64
07/27/07   sst     Added prototypes for: tcxomgr_field_switch_xo_trim
                   Reordered enum ft_qual_enum to match vco_ind
                   Moved NV defaults from _cal.c
                   Modify PMIC coef to be -37 (-1/110 in HR ppm/C)
06/29/07   cab     Added tcxomgr_field_cal
06/18/07   aps     Added prototypes tcxomgr_ft_eval( ) and
                    tcxomgr_cal_copy_fac_to_field( )
06/18/07   va      Remove undef of XO_FACTORY_CAL for XO builds
06/13/07   sst     Created tcxomgr_cal_ft_conv_type and tcxomgr_cal_fac_type
                   Misc code fixes and updates
06/08/07   sst     Moved tcxomgr_cal_temp_type to tcxomgr.h
                   Added completion callback function to Fac Cal functions
06/06/07   sst     Factory Cal merge to mainline from dev branch
03/07/07   sst     Initial Version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"

#if ( ( !defined( FEATURE_XO_FACTORY_CAL ) &&         \
         defined( FEATURE_XO_FIELD_CAL )      ) ||    \
      (  defined( FEATURE_XO_FACTORY_CAL ) &&         \
        !defined( FEATURE_XO_FIELD_CAL )      )    )
  #error Unsupported XO Cal combination
#endif


#if ( defined( FEATURE_XO_FACTORY_CAL ) && defined ( FEATURE_XO_FIELD_CAL ) )

#include "comdef.h"
#include "tcxomgr.h"

#undef TCXOMGR_CAL_DEBUG


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*============================================================================

                   Defines

============================================================================*/
#define TCXOMGR_CAL_NUM_SAMPLES   512       /* max number of samples to
                                               record                       */

#define CAL_NV_DEFAULT_VAL_C0     0         /* Default value for C0         */
#define CAL_NV_DEFAULT_VAL_C1    -3440839   /* Default value for C1         */
#define CAL_NV_DEFAULT_VAL_D21    146       /* Default value for D21        */
#define CAL_NV_DEFAULT_VAL_D20    120090    /* Default value for D20        */
#define CAL_NV_DEFAULT_VAL_D31   -110       /* Default value for D31        */
#define CAL_NV_DEFAULT_VAL_D30    274277    /* Default value for D30        */
#define CAL_NV_DEFAULT_VAL_T0     30642     /* Default value for T0         */
#define CAL_NV_DEFAULT_VAL_P     -37        /* Default value for P          */
#define CAL_NV_DEFAULT_VAL_T0P    30642     /* Default value for T0P        */
#define CAL_NV_DEFAULT_VAL_FT_QUAL_IND TCXOMGR_CAL_FT_QUAL_NOT_CAL
                                            /* Default value for FT_QUAL_IND*/


/*============================================================================

                   Macros

============================================================================*/


/*============================================================================

                   Structs, Enums

============================================================================*/


/*------------------------ General -----------------------------------------*/

/* tcxomgr factory cal state*/
typedef enum
{
  TCXOMGR_CAL_STATE_NOT_CAL,                     /* intial uncalibrated state*/
  TCXOMGR_CAL_STATE_COARSE,                      /* Coarse tuning completed  */
  TCXOMGR_CAL_STATE_SAMP,                        /* Samples Captured         */
  TCXOMGR_CAL_STATE_C1C2C3,                      /* C1C2C3 determined        */
  TCXOMGR_CAL_STATE_FINE,                        /* Fine tuning completed    */
  TCXOMGR_CAL_STATE_COMPLETED,                   /* Fac Cal completed        */
  TCXOMGR_CAL_STATE_FAILED,                      /* Fac Cal failed           */
} tcxomgr_cal_state_enum_type;

/* tcxomgr field cal states */
typedef enum
{
  TCXOMGR_FIELD0,
  TCXOMGR_FIELD1,
  TCXOMGR_FIELD2,
  TCXOMGR_FIELD3,
  TCXOMGR_FIELD4,
} tcxomgr_field_state_type;

/* indicator for NV values of the quality of the coefficients
   this value must stay in this order to match the NV item's enum */
typedef enum
{
  TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL,                /* uncalibrated             */
  TCXOMGR_CAL_NV_FT_QUAL_FAC1,                   /* 56 PPM mode              */
  TCXOMGR_CAL_NV_FT_QUAL_FT1,                    /*  5 PPM mode              */
  TCXOMGR_CAL_NV_FT_QUAL_FT2,                    /*  3 PPM mode              */
  TCXOMGR_CAL_NV_FT_QUAL_FAILED,                 /* 56 PPM mode
                                                    (Fac Cal Failed, phone
                                                     should have failed in
                                                     Factory)                */
} tcxomgr_cal_nv_ft_qual_enum_type;


/* indicator of the quality of the coefficients */
typedef enum
{
  TCXOMGR_CAL_FT_QUAL_FAILED  = TCXOMGR_FT_QUAL_FAILED,
                                                 /* 56 PPM mode
                                                    (Fac Cal Failed, phone
                                                     should have failed in
                                                     Factory)                */
  TCXOMGR_CAL_FT_QUAL_NOT_CAL = TCXOMGR_FT_QUAL_NOT_CAL,
                                                 /* uncalibrated             */
  TCXOMGR_CAL_FT_QUAL_FAC1    = TCXOMGR_FT_QUAL_FAC1,
                                                 /* 56 PPM mode              */
  TCXOMGR_CAL_FT_QUAL_FT1     = TCXOMGR_FT_QUAL_FT1,
                                                 /*  5 PPM mode              */
  TCXOMGR_CAL_FT_QUAL_FT2     = TCXOMGR_FT_QUAL_FT2,
                                                 /*  3 PPM mode              */
} tcxomgr_cal_ft_qual_enum_type;

/* indicator wether to record Fac Cal coefficients in RAM and/or NV during
   Factory Cal */
typedef enum
{
  TCXOMGR_CAL_RECORD_NONE = 0x0,                 /* do not record            */
  TCXOMGR_CAL_RECORD_RAM  = 0x1,                 /* record to RAM            */
  TCXOMGR_CAL_RECORD_NV   = 0x3,                 /* record to RAM and NV     */
} tcxomgr_cal_record_enum_type;


typedef struct
{
  tcxomgr_temp_set_type      min;                /* min temps encountered in
                                                    calibration              */
  tcxomgr_temp_set_type      max;                /* max temps encountered in
                                                    calibration              */
} tcxomgr_cal_temp_range_type;

typedef struct
{
  tcxomgr_temp_set_type      xo_temp[ TCXOMGR_CAL_NUM_SAMPLES ];
                                                 /* 2^-10 C, XTAL temp       */
  tcxomgr_hr_ppm_type        xo_freq[ TCXOMGR_CAL_NUM_SAMPLES ];
                                                 /* 2^-12 ppm, XO frequency  */
  int32                      c1_est;             /* estimate for coef C1     */
  int32                      num_samples;        /* num samples taken        */
  tcxomgr_cal_temp_range_type temp_range;        /* temp range encountered   */
} tcxomgr_cal_ft_sample_type;

/* Structure to describe the coefficients of the calculated C2/C3
   conversions */
typedef struct
{
  int32                      c1toc2_0;
  int32                      c1toc2_1;
  int32                      c1toc3_0;
  int32                      c1toc3_1;
} tcxomgr_cal_ft_conv_type;

/* Structure to describe the factory coefficients of the freq/temp curve
   f(t) = c3(t-t0)^3 + c2(t-t0)^2 + c1(t-t0) + c0

  Note: The order of the members in this data type must match exactly those of
  tcxomgr_cal_field_data_type */
typedef struct
{
  int32                      c3;                 /* units of 2^-32 ppm/C^3  */
  int32                      c2;                 /* units of 2^-28 ppm/C^2  */
  int32                      c1;                 /* units of 2^-24 ppm/C    */
  int32                      c0;                 /* units of 2^-12 ppm      */
  tcxomgr_temp_type          t0;                 /* Ref XO Temp term
                                                     units of 2^-10 C       */
  tcxomgr_temp_type          t0p;                /* Ref PMIC Temp term
                                                     units of 2^-10 C       */
  int32                      p;                  /* PMIC compensation factor*/
  tcxomgr_cal_ft_qual_enum_type ft_qual_ind;     /* quality indicator of
                                                     the coefficients       */
  uint8                      xo_trim;            /* corresponding xo_trim
                                                     val for coefficients   */
} tcxomgr_cal_fac_data_type;

/* Structure to describe the field coefficients of the freq/temp curve
   f(t) = c3(t-t0)^3 + c2(t-t0)^2 + c1(t-t0) + c0

  Note: The order of the first 9 members in this data type must match exactly
  those of tcxomgr_cal_fac_data_type */
typedef struct
{
  int32                      c3;                 /* units of 2^-32 ppm/C^3  */
  int32                      c2;                 /* units of 2^-28 ppm/C^2  */
  int32                      c1;                 /* units of 2^-24 ppm/C    */
  int32                      c0;                 /* units of 2^-12 ppm      */
  tcxomgr_temp_type          t0;                 /* Ref XO Temp term
                                                     units of 2^-10 C       */
  tcxomgr_temp_type          t0p;                /* Ref PMIC Temp term
                                                     units of 2^-10 C       */
  int32                      p;                  /* PMIC compensation factor*/
  tcxomgr_cal_ft_qual_enum_type ft_qual_ind;     /* quality indicator of
                                                     the coefficients       */
  uint8                      xo_trim;            /* corresponding xo_trim
                                                     val for coefficients   */
  uint64                     timestamp;          /* timestamp of last update*/
  uint16                     ft_sample_cnt;      /* num of freq-temp samples*/
  tcxomgr_field_state_type   state;              /* field calibration state */

} tcxomgr_cal_field_data_type;

#define FIELD_TO_FAC( data ) \
  /*lint -save -e740 We play this trick on purpose */ \
  ((tcxomgr_cal_fac_data_type*)((data))) \
  /*lint restore */

/* Factory calibration parameters */
typedef struct
{
  uint8                      band;               /* single tone band class   */
  uint16                     chan;               /* single tone CDMA channel */
  tcxomgr_hz_type            offset;             /* Hz, single tone freq
                                                    offset relative to the
                                                    carrier freq             */
  tcxomgr_temp_type          temp_span;          /* 2^-10 C, temp span for
                                                    factory calibration      */
  uint8                      pa_range;           /* PA range val             */
  int16                      agc_level;          /* TX AGC level val         */
  uint16                     min_ft_sample;      /* min num of F-T samples   */
  uint16                     max_ft_sample;      /* max num of F-T samples   */
  uint16                     substage_time;      /* time to wait between samp
                                                    substages (ms)           */
  uint16                     timeout_time;       /* maximum time to run each
                                                    step before declaring
                                                    failure (ms)             */
  tcxomgr_cal_record_enum_type update_nv;        /* update NV items flag     */
  tcxomgr_cal_state_enum_type state;             /* current state of fac cal */
  tcxomgr_cal_fac_data_type   nom_coef;         /* nominal coefficients     */
  tcxomgr_cal_fac_data_type   cal_coef;         /* calibrated coefficients  */
} tcxomgr_cal_fac_param_type;

/* Factory calibration informational structure */
typedef struct
{
  uint8                      init_xo_trim;       /* pre FacCal xo_trim val   */
  uint8                      dc_xo_trim;         /* post Coarse DC xo_trim   */
  uint8                      fin_xo_trim;        /* post Fine DC xo_trim     */
  uint8                      final_xo_trim;      /* post FacCal xo_trim val  */

  uint32                     s_ram_cap_cnt;      /* num of SRAM captures     */
  uint32                     freq_est_calc_cnt;  /* num freq est calculations*/

  tcxomgr_temp_set_type      init_temp;          /* initial temp (XO,PMIC,PA)*/
  tcxomgr_temp_set_type      final_temp;         /* final temp (XO,PMIC,PA)  */
  tcxomgr_cal_temp_range_type temp_range;        /* temp range encountered   */
} tcxomgr_cal_info_type;

/*------------------------ Global -------------------------------------------*/

/* structure for inclusion into the global tcxomgr structure */
typedef struct
{
  /* Field Cal coefficients (running) */
  tcxomgr_cal_field_data_type     field_data;

  /* Factory Cal coefficients (static, this should ONLY be written to in
     FacCal and when copied from NV in the field) */
  tcxomgr_cal_fac_data_type       fac_data;

  /* Conversion factors for Calibration coefficients */
  tcxomgr_cal_ft_conv_type        conv;

} tcxomgr_cal_fac_type;


/*------------------------ Macros -------------------------------------------*/

#define CAL_FT_QUAL_FROM_NV( nv, qual )                                       \
  switch ( nv )                                                               \
  {                                                                           \
    case TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL:                                      \
      qual = TCXOMGR_CAL_FT_QUAL_NOT_CAL;                                     \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FAC1:                                         \
      qual = TCXOMGR_CAL_FT_QUAL_FAC1;                                        \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FT1:                                          \
      qual = TCXOMGR_CAL_FT_QUAL_FT1;                                         \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FT2:                                          \
      qual = TCXOMGR_CAL_FT_QUAL_FT2;                                         \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FAILED:                                       \
    default:                                                                  \
      qual = TCXOMGR_CAL_FT_QUAL_FAILED;                                      \
      break;                                                                  \
  }

#define CAL_FT_QUAL_TO_NV( qual, nv )                                         \
  switch ( qual )                                                             \
  {                                                                           \
    case TCXOMGR_CAL_FT_QUAL_NOT_CAL:                                         \
      nv = TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL;                                    \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FAC1:                                            \
      nv = TCXOMGR_CAL_NV_FT_QUAL_FAC1;                                       \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FT1:                                             \
      nv = TCXOMGR_CAL_NV_FT_QUAL_FT1;                                        \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FT2:                                             \
      nv = TCXOMGR_CAL_NV_FT_QUAL_FT2;                                        \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FAILED:                                          \
    default:                                                                  \
      nv = TCXOMGR_CAL_NV_FT_QUAL_FAILED;                                     \
      break;                                                                  \
  }

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_CAL_DC_COARSE

DESCRIPTION
  This function collects initial FT samples and uses them to do initial
  coarse XO Trim tuning to match the sampled points to the nominal calibrated
  curve

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim could be changed

============================================================================*/
extern void tcxomgr_cal_dc_coarse
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_DC_FINE

DESCRIPTION
  This function collects final FT samples and uses them to do final
  fine XO Trim tuning to match the sampled points to the nominal calibrated
  curve

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim could be changed

============================================================================*/
extern void tcxomgr_cal_dc_fine
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_SAMP

DESCRIPTION
  This function collects CAL_FT_SAMPLES_PER_SUBSTAGE FT samples

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_samp
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_C1C2C3

DESCRIPTION
  This function takes the FT samples captured to determine the calibrated
  curve's C3, C2, C1, C0 factors.

DEPENDENCIES
  tcxomgr_cal_dc or tcxomgr_cal_samp must have been previously called

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_c1c2c3
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_INIT

DESCRIPTION
  This function initializes the three main structures used in factory
  calibration. It also sets some initial info_pkt information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_param_init
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_CALC_C2

DESCRIPTION
  This function uses the current "nominal" coefficients to calculate C2 based
  one the currently known/initialzed coefficients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_calc_c2
(
  tcxomgr_cal_fac_data_type  *coefs              /* current coefs            */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_CALC_C3

DESCRIPTION
  This function uses the current "nominal" coefficients to calculate C3 based
  one the currently known/initialzed coefficients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_calc_c3
(
  tcxomgr_cal_fac_data_type  *coefs              /* current coefs            */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_FT_EVAL

DESCRIPTION
  This function calculates the frequency error expected based on the given
  FT curve coefficients an the current temperature of the XO and/or PMIC

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in High Resolution PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_hr_ppm_type tcxomgr_cal_ft_eval
(
  tcxomgr_cal_fac_data_type  *ft_coef,           /* F/T equ coefficients    */
  tcxomgr_temp_type          t,                  /* temperature in 2^-10 C  */
  boolean                    pmic_comp,          /* PMIC compensation flag  */
  tcxomgr_temp_type          tp                  /* PMIC temp in 2^-10 C    */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_COPY_FAC_TO_FIELD

DESCRIPTION
  This function copies xo factory cal data to field data structure and
  initializes the extra fields in field data to their default values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The fields not found in factory data are initialized to default values.

============================================================================*/
extern void tcxomgr_cal_copy_fac_to_field ( void );

/*============================================================================

FUNCTION TCXOMGR_FIELD_CAL

DESCRIPTION
  Runs the field calibration state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_field_cal
(
  uint32                 time,
  tcxomgr_temp_set_type  temp,
  tcxomgr_rotator_type   rot
);

/*============================================================================

FUNCTION TCXOMGR_FIELD_SWITCH_XO_TRIM

DESCRIPTION
  This function pulls in the C1 error through the XO trim, moving a maximum
  of 1 XO trim step

DEPENDENCIES
  This function should only be called during power up (not in service)

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim can change

============================================================================*/

extern void tcxomgr_field_switch_xo_trim( void );

#endif /* FEATURE_XO_FACTORY_CAL && FEATURE_XO_FIELD_CAL */
#endif /* !TCXOMGR_CAL_H */
