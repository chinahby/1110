#ifndef SRCHLOG_H
#define SRCHLOG_H
/*===========================================================================

                  S R C H L O G    H E A D E R    F I L E

DESCRIPTION
   This file contains log types used internally by the Search task.
   It should NOT be included by any source file outside the Search task!

    Copyright (c) 2001 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srchlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/08   vks     Featurize instead of deleting SAMPLE RAM LOG ENTRY TYPE
                   and SECTOR MODULE LOG ENTRY TYPE
08/21/08   vks     Remove SAMPLE RAM LOG ENTRY TYPE and SECTOR MODULE LOG 
                   ENTRY TYPE declarations which are no longer in use
04/24/08   ljl     Move srch4 logging definations to srch4 files
01/16/06   awj     Moved to srch_stm interface
09/13/05   ejv     Remove obsolete (broken) triage logging.
08/05/05   br      Removed PPM log packet declarations.
05/04/05   kwo     Removed include srchzz_ctl.h
09/30/04   jcm     Moved searcher reserved log codes to srch_genlog.h
09/28/04   jcm     AFC subpacket is now defined in srch_genlog.h
09/16/04   jcm     Moved SRCH_AFC_LOG_BUFFER_SIZE to srch_afc_t.h
07/07/04   ejv     Add T_WINCE.
06/21/04   gs      Updated AFC log packet to version 2.
06/01/04   gs      Changed accumulator types in AFC log packet to signed    
05/28/04   jcm     Added search sector module log packet
05/18/04   va      Added log packet for AFC
05/12/04   hxw     Moved GPSSRCH related logs into gpssrchlog.h
05/04/04   sst     Merge from srch4 3Q dev branch
                   Changed include to be srch4trans_t.h
04/30/04   kwo     Extended sleep stats packet
03/09/04   ejv     Removed FEATURE_MSM5105_PPM_SRCH_ENGINE.
07/17/03   kwo     Moved generalized pilot set to srch_diag module
07/17/03   bt      Include Xfer reason into QPCH log packet V2.
07/17/03   sst     In srch4 log pkt, added rx_agc0 and rx_agc1
07/15/03   kwo      change rf agc to a signed value in sleep sleep stats log 
07/08/03   sst     Added Srch4 log packet definition
07/07/03   ejv     Mainline FEATURE_TRIAGE_LOGGING.
07/03/03   kwo     Added Sleep Stats log packet
06/23/03   ejv     Remove FEATURE_SEARCH3_PPM_ENGINE.
05/20/03   rdh     Added Gen Pilot Sets packet, mainlined JCDMA monitor.
04/02/03   bt      modified QPCH v2 packet and removed PACKED from QPCH 
                   structures. They align properly.
03/31/03   bt      Moved QPCH log packet from log_dmss.h and added new QPCH 
                   packet.
02/18/03   va      SRCH4 AFLT support.
02/05/03   gs/jyw  Removed an unused field and added new ones to bit-edge logging
12/12/02   lad     Fixed triage logging to handle MAX_FING_CNT > 4.
                   Added version 2 of the triage logging packet.
12/03/02   pj      Changed GPS multipeak version ID to 7. Added new fields for
                   bit-edge logging.
07/30/02   gs      Initial merge into perforce for cougar.
05/21/02   ejv     Mainlined FEATURE_IS2000.
05/09/02   abh     Added a log for Sample Ram logging. 
05/08/02   hxw     Incremented GPS_MULTIPEAK_LOG_VERSION_ID to 6 for Ultra
                   Medium Mode support.
04/22/02   aar     Added grid_id and changed rollover_chip to first_deskew in
                   the bit edge detection log.
04/17/02   br      Fixed Searcher 4 support. Added FEATURE_SEARCH3_PPM_ENGINE.
04/16/02   aar     Added log structure for single peak/MS-Based srch logging.
04/10/02   aar     Added log structure for bit edge detection results.
03/21/02   br      Added support for GPS and PPM logs.
11/15/01   ejv     Changed FEATURE_TRIAGE_DEBUG to FEATURE_TRIAGE_LOGGING.
10/03/01   ejv     Added triage debug packet structures from srch.c.
10/01/01   ejv     Initial revision.

===========================================================================*/

#ifdef CUST_H
#include "customer.h"
#endif
#include "log.h"
#include "cai.h"
#include "srch.h"
#include "srch_genlog.h"

