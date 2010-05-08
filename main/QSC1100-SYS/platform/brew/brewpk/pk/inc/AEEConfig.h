#ifndef AEECONFIG_H
#define AEECONFIG_H
/*======================================================
FILE:  AEEConfig.h

SERVICES:  Configuration Item interface to BREW

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for IConfig

        Copyright © 1999-2002,2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEShell.h"
#include "AEEModel.h"

typedef struct _IConfig          IConfig;

//
// Configuration Items - Passed to ICONFIG_GetItem()/ICONFIG_SetItem()
//

#define   CFGI_DNS_IP1                       0     // 0: 32-bit IP, Domain Name Server(1) in network byte-order
#define   CFGI_DNS_IP2                       1     // 32-bit IP, Domain Name Server(2) in network byte-order
#define   CFGI_DOWNLOAD                      2     // AEEDownloadInfo, see AEEDownload.h
#define   CFGI_SUBSCRIBERID                  3     // 32-byte ASCIIZ
#define   CFGI_MOBILEINFO                    4     // AEEMobileInfo
#define   CFGI_AUTOSTART                     5     // 5: AEECLSID - Auto-Started Applet (AEE_Init())
#define   CFGI_BUSY_CURSOR_OFFSET            6     // Position of hourglass (AEERect *)
#define   CFGI_DOWNLOAD_BUFFER               7     // Size in bytes to buffer data during download before calling fs_write (default 10K)
#define   CFGI_HTTP_BUFFER                   8     // Size in bytes of HTTP read buffer (default 4K)
#define   CFGI_NET_CONNTIMEOUT               9     // time in milliseconds! to wait for connect()
#define   CFGI_SUBSCRIBERID_LEN              10    // 10: Size in bytes of subscriber ID (default used if error returned)
#define   CFGI_MAX_DISPATCH_TIME             11    // Maximum time BREW should spend in the dispatcher before relinquishing control (default = 250 msecs)
#define   CFGI_MIN_IDLE_TIME                 12    // Minimum time BREW must relinquish from dispatcher (default = 35 msecs)
#define   CFGI_SLEEP_TIMER_RESOLUTION        13    // Timer resolution during when processor/os is in SLEEP mode (default = 1.2 seconds)
#define   CFGI_SYSMEM_SIZE                   14    // Size in bytes reserved to the system in low-memory (default = 2K)
#define   CFGI_DOWNLOAD_FS_INFO              15    // DLItemSize structure, only the dwFSAvail and dwFSSize will be used. If this is not supported by 
                                                   // the OEM, BREW will try to calculate available space with ISHELL_GetDeviceInfoEx().
#define   CFGI_SCREEN_SAVER                  16    // AEEScreenSaverInfo *
#define   CFGI_CLOSE_KEYS                    19    // OEMCloseKeys * (see structure below)
#define   CFGI_FILE_CACHE_INFO               20    // OEMFileCacheInfo * (see structure below)
#define   CFGI_GPSONE_TRANSPORT              21    // OEM GPS transport(IP, Data burst)OEM GPS transport (IP, Data burst) 
#define   CFGI_GPSONE_SVRIP                  22    // GPS server IP address
#define   CFGI_GPSONE_SVRPORT                23    // GPS server IP port
#define   CFGI_GPSONE_LOCK                   24    // GPS lock
#define   CFGI_MODULE_FSLIMIT                25    // MIFFSLimit * (wMaxFiles and dwMaxSpace for the module)
#define   CFGI_CARDID_LEN                    26    // Size in bytes of #define   CFGI_CARDID
#define   CFGI_CARDID                        27    // byte * of size returned by #define   CFGI_CARDID_LEN
#define   CFGI_DEBUG_KEY                     28    // OEMDebugKey * below
#define   CFGI_ALLOW_3G_2G_FAILOVER          30    // boolean, if TRUE, allow failover (see OEMSock.c)
#define   CFGI_ISTATIC_SCROLL                31    // Time in milliseconds for IStatic scroll speed
#define   CFGI_APPX5_MPC_ADDR                32    // where the MPC specified in CU Appx5 lives (IP addr in network-byte order)
#define   CFGI_APPX5_MPC_PORT                33    // where the MPC specified in CU Appx5 lives (IP port in network-byte order)
#define   CFGI_MAX_DEMO_ITEMS                34    // Count of demo items (default = 20), int *
#define   CFGI_WEB_IDLECONNTIMEOUT           35    // default IWeb IDLECONNTIMEOUT, in seconds, used if unspecified by application (see AEEWeb.h)
#define   CFGI_APP_KEY_1                     36    // Application key to be used for authorization/signing (uses OEMConfigData below)
#define   CFGI_GSM1X_PRL                     37    // PRL access
#define   CFGI_GSM1X_RTRE_CONFIG             38    // RTRE_CONFIG variable access
#define   CFGI_GSM1X_IDENTITY_PARAMS         39    // Writing IMSI, ACCOLC and MSISDN to GSM1x assigned NAM
#define   CFGI_GSM1X_SID_NID_PARAMS          40    // Read/write home or locked SID/NID stored in NV.
#define   CFGI_OFFLINE_PPP_TIMEOUT           41    // time in milliseconds! to wait in offline state before closing PPP
#define   CFGI_CACHED_RESOURCES              42    // OEMResCacheInfo * below
#define   CFGI_SAFEMODE_STARTMODE            43    // Starting mode for the handset. Mode values are defined in OEMSafeMode.h. 
                                                   // Possible values are:
                                                   // AEESAFEMODE_RUNLEVEL_SAFE 
                                                   // AEESAFEMODE_RUNLEVEL_PROBATION 
                                                   // AEESAFEMODE_RUNLEVEL_NORMAL
#define   CFGI_SAFEMODE_TIMER                44    // int32 - Time (in msecs) to wait before restoring device to normal mode
#define   CFGI_AUTOSTARTARGS                 45    // Auto start args
#define   CFGI_KB_AUTOREPEAT                 46    // OEMAutoRepeat * defined below
#define   CFGI_MOBILE_DIR_NUM                47    // Mobile Directory Number
#define   CFGI_MOBILE_DIR_NUM_LEN            48    // Mobile Directory Number length
#define   CFGI_FIRMWARE_ID                   49    // Firmware ID (OEM specific, Zero-terminated string representing firmware ID) 
#define   CFGI_FIRMWARE_ID_LEN               50    // Firmware ID length 
#define   CFGI_MAX_ACK_ATTEMPTS              51    // Max retries to flush preinstall/delete acks
#define   CFGI_SMS_EMAIL_DEST_LENGTH         52    // Length of destination referred by CFGI_SMS_EMAIL_DEST
#define   CFGI_SMS_EMAIL_DEST                53    // Destination to use if email needs to be sent as email address copied to payload 
#define   CFGI_SMS_MAX_PAYLOAD_LENGTH        54    // Maximum payload length in terms of number of bytes
#define   CFGI_SMS_DEFAULT_MO_ENC            55    // Default encoding for MO SMS
#define   CFGI_SMS_MO_ENC_TYPES              56    // Encoding types available for MO SMS
#define   CFGI_SMS_MO_ENC_TYPES_COUNT        57    // Count of encoding types available for MO SMS
#define   CFGI_BREWDIR_SMS_TELESERVICE       58    // Teleservice ID of BREW directed SMS
#define   CFGI_SMS_MO_ON_ACCESS_CHANNEL      59    // boolean, if TRUE, SMS message is sent on access channel
#define   CFGI_SMS_MO_ON_TRAFFIC_CHANNEL     60    // boolean, if TRUE, SMS message is sent on traffic channel. If both of
                                                   // CFGI_SMS_MO_ON_ACCESS_CHANNEL and CFGI_SMS_MO_ON_TRAFFIC_CHANNEL
                                                   // are TRUE, SMS message is first sent on access channel and on being
                                                   // too large for access channel is sent on traffic channel
#define   CFGI_DORMANCY_TIMEOUT_DEFAULT      61    // Default value for packet data inactivity timeout, which initiates dormancy
#define   CFGI_DOWNLOAD_CACHE                62    // maximum download'able item size that can be cached. default is 1k
#define   CFGI_LNG                           63    // Device Language Code
#define   CFGI_MANNER_MODE                   64    // Set device manner mode. pSrc = AEE_MANNER_MODE_XXX, nSize = sizeof(uint32)
#define   CFGI_APPX5_ON                      65    // User plane position determination ON (boolean)
#define   CFGI_GPSONE_LOCATION_SERVER_NAME1  66    // location server fully qualified internet name.
#define   CFGI_GPSONE_LOCATION_SERVER_NAME2  67    // location server fully qualified internet name.
#define   CFGI_SMS_GW_SCADDRESS_LENGTH       68    // length of default SC address for GW messages.
#define   CFGI_SMS_GW_SCADDRESS              69    // default SC address for GW messages. Gives a hook to OEM for providing service 
                                                   // center address for messages for which its not specified by the application. If 
                                                   // both of the application and OEM don't specify SC address, message is still 
                                                   // provided to service layer for processing.
#define   CFGI_SMS_GW_DOMAIN_PREF            70    // domain preference for GW messages
#define   CFGI_BACKLIGHT_CLASSIDS_LIST       71    // 0 terminated list of class IDs implementing IBacklight interface
#define   CFGI_RESERVED_1                    72    
#define   CFGI_BREWMA_CAPABILITIES           73    // BREWMA specific client capabilities. Bitflag Based.
#define   CFGI_LAST_VALID_TIME               74    // LastValidCfg* - Last valid time stored in non-volatile storage. LastValidCfg defined in AEEClock.h
                                                   // Updated with the secure clock's value during successful synchronizations 
                                                   // and during AEE_Init - if the secure clock is valid.
#define   CFGI_BANNERNAME                    75    // AECHAR* - Banner text (null terminated) to be displayed in the idle screen
#define   CFGI_BANNERNAME_LEN                76    // int* - Banner Length in bytes
#define   CFGI_THEMENAME                     77    // char* - Current themename copied onto the buffer (null terminated). The
                                                   // theme string is a sequence of one or more (semi-colon ';' separated) filenames to 
                                                   // the files making up the current theme.
#define   CFGI_THEMENAME_LEN                 78    // int* - Theme name length in bytes. A size of 0 indicates to the caller that 
                                                   // the Default Theme is currently active.
#define   CFGI_SUBLCD_PROPS                  79    // uint32* - Sets the visual properties of the sub-lcd of the device.  The data
                                                   // pointer holds a sequence of visual property key (uint32), value (uint32) pairs. The 
                                                   // size field is the number of bytes in the buffer. The property keys and their meaning 
                                                   // and expected values are defined in AEEWProperties.h in the UI Toolkit. The set of 
                                                   // properties that must be supported is:
                                                   //      - PROP_BGCOLOR
                                                   //      - PROP_FGCOLOR
                                                   // In order to avoid text to be illegible the following text properties may be supported:
                                                   //      - PROP_FONT_OUTLINECOLOR
                                                   //      - PROP_FONT_OUTLINEWIDTH
#define   CFGI_ANNUN_PROPS                   80    // uint32* - Sets the visual properties of the annunciator region (also called the
                                                   // icon row) of the device.  The data pointer holds a sequence of visual property key 
                                                   // (uint32), value (uint32) pairs. The size field is the number of bytes in the buffer.  
                                                   // The property keys and their meaning and expected values are defined in AEEWProperties.h 
                                                   // in the UI Toolkit. The set of properties that must be supported is:
                                                   //      - PROP_BGCOLOR
                                                   //      - PROP_FGCOLOR
#define   CFGI_IPV6_DNS_IP1                  81    // INAddr6, IPv6 Domain Name Server(1) in network byte-order
#define   CFGI_IPV6_DNS_IP2                  82    // INAddr6, IPv6 Domain Name Server(2) in network byte-order

#define   CFGI_USER_CLOCK_OFFSET             83    // int32* - User clock offset (in seconds) from the Secure Clock stored in non-volatile storage
                                                   // This config item is part of the Trusted Time API for UMTS devices
#define   CFGI_OEM_NOTIFY_APP_CTXT           84    // boolean - if TRUE then BREW generates OEM notifications for OEM app context data.
                                                   // notifications enabled are OEMNTF_APP_CTXT_NEW, OEMNTF_APP_CTXT_SWITCH and OEMNTF_APP_CTXT_DELETE

#define   CFGI_REQUIREPINENTRY               85    // boolean * - This config item would set TRUE or FALSE depending on whether PIN entry is 
                                                   // required to perform normal phone operations. 
                                                   // If this is TRUE, typically emergency calls and other special functionality would be allowed, 
                                                   // but not normal phone operations. Default: FALSE.

#define   CFGI_THEMES_INI                    86    // char* - Current themes INI filename copied onto the buffer (null terminated).
#define   CFGI_THEMES_INI_LEN                87    // int* - Themes INI filename length in bytes.
                                                   // A size of 0 indicates to the caller that there is no themes ini file configured.

#define   CFGI_WALLPAPERS_INI                88    // char* - Current wallpapers INI filename copied onto the buffer (null terminated).
#define   CFGI_WALLPAPERS_INI_LEN            89    // int* - Wallpapers INI filename length in bytes.
                                                   // A size of 0 indicates to the caller that there is no wallpapers ini file configured.

#define   CFGI_RINGTONES_INI                 90    // char* - Current ringtones INI filename copied onto the buffer (null terminated).
#define   CFGI_RINGTONES_INI_LEN             91    // int* - Ringtones INI filename length in bytes.
                                                   // A size of 0 indicates to the caller that there is no ringtones ini file configured.



#define   CFGI_DOWNLOAD_PDP_ID               95    // int* - to identify the PDP profile to be used
#define   CFGI_DOWNLOAD_PROXY                96    // char* - Proxy to be used for BREW Download Protocol, in proxyspec format
#define   CFGI_DOWNLOAD_PROXY_LEN            97    // int* - length of proxyspec returned for CFGI_DOWNLOAD_PROXY
#define   CFGI_DOWNLOAD_PROXYAUTH            98    // char* - auth credential of the proxy server specified in CFGI_DOWNLOAD_PROXY
#define   CFGI_DOWNLOAD_PROXYAUTH_LEN        99    // int* - length of credential of the proxy server specified in CFGI_DOWNLOAD_PROXY

#define   CFGI_OEMAUTH_CHALLENGE_CAP            100   //boolean * - if TRUE, OEM supports RUIM based challenge-response
#define   CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN   101   // int * - max. length of RUIM's resposne to NN challenge

#define   CFGI_MFLO_PROXY                    102   // char* - proxy address in proxyspec format (<protocol-to-proxy>/<proxy-address>)
#define   CFGI_MFLO_PROXY_LEN                103   // int*  - length of address of MediaFLO proxy

#define   CFGI_POINTER_MULTICLICK_TIME       104   // uint32* - number of milliseconds during which pointer clicks will be treated as multi-clicks (e.g. double-click). Default is AEEPOINTER_DEFAULT_MULTICLICK_TIME.
#define   CFGI_POINTER_MAX_STR_SIZE          105   // Deprecated. uint32* - maximum number of bytes that the character string pointed to by dwParam will ever be for any subsequent EVT_POINTER_* events. The default maximum size is AEE_POINTER_X_SIZE+AEE_POINTER_Y_SIZE.

#define   CFGI_LAZY_ACK                      107   // boolean * - TRUE if flushing of queued/pending ACKs is delayed until after the first launch of any data app after power up
                                                   // If FALSE: BREW will attempt to flush preisntall and queued ACKs on device power up
                                                   // Otherwise: BREW will attempt to flush preisntall and queued ACKs only after user initiates a data session for the first time.
                                                   
#define   CFGI_OEM_MAX_CACHE_SIZE            108   // Maximum size of the "app cache" used for direct download 
                                                   // to RMC and app launch from RMC 

#define   CFGI_OEM_RMC_PARK_POLICY           109   // Dictates OEM policy toward parking BREW apps to RMC.

#define   CFGI_OEM_IDS                       110   // List of OEM IDs used for sig verification
#define   CFGI_OEM_IDS_LEN                   111

#define   CFGI_DOWNLOAD_FS_INFOEX            112   // AEEFSFreeSpace structure. If this is not supported by the OEM, BREW will try to calculate available space 
                                                   // with the 32 bit CFGI_DOWNLOAD_FS_INFO config item. CFGI_DOWNLOAD_FS_INFOEX is the 64 bit version of 
                                                   // CFGI_DOWNLOAD_FS_INFO for devices with an efs > 4GB.
#define   CFGI_MMS_ICP                       201   // MMS Issuer Connectivity Parameters
#define   CFGI_MMS_CONFIG                    202   // MMS Configurations
#define   CFGI_MMS_UP                        203   // MMS User Preferences
#define   CFGI_BROWSER_CP                    204   // Browser Connectivity Parameters
#define   CFGI_JAVA_JDL                      205   // Java Download URL

#define   CFGI_MMS_ICP_LEN                   206   // MMS Issuer Connectivity Parameters length
#define   CFGI_MMS_CONFIG_LEN                207   // MMS Configurations length
#define   CFGI_MMS_UP_ATTRIBUTES             208   // MMS User Preferences record length
                                                   // and number of records
#define   CFGI_BROWSER_CP_LEN                209   // Browser Connectivity Parameters length
#define   CFGI_JAVA_JDL_LEN                  210   // Java Download URL length

#define   CFGI_SMS_VERIFY_BREW_HANDLER       211  // boolean * - If the config item is TRUE, a message with prefix "//BREWHANDLER:" 
                                                  // is considered as a BREW-directed message only if the handler is registered 
                                                  // with BREW. Otherwise, the message will not be considered as BREW-directed
                                                  // message and this allows other clients to handle the message.                                                
                                                  // If the config item is FALSSE, message with prefix "//BREWHANDLER:" is 
                                                  // automatically treated as BREW-directed message.
                                                                          
 
#define   CFGI_PROVISION_FIRST               0x1000 //Offset to build dependent items
#define   CFGI_PROVISION_LAST                0x2fff //End of build dependent items
#define   CFGI_MAX                           0x3000 //Max AEE value
#define   CFGI_FIRST_OEM                     CFGI_MAX //OEM added config items should start at this value


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
   uint32 nSize;
   uint8* pData;
} OEMConfigData;

//
// OEM Keypad Auto-Repeat Parameters
//
// Allows the OEM to override KB_AUTOREPEAT_START and KB_AUTOREPEAT_RATE.  Both
// numbers are represented in milliseconds.
//
// Note the following:
//
// - If dwStart is 0, auto-repeat is disabled
// - If dwDelay is 0, only a single EVT_KEY with the KB_AUTOREPEAT bit set is sent.
//

typedef struct OEMKBAutoRepeat
{
   uint32   dwStart;          // Default KB_AUTOREPEAT_START
   uint32   dwRate;           // Default KB_AUTOREPEAT_RATE
} OEMKBAutoRepeat;

//
// MIFFSLimit: Used in OEMGetConfig() when CFGI_MODULE_FSLIMIT is passed
//
typedef PACKED struct
{
   uint16            wMaxFiles;     // Maximum number of files in EFS this Module is allowed to create
   uint16            wPad;          // padding
   uint32            dwMaxSpace;    // Maximum EFS Space this module is allowed to consume
} MIFFSLimit;

/* Definitions and types needed for CFGI_GSM1X_... parameters.*/
#define GSM1X_MAX_MINS         (2)
#define GSM1X_DIR_NUMB_SIZ     (10)
#define GSM1X_DIR_NUMB_PCS_SIZ (15)

