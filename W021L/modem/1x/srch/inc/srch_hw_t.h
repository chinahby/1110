#ifndef SRCH_HW_T_H
#define SRCH_HW_T_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    H A R D W A R E    T Y P E S
            
            H E A D E R F I L E 
            
GENERAL DESCRIPTION
   This file contains constants and type definitions that pertain to search 1
   searching as well as other hw related operations.

  Copyright (c) 2001 - 2006 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_hw_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   tjc     Modified traffic to use the sector module
08/21/06   aps/awj removed references to trunc parameter
08/21/06   va/rkc  Added antenna, changed max_inx to max_pos in the PPM Pilot
                   struct type.
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
05/12/06   awj     Updated sector structure.
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/16/06   awj     Lint cleanup
08/08/05   ejv     Remove FEATURE_SRCH_4_SHOULDERS.
07/28/05   ejv     Extend lecio support throughout SRCH task.
05/31/05   rng     Merged in from SHDR Sandbox.
05/19/05   ejv     Set SRCH_MIN_PEAKS to 6 for 6700+.
04/01/05   kwo     Added finger filter
04/01/05   sfm     Added last_threshold to srch_sect_struct_type
12/15/04   kwo     Rework of DH interface
12/08/04   jcm     Removed unused config member from srch_sect_struct_type
12/07/04   sst     Added win_pos_offset field to sect structure
12/02/04   bt      Add last_time_seen_energy to sector structure.
10/28/04   kwo     Added lecio_biased and last_ota_win_searched
09/09/04   ejv     Remove REL_A and REL_C_DV featurization.
09/04/04   jcm     Moved srch_hw types to srch_hw_t.h

==========================================================================*/
#include "clk.h"
#include "comdef.h"
#include "queue.h"
#include "srch.h"
#include "cai.h"
#include "caii.h"
#include "srch_rx_t.h"

#ifdef FEATURE_SEC
#ifndef FEATURE_SEC_CALLS_SEARCHER_RANDOM
#include "secutil.h"
#endif /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */
#endif /* FEATURE_SEC */

#define SRCH_MAX_PEAKS      6
  /* maximum number of peaks for all searchers */

#ifdef T_MSM6700
#define SRCH_MIN_PEAKS      6
#else
#define SRCH_MIN_PEAKS      4
#endif /* T_MSM6700 */
  /* minimum number of peaks for all searchers (used for inits) */

#define SRCH_NUM_PEAKS      4
  /* number of peaks for searcher 1, 2, & 3 */
  
#define  WIN_MAX_SIZ        (226 * 2)
  /* Biggest window size ever used, in half-chip units. Buffers allocated
     this big. */

#define STATUS_DUMP_WAIT_TIME (10 + CLK_MS_PER_TICK)
  /* Time allowed for status dump interrupt to occur after the last programmed
     search should have been finished on the searcher hardware */

#define SRCH_DYNAMIC_LECIO_THRESHOLD  0
  /* This value is used for sector->lecio.threshold if a dynamic lecio
     threshold is requested based on window size */

/* convert coherent and non_coherent values to sector struct equivalents */
#define COH2SECT(coh)            ((coh) >> 2)
#define NONCOH2SECT(non_coh)     ((non_coh) - 1)
#define GAIN2TRUNC(gain)         (5 - (gain))
  /* Convert coherent and non_coherent values to sector struct equivalents */ 

#define SECT2COH(coh)            ((coh) << 2)
#define SECT2NONCOH(non_coh)     ((non_coh) + 1)
#define SECT2PREGAIN(gain)       (2 << (gain))  
#define TRUNC2GAIN(trunc)        (5 - (trunc))
  /* Convert coherent and non_coherent values from sector struct equivalents */  


/*--------------------------------------------------------------------------
                          Sector State Codes

   Each sector is assigned to a Set as per IS-95.  This enum defines
   tag values for each set.
--------------------------------------------------------------------------*/

