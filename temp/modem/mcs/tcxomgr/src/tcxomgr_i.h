#ifndef TCXOMGR_I_H
#define TCXOMGR_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the internal definitions for the txcomgr subsystem

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006 - 2009 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/src/tcxomgr_i.h#8 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
05/01/09   ns       Feature wrap set_polarity changes 
04/21/09   sst      Update XO targets to use temp.xo, check for temp table sat
04/02/09   ns       Added polarity field to tcxomgr structure
07/11/08   scm      Define DBG_STR4() macro.
06/20/08   mca      Merge from //depot
06/04/08   sg       Removed Sleep state, added vco_source to tcxomgr struct
05/06/08   rkc      Removed function tcxomgr_get_time_and_temp()
04/15/08   sg       Added cached working_restr member
03/03/08   va       Use temperature set for RGS
11/16/07   va       XO Freq Est logging Support
10/16/07   sst      Move XO Trim NV functions to tcxomgr_data.c
10/02/07   va       Move slope estimation suff to tcxomgr_frq_est.c
                    Added prototypes of new functions
09/27/07   cab      Include msg.h for MSG macros
09/25/07   cab      Moved tcxomgr global object reference to header file
                    Moved field cal to tcxomgr task
09/19/07   va       Slope estimation needs rot client state
09/06/07   sst      Lint fixes
09/05/07   cab      Field cal logging support
                    Cache therm read due to timeline impacts
07/26/07   sst      Change get_time_and_temp to use temp_set_type
                    Moved tcxomgr_get_temp here from tcxomgr.h
07/13/07   cab      Added DBG_4 macro
06/26/07   sst      Correct prototype
06/18/07   aps      Added factory cal NV read/write functions
06/18/07   va       Support for XO Compensation
06/13/07   sst      Combine all cal members into one structure
06/08/07   sst      Added cal_coefs member to tcxomgr_struct_type
06/06/07   sst      Added Saturated and NULL ptr check macros
05/17/07   cab      Added uncertainty for passive clients
05/16/07   cab      Updated rpush_set_flag to allow multiple client request
01/19/07   cab      Added xo_trim vars to tcxomgr_struct_type
11/03/06   cab      Added rpush flag to monitor rotator pushing
10/10/06   cab      Initial version for tcxomgr v3

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"

#include "tcxomgr.h"
#include "nv.h"
#include "msg.h"

#ifdef FEATURE_XO_TASK
#include "tcxomgr_task.h"
#endif
#ifdef FEATURE_XO_FACTORY_CAL
#include "tcxomgr_cal.h"
#endif

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/* Define Absolute value */
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/* Define Sign */
#define  SIGN( x ) ( ((x) < 0) ? (-1) : (1) )

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define NOTUSED(i) if(i){}

/* This Macro should be used to null check interfaces.  If a Void return
   value is desired, then pass in the RETURN_VOID macro */
#define TCXOMGR_NULL_PTR_CHK(ptr, retval)                                \
  if (ptr == NULL)                                                       \
  {                                                                      \
    MSG_ERROR("Unexpected Null Ptr", 0, 0, 0); { return retval; }        \
  }

#define RETURN_VOID /* null macro */


/*---------------------------------------------------------------------------
  Temperature
---------------------------------------------------------------------------*/

/* If we decide that some averaging is required when reading the thermistor
   then increase this value, otherwise leave at 1.  Keep in mind that each
   read is costly in terms of time. */
#define NUM_TEMP_SAMPLES  1


/*---------------------------------------------------------------------------
  TCXO Manager's NV access data
---------------------------------------------------------------------------*/

typedef struct
{
  /* Buffer to send command to NV */
  nv_cmd_type        cmd;

  /* Buffer to place NV items in */
  nv_item_type       item;

  /* Function to call for when waiting for NV read/write to complete */
  void               (*wait)(rex_sigs_type);
}
tcxomgr_nv_struct_type;


/*============================================================================

                             UNIT TEST STUFF

============================================================================*/
#ifdef TCXOMGR_UNIT_TEST

  #define TCXOMGR_RUN_TEST_CASE_SIG   0x0004
  #define TCXOMGR_ACK_SIG             0x0008
  #define TCXOMGR_RELEASE_SIG         0x0010
  #define TCXOMGR_TIMER_SIG           0x0020

  extern rex_tcb_type          tcxomgr_client1_tcb;
  extern rex_tcb_type          tcxomgr_client2_tcb;

  extern void tcxomgr_test_init(void);

