#ifndef _OI_BT_STACK_CONFIG_H
#define _OI_BT_STACK_CONFIG_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bt_stack_config.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 * This file provides type definitions for configuration of data structures for various subsystems 
 * of the BLUEmagic 3.0 protocol stack, used for configuring memory usage
 *
 * The comments in this file contain details on recommended and required usage of the
 * parameters defined herein. The values of these parameters may be changed in the file 
 * oi_bt_stack_config_default.c.
 * 
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_time.h"
#include "oi_bt_spec.h"
#include "oi_secmgr.h"
#include "oi_connect_policy.h"
#include "oi_sdpdb.h"

/**************************************************
    Common Configuration
**************************************************/
/** 
    A constant structure of this type is used to configure parameters used by several 
    modules of the BLUEmagic 3.0 Bluetooth protocol stack. 
 */
typedef struct  {
    /** 
     * This parameter indicates whether or not the application uses SCO data. If the 
     * application does not use SCO, the stack does not need to allocate memory for SCO data buffers.
     */
    OI_BOOL     scoSupported ;      /**< This parameter indicates whether or not the application uses SCO data connections. 
                                         A value of FALSE indicates that SCO will not be used. If the application does not use SCO, 
                                         the protocol stack does not need to allocate memory for SCO data buffers. */

    OI_UINT8    maxAclConnections ; /**< This parameter indicates how many concurrent 
                                         ACL connections the protocol stack should support. 
                                         This value is used to size control data structures. */

    OI_UINT8    maxScoConnections ; /**< This parameter indicates how many concurrent 
                                         SCO connections the protocol stack should support. 
                                         This value is used to size  control data structures. */

    OI_UINT8    deviceNameMaxLen ;  /**< The Bluetooth v1.1 specification allows device names
                                         to be up to 248 characters. This definition allows the application to 
                                         limit the internal device name to something less than 248 bytes.
                                         Length is specified as a number of bytes and includes the 
                                         NULL terminator. */

    OI_UINT16 maxMTU;               /**< This parameter configures the maximum L2CAP MTU that will be supported. */

}   OI_CONFIG_COMMON_CONFIG ;

/** The common system configuration is global, accessible to all modules at all times. */
extern const OI_CONFIG_COMMON_CONFIG oi_default_config_COMMON_CONFIG ;

/**************************************************
    Dispatcher Configuration
**************************************************/

/** A structure of this type configures the size of the table used by the CThru Dispatcher
 *  to store callback function registration entries. The parameter indicates the size of
 *  the table in 16-bit units, not number of entries. An entry consumes between 16 and 20 bytes.
 *  A reasonable parameter setting for many applications would be 64. A feasible minimum might 
 *  be between 16 and 32.
 */

typedef struct {
    OI_UINT16  DispatchTableSize;   /**< size of Dispatcher table */

} OI_CONFIG_DISPATCH;

extern const OI_CONFIG_DISPATCH oi_default_config_DISPATCH ;

/*****************************************************
 * Memory Manager configuration parameters
 *****************************************************/
/**
    Structures of this type are used as elements in an array that indicates how the 
    Memory Manager will partition the portion of the heap from which it dynamically allocates memory. 
    A memory pool is a linked list of memory blocks of the same size. The pool table is an array of
    memory pools.
    
    Each OI_MEMMGR_POOL_CONFIG structure configures one memory pool: the first field indicates the number 
    of blocks that will be allotted in a pool; the second field indicates the size of the blocks in this pool.  
    For example, the entry {100, 64} indicates that a pool of 100 64-byte blocks will be set aside.
    The block sizes must be multiples of 4.
 */
typedef struct {
    OI_UINT16 numBlocks;                /**< number of blocks in this pool */
    OI_UINT32 blockSize;                /**< size of each block in this pool */
} OI_MEMMGR_POOL_CONFIG;


/** 
 *  All memory allocated by the Memory Manager for a particular application comes from a single heap. This includes 
 *  statically allocated memory and the memory pools. A structure of this type configures 
 * the heap, setting a pointer to the heap and establishing the heap size. The structure is 
 *  eventually sent to OI_MEMMGR_Init() by way of OI_CONFIG_MEMMGR.
 */
