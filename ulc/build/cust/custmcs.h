#ifndef CUSTMCS_H
#define CUSTMCS_H
/*===========================================================================

DESCRIPTION
  Configuration for MCS.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2008 - 2009
                by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/build/main/latest/custmcs.h#14 $ $DateTime: 2009/02/19 09:13:33 $ $Author: awoodcox $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/09   adw     Enable tcxomgr diag interface except when reduced mem.
10/31/08   bb      Added additional check (!T_QSC6085) for SC2x mid tier builds.
10/31/08   bb      Added check for SC2x mid tier builds.
10/20/08   adw     Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG and
                   modified feature trigger to be based on no gps & no hdr.
10/10/08   adw     Stop cdma includes from being pulled into UMTS builds.
10/09/08   adw     Added check for SC2x low tier builds.
10/03/08   adw     Added support for varrying targets and technologies.
                   Pulled back in previously removed cust*.h files.
09/30/08   adw     Only define FEATURE_MCS_REDUCED_MEMORY for ULC targets.
09/26/08   mca     Removed custcdma2000.h, custhdr.h to be compatible with
                   UMTS only builds
09/16/08   adw     Corrected conditional check for including custhdr.h.
09/05/08   adw     Cleaned up featurization and reorganized each subsystem.
                   Modified includes to correctly determine GPS features.
09/02/08   adw     Added features to enable XO Cal for Rev1.1+ only.
08/28/08   adw     Initial revision.
===========================================================================*/

/* ------------------------------------------------------------------------
** Modem Common Services is used across a wide range of targets and supports
** a variety of different technologies. This includes varrying gps
** configurations, CDMA vs. UMTS, multi-antenna and single antenna, low
** memory configurations, etc. The following featurization groups targets into
** specific technology "buckets" and configures MCS to operate appropriately.
** This is an evolving configuration and will need updating as new targets and
** technologies begin to pull from the MCS code base. In order to support as
** many targets as possible, if the target specification is unknow, MCS will 
** set itself up with the best possible configuration. This configuration will
** include a default set of the most common features and services.
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Target Specific Configuration
** ------------------------------------------------------------------------ */

/* Group targets into technology buckets. Include the correct CDMA, GPS and HDR
   headers based on target type to figure out which target-specific features
   need to be defined later on. */

/* -------------------------------------------------------------
** ULC, CDMA200 1x only --> QSC11x0 
** ------------------------------------------------------------- */
#if ( defined( T_QSC1100 ) || defined( T_QSC1110 ) )
  /* This include is needed to figure out whether
     FEATURE_MULTI_RX_CHAIN is defined */
  #include "custcdma2000.h"

  /* No gps header file to include, target does not support GPS */

  /* No hdr header file to include, target does not support HDR */

/* -------------------------------------------------------------
** Aries and beyond gps, CDMA2000 --> SC2x, 76xx, 8650
** ------------------------------------------------------------- */
#elif ( defined( T_QSC60X5 ) ||                                       \
      ( defined( T_MSM8650 ) && defined( FEATURE_CDMA ) ) ||          \
      ( defined( T_MSM7600 ) && defined( FEATURE_CDMA ) ) )
  /* This include is needed to figure out whether
     FEATURE_MULTI_RX_CHAIN is defined */
  #include "custcdma2000.h"

  /* This include file for Aries and beyond (SC2x, 8650, 7600) */
  #include "custcgps.h"

  /* Check for SC2x low tier */
  #if ( defined( T_QSC6055 ) && !defined( T_QSC6065 ) )
    /* No hdr header file to include, SC2x Low tier does not support HDR */
  #elif ( defined( T_QSC6055 ) && 				     \
	  defined( T_QSC6065 ) && !defined( T_QSC6085 ))
    /* No header file to include, SC2X Mid tier does not support HDR */
  #else
    /* This include is needed to figure out whether
       FEATURE_HDR is defined */
    #include "custhdr.h"
  #endif /* FEATURE_HDR */

/* -------------------------------------------------------------
** gpsONE, CDMA2000 --> 6800, etc.
** ------------------------------------------------------------- */
#elif ( defined( T_MSM6800 ) && !defined( T_MSM8650 ) )
  /* This include is needed to figure out whether
     FEATURE_MULTI_RX_CHAIN is defined */
  #include "custcdma2000.h"

  /* This include file for gpsONE targets (MSM6800) */
  #include "custgps.h"

  /* This include is needed to figure out whether
     FEATURE_HDR is defined */
  #include "custhdr.h"

