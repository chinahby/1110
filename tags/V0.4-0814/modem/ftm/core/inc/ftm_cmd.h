#ifndef __FTM_CMD_H
#define __FTM_CMD_H

/*===========================================================================

                  FACTORY TEST COMMAND INTERFACE
DESCRIPTION

  This file contains prototypes, #defines, and enums which constitute
  the public interface for FTM functions intended to be called via
  the diag mechanism.

    Copyright (c) 1999--2002
                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_cmd.h#1 $
  $DateTime: 2008/04/14 04:57:43 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/07   aak     Added Enable_RPC parameter in ftm_cmd_fwd_hho_param_typenj 
01/10/07   aak     Featurize the FTM Hard Handoff feature (FTM_HAS_SRCH_HHO)
12/01/06   aak     Added support for FTM hard handoff feature
08/17/06   ak      Augmented the sturctures "hdr_ftm_cmd_mod_rtc_param_type" and
                   "hdr_ftm_cmd_mod_rtc_rela_param_type" by adding to them the
                   parameter "byte hdr_ack_mode"
12/12/05   vm      Fixed the parameters ordering for structure
                   hdr_ftm_cmd_mod_rtc_rela_param_type.
                   Removed the ambiguous function prototype.
12/06/05   ra      Added hdr_ftm_rela_mac_config - DO Reva API prototype
10/30/05   vm      Added support for new FTM DO Reva APIs
09/20/05   jac     Added enums, structures and prototypes for new API for 
                   1x EVDO Rev A testing.
09/08/05   ra      Added ftmicap.h
07/19/05   ra      Added more DO REV A support
05/03/05   ra      Added DO REV A support
07/06/04   bmg     Added prototypes for release A functions
05/18/04   rsr     Removed WINNT and SWIG support. Not being used
11/11/03   bmg     Merge Cougar release A
09/23/03   wd      Added new types and defs for simultaneous FCH and DCCH (Release A)
09/09/03   bmg     Added ftm_mc_cmd_resp_type for FTM MC command response
07/23/03   bmg     Converted test_* names to ftm_* names
02/10/03   wd      Marge files for FTM Task, Also added p4 info and clean up.
01/24/03   wd      Decouple legacy FTM from FTM_HWTC and FTM_RF
02/21/02   rjr     Support for RF commands and dependancy on rftest.h.
08/22/01   bn      Support DCCH in FTM mode
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
11/15/00   bgc     ftm_cmd_status has been changed to test_cmd_status.
08/22/00   bgc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "test_cmd.h"
#include "diagpkt.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#ifdef FEATURE_HDR
#error code not present
#endif



/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*--------------------------------------------------------------------------
                        ACQUISITION MODE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_ACQ_MODE_FULL,
  FTM_ACQ_MODE_MINI,
  FTM_ACQ_MODE_MICRO,
  ftm_acq_mode_sizer = 0x7FFFFFFF
} ftm_acq_mode_type;

#ifdef FEATURE_HDR
#error code not present
#endif

/*--------------------------------------------------------------------------
                        BAND CLASS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_BAND_CLASS_CELLULAR,
  FTM_BAND_CLASS_PCS,
  ftm_band_class_sizer = 0x7FFFFFFF
} ftm_band_class_type;

#ifdef FEATURE_HDR
#error code not present
#endif

/*--------------------------------------------------------------------------
                        RADIO CONFIGURATION ENUM TYPES
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_FWD_RC_1=0x01,
  FTM_FWD_RC_2,
  FTM_FWD_RC_3,
  FTM_FWD_RC_4,
  FTM_FWD_RC_5,
  FTM_FWD_RC_1X_MAX,
  ftm_fwd_rc_sizer = 0x7FFFFFFF
} ftm_fwd_rc_type;

typedef enum 
{
  FTM_REV_RC_1=0x01,
  FTM_REV_RC_2,
  FTM_REV_RC_3,
  FTM_REV_RC_4,
  FTM_REV_RC_1X_MAX,
  ftm_rev_rc_sizer = 0x7FFFFFFF
} ftm_rev_rc_type;

typedef enum 
{
  FTM_REV_FCH_FULL_RATE=0x00,
  FTM_REV_FCH_HALF_RATE=0x01,
  FTM_REV_FCH_QTR_RATE=0x02,
  FTM_REV_FCH_8TH_RATE=0x03,
  ftm_rev_fch_rate_sizer = 0x7FFFFFFF
} ftm_rev_fch_rate_type;

typedef enum 
{
  FTM_REV_SCH_1_5_RATE=0x00,
  FTM_REV_SCH_2_7_RATE=0x01,
  FTM_REV_SCH_4_8_RATE=0x02,
  FTM_REV_SCH_9_6_RATE=0x03,
  FTM_REV_SCH_19_2_RATE=0x04,
  FTM_REV_SCH_38_4_RATE=0x05,
  FTM_REV_SCH_76_8_RATE=0x06,
  FTM_REV_SCH_153_6_RATE=0x07,
  ftm_rev_sch_rate_sizer = 0x7FFFFFFF
} ftm_rev_sch_rate_type;


/*--------------------------------------------------------------------------
                        CHANNEL SPREAD RATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  FTM_RATE_1X,
  FTM_RATE_2X,
  FTM_RATE_4X,
  FTM_RATE_8X,
  FTM_RATE_16X,
  ftm_spread_rate_sizer = 0x7FFFFFFF
} ftm_spread_rate_type;


/*--------------------------------------------------------------------------
                        CODING TYPE ENUM TYPE
--------------------------------------------------------------------------*/

