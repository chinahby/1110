#ifndef FTM_H
#define FTM_H
/*===========================================================================

           F T M    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2002,2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm.h#4 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
2008-07-07 as      Updated with FTM_FMRDS_C
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
11/09/07   ans     Removed un-used typedefs.
11/08/07   vm      Added support for FTM API supporting CDMA Waveform, 
                   CW Waveform and Offset CW Waveform for Tx
11/08/07   ans     Added BC10 support for FTM.
08/24/07   vm      Add the function to return TRUE if current FTM Mode is 
                   RF Cal Mode 
08/01/07    kg     featurization changes for EDGE only support
06/12/07   tm      Updated ftm_event_type to make it reuseable
05/11/07   dp      Add support for UMTS single ended ber 
12/11/06   dbc     Support for MFLO Non-Signaling mode is now under FTM_MF_NS_C 
12/07/06   bt      Added support or MBP (multi broadcast platform).
10/02/06   dbc     Added support for FLO demod. in FTM mode. Featurized by 
                   FTM_HAS_MFLO and FTM_MFLO_SUPPORTS_FLO_DECODE
09/26/06   hkk     Added support for BC15, with featurization
07/06/06   anb     Added support to blow fuses via FTM commands.
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
03/28/06   rsr     Changed ftm_gsm_extended_parameters to be of type uint8, 
                   This variable is a place holder for a buffer. Defining it 
                   as void* implies it can be used as a pointer... when, in fact, 
                   diag returns the first element of the allocated buffer in this
                   variable. If it is defined as a pointer, we could erraneously 
                   dereference the first element in the buffer. 
02/21/06  tws      Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
12/19/05   dp      Add WCDMA 2nd chain command code.  Open up 
                   ftm_receive_chain_handle_type enumeration for WCDMA targets.
11/01/05   rmd     Updated the definition for FTM_PHONE_MODE_CDMA_BC14 and
                   added the definition for FTM_PHONE_MODE_CDMA_BC11.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/12/05   hkk     Changes for TX/RX sweep cal 
05/02/05   dbc     Added inital support for MFLO.
03/22/05   ra      Removed FTM_HAS_UMTS wrapper from ftm_get_mode() to allow all system mode access.
                   Also, deleted ftm_set_mode api located in this file(there is a 1x specific ftm_set_mode which has different 
                   functionality and is not part of this change). It set the global variable ftm_mode however this is to 
                   be done in ftm_activate_protocol and ftm_deactivate_protocol. 
12/03/04   bmg     Added 1x calibration v2 API - merged 03/21/05
03/11/05   ka      Added support for gsm extended commands.
10/31/04   ra      Removed reserved FTM_HWTC command codes
10/19/04   dp      Added FTM_WCDMA_BER_C command code.
10/15/04   ka      Removed unused code.
10/06/04   lp      Added FTM_CAMERA_C command code.
10/05/04   ka      Added mode ID for Customer Extensions.
10/05/04   ka      Added support for FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL.
10/01/04   ra      Renamed FTM_GET_RF_MODE to FTM_GET_RF_DB_STATE.
10/01/04   ra      Renamed FTM_SET_RF_MODE to FTM_SET_RF_DB_STATE.
10/01/04   wd      changed ftm header fields to support rsp_pkt_size.
10/01/04   wd      Added the right diagpkt header structure that defines the
                   subsys cmd id as a word as it should.
10/01/04   et      moved ftm_get_rf_mode prototype outside of FTM_HAS_UMTS
09/17/04   ra      removed FTM_LEDS... there are defined in ftm_task.h
09/08/04   ra      1X merge with UMTS
09/07/04   lcl     Added get IM2 with suppression.
09/03/04   dp      Added access functions for ftm mode variables
08/23/04   xw      Renamed FEATURE_FTM_BT to FEATURE_BT.
08/22/04   ra      Merge 1X and UMTS
08/23/04   xw      Renamed FEATURE_FTM_BT to FEATURE_BT.
8/12/04    ka      Added support for WCDMA 1800 band.
10/02/03   ka      Added support for I2C.
10/02/03   ka      Updated msg macros to remove legacy ids.
10/02/03   ka      Modified includes to fix compilation with FTM_USE_ASYNC_PKT
10/02/03   ka      Added support for variable length response packet.
06/26/03   lcl     Added LED to ftm_task
05/23/03   xw      Removed #define FEATURE_WCDMA_IM2_CAL
02/25/03   xw      Modified ftm_msg_type to support async ftm diag 
12/10/02   xw      Added ftm_rf_mode_type
08/25/02   sb      Initial revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"              /* Target specific definitions            */
#include "comdef.h"              /* Definitions for byte, word, etc.       */
#include "diagpkt.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "queue.h"
#include "rex.h"
#include "prot.h"
#include "msg.h"