#define GSM1X_HOME_SID_NID_MAX_CNT    (20)
#define GSM1X_LOCKED_SID_NID_MAX_CNT  (20)


typedef PACKED struct {
  uint8    *pRoamingListStruct;
  boolean   enabled;
  uint16    maxSizeBytes;
} AEEGSM1xPRLInfo;

typedef uint8  AEEGSM1xRTREConfig;

typedef PACKED struct {
   uint8   nam;
   uint32  imsi_s1[GSM1X_MAX_MINS];
   uint16  imsi_s2[GSM1X_MAX_MINS];
   uint16  mcc;
   byte    imsi_11_12;
   byte    addr_num;
   uint32  t_imsi_s1[GSM1X_MAX_MINS];
   uint16  t_imsi_s2[GSM1X_MAX_MINS];
   uint16  t_mcc;
   uint8   t_imsi_11_12;
   uint8   t_addr_num;
   uint8   accolc[GSM1X_MAX_MINS];
   uint8   msisdn[GSM1X_DIR_NUMB_SIZ];
   uint8   num_digits;
   uint8   digitn[GSM1X_DIR_NUMB_PCS_SIZ];
} AEEGSM1xIdentityParams;

typedef PACKED struct {
   uint8   nam;
   uint16  homePairCnt;
   uint16 *pHomeSID;
   uint16 *pHomeNID;
   uint16  lockedPairCnt;
   uint16 *pLockedSID;
   uint16 *pLockedNID;
} AEEGSM1xSIDNIDParams;

