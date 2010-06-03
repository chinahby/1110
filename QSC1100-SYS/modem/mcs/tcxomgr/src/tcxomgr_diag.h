#ifndef TCXOMGR_DIAG_H
#define TCXOMGR_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    T C X O M G R    D I A G

GENERAL DESCRIPTION
  This module contains functions pertaining to the search diag interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2006-2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/src/tcxomgr_diag.h#5 $

when         who      what, where, why
--------     ---      ----------------------------------------------------------
10/20/08     adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
08/28/08     adw      Added FEATURE_XO_DIAG to featurize for ULC.
04/07/08     ljl      Supported multi-carrier AFC log.
11/16/07     va       Remove TCXO Diag cal functions. 
                      XO Freq Est logging Support
                      Bumped AFC Log version num
09/13/07     sst      Correct featurization
09/04/07     cab      Added support for diag control of slope error
                      Field cal logging support
07/20/07     sst      Update FacCal to increase offset size and enable NV write
06/06/07     sst      Added debug msg levels and Cal diag commands
10/10/06     cab      Initial version for tcxomgr v3
===========================================================================*/

#include "customer.h"

#ifdef FEATURE_TCXOMGR_DIAG

#include "comdef.h"
#include "msg.h"
#include "log_codes.h"
#include "log.h"

#include "diagpkt.h"
#include "tcxomgr_i.h"

/*--------------------------------------------------------------------------
                        Message Mask Definations
--------------------------------------------------------------------------*/
/* the following are required for all subsystems and are defined in msg.h
 *
 * MSG_LEGACY_LOW      MSG_MASK_0
 * MSG_LEGACY_MED      MSG_MASK_1
 * MSG_LEGACY_HIGH     MSG_MASK_2
 * MSG_LEGACY_ERROR    MSG_MASK_3
 * MSG_LEGACY_FATAL    MSG_MASK_4
 *
 * The remaining bits are sub subsystem specific. 1X search specific
 * ones are defined below. To enable, ensure the bit mask is set
 * correctly in custsrch.h
 */

// For future use: #define MSG_DBG_INIT          MSG_MASK_5

#define MSG_DBG_XO_TRIM          MSG_MASK_5      /* XO Trim               */
#define MSG_DBG_FAC_CAL          MSG_MASK_6      /* Factory Calibration   */
#define MSG_DBG_FIELD_CAL        MSG_MASK_7      /* Field Calibration     */


/*--------------------------------------------------------------------------
                    Subsystem Dispatch Request Response
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                    AFC LOG PACKET INFO
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_TCXOMGR_AFC_DATA_C)

   uint8                               version;
     /* The version number of the packet. */

   tcxomgr_log_header_struct_type      afc_header_info;

   tcxomgr_log_entries_struct_type     entries[1];

LOG_RECORD_END

/* Macro for variable size log packet allocation. */
#define TCXOMGR_VAR_SIZEOF( main_type, sub_type, sub_cnt ) \
           ( sizeof( main_type ) + ( (sub_cnt-1) * sizeof( sub_type ) ) )

/* To calculate the log buffer size 
   with variable carriers and variable entries. 
   Will clean up the calculation when version 5 log is phased out. */
#define TCXOMGR_MC_VAR_SIZE( mc_log_header_size,                         \
                             mc_log_carriers_size,                       \
                             mc_log_entries_size )                       \
          (sizeof(LOG_TCXOMGR_AFC_DATA_C_type)                      -    \
           sizeof(tcxomgr_log_header_struct_type)                   -    \
           sizeof(tcxomgr_log_entries_struct_type)                  +    \
           mc_log_header_size                                       +    \
           mc_log_carriers_size                                     +    \
           mc_log_entries_size )

#define TCXOMGR_AFC_LOG_VERSION            5
#define TCXOMGR_MC_AFC_LOG_VERSION         6

#ifdef FEATURE_XO_FIELD_CAL
/*--------------------------------------------------------------------------
                    FIELD CAL LOG PACKET INFO
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_TCXOMGR_FIELD_CAL_C)

   uint8                               version;
     /* The version number of the packet. */

   tcxomgr_log_field_cal_struct_type   field_cal_data;

LOG_RECORD_END

#define TCXOMGR_FIELD_CAL_LOG_VERSION      1
#endif /* FEATURE_XO_FIELD_CAL */

#ifdef FEATURE_XO_TASK
/*--------------------------------------------------------------------------
                    FREQ EST LOG PACKET INFO
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_XO_FREQ_EST_C)

   uint8                               version;
     /* The version number of the packet. */

   tcxomgr_log_freq_est_struct_type    freq_est_data;

LOG_RECORD_END

#define TCXOMGR_FREQ_EST_LOG_VERSION      1
#endif /* FEATURE_XO_TASK */

