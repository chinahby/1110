/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S A M P L E   S E R V E R  --  SAMPLE SERVER INTERFACE

GENERAL DESCRIPTION
  This module contains functions and data which control the sampler
  server hardware.
  
EXTERNALIZED FUNCTIONS
  sample_server_set_chain_mode
  sample_server_init
  sample_server_arm_timed_capture

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2003 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/sample_server/main/latest/src/sample_server.c#4 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/08   gs      Migrate Macros needed by FTM into .h file
07/17/07   ljl     Support multi-carriers.
04/24/07   sst     Update SAMPLE_RTC_SET_ANT to be used for QSC60x5
03/23/07   gs      Remove dependencies on the 1x/srch codebase.
12/07/06   trc     Use new SAMPLE_STOP_IMMED() macro for cross-target support
10/27/06   trc     MSM7600 support
06/03/05   sfm     srch_rx naming changes
05/31/05   rng     Merged from SHDR Sandbox.
05/20/05   ejv     Replace srch_pn.h with srch_hw.h.
04/18/05   bt      Change arm_timed_capture from 2 chip units to 4.
07/14/04   ejv     Use macros for QUASAR-specific register settings.
02/19/04   ejv     Remove FEATURE_QUASAR_RF.
02/11/04   bt      Added sample_server_arm_timed_capture.
02/17/04   ejv     Changed T_QUASAR to T_QUASAR_BB.
01/28/04   ejv     Mainline T_MSM6500.
10/06/03   abh     Modified sample_server_set_mode for chain specific config.
09/24/03   ejv     Remove unnecessary comments for 6500.
07/30/03   ejv     Implementation, first cut.
===========================================================================*/ 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "assert.h"
#include "msm.h"

#include "sample_server.h"

/*===========================================================================
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

#define NOTUSED(i) if(i){}

/*=========================================================================
      Constants
=========================================================================*/

/* This mask covers all 4 chip stop time values for window mode */
#define MAX_REC_LENGTH_MASK                                 (0x3FF)

#define  SHORT_CODE_LEN         32768L

/*=========================================================================
      Typedefs
=========================================================================*/

/*=========================================================================
      Variables
=========================================================================*/


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*=========================================================================
      Static  Functions
=========================================================================*/

/*=========================================================================
      Global  Functions
=========================================================================*/

/*===========================================================================

FUNCTION       SAMPLE_SERVER_ARM_TIMED_CAPTURE

DESCRIPTION    This function configures the sample servers for timed capture
               on a given receive chain.

               Note that carries only take effect on targets that support
               HDR rev B.

DEPENDENCIES   sample server needs to be in window sampling mode

RETURN VALUE   None

SIDE EFFECTS   Sample server is reconfigured

===========================================================================*/
void sample_server_arm_timed_capture
(
  sample_server_rx_chain_type chain_id,    /* Which RX Chain? */
  uint8  carriers,                         /* Carriers bit mask */
  uint32 rtc_start,                        /* RTC START in 4 chip units */
  uint32 rtc_stop                          /* RTC STOP in 4 chip units */
)
{ 
  
  /*-----------------------------------------------------------------------*/

  /* make sure that we are withing a short code length in 4 chip units */
  ASSERT(rtc_start < (SHORT_CODE_LEN >> 2));
  /* make sure that we are withing a short code length in 4 chip units */
  ASSERT(rtc_stop  < (SHORT_CODE_LEN >> 2));

  #ifndef T_MDM7800
  NOTUSED(carriers);
  #endif /* T_MDM7800 */

  INTLOCK();

  /* substract one from the start to satisfy HW */
  rtc_start--;
  rtc_stop &= MAX_REC_LENGTH_MASK;

  /* convert to 2 chip units */
  rtc_start <<= 1;
  rtc_stop  <<= 1;

  #ifdef T_MSM6800
  /* Set the LSB to 1 to satisfy HW */
  rtc_start |= 0x1;
  rtc_stop  |= 0x1;
  #endif /* T_MSM6800 */

  /* reprogram the RTC start and stop positions */
  if ((chain_id == SAMPLE_SERVER_RXTX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH))
  {
    SAMPLE_RTC_START_VAL( BUFFER0, rtc_start);
    SAMPLE_RTC_STOP_VAL( BUFFER0, rtc_stop);
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER0, carriers );
    #endif /* T_MDM7800 */
  }

  if ((chain_id == SAMPLE_SERVER_RX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH))
  {
    SAMPLE_RTC_START_VAL( BUFFER1, rtc_start );
    SAMPLE_RTC_STOP_VAL( BUFFER1, rtc_stop );
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER1, carriers );
    #endif /* T_MDM7800 */
  }

  INTFREE();

} /* sample_server_arm_timed_capture */