#endif

/*============================================================================

                             DEBUG MESSAGE MACROS

============================================================================*/

#define DBG_0( LVL, FMT ) \
        MSG( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT )

#define DBG_1( LVL, FMT, A ) \
        MSG_1( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A )

#define DBG_2( LVL, FMT, A, B ) \
        MSG_2( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B )

#define DBG_3( LVL, FMT, A, B, C ) \
        MSG_3( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C )

#define DBG_4( LVL, FMT, A, B, C, D ) \
        MSG_4( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C, D )

#define DBG_STR1( LVL, FMT, A ) \
        MSG_SPRINTF_1( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A )

#define DBG_STR2( LVL, FMT, A, B ) \
        MSG_SPRINTF_2( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B )

#define DBG_STR3( LVL, FMT, A, B, C ) \
        MSG_SPRINTF_3( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C )

#define DBG_STR4( LVL, FMT, A, B, C, D ) \
        MSG_SPRINTF_4( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C, D )

/*---------------------------------------------------------------------------
  TCXO Manager's VCTCXO PDM value -vs- Temperature Table
---------------------------------------------------------------------------*/

/* The size of the vco-vs-temp table */
#define VCO_TEMP_TABLE_SIZE       64
#define VCO_TEMP_TABLE_MAX        (63 << 2)

/* Value to use for undefined table entries */
#define VCO_UNDEFINED             -32768


/*---------------------------------------------------------------------------
  TCXO Manager's RGS and Freq Err defines
---------------------------------------------------------------------------*/

/* The value to use if the NV_VCO_DEFAULT value is not defined.  This 16-bit
** signed value used by TCXO MGR corresponds to a 12-bit unsigned value of
** 2047, which corresponds to the mDSP's default value for the VCTCXO PDM. */
#define VCO_DEFAULT               -1

/* The value to use if the NV_VCO_SLOPE value is not defined */
#define VCO_SLOPE_DEFAULT         346

/* The value to use if the NV_VCO_SLOPE_RANGE value is not defined */
#define VCO_SLOPE_RANGE_DEFAULT   255

/* Rotator must be within this limit for VCO Temperature table update */
#define UPDATE_ROTATOR_PPM_LIMIT  307 /* 0.3ppm */

#define TCXOMGR_RGS_DELTA_TIME_THRESH  86400
#define TCXOMGR_RGS_DELTA_TEMP_THRESH  4

/* If temperature of the phone is unavailable, for whatever reason, we will
** still consider RGS to be OK if it is not older than this in seconds. */
#define TCXOMGR_RGS_DELTA_TIME_NO_TEMP_THRESH  30

/* update_rgs_value() should be called every 30 seconds.
   Updates will be ignored if they occur too often. */
#define TCXOMGR_READ_THERM_SEC          30
#define TCXOMGR_UPDATE_TEMP_TBL_SEC     30

   /* Set the update threshold to .5ppm or:
 * 2^20 / NV_VCO_SLOPE / 2 */
#define TCXOMGR_GEN_UPDATE_THRESH( x ) \
    ( (x == 0) ? 0 : ((tcxomgr_vco_type)((0x100000 / ( x )) / 2)))

/* Set the initial threshold to 5ppm or:
 * 5 * 2^20 / NV_VCO_SLOPE */
#define TCXOMGR_GEN_INITIAL_THRESH( x ) \
    ( (x == 0) ? 0 : ((tcxomgr_vco_type)(5 * 0x100000 / ( x ))))


/* VCO_TEMP IIR filter: Gain = 1/4 */
#define VCO_TEMP_FILT(old, x)     (((old) * 3 + (x)) / 4)

#define FREQ_ERR_IS_SATURATED( acc ) \
        ( ( ABS(acc) > TCXOMGR_FT_MAX_PPM ) ? (TRUE) : (FALSE) )


/*---------------------------------------------------------------------------
  TCXO Manager's Temperature defines
---------------------------------------------------------------------------*/

/* Means "tcxomgr_temp_type" temperature is invalid or unavailable. */
#define TCXOMGR_INVALID_TEMP      ( (tcxomgr_temp_type)( 0x80000000 ) )