typedef int AEEConfigItem;

//
// OEM Debug Key Definition
//
// Default:
//
// key = AVK_POUND
// nRepeat = 3
//
typedef struct
{
   AVKType  key;
   int16    nRepeat;
} OEMDebugKey;

#define DEFAULT_DBG_KEY       (AVK_POUND)
#define DEFAULT_DBG_KEY_COUNT (3)

//
// File Cache Defaults
//
// nCacheDefault - Default file cache size
// nCacheMin - Minimum size to cache (if below file size)
// nCacheMax - Maximum size to cache (if below file size)
// bCloseCachedFiles - Close file handle for file that is fully cached in read-only mode
//

#define FCACHE_ANY_VALUE   (-1)
#define FCACHE_DEFAULT     (0x1000)             // default cache size
#define FCACHE_MIN         (FCACHE_ANY_VALUE)   // minimum cache size (-1 == Ignored)
#define FCACHE_MAX         (FCACHE_ANY_VALUE)   // maximum cache size (-1 == Ignored)

typedef struct
{
   int32    nCacheDefault;
   int32    nCacheMin;
   int32    nCacheMax;
   boolean  bCloseCachedFiles;
} OEMFileCacheInfo;

//
// Cached Resource File Information
//
// This configuration item allows the OEM to control how many resource files are
// cached and how much data is cached for each resource file.  The "nCachedFiles"
// value is used to preallocate a structure that contains information about the
// file.  However, the entries are only populated when used.  The "nCacheSize"
// value allows the OEM to control how much data is cached for each file.  By
// default, the SCS_BEST value indicates that the entire resource file will
// be cached.
//