typedef enum
{
  SET_NONE,            /* Sector does not belong to any set */
  SET_A,               /* Sector belongs to Active Set */
  SET_C,               /* Sector belongs to Candidate Set */
  SET_PC_N,            /* Sector belongs to Neighbor Set, but is currently
                          considered a Pre-Candidate Set member */
  SET_N,               /* Sector belongs to Neighbor Set */
  SET_PC_R,            /* Sector belongs to the Remaining Set, but is
                          currently considered a Pre-Candidate Set member */
  SET_CF_N,            /* Sector belongs to the Candidate Freq set */
  SET_R                /* Sector belongs to the Remaining Set */
}
srch_set_enum_type;

/*--------------------------------------------------------------------------
                            TRPT Structure

  This structure is used in the sector structure and in temporary sector
  list structures to contain values used in reporting to the base station.
--------------------------------------------------------------------------*/
typedef struct
{
  int2  pos;
    /* POS_MATH position of this sector's best (usually earliest)
       path in 1/8 chip units. Numbers will be small deltas about
       POS_MATH_OFFSET. */

  int2  ear_pos;
    /* POS_MATH position of this sector's latest earliest path.
       In 1/8 chip units. Numbers will be small deltas about
       POS_MATH_OFFSET. */

  int2  nom_off;
    /* Initial offset of pilot from center in 1/8 chip units.*/

  byte  rev_dens;
    /* "Fast-Revisit" search density for speeding up looks at
        pilots apporaching T_ADD report threshold */

  byte  rev_indx;
    /* "Fast-Revisit" search index */

  dword  drop_timer;
    /* Handoff drop timer.  This value is set to the current system
       time in 26.66..ms units every time the pilot is observed to be
       above the t_drop threshold.  If this value plus the T_TDROP
       duration ever exceeds the current system time, the handoff
       drop timer will be considered expired. */

  int2  cnt;
    /* Number of entries in the following list */

  int2  pilots[ SRCH_ASET_MAX + 1 ];
    /* List of pilot offsets, used to keep track of what has been
       reported to the base station */

  byte  pilot_cnt[ SRCH_ASET_MAX ];
    /* T_COMP state machine, used to speed up reporting when
       there are many pilots in the Active/Candidate sets */
}
srch_trpt_struct_type;


/*--------------------------------------------------------------------------
                              Sector Structure

   The Sector is in many ways the basic element that the searcher task
   deals with.

   As used in the searcher task, a Sector is a signal or hoped-for signal
   at a particular pilot offset.  This may correspond to one sector of a
   sectored cell, or to an unsectored cell's single sector.

   The sector struct contains different types of information:

      * How to demodulate the sector
      * How to search for the sector's signal
      * The results of the most recent search of the sector
      * The combined results of all recent searches of the sector

   So, the sector struct can be passed around to various kinds of
   functions without also passing a lot of associated information.
   Which fields of the sector are meaningful at any given time varies.
--------------------------------------------------------------------------*/

typedef struct
{
  uint32 lecio;
    /* filtered linear ecio for the sector */

  uint32 len;
    /* number or samples in the filter (ie length)  */

  uint32 max_len;
    /* allow the filter to grow to this length */

  uint32 lecio_biased;
    /* the filtered energy biased based on neighbor info for HO decision */
}
srch_filter_struct_type;
  /* Long Term Average (Filtered) Linear Ecio from the searcher */

typedef struct
{
  /* The pilot offset (0-511) that defines this sector. */
  uint16                     pilot;

  /* The sweep window size to use when searching for this sector.
     Units are 1/8 chips to make slew math clear and simple. */
  int32                      win_size;

  /* The calculated position offset. Used to move the search window in
     searcher 4. Units are 1/8 chips. Useful for tracking moving peaks */
  int32                      win_pos_offset;

  /* Number of chips to integrate before converting to energy */
  uint16                     coh_int;

  /* Number of passes to accumulate non-coherently */
  /* Encoding: 0 = 1 pass, 1 = 2 passes, ... , 7 = 8 passes. */
  uint8                      non_coh_int;

  /* Pilot set membership info */
  uint32                     pilot_set_mask;

  struct
  {
    /* Transmit diversity info: power multiplier and mode */
    caii_add_pilot_rec_type    data;

    /* Type indicator for additional pilot record types */
    cai_pilot_rec_type         type;
  } pilot_rec;

} srch_sect_parm_type;

