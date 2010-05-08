#ifndef SAMP_SERV_H
#define SAMP_SERV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S A M P L E _ S E R V E R ---  H E A D E R   F I L E 

GENERAL DESCRIPTION
  This module contains header information pertaining to sample_server.
  sample_server contains functions which control the sampler server hardware.

EXTERNALIZED FUNCTIONS
  sample_server_set_mode
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

$Header: //source/qcom/qct/modem/mcs/sample_server/main/latest/inc/sample_server.h#5 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
08/12/08   sst     Create macros to handle IF_V2 function changes
07/01/08   gs      Add SAMPLE_SERVER_V2_IF define to let clients know that
                   this version requires a "carrier" parameter.  Also migrate 
                   macros needed by FTM into this file (from the .c)
07/11/07   ljl     Supported multi-carriers.
03/26/07   gs      Remove dependencies on the 1x/srch codebase.
06/03/05   sfm     srch_rx naming changes
05/31/05   rng     Merged from SHDR Sandbox.
04/18/05   bt      Change arm_timed_capture from 2 chip units to 4.
02/11/04   bt      Added sample_server_arm_timed_capture.
02/11/04   ejv     Remove featurization around sample_server_set_chain_mode.
01/28/04   ejv     Mainline T_MSM6500.
12/16/03   aaj     Added msm_drv.h to allow write to SSERV registers
10/06/03   abh     Modified sample_server_set_mode for chain specific config.
07/30/03   ejv     Implementation, first cut.
===========================================================================*/ 

#include "customer.h"
#include "comdef.h"

/*========================================================================
     Interface Specification
========================================================================*/ 

/* This version of the sample server interface requires a "carrier" parameter.
 * All technologies requiring "legacy" operation should use 
 * SAMPLE_SERVER_CARRIER_0 below.  Also note that "under the hood", we
 * may or may not have support for carriers other than 0...this will 
 * be dealt with by the sample_server driver.
 *
 * Version 2:  add carrier parameter
 * Version 1:  original interface, from the legacy SRCH subsystem
 */
#define SAMPLE_SERVER_V2_IF

/*=========================================================================
      Constants
=========================================================================*/
#define BUFFER0                                             0
#define BUFFER1                                             1

#define ANTENNA0                                            0x0
#define ANTENNA1                                            0x1

/* Sample Modes */
#define STOP_SAMPLE_MODE                                    0x0
#define CONTINUOUS_SAMPLE_MODE                              0x2
#define WINDOW_SAMPLE_MODE                                  0x3

#define RTC_1X                                              0x0
#define RTC_HDR                                             0x1
#define RTC_GPS                                             0x2


/*=========================================================================
      Macros
=========================================================================*/

#define SS_INIT_i( base, n ) \
    HWIO_OUTI( base, n, 0x0 )

#define SS_SET_i( base, field, n ) \
    HWIO_OUTMI( base, n, HWIO_FMSK(base,field), HWIO_FMSK(base,field))

#define SS_CLEAR_i( base, field, n ) \
    HWIO_OUTMI( base, n, HWIO_FMSK(base,field), 0 )

#define SS_R_MASKED_i( base, field, n ) \
    ( HWIO_INMI( base, n, HWIO_FMSK(base,field)) >> \
                HWIO_SHFT(base,field) )

#define SS_W_MASKED_i( base, field, val, n )                          \
    { /*lint -save  -e701 Shift left of signed quantity intentional */ \
      HWIO_OUTMI( base, n, HWIO_FMSK(base,field),                \
                  val << HWIO_SHFT(base,field));                 \
    } /*lint -restore */

/* used for HDR Rev B only */
#define SAMPLE_RTC_SET_CARRIERS( buffer, carrier ) \
    SS_W_MASKED_i( CDMA_SAMPSRV_n_MODE, CARRIER_EN, (carrier), (buffer) )

#define SAMPLE_RTC_SET_ANT( buffer, ant ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_MODE, ANT_SEL, (ant), (buffer) )

