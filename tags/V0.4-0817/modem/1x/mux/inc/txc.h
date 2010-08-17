#ifndef TXC_H
#define TXC_H
/*===========================================================================

          T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the transmit task that don't fit
  into one of the other H files such as CAI.H

Copyright (c) 1991 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/txc.h_v   1.27   16 Oct 2002 16:35:20   randyp  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/txc.h#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Allow function txc_tx_shutdown() to be visible for 
                   FEATURE_RL_JUMP_ENHANCEMENT. 
04/13/05   bn/fq   Moved all the common defines and structures to txcmc.h
01/25/05   vlc     Removed Feature Rel D from Jump to Hyperspace command.
01/17/05   ljl     Updated for Rel D interface changes by CP.
01/11/05   vlc     Added support for Rel D Fast Call Set Up - New Jump to 
                   Hyperspace command feature.
09/27/04   ljl     Replaced pdch with fwd_pdch and added revd_pdch.
06/16/04   sr      Merged in changes from 6500 5004 build
05/21/04   jyw     introduce new RF PA backoff interface
04/22/04   jrp     Changes to txc_tc_cmd_type in support of Rev D.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
02/03/04   sab     Featurized confirmation_type under Release B.
09/15/03   jrp     Added txc_tx_shutdown().
08/06/03   sab     Added confirmation_type to payload of TXC_MSG_F.
04/15/03   jrp     Support for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
04/07/03   jrp     Lint changes.
10/16/02   jrp     Added prototype for txc_write_pcbit_data().
08/01/02   jrp     Removed txc_flag_eib(), txc_flag_qib(), and txc_flag_sch_eib().
                   These are now modified directly by rxctraffic.
07/01/02   tc      Added support for rlgain_common_pilot, ic_thresh, ic_max.
06/18/02   jrp     Added txc_flag_qib() and txc_flag_sch_eib() in support of
                   FFPC Modes 4,5,6.
06/07/02   hrk     Added support to specify physical channel on which signaling
                   should be sent for VP2.
05/22/02   bn      Added new logics to support VP2 RPC 
04/03/02   jrp     Added TXC_REACH_PROBE_F.
03/04/02   bgc     Changed features to IS2000_R_SCH where appropriate.
02/25/02   hjr     Fixed featurization of FEATURE_IS2000_SCH with 
                   FEATURE_IS2000_DS_RS2 on.
02/25/02   bn      Added support for  MUX SVD signaling 
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   tc      Supported Reverse Enhance Access Channel.
12/10/01   hrk     Renamed txc_flag_erasure txc_flag_eib.
08/22/01   bn      Support DCCH in FTM mode
08/21/01   sj      Changed rs1_20ms_incl -> rc3_rc5_20ms_incl  &
                           rs2_20ms_incl -> rc4_rc6_20ms_incl
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/15/01   snn     Added externs for new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     modified the prototype for txc_get_sch_info().
05/18/01   kk      RS2 TDSO support.
05/14/01   vlc     Removed test feature so SCH RC4 can be used in DMSS software.
04/26/01   vlc     Changed name of constants to accomodate new SCH RC4
                   development on MSM5100.  Added rev_sch_rc to txc_tc_cmd_type
                   under Factory Test Mode only.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00 
           lcc     Added support for FEATURE_IS2000_P2.
03/23/01   sr      Added support for 32X
03/20/01   ych     Merged JCDMA Features.
03/07/01   va      Added p_rev_in_use info in TC_F and RETURNED_F interfaces.
01/30/01   tc      Merged GPS support.
12/01/00   ak      Added function extern for txc_inform_sch_chng.
10/04/00   jq      Added FTM FCH and SCH loopback feature
09/06/00   bgc     Added TXC_FTM_SCH_F to setup SCH channels separate of 
                   FCH for FEATURE_FACTORY_TESTMODE.
08/14/00   sr      Added support for pilot gating and FCH 1/8 rate gating in
                   PLT
07/17/00   va      Added support for 8th rate gating interface.
05/30/00   hrk     Changed prototype for txc_set_pch_fch_gain().
05/11/00   hrk     Added prototype for txc_set_fpc_mode().
04/24/00   lcc     Added TXC_MC_TRANS_F and related command structure.
04/11/00   na      Added prototype for txc_get_sch_info() under
                   FEATURE_DS_IS2000.
02/14/00   hrk     Added prototype for function txc_set_pch_fch_gain ()
02/10/00   hrk     Added prototype for function txc_get_fch_rate_set ()
1/31/00    sr      Added action time processing and implementation of
                   txc_ho_cmd, txc_hho_ok_cmd and txc_returned_cmd.
                   Parameters related to reverse link gain adjustment and
                   power control step size haven't yet been implemented
01/07/00   sr      Added num_preambles to txc_tc_cmd_type
12/22/99   hrk     Moved definition of txc_gain_type out of FEATURE_PLT
12/20/99   va      Added pwr_ctrl_step to the txc_pwr_ctrl_cmd_type,
                   txc_ho_cmd_type and txc_returned_cmd_type interfaces.
11/23/99   sr      Added prototype for txc_data_pump_init()
11/19/99   sr      Minimum implementation of new mc-txc interface
                   including support for new TXC_SO_F, TXC_CONNECT_SO_F
                   and TXC_RELEASE_F commands
                   Vanitha's new mc-txc interface
                   Added changes to support data pump and supplemental channels
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
                   Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
                   Updated the extern definition for txc_register_action_time()
                   Added extern definition for txc_get_time_in_1_25ms_unit()
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Added prototypes for txc_set_frame_offset(),
                   txc_hho_action_time_chk(), txc_flag_next_int() and
                   txc_int_occurred()
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added txc_suspend_tx function to suspend/enable Access
                   probes when T72M timer expires.
05/10/99   kmp     Merged in the IS-95B changes listed below
           fc      Put TXC_ACC_PROBE_F command under FEATURE_IS95B_PILOT_RPT.
           fc      Added TXC_ACC_PROBE_F command.
08/25/95   gb      Inform TX of the HHO parameters num_preamble and nom_pwr.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/13/95   gb      Changed mux option initialization.
06/14/95   gb      Added support for Rate set 2 service options.
03/29/95   jca     Added sigs and tcb_ptr to txc_tc_cmd_type.
12/23/94   jca     Added nom_pwr to TXC_ACCESS_F command parameters.
11/11/94   jca     Deleted TXC_ACC_KILL_F cmd.  Modified TXC_ACC_STOP_F cmd.
11/01/94   gb      Sector Lock test functionality added.
06/21/94   jca     Added TXC_FRAME_OFFSET_F command.
04/20/94   jca     Added Loopback service option.  Deleted support for
                   DIAG data services.
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
10/06/92   jai     Added support for access channel logging.
07/14/92   jai     First Revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "cai.h"
#include "enc.h"
#include "caii.h"  //IS2000 change
#include "mctrans.h"
#include "ffpc.h"
#include "txcmc.h"

#ifdef FEATURE_TX_POWER_BACK_OFF
#include "rfnv.h"
#endif /* FEATURE_TX_POWER_BACK_OFF */
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  txc_sig_frame_type sig;        /* Signaling mode */
  byte               data [ENC_14400_FULL_RATE_SIZE];
} txc_tc_frame_type;