#define RCACHE_DEFAULT_CACHE  (SCS_BEST)
#define RCACHE_DEFAULT_FILES  (24)

typedef struct
{
   int      nCachedFiles;     // Default is RCACHE_DEFAULT_FILES
   int      nCacheSize;       // Default is RCACHE_DEFAULT_CACHE
} OEMResCacheInfo;

//
// OEM Close Keys
//
typedef struct
{
   AVKType     wCloseApp;        // Virtual key to close current app
   AEEEvent    evtCloseApp;      // EVT_KEY_PRESS, EVT_KEY_RELEASE, EVT_KEY, EVT_KEY_HELD
   AVKType     wCloseAllApps;    // Virtual key to close all apps
   AEEEvent    evtCloseAllApps;  // EVT_KEY_PRESS, EVT_KEY_RELEASE, EVT_KEY, EVT_KEY_HELD
} OEMCloseKeys;

//
//
// CFGI_MOBILEINFO type - Mobile Info structure
//

typedef struct
{
   uint8                nCurrNAM;
   uint32               dwESN;
   char                 szMobileID[16];
} AEEMobileInfo;

// struct for following MMS, Browser and Java related provisioning items
// CFGI_MMS_ICP                         
// CFGI_MMS_CONFIG                      
// CFGI_MMS_UP                        
// CFGI_BROWSER_CP                    
// CFGI_JAVA_JDL

