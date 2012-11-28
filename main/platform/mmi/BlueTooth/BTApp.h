#ifndef __BTAPP_H__
#define __BTAPP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTApp.h                                    */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This applet is the BT App of 3GUI.            */  
/*                                                                           */  
/*              Copyright (c) 2003-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/inc/BTApp.h#3 $
  $DateTime: 2009/07/10 11:47:34 $
  $Author: nksingh $                    
                               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
2009-06-24   ns   Fixed BTAPP_CONFIG_VERSION so it is different when 
                  FEATURE_BT_2_1 is defined.      
2009-01-14   gs   Added support for OPP/FTP file transfer progress indication 
                  and OPP connect cancel.
2008-12-04   co   Removed bDeselectAudio.
2008-11-21   ssk  Added new user events for A2DP and AVRCP.
2008-10-26   pn   Removed inclusion of MediaPlayer.bid and PhoneApp.bid.
2008-10-17   co   Replaced BT_SSR_LPC_DEBUG with FEATURE_BT_TEST_API.
2008-09-23   co   Fixed compiler warnings.
2008-08-07   vm   Defined EVT_AG_DISABLED and EVT_NA_DISABLED. 
                  Added new functions for Initializing and De-Initializing AG. 
2008-07-28   rs   klocwork fixes for arr wTransactionId that might be indexed 
                  out of bound.
2008-06-09   co   Moved BT_SSR_LPC_DEBUG definition to bti.h
2008-05-27   rb   Fix compiler warnings


05/21/08    RS    Added a variable to track an ongoing activity in BIP.
04/22/08    VM    Changed default service security from LOW to MED.
04/10/08    VM    Added Service Enabling type BTApp_Enabling in CBTApp.
03/12/08    VM    Added bRebondOptMITMEnabled and bRebondOptBONDEnabled
                  in RM structure.
03/03/08    gs    Added support for HID profile Get/SetReport functions.
02/06/08    gb    Mainlining all the changes done for SM2-SM4 implementation
02/05/08    nm    Backing out changes in version BTApp.h#171
02/05/08    mh    Added flags to SAP to keep register, deregister and disconnect 
                  states
01/28/08    nm    Fix for legacy bonding failure between non 2.1 and 2.1 dev
01/29/08    ns    Added new functions for Initializing and De-Initializing AG
                  and NA. Added new events for to Select/Deselect BT
                  Devices from Applications other BREW Apps.
01/28/08    rs    Increased the buffer size that would be needed to display 
                  large information like SDP search response on screen.
01/17/08    gs    Added support for HID Host in enabling menu
01/09/08    co    Defined IS_SCO_4_VOICE_UP(),
                  BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B, and
                  BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B under SCO_REWORK for
                  SCO connection manipulations.
01/08/08    mh    Multi profile connection support for Lisbon SSR feature
                  low power config tests is moved under test feature.
12/17/07    mh    Pass key variable added in RM to fix legacy bonding issues.
12/13/07    co    Added bAudioDisconnecting to better track SCO state if
                  SCO_REWORK is supported.
12/04/07    mh    Remote BD address added in PBAP 
11/07/07    gs    Added HID profile host role support
11/01/07    ag    Merged Lisbon(BT2.1) code to the mainline
09/14/07    ns    Included Phone App CLSID file required for UIOne targest
09/12/07    pn    Added BT_APP_MENU_LIST_CONNECTED.
17/07/07    be    Cosmetic changes: removed extra spaces, corrected indentation
                  of #defines added in place of earlier BTApp_DiagStatusType ENUM.
11/07/07    be    Removed compiler warning due to IDIAG_OTHER_INT_ERROR out of int range.
07/06/07    jj    Fixed the problem with hadling media player incase of incoming
                  call events.
06/29/07    jj    Connection status lag added to AG
06/26/07    mh    Authenticate variable added to PBAP.
06/26/07    pn    Added BTAPP_AT_CMD_MIN to BTApp_ATCmdType.
05/17/07    pn    Replaced BT_APP_MENU_SWITCH_ROLE with 
                  BT_APP_MENU_SET_CONN_ROLE.
04/20/07    pn    Removed featurization for MAX_CONTROL_DATA_LEN.
03/27/07    pn    Added BTAPP_AG_FLAG_AUDIO_WANTED_B.
03/23/07    pn    Added EVT_SD_DISC_RSP.
03/09/07    jtl   Added L2 struct.
03/14/07    ns    Replaced ICM with ITelephone and IBatt with IBattery APIs.
                  Removed dependency on recent calls APP.
03/02/07    gs    Updated PBAP struct
02/22/07    pn    Added support for Config Scan Params menu.
02/16/07    pn    Updated macro MENU_SET_SEL to fix critical lint errors.
02/01/07    JJ    Added Automation support for FTP settings 
11/27/06    JJ    Modifications done to process passwd and passkey seperately 
                  in ServerRegistration and Clientconnect interfaces.
11/23/06    JJ    Added support for automation plugins.
11/15/06    smr   Added Authentication support for IOBEX.
11/08/06    gs    Added params to keep track of Primary and secondary BIP
                  roles.
11/07/06    smr   Added support for IOBEX.
11/06/06    pn    Added menu to configure how to handle HS button press.
10/13/06    ns    Added automation support to poll for AG connection status
10/10/06    gs    Added objSize param in FTP struct.
09/15/06    ns    Added support in HFP1.5 for handling additional AT commands
                  sent by SAP Siemens car kit
09/01/06    ns    Added support to return error response for AT+BTRH? command
07/31/06    ns    Added audio rework feature
07/13/06    ns    Added concurrency support for PBAP pull vcard obj operation
07/07/06    pn    Fixed compile error when built with FEATURE_BT_HFP_1_5
                  undefined.  Added bDeselectAudio in AG structure.
07/07/06    ns    Added support to allow browsing from one level above the 
                  leaf directory. Added concurrency support for download PB obj
06/12/06    ns    Added automation support for PBAP enabling and disabling
                  Added support for UserID during BPP obex authentication
06/07/06    ns    Added automation support for PBAP
05/30/06    gs    Fixing compile issues introduced in last check-in.
05/30/06    gs    Mainlining PBAP feature development.
05/08/06    ns    Added support to perform role switch operation
05/04/06    pn    Added BTAPP_AT_CMD_BLDN and BTAPP_AT_CMD_MEM_DIAL to 
                  BTApp_ATCmdType.
05/04/06    sn    Added support to choose EFS or SD MC as storage location for 
                  FTP.
04/25/06    pn    Added bStreaming to A2DP structure.
04/20/06    ns    Provided I/F to print file using BPP from another BREW App
04/11/06    pn    Removed EVT_DIALER_ANSWER. Added bVocoderOn and 
                  macro BDADDR_VALID().
04/06/06    dc    Added ability to set default dial string via automation
04/06/06    ns    Using AEE macro for BT device name length
03/08/06    dc    Added ability to query the link status
03/03/06    ns    Removed support for AT+CPBS=? command in HFP1.5 feature
01/23/06    ns    Added support for getting radio activities.
01/23/06    ns    Added diagnostic support to query information about the 
                  AVRCP operations performed by HS/HF. Added automation 
                  support for bonding when AG/NA are enabled
01/09/06    pn    Fixed featurization so BTApp can build without ICM.
01/05/06    ns    Added support for SAP APDU Tx and Rx through files in EFS
12/28/05    sn    Added diagnostic support for OPP Client abort feature.
12/26/05    ns    Added support to set the passkey while enabling A2DP and 
                  AVRCP through automation
                  Added diagnostic support to poll status of A2DP control flags
12/25/05    dc    Added the device address field to BTApp_DiagGetDevListRspType
12/20/05    pn    Featurized SAP Client with FEATURE_BT_EXTPF_TEST.
12/19/05    sn    Corrected DIAG error code and user event value.
12/15/05    ns    Added diagnostic support for enabling profiles
                  Enabled SAP client
12/12/05    sn    Added app support to abort OPP push/pull operation.
12/06/05    sn    Added diagnostic support for FTP Client ABORT feature.
12/05/05    ns    Added support for extra AT commands specific to parrot carkit
12/02/05    sn    Added support to enable A2DP and AVRCP from 'Settings' menu.
11/30/05    pn    Added support for SAP deregistration while connected.
11/14/05    ns    Added support to send a BIP print request from other apps
11/02/05    dc    Added as structure to hold the Filename returned by 
                  BTApp_DiagSendRemoteBrowsingRsp and BTApp_DiagSendLocalBrowsingRsp
10/27/05    pn    Replaced BT_SWDEV_HFP_1_5 with FEATURE_BT_HFP_1_5.
10/21/05    ns    Modified SAP server implementation to use ICARD interface 
                  instead of IBT. SAP server can be enabled from the BT ON/OFF
                  option. SAP client menu option removed.
                  Moved ICM handle out of AG as it is used across profiles
10/09/05    dc    Added IDIAG events for AVRCP Automation. Added IDIAG events 
                  setting the concurrcy flag and toggling Radio. Increased stack
                  depth to 50.
                  Added IDIAG support to set BT Name. 
                  Added BTAPP_DIAG_OPP_OBJECT_JPEG to the enumumeration 
                  BTApp_DiagOPPObjectType.
                  Moved the bEventIsProcessed and bGenericResult from the pMe 
                  struct to the individual OBEX structures.
09/28/05    ns    Used dynamic memory for storing SD and RM records. Increased
                  menu stack depth to 20 and max discoverable devices to 25. 
                  Passkey is stored in a seperate varible for all OBEX profiles.
                  Added debug message to track the menu stack depth.
09/21/05    pn    Replaced BT_SWDEV_AG_EXT_IO with BT_SWDEV_HFP_1_5. Undefined
                  BT_SWDEV_HFP_1_5.
09/15/05    ns    Used dynamic mem allocation in HFP1.5 to avoid exceeding 
                  static memory limitation for BTAPP.
09/09/05    ns    Included AEECard.h for HFP 1.5 implementation
09/07/05    ns    Added support to originate a video call using Headset
09/06/05    ns    Added support for HFP 1.5. Handled AT commands AT+COPS, 
                  AT+CLCC, AT+CNUM, AT+CMEE
09/01/05    ns    Config file version changed, variable added for private 
                  mode handling
08/24/05    ns    Added private mode handling for CCAP recommendations
08/19/05    ns    Added support for CCAP recommendations required for HF 
                  application
08/17/05    dc    Added events for Concurrent BIP Image Printing and SPP 
                  Settings menu.
08/03/05    pn    Replaced bGettingName in SD with uGetNameDevIdx in RM.
08/02/05    sn    Updated BTAPP_ROOT_DIR with "fs:/" namespace for BREW 3.1 
                  and above.
07/28/05    pn    Made number of bytes sent in SPP test configurable.
07/27/05    ns    Added support to enable obex profiles(servers) during startup
07/26/05    jh    Featurized AG External I/O code using BT_SWDEV_AG_EXT_IO
07/26/05    dc    Added BTAPP_FTP_BROWSING_MENU_STATUS to 
                  BTApp_DiagPolledStatusType
07/21/05    pn    Added initial support for AG External I/O.
07/21/05    sn    Added BT_APP_MENU_FTP_MOVE_TO_FLDR menu type.
07/13/05    ns    Added concurrency test automation support for BIP push, pull,
                  auto archive, remote camera, remote display, archived
                  objects,referenced objects.
07/12/05    dc    Added concurrency test automation interfaces for OPP Pull, 
                  FTP Get operations, and BPP Send Operations
                  Added automation interface for Master cntrl Role Switch Toggle
                  Added automation interface for accessing the BT Name, Address
07/06/05    sn    Added BT_APP_MENU_FTP_SRV_REGISTER and automation support to 
                  select allowbrowse, readonly options while registering FTP 
                  server.
06/27/05    pn    Removed bAutoDevtype from AG structure.
06/27/05    ns    Added concurrency test automation interfaces for OPP Push 
                  and FTP Put operations
                  Added automation interface to get the status of a concurrency
                  event
06/27/05    sn    Added support for FTP ABORT feature
06/20/05    pn    Added bDevDialed to AG structure. Added EVT_DIALER_ANSWER.
06/16/05    ns    Added UI and automation support for A2DP control interface
06/13/05    sn    Added app and automation support to input folder name for 
                  FTP create folder feature
06/09/05    ns    Externalised function BTApp_WriteConfigFile().
05/24/05    dc    Added Events for A2DP Automation. Defined a structure
                  to handle the payload for BTAPP_DiagA2DPUse. PollStatus,
                  SelectTest and Getcontext capability for A2DP.
05/23/05    ns    Automated bonding process on the server side for all profiles.
05/09/05    ns    Moved GET NAME definitions to BTApp.h from BTApp.c
05/01/05    gs    Added bOKToSend parameter in BPP struct to allow only
                  one BPP SendFile operation at a time.
04/21/05    pn    Replaced BT_APP_MENU_SPP_TEST with BT_APP_MENU_SPP_RESULTS.
                  Added BT_APP_MENU_SPP_TESTS.
04/12/05    dc    Added an extra optional field to BTApp_DiagBIPCliConExReqType,
                  BTApp_DiagOPPCliConExReqType, BTApp_DiagBPPCliConExReqType
                  and BTApp_DiagFTPCliConExReqType to handle the PIN code menu
                  during connection.
04/12/05    pn    Added support for master control to allow/disallow role 
                  switch, to turn on/off BT radio and visibility.
04/11/05    sn    Added support for FTP local folder browsing feature to PUT
                  local files and folders on to the server
                  BTApp_DiagOPPCliConExReqType, BTApp_DiagBPPCliConExReqType
                  and BTApp_DiagFTPCliConExReqType to handle the PIN code menu
                  during connection.
04/07/05    dc    Added Polling Status Capability to FTP, Discoverable Menu,
                  Bondable menu. 
                  Added IDIAG hooks to Access the SPP Server and client.

04/06/05    ns    Added support for default dial string edit.
03/22/05    ns    Added support to set SAP Max and Min Frame size from UI
03/21/05    pn    Modified POP_MENU(). Added MENU_SET_SEL(). Added uNumCalls to
                  AG struct. Replaced uSKMenuSel with uMenuSel.
03/16/05    ns    Replaced SAP #ifdef ICardConnection with 
                  BTAPP_ICARD_CONNECTION as it is conflicting with an 
                  ICARD interface structure
                  Moved some SAP struct members out of #ifdef ICardConnection
03/08/05    ns    Removed BTApp_DiagFTPListFolderRspType structure as it is 
                  not required
03/08/05    jtl   SPP test uses buffer and flow control.
03/02/05    ns    Added 
                  - support for SAP APDU requests
                  - removed password menu for SAP
                  - Automation support for SAP
02/24/05    pn    Added bGettingName to SD struct.  Changed enum value for each 
                  sub-group of menus.
02/23/05    dc    Added IDIAG Events for Discoverable Menu, Bondable Menu
                  and Security menu.
02/23/05    sn    Added automation support to browse through FTP server's 
                  folders backward to parent & root folder
02/23/05    ns    Modified SAP structure for SIM Card ATR support
02/18/05    sn    Added 
                  - FTP browsing menu name, related structure fields
                  declarations and macros 
                  - FTP related diagnostic events and errors
02/16/05    ns    Added support for IBT SAP
01/26/05    dc    Added Event EVT_BTAPP_DIAG_BPP_CLI_SEC_CONEX. Defined 
                  a request packet BTApp_DiagBPPCliSecConExReqType to handle
                  Secondary (STS) BPP connection.
                  Added the entry BTAPP_BPP_CLI_SEC_CON_STATUS to BTApp_DiagPolledStatusType enum
01/20/05    gs    Added bCallingParty and status in CTP/ICP structs
01/18/05    dc    Added a enum to select the object types for OPP Push.
01/12/05    gs    Added BT_APP_MENU_LIST_TELEPHONY
01/11/05    pn    Changed btapp.bid to BTApp.bid.
01/10/05    dc    Added a structure BTApp_DiagBIPCliConExReqType which contains
                  the payload information (OBEX Servers and Responder types).
                  Changed EVT_BTAPP_DIAG_BIP_CLI_CONNECT to EVT_BTAPP_DIAG_BIP_CLI_CONEX
01/05/05    ns    Added automation support for BPP Settings and Target Menu 
                  options
01/04/05    dc    Modified the BTApp_DiagBPPCliConExReqType structure to 
                  contain the Channel Types (DPS/STS).
01/03/05    gs    Added
                  - support for CTP/ICP 
                  - support for selecting OBEX servers for BIP
                  - OBEX and TCS-BIN menus.  Moved OBEX tests (OPP, FTP, 
                    BIP, BPP) under OBEX menu.
12/21/04    gs    Undef FEATURE_TEST_APP_AUTOMATION for 6300 target
12/17/04    pn    Fixed QBT featurization problem within test automation code.
12/17/04    gs    Added support for BPP Settings and Target menu options
12/14/04    gr    Added automation support for BIP PUSH OPS, PULL OPS, 
                  Image Printing, Automatic Archive, Remote camera, 
                  Remote Display, Archive/Referenced Objects
                  Added Settings Menu Control via automation
12/10/04    gr    Added Support for BIP Connect, Disconnect
12/06/04    gr    Added support for BIP Register and Deregister via Auto
                  More style changes that we missed out in the last review
11/23/04    gr    Updated to 80 col where applicable
10/27/04    pn    Added support for Test Mode control and Security Mode 2
                  testing on NA connections.
10/25/04    gr    - Provided automation support for Connect/Disconnect Functions
10/13/04    gr    Added support for 
                  - Selecting OBEX servers for BPP
                  - Selecting MIME type for BPP
                  - Entering Job ID for BPP
                  - Support for automation of Register and DeRegister
                  - Support for Automation of SendSOAP
                  - Support for Automation of Send
09/23/04    pn    Added bBusyIconUp.
09/23/04    gr    Added support for Device Search
09/20/04    gr    Updated the Fetch and Send interface (memory efficiency)
                  Implemented Bond, Use Unbond All and Remove All Dev OPtions
09/14/04    gr    Added Device operation support
                  Changed DiagSetSelection to DiagSetMenuSelection
                  Added DiagSetSoftMenuSelection
09/01/04    pn    Added bIgnoreButton to AG structure.
08/23/04    pn    Added support for FEATURE_BT_VT.
09/02/04    gr    - Added support AG Audio Transfer
08/26/04    gr    - Added support for AG interfaces set and get
08/23/04    pn    Added FTP settings menu.
08/10/04    pn    Added support for specifying FTP server password
08/11/04    gr    Added GetDevList, Client Exchange and conex support
08/05/04    gr    Added support for Business card exchange and Turn BT on context
08/04/04    gr    Added support for PollStatus, JumpToMenu
07/27/04    gr    Added OPP Client connect, disconnect, push, pull
07/22/04    gr    Added support for Select Test Diagnostic
07/15/04    gr    Added Core support for IDIAG
                  Added main menu control via SetContext and GetContext
07/27/04    pn    - Added support for IBT FTP.
                  - Added support for automatic launching of BTApp.
08/03/04    pn    Added support for automatic launching of BTApp.
                  Added BT_APP_MENU_LIST_OBEX_SERVERS.
08/02/04    gs    Added support for testing IBT BIP Responder APIs.
07/29/04    pn    Added OPP Business Card Exchange feature and support for IBT
                  FTP and BIP.
07/26/04    pn    Removed include file BTApp_res.h
07/01/04    pn    Added support for IBT OPP.
06/28/04    ry    Included AEE_OEM.h.
05/25/04    pn    Added connectedBDAddr to NA structure.
05/20/04    pn    Made 3G features target independent
05/04/04    pn    - Added support for setting local unit's discoverable mode
04/30/04    pn    - Added support for OPP and FTP tests
04/05/04    pn    - Added BT_APP_MENU_HCI.
03/12/04    pn    - Added bVolumesInit to AG structure.
03/01/04    pn    - Added bDoVerify to SPP structure.
02/09/04    pn    - Added support for targets having no SKUI
                  - Updated BTAppMenuType to support enabling/disabling VR
                    capability in AG.
01/14/04    pn    - Updated CBTApp to support persistent AG config feature.
                  - Added more local events
12/23/03    pn    Updated AG struct and BTAppMenuType to support audio device
                  type selection.
12/18/03    pn    Updated SPP struct and BTAppMenuType.
12/03/03    pn    - Added menuStack to track many levels of menus
                  - Added 3 menus
11/26/03    pn    Added AEEBTSppStatus SPP structure.  Updated BTAppMenuType.
                  Updated user events.
11/14/03    pn    Modified NA and SPP structures.
11/05/03    pn    Initial Revision.
                  
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "AppComFunc.h"
#include "BREWVersion.h"

#include "AEE_OEM.h"
#include "AEEModGen.h"          /* Module interface definitions */
#include "AEEAppGen.h"          /* Applet interface definitions */
#include "AEEShell.h"           /* Shell interface definitions */
#include "AEEFile.h"            /* File interface definitions */
#include "AEEStdLib.h"          /* AEE Stb Lib Services */
#include "AEEMenu.h"            /* AEE Menu Services */
#include "AEEText.h"
#include "AEEBTExt.h"
#include "AEEBTExtAG.h"
#include "AEEBTExtNA.h"
#include "AEEBTExtSD.h"
#include "AEEBTExtRM.h"
#include "AEEBTExtSPP.h"
#include "AEEBTExtL2.h"
#ifdef FEATURE_BT_EXTPF_OPP
#include "AEEBTExtOPP.h"
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_BT_EXTPF_FTP
#include "AEEBTExtFTP.h"
#endif
#ifdef FEATURE_BT_EXTPF_BIP
#include "AEEBTExtBIP.h"
#endif
#ifdef FEATURE_BT_EXTPF_BPP
#include "AEEBTExtBPP.h"
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
#include "AEEBTExtPBAP.h"
#include "AEEAddrBook.h"
#include "AEEAddrBookExt.h"
#include "AEECallHistory.h"
#include "AEECallHistoryExt.h"
#endif
#ifdef FEATURE_IOBEX
#include "AEEOBEX.h"
#include "AEEOBEXNotifier.h"
#include "AEEOBEX_priv.h"
#endif
#ifdef FEATURE_BT_EXTPF_CTP
#include "AEEBTExtCTP.h"
#include "AEEBTExtICP.h"
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
#include "AEECard.h"
#endif
#ifdef FEATURE_BT_EXTPF_AV
#include "AEEBTExtAVRCP.h"
#include "AEEBTExtA2DP.h"
#endif

