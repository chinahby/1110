#ifndef TRMCFG_H
#define TRMCFG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            T R A N S C E I V E R   R E S O U R C E   M A N A G E R

               Transceiver Resource Manager Configuration Header

GENERAL DESCRIPTION

  This file supports configuration Transceiver Resource Manager


EXTERNALIZED FUNCTIONS

  None
  

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


Copyright (c) 2005 - 2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/trm/main/latest/src/trmcfg.h#3 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
05/22/2008   sg      Integrated changes from old depot
05/02/2006   cab     Added ifdef's for offline compilation and testing
11/21/2005   cab     Moved NV item id's to compile time assign outside library
08/29/2005   grl     Added support for dynamic compatibility masks.
07/11/2005   grl     Added TRM config mask field to set_compatible_clients.
06/21/2005   ajn     Linting changes.
06/14/2005   ajn     Removed secondary client association.
03/28/2005   ajn     Added client/reason compatibility
03/02/2005   ajn     Initial AMSS version

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"

#ifdef FEATURE_MCS_TRM

extern "C"
{
  #include "trm.h"
  #ifdef TRM_OFFLINE
    #include "trm_offline.h"
  #else
    #include "nv.h"
  #endif
};            




/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/* Priorities are on the range 1..255 */
typedef uint8                     trm_pri_t;

/* Invalid client/reason pairs are indicated by a priority of BAD */
#define BAD                       0


/* Client/reason compatibility mask */
typedef uint16                    trm_compatible_mask_t;



/*-----------------------------------------------------------------------------
  TRM Constants & Configuration Class
-----------------------------------------------------------------------------*/

class TRM
{
public:

  struct Memory
  {
    /* Memory buffer, for TRM usage */
    uint8 *                       buffer;

    /* Size of TRM's memory buffer */
    uint32                        size;
  };

public:

  /* Number of clients in trm_client_enum_t */
  static const uint32             num_clients;

  /* TRM configuration nv item id */
  static const nv_items_enum_type nv_trm_config;

  /* GPS RF configuration nv item id */
  static const nv_items_enum_type nv_gps_rf_config;

  /* Configurable-Size Storage for TRM library */
  static Memory                   memory;

private:
  /* Cannot create a TRM object */
                                  TRM();

public:

  /* Client/Reason to priority map */
  static trm_pri_t                pri(trm_client_enum_t c, trm_reason_enum_t r);

  /* Client/Reason to compatibility mode map */
  static trm_compatible_mask_t    get_compatibility_mode(
                                    trm_client_enum_t     client,
                                    trm_reason_enum_t     reason);

  /* Get compatibility mask for the client/reason pair */
  static trm_compatible_mask_t    get_compatibility(
                                    trm_client_enum_t     client,
                                    trm_reason_enum_t     reason);

  /* Client to log code map */
  static uint8                    client_log_id(trm_client_enum_t c);

  /* Storage */
  static void *                   get_storage(uint32 size);

  /* Identify which clients can run simultaneously with other clients */
  static void                     set_compatible_clients(
                                    boolean               independent,
                                    uint32                mask);

  /* Enable the specified mode */
  static void                     enable_mode(trm_mode_enum_t mode);

  /* Disable the specified mode */
  static void                     disable_mode(trm_mode_enum_t mode);

};

#endif /* FEATURE_MCS_TRM */

#endif /* !TRMCFG_H */
