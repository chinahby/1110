#ifndef DS707_EXTIF_H
#define DS707_EXTIF_H
/*===========================================================================
                         D S 7 0 7 _ E X T I F
GENERAL DESCRIPTION
  This file is the external interface exported by ds707 to the other layers
  in the DMSS code than need to query ds707 for its state or other 
  information. This file is intended to provide an interface to other layers 
  in the DMSS code and is not intended to be used as an interface by
  entities outside of DMSS.

  This header file also exposes DS interface to MC module.
  
  This is specific to packet data calls. 

EXTERNALIZED FUNCTIONS
  DS_EXTIF_VERIFY_SO()
    Wrapper for DS_VERIFY_SO. Asks DS if the originating SO is correct.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

EXTERNALIZED MACROS
  DS_ISDATA_RS1_PRE707()
    Is the given Service option rateset 1 and considered part of the pre-707
    service option set?
    
  DS_ISDATA_RS1_IS707()
    Is the given Service option rateset 1 and considered part of the 707
    service option set?
    
  DS_ISDATA_RS2_PRE707()
    Is the given Service option rateset 2 and considered part of the pre-707
    service option set?
    
  DS_ISDATA_RS2_QC_PROPTRY()
    Is the given Service option rateset 2 and considered part of the Qualcomm
    proprietary service option set?
    
  DS_ISDATA_RS1()
    Is the given Service option rateset 1?
    
  DS_ISDATA_RS2()
    Is the given Service option rateset 2?
    
  DS_ISDATA()
    Is the given Service option a data service option?
  
  DS_ISDATA_RLP_TYPE2()
    Is the service option an MDR service option?
    
  DS_ISDATA_RLP_TYPE3()
    Is the service option a 1x service option (SO 33)?
    
EXTERNALIZED MACROS FOR CASE STATEMENTS
  DS_CASE_DATA_RS1_PRE707
    Case statements for rateset 1 service options which are in the pre-707
    service option set.
    
  DS_CASE_DATA_RS1_IS707
    Case statements for rateset 1 service options which are in the 707
    service option set.
    
  DS_CASE_DATA_RS2_PRE707
    Case statements for rateset 2 service options which are in the pre-707
    service option set.
    
  DS_CASE_DATA_RS2_QC_PROPTRY
    Case statements for rateset 2 service options which are in the
    proprietary service option set.

  DS_CASE_DATA_RS1
    Case statements for rateset 1 service options
  
  DS_CASE_DATA_RS2
    Case statements for rateset 1 service options
    
  DS_CASE_DATA
    Case statements for all data service options

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

 Copyright (c) 2003-2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_extif.h#1 $ $DateTime: 2008/04/11 08:25:15 $ $Author: nsivakum $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
10/24/07   sn         Removed external functions that are not needed for ULC. 
                      ULC requires only ds707_extif_verify_so().
12/05/05   as/kvd     Moved all external macros to ds707_defs.h
09/10/05   gr         Merged changes to lock out GPS in the middle of a PPP 
                      resync during a handup from 1x to DO
04/12/05   vrk        Moved MC specific definitions from ds707.h to ds707_extif.h
04/12/05   vrk        Added a wrapper function ds707_extif_verify_so
01/27/04   vr         Added function prototypes for
                      ds707_extif_was_last_data_network_cdma and
                      ds707_extif_was_last_data_network_hdr.
10/02/03   vr         Changed function names.
09/16/03   vr         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_defs.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_EXTIF_VERIFY_SO

DESCRIPTION   Wrapper for DS_VERIFY_SO. Called by MC.  
	      Asks DS if the originating SO is correct.

DEPENDENCIES  None

RETURN VALUE  service option to orig with

SIDE EFFECTS  None.
===========================================================================*/
word ds707_extif_verify_so
(
  word so_to_verify           /* SO to verify                              */
);

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_EXTIF_H      */
