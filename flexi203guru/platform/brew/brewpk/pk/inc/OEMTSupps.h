#ifndef __OEMTSUPPS_H__
#define __OEMTSUPPS_H__

/*============================================================================
FILE:  OEMTSupps.h

SERVICES:  OEM Telephony Supplementary Services.

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. to support BREW Telephony

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "OEMTelephone.h"

typedef struct OEMTSupps OEMTSupps;
typedef struct OEMSupps OEMSupps;

typedef struct OEMSuppsTrans
{
   AEETSuppsTransId     m_transId;
   AEETSuppsTransState  m_transState;
   AEETSuppsStatus      m_result;
   OEMTSupps*           pOEMTSupps;
}OEMSuppsTrans;

#ifdef __cplusplus
extern "C" {
#endif
int OEMSupps_ParseString(const char *pszString, AEETSuppsMessage *pMsg, uint32 *pdwSize);
int OEMTSupps_SendMessage(OEMTSupps* pOEMSupps, AEETSuppsMessage *pMsg, AEECallback *pcb);

int OEMTSupps_New(AEETSuppsTransId nTransId, OEMSuppsTrans* pTrans, 
                  OEMTSupps** ppOEMSupps);
void OEMTSupps_Delete(OEMTSupps*);

int OEMSuppsT_New(OEMSupps** ppOEMSupps);
uint32 OEMSuppsT_Release(OEMSupps* pOEMSupps);

#ifdef __cplusplus
}
#endif

/*===================================================================== 
 INTERFACES   DOCUMENTATION
=======================================================================
OEMTSupps Interface
=======================================================================

Function: 

Description:
  
Prototype:
      
Parameters:

Return Value:

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================*/

#endif /* __OEMTSUPPS_H__ */
