#ifndef CDMA2KDIAG_H
#define CDMA2KDIAG_H

/*==========================================================================
          Diagnostic System Packet Definitions for CDMA2000

  Description: Packet definitions to support the interface between the
  DMSS software and the external device for CDMA2000-related packets.

Copyright (c) 2001-2005,2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

  $Header: //depot/asic/msmshared/services/diag/cdma2kdiag.h#14 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/07   as      Enabled pragma pack support for WINCE targets
07/15/05   as      Added push & pop pragma pack
01/08/04   as      Moved prl specific functions to prldiag.c/h
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
08/26/03   as      Removed FEATURE_SPECIAL_MDR
10/14/02   sj      Removed definition of NV_SIO_BITRATE_115200 here.
08/28/02   lad     Created file.

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "nv.h" /* Need NV_MAX_DIAL_DIGITS */

#ifdef FEATURE_HWTC
#error code not present
#endif


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

/*---------------------------------------------------------------------------
   Operating modes for packet processing.  These masks are used to define
   permissions for packet processing in the dispatch table.
---------------------------------------------------------------------------*/
#define MODE_OFF_DIG  0x01  /* Packet accepted in offline-digital        */
#define MODE_OFF_ANA  0x02  /* Packet accepted in offline-analog         */
#define MODE_ONLINE   0x04  /* Packet accepted in online                 */
#define MODE_ANY      0xFF  /* Packet accepted in all modes.             */

#define MODE_OFFLINE  ( MODE_OFF_DIG | MODE_OFF_ANA )
                              /* Packet accepted in either offline mode    */



/*---------------------------------------------------------------------------

       Packet definitions for 1X-specific packets.

---------------------------------------------------------------------------*/

/*===========================================================================

PACKET   diag_status_req_type

ID       DIAG_STATUS_F

PURPOSE  Sent by the DM to request a set of status data

RESPONSE The DMSS gathers the status data and sends it in a
         diag_status_rsp_type packet to the DM.

============================================================================*/
typedef PACKED struct
{
  byte cmd_code;               /* Command code */
} diag_status_req_type;

/*-----------------------------------------------------------
   The following 2 numbers are for the entry_reason in
   diag_status_rsp_type
-----------------------------------------------------------*/

/* Taken from MCC_ENTER_OFFLINE in mcc_entry_name_type.
   Please refer to mcc.h */
#define DIAG_OFFLINE_DIGITAL 2

/* Taken from APC_ENTER_OFFLINE in apc_entry_name_type.
   Please refer to acpmc.h*/
#define DIAG_OFFLINE_ANALOG  6

typedef PACKED struct {
  byte    cmd_code;               /* Command code                         */
  byte    demod;                  /* Demod id byte                        */
  byte    decode;                 /* Decoder id byte                      */
  byte    inter;                  /* Interleaver id byte                  */
  dword   esn;                    /* Mobile Electronic Serial Number      */

  word    rf_mode;                /* 0->analog,  1->cdma,  2->pcn         */
  dword   min1[NV_MAX_MINS];      /* all MIN1s                            */
  word    min2[NV_MAX_MINS];      /* all MIN2s                            */
  byte    orig_min;               /* index (0-3) of the orig MIN          */

  word    cdma_rx_state;          /* current state for cdma only          */
  int1    cdma_good_frames;       /* whether or not frames we are
                                     receiving are good -- cdma only      */
  word    analog_corrected_frames;/* good frame count - analog only       */
  word    analog_bad_frames;      /* bad frame count - analog only        */
  word    analog_word_syncs;      /*  -- analog only                      */

  word    entry_reason;           /* entry reason                         */
  word    curr_chan;              /* center frequency channel             */
  byte    cdma_code_chan;         /* code for current channel - cdma only */
  word    pilot_base;             /* pilot pn of current cell - cdma only */

  word    sid;                    /* Current System ID                    */
  word    nid;                    /* Current Network ID                   */
  word    locaid;                 /* Current Location ID                  */
  word    rssi;                   /* Current RSSI level                   */
  byte    power;                  /* Current mobile output power level    */
} diag_status_rsp_type;

