/*===========================================================================

                            P R O F   D I A G
                      
DESCRIPTION


GENERAL DESCRIPTION

  Definitions of DIAG subsystem packets for profiling related activities.
  
EXTERNALIZED FUNCTIONS

  profdiag_init() - Registers the profiling diag command handlers with
                    Diag.

REGIONAL FUNCTIONS

  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Must call profdiag_init() before commands will be processed by profdiag.
  

  Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
  
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc6055/services/profdiag/profdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/05   tmz     Changed "trigger" mechanism from pulse to extended period
01/07/05   jwh     include "profdiagi.h"
12/07/04   zz      Added QGL DSP profiling and using FEATURE_QGL_STATISTICS
12/02/04   jwh     FEATURE-ized AHB statistics.
11/20/04   grl     Created

===========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "customer.h"
#include "profdiagi.h"
#include "profdiag.h"

#include "rexdiag.h"
#include "msmhwio.h"
#include "timer.h"
#include "diagpkt.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "msg.h"

#include "apm.h"

#include "bio.h"
#ifdef FEATURE_QGL_STATISTICS
#error code not present
#endif /*FEATURE_QGL_STATISTICS*/


/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */

/* EJECT */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */


/* ------------------------------------------------------------------------
** Generic Profiling Diag Functions
** ------------------------------------------------------------------------ */


/*===========================================================================

FUNCTION PROFDIAG_START

DESCRIPTION
  This procedure processes a start gathering statistics request and lets
  the caller know which stats are actually supported.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * profdiag_start (
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
)
{
  DIAG_SUBSYS_PROF_START_F_rsp_type *rsp_ptr;

  DIAG_SUBSYS_PROF_START_F_req_type *req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req_ptr = ( DIAG_SUBSYS_PROF_START_F_req_type * ) in_req_ptr;
    /* Convert to typesafe version. */

  /* Verify that the length passed in and the action specified are
  ** valid.
  */
  if ( pkt_len != sizeof( DIAG_SUBSYS_PROF_START_F_req_type ) )
  {
    return diagpkt_err_rsp(
             DIAG_BAD_LEN_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_START_F_req_type ) );
  }

  rsp_ptr = ( DIAG_SUBSYS_PROF_START_F_rsp_type * )
    diagpkt_alloc( diagpkt_get_cmd_code( req_ptr ),
                   sizeof( DIAG_SUBSYS_PROF_START_F_rsp_type ) );
    /* Allocate the response packet. */

  memcpy( ( void* ) rsp_ptr,
          ( void* ) req_ptr,
          sizeof( diagpkt_subsys_header_type ) );
    /* The response packet looks a lot like the request packet. */

  rsp_ptr->prof_mask = 0;
    /* Tell the terminal which profiling items are actually supported.
       Start off assuming we support nothing. */

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */

#ifdef FEATURE_QGL_STATISTICS
#error code not present
#endif /* FEATURE_QGL_STATISTICS */

  return rsp_ptr;
} /* profdiag_start */


/*===========================================================================

FUNCTION PROFDIAG_STOP

DESCRIPTION
  This procedure processes a stop gathering statistics request and lets
  the caller know which stats are actually supported.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * profdiag_stop (
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
)
{
  DIAG_SUBSYS_PROF_STOP_F_rsp_type *rsp_ptr;

  DIAG_SUBSYS_PROF_STOP_F_req_type *req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req_ptr = ( DIAG_SUBSYS_PROF_STOP_F_req_type * ) in_req_ptr;
    /* Convert to typesafe version. */

  /* Verify that the length passed in and the action specified are
  ** valid.
  */
  if ( pkt_len != sizeof( DIAG_SUBSYS_PROF_STOP_F_req_type ) )
  {
    return diagpkt_err_rsp(
             DIAG_BAD_LEN_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_STOP_F_req_type ) );
  }

  rsp_ptr = ( DIAG_SUBSYS_PROF_STOP_F_rsp_type * )
    diagpkt_alloc( diagpkt_get_cmd_code( req_ptr ),
                   sizeof( DIAG_SUBSYS_PROF_STOP_F_rsp_type ) );
    /* Allocate the response packet. */

  memcpy( ( void* ) rsp_ptr,
          ( void* ) req_ptr,
          sizeof( diagpkt_subsys_header_type ) );
    /* The response packet looks a lot like the request packet. */

  rsp_ptr->prof_mask = 0;
    /* Tell the terminal which profiling items are actually supported.
       Stop off assuming we support nothing. */

#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */

#ifdef FEATURE_QGL_STATISTICS
#error code not present
#endif /* FEATURE_QGL_STATISTICS */

  return rsp_ptr;
} /* profdiag_stop */


/*===========================================================================

FUNCTION PROFDIAG_GET_STATS

DESCRIPTION
  This procedure processes a get statistics request.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * profdiag_get_stats (
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
)
{
  DIAG_SUBSYS_PROF_GET_STATS_F_rsp_type *rsp_ptr;

  DIAG_SUBSYS_PROF_GET_STATS_F_req_type *req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req_ptr = ( DIAG_SUBSYS_PROF_GET_STATS_F_req_type * ) in_req_ptr;
    /* Convert to typesafe version. */

  /* Verify that the length passed in and the action specified are
  ** valid.
  */
  if ( pkt_len != sizeof( DIAG_SUBSYS_PROF_GET_STATS_F_req_type ) )
  {
    return diagpkt_err_rsp(
             DIAG_BAD_LEN_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_GET_STATS_F_req_type ) );
  }

  switch ( req_ptr->prof_stat_id )
  {
#ifdef FEATURE_AHB_TRANSACTION_STATISTICS
#error code not present
#endif /* FEATURE_AHB_TRANSACTION_STATISTICS */

#ifdef FEATURE_QGL_STATISTICS
#error code not present
#endif /* FEATURE_QGL_STATISTICS */

    default:

      rsp_ptr = ( DIAG_SUBSYS_PROF_GET_STATS_F_rsp_type * )
        diagpkt_alloc( diagpkt_get_cmd_code( req_ptr ),
                       PROFDIAG_GET_STAT_HEADER_LEN );
        /* Allocate the generic response packet. */

      break;
  }

  memcpy ( ( void * ) &( rsp_ptr->xx_header ), 
           ( void * ) &( req_ptr->xx_header ), 
           sizeof( diagpkt_subsys_header_type ) );

  rsp_ptr->prof_stat_id = req_ptr->prof_stat_id ;

  return rsp_ptr;
} /* profdiag_get_stats */

static const diagpkt_user_table_entry_type profdiag_tbl[] =
{
  { PROFDIAG_START_F, PROFDIAG_START_F , profdiag_start },
  { PROFDIAG_STOP_F, PROFDIAG_STOP_F , profdiag_stop },
  { PROFDIAG_GET_STATS_F, PROFDIAG_GET_STATS_F , profdiag_get_stats },
  { PROFDIAG_GET_CFG_TBL_ADDR_F, PROFDIAG_GET_CFG_TBL_ADDR_F, apm_get_cfg_tbl_addr},
  { PROFDIAG_START_LOG_F, PROFDIAG_START_LOG_F , apm_start_log}
};


/*===========================================================================

FUNCTION PROFDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_PROF, profdiag_tbl);

#else

  void profdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_PROF, profdiag_tbl);
    apm_init();
  }

#endif

