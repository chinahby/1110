#ifndef SRCH_DIAG_H
#define SRCH_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S R C H _ D I A G - H E A D E R  F I L E

GENERAL DESCRIPTION
  This module contains functions pertaining to the search diag interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2003 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/08   adw     Added support to dynamically assign pwr strobe gpio.
08/13/08   vks     Added FEATURE_SRCH_HAS_NO_RTL for ULC
04/23/07   sst     Split up QLIC Support and QLIC Algorithm using defines
02/23/07   sst     Add QLIC support
01/18/07   rkc     Added CGPS debug support
01/17/07   sst     Added xo_trim diag req pkts
08/07/06   rkc     Added srch_diag_request_system_restart() and
                   SRCH_REQUEST_SYSTEM_RESTART.
06/06/06   pa      Added MSG_DBG_AFC
02/08/06   awj     Fixed featurization issue for MDR
02/02/06   awj     Redesigned srch_gen_pilot_set_struct_type.
                   Added pilot info imaging/reporting interface.  Moved
                   srch_aset_walsh(), srch_get_pilot_set() here.
01/16/06   awj     Lint cleanup
11/04/05   awj     Added MSG_DBG_SRCH_IRM
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
09/26/05   awj     Added FEATURE_SRCH_RTL
08/19/05   ejv     Update deprecated types.
07/14/05   sfm     Mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC
05/20/05   ejv     Remove srch_fing_driver.h and srch_rpt_ta.
05/18/05   kwo     Relocated srch_rpt_ta() from srch.c
09/16/04   jcm     Featurized AFC with FEATURE_1X_SRCH_USES_ROTATOR_AFC
09/13/04   sst     Added srch_afc overwrite capabilities
07/07/04   ejv     Add T_WINCE.
06/15/04   ejv     Added MSG_DBG_DV
05/28/04   jcm     Added MSG_DBG_SRCH_SECT
02/27/04   ejv     Added MSG_DBG_RX_DIV
02/05/04   sst     Added MSG_DBG_SRCH4
01/21/04   kwo     Added MSG_DBG_SCHED
12/16/03   rng     Added support for sci override
09/19/03   kwo     Changed message levels to match 6100
08/20/03   kwo     Reworked debug message levels
08/13/03   kwo     Added support for MSG_DBG
07/25/03   bt      Added power strobe.
07/22/03   ejv     Include diagpkt.h.
07/16/03   kwo     implementation , first cut
===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "msg.h"

#include "diagpkt.h"
#include "srch.h"

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

#define MSG_DBG_INIT          MSG_MASK_5  /* Acq, sync, slew */
#define MSG_DBG_IDLE          MSG_MASK_6  /* Non slotted mode */
#define MSG_DBG_ZZ_TIMELINE   MSG_MASK_7  /* slotted timelines */
#define MSG_DBG_QPCH          MSG_MASK_8  /* qpch and qpch channel est */
#define MSG_DBG_SCHED         MSG_MASK_9   /* search scheduler */
#define MSG_DBG_TC            MSG_MASK_10  /* traffic state */

#define MSG_DBG_XO_TRIM       MSG_MASK_15  /* XO Trim switching algorithm */
#define MSG_DBG_STM           MSG_MASK_16  /* state machine driver module */
#define MSG_DBG_SRCH_SLOT     MSG_MASK_17  /* srch slot module */
#define MSG_DBG_FING          MSG_MASK_18  /* finger module */
#define MSG_DBG_TRIAGE        MSG_MASK_19  /* triage module */
#define MSG_DBG_SRCH4         MSG_MASK_20  /* srch4 messages */
#define MSG_DBG_RX_DIV        MSG_MASK_21  /* rxdiv messages */
#define MSG_DBG_SRCH_SECT     MSG_MASK_22  /* search sector module */
#define MSG_DBG_DV            MSG_MASK_23  /* DV messages */
#define MSG_DBG_SRCH_RX       MSG_MASK_24  /* srch_rx.c messages */
#define MSG_DBG_SRCH_IRM      MSG_MASK_25  /* srch_irm.c messages */
#define MSG_DBG_AFC           MSG_MASK_26  /* srch_afc_sm.c messages */
#define MSG_DBG_QLIC          MSG_MASK_27  /* QLIC messages */

/* Define if enabling system restart testing */
/* #define SRCH_DIAG_SYSTEM_RESTART_TEST */

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
  /* Macro for checking srch diag system restart timeline points */
  #define SRCH_DIAG_SYSTEM_RESTART_CHK(curr_timeline) \
      srch_diag_system_restart_check(curr_timeline);
#else
  #define SRCH_DIAG_SYSTEM_RESTART_CHK(curr_timeline)
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */
/*--------------------------------------------------------------------------
                    Subsystem Dispatch Request Response
--------------------------------------------------------------------------*/