typedef struct  {
    void        *heap ;         /**< pointer to the heap */
    OI_UINT32   heapSize ;      /**< size of the heap in bytes */
} OI_MEMMGR_HEAP_CONFIG ;

/** A single structure of this type is used for passing all memory configuration 
 *  information to OI_MEMMGR_Init(). 
 */
typedef struct {
    const OI_MEMMGR_POOL_CONFIG  *poolConfig ;       /**< pointer to zero-terminated list of pool-configuration pairs */
    const OI_MEMMGR_HEAP_CONFIG  *heapConfig ;       /**< pointer to heap configuration information */
} OI_CONFIG_MEMMGR ;

extern const OI_CONFIG_MEMMGR oi_default_config_MEMMGR ;

/*****************************************************
 * HCI configuration parameters
 *****************************************************/

/* A structure of this type will be used to configure HCI.  More detailed uasge information is available 
    in oi_bt_stack_config_default.c.
 */
typedef struct  {

    OI_UINT16 transportMaxRxBufSize ;            /**< This is used to size the transport layer (BCSP, 3-wire, H4) receive packet buffer size. The 
                                                      transport layer assembles complete ACL and SCO packets before passing them up to the HCI layer. 
                                                      During initialization, the HCI layer will configure the Bluetooth device such that packets sent 
                                                      up from the device to the host will never exceed this size. A typical value for this parameter
                                                      would be 339, corresponding to the largest baseband packet size.
                                                      @note This parameter defines the size of the largest packet excluding the packet header. The 
                                                      transport layer must size its buffers to this parameter's value plus the size of the largest 
                                                      packet header. An ACL packet has a header that is 4 bytes long. */ 

    OI_UINT8  maxPendingCommands ;               /**< This sets the maximum number of HCI commands that can be sent to the host 
                                                      controller before the 'command complete' event or 'command status' return value has been received.
                                                      This governs the size of the HCI module's command queue and the complete/status 
                                                      callback list. A value in the range of 4-8 ought to be enough for most simple applications.*/

    OI_UINT8  cmdCompleteTimeoutSecs ;           /**< This sets the timeout value for HCI commands that receive a 'command complete'
                                                      callback. This timeout value is specified as a number of seconds. 
                                                      A typical value might be 2-5 seconds.*/
    
    OI_UINT8  cmdStatusTimeoutSecs ;             /**< This sets the timeout value for the receipt of the status return value for 
                                                      HCI commands that return 'command status'. This timeout value is specified as a number of seconds.
                                                      A typical time might be 2-5 seconds. Such commands will also have a separate 
                                                      cmdCompleteEventTimeoutSecs timeout set for the receipt of the callback function that indicates 
                                                      that the asynchronous event resulting from the command has completed. */

    OI_UINT8  cmdCompleteEventTimeoutSecs ;      /**< This sets the timeout value for the receipt of the event that signals the completion
                                                      of the action initiated by an HCI command that returns 'command status'. This timeout value is
                                                      specified as a number of seconds. This timeout value will normally be much longer than that used
                                                      for the return of a 'command complete' callback function. A typical value might be 10-90 seconds. 
                                                      This timeout must be longer than the page timout specified. (Refer to HCI section 4.7.16 for page 
                                                      timeout information or in @ref OI_DEVMGR_STATIC_SETTINGS to see how page timeout is set by the Device Manager.)
                                                      @note This timeout is not used by the HCI-level inquiry command, since the inquiry procedure is
                                                      a special case with different, longer timeout requirements.*/

    OI_BOOL   hostAclFlowControlEnabled ;       /**<  Host flow control can be used to throttle the baseband's receiving of ACL packets. Some profiles cannot
                                                      consume ACL data as quickly as they can be transmitted by a remote device. Those profiles will need to
                                                      have host flow control enabled in order to use the HCI flow-control API functions. */
                                                      
    OI_BOOL   hostScoFlowControlEnabled ;       /**< Host flow control can be used to throttle the baseband's receiving of SCO packets. */
    
    
    OI_BOOL   initialResetCmdDisabled ;         /**< The HCI layer normally issues a reset command to the Bluetooth device as the first step in the initialization
                                                     sequence. If this variable is set to TRUE, then the initial reset command will not be sent. The most common 
                                                     reason for setting this variable to TRUE is to accommodate Bluetooth devices that do not properly implement the 
                                                     reset command. This variable is usually set to FALSE. 
                                                     @note: The reset command must not be disabled for H4 devices.  The H4 specification requires the
                                                     reset command in order to resynchronize communications. */

}   OI_CONFIG_HCI ;