#define MEMORY_TYPE_RUIM 1
#define MEMORY_TYPE_EFS  2


 typedef struct
{
   uint8*   pData;          //Raw data buffer for provisioning information
                            //This buffer would also be used to provide the
                            //size/length of raw data buffer. 
                            //Usage :- User would first call GetConfig with
                            //config item for length (e.g.CFGI_MMS_ICP_LEN)
                            //for a particular provisioning
                            //EF and get the length in pData.
                            //User would allocate memory for that much length
                            //User would again call GetConfig with the pData as
                            //pointer to allocated memory with the actual 
                            //config item (e.g. CFGI_MMS_ICP)

                            //For CFG_MMS_UP_ATTRIBUTES, pData will contain the
                            //record length
                            
   uint32   dwMemType;      //memory type either MEMORY_TYPE_RUIM or 
                            //MEMORY_TYPE_EFS
   uint32   dwRecordNumber; //Record number for CFGI_MMS_UP config item
                            //as it may contain multiple records

                            //Number of records in the file would be returned
                            //back when the user asks for CFG_MMS_UP_ATTRIBUTES

   int      nStatus;        //Status of the operation. SUCCESS if successful
                            //EFAILED otherwise
   AEECallback *pcbUserCallback;//User callback for getting the data 
                                //This callback must be provided whenever 
                                //user wants to read the specified config items
} OEMProvisionInfo;