/* list of search diag request indexes */
typedef enum
{
  TXCOMGR_DIAG_MAX_EXTERNAL_ID,

  TXCOMGR_DIAG_START_INTERNAL_ID = 0xF0,
  TEMP_TABLE_OVERRIDE            = 0xF1,
  TEMP_TABLE_READ                = 0xF2,
  VCO_DFLT_OVERRIDE              = 0xF3,
  VCO_SLOPE_OVERRIDE             = 0xF4,
  SLOPE_RANGE_OVERRIDE           = 0xF5,
  RGS_OVERRIDE                   = 0xF6,
  RUN_TEST_CASE                  = 0xF7,
  RFE_SLOPE_OVERRIDE             = 0xF8,

  TXCOMGR_DIAG_MAX_ID
} tcxomgr_diag_table_id_type;

/* Typedef for the override command from the QXDM */
typedef enum
{
  NO_OVERRIDE       = 0,
  OVERRIDE,
  OVERRIDE_AND_WRITE
} override_cmd_type;


#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */
typedef PACKED struct
{
  uint8                         system;
  int16                         delta_temp;
  int16                         vco;
  int16                         rot;
  uint32                        time;
}
tcxomgr_diag_rgs_struct_type;
#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */


/*--------------------------------------------------------------------------
                             TEMP TABLE OVERRIDE

This interface provides a way for a test tool to override the current
temperature vs vco table.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, TEMP_TABLE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  tcxomgr_vco_type  temp[ VCO_TEMP_TABLE_SIZE ];
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, TEMP_TABLE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             TEMP TABLE READ

This interface provides a way for a test tool to read the temperature vs
vco table.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, TEMP_TABLE_READ)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, TEMP_TABLE_READ)
  tcxomgr_vco_type  temp[ VCO_TEMP_TABLE_SIZE ];
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             DEFAULT VCO OVERRIDE

This interface provides a way for a test tool to override the current
 vco default value.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, VCO_DFLT_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  tcxomgr_vco_type  dflt;      /* Default VCO value */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, VCO_DFLT_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                            VCO SLOPE OVERRIDE

This interface provides a way for a test tool to override the current
 vco slope.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, VCO_SLOPE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  uint16            slope;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, VCO_SLOPE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             SLOPE RANGE OVERRIDE

This interface provides a way for a test tool to override the current
 slope range.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, SLOPE_RANGE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  uint8             slope_range;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, SLOPE_RANGE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             RGS OVERRIDE

This interface provides a way for a test tool to override the current
 recent good system values.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RGS_OVERRIDE)
  override_cmd_type             enable;    /* 0 = disable, 1 = enable */
  tcxomgr_diag_rgs_struct_type  rgs;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RGS_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             RUN TEST CASE

This interface provides a way for a test tool to override the current
 recent good system values.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RUN_TEST_CASE)
  uint8             test_case;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RUN_TEST_CASE)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             RFE SLOPE OVERRIDE

This interface provides a way for a test tool to override the AFC slope
 component of frequency estimation.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RFE_SLOPE_OVERRIDE)
  boolean             enable;    /* 0 = disable, 1 = enable */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RFE_SLOPE_OVERRIDE)
DIAGPKT_RSP_END


/*===========================================================================

FUNCTION TCXOMGR_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      TCXOMGR logging dispatch table.

DEPENDENCIES   tcxomgr_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_diag_init (void);


/*===========================================================================

FUNCTION SRCH_DIAG_GET_TEMP_TABLE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the temp table override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_temp_table_override_enable(void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_DFLT_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_dflt override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_vco_dflt_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_vco_slope_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_SLOPE_RANGE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the SLOPE_RANGE override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_slope_range_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the RGS override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_rgs_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RFE_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the rfe_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_rfe_slope_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TIME

DESCRIPTION    This function returns the delta time for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 tcxomgr_diag_get_rgs_time( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TEMP_DIFF

DESCRIPTION    This function returns the temperature difference for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   int16 delta temp.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 tcxomgr_diag_get_rgs_temp_diff( void );

#ifdef FEATURE_XO_FIELD_CAL
/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FIELD_CAL_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tcxomgr_diag_send_field_cal_log
(
  tcxomgr_log_field_cal_struct_type *field_cal_data
);
#endif /* FEATURE_XO_FIELD_CAL */

#ifdef FEATURE_XO_TASK
/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FREQ_EST_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tcxomgr_diag_send_freq_est_log
(
  tcxomgr_log_freq_est_struct_type *freq_est_data
);
#endif /* FEATURE_XO_TASK */

#endif /* FEATURE_TCXOMGR_DIAG */

#endif /* TCXOMGR_DIAG_H */