/*===========================================================================

PACKET   diag_markov_req_type

ID       DIAG_MARKOV_F

PURPOSE  Sent by the DM to reqest Markov Statistics

RESPONSE The DMSS prepares and sends a diag_markov_rsp_type packet.

============================================================================*/
typedef PACKED struct
{
  byte cmd_code;                /* Command code */
} diag_markov_req_type;

/* Definitions of values for the mode field - service option */
#define DIAG_SO_NONE            0x00
#define DIAG_SO_LOOPBACK        0x02
#define DIAG_SO_MARKOV          0x03
#define DIAG_SO_DATA            0x04
#define DIAG_SO_VOICE_96A       0x05
#define DIAG_SO_VOICE13         0x06 /* 13K (MSM2 only) Voice call */
#define DIAG_SO_RS2_MARKOV      0x07 /* 13K (MSM2 only) Markov call (new) */
#define DIAG_SO_RS1_MARKOV      0x08
#define DIAG_SO_LOOPBACK_13K    0x09
#define DIAG_SO_MARKOV_13K      0x0A /* 13K (MSM2 only) Markov call (old) */
#define DIAG_SO_EVRC            0x0B /* EVRC voice option */
#define DIAG_SO_VOICE_13K_IS733 0x11 /* IS-733 s.o. number */
#define DIAG_SO_MARKOV_SO54     0x36
#define DIAG_SO_LOOPBACK_SO55   0x37

#if defined(FEATURE_IS95B_MDR)
/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and reverse.  Default Mux = 9 forward 1 reverse */
#define DIAG_SO_MDR_PKT_DATA_FRS1_RRS1   0x16 /* IS-707A s.o. number = 22 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and rate set 2 reverse.
 Default Mux = 9 forward 2 reverse.  This SO is not support by MSM3000. */
#define DIAG_SO_MDR_PKT_DATA_FRS1_RRS2   0x17 /* IS-707A s.o. number = 23 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and rate set 1 reverse.
 Default Mux = 10 forward 1 reverse.  This SO is not support by MSM3000. */
#define DIAG_SO_MDR_PKT_DATA_FRS2_RRS1   0x18 /* IS-707A s.o. number = 24 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and reverse.  Default Mux = 10 forward 2 reverse */
#define DIAG_SO_MDR_PKT_DATA_FRS2_RRS2   0x19 /* IS-707A s.o. number = 25 */

#define DIAG_SO_MDR_DPT_FRS1_RRS1        0x24 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 1 fwd and rev */

#define DIAG_SO_MDR_DPT_FRS2_RRS2        0x25 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 2 fwd and rev */
#endif /* defined(FEATURE_IS95B_MDR) */


typedef PACKED struct
{
  byte cmd_code;               /* Command code                        */
  byte mode;                   /* rxc mode                            */
  word markov_rate;            /* rate for markov processing          */
  word rx_state;               /* receive task state                  */
  dword total_frames;          /* total number of Markov frames       */
  dword bad_frames;            /* total number of bad frames          */
  word data[ 4 ][ 10 ];        /* mar_test array items for expected
                                     full, half,  quarter and eighth
                                     rate packets                     */
  dword bit_errs;              /* Markov bit errors                   */
  word good_errs[4];           /* Rate decision good,  but bits bad   */
} diag_markov_rsp_type;


/*===========================================================================

PACKET   diag_markov_reset_req_type

ID       DIAG_MARKOV_RESET_F
PURPOSE  Sent by the DM to reset the DMSS's Markov Statistics

RESPONSE DMSS resets the accumulated Markov Statistics and replies with
         a diag_m_reset_rsp_type packet.

============================================================================*/
typedef PACKED struct
{
  byte cmd_code;                /* Command code */
} diag_markov_reset_req_type;