//
// Download/Device Flags
//

#define DIF_USE_A_KEY            0x0001   // default OFF (use bKey).  If set, use AKey value
#define DIF_TEST_ALLOWED         0x0002   // This phone can be used to test local apps
#define DIF_MIN_FOR_SID          0x0004   // Use the MIN for the SID if not set
#define DIF_PREPAY               0x0008   // Prepay phone
#define DIF_NO_AUTO_ACK          0x0040   // Do not force ACKs until user runs MobileShop
#define DIF_SID_ENCODE           0x0080   // Uses SID rather than ESN for app encoding
#define DIF_SID_VALIDATE_ALL     0x0100   // Validates all apps rather than just SSN apps
#define DIF_RUIM_DEL_OVERRIDE    0x0200   // Allows one RUIM user to delete apps owned by another
#define DIF_AUTO_UPGRADE         0x0400   // Perform automatic upgrade when new uses purchased
#define DIF_NO_LAUNCH_MOD_ACK_DISABLED \
                                 0x0800   //Do not launch an app if it has flag MOD_ACK_DISABLED set
                                          //Use in prepay to confirm payment before app usage
#define DIF_SENSITIVE_SID          0x2000   // Do not send SID in clear in app Download and Delete ACK url
                                          // UMTS/GSM network where SID is sensitive information