#include "BTApp.bid"

#include "AEETelephone.h"
#include "AEEAddrBookExt.h"

#ifdef FEATURE_BT_VT
#include "AEEVideoPhone.h"
#include "VideoPhone.h"
#endif

#ifdef FEATURE_BT_HFP_1_5
#include "AEECard.h"
#include "AEEBattery.h"
#include "AEEAddrBook.h"
#endif

//Add By zzg 2010_10_29
#include "AEEAnnunciator.h"
#include "Appscommon.h"
#include "Appscommon.brh"
#include "appscommonimages.brh"
#include "btapp.brh"
//Add End

#include "BTApp_priv.h"


#include "AEEConfig.h"		//Add By zzg 2010_11_23



#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

/*===========================================================================

                      MACROS

===========================================================================*/

#define TOP_MENU        pMe->menuStack[ pMe->uTopMenu ]
#define PREV_MENU       pMe->menuStack[ pMe->uTopMenu - 1 ]
#define PUSH_MENU(m) \
  (pMe->menuStack[ ++pMe->uTopMenu] = m); \
  MSG_LOW( "PUSH_MENU - Index=%d,Menu=%d", pMe->uTopMenu, m, 0 )
#define POP_MENU()      (pMe->uMenuSel[ pMe->uTopMenu-- ] = 0)
#define MENU_SET_SEL(s) (pMe->uMenuSel[ pMe->uTopMenu ] = (s))
#define MENU_SEL        pMe->uMenuSel[ pMe->uTopMenu ]

