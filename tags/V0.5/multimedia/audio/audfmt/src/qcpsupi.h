#ifndef QCPSUPI_H
#define QCPSUPI_H
/*===========================================================================

           Q C P   F O R M A T    U T I L I T I E S    I N T E R N A L
                         H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the QCP format utilites module.

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/qcpsupi.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
04/07/07    pl     Modified featurization for AMR-NB playback support
                   on MIDI_OUT interface.
03/21/07    at     Added qcpsup_evb_frame_type_enum for EVRC-B file support.
06/01/06    sg     Fixed featurization so that G711 content can be played
                   when the build does not support QSYNTH.
04/28/05   ymc     Added 4GV QCP file support.
           ymc     Updated Copyright.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
09/26/03    sm     Set QCPSUP_AMR_TYPE_NO_DATA = 15 so that blank frames are
                   not rejected.
04/17/03    sm     Added support for AMR file format.
11/06/02    sm     Added QCP AMR support.
09/19/01    st     Changed QCPSUP_RATE_EVRC_QUARTER_SIZE to 5 to handle
                   quarter rate EVRC frames properly, CR18371.
09/03/01    st     Modified code to support EVRC QCP files; added cnfg, labl
                   and text chunk IDs.
04/04/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB)
#define QCPSUP_RIFF_ID             "RIFF"
#define QCPSUP_QLCM_ID             "QLCM"
#define QCPSUP_DATA_ID             "data"
#define QCPSUP_OFFS_ID             "offs"
#define QCPSUP_VRAT_ID             "vrat"
#define QCPSUP_FMT_ID              "fmt "
#define QCPSUP_CNFG_ID             "cnfg"
#define QCPSUP_LABL_ID             "labl"
#define QCPSUP_TEXT_ID             "text"

#define QCPSUP_LENGTH_BYTES        4
#define QCPSUP_CHUNK_ID_BYTES      4
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP ||
          FEATURE_WAVE_PB */

#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP)

typedef enum {
  QCPSUP_RATE_BLANK   = 0x0,
  QCPSUP_RATE_EIGHTH  = 0x1,
  QCPSUP_RATE_QUARTER = 0x2,
  QCPSUP_RATE_HALF    = 0x3,
  QCPSUP_RATE_FULL    = 0x4,
  QCPSUP_RATE_ERASURE = 0xE,

  QCPSUP_RATE_4_75_AMR = 0x01, /* Fixed- 4.75 kbit/s, AMR            */
  QCPSUP_RATE_5_15_AMR = 0x02, /* Fixed- 5.15 kbit/s, AMR            */
  QCPSUP_RATE_5_9_AMR  = 0x03, /* Fixed- 5.9  kbit/s, AMR            */
  QCPSUP_RATE_6_7_AMR  = 0x04, /* Fixed- 6.7  kbit/s, AMR            */
  QCPSUP_RATE_7_4_AMR  = 0x05, /* Fixed- 7.4  kbit/s, AMR            */
  QCPSUP_RATE_7_95_AMR = 0x06, /* Fixed- 7.95 kbit/s, AMR            */
  QCPSUP_RATE_10_2_AMR = 0x07, /* Fixed-10.2  kbit/s, AMR            */
  QCPSUP_RATE_12_2_AMR = 0x08, /* Fixed-12.2  kbit/s, AMR            */

  QCPSUP_RATE_FULL_EFR = 0x09, /* Fixed full rate EFR                */
  QCPSUP_RATE_FULL_FR  = 0x0a, /* Fixed full rate FR                 */
  QCPSUP_RATE_FULL_HR  = 0x0b, /* Fixed full rate HR                 */
  
  QCPSUP_RATE_MAX     = 0xFF   /* Placeholder for featurization      */
} audqcp_frame_rate_type;

/* Sizes of various 13K packets, in bytes */
#define QCPSUP_RATE_13K_BLANK_SIZE   0
#define QCPSUP_RATE_13K_EIGHTH_SIZE  3
#define QCPSUP_RATE_13K_QUARTER_SIZE 7
#define QCPSUP_RATE_13K_HALF_SIZE    16
#define QCPSUP_RATE_13K_FULL_SIZE    34

/* Sizes of various EVRC packets, in bytes */
#define QCPSUP_RATE_EVRC_BLANK_SIZE   0
#define QCPSUP_RATE_EVRC_EIGHTH_SIZE  2
#define QCPSUP_RATE_EVRC_QUARTER_SIZE 5
#define QCPSUP_RATE_EVRC_HALF_SIZE    10
#define QCPSUP_RATE_EVRC_FULL_SIZE    22

