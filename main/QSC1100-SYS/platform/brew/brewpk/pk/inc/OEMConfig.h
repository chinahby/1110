#if !defined(OEMCONFIG_H)
#define OEMCONFIG_H  1
/*======================================================
FILE:  OEMConfig.h

SERVICES:  OEM Configuration Interface Function

GENERAL DESCRIPTION:

This file contains prototypes and definitions for
access to OEM configuration interfaces and structures.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEConfig.h"

#if defined(__cplusplus)
extern "C" {
#endif

//*******************************************************************************
//
// Static Functions - These functions can be called from statically linked code.
//
//*******************************************************************************

AEE_EXPORTS extern int  OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize);
extern int              OEM_SetConfig(AEEConfigItem i, void * pBuff, int nSize);
extern void             OEM_GetDeviceInfo(AEEDeviceInfo * pi);
extern int              OEM_GetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int *nSize);
extern int              OEM_SetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int nSize);
extern const char *     OEM_GetPath(uint16 wType);
extern boolean          OEM_GetItemStyle(AEEItemType t, AEEItemStyle * pNormal, AEEItemStyle * pSel);

#if defined(__cplusplus)
}
#endif


/*=======================================================================
  OEM  DOCUMENTATION
=======================================================================

Function: OEM_GetConfig()


Description:
   This function retrieves handset configuration information.

Prototype:
   int OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize);

Parameters:
   AEEConfigItem i: item that needs to be retrieved. It can be one of the
   following values:

===pre>
      - CFGI_DNS_IP1: 32-bit main Domain Name Server (DNS) IP Addresses in
        network byte-order
      - CFGI_DNS_IP2: 32-bit backup Domain Name Server (DNS) IP Address in
        network byte-order
      - CFGI_DOWNLOAD: information pertinent to the download service with the
        following information:
        + dwCarrierID: 32-bit carrier ID
        + dwPlatformID: 32-bit handset platform ID
        + bBKey: string of OEM programmed B-Key or all zeros
        + bAKey: string of SSD_A derived from A-Key or all zeros
        + szServer: string of server name
        + wFlags: 16-bit download related flags which can have one or more of the following
          values set:
          * DIF_USE_A_KEY: if set, use A-Key; otherwise use B-Key
          * DIF_TEST_ALLOWED: if set, the handset can be used to test local apps
          * DIF_MIN_FOR_SID: if set, use the MIN for the SID
          * DIF_PREPAY: if set, it is a prepay phone
          * DIF_NO_AUTO_ACK: if set, do not force ACKs until user runs MobileShop
          * DIF_SID_ENCODE: Uses SID rather than ESN for app encoding
          * DIF_SID_VALIDATE_ALL: Validates all apps rather than just SSN apps
          * DIF_RUIM_DEL_OVERRIDE: Allows one RUIM user to delete apps owned by another
          * DIF_AUTO_UPGRADE: Perform automatic upgrade when new uses purchased
          * DIF_NO_LAUNCH_MOD_ACK_DISABLED: If set, do not start an app with flag MOD_ACK_DISABLED set
          * DIF_SENSITIVE_SID: If set, do not send SID in app download and delete ACK url.
        + nAuth: download authentication policy which can be one of the following values:
          * APOLICY_NONE: No authentication required
          * APOLICY_SID: User's "subscriber ID" is passed to ADS before any set of transactions started
          * APOLICY_TEXT: User should be prompted for text "key" and this sent to ADS
          * APOLICY_NUM- User should be prompted for numeric "key" and this sent to ADS
        + nPolicy: privacy policy which determines the type of certification required
          to run applet on the handset:
          * PPOLICY_BREW: TRUE-BREW-signed applet only
          * PPOLICY_CARRIER: carrier-signed applet only
          * PPOLICY_BREW_AND_CARRIER: TRUE-BREW- and carrier-signed applet only
          * PPOLICY_BREW_OR_CARRIER: TRUE-BREW- or carrier-signed applet
      - CFGI_SUBSCRIBERID: 32-byte subscriber ID in ASCII
      - CFGI_MOBILEINFO: information about the handset which include the following:
        + nCurrNAM: 8-bit handset's NAM
        + dwESN: 32-bit handset's ESN
        + szMobileID: 16-byte mobile number which consists of the following components:
          * mcc: 3-digit mobile country code
          * mnc: 2-digit mobile network code
          * min2: 3-digit mobile area code
          * min1: 7-digit mobile phone number
      - CFGI_AUTOSTART: class ID of the applet to be auto-started when AEE is initialized
        through AEE_Init().
      - CFGI_BUSY_CURSOR_OFFSET: offset position of the hourglass from the center of the
        screen. It is of the type AEERect which has the following fields:
        + x: x-offset from the center of the screen
        + y: y-offset from the center of the screen
        + dx: ignored
        + dy: ignored
      - CFGI_DOWNLOAD_BUFFER:32-bit unsigned integer value for the download buffer size, in bytes.
        The default is 10 kilobytes.
      - CFGI_DOWNLOAD_CACHE:32-bit unsigned integer value for the download buffer cache, in bytes.  Used to configure the maximum download page size that can be cached.
        The default is 1kilobytes and maximum is 10 kilobytes.
      - CFGI_HTTP_BUFFER: 32-bit unsigned integer value for the HTTP read buffer size, in bytes.
        The default is 4 kilobytes.
      - CFGI_NET_CONNTIMEOUT: 32-bit unsigned integer value for the network connection timeout,
        in milliseconds. The default is 30 seconds.
      - CFGI_SUBSCRIBERID_LEN: 32-bit signed integer value for size in bytes of subscriber ID
        The default length is 32.
      - CFGI_MAX_DISPATCH_TIME: 32-bit unsigned integer value for the maximum time BREW should
        spend in the dispatcher before relinquishing control. The default is 250 msecs.
      - CFGI_MIN_IDLE_TIME: 32-bit unsigned integer value for the minimum time BREW must relinquish
        from dispatcher. The default is 35 msecs.
      - CFGI_SLEEP_TIMER_RESOLUTION: 32-bit unsigned integer value for timer resolution during when
        processor/os is in SLEEP mode. The default is 1.2 seconds.
      - CFGI_CLIPBOARD_SIZE: 32-bit unsigned integer value for size in bytes of pre-allocated
        low-memory area for clipboard. The default is 2K bytes.
      - CFGI_SYSMEM_SIZE: 32-bit unsigned integer value for size in bytes reserved to the system
        in low-memory. The default is 2K bytes.
      - CFGI_DOWNLOAD_FS_INFO: DLItemSize structure, only the dwFSAvail and dwFSSize will be used. If this
        is not supported by the OEM, BREW will try to calculate available space with ISHELL_GetDeviceInfoEx().
      - CFGI_DOWNLOAD_FS_INFOEX: 64 bit version of CFGI_DOWNLOAD_FS_INFO for devices with efs > 4GB
        AEEFSFreeSpace structure defined in AEEShell.h. If OEMs want to reserve more than 4GB of FS space
        for BREW they must implement this item. BREW will first query this item to calculate available space 
        and if it is not supported,BREW will query 32 bit CFGI_DOWNLOAD_FS_INFO config item. 
      - CFGI_SCREEN_SAVER: AEEScreenSaverInfo structure, this is used to configure screensaver settings.
        Default values are: wSecs:30seconds, wSecsMin:5seconds, wSecsMax:300seconds.
      - CFGI_CLOSE_KEYS: OEMCloseKeys structure, the default values will be obtained from AEEDeviceInfo
        otherwise will be set to the following defaults:   wCloseApp:AVK_CLR, evtCloseApp:EVT_KEY, wCloseAllApps:AVK_END,
        evtCloseAllApps:EVT_KEY. The OEMCloseKeys structure will be filled before calling OEM_GetConfig, so you can check
        the defaults and change if necessary.
      - CFGI_FILE_CACHE_INFO: OEMFileCacheInfo structure, used to determine file caching levels.
        The default values are nCacheDefault:4k, nCacheMin:-1,nCacheMax:-1
      - CFGI_PRELOAD_COUNT: OEMs must specify the count of the PreLoaded apps
        expected on this device. The parameter is an int * that must be filled with the
        exact count on return. On a test-enabled device, this value must be set to 0.
        On a Non test-enabled device,this value must exactly match the number of PreLoaded apps
        expected to be shipped on the device. If the counts do not match, all of the PreLoaded
        apps on the device will be deleted when the handset is powered on. The way to
        preload apps on the device is to the following: 1. Set this count to the exact
        value 2. Power Cycle the Device. 3. Preload the exact number of apps on the
        device using AppLoader or EFS-Explorer like device. 4. Power Cycle the Phone.
      - CFGI_MAX_DEMO_ITEMS: Count of demo apps/items supported on the handset.  This
        number is used to store a list of previously downloaded demo items.  The default
        size of the list is 20 entries.  This means that at least 19 other demos must
        be downloaded before the same demo can be downloaded again.
      - CFGI_GPSONE_TRANSPORT: uint32 OEM GPS transport method (IP, Data burst)
      - CFGI_GPSONE_SVRIP: uint32 GPS server IP address
      - CFGI_GPSONE_SVRPORT: uint32 GPS server IP port
      - CFGI_GPSONE_LOCK: GPS lock
      - CFGI_MODULE_FSLIMIT: MIFFSLimit structure to indicate Maximum File usage and Maximum Space usage
        for the module. Default is unlimited space usage.
      - CFGI_CARDID_LEN: Length of the card ID of the RUIM/USIM card (if any) on the device.
      - CFGI_CARDID: The Card ID of the RUIM/USIM card (if any) on the device.
        For RUIM cards this will be the contents of the Removable UIMID record (6F31)
        The first byte in the UIMID record will not be present in 
        CFGI_CARDID as it represents the length of the record. The rest of the 
        record will be copied in as stored on the RUIM (little-endian).
      - CFGI_DEBUG_KEY: OEMDebugKey structure. Debug key sequence
      - CFGI_ALLOW_3G_2G_FAILOVER: boolean, if TRUE, allow failover. 
      - CFGI_ISTATIC_SCROLL: 32-bit unsigned integer value for the auto-scrolling speed of each line
        of text in the IStatic control, in milliseconds. The default is 1 second per visible line.
        This is useful for reducing flickering that may happen when the default value is too low/high.
      - CFGI_APPX5_MPC_ADDR: where the MPC specified in CU Appx5 lives (IP addr in network-byte order)
      - CFGI_APPX5_MPC_PORT: where the MPC specified in CU Appx5 lives (IP port in network-byte order)
      - CFGI_MAX_DEMO_ITEMS: Count of demo apps/items supported on the handset.  This
        number is used to store a list of previously downloaded demo items.  The default
        size of the list is 20 entries.  This means that at least 19 other demos must
        be downloaded before the same demo can be downloaded again.
      - CFGI_WEB_IDLECONNTIMEOUT: default IWeb IDLECONNTIMEOUT, in seconds, used if unspecified by application (see AEEWeb.h)
      - CFGI_APP_KEY_1: OEMConfigData structure for a secret to be used by applications to authenticate
        or sign content.  This is sensitive key material, and so should not be stored in
        predictable locations (i.e. file system or fixed memory locations).
      - CFGI_GSM1X_PRL: PRL access
      - CFGI_GSM1X_RTRE_CONFIG: RTRE_CONFIG variable access
      - CFGI_GSM1X_IDENTITY_PARAMS: Writing IMSI, ACCOLC and MSISDN to GSM1x assigned NAM
      - CFGI_GSM1X_SID_NID_PARAMS: Read/write home or locked SID/NID stored in NV.
      - CFGI_OFFLINE_PPP_TIMEOUT: time in milliseconds! to wait in offline state before closing PPP
      - CFGI_CACHED_RESOURCES: Number of resource files to cache in the BREW resource file layer manages.
        The default value for this number is 5.  However, on devices with more RAM and file handles, increasing
        this value may greatly improve the usability of the device.
      - CFGI_CARDID: The Card ID of the RUIM/USIM card (if any) on the device.
        For RUIM cards this will be the contents of the Removable UIMID record (6F31)
        The first byte in the of the UIMID record will not be present in 
        CFGI_CARDID as the represents the length of the record. The rest of the 
        record will be copied in as stored on the RUIM (little-endian).
      - CFGI_CARDID_LEN: Length of the card ID of the RUIM/USIM card (if any) on the device.
      - CFGI_SAFEMODE_STARTMODE: Starting mode for the handset (Normal, Probation, or safe)
      - CFGI_SAFEMODE_TIMER: Time (in msecs) to wait before restoring device to normal mode
      - CFGI_AUTOSTARTARGS: Auto start args
      - CFGI_KB_AUTOREPEAT: OEMAutoRepeat structure 
      - CFGI_MOBILE_DIR_NUM: The Mobile Directory Number (MDN) for this device. As per NANPA specification
        this number should be 10 digit number. It should be usable as is through BREW APIs to make a voice call.
      - CFGI_MOBILE_DIR_NUM_LEN: The length of the MDN string.
      - CFGI_FIRMWARE_ID: OEM specified (zero-terminated string) Firmware ID for this device.
      - CFGI_FIRMWARE_ID_LEN: Length of the Firmware ID string.
      - CFGI_MAX_ACK_ATTEMPTS: Max retries attempted to flush the preinstall/delete acks
      - CFGI_SMS_EMAIL_DEST_LENGTH: Length of destination referred by CFGI_SMS_EMAIL_DEST
      - CFGI_SMS_EMAIL_DEST: Destination to use if email needs to be sent as email address copied to payload 
      - CFGI_SMS_MAX_PAYLOAD_LENGTH: Maximum payload length in terms of number of bytes
      - CFGI_SMS_DEFAULT_MO_ENC: Default encoding for MO SMS
      - CFGI_SMS_MO_ENC_TYPES: Encoding types available for MO SMS
      - CFGI_SMS_MO_ENC_TYPES_COUNT: Count of encoding types available for MO SMS
      - CFGI_BREWDIR_SMS_TELESERVICE: Teleservice ID of BREW directed SMS
      - CFGI_SMS_MO_ON_ACCESS_CHANNEL: boolean, if TRUE, SMS message is sent on access channel
      - CFGI_SMS_MO_ON_TRAFFIC_CHANNEL: boolean, if TRUE, SMS message is sent on traffic channel. If both of 
        CFGI_SMS_MO_ON_ACCESS_CHANNEL and CFGI_SMS_MO_ON_TRAFFIC_CHANNEL are TRUE, SMS message is first sent 
        on access channel and on being too large for access channel is sent on traffic channel.
      - CFGI_DORMANCY_TIMEOUT_DEFAULT: 8-bit unsigned integer value (uint8) for the default 
        packet data inactivity timeout (which initiates dormancy), in seconds. 0 means inactivity timeout disabled.
        Default is 30 seconds.
      - CFGI_DOWNLOAD_CACHE:32-bit unsigned integer value for the download buffer cache, in bytes.  Used to configure 
        the maximum download page size that can be cached. The default is 1kilobytes and maximum is 10 kilobytes.
      - CFGI_LNG: Device Language Code
      - CFGI_MANNER_MODE: Set device manner mode. pSrc = AEE_MANNER_MODE_XXX, nSize = sizeof(uint32)
      - CFGI_APPX5_ON: User plane position determination ON (boolean)
      - CFGI_GPSONE_LOCATION_SERVER_NAME1: Fully qualified name of location server. If provided will
        be resolved and used by position determination process when application choses to use the default
        server.
      - CFGI_GPSONE_LOCATION_SERVER_NAME2: Fully qualified name of location server. If provided will
        be used by position determination process when CFGI_GPSONE_LOCATION_SERVER_NAME1 cannot be resolved.
      - CFGI_SMS_GW_SCADDRESS_LENGTH: length of default SC address for GW messages.
      - CFGI_SMS_GW_SCADDRESS: default SC address for GW messages. Gives a hook to OEM for providing service 
      - CFGI_SMS_GW_DOMAIN_PREF: center address for messages for which its not specified by the application. If 
        both of the application and OEM don't specify SC address, message is still provided to service layer 
        for processing.
      - CFGI_BACKLIGHT_CLASSIDS_LIST: 0 terminated list of class IDs implementing IBacklight interface
      - CFGI_RESERVED_1: 
      - CFGI_BREWMA_CAPABILITIES: BREWMA specific client capabilities. Bitflag Based.
      - CFGI_LAST_VALID_TIME: Last valid time stored in non-volatile storage. Updated with the secure clock's 
        value during successful synchronizations and during AEE_Init - if the secure clock is valid.
      - CFGI_BANNERNAME: AECHAR* - Banner text (null terminated) to be displayed in the idle screen
      - CFGI_BANNERNAME_LEN: 32 bit signed integer value. Banner Length in bytes
      - CFGI_THEMENAME: char* - Current themename copied onto the buffer (null terminated). The theme string is 
        a sequence of one or more (semi-colon ';' separated) filenames to the files making up the current theme.
      - CFGI_THEMENAME_LEN: 32 bit signed integer value. Theme name length in bytes. A size of 0 indicates to 
        the caller that the Default Theme is currently active.
      - CFGI_SUBLCD_PROPS: 32 bit unsigned integer value. Provides the visual properties of the sub-lcd of the device.  
        The data pointer holds a sequence of visual property key (uint32), value (uint32) pairs. The size field is the 
        number of bytes in the buffer. The property keys and their meaning and expected values are defined in 
        AEEWProperties.h in the UI Toolkit. 
      - CFGI_ANNUN_PROPS: 32 bit unsigned integer value. Provides the visual properties of the annunciator region 
        (also called the icon row) of the device.  The data pointer holds a sequence of visual property key 
        (uint32), value (uint32) pairs. The size field is the number of bytes in the buffer. The property keys 
        and their meaning and expected values are defined in AEEWProperties.h in the UI Toolkit.                                            //      - PROP_FGCOLOR
      - CFGI_IPV6_DNS_IP1: INAddr6, primary IPv6 Domain Name Server (DNS) in network byte-order
      - CFGI_IPV6_DNS_IP2: INAddr6, secondary IPv6 Domain Name Server (DNS) in network byte-order
      - CFGI_LAST_VALID_TIME: LastValidCfg*, Last valid secure time information stored in non-volatile storage. 
        The information contains the last valid time in microseconds and the last setter class ID of that time. Updated 
        with the secure clock's value during successful synchronizations and during AEE_Init - if the secure clock is 
        valid. This config item is part of the Trusted Time API for UMTS devices.
        + LastValidCfg struct
           * uint64 qwTime: Last valid secure time in microseconds
           * AEECLSID OwnerCls: Last secure clock setter class ID
      - CFGI_USER_CLOCK_OFFSET: int32* - User clock offset (in seconds) from a valid secure clock time stored in 
        non-volatile storage. If the secure clock is not valid the user clock offset represents the actual time in seconds
        from midnight Jan 6, 1980 GMT.
      - CFGI_REQUIREPINENTRY: boolean value. This config item would set TRUE or FALSE depending on whether PIN entry 
        is required to perform normal phone operations. If this is TRUE, typically emergency calls and other special
        functionality would be allowed, but not normal phone operations. Default: FALSE.
      - CFGI_DOWNLOAD_PDP_ID: 32 bit unsigned integer value to identify the PDP profile to be used
      - CFGI_DOWNLOAD_PROXY: char * - Proxy server to be used for the BREW Download Protocol (null terminated proxyspec)
      - CFGI_DOWNLOAD_PROXY_LEN: 32 bit signed integer value, Length of Proxy server to specified in CFGI_DOWNLOAD_PROXY
        A size of 0 indicates to the caller that there is no proxy server configured.
      - CFGI_DOWNLOAD_PROXYAUTH: char * - Proxy Server's credential (user:pass) to be used (null terminated "username:password") for the proxy server specified in CFGI_DOWNLOAD_PROXY
      - CFGI_DOWNLOAD_PROXYAUTH_LEN: 32 bit signed integer value, Length of web proxy's credential in bytes.
        A size of 0 indicates to the caller that there is no proxy auth.
      - CFGI_OEMAUTH_CHALLENGE_CAP: boolean * - if TRUE, OEM supports RUIM based challenge-response and if so
        OEMs must also implement CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN and OEMAuth_GetChallengeRespons
      - CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN: int * - max. length of RUIM's resposne to NN challenge
        This must be implemented if CFGI_OEMAUTH_CHALLENGE_CAP is TRUE.
      - CFGI_LAZY_ACK : boolean * - TRUE if BREW prevents flushing queued/pending ACKs at power up and attempts 
        to flush them when user initiates a data connection first time after device powers up.
        If FALSE: BREW will attempt to flush preisntall and queued ACKs on device power up.
        By default, this flag is turned ON in OEM porting kit implementation code. So if this feature is not
        desired this flag needs to be turned OFF by OEMs.
      - CFGI_THEMES_INI: char* - Current themes INI filename copied onto the buffer (null terminated).
      - CFGI_THEMES_INI_LEN: 32 bit signed integer value.  Themes INI filename length in bytes.
        A size of 0 indicates to the caller that there is no themes ini file configured.
      - CFGI_WALLPAPERS_INI: char* - Current wallpapers INI filename copied onto the buffer (null terminated).
      - CFGI_WALLPAPERS_INI_LEN: 32 bit signed integer value.  Wallpapers INI filename length in bytes.
        A size of 0 indicates to the caller that there is no wallpapers ini file configured.
      - CFGI_RINGTONES_INI: char* - Current ringtones INI filename copied onto the buffer (null terminated).
      - CFGI_RINGTONES_INI_LEN: 32 bit signed integer value.  Ringtones INI filename length in bytes.
        A size of 0 indicates to the caller that there is no ringtones ini file configured.

===/pre>
   void * pBuff: buffer in which the return values are to be stored
   int nSize: size of the buffer

Return Value:
   0 on Success
        EFAILED or EBADPARM if the buffer is not big enough to hold the data to be returned.

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_GetConfigItem()
==================================================================
Function: OEM_SetConfig()

Description:
   This function sets new handset configuration information.

Prototype:
   int OEM_SetConfig(AEEConfigItem i, void * pBuff, int nSize);

Parameters:
   AEEConfigItem i: item whose new value needs to be set. It can be one of the
   following values:

===pre>
      - CFGI_DNS_IP1: 32-bit main Domain Name Server (DNS) IP Addresses in
        network byte-order
      - CFGI_DNS_IP2: 32-bit backup Domain Name Server (DNS) IP Address in
        network byte-order
      - CFGI_DOWNLOAD: information pertinent to the download service with the
        following information:
        + dwCarrierID: 32-bit carrier ID
        + dwPlatformID: 32-bit handset platform ID
        + bBKey: string of OEM programmed B-Key or all zeros
        + bAKey: string of SSD_A derived from A-Key or all zeros
        + szServer: string of server name
        + wFlags: 16-bit download related flags which can have one or more of the following
          values set:
          * DIF_USE_A_KEY: if set, use A-Key; otherwise use B-Key
          * DIF_TEST_ALLOWED: if set, the handset can be used to test local apps
          * DIF_MIN_FOR_SID: if set, use the MIN for the SID
          * DIF_PREPAY: if set, it is a prepay phone
          * DIF_NO_AUTO_ACK: if set, do not force ACKs until user runs MobileShop
          * DIF_SID_ENCODE: Uses SID rather than ESN for app encoding
          * DIF_SID_VALIDATE_ALL: Validates all apps rather than just SSN apps
          * DIF_RUIM_DEL_OVERRIDE: Allows one RUIM user to delete apps owned by another
          * DIF_AUTO_UPGRADE: Perform automatic upgrade when new uses purchased
          * DIF_NO_LAUNCH_MOD_ACK_DISABLED: If set, do not start an app with flag MOD_ACK_DISABLED set
          * DIF_SENSITIVE_SID: If set, do not send SID in app download and delete ACK url.
        + nAuth: download authentication policy which can be one of the following values:
          * APOLICY_NONE: No authentication required
          * APOLICY_SID: User's "subscriber ID" is passed to ADS before any set of transactions started
          * APOLICY_TEXT: User should be prompted for text "key" and this sent to ADS
          * APOLICY_NUM- User should be prompted for numeric "key" and this sent to ADS
        + nPolicy: privacy policy which determines the type of certification required
          to run applet on the handset:
          * PPOLICY_BREW: TRUE-BREW-signed applet only
          * PPOLICY_CARRIER: carrier-signed applet only
          * PPOLICY_BREW_AND_CARRIER: TRUE-BREW- and carrier-signed applet only
          * PPOLICY_BREW_OR_CARRIER: TRUE-BREW- or carrier-signed applet
      - CFGI_SUBSCRIBERID: 32-byte subscriber ID in ASCII
      - CFGI_MOBILEINFO: information about the handset which include the following:
        + nCurrNAM: 8-bit handset's NAM
        + dwESN: 32-bit handset's ESN
        + szMobileID: 16-byte mobile number which consists of the following components:
          * mcc: 3-digit mobile country code
          * mnc: 2-digit mobile network code
          * min2: 3-digit mobile area code
          * min1: 7-digit mobile phone number
      - CFGI_AUTOSTART: class ID of the applet to be auto-started when AEE is initialized
        through AEE_Init().
      - CFGI_BUSY_CURSOR_OFFSET: offset position of the hourglass from the center of the
        screen. It is of the type AEERect which has the following fields:
        + x: x-offset from the center of the screen
        + y: y-offset from the center of the screen
        + dx: ignored
        + dy: ignored
      - CFGI_DOWNLOAD_BUFFER:32-bit unsigned integer value for the download buffer size, in bytes.
        The default is 10 kilobytes.
      - CFGI_HTTP_BUFFER: 32-bit unsigned integer value for the HTTP read buffer size, in bytes.
        The default is 4 kilobytes.
      - CFGI_NET_CONNTIMEOUT: 32-bit unsigned integer value for the network connection timeout,
        in milliseconds. The default is 30 seconds.
      - CFGI_SUBSCRIBERID_LEN: 32-bit signed integer value for size in bytes of subscriber ID
        The default length is 32.
      - CFGI_MAX_DISPATCH_TIME: 32-bit unsigned integer value for the maximum time BREW should
        spend in the dispatcher before relinquishing control. The default is 250 msecs.
      - CFGI_MIN_IDLE_TIME: 32-bit unsigned integer value for the minimum time BREW must relinquish
        from dispatcher. The default is 35 msecs.
      - CFGI_SLEEP_TIMER_RESOLUTION: 32-bit unsigned integer value for timer resolution during when
        processor/os is in SLEEP mode. The default is 1.2 seconds.
      - CFGI_CLIPBOARD_SIZE: 32-bit unsigned integer value for size in bytes of pre-allocated
        low-memory area for clipboard. The default is 2K bytes.
      - CFGI_SYSMEM_SIZE: 32-bit unsigned integer value for size in bytes reserved to the system
        in low-memory. The default is 2K bytes.
      - CFGI_DOWNLOAD_FS_INFO: DLItemSize structure, only the dwFSAvail and dwFSSize will be used. If this
        is not supported by the OEM, BREW will try to calculate available space with ISHELL_GetDeviceInfoEx().
      - CFGI_DOWNLOAD_FS_INFOEX: 64 bit version of CFGI_DOWNLOAD_FS_INFO for devices with efs > 4GB
        AEEFSFreeSpace structure defined in AEEShell.h. If OEMs want to reserve more than 4GB of FS space
        for BREW they must implement this item. BREW will first query this item to calculate available space 
        and if it is not supported,BREW will query 32 bit CFGI_DOWNLOAD_FS_INFO config item. 
      - CFGI_SCREEN_SAVER: AEEScreenSaverInfo structure, this is used to configure screensaver settings.
        Default values are: wSecs:30seconds, wSecsMin:5seconds, wSecsMax:300seconds.
      - CFGI_CLOSE_KEYS: OEMCloseKeys structure, the default values will be obtained from AEEDeviceInfo
        otherwise will be set to the following defaults:   wCloseApp:AVK_CLR, evtCloseApp:EVT_KEY, wCloseAllApps:AVK_END,
        evtCloseAllApps:EVT_KEY. The OEMCloseKeys structure will be filled before calling OEM_GetConfig, so you can check
        the defaults and change if necessary.
	   - CFGI_PRELOAD_COUNT: OEMs must store the count of the PreLoaded apps passed
        to this function and must return this exact same value whenever OEM_GetConfig()
        is invoked with CFGI_PRELOAD_COUNT. This value must be stored in a persistent
        manner so that the same value can be remembered and returned whenever OEM_GetConfig()
        is invoked (including after power-cycling the phone).
        Failure to store this value in a persistent manner and return the same value is
        a FATAL error and may cause all of the pre-loaded apps on the device to be deleted.
      - CFGI_FILE_CACHE_INFO: OEMFileCacheInfo structure, used to determine file caching levels.
        The default values are nCacheDefault:4k, nCacheMin:-1,nCacheMax:-1
      - CFGI_GPSONE_TRANSPORT: uint32 OEM GPS transport method (IP, Data burst)
      - CFGI_GPSONE_SVRIP: uint32 GPS server IP address
      - CFGI_GPSONE_SVRPORT: uint32 GPS server IP port
      - CFGI_GPSONE_LOCK: GPS lock
      - CFGI_MODULE_FSLIMIT: MIFFSLimit structure to indicate Maximum File usage and Maximum Space usage
        for the module. Default is unlimited space usage.
      - CFGI_CARDID_LEN: Length of the card ID of the RUIM/USIM card (if any) on the device.
      - CFGI_CARDID: The Card ID of the RUIM/USIM card (if any) on the device.
        For RUIM cards this will be the contents of the Removable UIMID record (6F31)
        The first byte in the of the UIMID record will not be present in 
        CFGI_CARDID as the represents the length of the record. The rest of the 
        record will be copied in as stored on the RUIM (little-endian).
      - CFGI_DEBUG_KEY: OEMDebugKey structure. Debug key sequence
      - CFGI_ALLOW_3G_2G_FAILOVER: boolean, if TRUE, allow failover. 
      - CFGI_ISTATIC_SCROLL: 32-bit unsigned integer value for the auto-scrolling speed of each line
        of text in the IStatic control, in milliseconds. The default is 1 second per visible line.
        This is useful for reducing flickering that may happen when the default value is too low/high.
      - CFGI_OFFLINE_PPP_TIMEOUT: 32-bit unsigned integer value for the offline PPP timeout, in
        milliseconds. If a PPP session is in progress when the call state goes offline, PPP will be
        closed after this timeout, if the phone call state remains offline.  The default is 60 seconds.
      - CFGI_KB_AUTOREPEAT: Pointer to OEMKBAutoRepeat structure to allow OEM to control keypad auto-repeat
        and associated repeat rates.  OEM can control both the delay prior to the first repeat
        (OEMKBAutoRepeat.dwStart) as well as the repeat rate (OEMKBAutoRepeat.dwRate).  If OEMKBAutoRepeat.dwStart
        is 0, auto-repeat is disabled.  If OEMKBAutoRepeat.dwRate is 0, the EVT_KEY event will repeat only
        a single time. OEM is queried for (OEMKBAutoRepeat.dwStart) and (OEMKBAutoRepeat.dwRate) every
        time these values are used and thus OEMs can vary these on basis of OEM need.
      - CFGI_APPX5_MPC_ADDR: where the MPC specified in CU Appx5 lives (IP addr in network-byte order)
      - CFGI_APPX5_MPC_PORT: where the MPC specified in CU Appx5 lives (IP port in network-byte order)
      - CFGI_MAX_DEMO_ITEMS: Count of demo apps/items supported on the handset.  This
        number is used to store a list of previously downloaded demo items.  The default
        size of the list is 20 entries.  This means that at least 19 other demos must
        be downloaded before the same demo can be downloaded again.
      - CFGI_WEB_IDLECONNTIMEOUT: default IWeb IDLECONNTIMEOUT, in seconds, used if unspecified by application (see AEEWeb.h)
      - CFGI_APP_KEY_1: OEMConfigData structure for a secret to be used by applications to authenticate
        or sign content.  This is sensitive key material, and so should not be stored in
        predictable locations (i.e. file system or fixed memory locations).
      - CFGI_GSM1X_PRL: PRL access
      - CFGI_GSM1X_RTRE_CONFIG: RTRE_CONFIG variable access
      - CFGI_GSM1X_IDENTITY_PARAMS: Writing IMSI, ACCOLC and MSISDN to GSM1x assigned NAM
      - CFGI_GSM1X_SID_NID_PARAMS: Read/write home or locked SID/NID stored in NV.
      - CFGI_OFFLINE_PPP_TIMEOUT: time in milliseconds! to wait in offline state before closing PPP
      - CFGI_CACHED_RESOURCES: Number of resource files to cache in the BREW resource file layer manages.
        The default value for this number is 5.  However, on devices with more RAM and file handles, increasing
        this value may greatly improve the usability of the device.
      - CFGI_SAFEMODE_STARTMODE: Starting mode for the handset (Normal, Probation, or safe)
      - CFGI_SAFEMODE_TIMER: Time (in msecs) to wait before restoring device to normal mode
      - CFGI_AUTOSTARTARGS: Auto start args
      - CFGI_KB_AUTOREPEAT: OEMAutoRepeat structure 
      - CFGI_MOBILE_DIR_NUM: The Mobile Directory Number (MDN) for this device. As per NANPA specification
        this number should be 10 digit number. It should be usable as is through BREW APIs to make a voice call.
      - CFGI_MOBILE_DIR_NUM_LEN: The length of the MDN string.
      - CFGI_FIRMWARE_ID: OEM specified Firmware (zero-terminated string) ID for this device.
      - CFGI_FIRMWARE_ID_LEN: Length of the Firmware ID string.
      - CFGI_MAX_ACK_ATTEMPTS: Max retries attempted to flush the preinstall/delete acks
      - CFGI_SMS_EMAIL_DEST_LENGTH: Length of destination referred by CFGI_SMS_EMAIL_DEST
      - CFGI_SMS_EMAIL_DEST: Destination to use if email needs to be sent as email address copied to payload
      - CFGI_SMS_MAX_PAYLOAD_LENGTH: Maximum payload length in terms of number of bytes
      - CFGI_SMS_DEFAULT_MO_ENC: Default encoding for MO SMS
      - CFGI_SMS_MO_ENC_TYPES: Encoding types available for MO SMS
      - CFGI_SMS_MO_ENC_TYPES_COUNT: Count of encoding types available for MO SMS
      - CFGI_BREWDIR_SMS_TELESERVICE: Teleservice ID of BREW directed SMS
      - CFGI_SMS_MO_ON_ACCESS_CHANNEL: boolean, if TRUE, SMS message is sent on access channel
      - CFGI_SMS_MO_ON_TRAFFIC_CHANNEL: boolean, if TRUE, SMS message is sent on traffic channel. If both of 
        CFGI_SMS_MO_ON_ACCESS_CHANNEL and CFGI_SMS_MO_ON_TRAFFIC_CHANNEL are TRUE, SMS message is first sent 
        on access channel and on being too large for access channel is sent on traffic channel.
      - CFGI_DORMANCY_TIMEOUT_DEFAULT: 8-bit unsigned integer value (uint8) for the default 
        packet data inactivity timeout (which initiates dormancy), in seconds. 0 means inactivity timeout disabled.
        Default is 30 seconds.
      - CFGI_MANNER_MODE: AEE_MANNER_MODE_XXX value (uint32) to set the manner mode of the device.
	  - CFGI_DOWNLOAD_CACHE:32-bit unsigned integer value for the download buffer cache, in bytes.  Used to configure 
        the maximum download page size that can be cached. The default is 1kilobytes and maximum is 10 kilobytes.
      - CFGI_LNG: Device Language Code
      - CFGI_MANNER_MODE: Set device manner mode. pSrc = AEE_MANNER_MODE_XXX, nSize = sizeof(uint32)
      - CFGI_APPX5_ON: User plane position determination ON (boolean)
      - CFGI_GPSONE_LOCATION_SERVER_NAME1: Fully qualified name of location server. If provided will
        be resolved and used by position determination process when application choses to use the default
        server.
      - CFGI_GPSONE_LOCATION_SERVER_NAME2: Fully qualified name of location server. If provided will
        be used by position determination process when CFGI_GPSONE_LOCATION_SERVER_NAME1 cannot be resolved.
      - CFGI_SMS_GW_SCADDRESS_LENGTH: length of default SC address for GW messages.
      - CFGI_SMS_GW_SCADDRESS: default SC address for GW messages. Gives a hook to OEM for providing service 
      - CFGI_SMS_GW_DOMAIN_PREF: center address for messages for which its not specified by the application. If 
        both of the application and OEM don't specify SC address, message is still provided to service layer 
        for processing.
      - CFGI_BACKLIGHT_CLASSIDS_LIST: 0 terminated list of class IDs implementing IBacklight interface
      - CFGI_RESERVED_1: 
      - CFGI_BREWMA_CAPABILITIES: BREWMA specific client capabilities. Bitflag Based.
      - CFGI_LAST_VALID_TIME: Last valid time stored in non-volatile storage. Updated with the secure clock's 
        value during successful synchronizations and during AEE_Init - if the secure clock is valid.
      - CFGI_BANNERNAME: AECHAR* - Banner text (null terminated) to be displayed in the idle screen
      - CFGI_BANNERNAME_LEN: 32 bit signed integer value. Banner Length in bytes
      - CFGI_THEMENAME: char* - Current themename copied onto the buffer (null terminated). The theme string is 
        a sequence of one or more (semi-colon ';' separated) filenames to the files making up the current theme.
      - CFGI_THEMENAME_LEN: 32 bit signed integer value. Theme name length in bytes. A size of 0 indicates to 
        the caller that the Default Theme is currently active.
      - CFGI_SUBLCD_PROPS: 32 bit unsigned integer value. Provides the visual properties of the sub-lcd of the device.  
        The data pointer holds a sequence of visual property key (uint32), value (uint32) pairs. The size field is the 
        number of bytes in the buffer. The property keys and their meaning and expected values are defined in 
        AEEWProperties.h in the UI Toolkit. 
      - CFGI_ANNUN_PROPS: 32 bit unsigned integer value. Provides the visual properties of the annunciator region 
        (also called the icon row) of the device.  The data pointer holds a sequence of visual property key 
        (uint32), value (uint32) pairs. The size field is the number of bytes in the buffer. The property keys 
        and their meaning and expected values are defined in AEEWProperties.h in the UI Toolkit.                                            //      - PROP_FGCOLOR
      - CFGI_LAST_VALID_TIME: LastValidCfg*, Last valid secure time information stored in non-volatile storage. 
        The information contains the last valid time in microseconds and the last setter class ID of that time. Updated 
        with the secure clock's value during successful synchronizations and during AEE_Init - if the secure clock is 
        valid. This config item is part of the Trusted Time API for UMTS devices.
        + LastValidCfg struct
           * uint64 qwTime: Last valid secure time in microseconds
           * AEECLSID OwnerCls: Last secure clock setter class ID
      - CFGI_USER_CLOCK_OFFSET: int32* - User clock offset (in seconds) from a valid secure clock time stored in 
        non-volatile storage. If the secure clock is not valid the user clock offset represents the actual time in seconds
        from midnight Jan 6, 1980 GMT.
      - CFGI_DOWNLOAD_PDP_ID: 32 bit unsigned integer value to identify the PDP profile to be used
      - CFGI_DOWNLOAD_PROXY: char * - Proxy server to be used for the BREW Download Protocol (null terminated proxyspec)
      - CFGI_DOWNLOAD_PROXY_LEN: 32 bit signed integer value, Length of Proxy server to specified in CFGI_DOWNLOAD_PROXY
           A size of 0 indicates to the caller that there is no proxy server configured.
      - CFGI_DOWNLOAD_PROXYAUTH: char * - Proxy Server's credential (user:pass) to be used (null terminated "username:password") for the proxy server specified in CFGI_DOWNLOAD_PROXY
      - CFGI_DOWNLOAD_PROXYAUTH_LEN: 32 bit signed integer value, Length of web proxy's credential in bytes.
           A size of 0 indicates to the caller that there is no proxy auth.
===/pre>
   void * pBuff: buffer in which the new values are stored
   int nSize: size of the buffer

Return Value:
   0 on Success

Comments:
   None

Side Effects:
   None

See Also:
   Function: OEM_GetConfig()

==================================================================
Function: OEM_GetDeviceInfo()

Description:
   This function retrieves current handset's physical and hardware
characteristics.

Prototype:
   void OEM_GetDeviceInfo(AEEDeviceInfo * pi);

Parameters:
   AEEDeviceInfo * pi: retrieved buffer where AEEDeviceInfo
   (refer BREW SDK API Reference Guide) is stored.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: OEM_GetDeviceInfoEx()

Description:
   This function is used to get specific information about the device. It takes
   an ID that specifies what information is needed. The buffer contains the corresponding
   information on return.

Prototype:

   int   OEM_GetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int *pnSize);

Parameters:
   nItem: Specifies the Item whose info is needed. See documentation on
   AEEDeviceItem for the supported device items.
   pBuff: Contains the corresponding information on return.
   pnSize: Contains the size of pBuff. On return, it contains the size filled. This parameter may
   be NULL for certain device Items.

Return Value:
   SUCCESS: if successful
   EBADPARM: if bad parameters are passed in
   EUNSUPPORTED: if this item is not supported

Comments:
   None

Side Effects:
   None

See Also:
   AEEDeviceItem
   ISHELL_GetDeviceInfoEx()

=======================================================================
Function: OEM_GetPath()

Description:
   This function is called by the AEE to determine the path for the
directory names used by BREW. BREW uses six directories:
   - APPS: the directory where all the MIFs and
            applet directories are stored
   - SHARED: where all the shared files are
             stored
   - RINGER: where all the ringer files are
             stored
   - ADDRBOOK: where all of the address book
               files are stored
   - MIF: where all of the MIFs are stored
   - LOGO: where all of the LOGO files are stored

The path must not have a trailing directory separation character.

Prototype:

   const char * OEM_GetPath(uint16 wType)

Parameters:
   uint16 wType: any of the six directories used by the AEE: APPS, ADDRBOOK, MIF,
                 RINGER, SHARED or LOGO

Return Value:
   Returns const char * pointer to path

Comments:
   None

See Also:
   OEM_GetAppPath()
   OEM_GetMIFPath()
   OEM_GetRingerPath()
   OEM_GetSharedPath()
   OEM_GetAddrBookPath()
   OEM_GetLogoPath()

==================================================================
Function: OEM_GetSharedPath ()

Description:
   This function is called by the AEE to determine the path for the
shared directory. This is the directory that all the applets with
the shared directory access privilege share. The path must not have
a trailing directory separation character.

Prototype:

   const char * OEM_GetSharedPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================
Function: OEM_GetAppPath ()

Description:
   This function is called by the AEE to determine the path for the MIFs
(Module Information Files). This is the directory that the AppManager shall
look for the MIFs. The path must not have a trailing directory separation
character.

Prototype:

   const char * OEM_GetAppPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================
Function: OEM_GetRingerPath ()

Description:
   This function is called by the AEE to determine the path for the
ringers. This is the directory where all the ringer files are stored.
The path must not have a trailing directory separation character.

Prototype:

   const char * OEM_GetRingerPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================
Function: OEM_GetAddrBookPath ()

Description:
   This function is called by the AEE to determine the path for the
address book application and files. The path must not have a trailing
directory separation character.

Prototype:

   const char * OEM_GetAddrBookPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================
Function: OEM_GetMIFPath ()

Description:
   This function is called by the AEE to determine the path for the
Module Information Files (MIFs). The path is the directory in which
the AppManager looks for MIFs. The path must not have a trailing
directory separation character.

Prototype:

   const char * OEM_GetMIFPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================
Function: OEM_GetLogoPath ()

Description:
   This function is called by the AEE to determine the path for the
LOGO files. The path must not have a trailing directory separation
character.

Prototype:

   const char * OEM_GetLogoPath(void)

Parameters:
   None

Return Value:
   Returns const char * pointer to path.

Comments:
   None

See Also:
   OEM_GetPath()

==================================================================*/

#endif // AEE_CONFIG_H
