#ifndef CLK_REGIME_RM_H
#define CLK_REGIME_RM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C L O C K  R E S O U R C E  M A N A G E R  M O D U L E

                     
GENERAL DESCRIPTION
  This module defines the clock regime resource manager interface.

EXTERNALIZED FUNCTIONS
  clk_regime_resource_enable
  clk_regime_resource_disable
  clk_regime_resource_required
  clk_regime_resource_required_by_client

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_rm_init must be called first, then clients may request and
  release resources (in that order).


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/rm/clkrgm_rm.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/07   gfr     Split CDMA client into 1X and HDR.  Rename CDMA_RX
                   to CDMA_DEMOD.
08/13/07   sep     Support for changing TV clock source
05/15/07   gfr     Added ADSP and DMOV_APPS clients.
01/20/07   gfr     Added CDMA resources.
12/21/06   gfr     Added SMI resource, PMEM and BOOT_APPS clients.
11/16/06   gfr     Added VIDEO_CORE, VIDEO_FRONT_END and MDDI_CLIENT
                   resource and appropriate clients.
10/05/06   gfr     Added Media Flo client.
09/20/06   gfr     Added CDMA client.
08/08/06   gfr     Added clk_regime_rm_verbose function.
07/27/06   gfr     Added UIM client and UIM1/UIM2 resources
06/28/06   gfr     Added FTM client
05/09/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Feature definitions header             */


#ifdef FEATURE_CLKREGIM_RM


/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Clock regime clients */
typedef enum
{
   CLKRGM_CLIENT_SELF    = 0,

   CLKRGM_CLIENT_GSM,
   CLKRGM_CLIENT_WCDMA,
   CLKRGM_CLIENT_1X,
   CLKRGM_CLIENT_HDR,
   CLKRGM_CLIENT_GPS,
   CLKRGM_CLIENT_BT,
   CLKRGM_CLIENT_MDSP,
   CLKRGM_CLIENT_ADSP,
   CLKRGM_CLIENT_RF,
   CLKRGM_CLIENT_DMOV,
   CLKRGM_CLIENT_DMOV_APPS,
   CLKRGM_CLIENT_GLLC,
   CLKRGM_CLIENT_FTM,
   CLKRGM_CLIENT_UIM,
   CLKRGM_CLIENT_MFLO,
   CLKRGM_CLIENT_DTV,
   CLKRGM_CLIENT_JPEGD,
   CLKRGM_CLIENT_JPEGE,
   CLKRGM_CLIENT_QCAMCORDER,
   CLKRGM_CLIENT_QCAMERA,
   CLKRGM_CLIENT_QTV,
   CLKRGM_CLIENT_QVP,
   CLKRGM_CLIENT_PMEM,
   CLKRGM_CLIENT_BOOT_APPS,
   CLKRGM_CLIENT_TSIF,

   CLKRGM_NUM_OF_CLIENTS
} clk_regime_client_type;


/* Logical clock regime resources */
typedef enum
{
   /* Common resources */
   CLKRGM_RESOURCE_MDM,                /* Modem data mover */
   CLKRGM_RESOURCE_ADM,                /* Apps data mover */
   CLKRGM_RESOURCE_TCXO_PDM,           /* Required for access to the TCXO PDM */
   CLKRGM_RESOURCE_SLEEP_FEE,          /* Sleep frequency error estimator */
   CLKRGM_RESOURCE_SLEEP_CTL,          /* Sleep controller */
   CLKRGM_RESOURCE_MDSP,               /* Modem DSP */
   CLKRGM_RESOURCE_AGC,                /* AGC hardware block */
   CLKRGM_RESOURCE_RXF0,               /* RX front-end chain 0 */
   CLKRGM_RESOURCE_RXF1,               /* RX front-end chain 1 */
   CLKRGM_RESOURCE_SMI,                /* Stacked memory interface */

   /* GSM specific resources */
   CLKRGM_RESOURCE_GSM_STMR,           /* GSM system timer */
   CLKRGM_RESOURCE_GSM_RX,             /* GSM receiver */
   CLKRGM_RESOURCE_GSM_TX,             /* GSM transmitter */
   CLKRGM_RESOURCE_GSM_EDGE_RX,        /* GSM EDGE receiver */
   CLKRGM_RESOURCE_GSM_EDGE_TX,        /* GSM EDGE transmitter */
   CLKRGM_RESOURCE_GSM_ENCRYPT,        /* GSM encryption block */

   /* WCDMA specific resources */
   CLKRGM_RESOURCE_WCDMA_STMR,         /* WCDMA system timer */
   CLKRGM_RESOURCE_WCDMA_SRCH,         /* WCDMA searcher */
   CLKRGM_RESOURCE_WCDMA_DEMOD,        /* WCDMA demodulator */
   CLKRGM_RESOURCE_WCDMA_TX,           /* WCDMA transmitter */

   /* CDMA resources */
   CLKRGM_RESOURCE_CDMA_DEC,           /* CDMA decoder */
   CLKRGM_RESOURCE_CDMA_DEMOD,         /* CDMA demodulator */
   CLKRGM_RESOURCE_CDMA_TD,            /* CDMA turbo decoder */
   CLKRGM_RESOURCE_CDMA_VD,            /* CDMA viterbi decoder */
   CLKRGM_RESOURCE_CDMA_TX,            /* CDMA transmitter */

   /* GPS resources */
   CLKRGM_RESOURCE_GPS,                /* Core GPS resource */

   /* BT resources */
   CLKRGM_RESOURCE_BT,                 /* Core bluetooth resource */

   /* UIM resources */
   CLKRGM_RESOURCE_UIM1,               /* First UIM port */
   CLKRGM_RESOURCE_UIM2,               /* Second UIM port */

   /* Multimedia resources */
   CLKRGM_RESOURCE_VIDEO_CORE,         /* Video core */
   CLKRGM_RESOURCE_VIDEO_FRONT_END,    /* Video front end */
   CLKRGM_RESOURCE_MDDI_CLIENT,        /* MDDI client */

   /*TV/TSIF clock resources*/
   CLKRGM_RESOURCE_TV,
   CLKRGM_RESOURCE_TSIF,

   CLKRGM_NUM_OF_RESOURCES
} clk_regime_resource_type;



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_REQUIRED

  DESCRIPTION   Returns whether or not the given resource is currently
                required by any clients.

  PARAMETERS    resource - which resource to check

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the resource is required

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_resource_required
(
   clk_regime_resource_type resource
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_REQUIRED_BY_CLIENT

  DESCRIPTION   Returns whether or not the given resource is currently
                required by the given client.

  PARAMETERS    client   - which client to check
                resource - which resource to check

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the resource is required by this client

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_resource_required_by_client
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_ENABLE

  DESCRIPTION   Called when a client requires the use of a particular
                clock regime resource.  Does not support nesting.

  PARAMETERS    client   - which client is making the request
                resource - the resource being enabled

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_resource_enable
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_DISABLE

  DESCRIPTION   Called when a client no longer requires the use of a
                particular clock regime resource.

  PARAMETERS    client   - which client is making the request
                resource - the resource being disabled

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_resource_disable
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_VERBOSE

  DESCRIPTION   This function sets the resource manager to verbose mode.

  PARAMETERS    enable - whether or not to be verbose.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_verbose (boolean enable);



#endif /* FEATURE_CLKREGIM_RM */

#endif /* CLK_REGIME_RM_H */