/* -------------------------------------------------------------
** UMTS, etc.
** ------------------------------------------------------------- */
#else
  /* Target specification unknown, setup with best possible configuration. */
  #define FEATURE_MCS_BEST_CONFIG

  /* No cdma2000 header file to include, target is UMTS */

  /* No gps header file to include, unknown gps support */

  /* No hdr header file to include, target is UMTS */
#endif


/* ------------------------------------------------------------------------
** General MCS Features
** ------------------------------------------------------------------------ */

#ifndef FEATURE_MCS_BEST_CONFIG
  /* Target specification is know, configure accordingly. */

  /* Configure MCS for single antenna operation */
  #ifdef FEATURE_MULTI_RX_CHAIN
    #undef FEATURE_MCS_SINGLE_ANTENNA
  #else /* !FEATURE_MULTI_RX_CHAIN */
    /* Configure MCS for single antenna operation */
    #define FEATURE_MCS_SINGLE_ANTENNA
  #endif /* FEATURE_MULTI_RX_CHAIN */

  /* Memory Reduction */
  #ifdef FEATURE_LOW_MEMORY_USAGE
    #if ( defined( T_QSC1100 ) || defined( T_QSC1110 ) )
      /* Configure MCS for reduced memory targets */
      #define FEATURE_MCS_REDUCED_MEMORY
    #endif /* ULC */
  #else /* !FEATURE_LOW_MEMORY_USAGE */
    #undef FEATURE_MCS_REDUCED_MEMORY 
  #endif /* FEATURE_LOW_MEMORY_USAGE */

  /* Determine whether or not GPS code is enabled */
  #if ( defined( FEATURE_GPSONE ) || defined( FEATURE_SRCH_CGPS_IF ) )
    /* GPS code is enabled */
    #undef FEATURE_MCS_HAS_NO_GPS
  #else
    /* GPS code is NOT enabled, configure MCS for targets without GPS */
    #define FEATURE_MCS_HAS_NO_GPS
  #endif

  /* Determine whether or not HDR code is enabled */
  #ifdef FEATURE_HDR
#error code not present
  #else /* !FEATURE_HDR */
    /* HDR is not supported */
    #define FEATURE_MCS_HAS_NO_HDR
  #endif /* FEATURE_HDR */

#else /* !FEATURE_MCS_BEST_CONFIG */
  /* Target specification is unknown, setup MCS with the best possible 
     configuration. */

  /* Configure for multi-antenna */
  #undef FEATURE_MCS_SINGLE_ANTENNA

  /* Configure for full memory */
  #undef FEATURE_MCS_REDUCED_MEMORY

  /* Configure for gps */
  #undef FEATURE_MCS_HAS_NO_GPS

  /* Configure for hdr */
  #undef FEATURE_MCS_HAS_NO_HDR

#endif /* FEATURE_MCS_BEST_CONFIG */


/* ------------------------------------------------------------------------
** Sample Server Features
** ------------------------------------------------------------------------ */

/* Add sample server related features here ... */


/* ------------------------------------------------------------------------
** Sleep Control Features
** ------------------------------------------------------------------------ */

/* Add sleep control related features here ... */


/* ------------------------------------------------------------------------
** Search 4 Features
** ------------------------------------------------------------------------ */

/* Configure Search 4 and HDR interaction */
#ifdef FEATURE_MCS_HAS_NO_HDR
  #undef SRCH4_HDR_ENABLED
#else /* !FEATURE_MCS_HAS_NO_HDR */
  #define SRCH4_HDR_ENABLED
#endif /* FEATURE_HDR */

/* Configure SRCH4 for multi antenna operation */
#ifndef FEATURE_MCS_SINGLE_ANTENNA

  /* Configure Search 4 and GPS interaction */
  #if (!defined( FEATURE_GPSONE ) && !defined( FEATURE_GPSONE_PPM ) )
    /* Enable Search 4 support of automatic forward link trilateration */
    #define SRCH4_AFLT_ENABLED
  #endif /* !FEATURE_GPSONE && !FEATURE_GPSONE_PPM */

  /* Set Search 4 AFLT */
  #ifndef SRCH4_AFLT_ENABLED
    /* Disable Search 4 Low Priority Queue */
    #undef  SRCH4_USE_LPQ
  #else /* !SRCH4_AFLT_ENABLED */
    /* Enable Search 4 Low Priority Queue */
    #define SRCH4_USE_LPQ
  #endif /* SRCH4_AFLT_ENABLED */