#define SPP_BUF_FULL()  (pMe->mSPP.buf_count >= SPP_BUF_SZ)
#define SPP_BUF_EMPTY() (pMe->mSPP.buf_count == 0)

#define SPP_CLEAR_BUF() {pMe->mSPP.buf_count = 0;\
                         pMe->mSPP.buf_start = 0;\
                         pMe->mSPP.buf_end   = 0; }

#define BDADDR_VALID( bda ) \
  (AEEBT_BD_ADDRS_EQUAL( bda, &NULL_BD_ADDR ) == FALSE)

#ifdef FEATURE_AVS_BT_SCO_REWORK
#define IS_SCO_4_VOICE_UP() \
          ( ((pMe->mAG.callState == BTAPP_AG_CALL_ANSWERED) || \
             (pMe->mAG.callState == BTAPP_AG_CALL_ORGINATED) || \
             (pMe->mAG.callState == BTAPP_AG_CALL_CONNECTED)) && \
            ((pMe->mAG.bAudioConnected != FALSE) || \
             (pMe->mAG.bAudioConnecting != FALSE)) )
#endif /* FEATURE_AVS_BT_SCO_REWORK */

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define MOVE_LEFT               -1
#define MOVE_RIGHT               1
#ifdef FEATURE_BT_2_1
#define LONG_TEXT_BUF_LEN      600
#else
#define LONG_TEXT_BUF_LEN      600
#endif
#define SHORT_TEXT_BUF_LEN     100
#define MENU_STACK_DEPTH        50
#define ONE_SECOND            1000

#define FORMATED_BD_ADDR_SIZE   16
#define MAX_DEVICES             25
#define MAX_PASSKEY_LEN         (AEEBT_MAX_PIN_CODE_LEN + 1)
#define MAX_JOB_ID_LEN           4
#define MAX_CONTROL_DATA_LEN     6
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#define MAX_IDLE_TIME_SEC      300
#define MAX_VOL_LEVEL           15
#define UD1_GET_NAME_SUCCESS     1
#define UD1_GET_NAME_FAILED      2

/* Amount of time the SPP client waits before sending more 
   data if the queue is full */
#define SPP_CLIENT_WAIT_MS      50

// Size of the SPP read buffer for SPP
#define SPP_BUF_SZ            1024

#ifdef FEATURE_BT_EXTPF_FTP
/* No of items that can be displayed on the UI; Used when storing FTP folder 
   browsing contents */
#define MAX_OBJECTS             100	//25
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
#define MAX_FILTER_VAL_LEN      10
#define MAX_LIST_COUNT_VAL_LEN   5
#define MAX_HANDLE_NAME_LEN     15
#define MAX_PBAP_OBJECTS        30
#endif

#ifdef FEATURE_IOBEX
#define OBEX_DATA_BUFFER_SIZE   512
#endif

#ifdef FEATURE_BT_EXTPF_AV
#define MAX_AVRCP_EVENTS        20
#endif /* FEATURE_BT_EXTPF_AV */

#define EVT_CLR_MSG_SCR       1000
#define EVT_DEV_DISC_RESP     1001
#define EVT_DEV_BROWSE_RESP   1002
#define EVT_DEV_SEARCH_RESP   1003
#define EVT_DEV_NAME_RESP     1004
#define EVT_PIN_RQST          1005
#define EVT_SD_DISC_SET       1006
#define EVT_SD_DISC_RSP       1007
#define EVT_AG_DISABLED       1008
#define EVT_NA_DISABLED       1009
#define EVT_SPP_OPENED        1010
#define EVT_SPP_OPEN_FAILED   1011
#define EVT_SPP_CLOSED        1012
#define EVT_SPP_CONNECTED     1013
#define EVT_SPP_DISCONNECTED  1014
#define EVT_RM_LOCAL_NAME_SET 1020
#define EVT_RM_LOCAL_COD_SET  1021
#define EVT_RM_LOCAL_SEC_SET  1022
#define EVT_RM_BONDED         1030
#define EVT_RM_DEV_SEC_SET    1031
#define EVT_RM_DEV_SEC_FAILED 1032
#define EVT_RM_BONDABLE_SET   1033
#define EVT_RM_AUTH_REQ       1034
#define EVT_FIRST_LAUNCH      1035
#define EVT_BIP_PUT_IMAGE     1037
#define EVT_BPP_SIMPLE_PUSH   1038
#define EVT_OBEX_AUTH         1039

#ifdef FEATURE_BT_2_1
#define EVT_PASSKY            1040
#endif /* FEATURE_BT_2_1 */

/* Events posted by other BREW apps to set/release snd device to BT. ui_snd_xx
   interfaces are not exposed through BREW, hence support for these events.
   Event values should not be changed. Add new events below */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

#define EVT_AVRCP_DISCONNECTED 1043
#define EVT_A2DP_DISCONNECTED  1044

#ifdef FEATURE_BT_EXTPF_OPP
#define EVT_OPP               1100
#endif

#ifdef FEATURE_BT_EXTPF_FTP
#define EVT_FTP               1200
#endif

#ifdef FEATURE_BT_EXTPF_BIP
#define EVT_BIP               1300
#endif

#ifdef FEATURE_BT_EXTPF_BPP
#define EVT_BPP               1400
#endif

#ifdef FEATURE_BT_EXTPF_CTP
#define EVT_CTP               1500
#define EVT_ICP               1600
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
#define EVT_A2DP              1800
#define EVT_AVRCP             1900
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
#define EVT_PBAP              2000
#endif

#ifdef FEATURE_IOBEX
#define EVT_OBEX              2100
#endif

#if MIN_BREW_VERSION(3,1)
#define BTAPP_ROOT_DIR        AEEFS_HOME_DIR "btapp"
#else
#define BTAPP_ROOT_DIR        "btapp"
#endif

//Add By zzg 2010_11_19
#ifdef FEATURE_VERSION_M8P
#define BTAPP_PICTURE_DIR	  AEEFS_CARD0_DIR "Gambar"
#define BTAPP_MUSIC_DIR		  AEEFS_CARD0_DIR "Musik"
#else
#define BTAPP_PICTURE_DIR	  AEEFS_CARD0_DIR "pictures"
#define BTAPP_MUSIC_DIR		  AEEFS_CARD0_DIR "music"
#endif
#define BTAPP_OTHER_DIR		  AEEFS_CARD0_DIR
//Add End

