#ifndef AEEWMSDIAG_PRIV_H
#define AEEWMSDIAG_PRIV_H
/*=============================================================================
FILE:  AEEWMSDIAG_priv.h

SERVICES:  AEEWMSDIAG Private header File

DESCRIPTION: Definitions to support access to private AEEWMSDiag interface

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright 2004,2005,2006,2007,2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/AEEWMSDIAG_priv.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
05/10/04   Rex     Initial Version

===========================================================================*/


#include "AEEWMSDIAG.h"


/*===========================================================================

                             TYPE DEFINITIONS

===========================================================================*/

/* Static buffers in AEE layer where OEM layer can fill up notify information */
extern AEEWMSDIAGNotifyInfo   ReqNotifyInfo;



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

/* Request commands send by OEM to AEE layer for notifying WMSAPP */
int AEEWMSDIAG_Notify (AEEWMSDIAGNotifyInfo  *pInfo);



#endif /* AEEWMSDIAG_PRIV_H */
