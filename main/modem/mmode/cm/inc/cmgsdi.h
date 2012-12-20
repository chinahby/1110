#ifndef CMGSDI_H
#define CMGSDI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   GSDI   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager GSDI Interface block, which groups
  all the functionality that is a associated with CM<->GSDI interfacing.




EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmnv_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmgsdi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/20/08   rm      RAT balancing req - Accept UIM_USIM_RAT Refresh request
                   during CS/PS call. 
11/17/08   am      Adding GSDI support also for targets other than GW.
12/12/07   rm      Adding RAT-balancing feature support
07/18/06   ka/ic   Adding check for CM specific SIM files before voting for
                   file notify.
04/13/06   ka      Bringing global fields to header file.
01/06/06   ic      Lint cleanup  
01/17/05   dk      Added support to read unpacked User PLMN Lists.
07/12/04   ic      Added support for FEATURE_ALS
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
02/26/04   jar     MSM6500 Compile Fixes
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/10/04   ws      Added cm_gsdi wrapper cm_gsdi_in_traffic_channel_w_mode
02/06/04   ws      Initial jaguar/mainline merge.
05/01/03   jar     Added support for REFRESH File Change Notifications
04/11/03   prk     Removed references to FEATURE_MMGSDI.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for accessing User Preferred PLMN list.
10/30/02   CS      Initial release.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#if !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
#include "comdef.h"
#include "sys.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "uim.h"
#endif /* FEATURE_MMGSDI */


#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

#ifdef FEATURE_MMGSDI


/*==============================================================================

FUNCTION NAME CM_GSDI_READ

DESCRIPTION
  Function that reads a specified Elementary File (EF) from the SIM.

RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
==============================================================================*/
boolean cm_gsdi_read
(
  uim_items_enum_type sim_filename,
  gsdi_index_modes_T  index_mode,
  int                 rec_num,
  byte*               data_buffer_p,
  int32               data_length
);



/*==============================================================================

FUNCTION NAME CM_GSDI_WRITE

DESCRIPTION

  Function that writes a specified Elementary File (EF) to the SIM.

RETURN VALUE

  boolean  Flag indicating whether the specified EF was written to the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
==============================================================================*/
extern boolean cm_gsdi_write
(
  uim_items_enum_type sim_filename,
  gsdi_index_modes_T  index_mode,
  int                 rec_num,
  byte*               data_buffer_p,
  int32               data_length
);



/*===========================================================================
FUNCTION CM_GSDI_IN_TRAFFIC

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the CM task.
===========================================================================*/
void cm_gsdi_in_traffic(boolean in_traffic);


/*===========================================================================
FUNCTION CM_GSDI_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.
  mode is a indicates whether the call is GSM / UMTS or 1X.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the CM task.
===========================================================================*/
void cm_gsdi_in_traffic_channel_w_mode(
  boolean in_traffic,
  sys_sys_mode_e_type mode);

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

#if ((defined FEATURE_WCDMA) || (defined FEATURE_GSM))
#error code not present
#endif /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */


#endif /* FEATURE_MMGSDI */

#if ((defined FEATURE_WCDMA) || (defined FEATURE_GSM))
#error code not present
#endif /* ((defined FEATURE_WCDMA) || (defined FEATURE_GSM)) */
#endif /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */

#endif /* CMGSDI_H */