#define BTAPP_CONFIG_FILE  BTAPP_ROOT_DIR DIRECTORY_STR "btapp.cfg"


#define AEE_APPSBTAPP_RES_FILE AEE_RES_LANGDIR BTAPP_RES_FILE	//Add By zzg 2010_12_10

//Add By zzg 2011_11_04 
//2者取其一

#define BTAPP_HEADSET_USE_AG		
//#define BTAPP_HEADSET_USE_A2DP

//Add End


#ifdef FEATURE_BT_2_1
#define BTAPP_CONFIG_VERSION     12
#else
#define BTAPP_CONFIG_VERSION     11
#endif

/* bit in AG's uFlags */
#define BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B      1
#define BTAPP_AG_FLAG_AUDIO_WANTED_B              2
#ifdef FEATURE_AVS_BT_SCO_REWORK
#define BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B        4
#define BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B        8
#endif /* FEATURE_AVS_BT_SCO_REWORK */

#undef BT_SWDEV_BT_DISABLED_NV
#ifdef BT_SWDEV_BT_DISABLED_NV
#define BTAPP_DISABLED 1
#define BTAPP_ENABLED  0 
#endif /* BT_SWDEV_BT_DISABLED_NV */

typedef enum
{
  BTAPP_AG_CALL_NONE                    =   0x00,
  BTAPP_AG_CALL_INCOMING_INBAND,
  BTAPP_AG_CALL_INCOMING_NOINBAND,
  BTAPP_AG_CALL_ANSWERED,
  BTAPP_AG_CALL_ORG_REDIAL,
  BTAPP_AG_CALL_ORG_DEVDIAL,
  BTAPP_AG_CALL_ORG_MEMDIAL,
  BTAPP_AG_CALL_ORG_DIAL,
  BTAPP_AG_CALL_STARTVR,
  BTAPP_AG_CALL_ORGINATED,
  BTAPP_AG_CALL_CONNECTED,
  BTAPP_AG_CALL_HANGUP,
  BTAPP_AG_CALL_MAX_STATE   
}BTApp_AGTPCallState;

typedef struct
{
  char*         pPrintFileName;
  char*         pFileDescName;
}BTApp_BIPPrintDetails;

typedef struct
{
  uint16        uMIMMediaType;
  char*         pPrintFileName;
}BTApp_BPPPrintDetails;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

typedef enum
{
  BTAPP_AG  =   0x00,
  BTAPP_L2,
  BTAPP_A2DP,
  BTAPP_AVRCP,
  BTAPP_OPP,
  BTAPP_FTP,
  BTAPP_BIP,
  BTAPP_OBEX,
  BTAPP_BPP,
  BTAPP_PBAP,
  BTAPP_SAP,
  BTAPP_HID,
  BTAPP_NA,
  BTAPP_SPP,
  BTAPP_ALL
}BTApp_ProfileEnabled;

typedef enum
{
  BTAPP_ENABLING_NONE = 0x00,
  BTAPP_ENABLING_AG,
  BTAPP_ENABLING_NA,
#ifdef FEATURE_BT_EXTPF_OPP
  BTAPP_ENABLING_OPP,
#endif //FEATURE_BT_EXTPF_OPP
#ifdef FEATURE_IOBEX
  BTAPP_ENABLING_OBEX,
#endif //FEATURE_IOBEX
#ifdef FEATURE_BT_EXTPF_FTP
  BTAPP_ENABLING_FTP,
#endif //FEATURE_BT_EXTPF_FTP
#ifdef FEATURE_BT_EXTPF_BIP
  BTAPP_ENABLING_BIP,
#endif //FEATURE_BT_EXTPF_BIP
#ifdef FEATURE_BT_EXTPF_BPP
  BTAPP_ENABLING_BPP,
#endif //FEATURE_BT_EXTPF_BPP
#ifdef FEATURE_BT_EXTPF_PBAP
  BTAPP_ENABLING_PBAP,
#endif //FEATURE_BT_EXTPF_PBAP  
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif //FEATURE_BT_EXTPF_SAP
#ifdef FEATURE_BT_EXTPF_AV
  BTAPP_ENABLING_AV,
#endif //FEATURE_BT_EXTPF_AV
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif //FEATURE_BT_EXTPF_HID_HOST
  BTAPP_ENABLING_L2
}BTApp_Enabling;
#ifdef FEATURE_BT_2_1

#define BTAPP_RM_SVC_SEC_DEFAULT AEEBT_RM_SVC_SEC_MED
#endif //FEATURE_BT_2_1

#ifdef FEATURE_BT_HFP_1_5
#define BTAPP_CMD_BUFF_SIZE             200
#define BTAPP_CMD_PARAMS_MAX            15
#define BTAPP_CMD_TERM_CHAR             '\r'
#define BTAPP_CMD_DELIMITER             ';'
#define BTAPP_CMD_PARAM_SEPARATOR       ','
#define BTAPP_RSP_BUFF_SIZE             BTAPP_CMD_BUFF_SIZE

typedef enum
{
  BTAPP_AT_CMD_MIN                      = 0x0000,
  BTAPP_AT_CMD_SET_COPS                 = 0x0000,
  BTAPP_AT_CMD_READ_COPS                = 0x0001,
  BTAPP_AT_CMD_CLCC                     = 0x0002,
  BTAPP_AT_CMD_CNUM                     = 0x0003,
  BTAPP_AT_CMD_CMEE                     = 0x0004,
  BTAPP_AT_CMD_READ_CSCS                = 0x0005,
  BTAPP_AT_CMD_QUERY_CSCS               = 0x0006,
  BTAPP_AT_CMD_SET_CSCS                 = 0x0007,
  BTAPP_AT_CMD_CSQ                      = 0x0008,
  BTAPP_AT_CMD_CBC                      = 0x0009,
  BTAPP_AT_CMD_CIMI                     = 0x000A,
  BTAPP_AT_CMD_BLDN                     = 0x000B,
  BTAPP_AT_CMD_BTRH                     = 0x000C,
  BTAPP_AT_CMD_MEM_DIAL                 = 0x000D,
  BTAPP_AT_CMD_CREG                     = 0x000E,
  BTAPP_AT_CMD_CPMS                     = 0x000F,
  BTAPP_AT_CMD_CMGF                     = 0x0010,
  BTAPP_AT_CMD_SMGL                     = 0x0011,
  BTAPP_AT_CMD_SABD                     = 0x0012,
  BTAPP_AT_CMD_QUERY_CLCC               = 0x0013,
  BTAPP_AT_CMD_QUERY_CPBS               = 0x0014,
  BTAPP_AT_CMD_SET_CPBS                 = 0x0015,
  BTAPP_AT_CMD_CPBR                     = 0x0016,
  BTAPP_AT_CMD_READ_CPBR                = 0x0017,
  BTAPP_AT_CMD_SET_CIND                 = 0x0018,  
  BTAPP_AT_CMD_MAX
}BTApp_ATCmdType;

typedef struct
{
  char                cmd_text[ 11 ];
  uint8               param_cnt;
  BTApp_ATCmdType     cmd_type;
} BTApp_ATCmdTableType;

typedef struct 
{
  BTApp_ATCmdType     cmdType;
  uint8               rx_params_count;
  uint8*              rx_params[BTAPP_CMD_PARAMS_MAX];
  boolean             bParamComplete;
}BTApp_ATParseData;

/* AT+CLCC related */
#define BTAPP_MAX_NUM_CALLS             13
#define BTAPP_CALL_TYPE_NO_MPTY         0
#define BTAPP_CALL_TYPE_MPTY            1
#define BTAPP_TA_INTER_PREFIX           '+'
#define BTAPP_TA_INTERNATIONAL          145 // obtained from CM
#define BTAPP_TA_UNKNOWN                129 // obtained from CM

typedef enum
{
  BTAPP_CALL_DIRECTION_MO               = 0,
  BTAPP_CALL_DIRECTION_MT               = 1
}BTApp_ATCLCCCallDirectionType;

typedef enum
{
  BTAPP_CALL_STATE_ACTIVE               = 0,
  BTAPP_CALL_STATE_HELD                 = 1,
  BTAPP_CALL_STATE_DIALING              = 2,
  BTAPP_CALL_STATE_ALERTING             = 3,
  BTAPP_CALL_STATE_INCOMING             = 4,
  BTAPP_CALL_STATE_WAITING              = 5
}BTApp_ATCLCCCallStateType;

typedef enum
{
  BTAPP_CALL_MODE_VOICE                 = 0,
  BTAPP_CALL_MODE_DATA                  = 1,
  BTAPP_CALL_MODE_FAX                   = 2
}BTApp_ATCLCCCallModeType;

/* AT+COPS related */
#define BTAPP_COPS_FORMAT_LONG          0
#define BTAPP_COPS_FORMAT_SHORT         1
#define BTAPP_COPS_FORMAT_NUMERIC       2
#define BTAPP_COPS_NAME_LEN_LONG        16

typedef enum 
{
  BTAPP_COPS_MODE_AUTO                  = 0,  /* Automatic registration */
  BTAPP_COPS_MODE_MANUAL                = 1,  /* Manual registration */
  BTAPP_COPS_MODE_DEREG                 = 2,  /* Force deregister */
  BTAPP_COPS_MODE_SETFMT                = 3,  /* Set format only */
  BTAPP_COPS_MODE_MANAUTO               = 4,  /* Manual & auto registration */
  BTAPP_COPS_MODE_MAX                        /* Internal use only */
}BTApp_ATCOPSModeType;

/* AT+CNUM related */
#define BTAPP_CNUM_SERVICE_VOICE        4
#define BTAPP_CNUM_SERVICE_FAX          5

/* AT+CMEE related */
typedef enum
{
  BTAPP_CMEE_ERR_DISABLE                = 0x00,
  BTAPP_CMEE_ERR_ENABLE_NUMERIC         = 0x01,
  BTAPP_CMEE_ERR_ENABLE_VERBOSE         = 0x02,
  BTAPP_CMEE_ERR_MAX
}BTApp_ATCMEEErrMode;