extern const OI_CONFIG_HCI oi_default_config_HCI ;

/*****************************************************
 * Security Manager configuration
 *****************************************************/
/**
    A structure of this type is used to pass Security Manager configuration
    parameters to OI_SECMGR_Init(). This does not set a security policy. Setting
    a security policy is accomplished through a call to OI_SECMGR_Configure().
    See the @ref secmgr_api section for more information on setting a security
    policy. 
*/

typedef struct  {

    OI_INTERVAL policyEnforcementTimeout ;       /**<  This parameter defines how much time to allow for security policy
                                                       enforcement. This timeout should be rather long (multiple seconds) in
                                                       cases in which the Security Manager is configured for security mode 2 and user 
                                                       input is required for PIN input and/or device authorization. In other cases,
                                                       this timeout should be very short. */
    
    OI_UINT8    minPinCodeLength ;               /**< This parameter indicates the length in characters of the smallest
                                                      PIN (Bluetooth passkey) that will be used by the Security Manager.
                                                      The longer the PIN, the better the security. */
      
    OI_BOOL     unitKeyPairingSupported ;        /**< This parameter indicates whether pairing with a unit key is supported.
                                                      If not, then a new link key will not be stored unless it is a combination key. (H:1.5.2.23) */ 

} OI_CONFIG_SECMGR ;

extern const OI_CONFIG_SECMGR oi_default_config_SECMGR ;

/*****************************************************
 * Device Manager configuration
 *****************************************************/

/** A structure of this type is used by Device Manager initialization. */
typedef struct  {

    OI_UINT16   inquiryCacheSize ;                   /**< The Device Manager caches inquiry information for subsequent use in making a connection.
                                                          This parameter specifies the size (in number of inquiry records) of the cache used for inquiry information.
                                                          How large this cache should be depends on the needs of the specific application.
                                                          If the cache is too small, it may take longer to establish a connection to a
                                                          remote device. If the cache is too large, memory is wasted unnecessarily.  */

    OI_INTERVAL disconnectTimeout ;                 /**< This parameter indicates how long the Device Manager should wait after the L2CAP layer
                                                          has released all references to an HCI connection before calling HCI to terminate the
                                                          connection.  The value for disconnectTimeoutLocalInitiator should be relatively 
                                                          short: 0.2 seconds would be appropriate for many applications.  These timeouts 
                                                          are specified in @ref OI_INTERVAL units (tenths of a second).  If the timeout value
                                                          is zero, there will be no delay in disconnecting the ACL connection - the ACL link will
                                                          be disconnected immediately when all L2cap references are released. */

    OI_INTERVAL disconnectTimeoutZombie ;           /**<  A "zombie" connection is an ACL connection with no associated L2CAP or SCO connections.
                                                          If zombie connections are allowed to persist, there exists the possibility of subsequent ACL
                                                          connection requests being refused. In addition to less malicious failures, this would open
                                                          a device to a denial-of-service attack in which many zombie connections could saturate the device's
                                                          capacity for ACL connections. To counter this, the Device Manager will disconnect an ACL connection
                                                          if the reference count for that ACL connection remains zero for longer than the time indicated
                                                          by this timeout value. This timeout should be long enough to allow authentication and authorization 
                                                          user input. 
                                                          A value of 0 means no zombie timeout, the zombie connection may persist forever.  The use of a 
                                                          zero timeout is provided primarily for testing purposes. */
    
    OI_UINT8    maxConcurrentConnectReqs ;           /**< This parameter indicates how many concurrent outstanding L2CAP connection requests  
                                                          are supported. For many simple applications, a value of 4 might be approriate. */

    OI_UINT8    maxCallbacksPerEvent ;              /**< The Device Manager fields certain asychronous HCI events (e.g., role change, mode change).
                                                          This parameter indicates the maximum number of callbacks that can be registered at one 
                                                          time for these events. For most simple applications, a value of 2 might be appropriate. */

    OI_UINT16   aclPacketTypes ;                    /**< This parameter indicates what types of packets are to be used for ACL connections.  This 
                                                          value must be in a specific format defined for HCI. Use the preprocessor values defined
                                                          in the file oi_hcispec.h; the various ACL packet types can be ORed together as shown by
                                                          @ref OI_DHCI_ALL_PKT_TYPES. */
                                                          
    OI_UINT16   powerSaveLinkPolicy ;               /**< This parameter defines the power saving link policy that will initially be applied to all
                                                         new ACL connections. The value must be in a specific format defined for HCI. Use the 
                                                         preprocessor values defined in the file oi_hcispec.h; the various enables are ORed together.
                                                         This initial link policy may change as connections to services are made and the service policies
                                                         are applied.
                                                            @ref OI_DHCI_LINKPOLICY_ENABLE_HOLD
                                                            @ref OI_DHCI_LINKPOLICY_ENABLE_SNIFF
                                                            @ref OI_DHCI_LINKPOLICY_ENABLE_PARK */
                                                             
    OI_UINT8    maxPiconetsSupported ;              /**< This parameter describes the maximum number of piconets that the local Bluetooth device 
                                                        can participate in.  The parameter determines whether or not a role switch is required
                                                        when a connection request is received.  If the local device can support an additional 
                                                        piconet, no role switch is required; otherwise, a role switch is required in order to
                                                        accept the new connection. */
} OI_CONFIG_DEVMGR ;
     