/* Convert thermistor value to table index  (saturated)
   Note negative temperatures are not currently supported */
#define TEMP_TO_INX(temp)                                   \
  ( ( ( (temp) > 0 ) && ( (temp) < VCO_TEMP_TABLE_MAX ) ) ? \
    ( (uint8)( (temp) >> 2 ) ) :                            \
    ( (uint8)( 0 ) ) )

/* check for a valid temperature range */
#define VALID_TEMP( temp )                              \
  ( ( ( TEMP_TO_INX( temp ) == 0 ) ||                   \
      ( TEMP_TO_INX( temp ) > VCO_TEMP_TABLE_SIZE ) ||  \
      ( (temp) == TCXOMGR_INVALID_TEMP ) ) ?            \
    FALSE : TRUE )

/*---------------------------------------------------------------------------
  TCXO Manager's XO defines
---------------------------------------------------------------------------*/

/* set the default trim value to be equal to that of the HW default value */
#define XO_TRIM_DEFAULT_VALUE           32

#ifdef FEATURE_XO
/* Time between field cal updates in seconds */
#define FIELD_CAL_UPDATE_PERIOD         30

#endif /* FEATURE_XO */

/*---------------------------------------------------------------------------
  TCXO Manager State
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_UNINITIALIZED,
  TCXOMGR_INITIALIZING,
  TCXOMGR_IDLE,
  TCXOMGR_REPLY_PENDING,
  TCXOMGR_CONTROLLED,
  TCXOMGR_MAX_STATE
} tcxomgr_state_type;

#ifdef FEATURE_XO_FIELD_CAL
typedef PACKED struct
{
  int32                      c3;
  int32                      c2;
  int32                      c1;
  int32                      c0;
  tcxomgr_temp_type          t0;
  tcxomgr_temp_type          t0p;
  int32                      p;
} tcxomgr_log_coeff_struct_type;

typedef PACKED struct
{
  tcxomgr_log_coeff_struct_type  fac_coeff;
  tcxomgr_log_coeff_struct_type  field_coeff;
  tcxomgr_cal_ft_qual_enum_type  ft_qual_ind;
  uint8                          xo_trim;
  uint16                         ft_sample_cnt;
  tcxomgr_field_state_type       state;
  tcxomgr_temp_type              temp_xo;
  tcxomgr_temp_type              temp_pmic;
  tcxomgr_rotator_type           rot;
  int8                           coeff;
  tcxomgr_ppm_type               err;
  int32                          best_cand_coeff;
} tcxomgr_log_field_cal_struct_type;
#endif /* FEATURE_XO_FIELD_CAL */

#ifdef FEATURE_XO_TASK
/* Max number of entries in Freq Est log packet */
#define TCXOMGR_LOG_FREQ_EST_MAX_ENTRIES  10

/* Log header */
typedef PACKED struct
{
  time_type          ts;                  /* Timestamp of log */
  uint8              num_freq_est_data;   /* Num of freq_est data entries */
  uint8              num_rpush_data;      /* num of rpush entries */
  boolean            rot_valid;           /* num of rot samples */
} tcxomgr_log_freq_est_hdr_struct_type;

/* Freq Est data */
typedef PACKED struct
{
  uint32             delta_time;    /* Time since log packet start */
  uint32             time_ms;       /* Time in ms since start of estimator */
  tcxomgr_temp_type  temp_xo;       /* XO Temperature */
  tcxomgr_temp_type  temp_pmic;     /* PMIC temperature */
  int32              therm_accum;   /* Thermal Accum */
  int32              total_accum;   /* Total accum after RFE correction */
  int32              rfe;           /* Residual Freq Error */
  int32              rfe_slope;     /* RFE slope */
  int32              total_slope;   /* Total sloep = RFE Slope + thermal slope */
} tcxomgr_log_freq_est_data_struct_type;

/* Rpush information - input to Freq Estimator */
typedef PACKED struct
{
  uint16                         rot_time;         /* Time of rpush */
  tcxomgr_rotator_type           rot_value;        /* Rpush value */
  tcxomgr_client_id_type         rot_client;       /* Rpush client */
  tcxomgr_client_state_type      rot_client_state; /* Rpush client state */
  tcxomgr_temp_type              xo_temp;          /* XO temp at rpush */
} tcxomgr_log_freq_est_rpush_struct_type;