typedef enum
{
  BTAPP_CME_NO_ERROR                    =  -1,  /* RESERVED */
  BTAPP_CME_AG_FAILURE                  =   0,  /* phone failure */
  BTAPP_CME_NO_CONNECTION               =   1,  /* no connection to phone */
  BTAPP_CME_OP_NOT_ALLOWED              =   3,  /* operation not allowed */
  BTAPP_CME_OP_NOT_SUPPORTED            =   4,  /* operation not supported */
  BTAPP_CME_PH_SIM_PIN_REQUIRED         =   5,  /* PH-SIM PIN required */
  BTAPP_CME_SIM_NOT_INSERTED            =  10,  /* SIM not inserted */
  BTAPP_CME_SIM_PIN_REQUIRED            =  11,  /* SIM PIN required */
  BTAPP_CME_SIM_PUK_REQUIRED            =  12,  /* SIM PUK required */
  BTAPP_CME_SIM_FAILURE                 =  13,  /* SIM failure */
  BTAPP_CME_SIM_BUSY                    =  14,  /* SIM busy */
  BTAPP_CME_INCORRECT_PASSWORD          =  16,  /* incorrect password */
  BTAPP_CME_SIM_PIN2_REQUIRED           =  17,  /* SIM PIN2 required */
  BTAPP_CME_SIM_PUK2_REQUIRED           =  18,  /* SIM PUK2 required */
  BTAPP_CME_MEMORY_FULL                 =  20,  /* memory full */
  BTAPP_CME_INVALID_INDEX               =  21,  /* invalid index */
  BTAPP_CME_MEMORY_FAILURE              =  23,  /* memory failure */
  BTAPP_CME_TEXT_STRING_TOO_LONG        =  24,  /* text string too long */
  BTAPP_CME_INVALID_CHAR_IN_TEXT        =  25,  /* invalid chars in text */
  BTAPP_CME_DIAL_STRING_TOO_LONG        =  26,  /* dial string too long */
  BTAPP_CME_INVALID_CHAR_IN_DIAL_STRING =  27,  /* invalid chars in dial str */
  BTAPP_CME_NO_NETWORK_SERVICE          =  30,  /* no network service */
  BTAPP_CME_NETWORK_NOT_ALLOWED         =  32,  /* network not allowed 
                                                - emergency calls only */
  BTAPP_CME_MAX
}BTApp_ATCMEEErrCode;

typedef struct
{
  BTApp_ATCMEEErrCode cmeCode;
  char*               cmeText;
}BTApp_CmeTableType;

/* AT+CSCS related*/
typedef enum
{
  BTAPP_ALPHA_IRA                       = 0,
  BTAPP_ALPHA_GSM,
  BTAPP_ALPHA_UCS2,
  BTAPP_ALPHA_HEX,
  BTAPP_ALPHA_8BIT
}BTApp_ATCSCSCharSetType;

typedef struct
{
  char                       char_set[ 10 ];
  BTApp_ATCSCSCharSetType    char_type;
}BTApp_ATCSCSTableType;

/* AT+CSQ related */
#define BTAPP_CSQ_RSSI_TOOLO_CODE       0
#define BTAPP_CSQ_RSSI_TOOHI_CODE       31
#define BTAPP_CSQ_RSSI_UNKNOWN_CODE     99

#define BTAPP_CSQ_RSSI_MIN              51   /* per 3GPP 27.007 */
#define BTAPP_CSQ_RSSI_MAX              113  /* per 3GPP 27.007 */
#define BTAPP_CSQ_RSSI_NO_SIGNAL        125  /* from CM */
#define BTAPP_CSQ_RSSI_OFFSET           182.26
#define BTAPP_CSQ_RSSI_SLOPE            (-100.0/62.0)

#define BTAPP_CSQ_BER_UNKNOWN           99 /* per 3GPP 27.007 */

/* AT+CBC related */
#define BTAPP_CBC_BATTSTAT_POWERED          0
#define BTAPP_CBC_BATTSTAT_CON_NO_POWER     1
#define BTAPP_CBC_BATTSTAT_NOT_CONNECTED    2

#endif // FEATURE_BT_HFP_1_5

#define BTAPP_AUTOANSWER_TIME 7500

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef struct _AG
{
  IBTExtAG* po;

  boolean   bSelected;
  boolean   bInitialized;         /* true if initialized */
  boolean   bEnabled;             /* true if AG is enabled */
  boolean   bConnected;           /* true when ACL connection is up */
  boolean   bSLCUp;               /* true when SLC is up */
  boolean   bStartedVr;           /* true if BT HS button pressed started VR */
  boolean   bAudioSelected;       /* true to select BT HS as output device */
  boolean   bAudioConnected;      /* true if audio connected to BT */
  boolean   bAudioConnecting;     /* true if audio connection is being processed */
  boolean   bAudioDisconnecting;  /* true if audio disconnection is being processed */
  boolean   bForceUnmute;         /* true if snd device has to be unmuted */
  boolean   bVREnabled;           /* true if HF requests VR turned on */
  boolean   bDevPickedUp;         /* true if HS/HF answered incoming call */
  boolean   bDevDialed;           /* true if HS/HF initiated MO call */
  boolean   bInbandRing;          /* true if capable of sending inband ring tone */
  boolean   bUpdateVrState;       /* true if vr state needs to be updated */
  boolean   bIgnoreButton;        /* true if button press should be ignored */
  boolean   bPrivateMode;         /* true if HF is in private mode */
  boolean   bconnInPrgs;          /* true if AG connection is in progress */

  uint8             spkrGain;
  uint8             micGain;

  AEEBTBDAddr       bdAddr;
  AEEBTBDAddr       connectedBDAddr;
  AEEBTAudioDevice  devType;
  AEEBTIdleMode     idleMode;
  uint16            idleTimeout; /* in seconds */
  uint32            uFlags;

#ifdef FEATURE_BT_VT
  IVideoPhone*      pIVP;
  boolean           bVideoCallEnabled;
#endif /* FEATURE_BT_VT */
#ifdef FEATURE_UI_CORE
  boolean           bVolumesInit;
#endif //FEATURE_UI_CORE
  uint8             uNumCalls;
  AECHAR            wDialString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  BTApp_AGTPCallState   callState; /* TP call state */
  AEEBTSoundType        uSoundType; /* Sound type played by AVS */
#ifdef FEATURE_BT_HFP_1_5
  AEECallback       externalIOCb;
  uint8             externalIOBuffer[ BTAPP_CMD_BUFF_SIZE + 1 ];
  AEETSSInfo*       pSSInfo;
  ICallMgr*         pICallMgr;
  ICall*            pIncomingCall;
  ICall*            pOutgoingCall;
  BTApp_ATParseData parseData;
  boolean           bValidSSData;
  uint8             nwOperatorName[ BTAPP_COPS_NAME_LEN_LONG + 1 ];
  IBattery*            pIBattery;
  ICard*            pICard;
  AEECallback       cardCallback;
  AEECardGenericCmdStatus    *pCardCmdStatus;
  BTApp_ATCMEEErrMode        errATMode;
  BTApp_ATCSCSCharSetType    charSetType;
#endif /* FEATURE_BT_HFP_1_5 */
#ifdef FEATURE_BT_2_1
  boolean                bSecSelectedAll;
  boolean                bSecSelected;
  AEEBTSvcSecLevel       srvSecType;
  boolean                bAuthorize;
  boolean                bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  
} AG;

typedef struct _NA
{
  IBTExtNA*   po;

  boolean     bSelected;
  boolean     bEnabled;
  boolean     bConnected;
  boolean     bEnableSvcSec;
  boolean     bInitialized;         /* true if initialized */

  AEEBTBDAddr connectedBDAddr;
#ifdef FEATURE_BT_2_1
  boolean            bSecSelected;
  AEEBTSvcSecLevel   srvSecType;
  boolean            bAuthorize;
  boolean            bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  
} NA;

typedef struct _SD
{
  IBTExtSD*           po;

  AEEBTDeviceRecord*  devRec;
  uint8               uNumRecs;
  uint8               uCurRecIdx;
  boolean             bDiscovering;
  boolean             bDiscoverable;

  AEEBTBrowseRecord*  browseRec;
  uint8               uNumBrowseRecs;

  AEEBTServiceRecord* svcRec;
  uint8               uNumSvcRecs;

} SD;

typedef struct _RM
{
  IBTExtRM*              po;

  AEEBTLocalInfo         myInfo;
  AEEBTDeviceInfo*       device;
#ifdef FEATURE_BT_2_1
  uint8                  uDevToDspIdxMap[MAX_DEVICES];
#endif /* FEATURE_BT_2_1 */
  uint8                  uCurDevIdx;
  uint8                  uGetNameDevIdx; // GetName is being done on this dev

  AECHAR                 wPassKey[ MAX_PASSKEY_LEN ];
  boolean                bBondable;
  boolean                bBonding;
  boolean                bRadioOn;
  boolean                bVisibilityOn;
  boolean                bRoleSwitchAllowed;

  AEEBTServiceIdentifier svcId;
  AEEBTSecurity          minSec;
#ifdef FEATURE_BT_2_1
  AEECallback            OOBCreateCb;
  uint8                  OOBDataSize;
#endif /* FEATURE_BT_2_1 */
  AEECallback            radioActivityCb;
  AEEBTRadioActivity     radioActivity;
  AEEBTDeviceInfo*       devToSwitchRole;
  AEEBTLinkStatus*       linkStatus;
 
  AEEBTActVsPwr          pageScanParam;
  AEEBTActVsPwr          inquiryScanParam;
  //BT2.1
#ifdef FEATURE_BT_2_1
  AECHAR                 wManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ];
  AEEBTIOCapType         ioCaptype;
  AEEBTSvcSecLevel       srvSecType;
  boolean                bMITMEnabled;
  boolean                bRebondOptMITMEnabled;
  boolean                bRebondOptBONDEnabled; 
  boolean                bDBGEnabled;
  AEEBTSvcSecLevel       security;
  AEEBTBDAddr            BondBDAddr;
  AECHAR                 wSSPPassKey[ AEEBT_SSP_PASSKEY_LEN ];
  AECHAR                 wDisplayPassKey[ AEEBT_SSP_PASSKEY_LEN ];
  boolean                bUserCfm;
  boolean                bPassKey;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */  
  boolean                bpassKeyRqst;
  boolean                bAuthorize;
  boolean                bAuthorizeFirst;
