#ifndef CMDIAG_H
#define CMDIAG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*==========================================================================

                    Call Manager Diagnostic Packets

General Description
  This file contains packet definitions for CM diagnostic packets.

Copyright (c) 2002 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmdiag.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/09   aj      Added support for DIAG_MEAS_MODE_F and DIAG_MEAS_REQ_F 
02/02/09   ks      Adding support for QDSP 6 based compiler
11/04/08   cl      Adding DIAG support for SUPS Command.
09/15/08   np      Added Assisted dialing test code 
05/11/06   sk      Added CMDIAG_SO_VOICE_4GV_NB.
04/19/05   pk      Added CMDIAG_MODE_POWER_OFF_F mode
07/12/04   ic      Added support for WINCE 
02/06/04   ws      Initial jaguar/mainline merge.
07/24/03   vt      removed FEATURE_SPECIAL_MDR
02/28/03   ws      Added #def of CMDIAG_H
11/20/01   as      Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

/* This must be called during initializationfor the packet support to work. */
void cmdiag_init (void);

/* Mode change commands */
typedef enum
{
  CMDIAG_MODE_OFFLINE_A_F = 0,  /* Go to offline analog */
  CMDIAG_MODE_OFFLINE_D_F,      /* Go to offline digital */
  CMDIAG_MODE_RESET_F,          /* Reset. Only exit from offline */
  CMDIAG_MODE_FTM_F,            /* FTM mode - if supported */
  CMDIAG_MODE_ONLINE_F,         /* Online mode - if supported */
  CMDIAG_MODE_LPM_F,            /* LPM mode - if supported */
  CMDIAG_MODE_POWER_OFF_F,        /* Power off mode */
  CMDIAG_MODE_MAX_F             /* Last (and invalid) mode enum value */
}
cmdiag_mode_enum_type;


#ifdef T_WINCE
#error code not present
#endif
/*==========================================================================

PACKET   DIAG_CONTROL_F

PURPOSE  Request sent from the DM to the DMSS to change the operating mode

============================================================================*/
typedef PACKED struct PACKED_POST
{
  byte xx_cmd_code;
  word mode;            /* Mode to change to */
}
cmdiag_mode_control_req_type;

typedef cmdiag_mode_control_req_type cmdiag_mode_control_rsp_type;


/* Service options for call origination. */

#define CMDIAG_SO_NONE            0x00
#define CMDIAG_SO_LOOPBACK        0x02
#define CMDIAG_SO_MARKOV          0x03
#define CMDIAG_SO_DATA            0x04
#define CMDIAG_SO_VOICE_96A       0x05
#define CMDIAG_SO_VOICE13         0x06  /* 13K (MSM2 only) Voice call */
#define CMDIAG_SO_RS2_MARKOV      0x07  /* 13K (MSM2 only) Markov call (new) */
#define CMDIAG_SO_RS1_MARKOV      0x08
#define CMDIAG_SO_LOOPBACK_13K    0x09
#define CMDIAG_SO_MARKOV_13K      0x0A  /* 13K (MSM2 only) Markov call (old) */
#define CMDIAG_SO_EVRC            0x0B  /* EVRC voice option */
/* CAI SMS SO (6) value is overridden with a proprietary value from this
   list, so use unlikely SO value. */
#define CMDIAG_SO_SMS             0xFFFE
/* CAI_SO_RS2_SMS (14) is not in this proprietary list, so use the CAI SO. */
#define CMDIAG_SO_RS2_SMS         0x0E
#define CMDIAG_SO_VOICE_13K_IS733 0x11  /* IS-733 s.o. number */
#define CMDIAG_SO_MARKOV_SO54     0x36
#define CMDIAG_SO_LOOPBACK_SO55   0x37
#define CMDIAG_SO_VOICE_4GV_NB    0x44

#if defined(FEATURE_IS95B_MDR)
/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and reverse.  Default Mux = 9 forward 1 reverse */
#define CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS1   0x16 /* IS-707A s.o. number = 22 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and rate set 2 reverse.
 Default Mux = 9 forward 2 reverse.  This SO is not support by MSM3000. */
#define CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS2   0x17 /* IS-707A s.o. number = 23 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and rate set 1 reverse.
 Default Mux = 10 forward 1 reverse.  This SO is not support by MSM3000. */
#define CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS1   0x18 /* IS-707A s.o. number = 24 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and reverse.  Default Mux = 10 forward 2 reverse */
#define CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS2   0x19 /* IS-707A s.o. number = 25 */

#define CMDIAG_SO_MDR_DPT_FRS1_RRS1        0x24 /* MDR Data Pattern Test: Proprietary
                           Service Option rate set 1 fwd and rev */

#define CMDIAG_SO_MDR_DPT_FRS2_RRS2        0x25 /* MDR Data Pattern Test: Proprietary
                           Service Option rate set 2 fwd and rev */
#endif /* defined(FEATURE_IS95B_MDR) */

/*===========================================================================

PACKET   DIAG_ORIG_F

PURPOSE  Sent by DM to originate a call.

RESPONSE DMSS originates a call using the specified number.

===========================================================================*/
#define CMDIAG_NV_MAX_DIAL_DIGITS 32