#if ( defined( T_MSM7600 ) || defined( T_QSC60X5 ) )

  #define SAMPLE_SERVER_CTL_INIT( buffer ) \
      SS_INIT_i( CDMA_SAMPSRV_n_CTL, buffer )

  #define SAMPLE_START_IMMED( buffer ) \
      SS_SET_i( CDMA_SAMPSRV_n_CTL, IMMED_START, buffer )

  #define SAMPLE_STOP_IMMED( buffer ) \
      SS_CLEAR_i( CDMA_SAMPSRV_n_CTL, IMMED_START, buffer )

  #define SAMPLE_RTC_STOP_VAL( buffer, count ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_CTL, STOP_TIME, (count), buffer )

  #define SAMPLE_RTC_START_VAL( buffer, count ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_CTL, START_RTC, (count), buffer )

  #define SAMPLE_RTC_SET_SOURCE( buffer, src ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_MODE, SAMPLE_RXCH_SEL, (src), buffer )

  #define SAMPLE_LATEST_RTC_SET( buffer, value ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_LATEST_RTC, DATA, (value), buffer )

  #define SAMPLE_RTC_SET_MODE( buffer, mode ) \
      SS_W_MASKED_i( CDMA_SAMPSRV_n_MODE, SAMPLE_ENA, (mode), (uint32)(buffer) )
  #define SAMPLE_LATEST_RTC_COUNT( buffer ) \
      SS_R_MASKED_i( CDMA_SAMPSRV_n_LATEST_RTC, DATA, buffer )

#else
#error code not present
#endif /* T_MSM7600 || T_QSC60X5 */


/*=========================================================================
      Typedefs
=========================================================================*/
typedef enum
{
 SAMPLE_SERVER_RXTX_CHAIN,
 SAMPLE_SERVER_RX_CHAIN,
 SAMPLE_SERVER_RX_CHAIN_BOTH
} sample_server_rx_chain_type;

/* Definition for each carrier */
/* Note these only apply for chips that support HDR rev B */
typedef enum
{
 SAMPLE_SERVER_CARRIER_0 = 0x1,
 SAMPLE_SERVER_CARRIER_1 = 0x2,
 SAMPLE_SERVER_CARRIER_2 = 0x4
} sample_server_carrier_type;

/*=========================================================================
      Prototypes
=========================================================================*/

/*===========================================================================

FUNCTION       SAMPLE_SERVER_ARM_TIMED_CAPTURE

DESCRIPTION    This function configures the sample servers for timed capture
               on a given receive chain.

               Note that carriers only take effect if the chip supports 
               HDR Rev B.

DEPENDENCIES   Sample Server must be in window sampling mode.

RETURN VALUE   None

SIDE EFFECTS   Sample server is reconfigured

===========================================================================*/
extern void sample_server_arm_timed_capture
(
  sample_server_rx_chain_type chain_id,    /* Which RX Chain? */
  uint8  carriers,                         /* Carriers bit mask */
  uint32 rtc_start,                        /* RTC START in 4 chip units */
  uint32 rtc_stop                          /* RTC STOP in 4 chip units */
);
#define SAMPLE_SERVER_ARM_TIMED_CAPTURE( chain_id, carriers, rtc_start, rtc_stop ) \
          sample_server_arm_timed_capture( chain_id, carriers, rtc_start, rtc_stop )

/*===========================================================================

FUNCTION       SAMPLE_SERVER_SET_CHAIN_MODE

DESCRIPTION    This function configures the sample servers for 
               the specified chain to the mode specified. 

               Note that carriers only take effect if the chip supports 
               HDR Rev B.
               A carrier is enabled if its bit mask is set and is disabled 
               if its bit mask is reset in continuous mode or window sampling.

               Also, if the boolean variable sample_mode is set, 
               the sample server is configured in window capture mode.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Sample server is reconfigured

===========================================================================*/
extern void sample_server_set_chain_mode
(
  sample_server_rx_chain_type chain_id,    /* Which RX Chain? */
  uint8 carriers,    /* Carriers bit mask */
  uint8 mode,        /* specifies GPS or 1x mode */
  uint8 sample_mode  /* indicates continuous or window sampling */
);
#define SAMPLE_SERVER_SET_CHAIN_MODE( chain_id, carriers, mode, sample_mode ) \
          sample_server_set_chain_mode( chain_id, carriers, mode, sample_mode )
   
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
);

#endif /* SAMP_SERV_H */

