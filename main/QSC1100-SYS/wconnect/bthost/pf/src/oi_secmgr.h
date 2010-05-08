#ifndef _OI_SECMGR_H
#define _OI_SECMGR_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_secmgr.h#1 $ 

$Log: $

===========================================================================*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
/** 
    @file  
    This file provides the interface between the Security Manager and other program layers.
*/

#include "oi_common.h"
#include "oi_bt_spec.h"
                            
/*************************************************************

    basic security definitions

*************************************************************/

/** The overall device and stack is in one of the following security modes
*/
typedef enum  {
        /** No security measures are initiated by the Security Manager.
        */
    OI_SECURITY_MODE_1,

        /** Security is enforced for each service individually.
        */
    OI_SECURITY_MODE_2,        

        /** Security is enforced at the link level before a connection is completed.
        */
    OI_SECURITY_MODE_3         

} OI_SECURITY_MODE;


/** Security policies:
    
    This enumeration allows for specification of what security measures are required.
    The policy will be applied for all links (security mode 3) or for a given service 
    (security mode 2). Authorization and encryption both imply authentication. 
    An application may determine that authorization requires user input.
*/
typedef enum {
    OI_SEC_REQUIRE_NOTHING,            /**< no security at all required for service */
    OI_SEC_REQUIRE_AUTHENTICATION,     /**< authentication required */
    OI_SEC_REQUIRE_AUTHORIZATION,      /**< authorization required, which implies authentication */
    OI_SEC_REQUIRE_ENCRYPTION,         /**< encryption required, which implies authentication */
    OI_SEC_REQUIRE_AUTHOR_ENCRYPT      /**< authorization and encryption required, which implies authentication */
} OI_SECURITY_POLICY;

/** Persistence policy options:

    This enumeration allows for specification of where the Security Manager is to 
    store newly-generated link keys.
*/
typedef enum {

        /** Newly-generated link keys are ignored, never stored */
    OI_SEC_NEVER_STORE,

        /** Newly-generated link keys are always stored in the Bluetooth device
            itself. Available storage space is dependent on characteristics of the
            device. */
    OI_SEC_STORE_IN_DEVICE,

        /** New keys are passed to a higher-level process (e.g., the application) 
            for storage. Note that this also requires that the application specify 
            an appropriate callback function(@ref OI_SECMGR_HL_NEW_LINK_KEY) to the 
            configuration function OI_SECMGR_Configure() for such handling of newly-
            generated link keys. */
    OI_SEC_PASS_TO_HIGHER_LAYER
} OI_SECMGR_LINKKEY_STORAGE_POLICY;


/**     This typedef defines a structure that associates a link key with a specific 
        device. This type definition is used throughout the Security Manager API. 
*/
typedef struct {
    OI_BD_ADDR  addr;      /**< Bluetooth device address */
    OI_LINK_KEY linkKey;   /**< link key associated with a specific device */
} OI_SEC_LINKKEY_REC;

/*************************************************************

    API for calls made by the Security Manager to higher program layers

*************************************************************/

/**
    When the Security Manager needs a link key for authentication, it calls this function.

    @param pAddr      pointer to the Bluetooth device address of the device for 
                      which the link key is being requested

    @param pLinkKey   pointer to the buffer where the link key is to be stored

    @return           TRUE if a key has been returned in *pLinkKey,
                      FALSE otherwise
*/
typedef OI_BOOL(*OI_SECMGR_HL_LINK_KEY_REQUEST)(OI_BD_ADDR *pAddr,
                                             OI_LINK_KEY *pLinkKey);

/**
    If the link key storage policy(@ref OI_SECMGR_LINKKEY_STORAGE_POLICY) has been 
    specified as @ref OI_SEC_PASS_TO_HIGHER_LAYER, then the Security Manager calls this
    function to pass a new link key up to a higher program layer.
    
    @param pAddr     pointer to the Bluetooth device address of the device for 
                     which the key was generated

    @param pLinkKey  pointer to the buffer where the new link key is stored

    @param keyType   type of link key
*/
typedef void (*OI_SECMGR_HL_NEW_LINK_KEY)(OI_BD_ADDR *pAddr,
                                       OI_LINK_KEY *pLinkKey,
                                       OI_BT_LINK_KEY_TYPE keyType);