typedef PACKED struct PACKED_POST
{
  byte cmd_code;        /* Command code */
  byte cnt;         /* number of digits in dialed_digits array */
  char dialed_digits[CMDIAG_NV_MAX_DIAL_DIGITS];
  word so;
  /* desired service option - 0 = Reserved,  1 = Voice (IS96A), 2 = Loopback,
     8001 = Voice (IS96), 8002 = Markov, 8003 = Data (see cai.h) */
}
cmdiag_orig_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_orig_rsp_type;


/*===========================================================================

PACKET   DIAG_END_F

PURPOSE  Sent by DM to end a call.

RESPONSE DMSS ends the current call.

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_end_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_end_rsp_type;

/*===========================================================================

PACKET   DIAG_MEAS_MODE_F

PURPOSE  Sent by DM to change the measurement mode

RESPONSE DMSS changes the measurement mode

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;
  sys_meas_mode_e_type mode;            /* Mode to change to */
}
cmdiag_meas_mode_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_meas_mode_rsp_type;

/*===========================================================================

PACKET   DIAG_MEAS_REQ_F

PURPOSE  Sent by DM to request measurement

RESPONSE DMSS returns the measurement result

===========================================================================*/

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
  sys_sys_mode_e_type       sys_mode;
  PACKED union PACKED_POST
  {
    PACKED struct PACKED_POST
    {
      boolean               srch_win_size_inc; /**< indicates if search 
                                                **  window size is inc */
      byte                  srch_win_size;     /**< search window size */
      sys_channel_num_type  chan_num;          /** HDR channel number */
      sys_band_class_e_type band;              /** HDR band class */
      byte                  neighbor_count;    /**< number of neighbours  */
      sys_pilot_pn_type     neighbor_pilot_pn [ CM_MAX_DO_NEIGHBOR_CNT ]; 
                                             /**< pilot PNs of neighbours */
    }hdr_req;
  }req;
}cmdiag_meas_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_meas_rsp_type;

/*===========================================================================

PACKET   DIAG_CNTRY_INFO_F

PURPOSE  Sent by DM to query country network information.

RESPONSE CM sends the current country information corresponding to MCC

===========================================================================*/
/**< Length of international dialed digit */
#define  CMDIAG_IDD_DIGIT_LEN 7
/**< Length of national dialed digit */
#define CMDIAG_NDD_DIGIT_LEN    7
/**< Length of SPC */
#define CMDIAG_SPC_DIGIT_LEN    7
/**< Max entry of SPC */
#define CMDIAG_MAX_ENTRY_SPC   5

typedef PACKED struct
{
  byte cmd_code;
  byte pad; /* byte alighment*/
  word mcc;/* Mobile country code*/
}
cmdiag_cntry_info_req_type;


typedef PACKED struct
{
   word sid1; /* Starting of Sid range inclusive of sid1*/
   word sid2; /* end of Sid range inclusive of sid2 */
   word country_code;    /* cc carries the country code directly, Array to accommodate a leading 0 */
                    /* 0 can be neglected for now, used for some zones */
   char inter_dialed_digits[CMDIAG_IDD_DIGIT_LEN]; /* idd digits start after last F */
   char special_prefix_code[CMDIAG_SPC_DIGIT_LEN]; /* spc digits start after last F */
   char national_direct_dial[CMDIAG_NDD_DIGIT_LEN]; /* national direct dial */
   boolean is_network_soln; /* network solution */
   word  mobile_country_code;   /* Mobile country code */
}
cmdiag_cntry_info_type;

typedef PACKED struct
{  
   byte cmd_code;
   byte pad1; /* byte alignment*/ 
   word sid1; /* Starting of Sid range inclusive of sid1*/
   word sid2; /* end of Sid range inclusive of sid2 */
   word country_code;    /* cc carries the country code directly, Array to accommodate a leading 0 */
                    /* 0 can be neglected for now, used for some zones */
   char inter_dialed_digits[CMDIAG_IDD_DIGIT_LEN]; /* idd digits start after last F */
   byte pad2; /* Byte alignment*/         
   char special_prefix_code[CMDIAG_SPC_DIGIT_LEN]; /* spc digits start after last F */
   byte pad3; /* Byte alignment*/         
   char national_direct_dial[CMDIAG_NDD_DIGIT_LEN]; /* national direct dial */
   word  mobile_country_code;   /* Mobile country code */
   byte is_network_soln; /* network solution */

}
cmdiag_cntry_info_rsp_type;


/*===========================================================================

PACKET   DIAG_SUPS_F

PURPOSE  Sent suplementary service request

RESPONSE DMSS process Suplementary Service Request

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;      /* Command Code */
  byte cnt;           /* number of digits in dialed_digits array */
  char dialed_digits[CMDIAG_NV_MAX_DIAL_DIGITS];
  word sups_type;     /* Suplementary Service Type */
}
cmdiag_sups_req_type;

typedef PACKED struct
{
  byte cmd_code;
}
cmdiag_sups_rsp_type;

#ifdef T_WINCE
#error code not present
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMDIAG_H */