//
// The protected flags cannot be set by the ADS.  They can only be set by the OEM or
// in debug modes. Note that the DIF_OEM_PROTECTED_FLAGS cannot be updated
// by anyone but the OEM.
//

#define DIF_OEM_PROTECTED_FLAGS  (DIF_TEST_ALLOWED|DIF_SID_ENCODE)
#define DIF_PROTECTED_FLAGS      (DIF_TEST_ALLOWED|DIF_SID_ENCODE|DIF_USE_A_KEY|DIF_MIN_FOR_SID)
#define DIF_ALL_FLAGS            (DIF_USE_A_KEY|DIF_TEST_ALLOWED|DIF_MIN_FOR_SID|DIF_PREPAY|\
                                  DIF_NO_AUTO_ACK|DIF_SID_ENCODE|DIF_SID_VALIDATE_ALL|\
                                  DIF_RUIM_DEL_OVERRIDE|DIF_AUTO_UPGRADE|DIF_NO_LAUNCH_MOD_ACK_DISABLED|\
                                  DIF_SENSITIVE_SID)

//
// Default Screen-Saver timeout
//

#define DEFAULT_SCREEN_SAVER_TIMEOUT   (30)


//
// CFGI_SUBSCRIBERID - Default length in characters/bytes
//

#define DEFAULT_SUBSCRIBERID_LEN    (32)

typedef uint32 AEEGPSOneLockInfo;

#define GPSONE_LOCK_NONE 0   // All GPS requests are allowed
#define GPSONE_LOCK_MI   1   // Lock Mobile Initiated GPS requests
#define GPSONE_LOCK_MT   2   // Lock Mobile Terminated GPS requests
#define GPSONE_LOCK_ALL  3   // All GPS requests are Locked -- only emergency (E911) 
                             // GPS requests allowed


