#ifndef _OEMBTSPP_H_
#define _OEMBTSPP_H_
/*=============================================================================
FILE:         OEMBTSPP.h

SERVICES:     IBTSPP Interfaces

DESCRIPTION:  OEMBTSPP is the OEM implementation of IBTSPPServer and 
              IBTSPPClient interfaces.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBT.h"
#include "AEEBTCommon.h"
#include "AEEBTSPP.h"
#include "AEEBTSPPServer.h"
#include "AEEBTSPPClient.h"

/*============================================================================
 SPP Handle that is used to identify an instance of a OEM object
============================================================================*/
typedef uint32 BTHandle;


/*============================================================================
 Bit mask that keep a track of change in Connection state, Modem status lines
 and change in line error 
============================================================================*/
#define OEMBTSPP_STATE_CHANGE  0x01
#define OEMBTSPP_MODEM_CHANGE  0x02
#define OEMBTSPP_LINE_ERROR    0x04

typedef uint8 OEMBTSPP_STATUS ;



/*============================================================================
  OEM APIs exported to the AEE layer   
============================================================================*/
int OEMBTSPP_New( BTHandle* phBT,
                  const uint8* pszServiceName,
                  uint16 version,
                  BTServiceClassUUID uuid,
                  boolean bServer);
int OEMBTSPP_ServClone( BTHandle* phBT, int16 streamID );
int OEMBTSPP_Release( BTHandle hBT );

int32 OEMBTSPP_Read( BTHandle   hBT, 
                     char*      pDst, 
                     int32      uMaxBytes );

void OEMBTSPP_Readable( BTHandle      hBT,
                        AEECallback*  pCb,
                        IShell*       pIShell );

int32 OEMBTSPP_Write( BTHandle hBT, 
                      const char* pSrc, 
                      int32 uNumBytes );

int OEMBTSPP_GetLastError( BTHandle hBT );

void OEMBTSPP_Writable( BTHandle hBT, AEECallback* pCb, IShell* pIShell );


int OEMBTSPP_GetStatus( BTHandle hBT, BTSPPStatus* pStatus );

int OEMBTSPP_GetModemStatus( BTHandle hBT, BTSPPModemStatus* pStatus );

int OEMBTSPP_GetLineError( BTHandle hBT, BTSPPLineError* pLineError );

int OEMBTSPP_Configure( BTHandle hBT, const BTSPPConfig* pConfig,
                        BTResult* pR, AEECallback* pRCb );


int OEMBTSPP_IOCtlEx( BTHandle            hBT,
                      BTSPPIOCtlCommand   IOctlCmd,
                      BTSPPModemStatus*   pModemStatus, 
                      AEECallback*        pResultCb,
                      IShell              *pIShell);

int OEMBTSPP_OnStatusChange( BTHandle        hBT,
                             AEECallback*    pStatusCb,
                             OEMBTSPP_STATUS statusType,
                             IShell          *pIShell );

int OEMBTSPP_Disconnect( BTHandle      hBT,
                         BTResult*     pResult,
                         AEECallback*  pResultCb );

/*============================================================================
  OEM APIs exported to the AEE layer only for SPP Server operations  
============================================================================*/
int OEMBTSPPSRV_EnableService( BTHandle         hBT,
                               BTSecurityLevel  secLevel,
                               uint32           uServerChannel,
                               boolean          bUnRegSdp,
                               BTResult*        pResult,
                               AEECallback*     pResultCb);

int OEMBTSPPSRV_Authorize( BTHandle          hBT,
                           const BDAddress*  pBDAddr,
                           boolean           bAccept,
                           BTResult*         pResult,
                           AEECallback*      pResultCb);

int OEMBTSPPSRV_OnConnRequest( BTHandle       hBT,
                               AEECallback*   pCb,
                               BTEventData*   pEv );

int OEMBTSPPSRV_DisableService( BTHandle          hBT,
                                BTResult*         pResult,
                                AEECallback*      pResultCb);


/*============================================================================
  OEM APIs exported to the AEE layer only for SPP Client operations  
============================================================================*/

int OEMBTSPPCLI_FindServersInit( BTHandle      hBT,
                                 uint32        searchDuration,
                                 uint32        maxServers);

int OEMBTSPPCLI_FindServersNext( BTHandle           hBT,
                                 uint32*            pSize,
                                 AEECallback*       pCb,
                                 IShell             *pIShell );

int OEMBTSPPCLI_GetServerInfo( BTHandle hBT, 
                               BTSPPDeviceInfo*   pServerInfo);

int OEMBTSPPCLI_Connect( BTHandle            hBT,
                         const BDAddress*    pBDAddr,
                         uint32              uServerChannel,
                         BTResult*           pResult,
                         AEECallback*        pResultCb);

int OEMBTSPPCLI_CancelServerSearch( BTHandle hBT,AEECallback* pCb );


#endif