/* Complete information which is logged
   The rot samples if present are synchronous to freq_est data and are useful
   for debugging the performance of the estimator loop */
typedef PACKED struct
{
  tcxomgr_log_freq_est_hdr_struct_type     freq_est_hdr;
  tcxomgr_log_freq_est_data_struct_type    data[TCXOMGR_LOG_FREQ_EST_MAX_ENTRIES];
  tcxomgr_log_freq_est_rpush_struct_type   rpush[TCXOMGR_LOG_FREQ_EST_MAX_ENTRIES];
  tcxomgr_rotator_type                     rot[TCXOMGR_LOG_FREQ_EST_MAX_ENTRIES];
  timetick_type                            log_start_tick;
} tcxomgr_log_freq_est_struct_type;
#endif /* FEATURE_XO_TASK */


typedef struct
{
  /* VCTCXO -vs- Temperature table */
  tcxomgr_vco_type                temp[ VCO_TEMP_TABLE_SIZE ];

  /* Default value for when table entry is VCO_UNDEFINED */
  tcxomgr_vco_type                dflt;

  /* Last update time (in seconds since 6 Jan 1980 00:00:00 */
  uint32                          last_update_secs;

  /* Slope: Sensitivity 2^-20 ppm per lsb of a 16bit PDM */
  uint16                          slope;

  /* Slope Range (%)  [255 == not calibrated] */
  uint8                           slope_range;
}
tcxomgr_vco_struct_type;


/*---------------------------------------------------------------------------
  TCXO Manager's Recent Good System Values
---------------------------------------------------------------------------*/

typedef struct
{
  /* System that set the most recent good system values */
  tcxomgr_client_id_type          system;

  /* Temperature measurement when VCO & ROT values were captured */
  tcxomgr_temp_set_type           temp;

  /* Time when most recent good system values were captured */
  uint32                          time;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_vco_type                vco;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_rotator_type            rot;
}
tcxomgr_rgs_struct_type;


/*---------------------------------------------------------------------------
  TCXO Manager's XO Trim/Capacitor/Coarse Rotator Values
---------------------------------------------------------------------------*/

typedef struct
{
  /* keep track if table has been updated */
  boolean                         switched;

  /* Current XO_trim value, as programmed/read from the PMIC */
  uint8                           curr_val;

  /* Current XO_trim value that has been applied to the temp/freq table */
  uint8                           table_val;
}
tcxomgr_xo_trim_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager static data
---------------------------------------------------------------------------*/

typedef struct
{
  /* TCXO Manager State */
  tcxomgr_state_type              state;

  /* Indicates if a service/restr request has arrived while waiting for ack */
  boolean                         pending_request;

  /* Indicates a pre-emption is in process */
  boolean                         restr_changed;

  /* rpush mask - each client has a unique bit, conglomerate rpush vote */
  uint32                          rpush_mask;

  /* Active client for the tcxomgr */
  tcxomgr_client_id_type          client_in_control;

  /* Client last commanded from whom we are expecting an ack */
  tcxomgr_client_id_type          ack_client;

  /* Active client for the tcxomgr */
  tcxomgr_client_id_type          best_client;

  /* Current restriction */
  tcxomgr_restriction_type        curr_restr;

  /* working restriction */
  tcxomgr_restriction_type        working_restr;

  /* VCTCXO -vs- Temperature table */
  tcxomgr_vco_struct_type         vco;

  /* VCO Temp Initial threshold */
  tcxomgr_vco_type                vco_temp_initial_thresh;

  /* VCO Temp Update threshold */
  tcxomgr_vco_type                vco_temp_update_thresh;

  /* Recent Good System values */
  tcxomgr_rgs_struct_type         rgs;

  /* XO Trim/Capacitor/Coarse Rotator fields */
  tcxomgr_xo_trim_struct_type     xo_trim;

  #ifdef FEATURE_XO_FACTORY_CAL
  /* Frequency Temperature Curve fields */
  tcxomgr_cal_fac_type            cal_data;
  #endif

  /* Storage for reading/writing to NV */
  tcxomgr_nv_struct_type          nv;

  /* is tcxo manager currently in control and driving the 1x
     override register?  */
  boolean                         override;

  /* what value are we driving the override register with? */
  tcxomgr_vco_type                override_val;

  /* test case number, set via diag for unit testing only */
  uint8                           test_case;

  /* Client who is currently being signaled with a callback function */
  tcxomgr_client_id_type          cb_id;

  /* used by passive to determine worst case vco type being used by active */
  tcxomgr_vco_indicator_type      uncertainty;

  /* Revisit - store cached value of xo temp so no therm read in timeline */
  tcxomgr_temp_set_type           cached_xo_temp;

  /* Revisit - store timestamp of when cached xo temp value was read */
  uint32                          cached_xo_temp_time;

  /* Current VCO Source */
  tcxomgr_source_type             vco_source;
  
  #ifdef FEATURE_TCXOMGR_SET_POLARITY
  /* Current trk_lo polarity */
  tcxomgr_trk_lo_enum_type        polarity;
  #endif /* FEATURE_TCXOMGR_SET_POLARITY */

}
tcxomgr_struct_type;