/* list of search diag request indeces.  These can be activated via the DM
   command : send_data 75 29 COMMAND 00 DATA */
enum {
  SRCH_FEATURE                = 0x00,
  SRCH_PARM                   = 0x01,
  SRCH_GEN_PILOT_SET          = 0x02,

  SRCH_DIAG_MAX_EXTERNAL_ID,

  SRCH_DIAG_START_INTERNAL_ID = 0xF0,
  SRCH_PWR_STROBE             = 0xF1,
  SRCH_SNOOPER                = 0xF2,

  #ifdef FEATURE_SCI_OVERRIDE
  SRCH_SCI_OVERRIDE           = 0xF3,
  #endif /* FEATURE_SCI_OVERRIDE */

  #ifndef FEATURE_SRCH_HAS_NO_RTL
  SRCH_REQUEST_RTL            = 0xF4,
  #endif /* FEATURE_SRCH_HAS_NO_RTL */

  #ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
  SRCH_REQUEST_SYSTEM_RESTART = 0xF5,
  #endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

  #ifdef FEATURE_SRCH_CGPS_IF
#error code not present
  #endif /* FEATURE_SRCH_CGPS_IF */

  #ifdef FEATURE_SRCH_QLIC
  SRCH_QLIC_OVERRIDE          = 0xF7,
  #endif /* FEATURE_SRCH_QLIC */

  #ifdef FEATURE_XO
  SRCH_AFC_READ_XO_TRIM       = 0xF8,
  SRCH_AFC_WRITE_XO_TRIM      = 0xF9,
  #endif /* FEATURE_XO */
  SRCH_AFC_READ_ACCS          = 0xFA,
  SRCH_AFC_WRITE_ACCS         = 0xFB,
  SRCH_AFC_WRITE_MODE_GAINS   = 0xFC,

  SRCH_DIAG_MAX_ID
};

/*--------------------------------------------------------------------------
             GENERALIZED PILOT SET REQUEST REPORT STRUCTURE

This structure is used in the called interface when the caller desires to
get a read on how many and what pilots are in the Search sets.
--------------------------------------------------------------------------*/

#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */

/* The following packed structures are used to comply with diag packet
   specifications */

/* Collection of pilot counts. */
typedef PACKED struct
{
  uint16 pilot_inc;             /* Pilot increment for remaining sets */
  uint8  active_cnt;            /* Count of pilots in active set */
  uint8  cand_cnt;              /* Count of pilots in candidate set */
  uint8  neigh_cnt;             /* Count of pilots in neighbor set */
} srch_diag_pilot_set_count_type;

/* Each pilot has a pn/strength pair.  An dynamically sized array of these
   is included in diag packets/UI update requests */
typedef PACKED struct
{
  uint16 pn;
  uint16 strength;
} srch_diag_pilot_info_type;

/* Max sized array of pn/strength pairs */
typedef PACKED struct
{
  /* Sets of pilots (in the above order), plus one Remaining pilot */
  srch_diag_pilot_info_type sets[SRCH_MAX_SETS];
} srch_diag_pilot_set_info_type;

/* Complete pilot set type; includes counts and pn/strength data */
typedef PACKED struct
{
  srch_diag_pilot_set_count_type count;
  srch_diag_pilot_set_info_type  info;
} srch_diag_pilot_set_type;

/* The RF portion of certain diag requests.  This includes srch_state for
   compliance with the packet structure */
typedef PACKED struct
{
  uint16 cdma_freq;             /* Current CDMA channel */
  uint16 band_class;            /* Current band class */
  uint8  rx_agc;                /* Current value of rx agc register */
  uint8  tx_adj;                /* Current value of tx gain adjust register */
  uint8  tx_agc;                /* Current value of tx agc register */

  uint16 ref_inx;               /* Latest reference sector index */
  uint16 srch_state;            /* Latest state Searcher is in */
} srch_diag_rf_info_type;

/* Each active should have an offsect and a number 0-7 of supplemental
   channels.  Walsh codes for each possible channel (fundamental + 7
   supplemental) are included */

typedef PACKED struct
{
  /* Pilot offset used by search */
  uint16 pilot_off;

  /* Number of supplemental channels */
  uint8 num_sup_chans;

  /* Specifies which walsh code is being used, including fundamental */
#ifdef FEATURE_IS95B_MDR
  uint16 code[ SRCH_SUP_CHANNEL_MAX + 1 ];
#endif /* FEATURE_IS95B_MDR */

} srch_diag_walsh_data_type;

/* Latest Walsh code information; the number of actives and an array consisting
   of the walsh data for each */