typedef PACKED struct
{
  byte cmd_code;               /* Command code */
} diag_markov_reset_rsp_type;

/*===========================================================================

PACKET   diag_tagraph_req_type

ID       DIAG_TAGRAPH_F

PURPOSE  Sent by the DM to request info for lower-right corner of TA

RESPONSE The DMSS prepares and sends a diag_tagraph_rsp_type packet.

============================================================================*/
typedef PACKED struct
{
  byte cmd_code;                /* Command code */
} diag_tagraph_req_type;

typedef PACKED struct
{
  byte cmd_code;               /* Command code */
  byte  rx_rssi;               /* Current value of rx agc register */
  byte  adj_rssi;              /* Current value of tx gain adjust register */
  byte  tx_voc;                /* Current tx vocoder data rate */
  byte  rx_voc;                /* Current rx vocoder data rate */
  dword bad_frames;            /* Number of bad frames */
  dword total_frames;          /* Number of all the frames */
} diag_tagraph_rsp_type;



/*===========================================================================

PACKET   diag_ta_parm_req_type

ID       DIAG_TA_PARM_F

PURPOSE  Sent by the DM to set TA parameters in the DMSS

RESPONSE The DMSS sets the TA parameters and sends a diag_ta_parm_rsp_type
         packet.

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;
  word set_mask;
    /* Bit 15 = enable reporting of active set members
       Bit 14 = enable reporting of candidate set members
       Bit 13 = enable reporting of neighbor set members
       Bit 12 = enable reporting of remaining set members
       Bit 11 = enable reporting of pre-candidate set members */
  word win_siz;  /* specifies maximum window size in half chip units */

} diag_ta_parm_req_type;

typedef PACKED struct
{
  byte cmd_code;
} diag_ta_parm_rsp_type;



/*===========================================================================

PACKET   diag_ser_reset_req_type

ID       DIAG_SER_RESET_F

PURPOSE  Sent by DM to direct the DMSS reset the SER counts

RESPONSE DMSS resets counts and sends reply packet acknowledging the action

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;
} diag_ser_reset_req_type;


typedef PACKED struct
{
  byte cmd_code;
} diag_ser_reset_rsp_type;


/*===========================================================================

PACKET   diag_ser_report_req_type

ID       DIAG_SER_REPORT_F

PURPOSE  Sent by DM to direct the DMSS to report the SER counts.  They
         are not reset.

RESPONSE DMSS send reply packet containing the counts.

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;
} diag_ser_report_req_type;


typedef PACKED struct
{
  byte cmd_code;
  dword frame_cnt;  /* Total frame count */
  dword ser1;   /* Sum of all 192 bit frame symbol error counts */
  dword ser2;   /* Sum of all 96 bit frame symbol error counts */
  dword ser4;   /* Sum of all 48 bit frame symbol error counts */
  dword ser8;   /* Sum of all 24 bit frame symbol error counts */
} diag_ser_report_rsp_type;


/*===========================================================================

PACKET   diag_state_req_type

ID       DIAG_STATE_F

PURPOSE  Sent by external device to obtain the current state of operation
         the phone is in.

RESPONSE The diag_state_rsp_type will be sent in response.

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;              /* Command code */
} diag_state_req_type;

typedef PACKED struct
{
  byte cmd_code;              /* Command code */
  byte phone_state;           /* Current phone state */
  word event_count;           /* Count of possible state-changing events */
} diag_state_rsp_type;


/*===========================================================================

PACKET   diag_pilot_sets_req_type

ID       DIAG_PILOT_SETS_F

PURPOSE  Sent by external device to obtain the current contents of the
         various pilot sets.

RESPONSE The diag_pilot_sets_rsp_type will be sent in response.

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;
} diag_pilot_sets_req_type;

typedef PACKED struct
{
    word pilot_pn;            /* Pilot offset   */
    word pilot_strength;      /* Pilot strength */
} diag_pilot_data_type;