#include "diagpkt.h"
#include "diagbuf.h"
#include "diagcmd.h"

/*===========================================================================

                QPCH LOG PACKET DECLARATIONS

===========================================================================*/

/***************************************************************************
*               QPCH V1                                                    *
***************************************************************************/
/* The following structures do not need to be packed as their data fields 
   align without space being wasted. This has to be revisited if the any 
   structure is changed */
   
/* Array index values for QPCH indicators array.
*/
enum {
  LOG_QPCH_PI1_IND  = 0, /* PI Indicator bit #1  */
  LOG_QPCH_PI2_IND  = 1, /* PI Indicator bit #2  */
  LOG_QPCH_CCI1_IND = 2, /* CCI Indicator bit #1 */
  LOG_QPCH_CCI2_IND = 3, /* CCI Indicator bit #2 */
  LOG_QPCH_BI1_IND  = 4, /* BI Indicator bit #1  */
  LOG_QPCH_BI2_IND  = 5, /* BI Indicator bit #2  */
  LOG_QPCH_MAX_IND
};

/* Status values associated with each indicator bit entry.
*/
typedef enum {
  LOG_QPCH_NOT_APPLICABLE_S = 0, /* This indicator entry is not applicable */

  LOG_QPCH_IND_NO_DETECT_S  = 1, /* Indicator bit not detected             */
  LOG_QPCH_IND_DETECT_S     = 2, /* Indicator bit detected                 */
  LOG_QPCH_IND_ERASURE_S    = 3, /* Indicator bit was an erasure           */
  LOG_QPCH_IND_MISSED_S     = 4  /* Indicator bit missed                   */

} qpch_status_enum_log_type;

/* Status values associated with each indicator bit entry.
*/


typedef struct {
  uint8  status;         /* Use qpch_status_enum_log_type           */
  uint8  type;           /* Use qpch_indicator_enum_log_type        */

  uint16 position;       /* Indicator bit position (ignore for CCI) */

  int16  ind_I_energy;   /* QPCH indicator bit energy on I channel  */
  int16  ind_Q_energy;   /* QPCH indicator bit energy on Q channel  */
  int16  pilot_I_energy; /* Pilot energy on I channel               */
  int16  pilot_Q_energy; /* Pilot energy on Q channel               */

} qpch_indicator_log_type;

/* QPCH indicator log definition
*/
/* LOG_QPCH_C */
typedef struct
{
  log_hdr_type hdr;

  uint8  chan;       /* See mccqpch_channel_type               */
  uint8  rate;       /* See mccqpch_rate_type                  */

  uint16 pilot_pn;   /* Pilot PN offset.                       */

  int8  pi_level;    /* QPCH Power level in dB                 */
  byte  thi;         /* THI threshold for PI decision          */
  byte  thb;         /* THB threshold for PI decision          */

  uint8 num_entries; /* # of entries in the following array    */

  qpch_indicator_log_type ind[LOG_QPCH_MAX_IND];

}qpch_log_type;

/***************************************************************************
*               QPCH V2                                                    *
***************************************************************************/
typedef struct {
  uint8  walsh;          /* walsh code                                */
  int8   pwr_level;      /* QPCH Power level in dB                    */
  uint8  thi;            /* THI threshold for PI decision             */
  uint8  thb;            /* THB threshold for PI decision             */
  
} qpch_indicator_info_log_2_type;

/* QPCH indicator log definition
*/
/* LOG_QPCH2_C */
typedef struct
{
  log_hdr_type hdr;

  uint16  pilot_pn;                       /* Pilot PN offset.                */
  uint16  slot_number;                    /* number of slot */
  uint8   xfer_reason__rate;              /* QPCH xfer reason according to 
                                             definition in srchzz_qpch_sm and 
                                             rate acc. to mccqpch_rate_type  */
  
  qpch_indicator_info_log_2_type pi_info; /* info for paging indicators      */

  boolean bi_supported;                   /* Broadcast Indicators supported  */
  qpch_indicator_info_log_2_type bi_info; /* valid if bi_supported is true   */

  boolean cci_supported;        /* Configuration Change Indicators supported */
  qpch_indicator_info_log_2_type cci_info; /* valid if cci_supported is true */
    
  uint8 num_entries;            /* # of entries in the following array       */
  qpch_indicator_log_type ind[LOG_QPCH_MAX_IND];

}qpch_log_2_type;