/* Sizes of various 4GV packets, in bytes */
#define QCPSUP_RATE_4GV_NB_BLANK_SIZE   0
#define QCPSUP_RATE_4GV_NB_EIGHTH_SIZE  2
#define QCPSUP_RATE_4GV_NB_QUARTER_SIZE 5
#define QCPSUP_RATE_4GV_NB_HALF_SIZE    10
#define QCPSUP_RATE_4GV_NB_FULL_SIZE    22

typedef enum {
  QCPSUP_AMR_SPEECH_GOOD          = 0x00,  /* AMR mode frame type definitions */
  QCPSUP_AMR_SPEECH_DEGRADED      = 0x01,
  QCPSUP_AMR_ONSET                = 0x02,
  QCPSUP_AMR_SPEECH_BAD           = 0x03,
  QCPSUP_AMR_SID_FIRST            = 0x04,
  QCPSUP_AMR_SID_UPDATE           = 0x05,
  QCPSUP_AMR_SID_BAD              = 0x06,
  QCPSUP_AMR_NO_DATA              = 0x07
} audqcp_amr_frame_mode_type;

/* Macros to extract and manipulate the QCP AMR header byte */
#define QCPSUP_EXTRACT_AMR_RATE(x) ((audqcp_frame_rate_type)((x)&0xf))

#define QCPSUP_EXTRACT_AMR_FRAME_MODE(x) ((audqcp_amr_frame_mode_type)((x)>>4))

/* Sizes of various AMR packets, in bytes */
#define QCPSUP_RATE_AMR_BLANK_SIZE 0
#define QCPSUP_RATE_AMR_SID_SIZE   5
#define QCPSUP_RATE_AMR_475_SIZE   12
#define QCPSUP_RATE_AMR_515_SIZE   13
#define QCPSUP_RATE_AMR_590_SIZE   15
#define QCPSUP_RATE_AMR_670_SIZE   17
#define QCPSUP_RATE_AMR_740_SIZE   19
#define QCPSUP_RATE_AMR_795_SIZE   20
#define QCPSUP_RATE_AMR_102_SIZE   26
#define QCPSUP_RATE_AMR_122_SIZE   31

#if defined(FEATURE_AUDFMT_AMR) || (defined(FEATURE_MIDI_OUT_QCP) \
    && defined(FEATURE_GASYNTH_INTERFACE))
/* Defines to extract frame type and quality from frame header. */
#define AUDAMR_FRAME_TYPE_MASK     0x78
#define AUDAMR_FRAME_TYPE_SHIFT    3

typedef enum {
  QCPSUP_AMR_TYPE_0475,
  QCPSUP_AMR_TYPE_0515,
  QCPSUP_AMR_TYPE_0590,
  QCPSUP_AMR_TYPE_0670,
  QCPSUP_AMR_TYPE_0740,
  QCPSUP_AMR_TYPE_0795,
  QCPSUP_AMR_TYPE_1020,
  QCPSUP_AMR_TYPE_1220,
  QCPSUP_AMR_TYPE_AMR_SID,
  QCPSUP_AMR_TYPE_NO_DATA = 15
} qcpsup_amr_frame_type_enum;
#endif /* FEATURE_AUDFMT_AMR || 
          (FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE)  */

/* .AWB file Frame type and Frame size definations */
/*
Frame Type Index	Mode Indication	Mode Request	Frame content (AMR-WB mode, comfort noise, or other)
0	        0	                0	                                        AMR-WB 6.60 kbit/s 
1	        1	                1	                                        AMR-WB 8.85 kbit/s 
2	        2	                2	                                        AMR-WB 12.65 kbit/s 
3	        3	                3	                                        AMR-WB 14.25 kbit/s 
4	        4	                4	                                        AMR-WB 15.85 kbit/s 
5	        5               	5                                        	AMR-WB 18.25 kbit/s 
6	        6	                6	                                        AMR-WB 19.85 kbit/s 
7	        7	                7	                                        AMR-WB 23.05 kbit/s 
8	        8	                8	                                        AMR-WB 23.85 kbit/s
9	        -	                -	                                        AMR-WB SID (Comfort Noise Frame)
10-13	    -	                -	                                        For future use
14	        -	                -	                                        speech lost
15	        -	                -	                                        No Data (No transmission/No reception)
*/

/* Defines to extract frame type and quality from frame header. 
    0 1 2 3 4 5 6 7
   +-+-+-+-+-+-+-+-+
   |P|  FT   |Q|P|P|
   +-+-+-+-+-+-+-+-+
*/

#define AUDAWB_FRAME_TYPE_MASK     0x78
#define AUDAWB_FRAME_TYPE_SHIFT    3