typedef struct
{

  /* Absolute position of all energy paths found in the
     window in 1/8 chip units. */
  int32                      max_pos[ SRCH_MAX_PEAKS ];

  /* Linear ECIO of the maximum energy path */
  uint32                     max_lecio[ SRCH_MAX_PEAKS ];

  struct
  {
    /* total linear ecio for the sector */
    uint32                   total;

    /* threshold for creating total lecio total or 0 to look up 
       threshold as a function of window size */
    uint32                   threshold;
  } lecio;

  /* Absolute position in units of 1/8 chip of
     the END of the search window. */
  int32                      win_pos;

} srch_sect_result_type;

typedef struct
{

  /* Searcher Parameters */
  srch_sect_parm_type        search_parms;

  /* Search Results */
  srch_sect_result_type      search_results;

  /* Band Class for this sector:  0 = 800mhz, 1 = PCS */
  srch_rx_band_type          band_class;

  /* CDMA frequency assignment for this sector */
  srch_rx_chan_type          cdma_freq;

  /* Priority of this particular sector, used in neighbor searches */
  uint8                      pri;

  /* Parameters for the FCH */
  srch_fch_info_type         fch_info;

  /* Parameters for the DCCH */
  srch_dcch_info_type        dcch_info;

  #ifdef FEATURE_IS95B_MDR
  /* Parameters for the SCCH */
  srch_scch_info_type        scch_info;
  #endif

  srch_sch_info_type2        sch_info;

  /* Parameters for CPCCH */
  srch_cpcch_info_type       cpcch_info;

  /* information related to PDCH that may change per pilot */
  srch_dv_pilot_info_type    dv_pilot_info;

  /* Neighbor Window Search Offset for the pilot */
  uint8                      srch_offset_nghbr;

  /* The sweep window size of neighbor set received form the over-
     the-air messages. Units are 1/8 chips to make slew math clear
     and simple. */
  int16                      ota_win_n;

  /* The "age" of the sector; used only in the Neighbor Set */
  uint8                      age;

  /* Which set the sector is currently assigned to */
  srch_set_enum_type         set;

  /* Pre-Candidate Set state variable */
  int16                      pc_state;

  /* Cell index.  This indicates which cell in the local cell
     table contains this sector.  The cell numbering may change
     whenever a Handoff Direction Messsage is received. */
  uint32                     cell;


  /* The current POS_MATH position of the center of the search window.
     Units are 1/8 chips. Numbers will be relatively small deltas
     about POS_MATH_OFFSET */
  int16                      pos;

  /* Reporting info including filtered energy */
  srch_trpt_struct_type      trpt;

  /* Long Term Average (Filtered) Linear Ecio from the searcher */
  srch_filter_struct_type    srch_filter;

  /* Long Term Average (Filtered) Linear Ecio from the finger */
  struct
  {
    /* filtered linear ecio for the sector */
    uint32                   lecio;

    /* number or samples in the filter (ie length)  */
    uint32                   len;

    /* allow the filter to grow to this length */
    uint32                   max_len;
  } fing_filter;

  /* TC state centering slew memory in 1/8 chips */
  int16                      center_slew;

  /* Centering slew hysteresis counter */
  int16                      center_cnt;

  /* Centering slew throttling flag */
  boolean                    center_throttle;

  /* Hard-handoff related earliest path offset from center of
     srch win. In 1/8 chip units. */
  int16                      hard_off;

  /* The time (ms) when this sector was last 
     searched regardless of the srch parms */
  uint32                     last_time_searched;

  /* The time (ms) when this sector was last 
     searched with OTA parameters */
  uint32                     last_ota_win_searched;

  /* The time (ms) when this sector was searched and it's strongest
     path was above the REACQ_WIN_NOISE_THRESH_LECIO threshold */
  uint32                     last_time_seen_energy;

  /* the last value used to determine which peaks are paths */
  uint32                     last_threshold;

  /* Remember what window size the latest search was done at */
  int32                      last_win_siz;

  /* Access Entry Handoff allowed flag */
  boolean                    access_entry_ho;

  /* Access Handoff allowed flag */
  boolean                    access_ho_allowed;

  /* Flag tells whether we have attempted to access this pilot.
     Note: SRCH doesn't use this flag, it's merely for MC coding
           convenience */
  boolean                    access_attempted;

  boolean                    bcch_supported;

  /* Flag tells whether this sector (when Candidate Freq Neighbor), it to be
     searched. */
  boolean                    cfs_srch_flag;

}
srch_sect_struct_type;