/**
    When the Security Manager needs a PIN, it calls this callback function to request
    that a higher program layer provide the PIN.  After obtaining the PIN (or not),
    the higher-layer process must call the OI_SECMGR_PinCode() function, whether 
    or not the attempt to get the PIN was successful.  The OI_SECMGR_PinCode() may be 
    called directly from within the PIN request callback.

    @param pAddr         pointer to the Bluetooth device address of the device for 
                         which the pin code is requested

    @param minPinCodeLen minimum number of characters required for a PIN

*/
typedef void(*OI_SECMGR_HL_PIN_CODE_REQUEST)(OI_BD_ADDR *pAddr,
                                             OI_UINT8 minPinCodeLen);

/**
    When the Security Manager needs to authorize a device, it calls this callback function to 
    request authorization.  After authorizing the device (or not), the higher-layer process 
    must call OI_SECMGR_Authorize(), whether the device is authorized or not.  

    @param pAddr         pointer to the Bluetooth device address of the device for 
                         which authorization is requested

    @param serviceUUID   UUID of the service to which the remote device is trying to connect
                         (UUID's are enumerated in file oi_bt_assigned_nos.h).
                         Name of the service can be obtained via OI_UUIDText().
                
*/
typedef void(*OI_SECMGR_HL_AUTHORIZE_REQUEST)(OI_BD_ADDR *pAddr,
                                              OI_UINT32  uuid);
                                                  
                                                  

/**
    This is a pointer to a callback function that is invoked upon completion of the 
    OI_SECMGR_DeleteStoredLinkKey() function.

    @param status          result of the stored link key deletion operation

    @param addr            This is a pointer to the Bluetooth device address of the 
                           device whose link key was deleted. This will be NULL if the
                           deleteAll parameter was passed to OI_SECMGR_DeleteStoredLinkKey().

    @param numKeysDeleted  number of link keys that were actually deleted by the 
                           OI_SECMGR_DeleteStoredLinkKey() function
*/
typedef void(*OI_SECMGR_DELETE_KEYS_CALLBACK)(OI_STATUS status,
                                           OI_BD_ADDR  *addr,
                                           OI_UINT16   numKeysDeleted);

/**
    This is a pointer to a callback function that is invoked upon completion of the
    OI_SECMGR_GetAllStoredLinkKeys() function.

    @param status         result of the get link keys operation
    
    @param linkkeyArray   This is the same array as that passed in the originating 
                          call to OI_SECMGR_GetAllStoredLinkKeys(). This is the array in
                          which the Security Manager stores link keys read from the 
                          device. 

    @param numKeys        number of keys returned in the array linkkeyArray

    @param maxKeys        total number of link keys that the device can store
*/
typedef void(*OI_SECMGR_GET_LINKKEYS_CALLBACK)(OI_STATUS status,
                        OI_SEC_LINKKEY_REC  *linkkeyArray,
                        OI_UINT16        numKeys,
                        OI_UINT16        maxKeys);

/**
    This is a pointer to a callback function that is invoked upon completion of the
    OI_SECMGR_ChangeConnectionLinkKey() function.
    
    @param status    result of the change link keys operation
    
    @param addr      If the link key change was successful, this parameter is a pointer
                     to the Bluetooth device address of the device whose key was changed.
*/
typedef void(*OI_SECMGR_CHANGE_LINKKEY_CALLBACK)(OI_STATUS status,
                        OI_BD_ADDR       *addr);

/**
    This callback indicates completion of a bonding attempt. 
   
    @param status    result of the bonding attempt
    
    @param addr      address of the remote device
 */
typedef void(*OI_SECMGR_BOND_WITH_DEVICE_CALLBACK)(OI_STATUS status,
                                                   OI_BD_ADDR *addr);
                                                   
/**
    pointer to the callback function invoked upon completion of OI_SECMGR_Configure()

    @param status  result of the configuration
                    - OI_OK, configuratin successful
                    - Any other value indicates configuration failure. The status code
                    value contains the specific reason for failure.
*/
typedef void(*OI_SECMGR_CONFIGURE_CALLBACK)(OI_STATUS status);


/**
    The audit callback will be called when significant security events occur after a connection
    has been established. Users are cautioned against putting too much reliance on the audit 
    callbacks. In our experience, audit events are generated only when the authentication 
    and/or encryption has been initiated programmatically by the local device.  
        - If the remote device initiated the security, there are no audit events on the local device.
        - If the security was initiated by the Bluetooth device (security mode 3), no
            audit events are generated on the local device.
            
    The limitations are limitations of the Bluetooth HCI specification, not software capabilities
    of BLUEmagic 3.0 software.            
        
    @param  auditEvent  identifies the specific event that occurred
    
    @param  status      status associated with the event

    @param pAddr        pointer to the Bluetooth device address of the remote device
*/