extern tcxomgr_struct_type tcxomgr;



/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_NV_INIT

DESCRIPTION
  This function initializes the NV member of the TCXO manager

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
);

/*============================================================================

FUNCTION TCXOMGR_NV_CMD

DESCRIPTION
  This function reads/write an item from/to the NV into tcxomgr.nv.item

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern nv_stat_enum_type tcxomgr_nv_cmd
(
  /* READ, WRITE, PEEK, POKE, etc. */
  nv_func_enum_type cmd,

  /* Item ID to read/write from/to NV */
  nv_items_enum_type id
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_xo_trim_read( void );

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_xo_trim_write( void );

/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_READ

DESCRIPTION
  This function reads the calibration data from NV.

  If NV read of factory cal data fails or if ft_qual of factory cal data is
  FAIL, ERR_FATAL the phone. If field cal data read fails, load the factory cal
  data into field cal structure.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_nv_cal_data_read( void );

/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_WRITE

DESCRIPTION
  This function writes xo calibration data values to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_nv_cal_data_write( void );

/*============================================================================

FUNCTION TCXOMGR_NV_FIELD_CAL_DATA_WRITE

DESCRIPTION
  This function writes the field calibration coefficients to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_nv_field_cal_data_write( void );

/*============================================================================

FUNCTION TCXOMGR_NV_VCO_READ

DESCRIPTION
  This function reads the VCO data from NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_vco_read( void );


/*============================================================================

FUNCTION TCXOMGR_NV_VCO_WRITE

DESCRIPTION
  This function writes the frequency-temperature table to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_vco_write( void );


/*============================================================================

FUNCTION TCXOMGR_NV_RGS_READ

DESCRIPTION
  This function reads the RGS (recent good system) values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_rgs_read( void );


/*============================================================================

FUNCTION TCXOMGR_NV_RGS_WRITE

DESCRIPTION
  This function writes the RGS (recent good system) values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_rgs_write( void );


/*============================================================================

FUNCTION TCXOMGR_NV_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_xo_trim_read( void );


/*============================================================================

FUNCTION TCXOMGR_NV_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_xo_trim_write( void );


/*============================================================================

FUNCTION TCXOMGR_GET_TEMP

DESCRIPTION
  This function reads the thermistors for the XO, PMIC, and PA

DEPENDENCIES
  None

RETURN VALUE
  temperature of XO, PMIC, and PA (in 2^-10 deg C)

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_temp_set_type tcxomgr_get_temp( void );


#ifdef FEATURE_XO_TASK
/*============================================================================

FUNCTION TCXOMGR_UPDATE_SLOPE_ERR

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_update_slope_err( void );

/*============================================================================

FUNCTION TCXOMGR_FREQ_EST_RESTRICTION_CHANGE

DESCRIPTION
  Process the Restriction command. If restriction means starting the
  estimator again, reset the timer & set the slope update sig

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_freq_est_restriction_change(tcxomgr_notify_cmd_type notify);

/*============================================================================

FUNCTION TCXOMGR_FREQ_EST_PROCESS_RPUSH

DESCRIPTION
  Process the RPUSH command - save and temeprature tag rpush info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_freq_est_process_rpush(tcxomgr_rpush_cmd_type *rpush_ptr);
#endif /* FEATURE_XO_TASK */


#endif /* !TCXOMGR_I_H */