extern const OI_CONFIG_DEVMGR oi_default_config_DEVMGR ;

/*****************************************************
 *
 * L2CAP configuration parameters
 *
 *****************************************************/

/** A structure of this type is used to pass L2CAP configuration parameters to OI_L2CAP_Init(). */
typedef struct {
      
    OI_UINT16 maxPSMs;            /**< This parameter sets the maximum number of PSMs that can be registered and thereby restricts the number 
                                       of higher-layer protocols and services that can register with L2CAP to receive incoming connections. */

    OI_UINT16 maxChannels;        /**< This parameter sets the maximum number of L2CAP channels that can be in use at once. */

    OI_UINT16 connectionlessMTU;  /**< This parameter sets the MTU (maximum transmission unit) for connectionless traffic.
                                       The MTUs for inbound and outbound connection-based data are set on a per-connection basis in OI_L2CAP_CONNECT_PARAMS. 
                                       MTU parameter setting have a substantial impact on memory usage. */

    OI_INTERVAL RTXTimeout;       /**< This parameter sets the RTX (Response Timeout Expired) timeout in units of tenths of a second. */

    OI_INTERVAL ERTXTimeout;      /**< This definition sets the RTX (Extended Response Timeout Expired) timeout in units of tenths of a second.
                                       This value is required to be at least 60 seconds.   */
    
} OI_CONFIG_L2CAP;

extern const OI_CONFIG_L2CAP oi_default_config_L2CAP ;

/*****************************************************
 *
 * RFCOMM configuration parameters
 *
 *****************************************************/

/** A structure of this type is used to pass RFCOMM configuration parameters to OI_RFCOMM_Init(). */
typedef struct {

    OI_UINT8 maxSessions;               /**< This parameter set the maximum number of multiplexer sessions that can be maintained simultaneously. 
                                             Each multiplexer session requires a separate L2CAP channel ID, so the number set here should not be
                                             greater than maxChannels in OI_CONFIG_L2CAP. */

    OI_UINT8 maxLinksPerSession;        /**< This parameter sets the maximum number of links per session. */

    OI_UINT8 maxServers;                /**< This parameter sets the maximum number of server channels that can be registered with RFCOMM. */

    OI_UINT8 creditGrantThreshold;      /**< During the course of the connection, credits will not be returned to the remote device until the 
                                             number of remote credits has decreased to be equal to or less than this number. For example, if 
                                             the initial number of credits granted to a remote device is 20 and this parameter is 5, the remote 
                                             side will be able to send 15 frames before the local device sends 15 new credits to the remote device. */

    OI_INTERVAL frameTimeout;           /**< This parameter sets the duration of the timeout for RFCOMM frames. See section D.5.3 of the Bluetooth version 1.1 specification. */

    OI_INTERVAL extendedFrameTimeout;   /**< This parameter sets the duration of the timeout for RFCOMM frames when setting up the initial L2CAP connection. See section D.5.3 of the Bluetooth version 1.1 specification. */

    OI_INTERVAL commandTimeout;         /**< This parameter sets the duration of the timeout for multiplexer commands. See section D.5.3 of the Bluetooth version 1.1 specification. */

} OI_CONFIG_RFCOMM;