typedef PACKED struct
{
  byte cmd_code;              /* Command code */
  word pilot_inc;             /* Pilot increment for remaining sets */
  byte active_cnt;            /* Count of pilots in active set (up to 6) */
  byte cand_cnt;              /* Count of pilots in candidate set (up to 6) */
  byte neigh_cnt;             /* Count of pilots in neighbor set (up to 40) */
  diag_pilot_data_type pilot_sets[52];
                              /* Sets of pilots (in the above order) (6+6+40) */
} diag_pilot_sets_rsp_type;


/*===========================================================================

PACKET   DIAG_AKEY_VERIFY_F

PURPOSE  Sent by DM to verify AKEY update.  Contains AKEY.  DM or System
         Unit can ask AKEY update for a particular NAM or if NAM is 0xff,
         AKEY for current nam will be updated.

RESPONSE DMSS responds with message saying whether or not AKEY update was
         successful.

============================================================================*/
#define DIAG_AKEY_DIGITS 26
#define DIAG_AKEY_REQ_RESERVED 8

typedef PACKED struct {
  byte                cmd_code;                    /* Command code          */
  byte                akey[DIAG_AKEY_DIGITS];      /* AKEY with             */
                                                   /* cryptographic check   */
  byte                nam;                         /* NAM for which AKEY    */
                                                   /* applies               */
                                                   /* If NAM is set to 0xff,*/
                                                   /* AKEY for current NAM  */
                                                   /* is changed            */
  byte                reserved[DIAG_AKEY_REQ_RESERVED]; /* for future expansion  */
} diag_akey_verify_req_type;

typedef PACKED struct
{
  byte cmd_code;              /* Command code */
  boolean akey_ok;            /* The AKEY was entered correctly or not */
} diag_akey_verify_rsp_type;


/*===========================================================================

PACKET   DIAG_SSD_VERIFY_F

PURPOSE  Sent by DM to verify SSD update.  Contains SSDs.  DM or System
         Unit can ask for SSDs verified for a particular NAM or if NAM
         is 0xff, SSDs for current nam will be verified.

RESPONSE DMSS responds with message saying whether or not SSD verification
         was successful.

============================================================================*/
#define DIAG_SSD_A   0
#define DIAG_SSD_B   1

typedef PACKED struct
{
  byte               cmd_code;
  byte               ssd_select;                   /* 0 ssd_a and 1 ssd_b   */
  qword              ssd_a;                       /* Shared secret A       */
  qword              ssd_b;                       /* Shared secret B       */
  byte               nam;                         /* NAM for which SSDs    */
                                                  /* applies               */
                                                  /* If NAM is set to 0xff,*/
                                                  /* SSD for current NAM   */
                                                  /* is checked            */
} DIAG_SSD_VERIFY_F_req_type;

typedef PACKED struct
{
  byte cmd_code;
  boolean ssd_ok;             /* The SSDs were entered correctly or not */
} DIAG_SSD_VERIFY_F_rsp_type;


/*===========================================================================

PACKET   DIAG_STATUS_SNAPSHOT_REQ_TYPE
PURPOSE  This command gives a variable length response snapshot of all
  information needed at the start of logging to accuratley analyze the log
  data.

===========================================================================*/
typedef PACKED struct
{
 byte                cmd_code;  /* Command code   */
} diag_status_snapshot_req_type;