/*===========================================================================

                SEARCHER & FINGER LOG PACKET DECLARATIONS

===========================================================================*/

#ifdef FEATURE_IS2000_REL_A
/*--------------------------------------------------------------------------
                    Searcher/Finger Log Packet Ver2
--------------------------------------------------------------------------*/

#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */
typedef PACKED struct
{
  byte  ref_buf[1];
    /* Fill the Reference Buffer with the following:
       - the appropriate caii_add_pilot_rec_type info, based on
         the pilot_rec_type,
       - as many reserved bytes as necessary to complete a 32 bit boundary. */
}
srch_pilot_record_type;

typedef PACKED struct
{
  word  path_pos;
    /* The position of the path/peak, in 1/2 chip units */

  word  path_eng;
    /* The path/peak energy */
}
srch_path_info_type;
  /* Stores one peak position and the associated energy */

typedef PACKED union
{
  PACKED struct
  {
    byte  sync      :1;
    byte  pch       :1;
    byte  bcch      :1;
    byte  fccch     :1;
    byte  fch       :1;
    byte  dcch      :1;
    byte  sch       :1;
      /* Each bit is TRUE if the finger is assigned/enabled to the channel */

    byte  reserved  :1;

  }
  individual_chan;

  byte  mask;
    /* Used to init the entire field */
}
srch_demod_chan_type;
  /* Indicates the fingers assigned channels */

typedef PACKED union
{
  PACKED struct
  {
    word  reserved    :1;
    word  assgn       :1;
      /* True if the finger is assigned */

    word  comb_lock   :1;
      /* True if the finger is locked in the combiner */

    word  reserved_2  :13;
  }
  status_fields; 

  word  mask;
    /* Use to init entire mask. */
}
srch_fing_status_type;
  /* Indicates the fingers assignment and lock status */

typedef PACKED struct
{
  srch_demod_chan_type   chan;
    /* Indicates the fingers assigned channels */

  cai_pilot_rec_type     pilot_rec_type;
    /* Indicates the pilot type that this finger is assigned to */

  word                   f_pilot;
    /* Pilot the finger is assigned to */

  byte                   f_sect;
    /* Finger sector */

  byte                   f_rssi;
    /* Finger RSSI */

  srch_fing_status_type  status_mask;
    /* Indicates the fingers assignment and lock status */

  dword  f_pos;
    /* Finger position */

  byte  ref_buf[1];

  /* If the pilot is not a common pilot , include 1 instance of
     srch_pilot_record_type, otherwise, the reference buffer
     is not included */
}
srch_fing_info_type;
  /* Includes all pertinent finger information */

LOG_RECORD_DEFINE(LOG_SRCH_FING_INFO_VER_2_C)
  byte                band_class;
    /* Band Class for this sector:  0 = 800mhz, 1 = PCS */

  byte                pregain;
    /* Gain factor before the second accumulators in the searcher */

  byte                integ;
    /* Number of 4-chips to integrate before converting to energy */

  byte                non_coh;
    /* Number of passes to accumulate non-coherently */

  word                pilot_off;
    /* Pilot PN offset */

  word                win_size;
    /* Window size used to search the sector, in 1/8 chips */

  int4                win_pos;
    /* Window position after the search has finished, in 1/8 chips - note
       that (win_pos - (win_size/2) = win_center) */

  byte                rx_agc;
  byte                tx_agc;
  byte                tx_gain_adj;
  byte                tx_pwr_limit;
    /* RF related samples */

  byte                srch_state;
    /* Current state of the SRCH task */

  cai_pilot_rec_type  pilot_rec_type;
    /* Indicates the pilot type of this sector */

  byte                num_peaks;
    /* The number of peaks reported */

  byte                num_fingers;
    /* The number of fingers reported */

  byte                ref_buf[1];
    /*  Fill the Reference Buffer with the following:
        - If pilot_rec_type does not indicate a common pilot include 1
          instance of srch_pilot_record_type
        - If pilot_rec_type indicates a TD pilot, include (3 * num_peaks)
          instances of srch_path_info_type. Otherwise, include num_peaks
          instances of srch_path_info_type.
        - Include num_fingers instances of srch_fing_info_type */