#ifdef FTM_HAS_GSM
#error code not present
#endif



/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

/* FTM signals */
#define FTM_CMD_Q_SIG          0x0001    /* A command is available in the queue. */
#define FTM_RPT_TIMER_SIG      0x0002    /* Used to signal a watchdog report.    */ 
#define FTM_HS_CMD_SIG         0x0004    /* This signal is used by the test      */
#define FTM_ACK_SIG            0x0008    /* State change ack signal              */
#define FTM_CMD_RSP_SIG        0x0010    /* Command response signal              */
#define FTM_CLK_WAIT_SIG       0X0020    /* Used generic for ftm_clk_rex_wait    */ 
#define FTM_SIG_WAIT_SIG       0X0040    /* Used generic for ftm_sig_rex_wait    */ 
#define FTM_ASYNC_PKT_SIG      0x0080    /* Used to process asyn packets         */
#define FTM_LEDS_SIG           0x0100    /* Used LEDs update                     */
#define FTM_ENABLE_PROT_SIG    0x0200    /* FTM enable protocol signal           */
#define FTM_MC_READY_SIG       0x0400    /* MC FTM ready siganl                  */
#define FTM_BT_SIG             0x0800    /* BT FTM siganl                        */
#define FTM_SEARCH_SIG         0x1000
#define FTM_NV_CMD_SIG         0x2000
#define FTM_WLAN_SIG           0x4000   /* WLAN FTM siganl                      */
#define FTM_EVT_Q_SIG          0x8000    /* Event signal                         */
#define FTM_NUM_CMDS           5

#define FTM_NUM_HDET_TO_RETURN 32

#if defined( FTM_HAS_UMTS)
#error code not present
#endif 

/*===========================================================================

                                  MACROS

===========================================================================*/
#define IS_FTM_IN_TEST_MODE() ( ftm_get_rf_db_state() !=  FTM_DB_RF_IDLE )


/*===========================================================================
 
                            DATA DECLARATIONS

===========================================================================*/


typedef enum 
{
  AMSS_MODE,
  DMSS_MODE = AMSS_MODE,
  FTM_MODE
} ftm_mode_type;

extern ftm_mode_type ftm_mode;

enum 
{
  FTM_HWTC          = 10,
  FTM               = 11,

  FTM_MAX
};

/* FTM Command Codes. */
enum 
{
  FTM_1X_C           = 0,
  FTM_WCDMA_C        = 1,
  FTM_GSM_C          = 2,
  FTM_1X_RX_2_C      = 3,
  FTM_BT_C           = 4,
  FTM_I2C_C          = 5, 
  FTM_MC_C           = 7,
  FTM_HDR_C          = 8,
  FTM_LOG_C          = 9,
  FTM_AGPS_C         = 10,
  FTM_PMIC_C         = 11,
  FTM_GSM_BER_C      = 13,
  FTM_AUDIO_C        = 14,
  FTM_CAMERA_C       = 15,
  FTM_WCDMA_BER_C    = 16,
  FTM_GSM_EXTENDED_C = 17,
  FTM_1X_CAL_V2_C    = 18,
  FTM_MF_C           = 19,
  FTM_COMMON_C       = 20,
  FTM_WCDMA_RX_2_C   = 21,
  FTM_WLAN_C         = 22,
  FTM_DSPDIAG_C      = 23,
  FTM_QFUSE_C        = 24,
  FTM_MBP_C          = 25,
  FTM_MF_NS_C        = 26,
  FTM_SE_BER_C       = 27,
  FTM_FMRDS_C        = 28,
  FTM_MODE_FACTORY_C = 0x8000,
  FTM_RESERVED_C     = 0x8001,
  FTM_MODE_MAX_C
}; 
typedef enum {
  FTM_DB_RF_CDMA,                     /* RF is in CDMA band (800 or 1900) */
  FTM_DB_RF_CDMA_800,                 /* RF is in CDMA 800 mode of operation */
  FTM_DB_RF_CDMA_1900,                /* RF is in CDMA 1900 mode of operation */
  FTM_DB_RF_WCDMA_IMT,                /* RF is in WCDMA IMT mode of operation          */
  FTM_DB_RF_WCDMA_1900MHZ_A,          /* RF is in 1900MHz WCDMA, band A of operation   */
  FTM_DB_RF_WCDMA_1900MHZ_B,          /* RF is in 1900MHz WCDMA, band B of operation   */
  FTM_DB_RF_GSM_900,                  /* RF is in EGSM mode of operation               */
  FTM_DB_RF_GSM_1800,                 /* RF is in GSM DCS mode of operation            */
  FTM_DB_RF_GPS,                      /* RF is in GPS mode of operation                */
  FTM_DB_RF_SLEEP,                    /* RF is in sleep mode of operation              */
  FTM_DB_RF_IDLE,                     /* RF is in idle FTM mode of operation           */
  FTM_DB_RF_GSM_850,                  /* RF is in GSM 850 band                         */
  FTM_DB_RF_GSM_1900,                 /* RF is in GSM 1900 band                        */
  FTM_DB_RF_WCDMA_BC3,                /* RF is in WCDMA 1800MHz BC3 band               */
  FTM_DB_RF_WCDMA_BC4,                /* RF is in WCDMA 17/21 BC4 band                 */
  FTM_DB_RF_WCDMA_800MHZ,             /* RF is in WCDMA 800MHz band                    */
  FTM_DB_RF_WCDMA_BC8,                /* RF is in WCDMA 900MHz BC8 band                */
  FTM_DB_RF_WCDMA_BC9,                /* RF is in WCDMA 1800MHz BC9 band               */

  FTM_DB_RF_MAX
} ftm_rf_mode_type;