typedef PACKED struct
{
  byte                cmd_code;  /* Command code   */
  uint32 esn;

  PACKED struct {
    uint32 imsi_s1;       /* First part of Operational IMSI (IMSI_O_S1) */
    uint16 imsi_s2;        /* Second part of Operational IMSI (IMSI_O_S2) */
    uint64 imsi_s;        /* IMSI_S1 and IMSI_S2 converted to a qword imsi_s */
    uint8  imsi_11_12;     /* Mobile Network Code (MNC) for IMSI_O address */
    uint16 mcc;            /* Mobile Country Code (MCC) for IMSI_O address */
    uint8  imsi_addr_num;  /* Number of digits in NMSI - 4, or class 0 IMSI flag */
  } full_imsi;

  uint16 sid;
  uint16 nid;
  uint8  p_rev;
  uint8  prev_in_use;
  uint8  mob_p_rev;
  uint8  band_class;
  uint16 frequency;
  uint8  operation_mode; //CDMA, Analog, GPS, etc (AKA rf_mode) -LD

  /* MC state: Initialization, Idle, Access, Traffic */
  uint8  state;

  /* The sub-state depends on the value in the above 'state' value.
  ** init state:    System Determination
  **                Pilot Channel Acquisition
  **                Sync Channel Acquisition
  **                Timing Change
  ** idle state:    Slotted mode
  **                Non-slotted mode
  ** access state:  Update Overhead Information
  **                Registration Access
  **                Order/Message Response
  **                Transmission
  **                Origination Attempt
  **                Page Response
  ** traffic state: Traffic Channel Initialization
  **                Waiting for Order
  **                Waiting for MS Answer
  **                Conversation
  **                Release
  */
  uint8  sub_state;
} diag_status_snapshot_rsp_type;


#if defined(FEATURE_IS95B_MDR)
#define DIAG_SUP_CHANNEL_MAX 7
/*===========================================================================

PACKET   DIAG_SUP_WALSH_CODES_F

PURPOSE  Sent by DMSS, contains

============================================================================*/
typedef struct
{
  byte cmd_code;
} diag_sup_walsh_codes_req_type;

#define   DIAG_SRCH_SUP_CHANNEL_MAX 7
#define   DIAG_SRCH_ASET_MAX 6

typedef PACKED struct
{
  byte cmd_code;                                /* Command code */

  word num_aset;
    /* number of active set */

  PACKED  struct
  {
    int2 pilot_off;
      /* Pilot offset used by serach */

    byte num_sup_channels;
      /* Number of supplemental channels */

    word walsh_code[ DIAG_SRCH_SUP_CHANNEL_MAX + 1 ];
      /* Specifies which walsh code is being used, including fundamental */

  }ch_data [ DIAG_SRCH_ASET_MAX ];
    /* One for each Aset */

} diag_sup_walsh_codes_rsp_type;


/*===========================================================================

PACKET   diag_sup_fer_rsp_type

ID       DIAG_SUP_FER_F

PURPOSE  Sent by DMSS, contains Frame Error Rate data for the small
         window.  This is called if supplemental channels (MDR) is supported.
         This packet gives a snapshot of the FER info for all 8 channels at
         once.

============================================================================*/
typedef struct
{
  byte cmd_code;
} diag_sup_fer_req_type;

typedef PACKED struct
{
  byte cmd_code;                                /* Command code */

  dword bad_frames[1 + DIAG_SUP_CHANNEL_MAX];   /* Number of bad frames for:
                                                   Fundamental +
                                                   all sup channels. */
  dword total_frames[1 + DIAG_SUP_CHANNEL_MAX]; /* Total frames for:
                                                   Fundamental +
                                                   all sup channels. */

} diag_sup_fer_rsp_type;

/*===========================================================================

PACKET   diag_set_max_sup_ch_rsp_type

ID       DIAG_SET_MAX_SUP_CH_F

PURPOSE  This response acknowledges a valid setting.

============================================================================*/
typedef PACKED struct
{
  byte cmd_code;            /* Command code */
  word service_option;      /* Specifies service option (rate set) */
  byte num_sup_ch;          /* Number of supplemental channels to be allowed*/
} diag_set_max_sup_ch_req_type;

typedef PACKED struct
{
  byte  cmd_code;                           /* Command code */
} diag_set_max_sup_ch_rsp_type;