LOG_RECORD_END
  /* The base structure for the log packet, includes basic searcher info as
     well as a link search results and finger info */
#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */

#endif /* FEATURE_IS2000_REL_A */


/*===========================================================================

                  GENERAL LOG PACKET DECLARATIONS

===========================================================================*/

/* sub packet masks for the srchlog_sleep_stats sub_packet_mask */
#define  SRCHLOG_SLEEP_STATS_REACQ_MASK                0x80
#define  SRCHLOG_SLEEP_STATS_IS2000_MASK               0x40
#define  SRCHLOG_SLEEP_STATS_IS95A_MASK                0x20
#define  SRCHLOG_SLEEP_STATS_QPCH_ONTL_MASK            0x10
#define  SRCHLOG_SLEEP_STATS_REACQ_FAIL_MASK           0x08
#define  SRCHLOG_SLEEP_STATS_FAKE_REACQ_MASK           0x04
#define  SRCHLOG_SLEEP_STATS_NO_TIME_FOR_SLEEP_MASK    0x02

/* nothing is logged if we don't do a reacq */
#define  SRCHLOG_SLEEP_STATS_NO_REACQ_MASK             0x00

/* nothing is logged if sleep isn't attempted */ 
#define  SRCHLOG_SLEEP_STATS_NO_SLEEP_MASK             0x00

/* mask to check if sleep happened */
#define  SRCHLOG_SLEEP_STATS_SLEPT_MASK                0x70

#define  SRCHLOG_SLEEP_STATS_MAX_SUB_PACKET_SIZE  20 

#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */
typedef PACKED struct
{
  log_hdr_type xx_hdr; /* place holder - do not reference directly */

  uint8  sub_packet_mask;    /* mask indicating which sub packets are present */
  uint8  slot_type;          /* MSB set means offset 40ms position 
                              * followed by 3 bits of BCN if revelant, 
                              * the remaining bits specify what kind of slot */
  int16  slot_num;           /* slot number */ 
  uint16 rf_band_chan;       /* current rf band class and cmda freq */
  int16  rf_agc;             /* rf agc some time after the wakeup slot in dbm */

  /* optional sub packets based on mask */
  uint8  sub_packets[SRCHLOG_SLEEP_STATS_MAX_SUB_PACKET_SIZE];

} srchlog_sleep_stats_type;

typedef PACKED struct
{
  uint16 reacq_pilot;          /* the pilot we reacquired on */
  int16  reacq_slew;           /* the reacq slew */
  uint16 reacq_time;           /* the time it took to reacq */
  uint8  reacq_ecio;           /* ecio of the reacq pilot */
  uint8  old_active_ecio;      /* ecio of the old active if a raho happened, else 0 */
  uint8  reacq_type;           /* should be of type srchzz_sched_reacq_type */
  uint8  num_nbrs_srched;      /* number of neighbors prio searched */ 
} srchzz_sched_reacq_stats_type;

typedef PACKED struct
{
  uint8  pad[3];
  uint8  num_nbrs_srched;      /* number of neighbors prio searched */ 
} srchzz_sched_prio_reacq_stats_type;

typedef PACKED struct
{
  uint32 sleep_duration;       /* sleep duration in chips */
  int16  rf_agc;               /* rf agc at wakeup in dbm */
  uint16 overall_setup_time;   /* time it took to get the adj timing cmd 
                                * (0.1ms resolution)*/
} srchzz_tl_stats_type;

/* Internal log packets NOT DEFINED */
#ifndef FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS
/*---------------------------------------------------------------------------
                        SAMPLE RAM LOG ENTRY TYPE
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_SRCH4_SAMPRAM_C)
  byte sramONTIMEIQ[50];
  byte sramLATEIQ[50];
LOG_RECORD_END
typedef LOG_SRCH4_SAMPRAM_C_type         log_srch4_sampram_type; 

/*---------------------------------------------------------------------------
                        SECTOR MODULE LOG ENTRY TYPE
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(SRCH_LOG_SECT_DUMP_C)
  uint8 sect_dump[4000];
LOG_RECORD_END
typedef SRCH_LOG_SECT_DUMP_C_type log_srch_sect_dump_type;
#endif /* FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS */

#ifdef T_WINCE
#error code not present
#endif /* T_WINCE */

#endif /* SRCHLOG_H */