typedef struct
{
  byte frame_cat;        /* frame category */
  byte               data [ENC_14400_FULL_RATE_SIZE];
  boolean dtx;
} txc_tc_dcch_frame_type;

#ifdef FEATURE_IS2000_R_SCH
typedef struct
{
#ifdef FEATURE_32X_SCH_DATA_RATE
  byte               data [ENC_SCH_RC3_32X_SIZE];
#elif defined (FEATURE_DS_IS2000_RS2)
  byte               data [ENC_SCH_RC4_16X_SIZE];
#else
  byte               data [ENC_SCH_RC3_16X_SIZE];
#endif /* FEATURE_32X_SCH_DATA_RATE */ 
  word               size; /* for FACTORY_TESTMODE, the amount of data in */
                           /* the data buffer */
} txc_tc_supp_frame_type;                              
#endif /*FEATURE_IS2000_R_SCH*/

typedef struct {                  /* TXC_VOC_F */
  txc_hdr_type   hdr;
} txc_voc_cmd_type;

/* Gain values output to MDSP has 15 bit resolution as result of TX
** gain enhancement on Dora target.
*/
typedef struct
{
   word pch_gain; 
   word fch_gain; 
#ifdef FEATURE_IS2000_P2
   word dcch_gain;
#endif /*  FEATURE_IS2000_P2 */
   word sch_gain; 
} txc_gain_type;

#ifdef FEATURE_IS2000_P2
/*
** NOTE: when adding/deleting memebers to this enum,
** change appropriately the #define TXC_DCCH_SCH_CONFIG_TYPE
*/
typedef enum
{
   DCCH_ACTIVE_SCH_OFF = 0x0,
   DCCH_DTX_SCH_OFF,
   DCCH_ACTIVE_SCH_ACTIVE,
   DCCH_ACTIVE_SCH_DTX,
   DCCH_DTX_SCH_ACTIVE,
   DCCH_DTX_SCH_DTX
} txc_dcch_sch_config_type;

/* this is the number of modes as defined in txc_dcch_sch_config_type */
#define TXC_DCCH_SCH_CONFIG_MODES  6

typedef struct
{
  txc_gain_type gain;
  int16         pch_adj_gain;
} txc_dcch_sch_gain_type;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
typedef txc_dcch_sch_gain_type  txc_fch_dcch_sch_gain_type;
#endif /* FEATURE_IS2000_REL_A_SVD */

typedef enum
{
  TXC_FCH_ONLY=0x0,
  TXC_SCH_ACTIVE,
  TXC_SCH_DTX,
  TXC_DCCH_ACTIVE,
  TXC_DCCH_DTX
} txc_chan_status_type;

#ifdef FEATURE_PLT
#error code not present
#endif // FEATURE_PLT
/* <EJECT> */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION TXC_RPT_VOC_RATE

DESCRIPTION
  Reports the vocoder rate to the calling routine.

DEPENDENCIES
  None