#endif /* defined(FEATURE_IS95B_MDR) */


/*===========================================================================

PACKET   DIAG_RLP_STAT_RESET_F
PURPOSE  Reset RLP accumulated statistics

===========================================================================*/

/* No need to PACK this */
typedef struct {
  byte cmd_code;
  byte  sr_id;
} DIAG_RLP_STAT_RESET_F_req_type;

typedef struct {
  byte cmd_code;
} DIAG_RLP_STAT_RESET_F_rsp_type;

/*===========================================================================

PACKET   DIAG_TDSO_STAT_RESET_F
PURPOSE  Reset (S)TDSO accumulated statistics

===========================================================================*/

typedef struct {
  byte cmd_code;
} DIAG_TDSO_STAT_RESET_F_req_type;

typedef DIAG_TDSO_STAT_RESET_F_req_type DIAG_TDSO_STAT_RESET_F_rsp_type;


/*===========================================================================

PACKET   diag_ftm_req_type

ID       DIAG_FTM_CMD_F

PURPOSE  Sent by DM to request Factory Test Mode commands.

RESPONSE DMSS sends back status of the requested command.

===========================================================================*/

typedef PACKED struct
{
  byte cmd_code;
  word sub_cmd;
  word length;      /* Length of the request[] */
  byte request[1];  /* Actual data content varies with request */
} diag_ftm_req_type;


typedef PACKED struct
{
  byte cmd_code;
  word sub_cmd;
  byte status;
} diag_ftm_rsp_type;

/*===========================================================================

PACKET   diag_test_req_type

ID       DIAG_TEST_F

PURPOSE  Sent by DM to direct the DMSS to perform a specified test.

RESPONSE DMSS performs the test, then responds.

===========================================================================*/
/*-------------------------------------------------------------------------

                    Directed Test parameter types

--------------------------------------------------------------------------*/

/* Parameters for the set_attn directed test */

typedef PACKED struct {
  byte  attn;
} set_attn_parm_type;

/* Parameters for the switch_nam directed test */

typedef PACKED struct {
  byte  nam;
} nam_type;

/* Parameters for the send_min directed test */

typedef PACKED struct {
  byte  nam;
  byte  min;
} send_min_parm_type;

/* Parameters for the sat directed test */

typedef PACKED struct {
  byte  sat;
} sat_parm_type;

/* Parameters for the CDATA directed test */

typedef PACKED struct {
  byte  cdata[6];
} cdata_parm_type;

/* Parameters for the dtmf directed test */

typedef PACKED struct {
  byte  dtmf;
} dtmf_parm_type;

/* Union of the directed test parameter types */
typedef PACKED union {
  word                    chan;       /* Channel for load-synth cmd  */
  set_attn_parm_type      sa;         /* Attn level for set-attn cmd */
  nam_type                nam;
  send_min_parm_type      sm;
  sat_parm_type           sat;
  cdata_parm_type         cdata;
  dtmf_parm_type          dtmf;
} diag_test_parm_type;


typedef PACKED struct {
  byte                  cmd_code;
  word                  test_code; /* Use diag_test_enum_type */
  diag_test_parm_type   parm;
} diag_test_req_type;

/* Response packet definitions */

/* RCVS1 response (requested with a terminate command) */

typedef PACKED struct {
  byte  num_uncorr;
  byte  num_corr;
} rcvs1_type;

/* WSTS response (requested with a terminate command ) */

typedef PACKED struct {
  word  num_syncs;
} wsts_type;

/* RSSI response */

/* Union of directed test response types */

typedef PACKED union {
  rcvs1_type        rcvs1;              /* Response from RCVS1 command */
  wsts_type         wsts;               /* Response from WSTS command */
  byte              rssi;               /* Response from RSSI command */
} diag_test_results_type;