typedef struct
{
  /* number of active set */
  uint16                    num_aset;

  srch_diag_walsh_data_type data[ SRCH_ASET_MAX ];

} srch_diag_walsh_info_type;

/* This type is only used to direct DIAG to look beyond a packet header.  It
   should not be used for any other purpose.  Explanation follows:

   This type is only used to support the DIAG architectural model for packet
   layout.  The size of the packet is dynamically determined, so sizeof() is
   insufficient.  The reason it is preserved is to support the packet model, which is

struct
{
  uint32 packet_header;
  type*  packet_data;
};

  The size of this type is never checked, and a variable of this type should
  never be instantiated.  It is simply a pointer which will be cast to void*
  in srch_diag and will point to a diag packet allocated elsewhere.
  </warning> */
typedef void* srch_gen_pilot_set_struct_type;

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
/* System restart timeline points */
typedef enum {
  /* Common timeline points*/
  SRCH_DIAG_SYSTEM_RESTART_INIT = 1,
  SRCH_DIAG_SYSTEM_RESTART_SLEEP,
  SRCH_DIAG_SYSTEM_RESTART_WAKEUP,
  SRCH_DIAG_SYSTEM_RESTART_CX8,
  SRCH_DIAG_SYSTEM_RESTART_WAIT_SB,

  /* QPCH timelines only */
  SRCH_DIAG_SYSTEM_RESTART_DOZE,
  SRCH_DIAG_SYSTEM_RESTART_RECORD,
  SRCH_DIAG_SYSTEM_RESTART_WAIT_REACQ,
  SRCH_DIAG_SYSTEM_RESTART_WAIT_RECORD,
  SRCH_DIAG_SYSTEM_RESTART_DEMOD,

  /* IS95A timeline only */
  SRCH_DIAG_SYSTEM_RESTART_FAKE_SYNC80,
  SRCH_DIAG_SYSTEM_RESTART_FAKE_SYNC80_ACQ,

  /* RTL timeline only */
  SRCH_DIAG_SYSTEM_RESTART_WAIT_FOR_LOCK,

  /* Do not check for this value, only used for init/disable */
  SRCH_DIAG_SYSTEM_RESTART_DISABLED = 0xFF
} srch_diag_system_restart_type;
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */

/* Gen Pilot Sets diag request packet definition */

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_GEN_PILOT_SET)
DIAGPKT_REQ_END

/* The Gen Pilot Sets response packet definition */

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_GEN_PILOT_SET)
  srch_gen_pilot_set_struct_type pilot_set_response;
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             SEARCH PARMS

This interface provides a way for a test tool to query and clear search
parameters.
--------------------------------------------------------------------------*/

enum {
  SRCH_DIAG_PARM_INVALID = -1,  /* invalid param */
  SRCH_DIAG_PARM_ACQ_TOOK = 0,  /* how long acq took in ms */
  SRCH_DIAG_PARM_ACQ_FAIL_CNT,  /* number of time acq has failed */

  SRCH_DIAG_PARM_MAX_PARM
};

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_PARM)
  int16 parm_id;  /* the id of the requested parameter */
  int8  clear;    /* 0 = get without clearing, 1 = get and clear */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_PARM)
  int16 parm_id;  /* the id of the requested parameter */
  int16 value;    /* value of the parameter */
DIAGPKT_RSP_END



/*--------------------------------------------------------------------------
                             NERO STROBE

This interface provides a way for a test tool to enable and disable the
nero strobe
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_PWR_STROBE)
  uint8  enable;    /* 0 = disable, 1 = enable */
  #ifdef FEATURE_SRCH_PWR_STROBE_DYN_GPIO__STRIP
#error code not present
  #endif /* FEATURE_SRCH_PWR_STROBE_DYN_GPIO__STRIP */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_PWR_STROBE)
DIAGPKT_RSP_END


#ifdef FEATURE_SCI_OVERRIDE
/*--------------------------------------------------------------------------
                             SCI OVERRIDE

This interface provides a way for a test tool to override the current SCI
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_SCI_OVERRIDE)
  uint8  enable;    /* 0 = disable, 1 = enable */
  int8   sci_override;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_SCI_OVERRIDE)
DIAGPKT_RSP_END

#endif /* FEATURE_SCI_OVERRIDE */

#ifndef FEATURE_SRCH_HAS_NO_RTL
/*--------------------------------------------------------------------------
                             REQUEST_RTL

This interface provides a way for a test tool to transfer in and out of RTL
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_REQUEST_RTL)
  uint8  rtl_enable;    /* 0 = disable, 1 = enable */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_REQUEST_RTL)
DIAGPKT_RSP_END
#endif /* FEATURE_SRCH_HAS_NO_RTL */

