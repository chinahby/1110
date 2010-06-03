#ifndef DS_H
#define DS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                                  D S 

GENERAL DESCRIPTION
  This is a placeholder file for external (i.e, non-data) references to old
  DS interfaces which may now have different prototypes/names.
  
EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds.h_v   1.3   08 Nov 2002 11:57:22   ubabbar  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds.h#1 $ $DateTime: 2008/04/11 08:25:15 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
08/20/02   ak         Initial verion.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#ifdef FEATURE_DATA_IS707

#include "ds707.h"
#include "dstask.h"

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
unsigned char ds_scrm_max_allowable_sch_rate(void);

#endif /* FEATURE_DATA_IS707 */

#endif /* DS_H */
