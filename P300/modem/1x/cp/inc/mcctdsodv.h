#ifndef TDSODV_H
#define TDSODV_H
/*===========================================================================


         T E S T   D A T A   S E R V I C E   O P T I O N

                  H E A D E R    F I L E   
                
                F O R  D V  C H A N N E L S


DESCRIPTION
  This header file contains functions that are intended to be used internally
  within the simple/Full Test Data Service Options for configuring, managing
  and updating statistics the F-TDSO service option for DV channels.

Copyright (c) 2004-2005 by QUALCOMM Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

            EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mcctdsodv.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/05   az      Changes for Rel D DV TDSO.
09/30/04   ht      Fixed the patterns for R-CQICH and R-ACKCH.
09/13/04   vlc     Removed inclusion of rxcpdchdemux.h.  Moved TDSO defines 
                   from rxcpdchdemux.h to this file.  Changed names of a few 
                   defines to conform to CP naming convention.
08/19/04   ht      Corrected index to the PDCH counter table.
08/02/04   bkm     Updated featurization on file.
08/02/04   ht      Fixed Lint error.
07/09/04   ht      Created file.

===========================================================================*/

/* <EJECT>

*/
#include "customer.h"

#if defined(FEATURE_IS2000_REL_C_DV) && defined(FEATURE_IS2000_TDSO)
#error code not present
#endif // FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO


#endif // TDSODV_H