RETURN VALUE
  Current Vocoder rate.

SIDE EFFECTS
  None

===========================================================================*/

extern cai_data_rate_type txc_rpt_voc_rate( void );



/* <EJECT> */
/*===========================================================================

FUNCTION TXC_HHO

DESCRIPTION
  This function is called by MC on a hard handoff boundary to set the
  number of preamble frames to be transmitted and the new nominal power.

DEPENDENCIES
 Relies on the fact that TX_OFF_TX_F has already been sent and TXC is now
 waiting for TX_ON_TX_F.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_hho(
word num_preambles,
word nom_pwr
);






#if (defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE))
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_FLAG_NEXT_INT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void txc_flag_next_int( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_INT_OCCURRED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean txc_int_occurred( void );
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_DATA_PUMP_INIT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_PLT
#error code not present
#endif // FEATURE_PLT

/* <EJECT>^L */
/*===========================================================================

FUNCTION TXC_GET_FCH_RATE_SET

DESCRIPTION
  Returns the rate set for FCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern enc_rate_set_type txc_get_fch_rate_set ( void );

#ifdef FEATURE_IS2000_P2
/*===========================================================================

FUNCTION TXC_GET_DCCH_RATE_SET

DESCRIPTION
  Returns the rate set for FCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern enc_rate_set_type txc_get_dcch_rate_set (void );
/*===========================================================================

FUNCTION TXC_DCCH_IS_ON

DESCRIPTION
  Returns if DCCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None
===========================================================================*/
extern boolean txc_dcch_is_on(void );
#endif /* FEATURE_IS2000_P2 */

/* <EJECT>^L */
/*===========================================================================

FUNCTION TXC_SET_PCH_FCH_GAIN

DESCRIPTION
  Sets the gain values in txc_pch_fch_gain_tab for the specified rate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void txc_set_pch_fch_gain
(
 byte,
 txc_gain_type,
 int16,
 txc_chan_status_type
);

#ifdef FEATURE_DS_IS2000
/*===========================================================================

FUNCTION TXC_GET_SCH_INFO

DESCRIPTION This function returns current configuration information for
            the SCHs. This is used by the UI in the data display.

            PARAMETERS:
            sch_rc -       gives the Radio configuration of the R-SCH 
            sch_rate_ptr - points to the current SCH rate.
            double_ptr   - is TRUE when double sized PDUs are being used
                           is FALSE when single sized PDUs are being used

            Note, the above two pointer return values are only meaningful
            when SCH is assigned

DEPENDENCIES None

RETURN VALUE TRUE:  If SCH are assigned.
             FALSE: If no SCH are assigned.

SIDE EFFECTS None

===========================================================================*/
boolean txc_get_sch_info
(
  enc_sch_radio_config_type* sch_rc,                      /* ptr to SCH RC */
  enc_sch_rate_type* sch_rate_ptr,                   /* ptr to rate of SCH */
  boolean* double_ptr                   /* Ptr to single/double sized PDUs */
);

#endif  /* FEATURE_DS_IS2000 */

/*===========================================================================
FUNCTION TXC_SET_FPC_MODE

DESCRIPTION
  Sets the value of FPC_MODE in txc_so struct.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_set_fpc_mode (fpc_mode_type);


#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION       TXC_INFORM_SCH_CHNG

DESCRIPTION    Called by data when the SCH config has changed.  This
               figures out the R-SCH throttling parameters.

DEPENDENCIES   None

SIDE EFFECTS   None

RETURN VALUE   None
===========================================================================*/
extern void txc_inform_sch_chng(void);
#endif

#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION TXC_SET_VOC_ACTIVITY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void txc_set_voc_activity
(
  boolean enable,   /* enable/disable voice activity test */
  word voc_1,       /* # of full rate */
  word voc_2,       /* # of 1/2 rate */
  word voc_8        /* # of 1/8 rate */
);
#endif /* FEATURE_JCDMA */


/*===========================================================================

FUNCTION TXC_WRITE_PCBIT_DATA

DESCRIPTION
  This function is called by RXC to write the PCBIT data.  It must be called
  from RXC context because of timing constraints.  The power control data
  must be written so that it is transmitted two frames after the frame it
  is giving quality information about.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_write_pcbit_data( void );

#if defined (FEATURE_RL_JUMP_ENHANCEMENT)

/*===========================================================================

FUNCTION TXC_TX_SHUTDOWN

DESCRIPTION
  Turns off tx clock and disables TXC interrupt.  Allows MC to command SRCH to
  sleep.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_tx_shutdown( void );
#endif // FEATURE_RL_JUMP_ENHANCEMENT

#ifdef FEATURE_TX_POWER_BACK_OFF
/*===========================================================================

FUNCTION TXC_UPDATE_CHAN_CONFIG_TO_RF

DESCRIPTION This function calls by MUX to update the actual channel config to RF

            

DEPENDENCIES None


SIDE EFFECTS None

===========================================================================*/
extern void txc_update_chan_config_to_rf (boolean  valid_config);
#endif /* FEATURE_TX_POWER_BACK_OFF */

#endif /* TXC_H */

