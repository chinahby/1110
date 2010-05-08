#ifndef PROFDIAGI_H
#define PROFDIAGI_H

/*===========================================================================

           I N T E R N A L   P R O F   D I A G   H E A D E R 
                      
DESCRIPTION
  Internal profdiag header file 

GENERAL DESCRIPTION

  Definitions of DIAG subsystem packets for profiling related activities.
  
EXTERNALIZED FUNCTIONS
  None

REGIONAL FUNCTIONS

  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Must call profdiag_init() before commands will be processed by profdiag.
  

  Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
  
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc6055/services/profdiag/profdiagi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/06   cc      Added PROFDIAG_GET_CFG_TBL_ADDR_F and PROFDIAG_START_LOG_F
                   Commands.
01/07/05   jwh     Renamed as profdiagi.h and moved prototype to new profdiag.h
12/07/04   zz      Added QGL DSP profiling and using FEATURE_QGL_STATISTICS
12/02/04   jwh     FEATURE-ized AHB statistics.
11/20/04   grl     Created

===========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "diagpkt.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

/* PROF Subsystem Command IDs */
#define PROFDIAG_START_F      0x0000
#define PROFDIAG_STOP_F       0x0001
#define PROFDIAG_GET_STATS_F  0x0002
#define PROFDIAG_GET_CFG_TBL_ADDR_F 0x0003
#define PROFDIAG_START_LOG_F  0x0004

/* IDs of the profiling statistics one may collect. */
enum {

  PROFDIAG_AHB_STATS = 0x0,

  PROFDIAG_QGL_STATS = 0x1,

}; /* profdiag_stat_id_type */
typedef uint8 profdiag_stat_id_type;

/* Bits in the profiling bitmask (prof_mask) to set so that a given 
   statistic is collected.                                             */

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif 

#define PROFDIAG_QGL_STATS_BIT 0x2
#define PROFDIAG_GET_STAT_HEADER_LEN ( sizeof( profdiag_stat_id_type ) + \
                                       sizeof( diagpkt_subsys_header_type ) )
  /* Get stat response header length */

#ifdef FEATURE_UXMC_TRANSACTION_STATISTICS
#error code not present
#endif

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

/* Statistics packet definition for AHB statistics */
typedef PACKED struct {

  uint64 count[4];       
    /* Number of AHB clocks we are interested in that were counted. */

  uint32 overflow[4];
    /* Number of times we were too late in collecting statistics which 
       caused us to drop some clocks. */

} profdiag_ahb_stat_type;

/* Statistics packet definition for QGL statistics */
typedef PACKED struct {

  uint64 total_idle_cycles;       
    /* Total number of idle cycles in QGL DSP. */

  uint64 total_cycles;
    /* Total number of execution cycles in QGL DSP. */

} profdiag_qgl_stat_type;

typedef uint64 profdiag_mask_type;

/* ------------------------------------------------------------------------
 *
 * Count/Overflow Key 
 *
 * 1 - Total number of AHB clocks
 * 2 - Total number of SYS bus idle cycles.
 * 3 - Total number of APP bus idle cycles.
 * 4 - Total number of DMA bus idle cycles.
 *  
 * ------------------------------------------------------------------------ */

/* Union of all statistics one may collect. */
typedef PACKED union {

  profdiag_ahb_stat_type ahb_stats;
    /* The AHB related profiling statistics that show bus utilization. */

  profdiag_qgl_stat_type qgl_stats;
    /* The QGL related profiling statistics that show idle and total DSP cycles. */

} profdiag_stat_type;

/* -----------------------------------------------------------------------
** Diag packet definitions
** ----------------------------------------------------------------------- */

/*============================================================================

PACKET   Start Profiling Packet

ID       DIAG_SUBSYS_PROF_START_F

PURPOSE  Sent from the DM to the DMSS requesting the PROF to start profiling
         the given statistics. 

RESPONSE The request header, plus the profiling masks with all profiling bits
         that were requested and supported 1s.
         
============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( PROF, START_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that are requested to be turned on. */

DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, START_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that were requested to be turned on and are 
       supported. */

DIAGPKT_SUBSYS_RSP_END

/*============================================================================

PACKET   Stop Profiling Packet

ID       DIAG_SUBSYS_PROF_START_F

PURPOSE  Sent from the DM to the DMSS requesting the PROF to stop profiling
         the given statistics. 

RESPONSE The request header, plus the profiling masks with all profiling bits
         that were requested and supported 1s.

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( PROF, STOP_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that are requested to be turned off. */

DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, STOP_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that were requested to be turned off and are 
       supported. */

DIAGPKT_SUBSYS_RSP_END

/*============================================================================

PACKET   Get Profiling Statistics

ID       DIAG_SUBSYS_PROF_GET_STATS_F

PURPOSE  Sent from the DM to the DMSS requesting the PROF to stop profiling
         the given statistics. 

RESPONSE The request header, plus the statistic ID and the statistics.
         This is a variable length packet.

============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( PROF, GET_STATS_F )

  profdiag_stat_id_type prof_stat_id;
    /* The statistic ID which is requested. */

DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, GET_STATS_F )
  
  profdiag_stat_id_type prof_stat_id;
    /* The statistic ID which was requested. */

  profdiag_stat_type    prof_stat;
    /* The statistic info which was requested. */

DIAGPKT_SUBSYS_RSP_END

/*============================================================================

PACKET   Get AHB profiling config table address

ID       DIAG_SUBSYS_PROF_GET_CFG_ADDR_F

PURPOSE  Sent from the DM to the DMSS requesting the PROF to start profiling 
         and logging statistics. 

RESPONSE The request header, plus the profiling masks with all profiling bits
         that were requested and supported 1s.
         
============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( PROF, GET_CFG_TBL_ADDR_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that are requested to be turned on. */


DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, GET_CFG_TBL_ADDR_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that were requested to be turned on and are 
       supported. */

  void *  cfg_addr;  /* start address of the configuration table */
  void *  stat_addr; /* start address of the status table */

DIAGPKT_SUBSYS_RSP_END


/*============================================================================

PACKET   Start AHB Profile Logging Packet

ID       DIAG_SUBSYS_PROF_START_LOG_F

PURPOSE  Sent from the DM to the DMSS requesting the PROF to start profiling 
         and logging statistics. 

RESPONSE The request header, plus the profiling masks with all profiling bits
         that were requested and supported 1s.
         
============================================================================*/

DIAGPKT_SUBSYS_REQ_DEFINE( PROF, START_LOG_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that are requested to be turned on. */

DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, START_LOG_F )

  profdiag_mask_type prof_mask;
    /* Which statistics that were requested to be turned on and are 
       supported. */

DIAGPKT_SUBSYS_RSP_END



/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module. (See also profdiag.h)

==========================================================================*/


#endif /* PROFDIAGI_H */