typedef enum {
  QCPSUP_AWB_TYPE_0660 = 0,
  QCPSUP_AWB_TYPE_0885,
  QCPSUP_AWB_TYPE_1265,
  QCPSUP_AWB_TYPE_1425,
  QCPSUP_AWB_TYPE_1585,
  QCPSUP_AWB_TYPE_1825,
  QCPSUP_AWB_TYPE_1985,
  QCPSUP_AWB_TYPE_2305,
  QCPSUP_AWB_TYPE_2385,
  QCPSUP_AWB_TYPE_SID,
  QCPSUP_AWB_TYPE_SPEECH_LOST = 14,
  QCPSUP_AWB_TYPE_NO_DATA = 15

} qcpsup_amr_wb_frame_type_enum;



/*
Frame Type	AMR-WB codec mode	Total number of bits	Class A	Class B	Class C
0	                6.60	            132	                54	    78	    0
1	                8.85	            177	                64	    113	    0
2	                12.65	            253	                72	    181	    0
3               	14.25	            285	                72	    213	    0
4	                15.85	            317	                72	    245	    0
5	                18.25           	365	                72	    293	    0
6	                19.85	            397	                72	    325	    0
7	                23.05	            461             	72	    389	    0
8	                23.85	            477	                72	    405	    0
9                   SID                  40                 40       0      0
14                  speech lost           0
15                  No Data               0 
*/

/* Sizes of various AMR-WB packets, in bytes */
#define QCPSUP_AWB_RATE_0660_FRAME_SIZE      17
#define QCPSUP_AWB_RATE_0885_FRAME_SIZE      23
#define QCPSUP_AWB_RATE_1265_FRAME_SIZE      32
#define QCPSUP_AWB_RATE_1425_FRAME_SIZE      36
#define QCPSUP_AWB_RATE_1585_FRAME_SIZE      40
#define QCPSUP_AWB_RATE_1825_FRAME_SIZE      46
#define QCPSUP_AWB_RATE_1985_FRAME_SIZE      50
#define QCPSUP_AWB_RATE_2305_FRAME_SIZE      58
#define QCPSUP_AWB_RATE_2385_FRAME_SIZE      60
#define QCPSUP_AWB_SID_FRAME_SIZE            5
#define QCPSUP_AWB_SPEECH_LOST_FRAME_SIZE    0
#define QCPSUP_AWB_NO_DATA_FRMAE_SIZE        0



#ifdef FEATURE_AUDFMT_EVB
/* EVRC-B file format frame types */
typedef enum {
  QCPSUP_EVB_RATE_BLANK = 0,
  QCPSUP_EVB_RATE_EIGHTH,
  QCPSUP_EVB_RATE_QUARTER,
  QCPSUP_EVB_RATE_HALF,
  QCPSUP_EVB_RATE_FULL,
  QCPSUP_EVB_RATE_ERASURE
} qcpsup_evb_frame_type_enum;
#endif /* FEATURE_AUDFMT_EVB */

/* Sizes of various GSM packets, in bytes */
#define QCPSUP_RATE_GSM_BLANK_SIZE   0
#define QCPSUP_RATE_GSM_EFR_SIZE     31
#define QCPSUP_RATE_GSM_EFR_SID_SIZE 12
#define QCPSUP_RATE_GSM_FR_SIZE      33
#define QCPSUP_RATE_GSM_FR_SID_SIZE  12
#define QCPSUP_RATE_GSM_HR_SIZE      14
#define QCPSUP_RATE_GSM_HR_SID_SIZE  10

/* EFR, FR, HR support */
typedef enum {
  QCPSUP_GSM_SID,                 /* GSM HR, FR or EFR : silence descriptor   */
  QCPSUP_GSM_SPEECH_GOOD,         /* GSM HR, FR or EFR : good speech frame    */
  QCPSUP_GSM_BFI,                 /* GSM HR, FR or EFR : bad frame indicator  */
  QCPSUP_GSM_INVALID_SID,         /* GSM HR            : invalid SID frame    */
  QCPSUP_GSM_NO_DATA              /* GSM HR, FR or EFR : no data (DTX frame)  */
} audqcp_gsm_frame_mode_type;

/* Macros to extract and manipulate the QCP EFR/FR header byte */
#define QCPSUP_EXTRACT_GSM_RATE(x) ((audqcp_frame_rate_type)((x)&0xf))

#define QCPSUP_EXTRACT_GSM_FRAME_MODE(x) ((audqcp_gsm_frame_mode_type)((x)>>4))

#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#endif /* QCPSUPI_H */