#if (defined(FTM_HAS_1X) || defined(FTM_HAS_UMTS))
typedef enum
{
  FTM_RECEIVE_CHAIN_0,
  FTM_RECEIVE_CHAIN_1
} ftm_receive_chain_handle_type;
#endif

#ifdef FTM_HAS_1X
typedef enum {
  US_PCS_BAND,
  US_CELLULAR_BAND
} digital_band_type;

typedef  enum {
  DISABLE_SECONDARY_CHAIN,
  RECEIVE_DIVERSITY,
  OFF_FREQUENCY_SEARCH
} ftm_secondary_chain_control_enum_type;
#endif


typedef enum
{
  FTM_PRIMARY_CHAIN_ONLY,
  FTM_SECONDARY_CHAIN_ONLY,
  FTM_PRIMARY_AND_SECONDARY
} ftm_second_chain_test_call_type;


typedef enum
{
  FTM_SUCCESS,
  FTM_FAILURE,
  FTM_ERROR_SIZE=0xFFFF,
} ftm_error_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Generic Header used for all FTM Task Commands. */
typedef struct
{
  q_link_type    link;               /* Queue link */
  q_type         *done_q_ptr;        /* Queue to place this cmd on when done */
} ftm_hdr_type;

/* This is the Message which brings all Command packets under one name.  */
typedef struct
{
  ftm_hdr_type           hdr;
  uint16                 req_pkt_len;    /* temp req packet length */
  uint16                 rsp_pkt_len;    /* temp rsp packet length */
  void                   *ftm_data_ptr;  /* pointer to requesting ftm or hwtc data */
  void                   *data_rsp_ptr;  /* pointer to memory allocated for respond packet */
} ftm_msg_type;


typedef enum
{
  FTM_EVENT_DIAG,
  FTM_EVENT_ADSP,
  FTM_EVENT_CAMERA,
} ftm_event_category;


typedef struct
{
  ftm_hdr_type           hdr;
  ftm_event_category     evt_cat;
  boolean                reuse;
  boolean                queued;
  void                   *evt_data_ptr;
} ftm_event_type;


typedef struct
{
  uint16                 ref_ct;           /* helps cleanup */
  uint16                 req_pkt_len;      /* req packet length */
  void                   *ftm_data_ptr;    /* pointer to data */
  int                    max_alloc;
} ftm_diag_event_type;



typedef struct
{
  ftm_diag_event_type                   ftm_diag_event; 
  diagpkt_subsys_delayed_rsp_id_type    delay_rsp_id;
} ftm_diag_128_event_type;


typedef struct
{
  ftm_diag_event_type                   ftm_diag_event; 
  uint16                                ftm_subsys_code;
} ftm_diag_125_event_type;


