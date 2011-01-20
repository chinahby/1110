#ifndef _OEMBTEXTSD_H_
#define _OEMBTEXTSD_H_
/*===========================================================================

FILE:      OEMBTExtSD.h

SERVICES:  BlueTooth SD QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth SD

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004, 2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

Function:  OEMBTExtSD_Init()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IBTExtSD *pParent: Pointer to the IBTExtSD structure allocated by AEE

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates APP memory for an object

===========================================================================*/
int    OEMBTExtSD_Init( IBTExtSD *pParent );

// IBase (see AEE.h)
uint32 OEMBTExtSD_Release( IBTExtSD* po );

// IBTEXTSD_DiscoverDevices (see AEEBTExtSD.h)
int    OEMBTExtSD_DiscoverDevices( IBTExtSD* po, 
                                   AEEBTServiceClass svcCls, 
                                   const AEEBTBDAddr*,
                                   AEEBTDeviceRecord* pDevRec,
                                   uint8 uMaxNumRe );

// IBTEXTSD_StopDeviceDiscovery (see AEEBTExtSD.h)
int    OEMBTExtSD_StopDeviceDiscovery(IBTExtSD* po );

// IBTEXTSD_SetDiscoverable (see AEEBTExtSD.h)
int    OEMBTExtSD_SetDiscoverable( IBTExtSD* po, boolean discoverable );

// IBTExtSD_GetDeviceName (see AEEBTExtSD.h)
int    OEMBTExtSD_GetDeviceName( IBTExtSD* po, const AEEBTBDAddr*,
                                 AECHAR* pDevName, uint8 uMaxLen );

// IBTEXTSD_BrowseService (see AEEBTExtSD.h)
int    OEMBTExtSD_BrowseService( IBTExtSD* po, const AEEBTBDAddr*,
                                 AEEBTBrowseRecord*, uint8 );

// IBTEXTSD_SearchService (see AEEBTExtSD.h)
int    OEMBTExtSD_SearchService( IBTExtSD* po, const AEEBTBDAddr*,
                                 const AEEBTSearchPattern*, 
                                 AEEBTServiceRecord*, uint8 );

// IBTEXTSD_RegisterService (see AEEBTExtSD.h)
int    OEMBTExtSD_RegisterService( IBTExtSD* po, uint16 uuid, 
                                   uint16 version, uint8 scn, 
                                   const AECHAR* svcName );

// IBTEXTSD_RegisterCustomService (see AEEBTExtSD.h)
int    OEMBTExtSD_RegisterCustomService( IBTExtSD* po,
                                         const AEEBTUuidList* pCustomSvc,
                                         uint16 version, uint8 scn, uint16 psm,
                                         const AECHAR* svcName);

// IBTEXTSD_UnregisterService (see AEEBTExtSD.h)
int    OEMBTExtSD_UnregisterService( IBTExtSD* po, uint16 uuid );

// IBTEXTSD_UnregisterCustomService (see AEEBTExtSD.h)
int    OEMBTExtSD_UnregisterCustomService( IBTExtSD* po,
                                           const AEEBTUUID128* pCustomUUID);

// IBTEXTSD_SetLimitedDiscoverable (see AEEBTExtSD.h)
int    OEMBTExtSD_SetLimitedDiscoverable( IBTExtSD* po, AEEBTLIAC uLIAC );

// IBTEXTSD_GetDiscoverableMode (see AEEBTExtSD.h)
int    OEMBTExtSD_GetDiscoverableMode( IBTExtSD* po );

// IBTEXTSD_AddServiceRecord (see AEEBTExtSD.h)
int    OEMBTExtSD_AddServiceRecord( IBTExtSD* po,
                                    uint16 uuid,
                                    const AEEBTServiceRecord* pSvcRec );

// IBTEXTSD_AddServiceAttribute (see AEEBTExtSD.h)
int    OEMBTExtSD_AddServiceAttribute( IBTExtSD* po, uint32 uSvcRecHandle,
                                       const AEEBTServiceAttribute* pSvcAttr );

// IBTEXTSD_UpdateServiceAttribute (see AEEBTExtSD.h)
int    OEMBTExtSD_UpdateServiceAttribute( IBTExtSD* po, uint32 uSvcRecHandle,
                                          const AEEBTServiceAttribute* pSvcAttr ) ;

// IBTEXTSD_ServiceDBEnumInit (see AEEBTExtSD.h)
int    OEMBTExtSD_ServiceDBEnumInit( IBTExtSD* po );

// IBTEXTSD_ServiceDBEnumNext (see AEEBTExtSD.h)
int    OEMBTExtSD_ServiceDBEnumNext( IBTExtSD* po, AEEBTServiceRecord* pSvcRec );

// IBTEXTSD_GetDeviceNameCancel (see AEEBTExtSD.h)
int    OEMBTExtSD_GetDeviceNameCancel(  IBTExtSD* po, const AEEBTBDAddr*  pBDAddr );

// IBTEXTSD_BrowseServiceCancel(see AEEBTExtSD.h)
int OEMBTExtSD_BrowseServiceCancel(  IBTExtSD* po, const AEEBTBDAddr*  pBDAddr );
#endif /* _OEMBTEXTSD_H_ */
