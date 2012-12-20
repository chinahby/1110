/*============================================================================

FILE:  OEMVdec.c

SERVICES: IVDec interfaces

DESCRIPTION
   This file implements IVDec interfaces for accessing the vdec(video decoder).

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

               Copyright © 2009 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "vdecoder_types.h"
#include "OEMHeap.h"
#include "OEMConfig.h"
#include "OEMObjectMgr.h"
#include "AEE_OEM.h"
#include "AEEClassIDs.h"
#include "AEEShell.h"  /* AEE Shell services, IShell declarations */
#include "AEEStdLib.h" /* AEE StdLib decls */
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "vdecoder.h"
#if defined(T_MSM7500)
#error code not present
#endif
#include "msg.h"

#ifdef FEATURE_IVDEC_WMV
#error code not present
#endif