typedef enum
{
  FTM_DECODE_CONVOLUTIONAL,
  FTM_DECODE_TURBO,
  ftm_decoding_sizer = 0x7FFFFFFF
} ftm_coding_type;


/*--------------------------------------------------------------------------
                        FTM MODE ENUM TYPE
--------------------------------------------------------------------------*/

typedef enum
{
  FTM_MODE_DMSS,
  FTM_MODE_FTM,
  ftm_mode_sizer = 0x7FFFFFFF
} ftm_cmd_mode_type;

#ifdef FEATURE_HDR
#error code not present
#endif

/*--------------------------------------------------------------------------
                        WALSH CODE/QOF  SPECIFIER TYPE
--------------------------------------------------------------------------*/
typedef PACKED struct 
{
  byte walsh;    /* walsh channel number (0-63 for 95A/B, 0-127 for cdma2000) */

  byte qof_mask_id;  /* quasi orthoganal function mask (0-3) where 0 is 
                        true walsh codes */
} ftm_walsh_type;

/*--------------------------------------------------------------------------
             Structure for the FTM Fast Forward Power Control Parameters
--------------------------------------------------------------------------*/
typedef PACKED struct
{
  byte  target_fer;
  byte  min_setpt;
  byte  max_setpt;
} ftm_fpc_olc_param_type;


typedef PACKED struct
{
  ftm_band_class_type band_class;
  /* the band class, Cellular or PCS */

  word cdma_freq;
  /* the cdma frequency channel */

  ftm_acq_mode_type acq_mode;
  /* the acquisition mode, Full, Mini, Micro */
} ftm_cmd_acq_pilot_param_type;

