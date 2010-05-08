#ifndef _OEMBTEXTRM_H_
#define _OEMBTEXTRM_H_

/*===========================================================================

FILE:      OEMBTExtRM.h

SERVICES:  BlueTooth RM QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth RM

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtRM.h#2 $
$DateTime: 2009/04/16 17:46:44 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtRM_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtRM *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtRM_Init( IBTExtRM* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtRM_Release( IBTExtRM* pParent );

// IBTEXTRM_ConfigApplication (see AEEBTExtRM.h)
int OEMBTExtRM_ConfigApplication( IBTExtRM*      pParent, 
                                  boolean        bRemoveAppConfig,
                                  AEEBTActVsPwr  pageScanActVsPwr,
                                  AEEBTActVsPwr  inqScanActVsPwr,
                                  boolean        bDeliverDevUpdates );

// IBTEXTRM_AutoServiceSearchEnable (see AEEBTExtRM.h)
int OEMBTExtRM_AutoServiceSearchEnable( IBTExtRM*  pParent );

// IBTEXTRM_AutoServiceSearchDisable (see AEEBTExtRM.h)
int OEMBTExtRM_AutoServiceSearchDisable( IBTExtRM*  pParent );

// IBTEXTRM_GetLocalInfo (see AEEBTExtRM.h)
int OEMBTExtRM_GetLocalInfo( IBTExtRM*         pParent,
                             AEEBTLocalInfo*   pLocalInfo );

// IBTEXTRM_SetName (see AEEBTExtRM.h)
int OEMBTExtRM_SetName( IBTExtRM* pParent, 
                        const AECHAR* pName );

// IBTEXTRM_SetClassOfDevice (see AEEBTExtRM.h)
int OEMBTExtRM_SetClassOfDevice( IBTExtRM*             pParent, 
                                 AEEBTServiceClass     svcCls,
                                 AEEBTMajorDeviceClass majDevCls,
                                 AEEBTMinorDeviceClass minDevCls );

// IBTEXTRM_SetSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetSecurity( IBTExtRM* pParent, 
                            AEEBTSecurity sec );

// IBTEXTRM_Bond (see AEEBTExtRM.h)
int OEMBTExtRM_Bond( IBTExtRM*           pParent,
                     const AEEBTBDAddr  *pBDAddr, 
                     const AECHAR       *pPinCode );

// IBTEXTRM_Unbond (see AEEBTExtRM.h)
int OEMBTExtRM_Unbond( IBTExtRM* pParent, 
                       const AEEBTBDAddr* pBDAddr );

// IBTEXTSD_SetBondable (see AEEBTExtRM.h)
int OEMBTExtRM_SetBondable( IBTExtRM* pParent, 
                            boolean bondable );

// IBTEXTSD_PinReply (see AEEBTExtRM.h)
int OEMBTExtRM_PinReply( IBTExtRM*           pParent, 
                         const AEEBTBDAddr*  pBDAddr, 
                         const AECHAR*       pPinCode );

// IBTEXTRM_RegisterLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_RegisterLinkStatus( IBTExtRM*           pParent,
                                   const AEEBTBDAddr*  pBDAddr, 
                                   uint16              uRSSIDelta );

// IBTEXTRM_UnregisterLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_UnregisterLinkStatus( IBTExtRM*           pParent,
                                     const AEEBTBDAddr  *pBDAddr );

// IBTEXTRM_GetLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_GetLinkStatus( IBTExtRM*        pParent, 
                              AEEBTLinkStatus* pLinkStatus );

// IBTEXTRM_SetConnectionRole (see AEEBTExtRM.h)
int OEMBTExtRM_SetConnectionRole( IBTExtRM*           pParent,
                                  const AEEBTBDAddr*  pBDAddr,
                                  boolean             bMaster );

// IBTEXTRM_SetConnectionSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetConnectionSecurity( IBTExtRM*          pParent,
                                      const AEEBTBDAddr* pBDAddr, 
                                      AEEBTSecurity      security );

// IBTEXTRM_SetDeviceSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetDeviceSecurity( IBTExtRM*          pParent, 
                                  const AEEBTBDAddr* pBDAddr, 
                                  AEEBTSecurity      security );

// IBTEXTRM_DeviceAdd (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceAdd( IBTExtRM*              pParent,
                          const AEEBTDeviceInfo* pDevice );

// IBTEXTRM_DeviceUpdate (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceUpdate( IBTExtRM*               pParent,
                             AEEBTDeviceUpdateBitmap updateBitmap,
                             const AEEBTDeviceInfo*  pDevice );

// IBTEXTSD_DeviceServicesUpdate (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceServicesUpdate( IBTExtRM*                 pParent, 
                                     const AEEBTBDAddr*        pBDAddr,
                                     uint16                    uNumServices,
                                     const AEEBTSimpleService* pServices );

// IBTEXTRM_DeviceRead (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceRead( IBTExtRM*        pParent,
                           AEEBTDeviceInfo *pDevice );

// IBTEXTRM_DeviceEnumInit (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceEnumInit( IBTExtRM*                    pParent,
                               const AEEBTDeviceEnumerator *pEnum );

// IBTEXTRM_DeviceEnumNext (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceEnumNext( IBTExtRM*         pParent,
                               AEEBTDeviceInfo  *pDevice );

// IBTEXTRM_DeviceRemove (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceRemove( IBTExtRM* pParent, 
                             const AEEBTBDAddr* pBDAddr );

// IBTEXTRM_SetHCIMode (see AEEBTExtRM.h)
int OEMBTExtRM_SetHCIMode( IBTExtRM* pParent, 
                           AEEBTHCIMode HCIMode );

// IBTEXTRM_GetHCIMode (see AEEBTExtRM.h)
int OEMBTExtRM_GetHCIMode( IBTExtRM* pParent, AEEBTHCIMode* pHCIMode );

// IBTEXTRM_SetServiceSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetServiceSecurity( IBTExtRM* po, 
                                   const AEEBTServiceIdentifier* pServiceId,
                                   AEEBTSecurity minSecurity, 
                                   boolean bAuthorize, 
                                   boolean bAuthorizeFirst );

// IBTEXTRM_AuthorizeReply (see AEEBTExtRM.h)
int OEMBTExtRM_AuthorizeReply( IBTExtRM* po, 
                               const AEEBTServiceIdentifier* pServiceId,
                               const AEEBTBDAddr* pBDAddr,
                               boolean bAuthorize );

// IBTEXTRM_EnterTestMode (see AEEBTExtRM.h)
int OEMBTExtRM_EnterTestMode( IBTExtRM* po );

// IBTEXTRM_ExitTestMode (see AEEBTExtRM.h)
int OEMBTExtRM_ExitTestMode( IBTExtRM* po );

// IBTEXTRM_ExitTestMode (see AEEBTExtRM.h)
int OEMBTExtRM_GetBTOnOffState( IBTExtRM* po, boolean* pBTIsOn );

// IBTEXTRM_AllowRoleSwitch
int OEMBTExtRM_AllowRoleSwitch( IBTExtRM*          pParent, 
                                const AEEBTBDAddr* pBDAddr,
                                boolean            bAllow );

// IBTEXTRM_DisableRadio
int OEMBTExtRM_DisableRadio( IBTExtRM* po, boolean bDisable );

// IBTEXTRM_DisableVisibility
int OEMBTExtRM_DisableVisibility( IBTExtRM* po, boolean bDisable );

// IBTEXTRM_RegisterConnStatus (see AEEBTExtRM.h)
int OEMBTExtRM_RegisterConnStatus( IBTExtRM*  pParent );

// IBTEXTRM_UnregisterConnStatus (see AEEBTExtRM.h)
int OEMBTExtRM_UnregisterConnStatus( IBTExtRM*  pParent );

// IBTEXTRM_RegisterRadioActivity (see AEEBTExtRM.h)
int OEMBTExtRM_RegisterRadioActivity( IBTExtRM*           pParent, 
                                      AEEBTRadioActivity* pRadioAct,
                                      AEECallback*        pCb );

// IBTEXTRM_UnegisterRadioActivity (see AEEBTExtRM.h)
int OEMBTExtRM_UnregisterRadioActivity( IBTExtRM* pParent );

// IBTEXTRM_GetRadioStatus (see AEEBTExtRM.h)
int OEMBTExtRM_GetRadioActivity( IBTExtRM*           pParent, 
                                 AEEBTRadioActivity* pRadioActivity );

// IBTEXTRM_GetConnectionRole (see AEEBTExtRM.h)
int OEMBTExtRM_GetConnectionRole( IBTExtRM*           pParent,
                                  const AEEBTBDAddr*  pBDAddr,
                                  boolean*            pbMaster );

// IBTEXTRM_SetNickName
int OEMBTExtRM_SetNickName( IBTExtRM*          po, 
                            const AEEBTBDAddr* pBDAddr, 
                            const AECHAR*      pwNickName );

// IBTEXTRM_SetEIRManufData
int OEMBTExtRM_SetEIRManufData( IBTExtRM*    po, 
                                const uint8* pManufData,
                                uint8        uSize );

// IBTEXTRM_GetEIRUUIDList
int OEMBTExtRM_GetEIRUUIDList( IBTExtRM*          po, 
                               const AEEBTBDAddr* pBDAddr, 
                               AEEBTUUIDType      uuidType,
                               uint8              uuidListStartIdx,
                               uint8*             pListSize,
                               void*              pUUIDList );

// IBTEXTRM_GetEIRManufData
int OEMBTExtRM_GetEIRManufData( IBTExtRM*          po, 
                                const AEEBTBDAddr* pBDAddr, 
                                uint8*             pSize,
                                uint8*             pManufData );

// IBTEXTRM_SetIOCap
int OEMBTExtRM_SetIOCap( IBTExtRM* po, AEEBTIOCapType IOCap );

// IBTEXTRM_SetSecBySvcCls
int OEMBTExtRM_SetSecBySvcCls( IBTExtRM*         po, 
                               uint16            uServiceClass,
                               AEEBTSvcSecLevel  minSecurity, 
                               boolean           bAuthorize, 
                               boolean           bAuthorizeFirst );

// IBTEXTRM_SetSecBySrvrChnl
int OEMBTExtRM_SetSecBySrvrChnl( IBTExtRM*         po, 
                                 uint8             uSCN,
                                 AEEBTSvcSecLevel  minSecurity, 
                                 boolean           bAuthorize, 
                                 boolean           bAuthorizeFirst );

// IBTEXTRM_SetSecByPSM
int OEMBTExtRM_SetSecByPSM( IBTExtRM*         po, 
                            uint16            uServiceClass,
                            AEEBTSvcSecLevel  minSecurity, 
                            boolean           bAuthorize, 
                            boolean           bAuthorizeFirst );

// IBTEXTRM_SetRemoteOOBData
int OEMBTExtRM_SetRemoteOOBData( IBTExtRM*          po, 
                                 const uint8*       pOOBData,
                                 uint8              uSize );

// IBTEXTRM_UserConfirmationReply
int OEMBTExtRM_UserConfirmationReply( IBTExtRM*          po, 
                                      const AEEBTBDAddr* pBDAddr, 
                                      boolean            bConfirmed );

// IBTEXTRM_KeypressNotification
int OEMBTExtRM_KeypressNotification( IBTExtRM*          po, 
                                     const AEEBTBDAddr* pBDAddr, 
                                     AEEBTKeypressType  keypress );

// IBTEXTRM_PasskeyReply
int OEMBTExtRM_PasskeyReply( IBTExtRM*          po, 
                             const AEEBTBDAddr* pBDAddr,
                             const char passkey[ AEEBT_SSP_PASSKEY_LEN ]  );

// IBTEXTRM_CreateOOBData
int OEMBTExtRM_CreateOOBData( IBTExtRM*     po, 
                              uint8*        pOOBDataSize,
                              AEECallback*  pCb );

// IBTEXTRM_ReadOOBData
int OEMBTExtRM_ReadOOBData( IBTExtRM*     po, 
                            uint8*        pOOBDataSize,
                            uint8*        pOOBData );

// IBTEXTRM_AuthorizeRebond
int OEMBTExtRM_AuthorizeRebond( IBTExtRM*           po, 
                                const AEEBTBDAddr*  pBDAddr,
                                boolean             bAuthorized,
                                AEEBTRebondSecurity rebondSec );

// IBTEXTRM_SSPBond
int OEMBTExtSSPBond( IBTExtRM*           po, 
                     const AEEBTBDAddr*  pBDAddr,
                     boolean             bMITMProtectionRequired );

// IBTEXTRM_SetSM4DebugMode
int OEMBTExtRM_SetSM4DebugMode( IBTExtRM* po, boolean bEnabled );

// IBTEXTRM_LowPowerConfig
int OEMBTExtRM_LowPowerConfig( IBTExtRM* po, 
                               const AEEBTBDAddr* pBDAddr, 
                               AEEBTLowPowerControl lpc, 
                               uint16 uMaxInterval,
                               uint16 uIdleTO, 
                               uint16 uMaxLatency, 
                               boolean bMustUse, 
                               boolean bSniffInitiatorOriented );

// IBTEXTRM_LowPowerConfigTest
int OEMBTExtRM_LowPowerConfigTest( IBTExtRM* pParent,
                                   const AEEBTBDAddr*    pBDAddr, 
                                   AEEBTLowPowerControl  lpc, 
                                   uint16                uMaxInterval,
                                   uint16                uIdleTO, 
                                   uint16                uMaxLatency, 
                                   boolean               bMustUse, 
                                   boolean               bSniffInitiatorOriented, 
                                   uint8                 testId );


// IBTEXTRM_GetRoleSwitchPolicy (see AEEBTExtRM.h)
int OEMBTExtRM_GetRoleSwitchPolicy( IBTExtRM*           pParent,
                                    const AEEBTBDAddr*  pBDAddr,
                                    boolean*            pbAllowed );
// IBTEXTRM_BondCancel(see AEEBTExtRM.h)
int OEMBTExtRM_BondCancel( IBTExtRM*           pParent,
                           const AEEBTBDAddr*  pBDAddr);

#endif // _OEMBTEXTRM_H_