typedef enum  {
    OI_SECMGR_AUTHENTICATION_COMPLETED,
    OI_SECMGR_ENCRYPTION_ENABLED,
    OI_SECMGR_ENCRYPTION_DISABLED,
    OI_SECMGR_AUTHORIZATION_COMPLETED
} OI_SECMGR_AUDIT_EVENT ;


typedef void (*OI_SECMGR_AUDIT_CALLBACK)(
                            OI_SECMGR_AUDIT_EVENT   event,
                            OI_STATUS               status,
                            OI_BD_ADDR              *pAddr) ;

/** This API consists of function calls made by the Security Manager to 
    higher program layers, such as the application itself. These calls are made
    via pointers to callback functions registered with the CThru Dispatcher 
    in the Security Manager configuration process. Any of these pointers 
    to higher-layer callback functions may be NULL, in which case the 
    functionality associated with these functions is absent from the system's
    security policy.

    All callback functions from the Security Manager to higher program layers
    are passed to the Security Manager in a structure of this type.
    Callback functions that are not implemented should be set to NULL.
 */
typedef struct  {
    OI_SECMGR_HL_LINK_KEY_REQUEST  secmgr_HlLinkKeyRequest;   /**< function for requesting link key from higher layer */    
    OI_SECMGR_HL_NEW_LINK_KEY      secmgr_HlNewLinkKey;       /**< function for passing link key to higher layer */
    OI_SECMGR_HL_PIN_CODE_REQUEST  secmgr_HlPinCodeRequest;   /**< function for requesting PIN from higher layer */
    OI_SECMGR_HL_AUTHORIZE_REQUEST secmgr_HLAuthorizeRequest; /**< function for requesting authorization from higher layer */
    OI_SECMGR_AUDIT_CALLBACK       secmgr_HLAuditCallback;    /**< function which is called to report audit events */
}   OI_SECMGR_HL_FUNCTION_API;
/*************************************************************

    Security Manager API managing link keys stored in the Bluetooth device

*************************************************************/

/**
    This function reads the link keys stored internally in the Bluetooth device.
    The link keys are written to the link key array that is passed as an argument to
    this function. The size of this array is indicated, specifying the number of
    link keys that can be written to it. The callback function is called upon 
    completion of the called function.

    @param cb             This indicates the callback function that is to be invoked
                          upon completion of the called function.

    @param linkkeyArray   array in which the Security Manager will store the keys
                          read from the device

    @param maxKeys        size of the linkkeyArray array, specified as the maximum 
                          number of link keys that it can contain 
                          This value is a number of link keys, NOT size in bytes!
 */
extern OI_STATUS OI_SECMGR_GetAllStoredLinkKeys(OI_SECMGR_GET_LINKKEYS_CALLBACK cb,
                        OI_SEC_LINKKEY_REC         *linkkeyArray,
                        OI_UINT16               maxKeys);

/**
    This function deletes a single link key or all link keys stored internally
    in a Bluetooth device.
   
    @param cb           This indicates the callback function that is to be invoked
                        upon completion of the called function.

    @param addr         pointer to the Bluetooth device address of the device whose
                        link key is to be deleted

    @param deleteAll    This parameter indicates whether all internally stored link 
                        keys are to be deleted or just the link key associated with
                        the Bluetooth device address specified by addr is to be deleted.
                        If this parameter is
                        - TRUE, then all link keys stored by the device are deleted and
                        addr is ignored.
                        - FALSE, then only the link key associated with addr is deleted.
*/
extern OI_STATUS OI_SECMGR_DeleteStoredLinkKey(OI_SECMGR_DELETE_KEYS_CALLBACK cb,
                        OI_BD_ADDR          *addr,
                        OI_BOOL             deleteAll);


/*************************************************************

    Security Manager APIs

*************************************************************/

/**
    This function changes the link key used on a connection with a specific device.
    This function call will return a failure staus code if there is not currently
    a connection to the specified device. When a new link key is generated, it will
    be handled in accordance with the current link key storage policy.

    @param cb           This indicates the callback function that is to be invoked
                        upon completion of the called function.

    @param addr         pointer to the Bluetooth device address of the device whose 
                        link key is to be changed
*/
extern  OI_STATUS OI_SECMGR_ChangeConnectionLinkKey(OI_SECMGR_CHANGE_LINKKEY_CALLBACK cb,
                        OI_BD_ADDR          *addr);

