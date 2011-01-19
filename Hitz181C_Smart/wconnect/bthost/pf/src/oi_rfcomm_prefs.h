#ifndef _OI_RFCOMM_PREFS_H
#define _OI_RFCOMM_PREFS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_rfcomm_prefs.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 *
 * This file defines RFCOMM preferences.
 *
 The following are the RFCOMM channel number preferences for the various
    profiles which register RFCOMM servers. If the numbers are selected 
    appropriately, a given service using RFCOMM will always be assigned the 
    same RFCOMM channel number.
    
    @note: Numbers are restricted to range of 1-30.    
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define RFCOMM_PREF_OPP_SRV          4    
#define RFCOMM_PREF_FAX              5
#define RFCOMM_PREF_DUN              6    
#define RFCOMM_PREF_HANDSFREE        7
#define RFCOMM_PREF_HANDSFREE_AG     8
#define RFCOMM_PREF_HEADSET          9    
#define RFCOMM_PREF_HEADSET_AG      10    
#define RFCOMM_PREF_SAP_SRV         11    
#define RFCOMM_PREF_FTP_SRV         12    
#define RFCOMM_PREF_SPP             20    

/*****************************************************************************/
#endif /* _OI_RFCOMM_PREFS_H */