/*--------------------------------------------------------------------------
                   Searcher demod parameter type

  This structure is used throughout Search to carry around
  and load some of the common demodulation parameters for the searcher.
--------------------------------------------------------------------------*/

typedef struct
{
  int4  win_siz;
    /* Size of the search window in 1/8 chip units (Used mainly in ACQ) */

  byte  gain;
    /* Gain in the searcher hardware */

  word  chps;
    /* Number of chips to integrate coherently */

  byte  nc_int;
    /* Number of integrations to add non-coherently */

  word  thresh;
    /* Threshold to accept a window (used in ACQ only) */
}
srch_sect_parm_struct_type;




/*--------------------------------------------------------------------------
                Declaration of ISR structure variables
--------------------------------------------------------------------------*/

typedef struct
{
  word  phase80;
    /* 0,1,2 count of 26ms periods within an 80ms */

  dword  tick26;
    /* Overall count of 26.66.. ms roll periods */
}
srch_roll_isr_vars_struct_type;
  /* PN roll ISR variables */

/*------------------------------------------------------------------------*/

typedef struct
{
  int4 current_pos;
    /* Current "reported" searcher position */

  int4 raw_pos;
    /* Raw searcher position */

  int4 expected_pos;
    /* Expected searcher position */

  int2 pn_offset;
    /* The cumulative offset PN mask */

  int4 pos_offset;
    /* The cumulative offset position in chipx8 */

  int2 offset_nghbr_slew;
    /* The amount of slew required for neighbor search offset */
}srch_position_data_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: AFLT_PILOT_STRUCT_TYPE

This structure is used to maintain values applicable to a single pilot
channel. THIS IS A STRIPPED_DOWN VERSION FOR TESTING SEARCH BATCH LOGGING.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 pn;
    /* the pilot PN Offset */

  srch_set_enum_type set;
    /* the pilot set this pilot belongs to */

  int4 offset;
    /* The offset from the absolute searcher center position */

  int2 srch_win;
    /* the search window size */

  byte srch_offset;
    /* the index into the search window offset table */

  byte antenna;
    /* Antenna on which PPM searching took place */

  int2 srch_win_center_offset;
  /* this is the offset of the center of the search window for this pilot
     from {MSTR + (pilot index * 64)}.  For neighbors and remaining set
     pilots this will always be zero but for active and candidate set pilots
     it is the distance in chipX8 of the earliest arriving multipath of this
     pilot from the nominal window center, i.e. {MSTR + (pilot index * 64)} */

  uint2 num_peaks;
    /* number of peaks (based on which searcher was used) */

  uint32  max_pos[ SRCH_MAX_PEAKS ];
    /* Position of the sweep results of the maximum energy path */

  word  max_eng[ SRCH_MAX_PEAKS ];
    /* Energy of the maximum energy path */

  boolean list_end;
    /* marks the last pilot to be searched in the search list */
}
srch_ppm_pilot_struct_type;