#endif /* FEATURE_BT_2_1 */
} RM;

typedef struct _SPP
{
  IBTExtSPP*      po;
  boolean         bClient;

#ifdef FEATURE_BT_2_1
  boolean   bSecSelected;
  AEEBTSvcSecLevel  srvSecType;
  boolean           bAuthorize;
  boolean           bAuthorizeFirst;

#endif

  AEEBTSppStatus  status;

  AEECallback     writeableCb;
  AEECallback     readableCb;

  uint32          uNumDataBufs; // 1K, 1M, or 1G
  uint32          uTotalBytesSent;
  uint32          uTotalBytesRcvd;
  boolean         bDoVerify;

  uint8           buf[SPP_BUF_SZ];
  uint32          buf_start;
  uint32          buf_end;
  uint32          buf_count;

} SPP;

typedef struct _L2
{
  IBTExtL2*          pSrvO;
  IBTExtL2*          pCliO;
  boolean            bReleasing;
  uint8*             pMem;
  AEEBTServiceRecord sSvcRec;

  uint32            uDialogType;
  AECHAR            wControlData[ MAX_CONTROL_DATA_LEN + 1 ];

  boolean           bEnableL2Srv;
  boolean           bEnabled;
  boolean           bEnableSrvSvcSec;
  boolean           bEnableCliSvcSec;
  boolean           bSrvConnected;
  boolean           bCliConnected;
  boolean           bCliConnecting;
#ifdef FEATURE_BT_2_1
  boolean           bSecSelected;
  AEEBTSvcSecLevel  srvSecType;
  boolean           bAuthorize;
  boolean           bAuthorizeFirst;
#endif
  int32             dwSettingSec;
  boolean           bDoSDP;
  AEEBTL2ConfigInfo sSrvInfo;
  AEEBTL2ConfigInfo sCliInfo;
  AEEBTBDAddr       SrvBDAddr;
  AEEBTBDAddr       CliBDAddr;

  uint32          uSrvTotalBytesRcvd;
  boolean         bSrvDoVerify;
  uint32          uSrvPacketsRcvd;

  uint32          uCliBytesToSend;
  uint32          uCliTotalBytesSent;
  uint32          uCliStartTime;
  uint32          uCliPacketsSent;

  uint8           buf[SPP_BUF_SZ];
  uint32          buf_start;
  uint32          buf_end;
  uint32          buf_count;
} L2;

#ifdef FEATURE_BT_EXTPF_AV
typedef struct _A2DP
{
  IBTExtA2DP*     po;
  AEEBTBDAddr     bdAddr;
  boolean         bEnabled;
  boolean         bInitialized;  /* true if initialized */
  boolean         bVolumesInit;
  boolean         bConnected;
  boolean         bStreaming;
  boolean         bAudioSelected;
  AECHAR          wControlData[ MAX_CONTROL_DATA_LEN + 1 ];
  uint32          uControlEvtID;
  boolean         bCloseOnEnd;
  boolean         bHonorInUse;
  uint32          uCtrlData;
  AECHAR          wPassKey[ MAX_PASSKEY_LEN ];  
#ifdef FEATURE_BT_2_1
  boolean           bSecSelected;
  AEEBTSvcSecLevel  srvSecType;
  boolean           bAuthorize;
  boolean           bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  boolean         bEnableA2DP;
  boolean         bReleasing;
} A2DP;

typedef struct _AVRCP
{
  IBTExtAVRCP*           po;
  AEEBTBDAddr            bdAddr;
  boolean                bConnected;
  boolean                bEnabled;
  AEEBTAVRCPEventType    evtTable[ MAX_AVRCP_EVENTS ];
  AEEBTAVRCPOpStatusType evtType[ MAX_AVRCP_EVENTS ];
  uint32                 evtStart;
  uint32                 evtEnd;
  AECHAR                 wPassKey[ MAX_PASSKEY_LEN ];  
#ifdef FEATURE_BT_2_1
  boolean                bSecSelected;
  AEEBTSvcSecLevel       srvSecType;
  boolean                bAuthorize;
  boolean                bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  boolean                bEnableAVRCP;
  boolean                bReleasing;
} AVRCP;
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_OPP
typedef struct _OPP
{
  IBTExtOPP*      po;
  IFileMgr*       pIFileMgr;
  IFile*          pIFile;
  boolean         bRegistered;
  boolean         bConnected;
  boolean         bConnecting;
  boolean         bEndingTest;
  boolean         bExchanging;
  AECHAR          wPassKey[ MAX_PASSKEY_LEN ];
#ifdef FEATURE_BT_2_1
  boolean          bSecSelected;
  AEEBTSvcSecLevel srvSecType;
  boolean          bAuthorize;
  boolean          bAuthorizeFirst;
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

  AEEBTBDAddr     remoteBDAddr;
  AEEBTObjectType objType;
  uint16          wName[ AEE_MAX_FILE_NAME ];
  boolean         bServerEnable;
  boolean         bObjectTransfer;
  boolean         bPushFileReq;
} OPP;
#endif

#ifdef FEATURE_BT_EXTPF_FTP
typedef struct
{
  char             szFolderFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  AEEBTFTPObject   szTypeOfObj;  // Type of object - Folder(1) or File(0)
} BTApp_FTPObjAttr;

typedef struct _FTP
{
  IBTExtFTP*            po;
  IFileMgr*             pIFileMgr;
  boolean               bRegistered;
  boolean               bAuthenticated;
  boolean               bConnected;
  boolean               bEndingTest;
  boolean               bUserIDRequired;
  boolean               bDoAuthenticate;
  char                  szCurrentFolder[ AEEBT_MAX_FILE_NAME * 2 + 1 ];
  char                  szName[ AEEBT_MAX_FILE_NAME * 2 + 1 ];
  AEEBTBDAddr           remoteBDAddr;
  AEEBTFTPRegistration  reg;
  AECHAR                wPassWord[ AEEBT_MAX_PASSWORD_LEN ];
  BTApp_FTPObjAttr      localObjects[ MAX_OBJECTS ];
  BTApp_FTPObjAttr      remoteObjects[ MAX_OBJECTS ]; 
  uint8                 uNumOfObjInLocalDev;    // MAX is 256;
  uint8                 uNumOfObjInRemoteDev;   // MAX is 256;
  boolean               bRemoteBrowsing;
  // Stores local device browsing path
  char                  szLocalBrowsePath[ AEEBT_MAX_FILE_NAME * 2 + 1 ];
  /* Stores folder name for CREATE_FOLDER and MOVE_TO_FOLDER options */
  AECHAR                wFolderName[ AEEBT_MAX_FILE_NAME + 1 ]; 
  boolean               bObjectTransfer;
  boolean               bServerEnable;
  AECHAR                wPassKey[ MAX_PASSKEY_LEN ];  
  uint8                 u8StorageDevice;
  uint32                objSize; // size of object in Put request
#ifdef FEATURE_BT_2_1
  boolean               bSecSelected;
  AEEBTSvcSecLevel      srvSecType;
  boolean               bAuthorize;
  boolean               bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
} FTP;
#endif

#ifdef FEATURE_BT_EXTPF_BIP
typedef struct _BIP
{
  IBTExtBIP*      po;
  IFileMgr*       pIFileMgr;
  IFile*          pIFile;

  FileInfo        fileInfo;
// Primary server/connection
  boolean         bRegistered;
  boolean         bConnected;

  // Secondary server/connection
  boolean         bSecRegistered;
  boolean         bSecConnected;

  boolean         bEndingTest;

  AEEBTBDAddr     remoteBDAddr;

  // Printer Address to use
  AEEBTBDAddr           printerBDAddr;
  BTApp_BIPPrintDetails printDetails;
  boolean               bPrintFile;
  
  // Authentication related
  boolean         bDoAuthenticate;
  boolean         bAuthenticated;
  AECHAR          wPIN[ AEEBT_MAX_PASSWORD_LEN + 1 ];

  // Client buffers
  char            imageDesc[ AEEBT_BIP_MAX_IMAGE_DESC_LEN ];
  char            attachmentDesc[ AEEBT_BIP_MAX_IMAGE_DESC_LEN ];
  uint16          descLen;

  AECHAR          wImageHandle[ AEEBT_BIP_IMAGE_HANDLE_LEN + 1 ];
  AECHAR          wAttachmentName[ AEEBT_MAX_FILE_NAME + 1 ];

  // Buffer for storing xml strings
  char*           pXMLdata;
  uint16          XMLdataLen;
  uint16          XMLdataMaxSize;

  char            clientXMLdata[ AEEBT_BIP_MAX_IMAGE_DESC_LEN ];
  uint16          clientXMLdataLen;

  // GetImageList params
  uint16          handleCount;
  uint16          startOffset;
  boolean         bLatest;

  // GetMonitoringImage param
  boolean         bStoreFlag;

  // GetPartialImage params
  uint32          partialLength;
  uint32          partialOffset;
  AECHAR          wImageName[ AEEBT_MAX_FILE_NAME + 1 ];

  // RemoteDisplay param
  uint8           displayCommand;

  // Imaging server type
  AEEBTBIPServerType responderType;

  // ServiceID of secondary OBEX server (archived objects, referenced objects)
  AEEBTBIPSDUUID128type serviceID;

  // Cleanup flag
  boolean         bCleanup;

  //Tracking the core activity ongoing: PUSH PULL related
  boolean            bOpInProgress;

  // Default server selection
  AEEBTBIPServiceType   serviceSelection;
  boolean         bServerEnable;
  AECHAR          wPassKey[ MAX_PASSKEY_LEN ];
#ifdef FEATURE_BT_2_1
  boolean           bSecSelected;
  AEEBTSvcSecLevel  srvSecType;
  boolean           bAuthorize;
  boolean           bAuthorizeFirst;
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
} BIP;
#endif