/*--------------------------------------------------------------------------
                             AFC DEBUG

This interface provides functions to read/write trk_lo_adj (ft_freq) and the
AFC gains
--------------------------------------------------------------------------*/

#ifdef FEATURE_XO
DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_READ_XO_TRIM)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_READ_XO_TRIM)
    uint8  xo_trim;
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_XO_TRIM)
    uint8  xo_trim;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_XO_TRIM)
DIAGPKT_RSP_END
#endif /* FEATURE_XO */


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_READ_ACCS)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_READ_ACCS)
    int16  acc_vco;
    int16  acc_rot;
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_ACCS)
    int16  acc_vco;
    int16  acc_rot;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_ACCS)
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_MODE_GAINS)
    uint16 gain_vco;
    uint16 gain_rot;
    uint8  config_mode;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_MODE_GAINS)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             REQUEST_SYSTEM_RESTART

This interface provides a way for a test tool to restart current timeline
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_REQUEST_SYSTEM_RESTART)
    uint8 tl_point;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_REQUEST_SYSTEM_RESTART)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             SRCH CGPS API TEST

This interface provides a way for a test tool to test SRCH CGPS API
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_CGPS_API_TEST)
    uint8 internal_cmd;
    uint8 priority;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_CGPS_API_TEST)
DIAGPKT_RSP_END


#ifdef FEATURE_SRCH_QLIC
/*--------------------------------------------------------------------------
                             QLIC OVERRIDE

This interface provides a way for a test tool to enable/disable QLIC
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_QLIC_OVERRIDE)
  uint8  override;    /* 1 = disable QLIC, 0 = enable QLIC */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_QLIC_OVERRIDE)
DIAGPKT_RSP_END
#endif /* FEATURE_SRCH_QLIC */


/*===========================================================================

FUNCTION SRCH_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      SRCH logging dispatch table.

DEPENDENCIES   srch_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_init (void);

/*===========================================================================

FUNCTION SRCH_DIAG_GET_PILOT_SET

DESCRIPTION    This function fills up a diagnostic pilot set status
               packet with current set populations and memberships.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void srch_diag_get_pilot_set (srch_pilot_set_struct_type *pkt_ptr);

#ifdef FEATURE_IS95_MDR
/*===========================================================================

FUNCTION SRCH_DIAG_ASET_WALSH

DESCRIPTION    This function is used to provide walsh code information of the
               Active Set.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_aset_walsh(srch_walsh_struct_type *ptr);
#endif /* FEATURE_IS95_MDR */

/*===========================================================================

FUNCTION SRCH_DIAG_SET_PARM

DESCRIPTION    This function sets a parameter from the search parm table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_set_parm
(
  int16   parm_id, /* paramater id */
  int16   value    /* value to set parm to */
);

/*===========================================================================

FUNCTION SRCH_DIAG_GET_PARM

DESCRIPTION    This function gets a parameter from the search parm table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_diag_get_parm
(
  int16   parm_id /* paramater id */
);

#ifdef FEATURE_SCI_OVERRIDE
/*===========================================================================

FUNCTION SRCH_DIAG_GET_SCI_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the sci override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_diag_get_sci_override_enable( void );

/*===========================================================================

FUNCTION SRCH_DIAG_GET_SCI_OVERRIDE

DESCRIPTION    This function gets the sci override value.

DEPENDENCIES   None.

RETURN VALUE   INT8.

SIDE EFFECTS   None.

===========================================================================*/
extern int8 srch_diag_get_sci_override( void );
#endif /* FEATURE_SCI_OVERRIDE */

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
/*===========================================================================

FUNCTION       SRCH_DIAG_SET_SYSTEM_RESTART
DESCRIPTION    This function is for debug purposes set system restart
               at a point in the current timeline.

DEPENDENCIES   None
RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch_diag_set_system_restart (srch_diag_system_restart_type tl_point);

/*===========================================================================

FUNCTION       SRCH_DIAG_GET_SYSTEM_RESTART
DESCRIPTION    This function is for debug purposes get the current system
               restart point in the current timeline.

DEPENDENCIES   None

RETURN VALUE   Current timeline point to check for system restart

SIDE EFFECTS   None

===========================================================================*/
srch_diag_system_restart_type srch_diag_get_system_restart (void);

/*===========================================================================

FUNCTION       SRCH_DIAG_SYSTEM_RESTART_CHECK
DESCRIPTION    This function checks if it is necessary to invoke internal
               system restart for debug.  Internal CDMA_F command is also
               invoked to get the CDMA_F command immediately, i.e., faster
               than turn around time from MC as a result of system restart.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None
===========================================================================*/
void srch_diag_system_restart_check
(
  srch_diag_system_restart_type curr_point /* Current timeline point */
);
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

#endif /* SRCH_DIAG_H */