/**
 * Attempts to perform bonding (as specified by the GAP) with the specified
 * device.
 * @param cb    This indicates the callback function that is to be invoked
                        upon completion of the called function.
   @param addr      The device with which bonding is to be attempted
 */
extern OI_STATUS OI_SECMGR_BondWithDevice(OI_SECMGR_BOND_WITH_DEVICE_CALLBACK cb,
                                          OI_BD_ADDR *addr);


/**
    After the higher program layer process has attempted to obtain a PIN, it calls
    this function to pass the PIN down to the Security Manager. If the attempt to 
    obtain a PIN was unsuccessful, this function is still called to indicate the 
    failure.
  
    @param successful   indicates whether or not the higher program layer was able
                        to obtain a PIN
                            - TRUE: A PIN was obtained and all other parameter fields
                            for OI_SECMGR_PinCode() contain valid parameters.
                            - FALSE: A PIN was not obtained.
                            
    @param pAddr        pointer to the Bluetooth device address of the device for 
                        which the PIN was requested

    @param pPinCode     pointer to buffer where the new PIN is stored,
                        undefined if the first parameter value is FALSE

    @param pinCodeLen   number of characters(bytes) in the PIN,
                        undefined if the first parameter value is FALSE

    @note The Security Manager does not store PINs. If a PIN is passed to the 
    Security Manager before the PIN is requested, the unsolicited PIN is simply 
    ignored.
 */
extern void OI_SECMGR_PinCode(OI_BOOL successful,
                              OI_BD_ADDR *pAddr,
                              OI_PIN_CODE *pPinCode,
                              OI_UINT8 pinCodeLen);

/**
    After the higher program layer process has determined whether or not a remote
    device is authorized to access a service, it calls this function to complete 
    the authorization process.
  
    @param pAddr        pointer to the Bluetooth device address of the device for 
                        which authorization is requested

    @param uuid         UUID of the service for which authorization was requested.

    @param authorized   TRUE if the remote device is authorized, false otherwise
 */

extern void OI_SECMGR_Authorize(OI_BD_ADDR *pAddr,
                                OI_UINT32  uuid,
                                OI_BOOL    authorized);

/*************************************************************

    Configure Security Manager

*************************************************************/
/**
    
    This function configures the Security Manager and thereby sets the overall security mode.
    Service-level security policies (security mode 2) are established by the services themselves
    when creating/accepting connections.

    This function takes a number of parameters, some of which are applicable only to specific
    security modes.

        - Security mode 1: All parameters other than secMode are ignored.
        - Security mode 2: All parameters are required to be valid.
        - Security mode 3: Service database and device database parameters are ignored.

    @param cb                      pointer to the callback function to be called upon 
                                   completion of the configure operation

    @param secMode                 dictates the security mode
    
    @param mode3SecPolicy          link-level security policy for security mode 3. Parameter is ignored
                                   in security modes 1 and 2.

    @param linkKeyStoragePolicy    This parameter dictates where newly generated link 
                                   keys are to be stored.

    @param pHlApi                  This is a pointer to a structure that specifies the 
                                   callback functions available for the Security Manager
                                   to call higher program layers.

    @return                        configuration request status
                                   - OI_OK: The configuration request has been accepted 
                                   and the configuration is in progress. The application
                                   callback function will be called when configuration has
                                   completed.
                                   - any other value: The configuration request has been 
                                   rejected. The returned status value indicates the reason 
                                   for rejecting the request. The application callback function
                                   will not be called.     

*/
OI_STATUS OI_SECMGR_Configure(OI_SECMGR_CONFIGURE_CALLBACK      cb,
                              OI_SECURITY_MODE                  secMode,
                              OI_SECURITY_POLICY                mode3SecPolicy,
                              OI_SECMGR_LINKKEY_STORAGE_POLICY  linkKeyStoragePolicy,
                              const OI_SECMGR_HL_FUNCTION_API   *pHlApi);

/*************************************************************

    bonding with devices

*************************************************************/

/**
 This function initiates bonding with a specified device. No ACL connections
 will be accepted during bonding, and no local connection requests will serviced.

    @param cb                          pointer to the callback function to be called upon
                                       completion of the bonding operation

    @param addr                        pointer to the address of the device

*/
OI_STATUS OI_SECMGR_BondWithDevice(OI_SECMGR_BOND_WITH_DEVICE_CALLBACK cb,
                                   OI_BD_ADDR *addr);


/*****************************************************************************/
#endif /* _OI_SECMGR_H */