typedef enum
{
  FTM_PHONE_MODE_CDMA = 0,
  FTM_PHONE_MODE_SLEEP =2,
  FTM_PHONE_MODE_GPS,
  FTM_PHONE_MODE_CDMA_800 =5,
  FTM_PHONE_MODE_CDMA_1900,
  FTM_PHONE_MODE_HDR,
  FTM_PHONE_MODE_CDMA_1800,
  FTM_PHONE_MODE_WCDMA_IMT,
  FTM_PHONE_MODE_GSM_900,
  FTM_PHONE_MODE_GSM_1800,
  FTM_PHONE_MODE_GSM_1900,
  FTM_PHONE_MODE_BLUETOOTH,
  FTM_PHONE_MODE_JCDMA = 14,
  FTM_PHONE_MODE_WCDMA_1900A,
  FTM_PHONE_MODE_WCDMA_1900B,
  FTM_PHONE_MODE_CDMA_450,
  FTM_PHONE_MODE_GSM_850,
  FTM_PHONE_MODE_IMT,
  FTM_PHONE_MODE_HDR_800,
  FTM_PHONE_MODE_HDR_1900,
  FTM_PHONE_MODE_WCDMA_800,
  FTM_PHONE_MODE_WCDMA_BC3=25,
  FTM_PHONE_MODE_CDMA_BC14 = 26,
  FTM_PHONE_MODE_CDMA_BC11 = 27,
  FTM_PHONE_MODE_WCDMA_BC4=28,
  FTM_PHONE_MODE_WCDMA_BC8=29,
  FTM_PHONE_MODE_MF_700=30,
  FTM_PHONE_MODE_WCDMA_BC9=31,
  FTM_PHONE_MODE_CDMA_BC15=32,
  FTM_PHONE_MODE_CDMA_BC10=33,
  FTM_PHONE_MODE_MAX
} ftm_mode_id_type;


typedef enum
{
  FTM_PA_R0,
  FTM_PA_R1,
  FTM_PA_R2,
  FTM_PA_R3,

  FTM_PA_RMAX
} ftm_pa_range_type;

typedef enum
{
  FTM_LNA_RANGE_0,
  FTM_LNA_RANGE_1,
  FTM_LNA_RANGE_2,
  FTM_LNA_RANGE_3,
  FTM_LNA_RANGE_4,

  FTM_LNA_RANGE_MAX
} ftm_lna_range_type;

typedef PACKED struct
{
  word id;
  int2 val;
} ftm_id_val_type;

typedef PACKED struct
{
  byte byte0;
  byte byte1;
  byte byte2;
} ftm_byte_type;

typedef enum
{
  CW_OFFSET_ZER0_KHZ = 0,
  CW_OFFSET_19_2_KHZ = 1
}ftm_tx_waveform_attrib_type;

typedef enum
{
  CDMA_WAVEFORM = 0,
  CW_WAVEFORM = 1
}ftm_tx_waveform_type;

typedef PACKED struct
{
  ftm_tx_waveform_type ftm_tx_waveform;
  ftm_tx_waveform_attrib_type ftm_tx_waveform_attrib;  
}ftm_tx_waveform_data_type;

typedef enum
{
  FTM_PDM_TX_AGC_ADJ = 2,
  FTM_PDM_TRK_LO_ADJ = 4,

  FTM_PDM_MAX_NUM
} ftm_pdm_id_type;

typedef PACKED struct
{
  byte  byte0;
  byte  byte1;
  byte  byte2;
  int16 intv;
} ftm_mix_type;

/* the real definition of the diagpkt_subsys_header_type*/
typedef PACKED struct
{
  diagpkt_cmd_code_type         cmd_code;
  diagpkt_subsys_id_type        subsys_id;
  diagpkt_subsys_cmd_code_type  subsys_cmd_code;
} ftm_diagpkt_subsys_header_type;


/* the real definition of the ftm_diagpkt_subsys_hdr_type_v2 */
typedef PACKED struct
{
  uint8 command_code;
  uint8 subsys_id;
  uint16 subsys_cmd_code;
  uint32 status;  
  uint16 delayed_rsp_id;
  uint16 rsp_cnt; /* 0, means one response and 1, means two responses */
}
ftm_diagpkt_subsys_hdr_type_v2;


typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type    diag_header; /* same as diagpkt_subsys_header_type */
  byte data[1];
} ftm_subsys_cmd_packet_type;


typedef PACKED struct
{
  ftm_diagpkt_subsys_hdr_type_v2 header;
  byte data[1];
} ftm_subsys_rsp_packet_type;


/* FTM PKT Header */
typedef PACKED struct
{
  word cmd_id;            /* command id (required) */
  word cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers 
                             (optional, set to 0 if not used) 
                          */
  word cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt 
                             (optional, set to 0 if not used) 
                          */
} ftm_cmd_header_type;

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
typedef PACKED struct
{  
  int8 index;
  uint32 mask;
} ftm_block_info_type;