#ifdef FEATURE_IOBEX
typedef struct _OBEX
{
  IOBEX*             poServer;
  IOBEX*             poClient;
  IFileMgr*          pIFileMgr;
  IFile*             pIFile;

  FileInfo           fileInfo;

  boolean            bRegistered;
  boolean            bConnected;
  boolean            bEndingTest;

  AEEBTBDAddr        remoteBDAddr;

  // Cleanup flag
  boolean            bCleanup;
#ifdef FEATURE_BT_2_1
  boolean            bSecSelected;
  AEEBTSvcSecLevel   srvSecType;
  boolean            bAuthorize;
  boolean            bAuthorizeFirst;
#endif

  // Default server selection
  AEEBTBIPServiceType   serviceSelection;
  boolean            bServerEnable;
  AECHAR             wPassKey[ MAX_PASSKEY_LEN ];
  
  // Authentication & Connection
  uint8              ucState;
  //  Authentication related
  boolean            bDoAuthenticate;
  boolean            bAuthenticated;
  uint8              u8SCN;
  AEEBTBDAddr        mRemoteBDAddr;
  boolean            bAuthReqd;
  boolean            bAuthRequested;
  boolean            bDoAuthWithUid;
  boolean            bUidReqd;
  boolean            bFullAccess;
  boolean            bVerifyUID;
  IModel*            pIModel;
  ModelListener      sModelListener;
  char               szUIDForVerification[ AEEBT_MAX_USERID_LEN*2 + 1 ];
  AECHAR             wUserID[ AEEBT_MAX_USERID_LEN + 1 ];
  AECHAR             wPassword[ AEEBT_MAX_PASSWORD_LEN + 1 ];
  // Put, Get related
  AEECallback        m_cb;
  uint32             bytesSent;
  uint32             bytesRcvd;
  uint32             fileSize;
  boolean            bPutInProgress;
  boolean            bGetInProgress;
  char               szCurrentFolder[ AEEBT_MAX_FILE_NAME * 2 + 1 ];
  char               szName[ AEEBT_MAX_FILE_NAME * 2 + 1 ];
  byte               dataBuffer[ OBEX_DATA_BUFFER_SIZE ];
  boolean            bPendingDisc;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
} OBEX;
#endif