/* Configure SRCH4 for single antenna operation */
#else /* !FEATURE_MCS_SINGLE_ANTENNA */
  /* Disable Search 4 support of automatic forward link trilateration */
  #undef SRCH4_AFLT_ENABLED

  /* Disable Search 4 Low Priority Queue */
  #undef SRCH4_USE_LPQ

#endif /* FEATURE_MCS_SINGLE_ANTENNA */


/* Define Search 4 debug message mask */
#define MSG_DBG_SRCH4        MSG_MASK_20
  /* srch4 messages - originally in srch_diag.h, mask is reserved there */


/* ------------------------------------------------------------------------
** State Machine Features
** ------------------------------------------------------------------------ */

/* Add state machine related features here ... */


/* ------------------------------------------------------------------------
** TCXO Manager Features
** ------------------------------------------------------------------------ */

/* Turn on initialization and shutdown of the tcxomgr service within TMC */
#define FEATURE_TMC_TCXOMGR

/* Configure TCXOMGR for full memory targets */
#ifndef FEATURE_MCS_REDUCED_MEMORY

  #ifdef FEATURE_XO
    /* Enable XO Factory Calibration */
    #ifdef T_QSC60X5REV1_1 /* dragon-4 and dragon-5 */
      #define FEATURE_XO_TASK
      #define FEATURE_XO_FACTORY_CAL
      #define FEATURE_XO_FIELD_CAL
    #endif /* T_QSC60X5REV1_1 */

    #ifdef T_QSC60X5REV1_1 /* dragon-4 and dragon-5 */
      #define FEATURE_USE_XO_ADC_FOR_XO_THERM
    #elif defined(T_QSC60X5REV1_0) /*dragon-3 */
      #define FEATURE_USE_PATHERM_ADC_FOR_XO_THERM
    #endif /* T_QSC60X5REV1_1 */
  #endif /* FEATURE_XO */

  /* Enable tcxomgr diag interface */
  #define FEATURE_TCXOMGR_DIAG

/* Configure TCXOMGR for reduced memory targets */
#else /* !FEATURE_MCS_REDUCED_MEMORY */
  /* Remove features not supported on reduced memory targets */
  #undef FEATURE_XO_FIELD_CAL
  #undef FEATURE_XO_FACTORY_CAL
  #undef FEATURE_XO_CAL
  #undef FEATURE_XO_TASK

  /* Disable tcxomgr diag interface */
  #undef FEATURE_TCXOMGR_DIAG

#endif /* FEATURE_MCS_REDUCED_MEMORY */

/* Configure TCXOMGR for targets without GPS and HDR */
#if ( defined( FEATURE_MCS_HAS_NO_GPS ) && defined( FEATURE_MCS_HAS_NO_HDR ) )
  /* Remove un-used tcxomgr control on targets without GPS and HDR */
  #define FEATURE_XO_CTL_1X_ONLY

#else /* !FEATURE_MCS_HAS_NO_GPS || !FEATURE_MCS_HAS_NO_HDR */
  /* Do not remove tcxomgr control, target has GPS or HDR */
  #undef FEATURE_XO_CTL_1X_ONLY

#endif /* FEATURE_MCS_HAS_NO_GPS && FEATURE_MCS_HAS_NO_HDR */


/* ------------------------------------------------------------------------
** Transceiver Resource Manager Features
** ------------------------------------------------------------------------ */

/* Configure TRM for multi antenna operation */
#ifndef FEATURE_MCS_SINGLE_ANTENNA
  /* Turn on initialization of the TRM service within TMC */
  #define FEATURE_TMC_TRM

  /* Enable Transceiver Resource Manager */
  #define FEATURE_MCS_TRM

  /* Configure TRM for API Version 2 */
  #define FEATURE_TRM_API_V2

/* Configure TRM for single antenna operation */
#else /* !FEATURE_MCS_SINGLE_ANTENNA */
  /* Remove initialization of the TRM service within TMC */
  #undef FEATURE_TMC_TRM

  /* Remove TRM on single antenna targets */
  #undef FEATURE_MCS_TRM

#endif /* FEATURE_MCS_SINGLE_ANTENNA */


#endif /* CUSTMCS_H */

