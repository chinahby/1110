#ifndef MCCSYOBJ_H
#define MCCSYOBJ_H
/*===========================================================================

             P A C K E T   Z O N E   I D   S U P P O R T

GENERAL DESCRIPTION
  This module contains supporting functions for packet zone ID.

DESCRIPTION
  This module contains definitions and interfaces for service configuration
  and negotiation for Data Services service options.  This functions are
  created for support of MDR Release 1.0.  Note that once Call Manager is
  implemented, these function may become obsolete.

Copyright (c) 1999-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsyobj.h_v   1.2.1.0   30 Nov 2001 17:21:46   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccsyobj.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/08   bb      Code changes for IS707B addendum feature
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
06/23/06   pg      Added support for RPC and clean up.
03/31/05   fc      Added support for EPZID.
10/23/03   yll     Added support for updating HDR PANID.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
01/24/03   lh      Modified pzid change processing interface.
10/01/01   kk      Merge from common archive
                   09/14/01   lcc     Added reg_change_handler member in nid 
                   of mccsyobj_system_object_type.
02/27/01   kk      Added functions to set and get old system parameters where
                   the last packet data call is successfully originated.
11/13/00   lcc     Added members related to NID in mccsyobj_system_object_type.
11/03/99   lcc     Merged from GD4021:
           doj     Made reg_prev_change_handler, process_prev and get_prev
                   only defined for FEATURE_SPECIAL_MDR.
02/18/99   lcc     Corrected SID to use a word instead of byte.
01/25/99   lcc     Initial release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "caii.h"


/*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

#define MCCSYOBJ_PZID_UNKNOWN 0


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* Function to get current stored value of PZID */
/*===========================================================================

FUNCTION mccsyobj_get_pzid

DESCRIPTION
  This function gets current stored value of PZID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccsyobj_get_pzid( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_get_pzid */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Function to get current stored value of SID */
/*===========================================================================

FUNCTION mccsyobj_get_sid

DESCRIPTION
  This function gets current stored value of SID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_sid( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_get_sid */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Function to get current stored value of NID */
/*===========================================================================

FUNCTION mccsyobj_get_nid

DESCRIPTION
  This function gets current stored value of NID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_nid( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_get_nid */
#endif /* FEATURE_HTORPC_METACOMMENTS */

    /* Registers handler for changing EPZID */ 
/*===========================================================================

FUNCTION mccsyobj_reg_epzid_change_handler

DESCRIPTION
  This function registers handler for changing EPZID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*epzid_change_handler_type) (word sid, word nid, byte pzid);

void mccsyobj_reg_epzid_change_handler
(
  epzid_change_handler_type epzid_change_handler 
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_reg_epzid_change_handler */
#endif /* FEATURE_HTORPC_METACOMMENTS */

    /* Function to call when EPZID (i.e. SID, NID and PZID) are received */ 
/*===========================================================================

FUNCTION mccsyobj_process_epzid

DESCRIPTION
  Function to call when EPZID (i.e. SID, NID and PZID) are received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_process_epzid
( 
  word sid, 
  word nid, 
  byte pzid, 
  word curr_state 
); 



   /* Registers handler to know when mobile enters into the registration sub state */ 
/*===========================================================================

FUNCTION mccsyobj_epzid_reg_sub_state_handler 

DESCRIPTION
  This function registers the handler for knowing when mobile enters into the registration 
  sub state due to either "timer based" or "parameter based" or "distance based" and 
  "UZ based" registration types

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef boolean (*epzid_reg_change_handler_type) (word sid, word nid, byte pzid);

void mccsyobj_epzid_reg_sub_state_handler
(
  epzid_reg_change_handler_type epzid_reg_change_handler 
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_epzid_reg_sub_state_handler */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_IS707B_ADDENDUM 
#error code not present
#endif /* FEATURE_IS707B_ADDENDUM */

    /* Function to get current stored value of EPZID (SID, NID and PZID) */
/*===========================================================================

FUNCTION mccsyobj_get_epzid

DESCRIPTION
  This function gets current stored value of EPZID (SID, NID and PZID).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_get_epzid
( 
  word *sid, 
  word *nid, 
  byte *pzid 
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccsyobj_get_epzid */
/*~ PARAM sid POINTER */
/*~ PARAM nid POINTER */
/*~ PARAM pzid POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

  #ifdef FEATURE_IS2000_REL_A
/* Packet Data related system parameters */

/* Function to get old pzid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_pzid
  
DESCRIPTION
  This function get old pzid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccsyobj_get_old_pd_orig_pzid( void );

/* Function to set old pzid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_pzid

DESCRIPTION
  This function sets old pzid from the current onet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_pzid( byte pzid );

/* Function to get old sid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_sid

DESCRIPTION
  This function gets old sid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_old_pd_orig_sid( void );

/* Function to set old sid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_sid

DESCRIPTION
  This function sets old sid from the current one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_sid( word sid );

/* Function to get old nid */
/*===========================================================================

FUNCTION mccsyobj_get_old_pd_orig_nid

DESCRIPTION
  This function gets old nid.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccsyobj_get_old_pd_orig_nid( void );
    
/* Function to set old nid from the current one */
/*===========================================================================

FUNCTION mccsyobj_set_old_pd_orig_nid

DESCRIPTION
  This function sets old nid from the current one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsyobj_set_old_pd_orig_nid( word nid );
  
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION mccsyobj_initialize

DESCRIPTION
  This function initializes the mccsyobj_system object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyobj_initialize
(
  void
);

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#endif /* MCCSYOBJ_H */