#ifdef FEATURE_BT_EXTPF_BPP
typedef struct _BPP
{
  IBTExtBPP*      po;
  IFileMgr*       pIFileMgr;
  IFile*          pIFile;
  boolean         bRegistered;

  boolean         bOKToSend;

  // Job channel and Status channel
  boolean         bConnected;
  boolean         bSTSConnected;
  
  // Object channel
  boolean         bSecConnected;

  // Authentication related
  boolean         bDoAuthenticate;
  boolean         bAuthenticated;
  boolean         bAuthRequested;
  boolean         bUserIDRequired;
  AECHAR          wUserID[ AEEBT_MAX_USERID_LEN + 1 ];
  AECHAR          wPassword[ AEEBT_MAX_PASSWORD_LEN + 1 ];

  boolean         bEndingTest;
  uint32          iJobId;
  AECHAR          wJobId[ MAX_JOB_ID_LEN+1 ];
  uint16          iDocType;

  AEEBTBDAddr     remoteBDAddr;

  // Printer Address to use
  AEEBTBDAddr           printerBDAddr;
  BTApp_BPPPrintDetails printDetails;
  boolean               bPrintFile;
  
  // Target type (DPS, STS)
  AEEBTBPPTargetType target;

  // Sender's SOAP msg 
  char*           pSndrSOAPmsg;
  uint16          sndrSOAPmsgLen;
  uint16          sndrSOAPmsgMaxLen;

  // Printer's SOAP msg
  char*           pSOAPmsg;
  uint16          SOAPmsgLen;
  uint16          SOAPmsgMaxLen;

  // GetReferenceObject parameters
  AECHAR          wRefObjName[ AEEBT_MAX_FILE_NAME + 1];
  uint32          offset;
  int32           count;

  // Menu selection
  uint16          sel;
  boolean         bServerEnable;  
  AECHAR          wPassKey[ MAX_PASSKEY_LEN ];
#ifdef FEATURE_BT_2_1
  boolean           bSecSelected;
  AEEBTSvcSecLevel  srvSecType;
  boolean           bAuthorize;
  boolean           bAuthorizeFirst;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  // FEATURE_APP_TEST_AUTOMATION 
} BPP;
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
typedef struct
{
  char            szObjHdleName[ MAX_HANDLE_NAME_LEN + 1 ];
  char            szObjName[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean         bFolder;
} BTApp_PBAPObjAttr;

typedef struct _PBAP
{
  IBTExtPBAP*     po;
  IFileMgr*       pIFileMgr;
  IFile*          pIFile;
  IAddrBook*      pAddrbook;  //IADDRBOOK interface
  AEEBTBDAddr     remoteBDAddr;
  ICallHistory*   pCallHist;  
  boolean         bSrvRegistered;
  boolean         bCliRegistered;
  boolean         bConnected;
  boolean         bServerEnable;
  boolean         bIsServer;

  uint16          phoneBookSize;
  uint16          numMissedCalls;

  // Authentication related
  boolean         bAuthRequested;
  boolean         bAuthUserID;
  boolean         bAuthPwd;
  boolean         bDoAuthenticate;
  AEEBTPBAPAuthentication     authOptions;
  AEEBTPBAPFormat vCardFormat;
  AEEBTPBAPOrder  sortOrder;
  AEEBTPBAPSearchAttrib       searchAttrib;

  // Request phonebook object name
  char            szObjectName[ AEEBT_MAX_FILE_NAME + 1 ];

  AECHAR          wFilterValue[ MAX_FILTER_VAL_LEN + 1 ];
  AECHAR          wMaxListCount[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  AECHAR          wListStartOffset[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  AECHAR          wSearchValue[ MAX_PASSKEY_LEN + 1 ];
  AEEBTPBAPObject PBAPObject;
  AEEBTPBAPRepositories       pbRepository;
  // browsing related
  uint8                       uNumOfObj;
  BTApp_PBAPObjAttr           pbObjects[ MAX_PBAP_OBJECTS ];
  AEEBTPBAPSetPathFlag        setPathFlag;
  char                        sSetFolder[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean                     bListOneLevelAbove;
  AECHAR                      wPassword[ AEEBT_MAX_PASSWORD_LEN + 1 ];
  boolean                     bUserIDRequired;
  AECHAR                      wUserID[ AEEBT_MAX_USERID_LEN + 1 ];
  AECHAR                      wCliUserID[ AEEBT_MAX_USERID_LEN + 1 ];
#ifdef FEATURE_BT_2_1
  boolean                     bSecSelected;
  AEEBTSvcSecLevel            srvSecType;
  boolean                     bAuthorize;
  boolean                     bAuthorizeFirst;
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
} PBAP;
#endif

#ifdef FEATURE_BT_EXTPF_CTP
typedef struct _CTP
{
  IBTExtCTP*      po;

  boolean         bRegistered;
  boolean         bConnected;
  boolean         bEndingTest;
  boolean         bCallEstablished;

  // Outgoing call status
  AEEBTCTPCallStatusType status;

  AEEBTBDAddr     remoteBDAddr;
} CTP;

typedef struct _ICP
{
  IBTExtICP*      po;

  boolean         bRegistered;
  boolean         bConnected;
  boolean         bEndingTest;
  boolean         bCallEstablished;

  // Whether we are initiator or acceptor of a call
  boolean         bCallingParty;

  // Outgoing call status
  AEEBTICPCallStatusType status;
  
  AEEBTBDAddr     remoteBDAddr;
} ICP;
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

typedef enum
{
  BT_APP_MENU_NONE,             // 0
  BT_APP_MENU_MAIN,             // 1
  BT_APP_MENU_SEARCH,           // 2
  BT_APP_MENU_DEVICE,           // 3
  BT_APP_MENU_MY_INFO,          // 4
  BT_APP_MENU_SETTINGS,         // 5
  BT_APP_MENU_TESTS,            // 6
  BT_APP_MENU_EDIT_NAME,        // 7
  BT_APP_MENU_AG_SETTINGS,      // 8
  BT_APP_MENU_ENABLING,         // 9
  BT_APP_MENU_MASTER_CONTROL,   // 10
  BT_APP_MENU_CONN_MODE,        // 11
  BT_APP_MENU_IDLE_TO,          // 12
  BT_APP_MENU_SPKR_VOL,         // 13
  BT_APP_MENU_MIC_VOL,          // 14
  BT_APP_MENU_LIST_BONDED,      // 15
  BT_APP_MENU_LIST_ALL,         // 16
  BT_APP_MENU_UNBOND_ALL,       // 17
  BT_APP_MENU_REMOVE_ALL,       // 18
  BT_APP_MENU_DEV_RESP,         // 19
  BT_APP_MENU_DEV_INFO,         // 20
  BT_APP_MENU_PASSKEY,          // 21
  BT_APP_MENU_BROWSE_SVC_RESP,  // 22
  BT_APP_MENU_SEARCH_SVC_RESP,  // 23
  BT_APP_MENU_UNBOND_ONE,       // 24
  BT_APP_MENU_REMOVE_ONE,       // 25
  BT_APP_MENU_USE_AUDIO_DEV,    // 26
  BT_APP_MENU_SPP_RESULTS,      // 27
  BT_APP_MENU_DEV_TYPE,         // 28
  BT_APP_MENU_DEV_SECURITY,     // 29
  BT_APP_MENU_SECURITY,         // 30
  BT_APP_MENU_VR_CAPABLE,       // 31
  BT_APP_MENU_AG_CONNECT,       // 32
  BT_APP_MENU_AG_DISCONNECT,    // 33
  BT_APP_MENU_DEV_OPTIONS,      // 34
  BT_APP_MENU_HCI,              // 35
  BT_APP_MENU_DISCOVERABLE,     // 36
  BT_APP_MENU_LIST_OBEX_SERVERS,// 37
  BT_APP_MENU_NA_SETTINGS,      // 38
  BT_APP_MENU_AUTHORIZE_CONN,   // 39
  BT_APP_MENU_TEST_MODE,        // 40
  BT_APP_MENU_OBEX,             // 41
  BT_APP_MENU_TCS,              // 42
  BT_APP_MENU_LIST_TELEPHONY,   // 43
  BT_APP_MENU_DFLT_DIAL_STR,    // 44
  BT_APP_MENU_SPP_TESTS,        // 45
  BT_APP_MENU_SPP_SETTINGS,     // 46
  BT_APP_MENU_SET_CONN_ROLE,    // 47
  BT_APP_MENU_HS_BUTTON_HNDLR,  // 48
  BT_APP_MENU_SCAN_PARAMS,      // 49
  BT_APP_MENU_PAGE_SCAN,        // 50
  BT_APP_MENU_INQ_SCAN,         // 51
  BT_APP_MENU_LIST_CONNECTED,   // 52
#ifdef FEATURE_BT_2_1
  BT_APP_MENU_IOC_TYPE,         // 53
  BT_APP_MENU_SVR_SEC_MAIN_TYPE, //54                                // 
  BT_APP_MENU_SVR_SEC_TYPE,     // 55
  BT_APP_MENU_REBOND,           // 56
  BT_APP_MENU_DEV_BOND_OPTIONS, // 57
  BT_APP_MENU_USER_CFM_RQST,    // 58
  BT_APP_MENU_DISPLAY_PASSKEY,  // 59
  BT_APP_MENU_SNIF_PARAMS,      // 60
  BT_APP_MENU_LOW_POWER_MODE,
  BT_APP_MENU_MAX_INTERVAL,
  BT_APP_MENU_IDLE_TIMEOUT,
  BT_APP_MENU_MAX_LATENCY,
  BT_APP_MENU_SNIF_PARAM_SETTINGS,
  BT_APP_MENU_EDIT_SHORT_NAME,  // 65
  BT_APP_MENU_EDIT_MANU_DATA,   // 66
  BT_APP_MENU_SNIFF_PROFILES,   // 67
  BT_APP_MENU_SECURITY_OPTIONS,
  BT_APP_MENU_DBG_KEY,
  BT_APP_MENU_OPEN,				//Add By zzg 2010_11_17
  BT_APP_MENU_ON,				//Add By zzg 2011_05_25
  BT_APP_MENU_OFF,				//Add By zzg 2011_05_25
  BT_APP_OOB_CREATED,			//Add By zzg 2011_05_28
  BT_APP_WAITING,				//Add By zzg 2012_11_28
#endif

#ifdef FEATURE_BT_EXTPF_OPP
  BT_APP_MENU_OPP_TESTS         = 100,
  BT_APP_MENU_OPP_SERVER,
  BT_APP_MENU_OPP_CLIENT,
  BT_APP_MENU_OPP_LIST_FILE_TYPES,
  BT_APP_MENU_OPP_SENDFILE,					//Add By zzg 2010_11_09
  BT_APP_MENU_OPP_SETTING,					//Add By zzg 2010_11_17  
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  BT_APP_MENU_FTP_TESTS         = 200,
  BT_APP_MENU_FTP_SERVER,
  BT_APP_MENU_FTP_CLIENT,
  BT_APP_MENU_FTP_SETTINGS,
  BT_APP_MENU_FTP_BROWSING, 
  BT_APP_MENU_FTP_PASSWORD,
  BT_APP_MENU_FTP_CREATE_FLDR, 
  BT_APP_MENU_FTP_SRV_REGISTER,
  BT_APP_MENU_FTP_MOVE_TO_FLDR,
  BT_APP_MENU_FTP_ENABLING, 
#endif
#ifdef FEATURE_BT_EXTPF_BIP
  BT_APP_MENU_BIP_TESTS         = 300,
  BT_APP_MENU_BIP_SERVER,
  BT_APP_MENU_BIP_CLIENT,
  BT_APP_MENU_BIP_SETTINGS,
  BT_APP_MENU_BIP_PIN,
  BT_APP_MENU_BIP_ENABLING,
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  BT_APP_MENU_BPP_TESTS         = 400,
  BT_APP_MENU_BPP_SETTINGS,
  BT_APP_MENU_BPP_PASSWORD,
  BT_APP_MENU_BPP_LIST_MIME,
  BT_APP_MENU_BPP_LIST_TARGET,
  BT_APP_MENU_BPP_JOB_ID,
  BT_APP_MENU_BPP_USERID,
#endif
#ifdef FEATURE_BT_EXTPF_CTP
  BT_APP_MENU_CTP_TESTS         = 500,
  BT_APP_MENU_ICP_TESTS,
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  BT_APP_MENU_A2DP_TEST         = 700,
  BT_APP_MENU_A2DP_CONTROL,
  BT_APP_MENU_A2DP_CONTROL_DATA,
  BT_APP_MENU_AVRCP_TEST        = 800,
  BT_APP_MENU_AVRCP_OP_LIST,
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  BT_APP_MENU_PBAP_TESTS        = 900,
  BT_APP_MENU_PBAP_SERVER,
  BT_APP_MENU_PBAP_CLIENT,
  BT_APP_MENU_PBAP_SERVER_SETTINGS,
  BT_APP_MENU_PBAP_CLIENT_SETTINGS,
  BT_APP_MENU_PBAP_VCARD_FORMAT,
  BT_APP_MENU_PBAP_FILTER,
  BT_APP_MENU_PBAP_MAX_LIST_COUNT,
  BT_APP_MENU_PBAP_LIST_START_OFFSET,
  BT_APP_MENU_PBAP_SORT_ORDER,
  BT_APP_MENU_PBAP_SEARCH_ATTR,
  BT_APP_MENU_PBAP_SEARCH_VALUE,
  BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS,
  BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES,
  BT_APP_MENU_PBAP_CLIENT_DOWNLOAD,
  BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU,
  BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU,
  BT_APP_MENU_PBAP_AUTHENTICATE,
  BT_APP_MENU_PBAP_USERID,
  BT_APP_MENU_PBAP_PASSWORD,
#endif
#ifdef FEATURE_IOBEX
  BT_APP_MENU_OBEX_TESTS         = 1000,
  BT_APP_MENU_OBEX_SERVER,
  BT_APP_MENU_OBEX_CLIENT,
  BT_APP_MENU_OBEX_USERID,
  BT_APP_MENU_OBEX_PASSWORD,
  BT_APP_MENU_OBEX_SETTINGS,
  BT_APP_MENU_OBEX_ENABLING,
#endif
  BT_APP_MENU_L2_TEST            = 1100,
  BT_APP_MENU_L2_RESULTS,

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  BT_APP_MENU_MAX
} BTAppMenuType;

/*
typedef struct _CBTApp
{
  DECLARE_VTBL(IBTApp)
  	
  AEEApplet       a;
  IDisplay*       m_pIDisplay;

  AEERect         m_rect;
  uint16          m_nColorDepth;

  //Add By zzg 2011_01_04
  uint32           m_nRefs;            // 对象引用计数器
  IModule          *m_pModule;             
  IDialog          *m_pActiveDlg;      // 当前活动对话框ID
  uint16           m_pActiveDlgID;     // 活动对话框关闭时的返回结果
  DLGRet_Value_e_Type   m_eDlgRet;     // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
  boolean          m_bNotOverwriteDlgRet;// Applet 前一状态
  BTAppState 		m_ePreState;        // Applet 当前状态
  BTAppState 		m_eCurState;        // Applet是否处于挂起状态
  boolean          m_bSuspending;      // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
  boolean          m_bAppIsReady;
  uint16           m_nSubDlgId;		    //次级菜单位置
  //Add End

  IMenuCtl*       m_pIMenu;
  IMenuCtl*       m_pISoftMenu;
  IStatic*        m_pStatic;
  ITextCtl*       m_pText;

  IAnnunciator    *m_pIAnn;		//Add By zzg 2010_10_29
  IConfig         *m_pConfig;	//Add By zzg 2010_11_23 

  BTAppMenuType   menuStack[ MENU_STACK_DEPTH ];
  uint16          uMenuSel[ MENU_STACK_DEPTH ];

  char			  m_pfilepath[AEEBT_MAX_FILE_NAME];
  
  uint8           uTopMenu;
  BTApp_Enabling  mEnablingType;
  AG              mAG;
  NA              mNA;
  SD              mSD;
  RM              mRM;
  SPP             mSPP;
  L2              mL2;
#ifdef FEATURE_BT_EXTPF_AV
  A2DP            mA2DP;
  AVRCP           mAVRCP;
#endif
#ifdef FEATURE_BT_EXTPF_OPP
  OPP             mOPP;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  FTP             mFTP;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  //FEATURE_APP_TEST_AUTOMATION


#ifdef FEATURE_BT_EXTPF_BIP
  BIP             mBIP;
#endif
#ifdef FEATURE_IOBEX
  OBEX            mOBEX;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  BPP             mBPP;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  PBAP            mPBAP;
#endif
#ifdef FEATURE_BT_EXTPF_CTP
  CTP             mCTP;
  ICP             mICP;
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  ITelephone*     pIPhone;
  boolean         bFirstLaunch;
  boolean         bEnableAtStart;
  boolean         bSuspended;
  boolean         bBusyIconUp;
  boolean         bVocoderOn;
  boolean         bConfigChanged;
  uint16          uCurrMsgId;
  
  boolean		  bStartFromOtherApp;				//Add By zzg 2010_11_09  
  boolean		  bUpdateProgress;					//Add By zzg 2010_11_27
  
  AECHAR*         pText1;
  AECHAR*         pText2;
  uint8*          pMem;
#ifdef FEATURE_BT_2_1
  boolean         bRemoteOOBRead; 
#endif
  boolean         bStartBTaplication;
} CBTApp;



// Dialog Event Handler function pointer structure 
typedef boolean (*PFNDLGHANDLER) (void *pUser,AEEEvent evt,uint16 w,uint32 dw );

extern boolean BTApp_BuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern boolean BTApp_BuildTopMenu( CBTApp* pMe );
extern boolean BTApp_HandleClearKey( CBTApp *pMe );
extern boolean BTApp_WriteConfigFile( CBTApp* pMe );
extern boolean BTApp_AGInit( CBTApp *pMe );
extern void    BTApp_AGDeInit( IApplet *pi  );
extern void    BTApp_DeregisterAGNotif( CBTApp *pMe );
extern void    BTApp_EnableBT( CBTApp* pMe );

extern const AEEBTBDAddr NULL_BD_ADDR;
*/

#endif

