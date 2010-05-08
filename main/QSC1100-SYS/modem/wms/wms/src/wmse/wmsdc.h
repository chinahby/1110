#ifndef WMSDC_H
#define WMSDC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Dedicated Channel Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the data types for the Dedicated Channel group.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsdc.h_v   1.1   08 May 2002 08:41:58   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsdc.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
11/28/06   NP      Added new variable to keep track of wms originated
                   traffic calls which should then be released by wms.
10/05/06   PMD     Added default_so_from_nv variable in wms_dc_s_type
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Updated copyright info
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/09/02   HQ      Added support of Multimode WMS.
===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"
#include "cm.h"

/*===========================================================================
                             
                         DATA TYPE DECLARATIONS
                             
===========================================================================*/
/* DC data
*/
typedef struct wms_dc_struct
{
#ifdef FEATURE_CDSMS
  uint32                idle_timeout;
    /* in milli-seconds; 0 means auto-disconnect is disbaled */

  boolean               wms_initiated_call;
    /* Flag to keep track if the call was started by WMS.  If set, 
     * and auto-dis is disabled, wms is responsible for tearing
     * down call also once rec queue is empty 
     */
  boolean               call_active;
    /* Flag to keep track if call is active or not */

  rex_timer_type        call_out_timer;
  rex_timer_type        idle_timer;
  boolean               auto_disc_active;
  wms_dc_so_e_type      default_so_from_nv;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

} wms_dc_s_type;



/* <EJECT> */
/*===========================================================================
                             
                       FUNCTION DEFINITIONS
                             
===========================================================================*/

/*===========================================================================
FUNCTION wms_dc_s_ptr

DESCRIPTION
  Return the pointer to the DC data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal DC data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dc_s_type* wms_dc_s_ptr
(
  void
);



/*===========================================================================
FUNCTION wms_dc_process_cmd

DESCRIPTION
  Dispatch the DC command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dc_process_cmd
(
  wms_cmd_type  *cmd_ptr
);



/*===========================================================================
FUNCTION wms_dc_init

DESCRIPTION
  Do DC-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  DC data initialized.
===========================================================================*/
void wms_dc_init
(
  void
);


#ifdef FEATURE_CDSMS

void wms_dc_call_out_timer_proc( void );

void wms_dc_idle_timer_proc( void );

#endif /* FEATURE_CDSMS */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSDC_H */