//*******************************************************************************
//
// Dynamic Config Interface - Can be called by dynamically linked code.  Implementation
// is controlled by PL_SYSTEM and cascades to static functions.
//
//*******************************************************************************

QINTERFACE(IConfig)
{
   DECLARE_IBASE(IConfig)

   int   (*GetItem)(IConfig * po, AEEConfigItem n, void * pDest, int nSize);
   int   (*SetItem)(IConfig * po, AEEConfigItem n, void * pSrc, int nSize);
   int   (*GetModel)(IConfig * po, IModel ** ppIModel);
};

#define ICONFIG_AddRef(po)                GET_PVTBL(po,IConfig)->AddRef(po)
#define ICONFIG_Release(po)               GET_PVTBL(po,IConfig)->Release(po)
#define ICONFIG_SetItem(po,n,ps,ns)       GET_PVTBL(po,IConfig)->SetItem(po,n,ps,ns)
#define ICONFIG_GetItem(po,n,ps,ns)       GET_PVTBL(po,IConfig)->GetItem(po,n,ps,ns)
#define ICONFIG_GetModel(po,ppIModel)     GET_PVTBL(po,IConfig)->GetModel(po,ppIModel)

#if defined(__cplusplus)
}
#endif
/*=============================================================================
  INTERFACES   DOCUMENTATION
===============================================================================

Interface Name: IConfig

Description:
   IConfig interface lets OEMs:
      Get value of a configuration item CFGI_XXX
      Set value of a configuration item CFGI_XXX
      Get notified on change in the value of a configuration item CFGI_XXX

The IConfig interface instance is obtained via ISHELL_CreateInstance()
mechanism and requires PL_SYSTEM privilege.

===============================================================================

ICONFIG_AddRef()

Description:
   This method increments the reference count of the IConfig Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ICONFIG_AddRef(IConfig * po)

Parameters:

   po [in]: Pointer to the IConfig Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ICONFIG_Release()

===============================================================================

ICONFIG_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ICONFIG_Release(IConfig * po)

Parameters:

   po [in]: Pointer to the IConfig Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ICONFIG_AddRef()

===============================================================================

ICONFIG_GetItem

Description:  
   This method gets the value of the specified configuration item.

Prototype:
   int ICONFIG_GetItem(IConfig * po, AEEConfigItem n, void * pDest, int nSize)

Parameters:
   po [in]: Pointer to IConfig interface
   n [in]: Configuration Item ID (CFGI_XXX)
   pDest [out]: Place holder for configuration item value
   nSize [in]: Number of bytes in the buffer specified by pDest

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   None

Side Effects:
   None

===============================================================================
   
ICONFIG_SetItem

Description:  
   This method sets the value of the specified configuration item.

Prototype:
   int ICONFIG_SetItem(IConfig * po, AEEConfigItem n, void * pSrc, int nSize)

Parameters:
   po [in]: Pointer to IConfig interface
   n [in]: Configuration Item ID (CFGI_XXX)
   pSrc [in]: Configuration Item Value
   nSize [in]: Number of bytes in the buffer specified by pSrc

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   None

Side Effects:
   None

===============================================================================

ICONFIG_GetModel

Description:  
   This method gets the model interface object associated with IConfig interface. 
Applications can add listeners to the model interface to get notified on changes
to the configuration item values. On a successful ICONFIG_SetItem(), listeners
associated with model are delivered pointer to ModelEvent structure. evCode member
of ModelEvent structure specifies EVT_MDL_GENERIC_CHANGE event and dwParam member
of the ModelEvent structure specifies the configuration item ID of the updated
configuration item.

Prototype:
   int ICONFIG_GetModel(IConfig * po, IModel ** ppIModel)

Parameters:
   po [in]: Pointer to IConfig interface
   ppIModel [out]: Place holder for IModel interface

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   Applications must call IMODEL_Release() on the model interface obtained
   using this method.

Side Effects:
   None

=============================================================================*/
#endif   // AEECONFIG_H