#endif


typedef PACKED struct
{
  word ftm_rf_cmd;
  /*  Rf sub-command enums from host */

  PACKED union 
  {
    word                    gen_w;      /* Generic word                      */
    byte                    gen_b;      /* Generic byte                      */
    dword                   gen_d;      /* Generic dword                     */
    word                    chan;       /* For tuning to a specific channel  */
    boolean                 on_off;     /* For anything with an on_off state */
    ftm_mode_id_type        mode;       /* For Setting Phone operation mode  */
    ftm_pa_range_type       range;      /* Set the LNA/PA range              */
#ifdef FTM_HAS_1X
    ftm_secondary_chain_control_enum_type second_chain; /* set second chain to: Disabled, Rx Diversity, OFS */ 
#endif
    ftm_id_val_type         id_val;     /* For all indexed values            */
    ftm_byte_type           byte_struct;
#ifdef FTM_HAS_GSM
#error code not present
#endif
    ftm_mix_type            mix_struct;
    byte                    hdet_array[FTM_NUM_HDET_TO_RETURN]; /* ftm hdet block request/response type */
	uint16                  hdet_array_16_bit[FTM_NUM_HDET_TO_RETURN]; /* ftm hdet block request/response type */
    ftm_tx_waveform_data_type ftm_tx_waveform_data;
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
    ftm_block_info_type ftm_block_info; /* For Calibrating the BC or Sub-block when RF_HAS_SUB_BLOCK_SUPPORT*/
#endif

  }  ftm_rf_factory_data;               /* union of RF commands data types   */
} ftm_rf_factory_parms;

typedef PACKED struct
{
  diagpkt_subsys_header_type header;
  ftm_rf_factory_parms rf_params;
}  ftm_pkt_type;

#if defined( FTM_HAS_UMTS) && defined (FEATURE_MMGPS)
#error code not present
#endif 

/* FTM Composite PKT Header */
typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  diag_hdr;
  ftm_cmd_header_type             ftm_hdr;
} ftm_composite_cmd_header_type;

#ifdef FTM_HAS_GSM
#error code not present
#endif /* FTM_HAS_GSM */

#ifdef FTM_HAS_UMTS
#error code not present
#endif

typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type   diag_hdr;
  ftm_cmd_header_type              ftm_hdr;
  void                             *ftm_extended_params;     
} ftm_common_pkt_type;
/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION FTM_DEACTIVATE_PROTOCOL

DESCRIPTION  
    The function is responsible for release any resource used by FTM mode.  
 (for example, deregister isr handler from gstmr_handlers[],etc)

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_deactivate_protocol  (prot_deact_e_type, prot_trans_type  );
/*===========================================================================

FUNCTION FTM_ACTIVATE_PROTOCOL

DESCRIPTION  
   The function is responsible for getting MB ready to FTM mode.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_activate_protocol ( prot_act_e_type  );
/*===========================================================================

FUNCTION FTM_CMD

DESCRIPTION  
   This function is used to send command to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_cmd(ftm_msg_type *msg_ptr);

/*===========================================================================

FUNCTION FTM_EVENT

DESCRIPTION  
   This function is used to send events to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_event(ftm_event_type *evt_ptr);


/*===========================================================================

FUNCTION FTM_GET_MODE

DESCRIPTION
         

DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
ftm_mode_type  ftm_get_mode( void);


/*===========================================================================

FUNCTION FTM_SET_RF_SB_STATE

DESCRIPTION
         

DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
void ftm_set_rf_db_state( ftm_rf_mode_type  mode );


/*===========================================================================

FUNCTION FTM_GET_RF_DB_STATE

DESCRIPTION
         

DEPENDENCIES
     None.

RETURN VALUE
     None.

SIDE EFFECTS
     None.

===========================================================================*/
ftm_rf_mode_type  ftm_get_rf_db_state( void );

/*===========================================================================

FUNCTION FTM_CURRENT_MODE_IS_RF_CAL

DESCRIPTION  
   Determines if current ftm Mode is RF Cal Mode or not.

DEPENDENCIES
   None.
   
RETURN VALUE
   returns TRUE if FTM Mode is RF Cal Mode and FALSE Otherwise

SIDE EFFECTS
   None
   
===========================================================================*/
boolean ftm_current_mode_is_rf_cal(void);

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_H */