extern const OI_CONFIG_RFCOMM oi_default_config_RFCOMM ;


/*****************************************************
 *
 * SDP configuration parameters
 *
 *****************************************************/


/** A structure of this type is used to pass SDP server configuration parameters to OI_SDPServer_Init(). */
typedef struct {
    /** This parameter specifies the maximum number of clients allowed to
     * simultaneously connect to this SDP server. Most embedded devices will
     * want to use a small number, such as 1, to control resource use. Set this
     * parameter to zero if the server will not accept service requests.
     */
    OI_UINT8 MaxClientConnections;
    /** This parameter specifies the maximum number of bytes the SDP server will
     * send in response to any SDP request. This governs the size of the output
     * buffer the SDP server must allocate. The Bluetooth version 1.1
     * specification requires the SDP server to be able to return at least 7
     * bytes of attribute data. Many applications will be able to return
     * considerably more.
     */
    OI_UINT16 MaxResponseLen;
    /** This parameter specifies the maximum size (in number of bytes) of
     * request PDU that the SDP server will accept. Each active connection will
     * require an input buffer of this size for the duration of the connection.
     * This value must be at least 32 bytes to allow for the largest minimal
     * request. 
     */
    OI_UINT16 MaxRequestLen;
    /**
     * This parameter specifies the maximum number of service records
     * simultaneously registered in the SDP database. This governs the space
     * allocated for storing service record handles, not the service records
     * themselves. Memory for the actual service records must be allocated by
     * the application.
     */
    OI_UINT16 SDPDBMaxServiceRecords;

} OI_CONFIG_SDP_SRV;

extern const OI_CONFIG_SDP_SRV oi_default_config_SDP_SRV ;

/**
 * The language id list (attribute OI_ATTRID_LanguageBaseAttributeIDList) is
 * common to all service records. It will usually be statically initialized but
 * allow dynamic selection of the primary language it can also be set at
 * runtime.
 */
extern OI_SDPDB_ATTRIBUTE OI_SDP_LanguageList;

/**************************************************
    Policy Manager Configuration
**************************************************/

typedef struct {
    /**
     * This parameter specifies the maximum number of service connections that
     * can exist at any one time.  This parameter is used to size memory used to 
     * store connection policy records.  A connection in this context is any connection to a 
     * service which is being carried by a lower-level multiplexing protocols.  Generally,
     * this means services that use rfcomm or l2cap.
     */
    OI_UINT16   maxConcurrentConnections ;
    
    /**
     * This parameter defines the connection policy that will be applied to connections
     * which have no explicit policy defined.  Note that the default securityPolicy must
     * be consistent with security mode setting (1/2/3).
     *  Security mode 1: default security policy must be OI_SEC_REQUIRE_NOTHING
     *  Security mode 3: default security policy should be OI_SEC_REQUIRE_NOTHING
     *  Security mode 2: any default security policy is valid.  The SIG Security White Paper on Security
     *      Architecture recommends that the default security policy require authorization.
     */     
    OI_CONNECT_POLICY   defaultConnectPolicy ;

} OI_CONFIG_POLICYMGR ;

extern const OI_CONFIG_POLICYMGR oi_default_config_POLICYMGR ;

/**************************************************
    Security Database Configuration
**************************************************/

/** An array of elements of this type defines service security policies .
    The array is terminated by an entry whose
    service UUID is OI_UUID_NULL.  Services not listed will have the default security
    (@ref OI_CONFIG_POLICYMGR) applied .*/

typedef struct {

    /** The UUID of the service, OI_UUID_NULL terminates the list */
    OI_UUID32           serviceUuid32 ;     
    
    /** Security policy for the connection. **/
    OI_SECURITY_POLICY  securityPolicy ;
    

} OI_CONFIG_SECURITYDB ;

extern const OI_CONFIG_SECURITYDB oi_default_config_SECURITYDB[] ;

/*****************************************************************************/
#endif  /* _OI_BT_STACK_CONFIG_H */