typedef enum
{
  DT_CARRIER_ON_F,            /* Carrier on command                       */
  DT_CARRIER_OFF_F,           /* Carrier off command                      */
  DT_LOAD_SYNTH_F,            /* Load Synthesizer command                 */
  DT_SET_ATTN_F,              /* Set Attenuation command                  */
  DT_RXMUTE_F,                /* Mute the receiver audio signal           */
  DT_RXUNMUTE_F,              /* Unmute the receier audio signal          */
  DT_TXMUTE_F,                /* Mute the transmit audio signal           */
  DT_TXUNMUTE_F,              /* Unmute the transmit audio signal         */
  DT_ST_ON_F,                 /* Transmit continuous Signalling Tone      */
  DT_ST_OFF_F,                /* Stop transmission of Signalling Tone     */
  DT_SEND_NAM_F,              /* Send the specified NAM                   */
  DT_SWITCH_NAM_F,            /* Change to the specified NAM              */
  DT_SEND_MIN_F,              /* Send the specified MIN                   */
  DT_VERSION_F,               /* Send software version                    */
  DT_SEND_ESN_F,              /* Send the esn                             */
  DT_RCVS1_F,                 /* Count msgs on Analog Control Channel     */
  DT_WSTS_F,                  /* Count word syncs on Analog Control
                                 Channel                                  */
  DT_SAT_ON_F,                /* Enable transmission of SAT               */
  DT_SAT_OFF_F,               /* Disable transmission of SAT              */
  DT_TERMINATE_F,             /* Stop RCVS1 or WSTS and report counts     */
  DT_CDATA_F,                 /* Transmit continuous 5-word Reverse
                                 Control Channel messages                 */
  DT_DTMF_ON_F,               /* Activate the DTMF generator with
                                 specified tones                          */
  DT_DTMF_OFF_F,              /* Stop the DTMF                            */
  DT_COMPANDER_ON_F,          /* Enable the compander                     */
  DT_COMPANDER_OFF_F,         /* Disable the compander                    */
  DT_RSSI_F,                  /* Return the current RSSI measurement      */
  DT_MAX_F                    /* Last and invalid directed test choice    */
} diag_test_enum_type;

typedef PACKED struct
{
  byte                      cmd_code;
  word                      test_code; /* Use diag_test_enum_type */
  diag_test_results_type    results;
} diag_test_rsp_type;


#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */


#ifdef TMOBILE
#error code not present
#endif /* TMOBILE */

/* Some functions in 1X targets still want to see the union.
 * This method will be phased out. */

#include "parmdiag.h"

typedef PACKED union
{
  byte                       cmd_code;

  diag_status_req_type           stat;

  diag_markov_req_type         markov;
  diag_markov_reset_req_type  m_reset;

  diag_parm_set_req_type     parm_set;

  diag_parm_get2_req_type   parm_get2;

#if defined(FEATURE_IS95B_MDR)
  diag_parm_get_is95b_req_type   parm_get_is95b;
#endif

  diag_parm_retrieve_req_type     parm_retrieve;

} diag_req_type;

typedef PACKED union
{
  byte                       cmd_code;

  diag_status_rsp_type      stat;

  diag_markov_rsp_type      markov;
  diag_markov_reset_rsp_type    m_reset;

  diag_parm_set_rsp_type    parm_set;

  diag_parm_get2_rsp_type   parm_get2;

#if defined(FEATURE_IS95B_MDR)
  diag_parm_get_is95b_rsp_type   parm_get_is95b;
#endif

 diag_parm_retrieve_rsp_type     parm_retrieve;

#ifdef FEATURE_HWTC
#error code not present
#endif

} diag_rsp_type;

/*===========================================================================

FUNCTION DIAG_GET_EVENT_CNT

DESCRIPTION
  This function returns the count of events maintained by this module.

DEPENDENCIES
  None.

RETURN VALUE
  diag_event_count.

SIDE EFFECTS
  None.

===========================================================================*/
word diag_get_event_cnt(void);

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#endif /* CDMA2KDIAG_H  */