typedef PACKED struct
{
  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this FCH */

  ftm_walsh_type walsh;
  /* walsh index */

  boolean bypass_lc;
  /* bypass long code (long code set to all zero) */

  byte frame_offset;
  /* frame offset */

  byte subchan_gain;
  /* Power control subchannel gain */

  boolean ffpc_enable;
  /* fast forward power control enable */

  dword fpc_mode;
  /* forward power control mode */

  byte init_setpt;
  /* initial set point for inner loop FPC on FCH */

  ftm_fpc_olc_param_type  olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_fch_param_type;

typedef PACKED struct
{

  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this FCH */

  ftm_walsh_type walsh;
  /* walsh index */

  boolean bypass_lc;
  /* bypass long code (long code set to all zero) */

  word so_pri;
  /* Primary service option */

  word so_sec;
  /* Secondary service option */

  byte frame_offset;
  /* frame offset */

  boolean ilpc_enabled;
  /* to indicate if inner loop power control is enabled or not */

  boolean olpc_enabled;
  /* to indicate if outer loop power control is enabled or not */

  byte init_setpt;
  /* initial set point for inner loop FPC on DCCH */

  byte fpc_subchan_gain;
  /* subchan gain value for inner loop FPC */

  ftm_fpc_olc_param_type  dcch_olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_dcch_param_type;

typedef PACKED struct
{
  ftm_cmd_demod_fch_param_type fch;
     /* FCH parameters */

  ftm_cmd_demod_dcch_param_type dcch;
     /* DCCH parameters */

} ftm_cmd_demod_fch_dcch_param_type;

typedef PACKED struct
{
  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this SCH */

  ftm_walsh_type walsh;
  /* walsh index */

  ftm_spread_rate_type sch_rate;
  /* from 1X - 16X */

  ftm_coding_type coding_type;
  /* Convolutional or Turbo */

  boolean ffpc_enable;
  /* fast forward power control enable */

  dword fpc_mode;
  /* forward power control mode */

  byte init_setpt;
  /* initial set point for inner loop FPC on FCH */

  ftm_fpc_olc_param_type  olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_sch_param_type;

typedef PACKED struct
{
  ftm_rev_rc_type radio_cfg;
  /* Reverse FCH radio config */

  byte frame_offset;
  /* frame offset */

  ftm_rev_fch_rate_type fch_rate;

  boolean enable_rpc;
  /* Enable/disable reverse power control */

  word num_preambles;
  /* Preamble count for traffic channel initialization */

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */
} ftm_cmd_mod_fch_param_type;

typedef PACKED struct
{
  ftm_rev_rc_type dcch_rc;

  byte frame_offset;
  /* frame offset */

  boolean sch_on;

  ftm_rev_rc_type sch_rc;

  boolean sch_turbo;

  word    sch_ltu_size;

  ftm_rev_sch_rate_type sch_rate;

  word num_preambles;

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */

}  ftm_cmd_mod_dcch_param_type;

typedef PACKED struct
{
  ftm_rev_rc_type radio_cfg;
  /* Reverse FCH radio config */

  ftm_rev_sch_rate_type sch_rate;
  /* Rate of reverse SCH from */

  ftm_coding_type coding_type;
  /* turbo or convolutional */

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */
} ftm_cmd_mod_sch_param_type;

typedef PACKED struct
{
  ftm_cmd_mod_fch_param_type  fch;    /* reverse FCH */
  ftm_cmd_mod_dcch_param_type dcch;    /* reverse DCCH */

} ftm_cmd_mod_fch_dcch_param_type;

#ifdef FTM_HAS_SRCH_HHO
typedef PACKED struct
{
  ftm_band_class_type band_class;
  /* the band class, Cellular or PCS */

  word cdma_freq;
  /* the cdma frequency channel */

  boolean enable_rpc;

} ftm_cmd_fwd_hho_param_type;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/* ftm_packed_param_type is a union of all the above command 
 * parameters.  All ftm commands take this type as the argument */

typedef PACKED union 
{
  ftm_cmd_acq_pilot_param_type          acq;        /* acquisition */
  ftm_cmd_demod_fch_param_type          f_fch;      /* forward FCH */
  ftm_cmd_demod_dcch_param_type         f_dcch;     /* forward DCCH */
  ftm_cmd_demod_sch_param_type          f_sch;      /* forward SCH */
  ftm_cmd_demod_fch_dcch_param_type     f_fch_dcch; /* forward fch and dcch */
  ftm_cmd_mod_fch_param_type            r_fch;      /* reverse FCH */
  ftm_cmd_mod_dcch_param_type           r_dcch;     /* reverse DCCH */
  ftm_cmd_mod_sch_param_type            r_sch;      /* reverse SCH */
  ftm_cmd_mod_fch_dcch_param_type       r_fch_dcch; /* reverse fch and dcch */
  ftm_cmd_mode_type                     mode;       /* change mode of phone */
#ifdef FTM_HAS_SRCH_HHO
  ftm_cmd_fwd_hho_param_type            f_hho;      /* forward hard handoff*/
#endif
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
} ftm_packed_param_type;

/*===========================================================================

FUNCTION FTM_ACQUIRE

DESCRIPTION
  This procedure processes a FTM command for acquiring a pilot.   The
  arguments are param->acq.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_acquire
(
  ftm_packed_param_type *param
); /* ftm_aquire */

/*===========================================================================

FUNCTION FTM_SYNC

DESCRIPTION
  This procedure processes a FTM command for demodulating SYNC Channel. 
  The param is ignored.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_sync
(
  ftm_packed_param_type *param
); /* ftm_sync */



/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_FCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Fundamental Channel. 
  The arguments are param->f_fch.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
/* PARAMETERS */

/* FUNCTION */
ftm_cmd_status_type ftm_assign_fwd_fch
(
  ftm_packed_param_type *param
); /* ftm_assign_fwd_fch */


/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_DCCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Dedicated control Channel. 
  The arguments are param->f_dcch.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
/* PARAMETERS */

/* FUNCTION */
ftm_cmd_status_type ftm_assign_fwd_dcch
(
  ftm_packed_param_type *param
); /* ftm_assign_fwd_dcch */



/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_SCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Suplimental Channel. 
  The arguments are param->f_sch.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

/* FUNCTION */
ftm_cmd_status_type ftm_assign_fwd_sch
(
  ftm_packed_param_type *param
); /* ftm_assign_fwd_sch */

/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_FCH_DCCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Fundamental Channel 
  and Delicated Control Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

/* FUNCTION */
ftm_cmd_status_type ftm_assign_fwd_fch_dcch
(
   ftm_packed_param_type *p_param
); /* ftm_assign_fwd_fch_dcch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_FCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  fundamental channel (R-FCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/


/* FUNCTION */
ftm_cmd_status_type ftm_assign_rev_fch
(
  ftm_packed_param_type *param
);

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_DCCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  dedicated control channel (R-DCCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/


/* FUNCTION */
ftm_cmd_status_type ftm_assign_rev_dcch
(
  ftm_packed_param_type *param
); /* ftm_assign_rev_dcch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_SCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  supplimental channel (R-SCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

/* FUNCTION */
ftm_cmd_status_type ftm_assign_rev_sch
(
  ftm_packed_param_type *param
);

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_FCH_DCCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  fundamental channel (R-FCH) and dedicated control channel (R-DCCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_rev_fch_dcch
(
  ftm_packed_param_type *p_param
); /* ftm_assign_rev_fch_dcch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_FCH

DESCRIPTION
  This procedure processes a FTM command for enable FCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_loopback_fch
(
  ftm_packed_param_type *param
); /* ftm_loopback_fch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_DCCH

DESCRIPTION
  This procedure processes a FTM command for enable DCCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_loopback_dcch
(
  ftm_packed_param_type *param
); /* ftm_loopback_dcch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_SCH

DESCRIPTION
  This procedure processes a FTM command for enable SCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_loopback_sch
(
  ftm_packed_param_type *param
); /* ftm_loopback_sch */

/*===========================================================================

FUNCTION FTM_DEASSIGN_TRAFFIC

DESCRIPTION
  This procedure processes a FTM command for release and teardown of all
  forward and reverse channels.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_deassign_traffic
(
  ftm_packed_param_type *param
); /* ftm_deassign_traffic */
/*------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION FTM_SET_MODE

DESCRIPTION
  This procedure processes a FTM command for turning on SCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_set_mode
(
  ftm_packed_param_type *p_param
);

#ifdef FTM_HAS_SRCH_HHO
/*===========================================================================

FUNCTION FTM_FWD_HHO

DESCRIPTION
  This procedure processes a FTM command for a Hard Handoff on Traffic. 
  The param is ignored.
          
DEPENDENCIES
  None.

RETURN VALUE
  FTM_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_fwd_hho
(
  ftm_packed_param_type *param
); /* ftm_fwd_hho */
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

  
===========================================================================*/
typedef enum {
  FTM_FIRST_CMD = 100,
  FTM_ACQUIRE_SC = 100,
  FTM_SYNC_SC =    101,
  FTM_FWD_FCH_SC = 102,
  FTM_FWD_SCH_SC = 103,
  FTM_REV_FCH_SC = 104,
  FTM_REV_SCH_SC = 105,
  FTM_FCH_LOOP_SC = 106,
  FTM_SCH_LOOP_SC = 107,
  FTM_RELEASE_SC = 108,
  FTM_SET_MODE_SC = 109,
  FTM_FWD_DCCH_SC = 110,
  FTM_REV_DCCH_SC = 111,
  FTM_DCCH_LOOP_SC = 112,
#ifdef FEATURE_HDR
#error code not present
#endif
  FTM_FWD_FCH_DCCH_SC = 120,
  FTM_REV_FCH_DCCH_SC = 121,
#ifdef FEATURE_HDR
#error code not present
#endif
#ifdef FTM_HAS_SRCH_HHO
  FTM_FWD_HHO_SC = 128,
#endif
  FTM_MAX_CMD 
} ftm_subcmd_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type diag_hdr;
  word                       sub_cmd;
  uint8                      status;
} ftm_mc_cmd_resp_type;

/* FUNCTION */
/*============================================================================

FUNCTION      FTM_COMMAND_DISPATCHER

DESCRIPTION   This function will call the appropriate subcommand
              for a given FTM command.
DEPENDENCIES   

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the FTM_MCC command queue
      
============================================================================*/
ftm_cmd_status_type ftm_cmd_dispatcher
(
  byte sub_cmd_code,
  word length,
  ftm_packed_param_type *param
);

#ifdef T_WINNT  /* return to normal packing */
#error code not present
#endif
       
#ifdef FEATURE_HDR
#error code not present
#endif

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* __FTM_CMD_H */