typedef struct
{
  q_link_type  link;
    /* Link for putting this buffer on a queue. */

  srch_set_enum_type  set;
    /* Which set the sector is currently assigned to */

  boolean                age;
  int2                   pilot;
  byte                   nc_int;
  byte                   chps;
  byte                   gain;
  int4                   offset;
  int4                   winsiz;
  byte                   coh_tr;
    /* The above parameters are the standard parameters used to program
       a search.  they should be calculated ahead of time and stored in
       this structure, so that the searches can be quickly loaded into
       the queue */

  boolean                dump;
    /* An extra field for the 16x searcher which signals whether or not
       we want to receive a dump interrupt after this particular search */

  int4                   exp_pos;
    /* The expected position of the searcher after it has completed this
       search on the queue, assuming no interruptions to the searcher */

  srch_sect_struct_type  *sect_ptr;
    /* Pointer to the original sector structure which this search
       belongs to.  ie. if the parameters in this search belong to
       srch_aset[ 0 ], then *sect_ptr = srch_aset[ 0 ]  This is used
       to place the final results in the correct sector structure */

  srch_ppm_pilot_struct_type *ppm_ptr;
    /* Pointer to structure in PPM Pilot table, where the PPM results
       will be stored */

  struct
  {
    uint2  max_inx[ SRCH_MAX_PEAKS ];
      /* Index in the sweep results of the maximum energy path
         in 1/8 chip units. */

    word  max_eng[ SRCH_MAX_PEAKS ];
      /* Energy of the maximum energy path */
  }
  common_path;
    /* The raw results of common pilot searches go here first */

  struct
  {
    uint2  max_inx[ SRCH_MAX_PEAKS ];
      /* Index in the sweep results of the maximum energy path
         in 1/8 chip units. */

    word  max_eng[ SRCH_MAX_PEAKS ];
      /* Energy of the maximum energy path */
  }
  diversity_path;
    /* The raw results of diversity pilot searches go here first */
}
srch1_cmd_struct_type;


/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X                                                                  X
  X     Temporary placement of security and rf functions...          X
  X     hopefully to be moved soon to their own files/subsystems     X
  X                                                                  X */

#ifdef FEATURE_SEC
/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer fill
---------------------------------------------------------------------------*/

#define SRCH_SEC_TEMP_BUF_SIZE 64

#ifdef FEATURE_SEC_CALLS_SEARCHER_RANDOM
#error code not present
#else
typedef struct
{
  byte    fill_buf[ SRCH_SEC_TEMP_BUF_SIZE ];
    /* Data to merge into random number buffer */
  
  byte    *ptr;
    /* Pointer used to insert data into fill_buf */
}
srch_sec_type;
  /* Random number generator for SECurity features. */
#endif /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */

extern srch_sec_type srch_sec;

#define SRCH_SEC_RND_INIT( ) srch_sec.ptr = srch_sec.fill_buf;
  /* This macro initializes "ptr" for putting data in the "fill_buf" */

#ifdef FEATURE_SEC_CALLS_SEARCHER_RANDOM
#error code not present
#else /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */

#define SRCH_SEC_RND_DATA(x) \
  if(!secutil_rnd.done) { *srch_sec.ptr++ = (byte)(x); }
   
#define SRCH_SEC_RND_DONE( ) secutil_fill_rnd_data(srch_sec.fill_buf,\
                             srch_sec.ptr - srch_sec.fill_buf ); 
  /* This macro adds the fill buffer to the random number buffer */
#endif /* FEATURE_SEC_CALLS_SEARCHER_RANDOM */

#else /* !FEATURE_SEC */

#define SRCH_SEC_RND_INIT( )
#define SRCH_SEC_RND_DATA(x)
#define SRCH_SEC_RND_DONE( )
  /* No-Op */
#endif /* FEATURE_SEC */

#ifdef FEATURE_DH
/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer fill
---------------------------------------------------------------------------*/

extern byte srch_rand_buf[ ];
  /* This is the buffer where we will store the data */

#endif

/*X                                                                  X
  X      End of temporary stuff                                      X
  X                                                                  X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/       

#endif /* SRCH_HW_T_H */