/*===========================================================================

FUNCTION       SAMPLE_SERVER_SET_CHAIN_MODE

DESCRIPTION    This function configures the sample servers for 
               the specified chain to the mode specified. 
               Also, if the boolean variable sample_mode is set, 
               the sample server is configured in window capture mode.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Sample server is reconfigured

===========================================================================*/
void sample_server_set_chain_mode
(
  sample_server_rx_chain_type chain_id,    /* Which RX Chain? */
  uint8 carriers,    /* Carriers bit mask */
  uint8 mode,        /* specifies GPS or 1x mode */
  uint8 sample_mode  /* indicates continuous or window sampling */
)
{
  
  /*-----------------------------------------------------------------------*/

  #ifndef T_MDM7800
  NOTUSED(carriers);
  #endif /* T_MDM7800 */
      
  INTLOCK();

  if ((chain_id == SAMPLE_SERVER_RXTX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
     )
  {
    SAMPLE_RTC_SET_SOURCE( BUFFER0, mode );
    SAMPLE_RTC_SET_MODE( BUFFER0, sample_mode );
    #if ( defined( T_MSM7600 ) || defined( T_QSC60X5 ) )
    SAMPLE_RTC_SET_ANT( BUFFER0, ANTENNA0 );
    #endif /* T_MSM7600 || T_QSC60X5 */
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER0, carriers );
    #endif /* T_MDM7800 */  
  }
  if ((chain_id == SAMPLE_SERVER_RX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
     )
  {
    SAMPLE_RTC_SET_SOURCE( BUFFER1, mode );
    SAMPLE_RTC_SET_MODE( BUFFER1, sample_mode );
    #if ( defined( T_MSM7600 ) || defined( T_QSC60X5 ) )
    SAMPLE_RTC_SET_ANT( BUFFER1, ANTENNA1 );
    #endif /* T_MSM7600 || T_QSC60X5 */
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER1, carriers );
    #endif /* T_MDM7800 */
  }

  if ((sample_mode == WINDOW_SAMPLE_MODE) ||
      (sample_mode == STOP_SAMPLE_MODE)
     )
  {
    /* Set the Sample Ram in capture mode */
    if ((chain_id == SAMPLE_SERVER_RXTX_CHAIN) || 
        (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
       )
    {
      SAMPLE_STOP_IMMED( BUFFER0 );
    }

    if ((chain_id == SAMPLE_SERVER_RX_CHAIN) || 
        (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
       )
    {
      SAMPLE_STOP_IMMED( BUFFER1 );
    }
  }
  else /* CONTINUOUS Sample Mode. */
  {
    /* Set the Sample Ram in capture mode */
    if ((chain_id == SAMPLE_SERVER_RXTX_CHAIN) || 
        (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
       )
    {
      SAMPLE_START_IMMED( BUFFER0 );
    }

    if ((chain_id == SAMPLE_SERVER_RX_CHAIN) || 
        (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
       )
    {
      SAMPLE_START_IMMED( BUFFER1 );
    }
  }

  INTFREE();

} /* sample_server_set_chain_mode */


/*===========================================================================

FUNCTION       SAMPLE_SERVER_STOP

DESCRIPTION    This function stops the sample servers for 
               the specified chain.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Sample server is stopped

===========================================================================*/
void sample_server_stop
(
  sample_server_rx_chain_type chain_id    /* Which RX Chain? */
)
{
  
  /*-----------------------------------------------------------------------*/
      
  INTLOCK();

  if ((chain_id == SAMPLE_SERVER_RXTX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
     )
  {
    SAMPLE_RTC_SET_MODE( BUFFER0, STOP_SAMPLE_MODE );
    #ifdef T_MSM7600
#error code not present
    #endif /* T_MSM7600 */
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER0, 0 ); /* Disable all carriers */
    #endif /* T_MDM7800 */
    SAMPLE_STOP_IMMED( BUFFER0 );
  }
  if ((chain_id == SAMPLE_SERVER_RX_CHAIN) || 
      (chain_id == SAMPLE_SERVER_RX_CHAIN_BOTH)
     )
  {
    SAMPLE_RTC_SET_MODE( BUFFER1, STOP_SAMPLE_MODE );
    #ifdef T_MSM7600
#error code not present
    #endif /* T_MSM7600 */
    #ifdef T_MDM7800
    SAMPLE_RTC_SET_CARRIERS( BUFFER1, 0 ); /* Disable all carriers */
    #endif /* T_MDM7800 */
    SAMPLE_STOP_IMMED( BUFFER1 );
  }

  INTFREE();

} /* sample_server_stop */
