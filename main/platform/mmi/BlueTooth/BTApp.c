/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTApp.c                                    */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This applet is the BT App of 3GUI.            */  
/*                                                                           */  
/*              Copyright (c) 2003-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTApp.c#8 $
$DateTime: 2009/07/31 10:36:42 $
$Author: cou $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
07/31/09    co  To bring up SCO for MOS when AG connection is established.
06/11/09    ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
07/08/09    pn  Changed how A2DP events can affect AG audio.
06/11/09    ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
06/01/09    pn  Fixed critical Lint errors.
03/24/09    co  Made change to call new version of BTApp_DoSpeedDialLookup().
03/16/09    co  Fixed that pending audio disconnect operation did not get
                carried out.
12/04/08    co  To retrieve SCO after BT SCO device is re-selected by the UI
                during an audio session.
11/25/08    co  Not to send out-of-band ringers upon AEEBT_SND_DTMF_START.
11/21/08    co  Fixed improper SCO manipulation for inband ring.
11/21/08    ssk Handle A2DP\AVRCP disconnect evts after switching brew context.
11/13/08    co  Sync up BTApp menu update with AG/NA user events.
10/17/08    co  Replaced BT_SSR_LPC_DEBUG with FEATURE_BT_TEST_API.
10/13/08    vm  Refreshing display on incoming pairing request yes/no
09/22/08    co  Fixed improper IBTEXTAG API calls before the AG object is
                created.
09/16/08    co  Added error checking to last check-in.
09/15/08    co  Cleaned up External I/O's memory reference before the AG object
                is released.
09/11/08    gs  Removed DeviceAdd/Remove logic prior to bonding.
09/08/08    vm  Moved pIphone create instance and Itelephone event registration 
                inside AG_Init()
08/29/08    vk  Reading name in BTApp_BuildDevInfo only once.
08/20/08    co  Release BT device if SCO is terminated while calls present.
08/07/08    vm  De-registering AG and NA notification outside notifier call. 
                Initializing and De-Initializing AG and NA when enabled/disabled.
                Added support to allow other BREW apps to Select/Deselect BT 
                snd device by posting events.
07/28/08    sa  Resolved compiler warning because of unused variables.
07/28/08    rs  klocwork fix for arr BTApp_ATCSCSTable that might be indexed 
                out of bound.
07/22/08    rs  HFAG enabled event only to be handled when the static app 
                initiates the enable process.
06/09/08    co  Removed or replaced unnecessary FEATURE_BT_2_1 with
                BT_SSR_LPC_DEBUG.
05/21/08    co  If SCO REWORK is enabled and A2DP device is NOT enabled, set
                BT HS as the sound device while HFP SLC is up.
05/05/08    RS  Fixed the issue where bonding gets stuck in case the procedure
                fails.
05/12/08    VM  Replacing deprecated functions with BREW replacements.
05/09/08    hs  Added the code to rectify A2DP and HFAG scenarios. 
04/29/08    RS  Restored Automation related issues on 2.1 builds. Ranking not 
                done when the flag for test automation is enabled. Also some 
                events are now routed to diag from FEATURE_BT_2_1 code section.
04/24/08    VM  Refresh display in BTApp_BuildMenu.
04/22/08    VM  Initialized all the profile service security to 
                BTAPP_RM_SVC_SEC_DEFAULT
04/17/08    VK  Changing Max Default Sniff Latency to 1.28 sec.
04/16/08    co  To tear down SCO if BT HS is not in use.
04/14/08    co  Revised verdict conditions of call incoming in Snd_Chg_Evt
                processing.
04/11/08    VM  Changed the profile enabling mechanism to correct the failure
                in enabling profiles.
04/02/08    NM  Fix the issue of SPP connection not going through.This happened
                because when the MITM protection is enabled at the client, the
                user cfm screen was getting overwritten and hence bonding foll-
                owed by connection doesn't go through.
04/02/08    VK  Fixing Name request issue when name is not available with EIR.
03/20/08    NM  Fix the following issues
                - During dev discv BT Device Name is not updated with latest
                - Name discv fails, if a discovered device name is NULL
                - Further name requests fails after failure due to page timeout
03/14/08    VK  Fixing Featurization issue of FEATURE_BT_EXTPF_AV Flag.
03/12/08    VM  Added "Special (MITM_NO_BONDING)" under Security level when
                FEATURE_BT_TEST_API is enabled.
                Added support for Rebond Options in Bond Options screen and 
                compute DoRebond using the rebond options.
03/06/08    VM  Replaced mRM.srvSecType in IBTEXTRM_SetSecBySvcCls calls with 
                profile specific srvSecType.
                Added AEEBT_SD_SERVICE_CLASS_PBAP_PCE under 
                BTApp_UpdateSecurityOptions for PBAP.
03/04/08    VM  Added support to Set/Clear Bondable under Master Control
03/03/08    gs  Added support for HID profile Get/SetReport functions
02/29/08    gb  Removed unnecessary setting of RM security level to default 
                security level after reading config file.
02/29/08    VM  Fixed
                  -Display passkey menu staying on the top after successful 
                   bonding in initiator side
                  -Successfully bonded screen not showing up on the receiver 
                   side.
                  -Removed "No" softkey in display passkey screen
                  -Passing an empty string instead of NULL in PasskeyReply
02/29/08    VM  Added support for SPP under Service Security
02/28/08    mh  Lisbon SSP 
                  - Padding Passkey only if it is less than six bytes.
                  - Padding passkey if remote iniitates bonding.
02/27/08    NM  The change is to fix legacy bonding b/w a BT System with 2.1 
                controller and another BT System with 2.0 controller
02/27/08    co  Revised "+CMEE ERROR" response format to "+CME ERROR"
02/25/08    gb  Fix for updating the security settings with RM when target powers up
02/13/08    hs  Added the check for audio handling if AG is active
02/12/08    hs  Adding the A2dp device handling to prevent incoming call ring
                on A2dp media channel.
02/11/08    ns  Reverting changes made for app coexistance
02/08/08    gb  Lisbon SM2-SM4 related changes 
                -Security levels for profile is set with RM during powerup.
                -initialization of HID host srv security if its not present in
                 EFS.
02/06/08    gb  Mainlining all the changes done for SM2-SM4 implementation
02/05/08    nm  Backing out changes in version BTApp.c#283
02/05/08    mh  Freeing SAP allocated callback data
01/28/08    nm  Fix for legacy bonding failure between non 2.1 and 2.1 dev
01/29/08    ns  Initializing and De-Initializing AG & NA when enabled/disabled.
                Added support to allow other BREW apps to Select/Deselect BT 
                snd device by posting events.
01/25/08    mh  Replacing strcpy and other deprecated functions.
01/17/08    gs  Added HID Host in enabling menu.  Search for HID Devices
                is now based on LIMITED DISCOVERABLE service class.
01/15/08    pg  Fixed typo in null ptr check.
01/02/08   dsn  Remove coreapp dependency from BTApp
01/09/08    co  Made changes to track if SCO for voice is interrupted and
                needs to be re-raised.
01/08/08    mh  Multi profile connection support for SSR LPC feature test
12/20/07   jtl  Use URLs to answer and originate calls for targets which have
                the URL handlers. Otherwise will use ICallMgr or ICall.
12/17/07    mh  Fixing Lisbon SSP legacy bonding issues.
12/13/07    co  Added bAudioDisconnecting to better track SCO state if
                SCO_REWORK is supported.
                Increased BT_APP_DISC_AUDIO_DELAY_MS from 200ms to 1s to
                better support continuous audio playback scenarios.
12/04/07    mh  App manager to support low power configuration
11/21/07    ns  Fixed automation issue with BPP send on UIOne builds. Applied 
                the same fix wherever applicable for BIP, SAP, OBEX scenarios.
                Updated automation menu flow for Bond, SetDefaultDialString and
                EditName.
11/19/07    co  Changed BTApp_EndCall() to end only voice calls.
11/16/07    co  In BTApp_HandleAudioConnEv():
                - MT Calls will be answered if mAG.bDevPickedUp is TRUE.
                - when voice call audio is settled, A2DP audio is only
                  selected if SCO_REWORK is not supported.
11/08/07    gs  Added HID profile host role support.
11/08/07    co  Increased max_interval and max_latency upper limits to
                40900ms per spec.
11/01/07    ag  Merged Lisbon(BT2.1) code to the mainline
09/21/07    ns  Used a macro for simulating a key press event in automation
09/14/07    ns  Fixed problem of APP not receiving key press events during
                automation on UIOne targets.
                Replaced CLSID value with bid file of UIOne PhoneApp
09/12/07    pn  Added support for setting Connection Role and Allow/Disallow 
                Role Switch for existing connections, seperately from Master
                Control menu.  Added List Connected Devices under Devices menu.
08/29/07    pn  When processing voice call events, AEET_NMASK_NEW_CALLDESC is
                not required.
08/23/07    mh  Rearranged server registration sequence during BT ON
08/03/07    dm  Removed Updating Device when linkMode Changes.
07/20/07    sk  Fixed incorrect byte count issue in SPP when client abrupts
                disconnects in middle of a transfer and restarts SPP transfer.                
07/20/07    jj  Handled the case of getting PIN rqst and connected event 
                at the same time.
07/17/07    pr  Remote name req cancel and connect( browse) cancel support
07/06/07    jj  Fixed the problem with hadling media player incase of incoming 
                call events.
06/29/07    jj  Fixed the problem of App processing Data call events
06/26/07    pn  Removed compiler warnings.
06/18/07    dm  Removed busy icon when name request is cancelled.
05/22/07    ns  Fixed problem with automation while app is receiving delayed 
                SD name failed responses 
05/22/07    pn  Replaced "Do Role Switch" menu with "Set Connection Role" that
                allows setting local connection role with or without a 
                connection present.  Added device name for Passkey prompt.
05/21/07    pn  Appended missing postfix to ERROR response.
05/21/07    pn  Updated to use different string IDs for Scan Params.
05/17/07    pn  Fixed AG/A2DP concurrency scenarios involving 2 headsets.
                Removed all references to SND_DEVICE_BT_A2DP_SCO_HEADSET.
04/26/07    pn  Removed dependency on DialerApp for call handling.
04/20/07    pn  Added scan settings initilization. Changed to cancel delayed  
                audio disconnnection timer every time audio connection is 
                initiated. 
04/19/07    pg  Added call to RM_ConfigApplication() during BTApp_Init() 
                so that saved config in EFS is sent to BT driver on powerup.
04/18/07    pn  Fixed critical lint error in BTApp_ATResponseCOPS().
04/12/07    dm  Show Busy icon till the remote name request cancel is completed.
04/11/07    jj  Fixed problem of BT ON/OFF issue. Modified the check in the 
                case of originating a call using ICallMgr.
03/30/07    ns  Fixed problem of snd device not getting unmuted in some
                scenarios
03/28/07    pn  Fixed DevRedial failure.
03/27/07    pn  Fixed AG/A2DP concurrency issues.
03/27/07    co  Fixed that BTApp only parses the first AT command in Ext IO
                RX watermark.
03/23/07    pn  Fixed problem of occasionally not possible to restart SPP test,
                and leaving Discoverable on when ending SPP test.
03/19/07   jtl  Added L2 test app.
03/14/07    ns  Replaced ICM with ITelephone and IBatt with IBattery APIs.
                Removed dependency on recent calls APP.
03/02/07    pn  Sized info text box so all of the text gets displayed in HS
                Button Handler menu.  Leaving the menu now cleans up info text.
02/22/07    pn  Added Config Scan Params menu.
02/21/07    co  Added a space char for +CIMI response after the colon mark to
                prevent interop issues.
                Kept main menu refreshed upon SLC establishment.
02/19/07    ns  Improved AT+COPS command response.
02/16/07    pn  Fixed critical lint errors. Suppressed logging of 
                unnecessary debug messages.
02/14/07    gs  Fixed potential NULL pointer access in BTApp_DirNumPCSToText()
                Modifed BTApp_DoSPP() so that SPP client will do service
                search everytime before connecting.
02/01/07    pn  Current battery charge level is sent to AG at enable time.
01/26/07    pn  Font size is adjusted for VGA.  Debug msg for AG Disconnected 
                event shows BD address of remote device rather than error code.
01/04/07    pn  Consolidated event processing for voice and VT calls.
12/08/06    pn  Made use of the name's length field when retrieving network
                name from ICM.
11/27/06    pn  Fixed problem of not properly handling MT call answered 
                by HF while audio connection was already present, or when
                A2DP was streaming.
11/27/06    JJ  Modifications done to process passwd and passkey seperately 
                in ServerRegistration and Clientconnect interfaces.Added 
                support for OBEX context query , and passkey automation for
                bonding
11/23/06    JJ  Added automation plugins for IOBEX
11/16/06    ry  Made BTApp_DoSearchService compatible with the new BREW API
                changes to support 128-bit UUID.
11/15/06    smr Added Authentication support for IOBEX.
11/07/06    smr Added support for IOBEX.
11/06/06    pn  User can now configure how to handle HS Button press.
11/03/06    pn  Don't change AG device type when SLC comes up. VT call Connect 
                event is handled the same way as voice call Connect event. Use 
                BTApp_DisconnectAudio() rather than BTApp_ReleaseBTDevice() 
                when VT call ends.
10/19/06    pg  Added LMP version and subversion to MyInfo screen
10/13/06    pn  Verifies there's a BD address before issuing Ring and 
                ConnectAudio commands.  Macro BDADDR_VALID() and new macro 
                IS_AG_CONNECTABLE() are utilized where appropriate. AG audio 
                disconnection is delayed a few hundred ms to avoid rapid 
                toggling of AG audio state.
10/13/06    ns  Added automation support to poll for AG connection status
09/24/06    pn  Enabling SAP is now delayed at boot to wait for SIM to get
                initialized.  All state transition messages are now at HIGH
                level.  Additional AT commands removed. Unknown AT commands
                are now reponded with ERROR.
09/15/06    pn  When disconnected from HS during a call, remember to transfer
                call audio automatically to HS when connection comes back up.
09/15/06    ns  Added support in HFP1.5 for handling additional AT commands 
                sent by SAP Siemens car kit. AG conn mode can be set to passive
                any device
09/13/06    pn  Made BTApp work with and without SCO_REWORK enabled.
09/12/06    gs  Fixing RVCT compiler errors on 7K target.
09/11/06    pn  Consolidated audio control functions. Simplified debug printing
                for External I/O. Added AEECM_EVENT_CALL_ALL_CALLS_ENDED handler.
                AT+BTRH is now responded with OK. Improved handling of AT+CLCC.
                Button press events are always ignored for first 2 seconds.
                HF-initiated audio connection is rejected if A2DP is streaming.
09/01/06    ns  Suspended A2DP when h/w ringer is played during incoming call
                Added support to return error response for AT+BTRH? command
                Fixed audio connecting problem when AG is enabled while in call
                Select audio device for HS/HF initiated connection
                Fixed problem with audio connection while auto answer enabled
08/14/06   dsn  Fixing featurization for SoC solution.
08/12/06    bk  Stored Authentication value for FTP client
08/08/06    mp  Fixed the AT+BLDN and AT+ATD command error response
07/31/06    ns  Added audio rework feature
07/14/06    gs  Updated OPP Register function call.
07/13/06    ns  Added concurrency support for PBAP pull vcard obj operation
07/07/06    pn  Fixed problem of Audio connection being brought up 
                unintentionally.
07/07/06    ns  Added support in PBAP to allow browsing from one level above  
                leaf directory. Added concurrency support for download PB obj
06/28/06    ns  Fixed automation problem of not sending the response for
                radio on-off and role switch automation requests
06/21/06    pn  Stops sending ring alerts when a call ends only if no MT call
                is in progress. Releases BT once done with VR.
06/12/06    ns  Added automation support for PBAP enabling and disabling
                Added support for UserID during BPP obex authentication
06/07/06    ns  Added automation support for PBAP
06/05/06    pn  A2DP/AG concurrency issues with Auto Answer on, and switching
                between voice call and Media playing.
06/02/06    jh  Featurized variable result in BTApp_TextEditSave using 
                FEATURE_BT_EXTPF_PBAP to eliminate compiler warning.
05/31/06    gs  Added missing PBAP lines of code left out during merge. 
                CTP and ICP profile now use same notification mask (to free
                up a bit for PBAP).
05/30/06    gs  Mainlining PBAP feature development.
05/24/06    ns  Fixed problem in AT+CLCC response of missing the last digit
05/23/06    sr  Fixed bug in BTApp_ReadConfigFile - parameter to IFILE_Read
05/12/06    pn  Enables user to initiate connection to unbonded HS from Device
                Info screen.
05/12/06    pn  Fixed problem in handling AEEBT_RM_EVT_CONN.
05/08/06    ns  Added support to perform role switch operation
05/04/06    pn  For HFP 1.5., Memory Dialing and Redialing commands are 
                received via External I/O.
04/27/06    pn  Fixed featurization problem in previous change.
04/25/06    pn  Fixed problem with Redialing and Memory Dialing. In handling
                MT calls while A2DP is streaming, waits for A2DP to suspend
                before switching to use AG HS. Update config file if a device
                being used by AG/A2DP gets removed from DB.
04/25/06    pn  Fixed problem processing VT events.
04/20/06    ns  Provided I/F to print file using BPP from another BREW App
04/17/06    pn  Fixed featurization problem in previous change.
04/11/06    pn  Removed unused code. Added code to clean up after cancelling
                Passkey prompt. Added missing code to handle "No" response to
                Connect Authorization request. Fixed problem with Audio 
                connection being brought up unnecessarily when MT call is
                answered/rejected by AG. Fixed audio routing problem when AG 
                and A2DP HSs are used concurrently.
04/06/06    dc  Added ability to set default dial string via automation. 
                ShowMessage removed the connect and disconnect messages after
                2 seconds for connection/disconnection to audio devices.
04/06/06    ns  Using AEE macro for BT device name length
03/08/06    dc  Added ability to query the link status
03/03/06    ns  Removed support for AT+CPBS=? command in HFP1.5 feature
                Sending OK response to AT+CLCC when no active calls present
02/28/06    pn  Does not automatically answer call if already in a call.
02/20/06    jh  Fixed compiler warning.
02/15/06    pn  When bonding is initiated by remote device and user cancels 
                entering pass key, sends PinReply with empty pass key.
01/31/06    pn  Registers for and gets radio activities and link status.
01/23/06    ns  Added diagnostic support to query information about the 
                AVRCP operations performed by HS/HF. Added automation support
                for bonding when AG/NA are enabled
01/09/06    pn  Fixed featurization so BTApp can build without ICM.
01/05/06    ns  Added support for SAP APDU Tx and Rx through files in EFS
                Fixed problem with bonding during automation
12/28/05    sn  Added diagnostic support for OPP Client abort feature.
12/27/05    sn  Fixed issues in enabling BT services when BT is turned ON.
12/26/05    ns  Added support to set the passkey while enabling A2DP and AVRCP
                profiles during automation
                Fixed problem in bonding during automation
                Diagnostic support to get the A2DP and AVRCP contexts
                Diagnostic support to poll the status of A2DP control flags
12/20/05    pn  Featurized SAP Client with FEATURE_BT_EXTPF_TEST.
12/20/05    pn  Fixed memory corruption when preparing passkey prompt.
12/15/05    ns  Added diagnostic support for enabling profiles
                Enabled SAP client feature
                Fixed problem with remote printing 
12/14/05   dsn  Added featurization for MSM7500 BT bringup.
12/12/05    sn  Added app support to abort OPP push/pull operation.
12/06/05    sn  Added diagnostic support for FTP Client ABORT feature.
12/06/05    pn  Fixed RM database problem resulted from re-bond failure.
                Fixed problem parsing AT commands with string parameters.
                Added code to send OK response after handling each AT command.
12/05/05    ns  Added support for extra AT commands specific to parrot car kit
12/02/05    sn  Added support to enable A2DP and AVRCP from 'Settings' menu.
12/01/05    dc  Fixed the issue of Unbond menu appearing twice when Unbonded
                if the connection is still up
11/30/05    pn  Properly handles multi-party call scenario in which active
                call ends while waiting call remains.
11/22/05    pn  Releases BT device when HFK brings down audio connection. Uses
                AEEBT_MAX_NUM_OF_SRV_REC as number of Browse records.
11/14/05    ns  Added support to send a BIP print request from other apps
                Fixed problem in extracting AT parameters for HFP1.5
11/09/05    pn  Pass Key prompt now starts out empty.
10/27/05    pn  Replaced BT_SWDEV_HFP_1_5 with FEATURE_BT_HFP_1_5.  Removed
                call to ReleaseBTDevice() when audio disconnected.
10/23/05    pn  Fixed profiles featurization problems.
10/21/05    ns  Modified SAP server implementation to use ICARD interface 
                instead of IBT, SAP client menu option removed. SAP server can
                be enabled from the BT ON/OFF option.
                Deregister all servers when BT is turned OFF
                Moved ICM handle out of AG as it is used across profiles
                Fixed problem with AT+CNUM command
10/18/05    dc  Incorporating code review changes. Fixed bug in 
                BTApp_DiagSetBTName.
10/18/05    sn  Added MSG ID check before popping up AVRCP test menu.
10/14/05    pn  Fixed problems involving MO call by HF while playing non-BT 
                supported media types. Fixed audio loss problem in failing to
                connect to HS for MT call.
10/09/05    dc  Added function to set Radio. Added function to set concurrency 
                flag. Added IDIAG event to set BT Name.
09/28/05    ns  Used dynamic memory for storing SD and RM records. Device 
                address is displayed in discovery results when get name fails.
                Passkey is stored in a seperate varible for all OBEX profiles 
                to avoid problems encountered during concurrent operations.
09/21/05    pn  Replaced BT_SWDEV_AG_EXT_IO with BT_SWDEV_HFP_1_5.
09/15/05    ns  Used uisnd_set_device_xxx interfaces for est AG audio 
                connection instead of AG IBT interface.
                Used dynamic mem allocation in HFP1.5 to avoid exceeding 
                static memory limitation for BTAPP.
09/08/05    ns  Featurized HFP1.5 for GSM and CDMA platforms.
09/07/05    ns  Added support to originate a video call using Headset
09/06/05    ns  Added support for HFP 1.5. Handled AT commands AT+COPS, 
                AT+CLCC, AT+CNUM, AT+CMEE
09/01/05    ns  Fixed problem in accepting an incoming call when audio is
                already connected, problem of audio not being transfered
                when audio disconnects or SLC down with call being in active
                state, problems related to three way calling, problem of 
                audio not being tranfered when call is answered from phone
08/24/05    ns  Added private mode handling for CCAP recommendations
08/19/05    ns  Added support for CCAP recommendations required for 
                HF application
08/17/05    dc  Added function for handling the SPP settings menu
08/11/05    pn  Removed FEATURE_SPEAKER_PHONE.  Added support for in-band ring.
                Redialing from car-kit no longer requires user to press SEND
                key on phone. Fixed problem of unexpected automatic audio 
                transfer after VR is started in HS mode.
08/04/05    pn  Fixed problem of audio not transferred automatically to car-kit
                when re-dialing and memory-dialing are initiated by car-kit.
08/03/05    pn  Improved building and handling Device Discovery screen.  Fixed
                problem with call setup handling, Default Dial String menu 
                handling, and not applying VR Capability settings for HFP.
08/02/05    sn  Removed "BREWVersion.h" as now it is included in "BTApp.h".
07/28/05    pn  Made number of bytes sent in SPP test configurable.
07/27/05    ns  Added support to enable obex profiles (servers) during startup
07/26/05    jh  Featurized AG External I/O code using BT_SWDEV_AG_EXT_IO
07/26/05    dc  Added BT_APP_MENU_FTP_BROWSING, BT_APP_MENU_FTP_PASSWORD,
                BT_APP_MENU_FTP_CREATE_FLDR, BT_APP_MENU_FTP_SRV_REGISTER,
                BT_APP_MENU_FTP_MOVE_TO_FLDR to BTApp_DiagGetContext
                Added BTAPP_FTP_BROWSING_MENU_STATUS to 
                BTApp_DiagPolledStatusType
                Fixed problem with bonding. When bonding fails state is updated
                accordingly
07/25/05    pn  Removed BT_SWDEV_INBAND_RING_ENABLED.
07/21/05    pn  Added initial support for AG External I/O.
07/21/05    sn  Added FTP support for MOVE_TO_FOLDER soft key to move to a 
                subfolder by inputting the subfolder name in the text dialog;
07/13/05    ns  Added concurrency test automation support for BIP push, pull,
                auto archive, remote camera, remote display, archived objects,
                referenced objects.
07/12/05    dc  Added concurrency test automation interfaces for OPP Pull,
                FTP Get and BPP Send Operations
                Added automation interface for Master cntrl Role Switch Toggle
                Added automation interface for accessing the BT Name, Address
07/06/05    sn  Added support for BT_APP_MENU_FTP_SRV_REGISTER
07/01/05    jtl Fix compile issue when FEATURE_BT_EXTPF_FTP is not defined.
06/27/05    pn  Minor Device Class is no longer used to determine which Audio 
                profile to use.  Use, Connect, and Disconnect menu items are
                now only available for audio devices. Fixed featurization
                problem when FEATURE_BT_VT is defined but FEATURE_SPEAKER_PHONE
                is not. Brought VT call event processing up to date. Processes 
                new format of Device Discovery Complete event.
                BT_SWDEV_INBAND_RING_ENABLED is used.
06/27/05    ns  Added concurrency test automation interfaces for OPP Push 
                and FTP Put operations
                Added automation interface to get the status of a concurrency 
                event
06/27/05    sn  Added support for FTP ABORT feature
06/20/05    pn  Made changes for In-band ring feature. Only sends/stops RING if
                FEATURE_SPEAKER_PHONE is defined. Automatically transfers audio
                to HS/HF if call is present when BT AG is enabled. Retains 
                class-of-device info. No longer disconnects from HS when audio
                link drops in HSP mode.
06/20/05    ns  Handling of clear key when in FTP browsing menu changed
06/16/05    ns  Added UI and automation support for A2DP control interface
06/13/05    sn  Added app and automation support to input folder name for 
                FTP create folder feature
06/09/05    ns  Scope of function BTApp_WriteConfigFile() changed, 
                Static removed.
05/28/05    ns  Added automation support to query the context when app is in
                My Info Menu
05/24/05    dc  Added Events for A2DP Automation. PollStatus, SelectTest and
                Getcontext capability for A2DP. Modified SPPTest, HCIMenu and
                StartTestMode functions to work with the modified UI.
05/23/05    ns  Automated bonding process on the server side for all profiles.
                Fixed problem with bonding menu when mobile is not in the
                context of bluetooth app.
05/13/05    pn  BTApp_StopRing() now actually commands BT driver to stop ringing
                audio device.
05/13/05    ns  Moved GET NAME definitions to BTApp.h for access to IDIAG 
                utilitiy functions.
05/12/05    pn  Only audio brought up by car-kit to transfer voice call is 
                scheduled to be disconnected in case AVS does not set up PCM 
                path within 5 seconds.
05/03/05    pn  Fixed problem in selecting an SPP server from device list.
                Fixed problem in clearing Discoverable menu.
05/01/05    gs  Backing out changes made in #91.
04/29/05    pn  Fixed problem of UI being stuck displaying message when device 
                lists are empty.  Increased time to ignore button press to 
                2 seconds.
04/27/05   jtl  Add AVRCP test menu.
04/26/05    gs  Keep UI in hour-glass screen when BPP SendFile operation
                is in progress.
04/22/05    pn  Fixed compile error for when TEST_AUTOMATION is not defined.
04/21/05    pn  Set HCI Mode and Enter Test Mode menus are now under Master 
                Control test menu.  Audio Transfer menu is now available under
                Test menu when connection to a headset is present without voice
                call.  VR Capable is set to FALSE when FEATURE_PHONE_VR is off.
                Some code cleanup.
04/21/05    gr  Fixed compilation error when EXTPF_OPP is turned off
04/20/05    dc  Modified the UsesPayload entry for EVT_BTAPP_DIAG_FTP_CLI_PUT
                from FALSE to TRUE.
04/15/05    pn  No longer performs GetDeviceName for discovered devices whose 
                names are already known.
04/14/05   jtl  Support for another snd_set_device type for A2DP/SCO headsets.
04/12/05    dc  OPP, FTP, BPP and BIP profiles now can handle the PIN code menu 
                during connection. 
                Added automation support to access SPP server and Client menus, 
                HCI Menu and Test mode menu.
04/12/05    pn  Added support for master control to allow/disallow role switch,
                to turn on/off BT radio and visibility.
04/09/05    dc  Added Polling Status Capability to FTP, Discoverable Menu,
                Bondable menu. 
04/08/05    pn  User can now choose not to use VR.
04/07/05    pn  Now handles ButtonPressed event during MO call setup.
04/06/05    pn  Added menu to edit default dial string, to be used when
                FEATURE_PHONE_VR is not defined.
03/28/05    pn  Fixed problem of SELECT key having no effect after some messages
                are displayed.
03/25/05    pn  Removed Bondable settings.  BTApp is now always bondable when
                any service (AG, NA, SPP server, ...) is activated. Fixed 
                ConnMode menu building/handling.
03/22/05    ns  Added support to set SAP Max and Min Frame size from UI
03/21/05    pn  Made use of FEATURE_PHONE_VR. Selected menu item is now saved
                and restored. SELECT key is now ignored while message is 
                displayed. In Active mode, ACL connection to HS is no longer
                brought down as result of audio disconnection. UI no longer
                gets stuck after re-bonding to same remote device. Improved CM
                event processing to handle multi-party calls.
03/18/05    ns  Fixed problem with SAP Client Connect
                Removed #ifdef ICardConnection wherever not required
03/17/05   jtl  Added A2DP test menu
03/08/05    gr  Modified the case block for BROWSING menu in HandleClearKey
03/08/05   jtl  SPP test uses buffer and flow control.
03/02/05    ns  Added 
                - support for SAP APDU requests
                - removed password menu for SAP
                - Automation support for SAP
03/01/05    dc  Fixed the UI Issue with the FTP Browsing menu.
02/24/05    pn  No longer releases HS/HF when receiving VT Call Setup event.
                MyInfo screen refreshes as soon as local BT name is changed.
                GetName is performed for each discovered device whose BT name 
                is not known.  RM is now configured to not perform service 
                search automatically.
02/23/05    dc  Added IDIAG support to set Device in Discoverable mode and
                Bondable mode. Added support to set Security type.
02/18/05    sn  Added 
                - FTP browsing menu events, clear key handling cases
                - support for FTP diagnostic events 
02/16/05    ns  Added support for IBT SAP 
02/09/05    pn  Fixed featurization problem.
02/02/05    pn  Fixed memory leak.  Now uses Passive mode for AG if enabling
                both AG and NA.  Now informs user to re-enable AG in order to
                use newly bonded HS/HF.
01/31/05    pn  Set unused pointer in DoSpp() to NULL.  When call is auto-
                answered, use BT as audio device only if connection to HS/HF 
                is present.  Release audio device when audio connection fails.
01/20/05    dc  Added a entry EVT_BTAPP_DIAG_BPP_CLI_SEC_CONEX to BTApp_DiagEventReg
                Added the case for EVT_BTAPP_DIAG_BPP_CLI_SEC_CONEX in BTApp_HandleEvent
                Added case BTAPP_BPP_CLI_SEC_CON_STATUS to BTApp_DiagPollStatus.
01/20/05    gs  Inserted missing 'break' statement in HandleDevListMenu
01/18/05    dc  Functions BTApp_OnAppStop, BTApp_BuildMenu, BTApp_HandleClearKey,
                BTApp_MenuHandleEvent and BTApp_DiagGetContext now handle the 
                OPP Objct types appropriately.
01/12/05    gs  Added support for searching and connecting to devices based
                on Telephony service class.
01/11/05    jh  Removed reference to uProfileVerNum in AEEBTBrowseRecord.
01/11/05    pn  Changed RecentCalls.h to recentcalls.h.
01/10/05    dc  Changed the payload field for EVT_BTAPP_DIAG_BIP_CLI_CONEX 
                to TRUE
                Changed EVT_BTAPP_DIAG_BIP_CLI_CONNECT to EVT_BTAPP_DIAG_BIP_CLI_CONEX
                Modified BTApp_DiagSelectTest to handle the extra menu selection.
01/07/05    jh  Modified calls to IBTEXTRM_GetHCIMode.
01/05/05    jh  Replaced calls to IBTEXTRM_DeviceRemove and IBTEXTRM_DeviceAdd
                with call to IBTEXTRM_DeviceServicesUpdate in 
                BTApp_BuildBrowseRespScreen.
01/05/05    ns  Added automation support for  BPP Settings and Target menu
                options.  
01/03/05    gs  Added
                - support for CTP/ICP 
                - support for selecting OBEX servers for BIP
                - OBEX and TCS-BIN menus.  Moved OBEX tests (OPP, FTP, 
                  BIP, BPP) under OBEX menu. 
12/14/04    gr  Added automation support for BIP PUSH OPS, PULL OPS, IMG PRN, 
                Automatic Archive, Remote Camera, Remote Display, Arch/Ref
                Added Poll Status support for BIP
                Added GetContext support for BIP
                Added PIN update support for BIP
                Added Settings Menu Control via automation
                Added BuildMenu support for BT_APP_MENU_BIP_SETTINGS
12/10/04    gr  Added Support for BIP Connect, Disconnect, 
12/17/04    pn  Fixed QBT featurization problem within test automation code.
12/17/04    gs  Added support for BPP Settings and Target menu options
12/15/04    pn  Added code to check call state on targets without FEATURE_ICM 
                defined. Fixed problem when bonding is initiated by remote 
                device. Fixed problem when audio is brought up by car-kit but
                AVS fails to setup PCM path. Audio Transfer menu item is now 
                available when connected to a car-kit even without a call 
                present.  Consolidated voice call and VT call setup code.
12/14/04    gs  Added OBEX authentication support
12/06/04    gr  Added support for BIP Register and Deregister via Auto
                More style changes that we missed out in the last review
11/08/04    pn  OPP, FTP, BIP, and BPP objects get released when BTApp is freed.
                Browse Services screen now also shows service names.  Search
                Services now shows profile and version. HS connection is now
                brought down when BT audio connection goes down.
10/27/04    pn  Added support for Test Mode control and Security Mode 2 testing
                on NA connections. Config file is now created at boot if does
                not exist. Auto answer now works correctly for VT calls. Fixed
                various UI problems: wrong device name displayed, wrong BD addr
                displayed, screen locked up when Enable BT with neither AG nor 
                NA selected.
10/25/04    gr  Handled additional USE menu when bonding via automation
                Provided automation support for Connect/Disconnect Functions
                Changed the JOB_ID Text control title to Job ID
10/13/04    gr  Added support for 
                - Selecting OBEX servers for BPP
                - Selecting MIME type for BPP
                - Entering Job ID for BPP
                - Support for automation of Register and DeRegister
                - Fixed a couple of issues with GAP_DEV_OPS
                - Support for Automation of Connect and Disconnect
                - Support for Automation of Send
                - Support for Automation of SendSOAP
10/06/04    pn  Unexpected LEFT/RIGHT/UP/DOWN keys are now handled properly.
                Many BT operations now get disabled when HCI mode is on.
                Presence of any active BT operation now prevents HCI mode to
                be turned on. Device names, if available, are used in prompts
                and messages in place of BD addresses.  HS/HF volume change
                reports now cause immediate update of volume settings.
                HS/HF now gets alerted when MT VT call arrives.
09/27/04    pn  FEATURE_BT_VT brought up to date.
09/23/04    pn  Now periodically requests RING to be sent (as opposed to
                requesting RING to be periodically sent.)  Main menu now shows
                "D" when local device is discoverable.  Key strokes are now
                ignored while busy icon is displayed.  IFILE_Release() is now
                called only when file pointer is valid.
09/23/04    gr  Added support for Device Search and Keep
09/20/04    gr  Updated the Fetch and Send interface (memory efficiency)
                Implemented Bond, Use Unbond All and Remove All Dev OPtions
                Implemented Discard and Unbond, Search, Browse, Security  
                Updated Get and Set context 
                Implemented GetSecuirty 
09/14/04    gr  Added Device operation support
                Changed DiagSetSelection to DiagSetMenuSelection
                Added DiagSetSoftMenuSelection
09/02/04    gr  - Added support AG Audio Transfer
08/26/04    gr  - Added support for AG interfaces set and get
09/21/04    pn  Removed compiler warnings.
09/20/04    pn  Updates VR state when audio is disconnected if necessary.
                Cleans up properly when AG is disabled.
09/16/04    pn  Added missing code for FEATURE_BT_VT.
09/01/04    pn  No longer mistakes audio-transfer button press for call hang-up
                command.  Audio Transfer menu item is now available only if a
                call is present.  No longer commands BT driver to bring up/down
                audio link when Audio Transfer menu item is selected.
08/28/04    ry  Added support for FEATURE_BT_VT.                
08/26/04    pn  Fixed SPP server bug. HS button pressed while VR is active now
                ends VR.  Releases audio path only when BT audio disconnected
                by HFK.
08/24/04    pn  Fixed compiler warnings.  Fixed problem of enabling NA.  FTP 
                settings are now saved in config file.  Fixed problem of  
                multiple discoverable mode setting.
08/11/04    gr  Added GetDevList, Client Exchange and conex support
08/10/04    pn  - Added support for specifying FTP server password
                - SPP client now prompts user to select SPP server to connect to
                - BrowseService now updates RM database
08/03/04    pn  - Added support for being launched at powerup
                - Added persistent enable/disable (e.g. automatically enable
                  BT if BT was enabled before phone reset)
                - Added menu item to search for OBEX servers.
08/05/04    gr  Added support for Business card exchange and Turn BT on context
08/04/04    gr  Added support for PollStatus, JumpToMenu
07/22/04    gr  Added support for Select Test Diagnostic
07/15/04    gr  Added Core support for IDIAG
                Added main menu control via SetContext and GetContext
08/02/04    gs  Added support for testing IBT BIP Responder APIs.
07/29/04    pn  Added support for IBT FTP and BIP.
07/26/04    pn  Added include file BTApp_res.h
07/20/04    pn  Re-added support for IBT OPP
07/09/04    pn  HF re-dialing will now bring up BT audio link.
07/08/04    pn  Correctly handles CDialerApp_StartApp_Digit().
07/03/04    pn  No longer resets audio path when BT audio link is down.
07/01/04    pn  Backed out changes related to IBT OPP support.
07/01/04    pn  - Added support for IBT OPP
                - HS/HF no longer rings after AG rejects incoming call
                - AG connect now uses device type selected at AG enable
                - UI will now update after bonded to non-audio devices
05/25/04    pn  - Passkey prompt now takes numbers and echoes asterisks
                - Icons added next to devices on device lists
                - Error message is now displayed when NA enable fails
                - Server channel number is now used in SPP_Open()
                - When END key is pressed, BTApp now cleans up properly before
                  exiting; when restarted, BTApp displays its main menu
                - No longer displays non-existent device
                - Fixed bug in SPP test
05/20/04    pn  Made 3G features target independent
05/12/04    pn  On MSM6250/MSM6500: 
                - HF no longer rings after incoming call is rejected
                - When auto answer is on, audio no longer transferred to 
                  HS/HF if user answers on handset
                - For HFP, connection is now considered ready for SCO only
                  after SLC is up
                - Select key on text edit menu is now equivalent to selecting 
                  OK button
                - Buttons on prompts are now YES/NO instead of OK/CANCEL
05/10/04    pn  - Added menu choice to set FTM HCI mode
                - Added support for auto answer (MSM6250/MSM6500)
                - Processing of AG_AUDIO_CONNECTED event now includes selecting
                  HS/HF as audio output device
                - BTApp no longer processes AEECM_EVENT_CALL_CONNECT
05/04/04    pn  - Added support for setting local unit's discoverable mode
04/30/04    pn  - Added support for OPP and FTP tests
04/14/04    pn  - Disabled UP/DOWN keys for Device Info screen
                - Removed APIT mode from HCI mode settings
04/05/04    pn  - Turned off discoverable mode when ending SPP test
                - Setting HCI mode is now available under Tests menu
03/26/04    pn  - Fixed compile error for MSM6100 and MSM6300
                - Fixed problem of clearing some menus
03/12/04    pn  - BT audio volumes now get initialized when executing in
                  MSM6250 and MSM6500 (SKUI not present)
03/05/04    pn  - Fixed problem with BT enabling setting not persistent
                - "On"/"Off" on Main menu became "Turn BT On"/"Turn BT Off"
                - "Ac"/"AC" now indicate AG enabled and connected/Audio 
                  connected
                - Device lists now show BT names instead of BD addresses for
                  devices whose names are available
                - A message is displayed when browse/search services fails
03/01/04    pn  - Fixed featurization problem
                - Cleaned up SPP test
                - Correct screen displayed after a device is successfully 
                  unbonded or removed, and after local name is set.
02/23/04    pn  - Fixed SPP test re-start problem.
02/19/04    pn  - SPP test progress is now displayed on phone screen
                - BTApp can now enable AG with no BD address
                - Connect, Disconnect, Browse Services, and Search Services 
                  menus are now available on device info screen
                - Browse Services and Search Services no longer in Tests menu
                - Moved Bondable from Settings menu to MyInfo menu
                - Removed unused code
02/17/04    pn  - Moved BTApp_UseBTDevice() and BTApp_Release BTDevice() to
                  BTAppUtils.c
                - Added BTApp_StartVR()
                - BTApp now handles AEEBT_AG_EVT_VR_ON, AEEBT_AG_EVT_VR_OFF,
                  and AEECM_EVENT_CALL_CONNECT
                - BTApp running on FFAs now ring audio device
02/09/04    pn  - BTApp now functions in targets having no SKUI
                - pressing CLR key in main menu exits BTApp
                - added menu under AG settings to enable/disable VR in AG
                - added error checking in BTApp_ReadConfigFile() and 
                  BTApp_WriteConfigFile()
                - AG settings menus now highlight the selected menu item when
                  refreshed
                - Device Discovery will be stopped when CLR key pressed
                - Bondable now means Bondable and Discoverable
                - menu item to enable BT is now unavailable until BT is disabled
                - added code to select/deselect BT device when transferring audio
                  to/from BT device
                - added externalized function BTDeviceSelected()
01/14/04    pn  - made sure local device is discoverable before opening 
                  SPP server port
01/09/04    pn  - added menu item to set security (local and remote device)
                - added security info to My Info and Dev Info screens
                - added persistent AG configuration feature
                - updated BTApp_ProcessRMNotifications() to reflect changes in
                  OEM RM events
                - will stop Device Discovery when a device is selected
12/23/03    pn  Added menu item to select audio device type.
12/18/03    pn  - added menu item to transfer audio between AG and remote device
                - SPP can now be run more than once
                - AG audio connection status now gets updated
                - SPP status now gets updated when SPP state changes
                - fixed bug that can cause failure in bonding initiated by 
                  remote device
12/12/03    ry  Made BTApp_Load non-static.
12/05/03    pn  - now releases allocated text buffers in BTApp_FreeAppData()
                - "Not Avail" shown when LMP features is not available
                - when BT becomes disabled, "connected" flags get cleared
12/03/03    pn  More enhancements:
                 - user can now select one of the bonded devices as the audio
                  device with which to enable AG
                - added confirmation screens for unbonding/removing/using
                  devices
                - added "Feature unavail" screen for unimplemented features
                - the levels of menus are now properly tracked
                - general code clean up
11/26/03    pn  Enhanced BT app with the following capabilities:
                - improved SPP test
                - text editing for entering passkey and BT name
                - listing of all devices in device DB
                - listing of bonded devices
                - indication of status of AG and NA
                - by default, NA is not selected for enabling
                - when enabling AG, NULL BD address is used
                - device discovery can now be stopped
11/14/03    pn  BT app now receives BT events as notifications.
11/05/03    pn  Initial Revision.
  
===========================================================================*/
#include "comdef.h"
#include "AEECallHistory.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"
#include "BTAppUtils.h"
#include "btapp_res.h"
#include "AEEStd.h"

#ifdef FEATURE_APP_DIALER
//#include "DialerApp.h"
#endif /* FEATURE_APP_DIALER */

#ifdef FEATURE_APP_PUREVOICE
#include "purevoice.h"
#endif /* FEATURE_APP_PUREVOICE */

#ifdef FEATURE_UI_CORE
#include "OEMNV.h"
#include "uixsnd.h"
#else
#include "uistate.h"
#endif // FEATURE_UI_CORE

#if defined(FEATURE_BT_HFP_1_5) && defined(FEATURE_UI_CORE)
#  if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#  endif /* defined(FEATURE_WCDMA)||defined(FEATURE_GSM) */ 
#endif /* defined(FEATURE_BT_HFP_1_5) && defined(FEATURE_UI_CORE) */
     

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

#include "BTAppL2.h"

#ifdef FEATURE_BT_EXTPF_OPP
#include "BTAppOPP.h"
#endif

#ifdef FEATURE_BT_EXTPF_FTP
#include "BTAppFTP.h"
#endif

#ifdef FEATURE_BT_EXTPF_BIP
#include "BTAppBIP.h"
#endif

#ifdef FEATURE_BT_EXTPF_BPP
#include "BTAppBPP.h"
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
#include "BTAppPBAP.h"
#endif

#ifdef FEATURE_IOBEX
#include "BTAppOBEX.h"
#endif

#ifdef FEATURE_BT_EXTPF_CTP
#include "BTAppCTP.h"
#include "BTAppICP.h"
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
#include "BTAppA2DP.h"
#include "BTAppAVRCP.h"
#endif


#include "AEEAppletCtl.h"  	//Add By zzg 2011_10_20

#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#include "MediaGallery.h"
#endif 



#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
/*===========================================================================

                      STATIC DATA

===========================================================================*/

//Add By zzg 2010_10_29
#define BTBAR_HEIGHT	BOTTOMBAR_HEIGHT	//(BOTTOMBAR_HEIGHT + 7)
//Add End

//Add By zzg 2010_11_16
boolean 	bIsBTOn = FALSE;

NotificationData	NotifyData;
uint16				NotifyType;


const AEEBTBDAddr NULL_BD_ADDR = {0,0,0,0,0,0};
CBTApp* pTheBTApp = NULL;
uint32 uBTApp_NMask = NMASK_BT_RM  | NMASK_BT_SD | NMASK_BT_SPP |
                      (((uint32)(NOTIFIER_VAL_ANY)) << 16);
#ifdef FEATURE_BT_EXTPF_OPP
extern char*  szServerNameOPP;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
extern char*  szServerNameIR;
extern char*  szServerNameAO;
extern char*  szServerNameRO;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
extern char*  szServerNameBPP;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
extern char*  szServerNamePBAP;
#endif

#define BTAPP_WRITE_VALUE( x ) (IFILE_Write( pIFile, (void*)&(x), sizeof((x))) > 0)
#define BTAPP_READ_VALUE( x )  (IFILE_Read ( pIFile, (void*)&(x), sizeof((x))) > 0)

#define BTAPP_AG_ISBITSET( x ) ( (pMe->mAG.uFlags & (x)) ? TRUE : FALSE )

#define BTAPP_AG_CLRBIT( x )  \
  pMe->mAG.uFlags &= ~(x);    \
  MSG_LOW( "AG flag=0x%x", pMe->mAG.uFlags, 0, 0 )

#define BTAPP_AG_SETBIT( x )  \
  pMe->mAG.uFlags |= (x);     \
  MSG_LOW( "AG flag=0x%x", pMe->mAG.uFlags, 0, 0 )

#define BTAPP_BT_LMP_VER_2_0          0x03 /* 2.0 controller lmp version from
                                           ** BT Assigned numbers
                                           */

#define BT_APP_DISC_AUDIO_DELAY_MS    1000 /* it may be revised back to a
                                           ** smaller value if AVS can shorten
                                           ** the time that snd_chg_evt takes
                                           ** to reach BTApp.
                                           */

#define BT_APP_RELEASE_BT_DEVICE_DELAY_MS    2000

#ifdef FEATURE_BT_2_1
#define BTAPP_DEFAULT_SNIFF_MAX_INTERVAL        640
#define BTAPP_DEFAULT_SNIFF_IDLE_TO             7000
#define BTAPP_DEFAULT_SNIFF_MAX_LATENCY         1280
#define BTAPP_MAX_INTERVAL_LEN                  5
#define BTAPP_MAX_IDLE_TIMEOUT                  5
#define BTAPP_MAX_LATENCY_LEN                   5
#endif /* FEATURE_BT_2_1 */
#define BTAPP_MIN_NICKNAME_LEN                  3

#ifdef FEATURE_BT_HFP_1_5
BTApp_ATCmdTableType BTApp_ATCmdTable[ BTAPP_AT_CMD_MAX ] =
{
  { "AT+COPS=",    2, BTAPP_AT_CMD_SET_COPS }, /*set fomat of n/w optr string*/
  { "AT+COPS?\r",  0, BTAPP_AT_CMD_READ_COPS },/* read name of n/w optr */
  { "AT+CLCC\r",   0, BTAPP_AT_CMD_CLCC },     /* read call info */
  { "AT+CNUM\r",   0, BTAPP_AT_CMD_CNUM },     /* find subscriber info */
  { "AT+CMEE=",    1, BTAPP_AT_CMD_CMEE },     /* set extended error msgs */
  { "AT+CSCS?\r",  0, BTAPP_AT_CMD_READ_CSCS },/* read char set used*/
  { "AT+CSCS=?\r", 0, BTAPP_AT_CMD_QUERY_CSCS },/* query char set supported*/
  { "AT+CSCS=",    1, BTAPP_AT_CMD_SET_CSCS }, /* set char set */
  { "AT+CSQ\r",    0, BTAPP_AT_CMD_CSQ },      /* read rssi and ber */
  { "AT+CBC\r",    0, BTAPP_AT_CMD_CBC },      /* read battery info */
  { "AT+CIMI\r",   0, BTAPP_AT_CMD_CIMI },     /* read IMSI */
  { "AT+BLDN\r",   0, BTAPP_AT_CMD_BLDN },     /* redial */
  { "AT+BTRH?\r",  0, BTAPP_AT_CMD_BTRH },     /* response and hold */
  { "ATD>",        1, BTAPP_AT_CMD_MEM_DIAL }  /* memory dial */
};

BTApp_ATCSCSTableType BTApp_ATCSCSTable[] =
{
  { "IRA",     BTAPP_ALPHA_IRA },
  { "GSM",     BTAPP_ALPHA_GSM },
  { "UCS2",    BTAPP_ALPHA_UCS2 },
  { "HEX",     BTAPP_ALPHA_HEX },
  { "8BIT",    BTAPP_ALPHA_8BIT }
};

uint8 okResp[ ] = { '\r', '\n', 'O', 'K', '\r', '\n', 0 };
uint8 errorResp[ ] = { '\r', '\n', 'E', 'R', 'R', 'O', 'R', '\r', '\n', 0 };
#define SEND_RESP( rsp ) IBTEXTAG_SendResponse( pMe->mAG.po, (rsp), STRLEN((const char*)(rsp)) )//  \
  //(DBGPRINTF_FATAL ( "%s", (rsp) ),\
   IBTEXTAG_SendResponse( pMe->mAG.po, (rsp), STRLEN((const char*)(rsp)) )

#endif //FEATURE_BT_HFP_1_5

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION


// 重要提示：
//        g_dwAEEStdLibEntry全局变量仅用于SDK ！
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#ifndef WIN32
   #error This applet was never intended to be used with the AEE Simulator.
#endif
#endif


/*===========================================================================

                      FUNCITON DECLARATION

===========================================================================*/
//Add By zzg 2011_01_04
/*----------------------模块相关函数声明---------------------*/

int  BTAppMod_Load(IShell   *pIShell,
    void     *ph,
    IModule **ppMod
);

int  BTAppMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
);

static int  BTAppMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
);

static uint32 BTAppMod_AddRef(IModule *po);

static uint32 BTAppMod_Release(IModule *po);

static void BTAppMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
);

/*----------------------Applet相关函数声明---------------------*/
static int BTApp_New(IShell *ps,
    IModule *pIModule,
    IBTApp **ppObj
);

static uint32  BTApp_AddRef(IBTApp *pi);

static uint32  BTApp_Release (IBTApp *pi);

static boolean BTApp_HandleEvent(IBTApp *pi,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
);

static int BTApp_InitAppData(CBTApp *pMe);

static void BTApp_FreeAppData(CBTApp *pMe);

static void BTApp_RunFSM(CBTApp *pMe);




static void BTApp_RadioActivityChanged( CBTApp* pMe );




static BTAppMod gBTAppMod;

static const VTBL(IModule) gModFuncs =
{
    BTAppMod_AddRef,
    BTAppMod_Release,
    BTAppMod_CreateInstance,
    BTAppMod_FreeResources
};

// 只允许一个BTApp实例。每次创建BTApp Applet时，
// 返回同一结构指针给BREW层。
static CBTApp gBTApp;

static const VTBL(IBTApp) gBTAppMethods =
{
    BTApp_AddRef,
    BTApp_Release,
    BTApp_HandleEvent
};


//Add End




//static boolean BTApp_InitAppData( IApplet* pi );
static boolean BTApp_StartBT( CBTApp* pMe );

static boolean BTApp_Init( CBTApp* pme );
//static void BTApp_FreeAppData( IApplet* pi );

//static boolean BTApp_HandleEvent ( IApplet* pi, AEEEvent eCode, uint16 wParam, uint32 dwParam );
static boolean BTApp_MenuHandleEvent( void *pUser, AEEEvent evt, 
                                      uint16 wParam, uint32 dw );
static boolean BTApp_TextEditHandleEvent( void* pUser, AEEEvent evt,
                                          uint16 wParam, uint32 dw );

static boolean BTApp_KeysDisabled( CBTApp* pMe, uint16 key );
static boolean BTApp_ClearDiscoverable( CBTApp* pMe );

static void BTApp_BuildMainMenu( CBTApp* pMe);
static void BTApp_BuildDeviceSearchMenu( CBTApp* pMe );
static void BTApp_BuildDevicesMenu( CBTApp* pMe );
static void BTApp_BuildMyInfoMenu( CBTApp* pMe );
static void BTApp_BuildSettingsMenu( CBTApp* pMe );
static void BTApp_BuildTestsMenu( CBTApp* pMe );
static void BTApp_BuildHCIMenu( CBTApp* pMe );
static void BTApp_BuildOBEXMenu( CBTApp* pMe );
static void BTApp_BuildTCSMenu( CBTApp* pMe );
static void BTApp_BuildDeviceList( CBTApp* pMe, BTAppMenuType menu );

//Add By zzg 2010_11_08
static void BTApp_SaveSendFilePath(CBTApp *pMe, const char* filepath);
//Add End

static boolean BTApp_HCIModeOn( CBTApp* pMe );

static void BTApp_BuildAGSettingsMenu( CBTApp* pMe);
static void BTApp_BuildNASettingsMenu( CBTApp* pMe);
static void BTApp_BuildEnablingMenu( CBTApp* pMe );
static void BTApp_BuildDiscoverableMenu( CBTApp* pMe );
static void BTApp_BuildDevTypeMenu( CBTApp* pMe );
static void BTApp_BuildConnModeMenu( CBTApp* pMe );
static void BTApp_BuildHSButtonHndlrMenu( CBTApp* pMe );

#ifdef FEATURE_BT_2_1
static void BTApp_BuildIOCapabilityMenu( CBTApp* pMe );
//static boolean BTApp_DoRebond( CBTApp* pMe ,boolean bAuthorized ) ;
static boolean BTApp_HandleBondOptionsMenu( CBTApp* pMe, uint16 key );
//static boolean BTApp_UserConfirm( CBTApp* pMe, boolean bConfirmed );
static void BTApp_BuildBondOptionsMenu( CBTApp* pMe );
static void BTApp_BuildDisplayPasskeyMenu( CBTApp* pMe );
//static void BTApp_PadString(char* Passkey, int passKeyLen);
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif  /* FEATURE_BT_TEST_API */
//static void BTApp_LocalOOBCreated( CBTApp* pMe );
//static int BTApp_RemoteOOBWrite( CBTApp* pMe,  AEEBTDeviceInfo *pDev );
//  Security features
static void BTApp_BuildSvcSecMainMenu ( CBTApp* pMe );
static void BTApp_BuildSvcSecMenu( CBTApp* pMe );
static boolean BTApp_BuildSecurityOptionsMenu( CBTApp* pMe );
static boolean BTApp_HandleDbgKey( CBTApp* pMe, uint16 key );
static void BTApp_BuildDBGKeyHndlrMenu( CBTApp* pMe );
boolean BTApp_UpdateSecurityOptions( CBTApp* pMe , BTApp_ProfileEnabled profile );
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_PHONE_VR
static void BTApp_BuildVRCapableMenu( CBTApp* pMe );
static void BTApp_StartVR( CBTApp* pMe );
static void BTApp_EndVR( CBTApp* pMe );
#endif

static void BTApp_BuildDevRespMenu( CBTApp* pMe );
static void BTApp_BuildBrowseRespScreen( CBTApp* pMe );
static void BTApp_BuildSearchRespScreen( CBTApp* pMe );
static void BTApp_BuildDevInfo( CBTApp* pMe );
static void BTApp_BuildDevOptions( CBTApp* pMe );
static void BTApp_BuildSecurityMenu( CBTApp* pMe, AEEBTSecurity security, 
                                     BTAppMenuType menu );
static void BTApp_BuildSPPTestMenu( CBTApp* pMe );
static void BTApp_BuildSppTestResults( CBTApp* pMe );
static void BTApp_BuildSPPSettingsMenu( CBTApp* pMe );
/*static*/ void BTApp_BuildPrompt( CBTApp* pMe, BTAppMenuType menu );
static void BTApp_BuildListMenu( CBTApp* pMe, BTAppMenuType menu );
static void BTApp_BuildMasterControlMenu( CBTApp* pMe );
static void BTApp_BuildScanParamsMenu( CBTApp* pMe );
static void BTApp_BuildScanParamsSubmenu( CBTApp* pMe, BTAppMenuType menu );
static void BTApp_BuildSetConnRoleMenu( CBTApp* pMe );
static boolean BTApp_HandleSelection( CBTApp* pMe, uint16 selection );
static boolean BTApp_DoDeviceSearch( CBTApp* pMe, AEEBTServiceClass svcCls,
                                     const AEEBTBDAddr* pAddr );
static boolean BTApp_StopDeviceDiscovery( CBTApp* pMe );
static void BTApp_DoAGSettingsMenu( CBTApp* pMe, int item );
static void BTApp_DoScanParamsMenu( CBTApp* pMe, int item );

static void BTApp_DoSPP( CBTApp* pMe, boolean bClient );
static void BTApp_SendSPPData( CBTApp* pMe );
static void BTApp_RetrieveSPPData( CBTApp* pMe );
static void BTApp_EndSPPTest( CBTApp* pMe );

static void BTApp_ProcessAGNotifications( CBTApp* pMe, AEEEvent evt, 
                                          NotificationData* );
static void BTApp_ProcessNANotifications( CBTApp* pMe, AEEEvent evt, 
                                          NotificationData* );
static void BTApp_ProcessRMNotifications( CBTApp* pMe, AEEEvent evt, 
                                          NotificationData* );
static void BTApp_ProcessSDNotifications( CBTApp* pMe, AEEEvent evt, 
                                          NotificationData* );
static void BTApp_ProcessSPPNotifications( CBTApp* pMe, AEEEvent evt, 
                                           NotificationData* );

static void BTApp_HandleEventButtonPressed( CBTApp* pMe );
static void BTApp_HandleEventDevPickUp( CBTApp* pMe );
static void BTApp_HandleEventDevHangUp( CBTApp* pMe );
static boolean BTApp_ConnectAudio( CBTApp* pMe, boolean bForceUnmute );
static boolean BTApp_DisconnectAudio( CBTApp* pMe, boolean bForceUnmute );
static void BTApp_HandleMTCall( CBTApp* pMe );
//static void BTApp_CancelDevNameRequest( CBTApp* pMe );
static boolean BTApp_ProcessTELEPHONEEvents( CBTApp*, uint16, uint32 );
static void BTApp_HandleCallAndMOSSetup( CBTApp* pMe );
#ifdef FEATURE_AVS_BT_SCO_REWORK
static void BTApp_DisconnectAudioDelayed( CBTApp* pMe );
static void BTApp_ReleaseBTDeviceDelayed( CBTApp* pMe );
#endif /* FEATURE_AVS_BT_SCO_REWORK */

static boolean BTApp_NAInit( CBTApp *pMe );
static void BTApp_NADeInit( IApplet *pi );
static void BTApp_RetrieveATCmd( CBTApp* pMe );
static void BTApp_ProcEvtAGDisabled( CBTApp* pMe );

#ifdef FEATURE_BT_VT
static boolean BTApp_ProcessVTEvents( CBTApp*, uint16, uint32 );
#endif /* FEATURE_BT_VT */

#ifdef FEATURE_BT_HFP_1_5
static boolean BTApp_ProcessBattEvents( CBTApp*, uint16, uint32 );
static void BTApp_ATResponseErr( CBTApp* pMe, BTApp_ATCMEEErrCode errCode );
#endif //FEATURE_BT_HFP_1_5



//Add By zzg 2011_01_05
static void BTApp_FreeOtherApp( CBTApp* pMe );
static void BTApp_CreateOtherApp( CBTApp* pMe );
static void BTApp_APPIsReadyTimer(void *pme);
//Add End


//Add By zzg 2011_06_24
static boolean BTApp_CheckNotify(CBTApp* pMe, AEEEvent evt, NotificationData* pData, uint16 NotifyType);
static void BTApp_HandleNotify(CBTApp* pMe, AEEEvent evt, NotificationData* pData, uint16 NotifyType);
static boolean BTApp_CheckAudioDeviceList(CBTApp* pMe);
//Add End


static void BTApp_HandleEventDevRedial( CBTApp* pMe );


//static void BTApp_DisableBT( CBTApp* pMe );


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION



//Add By zzg 2011_01_04
#ifndef WIN32
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                                        void     *ph,
                                                        IModule **ppMod)
#else
/*static*/ int BTAppMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif
#else
/*static*/ int BTAppMod_Load(IShell   *pIShell,
                                                   void     *ph,
                                                   IModule **ppMod)
#endif//WIN32
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return BTAppMod_New(sizeof(BTAppMod),
                    pIShell,
                    ph,
                    ppMod,
                    NULL,
                    NULL);
}

int  BTAppMod_New(int16  nSize,
    IShell   *pIShell,
    void     *ph,
    IModule  **ppMod,
    PFNMODCREATEINST pfnMC,
    PFNFREEMODDATA pfnMF
)
{
    PARAM_NOT_REF(nSize)
    PARAM_NOT_REF(pfnMC)
    PARAM_NOT_REF(pfnMF)
    
#ifndef AEE_SIMULATOR
    PARAM_NOT_REF(ph)
#endif

    if (NULL == ppMod)
    {
        return EFAILED;
    }

    *ppMod = NULL;

    // 重要提示：:
    //        g_dwAEEStdLibEntry全局变量仅用于SDK ！
    //        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
    //        。
#ifndef WIN32
#ifdef AEE_SIMULATOR
    g_dwAEEStdLibEntry = (uint32)ph;
    if (!pIShell || !ph)
    {
        return EFAILED;
    }
#else
    if (!pIShell)
    {
        return EFAILED;
    }
#endif
#else
	if (!pIShell)
    {
        return EFAILED;
    }
#endif//WIN32

    MEMSET(&gBTAppMod, 0, sizeof(BTAppMod));

    INIT_VTBL(&gBTAppMod, IModule, gModFuncs);
	
    gBTAppMod.m_nRefs = 1;
    *ppMod = (IModule *)&gBTAppMod;
    return AEE_SUCCESS;
}

static uint32 BTAppMod_AddRef(IModule *po)
{
    return(++((BTAppMod *)po)->m_nRefs);
}

static uint32 BTAppMod_Release(IModule *po)
{
    if (((BTAppMod *) po)->m_nRefs == 0)
    {
        return 0;
    }
    return(--((BTAppMod *)po)->m_nRefs);
}

static int  BTAppMod_CreateInstance(IModule *po,
    IShell *pIShell,
    AEECLSID ClsId,
    void **ppObj
)
{
    *ppObj = NULL;

	MSG_FATAL("***zzg BTAppMod_CreateInstance***", 0, 0, 0);
	
    if (ClsId != AEECLSID_BLUETOOTH_APP)
    {
        return EFAILED;
    }
    if (BTApp_New(pIShell, po, (IBTApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }
    return SUCCESS;
}
static void BTAppMod_FreeResources(IModule  *po,
    IHeap  *ph,
    IFileMgr *pfm
)
{
    PARAM_NOT_REF(po)
    PARAM_NOT_REF(ph)
    PARAM_NOT_REF(pfm)
}

static int BTApp_New(IShell *ps,
    IModule *pIModule,
    IBTApp **ppObj
)
{
    int retVal = SUCCESS;

	MSG_FATAL("***zzg BTApp_New Start gBTApp.m_nRefs=%x***", gBTApp.m_nRefs, 0, 0);

    if (0 == gBTApp.m_nRefs)
    {
        // Must initialize the object
        MEMSET(&gBTApp,  0, sizeof(CBTApp));
        INIT_VTBL(&gBTApp, IBTApp, gBTAppMethods);

        gBTApp.m_nRefs     = 0;
        gBTApp.m_pShell    = ps;
        gBTApp.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

		if (!BTApp_InitAppData(&gBTApp))
		{
			retVal = EFAILED;
		}
        
    }

    ++gBTApp.m_nRefs;
    *ppObj = (IBTApp*)&gBTApp;
    return retVal;
}


static uint32  BTApp_AddRef(IBTApp *pi)
{
    register CBTApp *pMe = (CBTApp*)pi;

    ASSERT(pMe != NULL);
    ASSERT(pMe->m_nRefs > 0);

    return (++pMe->m_nRefs);
}
static uint32  BTApp_Release (IBTApp *pi)
{
    register CBTApp *pMe = (CBTApp*)pi;

    ASSERT(pMe != NULL);

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    // 释放Applet相关资源
    BTApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意：pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}


static void BTApp_RunFSM(CBTApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;

	MSG_FATAL("***zzg BTApp_RunFSM***", 0, 0, 0);
		
    for ( ; ; )
    {
        nextFSMAction = BTApp_ProcessState(pMe);

        if (pMe->m_bNotOverwriteDlgRet)
        {
            pMe->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            pMe->m_eDlgRet = DLGRET_CREATE;
        }

        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*************************************************************************************/
/*****                     BT APP EVENT HANDLERS                                 *****/
/*************************************************************************************/
static void BTApp_OnAppStop( CBTApp* pMe )
{
  MSG_FATAL("***zzg BTApp_OnAppStop TOP_MENU=%d***", TOP_MENU, 0, 0);
  switch ( TOP_MENU )
  {
    case BT_APP_MENU_SEARCH:
    case BT_APP_MENU_DEV_RESP:
      if ( pMe->mSD.bDiscovering )
      {
        IBTEXTSD_StopDeviceDiscovery( pMe->mSD.po );
      }
      BTApp_CancelDevNameRequest( pMe );
      break;
    case BT_APP_MENU_SPP_RESULTS:
      BTApp_EndSPPTest( pMe );
      break;

#ifdef FEATURE_BT_EXTPF_OPP
    case BT_APP_MENU_OPP_SETTING:	//Add By zzg 2010_11_17
	case BT_APP_MENU_OPP_SENDFILE:	//Add By zzg 2010_11_09
    case BT_APP_MENU_OPP_TESTS:
    case BT_APP_MENU_OPP_SERVER:
    case BT_APP_MENU_OPP_CLIENT:
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
      BTApp_OPPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif
  
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_TESTS:
    case BT_APP_MENU_FTP_SERVER:
    case BT_APP_MENU_FTP_CLIENT:
      BTApp_FTPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif
  
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_TESTS:
    case BT_APP_MENU_BIP_SERVER:
    case BT_APP_MENU_BIP_CLIENT:
      BTApp_BIPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif
  
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_TESTS:
    case BT_APP_MENU_BPP_LIST_MIME:
    case BT_APP_MENU_BPP_LIST_TARGET:
      BTApp_BPPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif

#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_TESTS:
    case BT_APP_MENU_OBEX_SERVER:
    case BT_APP_MENU_OBEX_CLIENT:
      BTApp_OBEXHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif

#ifdef FEATURE_BT_EXTPF_CTP
    case BT_APP_MENU_CTP_TESTS:
      BTApp_CTPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
    case BT_APP_MENU_ICP_TESTS:
      BTApp_ICPHandleMenus( pMe, AVK_CLR, TOP_MENU );
      break;
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  }

  pMe->uTopMenu = 0;
  TOP_MENU      = BT_APP_MENU_MAIN;
}


static boolean BTApp_CheckCoreAppState(IShell* piShell)
{
	MSG_FATAL("***zzg BTApp_CheckCoreAppState piShell=%x***", piShell, 0, 0);
	
	if (NULL != piShell)
	{
		IAppletCtl *pAppCtl = NULL;
		uint32 nState;
		int nSize = sizeof(nState);

		// Create an instance of appletctl
		(void)ISHELL_CreateInstance(piShell, AEECLSID_APPLETCTL, (void **)&pAppCtl);

		MSG_FATAL("***zzg AEECLSID_APPLETCTL pAppCtl=%x***", pAppCtl, 0, 0);		
		
		if (pAppCtl) 
		{
			// Read the state of the app
			if (SUCCESS == IAPPLETCTL_GetRunningInfo(pAppCtl, AEECLSID_CORE_APP,
			                                       AEE_APPITEM_STATE, &nState, &nSize)
			  && nState != APPSTATE_STOPPED 
			  && nState != APPSTATE_CLOSING) 
			{
				MSG_FATAL("***zzg IAPPLETCTL_GetRunningInfo nState=%x***", nState, 0, 0);				
				return TRUE;
			}
			else
			{
				MSG_FATAL("***zzg IAPPLETCTL_GetRunningInfo nState=%x***", nState, 0, 0);
			}
			
		}
	}

	// No App information OR app is not running in background mode
	return FALSE;
}

static boolean BTApp_CheckMusicPlayrState(IShell* piShell)
{
	MSG_FATAL("***zzg BTApp_CheckMusicPlayrState piShell=%x***", piShell, 0, 0);
	
	if (NULL != piShell)
	{
		IAppletCtl *pAppCtl = NULL;
		uint32 nState;
		int nSize = sizeof(nState);

		// Create an instance of appletctl
		(void)ISHELL_CreateInstance(piShell, AEECLSID_APPLETCTL, (void **)&pAppCtl);

		MSG_FATAL("***zzg AEECLSID_APPLETCTL pAppCtl=%x***", pAppCtl, 0, 0);		
		
		if (pAppCtl) 
		{
			// Read the state of the app
			if (SUCCESS == IAPPLETCTL_GetRunningInfo(pAppCtl, AEECLSID_APP_MUSICPLAYER,
			                                       AEE_APPITEM_STATE, &nState, &nSize)
			  && nState != APPSTATE_STOPPED 
			  && nState != APPSTATE_CLOSING) 
			{
				MSG_FATAL("***zzg IAPPLETCTL_GetRunningInfo nState=%x***", nState, 0, 0);				
				return TRUE;
			}
			else
			{
				MSG_FATAL("***zzg IAPPLETCTL_GetRunningInfo nState=%x***", nState, 0, 0);
			}
			
		}
	}

	// No App information OR app is not running in background mode
	return FALSE;
}



static void BTApp_GetAppList(IShell* piShell)
{
	int i;
	IAppletCtl *pAppCtl = NULL;
	uint32 nAppID[10];		
	int nAppSize = sizeof(nAppID);
	
	if (NULL != piShell)
	{
		// Create an instance of appletctl
		(void)ISHELL_CreateInstance(piShell, AEECLSID_APPLETCTL, (void **)&pAppCtl);

		MSG_FATAL("***zzg AEECLSID_APPLETCTL pAppCtl=%x***", pAppCtl, 0, 0);		
		
		if (pAppCtl) 
		{			
			// Read the state of the app
			if (SUCCESS != IAPPLETCTL_GetRunningList(pAppCtl, nAppID, &nAppSize))	
			{
				MSG_FATAL("***zzg IAPPLETCTL_GetRunningList Failed***", 0, 0, 0);
			}			
		}
	}

	for (i=0; i<10; i++)
	{
		MSG_FATAL("***zzg BTApp_GetAppList nAppID[%d]=%x***", i, nAppID[i], 0);
	}
	
	return;
}

//Add By zzg 2011_06_24
static boolean BTApp_CheckNotify(CBTApp* pMe, AEEEvent evt, NotificationData* pData, uint16 NotifyType)
{
	boolean result = FALSE;

	if (pMe == NULL)
	{
		return;
	}

	MSG_FATAL("***zzg BTApp_CheckNotify NotifyType=%x, evt=%x***", NotifyType, evt, 0);

	//result == TRUE : Need BTApp EVT_START
	//result == FALSE: Need not
	
	switch (NotifyType)
	{
		case NMASK_BT_AG:
		{	
			switch (evt)
			{
				case AEEBT_AG_EVT_CONNECTED:
				case AEEBT_AG_EVT_CONNECT_FAILED:	
				case AEEBT_AG_EVT_DISCONNECTED:	
				{
					result = TRUE;
					break;
				}	
				
				/*
				case AEEBT_AG_EVT_CONFIGURED:
				case AEEBT_AG_EVT_ENABLED:	
				case AEEBT_AG_EVT_RUNG:
				case AEEBT_AG_EVT_VR_CAPABLE_SET:     
			    case AEEBT_AG_EVT_SPKR_GAIN_SET:     
			    case AEEBT_AG_EVT_MIC_GAIN_SET:	
				case AEEBT_AG_EVT_AUDIO_CONNECTED:	
				case AEEBT_AG_EVT_AUDIO_CONNECT_FAILED:
				case AEEBT_AG_EVT_AUDIO_DISCONNECTED:	
				case AEEBT_AG_EVT_DISABLED:	
				case AEEBT_AG_EVT_BUTTON_PRESSED:
				case AEEBT_AG_EVT_SPKR_GAIN_REPORT:
				case AEEBT_AG_EVT_MIC_GAIN_REPORT:
				case AEEBT_AG_EVT_SLC_UP:
				case AEEBT_AG_EVT_SLC_DOWN: 
				case AEEBT_AG_EVT_DEV_PICKUP: 
				case AEEBT_AG_EVT_DEV_HANGUP: 	
#ifdef FEATURE_PHONE_VR	
				case AEEBT_AG_EVT_VR_ON: 
				case AEEBT_AG_EVT_VR_OFF: 	
#endif					
				case AEEBT_AG_EVT_DEV_DIAL:
#ifndef FEATURE_BT_HFP_1_5
				case AEEBT_AG_EVT_DEV_MEM_DIAL: 
				case AEEBT_AG_EVT_DEV_REDIAL:	
#endif
				case AEEBT_AG_EVT_DEV_SEND_DTMF:
				case AEEBT_AG_EVT_VR_STATE_UPDATED:
#ifdef FEATURE_BT_HFP_1_5
				case AEEBT_AG_EVT_EXTERNAL_IO_ENABLED:
				case AEEBT_AG_EVT_EXTERNAL_IO_DISABLED:
				case AEEBT_AG_EVT_RSSI_SET:
				case AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET:
				case AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET:	
#endif

#ifdef FEATURE_AVS_BT_SCO_REWORK
				case AEEBT_AG_EVT_SOUND_CHANGING:
#endif
				*/
				default:
				{
					break;
				}
				//Needn't  End
			}
			break;
		}
		case NMASK_BT_NA:
		{			
			switch (evt)
			{
				case AEEBT_NA_EVT_CONNECTED:
				case AEEBT_NA_EVT_DISCONNECTED:	
				{					
					result = TRUE;
					break;				
				}

				case AEEBT_NA_EVT_ENABLED:
				case AEEBT_NA_EVT_DISABLED:
				default:
				{
					break;
				}
			}
			break;
		}
		case NMASK_BT_RM:
		{			
			switch (evt)
			{	
				case AEEBT_RM_EVT_LOCAL_NAME_SET:
#ifdef FEATURE_BT_2_1					
				case AEEBT_RM_EVT_NICK_NAME_SET:
				case AEEBT_RM_EVT_MANUF_DATA_SET:	
				case AEEBT_RM_EVT_KEYPRESS_NOTIFICATION:					
				{			
					break;
				}	
				
				case AEEBT_RM_EVT_PASSKEY_REQUEST:					
				{
					result = TRUE;
					break;
				}

				case AEEBT_RM_EVT_PASSKEY_REPLIED:
				{
					if ( pData->uError !=  AEEBT_RM_ERR_NONE)
					{
						result = TRUE;
					}
					break;
				}
#endif				
				case AEEBT_RM_EVT_LOCAL_COD_SET:
				case AEEBT_RM_EVT_LOCAL_SECURITY_SET:	
				{
					break;
				}

				case AEEBT_RM_EVT_PIN_REQUESTED:	
				case AEEBT_RM_EVT_BONDED:	
#ifdef FEATURE_BT_2_1
				case AEEBT_RM_EVT_REBOND_REQUEST:
				{
					result = TRUE;
					break;
				}
				case AEEBT_RM_EVT_PASSKEY_NOTIFICATION:		//????
				{
					break;
				}
#endif				
				case AEEBT_RM_EVT_UNBONDED: 
#ifdef FEATURE_BT_2_1
   				case AEEBT_RM_EVT_USER_CFM_REQUEST: 	
#endif
				{
					result = TRUE;
					break;
				}

				case AEEBT_RM_EVT_BONDABLE_MODE:  
				case AEEBT_RM_EVT_PIN_REPLIED:	
#ifdef FEATURE_BT_2_1
				case AEEBT_RM_EVT_USER_CFM_REPLIED:
#endif
				{
					if (pData->uError != AEEBT_RM_ERR_NONE)
					{
						result = TRUE;						
					}
					break;
				}				

				case AEEBT_RM_EVT_DEVICE_ADDED:
				case AEEBT_RM_EVT_DEVICE_REMOVED:	
				case AEEBT_RM_EVT_TEST_MODE_ENTERED:
				case AEEBT_RM_EVT_ROLE_SW_CTRL_SET:
				case AEEBT_RM_EVT_RADIO_STATUS:
				case AEEBT_RM_EVT_VISIBILITY_STATUS:	
				case AEEBT_RM_EVT_CONN_ROLE_SET:	
				{
					result = TRUE;
					break;
				}

				case AEEBT_RM_EVT_DEVICE_UPDATED:
				case AEEBT_RM_EVT_LINK_STATUS_REG:
				case AEEBT_RM_EVT_LINK_STATUS:
				case AEEBT_RM_EVT_DEVICE_SECURITY_SET:
				case AEEBT_RM_EVT_SERVICE_SECURITY_SET:
				case AEEBT_RM_EVT_AUTHORIZE_REQUEST:
				case AEEBT_RM_EVT_AUTHORIZE_REPLIED:
				case AEEBT_RM_EVT_TEST_MODE_EXITED:
				case AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED:	
				case AEEBT_RM_EVT_CONN:
				case AEEBT_RM_EVT_DISCONN:
				case AEEBT_RM_EVT_APPLICATION_CONFIGURED:
#ifdef FEATURE_BT_2_1
				case AEEBT_RM_EVT_KEYPRESS_NOTIFICATION_REPLIED:	
				case AEEBT_RM_EVT_LPM_CONFIGURED:
#endif
				default:
				{
					break;
				}				
			}
			break;
		}
		case NMASK_BT_SD:
		{			
			break;
		}
		case NMASK_BT_SPP:
		{
			break;
		}
		case NMASK_BT_L2:
		{			
			break;
		}
#ifdef FEATURE_BT_EXTPF_OPP				
		case NMASK_BT_OPP:
		{		
			switch (evt)
			{
				case AEEBT_OPP_EVT_CONNECTED:
				{
					if (pMe->m_pActiveDlgID == IDD_BT_OBEX_LIST_SERVERS)
					{
						result = TRUE;
					}
					break;
				}
				//case AEEBT_OPP_EVT_PUSH_REQ:		//Del by zzg 2012_11_28
				case AEEBT_OPP_EVT_PROGRESS:
				{
					result = TRUE;
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
#endif				
#ifdef FEATURE_BT_EXTPF_FTP
		case NMASK_BT_FTP:
		{			
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_BIP		
		case NMASK_BT_BIP:
		{			
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_BPP
		case NMASK_BT_BPP:
		{			
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
		case NMASK_BT_PBAP:
		{			
			break;
		}
#endif
		
#ifdef FEATURE_BT_EXTPF_HID_HOST
		#error code not present
#endif
		
#ifdef FEATURE_BT_EXTPF_CTP
		case NMASK_BT_CTP:
		{
			if ( evt & AEEBT_ICP_EVT_BASE ) 
			{
				
			}
			else
			{
				
			}
			break;
		}
#endif
		
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
		#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
		case NMASK_BT_A2DP:
		{

			MSG_FATAL("***zzg BTApp_CheckNotify NMASK_BT_A2DP, evt=%x***", evt, 0, 0);
			
			switch (evt)
			{
				case AEEBT_A2DP_EVT_CONNECTED:
				case AEEBT_A2DP_EVT_CON_FAILED:	
				case AEEBT_A2DP_EVT_DISCONNECTED:	
				{
					//result = TRUE;
					break;
				}
				
				case AEEBT_A2DP_EVT_START:
				{
#ifdef FEATURE_APP_MUSICPLAYER
	                (void)ISHELL_SendEvent(pMe->m_pShell,
                                           AEECLSID_APP_MUSICPLAYER,
                                           EVT_BT_A2DP_RESUME,
                                           0,
                                           0);                	
#endif
					break;
				}
				case AEEBT_A2DP_EVT_SUSPEND:
				{
#ifdef FEATURE_APP_MUSICPLAYER
					(void)ISHELL_SendEvent(pMe->m_pShell,
										   AEECLSID_APP_MUSICPLAYER,
										   EVT_BT_A2DP_PAUSE,
										   0,
										   0);						
					
#endif
					break;
				}	

				case AEEBT_A2DP_EVT_BITRATE:				
				case AEEBT_A2DP_EVT_OPEN:
				case AEEBT_A2DP_EVT_CLOSE:
				default:
				{
					break;
				}				
			}
			break;
	  	}
		case NMASK_BT_AVRCP:
		{			
			break;
		}
#endif
		default:
		{
			break;
		}		
	}

	MSG_FATAL("***zzg BTApp_CheckNotify result=%x***", result, 0, 0);
	
	return result;
}
static void BTApp_HandleNotify(CBTApp* pMe, AEEEvent evt, NotificationData* pData, uint16 NotifyType)
{
	if (pMe == NULL)
	{
		return;
	}

	MSG_FATAL("***zzg BTApp_HandleNotify NotifyType=%x, evt=%x***", NotifyType, evt, 0);
	
	switch (NotifyType)
	{
		case NMASK_BT_AG:
		{
			BTApp_ProcessAGNotifications(pMe, evt, pData);	
			break;
		}
		case NMASK_BT_NA:
		{
			BTApp_ProcessNANotifications(pMe, evt, pData);	
			break;
		}
		case NMASK_BT_RM:
		{
			BTApp_ProcessRMNotifications(pMe, evt, pData);	
			break;
		}
		case NMASK_BT_SD:
		{
			BTApp_ProcessSDNotifications(pMe, evt, pData);	
			break;
		}
		case NMASK_BT_SPP:
		{
			BTApp_ProcessSPPNotifications(pMe, evt, pData);
			break;
		}
		case NMASK_BT_L2:
		{
			BTApp_ProcessL2Notifications(pMe, evt, pData);	
			break;
		}
#ifdef FEATURE_BT_EXTPF_OPP				
		case NMASK_BT_OPP:
		{
			BTApp_ProcessOPPNotifications(pMe, evt, pData);	
			break;
		}
#endif				
#ifdef FEATURE_BT_EXTPF_FTP
		case NMASK_BT_FTP:
		{
			BTApp_ProcessFTPNotifications(pMe, evt, pData);	
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_BIP		
		case NMASK_BT_BIP:
		{
			BTApp_ProcessBIPNotifications(pMe, evt, pData);	
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_BPP
		case NMASK_BT_BPP:
		{
			BTApp_ProcessBPPNotifications(pMe, evt, pData);	
			break;
		}
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
		case NMASK_BT_PBAP:
		{
			BTApp_ProcessPBAPNotifications(pMe, evt, pData);	
			break;
		}
#endif
		
#ifdef FEATURE_BT_EXTPF_HID_HOST
		#error code not present
#endif
		
#ifdef FEATURE_BT_EXTPF_CTP
		case NMASK_BT_CTP:
		{
			if ( evt & AEEBT_ICP_EVT_BASE ) 
			{
				BTApp_ProcessICPNotifications(pMe, evt, pData);
			}
			else
			{
				BTApp_ProcessCTPNotifications(pMe, evt, pData);
			}
			break;
		}
#endif
		
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
		#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
		case NMASK_BT_A2DP:
		{
			BTApp_ProcessA2DPNotifications(pMe, evt, pData);

			MSG_FATAL("***zzg BTApp_HandleNotify NMASK_BT_A2DP evt=%x***", evt, 0, 0);

			switch (evt)
			{
				case AEEBT_A2DP_EVT_START:
				{
					if ((((NotificationData*)pData)->A2DPStartInit == AEEBT_A2DP_EVENT_LOCAL_INIT) 
						||(pMe->mAG.callState == BTAPP_AG_CALL_NONE))
					{
						BTApp_DisconnectAudio( pMe, pMe->mAG.bForceUnmute );
					}
					// else, ignore remote-initiated Start command because  a call is present and call audio must be left alone 
					break;
				}

				case AEEBT_A2DP_EVT_SUSPEND:
				{
					MSG_FATAL( "AG: audioConnecting=%d callState=%d", pMe->mAG.bAudioConnecting, pMe->mAG.callState, 0);

					// did we suspend A2DP so we can bring up SCO? 
					if ( pMe->mAG.bAudioConnecting == TRUE )
					{
						// carry out pending SCO connection 
						pMe->mAG.bAudioConnecting = FALSE;
						BTApp_ConnectAudio( pMe, pMe->mAG.bForceUnmute );
					}
					else if ((pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_INBAND)
						|| (pMe->mAG.callState == BTAPP_AG_CALL_CONNECTED))
					{
						BTApp_ConnectAudio( pMe, TRUE );
					}
					break;
				}
			}
			break;
	  	}
		case NMASK_BT_AVRCP:
		{
			BTApp_ProcessAVRCPNotifications( pMe, evt, pData);
			break;
		}
#endif
		default:
		{
			break;
		}		
	}
}

static boolean BTApp_CheckAudioDeviceList(CBTApp* pMe)
{
	boolean result = FALSE;
	
	AEEBTDeviceEnumerator enumerator;
	AEEBTDeviceInfo*      pDev;
	uint8                 i;
	uint8				  j;
	uint8				  agcount=0;

	if (pMe == NULL)
	{
		return FALSE;
	}
		
	enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
	enumerator.bBonded = TRUE;			

	if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
	{
		MSG_FATAL("***zzg BTApp_CheckAudioDeviceList IBTEXTRM_DeviceEnumInit SUCCESS***", 0, 0, 0);		
		i	= 0;
		j	= 0;
		pDev = &pMe->mRM.device[i];

		while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev) == SUCCESS) 
				&& pDev->bValid && (i < MAX_DEVICES))
		{
			MSG_FATAL("***zzg BTApp_CheckAudioDeviceList IBTEXTRM_DeviceEnumNext SUCCESS***", 0, 0, 0);
					

			// find out if this is an audio device
			if ((pDev->majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO) 
				|| (pDev->serviceClass & AEEBT_COD_SC_AUDIO))
			{
				agcount++;
			}
			else
			{
				while (j < pDev->uNumSvcs) 
				{
					if ((pDev->svcs[j].uServiceClass == AEEBT_SD_SERVICE_CLASS_HEADSET) 
						||(pDev->svcs[j].uServiceClass == AEEBT_SD_SERVICE_CLASS_HANDSFREE))
					{
						agcount++;
						break;
					}
					j++;
				}
			}

			pDev = &pMe->mRM.device[++i];
			
		}
			
	}					

	MSG_FATAL("***zzg AudioDevice count = %x***", agcount, 0, 0);
	return ((agcount>0)? TRUE:FALSE);
}

//Add End


static boolean BTApp_HandleEvent(IBTApp *pi,
										  AEEEvent eCode,
										  uint16  wParam,
										  uint32 dwParam
)
{
    CBTApp *pMe = (CBTApp*)pi;	
	boolean event_processed = TRUE;
	uint16 msgID;
	
	AEEAppStart *args = (AEEAppStart*)dwParam;		//Add By zzg 2010_11_08 for Start App with args

#ifdef FEATURE_BT_EXTPF_HID_HOST
	#error code not present
#endif

  	AEEBTHCIMode HCIMode;

#ifdef FEATURE_APP_TEST_AUTOMATION  
	#error code not present
#endif 
	
    AEEAppStart *as;
    AEEDeviceInfo di; 

    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);  

	MSG_FATAL("***zzg BTApp_HandleEvent eCode=%x %d***", eCode, pMe->mEnablingType, 0);
    
    switch (eCode)
    {	     	
		case EVT_APP_START_BACKGROUND:		
		{
			MSG_FATAL("***zzg BTApp Start Background***", 0, 0, 0);			

			if ((args != NULL) && (args->pszArgs != NULL))	
			{
				if (STRNCMP(args->pszArgs, "ResetBT", 7) == 0)	//Factory Reset					
				{
					MSG_FATAL("***zzg ResetBT BTApp***", 0, 0, 0);

					bt_rm_refresh_efs_param();		
					
					BTApp_DisableBT(pMe);
					break;	
				}
			}
			else
			{
				BTApp_StartBT(pMe);		
			}
			 
      		return (TRUE);
		}		
		
        case EVT_APP_START:				
		{			
            ASSERT(dwParam != 0);
            as = (AEEAppStart*)dwParam;

			pMe->m_bAppIsReady = FALSE;
			pMe->bStartBTaplication = TRUE;

			MSG_FATAL("***zzg BTApp_Handle EVT_APP_START***", 0, 0, 0);
						
			if ((args != NULL) && (args->pszArgs != NULL))	
			{
				//Send File from T_Card.....the filepath was args->pszArgs
				//GetFile(OPPPush)//RestBT(Reset  Factory)
				if ((STRNCMP(args->pszArgs, "GetFile", 7) != 0)
					&& (STRNCMP(args->pszArgs, "BtHeadSet", 9) != 0)					
					&& (STRNCMP(args->pszArgs, "ResetBT", 7) != 0))
				{
					DBGPRINTF(("***zzg args->pszArgs=%s***", args->pszArgs));
					pMe->bStartFromOtherApp	= TRUE;		
					BTApp_SaveSendFilePath(pMe, args->pszArgs);				
				}
			}	

			BTApp_CreateOtherApp(pMe);	
			
			if (!pMe->bSuspended )
			{
				event_processed = BTApp_Init(pMe);
			}
			
			if (event_processed)
			{				
				MSG_FATAL("***zzg mAG.bStartedVr=%x, mAG.callState=%x***", pMe->mAG.bStartedVr, pMe->mAG.callState, 0);
			
				if (pMe->mAG.bStartedVr != FALSE)
				{					
					pMe->mAG.bStartedVr = FALSE;
					IBTEXTAG_UpdateVRState(pMe->mAG.po, FALSE);

					if (pMe->mAG.callState == BTAPP_AG_CALL_STARTVR)
					{
						pMe->mAG.callState = BTAPP_AG_CALL_NONE;
#ifndef FEATURE_AVS_BT_SCO_REWORK
						BTApp_ReleaseBTDevice(pMe, FALSE);
#endif 
					}
				}
				
				//Add By zzg 2010_11_09
				// Send File from T_Card
				if (TRUE == pMe->bStartFromOtherApp)
				{
					boolean bt_status = FALSE;
					
					ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));

					if (bt_status == FALSE)	
					{		
						if (BTApp_HCIModeOn(pMe) == FALSE)
						{
                            pMe->mEnablingType = BTAPP_ENABLING_AG;
							BTApp_EnableBT(pMe);	
						}							
					}
					
					if (BTApp_HCIModeOn(pMe) == FALSE)
					{
						MSG_FATAL("***zzg BTApp EVT_APP_START  BT_APP_MENU_OPP_SENDFILE***",0,0,0);

						if (BTApp_OPPInit(pMe) != FALSE)
						{
							MSG_FATAL("***zzg BTApp_OPPBuildMenu BT_APP_MENU_OPP_SENDFILE***", 0, 0, 0);
							//MOVE_TO_STATE(BTAPPST_BT_SEND_FILE)		//直接用CLIENT(如果是SERVER状态，则切换)	
							
							{
								AEEBTDeviceEnumerator enumerator;
								uint8				  i;						
								AEEBTDeviceInfo*	  pDev;						

								enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
								enumerator.svcCls  = AEEBT_COD_SC_OBJECT_TRANSFER;
									
								if (pMe->mRM.po == NULL)
								{
									MSG_FATAL("***zzg ObexListServers pMe->mRM.po == NULL***", 0, 0, 0);
								}
								else
								{				
									if (IBTEXTRM_DeviceEnumInit(pMe->mRM.po, &enumerator) == SUCCESS)
									{		
										i	 = 0;
										pDev = &pMe->mRM.device[i];
					
										while ((IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) 
												&& pDev->bValid && (i < MAX_DEVICES))
										{
											pDev = &pMe->mRM.device[ ++i ];
										}
									}
								}

								if (i > 0)
								{
									MOVE_TO_STATE(BTAPPST_BT_OBEX_LIST_SERVERS)	//Modify by zzg 2012_11_22
								}
								else
								{
									MOVE_TO_STATE(BTAPPST_DEVICESRH)			//Modify by zzg 2012_11_22;
								}				
							}								
						}
						else
						{
							MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
							BTApp_OPPCleanup( pMe );								
						}	
					}					
					
				}
				/*
				else if (STRNCMP(args->pszArgs, "GetFile", 7) == 0)	//OPP Get File
				{
					MSG_FATAL("***zzg GetFile StartBTApp***", 0, 0, 0);

					if (BTApp_HCIModeOn(pMe) == FALSE)
					{
						if (BTApp_OPPInit(pMe) != FALSE)
						{
							//MOVE_TO_STATE(BTAPPST_BT_FILE_PROGRESS)		//直接用CLIENT(如果是SERVER状态，则切换)									
							pMe->m_eDlgRet = DLGRET_BT_FILE_PROGRESS; 
							(void) ISHELL_EndDialog(pMe->m_pShell);	
						}
						else
						{
							MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
							BTApp_OPPCleanup(pMe);								
						}		
					}

					pMe->bFirstLaunch = FALSE;
					pMe->bSuspended = FALSE;	
					
				}
				*/
				else if (STRNCMP(args->pszArgs, "ResetBT", 7) == 0)	//Factory Reset					
				{
					MSG_FATAL("***zzg ResetBT BTApp***", 0, 0, 0);

					bt_rm_refresh_efs_param();		
					
					BTApp_DisableBT(pMe);
					break;	
				}	
				else if (STRNCMP(args->pszArgs, "BtHeadSet", 9) == 0)					
				{					
					boolean bt_status = FALSE;

					MSG_FATAL("***zzg SearchAudio BTApp***", 0, 0, 0);
					
					ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));

					if (bt_status == FALSE)	
					{		
						if (BTApp_HCIModeOn(pMe) == FALSE)
						{
                            pMe->mEnablingType = BTAPP_ENABLING_AG;
							BTApp_EnableBT(pMe);	
						}							
					}
					
					if (BTApp_HCIModeOn(pMe) == FALSE)
					{
						if (BTApp_OPPInit(pMe) != FALSE)
						{
							MSG_FATAL("***zzg BTApp AG: enable=%x, connect=%x, audioconncect=%x***", 
									pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.bAudioConnected);
							MSG_FATAL("***zzg BTApp AG: bconnInPrgs=%x, bAudioConnecting=%x, bAudioDisconnecting=%x***", 
										pMe->mAG.bconnInPrgs, pMe->mAG.bAudioConnecting, pMe->mAG.bAudioDisconnecting);

							pMe->bStartFromAudio = TRUE;		//Add By zzg 2012_11_28
								
							if (TRUE == BTApp_CheckAudioDeviceList(pMe))
							{
								pMe->uDeviceListType = AEEBT_RM_EC_MATCH_VALUE_1;	//AudioDeviceList
								MOVE_TO_STATE(BTAPPST_DEVICE_LIST);
							}
							else
							{
								pMe->uDeviceSrhType		= AEEBT_COD_SC_AUDIO;	//Add By zzg 2011_10_19
								MOVE_TO_STATE(BTAPPST_DEVICESRH);
							}
						}
						else
						{
							MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
							BTApp_OPPCleanup( pMe );								
						}	
					}					
				}				
				else
				{					
					//BTApp_BuildTopMenu( pMe );
				}
				//Add End        
				
				IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
				MSG_FATAL("***zzg HCIMode=%x***", HCIMode, 0, 0);
				if (HCIMode != AEEBT_HCIM_OFF)
				{
					BTApp_ShowMessage(pMe, IDS_MSG_HCI_ON_WARNING, NULL, 0);
				}				
			}
			
			pMe->bFirstLaunch = FALSE;
			pMe->bSuspended = FALSE;	
		    //Add End
						
            if (NULL != pMe->m_pIDisplay)
            {            
                (void) IDISPLAY_Release(pMe->m_pIDisplay);
                pMe->m_pIDisplay = NULL;
            }
			
            pMe->m_pIDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pIDisplay);					
			
            pMe->m_rect = as->rc;
            pMe->m_rect.dy = di.cyScreen;
           
            pMe->m_bSuspending = FALSE;					
			
			// 开始BTApp状态机
			BTApp_RunFSM(pMe); 			

			//Add By zzg 2011_01_20: 在非蓝牙应用界面收到NOTIFY、EVT_USER
			if (pMe->m_app_flag == TRUE)
			{
				MSG_FATAL("***zzg pMe->m_app_flag == TRUE***", 0, 0, 0);
				MSG_FATAL("***zzg pMe->m_user_wParam == %d***", pMe->m_user_wParam, 0, 0);
				DBGPRINTF("***zzg NotifyType == %d***", NotifyType);
				DBGPRINTF("***zzg m_user_wParam = %d***", pMe->m_user_wParam);

				if (pMe->m_user_wParam == AEEBT_RM_EVT_CONN)
				{
					AEEBTDeviceInfo dev;
					DBGPRINTF("***zzg EVT_START RM NotifyData: %04x%04x%04x", 
					        ((uint16)(NotifyData.bdAddr.uAddr[5] << 8) | NotifyData.bdAddr.uAddr[4]), 
					        ((uint16)(NotifyData.bdAddr.uAddr[3] << 8) | NotifyData.bdAddr.uAddr[2]), 
					        ((uint16)(NotifyData.bdAddr.uAddr[1] << 8) | NotifyData.bdAddr.uAddr[0]));
				}
				
				BTApp_HandleNotify(pMe, pMe->m_user_wParam, (void *)&NotifyData, NotifyType);
				
				pMe->m_user_wParam = 0;
				pMe->m_app_flag = FALSE;

				//return TRUE;
			}					
			
            return TRUE;
    	}		

#ifndef FEATURE_UIONE_HDK
		case EVT_FIRST_LAUNCH:
		{
			// starting for the first time on power up
			MSG_HIGH( "HndlEv - START", 0, 0, 0 );
			event_processed = BTApp_StartBT( pMe ); 
			break;
		}
#endif
		
        case EVT_APP_STOP:
        {
			boolean* pb = (boolean*)dwParam;

			MSG_FATAL("***zzg BTApp_HandleEvt EVT_APP_STOP dw=0x%x***", dwParam, 0, 0);
			
			if (pb)
			{
				*pb = FALSE;  						// Set the app to background app 			
				
				pMe->bSuspended = TRUE;
				pMe->bStartFromOtherApp = FALSE;	//Add By zzg 2010_11_08
				pMe->bStartFromPushReq  = FALSE;	//Add By zzg 2012_11_23		
				pMe->bStartFromAudio	= FALSE;	//Add By zzg 2012_11_28
				pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27
				pMe->uDeviceSrhType		= AEEBT_COD_SC_ALL;	//Add By zzg 2011_10_19
				pMe->uDeviceListType	= AEEBT_RM_EC_ALL;	//Add By zzg // 2011_10_26
			}
			
			BTApp_OnAppStop(pMe);		
				  
			if (pMe->m_pIDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pIDisplay);
                pMe->m_pIDisplay = NULL;
            }

            pMe->m_bSuspending = TRUE;
			//BTApp_FreeOtherApp(pMe);

            return TRUE;
        }
        case EVT_APP_SUSPEND:
        {
			/*
			AEESuspendInfo* pi = (AEESuspendInfo*)dwParam;
			MSG_HIGH( "HndlEv - APP_SUSPEND wP=0x%x dw=0x%x", wParam, dwParam, 0 );
			
			if ( pi != NULL )
			{
				pi->bCloseDialogs = FALSE;
			}
			*/
			
			pMe->bSuspended = TRUE;			
			pMe->m_bSuspending = TRUE;
            return TRUE;
        }
        case EVT_APP_RESUME:
        {
			ASSERT(dwParam != 0);

			MSG_FATAL("***zzg BTApp_Handle  EVT_APP_RESUME***", 0, 0, 0);
			
            as = (AEEAppStart*)dwParam;
            pMe->m_bSuspending = FALSE;
			
			pMe->bStartBTaplication = TRUE;

            if (pMe->m_pIDisplay != NULL)
            {
                (void) IDISPLAY_Release(pMe->m_pIDisplay);
                pMe->m_pIDisplay = NULL;
            }
			
            pMe->m_pIDisplay = as->pDisplay;
            (void) IDISPLAY_AddRef(pMe->m_pIDisplay);
			
            pMe->m_rect = as->rc;
            pMe->m_rect.y = pMe->m_rect.y;
            pMe->m_rect.dy = di.cyScreen;
			
            ERR("pMe->m_rc.y:%d,pMe->m_rc.dy:%d",pMe->m_rect.y,pMe->m_rect.dy,0);
			
	        BTApp_RunFSM(pMe);	
			
            return TRUE;
        }
		
        case EVT_DIALOG_INIT:
        {
			MSG_FATAL("***zzg BTApp_Handle EVT_DIALOG_INIT m_pActiveDlgID=%d***", wParam, 0, 0);
			if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}
			pMe->m_bAppIsReady = FALSE;
            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;

            return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
        }

		
        case EVT_DIALOG_START:
        {
            if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}

            (void)ISHELL_SetTimer ( pMe->m_pShell,
                            		APPISREADY_TIMER,
                            		BTApp_APPIsReadyTimer,
                            		pMe);
            
			return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
            
        }
		
        case EVT_USER_REDRAW:
        {	
			(void) BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

             /*
			(void)ISHELL_SetTimer ( pMe->m_pShell,
                            		APPISREADY_TIMER,
                            		BTApp_APPIsReadyTimer,
                            		pMe);
            */
                           
            return TRUE;
        }
		
        case EVT_APPISREADY:
        {
			pMe->m_bAppIsReady = TRUE;
            return TRUE;          
        }
		
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        case EVT_KEY:
        case EVT_COMMAND:
        {
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99) && !defined(FEATURE_QVGA_INHERIT_K212) && !defined(FEATURE_VERSION_IN50A)
			if (!pMe->m_bAppIsReady)
            {
                return TRUE;
            }
			#endif

			MSG_FATAL( "HndlEv - EVT_KEY wP=0x%x dw=0x%x", wParam, dwParam, 0 );
			
			return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
        }
		
        case EVT_DIALOG_END:
        {
			if (wParam == 0)
            {
                return TRUE;
            }
			if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_BLUETOOTH_APP,EVT_USER_REDRAW,0,0);
			}              
            pMe->m_bAppIsReady = FALSE;
            (void) BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			
            pMe->m_pActiveDlg = NULL;

            // Applet被挂起，不跑状态机，等待Applet返回。
            //（注意：EVT_APP_SUSPEND  事件在EVT_DIALOG_END事件前发出。
            if (pMe->m_bSuspending == FALSE)
            {	
            	// 开始BTApp状态机            	
	            BTApp_RunFSM(pMe);            	
            }			
            return TRUE;
        }	
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		//case EVT_PEN_DOWN:
		{
			return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
		}


#endif//FEATURE_LCD_TOUCH_ENABLE

		case EVT_NOTIFY:
	    {
			AEENotify* pN = (AEENotify*) dwParam;
			AEEEvent  evt = GET_NOTIFIER_VAL(pN->dwMask);
			
			MSG_FATAL("***zzg BTApp_HandleEvent EVT_NOTIFY cls=%x, NOTIFIER_MASK=%x %d***", pN->cls, GET_NOTIFIER_MASK(pN->dwMask), evt);	
            
			MEMCPY(&NotifyData, pN->pData, sizeof(NotificationData));	
		
			if (pN->cls == AEECLSID_BLUETOOTH_NOTIFIER)
			{
				NotifyType = GET_NOTIFIER_MASK(pN->dwMask);
                
				if (TRUE == BTApp_CheckNotify(pMe, evt, pN->pData, NotifyType))
				{
					MSG_FATAL("***zzg BTApp_CheckNotify bDevPickedUp=%x %d***", pMe->mAG.bDevPickedUp, evt, 0);
					
					//if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
					if ((ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
						&& (FALSE == pMe->mAG.bDevPickedUp))
					{							
						pMe->m_app_flag = TRUE;
						pMe->m_user_wParam = evt;	

						ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
						break;
					}
				}
				
				//Add End
					
				switch (GET_NOTIFIER_MASK(pN->dwMask))
				{
				  case NMASK_BT_AG:
				  {
					BTApp_ProcessAGNotifications( pMe, evt ,pN->pData );
					break;
				  }
				  case NMASK_BT_NA:
				  {
				  	BTApp_ProcessNANotifications( pMe, evt, pN->pData );
				    break;
				  }
				  case NMASK_BT_RM:
				  { 
				  	BTApp_ProcessRMNotifications(pMe, evt, pN->pData);	
				    break;
				  }
				  case NMASK_BT_SD:
				  {
				  	BTApp_ProcessSDNotifications( pMe, evt, pN->pData );
				    break;
				  }
				  case NMASK_BT_SPP:
				  {
				  	BTApp_ProcessSPPNotifications( pMe, evt, pN->pData );
				    break;
				  }
				  case NMASK_BT_L2:
				  {
				  	BTApp_ProcessL2Notifications( pMe, evt, pN->pData );
				    break;
				  }
				  
#ifdef FEATURE_BT_EXTPF_OPP
				  case NMASK_BT_OPP:
				  {
				  	BTApp_ProcessOPPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif

#ifdef FEATURE_BT_EXTPF_FTP
				  case NMASK_BT_FTP:
				  {
				  	BTApp_ProcessFTPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif

#ifdef FEATURE_BT_EXTPF_BIP
				  case NMASK_BT_BIP:
				  {
				  	BTApp_ProcessBIPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif

#ifdef FEATURE_BT_EXTPF_BPP
				  case NMASK_BT_BPP:
				  {
				  	BTApp_ProcessBPPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
				  case NMASK_BT_PBAP:
				  {
				  	BTApp_ProcessPBAPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
					#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_CTP
				  case NMASK_BT_CTP:
				  {
				  	if ( evt & AEEBT_ICP_EVT_BASE ) 
				    {
				      BTApp_ProcessICPNotifications( pMe, evt, pN->pData );
				    }
				    else
				    {
				      BTApp_ProcessCTPNotifications( pMe, evt, pN->pData );
				    }
				    break;
				  }
#endif

#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
					#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
				  case NMASK_BT_A2DP:
				  {
				  	MSG_FATAL("***zzg BTApp_HandlEvt NMASK_BT_A2DP evt=%x***", evt, 0, 0);
					
				  	BTApp_ProcessA2DPNotifications( pMe, evt, pN->pData );
					
					switch (evt)
					{
						case AEEBT_A2DP_EVT_START:
						{
							MSG_FATAL("***zzg BTApp_Handle AEEBT_A2DP_EVT_START***", 0, 0, 0);

							MSG_FATAL("***zzg AEEBT_A2DP_EVT_START mA2DP bEnabled=%x, bConnected=%x***", 
										pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);
							MSG_FATAL("***zzg AEEBT_A2DP_EVT_START mAG bEnabled=%x, bConnected=%x, bAudioConnect=%x***", 
										pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.bAudioConnected);


						
							if ((((NotificationData*)pN->pData)->A2DPStartInit == AEEBT_A2DP_EVENT_LOCAL_INIT) 
								||(pMe->mAG.callState == BTAPP_AG_CALL_NONE))
							{
								MSG_FATAL("***zzg AEEBT_A2DP_EVT_START BTApp_DisconnectAudio***", 0, 0, 0);
							  BTApp_DisconnectAudio( pMe, pMe->mAG.bForceUnmute );
							}								
							
							// else, ignore remote-initiated Start command because  a call is present and call audio must be left alone 
							break;
						}

						case AEEBT_A2DP_EVT_SUSPEND:
						{
							MSG_FATAL( "AG: audioConnecting=%d callState=%d", pMe->mAG.bAudioConnecting, pMe->mAG.callState, 0);

							MSG_FATAL("***zzg BTApp_Handle AEEBT_A2DP_EVT_SUSPEND***", 0, 0, 0);

							MSG_FATAL("***zzg AEEBT_A2DP_EVT_SUSPEND mA2DP bEnabled=%x, bConnected=%x***", 
											pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);
							MSG_FATAL("***zzg AEEBT_A2DP_EVT_SUSPEND mAG bEnabled=%x, bConnected=%x, bAudioConnect=%x***", 
											pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.bAudioConnected);

							MSG_FATAL("***zzg AEEBT_A2DP_EVT_SUSPEND mAG bAudioConnecting=%x, callState=%x***", 
											pMe->mAG.bAudioConnecting, pMe->mAG.callState, 0);


							// did we suspend A2DP so we can bring up SCO? 
							if ( pMe->mAG.bAudioConnecting == TRUE )
							{
								MSG_FATAL("***zzg AEEBT_A2DP_EVT_SUSPEND BTApp_ConnectAudio 1***", 0, 0, 0);
							  // carry out pending SCO connection 
							  pMe->mAG.bAudioConnecting = FALSE;
							  BTApp_ConnectAudio( pMe, pMe->mAG.bForceUnmute );
							}
							else if ((pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_INBAND)
									|| (pMe->mAG.callState == BTAPP_AG_CALL_CONNECTED))
							{
								MSG_FATAL("***zzg AEEBT_A2DP_EVT_SUSPEND BTApp_ConnectAudio 2***", 0, 0, 0);
							  BTApp_ConnectAudio( pMe, TRUE );
							}							
							
							break;
						}
				    }
				    break;
				  }
				  case NMASK_BT_AVRCP:
				  {
				  	BTApp_ProcessAVRCPNotifications( pMe, evt, pN->pData );
				    break;
				  }
#endif
				  default:
				  {
				  	return FALSE;
				  }
	        }
	      }
#ifdef FEATURE_IOBEX
	      else if ( pN->cls == AEECLSID_OBEX_NOTIFIER )
	      {
	        if( GET_NOTIFIER_MASK(pN->dwMask) == NMASK_OBEX_GOEP )
	        {
	          return BTApp_ProcessOBEXNotifications( pMe, eCode, wParam, dwParam );
	        }
	      }
#endif
	      else if ( ((AEENotify*) dwParam)->cls == AEECLSID_PHONENOTIFIER 
#ifdef FEATURE_BT_VT
	              || ( ((AEENotify*) dwParam)->cls == AEECLSID_VIDEOPHONE_NOTIFIER )
#endif
	              )
	      {
#ifdef FEATURE_BT_VT
	        BTApp_ProcessVTEvents( pMe, wParam, dwParam );
#endif
	        return BTApp_ProcessTELEPHONEEvents( pMe, wParam, dwParam );
	      }

#ifdef FEATURE_BT_HFP_1_5
	      else if ( pN->cls == AEECLSID_BATTERYNOTIFIER )
	      {
	        return BTApp_ProcessBattEvents( pMe, wParam, dwParam );
	      }
#endif      

#ifdef FEATURE_BT_EXTPF_SAP
		#error code not present
#endif

//#ifndef FEATURE_UIONE_HDK
	
		  //Modify by zzg 2010_12_13
		  //Mif 文件里注册过AEESHELL, 所以开机就能收到
		  else if (pN->cls == AEECLSID_SHELL)
	      {
	      	MSG_FATAL("***zzg BTApp_Handle NOTIFY cls == AEECLSID_SHELL***", 0, 0, 0);
			
	        switch ( GET_NOTIFIER_MASK(pN->dwMask) )
	        {     
	          case NMASK_SHELL_INIT:
			  {				
	            // starting for the first time on power up
	            MSG_FATAL("***zzg BTApp EVT_NOTIFY NMASK_SHELL_INIT***", 0, 0, 0);
				
	            //event_processed = BTApp_StartBT(pMe); 	//Del By zzg 2011_10_20					
					
	            break;
	          }
	          default:
	            return FALSE;
	        }
	      }
//#endif

		  //return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
	      break;
		}

	case EVT_USER:
	{		
		MSG_FATAL( "HndlEv - EVT_USER wP=%d tm=%d", wParam, TOP_MENU, 0 );

		MSG_FATAL("***zzg BTApp_HandleEvent EVT_USER wParam=%d, dwparam=%d***", wParam, dwParam, 0);

		switch (wParam)
		{
#ifdef FEATURE_LCD_TOUCH_ENABLE
			case AVK_CLR:
			{
                if((pMe->m_eCurState == BTAPPST_SRHRESULT)||(pMe->m_eCurState == BTAPPST_DEVICESRH)
                    ||(pMe->m_eCurState == BTAPPST_BOND_OPITION)
                    ||(pMe->m_eCurState == BTAPPST_SET_SECURITY)||(pMe->m_eCurState == BTAPPST_SET_DISCOVERABLE)
                    ||(pMe->m_eCurState == BTAPPST_SET_IOCAPABILITY)||(pMe->m_eCurState == BTAPPST_SET_DEBUGKEY)
                    ||(pMe->m_eCurState == BTAPPST_FTP_SETTING)||(pMe->m_eCurState == BTAPPST_FTP_SERVER_REGISTER)
                    ||(pMe->m_eCurState == BTAPPST_FTP_BROWSE)||(pMe->m_eCurState == BTAPPST_FTP_BROWSE_OPITION)
                    ||(pMe->m_pActiveDlgID == IDD_BT_PROMPT)||(pMe->m_pActiveDlgID == IDD_BT_MSGBOX)
                    ||(pMe->m_eCurState == BTAPPST_BT_OBEX_LIST_SERVERS)||(pMe->m_eCurState == BTAPPST_DEVICE_LIST))
                {
                    eCode = EVT_KEY_PRESS;
                }
                else
                {
                    eCode = EVT_KEY;
                }
				
				return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
			break;
			
			case AVK_SELECT:
			case AVK_INFO:
			{
				if (dwParam != 0)
				{
                    if((pMe->m_pActiveDlgID == IDD_BT_FTP_SERVER_REGISTER)||(pMe->m_pActiveDlgID == IDD_BT_FTP_SETTINGS)
                        ||(pMe->m_pActiveDlgID == IDD_BT_OBEX_LIST_SERVERS)||(pMe->m_pActiveDlgID == IDD_BT_DISCOVERABLE)
                        ||(pMe->m_pActiveDlgID == IDD_BT_BOND_OPITION))
                    {
                        eCode = EVT_KEY_PRESS;
                    }
                    else
                    {
    					eCode = EVT_COMMAND;
    					wParam = dwParam;
    					dwParam = 0;
                    }

				}
				else	
				{
                    if((pMe->m_eCurState == BTAPPST_SRHRESULT)||(pMe->m_eCurState == BTAPPST_DEVICESRH)
                        ||(pMe->m_eCurState == BTAPPST_BOND_OPITION)
                        ||(pMe->m_eCurState == BTAPPST_SET_SECURITY)||(pMe->m_eCurState == BTAPPST_SET_DISCOVERABLE)
                        ||(pMe->m_eCurState == BTAPPST_SET_IOCAPABILITY)||(pMe->m_eCurState == BTAPPST_SET_DEBUGKEY)
                        ||(pMe->m_eCurState == BTAPPST_FTP_SETTING)||(pMe->m_eCurState == BTAPPST_FTP_SERVER_REGISTER)
                        ||(pMe->m_eCurState == BTAPPST_FTP_BROWSE)||(pMe->m_eCurState == BTAPPST_FTP_BROWSE_OPITION)
                        ||(pMe->m_pActiveDlgID == IDD_BT_PROMPT)||(pMe->m_pActiveDlgID == IDD_BT_MSGBOX)
                        ||(pMe->m_eCurState == BTAPPST_BT_OBEX_LIST_SERVERS)||(pMe->m_eCurState == BTAPPST_DEVICE_LIST))
                    {
                        eCode = EVT_KEY_PRESS;
                    }
                    else
                    {
                        eCode = EVT_KEY;
                    }
				}
				return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
			}
			break;
#endif
			//Add By zzg 2011_10_25		
			case EVT_CALLAPP_DISCONNECT_FROM_BT:
			case EVT_MUSICPLAYER_DISCONNECT_FROM_BT:	//Disconnect BtHeadSet for MusicPlayer
			{
				MSG_FATAL("***zzg BTApp Disconnect BtHeadSet AG bEnabled=%x, Connected=%x***", pMe->mAG.bEnabled, pMe->mAG.bConnected, 0);	
				MSG_FATAL("***zzg BTApp Disconnect BtHeadSet A2DP bEnabled=%x, Connected=%x***", pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);	
				
				MSG_FATAL("***zzg IDS_PROMPT_DISCONNECT***", 0, 0, 0);

				/*
				if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
				{
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
				}	
				*/

				if ((pMe->mA2DP.bEnabled == TRUE) && (pMe->mA2DP.bConnected == TRUE))
				{
					if (IBTEXTA2DP_Disconnect(pMe->mA2DP.po) != SUCCESS)
					{
						MSG_FATAL("***zzg IBTEXTA2DP_Disconnect Failed***", 0, 0, 0);
						BTApp_ShowMessage(pMe, IDS_A2DP_DISCONNECT_FAILED, NULL, 3);
					}
					else
					{
						MSG_FATAL("***zzg IBTEXTA2DP_Disconnect Succeed***", 0, 0, 0);
						BTApp_A2DPSetRetries(pMe, FALSE);
					}
				}
				
//#ifdef BTAPP_HEADSET_USE_AG
				if ((TRUE == pMe->mAG.bEnabled) && (TRUE == pMe->mAG.bConnected))
				{
					IBTEXTAG_Disconnect(pMe->mAG.po);
				}
//#endif		

				/*
				//Add by zzg 2011_11_08
				if (BDADDR_VALID(&pMe->mAG.bdAddr) || BDADDR_VALID(&pMe->mA2DP.bdAddr))
				{
					if (BDADDR_VALID(&pMe->mAG.bdAddr))
					{
						pMe->mAG.bdAddr = NULL_BD_ADDR;
					}

					if (BDADDR_VALID(&pMe->mA2DP.bdAddr))
					{
						pMe->mA2DP.bdAddr = NULL_BD_ADDR;
					}
					
					//BTApp_WriteBtAddrConfigFile(pMe);
					BTApp_WriteConfigFile(pMe);
				}
				//Add End		
				*/
							
				break;
			}			

			case EVT_CALLAPP_CHECK_BT_STATUS: //Check BTAG connect for callapp
			{
				MSG_FATAL("***zzg BTApp EVT_USER AGConnected=%x A2DPConnect=%x***", pMe->mAG.bConnected, pMe->mA2DP.bConnected, 0);
				
				//Add By zzg 2012_12_05				
				if (dwParam == EVT_CALLAPP_USE_BT_HEADSET)
				{
					if ((FALSE == pMe->mAG.bConnected) && (FALSE == pMe->mA2DP.bConnected))
					{
						ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, "BtHeadSet"); 	
						break;
					}
				}				
				//Add End

				if (TRUE == pMe->mAG.bConnected)
				{
					ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,EVT_BT_AG_AUDIO_CONNECTED,0);
				}
				else
				{
					ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,EVT_BT_AG_AUDIO_DISCONNECTED,0);
				}
				
				if (TRUE == pMe->mA2DP.bConnected)
				{
					ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,EVT_BT_A2DP_AUDIO_CONNECTED,0);
				}
				else
				{
					ISHELL_PostEvent(pMe->m_pShell,AEECLSID_DIALER,EVT_USER,EVT_BT_A2DP_AUDIO_DISCONNECTED,0);
				}
				
				break;
			}
			
			case EVT_MUSICPLAYER_ALARM_SUSPEND_BT:		//MusicPlayer Suspend
			{
				pMe->bMusicPlayerSuspend = TRUE;
				break;
			}
			case EVT_MUSICPLAYER_ALARM_RESUME_BT:		//MusicPlayer Resume
			{	
				pMe->bMusicPlayerSuspend = FALSE;
				break;
			}
			
			//Add End
			case EVT_AG_DISABLED:
			{
				BTApp_ProcEvtAGDisabled(pMe);
			  	break;
			}
			case EVT_A2DP_DISCONNECTED:
			{
				BTApp_A2DPDisable(pMe);
			  	BTApp_BuildMenu(pMe, TOP_MENU);
			  	break;
			}
			case EVT_AVRCP_DISCONNECTED:
			{
				BTApp_AVRCPDisable(pMe);
				BTApp_BuildMenu(pMe, TOP_MENU);
				break;
			}
			case EVT_NA_DISABLED:
			{
				pMe->mNA.bEnabled   = FALSE;
				pMe->mNA.bConnected = FALSE;
				BTApp_ClearBondable(pMe);
				BTApp_NADeInit((IApplet*)pMe);
				if (!pMe->mAG.bSelected || !pMe->mAG.bEnabled)
				{
					BTApp_BuildMenu(pMe, TOP_MENU);
				}
				break;
			}
			case EVT_CLR_MSG_SCR:
			{
				BTApp_HandleClearKey( pMe );
				break;
			}
			case EVT_PIN_RQST:
			{
				/*
				if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
				{
					MSG_FATAL("***zzg BTApp_HandleEvent EVT_USER ISHELL_ActiveApplet=%x***", ISHELL_ActiveApplet(pMe->m_pShell), dwParam, 0);
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);

					pMe->m_app_flag = TRUE;
					pMe->m_user_wParam = wParam;
					return TRUE;			
				}
				*/

				MSG_FATAL("***zzg BTApp_Handle EVT_USER: EVT_PIN_RQST***", 0, 0, 0);

#ifdef FEATURE_BT_2_1
			//pMe->mRM.bBonding = FALSE;
#endif // FEATURE_BT_2_1 

				pMe->m_edit_state_id = BTAPPST_MAIN;

#ifdef FEATURE_BT_2_1
				if (IBTEXTRM_GetLocalInfo( pMe->mRM.po,&pMe->mRM.myInfo ) != SUCCESS)
				{
					MSG_FATAL("***zzg IBTEXTRM_GetLocalInfo Failed!***", 0, 0, 0);				
				}
				else
				{
					MSG_FATAL("***zzg Local HC LMPVersion = %d***", pMe->mRM.myInfo.uLMPVersion, 0, 0);

					if (WSTRLEN( pMe->mRM.device[pMe->mRM.uCurDevIdx].wName) == 0)
					{
						BTApp_BDAddr2Wstr(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName,&pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr);
					}

					WSTRLCPY(pMe->wEditBuf, pMe->mRM.device[pMe->mRM.uCurDevIdx].wName, ARR_SIZE(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName));

					BTApp_SetBondable(pMe);//ACG

					pMe->mRM.bpassKeyRqst = FALSE;

					pMe->m_edit_id = IDS_PASS_KEY;
					pMe->m_bEditNeedStr = TRUE;		

					pMe->m_edit_state_id = BTAPPST_DEVICEINFO;								

					pMe->m_eDlgRet = DLGRET_BT_EDIT; 
					(void) ISHELL_EndDialog(pMe->m_pShell);						

				}
#endif			
				//BTApp_BuildMenu( pMe, BT_APP_MENU_PASSKEY );		  

				break;
			}
			
#ifdef FEATURE_BT_2_1
			case EVT_PASSKY:
			{
				if (pMe->bBusyIconUp == TRUE)
				{
					(void)POP_MENU();
				}

				BTApp_BuildMenu( pMe, BT_APP_MENU_DISPLAY_PASSKEY );
				break;
			}
#endif // FEATURE_BT_2_1 

			case EVT_DEV_BROWSE_RESP:
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION             

#ifdef FEATURE_IOBEX
				if (TOP_MENU == BT_APP_MENU_OBEX_CLIENT)
				{
					BTApp_OBEXHandleUserEvents( pMe, (uint32)wParam );
				}
				else
#endif //FEATURE_IOBEX
				{			  	
					BTApp_BuildMenu( pMe, BT_APP_MENU_BROWSE_SVC_RESP );
				}
				break;
			}
			case EVT_DEV_SEARCH_RESP:
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				if (pMe->mL2.bCliConnecting == TRUE && pMe->mL2.bDoSDP == TRUE)
				{
					BTApp_ProcessL2Notifications( pMe, EVT_DEV_SEARCH_RESP, NULL );
				}
				else
				{					
					BTApp_BuildMenu( pMe, BT_APP_MENU_SEARCH_SVC_RESP );
				}
				break;
			}
			case EVT_SPP_OPENED:
			{
				pMe->mSPP.bDoVerify       = TRUE;		  
				BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
				break;
			}
			case EVT_SPP_OPEN_FAILED:
			{
				BTApp_ShowMessage(pMe, IDS_MSG_SPP_OPEN_FAILED, NULL, 0);
				break;
			}
			case EVT_SPP_CONNECTED:
			{
				pMe->mSPP.uTotalBytesRcvd = 0;
				pMe->mSPP.uTotalBytesSent = 0;
				BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
				break;
			}
			case EVT_SPP_CLOSED:
			{
				pMe->mSPP.bDoVerify       = TRUE;
				BTApp_ClearBondable( pMe );
				
				if (pMe->mSPP.bClient == FALSE)
				{
					BTApp_ClearDiscoverable( pMe );
				}
				else
				{
					BTApp_BuildTopMenu( pMe );
				}
				break;
			}
			case EVT_SPP_DISCONNECTED:
			{
				BTApp_EndSPPTest( pMe );
				break;
			}
			case EVT_RM_BONDED:
			{
				if (pMe->mRM.bBonding)
				{
					pMe->mRM.bBonding = FALSE;
				}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

//#ifndef FEATURE_BT_2_1
				if (pMe->mRM.device[ pMe->mRM.uCurDevIdx ].majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO )
				{
					if (AEEBT_BD_ADDRS_EQUAL(&pMe->mAG.bdAddr,&pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr) == FALSE)
					{
						BTApp_BuildMenu( pMe, BT_APP_MENU_USE_AUDIO_DEV );
					}
					else
					{
						BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, &pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr,2);
					}

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				}
				else
				{
					BTApp_ShowDevMsg(pMe, IDS_MSG_BONDED, &pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr, 2);
				}
//#else
			    //BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, NULL,2 );
//#endif // FEATURE_BT_2_1 
				break;
			}
			case EVT_RM_DEV_SEC_SET:
			{
				if (TOP_MENU == BT_APP_MENU_DEV_SECURITY)
				{
					IBTEXTRM_DeviceRead(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx]);
					BTApp_BuildTopMenu(pMe);
				}
				break;
			}
			case EVT_RM_DEV_SEC_FAILED:
			{
				if (TOP_MENU == BT_APP_MENU_DEV_SECURITY)
				{
					BTApp_ShowDevMsg(pMe, IDS_MSG_DEV_SEC_FAILED, &pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr,2);
					BTApp_BuildTopMenu(pMe);
				}
				break;
			}
			case EVT_RM_AUTH_REQ:
			{
				BTApp_BuildMenu(pMe, BT_APP_MENU_AUTHORIZE_CONN);
				break;
			}
			case EVT_SD_DISC_SET:
			{
				MSG_FATAL("***zzg BTApp_HandleEvt EVT_SD_DISC_SET dwParam=%x***", dwParam, 0, 0);
				
				if (dwParam == AEEBT_SD_ERR_NONE)
				{
					IBTEXTSD_GetDiscoverableMode(pMe->mSD.po);
				}
				else
				{
					msgID = pMe->mSD.bDiscoverable ? IDS_MSG_DISCOVERABLE_NOT_CLEARED : IDS_MSG_DISCOVERABLE_NOT_SET;
					BTApp_ShowMessage(pMe, msgID, NULL, 2);
				}
				break;
			}
			case EVT_SD_DISC_RSP:
			{
				MSG_FATAL("***zzg BTApp_HandleEvent EVT_SD_DISC_RSP dwParam=%d, bStartBTaplication=%d***", dwParam, pMe->bStartBTaplication, 0);	

				pMe->mSD.bDiscoverable = (dwParam == AEEBT_DISCOVERABLE_MODE_NONE) ? FALSE : TRUE;

				(void) ISHELL_PostEvent(pMe->m_pShell,
							            AEECLSID_BLUETOOTH_APP,
							            EVT_USER_REDRAW,
							            0,
							            0); 

				if (pMe->bStartBTaplication)
				{
					BTApp_BuildMenu(pMe, TOP_MENU);		 
				}
				break;
			}
			case EVT_BIP_PUT_IMAGE:
			{
#ifdef FEATURE_BT_EXTPF_BIP
				if (!BDADDR_VALID(&pMe->mBIP.printerBDAddr))
				{
					MSG_FATAL( "BIP Printing - No address selected for print", 0, 0, 0 );
					BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, NULL, 3 );
					return FALSE;
				}
				// initialize BIP if not
				if (BTApp_BIPInit(pMe) != FALSE)
				{
					return BTApp_BIPRemotePrintImage( pMe, dwParam );
				}
				else
				{
					MSG_FATAL("BIP Printing - failed to create BIP object", 0, 0, 0);
					BTApp_BIPCleanup(pMe);
					return FALSE;
				}
#else
				MSG_FATAL( "HandleEvent: No support of BIP for printing", 0, 0, 0 );
				return FALSE;
#endif //FEATURE_BT_EXTPF_BIP
			}
			case EVT_BPP_SIMPLE_PUSH:
			{
#ifdef FEATURE_BT_EXTPF_BPP
				if (!BDADDR_VALID(&pMe->mBPP.printerBDAddr))
				{
					MSG_FATAL("BPP Printing - No address selected for print", 0, 0, 0);
					BTApp_ShowMessage(pMe, IDS_BPP_MSG_SEND_FILE_FAILED, NULL, 3);
					return FALSE;
				}
				// initialize BPP if not
				if (BTApp_BPPInit(pMe) != FALSE)
				{
					return BTApp_BPPRemotePrintReq( pMe, dwParam );
				}
				else
				{
					MSG_FATAL("BPP Printing - failed to create BPP object", 0, 0, 0);
					BTApp_BPPCleanup(pMe);
					return FALSE;
				}
#else
				MSG_FATAL( "HandleEvent: No support of BPP for printing", 0, 0, 0 );
				return FALSE;
#endif //FEATURE_BT_EXTPF_BPP
			}
			
#ifdef FEATURE_BT_EXTPF_OPP
			case EVT_OPP:
			{
				BTApp_OPPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_BT_EXTPF_FTP
			case EVT_FTP:
			{
				BTApp_FTPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_BT_EXTPF_BIP
			case EVT_BIP:
			{
				BTApp_BIPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_BT_EXTPF_BPP
			case EVT_BPP:
			{
				BTApp_BPPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
			case EVT_PBAP:
			{
				BTApp_PBAPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_IOBEX
			case EVT_OBEX:
			{
				BTApp_OBEXHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#ifdef FEATURE_BT_EXTPF_CTP
			case EVT_CTP:
			{
				BTApp_CTPHandleUserEvents( pMe, dwParam );
				break;
			}
			case EVT_ICP:
			{
				BTApp_ICPHandleUserEvents( pMe, dwParam );
				break;
			}
#endif

#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION    
		}

		MSG_FATAL("***zzg ISHELL_ActiveApplet=%x***", ISHELL_ActiveApplet(pMe->m_pShell), 0, 0);

	
		//Add By zzg 2011_02_16		
		if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_BLUETOOTH_APP)
		{
			(void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_BLUETOOTH_APP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);			
		}		
		//Add End
		

		//return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
		break;
	}

    default:
    {   
		// 将接收到的事件路由至当前活动的对话框事件处理函数。
        return BTApp_RouteDialogEvent(pMe,eCode,wParam,dwParam);
    }
   }

	return event_processed;
}

//Add End

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this 
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell object. 

  pIModule: pin]: Contains pointer to the IModule object to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/

/*
#if defined(BREW_STATIC_APP)
int BTApp_CreateInstance( AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)
#else
int AEEClsCreateInstance( AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)
#endif
{
  boolean created = FALSE;
  int     result  = EFAILED;

  *ppObj = NULL;
 
#ifdef BT_SWDEV_BT_DISABLED_NV
  {
    nv_item_type  nvi;
    if ( (OEMNV_Get( NV_BT_DISABLED_I, &nvi ) == NV_DONE_S) )
    {
      if ( nvi.bt_disabled ==  BTAPP_DISABLED ) 
      {
        return ECLASSNOTSUPPORT;
      }
    }
  }
#endif // BT_SWDEV_BT_DISABLED_NV 

  if ( ClsId == AEECLSID_BLUETOOTH_APP )
  {	
  	
    created = AEEApplet_New( sizeof(CBTApp), 
                             ClsId, 
                             pIShell,
                             po,
                             (IApplet**) ppObj,
                             (AEEHANDLER) BTApp_HandleEvent,
                             (PFNFREEAPPDATA) BTApp_FreeAppData);
                          
    if(created)
    {
      if( BTApp_InitAppData( (IApplet*)*ppObj ) )
      {            	
        pTheBTApp = (CBTApp*)*ppObj;

        //Data initialized successfully
        result = AEE_SUCCESS;
      }
      else
      {		
        //Release the applet. This will free the memory allocated for the applet
        IAPPLET_Release( ((IApplet*)*ppObj) );
      }
    }
  }
  return (result);
}
*/

/*============================ Static Application ==========================*/

#if defined(BREW_STATIC_APP)

static const AEEAppInfo gaiBt = 
{
  AEECLSID_BLUETOOTH_APP,
  AEE_APPSBTAPP_RES_FILE, 
  IDS_BT_TITLE,
  0,
  0,
  0,
  0,
  AFLAG_TOOL|AFLAG_HIDDEN
};

/*
int BTApp_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
  int result;  
  result = AEEStaticMod_New((int16)(sizeof(AEEMod)),
                            ps,
                            pHelpers,
                            pMod,
                            BTApp_CreateInstance,
                            NULL);
  
  return result;
}

PFNMODENTRY BTApp_GetModInfo(IShell *ps, 
                             AEECLSID **ppClasses, 
                             AEEAppInfo **pApps, 
                             uint16 *pnApps,
                             uint16 *pwMinPriv)
{
   *pApps = (AEEAppInfo *)&gaiBt;
   *pnApps = 1;
   *pwMinPriv = PL_SYSTEM;  // because I'm a background app
   return((PFNMODENTRY)BTApp_Load);
}
*/

#endif  /* BREW_STATIC_APP */

/* ==========================================================================
FUNCTION BTApp_WriteBtAddrConfigFile
DESCRIPTION
============================================================================= */

//Add By zzg 2011_11_08
boolean BTApp_WriteBtAddrConfigFile(CBTApp* pMe)
{
	boolean   success   = FALSE;
	IFile*    pIFile    = NULL;
	IFileMgr* pIFileMgr = NULL;
	uint8     version   = BTAPP_CONFIG_VERSION;

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile***", 0, 0, 0);

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile before bEnableAtStart=%x!***", pMe->bEnableAtStart, 0, 0);


	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile mAG:%04x %04x %04x***", 
			((uint16)(pMe->mAG.bdAddr.uAddr[ 5 ] << 8) | pMe->mAG.bdAddr.uAddr[ 4 ]),
       		((uint16)(pMe->mAG.bdAddr.uAddr[ 3 ] << 8) | pMe->mAG.bdAddr.uAddr[ 2 ]),
       		((uint16)(pMe->mAG.bdAddr.uAddr[ 1 ] << 8) | pMe->mAG.bdAddr.uAddr[ 0 ]));	

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile mA2DP:%04x %04x %04x***", 
			((uint16)(pMe->mA2DP.bdAddr.uAddr[ 5 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 4 ]),
       		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 3 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 2 ]),
       		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 1 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 0 ]));
  	
	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 1***", 0, 0, 0);
	
	if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
	                      (void **)&pIFileMgr ) == SUCCESS )
	{
		MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 2***", 0, 0, 0);
		if( IFILEMGR_Test( pIFileMgr, BTAPP_CONFIG_FILE ) != SUCCESS )
		{
			MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 3***", 0, 0, 0);
			pIFile = IFILEMGR_OpenFile( pIFileMgr, BTAPP_CONFIG_FILE, _OFM_CREATE );
		}
		else
		{
			MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 4***", 0, 0, 0);
			pIFile = IFILEMGR_OpenFile( pIFileMgr, BTAPP_CONFIG_FILE, _OFM_READWRITE );
		}

		MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 5***", 0, 0, 0);
		
    	if ((pIFile != NULL) 
			&& BTAPP_WRITE_VALUE(version) 
			&& BTAPP_WRITE_VALUE(pMe->mAG.bdAddr)           
#ifdef FEATURE_BT_EXTPF_AV
         	&&	BTAPP_WRITE_VALUE(pMe->mA2DP.bdAddr)        
#endif
        	&& TRUE ) // fix BT_2_1 compilation issue because of trailing &&'s, if ( expr && TRUE) is same as if( expr )
	    {
	    	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 6***", 0, 0, 0);
			success = TRUE;
	    }
	    else
	    {
	    	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile write failed***", 0, 0, 0);			
	    }

		MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 7***", 0, 0, 0);
		
		if ( pIFile != NULL )
		{
			MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 8***", 0, 0, 0);
			IFILE_Release( pIFile );
		}

		MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 9***", 0, 0, 0);
		IFILEMGR_Release( pIFileMgr );
  	}

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile 10***", 0, 0, 0);

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile version=%x***", version, 0, 0);

	MSG_FATAL("***zzg BTApp_WriteBtAddrConfigFile after bEnableAtStart=%x!***", pMe->bEnableAtStart, 0, 0);

	return success;
}
//Add End






/* ==========================================================================
FUNCTION BTApp_WriteConfigFile
DESCRIPTION
============================================================================= */

boolean BTApp_WriteConfigFile( CBTApp* pMe )
{
  boolean   success   = FALSE;
  IFile*    pIFile    = NULL;
  IFileMgr* pIFileMgr = NULL;
  uint8     version   = BTAPP_CONFIG_VERSION;

  MSG_FATAL("***zzg BTApp_WriteConfigFile***", 0, 0, 0);

  MSG_FATAL("***zzg WriteConfigFile mA2DP:%04x %04x %04x***", 
					((uint16)(pMe->mA2DP.bdAddr.uAddr[ 5 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 4 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 3 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 2 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 1 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 0 ]));

  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                              (void **)&pIFileMgr ) == SUCCESS )
  {
    if( IFILEMGR_Test( pIFileMgr, BTAPP_CONFIG_FILE ) != SUCCESS )
    {
      pIFile = IFILEMGR_OpenFile( pIFileMgr, BTAPP_CONFIG_FILE, _OFM_CREATE );
    }
    else
    {
      pIFile = IFILEMGR_OpenFile( pIFileMgr, BTAPP_CONFIG_FILE, _OFM_READWRITE );
    }

    if ( (pIFile != NULL) &&
         BTAPP_WRITE_VALUE( version )              &&
         BTAPP_WRITE_VALUE( pMe->bEnableAtStart )  &&
         BTAPP_WRITE_VALUE( pMe->mAG.bSelected )   &&
         BTAPP_WRITE_VALUE( pMe->mAG.bdAddr )      &&
         BTAPP_WRITE_VALUE( pMe->mAG.devType )     &&
         BTAPP_WRITE_VALUE( pMe->mAG.bPrivateMode )&&
         BTAPP_WRITE_VALUE( pMe->mAG.idleMode )    &&
         BTAPP_WRITE_VALUE( pMe->mAG.idleTimeout ) &&
         BTAPP_WRITE_VALUE( pMe->mAG.spkrGain )    &&
         BTAPP_WRITE_VALUE( pMe->mAG.micGain )     &&
         BTAPP_WRITE_VALUE( pMe->mAG.bVREnabled )  &&
         BTAPP_WRITE_VALUE( pMe->mAG.uFlags )      &&
         BTAPP_WRITE_VALUE( pMe->mAG.wDialString ) &&
         BTAPP_WRITE_VALUE( pMe->mNA.bSelected )        &&
         BTAPP_WRITE_VALUE( pMe->mNA.bEnableSvcSec )    &&
         BTAPP_WRITE_VALUE( pMe->mSPP.uNumDataBufs )    &&
         BTAPP_WRITE_VALUE( pMe->mRM.pageScanParam )    &&
         BTAPP_WRITE_VALUE( pMe->mRM.inquiryScanParam ) &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mRM.srvSecType )       &&
         BTAPP_WRITE_VALUE( pMe->mAG.bSecSelectedAll )  &&
         BTAPP_WRITE_VALUE( pMe->mAG.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mAG.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mAG.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mAG.bAuthorizeFirst)   &&
         BTAPP_WRITE_VALUE( pMe->mNA.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mNA.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mNA.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mNA.bAuthorizeFirst)   &&

#endif /* FEATURE_BT_2_1 */
#ifdef FEATURE_BT_EXTPF_OPP
         BTAPP_WRITE_VALUE( pMe->mOPP.bServerEnable )   &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mOPP.bSecSelected )    &&
         BTAPP_WRITE_VALUE( pMe->mOPP.srvSecType)       &&
         BTAPP_WRITE_VALUE( pMe->mOPP.bAuthorize)       &&
         BTAPP_WRITE_VALUE( pMe->mOPP.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_FTP
         BTAPP_WRITE_VALUE( pMe->mFTP.bDoAuthenticate )    &&
         BTAPP_WRITE_VALUE( pMe->mFTP.reg.bReadOnly )      &&
         BTAPP_WRITE_VALUE( pMe->mFTP.reg.bBrowseAllowed ) &&
         BTAPP_WRITE_VALUE( pMe->mFTP.bServerEnable )      &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mFTP.bSecSelected )       &&
         BTAPP_WRITE_VALUE( pMe->mFTP.srvSecType)          &&
         BTAPP_WRITE_VALUE( pMe->mFTP.bAuthorize)          &&
         BTAPP_WRITE_VALUE( pMe->mFTP.bAuthorizeFirst)     &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BIP
         BTAPP_WRITE_VALUE( pMe->mBIP.serviceSelection ) &&
         BTAPP_WRITE_VALUE( pMe->mBIP.bServerEnable )    &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mBIP.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mBIP.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mBIP.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mBIP.bAuthorizeFirst)   &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BPP
         BTAPP_WRITE_VALUE( pMe->mBPP.bServerEnable ) &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mBPP.bSecSelected )   &&
         BTAPP_WRITE_VALUE( pMe->mBPP.srvSecType)      &&
         BTAPP_WRITE_VALUE( pMe->mBPP.bAuthorize)      &&
         BTAPP_WRITE_VALUE( pMe->mBPP.bAuthorizeFirst) &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
         BTAPP_WRITE_VALUE( pMe->mPBAP.bServerEnable ) &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.authOptions )   &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.vCardFormat )   &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.sortOrder )     &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.searchAttrib )  &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mPBAP.bSecSelected )   &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.srvSecType)      &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.bAuthorize)      &&
         BTAPP_WRITE_VALUE( pMe->mPBAP.bAuthorizeFirst) &&
#endif
#endif
#ifdef FEATURE_IOBEX
         BTAPP_WRITE_VALUE( pMe->mOBEX.bServerEnable )   &&
         BTAPP_WRITE_VALUE( pMe->mOBEX.bDoAuthenticate ) &&
         BTAPP_WRITE_VALUE( pMe->mOBEX.bDoAuthWithUid )  &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mOBEX.bSecSelected )    &&
         BTAPP_WRITE_VALUE( pMe->mOBEX.srvSecType)       &&
         BTAPP_WRITE_VALUE( pMe->mOBEX.bAuthorize)       &&
         BTAPP_WRITE_VALUE( pMe->mOBEX.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
         BTAPP_WRITE_VALUE( pMe->mA2DP.bdAddr )        &&
         BTAPP_WRITE_VALUE( pMe->mA2DP.bEnableA2DP )   &&
         BTAPP_WRITE_VALUE( pMe->mAVRCP.bEnableAVRCP ) &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mA2DP.bSecSelected )      &&
         BTAPP_WRITE_VALUE( pMe->mA2DP.srvSecType)         &&
         BTAPP_WRITE_VALUE( pMe->mA2DP.bAuthorize)         &&
         BTAPP_WRITE_VALUE( pMe->mA2DP.bAuthorizeFirst)    &&
         BTAPP_WRITE_VALUE( pMe->mAVRCP.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mAVRCP.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mAVRCP.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mAVRCP.bAuthorizeFirst)   &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
         BTAPP_WRITE_VALUE( pMe->mL2.bEnableL2Srv )     &&
         BTAPP_WRITE_VALUE( pMe->mL2.sSrvInfo )         &&
         BTAPP_WRITE_VALUE( pMe->mL2.uCliBytesToSend )  &&
         BTAPP_WRITE_VALUE( pMe->mL2.bEnableSrvSvcSec ) &&
         BTAPP_WRITE_VALUE( pMe->mL2.bEnableCliSvcSec ) &&
#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mL2.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mL2.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mL2.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mL2.bAuthorizeFirst)   &&
#endif
         BTAPP_WRITE_VALUE( pMe->mL2.bDoSDP )            &&

#ifdef FEATURE_BT_2_1
         BTAPP_WRITE_VALUE( pMe->mSPP.bSecSelected )     &&
         BTAPP_WRITE_VALUE( pMe->mSPP.srvSecType)        &&
         BTAPP_WRITE_VALUE( pMe->mSPP.bAuthorize)        &&
         BTAPP_WRITE_VALUE( pMe->mSPP.bAuthorizeFirst)   &&
#endif
         TRUE ) // fix BT_2_1 compilation issue because of trailing &&'s, if ( expr && TRUE) is same as if( expr )
    {
    	 MSG_FATAL("***zzg BTApp_WriteConfigFile Succeed!***", 0, 0, 0);

		 MSG_FATAL("***zzg BTApp_WriteConfigFile Succeed bEnableAtStart=%x!***", pMe->bEnableAtStart, 0, 0);
      success = TRUE;
    }
    else
    {
    	 MSG_FATAL("***zzg BTApp_WriteConfigFile Failed!***", 0, 0, 0);
      MSG_FATAL( "WriteConfigFile - write failed", 0, 0, 0 );
    }

    if ( pIFile != NULL )
    {
      IFILE_Release( pIFile );
    }

    IFILEMGR_Release( pIFileMgr );
  }

  MSG_FATAL("***zzg BTApp_WriteConfigFile version=%x***", version, 0, 0);

  return success;
}


/* ==========================================================================
FUNCTION BTApp_ReadConfigFile
DESCRIPTION
============================================================================= */
static boolean BTApp_ReadConfigFile( CBTApp* pMe )
{
  boolean   success   = FALSE;
  IFile*    pIFile    = NULL;
  IFileMgr* pIFileMgr = NULL;
  uint8     version;

  MSG_FATAL("***zzg BTApp_ReadConfigFile start***", 0, 0, 0);
  
  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                              (void **)&pIFileMgr ) == SUCCESS )
  {
  	MSG_FATAL("***zzg BTApp_ReadConfigFile 1***", 0, 0, 0);
    if( IFILEMGR_Test( pIFileMgr, BTAPP_CONFIG_FILE ) == SUCCESS )
    {
    	MSG_FATAL("***zzg BTApp_ReadConfigFile 2***", 0, 0, 0);
      pIFile = IFILEMGR_OpenFile( pIFileMgr, BTAPP_CONFIG_FILE, _OFM_READ );
    }
    if ( pIFile != NULL )
    {
    	MSG_FATAL("***zzg BTApp_ReadConfigFile 3***", 0, 0, 0);
		
      if ( !BTAPP_READ_VALUE( version ) )
      {
      	MSG_FATAL("***zzg BTApp_ReadConfigFile 4***", 0, 0, 0);
        MSG_FATAL( "Can't read version from DB", 0, 0, 0);
        version = 0;
      }

	  MSG_FATAL("***zzg BTApp_ReadConfigFile start version=%x***", version, 0, 0);
	  
      if ( (version == BTAPP_CONFIG_VERSION)         &&
           BTAPP_READ_VALUE( pMe->bEnableAtStart )   &&
           BTAPP_READ_VALUE( pMe->mAG.bSelected )    &&
           BTAPP_READ_VALUE( pMe->mAG.bdAddr )       &&
           BTAPP_READ_VALUE( pMe->mAG.devType )      &&
           BTAPP_READ_VALUE( pMe->mAG.bPrivateMode ) &&
           BTAPP_READ_VALUE( pMe->mAG.idleMode )     &&
           BTAPP_READ_VALUE( pMe->mAG.idleTimeout )  &&
           BTAPP_READ_VALUE( pMe->mAG.spkrGain )     &&
           BTAPP_READ_VALUE( pMe->mAG.micGain )      &&
           BTAPP_READ_VALUE( pMe->mAG.bVREnabled )   &&
           BTAPP_READ_VALUE( pMe->mAG.uFlags )       &&
           BTAPP_READ_VALUE( pMe->mAG.wDialString )  &&
           BTAPP_READ_VALUE( pMe->mNA.bSelected )        &&
           BTAPP_READ_VALUE( pMe->mNA.bEnableSvcSec )    &&
           BTAPP_READ_VALUE( pMe->mSPP.uNumDataBufs )    &&
           BTAPP_READ_VALUE( pMe->mRM.pageScanParam )    &&
           BTAPP_READ_VALUE( pMe->mRM.inquiryScanParam ) &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mRM.srvSecType )       &&
           BTAPP_READ_VALUE( pMe->mAG.bSecSelectedAll )  && 
           BTAPP_READ_VALUE( pMe->mAG.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mAG.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mAG.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mAG.bAuthorizeFirst)   &&
           BTAPP_READ_VALUE( pMe->mNA.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mNA.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mNA.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mNA.bAuthorizeFirst)   &&
#endif /* FEATURE_BT_2_1 */
#ifdef FEATURE_BT_EXTPF_OPP
           BTAPP_READ_VALUE( pMe->mOPP.bServerEnable ) &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mOPP.bSecSelected )    &&
           BTAPP_READ_VALUE( pMe->mOPP.srvSecType)       &&
           BTAPP_READ_VALUE( pMe->mOPP.bAuthorize)       &&
           BTAPP_READ_VALUE( pMe->mOPP.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_FTP
           BTAPP_READ_VALUE( pMe->mFTP.bDoAuthenticate )    &&
           BTAPP_READ_VALUE( pMe->mFTP.reg.bReadOnly )      &&
           BTAPP_READ_VALUE( pMe->mFTP.reg.bBrowseAllowed ) &&
           BTAPP_READ_VALUE( pMe->mFTP.bServerEnable )      &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mFTP.bSecSelected )       && 
           BTAPP_READ_VALUE( pMe->mFTP.srvSecType)          &&
           BTAPP_READ_VALUE( pMe->mFTP.bAuthorize)          &&
           BTAPP_READ_VALUE( pMe->mFTP.bAuthorizeFirst)     &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BIP
           BTAPP_READ_VALUE( pMe->mBIP.serviceSelection ) &&
           BTAPP_READ_VALUE( pMe->mBIP.bServerEnable )    &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mBIP.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mBIP.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mBIP.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mBIP.bAuthorizeFirst)   &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BPP
           BTAPP_READ_VALUE( pMe->mBPP.bServerEnable ) &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mBPP.bSecSelected )     &&
           BTAPP_READ_VALUE( pMe->mBPP.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mBPP.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mBPP.bAuthorizeFirst)   &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
           BTAPP_READ_VALUE( pMe->mPBAP.bServerEnable ) &&
           BTAPP_READ_VALUE( pMe->mPBAP.authOptions )   &&
           BTAPP_READ_VALUE( pMe->mPBAP.vCardFormat )   &&
           BTAPP_READ_VALUE( pMe->mPBAP.sortOrder )     &&
           BTAPP_READ_VALUE( pMe->mPBAP.searchAttrib )  &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mPBAP.bSecSelected )    && 
           BTAPP_READ_VALUE( pMe->mPBAP.srvSecType)       &&
           BTAPP_READ_VALUE( pMe->mPBAP.bAuthorize)       &&
           BTAPP_READ_VALUE( pMe->mPBAP.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_IOBEX
           BTAPP_READ_VALUE( pMe->mOBEX.bServerEnable )   &&
           BTAPP_READ_VALUE( pMe->mOBEX.bDoAuthenticate ) &&
           BTAPP_READ_VALUE( pMe->mOBEX.bDoAuthWithUid )  &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mOBEX.bSecSelected )    && 
           BTAPP_READ_VALUE( pMe->mOBEX.srvSecType)       &&
           BTAPP_READ_VALUE( pMe->mOBEX.bAuthorize)       &&
           BTAPP_READ_VALUE( pMe->mOBEX.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
           BTAPP_READ_VALUE( pMe->mA2DP.bdAddr )        &&
           BTAPP_READ_VALUE( pMe->mA2DP.bEnableA2DP )   &&
           BTAPP_READ_VALUE( pMe->mAVRCP.bEnableAVRCP ) &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mA2DP.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mA2DP.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mA2DP.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mA2DP.bAuthorizeFirst)   &&
           BTAPP_READ_VALUE( pMe->mAVRCP.bSecSelected )    && 
           BTAPP_READ_VALUE( pMe->mAVRCP.srvSecType)       &&
           BTAPP_READ_VALUE( pMe->mAVRCP.bAuthorize)       &&
           BTAPP_READ_VALUE( pMe->mAVRCP.bAuthorizeFirst)  &&
#endif
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
           BTAPP_READ_VALUE( pMe->mL2.bEnableL2Srv )     &&
           BTAPP_READ_VALUE( pMe->mL2.sSrvInfo )         &&
           BTAPP_READ_VALUE( pMe->mL2.uCliBytesToSend )  &&
           BTAPP_READ_VALUE( pMe->mL2.bEnableSrvSvcSec ) &&
           BTAPP_READ_VALUE( pMe->mL2.bEnableCliSvcSec ) &&
#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mL2.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mL2.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mL2.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mL2.bAuthorizeFirst)   &&
#endif
           BTAPP_READ_VALUE( pMe->mL2.bDoSDP )        &&

#ifdef FEATURE_BT_2_1
           BTAPP_READ_VALUE( pMe->mSPP.bSecSelected )     && 
           BTAPP_READ_VALUE( pMe->mSPP.srvSecType)        &&
           BTAPP_READ_VALUE( pMe->mSPP.bAuthorize)        &&
           BTAPP_READ_VALUE( pMe->mSPP.bAuthorizeFirst)   &&
#endif
           TRUE )// fix BT_2_1 compilation issue because of trailing &&'s, if ( expr && TRUE) is same as if( expr ) 
      {
        success = TRUE;

		MSG_FATAL("***zzg BTApp_ReadConfigFile TRUE***", 0, 0, 0);

		MSG_FATAL("***zzg ReadConfigFile mA2DP:%04x %04x %04x***", 
					((uint16)(pMe->mA2DP.bdAddr.uAddr[ 5 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 4 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 3 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 2 ]),
               		((uint16)(pMe->mA2DP.bdAddr.uAddr[ 1 ] << 8) | pMe->mA2DP.bdAddr.uAddr[ 0 ]));

#ifdef FEATURE_BT_2_1
        if( pMe->mAG.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mAG.bAuthorize != FALSE ||
            pMe->mAG.bAuthorizeFirst != FALSE )
        {
          pMe->mAG.bSecSelected = TRUE;
        }
        if( pMe->mNA.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mNA.bAuthorize != FALSE ||
            pMe->mNA.bAuthorizeFirst != FALSE )
        {
          pMe->mNA.bSecSelected = TRUE;
        }
#ifdef FEATURE_BT_EXTPF_OPP
        if( pMe->mOPP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mOPP.bAuthorize != FALSE ||
            pMe->mOPP.bAuthorizeFirst != FALSE )
        {
          pMe->mOPP.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
        if( pMe->mFTP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mFTP.bAuthorize != FALSE ||
            pMe->mFTP.bAuthorizeFirst != FALSE )
        {
          pMe->mFTP.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
        if( pMe->mBIP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mBIP.bAuthorize != FALSE ||
            pMe->mBIP.bAuthorizeFirst != FALSE )
        {
          pMe->mBIP.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_BPP
        if( pMe->mBPP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mBPP.bAuthorize != FALSE ||
            pMe->mBPP.bAuthorizeFirst != FALSE )
        {
          pMe->mBPP.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
        if( pMe->mPBAP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mPBAP.bAuthorize != FALSE ||
            pMe->mPBAP.bAuthorizeFirst != FALSE )
        {
          pMe->mPBAP.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_IOBEX
        if( pMe->mOBEX.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mOBEX.bAuthorize != FALSE ||
            pMe->mOBEX.bAuthorizeFirst != FALSE )
        {
          pMe->mOBEX.bSecSelected = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
        if( pMe->mA2DP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mA2DP.bAuthorize != FALSE ||
            pMe->mA2DP.bAuthorizeFirst != FALSE )
        {
          pMe->mA2DP.bSecSelected = TRUE;
        }
        if( pMe->mAVRCP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mAVRCP.bAuthorize != FALSE ||
            pMe->mAVRCP.bAuthorizeFirst != FALSE )
        {
          pMe->mAVRCP.bSecSelected = TRUE;
        }
#endif
        if( pMe->mL2.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mL2.bAuthorize != FALSE ||
            pMe->mL2.bAuthorizeFirst != FALSE )
        {
          pMe->mL2.bSecSelected = TRUE;
        }

        if( pMe->mSPP.srvSecType != BTAPP_RM_SVC_SEC_DEFAULT ||
            pMe->mSPP.bAuthorize != FALSE ||
            pMe->mSPP.bAuthorizeFirst != FALSE )
        {
          pMe->mSPP.bSecSelected = TRUE;
        }
#endif
      }
      IFILE_Release( pIFile );
    }
    IFILEMGR_Release( pIFileMgr );
  }

  if ( success == FALSE )
  {
    MSG_FATAL( "ReadConfigFile - read failed", 0, 0, 0 );

	MSG_FATAL("***zzg BTApp_ReadConfigFile FALSE***", 0, 0, 0);
	
    pMe->mAG.bSelected    = TRUE;
    pMe->mAG.bdAddr       = NULL_BD_ADDR;
    pMe->mAG.devType      = AEEBT_AG_AUDIO_DEVICE_HEADSET;
    pMe->mAG.bPrivateMode = FALSE;
    pMe->mAG.idleMode     = AEEBT_AG_IDLEMODE_PASSIVE;
    pMe->mAG.idleTimeout  = 0;  // in seconds
    pMe->mAG.spkrGain     = 10;
    pMe->mAG.micGain      = 10;
    pMe->mAG.uFlags       = 0;
#ifdef FEATURE_BT_2_1
    pMe->mRM.srvSecType       = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mAG.bSecSelectedAll  = FALSE;
    pMe->mAG.bSecSelected     = FALSE;
    pMe->mAG.srvSecType       = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mAG.bAuthorize       = FALSE;
    pMe->mAG.bAuthorizeFirst  = FALSE;
    pMe->mNA.bSecSelected     = FALSE;
    pMe->mNA.srvSecType       = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mNA.bAuthorize       = FALSE;
    pMe->mNA.bAuthorizeFirst  = FALSE;
#endif /* FEATURE_BT_2_1 */

#ifdef FEATURE_PHONE_VR
    pMe->mAG.bVREnabled   = TRUE;
#else
    pMe->mAG.bVREnabled   = FALSE;
#endif
    STRTOWSTR( "1234", pMe->mAG.wDialString, sizeof( pMe->mAG.wDialString ) );
    pMe->mNA.bSelected            = FALSE;
    pMe->mNA.bEnableSvcSec        = FALSE;
    pMe->mSPP.uNumDataBufs        = 1000;
    pMe->mRM.pageScanParam        = AEEBT_RM_AVP_AUTOMATIC;
    pMe->mRM.inquiryScanParam     = AEEBT_RM_AVP_AUTOMATIC;

#ifdef FEATURE_BT_EXTPF_OPP
    pMe->mOPP.bServerEnable       = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mOPP.bSecSelected        = FALSE;
    pMe->mOPP.srvSecType          = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mOPP.bAuthorize          = FALSE;
    pMe->mOPP.bAuthorizeFirst     = FALSE;
#endif
#endif
#ifdef FEATURE_BT_EXTPF_FTP
    pMe->mFTP.bDoAuthenticate     = FALSE;
    pMe->mFTP.reg.bReadOnly       = FALSE; 
    pMe->mFTP.reg.bBrowseAllowed  = TRUE;
    pMe->mFTP.bServerEnable       = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mFTP.bSecSelected        = FALSE;
    pMe->mFTP.srvSecType          = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mFTP.bAuthorize          = FALSE;
    pMe->mFTP.bAuthorizeFirst     = FALSE;
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    pMe->mBIP.serviceSelection    = AEEBT_BIP_IMAGING_RESPONDER;
    pMe->mBIP.bServerEnable       = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mBIP.bSecSelected        = FALSE;
    pMe->mBIP.srvSecType          = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mBIP.bAuthorize          = FALSE;
    pMe->mBIP.bAuthorizeFirst     = FALSE;
#endif
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    pMe->mBPP.bServerEnable       = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mBPP.bSecSelected        = FALSE;
    pMe->mBPP.srvSecType          = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mBPP.bAuthorize          = FALSE;
    pMe->mBPP.bAuthorizeFirst     = FALSE;
#endif 
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    pMe->mPBAP.bServerEnable      = FALSE;
    pMe->mPBAP.authOptions        = AEEBT_PBAP_AUTH_NONE;
    pMe->mPBAP.vCardFormat        = AEEBT_PBAP_FORMAT_VCARD_2_1;
#ifdef FEATURE_BT_2_1
    pMe->mPBAP.bSecSelected       = FALSE;
    pMe->mPBAP.srvSecType         = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mPBAP.bAuthorize         = FALSE;
    pMe->mPBAP.bAuthorizeFirst    = FALSE;
#endif
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    pMe->mA2DP.bdAddr             = NULL_BD_ADDR;
    pMe->mA2DP.bEnableA2DP        = FALSE;
    pMe->mAVRCP.bEnableAVRCP      = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mA2DP.bSecSelected       = FALSE;
    pMe->mA2DP.srvSecType         = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mA2DP.bAuthorize         = FALSE;
    pMe->mA2DP.bAuthorizeFirst    = FALSE;
    pMe->mAVRCP.bSecSelected      = FALSE;
    pMe->mAVRCP.srvSecType        = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mAVRCP.bAuthorize        = FALSE;
    pMe->mAVRCP.bAuthorizeFirst   = FALSE;
#endif
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    pMe->mL2.bEnableL2Srv         = FALSE;
    pMe->mL2.sSrvInfo.psm         = 7777;
    pMe->mL2.sSrvInfo.cid         = 0;
    pMe->mL2.sSrvInfo.in_flush_to = AEEBT_L2_FLUSH_TO_DEFALUT;
    pMe->mL2.sSrvInfo.in_mtu      = BTAPP_L2_TEST_MTU;
    pMe->mL2.sSrvInfo.out_mtu     = BTAPP_L2_TEST_MTU;
    pMe->mL2.sSrvInfo.in_qos      = AEEBT_L2_QOS_DEFAULT_TYPE;
    pMe->mL2.sSrvInfo.token_rate  = 0;
    pMe->mL2.sSrvInfo.token_bucket_size = 0;
    pMe->mL2.sSrvInfo.peak_bandwidth    = 0;
    pMe->mL2.sSrvInfo.latency           = 0;
    pMe->mL2.sSrvInfo.delay_variation   = 0;    
    pMe->mL2.bSrvDoVerify         = TRUE;
    pMe->mL2.uCliBytesToSend      = 2000;
    pMe->mL2.bEnableSrvSvcSec     = FALSE;
    pMe->mL2.bEnableCliSvcSec     = FALSE;

    pMe->mSPP.status.uChannelNumber  = 0;
#ifdef FEATURE_BT_2_1
    pMe->mL2.bSecSelected     = FALSE;
    pMe->mL2.srvSecType       = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mL2.bAuthorize       = FALSE;
    pMe->mL2.bAuthorizeFirst  = FALSE;
    pMe->mSPP.bSecSelected     = FALSE;
    pMe->mSPP.srvSecType       = BTAPP_RM_SVC_SEC_DEFAULT;
    pMe->mSPP.bAuthorize       = FALSE;
    pMe->mSPP.bAuthorizeFirst  = FALSE;
#endif

    BTApp_WriteConfigFile( pMe );
  }
#ifndef FEATURE_PHONE_VR
  else
  {
    pMe->mAG.bVREnabled   = FALSE;
  }
#endif

  pMe->mEnablingType = BTAPP_ENABLING_AG;
  return success;
}

/* ==========================================================================
FUNCTION BTApp_EnableServiceSecurity
DESCRIPTION
============================================================================= */
static void BTApp_EnableServiceSecurity( CBTApp* pMe )
{
  int     result;
  boolean bAuthorize;

  pMe->mRM.svcId.serviceIdMethod  = AEEBT_SIM_SDP_UUID;
  pMe->mRM.svcId.id.uServiceClass = AEEBT_SD_SERVICE_CLASS_DIALUP_NETWORKING;

  if ( pMe->mNA.bEnableSvcSec != FALSE )
  {
    pMe->mRM.minSec = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
    bAuthorize = TRUE;
  }
  else
  {
    pMe->mRM.minSec = AEEBT_SEC_NONE;
    bAuthorize = FALSE;
  }
  result = IBTEXTRM_SetServiceSecurity( pMe->mRM.po, &pMe->mRM.svcId, 
                                        pMe->mRM.minSec, bAuthorize,
                                        TRUE );  // authorize first
  if ( result != SUCCESS )
  {
    MSG_FATAL( "HndlSlction - SetServiceSecurity failed r=%x ss=%x", 
               result, pMe->mNA.bEnableSvcSec, 0 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
}


//Add By zzg 2011_01_05
/* ==========================================================================
FUNCTION BTApp_CreateOtherApp/BTApp_FreeOtherAPp
DESCRIPTION: 因为蓝牙可能有多个APP实例。
============================================================================= */

//Add By zzg 2010_10_29  
//将m_pIAnn/m_pConfig/m_pIRUIM分离出来以进行反复CREATE/FREE
//因为BTApp_InitAppData/BTApp_FreeAppData只在启动和退出时调用一次
static void BTApp_CreateOtherApp( CBTApp* pMe )
{
	AEEDeviceInfo  di;
	static boolean  bCtDone = FALSE;
	
	MSG_FATAL("***zzg BTApp_CreateOtherApp Start bCtDone=%x***", bCtDone, 0, 0);
	MSG_FATAL("***zzg BTApp_CreateOtherApp m_pConfig=%x, m_pIAnn=%x, m_pIRUIM=%x***", pMe->m_pConfig, pMe->m_pIAnn, pMe->m_pIRUIM);
	
    if (NULL == pMe)
    {
        return;
    }
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
	
    pMe->m_rect.x       = 0;
    pMe->m_rect.y       = 0;
    pMe->m_rect.dx      = (int16)di.cxScreen;
    pMe->m_rect.dy      = (int16)di.cyScreen;

    pMe->m_bAppIsReady = FALSE;
    pMe->m_ePreState = BTAPPST_NONE;
    pMe->m_eCurState = BTAPPST_INIT;
    pMe->m_pActiveDlg = NULL;
    pMe->m_eDlgRet = DLGRET_CREATE;
    pMe->m_bNotOverwriteDlgRet = FALSE;

    pMe->m_currDlgId = 0;
    pMe->m_nSubDlgId = 0;

	if (bCtDone == TRUE)
	{
		return;
	}
	
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG, (void **)&pMe->m_pConfig))
	{    	
	    BTApp_FreeOtherApp(pMe);
	    return;
	}


	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void **)&pMe->m_pIAnn))
	{    	
	    BTApp_FreeOtherApp(pMe);
	    return;
	}
	

#ifndef WIN32    
	// 创建 IRUIM 接口	
	if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_RUIM, (void **)&pMe->m_pIRUIM))
	{
	    BTApp_FreeOtherApp(pMe);
	    return;
	}	
#endif//WIN32	

	bCtDone = TRUE;
}

static void BTApp_FreeOtherApp( CBTApp* pMe )
{	
	if (pMe->m_pIAnn != NULL )
	{
	  IANNUNCIATOR_Release(pMe->m_pIAnn);
	  pMe->m_pIAnn = NULL;
	}

	if (pMe->m_pConfig) 
	{
	  ICONFIG_Release(pMe->m_pConfig);
	  pMe->m_pConfig = NULL;
	}

	// 释放 IRUIM 接口
	if (pMe->m_pIRUIM != NULL)
	{
	  IRUIM_Release(pMe->m_pIRUIM);
	  pMe->m_pIRUIM = NULL;
	}
  
}
//Add End


//Add By zzg 2011_06_24
/*
static void BTApp_HandleRMNotify(CBTApp* pMe, uint16 wParam)
{	
	MSG_FATAL("***zzg BTApp_HandleRMNotify wParam=%d***", wParam, 0, 0);		
	
	switch (wParam)
	{
		//RM_Notify Start
		case AEEBT_RM_EVT_REBOND_REQUEST:	
		{
			char   passkey[AEEBT_SSP_PASSKEY_LEN];
			
			WSTRTOSTR(pMe->mRM.wSSPPassKey, passkey, sizeof(passkey));		
			MSG_FATAL("***zzg BTApp_HandleRMNotify AEEBT_RM_EVT_REBOND_REQUEST passkey=%s***", passkey, 0, 0);
			DBGPRINTF("***zzg BTApp_HandleRMNotify AEEBT_RM_EVT_REBOND_REQUEST passkey=%s***", passkey);

			BTApp_BuildPrompt(pMe, BT_APP_MENU_REBOND);
			break;
		}
		
		case AEEBT_RM_EVT_USER_CFM_REQUEST: 
		{			
			char   passkey[AEEBT_SSP_PASSKEY_LEN];
			
			WSTRTOSTR(pMe->mRM.wSSPPassKey, passkey, sizeof(passkey));		
			MSG_FATAL("***zzg BTApp_HandleRMNotify AEEBT_RM_EVT_USER_CFM_REQUEST passkey=%s***", passkey, 0, 0);
			DBGPRINTF("***zzg BTApp_HandleRMNotify AEEBT_RM_EVT_USER_CFM_REQUEST passkey=%s***", passkey);
			
			BTApp_BuildPrompt(pMe, BT_APP_MENU_USER_CFM_RQST); 			
			break;
		}		

		case AEEBT_RM_EVT_PASSKEY_REQUEST:
		case AEEBT_RM_EVT_PIN_REQUESTED:	
		{
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
	                           AEECLSID_BLUETOOTH_APP,
	                           EVT_USER, EVT_PIN_RQST, 0L);
			break;
		}
		
		case AEEBT_RM_EVT_PASSKEY_REPLIED:
		{
			BTApp_ShowDevMsg(pMe, IDS_MSG_BOND_FAILED, NULL, 0);		
			break;
		}
					
		case AEEBT_RM_EVT_BONDED:
		{
			break;
		}
		
		case AEEBT_RM_EVT_PASSKEY_NOTIFICATION:
		{
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
			                   AEECLSID_BLUETOOTH_APP,
			                   EVT_USER, EVT_PASSKY, 0L);   
			break;
		}
		
		case AEEBT_RM_EVT_UNBONDED: 				
		case AEEBT_RM_EVT_PIN_REPLIED:
		case AEEBT_RM_EVT_DEVICE_ADDED:
		case AEEBT_RM_EVT_DEVICE_REMOVED:
		case AEEBT_RM_EVT_AUTHORIZE_REQUEST:
		case AEEBT_RM_EVT_USER_CFM_REPLIED:
		{			
			break;
		}
		//RM_Notify End
		
		default:
		{
			break;
		}
	}
}
//Add End

*/

static void BTApp_APPIsReadyTimer(void *pme)
{
    CBTApp *pMe = (CBTApp *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_BLUETOOTH_APP,
                            EVT_APPISREADY,
                            0,
                            0);
}

//Add End



/* ==========================================================================
FUNCTION BTApp_InitAppData
DESCRIPTION
============================================================================= */

//static boolean BTApp_InitAppData(IApplet* pi)
static int BTApp_InitAppData(CBTApp *pMe)
{
	boolean               success = FALSE;	
	IFileMgr*             pIFileMgr;

#ifdef FEATURE_APP_TEST_AUTOMATION
	#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

	MSG_FATAL("***zzg BTApp_InitAppData Start***", 0, 0, 0);  
 

  /* Check for bar file here. */  
  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                              (void **)&pIFileMgr ) == SUCCESS )
  {

#if 0   
    if( IFILEMGR_Test( pIFileMgr, BTAPP_RES_FILE ) == SUCCESS )
    {
      success = TRUE;
    }
	else
	{
		MSG_FATAL("***zzg IFILEMGR_Test AEE_APPSBTAPP_RES_FILE Failed!***", 0, 0, 0);
	}
#else
	success = TRUE;
#endif
	
    IFILEMGR_Release( pIFileMgr );
  }

  //Add For BT_MSG_BOX
  pMe->m_bNeedStr = FALSE;
  pMe->m_bPromptNeedStr = FALSE;  
  pMe->m_bEditNeedStr = FALSE;

  pMe->m_app_flag = FALSE;
  //Add End
   
  if ( success )
  {
    pMe->pText1 = MALLOC( LONG_TEXT_BUF_LEN*sizeof(AECHAR) );
    pMe->pText2 = MALLOC( SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );

    if ( (pMe->pText1 == NULL) || (pMe->pText2 == NULL) )
    {
      if ( pMe->pText1 != NULL )
      {
        FREE( pMe->pText1 );
      }
      else
      {
        FREE( pMe->pText2 );
      }
      success = FALSE;
    }
  }
 
  if ( success )
  {
    AEEDeviceInfo di;
    di.wStructSize = sizeof(AEEDeviceInfo);

    /* Get the device information */
    ISHELL_GetDeviceInfo( pMe->m_pShell, &di );
    pMe->m_rect.dx      = di.cxScreen;
    pMe->m_rect.dy      = di.cyScreen;
    pMe->m_nColorDepth  = di.nColorDepth;

    // increase font size on VGA displays
    //AppComFunc_SelectFonts( pMe->m_pIDisplay );
	
    pMe->pMem             = NULL;
    pMe->m_pISoftMenu     = NULL;
    pMe->m_pIMenu         = NULL;
    pMe->m_pStatic        = NULL;
    pMe->m_pText          = NULL;
    pMe->mAG.po           = NULL;
    pMe->mNA.po           = NULL;
    pMe->mRM.po           = NULL;
    pMe->mSD.po           = NULL;
    pMe->mSPP.po          = NULL;
    pMe->mAG.pIncomingCall  = NULL;
    pMe->mAG.pOutgoingCall  = NULL;
#ifdef FEATURE_BT_EXTPF_OPP
    pMe->mOPP.po          = NULL;	
#endif   
#ifdef FEATURE_BT_EXTPF_FTP
    pMe->mFTP.po          = NULL;
#endif   
#ifdef FEATURE_BT_EXTPF_BIP
    pMe->mBIP.po          = NULL;
    pMe->mBIP.bPrintFile  = FALSE;
    pMe->mBIP.printerBDAddr = NULL_BD_ADDR;
#endif   
#ifdef FEATURE_BT_EXTPF_BPP
    pMe->mBPP.po          = NULL;
    pMe->mBPP.bPrintFile  = FALSE;
    pMe->mBPP.printerBDAddr = NULL_BD_ADDR;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    pMe->mPBAP.po         = NULL;
    pMe->mPBAP.vCardFormat= AEEBT_PBAP_FORMAT_VCARD_2_1;
    STRTOWSTR( "7", pMe->mPBAP.wFilterValue, 
               sizeof(pMe->mPBAP.wFilterValue) );
    STRTOWSTR( "0", pMe->mPBAP.wListStartOffset, 
               sizeof(pMe->mPBAP.wListStartOffset) );
    STRTOWSTR( "65535", pMe->mPBAP.wMaxListCount, 
               sizeof(pMe->mPBAP.wMaxListCount) );
#endif    
#ifdef FEATURE_BT_EXTPF_AV
    pMe->mA2DP.po         = NULL;
    pMe->mAVRCP.po        = NULL;
    pMe->mA2DP.bVolumesInit = FALSE;
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    pMe->bFirstLaunch     = TRUE;
    pMe->bSuspended       = FALSE;

	pMe->bStartFromOtherApp	= FALSE;	//Add By zzg 2010_11_08
	pMe->bStartFromPushReq  = FALSE;	//Add By zzg 2012_11_23
	pMe->bStartFromAudio	= FALSE;	//Add By zzg 2012_11_28
	pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27	

	pMe->uDeviceSrhType		= AEEBT_COD_SC_ALL;	//Add By zzg 2011_10_19
	pMe->uDeviceListType	= AEEBT_RM_EC_ALL;	//Add By zzg 2011_10_26

	pMe->bMusicPlayerSuspend = FALSE;	//Add By zzg 2011_11_07
	
    pMe->bVocoderOn       = FALSE;
    pMe->uTopMenu         = 0;
    TOP_MENU              = BT_APP_MENU_MAIN;
    MENU_SET_SEL( 0 );
#ifdef FEATURE_UI_CORE
    pMe->mAG.bVolumesInit = FALSE;
#endif //FEATURE_UI_CORE	
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return success;
}

/* ==========================================================================
FUNCTION BTApp_FreeAppData
DESCRIPTION
============================================================================= */
//static void BTApp_FreeAppData( IApplet* pi )
static void BTApp_FreeAppData(CBTApp *pMe)
{

#ifdef FEATURE_APP_TEST_AUTOMATION
	#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

	//CBTApp* pMe = (CBTApp*)pi;

	MSG_FATAL("***zzg BTApp_FreeAppData***", 0, 0, 0);

	ISHELL_RegisterNotify(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, AEECLSID_BLUETOOTH_NOTIFIER, 0);

	BTApp_AGDeInit((IApplet*)pMe);

	BTApp_FreeOtherApp(pMe);		//Add By zzg 2011_09_28

    //Add By zzg 2011_02_26 
	if (NULL != pMe->mSD.devRec)
	{
		FREE(pMe->mSD.devRec);
		pMe->mSD.devRec = NULL;
	}
	
	if (NULL != pMe->mSD.browseRec)
	{
		FREE(pMe->mSD.browseRec);
		pMe->mSD.browseRec = NULL;
	}

	if (NULL != pMe->mSD.svcRec)
	{
		FREE(pMe->mSD.svcRec);
		pMe->mSD.svcRec = NULL;
	}

	if (NULL != pMe->mRM.device)
	{
		FREE( pMe->mRM.device );
		pMe->mRM.device = NULL;
	}
	
	if (NULL != pMe->mRM.devToSwitchRole)
	{
		FREE( pMe->mRM.devToSwitchRole );
		pMe->mRM.devToSwitchRole = NULL;
	}

	if (NULL != pMe->mRM.linkStatus)
	{
		FREE( pMe->mRM.linkStatus );
		pMe->mRM.linkStatus = NULL;
	}
    //Add End	
  
	if (pMe->mNA.po != NULL)
	{
		IBTEXTNA_Release(pMe->mNA.po);
		pMe->mNA.po = NULL;
	}

	if ( pMe->mSD.po != NULL )
	{
		IBTEXTSD_Release( pMe->mSD.po );
		pMe->mSD.po = NULL;
	}

	if ( pMe->mRM.po != NULL )
	{
		IBTEXTRM_Release( pMe->mRM.po );
		pMe->mRM.po = NULL;
	}

	if ( pMe->mSPP.po != NULL )
	{
		IBTEXTSPP_Release( pMe->mSPP.po );
		pMe->mSPP.po = NULL;
	}

#ifdef FEATURE_BT_EXTPF_OPP
	if ( pMe->mOPP.po != NULL )
	{
		IBTEXTOPP_Release( pMe->mOPP.po );
		pMe->mOPP.po = NULL;
	}
#endif 

#ifdef FEATURE_BT_EXTPF_FTP
	if ( pMe->mFTP.po != NULL )
	{
		IBTEXTFTP_Release( pMe->mFTP.po );
		pMe->mFTP.po = NULL;
	}
#endif 

#ifdef FEATURE_BT_EXTPF_BIP
	if ( pMe->mBIP.po != NULL )
	{
		IBTEXTBIP_Release( pMe->mBIP.po );
		pMe->mBIP.po = NULL;
	}
#endif 

#ifdef FEATURE_BT_EXTPF_BPP
	if ( pMe->mBPP.po != NULL )
	{
		IBTEXTBPP_Release( pMe->mBPP.po );
		pMe->mBPP.po = NULL;
	}
#endif

#ifdef FEATURE_BT_EXTPF_CTP
	if ( pMe->mCTP.po != NULL )
	{
		IBTEXTCTP_Release( pMe->mCTP.po );
		pMe->mCTP.po = NULL;
	}
	
	if ( pMe->mICP.po != NULL )
	{
		IBTEXTICP_Release( pMe->mICP.po );
		pMe->mICP.po = NULL;
	}
#endif

#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
	#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
	if ( pMe->mA2DP.po != NULL )
	{
		IBTEXTA2DP_Release( pMe->mA2DP.po );
		pMe->mA2DP.po = NULL;
	}
	
	if ( pMe->mAVRCP.po != NULL )
	{
		IBTEXTAVRCP_Release( pMe->mAVRCP.po );
		pMe->mAVRCP.po = NULL;
	}
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
	if ( pMe->mPBAP.po != NULL )
	{
		IBTEXTPBAP_Release( pMe->mPBAP.po );
		pMe->mPBAP.po = NULL;
	}
#endif

#ifdef FEATURE_IOBEX
	if ( pMe->mOBEX.poServer != NULL )
	{
		IOBEXServer_Release( pMe->mOBEX.poServer );
		pMe->mOBEX.poServer = NULL;
	}
	
	if ( pMe->mOBEX.poClient != NULL )
	{
		IOBEXClient_Release( pMe->mOBEX.poClient );
		pMe->mOBEX.poClient = NULL;
	}
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

	if ( pMe->m_pISoftMenu != NULL )
	{
		IMENUCTL_Release( pMe->m_pISoftMenu );
		pMe->m_pISoftMenu = NULL;
	}

	if ( pMe->m_pIMenu != NULL )
	{
		IMENUCTL_Release( pMe->m_pIMenu );
		pMe->m_pIMenu = NULL;
	}

	if ( pMe->m_pStatic != NULL )
	{
		ISTATIC_Release( pMe->m_pStatic );
		pMe->m_pStatic = NULL;
	}

	if ( pMe->m_pText != NULL )
	{
		ITEXTCTL_Release( pMe->m_pText );
		pMe->m_pText = NULL;
	}

	if ( pMe->pText1 != NULL )
	{
		FREE( pMe->pText1 );
		pMe->pText1 = NULL;
	}

	if ( pMe->pText2 != NULL )
	{
		FREE( pMe->pText2 );
		pMe->pText2 = NULL;
	}

	if ( pMe->pMem != NULL )
	{
		FREE( pMe->pMem );
		pMe->pMem = NULL;
	}

#ifdef FEATURE_BT_EXTPF_SAP
	#error code not present
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION  
	#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}
/* ==========================================================================
FUNCTION BTApp_CancelDevNameRequest
DESCRIPTION
============================================================================= */

/*static*/ void BTApp_CancelDevNameRequest( CBTApp* pMe )
{
  if ( pMe->mRM.uGetNameDevIdx != MAX_DEVICES )
  {
    AEEBTBDAddr *pBdAddr = &pMe->mRM.device[ pMe->mRM.uGetNameDevIdx ].bdAddr;
    MSG_FATAL( "BTApp_OnAppStop Cancel device name req", 0, 0, 0 );
    IBTEXTSD_GetDeviceNameCancel( pMe->mSD.po,pBdAddr );
  }
   return;
}

/*************************************************************************************

FUNCTION BTApp_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean BTApp_HandleEvent(IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet. It was initialized during the AEEClsCreateInstance() function.

  ecode: Specifies the Event sent to this applet

  wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
**************************************************************************************/

/*
static boolean BTApp_HandleEvent
(
  IApplet*  pi, 
  AEEEvent  eCode, 
  uint16    wParam, 
  uint32    dwParam
)
{  
  CBTApp* pMe = (CBTApp*)pi;
  boolean event_processed = TRUE;
  uint16 msgID;

  AEEAppStart *args = (AEEAppStart*)dwParam;		//Add By zzg 2010_11_08
  
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  AEEBTHCIMode HCIMode;
#ifdef FEATURE_APP_TEST_AUTOMATION  
#error code not present
#endif //  FEATURE_APP_TEST_AUTOMATION

  switch (eCode) 
  {
	//Add By zzg 2010_11_23
  	case EVT_APP_START_BACKGROUND:	//Config蓝牙开启，COREAPP进行蓝牙初始化
	{		
		MSG_FATAL("***zzg BTApp_HandleEvent EVT_APP_START_BACKGROUND***", 0, 0, 0);
		
		if ((args != NULL) && (args->pszArgs != NULL))	
		{
			if (STRNCMP(args->pszArgs, "ResetBT", 7) == 0)		//Reset Factory
			{				
				if ( event_processed )
				{
					MSG_FATAL("***zzg EVT_APP_START_BACKGROUND ResetBT Factory***", 0, 0, 0);
					BTApp_DisableBT(pMe);	
				}
			}			
		}		
		
		pMe->bFirstLaunch = FALSE;
		pMe->bSuspended = FALSE;						
		break;
	}
	//Add End
		
#ifndef FEATURE_UIONE_HDK
    case EVT_FIRST_LAUNCH:
    {
      // starting for the first time on power up
      MSG_HIGH( "HndlEv - START", 0, 0, 0 );
      event_processed = BTApp_StartBT( pMe ); 
      break;
    }
#endif
    case EVT_APP_START:
    case EVT_APP_RESUME:
    {
    	MSG_HIGH( "HndlEv - START/RESUME ev=%x tm=%x suspend=%x", eCode, pMe->uTopMenu, pMe->bSuspended );

		if (EVT_APP_START == eCode)
		{
			pMe->bStartBTaplication = TRUE;
			MSG_FATAL("***zzg BTApp EVT_APP_START***",0,0,0);
		}
		else
		{
			pMe->bStartBTaplication = TRUE;
			MSG_FATAL("***zzg BTApp EVT_APP_RESUME***",0,0,0);
		}

		//Add By zzg 2010_11_08
    	if ((args != NULL) && (args->pszArgs != NULL))	
    	{
    		if (STRNCMP(args->pszArgs,"GetFile",7) == 0)	//Get File
			{
				MSG_FATAL("***zzg GetFile***", 0, 0, 0);

				if ( !pMe->bSuspended )
		        {
		          event_processed = BTApp_Init( pMe );
		        }
		        if ( event_processed )
		        {
		          if ( pMe->mAG.bStartedVr != FALSE )
		          {
		            pMe->mAG.bStartedVr = FALSE;
		            IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
		            if ( pMe->mAG.callState == BTAPP_AG_CALL_STARTVR )
		            {
		              pMe->mAG.callState = BTAPP_AG_CALL_NONE;
		  #ifndef FEATURE_AVS_BT_SCO_REWORK
		              BTApp_ReleaseBTDevice( pMe, FALSE );
		  #endif // !FEATURE_AVS_BT_SCO_REWORK
		            }
		          }

				  BTApp_ShowMessage( pMe, NULL, NULL, 0 );
		  		  		
		          IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
		          if ( HCIMode != AEEBT_HCIM_OFF )
		          {
		            BTApp_ShowMessage( pMe, IDS_MSG_HCI_ON_WARNING, NULL, 0 );
		          }
		        }
				
		        pMe->bFirstLaunch = FALSE;
		        pMe->bSuspended = FALSE;		
				
		        break;
			}
			else											//file send via bluetooth
			{
	    		pMe->bStartFromOtherApp	= TRUE;		
	    		BTApp_SaveSendFilePath(pMe, args->pszArgs);				
			}
    	}
    	//Add End				
	  
        if ( !pMe->bSuspended )
        {
          event_processed = BTApp_Init( pMe );
        }
        if ( event_processed )
        {
          if ( pMe->mAG.bStartedVr != FALSE )
          {
            pMe->mAG.bStartedVr = FALSE;
            IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
            if ( pMe->mAG.callState == BTAPP_AG_CALL_STARTVR )
            {
              pMe->mAG.callState = BTAPP_AG_CALL_NONE;
  #ifndef FEATURE_AVS_BT_SCO_REWORK
              BTApp_ReleaseBTDevice( pMe, FALSE );
  #endif // !FEATURE_AVS_BT_SCO_REWORK
            }
          }
  
  		//Add By zzg 2010_11_09
  		if (TRUE == pMe->bStartFromOtherApp)
  		{
  			boolean bt_status = FALSE;
  			ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS,&bt_status, sizeof(bt_status));
  			
  			if (bt_status == FALSE)	
  			{
  				BTApp_BuildPrompt(pMe, BT_APP_MENU_OPEN);
  			}
  			else
  			{
  				if (BTApp_HCIModeOn( pMe ) == FALSE)
  				{
  					MSG_FATAL("***zzg BTApp EVT_APP_START  BT_APP_MENU_OPP_SENDFILE***",0,0,0);
  					BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_SENDFILE);
  				}
  			}
  		}
  		else
  		{
  			
  			BTApp_BuildTopMenu( pMe );
  		}
  		//Add End        
  		
          IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
          if ( HCIMode != AEEBT_HCIM_OFF )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_HCI_ON_WARNING, NULL, 0 );
          }
        }
        pMe->bFirstLaunch = FALSE;
        pMe->bSuspended = FALSE;						
        break;
    }
    case EVT_APP_STOP:
    {
      boolean* pb = (boolean*)dwParam;
	  
      MSG_HIGH( "HndlEv - APP_STOP tm=%x dw=0x%x", pMe->uTopMenu, dwParam, 0 );
      if(pb)
      {
        *pb = FALSE;  // Set the app to background app 
        pMe->bSuspended = TRUE;
		pMe->bStartFromOtherApp = FALSE;		//Add By zzg 2010_11_08
		pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27
      }
      BTApp_OnAppStop( pMe );		
      break;
    }
    case EVT_APP_SUSPEND:
    {
      AEESuspendInfo* pi = (AEESuspendInfo*)dwParam;
      MSG_HIGH( "HndlEv - APP_SUSPEND wP=0x%x dw=0x%x", wParam, dwParam, 0 );
      if ( pi != NULL )
      {
        pi->bCloseDialogs = FALSE;
      }
      pMe->bSuspended = TRUE;
      break;
    }
    case EVT_APP_NO_SLEEP:
    {
      return FALSE;
    }
    case EVT_DIALOG_INIT:
    {
      IDIALOG_SetEventHandler( (IDialog*)dwParam, BTApp_TextEditHandleEvent, 
                               pMe );
      IDIALOG_SetProperties( (IDialog*)dwParam, DLG_HANDLE_ALL_EVENTS );
      break;
    }
    case EVT_DIALOG_END:
    {
		MSG_FATAL( "HndlEv - DIALOG_END wP=0x%x dw=0x%x", wParam, dwParam, 0 );
		BTApp_HandleClearKey( pMe );
		break;
    }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

    case EVT_KEY:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    {
      MSG_FATAL( "HndlEv - EVT_KEY wP=0x%x dw=0x%x", wParam, dwParam, 0 );

	  //Add  By zzg 2010_11_27
	  if (TRUE == pMe->bUpdateProgress)
	  {
		if ((AVK_CANCEL == wParam) || (AVK_CLR == wParam))
		{		
			IBTEXTOPP_Abort(pMe->mOPP.po);
			
			if ((pMe->mOPP.bConnected == TRUE) || ((pMe->mOPP.bConnecting == TRUE)))
			{
				if (IBTEXTOPP_Disconnect( pMe->mOPP.po) != SUCCESS)
				{
					MSG_FATAL("***zzg IBTEXTOPP_Disconnect != SUCCESS***", 0, 0, 0);
				}	
				else 
				{
					pMe->mOPP.bConnecting = FALSE;
				}
			}
			else
			{		
				if (pMe->mOPP.bRegistered == FALSE)
				{
					int result;						

					BTApp_SetBondable(pMe);

					if ((result = IBTEXTOPP_Register( pMe->mOPP.po, AEEBT_OPP_FORMAT_ALL,szServerNameOPP )) != SUCCESS )
					{
						MSG_FATAL("***zzg BTApp_OPPPull OPP_Register() failed with %x***", result, 0, 0 );
						BTApp_ClearBondable( pMe ); 
					}
					else
					{	
						if (pMe->mSD.bDiscoverable == FALSE)
						{
							IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
						}		
					} 	 
				}	
			}
		}
		
		return TRUE;
	  }	  	  
	  else
	  {
		 if (AVK_CANCEL == wParam)
		 {
			ISHELL_CloseApplet(pMe->m_pShell, FALSE );
			return TRUE;
		 }
	  }
	  //Add End
	  
      if ( wParam == AVK_END )
      {
      	MSG_FATAL("***zzg BTApp_HandleEvent EVT_KEY wParam == AVK_END***", 0, 0, 0);
        return FALSE;
      }
	  
      if ( BTApp_KeysDisabled( pMe, wParam ) )
      {
        return TRUE;
      }

      event_processed = BTApp_MenuHandleEvent( pMe, eCode, wParam, dwParam );
	  
      if ( event_processed || (wParam == AVK_CLR) )
      {
        return event_processed;
      }

      if ( IMENUCTL_HandleEvent( pMe->m_pISoftMenu, eCode, wParam, 0 ) )
      {
        if ( (TOP_MENU == BT_APP_MENU_DEV_INFO) && 
             ((wParam == AVK_RIGHT) || (wParam == AVK_LEFT)) )
        {
          MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pISoftMenu ) );
        }
        return TRUE;
      }
      if ( IMENUCTL_HandleEvent( pMe->m_pIMenu, eCode, wParam, 0 ) )
      {
        if ( (TOP_MENU == BT_APP_MENU_DEV_RESP) && 
             ((wParam == AVK_UP) || (wParam == AVK_DOWN)) )
        {
          MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pIMenu ) );
        }
        return TRUE;
      }
      break;
    }
    case EVT_COMMAND: //0x200
    {
      MSG_FATAL( "HndlEv - EVT_COMMAND wP=0x%x dw=0x%x", wParam, dwParam, 0 );
      break;
    }
    case EVT_NOTIFY:
    {
      AEENotify* pN = (AEENotify*) dwParam;
      AEEEvent  evt = GET_NOTIFIER_VAL(pN->dwMask);
	  
      //MSG_FATAL( "HndlEv - NOTIFY cls=%x m=%x ev=%d", 
      //          pN->cls, GET_NOTIFIER_MASK(pN->dwMask), evt );
      if ( pN->cls == AEECLSID_BLUETOOTH_NOTIFIER )
      {
        switch ( GET_NOTIFIER_MASK(pN->dwMask) )
        {
          case NMASK_BT_AG:
            BTApp_ProcessAGNotifications( pMe, evt ,pN->pData );
            break;
          case NMASK_BT_NA:
            BTApp_ProcessNANotifications( pMe, evt, pN->pData );
            break;
          case NMASK_BT_RM:
            BTApp_ProcessRMNotifications( pMe, evt, pN->pData );
            break;
          case NMASK_BT_SD:
            BTApp_ProcessSDNotifications( pMe, evt, pN->pData );
            break;
          case NMASK_BT_SPP:
            BTApp_ProcessSPPNotifications( pMe, evt, pN->pData );
            break;
          case NMASK_BT_L2:
            BTApp_ProcessL2Notifications( pMe, evt, pN->pData );
            break;
#ifdef FEATURE_BT_EXTPF_OPP
          case NMASK_BT_OPP:
            BTApp_ProcessOPPNotifications( pMe, evt, pN->pData );
            break;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
          case NMASK_BT_FTP:
            BTApp_ProcessFTPNotifications( pMe, evt, pN->pData );
            break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
          case NMASK_BT_BIP:
            BTApp_ProcessBIPNotifications( pMe, evt, pN->pData );
            break;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
          case NMASK_BT_BPP:
            BTApp_ProcessBPPNotifications( pMe, evt, pN->pData );
            break;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
          case NMASK_BT_PBAP:
            BTApp_ProcessPBAPNotifications( pMe, evt, pN->pData );
            break;
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_CTP
          case NMASK_BT_CTP:
            if ( evt & AEEBT_ICP_EVT_BASE ) 
            {
              BTApp_ProcessICPNotifications( pMe, evt, pN->pData );
            }
            else
            {
              BTApp_ProcessCTPNotifications( pMe, evt, pN->pData );
            }
            break;
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
          case NMASK_BT_A2DP:
            BTApp_ProcessA2DPNotifications( pMe, evt, pN->pData );
            switch ( evt )
            {
              case AEEBT_A2DP_EVT_START:
                if ((((NotificationData*)pN->pData)->A2DPStartInit == 
                     AEEBT_A2DP_EVENT_LOCAL_INIT) ||
                    (pMe->mAG.callState == BTAPP_AG_CALL_NONE))
                {
                  BTApp_DisconnectAudio( pMe, pMe->mAG.bForceUnmute );
                }
                // else, ignore remote-initiated Start command because
                 //  a call is present and call audio must be left alone 
                break;
              case AEEBT_A2DP_EVT_SUSPEND:
                MSG_FATAL( "AG: audioConnecting=%d callState=%d", 
                         pMe->mAG.bAudioConnecting, pMe->mAG.callState, 0);
                // did we suspend A2DP so we can bring up SCO? 
                if ( pMe->mAG.bAudioConnecting == TRUE )
                {
                  // carry out pending SCO connection 
                  pMe->mAG.bAudioConnecting = FALSE;
                  BTApp_ConnectAudio( pMe, pMe->mAG.bForceUnmute );
                }
                else if ( (pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_INBAND) ||
                          (pMe->mAG.callState == BTAPP_AG_CALL_CONNECTED) )
                {
                  BTApp_ConnectAudio( pMe, TRUE );
                }
                break;
            }
            break;
          case NMASK_BT_AVRCP:
            BTApp_ProcessAVRCPNotifications( pMe, evt, pN->pData );
            break;
#endif
          default:
            return FALSE;
        }
      }
#ifdef FEATURE_IOBEX
      else if ( pN->cls == AEECLSID_OBEX_NOTIFIER )
      {
        if( GET_NOTIFIER_MASK(pN->dwMask) == NMASK_OBEX_GOEP )
        {
          return BTApp_ProcessOBEXNotifications( pMe, eCode, wParam, dwParam );
        }
      }
#endif
      else if ( ((AEENotify*) dwParam)->cls == AEECLSID_PHONENOTIFIER 
#ifdef FEATURE_BT_VT
              || ( ((AEENotify*) dwParam)->cls == AEECLSID_VIDEOPHONE_NOTIFIER )
#endif
              )
      {
#ifdef FEATURE_BT_VT
        BTApp_ProcessVTEvents( pMe, wParam, dwParam );
#endif
        return BTApp_ProcessTELEPHONEEvents( pMe, wParam, dwParam );
      }
#ifdef FEATURE_BT_HFP_1_5
      else if ( pN->cls == AEECLSID_BATTERYNOTIFIER )
      {
        return BTApp_ProcessBattEvents( pMe, wParam, dwParam );
      }
#endif      
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

//#ifndef FEATURE_UIONE_HDK	//Modify by zzg 2010_12_13
      else if (  pN->cls == AEECLSID_SHELL )
      {
        switch ( GET_NOTIFIER_MASK(pN->dwMask) )
        {     
          case NMASK_SHELL_INIT:
		  {				
            // starting for the first time on power up
            MSG_HIGH( "HndlEv - START", 0, 0, 0 );
            event_processed = BTApp_StartBT( pMe ); 
			
			if ((event_processed) && (pMe->bEnableAtStart))
			{
				if (pMe->mAG.bStartedVr != FALSE)
				{
					pMe->mAG.bStartedVr = FALSE;
					IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
					
					if ( pMe->mAG.callState == BTAPP_AG_CALL_STARTVR )
					{
						pMe->mAG.callState = BTAPP_AG_CALL_NONE;
#ifndef FEATURE_AVS_BT_SCO_REWORK
						BTApp_ReleaseBTDevice( pMe, FALSE );
#endif 
					}
				}

				if (BTApp_HCIModeOn(pMe) == FALSE)
				{
					if (BTApp_OPPInit(pMe) != FALSE)	//Init BTApp--->OPP	
					{					
						MSG_FATAL("***zzg EVT_APP_START_BACKGROUND bConnected=%d, bConnecting=%d, bRegistered=%d***", 
						           pMe->mOPP.bConnected, pMe->mOPP.bConnecting, pMe->mOPP.bRegistered);

						BTApp_SetBondable(pMe);	//开机如果蓝牙是开启状态则直接设置为可绑定
						
						//change to the server for getin file
						if ((pMe->mOPP.bConnected == TRUE) || ((pMe->mOPP.bConnecting == TRUE)))
						{
							if (IBTEXTOPP_Disconnect( pMe->mOPP.po) != SUCCESS)
							{
								MSG_FATAL("***zzg IBTEXTOPP_Disconnect != SUCCESS***", 0, 0, 0);
							}	
							else 
							{
								pMe->mOPP.bConnecting = FALSE;
							}
						}
						else
						{		
							if (pMe->mOPP.bRegistered == FALSE)
							{
								int result;						

								BTApp_SetBondable(pMe);

								if ((result = IBTEXTOPP_Register( pMe->mOPP.po, AEEBT_OPP_FORMAT_ALL,szServerNameOPP )) != SUCCESS )
								{
									MSG_FATAL("***zzg BTApp_OPPPull OPP_Register() failed with %x***", result, 0, 0 );
									BTApp_ClearBondable( pMe ); 
								}
								else
								{					
									if (pMe->mSD.bDiscoverable == FALSE)
									{
										IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
									}		
								} 	 
							}	
						}
						
					}
					else
					{
						MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
						BTApp_OPPCleanup( pMe );				
					}
				}

				IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
				
				if ( HCIMode != AEEBT_HCIM_OFF )
				{
					MSG_FATAL("***zzg HCIMode=%x***", HCIMode, 0, 0);
					
					//BTApp_ShowMessage( pMe, IDS_MSG_HCI_ON_WARNING, NULL, 0 );
				}
			}
	  		
			//Add End			
            break;
          }
          default:
            return FALSE;
        }
      }
//#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
      break;
    }

    case EVT_USER:
    {
      MSG_FATAL( "HndlEv - EVT_USER wP=%d tm=%d", wParam, TOP_MENU, 0 );
	  MSG_FATAL("***zzg BTApp_HandleEvent EVT_USER wParam=%d, dwparam=%d***", wParam, dwParam, 0);
	  
      switch ( wParam )
      {
        case EVT_AG_DISABLED:
          BTApp_ProcEvtAGDisabled( pMe );
          break;
        case EVT_A2DP_DISCONNECTED:
          BTApp_A2DPDisable( pMe );
          BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        case EVT_AVRCP_DISCONNECTED:
          BTApp_AVRCPDisable( pMe );
          BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        case EVT_NA_DISABLED:
          pMe->mNA.bEnabled   = FALSE;
          pMe->mNA.bConnected = FALSE;
          BTApp_ClearBondable( pMe );
          BTApp_NADeInit( (IApplet*)pMe );
          if ( !pMe->mAG.bSelected || !pMe->mAG.bEnabled )
          {
            BTApp_BuildMenu( pMe, TOP_MENU );
          }
          break;
        case EVT_CLR_MSG_SCR:
          BTApp_HandleClearKey( pMe );
          break;
        case EVT_PIN_RQST:
          if ( TOP_MENU == BT_APP_MENU_SPP_RESULTS )
          {
            ISHELL_CancelTimer( pMe->m_pShell, 
                                (PFNNOTIFY) BTApp_BuildSppTestResults, 
                                pMe );
            ISHELL_CancelTimer( pMe->m_pShell, 
                                (PFNNOTIFY) BTApp_BuildL2ResultsMenu,
                                pMe );
          }
#ifdef FEATURE_BT_2_1
          //pMe->mRM.bBonding = FALSE;
#endif // FEATURE_BT_2_1 

		  ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
		  
          BTApp_BuildMenu( pMe, BT_APP_MENU_PASSKEY );
		  
          break;
#ifdef FEATURE_BT_2_1
        case EVT_PASSKY:
          if ( pMe->bBusyIconUp == TRUE)
          {
            (void)POP_MENU();
          }
         
          BTApp_BuildMenu( pMe, BT_APP_MENU_DISPLAY_PASSKEY );
          break;
#endif // FEATURE_BT_2_1 
        case EVT_DEV_BROWSE_RESP:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                     
#ifdef FEATURE_IOBEX
          if ( TOP_MENU == BT_APP_MENU_OBEX_CLIENT )
          {
            BTApp_OBEXHandleUserEvents( pMe, (uint32)wParam );
          }
          else
#endif //FEATURE_IOBEX
          {
          	
            BTApp_BuildMenu( pMe, BT_APP_MENU_BROWSE_SVC_RESP );
          }
          break;
        case EVT_DEV_SEARCH_RESP:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          if( pMe->mL2.bCliConnecting == TRUE &&
              pMe->mL2.bDoSDP == TRUE )
          {
            BTApp_ProcessL2Notifications( pMe, EVT_DEV_SEARCH_RESP, NULL );
          }
          else
          {
          	
            BTApp_BuildMenu( pMe, BT_APP_MENU_SEARCH_SVC_RESP );
          }
          break;
        case EVT_SPP_OPENED:
          pMe->mSPP.bDoVerify       = TRUE;
          
          BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
          break;
        case EVT_SPP_OPEN_FAILED:
          BTApp_ShowMessage( pMe, IDS_MSG_SPP_OPEN_FAILED, NULL, 0 );
          break;
        case EVT_SPP_CONNECTED:
          pMe->mSPP.uTotalBytesRcvd = 0;
          pMe->mSPP.uTotalBytesSent = 0;
          
          BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
          break;
        case EVT_SPP_CLOSED:
          pMe->mSPP.bDoVerify       = TRUE;
          BTApp_ClearBondable( pMe );
          if ( pMe->mSPP.bClient == FALSE )
          {
            BTApp_ClearDiscoverable( pMe );
          }
          else
          {
            BTApp_BuildTopMenu( pMe );
          }
          break;
        case EVT_SPP_DISCONNECTED:
          BTApp_EndSPPTest( pMe );
          break;
        case EVT_RM_BONDED:
          if ( pMe->mRM.bBonding )
          {
            pMe->mRM.bBonding = FALSE;
          }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
//#ifndef FEATURE_BT_2_1
          if ( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].majorDevClass ==
               AEEBT_COD_MAJ_DEV_CLS_AUDIO )
          {
            if ( AEEBT_BD_ADDRS_EQUAL( 
                   &pMe->mAG.bdAddr,
                   &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr ) == FALSE )
            {
            	
              BTApp_BuildMenu( pMe, BT_APP_MENU_USE_AUDIO_DEV );
            }
            else
            {
              BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, 
                                &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr,
                                2 );
            }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          }
          else
          {
            BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr,
                              2 );
          }
//#else
//          BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, NULL,2 );
//#endif // FEATURE_BT_2_1 
          break;
        case EVT_RM_DEV_SEC_SET:
          if ( TOP_MENU == BT_APP_MENU_DEV_SECURITY )
          {
            IBTEXTRM_DeviceRead( pMe->mRM.po,
                                 &pMe->mRM.device[ pMe->mRM.uCurDevIdx ] );
            BTApp_BuildTopMenu( pMe );
          }
          break;
        case EVT_RM_DEV_SEC_FAILED:
          if ( TOP_MENU == BT_APP_MENU_DEV_SECURITY )
          {
            BTApp_ShowDevMsg( pMe, IDS_MSG_DEV_SEC_FAILED, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr,
                              2 );
            BTApp_BuildTopMenu( pMe );
          }
          break;
        case EVT_RM_AUTH_REQ:
        
          BTApp_BuildMenu( pMe, BT_APP_MENU_AUTHORIZE_CONN );
          break;
        case EVT_SD_DISC_SET:
        {
          if ( dwParam == AEEBT_SD_ERR_NONE )
          {
            IBTEXTSD_GetDiscoverableMode( pMe->mSD.po );
          }
          else
          {
            msgID = pMe->mSD.bDiscoverable ? IDS_MSG_DISCOVERABLE_NOT_CLEARED : 
                      IDS_MSG_DISCOVERABLE_NOT_SET;
            BTApp_ShowMessage( pMe, msgID, NULL, 2 );
          }
          break;
        }
        case EVT_SD_DISC_RSP:
        {
          pMe->mSD.bDiscoverable = (dwParam == AEEBT_DISCOVERABLE_MODE_NONE) ? 
                                   FALSE : TRUE;
		  MSG_FATAL("BTApp_BuildMenu....................9",0,0,0);
		  if(pMe->bStartBTaplication)
		  {
		  	BTApp_BuildMenu( pMe, TOP_MENU );		 
		  }
          break;
        }
        case EVT_BIP_PUT_IMAGE:
        {
#ifdef FEATURE_BT_EXTPF_BIP
          if ( !BDADDR_VALID( &pMe->mBIP.printerBDAddr ) )
          {
            MSG_FATAL( "BIP Printing - No address selected for print", 0, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, NULL, 3 );
            return FALSE;
          }
          // initialize BIP if not
          if ( BTApp_BIPInit( pMe ) != FALSE )
          {
            return BTApp_BIPRemotePrintImage( pMe, dwParam );
          }
          else
          {
            MSG_FATAL( "BIP Printing - failed to create BIP object", 0, 0, 0 );
            BTApp_BIPCleanup( pMe );
            return FALSE;
          }
#else
          MSG_FATAL( "HandleEvent: No support of BIP for printing", 0, 0, 0 );
          return FALSE;
#endif //FEATURE_BT_EXTPF_BIP
        }
        case EVT_BPP_SIMPLE_PUSH:
        {
#ifdef FEATURE_BT_EXTPF_BPP

          if ( !BDADDR_VALID( &pMe->mBPP.printerBDAddr ) )
          {
			
            MSG_FATAL( "BPP Printing - No address selected for print", 0, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, NULL, 3 );
            return FALSE;
          }
          // initialize BPP if not
          if ( BTApp_BPPInit( pMe ) != FALSE )
          {
            return BTApp_BPPRemotePrintReq( pMe, dwParam );
          }
          else
          {
			
            MSG_FATAL( "BPP Printing - failed to create BPP object", 0, 0, 0 );
            BTApp_BPPCleanup( pMe );
            return FALSE;
          }
#else
          MSG_FATAL( "HandleEvent: No support of BPP for printing", 0, 0, 0 );
          return FALSE;
#endif //FEATURE_BT_EXTPF_BPP
        }
#ifdef FEATURE_BT_EXTPF_OPP
        case EVT_OPP:
          BTApp_OPPHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
        case EVT_FTP:
          BTApp_FTPHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
        case EVT_BIP:
          BTApp_BIPHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
        case EVT_BPP:
          BTApp_BPPHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
        case EVT_PBAP:
          BTApp_PBAPHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_IOBEX
        case EVT_OBEX:
          BTApp_OBEXHandleUserEvents( pMe, dwParam );
          break;
#endif
#ifdef FEATURE_BT_EXTPF_CTP
        case EVT_CTP:
          BTApp_CTPHandleUserEvents( pMe, dwParam );
          break;
        case EVT_ICP:
          BTApp_ICPHandleUserEvents( pMe, dwParam );
          break;
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION    
      }
      break;
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    default:
    {
      //MSG_FATAL( "HndlEv - ev=0x%x wP=0x%x dw=0x%x", eCode, wParam, dwParam );
      event_processed = FALSE;
      break;
    }
  }

  return event_processed;
}

*/

/* ==========================================================================
FUNCTION BTApp_AutoAnswer
DESCRIPTION
============================================================================= */
static boolean BTApp_AutoAnswer( CBTApp* pMe )
{
  BTAppCallType callConn = BTApp_CallConnected( pMe );
  BTAppCallType callPresent = BTApp_CallPresent( pMe );

  MSG_FATAL("***zzg BTApp_AutoAnswer callPresent=%x, callConn=%x***", callPresent, callConn, 0);

  MSG_FATAL( "AutoAnswer timer expired, cp=%x cc=%x", callPresent, callConn, 0 );

  if ( callConn != BT_APP_CALL_NONE )
  {
    if ( pMe->mAG.bConnected != FALSE )
    {
      BTApp_ConnectAudio( pMe, TRUE );
    }
  }
  else if ( callPresent != BT_APP_CALL_NONE )
  {
    pMe->mAG.bDevPickedUp = TRUE;
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_CheckAutoAnswer
DESCRIPTION
============================================================================= */
static void BTApp_CheckAutoAnswer( CBTApp* pMe )
{
  nv_item_type nvi;

  if ( (OEMNV_Get( NV_AUTO_ANSWER_I, &nvi ) == NV_DONE_S) &&
       (nvi.auto_answer.enable != FALSE) && (pMe->mAG.uNumCalls == 0) )
  {
    ISHELL_SetTimer( pMe->m_pShell, BTAPP_AUTOANSWER_TIME, 
                     (PFNNOTIFY) BTApp_AutoAnswer, (void*) pMe );
  }
}

/* ==========================================================================
FUNCTION BTApp_SendRing
DESCRIPTION
============================================================================= */
static void BTApp_SendRing( CBTApp* pMe )
{
	MSG_FATAL("***zzg BTApp_SendRing***", 0, 0, 0);
  if ( (BTApp_CallIncoming( pMe ) != BT_APP_CALL_NONE) && 
       (IS_AG_CONNECTABLE() != FALSE) )
  {
    // when AVS sends in-band ring tone, it does not command 
    // BT driver to send RINGs to BT HS

	MSG_FATAL("***zzg BTApp_SendRing IBTEXTAG_Ring***", 0, 0, 0);
	
    IBTEXTAG_Ring( pMe->mAG.po, 1, 0 );
    ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*2, 
                     (PFNNOTIFY) BTApp_SendRing, pMe );
  } 
}

/* ==========================================================================
FUNCTION BTApp_StopRing
DESCRIPTION
============================================================================= */
static void BTApp_StopRing( CBTApp* pMe )
{
  ISHELL_CancelTimer( pMe->m_pShell, 
                      (PFNNOTIFY) BTApp_AutoAnswer, pMe );

  if ( ISHELL_GetTimerExpiration( pMe->m_pShell, (PFNNOTIFY) BTApp_SendRing,
                                  pMe ) != 0)
  {
    ISHELL_CancelTimer( pMe->m_pShell, 
                        (PFNNOTIFY) BTApp_SendRing, pMe );
    IBTEXTAG_Ring( pMe->mAG.po, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_EndCall
DESCRIPTION
============================================================================= */
static void BTApp_EndCall( CBTApp* pMe, BTAppCallType callType )
{
  MSG_FATAL( "EndCall - c=%d ac=%d", callType, pMe->mAG.bAudioConnected, 0 );
#ifdef FEATURE_AVS_BT_SCO_REWORK
  if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B ) )
  {
    /* clear the flag, since no longer need to bring
    ** up SCO for calls when media playback is over.
    */
    BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B );
    /* SCO may be up and used for media playbacks, so leave it as it is */
  }
  else /* if SCO is NOT used for media playbacks, safe to be disconnected */
#endif /* FEATURE_AVS_BT_SCO_REWORK */
  {
    if ( BTApp_DisconnectAudio( pMe, FALSE ) == FALSE )
    {
      pMe->mAG.callState = BTAPP_AG_CALL_HANGUP;
    }
    else
    {
      pMe->mAG.callState = BTAPP_AG_CALL_NONE;
    }
  }
  
  if ( pMe->mAG.callState == BTAPP_AG_CALL_HANGUP )
  {
    // wait for SCO to settle
  }
  else if ( callType == BT_APP_CALL_VOICE )
  {
    AEETCalls*           pCallsDesc = NULL;
    ICall*               pCall      = NULL;
    uint8                uSizeOfCallsDesc;
    uint8                index;

    uSizeOfCallsDesc = sizeof(AEETCalls) +
                         (sizeof(AEECallDesc)*(BTAPP_MAX_NUM_CALLS - 1));
    pCallsDesc = (AEETCalls*)MALLOC( uSizeOfCallsDesc );

    if ( pCallsDesc == NULL )
    {
      MSG_FATAL( "EndCall - Memory allocation failed", 0, 0, 0 );
      return;
    }

    if ( ITELEPHONE_GetCalls( pMe->pIPhone, pCallsDesc, 
                              uSizeOfCallsDesc ) == SUCCESS )
    {
      /* To end only voice calls */
      for ( index = 0; index < pCallsDesc->dwCount; index++ )
      {
        /* get call type */
        if ( (ICALLMGR_GetCall( pMe->mAG.pICallMgr, pCallsDesc->calls[index], 
                                &pCall ) == SUCCESS) &&
             (AEET_CALL_TYPE_VOICE == ICALL_GetType( pCall )) )
        {
          ICALL_End( pCall );
        }        

        if (pCall != NULL)
        {
          ICALL_Release(pCall);
          pCall = NULL;
        }
        else
        {
          MSG_FATAL( "EndCall - failed to get call info", 0, 0, 0 );
        }
      }
    }
    else
    {
      MSG_FATAL( "EndCall - failed to get calls", 0, 0, 0 );
    }
    
    FREEIF( pCallsDesc );
  }
#ifdef FEATURE_BT_VT
  else if ( callType == BT_APP_CALL_VT )
  {
    if ( CVideoPhone_EndVideoCall( pMe->m_pShell ) == FALSE )
    {
      MSG_FATAL( "EndCall - failed to end video call", 0, 0, 0 );
    }
  }
#endif //FEATURE_BT_VT
  else
  {
    MSG_FATAL( "EndCall - unexpected call type", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_Originate
DESCRIPTION
  Used by BTApp to originate a call. Replaces calls to ICALLMGR_Originate,
  and will do the proper thing (send URL, etc) to dial a number.
  Returns TRUE if the call has been placed.
============================================================================= */
static boolean BTApp_Originate( CBTApp* pMe, char* szDialString )
{
  static const char szTelURI[] = "tel:Call?Number=";
  static const int  dwStrLen = AEEBT_MAX_PHONE_NUM_DIGITS + sizeof( szTelURI ) + 1;
  char   szURI[ dwStrLen ];
  boolean result = FALSE;

  std_strlcpy( szURI, szTelURI, dwStrLen );
  std_strlcat( szURI, szDialString, dwStrLen );

  if ( ISHELL_SendURL( pMe->m_pShell, szURI ) == TRUE )
  {
    DBGPRINTF_FATAL("BTAPP Originate: Sent URL %s", szURI );
    result = TRUE;
  }
  else
  {
    MSG_FATAL("BTAPP Originate: URL failed, originating via ICALLMGR", 0,0,0);
    result = (ICALLMGR_Originate( pMe->mAG.pICallMgr, AEET_CALL_TYPE_VOICE, 
                                  szDialString, NULL, &pMe->mAG.pOutgoingCall,
                                  NULL )
              == SUCCESS);
  }
  return result;
}

/* ==========================================================================
FUNCTION BTApp_Dial
DESCRIPTION
============================================================================= */
static void BTApp_Dial( CBTApp* pMe )
{
  char szString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ] = "1234";
  
  MSG_FATAL( "Dial - ad=%d as=%d", 
           pMe->mAG.bAudioConnected, pMe->mAG.bAudioSelected, 0 );

  if ( WSTRLEN( pMe->mAG.wDialString ) == 0 )
  {
    MSG_FATAL( "Dial - using 1234", 0, 0, 0 );
  }
  else
  {
    WSTRTOSTR( pMe->mAG.wDialString, szString, sizeof( szString ) );
    MSG_FATAL( "Dial - using number stored in phone", 0, 0, 0 );
  }

#ifdef FEATURE_BT_VT
  if ( pMe->mAG.bVideoCallEnabled != FALSE )
  {
    AECHAR wString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
    STRTOWSTR( szString, wString, sizeof( wString ) );
    pMe->mAG.bDevDialed = CVideoPhone_OriginateVideoCall( pMe->m_pShell, 
                                                          wString );
  }
  else
#endif //FEATURE_BT_VT
  {
    pMe->mAG.bDevDialed = BTApp_Originate( pMe, szString );
  }
  if ( pMe->mAG.bDevDialed != FALSE )
  {
    if ( pMe->mAG.bUpdateVrState != FALSE )
    {
      pMe->mAG.bUpdateVrState = FALSE;
      pMe->mAG.bStartedVr = FALSE;
      IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
    }
    BTApp_ConnectAudio( pMe, TRUE );
  }
  else
  {
    MSG_FATAL( "Dial - failed to originate voice call", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_AnswerCall
DESCRIPTION
============================================================================= */
static void BTApp_AnswerCall( CBTApp* pMe, BTAppCallType callIncoming )
{
  boolean bAnswered = FALSE;
  static const char szAnswer[] = "tel:AnswerCall";

  MSG_FATAL( "AnswerCall - dev=%d c=%d st=%d", 
           pMe->mAG.devType, callIncoming, pMe->mAG.callState );

  MSG_FATAL("***zzg BTApp_AnswerCall***", 0, 0, 0);

  pMe->mAG.bDevPickedUp = TRUE;
  BTApp_StopRing( pMe );

  if ( callIncoming == BT_APP_CALL_NONE )
  {
    MSG_FATAL( "AnswerCall - no incoming call", 0, 0, 0 );
  }
  else if ( BTApp_ConnectAudio( pMe, TRUE ) != FALSE )
  {
    if ( callIncoming == BT_APP_CALL_VOICE )
    {
      if ( pMe->mAG.pIncomingCall != NULL )
      {
        if ( ISHELL_SendURL( pMe->m_pShell, szAnswer ) == TRUE )
        {
          DBGPRINTF_FATAL("BTAPP: Ansering call by sending URL %s", szAnswer );
          bAnswered = TRUE;
        }
        else if ( ICALL_Answer( pMe->mAG.pIncomingCall ) == SUCCESS )
        {
          MSG_FATAL("BTApp: URL answer failed. Used ICALL_Answer",0,0,0);
          bAnswered = TRUE;
        }
        pMe->mAG.pIncomingCall = NULL;
      }
      else
      {
        bAnswered = FALSE;
      }
    }
#ifdef FEATURE_BT_VT
    else if ( callIncoming == BT_APP_CALL_VT )
    {
      bAnswered = CVideoPhone_AnswerVideoCall( pMe->m_pShell );
    }
#endif //FEATURE_BT_VT
    if ( bAnswered == FALSE )
    {
      MSG_FATAL( "AnswerCall - failed", 0, 0, 0 );
    }
  }
}

#ifdef FEATURE_BT_EXTPF_AV
/* ==========================================================================
FUNCTION BTApp_PickAudioLink
DESCRIPTION
  Determines which between A2DP streaming and AG audio should be allowed.
  Returns TRUE if AG audio has priority.
============================================================================= */
static boolean BTApp_PickAudioLink( CBTApp* pMe )
{
  boolean bAGWins = FALSE; // assume A2DP will be selected
  
  if ( BTApp_CallPresent( pMe ) != BT_APP_CALL_NONE )
  {
    // most likely the case where user plays a Media file while a voice
    // call is up, then HS initiates audio transfer from AG

#if defined( FEATURE_UIONE_HDK )
    MSG_FATAL( "PickAudioLink - on UIOne MP is in background", 0, 0, 0 );
    bAGWins = TRUE; // allow SCO
#elif !defined( FEATURE_APP_DIALER )
    MSG_FATAL( "PickAudioLink - can't suspend MP ", 0, 0, 0 );
#else
    // must suspend MediaPlayer so AVS would switch to play voice audio
    MSG_FATAL( "PickAudioLink - launching DialerApp", 0, 0, 0 );
    bAGWins = TRUE; // allow SCO
#endif //FEATURE_APP_DIALER
  }
  if ( bAGWins == FALSE )
  {
    BTApp_DisconnectAudio( pMe, TRUE );
  }
  return bAGWins;
}
#endif //FEATURE_BT_EXTPF_AV


/* ==========================================================================
FUNCTION BTApp_HandleEventButtonPressed
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventButtonPressed( CBTApp* pMe )
{
  BTAppCallType callConn = BTApp_CallConnected( pMe );
  BTAppCallType callPresent = BTApp_CallPresent( pMe );
  BTAppCallType callIncoming = BTApp_CallIncoming( pMe );

  MSG_HIGH( "ButtonPressed - ci=%x cc=%x cp=%x",
            callIncoming, callConn, callPresent );

  if ( pMe->mAG.bIgnoreButton != FALSE )
  {
    MSG_FATAL( "ButtonPressed - ignored", 0, 0, 0 );
    return; // ignore this button pressed event
  }

  if ( callConn != BT_APP_CALL_NONE )
  {
    if ( pMe->mAG.bAudioConnected )
    {
      MSG_FATAL( "ButtonPressed - HS hanging up call", 0, 0, 0 );
      BTApp_EndCall( pMe, callPresent );
    }
    else // remote device initiates voice transfer
    {
      boolean bUseAG = TRUE;
      MSG_FATAL( "ButtonPressed - HS transfers audio to HS", 0, 0, 0 );
#ifdef FEATURE_BT_EXTPF_AV
      if ( pMe->mA2DP.bStreaming != FALSE )
      {
        bUseAG = BTApp_PickAudioLink( pMe );
      }
#endif
      if ( bUseAG != FALSE )
      {
        BTApp_ConnectAudio( pMe, TRUE );
      }
    }
  }
  else if ( callIncoming != BT_APP_CALL_NONE )
  {
    BTApp_AnswerCall( pMe, callIncoming );
  }
  else if ( callPresent != BT_APP_CALL_NONE )
  {
    MSG_FATAL( "ButtonPressed - HS cancelling MO call", 0, 0, 0 );
    BTApp_EndCall( pMe, callPresent );
  }
#if defined( FEATURE_PHONE_VR )
  else if ( pMe->mAG.bVREnabled != FALSE )
  {
    if ( pMe->mAG.bStartedVr == FALSE)
    {
      BTApp_StartVR( pMe );
    }
    else
    {
      BTApp_EndVR( pMe );
    }
  }
#endif /* FEATURE_PHONE_VR */
  else
  {
//Add By zzg 2013_01_17  
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_IC241A_MMX)
    BTApp_HandleEventDevRedial(pMe); 
#endif    
//Add End
    
    //BTApp_Dial( pMe );
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleEventDevPickUp
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventDevPickUp( CBTApp* pMe )
{
  BTAppCallType callPresent = BTApp_CallPresent( pMe );
  BTAppCallType callIncoming = BTApp_CallIncoming( pMe );

  MSG_HIGH( "AG - DevPickup cp=%x", callPresent, 0, 0 );

  if ( callPresent == BT_APP_CALL_NONE )
  {
    MSG_FATAL( "AG - DevPickup, no call present", 0, 0, 0 );
  }
  else if ( callIncoming != BT_APP_CALL_NONE )
  {
    BTApp_AnswerCall( pMe, callIncoming );
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleEventDevHangUp
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventDevHangUp( CBTApp* pMe )
{
  BTAppCallType callPresent = BTApp_CallPresent( pMe );

  MSG_HIGH( "AG - DevHangup c=%x i=%x", callPresent, BTApp_CallIncoming( pMe ), 0 );

  if ( callPresent == BT_APP_CALL_NONE )
  {
    MSG_FATAL( "AG - DevHangup, no call present", 0, 0, 0 );
  }
  else // a call is present
  {
    BTApp_StopRing( pMe );
    BTApp_EndCall( pMe, callPresent );
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleEventDevDial
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventDevDial( CBTApp* pMe )
{
  char   szString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  AECHAR wString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  uint8  numChars = AEEBT_MAX_PHONE_NUM_DIGITS + 1;

  MSG_FATAL( "DevDial - ac=%d as=%d", 
           pMe->mAG.bAudioConnected, pMe->mAG.bAudioSelected, 0 );

  if ( IBTEXTAG_GetDialedString( pMe->mAG.po, wString, &numChars, 
                                 FALSE ) == SUCCESS )
  {
    MSG_FATAL( "DevDial - %d digits", numChars, 0, 0 );
    WSTRTOSTR( wString, szString, sizeof( szString ) );

    pMe->mAG.bDevDialed = BTApp_Originate( pMe, szString );
    if ( pMe->mAG.bDevDialed == FALSE )
    {
      MSG_FATAL( "DevDial - failed to originate voice call", 
                 0, 0, 0 );
    }
    else
    {
      BTApp_ConnectAudio( pMe, TRUE );
    }
  }
  else
  {
    MSG_FATAL( "DevDial - no dial string", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleEventDevMemDial
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventDevMemDial( CBTApp* pMe )
{
  AECHAR wString[ AEEBT_MAX_MEM_DIAL_DIGITS + 1 ];
  char   szString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  AECHAR wString2[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ] = {L""};

  MSG_FATAL( "DevMemDial - ac=%d as=%d digits=%d",
           pMe->mAG.bAudioConnected,
           pMe->mAG.bAudioSelected,
           AEEBT_MAX_MEM_DIAL_DIGITS + 1 );

#ifdef FEATURE_BT_HFP_1_5
  pMe->mAG.bDevDialed = FALSE;
  STRTOWSTR( (char*)(pMe->mAG.parseData.rx_params[ 0 ]),
             wString, sizeof( wString ) );
#else
  uint8  numChars = AEEBT_MAX_MEM_DIAL_DIGITS + 1;
  if ( IBTEXTAG_GetDialedString( pMe->mAG.po, wString, &numChars, 
                                 TRUE ) == SUCCESS )
#endif
  {
    BTApp_DoSpeedDialLookup( pMe->m_pShell,
                             wString,
                             wString2,
                             sizeof(wString2) );

    if ( WSTRLEN( wString2 ) == 0 )
    {
      MSG_FATAL( "DevMemDial - no such entry in phone book", 0, 0, 0 );
    }
    else
    {
      WSTRTOSTR( wString2, szString, sizeof( szString ) );
      pMe->mAG.bDevDialed = BTApp_Originate( pMe, szString );
      if ( pMe->mAG.bDevDialed == FALSE )
      {
        MSG_FATAL( "DevMemDial - failed to originate voice call", 
                   0, 0, 0 );
      }
      else
      {
        BTApp_ConnectAudio( pMe, TRUE );
      }
    }
  }
#ifndef FEATURE_BT_HFP_1_5
  else
  {
    MSG_FATAL( "DevMemDial - no dial string", 0, 0, 0 );
  }
#endif
}

/* ==========================================================================
FUNCTION BTApp_HandleEventDevRedial
DESCRIPTION
============================================================================= */
static void BTApp_HandleEventDevRedial( CBTApp* pMe )
{
  ICallHistory*  pCallHist = NULL;
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;
  AECHAR      wString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  char        szString[ AEEBT_MAX_PHONE_NUM_DIGITS + 1 ];
  int         Status;
  int         i = 0;
  boolean     bFound = FALSE;

  MSG_FATAL( "DevRedial - ac=%d as=%d", 
           pMe->mAG.bAudioConnected, pMe->mAG.bAudioSelected, 0 );

  MSG_FATAL("***zzg BTApp_HandleEventDevRedial***", 0, 0, 0 );

#ifdef FEATURE_BT_HFP_1_5
  pMe->mAG.bDevDialed = FALSE; 
#endif
  if(ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_CALLHISTORY,
                                 (void**)&pCallHist ) != AEE_SUCCESS)
  {
     MSG_FATAL( "DevRedial - Call history instance failed", 0, 0, 0 );
  }
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_IC241A_MMX)
  else if(ICALLHISTORY_EnumInit(pCallHist) != SUCCESS)
  {
     MSG_FATAL( "DevRedial - EnumInitByCallType failed, "
                "no entry in dialed list", 0, 0, 0 );
  } 
#else
  else if(ICALLHISTORY_EnumInitByCallType(pCallHist,
                                 AEECALLHISTORY_CALL_TYPE_TO ) != AEE_SUCCESS)
  {
     MSG_FATAL( "DevRedial - EnumInitByCallType failed, "
                "no entry in dialed list", 0, 0, 0 );
  }
#endif  
  else
  {
    pCallHistoryEntry = ICALLHISTORY_EnumNext( pCallHist, &Status );

    MSG_FATAL("***zzg BTApp_HandleEventDevRedial Status=%x***", Status, 0, 0 );
    MSG_FATAL("***zzg BTApp_HandleEventDevRedial pCallHistoryEntry=%x***", pCallHistoryEntry, 0, 0 );
    
    if ( (Status == SUCCESS) && (pCallHistoryEntry != NULL) )
    {
      MEMSET( wString, 0, sizeof(wString) );
      pItems = pCallHistoryEntry->pFields;
      for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
      {
        if ( pItems[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX )
        {
          bFound = TRUE;

          MSG_FATAL("***zzg BTApp_HandleEventDevRedial AEECALLHISTORY_FIELD_NUMBER_EX***", 0, 0, 0 );
          
          MEMCPY( wString, pItems[i].pData, 
                  MIN(ARR_SIZE(wString)*sizeof(AECHAR), pItems[i].wDataLen) );
          break;
        }        
      }

      if (bFound == FALSE)
      {
          for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
          {
            if ( pItems[i].wID == AEECALLHISTORY_FIELD_NUMBER )
            {
              bFound = TRUE;

              MSG_FATAL("***zzg BTApp_HandleEventDevRedial AEECALLHISTORY_FIELD_NUMBER***", 0, 0, 0 );
              
              MEMCPY( wString, pItems[i].pData, 
                      MIN(ARR_SIZE(wString)*sizeof(AECHAR), pItems[i].wDataLen) );
              break;
            }        
           }
       }
      
      MSG_FATAL("***zzg BTApp_HandleEventDevRedial bFound=%x***", bFound, 0, 0 );
      
      if ( bFound != FALSE )
      {
        WSTRTOSTR( wString, szString, sizeof( szString ) );

        MSG_FATAL("***zzg BTApp_HandleEventDevRedial szString=%x***", szString, 0, 0 );
        
        pMe->mAG.bDevDialed = BTApp_Originate( pMe, szString );
        if ( pMe->mAG.bDevDialed == FALSE )
        {
          MSG_FATAL( "DevRedial - failed to originate voice call r=%d", 
                     Status, 0, 0 );
        }
      }
      if ( pMe->mAG.bDevDialed != FALSE )
      {
        BTApp_ConnectAudio( pMe, TRUE );
      }
    }
  }
  if (pCallHist != NULL)
  {
     ICALLHISTORY_Release( pCallHist );
  }
#ifdef FEATURE_BT_HFP_1_5
  SEND_RESP( ( pMe->mAG.bDevDialed == FALSE ) ? errorResp : okResp );
#endif
}

/* ==========================================================================
FUNCTION BTApp_InitTextDlg
DESCRIPTION
============================================================================= */
static boolean BTApp_InitTextDlg( CBTApp* pMe, IDialog* pDlg )
{
  IMenuCtl* pICurrentMenu;
  ITextCtl* pTextCtl;
  AEERect   rc;
  AECHAR*   pText;
  uint16    stringID;
  uint16    maxLen;
  uint32    prop;
  AEETextInputMode newIM;

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl( pDlg, IDC_SK_MENU );
  pTextCtl = (ITextCtl*)IDIALOG_GetControl( pDlg, IDC_TEXT_VALUE );

  if ( (pICurrentMenu == NULL) || (pTextCtl == NULL) )
  {
    MSG_FATAL( "InitTextDlg - NULL pointers", 0, 0, 0 );
    return FALSE;
  }

  switch ( TOP_MENU )
  {
    case BT_APP_MENU_EDIT_NAME:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_BT_NAME;
      maxLen    = AEEBT_MAX_DEVICENAME_LEN;
      pText     = pMe->mRM.myInfo.wName;
      break;
    }
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_EDIT_SHORT_NAME:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_BT_SHORT_NAME;
      maxLen    = AEEBT_MAX_DEVICENAME_LEN;
      pText     = pMe->mRM.myInfo.wShortName; 
      break;
    }
    case BT_APP_MENU_EDIT_MANU_DATA:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_BT_MANU_DATA;
      maxLen    = AEEBT_MAX_EIR_MANUF_DATA_LEN;
      pText     = pMe->mRM.wManuData; 
      break;
    }
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_PASSKEY:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASS_KEY;
      maxLen    = MAX_PASSKEY_LEN;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else 
      pText     = pMe->mRM.wPassKey;
      pText[ 0 ] = NULL; // don't remember previous pass key
#endif//FEATURE_APP_TEST_AUTOMATION

      break;
    }
    case BT_APP_MENU_DFLT_DIAL_STR:
    {
      prop      = TP_FRAME;
#if MIN_BREW_VERSION(3, 0)
      newIM     = AEE_TM_DIALED_DIGITS;
#else
      newIM     = OEM_TM_DIALED_DIGITS;
#endif
      stringID  = IDS_DFLT_DIAL_STR;
      maxLen    = AEEBT_MAX_PHONE_NUM_DIGITS;
      pText     = pMe->mAG.wDialString;
      break;
    }
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_PASSWORD:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASSWORD;
      maxLen    = AEEBT_MAX_PASSWORD_LEN;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else     
      pText     = pMe->mFTP.wPassWord;
#endif//FEATURE_APP_TEST_AUTOMATION
      break;
    }
    case BT_APP_MENU_FTP_CREATE_FLDR:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_FTP_CREATE_FOLDER;
      maxLen    = AEEBT_MAX_FILE_NAME;
      pText     = pMe->mFTP.wFolderName;
      break;
    }
    case BT_APP_MENU_FTP_MOVE_TO_FLDR:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_FTP_MOVE_TO_FOLDER;
      maxLen    = AEEBT_MAX_FILE_NAME;
      pText     = pMe->mFTP.wFolderName;
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_PIN:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASSWORD;
      maxLen    = AEEBT_MAX_PASSWORD_LEN;  
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else 
      pText     = pMe->mBIP.wPIN;
#endif//FEATURE_APP_TEST_AUTOMATION
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_PASSWORD:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASSWORD;
      maxLen    = AEEBT_MAX_PASSWORD_LEN;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else     
      pText     = pMe->mBPP.wPassword;
#endif//FEATURE_APP_TEST_AUTOMATION
      break;
    }
    case BT_APP_MENU_BPP_USERID:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_USERID;
      maxLen    = AEEBT_MAX_USERID_LEN;
      pText     = pMe->mBPP.wUserID;
      break;
    }    
    case BT_APP_MENU_BPP_JOB_ID:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_JOB_ID;
      maxLen    = sizeof( uint32 ); /* Job ID */
      pText     = pMe->mBPP.wJobId;
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case BT_APP_MENU_PBAP_FILTER:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PBAP_FILTER;
      maxLen    = MAX_FILTER_VAL_LEN;
      pText     = pMe->mPBAP.wFilterValue;
      break;
    }
    case BT_APP_MENU_PBAP_MAX_LIST_COUNT:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PBAP_MAX_LIST_COUNT;
      maxLen    = MAX_LIST_COUNT_VAL_LEN;
      pText     = pMe->mPBAP.wMaxListCount;
      break;
    }
    case BT_APP_MENU_PBAP_LIST_START_OFFSET:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PBAP_LIST_START_OFFSET;
      maxLen    = MAX_LIST_COUNT_VAL_LEN;
      pText     = pMe->mPBAP.wListStartOffset;
      break;
    }
    case BT_APP_MENU_PBAP_SEARCH_VALUE:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_PBAP_SEARCH_VALUE;
      maxLen    = MAX_PASSKEY_LEN;
      pText     = pMe->mPBAP.wSearchValue;
      break;
    }
    case BT_APP_MENU_PBAP_USERID:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_USERID;
      maxLen    = AEEBT_MAX_USERID_LEN;
      pText     = pMe->mPBAP.wUserID;
      break;
    }    
    case BT_APP_MENU_PBAP_PASSWORD:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASSWORD;
      maxLen    = AEEBT_MAX_PASSWORD_LEN;
      pText     = pMe->mPBAP.wPassword;
      break;
    }    
#endif
#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_PASSWORD:
    {
      prop      = TP_FRAME | TP_PASSWORD;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_PASSWORD;
      maxLen    = AEEBT_MAX_PASSWORD_LEN;
      pText     = pMe->mOBEX.wPassword;
      break;
    }
    case BT_APP_MENU_OBEX_USERID:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_LETTERS;
      stringID  = IDS_USERID;
      maxLen    = AEEBT_MAX_USERID_LEN;
      pText     = pMe->mOBEX.wUserID;
      break;
    }    
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    case BT_APP_MENU_A2DP_CONTROL_DATA:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = IDS_A2DP_CONTROL_DATA;
      maxLen    = MAX_CONTROL_DATA_LEN; /* control data */
      pText     = pMe->mA2DP.wControlData;
      break;
    }
#endif
    case BT_APP_MENU_L2_TEST:
    {
      prop      = TP_FRAME;
      newIM     = AEE_TM_NUMBERS;
      stringID  = pMe->mL2.uDialogType;
      maxLen    = MAX_CONTROL_DATA_LEN; /* control data */
      pText     = pMe->mL2.wControlData;      
      break;
    }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    default:
      MSG_FATAL( "InitTextDlg - unexp menu %x", TOP_MENU, 0, 0 );
      return FALSE;
  }

  // set the title

  /*
  if ( stringID == IDS_PASS_KEY )
  {
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, stringID, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );
    if ( WSTRLEN( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName ) == 0 )
    {
      BTApp_BDAddr2Wstr( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName,
                         &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
    }
    // build the title
    WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof( AECHAR ), 
              pMe->pText2, pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName );
    ITEXTCTL_SetTitle( pTextCtl, AEE_APPSBTAPP_RES_FILE, 0, pMe->pText1 );
  }
  else
  {
    ITEXTCTL_SetTitle( pTextCtl, AEE_APPSBTAPP_RES_FILE, stringID, NULL );
  }
  */

	//Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	    //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
	}  

	
	if ( stringID == IDS_PASS_KEY )
    {
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, stringID, 
                            pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );
      if ( WSTRLEN( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName ) == 0 )
      {
        BTApp_BDAddr2Wstr( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName,
                           &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
      }
      // build the title
      WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof( AECHAR ), 
                pMe->pText2, pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName );
	  
      //ITEXTCTL_SetTitle( pTextCtl, AEE_APPSBTAPP_RES_FILE, 0, pMe->pText1 );
	  
	  if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText1);
      }
    }
    else	
    {
      AECHAR wTitle[40];
      //ITEXTCTL_SetTitle( pTextCtl, AEE_APPSBTAPP_RES_FILE, stringID, NULL );
	  ISHELL_LoadResString(pMe->m_pShell,
	                           AEE_APPSBTAPP_RES_FILE,                                
	                           stringID,
	                           wTitle,
	                           sizeof(wTitle));

      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTitle);
      }
    }  	
	//Add End

  ITEXTCTL_SetSoftKeyMenu( pTextCtl, NULL );
  //SetDefaultSoftkeyLook( pMe->m_pShell, pICurrentMenu );  //Del By zzg 2010_11_01

  IMENUCTL_DeleteAll( pICurrentMenu );       	
  
  //IMENUCTL_AddItem(pICurrentMenu,  APPSCOMMON_RES_FILE, IDS_OK, IDS_OK, NULL, 0); //AEE_APPSBTAPP_RES_FILE

  IMENUCTL_AddItem(pICurrentMenu,  AEE_APPSBTAPP_RES_FILE, IDS_BT_YES, IDS_BT_YES, NULL, 0); //AEE_APPSBTAPP_RES_FILE
  IMENUCTL_AddItem(pICurrentMenu,  AEE_APPSBTAPP_RES_FILE, IDS_CAN, IDS_CAN, NULL, 0); //AEE_APPSBTAPP_RES_FILE

  //SETAEERECT ( &rc, 10, 10, pMe->m_rect.dx-10, 30 );
  //Modify by zzg 2010_11_01
  SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy - BTBAR_HEIGHT);
  
  ITEXTCTL_SetRect( pTextCtl, &rc );

  ITEXTCTL_SetMaxSize( pTextCtl, maxLen );
  ITEXTCTL_SetProperties( pTextCtl, prop );
  ITEXTCTL_SetText( pTextCtl, pText, WSTRLEN( pText ) );
  ITEXTCTL_SetCursorPos( pTextCtl, TC_CURSOREND );

  ITEXTCTL_SetInputMode( pTextCtl, newIM );

  ITEXTCTL_Redraw( pTextCtl );
  ITEXTCTL_SetActive( pTextCtl, TRUE );

// Send AVK_SELECT to dismiss the Pin Menu
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

  return TRUE;
}

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_TextEditSave
DESCRIPTION
============================================================================= */
static boolean BTApp_TextEditSave( CBTApp* pMe )
{
#ifdef FEATURE_BT_EXTPF_BPP
  /* Needed to BPP Text Control */
  char sBPPJobId [ sizeof( uint32 ) + 1 ];
#endif
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  char sControlData [ MAX_CONTROL_DATA_LEN + 1 ];
#endif
#if defined(FEATURE_BT_EXTPF_PBAP) || defined(FEATURE_BT_EXTPF_BPP)
  int  result = EFAILED;
#endif
#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
  char passKey[AEEBT_SSP_PASSKEY_LEN];

#endif /* FEATURE_BT_2_1 */

  switch ( TOP_MENU )
  {
    case BT_APP_MENU_EDIT_NAME:
    {
      if ( WSTRCMP( pMe->mRM.myInfo.wName, pMe->pText2 ) != 0 )
      {
        WSTRLCPY( pMe->mRM.myInfo.wName, pMe->pText2,
                  ARR_SIZE(pMe->mRM.myInfo.wName) );
        if ( IBTEXTRM_SetName( pMe->mRM.po, pMe->mRM.myInfo.wName ) != SUCCESS )
        {
          MSG_FATAL( "TextEditSave - SetName failed", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        }
        else
        {
          BTApp_ShowBusyIcon( pMe );
        }
      }
      break;
    }
#ifdef FEATURE_BT_2_1
    case  BT_APP_MENU_EDIT_SHORT_NAME:
    {
      if ( WSTRCMP( pMe->mRM.myInfo.wShortName, pMe->pText2 ) != 0 )
      {
        WSTRLCPY( pMe->mRM.myInfo.wShortName, pMe->pText2, 
                  ARR_SIZE( pMe->mRM.myInfo.wShortName ) );
        if( ( WSTRLEN( pMe->mRM.myInfo.wShortName ) > 
              AEEBT_MAX_NICKNAME_LEN ) ||
             IBTEXTRM_SetNickName( pMe->mRM.po, NULL, 
                                   pMe->mRM.myInfo.wShortName ) != SUCCESS )
        {
          MSG_FATAL( "TextEditSave - SetShortName failed", 0, 0, 0 );
        }
        else
        {
          BTApp_ShowBusyIcon( pMe );
        }
      }
      break;
    } 
    case  BT_APP_MENU_EDIT_MANU_DATA:
    {
      if ( WSTRCMP( pMe->mRM.wManuData, pMe->pText2 ) != 0 )
      {
        char szText[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ];
        WSTRLCPY( pMe->mRM.wManuData, pMe->pText2, 
                  ARR_SIZE( pMe->mRM.wManuData ) );
        WSTRTOSTR( pMe->mRM.wManuData, szText, sizeof(szText) );
        if ( ( IBTEXTRM_SetEIRManufData( pMe->mRM.po,(uint8*)szText, 
                                         STRLEN( szText ) ) != SUCCESS ) )
        {
          MSG_FATAL( "TextEditSave- SetManuData failed", 0, 0, 0 );
        }
        else
        {
          BTApp_ShowBusyIcon( pMe );
        } 
      }
      break;
    }
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_PASSKEY:
    {
      AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
      WSTRLCPY( pMe->mRM.wPassKey, pMe->pText2, ARR_SIZE(pMe->mRM.wPassKey) );
#ifdef FEATURE_BT_2_1
      DBGPRINTF_FATAL ( "TextEditHndlSave-BT_APP_MENU_PASSKEY with "
                        "pMe->pText2=%s ",pMe->pText2 ); 
      MSG_FATAL( "TextEditHndlSave, pMe->mRM.bBonding=%d ", 
               pMe->mRM.bBonding,0,0); 
      MSG_FATAL( "TextEditHndlSave, pMe->mRM.ioCaptype=%d ", 
               pMe->mRM.ioCaptype,0,0); 
#endif /* FEATURE_BT_2_1 */
      if ( pMe->mRM.bBonding )
      {
#ifdef FEATURE_BT_2_1
        if( (pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) &&
            (pMe->mRM.bPassKey == TRUE) )
        {
          if ( IBTEXTRM_KeypressNotification( pMe->mRM.po, &pDev->bdAddr, 
                 AEEBT_RM_KPN_COMPLETED ) != SUCCESS )
          {
            MSG_HIGH( "KeyPress Notification- Failed", 0, 0, 0 );
            return FALSE;
          }
          WSTRTOSTR ( pMe->mRM.wPassKey, passKey, 
                      sizeof(char)*AEEBT_SSP_PASSKEY_LEN );

          if ( STRLEN( passKey ) < ( AEEBT_SSP_PASSKEY_LEN - 1 ) ) 
          {
            BTApp_PadString(passKey, sizeof(passKey));
          }

          DBGPRINTF_FATAL ( "TextEditHndlSave-BT_APP_MENU_PASSKEY with "
                            "passKey=%s ",passKey ); 
          if ( IBTEXTRM_PasskeyReply( pMe->mRM.po, &pDev->bdAddr, 
                                      passKey ) != SUCCESS )
          {
            MSG_HIGH( "PassKey Reply- Failed", 0, 0, 0 );
            return FALSE;
          }
          BTApp_ShowBusyIcon( pMe );
        }
        else
#endif /* FEATURE_BT_2_1 */
        {
          if ( IBTEXTRM_Bond( pMe->mRM.po, &pDev->bdAddr, 
                              pMe->mRM.wPassKey ) != SUCCESS )
          {
            MSG_FATAL( "TextEditSave - Bond failed", 0, 0, 0 );
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
        }
      }
      else
      {
#ifdef FEATURE_BT_2_1
        if( (pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) && 
            (pMe->mRM.bPassKey == TRUE) )
        {
          MSG_HIGH( "Sending IBTEXTRM_KeypressNotification", 0, 0, 0 );
          if ( IBTEXTRM_KeypressNotification( pMe->mRM.po, &pDev->bdAddr, 
                 AEEBT_RM_KPN_COMPLETED ) != SUCCESS )
          {
            MSG_HIGH( "KeyPress Notification- Failed", 0, 0, 0 );
            return FALSE;
          }
          DBGPRINTF_FATAL ( "Sending IBTEXTRM_PasskeyReply with "
                            "pMe->mRM.wPassKey=%s ", pMe->mRM.wPassKey ); 
          WSTRTOSTR ( pMe->mRM.wPassKey, passKey, 
                      sizeof(char)*AEEBT_SSP_PASSKEY_LEN );
          DBGPRINTF_FATAL ( "Sending IBTEXTRM_PasskeyReply with "
                            "Passkey=%s ", passKey ); //was DBGPRINTF
          MSG_FATAL ( "BTApp_TextEditSave IBTEXTRM_PasskeyReply with BDAddr "
                    "- %2x%2x%2x", pDev->bdAddr.uAddr[0], pDev->bdAddr.uAddr[1], 
                    pDev->bdAddr.uAddr[2] );

          if ( STRLEN( passKey ) < (AEEBT_SSP_PASSKEY_LEN - 1) ) 
          {
            BTApp_PadString(passKey, sizeof(passKey));
          }

          if ( IBTEXTRM_PasskeyReply( pMe->mRM.po, &pDev->bdAddr, passKey ) 
                                      != SUCCESS )
          {
            MSG_HIGH( "PassKey Reply- Failed", 0, 0, 0 );
            return FALSE;
          }
          BTApp_ShowBusyIcon( pMe );
        }
        else
#endif /* FEATURE_BT_2_1 */
        {
          if ( IBTEXTRM_PinReply( pMe->mRM.po, &pDev->bdAddr, 
                                  pMe->mRM.wPassKey ) != SUCCESS )
          {
            MSG_FATAL( "TextEditSave - PinReply failed", 0, 0, 0 );
          }
          else if ( WSTRLEN( pMe->mRM.wPassKey ) > 0 )
          {
            pMe->mRM.bBonding = TRUE ;
            BTApp_ShowBusyIcon( pMe );
          }
        }
      }
      break;
    }
    case BT_APP_MENU_DFLT_DIAL_STR:
    {
      if ( WSTRCMP( pMe->mAG.wDialString, pMe->pText2 ) != 0 )
      {
        MSG_FATAL( "TextEditSave - dial string len=%d",
                 WSTRLEN( pMe->pText2 ), 0, 0 );
        WSTRLCPY( pMe->mAG.wDialString, pMe->pText2, 
                  ARR_SIZE( pMe->mAG.wDialString ) );
        pMe->bConfigChanged = TRUE;
      }
      break;
    }
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_PASSWORD:
    {
      WSTRLCPY( pMe->mFTP.wPassWord, pMe->pText2, 
                ARR_SIZE(pMe->mFTP.wPassWord) );
      if ( IBTEXTFTP_Authenticate( pMe->mFTP.po, &pMe->mFTP.remoteBDAddr,
                                   NULL, pMe->mFTP.wPassWord ) != SUCCESS )
      {
        MSG_FATAL( "TextEditSave - FTP_Authenticate failed", 0, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case BT_APP_MENU_FTP_CREATE_FLDR:
    {
      WSTRLCPY( pMe->mFTP.wFolderName, pMe->pText2, 
                ARR_SIZE(pMe->mFTP.wFolderName) );
      if ( IBTEXTFTP_CreateFolder( pMe->mFTP.po, pMe->mFTP.wFolderName ) != 
                                   SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

        MSG_FATAL( "FTP Create Folder failed", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_FTP_MSG_FOLDER_CREATION_FAILED, 
                           pMe->mFTP.wFolderName, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe ); /* Wait for create folder response */
      }
      break;
    }
    case BT_APP_MENU_FTP_MOVE_TO_FLDR:
    {
      WSTRLCPY( pMe->mFTP.wFolderName, pMe->pText2, 
                ARR_SIZE(pMe->mFTP.wFolderName) );
      if ( IBTEXTFTP_SetPath( pMe->mFTP.po, pMe->mFTP.wFolderName, 
                              AEEBT_FTP_SET_PATH_TO_FOLDER ) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */
        MSG_FATAL( "Set Path to folder failed", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, 
                           pMe->mFTP.wFolderName, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe ); /* Wait for connect confirm */
      }
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_PIN:
    {
      WSTRLCPY( pMe->mBIP.wPIN, pMe->pText2, ARR_SIZE(pMe->mBIP.wPIN) );
      if ( IBTEXTBIP_Authenticate( pMe->mBIP.po, pMe->mBIP.wPIN ) != SUCCESS )
      {
        MSG_FATAL( "TextEditSave - BIP_Authenticate failed", 0, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_PASSWORD:
    {
      WSTRLCPY( pMe->mBPP.wPassword, pMe->pText2, 
                ARR_SIZE(pMe->mBPP.wPassword) );
      if ( pMe->mBPP.bUserIDRequired != FALSE )
      {
        result = IBTEXTBPP_Authenticate( pMe->mBPP.po, 
                                         pMe->mBPP.target, pMe->mBPP.wUserID, 
                                         pMe->mBPP.wPassword );
      }
      else
      {
        result = IBTEXTBPP_Authenticate( pMe->mBPP.po, 
                                         pMe->mBPP.target, NULL, 
                                         pMe->mBPP.wPassword );
      }
      if ( result != SUCCESS )
      {
        MSG_FATAL( "TextEditSave - BPP_Authenticate failed", 0, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case BT_APP_MENU_BPP_USERID:
    {
      WSTRLCPY( pMe->mBPP.wUserID, pMe->pText2, ARR_SIZE(pMe->mBPP.wUserID) );
      break;
    }    
    case BT_APP_MENU_BPP_JOB_ID:
    {
      WSTRLCPY( pMe->mBPP.wJobId, pMe->pText2, ARR_SIZE(pMe->mBPP.wJobId) ); /* For future reference default */
      WSTRTOSTR( pMe->pText2, sBPPJobId, sizeof( sBPPJobId ) );
      pMe->mBPP.iJobId = ATOI( sBPPJobId );
      break;
    }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case BT_APP_MENU_PBAP_FILTER:
    {
      WSTRLCPY( pMe->mPBAP.wFilterValue, pMe->pText2, 
                ARR_SIZE(pMe->mPBAP.wFilterValue) );
      break;
    }
    case BT_APP_MENU_PBAP_MAX_LIST_COUNT:
    {
      WSTRLCPY( pMe->mPBAP.wMaxListCount, pMe->pText2, 
                ARR_SIZE(pMe->mPBAP.wMaxListCount) );
      break;
    }
    case BT_APP_MENU_PBAP_LIST_START_OFFSET:
    {
      WSTRLCPY( pMe->mPBAP.wListStartOffset, pMe->pText2, 
                ARR_SIZE(pMe->mPBAP.wListStartOffset) );
      break;
    }
    case BT_APP_MENU_PBAP_SEARCH_VALUE:
    {
      WSTRLCPY( pMe->mPBAP.wSearchValue, pMe->pText2, 
                ARR_SIZE(pMe->mPBAP.wSearchValue) );
      break;
    }
    case BT_APP_MENU_PBAP_USERID:
    {
      WSTRLCPY( pMe->mPBAP.wUserID, pMe->pText2, ARR_SIZE(pMe->mPBAP.wUserID) );
      break;
    }
    case BT_APP_MENU_PBAP_PASSWORD:
    {
      WSTRLCPY( pMe->mPBAP.wPassword, pMe->pText2, 
                ARR_SIZE(pMe->mPBAP.wPassword) );
      if ( pMe->mPBAP.bUserIDRequired != FALSE )
      {
        result = IBTEXTPBAP_Authenticate( pMe->mPBAP.po, 
                                          pMe->mPBAP.wUserID, 
                                          pMe->mPBAP.wPassword );
      }
      else
      {
        result = IBTEXTPBAP_Authenticate( pMe->mPBAP.po, 
                                          NULL, 
                                          pMe->mPBAP.wPassword );
      }
      if ( result != SUCCESS )
      {
        MSG_FATAL( "TextEditSave - PBAP_Authenticate failed", 0, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }    
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif // FEATURE_BT_EXTPF_SAP
#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_PASSWORD:
    {
      WSTRLCPY( pMe->mOBEX.wPassword, pMe->pText2, 
                ARR_SIZE(pMe->mOBEX.wPassword) );
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_OBEX, EVT_OBEX_AUTH );
      break;
    }
    case BT_APP_MENU_OBEX_USERID:
    {
      WSTRLCPY( pMe->mOBEX.wUserID, pMe->pText2, ARR_SIZE(pMe->mOBEX.wUserID) );
      break;
    }
#endif // FEATURE_IOBEX
#ifdef FEATURE_BT_EXTPF_AV
    case BT_APP_MENU_A2DP_CONTROL_DATA:
    {
      WSTRLCPY( pMe->mA2DP.wControlData, pMe->pText2, 
                ARR_SIZE(pMe->mA2DP.wControlData) );
      WSTRTOSTR( pMe->pText2, sControlData, sizeof( sControlData ) );
      pMe->mA2DP.uCtrlData = ATOI( sControlData );
      if ( pMe->mA2DP.uControlEvtID == IDS_A2DP_START_DELAY )
      {
        if ( IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_START_DELAY, 
                             pMe->mA2DP.uCtrlData ) != SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_FAILED, NULL, 3 );
        }
        else
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_SUCCESS, NULL, 3 );
        }
      }
      else if ( pMe->mA2DP.uControlEvtID == IDS_A2DP_SET_BITRATE )
      {
        if ( IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_SET_BITRATE,
                             pMe->mA2DP.uCtrlData ) != SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_FAILED, NULL, 3 );
        }
        else
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_SUCCESS, NULL, 3 );
        }
      }
      else
      {
        MSG_FATAL( "TextEditSave - Invalid A2DP control event", 0, 0, 0 );
      }
      break;
    }
#endif
    case BT_APP_MENU_L2_TEST:
    {
      WSTRLCPY( pMe->mL2.wControlData, pMe->pText2, 
                ARR_SIZE(pMe->mL2.wControlData) );
      WSTRTOSTR( pMe->pText2, sControlData, sizeof( sControlData ) );
      if( pMe->mL2.uDialogType == IDS_SERVER_PSM )
      {
        pMe->mL2.sSrvInfo.psm = ATOI( sControlData );
      }
      else
      {
        pMe->mL2.sCliInfo.psm = ATOI( sControlData );
      }
      break;
    }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif 
    default:
    {
      MSG_FATAL( "TextEditSave - aM=%d", TOP_MENU, 0, 0 );
      return FALSE;
    }
  }

  if ( ISHELL_EndDialog( pMe->m_pShell ) == EFAILED )
  {
    MSG_FATAL( "TextEditSave - ISHELL_EndDialog() failed", 0, 0, 0 );
  }

  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_TextEditHandleEvent
DESCRIPTION
============================================================================= */
static boolean BTApp_TextEditHandleEvent( 
  void*     pUser, 
  AEEEvent  evt, 
  uint16    wParam, 
  uint32    dw
)
{
  CBTApp*   pMe = (CBTApp*)pUser;
  IDialog*  pCurrentDialog;
  ITextCtl* pTextCtl;
#ifdef FEATURE_BT_2_1
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
  AEEBTKeypressType keyPressType;

  MSG_FATAL( "TextEditHndlEv - pMe->mRM.ioCaptype = %d", pMe->mRM.ioCaptype, 0, 0 );
#endif /* FEATURE_BT_2_1 */

  MSG_FATAL( "TextEditHndlEv - evt=%x wP=%x dw=%x", evt, wParam, dw );

  if ( (pCurrentDialog = ISHELL_GetActiveDialog( pMe->m_pShell )) == NULL )
  {
    if ( evt == EVT_DIALOG_END )
    {
      MSG_FATAL( "TextEditHndlEv - dialog end tm=%d", TOP_MENU, 0, 0 );
      if ( pMe->bBusyIconUp == FALSE )
      {
        BTApp_HandleClearKey( pMe );
      }
      else
      {
        (void)POP_MENU(); // pop the menu, but leave the busy icon up
      }
      return TRUE;
    }
    return FALSE;
  }

  IDIALOG_GetControl( pCurrentDialog, IDC_SK_MENU );
  pTextCtl = (ITextCtl*)IDIALOG_GetControl( pCurrentDialog, 
                                            IDC_TEXT_VALUE);

  ITEXTCTL_GetText( pTextCtl, pMe->pText2, SHORT_TEXT_BUF_LEN );

  switch ( evt )
  {
    case EVT_DIALOG_START:
    {
      MSG_FATAL( "TextEditHndlEv - dialog start", 0, 0, 0 );
      return BTApp_InitTextDlg( pMe, pCurrentDialog );
    }
    case EVT_KEY:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    {
      MSG_FATAL( "TextEditHndlEv - EVT_KEY %x", wParam, 0, 0 );
      if ( wParam == AVK_CLR )
      {
        if ( (TOP_MENU == BT_APP_MENU_PASSKEY) && (pMe->mRM.bBonding == FALSE) )
        {
          pMe->mRM.wPassKey[ 0 ] = NULL;
#ifdef FEATURE_BT_2_1
          if (( pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY ) && 
                ( pMe->mRM.bPassKey == TRUE ))
          {
            MSG_FATAL( "TextEditHandleEvent - User has cleared all the digit, "
                     "cancelling the Bonding operation with NULL passkey", 
                      0,0,0 );
            MSG_FATAL( "TextEditHandleEvent - RemoteDev BDa=%2x%2x%2x", 
                      pDev->bdAddr.uAddr[0], pDev->bdAddr.uAddr[1], 
                      pDev->bdAddr.uAddr[2] );
            pMe->mRM.bPassKey = FALSE ;
            IBTEXTRM_PasskeyReply( pMe->mRM.po, &pDev->bdAddr, "");
          }
          else
#endif /* FEATURE_BT_2_1 */
          {
            IBTEXTRM_PinReply( pMe->mRM.po, 
                               &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                               pMe->mRM.wPassKey );
          }
        }
        pMe->mRM.bBonding = FALSE;
        if ( ISHELL_EndDialog( pMe->m_pShell ) == EFAILED )
        {
          MSG_FATAL( "TextEditHndlEv - ISHELL_EndDialog() failed", 0, 0, 0 );
        }
        return TRUE;
      }
      //else if ( wParam == AVK_SELECT )
      else if ((wParam == AVK_SELECT) || (wParam == AVK_INFO))	//Modify by zzg  2010_11_03
      {
        return BTApp_TextEditSave( pMe );
      }
      return FALSE;
    }
    case EVT_COMMAND:
    {
      MSG_FATAL( "TextEditHndlEv - COMMAND wP=%x", wParam, 0, 0 );
      if ( (wParam == IDS_OK) || (wParam == AVK_SELECT) || (wParam == AVK_INFO))//Modify by zzg  2010_11_03
      {
        return BTApp_TextEditSave( pMe );
      }
      return TRUE;
    }
#ifdef FEATURE_BT_2_1
    case EVT_KEY_PRESS:
    {
      MSG_FATAL( "TextEditHndlEv-EVT_KEY_PRESS, pMe->mRM.ioCaptype = %d ", 
               pMe->mRM.ioCaptype, 0, 0 );
      if ( (pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) && 
           (pMe->mRM.bPassKey == TRUE) )
      {
        if ( wParam == AVK_CLR )
        {
          keyPressType = AEEBT_RM_KPN_DIGIT_ERASED;
        }
        else if( (wParam >= AVK_0) && (wParam <= AVK_POUND) )
        {
          keyPressType = AEEBT_RM_KPN_DIGIT_ENTERED;
        }
        else
        {
          MSG_FATAL( "TextEditHndlEv-EVT_KEY_PRESS, Neither Digit Entered or "
                   "Erased, returning FALSE ", 0, 0, 0 ); 
		 
          return FALSE;
        }
        MSG_FATAL( "Before IBTEXTRM_KeypressNotification, keyPressType=%d ", 
                 keyPressType, 0, 0 );
        if ( IBTEXTRM_KeypressNotification( pMe->mRM.po, &pDev->bdAddr, 
                                            keyPressType ) !=SUCCESS )
        {
          MSG_HIGH( "KeyPress Notification- Failed", 0, 0, 0 );
          return FALSE;
        }
        return TRUE;
      }    
      return FALSE;
    }
#endif /* FEATURE_BT_2_1 */
    default:
    {
      return FALSE;
    }
  }
}


/* ==========================================================================
FUNCTION BTApp_UpdateAGSettings
DESCRIPTION
============================================================================= */
static void BTApp_UpdateAGSettings( CBTApp* pMe, int newVal )
{
  switch ( TOP_MENU )
  {
    case BT_APP_MENU_CONN_MODE:
      if ( pMe->mAG.idleMode != (AEEBTIdleMode) newVal )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mAG.idleMode = (AEEBTIdleMode) newVal;
        
        if ( pMe->mAG.po != NULL )
        {
          IBTEXTAG_Config( pMe->mAG.po, pMe->mAG.idleMode, 
                           pMe->mAG.idleTimeout );
        }
      }
      break;
    case BT_APP_MENU_IDLE_TO:
      if ( pMe->mAG.idleTimeout != (uint16) newVal )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mAG.idleTimeout = (uint16) newVal;
        
        if ( pMe->mAG.po != NULL )
        {
          IBTEXTAG_Config( pMe->mAG.po, pMe->mAG.idleMode, 
                           pMe->mAG.idleTimeout );
        }
      }
      break;
    case BT_APP_MENU_SPKR_VOL:
      if ( pMe->mAG.spkrGain != (uint8) newVal )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mAG.spkrGain = (uint8) newVal;
        
        if ( pMe->mAG.po != NULL )
        {
          IBTEXTAG_SetSpkrGain( pMe->mAG.po, pMe->mAG.spkrGain );
        }
      }
      break;
    case BT_APP_MENU_MIC_VOL:
      if ( pMe->mAG.micGain != (uint8) newVal )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mAG.micGain = (uint8) newVal;
        
        if ( pMe->mAG.po != NULL )
        {
          IBTEXTAG_SetMicGain( pMe->mAG.po, pMe->mAG.micGain );
        }
      }
      break;
    default:
      MSG_FATAL( "UpdateAGSettings - m=%d", TOP_MENU, 0, 0 );
  }
}

#ifdef FEATURE_PHONE_VR
/* ==========================================================================
FUNCTION BTApp_HandleVRCapableMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleVRCapableMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  uint16  sel = IDS_BT_YES;
  boolean vr_enabled;

  MSG_FATAL( "HndlVRCapable - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
      if ( key == AVK_LEFT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
	case AVK_INFO:		//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_BT_YES:
            sel = IDS_BT_NO;
            break;
          case IDS_BT_NO:
            sel = 0;
            break;
        }
      }
      switch ( selection )
      {
        case IDS_BT_YES:
          vr_enabled = TRUE;
          break;
        case IDS_BT_NO:
          vr_enabled = FALSE;
          break;
        default:
          MSG_FATAL( "HndlVRCapable - sel=%d", selection, 0, 0 );
          vr_enabled = TRUE;
          break;
      }
      if ( pMe->mAG.bVREnabled != vr_enabled )
      {
        pMe->mAG.bVREnabled = vr_enabled;
        pMe->bConfigChanged = TRUE;
        //IBTEXTAG_SetVRCapable( pMe->mAG.po, vr_enabled );
      }

      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_HandleDevTypeMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDevTypeMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  uint16  sel = IDS_HEADSET;
  AEEBTAudioDevice devType = pMe->mAG.devType;
  boolean bPrivateMode = pMe->mAG.bPrivateMode;

  MSG_FATAL( "HndlDevType - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      if ( key == AVK_LEFT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }

	case AVK_INFO:		//Add By zzg 2010_11_03
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_HEADSET:
            sel = IDS_HANDSFREE;
            break;
          case IDS_HANDSFREE:
            sel = IDS_HANDSFREE_PRIVATE;
            break;
          case IDS_HANDSFREE_PRIVATE:
            sel = 0;
            break;
        }
      }

      switch ( selection )
      {
        case IDS_HEADSET:
          devType = AEEBT_AG_AUDIO_DEVICE_HEADSET;
          bPrivateMode = FALSE;
          break;
        case IDS_HANDSFREE:
          devType = AEEBT_AG_AUDIO_DEVICE_HANDSFREE;
          bPrivateMode = FALSE;
          break;
        case IDS_HANDSFREE_PRIVATE:
          devType = AEEBT_AG_AUDIO_DEVICE_HANDSFREE;
          bPrivateMode = TRUE;
          break;          
        default:
          MSG_FATAL( "HndlDevType - sel=%d", selection, 0, 0 );
          devType = pMe->mAG.devType;
          break;
      }
      if ( (pMe->mAG.devType != devType) || 
           (pMe->mAG.bPrivateMode != bPrivateMode) )
      {
        pMe->mAG.devType      = devType;
        pMe->mAG.bPrivateMode = bPrivateMode;
        pMe->bConfigChanged   = TRUE;
      }

      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_HandleIOCMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleIOCMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  uint16  sel = IDS_IOC_YESNO;
  AEEBTIOCapType ioCapType = pMe->mRM.ioCaptype; 

  MSG_FATAL( " BTApp_HandleIOCMenu - k=0x%x", key, 0, 0 );
  selection = IMENUCTL_GetSel( pMe->m_pIMenu );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    {
      ev_processed = TRUE;
      break;
    }
    case AVK_CLR:
	{
	  ev_processed = BTApp_HandleClearKey( pMe );
      break;
	}
	case AVK_SELECT:	
    {
      switch ( selection )
      {
        case IDS_IOC_YESNO:
          ioCapType = AEEBT_RM_IOC_DISPLAY_YES_NO;
          break;
        case IDS_IOC_DISPLAY_ONLY:
          ioCapType = AEEBT_RM_IOC_DISPLAY_ONLY;
          break;
        case IDS_IOC_KEYBOARD_ONLY:
          ioCapType = AEEBT_RM_IOC_KEYBOARD_ONLY;
          break;   
        case IDS_IOC_NO:
          ioCapType = AEEBT_RM_IOC_NO_IO;
          break;   
        default:
          MSG_FATAL( "HandleIOCMenu - sel=%d", selection, 0, 0 );
          ioCapType = pMe->mRM.ioCaptype;
          break;
      }
      if ( pMe->mRM.ioCaptype != ioCapType )
      {
        pMe->mRM.ioCaptype = ioCapType;
        pMe->bConfigChanged = TRUE;
        MSG_FATAL( "HandleIOCMenu - Setting IO Cap: %d", pMe->mRM.ioCaptype, 
                  0, 0 );
        if ( IBTEXTRM_SetIOCap( pMe->mRM.po, ioCapType ) != SUCCESS )
        {
          MSG_FATAL( "IBTEXTRM_SetIOCap - setting of ioCap", 0, 0, 0 );
          ev_processed = FALSE;
        }
      }
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
    }
	case AVK_INFO:	//Add By zzg 2010_11_03
    //case AVK_SELECT:
    {
      // update menu
       while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
        switch ( sel )
        {
          case IDS_IOC_YESNO:
            sel = IDS_IOC_DISPLAY_ONLY;
            break;
          case IDS_IOC_DISPLAY_ONLY:
            sel = IDS_IOC_KEYBOARD_ONLY;
            break;
          case IDS_IOC_KEYBOARD_ONLY:
            sel = IDS_IOC_NO;
            break;
          case IDS_IOC_NO:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}
/* ==========================================================================
FUNCTION BTApp_HandleServiceSecurityOptionsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleServiceSecurityOptionsMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  boolean sec_applied = FALSE;
  uint16  selection;
  uint16  sel = IDS_SECURITY_LOW;
  AEEBTSvcSecLevel srvSecType = pMe->mRM.srvSecType; 

  MSG_FATAL( "HandleServiceSecurityOptionsMenu - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key)) == 0 )
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }
  MENU_SET_SEL( selection );

  switch ( key )
  {
    case AVK_CLR:
    {
      sec_applied = BTApp_UpdateSecurityOptions(pMe , BTAPP_ALL); 
      if ( sec_applied == TRUE )
      {
          BTApp_ShowMessage( pMe, IDS_MSG_SECURITY_SAVED, NULL, 3);
      }
      else
      {
         BTApp_ShowMessage( pMe, IDS_MSG_SECURITY_NOT_SAVED, NULL, 3 );
      }
      //ev_processed = BTApp_HandleClearKey( pMe );
      ev_processed = TRUE;
      break;
    }

    case AVK_LEFT:
    case AVK_RIGHT:
      ev_processed = TRUE;  // ignored
      break;

    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
	case AVK_INFO:		//Add By zzg 2010_11_03	
    case AVK_SELECT:
    {
#ifdef FEATURE_BT_TEST_API
#error code not present
#else
      if ( ( selection >= IDS_SECURITY_NONE ) && 
           ( selection <= IDS_SECURITY_HIGH ) ) 
          //keep the same order in resource file
#endif
      {
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
        switch ( sel )
        {
          case IDS_SECURITY_LOW:
            sel = IDS_SECURITY_MED;
          break;
          case IDS_SECURITY_MED:
            sel = IDS_SECURITY_HIGH;
          break;
          case IDS_SECURITY_HIGH:
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
            sel = IDS_SECURITY_NONE;
          break;
          case IDS_SECURITY_NONE:
            sel = 0;
          break;
        }
      }
      switch ( selection )
      {
        case IDS_SECURITY_LOW:
          srvSecType = AEEBT_RM_SVC_SEC_LOW;
        break;
        case IDS_SECURITY_MED:
          srvSecType = AEEBT_RM_SVC_SEC_MED;
        break;
        case IDS_SECURITY_HIGH:
          srvSecType = AEEBT_RM_SVC_SEC_HIGH;
        break;   
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
        case IDS_SECURITY_NONE:
          srvSecType = AEEBT_RM_SVC_SEC_NONE;
        break;   
          default:
            MSG_FATAL( "HandleSvcSecurityMenu - sel=%d", selection, 0, 0 );
            srvSecType = pMe->mRM.srvSecType; 
            break;
        }
        if ( pMe->mRM.srvSecType != srvSecType )
        {
          pMe->mRM.srvSecType      = srvSecType;
          pMe->bConfigChanged = TRUE;
        }
        if ( ( pMe->mAG.srvSecType != srvSecType ) && 
             ( pMe->mAG.bSecSelected == TRUE ) )
        {
          pMe->mAG.srvSecType      = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
        if ( ( pMe->mNA.srvSecType != srvSecType ) &&
             ( pMe->mNA.bSecSelected == TRUE ) )
        {
          pMe->mNA.srvSecType     = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#ifdef FEATURE_BT_EXTPF_OPP 
        if ( ( pMe->mOPP.srvSecType != srvSecType ) &&
             ( pMe->mOPP.bSecSelected == TRUE ) )
        {
          pMe->mOPP.srvSecType     = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
        if ( ( pMe->mFTP.srvSecType != srvSecType ) &&
             ( pMe->mFTP.bSecSelected == TRUE ) )
        {
          pMe->mFTP.srvSecType     = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
        if ( ( pMe->mBIP.srvSecType != srvSecType ) &&
             ( pMe->mBIP.bSecSelected == TRUE ) )
        {
          pMe->mBIP.srvSecType     = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_BPP 
        if ( ( pMe->mBPP.srvSecType != srvSecType ) &&
             ( pMe->mBPP.bSecSelected == TRUE ) )
        {
          pMe->mBPP.srvSecType     = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
        if ( ( pMe->mPBAP.srvSecType != srvSecType ) &&
             ( pMe->mPBAP.bSecSelected == TRUE ) )
        {
          pMe->mPBAP.srvSecType    = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
        if ( ( pMe->mA2DP.srvSecType != srvSecType ) &&
             ( pMe->mA2DP.bSecSelected == TRUE ) )
        {
          pMe->mA2DP.srvSecType    = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
        if ( ( pMe->mAVRCP.srvSecType != srvSecType ) &&
             ( pMe->mAVRCP.bSecSelected == TRUE ) )
        {
          pMe->mAVRCP.srvSecType   = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
#endif
        if ( ( pMe->mL2.srvSecType != srvSecType ) &&
             ( pMe->mL2.bSecSelected == TRUE ) )
        {
          pMe->mL2.srvSecType      = srvSecType;
          pMe->bConfigChanged      = TRUE;
        }
        if ( ( pMe->mSPP.srvSecType != srvSecType ) &&
             ( pMe->mSPP.bSecSelected == TRUE ) )
        {
           pMe->mSPP.srvSecType     = srvSecType;
           pMe->bConfigChanged      = TRUE;
        }
      }
      else
      {
        switch ( selection )     
        {
          case IDS_AUTHORIZE:
          {
            MSG_FATAL( "HndlSlction - IDS_AUTHORIZE m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            pMe->mRM.bAuthorize = pMe->mRM.bAuthorize ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
                                     pMe->m_pIMenu,selection,
                                       pMe->mRM.bAuthorize ? 
                                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
          case IDS_AUTHORIZE_FIRST:
          {
            MSG_FATAL( "HndlSlction - IDS_AUTHORIZE_FIRST m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            pMe->mRM.bAuthorizeFirst = pMe->mRM.bAuthorizeFirst ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
                                       pMe->m_pIMenu,selection,
                                       pMe->mRM.bAuthorizeFirst ? 
                                       IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            break;
          }
        }  
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_UpdateSecurityOptions
DESCRIPTION
============================================================================= */
boolean BTApp_UpdateSecurityOptions( CBTApp* pMe , BTApp_ProfileEnabled profile )
{
  boolean  bSecApplied = FALSE; 

  MSG_FATAL( "UpdateSecurityOptions, pMe->mRM.srvSecType = %d, "
           "pMe->mRM.bAuthorize =%d, pMe->mRM.bAuthorizeFirst=%d,is", 
            pMe->mRM.srvSecType, pMe->mRM.bAuthorize, pMe->mRM.bAuthorizeFirst);

  if ( /*( pMe->mAG.bEnabled != FALSE )    && */
       ( pMe->mAG.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_AG ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po,
                             AEEBT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY,
                             pMe->mAG.srvSecType,
                             pMe->mAG.bAuthorize,
                             pMe->mAG.bAuthorizeFirst );
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY,
                             pMe->mAG.srvSecType, 
                             pMe->mAG.bAuthorize,
                             pMe->mAG.bAuthorizeFirst );
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po,
                             AEEBT_SD_SERVICE_CLASS_HANDSFREE,
                             pMe->mAG.srvSecType, 
                             pMe->mAG.bAuthorize,
                             pMe->mAG.bAuthorizeFirst );
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_HEADSET,
                             pMe->mAG.srvSecType,
                             pMe->mAG.bAuthorize,
                             pMe->mAG.bAuthorizeFirst );

    bSecApplied = TRUE; 
  } 
  if ( /*( pMe->mNA.bEnabled ) && */
       ( pMe->mNA.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL) || (profile == BTAPP_NA ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_DIALUP_NETWORKING,
                             pMe->mNA.srvSecType,
                             pMe->mNA.bAuthorize,
                             pMe->mNA.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
#ifdef FEATURE_BT_EXTPF_OPP 
  if ( /*( pMe->mOPP.bRegistered == TRUE )  && */
       ( pMe->mOPP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_OPP ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH , 
                             pMe->mOPP.srvSecType,
                             pMe->mOPP.bAuthorize,
                             pMe->mOPP.bAuthorizeFirst);

    bSecApplied = TRUE; 
  }
#endif /* FEATURE_BT_EXTPF_OPP */
#ifdef FEATURE_BT_EXTPF_FTP 
  if ( /*( pMe->mFTP.bRegistered == TRUE )  && */
       ( pMe->mFTP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_FTP ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER , 
                             pMe->mFTP.srvSecType,
                             pMe->mFTP.bAuthorize,
                             pMe->mFTP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
#endif /* FEATURE_BT_EXTPF_FTP */
#ifdef FEATURE_BT_EXTPF_BIP
  if ( /*( pMe->mBIP.bRegistered == TRUE )  && */
       ( pMe->mBIP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_BIP ) ) )
  {
    if( pMe->mBIP.serviceSelection == AEEBT_BIP_IMAGING_RESPONDER )
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_IMAGING_RESPONDER,
                               pMe->mBIP.srvSecType,
                               pMe->mBIP.bAuthorize,
                               pMe->mBIP.bAuthorizeFirst );  
      bSecApplied = TRUE; 
    }
    else if ( pMe->mBIP.serviceSelection == AEEBT_BIP_ARCHIVED_OBJECTS )
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE,
                               pMe->mBIP.srvSecType,
                               pMe->mBIP.bAuthorize,
                               pMe->mBIP.bAuthorizeFirst );
      bSecApplied = TRUE; 
    }
    else if ( pMe->mBIP.serviceSelection == AEEBT_BIP_REFERENCED_OBJECTS )
    {
      IBTEXTRM_SetSecBySvcCls( 
                              pMe->mRM.po, 
                              AEEBT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS,
                              pMe->mBIP.srvSecType,
                              pMe->mBIP.bAuthorize,
                              pMe->mBIP.bAuthorizeFirst );
      bSecApplied = TRUE; 
    } 
  }
#endif /* FEATURE_BT_EXTPF_BIP */
#ifdef FEATURE_BT_EXTPF_BPP
  if ( /*( pMe->mBPP.bRegistered == TRUE )  && */
       ( pMe->mBPP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_BPP ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( 
             pMe->mRM.po, 
             AEEBT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE,
             pMe->mBPP.srvSecType,
             pMe->mBPP.bAuthorize,
             pMe->mBPP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
#endif /* FEATURE_BT_EXTPF_BPP */
#ifdef FEATURE_BT_EXTPF_PBAP
  if ( /*( pMe->mPBAP.bSrvRegistered == TRUE ) && */
       ( pMe->mPBAP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_PBAP ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_PBAP_PSE,
                             pMe->mPBAP.srvSecType,
                             pMe->mPBAP.bAuthorize,
                             pMe->mPBAP.bAuthorizeFirst );
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_PBAP_PCE,
                             pMe->mPBAP.srvSecType,
                             pMe->mPBAP.bAuthorize,
                             pMe->mPBAP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */
#ifdef FEATURE_BT_EXTPF_AV
  if ( /*( pMe->mA2DP.bEnabled == TRUE )     && */
       ( pMe->mA2DP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL) || (profile == BTAPP_A2DP ) ) )
  {
    IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                          AEEBT_SD_PROTOCOL_AVDTP,
                          pMe->mA2DP.srvSecType,
                          pMe->mA2DP.bAuthorize,
                          pMe->mA2DP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
  if ( /*( pMe->mAVRCP.bEnabled == TRUE ) && */
       ( pMe->mAVRCP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL) || (profile == BTAPP_AVRCP ) ) )
  {
    IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                          AEEBT_SD_PROTOCOL_AVCTP,
                          pMe->mAVRCP.srvSecType,
                          pMe->mAVRCP.bAuthorize,
                          pMe->mAVRCP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }
#endif /* FEATURE_BT_EXTPF_AV */
  if ( /*( pMe->mL2.bEnabled == TRUE ) && */
       ( pMe->mL2.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL ) || ( profile == BTAPP_L2 ) ) )
  {
    IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                          pMe->mL2.sSrvInfo.psm,
                          pMe->mL2.srvSecType,FALSE,FALSE );    
    bSecApplied = TRUE; 
    if ( pMe->mL2.sCliInfo.psm != pMe->mL2.sSrvInfo.psm )
    {
      IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                            pMe->mL2.sCliInfo.psm,
                            pMe->mL2.srvSecType,
                            pMe->mL2.bAuthorize,
                            pMe->mL2.bAuthorizeFirst );
      bSecApplied = TRUE; 
    }
  }
  if ( ( pMe->mSPP.bSecSelected !=FALSE ) &&
       ( ( profile == BTAPP_ALL) || (profile == BTAPP_SPP ) ) )
  {
    IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                             AEEBT_SD_SERVICE_CLASS_SERIAL_PORT,
                             pMe->mSPP.srvSecType,
                             pMe->mSPP.bAuthorize,
                             pMe->mSPP.bAuthorizeFirst );
    bSecApplied = TRUE; 
  }

  pMe->bConfigChanged = bSecApplied;
  return bSecApplied;
}
/* ==========================================================================
FUNCTION BTApp_HandleSvrMainSec
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSvrMainSec( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;
  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
      switch( selection )
      {
         case IDS_STRING_SRV_PROFILE_SELECTION:
         	
            BTApp_BuildMenu(pMe, BT_APP_MENU_SVR_SEC_TYPE );
            break;

        case IDS_STRING_SRV_SEC_LEVEL:
        	
            BTApp_BuildMenu(pMe, BT_APP_MENU_SECURITY_OPTIONS );
            break;
            
        
        default:
            ev_processed = FALSE;
        break;
      }
  }
  else
      {
    switch ( key)
    {
      case AVK_INFO:		//Add By zzg 2010_11_03
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        switch( selection )
        {
          case IDS_STRING_SRV_PROFILE_SELECTION:
          
            BTApp_BuildMenu(pMe, BT_APP_MENU_SVR_SEC_TYPE );
            break;

          case IDS_STRING_SRV_SEC_LEVEL:
          	
            BTApp_BuildMenu(pMe, BT_APP_MENU_SECURITY_OPTIONS );
            break;

          default:
            ev_processed = FALSE;
            break;
      }
      break;
    }
      case AVK_CLR:
      {
        BTApp_HandleClearKey( pMe );
        break;
      }
      default:
      {
        ev_processed = FALSE;
        break;
      }
    }
  }
  return ev_processed;
}
/* ==========================================================================
FUNCTION BTApp_HandleSvrSec
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSvrSec( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  boolean ev_processed = TRUE , bEnableAll = FALSE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    key = AVK_SELECT;
  }
  else
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MSG_FATAL( "BTApp_HandleSvrSec - k=0x%x sel=%x", key, selection, 0 );
  MENU_SET_SEL( selection );
  
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      break;
    case AVK_UP:
    case AVK_DOWN:
      ev_processed = ISTATIC_HandleEvent( pMe->m_pStatic, EVT_KEY, key, 0 );
      break;
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe ); 
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      do
      {
        switch( selection )
        {
          case IDS_ENABLE_ALL:
          {
            MSG_FATAL( "HndlSlction - Enable all m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            bEnableAll = TRUE;
            pMe->mAG.bSecSelectedAll = pMe->mAG.bSecSelectedAll ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, selection,
              pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            selection = IDS_AUDIO_GATEWAY;
            break;
          }
          case IDS_AUDIO_GATEWAY:
          {
            MSG_FATAL( "HndlSlction - AUDIO_GATEWAY_SECURITY m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mAG.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mAG.bSecSelected = pMe->mAG.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, selection,
              pMe->mAG.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
              selection = IDS_ENABLE_L2;
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );      
              } 
            }
            break;
          }
          case IDS_ENABLE_L2:
          {
            MSG_FATAL( "HndlSlction - L2 m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mL2.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mL2.bSecSelected = pMe->mL2.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, selection,
              pMe->mL2.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
              selection = IDS_ENABLE_SPP;
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE; 
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }
          case IDS_ENABLE_SPP:
          {
            MSG_FATAL( "HndlSlction - SPP m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mSPP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mSPP.bSecSelected = pMe->mSPP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, selection,
              pMe->mSPP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
              selection = IDS_NETWORK_ACCESS;
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE; 
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }
          case IDS_NETWORK_ACCESS:
          {
            MSG_FATAL( "HndlSlction - NETWORK_ACCESS_SECURITY m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mNA.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mNA.bSecSelected = pMe->mNA.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, selection,
              pMe->mNA.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_OPP
              selection = IDS_ENABLE_OPP;
#elif defined (FEATURE_BT_EXTPF_FTP)
              selection = IDS_ENABLE_FTP;
#elif defined (FEATURE_BT_EXTPF_BIP)
              selection = IDS_ENABLE_BIP;
#elif defined (FEATURE_BT_EXTPF_BPP)
              selection = IDS_ENABLE_BPP;
#elif defined (FEATURE_BT_EXTPF_PBAP)
              selection = IDS_ENABLE_PBAP;
#elif defined (FEATURE_BT_EXTPF_HID_HOST)
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE; 
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;      
          }
#ifdef FEATURE_BT_EXTPF_OPP
          case IDS_ENABLE_OPP:
          {
            MSG_FATAL( "HndlSlction - OPP m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mOPP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mOPP.bSecSelected = pMe->mOPP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mOPP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_FTP
              selection = IDS_ENABLE_FTP;
#elif defined (FEATURE_BT_EXTPF_BIP)
              selection = IDS_ENABLE_BIP;
#elif defined (FEATURE_BT_EXTPF_BPP)
              selection = IDS_ENABLE_BPP;
#elif defined (FEATURE_BT_EXTPF_PBAP)
              selection = IDS_ENABLE_PBAP;
#elif defined (FEATURE_BT_EXTPF_HID_HOST)
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;  
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
          case IDS_ENABLE_FTP:
          {
            MSG_FATAL( "HndlSlction - FTP m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mFTP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mFTP.bSecSelected = 
              pMe->mFTP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_BIP
              selection = IDS_ENABLE_BIP;
#elif defined (FEATURE_BT_EXTPF_BPP)
              selection = IDS_ENABLE_BPP;
#elif defined (FEATURE_BT_EXTPF_PBAP)
              selection = IDS_ENABLE_PBAP;
#elif defined (FEATURE_BT_EXTPF_HID_HOST)
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;   
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
          case IDS_ENABLE_BIP:
          {
            MSG_FATAL( "HndlSlction - BIP m=%d", TOP_MENU, 0, 0 );
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mBIP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mBIP.bSecSelected = pMe->mBIP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
            pMe->mBIP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_BPP
              selection = IDS_ENABLE_BPP;
#elif defined (FEATURE_BT_EXTPF_PBAP)
              selection = IDS_ENABLE_PBAP;
#elif defined (FEATURE_BT_EXTPF_HID_HOST)
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;      
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }        
#endif
#ifdef FEATURE_BT_EXTPF_BPP
          case IDS_ENABLE_BPP:
          {
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mBPP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mBPP.bSecSelected = pMe->mBPP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
            pMe->mBPP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_PBAP
              selection = IDS_ENABLE_PBAP;
#elif defined (FEATURE_BT_EXTPF_HID_HOST)
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;    
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }        
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
          case IDS_ENABLE_PBAP:
          {
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mPBAP.bSecSelected = 
                pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mPBAP.bSecSelected = pMe->mPBAP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
            pMe->mPBAP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#elif defined (FEATURE_IOBEX)
              selection = IDS_ENABLE_OBEX;
#elif defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;   
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }        
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_IOBEX
          case IDS_ENABLE_OBEX:
          {
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mOBEX.bSecSelected = 
                pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mOBEX.bSecSelected = pMe->mOBEX.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
              pMe->mOBEX.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#elif defined (FEATURE_BT_EXTPF_AV)
              selection = IDS_ENABLE_A2DP;
#else
              bEnableAll = FALSE;
#endif    
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;    
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }        
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
          case IDS_ENABLE_A2DP:
          {
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              pMe->mA2DP.bSecSelected = 
                pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            pMe->mA2DP.bSecSelected = pMe->mA2DP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
              pMe->mA2DP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            if ( bEnableAll == TRUE )
            {
              selection = IDS_ENABLE_AVRCP;
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;   
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            break;
          }
          case IDS_ENABLE_AVRCP:
          {
            pMe->bConfigChanged = TRUE;
            if ( bEnableAll == TRUE )
            {
              bEnableAll = FALSE;
              pMe->mAVRCP.bSecSelected = pMe->mAG.bSecSelectedAll ? FALSE : TRUE ;
            }
            else
            {
              if ( pMe->mAG.bSecSelectedAll == TRUE )
              {
                pMe->mAG.bSecSelectedAll = FALSE;      
                BTApp_UpdateMenuItemImage( 
                  pMe->m_pIDisplay, pMe->m_pIMenu, IDS_ENABLE_ALL,
                  pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );     
              } 
            }
            pMe->mAVRCP.bSecSelected = pMe->mAVRCP.bSecSelected ? FALSE : TRUE;
            BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
              pMe->m_pIMenu, selection, 
            pMe->mAVRCP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
            break;
          }
#endif  //FEATURE_BT_EXTPF_AV
          default:
            ev_processed = FALSE;
            break;
        }
      } while ( bEnableAll == TRUE );
      break;
    } 
    default:
      ev_processed = FALSE;
      break;   
  }
  return ev_processed;
}
/* ==========================================================================
FUNCTION    BTApp_HandleBondOptionsMenu
DESCRIPTION 
============================================================================= */
static boolean BTApp_HandleBondOptionsMenu( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    key = AVK_SELECT;
  }
  else
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MSG_FATAL( "HandleBondOptions - k=0x%x sel=%x", key, selection, 0 );
  MENU_SET_SEL( selection );
  
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      break;
    case AVK_UP:
    case AVK_DOWN:
      ev_processed = ISTATIC_HandleEvent( pMe->m_pStatic, EVT_KEY, key, 0 );
      break;
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    case AVK_SELECT:
    {
      switch( selection )
      {
        case IDS_MITM:
        {
          MSG_FATAL( "HndlSlction - MITM settings m=%d, MITM=%d", 
                   TOP_MENU, pMe->mRM.bMITMEnabled, 0 );
          pMe->bConfigChanged = TRUE;
          pMe->mRM.bMITMEnabled =  pMe->mRM.bMITMEnabled ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay,
                                     pMe->m_pIMenu,selection,
                                     pMe->mRM.bMITMEnabled ? 
                                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_REBOND_OPT_MITM:
        {
          MSG_FATAL( "HndlSlction - Rebond options MITM settings m=%d, MITM=%d", 
                   TOP_MENU, pMe->mRM.bRebondOptMITMEnabled, 0 );
          pMe->bConfigChanged = TRUE;
          pMe->mRM.bRebondOptMITMEnabled = pMe->mRM.bRebondOptMITMEnabled ?
                                           FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay,
                                     pMe->m_pIMenu,selection,
                                     pMe->mRM.bRebondOptMITMEnabled ? 
                                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_REBOND_OPT_BOND:
        {
          MSG_FATAL( "HndlSlction - Rebond options BOND settings m=%d, BOND=%d", 
                   TOP_MENU, pMe->mRM.bRebondOptBONDEnabled, 0 );
          pMe->bConfigChanged = TRUE;
          pMe->mRM.bRebondOptBONDEnabled = pMe->mRM.bRebondOptBONDEnabled ?
                                           FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay,
                                     pMe->m_pIMenu,selection,
                                     pMe->mRM.bRebondOptBONDEnabled ? 
                                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        default:
          ev_processed = FALSE;
          break;   
      }
    }
    default:
      ev_processed = FALSE;
      break;    
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_BuildBondMenu
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_BuildBondMenu( CBTApp* pMe )
{
	AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];

	MSG_FATAL("***zzg BTApp_BuildBondMenu bBonding=%d, bSSPCapable=%d, bpassKeyRqst=%d***",
					pMe->mRM.bBonding, pDev->bSSPCapable, pMe->mRM.bpassKeyRqst);

	//Del By zzg 2010_12_28
#if 1
	if (pMe->mRM.bBonding && pDev->bSSPCapable && !(pMe->mRM.bpassKeyRqst))
	{
		MSG_FATAL("BuildBondMenu - MITM Enabled : %d", pMe->mRM.bMITMEnabled, 0, 0);

		MSG_FATAL("***zzg BuildBondMenu - MITM Enabled : %d***", pMe->mRM.bMITMEnabled, 0, 0);

		BTApp_BuildPrompt(pMe, BT_APP_WAITING);		//Add By zzg 2012_11_28

		if (IBTEXTRM_SSPBond( pMe->mRM.po, &pDev->bdAddr, pMe->mRM.bMITMEnabled ) != SUCCESS)
		{
			MSG_FATAL("***zzg BTApp_BuildBondMenu IBTEXTRM_SSPBond Failed!***", 0, 0, 0);
			MSG_FATAL("TextEditSave - Bond failed", 0, 0, 0);
		}
		else
		{
			//Modify by zzg 2011_03_04
			//Showbusyicon............
			ShowBusyIcon(pMe->m_pShell, 
			             pMe->m_pIDisplay, 
			             &pMe->m_rect, 
			             FALSE);
			
			pMe->bBusyIconUp = TRUE;
			//Add End
										
			MSG_FATAL("***zzg BTApp_BuildBondMenu IBTEXTRM_SSPBond SUCCEED!***", 0, 0, 0);
			BTApp_ShowBusyIcon(pMe);
		}
	}
	else
#endif   	
	//Del End
	{     
		if (WSTRLEN(pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName) == 0)
		{
			BTApp_BDAddr2Wstr(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName,&pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr);
		}

		WSTRLCPY(pMe->wEditBuf, pMe->mRM.device[pMe->mRM.uCurDevIdx].wName, ARR_SIZE(pMe->mRM.device[pMe->mRM.uCurDevIdx].wName));

		BTApp_SetBondable(pMe);//ACG

		pMe->mRM.bpassKeyRqst = FALSE;

		pMe->m_edit_id = IDS_PASS_KEY;
		pMe->m_bEditNeedStr = TRUE;		

		pMe->m_edit_state_id = BTAPPST_DEVICEINFO;

		//CLOSE_DIALOG(DLGRET_BT_EDIT) 
		pMe->m_eDlgRet = DLGRET_BT_EDIT; 
		(void) ISHELL_EndDialog(pMe->m_pShell);  

	}
}

/* ==========================================================================
FUNCTION BTApp_BuildDisplayPasskeyMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildDisplayPasskeyMenu( CBTApp* pMe )
{
  AEERect     rc;
  uint16      uLen = 0;
  AECHAR      wTitle[ 32 ]; //increased memory here
  AECHAR      wBuf1[ 64 ];
  AECHAR      wBDAddr[ 32 ];
  AECHAR*     wText ;
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  CLEAR_SCREEN();
  wText = MALLOC( sizeof(AECHAR) * SHORT_TEXT_BUF_LEN );
  if( wText == NULL )
  {
    MSG_FATAL( "BTApp_BuildDisplayPasskeyMenu - Failure memory allocation", 
             0, 0, 0 );
    return;
  }
  else
  {
    // set rect for info display area
    SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy - BTBAR_HEIGHT); // leave room for SK menu
    ISTATIC_SetRect( pMe->m_pStatic, &rc );

	
    ISTATIC_SetProperties( pMe->m_pStatic, 
                           ISTATIC_GetProperties( pMe->m_pStatic ) 
                           & ~ST_MIDDLETEXT & ~ST_CENTERTEXT );
          
    //Add By zzg 2010_10_29        
	ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
	ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 
    //Add End


	/*
    // get title
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_DISPLAY_PSSKEY, 
                          wTitle, sizeof(wTitle) );
                          */


	//Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
    {
        //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
    }  
    {   
	  	ISHELL_LoadResString(pMe->m_pShell,
	                           AEE_APPSBTAPP_RES_FILE,                                
	                           IDS_DISPLAY_PSSKEY,
	                           wTitle,
	                           sizeof(wTitle));
   
	    if(pMe->m_pIAnn != NULL)
	  	{
	  	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTitle);
	  	}
    }
	//Add End
	
    //add Title Here
    if ( WSTRLEN( pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName ) == 0 )
    {
      BTApp_BDAddr2Wstr( wBDAddr,&pDev->bdAddr  );
    }
    else
    {
      WSTRLCPY( wBDAddr, pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName, 
                ARR_SIZE( wBDAddr ) );
    }

    WSPRINTF(wText, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ),wTitle,wBDAddr );
    // Display the passkey generated from the lower layers


    /*uLen += BTApp_FormatPasskey( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen, 
                                 pMe->mRM.wPassKey , pMe->mRM.wDisplayPassKey );*/

    //Display Locally Generated Passkey 
    DBGPRINTF_FATAL( "BTApp_BuildDisplayPasskeyMenu , LocalPasskey=%s",
                      pMe->mRM.wPassKey,0,0); 
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                          IDS_LABEL_LOCAL_PSSKEY, wBuf1, sizeof( wBuf1 ) );
    WSPRINTF( pMe->pText1, (LONG_TEXT_BUF_LEN - uLen)*sizeof(AECHAR),wBuf1, 
              pMe->mRM.wPassKey );
    wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
    wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
    WSTRLCAT(pMe->pText1, wBuf1, LONG_TEXT_BUF_LEN*sizeof(AECHAR));

    //Display Remote user Passkey in terms of "*"s
    uLen += WSTRLEN(pMe->pText1);
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                          IDS_LABEL_REMOTE_PSSKEY, wBuf1, sizeof( wBuf1 ) );
    WSPRINTF( (pMe->pText1+ WSTRLEN(pMe->pText1) ), (LONG_TEXT_BUF_LEN - uLen)*sizeof(AECHAR),wBuf1, 
               pMe->mRM.wDisplayPassKey );
    DBGPRINTF_FATAL( "BTApp_BuildDisplayPasskeyMenu, RemotePasskey=%s ",
                     pMe->mRM.wDisplayPassKey,0,0); 
    wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
    wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
    WSTRLCAT(pMe->pText1, wBuf1, LONG_TEXT_BUF_LEN*sizeof(AECHAR));

    // display text
    //ISTATIC_SetText( pMe->m_pStatic, wText, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
    ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
	
    ISTATIC_Redraw( pMe->m_pStatic );

    // add the SK menu to bottom of screen
    IMENUCTL_Reset( pMe->m_pISoftMenu );

    // set rect for SK menu
    rc.y  = rc.dy;
    rc.dy = pMe->m_rect.dy - rc.y;
    IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

    PUSH_MENU( BT_APP_MENU_DISPLAY_PASSKEY );
    IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
    IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
    IMENUCTL_SetSel( pMe->m_pISoftMenu, MENU_SEL );
    FREEIF(wText);
  } 
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_HandleConnModeMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleConnModeMenu( CBTApp* pMe, uint16 key )
{
  boolean       ev_processed = FALSE;
  uint8         i;
  uint16        selection = 0;
  uint16        sel = IDS_ACTIVE_ANY_DEVICE;
  AEEBTIdleMode newMode;
  CtlAddItem    ai;

  MSG_FATAL( "HndlConnMode - k=0x%x", key, 0, 0 );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      for ( i=IMENUCTL_GetItemCount( pMe->m_pIMenu ); i>0; i-- )
      {
        if ( IMENUCTL_GetItem( pMe->m_pIMenu, 
                               IMENUCTL_GetItemID( pMe->m_pIMenu, i-1 ), 
                               &ai ) != FALSE )
        {
          if ( ai.wImage == IDB_BT_RADIO_FILLED )
          {
            selection = ai.wItemID;
            break;
          }
        }
      }
      switch ( selection )
      {
        case IDS_ACTIVE:
          newMode = AEEBT_AG_IDLEMODE_ACTIVE;
          break;
        case IDS_ACTIVE_ANY_DEVICE:
          newMode = AEEBT_AG_IDLEMODE_ACTIVE;
          pMe->mAG.bdAddr = NULL_BD_ADDR;
          break;
        case IDS_PASSIVE_ANY_DEVICE:
          newMode = AEEBT_AG_IDLEMODE_PASSIVE;
          pMe->mAG.bdAddr = NULL_BD_ADDR;
          break;          
        case IDS_PASSIVE:
        default:
          newMode = AEEBT_AG_IDLEMODE_PASSIVE;
          break;
      }
      if ( newMode != pMe->mAG.idleMode )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mAG.idleMode = newMode;
        
        if ( pMe->mAG.po != NULL )
        {
          IBTEXTAG_Config( pMe->mAG.po, pMe->mAG.idleMode, 
                           pMe->mAG.idleTimeout );
        }
      }
      if ( key == AVK_LEFT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
	case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
        switch ( sel )
        {
          case IDS_ACTIVE_ANY_DEVICE:
            sel = IDS_PASSIVE;
            break;
          case IDS_PASSIVE:
            sel = IDS_ACTIVE;
            break;
          case IDS_ACTIVE:
            sel = IDS_PASSIVE_ANY_DEVICE;
            break;
          case IDS_PASSIVE_ANY_DEVICE:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleHSButtonHndlrMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleHSButtonHndlrMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection = IDS_IGNORE;
  boolean bIgnore;
  CtlAddItem ai;

  IMENUCTL_GetItem( pMe->m_pIMenu, selection, &ai );
  bIgnore = (ai.wImage == IDB_CHECK_ON) ? TRUE : FALSE;

  MSG_FATAL( "HndlHSButton - k=0x%x bIgnore=%d f=0x%x", 
           key, bIgnore, pMe->mAG.uFlags );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      if ( bIgnore != BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B ) )
      {
        pMe->bConfigChanged = TRUE;
        if ( bIgnore )
        {
          BTAPP_AG_SETBIT( BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B );
        }
        else
        {
          BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B );
        }
      }
      if ( key == AVK_LEFT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_DoAGSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
	case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      BTApp_UpdateMenuItemImage( 
        pMe->m_pIDisplay, pMe->m_pIMenu, selection,
        bIgnore ? IDB_BT_CHECK_OFF : IDB_BT_CHECK_ON );
      ISTATIC_Redraw( pMe->m_pStatic );

      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleListMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleListMenu( CBTApp* pMe, uint16 key )
{
  int     oldVal, newVal, ceiling, inc=1;
  char    szBuf[ 20 ];
  AECHAR  wBuf[ 5 ];
  boolean ev_processed = FALSE;

  MSG_FATAL( "HndlList - m=%d k=0x%x", TOP_MENU, key, 0 );

  switch ( TOP_MENU )
  {
    case BT_APP_MENU_IDLE_TO:
      ceiling = MAX_IDLE_TIME_SEC;
      inc     = 5;
      break;
    case BT_APP_MENU_SPKR_VOL:
    case BT_APP_MENU_MIC_VOL:
      ceiling = MAX_VOL_LEVEL;
      inc     = 1;
      break;
    default:
      return FALSE;
  }
  if ( ITEXTCTL_GetText( pMe->m_pText, wBuf, ARR_SIZE( wBuf ) ) == FALSE )
  {
    return FALSE;
  }

  if ( WSTRTOSTR( wBuf, szBuf, sizeof( szBuf ) ) != szBuf )
  {
    return FALSE;
  }

  oldVal = newVal = ATOI( szBuf );

  switch ( key )
  {
    case AVK_UP:
      if ( newVal+inc <= ceiling )
      {
        newVal += inc;
      }
      if ( newVal != oldVal )
      {
        // set the value as text
        WWRITELONG( wBuf, newVal );
        ITEXTCTL_SetText( pMe->m_pText, wBuf, WSTRLEN( wBuf ) );
        ITEXTCTL_Redraw( pMe->m_pText );
      }
      ev_processed = TRUE;
      break;
    case AVK_DOWN:
      if ( newVal >= inc )
      {
        newVal -= inc;
      }
      if ( newVal != oldVal )
      {
        // set the value as text
        WWRITELONG( wBuf, newVal );
        ITEXTCTL_SetText( pMe->m_pText, wBuf, WSTRLEN( wBuf ) );
        ITEXTCTL_Redraw( pMe->m_pText );
      }
      ev_processed = TRUE;
      break;
    case AVK_LEFT:
      BTApp_UpdateAGSettings( pMe, newVal );
      BTApp_DoAGSettingsMenu( pMe, MOVE_LEFT );
      break;
    case AVK_RIGHT:
      BTApp_UpdateAGSettings( pMe, newVal );
      BTApp_DoAGSettingsMenu( pMe, MOVE_RIGHT );
      break;
    case AVK_CLR:
      BTApp_UpdateAGSettings( pMe, newVal );
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    case AVK_SELECT:
      ev_processed = TRUE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleDevListMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDevListMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;

MSG_FATAL("***zzg BTApp_HandleDevListMenu key=%x***", key, 0, 0);
  switch ( key )
  {
    case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
      {
        pMe->mRM.uCurDevIdx = IMENUCTL_GetSel( pMe->m_pIMenu );
        MENU_SET_SEL( pMe->mRM.uCurDevIdx );
        if ( PREV_MENU == BT_APP_MENU_SPP_TESTS )
        {
          // dev list was displayed so user can choose an SPP server
          (void)POP_MENU();
          BTApp_DoSPP( pMe, TRUE );
        }
#ifdef FEATURE_IOBEX
        else if ( PREV_MENU == BT_APP_MENU_OBEX_CLIENT )
        {
          // dev list was displayed so user can choose an OBEX server
          (void)POP_MENU();
          BTApp_OBEXConnect( 
            pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
        }
#endif
        else if ( (TOP_MENU != BT_APP_MENU_LIST_OBEX_SERVERS) && 
                  (TOP_MENU != BT_APP_MENU_LIST_TELEPHONY)
#ifdef FEATURE_BT_EXTPF_AV
                  && (PREV_MENU != BT_APP_MENU_A2DP_TEST)
                  && (PREV_MENU != BT_APP_MENU_AVRCP_TEST)
#endif
                  && (PREV_MENU != BT_APP_MENU_L2_TEST)
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
                )
        {
          BTApp_BuildDevInfo( pMe );
        }
        else
        {
          (void)POP_MENU();
          switch ( TOP_MENU )
          {
#ifdef FEATURE_BT_EXTPF_OPP
			case BT_APP_MENU_OPP_SENDFILE:	//Add By zzg 2010_11_10
            case BT_APP_MENU_OPP_CLIENT:
            {
#ifdef FEATURE_BT_2_1
              //Client side service security settings
              IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                       AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH , 
                                       pMe->mOPP.srvSecType,FALSE,FALSE );
#endif /* FEATURE_BT_2_1 */
				MSG_FATAL("***zzg BTApp_OPPConnect 111**", 0, 0, 0);
              BTApp_OPPConnect( 
                pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
            case BT_APP_MENU_FTP_CLIENT:
            {
#ifdef FEATURE_BT_2_1
              //Client side service security settings
              IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                       AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER , 
                                       pMe->mFTP.srvSecType,FALSE,FALSE );
#endif /* FEATURE_BT_2_1 */
              BTApp_FTPConnect( 
                pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
            case BT_APP_MENU_BIP_CLIENT:
            {
#ifdef FEATURE_BT_2_1
              //Client side service security settings
              //IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
              //                         AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER , 
              //                         pMe->mRM.srvSecType,FALSE,FALSE );
#endif /* FEATURE_BT_2_1 */
              BTApp_BIPBuildClientSubMenu( pMe );
              break;
            }
            case BT_APP_MENU_BIP_SETTINGS:
            {
              pMe->mBIP.printerBDAddr = pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr;
			  
              BTApp_BuildMenu( pMe, TOP_MENU );
              break;
            }
#endif
#ifdef FEATURE_BT_EXTPF_BPP
            case BT_APP_MENU_BPP_LIST_TARGET:
            {
              BTApp_BPPConnect(pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
            case BT_APP_MENU_BPP_SETTINGS:
            {
              pMe->mBPP.printerBDAddr = pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr;
              
              BTApp_BuildMenu( pMe, TOP_MENU );
              break;
            }
#endif //FEATURE_BT_EXTPF_BPP
#ifdef FEATURE_BT_EXTPF_PBAP
            case BT_APP_MENU_PBAP_CLIENT:
            {
#ifdef FEATURE_BT_2_1
              //Client side service security settings
              IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                       AEEBT_SD_SERVICE_CLASS_PBAP_PSE , 
                                       pMe->mPBAP.srvSecType,FALSE,FALSE );
#endif /* FEATURE_BT_2_1 */
              BTApp_PBAPConnect( 
                pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
#endif //FEATURE_BT_EXTPF_PBAP
#ifdef FEATURE_IOBEX
            case BT_APP_MENU_OBEX_CLIENT:
            {
#ifdef FEATURE_BT_2_1
              //Client side service security settings
              //IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
              //                         AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER , 
              //                         pMe->mRM.srvSecType,FALSE,FALSE );
#endif /* FEATURE_BT_2_1 */
              BTApp_OBEXConnect( 
                pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
#endif
#ifdef FEATURE_BT_EXTPF_CTP
            case BT_APP_MENU_ICP_TESTS:
            {
              BTApp_ICPConnect( 
                pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
              break;
            }
#endif // FEATURE_BT_EXTPF_CTP
#if defined(FEATURE_BT_EXTPF_SAP) && defined(FEATURE_BT_TEST_API)
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
            case BT_APP_MENU_A2DP_TEST:
              BTApp_A2DPHandleDevListMenu( pMe );
              break;
            case BT_APP_MENU_AVRCP_TEST:
              BTApp_AVRCPHandleDevListMenu( pMe );
              break;
#endif // FEATURE_BT_EXTPF_AV
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif //FEATURE_BT_EXTPF_HID_HOST
            case BT_APP_MENU_L2_TEST:
              BTApp_L2HandleDevListMenu( pMe );
              break;
            default:
              BTApp_BuildDevInfo( pMe );
              break;
          }
        }
        ev_processed = TRUE;
      }
      else
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleDevRespMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDevRespMenu( CBTApp* pMe, uint16 key )
{
  AEEBTDeviceInfo*    pDev;
  AEEBTDeviceRecord*  pRec;
  boolean             ev_processed = FALSE;

  switch ( key )
  {
    case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      if ( pMe->mSD.bDiscovering )
      {
        IBTEXTSD_StopDeviceDiscovery( pMe->mSD.po );
      }

      if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
      {
        MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pIMenu ) );
#ifdef FEATURE_BT_2_1
        pMe->mRM.uCurDevIdx = 
          pMe->mSD.uCurRecIdx = pMe->mRM.uDevToDspIdxMap[MENU_SEL];
        pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
        pRec = &pMe->mSD.devRec[ pMe->mSD.uCurRecIdx ];
#else
        pMe->mRM.uCurDevIdx = pMe->mSD.uCurRecIdx = MENU_SEL;
        pDev = &pMe->mRM.device[ MENU_SEL ];
        pRec = &pMe->mSD.devRec[ MENU_SEL ];
#endif /* FEATURE_BT_2_1 */
        if ( pDev->bValid == FALSE )
        {
          if ( (pDev->serviceClass == AEEBT_COD_SC_UNKNOWN) &&
               (pRec->serviceClass != AEEBT_COD_SC_UNKNOWN) )
          {
            pDev->serviceClass  = pRec->serviceClass;
            pDev->majorDevClass = pRec->majorDevClass;
            pDev->minorDevClass = pRec->minorDevClass;
          }
        }
        BTApp_BuildDevInfo( pMe );
        ev_processed = TRUE;
      }
      else
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_CLR:
    {
      if ( pMe->mSD.bDiscovering )
      {
        IBTEXTSD_StopDeviceDiscovery( pMe->mSD.po );
      }
      BTApp_CancelDevNameRequest( pMe );
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
    }
  }
  return ev_processed;
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_RemoteOOBWrite
DESCRIPTION
============================================================================= */
int BTApp_RemoteOOBWrite( CBTApp* pMe, AEEBTDeviceInfo *pDev )
{
  IFile*            pIFile    = NULL;
  IFileMgr*         pIFileMgr = NULL;
  uint8             uOOBDataBuffer[ 100 ];
  char              szOOBFileNm[ AEEBT_MAX_FILE_NAME+1 ];
  uint8             nOOBDataSz;
  uint8             result = EFAILED; 
  AECHAR            wBuf1[18]; 
  char              szBDAddr [18];
  MSG_FATAL( "OOB Remote Data Write", 0, 0, 0 );


  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                              (void **)&pIFileMgr ) == SUCCESS )
  {
    BTApp_BDAddr2Wstr (wBuf1, &( pDev->bdAddr ));
    WSTRTOSTR( wBuf1, szBDAddr, sizeof( szBDAddr ) );

    STRLCPY( szOOBFileNm, BTAPP_ROOT_DIR, sizeof(szOOBFileNm));
    STRLCAT( szOOBFileNm, DIRECTORY_STR, sizeof(szOOBFileNm) );
    STRLCAT( szOOBFileNm, "OOB-",sizeof(szOOBFileNm) );
    STRLCAT( szOOBFileNm, szBDAddr, sizeof (szOOBFileNm));
    
    if( IFILEMGR_Test( pIFileMgr, szOOBFileNm ) == SUCCESS )
    {
      pIFile = IFILEMGR_OpenFile( pIFileMgr, szOOBFileNm , _OFM_READWRITE );

      if ( (pIFile != NULL) )
      {
        /* How do you read the file till EOF */
        nOOBDataSz = IFILE_Read( pIFile, uOOBDataBuffer, 100);
        if (IBTEXTRM_SetRemoteOOBData( pMe->mRM.po, uOOBDataBuffer,
                                        nOOBDataSz ) == SUCCESS)
        {
          result = SUCCESS; 
        }
      }
      else
      {
        MSG_FATAL( "OOB Remote read failed", 0, 0, 0 );
      }
      
      if ( pIFile != NULL )
      {
        IFILE_Release( pIFile );
      }
      IFILEMGR_Remove( pIFileMgr, szOOBFileNm );
    }
    else
    {
      MSG_FATAL( "OOB Remote does not exist", 0, 0, 0 );
    }
    IFILEMGR_Release( pIFileMgr );
  }
  return result;
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_HandleDevInfoMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDevInfoMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  switch ( key )
  {
    case AVK_UP:
    case AVK_DOWN:
      break;

    case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pISoftMenu ) );
      switch ( MENU_SEL )
      {
        case IDS_KEEP:
          pDev->bValid = ( PREV_MENU == BT_APP_MENU_DEV_RESP ) ? TRUE : FALSE;
          IBTEXTRM_DeviceAdd( pMe->mRM.po, pDev );
          break;
        case IDS_DISCARD:
          BTApp_BuildMenu( pMe, BT_APP_MENU_REMOVE_ONE );
          break;
        case IDS_BOND:
          pMe->mRM.bBonding = TRUE;		 
          BTApp_BuildMenu( pMe, BT_APP_MENU_PASSKEY );
          break;
        case IDS_UNBOND:
          BTApp_BuildMenu( pMe, BT_APP_MENU_UNBOND_ONE );
          break;
        case IDS_USE:
          BTApp_BuildMenu( pMe, BT_APP_MENU_USE_AUDIO_DEV );
          break;
        case IDS_CONNECT:
          BTApp_BuildMenu( pMe, BT_APP_MENU_AG_CONNECT );
          break;
        case IDS_DISCONNECT:
          BTApp_BuildMenu( pMe, BT_APP_MENU_AG_DISCONNECT );
          break;
        case IDS_OPTIONS:
          BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_OPTIONS );
          break;
#ifdef FEATURE_BT_2_1
        case IDS_BOND_OPTIONS:
          BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_BOND_OPTIONS );
          break;
        case IDS_READ_OOB:
          pMe->bRemoteOOBRead = TRUE; 
          (void)POP_MENU();
          BTApp_BuildDevInfo (pMe); 
          pMe->bRemoteOOBRead = FALSE;  
          break;

#endif /* FEATURE_BT_2_1 */
      }
      break;
    }
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;

    default:
      ev_processed = FALSE;
  }

  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleSecurityMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSecurityMenu( CBTApp* pMe, uint16 key )
{
  int               result;
  boolean           ev_processed = FALSE;
  uint16            selection;
  uint16            sel = IDS_SECURITY_NONE;
  AEEBTSecurity     selectedSecurity;
  AEEBTSecurity*    pCurrentSecurity;
  AEEBTDeviceInfo*  pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  MSG_FATAL( "HndlSec - k=0x%x m=%d", key, TOP_MENU, 0 );

  selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      ev_processed = TRUE;
      break;
    case AVK_CLR:
      if ( TOP_MENU == BT_APP_MENU_DEV_SECURITY )
      {
        pCurrentSecurity = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].security;
      }
      else  // must be local security
      {
        pCurrentSecurity = &pMe->mRM.myInfo.security;
      }

      switch ( selection )
      {
        case IDS_SECURITY_NONE:
          selectedSecurity = AEEBT_SEC_NONE;
          break;
        case IDS_SECURITY_AUTH:
          selectedSecurity = AEEBT_SEC_AUTHENTICATE;
          break;
        case IDS_SECURITY_AUTH_ENC:
          selectedSecurity = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
          break;
        default:
          MSG_FATAL( "HndlDevSec - selection=%d", selection, 0, 0 );
          selectedSecurity = *pCurrentSecurity;
      }

      if ( selectedSecurity != *pCurrentSecurity )
      {
        *pCurrentSecurity = selectedSecurity;

        if ( TOP_MENU == BT_APP_MENU_DEV_SECURITY )
        {
          result = IBTEXTRM_SetDeviceSecurity( pMe->mRM.po, &pDev->bdAddr, 
                                               *pCurrentSecurity );
        }
        else  // must be local security
        {
          result = IBTEXTRM_SetSecurity( pMe->mRM.po, *pCurrentSecurity );
        }
        if ( result != SUCCESS )
        {
          MSG_FATAL( "HndlSec - failed to set security", 0, 0, 0 );
        }
      }
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    case AVK_SELECT:
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_SECURITY_NONE:
            sel = IDS_SECURITY_AUTH;
            break;
          case IDS_SECURITY_AUTH:
            sel = IDS_SECURITY_AUTH_ENC;
            break;
          case IDS_SECURITY_AUTH_ENC:
            sel = 0;
            break;
        }
      }
      //IMENUCTL_Redraw( pMe->m_pIMenu );
      ev_processed = TRUE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleDiscoverableMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDiscoverableMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  boolean discoverable;
  uint16  selection;
  uint16  sel = IDS_ON_TIMED;
  int     result;

  MSG_FATAL( "HndlDiscoverable - k=0x%x", key, 0, 0 );

  selection = IMENUCTL_GetSel( pMe->m_pIMenu );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:		
      ev_processed = TRUE;
      break;
    case AVK_CLR:
	{
		ev_processed = BTApp_HandleClearKey(pMe);
		break;
	}
	case AVK_SELECT:
      switch (selection )
      {
        case IDS_ON_TIMED:
          discoverable = TRUE;
          break;
        case IDS_OFF:
          discoverable = FALSE;
          break;
        default:
          MSG_FATAL( "HndlDiscoverable - selection=%d", selection, 0, 0 );
          discoverable = pMe->mSD.bDiscoverable;
          
      }
      if ( discoverable != pMe->mSD.bDiscoverable )
      {
        if ( discoverable )
        {
          if ( (result = IBTEXTSD_SetDiscoverable( pMe->mSD.po, 
                                                   TRUE )) != SUCCESS )
          {
            MSG_FATAL( "HndlDiscoverable - SetDisc failed r=%d", result, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_DISCOVERABLE_NOT_SET, NULL, 2 );
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
            ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*60*3, 
                             (PFNNOTIFY) BTApp_ClearDiscoverable, pMe );
          }
        }
        else
        {
          result = BTApp_ClearDiscoverable( pMe );
        }
        (void)POP_MENU();
        ev_processed = TRUE;
      }
      else
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    //case AVK_SELECT:
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
        switch ( sel )
        {
          case IDS_ON_TIMED:
            sel = IDS_OFF;
            break;
          case IDS_OFF:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleSppTestMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSppTestMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;

  MSG_FATAL( "HndlSppTest - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_CLR:
      BTApp_HandleClearKey( pMe );
      break;

    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_UP:
    case AVK_DOWN:
      break;  // do nothing

    default:
      ev_processed = FALSE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleSppSettingsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSppSettingsMenu( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  boolean ev_processed = FALSE;
  uint16  sel          = IDS_SPP_DATA_SIZE_THOUSAND;
  uint32  uNumBufs     = pMe->mSPP.uNumDataBufs;

  MSG_FATAL( "HndlSPPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      ev_processed = TRUE;
      break;
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_SPP_DATA_SIZE_THOUSAND:
            sel = IDS_SPP_DATA_SIZE_MILLION;
            break;
          case IDS_SPP_DATA_SIZE_MILLION:
            sel = IDS_SPP_DATA_SIZE_BILLION;
            break;
          case IDS_SPP_DATA_SIZE_BILLION:
            sel = 0;
            break;
        }
      }

      switch ( selection )
      {
        case IDS_SPP_DATA_SIZE_THOUSAND:
          uNumBufs = 1;
          break;
        case IDS_SPP_DATA_SIZE_MILLION:
          uNumBufs = 1000;
          break;
        case IDS_SPP_DATA_SIZE_BILLION:
          uNumBufs = 1000*1000;
          break;
        default:
          MSG_FATAL( "HndlSPPSettings - sel=%d", selection, 0, 0 );
          uNumBufs = 1000; // default is 2M
          break;
      }
      if ( pMe->mSPP.uNumDataBufs != uNumBufs )
      {
        pMe->mSPP.uNumDataBufs = uNumBufs;
        pMe->bConfigChanged    = TRUE;
      }

      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleSvcRspMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSvcRspMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;

  MSG_FATAL( "HndlSvcRsp - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      ev_processed = TRUE;
      break;
    case AVK_UP:
    case AVK_DOWN:
      ev_processed = ISTATIC_HandleEvent( pMe->m_pStatic, EVT_KEY, key, 0 );
      break;
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleSettingsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSettingsMenu( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  boolean ev_processed = TRUE;
#ifdef FEATURE_BT_EXTPF_BIP
  uint16  sel = IDS_BIP_REGISTER_IMAGING_RESPONDER;
#endif

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    key = AVK_SELECT;
  }
  else
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MSG_FATAL( "HndlSettingsEnabling - k=0x%x sel=%x", key, selection, 0 );
  MENU_SET_SEL( selection );
  
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      break;
    case AVK_UP:
    case AVK_DOWN:
      ev_processed = ISTATIC_HandleEvent( pMe->m_pStatic, EVT_KEY, key, 0 );
      break;
    case AVK_CLR:
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
	case AVK_INFO:	//Add By zzg 2010_11_03    
    case AVK_SELECT:
    {
      switch( selection )
      {
        case IDS_AUDIO_GATEWAY:
        {
          MSG_FATAL( "HndlSlction - AUDIO_GATEWAY m=%d", TOP_MENU, 0, 0 );
          switch ( TOP_MENU )
          {
            case BT_APP_MENU_SETTINGS:
            {
              BTApp_BuildMenu( pMe, BT_APP_MENU_AG_SETTINGS );
              break;
            }
            case BT_APP_MENU_ENABLING:
            {
              pMe->bConfigChanged = TRUE;
              pMe->mAG.bSelected = pMe->mAG.bSelected ? FALSE : TRUE;
              BTApp_UpdateMenuItemImage( 
                pMe->m_pIDisplay, pMe->m_pIMenu, selection,
                pMe->mAG.bSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
              break;
            }
            default:
              ev_processed = FALSE;
              break;
          }
          break;
        }
        case IDS_NETWORK_ACCESS:
        {
          MSG_FATAL( "HndlSlction - NETWORK_ACCESS m=%d", TOP_MENU, 0, 0 );
          switch ( TOP_MENU )
          {
            case BT_APP_MENU_SETTINGS:
              BTApp_BuildMenu( pMe, BT_APP_MENU_NA_SETTINGS );
              break;
            case BT_APP_MENU_ENABLING:
              pMe->bConfigChanged = TRUE;
              pMe->mNA.bSelected = pMe->mNA.bSelected ? FALSE : TRUE;
              BTApp_UpdateMenuItemImage( 
                pMe->m_pIDisplay, pMe->m_pIMenu, selection,
                pMe->mNA.bSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
              break;
            default:
              ev_processed = FALSE;
              break;
          }
          break;
        }
#ifdef FEATURE_BT_EXTPF_OPP
        case IDS_ENABLE_OPP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mOPP.bServerEnable = pMe->mOPP.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mOPP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
        case IDS_SETTINGS_FTP:
        {
          MSG_FATAL( "HndlSlction - SETTINGS_FTP m=%d", TOP_MENU, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_ENABLING );
          break;
        }
        case IDS_FTP_SRV_ALLOW_BROWSE:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mFTP.reg.bBrowseAllowed = 
            pMe->mFTP.reg.bBrowseAllowed ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.reg.bBrowseAllowed ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_FTP_SRV_READ_ONLY:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mFTP.reg.bReadOnly = pMe->mFTP.reg.bReadOnly ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.reg.bReadOnly ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_ENABLE_FTP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mFTP.bServerEnable = pMe->mFTP.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
        case IDS_SETTINGS_BIP:
        {
          MSG_FATAL( "HndlSlction - SETTINGS_BIP m=%d", TOP_MENU, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_BIP_ENABLING );
          break;
        }
        case IDS_BIP_REGISTER_IMAGING_RESPONDER:
        case IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER:
        case IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER:
        {
          // update menu
          while ( sel != 0 )
          {
            BTApp_UpdateMenuItemImage( 
              pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
              (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
            switch ( sel )
            {
              case IDS_BIP_REGISTER_IMAGING_RESPONDER:
              {
                sel = IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER;
                break;
              }
              case IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER:
              {
                sel = IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER;
                break;
              }
              case IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER:
              {
                sel = 0;
                break;
              }
            }
          }
          // save the selection
          switch ( selection )
          {
            case IDS_BIP_REGISTER_IMAGING_RESPONDER:
            {
              pMe->bConfigChanged = TRUE;
              pMe->mBIP.serviceSelection = AEEBT_BIP_IMAGING_RESPONDER;
              break;
            }
            case IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER:
            {
              pMe->bConfigChanged = TRUE;
              pMe->mBIP.serviceSelection = AEEBT_BIP_ARCHIVED_OBJECTS;
              break;
            }
            case IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER:
            {
              pMe->bConfigChanged = TRUE;
              pMe->mBIP.serviceSelection = AEEBT_BIP_REFERENCED_OBJECTS;
              break;
            }
            default:
              MSG_FATAL( "HndlDevType - sel=%d", selection, 0, 0 );
              break;
          }
          break;
        }
        case IDS_ENABLE_BIP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mBIP.bServerEnable = pMe->mBIP.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mBIP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }        
#endif
#ifdef FEATURE_BT_EXTPF_BPP
        case IDS_ENABLE_BPP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mBPP.bServerEnable = pMe->mBPP.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mBPP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }        
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
        case IDS_ENABLE_PBAP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mPBAP.bServerEnable = pMe->mPBAP.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mPBAP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }        
#endif
#ifdef FEATURE_IOBEX
        case IDS_ENABLE_OBEX:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mOBEX.bServerEnable = pMe->mOBEX.bServerEnable ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mOBEX.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }        
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
        case IDS_ENABLE_A2DP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mA2DP.bEnableA2DP = pMe->mA2DP.bEnableA2DP ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mA2DP.bEnableA2DP ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_ENABLE_AVRCP:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mAVRCP.bEnableAVRCP = pMe->mAVRCP.bEnableAVRCP ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mAVRCP.bEnableAVRCP ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
#endif  //FEATURE_BT_EXTPF_AV
        case IDS_ENABLE_L2:
        {
          pMe->bConfigChanged = TRUE;
          pMe->mL2.bEnableL2Srv = pMe->mL2.bEnableL2Srv ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mL2.bEnableL2Srv ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
        case IDS_ENABLING:
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_ENABLING );
          break;
        }
        case IDS_DFLT_DIAL_STR:
          ev_processed = BTApp_BuildMenu( pMe, BT_APP_MENU_DFLT_DIAL_STR );
          break;
#ifdef FEATURE_BT_VT
        case IDS_ENABLE_VIDEO_CALL:
        {
          pMe->mAG.bVideoCallEnabled = TRUE;
          ev_processed = BTApp_BuildTopMenu( pMe );
          break;
        }
        case IDS_DISABLE_VIDEO_CALL:
        {
          pMe->mAG.bVideoCallEnabled = FALSE;
          ev_processed = BTApp_BuildTopMenu( pMe );
          break;
        }
#endif //FEATURE_BT_VT
#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
        case IDS_SERV_SECURITY:
        {
          ev_processed = BTApp_BuildMenu( pMe, BT_APP_MENU_SVR_SEC_MAIN_TYPE ); 
          break;
        }
#endif /* FEATURE_BT_2_1 */
        default:
          ev_processed = FALSE;
          break;
      }
      break;
    }
    default:
      ev_processed = FALSE;
      break;   
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleScanParamsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleScanParamsMenu( 
  CBTApp*       pMe, 
  BTAppMenuType menu, 
  uint16        key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  uint16  sel = IDS_SCAN_PARAM_DEFAULT;
  AEEBTActVsPwr scanParam;

  MSG_FATAL( "HndlScanParams - m=%d k=0x%x", menu, key, 0 );

  if ( (menu != BT_APP_MENU_PAGE_SCAN) && (menu != BT_APP_MENU_INQ_SCAN) )
  {
    MSG_FATAL( "HndlScanParams - unexpected menu", 0, 0, 0 );
    return FALSE;
  }

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      if ( key == AVK_LEFT )
      {
        BTApp_DoScanParamsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_DoScanParamsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
	case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_SCAN_PARAM_DEFAULT:
            sel = IDS_SCAN_PARAM_SMALLEST_CONN_TIME;
            break;
          case IDS_SCAN_PARAM_SMALLEST_CONN_TIME:
            sel = IDS_SCAN_PARAM_LARGER_CONN_TIME;
            break;
          case IDS_SCAN_PARAM_LARGER_CONN_TIME:
            sel = IDS_SCAN_PARAM_LARGEST_CONN_TIME;
            break;
          case IDS_SCAN_PARAM_LARGEST_CONN_TIME:
            sel = 0;
            break;
        }
      }

      switch ( selection )
      {
        case IDS_SCAN_PARAM_DEFAULT:
          scanParam = AEEBT_RM_AVP_AUTOMATIC;
          break;
        case IDS_SCAN_PARAM_SMALLEST_CONN_TIME:
          scanParam = AEEBT_RM_AVP_SMALLEST_ACT_MOST_POWER;
          break;
        case IDS_SCAN_PARAM_LARGER_CONN_TIME:
          scanParam = AEEBT_RM_AVP_LARGER_ACT_LESS_POWER;
          break;
        case IDS_SCAN_PARAM_LARGEST_CONN_TIME:
          scanParam = AEEBT_RM_AVP_LARGEST_ACT_LEAST_POWER;
          break;
        default:
          MSG_FATAL( "HndlScanParams - sel=%d", selection, 0, 0 );
          scanParam = AEEBT_RM_AVP_AUTOMATIC;
          break;
      }
      ev_processed = TRUE;

      if ( menu == BT_APP_MENU_PAGE_SCAN )
      {
        if ( pMe->mRM.pageScanParam != scanParam )
        {
          pMe->mRM.pageScanParam = scanParam;
          pMe->bConfigChanged    = TRUE;
        }
      }
      else
      {
        if ( pMe->mRM.inquiryScanParam != scanParam )
        {
          pMe->mRM.inquiryScanParam = scanParam;
          pMe->bConfigChanged       = TRUE;
        }
      }

      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_BuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  AEEBTDeviceInfo* pDev;
  IDialog*  pCurrentDialog;

  return built;	//Add By zzg 2011_01_08

  IDISPLAY_Backlight( pMe->m_pIDisplay, TRUE );

  MSG_FATAL( "BuildMenu - m=%d tm=%d", menu, TOP_MENU, 0 );
  MSG_FATAL("BuildMenu - m=%d tm=%d", menu, TOP_MENU, 0);
  ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY) BTApp_MessageTimerExpired, 
                      pMe );

  pMe->bBusyIconUp = FALSE;
  pMe->uCurrMsgId  = 0;

  IDISPLAY_Backlight( pMe->m_pIDisplay, TRUE );

 
  if ( (menu == TOP_MENU) && (menu == BT_APP_MENU_PASSKEY) )
  {
    if ( (pCurrentDialog = ISHELL_GetActiveDialog( pMe->m_pShell )) != NULL )
    {
      //Add By zzg 2010_12_29
      if(pMe->m_pIAnn != NULL)
      {
          //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
      }  
	  //Add End
	  
      IDIALOG_Redraw( pCurrentDialog );
      return built;      
    }
  }
  
  // rebuilding the top menu?
  if ( (menu == TOP_MENU) && (menu != BT_APP_MENU_DEV_RESP) )
  {
    pMe->uTopMenu--;
  }

  switch ( menu )
  {
    case BT_APP_MENU_MAIN:
      MSG_FATAL("BT_APP_MENU_MAIN   BTApp_BuildMainMenu",0,0,0);
	  BTApp_BuildMainMenu( pMe );      
      break;	
    case BT_APP_MENU_SEARCH:
      BTApp_BuildDeviceSearchMenu( pMe );
      break;
    case BT_APP_MENU_DEVICE:
      BTApp_BuildDevicesMenu( pMe );
      break;
    case BT_APP_MENU_MY_INFO:	  
      BTApp_BuildMyInfoMenu( pMe );
      break;
    case BT_APP_MENU_SETTINGS:
      BTApp_BuildSettingsMenu( pMe );
      break;
  case BT_APP_MENU_TESTS:
      pMe->mEnablingType = BTAPP_ENABLING_NONE;
      BTApp_BuildTestsMenu( pMe );
      break;
    case BT_APP_MENU_EDIT_NAME:
      PUSH_MENU( BT_APP_MENU_EDIT_NAME );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_EDIT_SHORT_NAME:
      PUSH_MENU( BT_APP_MENU_EDIT_SHORT_NAME );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
    case BT_APP_MENU_EDIT_MANU_DATA:
      PUSH_MENU( BT_APP_MENU_EDIT_MANU_DATA );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
    case BT_APP_MENU_IOC_TYPE:
      PUSH_MENU( BT_APP_MENU_IOC_TYPE );
      BTApp_BuildIOCapabilityMenu ( pMe );
      break;
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
 case BT_APP_MENU_SECURITY_OPTIONS:
    {
      built = BTApp_BuildSecurityOptionsMenu( pMe );
      break;
    }
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_AG_SETTINGS:
      BTApp_BuildAGSettingsMenu( pMe );
      break;
    case BT_APP_MENU_NA_SETTINGS:
      BTApp_BuildNASettingsMenu( pMe );
      break;
#ifdef FEATURE_BT_EXTPF_FTP      
    case BT_APP_MENU_FTP_ENABLING:
      BTApp_FTPBuildEnableMenu( pMe );
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_ENABLING:
      BTApp_BIPBuildEnableMenu( pMe );
      break;
#endif      
    case BT_APP_MENU_ENABLING:
      BTApp_BuildEnablingMenu( pMe );
      break;
    case BT_APP_MENU_DISCOVERABLE:
      BTApp_BuildDiscoverableMenu( pMe );
      break;
#ifdef FEATURE_PHONE_VR
    case BT_APP_MENU_VR_CAPABLE:
      BTApp_BuildVRCapableMenu( pMe );
      break;
#endif
    case BT_APP_MENU_DFLT_DIAL_STR:
      PUSH_MENU( BT_APP_MENU_DFLT_DIAL_STR );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
    case BT_APP_MENU_DEV_TYPE:
      BTApp_BuildDevTypeMenu( pMe );
      break;
    case BT_APP_MENU_CONN_MODE:
      BTApp_BuildConnModeMenu( pMe );
      break;
    case BT_APP_MENU_IDLE_TO:
      BTApp_BuildListMenu( pMe, BT_APP_MENU_IDLE_TO );
      break;
    case BT_APP_MENU_SPKR_VOL:
      BTApp_BuildListMenu( pMe, BT_APP_MENU_SPKR_VOL );
      break;
    case BT_APP_MENU_MIC_VOL:
      BTApp_BuildListMenu( pMe, BT_APP_MENU_MIC_VOL );
      break;
    case BT_APP_MENU_HS_BUTTON_HNDLR:
      BTApp_BuildHSButtonHndlrMenu( pMe );
      break;
    case BT_APP_MENU_LIST_CONNECTED:
      BTApp_BuildDeviceList( pMe, BT_APP_MENU_LIST_CONNECTED );
      break;
    case BT_APP_MENU_LIST_BONDED:
      BTApp_BuildDeviceList( pMe, BT_APP_MENU_LIST_BONDED );
      break;
    case BT_APP_MENU_LIST_ALL:
      BTApp_BuildDeviceList( pMe, BT_APP_MENU_LIST_ALL );
      break;
    case BT_APP_MENU_LIST_OBEX_SERVERS:
      BTApp_BuildDeviceList( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      break;
    case BT_APP_MENU_LIST_TELEPHONY:
      BTApp_BuildDeviceList( pMe, BT_APP_MENU_LIST_TELEPHONY );
      break;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_REBOND:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_REBOND );
      break;
    case BT_APP_MENU_DBG_KEY:
      BTApp_BuildDBGKeyHndlrMenu( pMe );
      break;
    case BT_APP_MENU_SVR_SEC_MAIN_TYPE:
      BTApp_BuildSvcSecMainMenu( pMe );
      break;
    case BT_APP_MENU_SVR_SEC_TYPE:
      BTApp_BuildSvcSecMenu( pMe );
      break;
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_UNBOND_ALL:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_UNBOND_ALL );
      break;
    case BT_APP_MENU_REMOVE_ALL:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_REMOVE_ALL );
      break;
    case BT_APP_MENU_DEV_RESP:
      BTApp_BuildDevRespMenu( pMe );
      break;
    case BT_APP_MENU_DEV_INFO:
      BTApp_BuildDevInfo( pMe );
      break;
    case BT_APP_MENU_DEV_OPTIONS:
      BTApp_BuildDevOptions( pMe );
      break;
    case BT_APP_MENU_PASSKEY:
	  
#ifdef FEATURE_BT_2_1
      if ( IBTEXTRM_GetLocalInfo( pMe->mRM.po,&pMe->mRM.myInfo ) != SUCCESS )
      {
      	MSG_FATAL("***zzg IBTEXTRM_GetLocalInfo Failed!***", 0, 0, 0);
        MSG_FATAL( "Get Local Device Information failed!!! ", 0, 0, 0 );
      }
      else
      {
        MSG_FATAL("Local HC LMPVersion = %d",pMe->mRM.myInfo.uLMPVersion, 0, 0 );

		MSG_FATAL("***zzg Local HC LMPVersion = %d***", pMe->mRM.myInfo.uLMPVersion, 0, 0);
		
        if(pMe->mRM.myInfo.uLMPVersion > BTAPP_BT_LMP_VER_2_0)
        {
          MSG_FATAL("***zzg LMPVersion > BTAPP_BT_LMP_VER_2_0!***", 0, 0, 0);
			
          // Host is 2.1 enabled and Host Controller is 2.1      		  
          BTApp_SetBondable( pMe );//ACG
          BTApp_BuildBondMenu( pMe );
        }
        else
        {
        
          MSG_FATAL("***zzg IBTEXTRM_GetLocalInfo Failed!***", 0, 0, 0);
          // Host is 2.1 but, Host Controller is non 2.1          
         
		  PUSH_MENU( BT_APP_MENU_PASSKEY );
          built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE,IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
		}
      }
#else
	  PUSH_MENU( BT_APP_MENU_PASSKEY );     	  
	  
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                   IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
#endif /* FEATURE_BT_2_1 */
      break;
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_DISPLAY_PASSKEY:
      BTApp_BuildDisplayPasskeyMenu( pMe );
      break;
    case BT_APP_MENU_DEV_BOND_OPTIONS:
      BTApp_BuildBondOptionsMenu( pMe );
      break;
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_BROWSE_SVC_RESP:
      BTApp_BuildBrowseRespScreen( pMe );
      break;
    case BT_APP_MENU_SEARCH_SVC_RESP:
      BTApp_BuildSearchRespScreen( pMe );
      break;
    case BT_APP_MENU_UNBOND_ONE:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_UNBOND_ONE );
      break;
    case BT_APP_MENU_REMOVE_ONE:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_REMOVE_ONE );
      break;
    case BT_APP_MENU_USE_AUDIO_DEV:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_USE_AUDIO_DEV );
      break;
    case BT_APP_MENU_DEV_SECURITY:
      pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
      BTApp_BuildSecurityMenu( pMe, pDev->security, BT_APP_MENU_DEV_SECURITY );
      break;
    case BT_APP_MENU_SECURITY:
      BTApp_BuildSecurityMenu( pMe, pMe->mRM.myInfo.security, 
                               BT_APP_MENU_SECURITY );
      break;
    case BT_APP_MENU_SPP_RESULTS:
      BTApp_BuildSppTestResults( pMe );
      break;
    case BT_APP_MENU_AG_CONNECT:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_AG_CONNECT );
      break;
    case BT_APP_MENU_AG_DISCONNECT:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_AG_DISCONNECT );
      break;
    case BT_APP_MENU_HCI:
      BTApp_BuildHCIMenu( pMe );
      break;
    case BT_APP_MENU_SCAN_PARAMS:
      BTApp_BuildScanParamsMenu( pMe );
      break;
    case BT_APP_MENU_PAGE_SCAN:
    case BT_APP_MENU_INQ_SCAN:
      BTApp_BuildScanParamsSubmenu( pMe, menu );
      break;
    case BT_APP_MENU_MASTER_CONTROL:
      BTApp_BuildMasterControlMenu( pMe );
      break;
    case BT_APP_MENU_SPP_TESTS:
      BTApp_BuildSPPTestMenu( pMe );
      break;
    case BT_APP_MENU_SPP_SETTINGS:
      BTApp_BuildSPPSettingsMenu( pMe );
      break;
    case BT_APP_MENU_OBEX:
      BTApp_BuildOBEXMenu( pMe );
      break;
    case BT_APP_MENU_TCS:
      BTApp_BuildTCSMenu( pMe );
      break;
    case BT_APP_MENU_TEST_MODE:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_TEST_MODE );
      break;
    case BT_APP_MENU_AUTHORIZE_CONN:
      BTApp_BuildPrompt( pMe, BT_APP_MENU_AUTHORIZE_CONN );
      break;
    case BT_APP_MENU_SET_CONN_ROLE:
      BTApp_BuildSetConnRoleMenu( pMe );
      break;
#ifdef FEATURE_BT_EXTPF_OPP
    case BT_APP_MENU_OPP_TESTS:
	case BT_APP_MENU_OPP_SENDFILE:	//Add By zzg 2010_11_09	
	case BT_APP_MENU_OPP_SETTING:	//Add By zzg 2010_11_17	
    case BT_APP_MENU_OPP_SERVER:
    case BT_APP_MENU_OPP_CLIENT:
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
	{
	  if (menu == BT_APP_MENU_OPP_SENDFILE)
	  {
		MSG_FATAL("***zzg BTApp_BuildMenu BT_APP_MENU_OPP_SENDFILE***", 0, 0, 0);
	  }
	  else if (menu == BT_APP_MENU_OPP_SETTING)
	  {
		MSG_FATAL("***zzg BTApp_BuildMenu BT_APP_MENU_OPP_SETTING***", 0, 0, 0);
	  }
      BTApp_OPPBuildMenu( pMe, menu );
      break;
	}
#endif
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_TESTS:
    case BT_APP_MENU_FTP_SERVER:
    case BT_APP_MENU_FTP_CLIENT:
    case BT_APP_MENU_FTP_SETTINGS:
    case BT_APP_MENU_FTP_SRV_REGISTER:
    case BT_APP_MENU_FTP_BROWSING:
    case BT_APP_MENU_FTP_CREATE_FLDR:
    case BT_APP_MENU_FTP_MOVE_TO_FLDR:
      BTApp_FTPBuildMenu( pMe, menu );
      break;
    case BT_APP_MENU_FTP_PASSWORD:
      PUSH_MENU( BT_APP_MENU_FTP_PASSWORD );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_TESTS:
    case BT_APP_MENU_BIP_SERVER:
    case BT_APP_MENU_BIP_CLIENT:
    case BT_APP_MENU_BIP_SETTINGS:
      BTApp_BIPBuildMenu( pMe, menu );
      break;
    case BT_APP_MENU_BIP_PIN:
      PUSH_MENU( BT_APP_MENU_BIP_PIN );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_TESTS:
    case BT_APP_MENU_BPP_LIST_MIME:
    case BT_APP_MENU_BPP_LIST_TARGET:
    case BT_APP_MENU_BPP_SETTINGS:
      BTApp_BPPBuildMenu( pMe, menu );
      break;
    case BT_APP_MENU_BPP_USERID:
      PUSH_MENU( BT_APP_MENU_BPP_USERID );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;      
    case BT_APP_MENU_BPP_PASSWORD:
      PUSH_MENU( BT_APP_MENU_BPP_PASSWORD );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
    case BT_APP_MENU_BPP_JOB_ID:
      PUSH_MENU( BT_APP_MENU_BPP_JOB_ID );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;  
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case BT_APP_MENU_PBAP_TESTS:
    case BT_APP_MENU_PBAP_SERVER:
    case BT_APP_MENU_PBAP_CLIENT:
    case BT_APP_MENU_PBAP_SERVER_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS:
    case BT_APP_MENU_PBAP_VCARD_FORMAT:
    case BT_APP_MENU_PBAP_FILTER:
    case BT_APP_MENU_PBAP_MAX_LIST_COUNT:
    case BT_APP_MENU_PBAP_LIST_START_OFFSET:
    case BT_APP_MENU_PBAP_SORT_ORDER:
    case BT_APP_MENU_PBAP_SEARCH_ATTR:
    case BT_APP_MENU_PBAP_SEARCH_VALUE:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU:
    case BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU:
    case BT_APP_MENU_PBAP_AUTHENTICATE:
      BTApp_PBAPBuildMenu( pMe, menu );
      break;
    case BT_APP_MENU_PBAP_USERID:
      PUSH_MENU( BT_APP_MENU_PBAP_USERID );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
    case BT_APP_MENU_PBAP_PASSWORD:
      PUSH_MENU( BT_APP_MENU_PBAP_PASSWORD );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;      
#endif
#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_TESTS:
    case BT_APP_MENU_OBEX_SERVER:
    case BT_APP_MENU_OBEX_CLIENT:
    case BT_APP_MENU_OBEX_SETTINGS:
      BTApp_OBEXBuildMenu( pMe, menu );
      break;
    case BT_APP_MENU_OBEX_USERID:
      PUSH_MENU( BT_APP_MENU_OBEX_USERID );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;      
    case BT_APP_MENU_OBEX_PASSWORD:
      PUSH_MENU( BT_APP_MENU_OBEX_PASSWORD );
      built = (ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS);
      break;
#endif 
#ifdef FEATURE_BT_EXTPF_CTP
    case BT_APP_MENU_CTP_TESTS:
      BTApp_CTPBuildMenu( pMe, menu );
      break; 
    case BT_APP_MENU_ICP_TESTS:
      BTApp_ICPBuildMenu( pMe, menu );
      break;
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    case BT_APP_MENU_A2DP_TEST:
      BTApp_BuildA2DPTestMenu( pMe );
      break;
    case BT_APP_MENU_A2DP_CONTROL:
      BTApp_BuildA2DPControlMenu( pMe );
      break;
    case BT_APP_MENU_AVRCP_TEST:
    case BT_APP_MENU_AVRCP_OP_LIST:
      BTApp_BuildAVRCPTestMenu( pMe, menu );
      break;
#endif
    case BT_APP_MENU_L2_TEST:
      BTApp_BuildL2TestMenu( pMe );
      break;
    case BT_APP_MENU_L2_RESULTS:
      BTApp_BuildL2ResultsMenu( pMe );
      break;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_BuildTopMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildTopMenu( CBTApp* pMe )
{
	return TRUE;
	
	MSG_FATAL("BTApp_BuildMenu....................19",0,0,0);
	return ( BTApp_BuildMenu( pMe, TOP_MENU ) ); 
}

/* ==========================================================================
FUNCTION BTApp_HandleClearKey
DESCRIPTION
============================================================================= */
boolean BTApp_HandleClearKey( CBTApp* pMe )
{
  boolean key_handled = TRUE;
  int result;
  boolean bBuildTopMenu = TRUE;

  return key_handled;		//Add By zzg 2011_01_14


  

  MSG_FATAL( "HndlClearKey - m=%d msgId=%d", TOP_MENU, pMe->uCurrMsgId, 0 );

  pMe->bBusyIconUp = FALSE;
  ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY) BTApp_MessageTimerExpired, 
                      pMe );

  CLEAR_SCREEN();

  IMENUCTL_SetActive( pMe->m_pIMenu, FALSE );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, FALSE );
  ISTATIC_SetActive( pMe->m_pStatic, FALSE );

  switch ( TOP_MENU )
  {
    case BT_APP_MENU_NONE:
      key_handled = FALSE;
      break;	
    case BT_APP_MENU_MAIN:
      if ( pMe->uCurrMsgId == 0 )
      {
        ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                            AEECLSID_BLUETOOTH_APP,
                            EVT_KEY, AVK_END, 0L );

		MSG_FATAL("***zzg BTApp_HandleClearKey BT_APP_MENU_MAIN bConnected=%d, bConnecting=%d, bRegistered=%d***", 
						pMe->mOPP.bConnected, pMe->mOPP.bConnecting, pMe->mOPP.bRegistered);
	
        key_handled = FALSE;
      }
      break;
    case BT_APP_MENU_TESTS:
      if ( pMe->uCurrMsgId != IDS_MSG_HCI_MODE_OFF_FIRST )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_SPP_RESULTS:
    {
      if ( pMe->uCurrMsgId == 0 ) // not clearing a message?
      {
        BTApp_EndSPPTest( pMe );
        (void)POP_MENU();
      }
      // else, just refresh the screen to clear the message
      break;
    }
    case BT_APP_MENU_PAGE_SCAN:
    case BT_APP_MENU_INQ_SCAN:
    {
      int result;
      if ( pMe->bConfigChanged )
      {
        MSG_FATAL( "Configuring app with pS=%d iS=%d", 
                 pMe->mRM.pageScanParam, pMe->mRM.inquiryScanParam, 0 );
         result = IBTEXTRM_ConfigApplication( pMe->mRM.po, FALSE,
                                              pMe->mRM.pageScanParam,
                                              pMe->mRM.inquiryScanParam,
                                              FALSE );
        if ( result != SUCCESS )
        {
          MSG_FATAL( "Failed to configure application r=%d", result, 0, 0 );
        }
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      (void)POP_MENU();
      (void)POP_MENU();
      break;
    }
    case BT_APP_MENU_PASSKEY:
    {
      if  ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    }
    case BT_APP_MENU_HS_BUTTON_HNDLR:
    {
      ISTATIC_Reset( pMe->m_pStatic );
      // deliberate continuation
    }
    case BT_APP_MENU_DEV_TYPE:
    case BT_APP_MENU_CONN_MODE:
    case BT_APP_MENU_IDLE_TO:
    case BT_APP_MENU_SPKR_VOL:
    case BT_APP_MENU_MIC_VOL:
    case BT_APP_MENU_DFLT_DIAL_STR:
    case BT_APP_MENU_ENABLING:
    case BT_APP_MENU_SPP_SETTINGS:
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_IOC_TYPE:
    case BT_APP_MENU_SVR_SEC_MAIN_TYPE:
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_ENABLING:
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_ENABLING:
#endif
#ifdef FEATURE_PHONE_VR
    case BT_APP_MENU_VR_CAPABLE:
#endif
    {
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      if ( (TOP_MENU != BT_APP_MENU_ENABLING) &&
#ifdef FEATURE_BT_EXTPF_FTP
           (TOP_MENU != BT_APP_MENU_FTP_ENABLING) &&
#endif
#ifdef FEATURE_BT_EXTPF_BIP
           (TOP_MENU != BT_APP_MENU_BIP_ENABLING) &&
#endif
           (TOP_MENU != BT_APP_MENU_SPP_SETTINGS) &&
#ifdef FEATURE_BT_2_1
           (TOP_MENU != BT_APP_MENU_SNIF_PARAM_SETTINGS) &&
           (TOP_MENU != BT_APP_MENU_LOW_POWER_MODE) &&
           (TOP_MENU != BT_APP_MENU_SVR_SEC_MAIN_TYPE) &&
#endif /* FEATURE_BT_2_1 */
           (TOP_MENU != BT_APP_MENU_DFLT_DIAL_STR) )
      {
        (void)POP_MENU();
      }
      (void)POP_MENU();
      break;
    }
    case BT_APP_MENU_NA_SETTINGS:
    {
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      (void)POP_MENU();
      break;
    }
    case BT_APP_MENU_USE_AUDIO_DEV:
      (void)POP_MENU(); // remove prompt
      if ( TOP_MENU == BT_APP_MENU_PASSKEY )
      {
        (void)POP_MENU(); // remove passkey prompt
      }
      break;
    case BT_APP_MENU_AG_CONNECT:
    case BT_APP_MENU_AG_DISCONNECT:
      (void)POP_MENU(); // remove message screen
      (void)POP_MENU(); // remove prompt
      break;
    case BT_APP_MENU_UNBOND_ONE:
    case BT_APP_MENU_UNBOND_ALL:
    case BT_APP_MENU_REMOVE_ONE:
    case BT_APP_MENU_REMOVE_ALL:
    {
      if ( pMe->uCurrMsgId != 0 )
      {
        (void)POP_MENU(); // remove message screen
      }
      (void)POP_MENU();   // remove prompt
      break;
    }
	//Add By zzg 2010_11_17
	case BT_APP_MENU_OPEN:	//If Open the BlueTooth
	{
		MSG_FATAL("***zzg BT_APP_MENU_OPP_SENDFILE HandleCleartKey bConnected=%d, bRegistered=%d***", pMe->mOPP.bConnected, pMe->mOPP.bRegistered, 0);
		
		ISHELL_CloseApplet(pMe->m_pShell, FALSE );
		break;
	}
	//Add End
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_REBOND:
    {
      //BTApp_DoRebond( pMe,FALSE ); 
      (void)POP_MENU();   // remove prompt
    }
    break;
    case BT_APP_MENU_USER_CFM_RQST:
    {
      (void)POP_MENU();   // remove prompt
      //ACG
      if ( TOP_MENU == BT_APP_MENU_REBOND )
      {
        (void)POP_MENU(); // remove rebond prompt
      }
    }
    break;
    case BT_APP_MENU_DISPLAY_PASSKEY:
    {
      (void)POP_MENU();   // remove prompt
    }
    break;
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_MASTER_CONTROL:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_SET_CONN_ROLE:
      (void)POP_MENU();
      break;
#ifdef FEATURE_BT_EXTPF_OPP	
    //Add By zzg 2010_11_10    
	case BT_APP_MENU_OPP_SENDFILE:	
	{  
		//Add By zzg 2010_11_22		
		//If client, change to server , first Disconnect
		MSG_FATAL("***zzg BT_APP_MENU_OPP_SENDFILE HandleCleartKey bConnected=%d, bConnecting=%d, bRegistered=%d***", 
										pMe->mOPP.bConnected, pMe->mOPP.bConnecting, pMe->mOPP.bRegistered);
		
		if ((pMe->mOPP.bConnected == TRUE) || ((pMe->mOPP.bConnecting == TRUE)))
		{
			if (IBTEXTOPP_Disconnect( pMe->mOPP.po) != SUCCESS)
			{
				MSG_FATAL("***zzg IBTEXTOPP_Disconnect != SUCCESS!***", 0, 0, 0);
			}	
			else 
			{
				MSG_FATAL("***zzg IBTEXTOPP_Disconnect == SUCCESS!***", 0, 0, 0);
				pMe->mOPP.bConnecting = FALSE;
			}
		}
		//Add End
		
		ISHELL_CloseApplet(pMe->m_pShell, FALSE );
		break;       
	}
	//Add End
	
	case BT_APP_MENU_OPP_SETTING:	//Add By zzg 2010_11_17  
    case BT_APP_MENU_OPP_TESTS:
    case BT_APP_MENU_OPP_SERVER:
    case BT_APP_MENU_OPP_CLIENT:
      if ( ( pMe->uCurrMsgId == 0 ) && ( !pMe->mOPP.bObjectTransfer ) )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
      if( !pMe->mOPP.bObjectTransfer )
      {
        (void)POP_MENU();
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_TESTS:
    case BT_APP_MENU_FTP_SERVER:
    case BT_APP_MENU_FTP_CLIENT:
    case BT_APP_MENU_FTP_SETTINGS:
    case BT_APP_MENU_FTP_BROWSING:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_FTP_PASSWORD:
    case BT_APP_MENU_FTP_CREATE_FLDR:
    case BT_APP_MENU_FTP_SRV_REGISTER:
    case BT_APP_MENU_FTP_MOVE_TO_FLDR:
      (void)POP_MENU();
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_TESTS:
    case BT_APP_MENU_BIP_SERVER:
    case BT_APP_MENU_BIP_CLIENT:
    case BT_APP_MENU_BIP_SETTINGS:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_BIP_PIN:
      (void)POP_MENU();
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_TESTS:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_BPP_SETTINGS:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_BPP_USERID:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
        BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_PASSWORD );
        key_handled = FALSE;
      }
      break;      
    case BT_APP_MENU_BPP_PASSWORD:
      (void)POP_MENU();
      break;
    case BT_APP_MENU_BPP_LIST_MIME:
      (void)POP_MENU(); /* BPP_TESTS */
      break;
    case BT_APP_MENU_BPP_LIST_TARGET: 
      (void)POP_MENU(); /* BPP_TESTS */
      break;
    case BT_APP_MENU_BPP_JOB_ID:
      ( void ) POP_MENU(); /* MIME */
      ( void ) POP_MENU(); /* BPP_TESTS */
      BTApp_BPPSendFile( pMe, pMe->mBPP.iDocType, pMe->mBPP.iJobId );
      if ( pMe->uCurrMsgId == 0 )
      {
        key_handled = TRUE;
      }
      else
      {
        key_handled = FALSE;
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case BT_APP_MENU_PBAP_VCARD_FORMAT:
    case BT_APP_MENU_PBAP_SORT_ORDER:
    case BT_APP_MENU_PBAP_SEARCH_ATTR:
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      (void)POP_MENU();
      (void)POP_MENU();
      break;
    case BT_APP_MENU_PBAP_TESTS:
    case BT_APP_MENU_PBAP_SERVER:
    case BT_APP_MENU_PBAP_CLIENT:
    case BT_APP_MENU_PBAP_SERVER_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU:
    case BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU:
    case BT_APP_MENU_PBAP_FILTER:
    case BT_APP_MENU_PBAP_MAX_LIST_COUNT:
    case BT_APP_MENU_PBAP_LIST_START_OFFSET:
    case BT_APP_MENU_PBAP_SEARCH_VALUE:
    case BT_APP_MENU_PBAP_PASSWORD:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_PBAP_AUTHENTICATE:
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;    
    case BT_APP_MENU_PBAP_USERID:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
        /* in case of client build the password menu */
        if ( pMe->mPBAP.bCliRegistered != FALSE )
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_PASSWORD );
          key_handled = FALSE;
        }
      }
      break;
#endif
#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_TESTS:
    case BT_APP_MENU_OBEX_SERVER:
    case BT_APP_MENU_OBEX_CLIENT:
    case BT_APP_MENU_OBEX_SETTINGS:
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_OBEX_USERID:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
        BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_PASSWORD );
        key_handled = FALSE;
      }
      break;      
    case BT_APP_MENU_OBEX_PASSWORD:
      (void)POP_MENU();
       break;
#endif
#ifdef FEATURE_BT_EXTPF_CTP
    case BT_APP_MENU_CTP_TESTS:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_ICP_TESTS:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    case BT_APP_MENU_A2DP_TEST:
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_A2DP_CONTROL:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
    case BT_APP_MENU_A2DP_CONTROL_DATA:
    case BT_APP_MENU_AVRCP_OP_LIST:
      (void)POP_MENU();
      break;
    case BT_APP_MENU_AVRCP_TEST:
      if ( pMe->uCurrMsgId == 0 )
      {
        (void)POP_MENU();
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_SECURITY_OPTIONS:
    {
      (void)POP_MENU();
      if ( pMe->bConfigChanged )
      {
        pMe->bConfigChanged = FALSE;
        BTApp_WriteConfigFile( pMe );
      }
      key_handled = BTApp_BuildTopMenu( pMe );
      break;   
    }
#endif
    default:
    {
      switch ( pMe->uCurrMsgId )
      {
        case IDS_MSG_SEARCHING:
          if ( pMe->mSD.bDiscovering )
          {
          IBTEXTSD_StopDeviceDiscovery( pMe->mSD.po );
          }
          BTApp_CancelDevNameRequest( pMe );

          (void)POP_MENU();
          break;
        case IDS_MSG_BROWSING:
        {
          result = IBTEXTSD_BrowseServiceCancel( 
                   pMe->mSD.po, 
                   &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr );
          if ( result == SUCCESS )
          {
            BTApp_ShowBusyIcon( pMe ); //wait for CANCELED event
            bBuildTopMenu = FALSE;
          }
          else
          {
            MSG_FATAL( "BrowseServiceCancel failed - r=%d", result, 0, 0 );
          }
          break;
        }
        case IDS_MSG_SPP_OPEN_FAILED:
          BTApp_EndSPPTest( pMe );
          break;
        case IDS_MSG_TEST_MODE:
        {
          int result;
          if ( (result = IBTEXTRM_ExitTestMode( pMe->mRM.po )) != SUCCESS )
          {
            ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                  IDS_MSG_TEST_MODE_NOT_EXITED, 
                                  pMe->pText2, 
                                  SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );
            WSPRINTF( pMe->pText1, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ), 
                      pMe->pText2, result );
            BTApp_ShowMessage( pMe, 0, pMe->pText1, 0 );
          }
          break;
        }

        case  IDS_MSG_OOB_DATA_CREATED:
          bBuildTopMenu = TRUE;
          break;

        default:
          (void)POP_MENU();
          break;
      }
      break;
    }
  }

  pMe->uCurrMsgId = 0;

  if ( key_handled )
  {
    if ( pMe->uTopMenu > MENU_STACK_DEPTH )
    {
      pMe->uTopMenu = 0;
    }

    if ( bBuildTopMenu )
    {
      BTApp_BuildTopMenu( pMe );
    }
  }

  return key_handled;
}

/* ==========================================================================
FUNCTION BTApp_MenuHandleEvent
DESCRIPTION
============================================================================= */
static boolean BTApp_MenuHandleEvent
(
  void      *pUser, 
  AEEEvent  evt, 
  uint16    wParam, 
  uint32    dw
)
{
  CBTApp*   pMe = (CBTApp *)pUser;
  boolean   ev_processed = FALSE;
  IMenuCtl* pMenu = pMe->m_pIMenu;  
  uint16    selection;

  boolean   bInStatic = FALSE;	//Add By zzg 2010_11_03
  
  MSG_FATAL( "MenuHndlEv - wP=0x%x dw=0x%x m=%d", wParam, dw, TOP_MENU );


  if ( (wParam != AVK_CLR) && (pMe->uCurrMsgId != 0) )
  {
    return TRUE;
  }

  switch ( TOP_MENU )
  {
#ifdef FEATURE_PHONE_VR
    case BT_APP_MENU_VR_CAPABLE:
    {
      return (BTApp_HandleVRCapableMenu( pMe, wParam ));
    }
#endif
    case BT_APP_MENU_PASSKEY:
    {
      return ( BTApp_TextEditHandleEvent( pUser, evt, wParam, dw ) );
    }
    case BT_APP_MENU_DEV_TYPE:
    {
      return (BTApp_HandleDevTypeMenu( pMe, wParam ));
    }
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_IOC_TYPE:
    {
      return (BTApp_HandleIOCMenu( pMe, wParam ));
    }
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_CONN_MODE:
    {
      return (BTApp_HandleConnModeMenu( pMe, wParam ));
    }
    case BT_APP_MENU_IDLE_TO:
    case BT_APP_MENU_SPKR_VOL:
    case BT_APP_MENU_MIC_VOL:
    {
      return (BTApp_HandleListMenu( pMe, wParam ));
    }
    case BT_APP_MENU_HS_BUTTON_HNDLR:
    {
      return (BTApp_HandleHSButtonHndlrMenu( pMe, wParam ));
    }
    case BT_APP_MENU_LIST_CONNECTED:
    case BT_APP_MENU_LIST_BONDED:
    case BT_APP_MENU_LIST_ALL:
    case BT_APP_MENU_LIST_OBEX_SERVERS:
    case BT_APP_MENU_LIST_TELEPHONY:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    {
      return (BTApp_HandleDevListMenu( pMe, wParam ));
    }
    case BT_APP_MENU_DEV_RESP:
    {
      return (BTApp_HandleDevRespMenu( pMe, wParam ));
    }
    case BT_APP_MENU_DEV_INFO:
    {
      return (BTApp_HandleDevInfoMenu( pMe, wParam ));
    }
    case BT_APP_MENU_DEV_SECURITY:
    case BT_APP_MENU_SECURITY:
    {
      return (BTApp_HandleSecurityMenu( pMe, wParam ));
    }
    case BT_APP_MENU_DISCOVERABLE:
    {
      return (BTApp_HandleDiscoverableMenu( pMe, wParam ));
    }
    case BT_APP_MENU_SPP_RESULTS:
    {
      return (BTApp_HandleSppTestMenu( pMe, wParam ));
    }
    case BT_APP_MENU_SPP_SETTINGS:
    {
      return (BTApp_HandleSppSettingsMenu( pMe, wParam ));
    }
    case BT_APP_MENU_BROWSE_SVC_RESP:
    case BT_APP_MENU_SEARCH_SVC_RESP:
    {
      return (BTApp_HandleSvcRspMenu( pMe, wParam ));
    }
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_DEV_BOND_OPTIONS:
    {
      return ( BTApp_HandleBondOptionsMenu( pMe, wParam ) );
    }
    case BT_APP_MENU_DBG_KEY:
    {
      return ( BTApp_HandleDbgKey( pMe,wParam ) );
    }
    case BT_APP_MENU_SVR_SEC_MAIN_TYPE:
    {
      return (BTApp_HandleSvrMainSec( pMe, wParam ) );
    }
    case BT_APP_MENU_SVR_SEC_TYPE:
    {
      return (BTApp_HandleSvrSec( pMe, wParam ) );
    }
    case BT_APP_MENU_SECURITY_OPTIONS:
    {
      return ( BTApp_HandleServiceSecurityOptionsMenu( pMe, wParam ) );
    }
   
#endif /* FEATURE_BT_2_1 */
    case BT_APP_MENU_SETTINGS:
    case BT_APP_MENU_ENABLING:
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_ENABLING:
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_ENABLING:
#endif
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_SNIF_PARAM_SETTINGS:
#endif /* FEATURE_BT_2_1 */
    {
      return (BTApp_HandleSettingsMenu( pMe, wParam ));
    }
#ifdef FEATURE_BT_EXTPF_OPP
	case BT_APP_MENU_OPP_SENDFILE:	//Add By zzg 2010_11_09
	case BT_APP_MENU_OPP_SETTING:	//Add By zzg 2010_11_17
    case BT_APP_MENU_OPP_TESTS:
    case BT_APP_MENU_OPP_SERVER:
    case BT_APP_MENU_OPP_CLIENT:
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
    {
      return BTApp_OPPHandleMenus( pMe, wParam, TOP_MENU );
    }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
    case BT_APP_MENU_FTP_TESTS:
    case BT_APP_MENU_FTP_SERVER:
    case BT_APP_MENU_FTP_CLIENT:
    case BT_APP_MENU_FTP_SETTINGS:
    case BT_APP_MENU_FTP_SRV_REGISTER:
    case BT_APP_MENU_FTP_BROWSING:  
    {
      return BTApp_FTPHandleMenus( pMe, wParam, TOP_MENU );
    }
    case BT_APP_MENU_FTP_CREATE_FLDR:
    {
      return ( BTApp_TextEditHandleEvent( pUser, evt, wParam, dw ) );
    }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case BT_APP_MENU_BIP_TESTS:
    case BT_APP_MENU_BIP_SERVER:
    case BT_APP_MENU_BIP_CLIENT:
    case BT_APP_MENU_BIP_SETTINGS:
    {
      return BTApp_BIPHandleMenus( pMe, wParam, TOP_MENU );
    }
    case BT_APP_MENU_BIP_PIN:
    {
      return ( BTApp_TextEditHandleEvent( pUser, evt, wParam, dw ) );
    }    
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case BT_APP_MENU_BPP_TESTS:
    case BT_APP_MENU_BPP_SETTINGS:
    case BT_APP_MENU_BPP_LIST_MIME:    
    case BT_APP_MENU_BPP_LIST_TARGET: 
    {
      return BTApp_BPPHandleMenus( pMe, wParam, TOP_MENU );
    }
    case BT_APP_MENU_BPP_PASSWORD:
    case BT_APP_MENU_BPP_JOB_ID:
    case BT_APP_MENU_BPP_USERID:
    {
      return ( BTApp_TextEditHandleEvent( pUser, evt, wParam, dw ) );
    }   
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case BT_APP_MENU_PBAP_TESTS:
    case BT_APP_MENU_PBAP_SERVER:
    case BT_APP_MENU_PBAP_CLIENT:
    case BT_APP_MENU_PBAP_SERVER_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS:
    case BT_APP_MENU_PBAP_VCARD_FORMAT:
    case BT_APP_MENU_PBAP_SORT_ORDER:
    case BT_APP_MENU_PBAP_SEARCH_ATTR:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD:
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU:
    case BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU:
    case BT_APP_MENU_PBAP_AUTHENTICATE:
    {
      return BTApp_PBAPHandleMenus( pMe, wParam, TOP_MENU );
    }
#endif
#ifdef FEATURE_IOBEX
    case BT_APP_MENU_OBEX_TESTS:
    case BT_APP_MENU_OBEX_SERVER:
    case BT_APP_MENU_OBEX_CLIENT:
    case BT_APP_MENU_OBEX_SETTINGS:
    {
      return BTApp_OBEXHandleMenus( pMe, wParam, TOP_MENU );
    }
    case BT_APP_MENU_OBEX_PASSWORD:
    case BT_APP_MENU_OBEX_USERID:
    {
      return ( BTApp_TextEditHandleEvent( pUser, evt, wParam, dw ) );
    }    
#endif
#ifdef FEATURE_BT_EXTPF_CTP
    case BT_APP_MENU_CTP_TESTS: 
    {
      return BTApp_CTPHandleMenus( pMe, wParam, TOP_MENU );
    }
    case BT_APP_MENU_ICP_TESTS: 
    {
      return BTApp_ICPHandleMenus( pMe, wParam, TOP_MENU );
    }
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    case BT_APP_MENU_A2DP_TEST:
    {
      return BTApp_HandleA2DPTestMenu( pMe, wParam );
    }
    case BT_APP_MENU_A2DP_CONTROL:
    {
      return BTApp_HandleA2DPControlMenu( pMe, wParam );
    }
    case BT_APP_MENU_AVRCP_TEST:
    case BT_APP_MENU_AVRCP_OP_LIST:
    {
      return BTApp_HandleAVRCPTestMenu( pMe, wParam, TOP_MENU );
    }
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    case BT_APP_MENU_PAGE_SCAN:
    case BT_APP_MENU_INQ_SCAN:
    {
      return (BTApp_HandleScanParamsMenu( pMe, TOP_MENU, wParam ));
    }
    case BT_APP_MENU_MY_INFO:
    case BT_APP_MENU_UNBOND_ALL:
	case BT_APP_MENU_OPEN:			//Add By zzg 2010_11_17
    case BT_APP_MENU_REMOVE_ALL:
    case BT_APP_MENU_UNBOND_ONE:
    case BT_APP_MENU_REMOVE_ONE:
    case BT_APP_MENU_USE_AUDIO_DEV:
    case BT_APP_MENU_AG_CONNECT:
    case BT_APP_MENU_AG_DISCONNECT:
    case BT_APP_MENU_AUTHORIZE_CONN:
    case BT_APP_MENU_TEST_MODE:
    case BT_APP_MENU_SET_CONN_ROLE:	
#ifdef FEATURE_BT_2_1
    case BT_APP_MENU_REBOND:
    case BT_APP_MENU_USER_CFM_RQST:
    case BT_APP_MENU_DISPLAY_PASSKEY:
    case BT_APP_MENU_SNIF_PARAMS:
#endif /* FEATURE_BT_2_1 */
    {
      pMenu = pMe->m_pISoftMenu;
	  bInStatic = TRUE;				//Add By zzg 2010_11_03
      break;
    }
    case BT_APP_MENU_L2_TEST:
    {
      return BTApp_HandleL2TestMenu( pMe, wParam );
    }
    case BT_APP_MENU_L2_RESULTS:
    {
      return BTApp_HandleL2ResultsMenu( pMe, wParam );
    }
    default:
    {
      pMenu = pMe->m_pIMenu;
      break;
    }
  }


  //Add By zzg 2010_11_03
  if (TOP_MENU == BT_APP_MENU_DEV_RESP)
  {
	  bInStatic = TRUE;				
  }
  //Add End
  

  //Add  By zzg 2010_11_03
  if (TRUE == bInStatic)
  {
	 if ((wParam == AVK_UP) || (wParam == AVK_DOWN))
	 {
		ev_processed = ISTATIC_HandleEvent(pMe->m_pStatic, EVT_KEY, wParam, 0);     		
	 }
  }
  //AddEnd

  if ( (pMenu == pMe->m_pIMenu) &&
       ((selection = BTApp_NumKey2Selection( pMenu, wParam )) != 0) )
  {
    ev_processed = BTApp_HandleSelection( pMe, selection );
  }
  else
  {
    switch ( wParam )
    {
      case AVK_CLR:
        ev_processed = BTApp_HandleClearKey( pMe ); // rebuild previous menu
        break;
	  case AVK_INFO:		//Add By zzg 2010_11_01
      case AVK_SELECT:
        MSG_FATAL( "MenuHndlEv - SELECT key, m=%d", TOP_MENU, 0, 0 );
        selection = IMENUCTL_GetSel( pMenu );
		MSG_FATAL( "MenuHndlEv - SELECT key, selection=%d", selection, 0, 0 );
        ev_processed = BTApp_HandleSelection( pMe, selection );
        break;
      case AVK_UP:
      case AVK_DOWN:
        if ( pMenu == pMe->m_pISoftMenu )
        {
          ev_processed = TRUE;
        }
        break;
      case AVK_LEFT:
      case AVK_RIGHT:
        if ( pMenu == pMe->m_pIMenu )
        {
          ev_processed = TRUE;
        }
        break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_KeysDisabled
DESCRIPTION
============================================================================= */
static boolean BTApp_KeysDisabled( CBTApp* pMe, uint16 key )
{
  boolean bKeyDisabled = FALSE;

#ifdef FEATURE_BT_EXTPF_PBAP
  if ( ( (TOP_MENU == BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU) ||
       (TOP_MENU == BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU) ) && 
       (pMe->bBusyIconUp != FALSE) && (key == AVK_CLR) )
  {
    bKeyDisabled = FALSE;
  }
  else   
#endif /* FEATURE_BT_EXTPF_PBAP */
  if ( pMe->bBusyIconUp != FALSE )
  {
    MSG_FATAL( "KeysDisabled - Busy Icon up, keys disabled", 0, 0, 0 );
    bKeyDisabled = TRUE;
  }
  else if ( (pMe->uCurrMsgId == IDS_MSG_SEARCHING) ||
            (pMe->uCurrMsgId == IDS_MSG_BROWSING) )
  {
    if ( key != AVK_CLR )
    {
      bKeyDisabled = TRUE;
    }
  }
#ifdef FEATURE_BT_EXTPF_FTP
  else if ( (TOP_MENU == BT_APP_MENU_FTP_BROWSING) && 
            (pMe->mFTP.bObjectTransfer == TRUE) && (key != AVK_CLR) )
  {
    bKeyDisabled = TRUE;
  }
#endif /* FEATURE_BT_EXTPF_FTP */
#ifdef FEATURE_BT_EXTPF_OPP
  else if ( ((TOP_MENU == BT_APP_MENU_OPP_LIST_FILE_TYPES) || 
             (TOP_MENU == BT_APP_MENU_OPP_CLIENT)) && 
            (pMe->mOPP.bObjectTransfer == TRUE) && (key != AVK_CLR) )
  {
    bKeyDisabled = TRUE;
  }
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_IOBEX  
  if ( ( TOP_MENU == BT_APP_MENU_OBEX_CLIENT ) && 
       ( ( pMe->mOBEX.bPutInProgress == TRUE ) || 
       ( pMe->mOBEX.bGetInProgress == TRUE ) ) && 
       (pMe->bBusyIconUp != FALSE) && (key == AVK_CLR) )
  {
    bKeyDisabled = FALSE;
  }
#endif /* FEATURE_OBEX */

  return bKeyDisabled;
}

/* ==========================================================================
FUNCTION BTApp_HCIModeOn
DESCRIPTION
============================================================================= */
static boolean BTApp_HCIModeOn( CBTApp* pMe )
{
  AEEBTHCIMode HCIMode;

  MSG_FATAL("***zzg BTApp_HCIModeOn***", 0, 0, 0);
  
  IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
 
  if ( HCIMode != AEEBT_HCIM_OFF )
  {
    BTApp_ShowMessage( pMe, IDS_MSG_HCI_MODE_OFF_FIRST, NULL, 0 );
    return TRUE;
  }
  return FALSE;
}

/* ==========================================================================
FUNCTION BTApp_RegisterAGNotif
DESCRIPTION: Registers for AG notifications
============================================================================= */
boolean BTApp_RegisterAGNotif( CBTApp* pMe )
{
  boolean bDone = ((uBTApp_NMask & NMASK_BT_AG) != NMASK_BT_AG) ? FALSE : TRUE;
  if ( bDone == FALSE )
  {
    uint32 umask = NMASK_BT_AG | (((uint32)(NOTIFIER_VAL_ANY)) << 16);
    int result = ISHELL_RegisterNotify( pMe->m_pShell,  
                                        AEECLSID_BLUETOOTH_APP,
                                        AEECLSID_BLUETOOTH_NOTIFIER, 
                                        umask );
    if ( result != SUCCESS )
    {
      MSG_FATAL( "RegisterAGNotif - Failed r=%d", 0, 0, 0 );
    }
    else
    {
      uBTApp_NMask |= NMASK_BT_AG;
      bDone = TRUE;
    }
  }

  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_DeregisterAGNotif
DESCRIPTION: Deregisters for AG notifications
============================================================================= */
void BTApp_DeregisterAGNotif( CBTApp* pMe )
{
  boolean bDone = ((uBTApp_NMask & NMASK_BT_AG) != NMASK_BT_AG) ? TRUE : FALSE;
  if ( bDone == FALSE )
  {
    int result;
    result = ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, 0 );
    if (result != SUCCESS)
    {
      MSG_FATAL( "DeregisterAGNotif - m=0 failed r=%d", result, 0, 0 );
    }
    uBTApp_NMask &= ~NMASK_BT_AG;
    result = ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
    if (result != SUCCESS)
    {
      MSG_FATAL( "DeregisterAGNotif - m=0x%x failed r=%d", 
                 uBTApp_NMask, result, 0 );
    }
  }
}


/* ==========================================================================
FUNCTION BTAppAG_Init
DESCRIPTION: This function initializes AG data.
============================================================================= */
boolean BTApp_AGInit( CBTApp *pMe )
{
  uint32 nmask;

  MSG_FATAL("***zzg BTApp_AGInit bInitialized=%x***", pMe->mAG.bInitialized, 0, 0);

  if ( pMe->mAG.bInitialized != FALSE )
  {
    return TRUE;
  }

  // Initialize the AG part
  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_AG, 
                              (void**)&pMe->mAG.po ) == SUCCESS )
  {
    if ( BTApp_RegisterAGNotif( pMe ) == FALSE )
    {
      BTApp_AGDeInit( (IApplet*) pMe );
      return FALSE;
    }

     /* Instance for ICallMgr */
    if (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_CALLMGR,
                               (void**)&pMe->mAG.pICallMgr ) != SUCCESS )
    {
      MSG_FATAL( "BTApp_AGInit - ICallMgr create instance failed", 0, 0, 0 );
	  MSG_FATAL("***zzg BTApp_AGInit - ICallMgr create instance failed***", 0, 0, 0);
      pMe->mAG.pICallMgr = NULL;
    }
#ifdef FEATURE_BT_HFP_1_5
    /* Instance for ICard */
    if (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_CARD,
                               (void**)&pMe->mAG.pICard ) != SUCCESS )
    {
      pMe->mAG.pICard = NULL;
    }
#endif
#ifdef FEATURE_BT_VT
    if (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_VIDEOPHONE,
                               (void**)&pMe->mAG.pIVP ) != SUCCESS )
    {
      pMe->mAG.pIVP = NULL;
    }
    
    if ( pMe->mAG.pIVP != NULL )
    {
      nmask = AEEVP_EVENT_MASK | (((uint32)(NOTIFIER_VAL_ANY)) << 16);
      // Register with IVideoPhone to receive events
      ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                             AEECLSID_VIDEOPHONE_NOTIFIER, nmask );
    }
    pMe->mAG.bVideoCallEnabled = FALSE;
#endif /* FEATURE_BT_VT */
#ifdef FEATURE_BT_HFP_1_5
    /* Instance for IBattery */
    if (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BATTERY,
                               (void**)&pMe->mAG.pIBattery ) != SUCCESS )
    {
      pMe->mAG.pIBattery = NULL;
    }

    if ( pMe->mAG.pIBattery != NULL )
    {
      nmask = NMASK_BATTERY_LEVEL_CHANGE | NMASK_BATTERY_STATUS_CHANGE;
      nmask = nmask | (((uint32)(NOTIFIER_VAL_ANY)) << 16);
      // Register with IBattery to recieve events
      ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                             AEECLSID_BATTERYNOTIFIER,
                             nmask );
    }
    CALLBACK_Init ( &pMe->mAG.externalIOCb, BTApp_RetrieveATCmd, pMe );
    if ( IBTEXTAG_EnableExternalIO( pMe->mAG.po, &pMe->mAG.externalIOCb ) !=
         SUCCESS )
    {
      MSG_FATAL( "BTApp_AGInit - failed enabling AG external I/O", 0, 0, 0 );
	  MSG_FATAL("***zzg BTApp_AGInit - failed enabling AG external I/O***", 0, 0, 0);
    }
#endif /* FEATURE_BT_HFP_1_5 */
/* Instance for ITelephone*/
    if (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_TELEPHONE,
                               (void**)&pMe->pIPhone ) != SUCCESS )
    {
      MSG_FATAL( "BTApp_AGInit - ITelephone create instance failed", 0, 0, 0 );
	  MSG_FATAL("***zzg BTApp_AGInit - ITelephone create instance failed***", 0, 0, 0);
      pMe->pIPhone = NULL;
    }
    if ( pMe->pIPhone != NULL )
    {
      nmask = AEET_NMASK_VOICE_CALL | AEET_NMASK_NEW_CALLDESC;
#ifdef FEATURE_BT_HFP_1_5
      nmask = nmask | AEET_NMASK_SS;
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
      nmask = nmask | (((uint32)(NOTIFIER_VAL_ANY)) << 16);
      // Register with ITelephone to recieve events
      ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                             AEECLSID_PHONENOTIFIER,
                             nmask );
    }
  }
  else
  {
    MSG_FATAL( "BTAppAG_Init - Failed to create AG instance", 0, 0, 0 );
	MSG_FATAL("***zzg BTAppAG_Init - Failed to create AG instance***", 0, 0, 0);
    /*  De-register the events for AG if A2DP is not enabled */
#ifdef FEATURE_BT_EXTPF_AV
    if ( pMe->mA2DP.bEnabled == FALSE )
#endif //FEATURE_BT_EXTPF_AV   
    {
      BTApp_DeregisterAGNotif( pMe );
    }
    BTApp_AGDeInit( (IApplet*) pMe );
    return FALSE;
  }
  pMe->mAG.bInitialized = TRUE;
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_AGDeInit
DESCRIPTION: This function frees AG Data and all data related to AG.
============================================================================= */
void BTApp_AGDeInit( IApplet* pi )
{
  CBTApp* pMe = (CBTApp*)pi;

#ifdef FEATURE_BT_HFP_1_5
  ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BATTERYNOTIFIER, 0 );
  if ( pMe->mAG.pIBattery != NULL )
  {
    IBATTERY_Release( pMe->mAG.pIBattery );
    pMe->mAG.pIBattery = NULL;
  }
#endif

  if ( pMe->mAG.pICallMgr != NULL )
  {
    ICALLMGR_Release( pMe->mAG.pICallMgr );
    pMe->mAG.pICallMgr = NULL;
  }

#ifdef FEATURE_BT_HFP_1_5
  ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                         AEECLSID_CARD, 0 );
  if ( pMe->mAG.pICard != NULL )
  {
    ICARD_Release( pMe->mAG.pICard );
    pMe->mAG.pICard = NULL;
  }
#endif

#ifdef FEATURE_BT_VT
  ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                         AEECLSID_VIDEOPHONE_NOTIFIER, 0 );

  if ( pMe->mAG.pIVP != NULL )
  {
    IVIDEOPHONE_Release( pMe->mAG.pIVP );
    pMe->mAG.pIVP = NULL;
  }
#endif
  ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                         AEECLSID_PHONENOTIFIER, 0 );
  if ( pMe->pIPhone != NULL )
  {
    ITELEPHONE_Release( pMe->pIPhone );
    pMe->pIPhone = NULL;
  }

  if ( pMe->mAG.po != NULL )
  {
    /* Must clean up External I/O's memory reference
     * before releasing the AG object.
     */
    IBTEXTAG_DisableExternalIO( pMe->mAG.po );

    IBTEXTAG_Release( pMe->mAG.po );
    pMe->mAG.po = NULL;
    pMe->mAG.bInitialized = FALSE;
  }
}


/* ==========================================================================
FUNCTION BTApp_ProcEvtAGDisabled
DESCRIPTION: This function processes EVT_AG_DISABLED.
============================================================================= */
static void BTApp_ProcEvtAGDisabled( CBTApp* pMe )
{
	BTApp_ReleaseBTDevice(pMe, FALSE);
	
	pMe->mAG.bStartedVr      = FALSE;
	pMe->mAG.bDevPickedUp    = FALSE;
	pMe->mAG.bDevDialed      = FALSE;
	pMe->mAG.bIgnoreButton   = FALSE;
	pMe->mAG.bAudioConnected = FALSE;
	pMe->mAG.bEnabled        = FALSE;
	pMe->mAG.bConnected      = FALSE;

#ifdef FEATURE_BT_HFP_1_5
	if (pMe->mAG.pSSInfo != NULL)
	{
		FREE((void*)pMe->mAG.pSSInfo);
		pMe->mAG.pSSInfo = NULL;
	}
	pMe->mAG.bValidSSData = FALSE;
#endif 

	BTApp_ClearBondable(pMe);

#ifdef FEATURE_BT_EXTPF_AV
	// De-register the events for AG if A2DP is not enabled
	if ( pMe->mA2DP.bEnabled == FALSE )
#endif //FEATURE_BT_EXTPF_AV   
	{
		BTApp_DeregisterAGNotif(pMe);
		BTApp_AGDeInit((IApplet*)pMe);
	}

	/* should update the menu to allow further operations
	* ONLY when all necessary routines are done.
	*/
	
	if ( TOP_MENU == BT_APP_MENU_MAIN )
	{
		BTApp_BuildMenu( pMe, TOP_MENU );
	}
	
}


/* ==========================================================================
FUNCTION BTApp_RegisterNANotif
DESCRIPTION: Registers for NA notifications
============================================================================= */
boolean BTApp_RegisterNANotif( CBTApp* pMe )
{
  boolean bDone = ((uBTApp_NMask & NMASK_BT_NA) != NMASK_BT_NA) ? FALSE : TRUE;
  if ( bDone == FALSE )
  {
    uint32 umask = NMASK_BT_NA | (((uint32)(NOTIFIER_VAL_ANY)) << 16);
    int result = ISHELL_RegisterNotify( pMe->m_pShell,  
                                        AEECLSID_BLUETOOTH_APP,
                                        AEECLSID_BLUETOOTH_NOTIFIER, 
                                        umask );
    if ( result != SUCCESS )
    {
      MSG_FATAL( "RegisterNANotif - Failed r=%d", 0, 0, 0 );
    }
    else
    {
      uBTApp_NMask |= NMASK_BT_NA;
      bDone = TRUE;
    }
  }
  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_DeregisterNANotif
DESCRIPTION: Deregisters for NA notifications
============================================================================= */
void BTApp_DeregisterNANotif( CBTApp* pMe )
{
  boolean bDone = ((uBTApp_NMask & NMASK_BT_NA) != NMASK_BT_NA) ? TRUE : FALSE;
  if ( bDone == FALSE )
  {
    int result;
    result = ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, 0 );
    if (result != SUCCESS)
    {
      MSG_FATAL( "DeregisterNANotif - m=0 failed r=%d", result, 0, 0 );
    }
    uBTApp_NMask &= ~NMASK_BT_NA;
    result = ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
    if (result != SUCCESS)
    {
      MSG_FATAL( "DeregisterNANotif - m=0x%x failed r=%d", 
                 uBTApp_NMask, result, 0 );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_NAInit
DESCRIPTION: This function instantiates NA and registers for the NA 
             notifications
============================================================================= */
static boolean BTApp_NAInit( CBTApp *pMe )
{
  if ( pMe->mNA.bInitialized != FALSE )
  {
    return TRUE;
  }
  // Initialize the NA part
  if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_NA, 
                              (void**)&pMe->mNA.po ) == SUCCESS )
  {
    // Now register for NA Notifications
    if ( BTApp_RegisterNANotif( pMe ) == FALSE )
    {
      MSG_FATAL( "BTApp_NAInit - Failed to Register NA events", 0, 0, 0 );
      BTApp_NADeInit( (IApplet*) pMe );
      return FALSE;
    } 
  }
  else
  {
    MSG_FATAL( "BTApp_NAInit - Failed to create NA instance", 0, 0, 0 );
    BTApp_NADeInit( (IApplet*) pMe );
    return FALSE;
  }
  pMe->mNA.bInitialized = TRUE;
  return TRUE;
}
/* ==========================================================================
FUNCTION BTApp_NADeInit
DESCRIPTION: This function frees NA Data
============================================================================= */
static void BTApp_NADeInit( IApplet* pi )
{
  CBTApp* pMe = (CBTApp*)pi;
  /*  De-register the events for NA */
  
   BTApp_DeregisterNANotif( pMe );

  if ( pMe->mNA.po != NULL )
  {
    IBTEXTNA_Release( pMe->mNA.po );
    pMe->mNA.po = NULL;
    pMe->mNA.bInitialized = FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_EnableAG
DESCRIPTION
============================================================================= */
static boolean BTApp_EnableAG( CBTApp* pMe, boolean* pbSettingBondable)
{
  AEEBTBDAddr* pBDAddr = NULL;
  boolean bAGEnabled = FALSE;

  if ( pMe->mAG.bSelected && !pMe->mAG.bEnabled )
  {
    AEEBTIdleMode idleMode = pMe->mAG.idleMode;
    uint16 idleTimeout     = pMe->mAG.idleTimeout;

#ifdef FEATURE_BT_2_1
    if ( pMe->mAG.bSecSelected !=FALSE ) 
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po,
                               AEEBT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY,
                               pMe->mAG.srvSecType,
                               pMe->mAG.bAuthorize,
                               pMe->mAG.bAuthorizeFirst );
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY,
                               pMe->mAG.srvSecType, 
                               pMe->mAG.bAuthorize,
                               pMe->mAG.bAuthorizeFirst );
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po,
                               AEEBT_SD_SERVICE_CLASS_HANDSFREE,
                               pMe->mAG.srvSecType, 
                               pMe->mAG.bAuthorize,
                               pMe->mAG.bAuthorizeFirst );
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_HEADSET,
                               pMe->mAG.srvSecType,
                               pMe->mAG.bAuthorize,
                               pMe->mAG.bAuthorizeFirst );

    }
#endif //FEATURE_BT_2_1

    if ( BTApp_AGInit( pMe ) == FALSE )
    {
      MSG_FATAL("BTApp_EnableBT - Failed to register for AG events", 0, 0, 0);
	  MSG_FATAL("***zzg BTApp_EnableBT - Failed to register for AG events***", 0, 0, 0);
    }
    else
    {
      // should use Passive mode if also enabling NA
      if ( pMe->mNA.bSelected )
      {
        if ( idleMode == AEEBT_AG_IDLEMODE_ACTIVE )
        {
          MSG_FATAL( "EnableAG - forcing Passive mode", 0, 0, 0 );
		  MSG_FATAL("***zzg EnableAG - forcing Passive mode***", 0, 0, 0);
          idleMode    = AEEBT_AG_IDLEMODE_PASSIVE;
          idleTimeout = 0;
        }
      }
  
      BTApp_SetBondable( pMe );
      *pbSettingBondable = TRUE;
  
      IBTEXTAG_Config( pMe->mAG.po, idleMode, idleTimeout );
      IBTEXTAG_SetInbandRingCapable( pMe->mAG.po, pMe->mAG.bInbandRing );
  
#ifdef FEATURE_BT_HFP_1_5
      if ( pMe->mAG.pIBattery != NULL )
      {
        uint8 uLevel;
        if ( !( BTApp_GetBatteryLevel(pMe, &uLevel) ) || 
             (IBTEXTAG_SetBatteryChargeIndicator(pMe->mAG.po, 
               (AEEBTBatteryChargeLevel)uLevel)) != SUCCESS )
        {
        	MSG_FATAL("***zzg EnableAG- Invalid no of Batt Scale***", 0, 0, 0);
          MSG_FATAL( "EnableAG- Invalid no of Batt Scale ", 0, 0, 0 );
        }
      }
#endif /* FEATURE_BT_HFP_1_5*/
     MSG_FATAL( "EnableAG - AG BDa=%2x%2x%2x", pMe->mAG.bdAddr.uAddr[5],
               pMe->mAG.bdAddr.uAddr[4], pMe->mAG.bdAddr.uAddr[3] );
 
      if ( BDADDR_VALID( &pMe->mAG.bdAddr ) )
      {
      	MSG_FATAL("BDADDR_VALID",0,0,0);
        pBDAddr = &pMe->mAG.bdAddr;
      }

	  MSG_FATAL("***zzg EnableAG mAG:%04x %04x %04x***", 
			((uint16)(pMe->mAG.bdAddr.uAddr[ 5 ] << 8) | pMe->mAG.bdAddr.uAddr[ 4 ]),
       		((uint16)(pMe->mAG.bdAddr.uAddr[ 3 ] << 8) | pMe->mAG.bdAddr.uAddr[ 2 ]),
       		((uint16)(pMe->mAG.bdAddr.uAddr[ 1 ] << 8) | pMe->mAG.bdAddr.uAddr[ 0 ]));

	  MSG_FATAL("***zzg EnableAG pBDAddr:%04x %04x %04x***", 
			((uint16)(pBDAddr->uAddr[ 5 ] << 8) | pBDAddr->uAddr[ 4 ]),
       		((uint16)(pBDAddr->uAddr[ 3 ] << 8) | pBDAddr->uAddr[ 2 ]),
       		((uint16)(pBDAddr->uAddr[ 1 ] << 8) | pBDAddr->uAddr[ 0 ]));

	  MSG_FATAL("***zzg IBTEXTAG_Enable devType=%x***", pMe->mAG.devType, 0, 0);
	  
      if ( IBTEXTAG_Enable( pMe->mAG.po, pBDAddr, pMe->mAG.devType ) != SUCCESS )
      {
        MSG_FATAL( "EnableAG - failed to enable AG", 0, 0, 0 );
		MSG_FATAL("***zzg EnableAG - failed to enable AG***", 0, 0, 0);
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        /*  De-register the events for AG if A2DP is not enabled */
#ifdef FEATURE_BT_EXTPF_AV
        if ( pMe->mA2DP.bEnabled == FALSE )
#endif //FEATURE_BT_EXTPF_AV   
        {
          BTApp_DeregisterAGNotif( pMe );
        }
        BTApp_AGDeInit( (IApplet*)pMe );
      }
      else
      {
        if ( BTApp_CallConnected( pMe ) != BT_APP_CALL_NONE )
        {
          pMe->mAG.bDevPickedUp = TRUE; // so voice would go to HS
        }
        bAGEnabled = TRUE;
        //BTApp_ShowBusyIcon( pMe );
      }
    }
  }

  MSG_FATAL("***zzg EnableAG Final bAGEnabled=%x***", bAGEnabled, 0, 0);
  return bAGEnabled;
}
/* ==========================================================================
FUNCTION BTApp_EnableNA
DESCRIPTION
============================================================================= */
static boolean BTApp_EnableNA( CBTApp* pMe, boolean* pbSettingBondable)
{
  boolean bNAEnabled = FALSE;
  int     result;

  if ( pMe->mNA.bSelected && !pMe->mNA.bEnabled )
  {

#ifdef FEATURE_BT_2_1
    if ( pMe->mNA.bSecSelected !=FALSE ) 
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_DIALUP_NETWORKING,
                               pMe->mNA.srvSecType,
                               pMe->mNA.bAuthorize,
                               pMe->mNA.bAuthorizeFirst );

    }
#endif // FEATURE_BT_2_1

    if ( pMe->bFirstLaunch != FALSE )
    {
      // must wait for CoreApp to map Data port to BT NA
	  
      ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*5, 
                       (PFNNOTIFY) BTApp_EnableBT, pMe );
      return TRUE;
    }
    if ( BTApp_NAInit( pMe ) == FALSE )
    {
      MSG_FATAL( "BTApp_EnableBT - Failed to register for NA events", 0, 0, 0 );
    }
    else
    {
      if ( *pbSettingBondable == FALSE )
      {
        BTApp_SetBondable( pMe );
        *pbSettingBondable = TRUE;
      }
  
      MSG_FATAL( "EnableNA - enabling NA", 0, 0, 0 );
  
      result = IBTEXTNA_Enable( pMe->mNA.po );
      if ( result != SUCCESS )
      {
        MSG_FATAL( "EnableNA - failed to enable NA r=%x", result, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
        bNAEnabled = TRUE;
      }
    }
  }
  return bNAEnabled;
}

/* ==========================================================================
FUNCTION BTApp_EnableBT
DESCRIPTION
============================================================================= */
void BTApp_EnableBT(CBTApp* pMe)
{
	boolean      bSettingBondable = FALSE;
	
#if (defined(FEATURE_BT_EXTPF_OPP) || defined(FEATURE_BT_EXTPF_FTP) || \
	 defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || \
	 defined(FEATURE_BT_EXTPF_PBAP) || defined(FEATURE_IOBEX))
	boolean      bSettingDiscoverable = FALSE;
#endif

    if(bIsBTOn)
    {
        return;
    }
    
	if (pMe->mEnablingType == BTAPP_ENABLING_NONE)
	{
		pMe->mEnablingType = BTAPP_ENABLING_AG;
	}
	
	if (BTApp_HCIModeOn(pMe) == TRUE)
	{
		MSG_FATAL("BTApp_HCIModeOn(pMe) == TRUE!", 0, 0, 0);
		return;
	}
	
	if (pMe->bEnableAtStart == FALSE)
	{
		pMe->bEnableAtStart = TRUE;
		BTApp_WriteConfigFile(pMe);
	}

	MSG_FATAL("***zzg BTApp_EnableBT mEnablingType=%x***", pMe->mEnablingType, 0, 0);
	
	switch (pMe->mEnablingType)
	{
		case BTAPP_ENABLING_AG:
		{
			if (BTApp_EnableAG(pMe, &bSettingBondable) == TRUE)
			{	
				MSG_FATAL("***zzg BTApp Enable AG TRUE***", 0, 0, 0);
				break;
			}

			MSG_FATAL("***zzg BTApp Enable AG FALSE***", 0, 0, 0);	
            pMe->mEnablingType++;
		}
		
		case BTAPP_ENABLING_NA:
		{
#if 0
			if (BTApp_EnableNA(pMe, &bSettingBondable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable NA TRUE***", 0, 0, 0);
				break;
			}
#endif
			MSG_FATAL("***zzg BTApp Enable NA FALSE***", 0, 0, 0);
			pMe->mEnablingType++;
		}
		
#ifdef FEATURE_BT_EXTPF_OPP  
		case BTAPP_ENABLING_OPP:
		{			
			if (BTApp_EnableOPP(pMe, &bSettingBondable, &bSettingDiscoverable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable OPP TRUE***",0,0,0);
				break;
			}

			MSG_FATAL("***zzg BTApp Enable OPP FALSE***",0,0,0);
            pMe->mEnablingType++;
		}
#endif //FEATURE_BT_EXTPF_OPP

#ifdef FEATURE_IOBEX  
		case BTAPP_ENABLING_OBEX:
		{
			BTApp_EnableOBEX(pMe, &bSettingBondable, &bSettingDiscoverable);
            pMe->mEnablingType++;
		}
#endif //FEATURE_IOBEX

#ifdef FEATURE_BT_EXTPF_FTP 
		case BTAPP_ENABLING_FTP:
		{
			if (BTApp_EnableFTP(pMe, &bSettingBondable, &bSettingDiscoverable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable FTP TRUE***", 0, 0, 0);
				break;
			}
			
			MSG_FATAL("***zzg BTApp Enable FTP FALSE***", 0, 0, 0);
			pMe->mEnablingType++;
		}
#endif /* FEATURE_BT_EXTPF_FTP */

#ifdef FEATURE_BT_EXTPF_BIP
		case BTAPP_ENABLING_BIP:
		{
			if (BTApp_EnableBIP(pMe, &bSettingBondable, &bSettingDiscoverable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable BIP TRUE***", 0, 0, 0);
				break;
			}

			MSG_FATAL("***zzg BTApp Enable BIP FALSE***", 0, 0, 0);
            pMe->mEnablingType++;
		}
#endif //FEATURE_BT_EXTPF_BIP

#ifdef FEATURE_BT_EXTPF_BPP  
		case BTAPP_ENABLING_BPP:
		{
			if (BTApp_EnableBPP(pMe, &bSettingBondable ,&bSettingDiscoverable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable BPP TRUE***", 0, 0, 0);
				break;
			}

			MSG_FATAL("***zzg BTApp Enable BPP FALSE***", 0, 0, 0);
            pMe->mEnablingType++;
		}
#endif //FEATURE_BT_EXTPF_BPP

#ifdef FEATURE_BT_EXTPF_PBAP
		case BTAPP_ENABLING_PBAP:
		{
			if(BTApp_EnablePBAP(pMe, &bSettingBondable, &bSettingDiscoverable) == TRUE)
			{
				MSG_FATAL("***zzg BTApp Enable PBAP TRUE***", 0, 0, 0);
				break;
			}

			MSG_FATAL("***zzg BTApp Enable PBAP FALSE***", 0, 0, 0);
            pMe->mEnablingType++;
		}
#endif /*FEATURE_BT_EXTPF_PBAP*/

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif //FEATURE_BT_EXTPF_SAP

#ifdef FEATURE_BT_EXTPF_AV
		case BTAPP_ENABLING_AV:
		{
			MSG_FATAL("***zzg BTApp_EnableBT BTAPP_ENABLING_AV***", 0, 0, 0);
			BTApp_EnableA2DP(pMe, &bSettingBondable);
			BTApp_EnableAVRCP(pMe, &bSettingBondable);
            pMe->mEnablingType++;
		}
#endif //FEATURE_BT_EXTPF_AV

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif //FEATURE_BT_EXTPF_HID_HOST

		case BTAPP_ENABLING_L2:
		{
			BTApp_EnableL2(pMe);
			pMe->mEnablingType = BTAPP_ENABLING_AG;
			BTApp_BuildTopMenu(pMe);
            
            //Add By zzg 2010_11_16
        	bIsBTOn = TRUE;

        	if (pMe->m_pIAnn)
        	{
        		IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_ON);		
        	}

        	if (pMe->m_pConfig)
        	{
        		(void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_BT_STATUS,&bIsBTOn,sizeof(bIsBTOn));   
        	}		  
        	//Add End
			break;
		}
	}
}
/* ==========================================================================
FUNCTION BTApp_DisableBT
DESCRIPTION
============================================================================= */
//static void BTApp_DisableBT( CBTApp* pMe )
void BTApp_DisableBT( CBTApp* pMe )
{
	int     result;
	boolean disabling = FALSE;
  
#if (defined(FEATURE_BT_EXTPF_OPP) || defined(FEATURE_BT_EXTPF_FTP) || \
	 defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || \
	 defined(FEATURE_IOBEX))
	boolean bSettingDiscoverable = FALSE;
#endif

	MSG_FATAL("***zzg BTApp_DisableBT***", 0, 0, 0);
    if(!bIsBTOn)
    {
        return;
    }
    
	if (pMe->bEnableAtStart != FALSE)
	{
		pMe->bEnableAtStart = FALSE;
		BTApp_WriteConfigFile(pMe);
	}

	if (pMe->mAG.bEnabled)
	{
		MSG_FATAL( "DisableBT - disabling AG", 0, 0, 0 );
		if ((result = IBTEXTAG_Disable(pMe->mAG.po)) != SUCCESS)
		{
			MSG_FATAL("DisableBT - failed to disable AG, r=%d", result, 0, 0);
		}
		else
		{
			disabling = TRUE;
		}
	}	
	
	if (pMe->mNA.bEnabled)
	{
		MSG_FATAL( "DisableBT - disabling NA", 0, 0, 0 );
		if ((result = IBTEXTNA_Disable(pMe->mNA.po)) != SUCCESS)
		{
			MSG_FATAL("DisableBT - failed to disable NA, r=%d", result, 0, 0);
		}
		else
		{
			disabling = TRUE;
		}
	}	
	
	
#ifdef FEATURE_BT_EXTPF_OPP
	if (pMe->mOPP.bRegistered == TRUE)
	{
		if ((result = IBTEXTOPP_Deregister(pMe->mOPP.po)) != SUCCESS)
		{
			MSG_FATAL("OPP_Deregister() failed with %x", result, 0, 0);
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			BTApp_ClearBondable(pMe); 	//Add By zzg 2010_12_29							 
									
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
		}
	}
#endif //FEATURE_BT_EXTPF_OPP

#ifdef FEATURE_BT_EXTPF_FTP
	if (pMe->mFTP.bRegistered == TRUE)
	{
		if ((result = IBTEXTFTP_Deregister(pMe->mFTP.po)) != SUCCESS)
		{
			MSG_FATAL("FTP_Deregister() failed with %x", result, 0, 0);
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
		}
	}
#endif //FEATURE_BT_EXTPF_FTP

#ifdef FEATURE_BT_EXTPF_BIP
	if (pMe->mBIP.bRegistered == TRUE)
	{
		if (IBTEXTBIP_Deregister( pMe->mBIP.po) != SUCCESS)
		{
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
		}
	}
#endif //FEATURE_BT_EXTPF_BIP

#ifdef FEATURE_BT_EXTPF_BPP
	if (pMe->mBPP.bRegistered == TRUE)
	{
		if (IBTEXTBPP_Deregister(pMe->mBPP.po) != SUCCESS)
		{
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
		}
	}
#endif //FEATURE_BT_EXTPF_BPP

#ifdef FEATURE_BT_EXTPF_PBAP
	if (pMe->mPBAP.bSrvRegistered != FALSE)
	{
		if (IBTEXTPBAP_DeregisterServer(pMe->mPBAP.po) != SUCCESS)
		{
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
		}
	}
#endif //FEATURE_BT_EXTPF_PBAP

#ifdef FEATURE_IOBEX
	if (pMe->mOBEX.bRegistered == TRUE)
	{
		result = IOBEXServer_Deregister(pMe->mOBEX.poServer);
		if (result != SUCCESS)
		{
			MSG_FATAL("DisableBT - OBEX Deregister Failed with %x", result, 0, 0);
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
		}
		else
		{
			pMe->mOBEX.bRegistered = FALSE;
			
			if (bSettingDiscoverable == FALSE)
			{
				IBTEXTSD_SetDiscoverable(pMe->mSD.po, FALSE);
				bSettingDiscoverable = TRUE;
			}
			disabling = TRUE;
			
			if (pMe->mOBEX.poServer != NULL)
			{
				IOBEXServer_Release(pMe->mOBEX.poServer);
				pMe->mOBEX.poServer = NULL;
			}
		}
	}
#endif //FEATURE_IOBEX

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif //FEATURE_BT_EXTPF_SAP

#ifdef FEATURE_BT_EXTPF_AV
	if (pMe->mA2DP.bEnabled == TRUE)
	{
		BTApp_A2DPDisable(pMe);
	}

	if (pMe->mAVRCP.bEnabled == TRUE)
	{
		BTApp_AVRCPDisable(pMe);
	}
#endif //FEATURE_BT_EXTPF_AV

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif //FEATURE_BT_EXTPF_HID_HOST

	if (pMe->mL2.bEnabled == TRUE)
	{
		BTApp_L2Disable(pMe);
	}
	
	if (disabling)
	{
		BTApp_ShowBusyIcon(pMe);
	}
    
	//Add By zzg 2010_11_16
	bIsBTOn = FALSE;	

	if (pMe->m_pIAnn)
	{
		IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_OFF);
	}

	if (pMe->m_pConfig)
	{
		(void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_BT_STATUS,&bIsBTOn,sizeof(bIsBTOn));   
	}	
	//Add End
  
}

#ifdef FEATURE_BT_HFP_1_5
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

/* ==========================================================================
FUNCTION BTApp_ATResponseCIMI
DESCRIPTION
============================================================================= */
static boolean BTApp_ATResponseCIMI( CBTApp* pMe )
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
  MSG_FATAL( "ATResponseCIMI - Not supported for CDMA", 0, 0, 0 );
  return FALSE;
#endif
}

/* ==========================================================================
FUNCTION BTApp_ATResponseCBC
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseCBC( CBTApp* pMe )
{
  AEEBatteryStatus   BattStatus;
  uint32             BattLevel;
  uint16             uBattLevel;
  uint16             uBattScale;
  uint8              ATBattStatus;
  uint8              ATBattLevel;
  uint8              rspBuf[BTAPP_RSP_BUFF_SIZE];
  
  if ( pMe->mAG.pIBattery == NULL )
  {
    MSG_FATAL( "ATResponseCBC - No Battery I/F available", 0, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
    return;
  }
  /* get battery status */
  if ( IBATTERY_GetBatteryStatus( pMe->mAG.pIBattery, 
                                  &BattStatus ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseCBC - cannot obtain battery status", 0, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
    return;
  }
  switch ( BattStatus )
  {
    case AEEBATTERY_STATUS_UNKNOWN:
    {
      ATBattStatus = BTAPP_CBC_BATTSTAT_NOT_CONNECTED;
      break;
    }
    case AEEBATTERY_STATUS_POWERDOWN:
    {
      ATBattStatus = BTAPP_CBC_BATTSTAT_CON_NO_POWER;
      break;
    }
    case AEEBATTERY_STATUS_LOW:
    case AEEBATTERY_STATUS_NORMAL:
    {
      ATBattStatus = BTAPP_CBC_BATTSTAT_POWERED;
      break;
    }
    default:
      ATBattStatus = BTAPP_CBC_BATTSTAT_NOT_CONNECTED;
      break;
  }

  /* get battery level */
  if ( IBATTERY_GetBatteryLevel( pMe->mAG.pIBattery, 
                                 &BattLevel ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseCBC - cannot obtain battery level", 0, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
    return;
  }
  uBattLevel = GETBATTERYLEVEL(BattLevel);
  uBattScale = GETBATTERYSCALE(BattLevel);
  if ( uBattScale == 0 )
  {
    MSG_FATAL( "ATResponseCBC - Invalid no of Batt Scale ", 0, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
    return;
  }

  ATBattLevel = (uBattLevel * 100) / uBattScale;
  MSG_FATAL( "ATResponseCBC - BattLevel: %d, BattScale: %d", uBattLevel,uBattScale,0 );
  /* format the response message */
  SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CBC: %d,%d\r\n", ATBattStatus, ATBattLevel );
  STRLCAT( (char*)rspBuf, (char*)okResp, sizeof(rspBuf) ); // append OK

  /* send response */
  if ( SEND_RESP( rspBuf ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseCBC - Failure in sending response", 0, 0, 0 );
  }  
}

/* ==========================================================================
FUNCTION BTApp_ATConvertRSSI
DESCRIPTION
============================================================================= */
static uint8 BTApp_ATConvertRSSI( uint16 rssi, uint8 scale )
{
  uint8 siglvl = 0;

  /* Note: RSSI dbm is really a negative value        */
  if ( (BTAPP_CSQ_RSSI_MIN < rssi) && (BTAPP_CSQ_RSSI_MAX > rssi) )
  {
    /* Round to nearest integer on scale */
    siglvl = (uint8)FFLOOR( ((rssi * BTAPP_CSQ_RSSI_SLOPE +
                            BTAPP_CSQ_RSSI_OFFSET) * scale)/100 + 0.5 );
  }
  else if ( (BTAPP_CSQ_RSSI_MAX <= rssi) && 
            (BTAPP_CSQ_RSSI_NO_SIGNAL != rssi) ) 
  {
    siglvl = BTAPP_CSQ_RSSI_TOOLO_CODE;
  }
  else if (BTAPP_CSQ_RSSI_MIN >= rssi)
  {
    siglvl = BTAPP_CSQ_RSSI_TOOHI_CODE;  /* Capped maximum */
  }
  else
  {
    MSG_FATAL( "ATConvertRSSI: RSSI outside signal range: %d", rssi, 0, 0 );
    siglvl = BTAPP_CSQ_RSSI_UNKNOWN_CODE;
  }
  return siglvl;
}

/* ==========================================================================
FUNCTION BTApp_ATResponseCSQ
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseCSQ( CBTApp* pMe )
{
  uint8       rspBuf[BTAPP_RSP_BUFF_SIZE];
  uint16      rssiDB;
  uint8       rssiValue;

  /* calculate the RSSI value */
  /* check if we are in not in HDR mode */
  if ( (pMe->mAG.pSSInfo->sys_mode != AEET_SYS_MODE_HDR) &&
       ((pMe->mAG.pSSInfo->srv_status == AEET_SRV_STATUS_SRV) ||
       (pMe->mAG.pSSInfo->srv_status == AEET_SRV_STATUS_LIMITED)) )
  {
    rssiDB = pMe->mAG.pSSInfo->rssi;
  }
  else
  {
    MSG_FATAL( "ATResponseCSQ - No RSSI Value available:%d", 
             pMe->mAG.pSSInfo->srv_status, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_NO_NETWORK_SERVICE );
    return;
  }

  rssiValue = BTApp_ATConvertRSSI( rssiDB, BTAPP_CSQ_RSSI_TOOHI_CODE );

  /* format the response message */
  SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CSQ: %d,%d\r\n", rssiValue, 
           BTAPP_CSQ_BER_UNKNOWN );
  STRLCAT( (char*)rspBuf, (char*)okResp, sizeof(rspBuf) ); // append OK

  /* send response */
  if ( SEND_RESP( rspBuf ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseCSQ - Failure in sending response", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ATResponseQueryCSCS
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseQueryCSCS( CBTApp* pMe )
{
  uint8       rspBuf[BTAPP_RSP_BUFF_SIZE];
  uint8       i=0;
  uint16      length;

  if ( ARR_SIZE(BTApp_ATCSCSTable) == 0 )
  {
    MSG_FATAL( "ATResponseQueryCSCS - No character set supported", 0, 0, 0 );
  }

  /* format response packet */
  SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CSCS: (\"%s\"", BTApp_ATCSCSTable[i].char_set );
  i++;
  length = STRLEN( (const char*)rspBuf );
  while ( i < ARR_SIZE(BTApp_ATCSCSTable) )
  {
    /* add comma before adding the char set */
    SNPRINTF( (char*)&rspBuf[length], sizeof(rspBuf),  ",\"%s\"", BTApp_ATCSCSTable[i].char_set );
    length = STRLEN( (const char*)rspBuf );
    i++;
  }
  SNPRINTF( (char*)&rspBuf[length], sizeof(rspBuf),  ")\r\n" );
  STRLCAT( (char*)rspBuf, (char*)okResp, sizeof(rspBuf ) ); // append OK

  /* send response */
  if ( SEND_RESP( rspBuf ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseQueryCSCS - Failure in sending response", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ATResponseReadCSCS
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseReadCSCS( CBTApp* pMe )
{
  uint8       rspBuf[BTAPP_RSP_BUFF_SIZE];

  SNPRINTF( (char*)rspBuf, sizeof(rspBuf),  "\r\n+CSCS: \"%s\"\r\n", 
           BTApp_ATCSCSTable[pMe->mAG.charSetType].char_set );
  STRLCAT( (char*)rspBuf, (char*)okResp, sizeof( rspBuf ) ); // append OK

  /* send response */
  if ( SEND_RESP( rspBuf ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseReadCSCS - Failure in sending response", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ATResponseErr
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseErr( 
  CBTApp* pMe, 
  BTApp_ATCMEEErrCode errCode )
{
  uint8       rspBuf[BTAPP_RSP_BUFF_SIZE];
  int         index;
  char*       cmdString = NULL;
  BTApp_CmeTableType BTApp_CmeTable[] = 
  {
    BTAPP_CME_AG_FAILURE,                 "AG failure",
    BTAPP_CME_NO_CONNECTION,              "no connection to phone",
    BTAPP_CME_OP_NOT_ALLOWED,             "operation not allowed",
    BTAPP_CME_OP_NOT_SUPPORTED,           "operation not supported",
    BTAPP_CME_PH_SIM_PIN_REQUIRED,        "PH-SIM PIN required",
    BTAPP_CME_SIM_NOT_INSERTED,           "SIM not inserted",
    BTAPP_CME_SIM_PIN_REQUIRED,           "SIM PIN required",
    BTAPP_CME_SIM_PUK_REQUIRED,           "SIM PUK required",
    BTAPP_CME_SIM_FAILURE,                "SIM failure",
    BTAPP_CME_SIM_BUSY,                   "SIM busy",
    BTAPP_CME_INCORRECT_PASSWORD,         "incorrect password",
    BTAPP_CME_SIM_PIN2_REQUIRED,          "SIM PIN2 required",
    BTAPP_CME_SIM_PUK2_REQUIRED,          "SIM PUK2 required",
    BTAPP_CME_MEMORY_FULL,                "memory full",
    BTAPP_CME_INVALID_INDEX,              "invalid index",
    BTAPP_CME_MEMORY_FAILURE,             "memory failure",
    BTAPP_CME_TEXT_STRING_TOO_LONG,       "text string too long",
    BTAPP_CME_INVALID_CHAR_IN_TEXT,       "invalid chars in text",
    BTAPP_CME_DIAL_STRING_TOO_LONG,       "dial string too long",
    BTAPP_CME_INVALID_CHAR_IN_DIAL_STRING,"invalid characters in dial string",
    BTAPP_CME_NO_NETWORK_SERVICE,         "no network service",
    BTAPP_CME_NETWORK_NOT_ALLOWED,        "Network not allowed - Emergency calls only",
    BTAPP_CME_MAX,                        NULL
  };

  if ( pMe->mAG.errATMode == BTAPP_CMEE_ERR_ENABLE_NUMERIC )
  {
    SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CME ERROR: %d", errCode );
  }
  else if ( pMe->mAG.errATMode == BTAPP_CMEE_ERR_ENABLE_VERBOSE )
  {
    /* parse the table */
    index = 0;
    do
    {
      cmdString = BTApp_CmeTable[index].cmeText;
    }while ( (BTApp_CmeTable[index].cmeCode != errCode) && 
             (BTApp_CmeTable[index].cmeText != NULL) );
    if ( cmdString == NULL )
    {
      MSG_FATAL( "ATResponseErr - Invalid Error Code", 0, 0, 0 );
      return;
    }
    SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CME ERROR: %s", cmdString );    
  }
  else
  {
    SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\nERROR", cmdString );    
  }

  /* append response postfix, but make sure to avoid memory corruption */
  if ( STRLEN( (char*)rspBuf ) < (BTAPP_RSP_BUFF_SIZE-3) ) 
  {
    /* append response postfix */
    STRLCAT( (char*)rspBuf, "\r\n", sizeof( rspBuf ) );
  }

  /* send error response */
  if ( SEND_RESP( rspBuf ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseErr - Failure in sending response", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ATResponseCNUM
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseCNUM( CBTApp* pMe )
{
  char                 pszBuf[ NV_DIR_NUMB_PCS_SIZ ];
  char                 pRspBuff[ BTAPP_RSP_BUFF_SIZE ];
  char                 numBuff[ BTAPP_RSP_BUFF_SIZE ];
  uint8                nameType;
  uint8                serviceType;
  int                  result;
  int                  numBuffSize;

  if ( pMe->mAG.bValidSSData == FALSE )
  {
    MSG_FATAL( "ATResponseCNUM - SS Info not avaiable to process", 0, 0, 0 );
    SEND_RESP( okResp );
    return;
  }
  if ( pMe->mAG.pSSInfo->sys_mode == AEET_SYS_MODE_CDMA )
  {
    /* 1x mode */
    numBuffSize = BTAPP_RSP_BUFF_SIZE;
    if ( ISHELL_GetDeviceInfoEx( pMe->m_pShell, AEE_DEVICEITEM_MDN, 
                                 (void*)numBuff, &numBuffSize) != SUCCESS )
    {
      MSG_FATAL( "ATResponseCNUM - cannot obtain MSISDN", 0, 0, 0 );
      MSG_FATAL( "ATResponseCNUM - sending empty MSISDN", 0, 0, 0 );
      SEND_RESP( okResp );
      return;
    }
    (void)MEMCPY(pszBuf, numBuff, STRLEN(numBuff));
    pszBuf[ STRLEN(numBuff) ] = 0;
    /* send the response */
    if ( pszBuf[0] == BTAPP_TA_INTER_PREFIX )
    {
      nameType = BTAPP_TA_INTERNATIONAL;
    }
    else
    {
      nameType = BTAPP_TA_UNKNOWN;
    }
    serviceType = BTAPP_CNUM_SERVICE_VOICE;
    SNPRINTF( pRspBuff, sizeof(pRspBuff), "\r\n+CNUM: ,\"%s\",%d,,%d\r\n", 
             pszBuf, nameType, serviceType );
    /* print the response buffer */
    {
      int  test;
      for ( test = 0; test < STRLEN((const char*)pRspBuff); test++ )
      {
        MSG_FATAL( "ATResponseCNUM - pRspBuff : %c", pRspBuff[test], 0, 0 );
      }
    }
    /* send the response */
    result = SEND_RESP( (uint8*) pRspBuff );
    if ( result != SUCCESS )
    {
      MSG_FATAL( "ATResponseCNUM - Failure in sending response", 0, 0, 0 );
      return;
    }
  }
  else
  {
    /* GW mode */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif
  }
  /* send an OK response */
  SEND_RESP( okResp );
  return;
}

/* ==========================================================================
FUNCTION BTApp_ATResponseCLCC
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseCLCC( CBTApp* pMe )
{
  AEETCalls*            pCallsDesc = NULL;
  uint8                 uSizeOfCallsDesc = 0;
  AEETCallInfo          callInfo;
  uint8                 index = 0;
  int                   result = SUCCESS;
  uint8                 callDirection;
  uint8                 callStatus;
  uint8                 callMode;
  boolean               callMultiparty;
  uint8                 callNumType;
  uint8                 rspBuf[ BTAPP_RSP_BUFF_SIZE ];

  uSizeOfCallsDesc = sizeof(AEETCalls) + 
                     (sizeof(AEECallDesc)*(BTAPP_MAX_NUM_CALLS - 1));
  pCallsDesc = (AEETCalls*)MALLOC( uSizeOfCallsDesc );
  if ( pCallsDesc == NULL )
  {
    MSG_FATAL( "ATResponseCLCC - Memory allocation failed", 0, 0, 0 );
    SEND_RESP( okResp );
    return;  
  }
  /* get calls information */
  if ( ITELEPHONE_GetCalls( pMe->pIPhone, pCallsDesc, 
                            uSizeOfCallsDesc ) != SUCCESS )
  {
    MSG_FATAL( "ATResponseCLCC - ITELEPHONE_GetCalls failed", 0, 0, 0 );
    SEND_RESP( okResp );
    FREEIF( pCallsDesc );
    return;
  }
  
  MSG_FATAL( "ATResponseCLCC - active calls=%d", pCallsDesc->dwCount, 0, 0 );
  if ( pCallsDesc->dwCount > 0 )
  {
    /* process information for each call */
    for ( index = 0; index < pCallsDesc->dwCount; index++ )
    {
      /* get call info */
      result = ITELEPHONE_GetCallInfo( pMe->pIPhone, pCallsDesc->calls[index],
                                &callInfo, sizeof(callInfo) );
      if ( result != SUCCESS )
      {
        MSG_FATAL( "ATResponseCLCC - Cannot obtain call info for callID %d",
                   pCallsDesc->calls[index], 0, 0 );
        continue; // skip this call
      }
      MSG_FATAL( "ATResponseCLCC - id=%d dir|m=0x%04x st|prg=0x%04x", 
               pCallsDesc->calls[index], 
               ((uint16)(callInfo.direction << 8) | callInfo.call_type), 
               ((uint16)(callInfo.call_state << 8) | callInfo.call_progress)  );

      /* obtain call direction */
      if ( callInfo.direction == AEET_CALL_DIRECTION_MT )
      {
        callDirection = BTAPP_CALL_DIRECTION_MT;
      }
      else if ( (callInfo.direction == AEET_CALL_DIRECTION_MO) ||
                (pMe->mAG.callState == BTAPP_AG_CALL_ORGINATED) )
      {
        callDirection = BTAPP_CALL_DIRECTION_MO;
      }
      else
      {
        MSG_FATAL( "ATResponseCLCC - invalid dir %d",
                   callInfo.direction , 0, 0 );
        continue; // skip this call
      }

      /* obtain call status */
      if ( callInfo.call_state == AEET_CALL_STATE_CONV )
      {
        callStatus = BTAPP_CALL_STATE_ACTIVE;
      }
      else if ( callInfo.call_state == AEET_CALL_STATE_ONHOLD )
      {
        callStatus = BTAPP_CALL_STATE_HELD;
      }
      else if ( callInfo.call_state == AEET_CALL_STATE_ORIG )
      {
        if ( callInfo.call_progress == AEET_PROGIND_REMOTE_ALERT )
        {
          callStatus = BTAPP_CALL_STATE_ALERTING;
        }
        else
        {
          callStatus = BTAPP_CALL_STATE_DIALING;
        }
      }
      else if ( callInfo.call_state == AEET_CALL_STATE_INCOM )
      {
        callStatus = BTAPP_CALL_STATE_INCOMING;      
      }
      else if ( callInfo.call_state == AEET_CALL_STATE_DORMANT )
      {
        callStatus = BTAPP_CALL_STATE_WAITING;
      }
      else
      {
        MSG_FATAL( "ATResponseCLCC - invalid call state %d",
                   callInfo.call_state, 0, 0 );
        continue; // skip this call
      }
      /* obtain the call mode */
      if ( callInfo.call_type == AEET_CALL_TYPE_VOICE )
      {
        callMode = BTAPP_CALL_MODE_VOICE;
      }
      else if ( callInfo.call_type == AEET_CALL_TYPE_CS_DATA )
      {
        callMode = BTAPP_CALL_MODE_FAX;
      }
      else if ( callInfo.call_type == AEET_CALL_TYPE_PS_DATA )
      {
        callMode = BTAPP_CALL_MODE_DATA;
      }
      else
      {
        MSG_FATAL( "ATResponseCLCC - invalid call mode %d", 
                   callInfo.call_type, 0, 0 );
        continue; // skip this call
      }
      /* obtain multi party info */
      if ( pCallsDesc->dwCount >  1 )
      {
        callMultiparty = BTAPP_CALL_TYPE_MPTY;
      }
      else
      {
        callMultiparty = BTAPP_CALL_TYPE_NO_MPTY;
      }

      /* obtain the number type */
      if ( callInfo.other_party_no[0] == BTAPP_TA_INTER_PREFIX )
      {
        callNumType = BTAPP_TA_INTERNATIONAL;
      }
      else
      {
        callNumType = BTAPP_TA_UNKNOWN;
      }
      /* format the response packet */
      SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+CLCC: %d,%d,%d,%d,%d,\"%s\",%d\r\n", 
               pCallsDesc->calls[index], callDirection, callStatus, callMode,
               callMultiparty, callInfo.other_party_no, callNumType );

      result = SEND_RESP( rspBuf );
      if ( result != SUCCESS )
      {
        MSG_FATAL( "ATResponseCLCC - Failure in sending response", 0, 0, 0 );
      }
    }
  }
  SEND_RESP( okResp );
  FREEIF( pCallsDesc );
  return;
}

/* ==========================================================================
FUNCTION BTApp_ATResponseCOPS
DESCRIPTION
============================================================================= */
static void BTApp_ATResponseCOPS( CBTApp* pMe )
{
  int                   result = SUCCESS;
  uint8                 rspBuf[ BTAPP_RSP_BUFF_SIZE ];
  BTApp_ATCOPSModeType  mode = BTAPP_COPS_MODE_AUTO;
  AEETMMInfo*           pMMInfo = NULL;
  AEETPhInfo            m_phInfo;
#ifdef FEATURE_UI_CORE
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif
#endif //FEATURE_UI_CORE
  uint8                 format = BTAPP_COPS_FORMAT_LONG;
  boolean               bNameAvailable = FALSE;
  int                   length = 0;
  
  result = ITELEPHONE_GetPhoneInfo( pMe->pIPhone, &m_phInfo, 
                                    sizeof(AEETPhInfo) );
  if ( result == SUCCESS )
  {
    switch ( m_phInfo.network_sel_mode_pref )
    {
      case AEET_NETWORK_SEL_MODE_PREF_AUTOMATIC:
        mode = BTAPP_COPS_MODE_AUTO;
        break;
      case AEET_NETWORK_SEL_MODE_PREF_MANUAL:
      case AEET_NETWORK_SEL_MODE_PREF_LIMITED_SRV:
        mode = BTAPP_COPS_MODE_MANUAL;
        break;
      default:
        MSG_FATAL( "ATResponseCOPS - Mode:%d not supported", 
                 m_phInfo.network_sel_mode_pref, 0, 0 );
        BTApp_ATResponseErr( pMe, BTAPP_CME_OP_NOT_ALLOWED );
        return;
    }
    /* obtain network operator name */
    if ( ITELEPHONE_GetServingSystemInfo( pMe->pIPhone, pMe->mAG.pSSInfo, 
                                          sizeof(AEETSSInfo)) != SUCCESS )
    {
      MSG_FATAL( "ATResponseCOPS - SS Info not available", 0, 0, 0 );
      BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
      return;
    }
    pMMInfo = &pMe->mAG.pSSInfo->mm_information;
    if ( pMMInfo->full_name_avail != FALSE )
    {
      length = MIN( pMMInfo->full_name.length, BTAPP_COPS_NAME_LEN_LONG );
      MEMCPY( pMe->mAG.nwOperatorName,
              pMMInfo->full_name.name,
              length );
      bNameAvailable = TRUE;
      MSG_FATAL( "ATResponseCOPS - name from ITELEPHONE, len:%d", length, 0, 0 );
    }
#ifdef FEATURE_UI_CORE
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#endif //FEATURE_UI_CORE
    if ( bNameAvailable != FALSE )
    {
      pMe->mAG.nwOperatorName[ length ] = '\0';
      SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+COPS: %d,%d,\"%s\"\r\n", (uint8)mode, format,
               pMe->mAG.nwOperatorName );
    }
    else
    {
      /* name not available send mode field only */
      MSG_FATAL( "ATResponseCOPS - n/w Optr Name not available", 0, 0, 0 );     
      SNPRINTF( (char*)rspBuf, sizeof(rspBuf), "\r\n+COPS: %d\r\n", mode );      
    }
    STRLCAT( (char*)rspBuf, (char*)okResp, sizeof(rspBuf ) );
    result = SEND_RESP( rspBuf );
  }
  if ( result != SUCCESS )
  {
    MSG_FATAL( "ATResponseCOPS - Failure in sending response", 0, 0, 0 );
    BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
  }
}

/* ==========================================================================
FUNCTION BTApp_ExtractATCommands
DESCRIPTION
============================================================================= */
static void BTApp_ProcessATCommands( CBTApp* pMe )
{
  int result = SUCCESS;

  switch ( pMe->mAG.parseData.cmdType )
  {
    case BTAPP_AT_CMD_SET_COPS:
    {
      uint8  mode;
      int    format;

      MSG_FATAL( "ProcessATCommands - AT+COPS=", 0, 0, 0 );
      if ( (pMe->mAG.parseData.rx_params[0] != NULL) &&
           (pMe->mAG.parseData.rx_params[1] != NULL) )
      {
        mode = ATOI((const char*)pMe->mAG.parseData.rx_params[0]);
        format = ATOI((const char*)pMe->mAG.parseData.rx_params[1]);
        /* validate params */
        if ( (mode == BTAPP_COPS_MODE_SETFMT) && 
             (format == BTAPP_COPS_FORMAT_LONG) )
        {
          MSG_FATAL( "ProcessATCommands - Send OK Response", 0, 0, 0 );
          result = SEND_RESP( okResp );
        }
        else
        {
          BTApp_ATResponseErr( pMe, BTAPP_CME_OP_NOT_ALLOWED );
        }
      }
      else
      {
        BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
      }
      break;
    }
    case BTAPP_AT_CMD_READ_COPS:
    {
      MSG_FATAL( "ProcessATCommands - AT+COPS?", 0, 0, 0 );
#ifdef FEATURE_ICM
      BTApp_ATResponseCOPS( pMe );
#else
      BTApp_ATResponseErr( pMe, BTAPP_CME_OP_NOT_SUPPORTED );
#endif
      break;
    }
    case BTAPP_AT_CMD_CLCC:
    {
      MSG_FATAL( "ProcessATCommands - AT+CLCC", 0, 0, 0 );
#ifdef FEATURE_ICM
      BTApp_ATResponseCLCC( pMe );
#endif
      break;
    }
    case BTAPP_AT_CMD_CNUM:
    {
      MSG_FATAL( "ProcessATCommands - AT+CNUM", 0, 0, 0 );
#ifdef FEATURE_ICM
      BTApp_ATResponseCNUM( pMe );
#else
      BTApp_ATResponseErr( pMe, BTAPP_CME_OP_NOT_SUPPORTED );
#endif
      break;
    }
    case BTAPP_AT_CMD_CMEE:
    {
      BTApp_ATCMEEErrMode errMode;

      MSG_FATAL( "ProcessATCommands - AT+CMEE=", 0, 0, 0 );
      if ( pMe->mAG.parseData.rx_params[0] != NULL )
      {
        errMode = (BTApp_ATCMEEErrMode)ATOI( (const char*)pMe->mAG.parseData.
                                             rx_params[0] );
        /* validate error mode */
        if ( (errMode == BTAPP_CMEE_ERR_DISABLE) || 
             (errMode == BTAPP_CMEE_ERR_ENABLE_NUMERIC) ||
             (errMode == BTAPP_CMEE_ERR_ENABLE_VERBOSE) )
        {
          pMe->mAG.errATMode = errMode;
          MSG_FATAL( "AT+CMEE : Sending OK Resp", 0, 0, 0 );
          result = SEND_RESP( okResp );
        }
        else
        {
          BTApp_ATResponseErr( pMe, BTAPP_CME_OP_NOT_ALLOWED );
        }
      }
      else
      {
        BTApp_ATResponseErr( pMe, BTAPP_CME_AG_FAILURE );
      }
      break;
    }
    case BTAPP_AT_CMD_READ_CSCS:
    {
      MSG_FATAL( "ProcessATCommands - AT+CSCS?", 0, 0, 0 );
      BTApp_ATResponseReadCSCS( pMe );
      break;
    }
    case BTAPP_AT_CMD_QUERY_CSCS:
    {
      MSG_FATAL( "ProcessATCommands - AT+CSCS=?", 0, 0, 0 );
      BTApp_ATResponseQueryCSCS( pMe );
      break;
    }
    case BTAPP_AT_CMD_SET_CSCS:
    {
      MSG_FATAL( "ProcessATCommands - AT+CSCS=", 0, 0, 0 );
      if ( pMe->mAG.parseData.rx_params[0] != NULL )
      {
        uint8  i = 0;
        char* pStr = (char*)(pMe->mAG.parseData.rx_params[ 0 ]);
        uint8  numChars = STRLEN( pStr );

        if ( *pStr == '\"' )
        {
          pStr++;
          numChars = STRRCHR( pStr, '\"' ) - pStr;
        }
        while ( i < ARR_SIZE(BTApp_ATCSCSTable) )
        {
          if ( STRNCMP( pStr, BTApp_ATCSCSTable[i].char_set, numChars ) == 0 )
          {
            MSG_HIGH( "ProcessATCommands - AT+CSCS CharSet:%d", 
                      BTApp_ATCSCSTable[i].char_type, 0, 0 );
            pMe->mAG.charSetType = BTApp_ATCSCSTable[i].char_type;
            SEND_RESP( okResp );
            break;
          }
          i++;
        }
        if ( i == ARR_SIZE(BTApp_ATCSCSTable) )
        {
          MSG_FATAL( "ProcessATCommands - AT+CSCS, invalid char set", 0, 0, 0 );
          SEND_RESP( errorResp );
        }
      }
      else
      {
        MSG_FATAL( "ProcessATCommands - AT+CSCS, expected parms", 0, 0, 0 );
        SEND_RESP( errorResp );
      }      
      break;
    }
    case BTAPP_AT_CMD_CSQ:
    {
      MSG_FATAL( "ProcessATCommands - AT+CSQ", 0, 0, 0 );
      BTApp_ATResponseCSQ( pMe );
      break;
    }
    case BTAPP_AT_CMD_CBC:
    {
      MSG_FATAL( "ProcessATCommands - AT+CBC", 0, 0, 0 );
      BTApp_ATResponseCBC( pMe );
      break;
    }
    case BTAPP_AT_CMD_CIMI:
    {
      MSG_FATAL( "ProcessATCommands - AT+CIMI", 0, 0, 0 );
      if ( BTApp_ATResponseCIMI( pMe ) == FALSE )
      {
        BTApp_ATResponseErr( pMe, BTAPP_CME_SIM_FAILURE );
      }
      break;
    }
    case BTAPP_AT_CMD_BLDN:
    {
      MSG_FATAL( "ProcessATCommands - AT+BLDN", 0, 0, 0 );
      BTApp_HandleEventDevRedial( pMe );
      break;
    }
    case BTAPP_AT_CMD_BTRH:
    {
      MSG_FATAL( "ProcessATCommands - AT+BTRH?", 0, 0, 0 );
      SEND_RESP( okResp );
      break;
    }
    case BTAPP_AT_CMD_MEM_DIAL:
    {
      MSG_FATAL( "ProcessATCommands - ATD>", 0, 0, 0 );
      if ( pMe->mAG.parseData.rx_params[0] != NULL )
      {
        BTApp_HandleEventDevMemDial( pMe );
        SEND_RESP( ( pMe->mAG.bDevDialed == FALSE ) ? errorResp : okResp );
      }
      break;
    }
    default:
      MSG_FATAL( "ProcessATCommands - Unknown command", 0, 0, 0 );
      SEND_RESP( errorResp );
      break;
  }
  if ( pMe->mAG.parseData.rx_params[ 0 ] != NULL )
  {
    FREE((void*)pMe->mAG.parseData.rx_params[ 0 ]);
    pMe->mAG.parseData.rx_params[ 0 ] = NULL;
  }
  if ( pMe->mAG.parseData.rx_params[ 1 ] != NULL )
  {
    FREE((void*)pMe->mAG.parseData.rx_params[ 1 ]);
    pMe->mAG.parseData.rx_params[ 1 ] = NULL;
  }

  if ( result != SUCCESS )
  {
    MSG_FATAL( "ProcessATCommand - failed to send response r=%d", result, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ExtractATParams
DESCRIPTION
============================================================================= */
static boolean BTApp_ExtractATParams( 
  CBTApp* pMe, 
  uint8* pParamsBuff )
{
  const char* pEndOfParam;
  const char* pStartOfParam;
  uint8       LenOfParam;
  uint8       rx_param_count;
  uint8*      pParam = NULL;
  boolean     result = TRUE;
  int         i;

  MSG_FATAL( "ExtractATParams - parm cnt=%d", 
           pMe->mAG.parseData.rx_params_count, 0, 0 );
  pStartOfParam = (const char*)pParamsBuff;
  rx_param_count = pMe->mAG.parseData.rx_params_count;
  while ( ((pEndOfParam = STRCHR(pStartOfParam, 
                                 BTAPP_CMD_PARAM_SEPARATOR)) != NULL) ||
          ((pEndOfParam = STRCHR(pStartOfParam, 
                                 BTAPP_CMD_TERM_CHAR)) != NULL) ||
          ((pEndOfParam = STRCHR(pStartOfParam, 
                                 BTAPP_CMD_DELIMITER)) != NULL) ||
          ((pEndOfParam = STRCHR(pStartOfParam, '\0')) != NULL) )
  {
    if ( rx_param_count >= BTAPP_CMD_PARAMS_MAX )
    {
      MSG_FATAL( "ExtractATParams - More AT Params then allowed", 0, 0, 0 );
      result = FALSE;
      break;
    }
    LenOfParam = pEndOfParam - pStartOfParam;
    if ( pMe->mAG.parseData.bParamComplete == FALSE )
    {
      LenOfParam += STRLEN((const char*)pMe->mAG.parseData.rx_params[rx_param_count]);
    }
    pParam = (uint8*)MALLOC(LenOfParam + 1);
    if ( pParam == NULL )
    {
      MSG_FATAL( "ExtractATParams - Memory allocation failure", 0, 0, 0 );
      result = FALSE;
      break;
    }
    if ( pMe->mAG.parseData.bParamComplete == FALSE )
    {
      i = STRLEN((const char*)pMe->mAG.parseData.rx_params[rx_param_count]);
      MEMCPY( pParam, pMe->mAG.parseData.rx_params[rx_param_count], i );
      MEMCPY( pParam, pStartOfParam, (LenOfParam - i) );
      FREE(pMe->mAG.parseData.rx_params[rx_param_count]);
      pMe->mAG.parseData.bParamComplete = TRUE;
    }
    else
    {
      MEMCPY( pParam, pStartOfParam, LenOfParam );
    }
    pParam[LenOfParam] = NULL; //null terminated
    pMe->mAG.parseData.rx_params[rx_param_count] = pParam;
    rx_param_count++;
    pMe->mAG.parseData.rx_params_count = rx_param_count;
    if ( (*pEndOfParam == BTAPP_CMD_TERM_CHAR) || 
         (*pEndOfParam == BTAPP_CMD_DELIMITER) )
    {
      /* completed parsing the cmd parameters */
      result = TRUE;
      break;
    }
    if ( *pEndOfParam == NULL )
    {
      /* parameter received is not complete */
      pMe->mAG.parseData.bParamComplete = FALSE;
      result = TRUE;
      break;      
    }
    pStartOfParam = ++pEndOfParam;
  }
  if ( result == FALSE )
  {
    /* free memory allocated for params */
    for ( i =0; i < BTAPP_CMD_PARAMS_MAX; i++)
    {
      if ( pMe->mAG.parseData.rx_params[i] != NULL )
      {
        FREE(pMe->mAG.parseData.rx_params[i]);
        pMe->mAG.parseData.rx_params[i] = NULL;
      }
    }
  }
  return result;
}

/* ==========================================================================
FUNCTION BTApp_RetrieveATCmd
DESCRIPTION
============================================================================= */
static void BTApp_RetrieveATCmd( CBTApp* pMe )
{
  int         result;
  uint8       i;
  uint8       bufIdx;
  uint8       bytesRcvd      = BTAPP_CMD_BUFF_SIZE;
  uint8*      pParamsBuff    = NULL;
  boolean     bResult        = TRUE;
  BTApp_ATCmdType  cmdType   = BTAPP_AT_CMD_MIN;

  /* parse AT commands instead of parameters first */
  pMe->mAG.parseData.bParamComplete = TRUE;

  /* while there's more data follow */
  while ( bytesRcvd == BTAPP_CMD_BUFF_SIZE )
  {
    result = IBTEXTAG_RetrieveCommand( pMe->mAG.po, pMe->mAG.externalIOBuffer, 
                                       &bytesRcvd );
    if ( result != SUCCESS )
    {
      MSG_FATAL( "RetrieveATCmd - failed retrieving AT cmd r=%d", 
                 result, 0, 0 );
      break;
    }

    /* check if some more data follows */
    if ( bytesRcvd == BTAPP_CMD_BUFF_SIZE )
    {
      if (pMe->mAG.externalIOBuffer[ BTAPP_CMD_BUFF_SIZE - 1] != BTAPP_CMD_TERM_CHAR)
      {
        /* last character null will be used in ExtractATParams */
        pMe->mAG.externalIOBuffer[ bytesRcvd ] = NULL; /* terminate with NULL */
      }
    }
    else
    {
      if ( pMe->mAG.externalIOBuffer[ bytesRcvd - 1] != BTAPP_CMD_TERM_CHAR )
      {
        MSG_FATAL( "AT command not terminated with term character", 0, 0, 0);
        return;
      }
      else
      {
        pMe->mAG.externalIOBuffer[ bytesRcvd ] = NULL;
      }
    }

    DBGPRINTF_FATAL ( "%s", pMe->mAG.externalIOBuffer );

    bufIdx = 0;

    while ( (bufIdx < BTAPP_CMD_BUFF_SIZE) &&
            (pMe->mAG.externalIOBuffer[ bufIdx ] != NULL) )
    {
      /* if NOT the first AT command in the buffer */
      if ( bufIdx != 0 )
      {
        /* set index to the first char of the next AT command */
        if ( pMe->mAG.externalIOBuffer[ ++bufIdx ] == NULL )
        {
          /* reach the end of the buffer */
          break;
        }
      }

      /* check if received buff contains cmd+params or params */
      if ( pMe->mAG.parseData.bParamComplete != FALSE )
      {
        /* initialize parser data */
        pMe->mAG.parseData.cmdType = BTAPP_AT_CMD_MAX;
        pMe->mAG.parseData.rx_params_count = 0;
        //pMe->mAG.parseData.bParamComplete = TRUE;

        for ( i=0; i<BTAPP_CMD_PARAMS_MAX; i++ )
        {
          if ( pMe->mAG.parseData.rx_params[i] != NULL )
          {
            FREE( (void*)pMe->mAG.parseData.rx_params[i] );
          }
          pMe->mAG.parseData.rx_params[i] = NULL;
        }

        /* check for AT command */
        /* the algorithm used here is simple but doesn't cover the case that
        ** an AT commmand (exclusive of parameters) is truncated into two
        ** pieces due to buffer size limitation.  But this is not likely to
        ** happen, because:
        **  1. we actually have a "huge" buffer compared with normal length
        **     of a single AT command.
        **  2. we process AT commands fast enough so that no AT command will
        **     be truncated - normally, there will be no more than two AT
        **     commands in the buffer.
        */
        for ( cmdType=BTAPP_AT_CMD_MIN; cmdType<BTAPP_AT_CMD_MAX; cmdType++ )
        {
          if ( MEMCMP( pMe->mAG.externalIOBuffer + bufIdx, 
                       BTApp_ATCmdTable[cmdType].cmd_text,
                       STRLEN( BTApp_ATCmdTable[cmdType].cmd_text )) == 0 )
          {
            pMe->mAG.parseData.cmdType = cmdType;
            MSG_FATAL( "RetrieveATCmd - CmdType=%d", cmdType, 0, 0 );
            pParamsBuff = pMe->mAG.externalIOBuffer + bufIdx +
                            STRLEN( BTApp_ATCmdTable[cmdType].cmd_text );
            /* extract parameters */
            if ( BTApp_ATCmdTable[cmdType].param_cnt > 0 )
            {
              bResult = BTApp_ExtractATParams( pMe, pParamsBuff );
            }
            break;
          }
        }

        if ( cmdType == BTAPP_AT_CMD_MAX )
        {
          MSG_FATAL( "RetrieveATCmd - Sending ERROR response", 0, 0, 0 );

          result = SEND_RESP( errorResp );
          if ( result != SUCCESS )
          {
            MSG_FATAL( "RetrieveATCmd - failed to send response r=%d", result, 0, 0 );
          }
          break;
        }
      }
      else
      {
        pParamsBuff = pMe->mAG.externalIOBuffer;
        bResult = BTApp_ExtractATParams( pMe, pParamsBuff );
      }

      /* decide how to process current AT command */
      if ( bResult == FALSE )
      {
        MSG_HIGH( "RetrieveATCmd - failure in extracting AT params", 0, 0, 0 );

        result = SEND_RESP( errorResp );
        if ( result != SUCCESS )
        {
          MSG_FATAL( "RetrieveATCmd - failed to send response r=%d", result, 0, 0 );
        }
      }
      else if ( pMe->mAG.parseData.bParamComplete != FALSE )
      {
        /* process command */
        BTApp_ProcessATCommands( pMe );
      }
      else
      {
        /* break to retrieve data from the RX watermark */
        break;
      }

      /* set buffer index to the end of current AT command */
      while ( (pMe->mAG.externalIOBuffer[bufIdx] != NULL) &&
              (pMe->mAG.externalIOBuffer[bufIdx] != BTAPP_CMD_TERM_CHAR) )
      {
        bufIdx++;
      }
    }
  }
}
#endif /* FEATURE_BT_HFP_1_5 */

/* ==========================================================================
FUNCTION BTApp_DoDeviceSearch
DESCRIPTION
============================================================================= */
static boolean BTApp_DoDeviceSearch(
  CBTApp              *pMe, 
  AEEBTServiceClass   svcCls,
  const AEEBTBDAddr*  pAddr
)
{
  boolean success = FALSE; 

  pMe->mSD.uNumRecs = 0;
  MEMSET( pMe->mSD.devRec, 0, (sizeof(AEEBTDeviceRecord)*MAX_DEVICES) );
  success = ( IBTEXTSD_DiscoverDevices( pMe->mSD.po, svcCls, pAddr,
                                        pMe->mSD.devRec, 
                                        MAX_DEVICES ) == SUCCESS );
  if ( success )
  {
    MSG_FATAL( "DevSrch - DiscDev requested svcCls=%x", svcCls, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_SEARCHING, NULL, 0 );
  }
  else
  {
    MSG_FATAL( "DevSrchAll - DiscDev failed", 0, 0, 0 );
  }
  return success;
}

/* ==========================================================================
FUNCTION BTApp_StopDeviceDiscovery
DESCRIPTION
============================================================================= */
static boolean BTApp_StopDeviceDiscovery( CBTApp* pMe )
{
  boolean success = FALSE;

  success = ( IBTEXTSD_StopDeviceDiscovery( pMe->mSD.po ) == SUCCESS );

  BTApp_ShowMessage( pMe, IDS_MSG_NO_DEV_DISC, NULL, 4 );

  return success;
}

/* ==========================================================================
FUNCTION BTApp_DoUnbondAll
DESCRIPTION
============================================================================= */
/*static*/ boolean BTApp_DoUnbondAll( CBTApp* pMe )
{
  boolean               success = TRUE;
  AEEBTDeviceEnumerator enumerator;
  AEEBTDeviceInfo*      pDev = &pMe->mRM.device[ 0 ];
  uint8 i = 0;

  enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
  enumerator.bBonded = TRUE;

  //(void) POP_MENU();

  if ( IBTEXTRM_DeviceEnumInit( pMe->mRM.po, &enumerator ) == SUCCESS )
  {
    while ( (IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) &&
            pDev->bValid && (i++ < MAX_DEVICES) )
    {
      IBTEXTRM_Unbond( pMe->mRM.po, &pDev->bdAddr );
    }
  }
  else
  {
    MSG_FATAL( "DoUnbondAll - DeviceEnumInit failed", 0, 0, 0 );
    success = FALSE;
  }

  MSG_FATAL("***zzg BTApp_DoUnbondAll DeviceNum=%d***", i, 0, 0);

  if ( i == 0 )
  {
    BTApp_ShowMessage( pMe, IDS_MSG_NO_BONDED_DEV, NULL, 1 );
  }
  else
  {
    MSG_FATAL( "DoUnbondAll - #devs=%d", i, 0, 0 );
    BTApp_ShowBusyIcon( pMe );
  }
  return success;
}

/* ==========================================================================
FUNCTION BTApp_DoRemoveAll
DESCRIPTION
============================================================================= */
/*static*/ boolean BTApp_DoRemoveAll( CBTApp* pMe )
{
  boolean               success = TRUE;
  AEEBTDeviceEnumerator enumerator;
  AEEBTDeviceInfo*      pDev = &pMe->mRM.device[ 0 ];
  uint8 i = 0;

  enumerator.control = AEEBT_RM_EC_ALL;

  if ( IBTEXTRM_DeviceEnumInit( pMe->mRM.po, &enumerator ) == SUCCESS )
  {
    while ( (IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) &&
            pDev->bValid && (i++ < MAX_DEVICES) )
    {
      IBTEXTRM_DeviceRemove( pMe->mRM.po, &pDev->bdAddr );
    }
  }
  else
  {
    MSG_FATAL( "DoRemoveAll - DeviceEnumInit failed", 0, 0, 0 );
    success = FALSE;
  }

  MSG_FATAL("***zzg BTApp_DoRemoveAll DeviceNum=%d***", i, 0, 0);

  if ( i == 0 )
  {
    BTApp_ShowMessage( pMe, IDS_MSG_NO_DEV, NULL, 1 );
  }
  else
  {
    MSG_FATAL( "DoRemoveAll - #devs=%d", i, 0, 0 );
    BTApp_ShowBusyIcon( pMe );
  }
  return success;
}
#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_DoRebond
DESCRIPTION
============================================================================= */
/*static*/ boolean BTApp_DoRebond( CBTApp* pMe ,boolean bAuthorized )
{
  boolean               success = TRUE;
  AEEBTRebondSecurity   rebondSec;

  if ( bAuthorized == TRUE)
  {
    if ((pMe->mRM.bRebondOptMITMEnabled == TRUE) && 
        (pMe->mRM.bRebondOptBONDEnabled == TRUE))
    {
      rebondSec = AEEBT_REBOND_MITM_BOND;   
    }
    else if((pMe->mRM.bRebondOptMITMEnabled == TRUE) && 
            (pMe->mRM.bRebondOptBONDEnabled == FALSE))
    {
      rebondSec = AEEBT_REBOND_MITM;   
    }
    else if((pMe->mRM.bRebondOptMITMEnabled == FALSE) && 
            (pMe->mRM.bRebondOptBONDEnabled == TRUE))
    {
      rebondSec = AEEBT_REBOND_BOND;   
    }
    else 
    {
      rebondSec = AEEBT_REBOND_DONT_CARE;   
    }
  }
  else
  {
    rebondSec = AEEBT_REBOND_DONT_CARE;
  }

  if ( IBTEXTRM_AuthorizeRebond( pMe->mRM.po, &(pMe->mRM.BondBDAddr), 
                                 bAuthorized, rebondSec ) != SUCCESS )
  {
    MSG_FATAL( "DoRebond - Rebond failed", 0, 0, 0 );
    success = FALSE;
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
 
  return success;
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_DoUnbondOne
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_DoUnbondOne( CBTApp* pMe )
{
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  MSG_FATAL( "DoUnbondOne - %2x%2x%2x", pDev->bdAddr.uAddr[0], 
           pDev->bdAddr.uAddr[1], pDev->bdAddr.uAddr[2] );

  (void) POP_MENU();
  if ( IBTEXTRM_Unbond( pMe->mRM.po, &pDev->bdAddr ) != SUCCESS )
  {
    BTApp_ShowDevMsg( pMe, IDS_MSG_DEV_UNBOND_FAILED, &pDev->bdAddr, 0 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
}

/* ==========================================================================
FUNCTION BTApp_DoRemoveOne
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_DoRemoveOne( CBTApp* pMe )
{
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  MSG_FATAL( "DoRemoveOne - %2x%2x%2x", pDev->bdAddr.uAddr[0], 
           pDev->bdAddr.uAddr[1], pDev->bdAddr.uAddr[2] );

  if ( IBTEXTRM_DeviceRemove( pMe->mRM.po, &pDev->bdAddr ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_MSG_DEV_REM_FAILED, NULL, 1 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
}

/* ==========================================================================
FUNCTION BTApp_DoBrowseService
DESCRIPTION
============================================================================= */
static void BTApp_DoBrowseService( CBTApp* pMe, AEEBTDeviceInfo* pDev )
{
  if ( (IBTEXTSD_BrowseService( pMe->mSD.po, &pDev->bdAddr,
                                pMe->mSD.browseRec, 
                                AEEBT_MAX_NUM_OF_SRV_REC )) == SUCCESS )
  {
    MSG_FATAL( "DoBrowseService - Started...", 0, 0, 0 );
    BTApp_ShowDevMsg( pMe, IDS_MSG_BROWSING, &pDev->bdAddr, 0 );
  }
  else
  {
    MSG_FATAL( "DoBrowseService failed", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_DoSearchService
DESCRIPTION
============================================================================= */
static void BTApp_DoSearchService( CBTApp* pMe, AEEBTDeviceInfo* pDev )
{
  AEEBTSearchPattern  srchPattern;

  srchPattern.uNumUUID128s = 0;
  srchPattern.uNumUUIDs = 1;
  srchPattern.aUUID[0]  = AEEBT_SD_PROTOCOL_L2CAP;
  srchPattern.uNumAttrIDs         = 1;
  srchPattern.aAttrID[0].bIsRange = FALSE;
  srchPattern.aAttrID[0].uValue   = 
    AEEBT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;

  if ( (IBTEXTSD_SearchService( pMe->mSD.po, &pDev->bdAddr, &srchPattern,
                                pMe->mSD.svcRec, MAX_DEVICES )) == SUCCESS )
  {
    BTApp_ShowBusyIcon( pMe );
  }
  else
  {
    MSG_FATAL( "DoSearchService failed", 0, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_DoSPP
DESCRIPTION
============================================================================= */
static void BTApp_DoSPP( CBTApp* pMe, boolean bClient )
{
  int                 result = SUCCESS;
  AEEBTSppOpenConfig  openCfg;

  if ( BTApp_HCIModeOn( pMe ) == TRUE )
  {
    return;
  }

  // reset counts
  pMe->mSPP.uTotalBytesRcvd = 0;
  pMe->mSPP.uTotalBytesSent = 0;
  pMe->mSPP.bDoVerify       = TRUE;
  SPP_CLEAR_BUF();

  if ( pMe->mSPP.status.state != AEEBT_SPP_ST_CLOSED )
  {
    MSG_FATAL( "DoSPP - ending previously started SPP test.", 0, 0, 0 );
    BTApp_EndSPPTest( pMe );
  }

  openCfg.bClientApp = pMe->mSPP.bClient = bClient;
  if ( bClient )
  {
    openCfg.pBDAddr = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
  }
  else
  {
    openCfg.pBDAddr = NULL;
    result = IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
  }
  openCfg.uChannelNumber = 0; // =BT_SPP_SCN_UNSPECIFIED;
  openCfg.pSvcName      = NULL;
  openCfg.uSvcId        = AEEBT_SD_SERVICE_CLASS_SERIAL_PORT;
  openCfg.uSvcVersion   = 0;
  openCfg.uMaxFrameSize = 0;

  if ( result != SUCCESS )
  {
    MSG_FATAL( "DoSPP - failed setting discoverable", 0, 0, 0 );
    return;
  }

  BTApp_SetBondable( pMe );

  if ( (result = IBTEXTSPP_Open( pMe->mSPP.po, &openCfg )) != SUCCESS )
  {
    AECHAR wBuf[ 32 ];
    MSG_FATAL( "DoSPP - failed to open SPP - client=%d r=%d", 
               bClient, result, 0 );
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                          IDS_MSG_SPP_OPEN_FAILED, pMe->pText1, 
                          LONG_TEXT_BUF_LEN*sizeof(AECHAR) );
    WSPRINTF( wBuf, sizeof(wBuf), pMe->pText1, result );
    BTApp_ShowMessage( pMe, 0, wBuf, 0 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
}

/* ==========================================================================
FUNCTION BTApp_EndSPPTest
DESCRIPTION
============================================================================= */
static void BTApp_EndSPPTest( CBTApp* pMe )
{
  int result;

  MSG_FATAL( "EndSPPTest - st=%d isCli=%d", 
           pMe->mSPP.status.state, pMe->mSPP.bClient, 0 );

  //clearing any unread data from the mSPP buffer
  SPP_CLEAR_BUF();
  pMe->mSPP.uTotalBytesRcvd = 0;
  pMe->mSPP.uTotalBytesSent = 0;

  CALLBACK_Cancel( &pMe->mSPP.readableCb );
  CALLBACK_Cancel( &pMe->mSPP.writeableCb );

  ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY) BTApp_BuildSppTestResults, 
                      pMe );
  ISHELL_CancelTimer( pMe->m_pShell, (PFNNOTIFY) BTApp_SendSPPData, pMe );

  if ( pMe->mSPP.status.state != AEEBT_SPP_ST_CLOSED )
  {
    if ( (result = IBTEXTSPP_Close( pMe->mSPP.po )) != SUCCESS )
    {
      MSG_FATAL( "EndSPPTest - failed to close SPP r=%d", result, 0, 0 );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_SetHCIMode
DESCRIPTION
============================================================================= */
static void BTApp_SetHCIMode( CBTApp* pMe, uint16 msgID )
{
  int          result;
  uint16       promptID;
  AECHAR       wBuf[ 32 ];
  AEEBTHCIMode HCIMode;

  switch ( msgID )
  {
    case IDS_OFF:
      HCIMode = AEEBT_HCIM_OFF;
      break;
    case IDS_ON:
      if ( pMe->mAG.bEnabled ||
           pMe->mNA.bEnabled ||
           pMe->mSD.bDiscoverable
#ifdef FEATURE_BT_EXTPF_OPP
           || pMe->mOPP.bRegistered || pMe->mOPP.bConnected
#endif
#ifdef FEATURE_BT_EXTPF_FTP
           || pMe->mFTP.bRegistered || pMe->mFTP.bConnected
#endif
#ifdef FEATURE_BT_EXTPF_BIP
           || pMe->mBIP.bRegistered || pMe->mBIP.bConnected
#endif
#ifdef FEATURE_BT_EXTPF_BPP
           || pMe->mBPP.bRegistered || pMe->mBPP.bConnected
#endif
#ifdef FEATURE_BT_EXTPF_CTP
           || pMe->mCTP.bRegistered || pMe->mCTP.bConnected
           || pMe->mICP.bRegistered || pMe->mICP.bConnected
#endif
           )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_BT_ACTIVE, NULL, 0 );
        return;
      }
      else
      {
        HCIMode = AEEBT_HCIM_STANDARD_HCI_VOC_ON;
      }
      break;
#ifdef FEATURE_FACTORY_TESTMODE
    case IDS_FTM:
      HCIMode = AEEBT_HCIM_STANDARD_HCI_FTM;
      break;
#endif
    default:
      MSG_FATAL( "SetHCIMode - unexpect msgID %x", msgID, 0, 0 );
      HCIMode = AEEBT_HCIM_OFF;
      break;
  }

  BTApp_ShowBusyIcon( pMe );
  result = IBTEXTRM_SetHCIMode( pMe->mRM.po, HCIMode );
  if ( result == SUCCESS )
  {
    promptID = IDS_MSG_HCI_MODE;
  }
  else if ( result == AEEBT_EPORT )
  {
#ifdef FEATURE_FACTORY_TESTMODE
    if ( HCIMode == AEEBT_HCIM_STANDARD_HCI_FTM )
    {
      promptID = IDS_MSG_UNMAP_HCI_PORT_FIRST;
    }
    else
#endif
    promptID = IDS_MSG_MAP_HCI_PORT_FIRST;
  }
  else
  {
    promptID = IDS_MSG_HCI_MODE_FAILED;
  }
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID, 
                        wBuf, sizeof( wBuf ) );
  BTApp_ShowMessage( pMe, promptID, wBuf, 2 );
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_LocalOOBCreated
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_LocalOOBCreated( CBTApp* pMe )
{
  IFile*    pIFile    = NULL;
  IFileMgr* pIFileMgr = NULL;
  uint8     uOOBDataBuffer[ 100 ];
  char      szOOBFileNm[ AEEBT_MAX_FILE_NAME+1 ];
  AECHAR    wBuf1 [18]; 
  char      szBDAddr [18];

  pMe->bBusyIconUp = FALSE; 

  MSG_FATAL( "OOB Local Data size 0x%x", pMe->mRM.OOBDataSize, 0, 0 );

  if ( IBTEXTRM_ReadOOBData( pMe->mRM.po, &pMe->mRM.OOBDataSize,
                             uOOBDataBuffer ) == SUCCESS )
  {
    if ( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                                (void **)&pIFileMgr ) == SUCCESS )
    {
		
      STRLCPY( szOOBFileNm, BTAPP_ROOT_DIR , sizeof (szOOBFileNm));
      STRLCAT( szOOBFileNm, DIRECTORY_STR , sizeof (szOOBFileNm) );
      STRLCAT( szOOBFileNm, "OOB-", sizeof (szOOBFileNm) );
      BTApp_BDAddr2Wstr (wBuf1, &(  pMe->mRM.myInfo.bdAddr ));
      WSTRTOSTR( wBuf1, szBDAddr, sizeof( szBDAddr ) );
      STRLCAT( szOOBFileNm, szBDAddr, sizeof (szOOBFileNm) );

      if( IFILEMGR_Test( pIFileMgr, szOOBFileNm ) != SUCCESS )
      {
        pIFile = IFILEMGR_OpenFile( pIFileMgr, szOOBFileNm , _OFM_CREATE );
      }
      else
      {   
        pIFile = IFILEMGR_OpenFile( pIFileMgr, szOOBFileNm , _OFM_READWRITE );
      }

      if ( (pIFile != NULL) )
      {
        IFILE_Write( pIFile, uOOBDataBuffer, pMe->mRM.OOBDataSize );
      }
      else
      {
        MSG_FATAL( "OOB Local Write failed", 0, 0, 0 );
      }
      
      if ( pIFile != NULL )
      {
        IFILE_Release( pIFile );
      }
        
      IFILEMGR_Release( pIFileMgr );
    }
  }   
  else
  {
    MSG_FATAL( "OOB Data Read Error", 0, 0, 0 );
  }
  // Show the status message
  BTApp_ShowMessage (pMe,IDS_MSG_OOB_DATA_CREATED, NULL, 2);
}
#endif /* FEATURE_BT_2_1 */


//Add By zzg 2011_01_10
void BTApp_CancelMsgBox( CBTApp* pMe )
{
  MSG_FATAL("***zzg BTApp_CancelMsgBox***", 0, 0, 0);
  
  ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelMsgBox, pMe);

  pMe->m_eDlgRet = DLGRET_MSGBOX_CANCELED;
  (void) ISHELL_EndDialog(pMe->m_pShell);   
}

void BTApp_CancelProMptBox(CBTApp* pMe)
{
  MSG_FATAL("***zzg BTApp_CancelProMptBox***", 0, 0, 0);
  
  ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_CancelProMptBox, pMe);

  pMe->m_eDlgRet = DLGRET_PROMPT_CANCELED;
  (void) ISHELL_EndDialog(pMe->m_pShell);   
}

//Add End



/* ==========================================================================
FUNCTION BTApp_DeviceChangeCallBack
DESCRIPTION
============================================================================= */
void BTApp_DeviceChangeCallBack( CBTApp* pMe,
                                 const uisnd_notify_data_s_type* pNotifyData )
{
  MSG_HIGH( "Device or Mute setting Changed, as=%d flag=%d",
            pMe->mAG.bAudioSelected, pMe->mAG.uFlags, 0 );

  MSG_HIGH( "BT Headset id %x Current Device id %x",
            SND_DEVICE_BT_HEADSET,
            pNotifyData->out_device, 0 );

  MSG_FATAL("***zzg BTApp_DeviceChangeCallBack out_device=%x***", pNotifyData->out_device, 0, 0);

  if ( pNotifyData->out_device != SND_DEVICE_BT_HEADSET )
  {
    /* if device change was caused by non-BT subsystems */
    if ( pMe->mAG.bAudioSelected == TRUE )
    {
      if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B ) ||
           (BTApp_CallPresent( pMe ) != BT_APP_CALL_NONE) )
      {
        /* set to indicate SCO needs to be brought up again,
         * when BT device is back in use.
         */
        BTAPP_AG_SETBIT( BTAPP_AG_FLAG_AUDIO_WANTED_B );
      }

		MSG_FATAL("***zzg BTApp_DeviceChangeCallBack BTApp_DisconnectAudio***", 0, 0, 0);
      BTApp_DisconnectAudio( pMe, FALSE );
    }
  }
  else /* BT device is back in use */
  {
    if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) )
    {
      /* SCO was needed and is now still needed */
      BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_AUDIO_WANTED_B );

	  MSG_FATAL("***zzg BTApp_DeviceChangeCallBack BTApp_ConnectAudio***", 0, 0, 0);
      BTApp_ConnectAudio( pMe, TRUE );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_RadioActivityChanged
DESCRIPTION
============================================================================= */
static void BTApp_RadioActivityChanged( CBTApp* pMe )
{
  boolean bDiscoverable = 
    (pMe->mRM.radioActivity & AEEBT_RA_INQ_SCANNING) ? TRUE : FALSE;

  MSG_FATAL( "Current radio activity 0x%x", pMe->mRM.radioActivity, 0, 0 );

  if ( pMe->mSD.bDiscoverable != bDiscoverable )
  {
    if ( (TOP_MENU == BT_APP_MENU_MY_INFO) || (TOP_MENU == BT_APP_MENU_MAIN) )
    {
      MSG_FATAL("BTApp_BuildMenu....................16",0,0,0);
      if(pMe->bStartBTaplication)
      {
      	BTApp_BuildMenu( pMe, TOP_MENU );
      }
    }
  }
}


/* ==========================================================================
FUNCTION BTApp_HandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleSelection( CBTApp* pMe, uint16 selection )
{
  boolean          built = TRUE;
  int              result;
  AECHAR           wShortText[ 10 ];
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  MSG_FATAL( "HndlSlction - sel=%d m=%d", selection, TOP_MENU, 0 );
  MENU_SET_SEL( selection );
  MSG_FATAL("BTApp_HandleSelection:::::::=%d",selection,0,0);
  switch ( selection )
  {
    case IDS_BT_ON:
    {
      MSG_FATAL( "HndlSlction - ON m=%d", TOP_MENU, 0, 0 );
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        MSG_FATAL("***zzg BTApp_HandleSelection BTApp_EnableBT***",0,0,0);
        pMe->mEnablingType = BTAPP_ENABLING_AG;
        BTApp_EnableBT( pMe );
		IDISPLAY_UpdateEx( pMe->m_pIDisplay, TRUE );	//Add By zzg 2010_12_24
      }
      break;
    }
    case IDS_BT_OFF:
    {
      MSG_FATAL( "HndlSlction - OFF m=%d", TOP_MENU, 0, 0 );
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        BTApp_DisableBT( pMe );		
		IDISPLAY_UpdateEx( pMe->m_pIDisplay, TRUE ); //Add By zzg 2010_12_24
      }
      break;
    }
    case IDS_BT_YES:
    {
      MSG_FATAL( "HndlSlction - YES m=%d", TOP_MENU, 0, 0 );
	  
      switch ( TOP_MENU )
      {
#ifdef FEATURE_BT_2_1
        case BT_APP_MENU_REBOND:
          BTApp_DoRebond( pMe , TRUE );
          break;
        case BT_APP_MENU_USER_CFM_RQST:
          BTApp_UserConfirm( pMe , TRUE );
          break;
        case BT_APP_MENU_DISPLAY_PASSKEY:
        {
          //ACG
          //Bonding has already started. SSPBond should not be sent at
          //this point
          /*
          if ( IBTEXTRM_SSPBond( pMe->mRM.po, &pDev->bdAddr, 
                              pMe->mRM.bMITMEnabled ) != SUCCESS )
          {
            MSG_FATAL( "HandleSelection YES -SSP Bond Failed", 0, 0, 0 );
            return FALSE;
          }
          */
          break;
        }
#endif /* FEATURE_BT_2_1 */

		//Add By zzg 2010_11_17
		case BT_APP_MENU_OPEN:
		{
			if ( BTApp_HCIModeOn( pMe ) == FALSE )
			{
                pMe->mEnablingType = BTAPP_ENABLING_AG;
				BTApp_EnableBT(pMe);
				
				BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_SENDFILE);
			}			
			break;
		}
		//Add End
        case BT_APP_MENU_UNBOND_ALL:
          BTApp_DoUnbondAll( pMe );
          break;
        case BT_APP_MENU_REMOVE_ALL:
          BTApp_DoRemoveAll( pMe );
          break;
        case BT_APP_MENU_UNBOND_ONE:
          BTApp_DoUnbondOne( pMe );
          break;
        case BT_APP_MENU_REMOVE_ONE:
          BTApp_DoRemoveOne( pMe );
          break;
        case BT_APP_MENU_USE_AUDIO_DEV:
        {
          pMe->mAG.bdAddr = pDev->bdAddr;
          BTApp_WriteConfigFile( pMe );
          BTApp_HandleClearKey( pMe );
          if ( pMe->mAG.bEnabled != FALSE )
          {
            BTApp_ShowDevMsg( pMe, IDS_MSG_MUST_REENABLE_AG, 
                              &pMe->mAG.bdAddr, 0 );
          }
          break;
        }
        case BT_APP_MENU_AG_CONNECT:
        {
          if ( pMe->mAG.bconnInPrgs == FALSE )
          {
            if ( pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET )
            {
              MSG_FATAL( "HndlSlction - connecting to HS", 0, 0, 0 );
            }
            else
            {
              MSG_FATAL( "HndlSlction - connecting to HF", 0, 0, 0 );
            }
            if ( BTApp_CallConnected( pMe ) != BT_APP_CALL_NONE )
            {
              pMe->mAG.bDevPickedUp = TRUE; // signal self to send audio to HS/HF
            }
            pMe->mAG.bconnInPrgs = TRUE;
            IBTEXTAG_Connect( pMe->mAG.po, &pDev->bdAddr, pMe->mAG.devType );
            BTApp_ShowBusyIcon( pMe );
          }
          else
          {
            BTApp_ShowMessage( pMe, IDS_AG_CONNECTION_IN_PRGS, NULL, 3 );
          }
          break;
        }
        case BT_APP_MENU_AG_DISCONNECT:
        {
			MSG_FATAL("***zzg IBTEXTAG_Disconnect***", 0, 0, 0);
          IBTEXTAG_Disconnect( pMe->mAG.po );
          BTApp_HandleClearKey( pMe );
          break;
        }
        case BT_APP_MENU_TEST_MODE:
        {
          if ( (result = IBTEXTRM_EnterTestMode( pMe->mRM.po )) != SUCCESS )
          {
            MSG_FATAL( "HndlSlction - failed to enter Test Mode r=%x", 
                       result, 0, 0 );
            ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                  IDS_MSG_TEST_MODE_NOT_ENTERED, 
                                  pMe->pText2, 
                                  SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );
            WSPRINTF( pMe->pText1, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ), 
                      pMe->pText2, result );
            BTApp_ShowMessage( pMe, 0, pMe->pText1, 0 );
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        }
        case BT_APP_MENU_AUTHORIZE_CONN:
        {
          if ( (result = IBTEXTRM_AuthorizeReply( pMe->mRM.po, 
                                                  &pMe->mRM.svcId,
                                                  &pDev->bdAddr,
                                                  TRUE )) != SUCCESS )
          {
            MSG_FATAL( "HndlSlction - AuthorizeReply failed r=%x", 
                       result, 0, 0 );
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        }
        case BT_APP_MENU_SET_CONN_ROLE:
        {
          AEEBTBDAddr* pBDAddr = NULL;
          if ( BDADDR_VALID( &pMe->mRM.devToSwitchRole->bdAddr ) != FALSE )
          {
            pBDAddr = &pMe->mRM.devToSwitchRole->bdAddr;
          }
          if ( (result = IBTEXTRM_SetConnectionRole( 
                           pMe->mRM.po, pBDAddr,
                           !(pMe->mRM.linkStatus->bMaster) )) != SUCCESS )
          {
            MSG_FATAL( "HndlSlction - SetConnectionRole failed r=%x", 
                       result, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_SET_CONN_ROLE_FAILED, NULL, 0 );
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        }
        default:
          MSG_FATAL( "HndlSlction - OK m=%d", TOP_MENU, 0, 0 );
          BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case IDS_BT_NO:
    {
      MSG_FATAL( "HndlSlction - NO m=%d", TOP_MENU, 0, 0 );
      if ( TOP_MENU == BT_APP_MENU_AUTHORIZE_CONN )
      {
        if ( (result = IBTEXTRM_AuthorizeReply( pMe->mRM.po, 
                                                &pMe->mRM.svcId,
                                                &pDev->bdAddr,
                                                FALSE )) != SUCCESS )
        {
          MSG_FATAL( "HndlSlction - AuthorizeReply failed r=%x", result, 0, 0 );
        }
        else
        {
          BTApp_ShowBusyIcon( pMe );
        }
        break;
      }
#ifdef FEATURE_BT_2_1
      
      else if ( TOP_MENU == BT_APP_MENU_REBOND )
      {
          BTApp_DoRebond( pMe , FALSE );
          break;
      }
      else if ( TOP_MENU == BT_APP_MENU_USER_CFM_RQST )
      {
        BTApp_UserConfirm( pMe , FALSE );
        break;
      }
#endif /* FEATURE_BT_2_1 */
      else
      {
        built = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case IDS_DEVICE_SEARCH:		
	{
	  boolean bt_status = FALSE;
	  ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));

	  if (bt_status == FALSE)
	  {
	  	BTApp_ShowMessage( pMe, IDS_BT_CLOSED, NULL, 5);
		return built;
	  }
	  else
	  {
	      if ( BTApp_HCIModeOn( pMe ) == FALSE )
	      {
	        //built = BTApp_BuildMenu( pMe, BT_APP_MENU_SEARCH );
		    //跳过SEARCH选项，直接搜索所有设备。
			built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_ALL, NULL );	//Modify by zzg 2010_11_02
	      }
		  
		  break;
	  }

	  break;
    }
    case IDS_DEVICES:
	{
	  boolean bt_status = FALSE;
	  //ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
	  if (bt_status == FALSE)  
	  {
	  	BTApp_ShowMessage( pMe, IDS_BT_CLOSED, NULL, 5);
		return built;
	  }
	  else
	  {
	      built = BTApp_BuildMenu( pMe, BT_APP_MENU_DEVICE );
	      break;
	  }
	  break;
	}
    case IDS_MY_INFO:
	{
	  boolean bt_status = FALSE;
	  ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
	 if (bt_status == FALSE)
	  {
	  	BTApp_ShowMessage( pMe, IDS_BT_CLOSED, NULL, 5);
		return built;
	  }
	  else
	  {
	      built = BTApp_BuildMenu( pMe, BT_APP_MENU_MY_INFO );
	      break;
	  }
	  break;
    }
	  
    case IDS_SETTINGS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_SETTINGS );
      break;
    case IDS_TESTS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_TESTS );
      break;
	  
	//Add By zzg 2010_11_17
	case IDS_BT_OPP_SETTING:
    {
		boolean bt_status = FALSE;
   	    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
   	    if (bt_status == FALSE)
    	{
    		BTApp_ShowMessage( pMe, IDS_BT_CLOSED, NULL, 5);
    		return built;
    	}
    	else
    	{
    		built = BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_SETTING );
    	}
		
		break;
	}
     
	//Add End
    case IDS_AUDIO_TRANSFER:
    {
      BTAppCallType callPresent = BTApp_CallPresent( pMe );
      MSG_FATAL( "HndlSlction - Audio Xfer c=%x", callPresent, 0, 0 );
      if ( pMe->mAG.bAudioConnected == FALSE )
      {
        BTApp_ConnectAudio( pMe, TRUE );
      }
      else
      {
#ifdef FEATURE_AVS_BT_SCO_REWORK
        BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* FEATURE_AVS_BT_SCO_REWORK */
        BTApp_DisconnectAudio( pMe, callPresent ? TRUE : FALSE );
      }
      BTApp_ShowBusyIcon( pMe );
      break;
    }
    case IDS_DEVICE_ALL:
      built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_ALL, NULL );
      break;
    case IDS_DEVICE_AUDIO:
      built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_AUDIO, NULL );
      break;
    case IDS_DEVICE_NETWORK:
      built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_NETWORKING, NULL );
      break;
    case IDS_OBEX_SERVERS:
      built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_OBJECT_TRANSFER, NULL );
      break;
    case IDS_DEVICE_TELEPHONY:
      built = BTApp_DoDeviceSearch( pMe, AEEBT_COD_SC_TELEPHONY, NULL );
      break;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    case IDS_BY_BD_ADDR:
      BTApp_ShowMessage( pMe, IDS_MSG_FEATURE_UNAVAIL, NULL, 4 ); // TBD
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
      break;
    case IDS_LIST_CONNECTED:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_CONNECTED );
      break;
    case IDS_LIST_BONDED:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_BONDED );
      break;
    case IDS_LIST_ALL:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
      break;
    case IDS_UNBOND_ALL:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_UNBOND_ALL );
      break;
    case IDS_REMOVE_ALL:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_REMOVE_ALL );
      break;
    case IDS_ENABLE_SERVICE_SECURITY:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mNA.bEnableSvcSec = pMe->mNA.bEnableSvcSec ? FALSE : TRUE;
      BTApp_EnableServiceSecurity( pMe );
      break;
    }
    case IDS_SET_CONN_ROLE:
    {
      *pMe->mRM.devToSwitchRole = pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
      BTApp_BuildMenu( pMe, BT_APP_MENU_SET_CONN_ROLE );
      break;
    }
    case IDS_ACCEPT_CONN_ROLE:
    {
      if ( IBTEXTRM_GetConnectionRole( 
             pMe->mRM.po, NULL, &pMe->mRM.linkStatus->bMaster ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_GET_CONN_ROLE_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mRM.devToSwitchRole->bdAddr = NULL_BD_ADDR;
        BTApp_BuildMenu( pMe, BT_APP_MENU_SET_CONN_ROLE );
      }
      break;
    }
    case IDS_CONFIG_SCAN_PARAMS:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_SCAN_PARAMS );
      break;
    }
    case IDS_HCI_MODE:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_HCI );
      break;
    case IDS_MASTER_CONTROL:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_MASTER_CONTROL );
      break;
    case IDS_RADIO_ON:
    case IDS_RADIO_OFF:
    {
      result = IBTEXTRM_DisableRadio( pMe->mRM.po, pMe->mRM.bRadioOn );
      if ( result == SUCCESS )
      {
        BTApp_ShowBusyIcon( pMe );
      }
      else if ( result == EUNSUPPORTED )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_FEATURE_UNAVAIL, NULL, 4 );
      }
      else
      {
        STRTOWSTR( pMe->mRM.bRadioOn ? "OFF" : "ON", 
                   wShortText, sizeof( wShortText ) );
        BTApp_ShowMessage( pMe, IDS_MSG_RADIO_CTRL_FAILED, wShortText, 3 );
      }
      break;
    }
    case IDS_VISIBILITY_ON:
    case IDS_VISIBILITY_OFF:
    {
      result = IBTEXTRM_DisableVisibility( pMe->mRM.po,
                                           pMe->mRM.bVisibilityOn );
      if ( result == SUCCESS )
      {
        BTApp_ShowBusyIcon( pMe );
      }
      else if ( result == EUNSUPPORTED )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_FEATURE_UNAVAIL, NULL, 4 );
      }
      else
      {
        STRTOWSTR( pMe->mRM.bVisibilityOn ? "OFF" : "ON", 
                   wShortText, sizeof( wShortText ) );
        BTApp_ShowMessage( pMe, IDS_MSG_VISIBILITY_CTRL_FAILED, wShortText, 3 );
      }
      break;
    }
    case IDS_ALLOW_ROLE_SWITCH:
    case IDS_DISALLOW_ROLE_SWITCH:
    {
      AEEBTBDAddr* pBDAddr = NULL;
      if ( TOP_MENU == BT_APP_MENU_DEV_OPTIONS )
      {
        pBDAddr = &pDev->bdAddr;
      }
      result = IBTEXTRM_AllowRoleSwitch( pMe->mRM.po, pBDAddr, 
                                         !pMe->mRM.bRoleSwitchAllowed );
      if ( result == SUCCESS )
      {
        BTApp_ShowBusyIcon( pMe );
      }
      else
      {
        MSG_FATAL( "HndlSlction - AllowRoleSwitch() failed %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_ROLE_SW_CTRL_FAILED, NULL, 3 );
      }
      break;
    }
    case IDS_SET_BONDABLE:
    {
      BTApp_SetBondable( pMe );
      break;
    }
    case IDS_CLEAR_BONDABLE:
    {
      /* Clear bondable irrespective of servers being registered*/
      if ( IBTEXTRM_SetBondable( pMe->mRM.po, FALSE ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_BONDABLE_NOT_CLEARED, NULL, 2 );
      }
      break;
    }
    case IDS_SPP_TESTS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_TESTS );
      break;
    case IDS_OBEX_TESTS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX );
      break;
    case IDS_TCS_TESTS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_TCS );
      break;
    case IDS_TEST_MODE:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_TEST_MODE );
      break;
    case IDS_EDIT_NAME:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_EDIT_NAME );
      break;
#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif /* FEATURE_BT_TEST_API */
    case IDS_EDIT_SHORT_NAME:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_EDIT_SHORT_NAME );
      break;
    case IDS_EDIT_MANU_DATA:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_EDIT_MANU_DATA );
      break;
    case IDS_DBG_KEY:
      built =  BTApp_BuildMenu( pMe, BT_APP_MENU_DBG_KEY );
      break;
#endif /* FEATURE_BT_2_1 */
    case IDS_SECURITY:
      if ( TOP_MENU == BT_APP_MENU_DEV_OPTIONS )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_SECURITY );
      }
      else
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_SECURITY );
      }
      break;
    case IDS_DISCOVERABLE:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_DISCOVERABLE );
      }
      break;
#ifdef FEATURE_BT_2_1
    case IDS_IOCAPABILITY:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_IOC_TYPE ); 
      break;
    }
    case IDS_SERV_SECURITY:
     {
       built = BTApp_BuildMenu( pMe, BT_APP_MENU_SVR_SEC_MAIN_TYPE ); 
       break;
     }
    case IDS_CHNG_LOW_PWR_MODE:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_LOW_POWER_MODE ); 
      break;
    }
    case IDS_CHNG_MAX_INTERVAL:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_MAX_INTERVAL ); 
      break;
    }
    case IDS_CHNG_IDLE_TIMEOUT:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_IDLE_TIMEOUT ); 
      break;
    }
    case IDS_CHNG_MAX_LATENCY:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_MAX_LATENCY ); 
      break;
    }
    case IDS_CHNG_SNIF_PARAM_SETTINGS:
    {
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_SNIF_PARAM_SETTINGS ); 
      break;
    }
    case IDS_WRITE_OOB:
    BTApp_ShowBusyIcon( pMe );
    CALLBACK_Init ( &pMe->mRM.OOBCreateCb, BTApp_LocalOOBCreated, pMe );
    if ( IBTEXTRM_CreateOOBData( pMe->mRM.po, &pMe->mRM.OOBDataSize, 
                               &pMe->mRM.OOBCreateCb ) != SUCCESS )
    {
      MSG_FATAL( "BTApp - Failed to create OOB data", 
               0, 0, 0 );
    }
    break;

#endif /* FEATURE_BT_2_1 */
    case IDS_TYPE:
    case IDS_MODE:
    case IDS_TIME:
    case IDS_SPKR:
    case IDS_MIC:
      break;
    case IDS_BROWSE_SVC:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        BTApp_DoBrowseService( pMe, pDev );
      }
      break;
    case IDS_SEARCH_SVC:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        BTApp_DoSearchService( pMe, pDev );
      }
      break;
    case IDS_REGISTER_SVC:
      BTApp_ShowMessage( pMe, IDS_MSG_FEATURE_UNAVAIL, NULL, 4 ); // TBD
      break;
    case IDS_UNREGISTER_SVC:
      BTApp_ShowMessage( pMe, IDS_MSG_FEATURE_UNAVAIL, NULL, 4 ); // TBD
      break;
    case IDS_SPP_SERVER:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        if ( pMe->mSPP.status.state != AEEBT_SPP_ST_CLOSED )
        {
          built = BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
        }
        else
        {
          BTApp_DoSPP( pMe, FALSE );
        }
      }
      break;
    case IDS_SPP_CLIENT:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        if ( pMe->mSPP.status.state != AEEBT_SPP_ST_CLOSED )
        {
          built = BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_RESULTS );
        }
        else
        {
          built = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
        }
      }
      break;
    case IDS_SPP_SETTINGS:
      built = BTApp_BuildMenu( pMe, BT_APP_MENU_SPP_SETTINGS );
      break;
#ifdef FEATURE_FACTORY_TESTMODE
    case IDS_FTM:
#endif
    case IDS_OFF:
    case IDS_ON:
      BTApp_SetHCIMode( pMe, selection );
      break;
#ifdef FEATURE_BT_EXTPF_OPP
    case IDS_OPP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
    case IDS_FTP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    case IDS_BIP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_BIP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
    case IDS_BPP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
    case IDS_PBAP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_IOBEX
    case IDS_OBEX:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_CTP
    case IDS_CTP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_CTP_TESTS );
      }
      break;
    case IDS_ICP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_ICP_TESTS );
      }
      break;
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
    case IDS_A2DP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_A2DP_TEST );
      }
      break;
    case IDS_AVRCP:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_AVRCP_TEST );
      }
      break;
#endif
    case IDS_L2:
      if ( BTApp_HCIModeOn( pMe ) == FALSE )
      {
        built = BTApp_BuildMenu( pMe, BT_APP_MENU_L2_TEST );
      }
      break;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    default:
      built = FALSE;
      break;
  }

  return built;
}

/*========================== END OF EVENT HANDLERS ==========================*/

#if defined (FEATURE_UI_CORE) && defined (FEATURE_BT)
/* ==========================================================================
FUNCTION BTApp_InitVolumes
DESCRIPTION
============================================================================= */
static void BTApp_InitVolumes( CBTApp* pMe )
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  MSG_FATAL("***zzg BTApp_InitVolumes***", 0, 0, 0);

  if ( pMe->mAG.bVolumesInit != FALSE )
  {
    return;
  }

  // beep volume
  result = OEMNV_Get( NV_BEEP_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.beep_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_BEEP_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_HEADSET, SND_METHOD_KEY_BEEP, 
                  nvi.beep_lvl_shadow, NULL, NULL );

  // voice volume
  result = OEMNV_Get( NV_EAR_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.ear_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_EAR_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_HEADSET, SND_METHOD_VOICE, 
                  nvi.ear_lvl_shadow, NULL, NULL );

  // message volume
  result = OEMNV_Get( NV_ALERTS_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.alerts_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_ALERTS_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_HEADSET, SND_METHOD_MESSAGE, 
                  nvi.alerts_lvl_shadow, NULL, NULL );

  // ring volume
  result = OEMNV_Get( NV_RINGER_LVL_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.ringer_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_RINGER_LVL_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_HEADSET, SND_METHOD_RING, 
                  nvi.ringer_lvl_shadow, NULL, NULL);

  // midi volume
#ifdef FEATURE_MULTIMEDIA
  result = OEMNV_Get( NV_MM_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.mm_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_MM_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_HEADSET, SND_METHOD_MIDI, 
                  nvi.mm_lvl_shadow, NULL, NULL );
  
#endif
  pMe->mAG.bVolumesInit = TRUE;
}

#ifdef FEATURE_BT_EXTPF_AV
/* ==========================================================================
FUNCTION BTApp_InitA2DPVolumes
DESCRIPTION
============================================================================= */
void BTApp_InitA2DPVolumes( CBTApp* pMe )
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  MSG_FATAL("***zzg BTApp_InitA2DPVolumes***", 0, 0, 0);

  if ( pMe->mA2DP.bVolumesInit != FALSE )
  {
    return;
  }
  MSG_FATAL("InitA2DPVolumes - Initializing the volume setting for A2DP device", 0, 0, 0 );
  
  // beep volume
  result = OEMNV_Get( NV_BEEP_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.beep_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_BEEP_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_A2DP_HEADSET, SND_METHOD_KEY_BEEP, 
                  nvi.beep_lvl_shadow, NULL, NULL );

  // voice volume
  result = OEMNV_Get( NV_EAR_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.ear_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_EAR_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_A2DP_HEADSET, SND_METHOD_VOICE, 
                  nvi.ear_lvl_shadow, NULL, NULL );

  // message volume
  result = OEMNV_Get( NV_ALERTS_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.alerts_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_ALERTS_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_A2DP_HEADSET, SND_METHOD_MESSAGE, 
                  nvi.alerts_lvl_shadow, NULL, NULL );

  // ring volume
  result = OEMNV_Get( NV_RINGER_LVL_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.ringer_lvl_shadow = UISND_2ND_VOL;
    (void) OEMNV_Put (NV_RINGER_LVL_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_A2DP_HEADSET, SND_METHOD_RING, 
                  nvi.ringer_lvl_shadow, NULL, NULL);

  // midi volume
#ifdef FEATURE_MULTIMEDIA
  result = OEMNV_Get( NV_MM_LVL_SHADOW_I, &nvi );
  if ( result == NV_NOTACTIVE_S )
  {
    nvi.mm_lvl_shadow = UISND_4TH_VOL;
    (void) OEMNV_Put (NV_MM_LVL_SHADOW_I, &nvi);
  }
  snd_set_volume( SND_DEVICE_BT_A2DP_HEADSET, SND_METHOD_MIDI, 
                  nvi.mm_lvl_shadow, NULL, NULL );
  
#endif
  pMe->mA2DP.bVolumesInit = TRUE;
}
#endif //FEATURE_BT_EXTPF_AV

#endif /* defined (FEATURE_UI_CORE) && defined (FEATURE_BT) */

/* ==========================================================================
FUNCTION BTApp_Init
DESCRIPTION
============================================================================= */

static boolean BTApp_Init( CBTApp* pMe )
{
	static boolean bInitDone = FALSE;

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif // FEATURE_BT_TEST_API 
#endif // FEATURE_BT_2_1

	MSG_FATAL("***zzg BTApp_Init bInitDone=%d***", bInitDone, 0, 0);

	if (bInitDone == TRUE)
	{
		return bInitDone;
	}	

	/*
		   (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_MENUCTL,
	                           (void**) &pMe->m_pIMenu ) == SUCCESS) &&	  
	   (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR,
	 						   (void **) &pMe->m_pIAnn) == SUCCESS) &&
	   (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
	 						   (void **) &pMe->m_pConfig) == SUCCESS) &&						   
	   (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_SOFTKEYCTL,
	                           (void**) &pMe->m_pISoftMenu ) == SUCCESS) &&
	   (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_STATIC, 
	                           (void **)&pMe->m_pStatic ) == SUCCESS) &&
	   (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_TEXTCTL, 
	                           (void **)&pMe->m_pText ) == SUCCESS) &&
	*/	
		
	if ((ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_BLUETOOTH_SD, (void**)&pMe->mSD.po ) == SUCCESS) 
		   && (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_BLUETOOTH_RM, (void**)&pMe->mRM.po ) == SUCCESS) 
		   && (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_BLUETOOTH_SPP, (void**)&pMe->mSPP.po ) == SUCCESS) 
		   && (ISHELL_RegisterNotify(pMe->m_pShell,  AEECLSID_BLUETOOTH_APP, AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask ) == SUCCESS))
	{
	//Del By zzg 2010_10_29
	//SetDefaultMenuLook( pMe->m_pShell, pMe->m_pIMenu );
	//SetDefaultSoftkeyLook( pMe->m_pShell, pMe->m_pISoftMenu );
	//IMENUCTL_SetProperties( pMe->m_pISoftMenu, MP_MAXSOFTKEYITEMS );
	//Del End
	
    BTApp_ReadConfigFile( pMe );
	
    pMe->mAG.bInbandRing         = FALSE;
    pMe->mAG.bInitialized        = FALSE;
    pMe->mNA.bInitialized        = FALSE;
    pMe->mA2DP.bInitialized      = FALSE; 
    pMe->mAG.bForceUnmute        = TRUE;
    pMe->mRM.bBondable           = FALSE;
    pMe->mSD.bDiscoverable       = FALSE;
    pMe->mRM.bRadioOn            = TRUE;
    pMe->mRM.bVisibilityOn       = TRUE;
    pMe->mRM.bRoleSwitchAllowed  = FALSE;
    pMe->mRM.uGetNameDevIdx      = MAX_DEVICES;
    pMe->bStartBTaplication      = FALSE;
#ifdef FEATURE_BT_2_1
    pMe->mRM.bMITMEnabled        = FALSE;
    pMe->mRM.bRebondOptMITMEnabled = FALSE;
    pMe->mRM.bRebondOptBONDEnabled = FALSE;
    pMe->mRM.bDBGEnabled         = FALSE;
    pMe->mRM.bUserCfm            = FALSE;
    pMe->mRM.ioCaptype           = AEEBT_RM_IOC_DISPLAY_YES_NO;

#ifdef FEATURE_BT_TEST_API 
#error code not present
#endif // FEATURE_BT_TEST_API 
#endif // FEATURE_BT_2_1 

    // allocate memory for SD and RM records 
    pMe->mSD.devRec = (AEEBTDeviceRecord*)MALLOC(sizeof(AEEBTDeviceRecord)*MAX_DEVICES);
    pMe->mSD.browseRec = (AEEBTBrowseRecord*)MALLOC(sizeof(AEEBTBrowseRecord)*AEEBT_MAX_NUM_OF_SRV_REC);
    pMe->mSD.svcRec = (AEEBTServiceRecord*)MALLOC(sizeof(AEEBTServiceRecord)*MAX_DEVICES);
    pMe->mRM.device = (AEEBTDeviceInfo*)MALLOC(sizeof(AEEBTDeviceInfo)*MAX_DEVICES);
    pMe->mRM.devToSwitchRole = MALLOCREC( AEEBTDeviceInfo );
    pMe->mRM.linkStatus = MALLOCREC( AEEBTLinkStatus );
                        
    if ((pMe->mSD.devRec == NULL) || (pMe->mSD.browseRec == NULL) || 
        (pMe->mSD.svcRec == NULL) || (pMe->mRM.device == NULL) ||
        (pMe->mRM.devToSwitchRole == NULL) || (pMe->mRM.linkStatus == NULL))
    {
		if (pMe->mSD.devRec != NULL)
		{
			FREE((void*)pMe->mSD.devRec);
			pMe->mSD.devRec = NULL;
		}
		
		if (pMe->mSD.browseRec != NULL)
		{
			FREE((void*)pMe->mSD.browseRec);
			pMe->mSD.browseRec = NULL;
		}
		
		if (pMe->mSD.svcRec != NULL)
		{
			FREE((void*)pMe->mSD.svcRec);
			pMe->mSD.svcRec = NULL;
		}
		
		if (pMe->mRM.device != NULL)
		{
			FREE((void*)pMe->mRM.device);
			pMe->mRM.device = NULL;
		}
		
		if (pMe->mRM.devToSwitchRole != NULL)
		{
			FREE((void*)pMe->mRM.devToSwitchRole);
			pMe->mRM.devToSwitchRole = NULL;
		}
		
		if (pMe->mRM.linkStatus != NULL)
		{
			FREE((void*)pMe->mRM.linkStatus);
			pMe->mRM.linkStatus = NULL;
		}
		
		MSG_FATAL("BTApp_Init - failed allocating Memory", 0, 0, 0 );   
		
      	return FALSE;
    }
    
    if ((IBTEXTRM_AutoServiceSearchDisable(pMe->mRM.po)) != SUCCESS)
    {
		MSG_FATAL("BTApp_Init - failed disabling auto service search", 0, 0, 0);		
    }

    if (IBTEXTRM_RegisterConnStatus(pMe->mRM.po) != SUCCESS)
    {
		MSG_FATAL("BTApp_Init - failed registering for connection status", 0, 0, 0);		
    }

    CALLBACK_Init(&pMe->mRM.radioActivityCb, BTApp_RadioActivityChanged, pMe);
	
    if (IBTEXTRM_RegisterRadioActivity(pMe->mRM.po, &pMe->mRM.radioActivity, &pMe->mRM.radioActivityCb) != SUCCESS)
    {
		MSG_FATAL("BTApp_Init - failed registering for radio activity change", 0, 0, 0);		
    }

    if (IBTEXTRM_ConfigApplication(pMe->mRM.po, FALSE, pMe->mRM.pageScanParam, pMe->mRM.inquiryScanParam, FALSE) != SUCCESS)
    {
		MSG_FATAL("BTApp_Init - Failed to configure application", 0, 0, 0);		
    }
	
    bInitDone = TRUE;
  }
  else
  {
	MSG_FATAL("BTApp_Init - Failed to create ... some'n", 0, 0, 0);
	
	//BTApp_FreeAppData( (IApplet*) pMe );
	BTApp_FreeAppData(pMe);
  }

  return bInitDone;
}


/* ==========================================================================
FUNCTION BTApp_StartBT
DESCRIPTION
============================================================================= */
static boolean BTApp_StartBT( CBTApp* pMe)
{
	BTApp_CreateOtherApp(pMe);			//Add By zzg 2011_01_05
	
	if (BTApp_Init(pMe))  
	{
		MSG_FATAL("***zzg BTApp_StartBT bEnableAtStart=%d***", pMe->bEnableAtStart, 0, 0);

		if (pMe->bEnableAtStart != FALSE)
		{
            pMe->mEnablingType = BTAPP_ENABLING_AG;
			BTApp_EnableBT(pMe);
		}
		
#ifndef  FEATURE_BT_2_1
		if (pMe->mNA.bEnableSvcSec != FALSE)
		{
			BTApp_EnableServiceSecurity( pMe);
		}
#endif // FEATURE_BT_2_1

		ISHELL_CloseApplet(pMe->m_pShell, FALSE);		
		
		pMe->bFirstLaunch = FALSE;
		
		return TRUE;
		
	}
	else	
	{ 
		pMe->bFirstLaunch = FALSE;
		return FALSE;  
	}
}

/* ==========================================================================
FUNCTION BTApp_BuildMainMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildMainMenu( CBTApp* pMe)
{
  CtlAddItem  ai;
  char        nzStatus[ 20 ] = " -                ";
  uint8       len = 0;
  uint16      itemID = IDS_BT_ON;
  boolean     bScvEnabled = FALSE;
  
  boolean 	  bt_status = FALSE;
  
   MSG_FATAL("BTApp_BuildMainMenu...........................",0,0,0);
  IMENUCTL_Reset( pMe->m_pIMenu );

  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );
   
  BTApp_InitAddItem( &ai );
  

  /*
  if ( pMe->mAG.bEnabled )
  {
    nzStatus[ 3 ] = 'A';
    if ( pMe->mAG.bAudioConnected )
    {
      nzStatus[ 4 ] = 'C';
    }
    else if ( pMe->mAG.bConnected )
    {
      nzStatus[ 4 ] = 'c';
    }
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }

  if ( pMe->mNA.bEnabled )
  {
    nzStatus[ 5 ] = 'N';
    if ( pMe->mNA.bConnected )
    {
      nzStatus[ 6 ] = 'C';
    }
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }

  if ( pMe->mSD.bDiscoverable )
  {
    nzStatus[ 7 ] = 'D';
    bScvEnabled = TRUE;
  }
  // Profiles
#ifdef FEATURE_BT_EXTPF_OPP
  if ( pMe->mOPP.bRegistered )
  {
    nzStatus[ 9 ] = 'O';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  if ( pMe->mFTP.bRegistered )
  {
    nzStatus[ 10 ] = 'F';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
  if ( pMe->mBIP.bRegistered )
  {
    nzStatus[ 11 ] = 'I';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  if ( pMe->mBPP.bRegistered )
  {
    nzStatus[ 12 ] = 'P';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  if ( pMe->mA2DP.bEnabled )
  {
    nzStatus[ 14 ] = '2';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }

  if ( pMe->mAVRCP.bEnabled )
  {
    nzStatus[ 15 ] = 'R';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  if ( pMe->mPBAP.bSrvRegistered )
  {
    nzStatus[ 16 ] = 'B';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
#ifdef FEATURE_IOBEX
  if ( pMe->mOBEX.bRegistered )
  {
    nzStatus[ 17 ] = 'X';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
#endif
  if ( pMe->mL2.bEnabled )
  {
    nzStatus[ 18 ] = 'L';
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  }
  */

  //Add By zzg 2010_11_16  
  ICONFIG_GetItem(pMe->m_pConfig, CFGI_BT_STATUS, &bt_status, sizeof(bt_status));
  if (bt_status == TRUE)  
  {
	nzStatus[ 3 ] = 'O';
    nzStatus[ 4 ] = 'N';    
    itemID = IDS_BT_OFF;
    bScvEnabled = TRUE;
  } 
  //Add End

  

  /*
  if ( bScvEnabled )
  {
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_BT_TITLE, 
                          pMe->pText1, LONG_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText1 );

    STRTOWSTR( nzStatus, &pMe->pText1[ len ], 
               (LONG_TEXT_BUF_LEN-len) * sizeof(AECHAR) );

    // set the title
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText1 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_BT_TITLE, NULL );
  }
  */

  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }

  if ( bScvEnabled )
  {
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_BT_TITLE, 
                          pMe->pText1, LONG_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText1 );

    STRTOWSTR( nzStatus, &pMe->pText1[ len ], 
               (LONG_TEXT_BUF_LEN-len) * sizeof(AECHAR) );

    // set the title
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText1 );

	if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText1);
	}
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_BT_TITLE, NULL );

	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_BT_TITLE,
                         pMe->pText1,
                         LONG_TEXT_BUF_LEN * sizeof(AECHAR));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText1);
	}
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, itemID, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICE_SEARCH, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICES, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_MY_INFO, 0 );

  //Add By zzg 2010_11_17
  //BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BT_OPP_SETTING, 0);
  //Add End

  /*
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_TESTS, 0 );  
  if ( pMe->mAG.bConnected )
  {
    if ( (pMe->mAG.bSLCUp != FALSE) ||
         (BTApp_CallConnected( pMe ) != BT_APP_CALL_NONE) )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUDIO_TRANSFER, 0 );
    }
  }
  */
  
  
  //Add by zzg 2010_10_29
  IMENUCTL_SetProperties(pMe->m_pIMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
  IMENUCTL_SetOemProperties(pMe->m_pIMenu, OEMMP_USE_MENU_STYLE);
  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  
  //Add End
  
  // Activate menu
  pMe->uTopMenu = 0;
  TOP_MENU = BT_APP_MENU_MAIN;
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}
                                     

/* ==========================================================================
FUNCTION BTAPP_BUILDDEVICESEARCHGMENU
DESCRIPTION
============================================================================= */
static void BTApp_BuildDeviceSearchMenu( CBTApp* pMe)
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEVICE_SEARCH, NULL );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_DEVICE_SEARCH,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  //Add End
  
  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICE_ALL, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICE_AUDIO, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICE_NETWORK, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_SERVERS, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEVICE_TELEPHONY, 0 );
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BY_BD_ADDR, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_SEARCH );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTAPP_BUILDDEVICESMENU
DESCRIPTION
============================================================================= */
static void BTApp_BuildDevicesMenu( CBTApp* pMe)
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEVICES, NULL );

  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_DEVICES,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_LIST_BONDED, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_LIST_ALL, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_LIST_CONNECTED, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_UNBOND_ALL, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REMOVE_ALL, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_DEVICE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTApp_BuildMyInfoMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildMyInfoMenu( CBTApp* pMe)
{
  CtlAddItem  ai;
  AEERect     rc;
  uint16      uLen = 0;
  AECHAR      wTitle[ 20 ];
#ifdef FEATURE_BT_2_1
  boolean     bNameSame = FALSE;
  int         i;
  uint8       maxManuDataLen = AEEBT_MAX_EIR_MANUF_DATA_LEN;
  uint8       pManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN ];
  char        szManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
#endif /* FEATURE_BT_2_1 */

  if ( IBTEXTRM_GetLocalInfo( pMe->mRM.po, &pMe->mRM.myInfo ) == SUCCESS )
  {
    // set rect for info display area
    SETAEERECT ( &rc, pMe->m_rect.x, 
                 pMe->m_rect.y, 
                 pMe->m_rect.dx, 
                 pMe->m_rect.dy - BTBAR_HEIGHT); // leave room for SK menu
                 
    ISTATIC_SetRect( pMe->m_pStatic, &rc );

    /*
	// get title
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_MY_INFO, wTitle, sizeof(wTitle) );
      */                    

	//Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
    {
        //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
    }  
    {   
	  	ISHELL_LoadResString(pMe->m_pShell,
	                           AEE_APPSBTAPP_RES_FILE,                                
	                           IDS_MY_INFO,
	                           wTitle,
	                           sizeof(wTitle));
   
	    if(pMe->m_pIAnn != NULL)
	  	{
	  	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTitle);
	  	}
    }
	//Add End

    ISTATIC_SetProperties(pMe->m_pStatic, 
                          ISTATIC_GetProperties( pMe->m_pStatic ) 
                          & ~ST_MIDDLETEXT & ~ST_CENTERTEXT);      

    //Add By zzg 2010_10_29        
	ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
	ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 	
    //Add End

    

    // BT name
    uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
                                LONG_TEXT_BUF_LEN - uLen, 
                                pMe->mRM.myInfo.wName );
#ifdef FEATURE_BT_2_1
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
    // BT ShortName
    if ( WSTRCMP (pMe->mRM.myInfo.wName , pMe->mRM.myInfo.wShortName ) == 0 )
    {
      bNameSame = TRUE;
    } 
    uLen += BTApp_FormatBTShortName( pMe, &pMe->pText1[ uLen], 
                                     LONG_TEXT_BUF_LEN - uLen, 
                                     pMe->mRM.myInfo.wShortName,
                                     bNameSame ); 
#endif    
#endif
#endif /* FEATURE_BT_2_1 */
    // BD address
    uLen += BTApp_FormatBDAddress( pMe, &pMe->pText1[ uLen], 
                                   LONG_TEXT_BUF_LEN - uLen, 
                                   &pMe->mRM.myInfo.bdAddr );
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
    // Security Level
    uLen += BTApp_FormatSecurity( pMe, &pMe->pText1[ uLen], 
                                  LONG_TEXT_BUF_LEN - uLen, 
                                  pMe->mRM.myInfo.security );
    // Bondable status
    uLen += BTApp_FormatBondable( pMe, &pMe->pText1[ uLen], 
                                  LONG_TEXT_BUF_LEN - uLen, 
                                  pMe->mRM.bBondable );
#endif    
#endif
	
    // Discoverable status
    uLen += BTApp_FormatDiscoverable( pMe, &pMe->pText1[ uLen], 
                                      LONG_TEXT_BUF_LEN - uLen, 
                                      pMe->mSD.bDiscoverable );
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
    // Service Class
    uLen += BTApp_FormatSvcCls( pMe, &pMe->pText1[ uLen], 
                                LONG_TEXT_BUF_LEN - uLen, 
                                pMe->mRM.myInfo.svcCls );
    // LMP Version
    uLen += BTApp_FormatLMPVersion( pMe, &pMe->pText1[ uLen], 
                                     LONG_TEXT_BUF_LEN - uLen );

    // LMP Subversion
    uLen += BTApp_FormatLMPSubVersion( pMe, &pMe->pText1[ uLen], 
                                     LONG_TEXT_BUF_LEN - uLen );
    // LMP features
    uLen += BTApp_FormatLMPFeatures( pMe, &pMe->pText1[ uLen], 
                                     LONG_TEXT_BUF_LEN - uLen, 
                                     pMe->mRM.myInfo.uLMPFeatures );
#ifdef FEATURE_BT_2_1
    //ManuData
    if ( IBTEXTRM_GetEIRManufData( pMe->mRM.po, NULL,
                                     &maxManuDataLen, 
                                     pManuData ) != SUCCESS )
    {
      MSG_FATAL( "MY Info - Manufacturer Data failed failed to read from EFS", 
                 0, 0, 0 );
    } 
    else
    {
      for ( i = 0; i < maxManuDataLen; i++ )
      {
        SNPRINTF( (szManuData + STRLEN(szManuData)), 
                  sizeof(szManuData) - STRLEN(szManuData), "%c", pManuData[i] );
      }
      STRTOWSTR( (char*)szManuData, pMe->mRM.wManuData, 
                 sizeof(pMe->mRM.wManuData) );
    }

    uLen += BTApp_FormatManuData( pMe, &pMe->pText1[ uLen], 
                                  LONG_TEXT_BUF_LEN - uLen, 
                                  pMe->mRM.wManuData );
#endif /* FEATURE_BT_2_1 */
#endif
#endif


    // display text
    //ISTATIC_SetText( pMe->m_pStatic, wTitle, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
    ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
    ISTATIC_Redraw( pMe->m_pStatic );

    // add the SK menu to bottom of screen
    IMENUCTL_Reset( pMe->m_pISoftMenu );
	
    // set rect for SK menu
    rc.y  = rc.dy;
    rc.dy = BTBAR_HEIGHT; //pMe->m_rect.dy - rc.y;
    IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

    BTApp_InitAddItem( &ai );

    // Add individual entries to the Menu
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_EDIT_NAME, 0 );
#ifdef FEATURE_BT_2_1
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_EDIT_SHORT_NAME, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_EDIT_MANU_DATA, 0 );
#endif    
#endif	
#endif /* FEATURE_BT_2_1 */
#ifndef FEATURE_BT_2_1
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_SECURITY, 0 );
#endif /* !FEATURE_BT_2_1 */
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_DISCOVERABLE, 0 );
#ifdef FEATURE_BT_2_1
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_IOCAPABILITY, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_DBG_KEY, 0);
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_WRITE_OOB, 0);
#endif /* FEATURE_BT_2_1 */

    PUSH_MENU( BT_APP_MENU_MY_INFO );
    IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
    IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
    IMENUCTL_SetSel( pMe->m_pISoftMenu, MENU_SEL );
  }

#ifdef FEATURE_BT_2_1
  /*
  CALLBACK_Init ( &pMe->mRM.OOBCreateCb, BTApp_LocalOOBCreated, pMe );
  if ( IBTEXTRM_CreateOOBData( pMe->mRM.po, &pMe->mRM.OOBDataSize, 
                               &pMe->mRM.OOBCreateCb ) != SUCCESS )
  {
    MSG_FATAL( "BTApp - Failed to create OOB data", 
               0, 0, 0 );
  }
  */
#endif /* FEATURE_BT_2_1 */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}


/* ==========================================================================
FUNCTION BTApp_BuildSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSettingsMenu( CBTApp* pMe)
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, NULL );

  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SETTINGS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUDIO_GATEWAY, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_NETWORK_ACCESS, 0 );
#ifdef FEATURE_BT_EXTPF_FTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS_FTP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_BIP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS_BIP, 0 );
#endif
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLING, 0 );
  
  if ( pMe->mAG.bVREnabled == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DFLT_DIAL_STR, 0 );
#ifdef FEATURE_BT_VT
    if ( pMe->mAG.bVideoCallEnabled == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_VIDEO_CALL, 0 );
    }
    else
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISABLE_VIDEO_CALL, 0 );
    }
#endif //FEATURE_BT_VT
  }
#ifdef FEATURE_BT_2_1
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SERV_SECURITY, 0 );
#endif

  // Activate menu
  PUSH_MENU( BT_APP_MENU_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTApp_BuildTestsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildTestsMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_TESTS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_TESTS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_MASTER_CONTROL, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_TESTS, 0 );

  if ( (pMe->mAG.bConnected != FALSE) && 
       (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET) &&
       (BTApp_CallPresent( pMe ) == BT_APP_CALL_NONE) )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUDIO_TRANSFER, 0 );
  }
  //BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER_SVC, 0 );
  //BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_UNREGISTER_SVC, 0 );

  // OBEX Menu
#if defined(FEATURE_BT_EXTPF_OPP) || defined(FEATURE_BT_EXTPF_FTP) || \
    defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) 
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_TESTS, 0 );
#endif

  // TCS-BIN Menu
#ifdef FEATURE_BT_EXTPF_CTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_TCS_TESTS, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AVRCP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_L2, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTApp_BuildHCIMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildHCIMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  AEEBTHCIMode HCIMode;

  IBTEXTRM_GetHCIMode( pMe->mRM.po, &HCIMode );
  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_HCI_MODE, NULL );
  
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_HCI_MODE,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OFF, 
                     (HCIMode == AEEBT_HCIM_OFF) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ON, 
                     (HCIMode == AEEBT_HCIM_STANDARD_HCI_VOC_ON) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
#ifdef FEATURE_FACTORY_TESTMODE
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTM, 
                     (HCIMode == AEEBT_HCIM_STANDARD_HCI_FTM) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
#endif

  // Activate menu
  PUSH_MENU( BT_APP_MENU_HCI );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_DoScanParamsMenu
DESCRIPTION
============================================================================= */
static void BTApp_DoScanParamsMenu( CBTApp* pMe, int item )
{
  uint16 selection;
  int32 index;
  int count;
  
  selection = IMENUCTL_GetSel( pMe->m_pISoftMenu );
  IMENUCTL_GetItemData( pMe->m_pISoftMenu, selection, (uint32 *)&index );
  count = IMENUCTL_GetItemCount( pMe->m_pISoftMenu );
  index += item - (AVK_0 + 1);
  if (index < 0)
  {
    index = count - 1;
  }
  else if (index >= count)
  {
    index = 0;
  }
  MSG_FATAL( "DoScanParams - item=%d curSel=%d", item, selection, 0 );

  selection = IMENUCTL_GetItemID( pMe->m_pISoftMenu, index );
  
  if ( item != 0)
  {
    (void)POP_MENU();
  }
  switch ( selection )
  {
    case IDS_PAGE_SCAN:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PAGE_SCAN );
      break;
    case IDS_INQ_SCAN:
      BTApp_BuildMenu( pMe, BT_APP_MENU_INQ_SCAN );
      break;
   }
   
  IMENUCTL_Redraw( pMe->m_pISoftMenu );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildScanParamsSubmenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildScanParamsSubmenu( CBTApp* pMe, BTAppMenuType menu )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;
  uint16      titleID;
  AEEBTActVsPwr* pScanParam;

  MSG_FATAL( "BuildScanParams - menu=%d pS=%d iS=%d", 
           menu, pMe->mRM.pageScanParam, pMe->mRM.inquiryScanParam );

  if ( menu == BT_APP_MENU_PAGE_SCAN )
  {
    pScanParam = &pMe->mRM.pageScanParam;
    titleID    = IDS_PAGE_SCAN;
  }
  else
  {
    pScanParam = &pMe->mRM.inquiryScanParam;
    titleID    = IDS_INQ_SCAN;
  }

  IMENUCTL_Reset( pMe->m_pIMenu );
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, titleID, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         titleID,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SCAN_PARAM_DEFAULT, 
                     (*pScanParam == AEEBT_RM_AVP_AUTOMATIC) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SCAN_PARAM_SMALLEST_CONN_TIME, 
                     (*pScanParam == AEEBT_RM_AVP_SMALLEST_ACT_MOST_POWER) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SCAN_PARAM_LARGER_CONN_TIME, 
                     (*pScanParam == AEEBT_RM_AVP_LARGER_ACT_LESS_POWER) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SCAN_PARAM_LARGEST_CONN_TIME, 
                     (*pScanParam == AEEBT_RM_AVP_LARGEST_ACT_LEAST_POWER) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  
  // select current setting
  switch ( *pScanParam )
  {
    case AEEBT_RM_AVP_AUTOMATIC:
      sel = IDS_SCAN_PARAM_DEFAULT;
      break;
    case AEEBT_RM_AVP_SMALLEST_ACT_MOST_POWER:
      sel = IDS_SCAN_PARAM_SMALLEST_CONN_TIME;
      break;
    case AEEBT_RM_AVP_LARGER_ACT_LESS_POWER:
      sel = IDS_SCAN_PARAM_LARGER_CONN_TIME;
      break;
    case AEEBT_RM_AVP_LARGEST_ACT_LEAST_POWER:
      sel = IDS_SCAN_PARAM_LARGEST_CONN_TIME;
      break;
    default:
      sel = IDS_SCAN_PARAM_DEFAULT;
      break;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( menu );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildScanParamsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildScanParamsMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pISoftMenu );
  
  //SETAEERECT (&rc, 0, 160, pMe->m_rect.dx, pMe->m_rect.dy-160);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.dy-BTBAR_HEIGHT, pMe->m_rect.dx, BTBAR_HEIGHT);
  
  IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_PAGE_SCAN, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_INQ_SCAN, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_SCAN_PARAMS );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
  BTApp_DoScanParamsMenu( pMe, 0 );
}

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_BuildMasterControlMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildMasterControlMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_MASTER_CONTROL, NULL );

  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_MASTER_CONTROL,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  

  BTApp_AddMenuItem( 
    pMe, pMe->m_pIMenu, &ai, 
    pMe->mRM.bRadioOn ? IDS_RADIO_OFF : IDS_RADIO_ON, 0 );
  BTApp_AddMenuItem( 
    pMe, pMe->m_pIMenu, &ai, 
    pMe->mRM.bVisibilityOn ? IDS_VISIBILITY_OFF : IDS_VISIBILITY_ON, 0 );

  if ( IBTEXTRM_GetRoleSwitchPolicy( pMe->mRM.po, NULL, 
                                     &pMe->mRM.bRoleSwitchAllowed ) != SUCCESS )
  {
    MSG_FATAL( "BuildMasterCtl - Get Role Switch Policy failed", 0, 0, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, 
                     pMe->mRM.bRoleSwitchAllowed ? 
                     IDS_DISALLOW_ROLE_SWITCH : IDS_ALLOW_ROLE_SWITCH, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ACCEPT_CONN_ROLE, 0 );                     
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_HCI_MODE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_TEST_MODE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONFIG_SCAN_PARAMS, 0 );
#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, pMe->mRM.bBondable ? 
                     IDS_CLEAR_BONDABLE : IDS_SET_BONDABLE, 0 ); 
  // Activate menu
  PUSH_MENU( BT_APP_MENU_MASTER_CONTROL );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BuildSPPTestMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSPPTestMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SPP_TESTS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SPP_TESTS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_SERVER, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_CLIENT, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_SETTINGS, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_SPP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTApp_BuildSPPSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSPPSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  uint16     sel;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SPP_SETTINGS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SPP_SETTINGS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_THOUSAND, 
                     (pMe->mSPP.uNumDataBufs == 1) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_MILLION, 
                     (pMe->mSPP.uNumDataBufs == 1000) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_BILLION, 
                     (pMe->mSPP.uNumDataBufs == 1000*1000) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  // select current setting
  switch ( pMe->mSPP.uNumDataBufs  )
  {
    case 1:
      sel = IDS_SPP_DATA_SIZE_THOUSAND;
      break;
    case 1000:
      sel = IDS_SPP_DATA_SIZE_MILLION;
      break;
    case 1000*1000:
      sel = IDS_SPP_DATA_SIZE_BILLION;
      break;
    default:
      sel = IDS_SPP_DATA_SIZE_MILLION;
      break;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_SPP_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildOBEXMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildOBEXMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OBEX_TESTS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_OBEX_TESTS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

#ifdef FEATURE_BT_EXTPF_OPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_BIP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BPP, 0 );
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP, 0 );
#endif
#ifdef FEATURE_IOBEX
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX, 0 );
#endif

  // Activate menu
  PUSH_MENU( BT_APP_MENU_OBEX );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildTCSMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildTCSMenu( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_TCS_TESTS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_TCS_TESTS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

#ifdef FEATURE_BT_EXTPF_CTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CTP, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ICP, 0 );
#endif

  // Activate menu
  PUSH_MENU( BT_APP_MENU_TCS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildDevTypeMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildDevTypeMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_FATAL( "BuildDevType - type=%d", pMe->mAG.devType, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_DEVICE_TYPE, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_DEVICE_TYPE,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_HEADSET, 
                     (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_HANDSFREE, 
                     ((pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HANDSFREE) &&
                      (pMe->mAG.bPrivateMode == FALSE)) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_HANDSFREE_PRIVATE, 
                     ((pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HANDSFREE) &&
                      (pMe->mAG.bPrivateMode != FALSE)) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  
  // select current setting
  if ( pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET )
  {
    sel = IDS_HEADSET;
  }
  else
  {
    if ( pMe->mAG.bPrivateMode != FALSE )
    {
      sel = IDS_HANDSFREE_PRIVATE;
    }
    else
    {
      sel = IDS_HANDSFREE;
    }
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_DEV_TYPE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildConnModeMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildConnModeMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;
  boolean     bNoBDAddr = !BDADDR_VALID( &pMe->mAG.bdAddr );

  MSG_FATAL( "BuildConnMode - idleMode=%d", pMe->mAG.idleMode, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_CONN_MODE, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_CONN_MODE,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ACTIVE_ANY_DEVICE, 
                     ((pMe->mAG.idleMode == AEEBT_AG_IDLEMODE_ACTIVE) &&
                      bNoBDAddr) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ACTIVE, 
                     ((pMe->mAG.idleMode == AEEBT_AG_IDLEMODE_ACTIVE) &&
                      (bNoBDAddr == FALSE)) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PASSIVE_ANY_DEVICE, 
                     ((pMe->mAG.idleMode == AEEBT_AG_IDLEMODE_PASSIVE) &&
                      bNoBDAddr) ? IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PASSIVE, 
                     ((pMe->mAG.idleMode == AEEBT_AG_IDLEMODE_PASSIVE) &&
                      (bNoBDAddr == FALSE))?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  // select current setting
  sel = (pMe->mAG.idleMode == AEEBT_AG_IDLEMODE_PASSIVE) ? 
        IDS_PASSIVE : IDS_ACTIVE;
  if ( (sel == IDS_ACTIVE) && (bNoBDAddr != FALSE) )
  {
    sel = IDS_ACTIVE_ANY_DEVICE;
  }
  else if ( (sel == IDS_PASSIVE) && (bNoBDAddr != FALSE) )
  {
    sel = IDS_PASSIVE_ANY_DEVICE;
  }  
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_CONN_MODE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildHSButtonHndlrMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildHSButtonHndlrMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  boolean     bIgnore = 
    BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B );

  MSG_FATAL( "BuildHSButtonHndlr - ignore=%d", bIgnore, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_HS_BUTTON, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_HS_BUTTON,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_IGNORE, 
                     bIgnore ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_HS_BUTTON_HNDLR );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  // Add info
  //SETAEERECT ( &rc, 5, pMe->m_rect.dy/2 - 20, pMe->m_rect.dx - 10, pMe->m_rect.dy/2);
  //Modify by zzg 2010_11_01
  SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx,pMe->m_rect.dy-BTBAR_HEIGHT);
  
  
  ISTATIC_SetRect( pMe->m_pStatic, &rc );
  ISTATIC_Reset( pMe->m_pStatic );
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_INFO_HS_BUTTON,
                        pMe->pText1, LONG_TEXT_BUF_LEN*sizeof(AECHAR) );
  
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
  
  //ISTATIC_SizeToFit( pMe->m_pStatic, &rc ); //Del By zzg 2010_11_01
  
  ISTATIC_SetRect( pMe->m_pStatic, &rc );
  ISTATIC_Redraw( pMe->m_pStatic );
  ISTATIC_SetActive( pMe->m_pStatic, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BuildListMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildListMenu( CBTApp* pMe, BTAppMenuType menu )
{
  AEERect rc;
  IImage *pImage = NULL;
  uint16 uTitleID = 0;
  long val;

  MSG_FATAL( "BuildListMenu - m=%d", menu, 0, 0 );

  switch ( menu )
  {
    case BT_APP_MENU_IDLE_TO:
      val = pMe->mAG.idleTimeout;
      uTitleID = IDS_IDLE_TO;
      break;
    case BT_APP_MENU_SPKR_VOL:
      val = pMe->mAG.spkrGain;
      uTitleID = IDS_SPKR_VOL;
      break;
    case BT_APP_MENU_MIC_VOL:
      val = pMe->mAG.micGain;
      uTitleID = IDS_MIC_VOL;
      break;
    default:
      val = -1;
      break;
  }

  if ( val != -1 )
  {
    CLEAR_SCREEN();
	
    //SETAEERECT ( &rc, 10, 10, pMe->m_rect.dx-50, 30 );
    //Modify by zzg 2010_11_01
    SETAEERECT ( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx-10,pMe->m_rect.dy - BTBAR_HEIGHT);
	
    ITEXTCTL_SetRect( pMe->m_pText, &rc );

    ITEXTCTL_Reset( pMe->m_pText );

    // set the title
    //ITEXTCTL_SetTitle( pMe->m_pText, AEE_APPSBTAPP_RES_FILE, uTitleID, NULL );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	    //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
	}  	
    {
      AECHAR wTitle[20];      
	  ISHELL_LoadResString(pMe->m_pShell,
	                           AEE_APPSBTAPP_RES_FILE,                                
	                           uTitleID,
	                           wTitle,
	                           sizeof(wTitle));

      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTitle);
      }
    }  	
	//Add End
	

    // set the value as text
    WWRITELONG( pMe->pText2, val );
    ITEXTCTL_SetText( pMe->m_pText, pMe->pText2, WSTRLEN( pMe->pText2 ) );
    ITEXTCTL_SetCursorPos( pMe->m_pText, TC_CURSOREND );
    ITEXTCTL_SetProperties( pMe->m_pText, TP_FRAME );

    pImage = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDB_UP_DOWN );
    IIMAGE_Draw( pImage, rc.x + rc.dx + 2, rc.dy - 4 );
    IIMAGE_Release(pImage);

    // Activate menu
    PUSH_MENU( menu );
    ITEXTCTL_SetActive( pMe->m_pText, TRUE );
    IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  }
}

/* ==========================================================================
FUNCTION BTApp_DoAGSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_DoAGSettingsMenu( CBTApp* pMe, int item )
{
  uint16 selection;
  int32 index;
  int count;

  ISTATIC_Reset( pMe->m_pStatic );
  selection = IMENUCTL_GetSel( pMe->m_pISoftMenu );
  IMENUCTL_GetItemData( pMe->m_pISoftMenu, selection, (uint32 *)&index );
  count = IMENUCTL_GetItemCount( pMe->m_pISoftMenu );
  index += item - (AVK_0 + 1);
  if (index < 0)
  {
    index = count - 1;
  }
  else if (index >= count)
  {
    index = 0;
  }
  MSG_FATAL( "DoAGSettings - item=%d curSel=%d", item, selection, 0 );
  
  selection = IMENUCTL_GetItemID( pMe->m_pISoftMenu, index );
  
  if ( item != 0)
  {
    (void)POP_MENU();
  }
  switch (selection)
  {
    case IDS_TYPE:
      BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_TYPE );
      break;
    case IDS_MODE:
      BTApp_BuildMenu( pMe, BT_APP_MENU_CONN_MODE );
      break;
    case IDS_TIME:
      BTApp_BuildMenu( pMe, BT_APP_MENU_IDLE_TO );
      break;
    case IDS_SPKR:
      BTApp_BuildMenu( pMe, BT_APP_MENU_SPKR_VOL );
      break;
    case IDS_MIC:
      BTApp_BuildMenu( pMe, BT_APP_MENU_MIC_VOL );
      break;
    case IDS_HS_BUTTON:
      BTApp_BuildMenu( pMe, BT_APP_MENU_HS_BUTTON_HNDLR );
      break;
#ifdef FEATURE_PHONE_VR
    case IDS_VR:
      BTApp_BuildMenu( pMe, BT_APP_MENU_VR_CAPABLE );
      break;
#endif
   }
   
  IMENUCTL_Redraw( pMe->m_pISoftMenu );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}


/* ==========================================================================
FUNCTION BTApp_BuildAGSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildAGSettingsMenu( CBTApp* pMe)
{
  AEERect     rc;
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pISoftMenu );
  
  //SETAEERECT (&rc, 0, 160, pMe->m_rect.dx, pMe->m_rect.dy-160);
  //Modify By zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.dy-BTBAR_HEIGHT, pMe->m_rect.dx, BTBAR_HEIGHT); 
  
  IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_TYPE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_MODE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_TIME, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_SPKR, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_MIC, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_HS_BUTTON, 0 );
#ifdef FEATURE_PHONE_VR
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_VR, 0 );
#endif
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_AG_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
  BTApp_DoAGSettingsMenu( pMe, 0 );
}


/* ==========================================================================
FUNCTION BTApp_BuildNASettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildNASettingsMenu( CBTApp* pMe)
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_NETWORK_ACCESS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_NETWORK_ACCESS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_SERVICE_SECURITY, 
                     pMe->mNA.bEnableSvcSec ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_NA_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}


/* ==========================================================================
FUNCTION BTApp_BuildEnablingMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildEnablingMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_ENABLING, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_ENABLING,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUDIO_GATEWAY, 
                     pMe->mAG.bSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_NETWORK_ACCESS,
                     pMe->mNA.bSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#ifdef FEATURE_BT_EXTPF_OPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_OPP, 
                     pMe->mOPP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_FTP, 
                     pMe->mFTP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
                     
#ifdef FEATURE_BT_EXTPF_BIP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_BIP, 
                     pMe->mBIP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_BPP, 
                     pMe->mBPP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_PBAP, 
                     pMe->mPBAP.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
#ifdef FEATURE_IOBEX
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_OBEX, 
                     pMe->mOBEX.bServerEnable ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_A2DP, 
                     pMe->mA2DP.bEnableA2DP ? 
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_AVRCP, 
                     pMe->mAVRCP.bEnableAVRCP ? 
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#endif  //FEATURE_BT_EXTPF_AV
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_L2,
                     pMe->mL2.bEnableL2Srv ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif // FEATURE_BT_EXTPF_HID_HOST
  // Activate menu
  PUSH_MENU( BT_APP_MENU_ENABLING );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_BuildDiscoverableMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildDiscoverableMenu( CBTApp* pMe )
{
	CtlAddItem ai;
	uint16     sel = 0;

	AECHAR WTitle[20] = {0};

	IMENUCTL_Reset(pMe->m_pIMenu);

	BTApp_InitAddItem(&ai);

	// set the title    
	ISHELL_LoadResString(pMe->m_pShell,
   		                 AEE_APPSBTAPP_RES_FILE,                                
   		                 IDS_DISCOVERABLE,
   		                 WTitle,
   		                 sizeof(WTitle));

	if (pMe->m_pIAnn != NULL)
	{
		IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}


	// Add individual entries to the Menu
	BTApp_AddMenuItem(pMe, pMe->m_pIMenu, &ai, 
					  IDS_ON_TIMED, 
	                  pMe->mSD.bDiscoverable ? 
	                  IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED);
	BTApp_AddMenuItem(pMe, pMe->m_pIMenu, &ai, 
					  IDS_OFF,
	                  pMe->mSD.bDiscoverable ? 
	                  IDB_BT_RADIO_UNFILLED : IDB_BT_RADIO_FILLED);

	// select current setting
	if (pMe->mSD.bDiscoverable != FALSE)
	{
		sel = IDS_ON_TIMED;
	}
	else
	{
		sel = IDS_OFF;
	}
	
	IMENUCTL_SetSel(pMe->m_pIMenu, sel);

	// Activate menu
	PUSH_MENU(BT_APP_MENU_DISCOVERABLE);
	IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
	IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE);
}

#ifdef FEATURE_PHONE_VR
/* ==========================================================================
FUNCTION BTApp_BuildVRCapableMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildVRCapableMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  uint16     sel = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_VR_CAPABLE, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_VR_CAPABLE,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BT_YES, 
                     pMe->mAG.bVREnabled ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BT_NO,
                     pMe->mAG.bVREnabled ? 
                     IDB_BT_RADIO_UNFILLED : IDB_BT_RADIO_FILLED );
  
  // select current setting
  if ( pMe->mAG.bVREnabled != FALSE )
  {
    sel = IDS_BT_YES;
  }
  else
  {
    sel = IDS_BT_NO;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_VR_CAPABLE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}
#endif /* FEATURE_PHONE_VR */

/* ==========================================================================
FUNCTION BTApp_BuildPrompt
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_BuildPrompt( CBTApp* pMe, BTAppMenuType menu )
{	
	uint16		msgID = 0;	
	AECHAR* 	pText = pMe->pText2;
	boolean 	b_needstr = FALSE;	//Add By zzg 2011_01_13 for Prompt Dialog

#ifdef FEATURE_BT_2_1
	AECHAR		wBuf[ 100 ];
	AECHAR		wBuf2[ 100 ];
	AEEBTDeviceInfo* dev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
#else
	AECHAR		wBuf[ 32 ];
#endif 
	
	IDISPLAY_Backlight( pMe->m_pIDisplay, TRUE );

#ifdef FEATURE_BT_2_1
	if ( (menu == BT_APP_MENU_USER_CFM_RQST) || (menu == BT_APP_MENU_REBOND) )
	{
		if ( TOP_MENU == BT_APP_MENU_SPP_RESULTS )
		{
			ISHELL_CancelTimer(pMe->m_pShell,
			                   (PFNNOTIFY) BTApp_BuildSppTestResults,
			                   pMe);
		}
		
		dev->bdAddr = pMe->mRM.BondBDAddr;
	}
	
	IBTEXTRM_DeviceRead( pMe->mRM.po, dev );
	
	if ( WSTRLEN( dev->wName ) == 0 )
	{
		BTApp_BDAddr2Wstr( wBuf, &dev->bdAddr ); 
	}
	else
	{
		WSTRLCPY( wBuf, dev->wName, ARR_SIZE( wBuf ) );
	}
#else
	IBTEXTRM_DeviceRead(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx]);

	if (WSTRLEN(pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName) == 0)
	{
		BTApp_BDAddr2Wstr(wBuf, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr);
	}
	else
	{
		WSTRLCPY(wBuf, pMe->mRM.device[ pMe->mRM.uCurDevIdx ].wName, ARR_SIZE(wBuf));
	}
#endif 

	switch (menu)
	{
		case BT_APP_MENU_UNBOND_ALL:
		{
			msgID = IDS_PROMPT_UNBOND_ALL;
		  	break;
		}	
#ifdef FEATURE_BT_2_1		
		case BT_APP_MENU_OPEN:	
		{
			msgID = IDS_OPEN_BT; 			
			break;
		}		
		case BT_APP_MENU_REBOND:
		{
			msgID = IDS_PROMPT_REBOND;			
			b_needstr = TRUE; 
			pText = pMe->pText1;
			break;
		}
#endif 
		//Add By zzg 2011_05_25
		case BT_APP_MENU_ON:
		{
			msgID = IDS_MSG_BT_ENABLED; 		
			break;
		}

		case BT_APP_MENU_OFF:
		{
			msgID = IDS_MSG_BT_DISABLED; 		
			break;
		}		
		//Add End

		//Add By zzg 2012_11_28
		case BT_APP_WAITING:
		{
			msgID = IDS_WAITING; 		
			break;
		}
		//Add End

		case BT_APP_MENU_REMOVE_ALL:
		{
			msgID = IDS_PROMPT_REMOVE_ALL;
			break;
		}
		case BT_APP_MENU_UNBOND_ONE:
		{
			msgID = IDS_PROMPT_UNBOND_ONE;
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_REMOVE_ONE:
		{
			msgID = IDS_PROMPT_REMOVE_ONE;
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_USE_AUDIO_DEV:
		{
			msgID = IDS_PROMPT_USE_AUDIO_DEV;		  	
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_AG_CONNECT:
		{
			msgID = IDS_PROMPT_CONNECT;		  	
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_AG_DISCONNECT:
		{
			msgID = IDS_PROMPT_DISCONNECT;
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_NA_SETTINGS:
		{
			msgID = IDS_PROMPT_USE_SRVC_SECURITY;
		  	break;
		}
		case BT_APP_MENU_AUTHORIZE_CONN:
		{
			msgID = IDS_PROMPT_AUTHORIZE;
			b_needstr = TRUE; 
		  	pText = pMe->pText1;
		  	break;
		}
		case BT_APP_MENU_TEST_MODE:
		{
			msgID = IDS_PROMPT_ENTER_TEST_MODE;
		  	break;
		}
#ifdef FEATURE_BT_2_1
		case BT_APP_MENU_USER_CFM_RQST:
		{
			MSG_FATAL("***zzg BuildPrompt BT_APP_MENU_USER_CFM_RQST***", 0, 0, 0);
		  	msgID = IDS_PROMPT_PROCEED_BONDING;
		  	b_needstr = TRUE; 		
		  	pText = pMe->pText1;
		  	break;
		}
#endif
		default:
		{
			MSG_FATAL( "BTApp_BldPrompt - unexp menu %d", menu, 0, 0 );
		  	return;
		}
	}

	
 	pMe->m_prompt_id = msgID;
	pMe->m_bPromptNeedStr = b_needstr;


	MEMSET(pMe->wPromptBuf, 0, WSTRLEN(pMe->wPromptBuf)*sizeof(AECHAR));
	WSTRCPY(pMe->wPromptBuf, wBuf);	
				

	//MOVE_TO_STATE(BTAPPST_PROMPT)
	//CLOSE_DIALOG(DLGRET_PROMPT)

	pMe->m_eDlgRet = DLGRET_PROMPT; 
    (void) ISHELL_EndDialog(pMe->m_pShell); 
  
}

/* ==========================================================================
FUNCTION BTApp_BuildDeviceList
DESCRIPTION
============================================================================= */
static void BTApp_BuildDeviceList( CBTApp* pMe, BTAppMenuType menu )
{
  AEEBTDeviceEnumerator enumerator;
  AEEBTDeviceInfo*      pDev;
  CtlAddItem            ai;
  AECHAR                wName[32];
  uint16                stringID;
  uint16                msgID;
  uint8                 i;
#ifdef FEATURE_BT_2_1
  AECHAR*               pwName;
  AECHAR                wBuf[ 5 ];
  STRTOWSTR( "...", wBuf, sizeof(wBuf) );
#endif /* FEATURE_BT_2_1 */


  MSG_FATAL( "BuildDeviceList - m=%d", menu, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );  

  switch ( menu )
  {
    case BT_APP_MENU_DEV_RESP:
      break;
    case BT_APP_MENU_LIST_CONNECTED:
    case BT_APP_MENU_LIST_BONDED:
    case BT_APP_MENU_LIST_ALL:
    case BT_APP_MENU_LIST_OBEX_SERVERS:
    case BT_APP_MENU_LIST_TELEPHONY:
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
    {
      if ( menu == BT_APP_MENU_LIST_CONNECTED )
      {
        enumerator.control = AEEBT_RM_EC_ALL;
        stringID           = IDS_CONNECTED_DEVICES;
        msgID              = IDS_MSG_NO_CONNECTED_DEV;
      }
      else if ( menu == BT_APP_MENU_LIST_BONDED )
      {
        enumerator.control = AEEBT_RM_EC_MATCH_BONDED;
        enumerator.bBonded = TRUE;
        stringID           = IDS_BONDED_DEVICES;
        msgID              = IDS_MSG_NO_BONDED_DEV;
      }
      else if ( menu == BT_APP_MENU_LIST_ALL )
      {
        enumerator.control = AEEBT_RM_EC_ALL;
        stringID           = IDS_KNOWN_DEVICES;
        msgID              = IDS_MSG_NO_DEV;
      }
      else if ( menu == BT_APP_MENU_LIST_OBEX_SERVERS ) 
      {
        enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
        enumerator.svcCls  = AEEBT_COD_SC_OBJECT_TRANSFER;
        stringID           = IDS_OBEX_SERVERS;
        msgID              = IDS_MSG_NO_OBEX_SERVERS;
      }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
      else
      {
        // Telephony devices
        enumerator.control = AEEBT_RM_EC_MATCH_SERVICE_CLASS;
        enumerator.svcCls  = AEEBT_COD_SC_TELEPHONY;
        stringID           = IDS_DEVICE_TELEPHONY;
        msgID              = IDS_MSG_NO_TELEPHONY_DEV;
      }

      PUSH_MENU( menu );

      // set the title      	  
	  {
	    AECHAR WTitle[20] = {0};
		ISHELL_LoadResString(pMe->m_pShell,
	                         AEE_APPSBTAPP_RES_FILE,                                
	                         stringID,
	                         WTitle,
	                         sizeof(WTitle));
	 
	    if (pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}		
	  }

      if ( IBTEXTRM_DeviceEnumInit( pMe->mRM.po, &enumerator ) == SUCCESS )
      {
        BTApp_InitAddItem( &ai );
        ai.wFont = AEE_FONT_BOLD;

        i    = 0;
        pDev = &pMe->mRM.device[ i ];

        while ( (IBTEXTRM_DeviceEnumNext( pMe->mRM.po, pDev ) == SUCCESS) &&
                pDev->bValid && (i < MAX_DEVICES) )
        {
          if ( menu == BT_APP_MENU_LIST_CONNECTED ) // special case
          {
            pMe->mRM.linkStatus->bdAddr = pDev->bdAddr;
            if ( IBTEXTRM_GetLinkStatus( pMe->mRM.po, 
                                         pMe->mRM.linkStatus ) != SUCCESS )
            {
              // dev not connected to us; skip it
              continue;
            }
          }
          if ( WSTRLEN( pDev->wName ) == 0 )
          {
            BTApp_BDAddr2Wstr( wName, &pDev->bdAddr );
          }
          else
          {
            WSTRLCPY( wName, pDev->wName, ARR_SIZE( wName ) );
          }
          if ( menu == BT_APP_MENU_LIST_CONNECTED )
          {
            AECHAR wM[] = { ' ', '-', ' ', 'M', 0 };
            AECHAR wS[] = { ' ', '-', ' ', 'S', 0 };

            if ( WSTRLEN( wName ) > (ARR_SIZE( wName ) - 5) )
            {
              wName[ ARR_SIZE( wName ) - 5 ] = NULL;
            }
            WSTRLCAT( wName, pMe->mRM.linkStatus->bMaster ? wM : wS, sizeof(wName) );
          }
#ifdef FEATURE_BT_2_1
            pwName = pDev->wName;
            if ( (pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) && 
               (!(pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)) )
            {
              WSTRLCAT( pwName, wBuf, AEEBT_MAX_DEVICENAME_LEN );
            }
            IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, i, pwName, 0 );
#else
          IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, i, wName, 0 );
#endif /* FEATURE_BT_2_1 */

          if ( pDev->bBonded )
          {
            IMENUCTL_SetItem( pMe->m_pIMenu, i, MSIF_FONT, &ai );
          }

          ai.wImage = BTApp_GetDevTypeImageID( pMe, pDev->serviceClass,
                                               &pDev->bdAddr );
          IMENUCTL_SetItem( pMe->m_pIMenu, i, MSIF_IMAGE, &ai );

          pDev = &pMe->mRM.device[ ++i ];
        }

        if ( i > 0 )
        {
          IMENUCTL_Redraw( pMe->m_pIMenu );
          IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );

          // Activate menu
          IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
          IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
        }
        else
        {
          BTApp_ShowMessage( pMe, msgID, NULL, 0 );
        }
      }
      break;
    }
    default:
      MSG_FATAL( "BuildDeviceList - unexpected m=%d", menu, 0, 0 );
      break;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

//Add By zzg 2010_11_10
static void BTApp_SaveSendFilePath(CBTApp *pMe, const char* filepath)
{	
	DBGPRINTF("***zzg BTApp_SaveSendFilePath filepath=%s***", filepath);
		
	MEMSET(pMe->m_pfilepath, 0, 256*sizeof(char) );		//AEEBT_MAX_FILE_NAME
	MEMCPY(pMe->m_pfilepath, filepath, STRLEN(filepath));

	DBGPRINTF("***zzg BTApp_SaveSendFilePath m_pfilepath=%s***", pMe->m_pfilepath);
}
//Add End

/* ==========================================================================
FUNCTION BTApp_BuildDevRespMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildDevRespMenu( CBTApp* pMe )
{  
  CtlAddItem ai;
  int        result = EFAILED;
  uint8      i, numItems, len = 0;
  uint16     stringID = pMe->mSD.bDiscovering ? IDS_SEARCHING : IDS_DISCOVERED_DEVICES;
  AEEBTDeviceInfo* pDev;
  AECHAR* pwName;
#ifdef FEATURE_BT_2_1
  int MapIndex;
  AEEBTDeviceInfo* pMapDev = NULL;
  AECHAR           wBuf[ 5 ];
  boolean          bEIRReqName = TRUE;
  boolean          nameReqIssued = FALSE;
  uint16           tempuValue1 = 0;

  STRTOWSTR( "...", wBuf, sizeof(wBuf) );
#else

#endif /* FEATURE_BT_2_1 */


  pMe->uCurrMsgId = 0;
  if ( TOP_MENU != BT_APP_MENU_DEV_RESP )
  {
    PUSH_MENU( BT_APP_MENU_DEV_RESP );
    IMENUCTL_Reset( pMe->m_pIMenu );
    IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );  
    MEMSET( pMe->mRM.device, 0, (sizeof(AEEBTDeviceInfo)*MAX_DEVICES) );
  }

  BTApp_InitAddItem( &ai );
  ai.wFont = AEE_FONT_BOLD;

  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         stringID,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  if ( pMe->mSD.bDiscovering == FALSE )
  {
    numItems = 0; // force refreshing
    IMENUCTL_DeleteAll( pMe->m_pIMenu );
  }
  else
  {
    MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pIMenu ) );
    numItems = IMENUCTL_GetItemCount( pMe->m_pIMenu );
  }
  MSG_FATAL( "BldDevRespMenu - curSel=%d items=%d tm=%d", 
           MENU_SEL, numItems, TOP_MENU );

  MSG_FATAL( "BldDevRespMenu - numItems = %d uGetNameDevIdx = %d", numItems, pMe->mRM.uGetNameDevIdx, 0);

#ifdef FEATURE_BT_2_1    
  for ( i=numItems; i<pMe->mSD.uNumRecs; i++ )
  {    
    pDev = &pMe->mRM.device[ i ];
    pDev->bdAddr = pMe->mSD.devRec[ i ].bdAddr;
    pwName = pDev->wName;
    result = EFAILED;
    len    = 0;

    MSG_FATAL( "BldDevRespMenu - pDev->uValue1 = %d", pDev->uValue1, 0, 0);
    tempuValue1 = 0;
    if( (pMe->mSD.bDiscovering == FALSE) && (pDev->uValue1 != 0) )
    {
      // save the value of get name status
      tempuValue1 = pDev->uValue1;
      MSG_FATAL( "BldDevRespMenu - tempuValue1 = %d", tempuValue1, 0, 0);
    }

    result = IBTEXTRM_DeviceRead( pMe->mRM.po, pDev );
    if ( result != SUCCESS )
    {
      MSG_FATAL( "BldDevRespMenu - DeviceRead() failed for index=%d", 
                 i, 0, 0 );
    }

    if ( (result == SUCCESS) && (pMe->mSD.bDiscovering == FALSE) && 
         (tempuValue1 != 0) )
    {
      // restore the value of get name status      
      pDev->uValue1 = tempuValue1;
      MSG_FATAL( "BldDevRespMenu - restored pDev->uValue1 = %d", pDev->uValue1, 0, 0);
    }

    len = WSTRLEN( pDev->wName );
    bEIRReqName = TRUE;
    if ( (pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) &&
         (pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B) )
    {
      // check if short name is already available from EIR data
      bEIRReqName = FALSE;
    }

    MSG_FATAL( "BldDevRespMenu - uFlags = 0x%x", pDev->EIRData.uFlags, 0, 0);
    MSG_FATAL( "BldDevRespMenu - nameLen=%d, uValue1=%d, bEIRReqName=%d", len, pDev->uValue1, bEIRReqName );
    if ( (pDev->uValue1 == 0) && // GetName not done?
         (bEIRReqName) )
    {
      if ( (pMe->mSD.bDiscovering == FALSE) &&
           (pMe->mRM.uGetNameDevIdx == MAX_DEVICES) )
      {
        nameReqIssued = TRUE;
        MSG_FATAL( "BldDevRespMenu - requesting name for index=%d", i, 0, 0 );
        if ( IBTEXTSD_GetDeviceName( pMe->mSD.po, &pDev->bdAddr,
                                     pDev->wName,
                                     ARR_SIZE( pDev->wName ) ) == SUCCESS )
        {
          pMe->mRM.uGetNameDevIdx = i;
        }
      }
    }
    else if ( (result == SUCCESS) && (len > 0) )
    {
      pDev->uValue1 = UD1_GET_NAME_SUCCESS;
    }

    for ( MapIndex = i; MapIndex >= 0; MapIndex-- )
    {
      if ( MapIndex == 0 )
      {
        pMe->mRM.uDevToDspIdxMap[MapIndex] = i;
        break;
      }
      pMapDev = &pMe->mRM.device[(pMe->mRM.uDevToDspIdxMap[MapIndex - 1])];

#ifndef FEATURE_APP_TEST_AUTOMATION
/* 
 * The RM database store the device in the order it comes and so should it be
 *  stored at the app for the PTS automation to work 
 */
      if ( pMapDev->EIRData.deviceRanking > pDev->EIRData.deviceRanking )
      {
        /* moving the element down so that the new index can be inserted */
        pMe->mRM.uDevToDspIdxMap[MapIndex] = pMe->mRM.uDevToDspIdxMap[MapIndex - 1];
      }
      else
#endif //FEATURE_APP_TEST_AUTOMATION
      {
        /* place identified - add the new index below the current index */
        pMe->mRM.uDevToDspIdxMap[MapIndex] = i;
        break;
      }
    }
    MSG_FATAL( "BldDevRespMenu - mapIndex=%d devRank=%d DevIndex=%d", 
             MapIndex, pDev->EIRData.deviceRanking, 
             pMe->mRM.uDevToDspIdxMap[MapIndex] );
  }

  if ( ( nameReqIssued == FALSE ) &&
       ( pMe->mSD.bDiscovering == FALSE ) )
  {
      /* Name request for all devices have completed so init uValue1 */
      for ( i = 0; i < MAX_DEVICES; i++ )
      {
        pDev = &pMe->mRM.device[i];
        pDev->uValue1 = 0;
      }
  }

  /* delete menu since we are going to redraw all menu elements*/
  IMENUCTL_DeleteAll( pMe->m_pIMenu );

  //display the devices
  for ( MapIndex = 0; MapIndex < pMe->mSD.uNumRecs; MapIndex++ )
  {
    pMapDev = &pMe->mRM.device[(pMe->mRM.uDevToDspIdxMap[MapIndex])];

    pwName = pMe->pText2;
    if ( (len = WSTRLEN(pMapDev->wName)) == 0 )
    {
      BTApp_BDAddr2Wstr( 
        pwName, 
        &pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].bdAddr );
    }
    else
    {
      WSTRLCPY( pwName, pMapDev->wName, AEEBT_MAX_DEVICENAME_LEN);
      /* check if this is a short name and append "..."*/
      if ( (pMapDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) && 
           (!(pMapDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)) )
      {
        MSG_FATAL( "BldDevRespMenu - short name for DevIndex=%d", 
                 pMe->mRM.uDevToDspIdxMap[MapIndex], 0, 0 );
        WSTRLCAT( pwName, wBuf, AEEBT_MAX_DEVICENAME_LEN );
      }
    }

    if ( IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, MapIndex, 
                           pwName, 0 ) == FALSE )
    {
      MSG_FATAL( "BldDevRespMenu - failed adding item %d to menu", MapIndex, 0, 0 );
    }
    else
    {
      ai.wImage = BTApp_GetDevTypeImageID( 
                    pMe, 
                    pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].serviceClass,
                    &pMe->mSD.devRec[(pMe->mRM.uDevToDspIdxMap[MapIndex])].bdAddr );
      IMENUCTL_SetItem( pMe->m_pIMenu, MapIndex, MSIF_IMAGE, &ai );
      MSG_FATAL( "BldDevRespMenu - addr=%04x %04x %04x", 
               ((uint16)(pMapDev->bdAddr.uAddr[ 5 ] << 8) | pMapDev->bdAddr.uAddr[ 4 ]),
               ((uint16)(pMapDev->bdAddr.uAddr[ 3 ] << 8) | pMapDev->bdAddr.uAddr[ 2 ]),
               ((uint16)(pMapDev->bdAddr.uAddr[ 1 ] << 8) | pMapDev->bdAddr.uAddr[ 0 ]) );
      MSG_FATAL( "BldDevRespMenu - rank=%d", pMapDev->EIRData.deviceRanking, 
               0, 0 ); 
    }
  }
#else
  for (i=numItems; i<pMe->mSD.uNumRecs; i++ )
  {
    pDev = &pMe->mRM.device[ i ];
    pDev->bdAddr = pMe->mSD.devRec[ i ].bdAddr;
    pwName = pDev->wName;
    result = EFAILED;
    len    = 0;
    if ( (pDev->uValue1 == 0) && // GetName not done?
         (((result = IBTEXTRM_DeviceRead( pMe->mRM.po, pDev )) != SUCCESS) ||
          ((len = WSTRLEN( pDev->wName )) == 0)) )
    {
      if ( (pMe->mSD.bDiscovering == FALSE) && 
           (pMe->mRM.uGetNameDevIdx == MAX_DEVICES) )
      {
        if ( IBTEXTSD_GetDeviceName( pMe->mSD.po, &pDev->bdAddr,
                                     pDev->wName, 
                                     ARR_SIZE( pDev->wName ) ) == SUCCESS )
        {
          pMe->mRM.uGetNameDevIdx = i;
        }
      }
      pwName = pMe->pText2;
      BTApp_BDAddr2Wstr( pwName, &pMe->mSD.devRec[ i ].bdAddr );
    }
    else if ( pDev->uValue1 == UD1_GET_NAME_FAILED ) // GetName failed
    {
      pwName = pMe->pText2;
      BTApp_BDAddr2Wstr( pwName, &pMe->mSD.devRec[ i ].bdAddr );
    }
    else if ( (result == SUCCESS) && (len > 0) )
    {
      pDev->uValue1 = UD1_GET_NAME_SUCCESS;
    }

    if ( IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, i, 
                           pwName, 0 ) == FALSE )
    {
      MSG_FATAL( "BldDevRespMenu - failed adding item %d to menu", i, 0, 0 );
    }
    else
    {
      AEEBTBDAddr* pA = &pDev->bdAddr;
      ai.wImage = BTApp_GetDevTypeImageID( pMe, 
                                           pMe->mSD.devRec[ i ].serviceClass,
                                           &pMe->mSD.devRec[ i ].bdAddr );
      IMENUCTL_SetItem( pMe->m_pIMenu, i, MSIF_IMAGE, &ai );
      MSG_FATAL( "BldDevRespMenu - addr=%04x %04x %04x", 
               ((uint16)(pA->uAddr[ 5 ] << 8) | pA->uAddr[ 4 ]),
               ((uint16)(pA->uAddr[ 3 ] << 8) | pA->uAddr[ 2 ]),
               ((uint16)(pA->uAddr[ 1 ] << 8) | pA->uAddr[ 0 ]) );
    }
  }

#endif /* FEATURE_BT_2_1 */
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );  // highlight the selected item
  IMENUCTL_Redraw( pMe->m_pIMenu );

  // Activate menu
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_RefreshDevInfo
DESCRIPTION
============================================================================= */
static void BTApp_RefreshDevInfo( CBTApp* pMe )
{
  if ( TOP_MENU == BT_APP_MENU_DEV_INFO )
  {
    BTApp_BuildTopMenu( pMe );
  }
}

//Add By zzg 2011_03_02
void BTApp_UpdateDeviceInfo(CBTApp* pMe)
{
	(void) ISHELL_PostEvent( pMe->m_pShell,
                             AEECLSID_BLUETOOTH_APP,
                             EVT_USER_REDRAW,
                             0,
 							 0);
}
//Add End

/* ==========================================================================
FUNCTION BTApp_BuildDevInfo
DESCRIPTION
============================================================================= */
static void BTApp_BuildDevInfo( CBTApp* pMe )
{
  uint16            uLen = 0;
  uint16            tempuValue1 = 0;    
  CtlAddItem        ai;
  AEERect           rc;
  AEEBTDeviceInfo*  pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
#ifdef FEATURE_BT_2_1
  uint8 maxManuDataLen = AEEBT_MAX_EIR_MANUF_DATA_LEN;
  uint8             pManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN ];
  char              szManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
  AECHAR            wManuData[ AEEBT_MAX_EIR_MANUF_DATA_LEN + 1 ] = {0};
  int               i;
#endif /* FEATURE_BT_2_1 */

  CLEAR_SCREEN();

  // set rect for info display area
  SETAEERECT ( &rc, pMe->m_rect.x, 
               pMe->m_rect.y, 
               pMe->m_rect.dx, 
               pMe->m_rect.dy - BTBAR_HEIGHT);	//Modify by zzg 2010_11_01
               //pMe->m_rect.dy - 20); // leave room for SK menu
  ISTATIC_SetRect( pMe->m_pStatic, &rc );

  
  ISTATIC_SetProperties(pMe->m_pStatic, 
                        ISTATIC_GetProperties( pMe->m_pStatic ) 
                        & ~ST_MIDDLETEXT & ~ST_CENTERTEXT );    

  //Add By zzg 2010_10_29        
  ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
  ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 
  //Add End

  // save the value of get name status
  tempuValue1 = pDev->uValue1;
  MSG_FATAL( "BTApp_BuildDevInfo - tempuValue1 = %d", tempuValue1, 0, 0);

  if ( TOP_MENU != BT_APP_MENU_DEV_RESP )
  {
    IBTEXTRM_DeviceRead( pMe->mRM.po, pDev );
  }

  // restore the value of get name status      
   pDev->uValue1 = tempuValue1;

  if (pDev->uValue1 == 0 )
  {
      // BT name or Short Name
      if ( (WSTRLEN( pDev->wName ) == 0) &&
           (pMe->mRM.uGetNameDevIdx == MAX_DEVICES) )
      {
        if ( IBTEXTSD_GetDeviceName( pMe->mSD.po, &pDev->bdAddr,
                                     pDev->wName, 
                                     ARR_SIZE( pDev->wName ) ) == SUCCESS )
        {
          pMe->mRM.uGetNameDevIdx = pMe->mRM.uCurDevIdx;
        }
      }
      else if ( WSTRLEN( pDev->wName ) == 0 )
      {
        // schedule to get name when current get name session is done        
		ISHELL_SetTimer( pMe->m_pShell, 500,(PFNNOTIFY) BTApp_RefreshDevInfo, pMe );
    
      }
  }
  else
  {
      pDev->uValue1 = 0;
  }
//#ifndef FEATURE_VERSION_C337	  
#if !defined (FEATURE_VERSION_C337) && !defined (FEATURE_VERSION_IC241A_MMX)
#ifdef FEATURE_BT_2_1
  // manufacturer data
  if (  pDev->EIRData.uManufDataSize > 0  )
  {
    //this means that manufacturer data is present 
    if ( ( pDev->EIRData.uManufDataSize > AEEBT_MAX_EIR_MANUF_DATA_LEN ) || 
         ( IBTEXTRM_GetEIRManufData( pMe->mRM.po, &pDev->bdAddr,
                                     &maxManuDataLen, 
                                     pManuData ) != SUCCESS ) )  
    {
      MSG_FATAL( "DevInfo - Manufacturer Data failed", 0, 0, 0 );
    } 
    else
    {
      for ( i = 0; i < maxManuDataLen; i++ )
      {
        SNPRINTF( (szManuData + STRLEN(szManuData)), 
                  sizeof(szManuData) - STRLEN(szManuData), "%c", pManuData[i] );
      }
      STRTOWSTR( (char*)szManuData, wManuData, sizeof(wManuData) );
    }
  }
  if ( (pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) && 
           (!(pDev->EIRData.uFlags & AEEBT_EIR_NAME_CMPLT_B)) )
  {
    uLen += BTApp_FormatBTShortName( pMe, &pMe->pText1[ uLen], 
                              LONG_TEXT_BUF_LEN - uLen, pDev->wName, FALSE );
  }
  else
  {
    uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
                                     LONG_TEXT_BUF_LEN - uLen, 
                                     pDev->wName ); 
  }
#else
  uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
                              LONG_TEXT_BUF_LEN - uLen, pDev->wName );
#endif /* FEATURE_BT_2_1 */
#else
	uLen += BTApp_FormatBTName( pMe, &pMe->pText1[ uLen], 
                              LONG_TEXT_BUF_LEN - uLen, pDev->wName );
#endif

#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
#ifdef FEATURE_BT_2_1
   uLen += BTApp_FormatSSPCapable( pMe, &pMe->pText1[ uLen], 
                                   LONG_TEXT_BUF_LEN - uLen, 
                                   pDev->bSSPCapable );
   //Use linkKeyStatus which is now avaiable in AEEBTDeviceInfo
   MSG_HIGH( "FormatLinkKey", 0, 0, 0 );
   MSG_HIGH( "FormatLinkKey %d", pDev->linkKeyStatus, 0, 0 );
   uLen += BTApp_FormatLinkKey( pMe, &pMe->pText1[ uLen], 
                                LONG_TEXT_BUF_LEN - uLen, 
                                pDev->linkKeyStatus );
#endif /* FEATURE_BT_2_1 */
#endif
#endif

  // BD address
  uLen += BTApp_FormatBDAddress( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen, &pDev->bdAddr );

#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
  // Security Level
  uLen += BTApp_FormatSecurity( pMe, &pMe->pText1[ uLen], 
                                LONG_TEXT_BUF_LEN - uLen, pDev->security );
  // Service Class
  uLen += BTApp_FormatSvcCls( pMe, &pMe->pText1[ uLen], 
                              LONG_TEXT_BUF_LEN - uLen, pDev->serviceClass );
  // LMP features
  if ( pDev->bValid == FALSE )
  {
    pDev->uLMPFeatures[ 0 ] = 0;
    pDev->uLMPFeatures[ 1 ] = 0;
    pDev->uLMPFeatures[ 2 ] = 0;
  }
  uLen += BTApp_FormatLMPFeatures( pMe, &pMe->pText1[ uLen], 
                                   LONG_TEXT_BUF_LEN - uLen, 
                                   pDev->uLMPFeatures );
#ifdef FEATURE_BT_2_1

  if ( pMe->bRemoteOOBRead == TRUE )
  {
    if ( BTApp_RemoteOOBWrite ( pMe, pDev ) == SUCCESS )
    {
      uLen += BTApp_FormatOOBDataStr ( pMe,&pMe->pText1[ uLen], 
                            LONG_TEXT_BUF_LEN - uLen, TRUE );
    }
    else
    {
      uLen += BTApp_FormatOOBDataStr ( pMe,&pMe->pText1[ uLen], 
                            LONG_TEXT_BUF_LEN - uLen, FALSE );
    }
                            
  }

  //manu data
  if (WSTRLEN (wManuData) > 0 )
  {
    uLen += BTApp_FormatManuData( pMe, &pMe->pText1[ uLen], 
                                  LONG_TEXT_BUF_LEN - uLen,
                                  wManuData );
  }
  // UUIDs
  uLen += BTApp_FormatUUID( pMe, &pMe->pText1[ uLen], 
                            LONG_TEXT_BUF_LEN - uLen,
                            pDev );
#endif /* FEATURE_BT_2_1 */
#endif
#endif

  // display text
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_Redraw( pMe->m_pStatic );

  // add the SK menu to bottom of screen
  IMENUCTL_Reset( pMe->m_pISoftMenu );

  // set rect for SK menu
  rc.y  = rc.dy;
  //rc.dy = pMe->m_rect.dy - rc.y;
  rc.dy = BTBAR_HEIGHT; //Modify by zzg 2010_11_01
  IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( TOP_MENU == BT_APP_MENU_DEV_RESP )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, 
                       pDev->bValid ? IDS_DISCARD : IDS_KEEP, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_BOND, 0 );
#ifdef FEATURE_BT_2_1
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_BOND_OPTIONS, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_READ_OOB, 0 );
#endif /* FEATURE_BT_2_1 */
  }
  else
  {
    uint8 i = 0;
    boolean bIsAudioDev = FALSE;

    // find out if this is an audio device
    if ( (pDev->majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO) ||
         (pDev->serviceClass & AEEBT_COD_SC_AUDIO ) )
    {
      bIsAudioDev = TRUE;
    }
    else
    {
      while ( (i<pDev->uNumSvcs) && (bIsAudioDev == FALSE) )
      {
        if ( (pDev->svcs[ i ].uServiceClass == 
              AEEBT_SD_SERVICE_CLASS_HEADSET) ||
             (pDev->svcs[ i ].uServiceClass == 
              AEEBT_SD_SERVICE_CLASS_HANDSFREE) )
        {
          bIsAudioDev = TRUE;
        }
        i++;
      }
    }

    if ( bIsAudioDev != FALSE )
    {
      // only Use, Connect, and Disconnect audio device
      if ( pMe->mAG.bEnabled == FALSE )
      {
        BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_USE, 0 );
      }
      else if ( pMe->mAG.bConnected == FALSE )
      {
        BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_CONNECT, 0 );
      }
      else if ( AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.connectedBDAddr, 
                                      &pDev->bdAddr ) != FALSE )
      {
        BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_DISCONNECT, 0 );
      }
    }
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, 
                       pDev->bBonded ? IDS_UNBOND : IDS_BOND, 0 );
#ifdef FEATURE_BT_2_1
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_BOND_OPTIONS, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_READ_OOB, 0 );
#endif /* FEATURE_BT_2_1 */
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_DISCARD, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_OPTIONS, 0 );

  PUSH_MENU( BT_APP_MENU_DEV_INFO );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
  IMENUCTL_SetSel( pMe->m_pISoftMenu, MENU_SEL );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BuildDevOptions
DESCRIPTION
============================================================================= */
static void BTApp_BuildDevOptions( CBTApp* pMe )
{
  CtlAddItem  ai;
  AECHAR      wTempBuf[ 64 ];
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );
   
  BTApp_InitAddItem( &ai );

  // form the title
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_OPTIONS, 
                        wTempBuf, sizeof(wTempBuf) );
  if ( WSTRLEN( pDev->wName ) > 0 )
  {
    WSTRLCPY( pMe->pText2, pDev->wName, (SHORT_TEXT_BUF_LEN*sizeof( AECHAR )) );
  }
  else
  {
    BTApp_BDAddr2Wstr( pMe->pText2, &pDev->bdAddr );
  }
  WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN * sizeof(AECHAR), 
            wTempBuf, pMe->pText2, wTempBuf );
  //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText1 );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText1);
	}
	
  }

  // Add individual entries to the Menu
#ifndef FEATURE_BT_2_1
  if ( pDev->bBonded )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY, 0 );
  }
#endif /* !FEATURE_BT_2_1 */
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BROWSE_SVC, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SEARCH_SVC, 0 );

  pMe->mRM.linkStatus->bdAddr = pDev->bdAddr;
  if ( IBTEXTRM_GetLinkStatus( pMe->mRM.po, pMe->mRM.linkStatus ) == SUCCESS )
  {
    boolean bSwitchAllowed = FALSE; // assume role switch not allowed
    uint16 strID;

    if ( IBTEXTRM_GetRoleSwitchPolicy( pMe->mRM.po, &pDev->bdAddr, 
                                       &bSwitchAllowed ) != SUCCESS )
    {
      MSG_FATAL( "BuildDevOptions - Get Role Switch Policy failed; "
                 "assumed not allowed", 0, 0, 0 );
    }
    strID = bSwitchAllowed ? IDS_DISALLOW_ROLE_SWITCH : IDS_ALLOW_ROLE_SWITCH;
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, strID, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SET_CONN_ROLE, 0 );
  }

  // Activate menu
  PUSH_MENU( BT_APP_MENU_DEV_OPTIONS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}

/* ==========================================================================
FUNCTION BTApp_BuildSecurityMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSecurityMenu( 
  CBTApp* pMe, 
  AEEBTSecurity security,
  BTAppMenuType menu )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_FATAL( "BuildSec - sec=0x%x", security, 0, 0 );

  CLEAR_SCREEN();

  IMENUCTL_Reset( pMe->m_pIMenu );
  
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SECURITY, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SECURITY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_NONE, 
                     (security == AEEBT_SEC_NONE) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_AUTH, 
                     (security == AEEBT_SEC_AUTHENTICATE) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_AUTH_ENC, 
                     (security == AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  
  // select current setting
  switch ( security )
  {
    case AEEBT_SEC_NONE:
      sel = IDS_SECURITY_NONE;
      break;
    case AEEBT_SEC_AUTHENTICATE:
      sel = IDS_SECURITY_AUTH;
      break;
    case AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT:
      sel = IDS_SECURITY_AUTH_ENC;
      break;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( menu );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_BuildBondOptionsMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildBondOptionsMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  IMENUCTL_Reset( pMe->m_pIMenu );
  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_BOND_OPTIONS, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_BOND_OPTIONS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_MITM, 
                     pMe->mRM.bMITMEnabled ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REBOND_OPT_MITM, 
                     pMe->mRM.bRebondOptMITMEnabled ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REBOND_OPT_BOND, 
                     pMe->mRM.bRebondOptBONDEnabled ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_DEV_BOND_OPTIONS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_UserConfirm
DESCRIPTION
============================================================================= */
/*static*/ boolean BTApp_UserConfirm( CBTApp* pMe, boolean bConfirmed )
{
	boolean  success = TRUE;

	MSG_FATAL("***zzg BTApp_UserConfirm***", 0, 0, 0);

	if (IBTEXTRM_UserConfirmationReply(pMe->mRM.po, &pMe->mRM.BondBDAddr, bConfirmed ) != SUCCESS )
	{
		MSG_FATAL( "UserConfirm - DeviceEnumInit failed", 0, 0, 0 );
		success = FALSE;
	}
	return success;
}
/* ==========================================================================
FUNCTION BTApp_HandleDbgKey
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleDbgKey( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
 
  MSG_FATAL( "BTApp_HandleDbgKey - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_CLR:
    {
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
    }
	case AVK_INFO:	//Add By zzg 2010_11_03  
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      
      switch ( selection )
      {
        case IDS_DBG_KEY:
        {
          MSG_FATAL( "HndlSlction - IDS_DBG_KEY m=%d", TOP_MENU, 0, 0 );
          pMe->bConfigChanged = TRUE;
          pMe->mRM.bDBGEnabled = pMe->mRM.bDBGEnabled ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, pMe->m_pIMenu,selection,
                                     pMe->mRM.bDBGEnabled ? 
                                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          IBTEXTRM_SetSM4DebugMode( pMe->mRM.po,pMe->mRM.bDBGEnabled );

          break;
        }
        
        default:
          MSG_FATAL( "BTApp_HandleDbgKey - sel=%d", selection, 0, 0 );
          break;
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_BuildBrowseRespScreen
DESCRIPTION
============================================================================= */
static void BTApp_BuildBrowseRespScreen( CBTApp* pMe )
{
  uint16      uLen = 0;
  AECHAR      wTempBuf[ 32 ];
  AEERect     rc;
  uint8       i;
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];
  boolean     bUpdateRec = FALSE;

  STRTOWSTR( "0x%x/%d/%s\n", wTempBuf, sizeof(wTempBuf) );

  // see if we should save it in RM database
  if ( (pDev->bValid != FALSE) && (pDev->uNumSvcs == 0) )
  {
    bUpdateRec = TRUE;
  }

  CLEAR_SCREEN();

  // set rect for info display area
  SETAEERECT ( &rc, pMe->m_rect.x, 
               pMe->m_rect.y, 
               pMe->m_rect.dx, 
               pMe->m_rect.dy); // - BTBAR_HEIGHT); // leave room for SK menu
  ISTATIC_SetRect( pMe->m_pStatic, &rc );
  IMENUCTL_Reset( pMe->m_pIMenu );

  // get the screen title
  //ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_BROWSE_SVC, pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );


  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  {   
    	ISHELL_LoadResString(pMe->m_pShell,
                             AEE_APPSBTAPP_RES_FILE,                                
                             IDS_BROWSE_SVC,
                             pMe->pText2,
                             SHORT_TEXT_BUF_LEN * sizeof( AECHAR ));
  
      if(pMe->m_pIAnn != NULL)
    	{
    	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
    	}
  }
  //Add End


  
  // BD address
  uLen += BTApp_FormatBDAddress( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen, &pDev->bdAddr );
  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\n');

  // label
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_BROWSE_SVC_RESULTS, &pMe->pText1[ uLen], 
                        (LONG_TEXT_BUF_LEN - uLen) * sizeof(AECHAR) );
  uLen = WSTRLEN( pMe->pText1 );

  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\n');

  for ( i=0; i<pMe->mSD.uNumBrowseRecs && i<AEEBT_MAX_NUM_OF_SRV_REC; i++)
  {
    if ( bUpdateRec != FALSE )
    {
      pDev->svcs[ i ].uServiceClass   = pMe->mSD.browseRec[ i ].uSvcClsId;
      pDev->svcs[ i ].uServerChannel  = pMe->mSD.browseRec[ i ].uSrvChanNum;
      pDev->svcs[ i ].uProfileVersion = 0;
      pDev->uNumSvcs++;
    }

    if ( (WSTRLEN( wTempBuf ) + 6 + WSTRLEN( pMe->mSD.browseRec[i].wSvcName )) < 
         (LONG_TEXT_BUF_LEN - uLen))
    {
      WSPRINTF( &pMe->pText1[ uLen ], 
                (LONG_TEXT_BUF_LEN - uLen) * sizeof(AECHAR), 
                wTempBuf, pMe->mSD.browseRec[i].uSvcClsId,
                pMe->mSD.browseRec[i].uSrvChanNum, 
                pMe->mSD.browseRec[i].wSvcName );
      uLen = WSTRLEN( pMe->pText1 );
    }
  }

  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\0');

  if ( bUpdateRec != FALSE )
  {
    IBTEXTRM_DeviceServicesUpdate( pMe->mRM.po, &pDev->bdAddr,
                                   pMe->mSD.uNumBrowseRecs, pDev->svcs );
  }

  // display text
  //ISTATIC_SetText( pMe->m_pStatic, pMe->pText2, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  
  ISTATIC_Redraw( pMe->m_pStatic );
 
  PUSH_MENU( BT_APP_MENU_BROWSE_SVC_RESP );
  ISTATIC_SetActive( pMe->m_pStatic, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BuildSearchRespScreen
DESCRIPTION
============================================================================= */
static void BTApp_BuildSearchRespScreen( CBTApp* pMe )
{
  uint16      uLen = 0;
  AECHAR      wTempBuf[ 32 ];
  AEERect     rc;
  uint8       i, j;
  boolean     bNoMoreRoom = FALSE;
  AEEBTDeviceInfo* pDev = &pMe->mRM.device[ pMe->mRM.uCurDevIdx ];

  STRTOWSTR( "0x%x/0x%x\n", wTempBuf, sizeof(wTempBuf) );

  CLEAR_SCREEN();

  // set rect for info display area
  SETAEERECT ( &rc, pMe->m_rect.x, 
               pMe->m_rect.y, 
               pMe->m_rect.dx, 
               pMe->m_rect.dy); // - BTBAR_HEIGHT); // leave room for SK menu
  ISTATIC_SetRect( pMe->m_pStatic, &rc );
  IMENUCTL_Reset( pMe->m_pIMenu );

  // get the screen title
  //ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_SEARCH_SVC, pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );


  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  {   
    	ISHELL_LoadResString(pMe->m_pShell,
                             AEE_APPSBTAPP_RES_FILE,                                
                             IDS_SEARCH_SVC,
                             pMe->pText2,
                             SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  
      if(pMe->m_pIAnn != NULL)
    	{
    	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
    	}
  }
  //Add End
  
  // BD address
  uLen += BTApp_FormatBDAddress( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen, &pDev->bdAddr );
  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\n');

  // label
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_SRCH_SVC_RESULTS, 
                        &pMe->pText1[ uLen], 
                        (LONG_TEXT_BUF_LEN - uLen) * sizeof(AECHAR) );
  uLen = WSTRLEN( pMe->pText1 );
  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\n');

  for ( i=0; i<pMe->mSD.uNumSvcRecs && i<MAX_DEVICES && !bNoMoreRoom; i++)
  {
    for ( j=0; j<pMe->mSD.svcRec[i].uNumAttr && 
               j<AEEBT_SD_MAX_SVC_ATTR_ID && !bNoMoreRoom; j++ )
    {
      AEEBTServiceAttribute *pSvcAttr = &pMe->mSD.svcRec[i].svcAttr[j];
      AEEBTSDProtoDesc *pProDesc;

      if ( (WSTRLEN(wTempBuf) + 8) >= (LONG_TEXT_BUF_LEN - uLen))
      {
        bNoMoreRoom = TRUE;
        break;
      }
      if ( pSvcAttr->uAttrID != 
           AEEBT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST )
      {
        continue;
      }
      pProDesc = &pSvcAttr->value.protoDescList.aProtoDesc[0];
      WSPRINTF( &pMe->pText1[ uLen ], (LONG_TEXT_BUF_LEN - uLen)*sizeof(AECHAR), 
                wTempBuf, pProDesc->uUUID, pProDesc->aParam[0].uValue );
      uLen = WSTRLEN( pMe->pText1 );
    }
  }
  pMe->pText1[ uLen++ ] = (AECHAR)(unsigned char) ('\0');

  // display text
  //ISTATIC_SetText( pMe->m_pStatic, pMe->pText2, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  
  ISTATIC_Redraw( pMe->m_pStatic );
 
  PUSH_MENU( BT_APP_MENU_SEARCH_SVC_RESP );
  ISTATIC_SetActive( pMe->m_pStatic, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BuildSppTestResults
DESCRIPTION
============================================================================= */
static void BTApp_BuildSppTestResults( CBTApp* pMe )
{
  uint16  uLen = 0;
  uint16  titleID;
  AEERect rc;

  if ( TOP_MENU != BT_APP_MENU_SPP_RESULTS )
  {
    CLEAR_SCREEN();

    // set rect for info display area
    SETAEERECT ( &rc, pMe->m_rect.x, 
                 pMe->m_rect.y, 
                 pMe->m_rect.dx, 
                 pMe->m_rect.dy);
    ISTATIC_SetRect( pMe->m_pStatic, &rc );

    
    ISTATIC_SetProperties(pMe->m_pStatic, 
		                  ISTATIC_GetProperties( pMe->m_pStatic ) 
		                  & ~ST_MIDDLETEXT & ~ST_CENTERTEXT );      

	//Add By zzg 2010_10_29        
	ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
	ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 
    //Add End

    // get the screen title

	/*
    titleID = pMe->mSPP.bClient ? IDS_SPP_CLIENT : IDS_SPP_SERVER;
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, titleID, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
                          */
    //Add By zzg 2010_11_01
    titleID = pMe->mSPP.bClient ? IDS_SPP_CLIENT : IDS_SPP_SERVER;
	if(pMe->m_pIAnn != NULL)
    {
        //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
    }  
    {   
	  	ISHELL_LoadResString(pMe->m_pShell,
	                           AEE_APPSBTAPP_RES_FILE,                                
	                           titleID,
	                           pMe->pText2,
	                           SHORT_TEXT_BUF_LEN * sizeof(AECHAR));
   
	    if(pMe->m_pIAnn != NULL)
	  	{
	  	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
	  	}
    }
	//Add End

	
  }

  uLen += BTApp_FormatPortStatus( pMe, &pMe->pText1[ uLen], 
                                  LONG_TEXT_BUF_LEN - uLen );
  uLen += BTApp_FormatBytesSent( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen );
  uLen += BTApp_FormatBytesRcvd( pMe, &pMe->pText1[ uLen], 
                                 LONG_TEXT_BUF_LEN - uLen );
  // display text
  //ISTATIC_SetText( pMe->m_pStatic, pMe->pText2, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_Redraw( pMe->m_pStatic );

  if ( TOP_MENU != BT_APP_MENU_SPP_RESULTS )
  {
    PUSH_MENU( BT_APP_MENU_SPP_RESULTS );
  }

  ISHELL_SetTimer( pMe->m_pShell, 
                   (pMe->mSPP.status.state == AEEBT_SPP_ST_CONNECTED) ? 
                    ONE_SECOND : (ONE_SECOND * 5), 
                   (PFNNOTIFY) BTApp_BuildSppTestResults, pMe );
}

/* ==========================================================================
FUNCTION BTApp_BuildSetConnRoleMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSetConnRoleMenu( CBTApp* pMe )
{
  uint16            uLen = 0;
  uint16            msgID;
  uint16            titleID;
  CtlAddItem        ai;
  AEERect           rc;
  AECHAR            wTitle[ 20 ];

  CLEAR_SCREEN();
  // set rect for info display area
  SETAEERECT ( &rc, pMe->m_rect.x, 
               pMe->m_rect.y, 
               pMe->m_rect.dx, 
               pMe->m_rect.dy - BTBAR_HEIGHT); //Modify by zzg 2010_11_01
               //pMe->m_rect.dy - 20); // leave room for SK menu
  ISTATIC_SetRect( pMe->m_pStatic, &rc );

  
  ISTATIC_SetProperties(pMe->m_pStatic, 
                        ISTATIC_GetProperties( pMe->m_pStatic ) 
                        | ST_MIDDLETEXT | ST_CENTERTEXT );  

  //Add By zzg 2010_10_29        
  ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
  ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 
  //Add End

  if ( BDADDR_VALID( &pMe->mRM.devToSwitchRole->bdAddr ) == FALSE )
  {
    msgID = IDS_PROMPT_SET_CONN_ROLE_FUTURE;
    titleID = IDS_ACCEPT_CONN_ROLE;
  }
  else
  {
    msgID = IDS_PROMPT_SET_CONN_ROLE_CURRENT;
    titleID = IDS_SET_CONN_ROLE;
  }
  // get title
  //ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, titleID, wTitle, sizeof(wTitle) );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  {   
    	ISHELL_LoadResString(pMe->m_pShell,
                             AEE_APPSBTAPP_RES_FILE,                                
                             titleID,
                             wTitle,
                             sizeof(wTitle));
  
      if(pMe->m_pIAnn != NULL)
    	{
    	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTitle);
    	}
  }
  //Add End

  // get prompt
  uLen = ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID,
                               pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof(AECHAR) );

  msgID = (pMe->mRM.linkStatus->bMaster) ? IDS_SLAVE : IDS_MASTER ;

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID,
                        &pMe->pText2[ uLen + 1], 
                        (SHORT_TEXT_BUF_LEN-uLen)*sizeof(AECHAR) );

  WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof( AECHAR ), pMe->pText2, 
            &pMe->pText2[ uLen + 1 ], pMe->mRM.devToSwitchRole->wName );

  // display text
  //ISTATIC_SetText( pMe->m_pStatic, wTitle, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_SetText( pMe->m_pStatic, NULL, pMe->pText1, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  
  ISTATIC_Redraw( pMe->m_pStatic );

  // add the SK menu to bottom of screen
  IMENUCTL_Reset( pMe->m_pISoftMenu );

  // set rect for SK menu
  rc.y  = rc.dy;
  //rc.dy = pMe->m_rect.dy - rc.y;
  rc.dy = BTBAR_HEIGHT;	//Modify by zzg 2010_11_01
  IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

  BTApp_InitAddItem( &ai );
  
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_BT_YES, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_BT_NO, 0 );

  PUSH_MENU( BT_APP_MENU_SET_CONN_ROLE );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pISoftMenu, MENU_SEL );
  
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_BuildIOCapabilityMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildIOCapabilityMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_FATAL( "BTApp_BuildIOCapabilityMenu - type=%d", pMe->mRM.ioCaptype, 0, 0 ); 

  IMENUCTL_Reset( pMe->m_pIMenu );
  
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_IOCAPABILITY, NULL ); 
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_IOCAPABILITY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  MSG_FATAL("***zzg ioCaptype=%d***", pMe->mRM.ioCaptype, 0, 0);

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_IOC_YESNO, 
                     (pMe->mRM.ioCaptype == AEEBT_RM_IOC_DISPLAY_YES_NO) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_IOC_DISPLAY_ONLY, 
                     (pMe->mRM.ioCaptype == AEEBT_RM_IOC_DISPLAY_ONLY) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_IOC_KEYBOARD_ONLY, 
                     (pMe->mRM.ioCaptype == AEEBT_RM_IOC_KEYBOARD_ONLY) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_IOC_NO, 
                     (pMe->mRM.ioCaptype == AEEBT_RM_IOC_NO_IO) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );


  
  // select current setting
  switch ( pMe->mRM.ioCaptype )
  {
    case AEEBT_RM_IOC_DISPLAY_YES_NO:
      sel = IDS_IOC_YESNO;
      break;
    case AEEBT_RM_IOC_DISPLAY_ONLY:
      sel = IDS_IOC_DISPLAY_ONLY;
      break;
    case AEEBT_RM_IOC_KEYBOARD_ONLY:
      sel = IDS_IOC_KEYBOARD_ONLY;
      break;
    case AEEBT_RM_IOC_NO_IO:
      sel = IDS_IOC_NO;
      break;
  }

  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_IOC_TYPE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_BuildSvcSecMainMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSvcSecMainMenu ( CBTApp* pMe )
{
  CtlAddItem  ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SERV_SECURITY, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SERV_SECURITY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_STRING_SRV_PROFILE_SELECTION, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_STRING_SRV_SEC_LEVEL, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_SVR_SEC_MAIN_TYPE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
}
/* ==========================================================================
FUNCTION BTApp_BuildSvcSecMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildSvcSecMenu( CBTApp* pMe )
{

  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SERV_SECURITY, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SERV_SECURITY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_ALL, 
                     pMe->mAG.bSecSelectedAll ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  //Display the menus based on the current security level
  BTApp_AddMenuItem(pMe, pMe->m_pIMenu, &ai, IDS_AUDIO_GATEWAY, 
                    pMe->mAG.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch( pMe->mAG.srvSecType )
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_AUDIO_GATEWAY, AEE_APPSBTAPP_RES_FILE,
                          IDS_AUDIO_GATEWAY_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_AUDIO_GATEWAY, AEE_APPSBTAPP_RES_FILE,
                          IDS_AUDIO_GATEWAY_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_AUDIO_GATEWAY, AEE_APPSBTAPP_RES_FILE,
                          IDS_AUDIO_GATEWAY_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_NETWORK_ACCESS,
                     pMe->mNA.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mNA.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_NETWORK_ACCESS, AEE_APPSBTAPP_RES_FILE,
                          IDS_NETWORK_ACCESS_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_NETWORK_ACCESS, AEE_APPSBTAPP_RES_FILE,
                          IDS_NETWORK_ACCESS_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_NETWORK_ACCESS, AEE_APPSBTAPP_RES_FILE,
                          IDS_NETWORK_ACCESS_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }

#ifdef FEATURE_BT_EXTPF_OPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_OPP, 
                     pMe->mOPP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mOPP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OPP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OPP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OPP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif

#ifdef FEATURE_BT_EXTPF_FTP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_FTP, 
                     pMe->mFTP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mFTP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_FTP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_FTP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_FTP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_FTP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_FTP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_FTP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif

#ifdef FEATURE_BT_EXTPF_BIP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_BIP, 
                     pMe->mBIP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mBIP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BIP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BIP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BIP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BIP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BIP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BIP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif

#ifdef FEATURE_BT_EXTPF_BPP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_BPP, 
                     pMe->mBPP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mBPP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BPP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BPP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_BPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_BPP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_PBAP, 
                     pMe->mPBAP.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mPBAP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_PBAP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_PBAP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_PBAP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_PBAP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_PBAP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_PBAP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif

#ifdef FEATURE_IOBEX
/*
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_OBEX, 
                     pMe->mOBEX.bSecSelected ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mOBEX.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OBEX, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OBEX_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OBEX, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OBEX_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_OBEX, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_OBEX_SEC_LOW, NULL);
      break;
  }
*/
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_A2DP, 
                     pMe->mA2DP.bSecSelected ? 
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mA2DP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_A2DP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_A2DP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_A2DP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_A2DP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_A2DP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_A2DP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_AVRCP, 
                     pMe->mAVRCP.bSecSelected ? 
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mAVRCP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_AVRCP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_AVRCP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_AVRCP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_AVRCP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_AVRCP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_AVRCP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
#endif  //FEATURE_BT_EXTPF_AV

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_L2,
                     pMe->mL2.bSecSelected ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mL2.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_L2, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_L2_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_L2, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_L2_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_L2, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_L2_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_SPP,
                     pMe->mSPP.bSecSelected ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  switch(pMe->mSPP.srvSecType)
  {
    case AEEBT_RM_SVC_SEC_HIGH:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_SPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_SPP_SEC_HIGH, NULL);
      break;
    case AEEBT_RM_SVC_SEC_MED:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_SPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_SPP_SEC_MED, NULL);
      break;
    case AEEBT_RM_SVC_SEC_LOW:
      IMENUCTL_SetItemText( pMe->m_pIMenu,IDS_ENABLE_SPP, AEE_APPSBTAPP_RES_FILE,
                          IDS_ENABLE_SPP_SEC_LOW, NULL);
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  }
  // Activate menu
  PUSH_MENU( BT_APP_MENU_SVR_SEC_TYPE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
 
}

/* ==========================================================================
FUNCTION BTApp_BuildSecurityOptionsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_BuildSecurityOptionsMenu(   CBTApp* pMe   )
{
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_FATAL( "BuildSecurityOptionsMenu - ", 0, 0, 0 );

  CLEAR_SCREEN();
  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SERV_SECURITY, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_SERV_SECURITY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

   BTApp_InitAddItem( &ai );



  // Add individual entries to the Menu

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_LOW, 
                     (pMe->mRM.srvSecType == AEEBT_RM_SVC_SEC_LOW) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_MED, 
                     (pMe->mRM.srvSecType == AEEBT_RM_SVC_SEC_MED) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_HIGH, 
                     (pMe->mRM.srvSecType == AEEBT_RM_SVC_SEC_HIGH) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY_NONE, 
                     (pMe->mRM.srvSecType == AEEBT_RM_SVC_SEC_NONE) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTHORIZE, 
                     (pMe->mRM.bAuthorize ) ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTHORIZE_FIRST, 
                     (pMe->mRM.bAuthorizeFirst ) ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  
  // select current setting
  switch ( pMe->mRM.srvSecType )
  {
    case AEEBT_RM_SVC_SEC_LOW:
      sel = IDS_SECURITY_LOW;
      break;
    case AEEBT_RM_SVC_SEC_MED:
      sel = IDS_SECURITY_MED;
      break;
    case AEEBT_RM_SVC_SEC_HIGH:
      sel = IDS_SECURITY_HIGH;
      break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
    case AEEBT_RM_SVC_SEC_NONE:
      sel = IDS_SECURITY_NONE;
      break;
  }
  if ( ( pMe->mAG.srvSecType != pMe->mRM.srvSecType ) && 
        ( pMe->mAG.bSecSelected == TRUE ) )
  {
    pMe->mAG.srvSecType      = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
  if ( ( pMe->mNA.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mNA.bSecSelected == TRUE ) )
  {
    pMe->mNA.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#ifdef FEATURE_BT_EXTPF_OPP 
  if ( ( pMe->mOPP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mOPP.bSecSelected == TRUE ) )
  {
    pMe->mOPP.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  if ( ( pMe->mFTP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mFTP.bSecSelected == TRUE ) )
  {
    pMe->mFTP.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_BIP
  if ( ( pMe->mBIP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mBIP.bSecSelected == TRUE ) )
  {
    pMe->mBIP.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_BPP 
  if ( ( pMe->mBPP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mBPP.bSecSelected == TRUE ) )
  {
    pMe->mBPP.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  if ( ( pMe->mPBAP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mPBAP.bSecSelected == TRUE ) )
  {
    pMe->mPBAP.srvSecType    = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
  if ( ( pMe->mA2DP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mA2DP.bSecSelected == TRUE ) )
  {
    pMe->mA2DP.srvSecType    = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
  if ( ( pMe->mAVRCP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mAVRCP.bSecSelected == TRUE ) )
  {
    pMe->mAVRCP.srvSecType   = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
#endif
  if ( ( pMe->mL2.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mL2.bSecSelected == TRUE ) )
  {
    pMe->mL2.srvSecType      = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
  if ( ( pMe->mSPP.srvSecType != pMe->mRM.srvSecType ) &&
       ( pMe->mSPP.bSecSelected == TRUE ) )
  {
    pMe->mSPP.srvSecType     = pMe->mRM.srvSecType;
    pMe->bConfigChanged      = TRUE;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );
 // Activate menu
  PUSH_MENU( BT_APP_MENU_SECURITY_OPTIONS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  return TRUE;
}
/* ==========================================================================
FUNCTION BTApp_BuildDBGKeyHndlrMenu
DESCRIPTION
============================================================================= */
static void BTApp_BuildDBGKeyHndlrMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  MSG_FATAL( "BuildDBGKeyHndlrMenu ",0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  
  //SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  //Modify by zzg 2010_11_01
  SETAEERECT (&rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, pMe->m_rect.dy);
  
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_DBG_KEY, NULL );
  if(pMe->m_pIAnn != NULL)
  {
      //MSG_FATAL("***zzg BTAppMod_Release m_nRefs=%d***", ((BTAppMod *)po)->m_nRefs, 0, 0);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_DBG_KEY,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DBG_KEY, 
                     (pMe->mRM.bDBGEnabled) ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  PUSH_MENU( BT_APP_MENU_DBG_KEY );
  
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

}

#endif /* FEATURE_BT_2_1 */

#define NUM_SRC_BYTES 2000
/* ==========================================================================
FUNCTION BTApp_VerifyRcvdData
DESCRIPTION
============================================================================= */
static boolean BTApp_VerifyRcvdData( 
  CBTApp* pMe, 
  uint32  uBytesRcvd
)
{
  boolean good = TRUE;
  int32 i,j;

  if( SPP_BUF_EMPTY() )
  {
    return TRUE;
  }

  for ( j = pMe->mSPP.buf_count-1; j >= 0 ; j-- )
  {
    i = ((uBytesRcvd - 1) % NUM_SRC_BYTES);
    if ( pMe->mSPP.buf[(pMe->mSPP.buf_start + j) % SPP_BUF_SZ] != (uint8)i )
    {
      MSG_FATAL( "VerifyRcvdData - i=%x ith=%x data=%x", 
                 (uint8)i, uBytesRcvd-1,
                 pMe->mSPP.buf[(pMe->mSPP.buf_start + j) % SPP_BUF_SZ] );
      good = FALSE;
      SPP_CLEAR_BUF();
      break;
    }
    pMe->mSPP.buf_count--;
    uBytesRcvd--;
  }

  pMe->mSPP.buf_end = pMe->mSPP.buf_start;

  return good;
}

/* ==========================================================================
FUNCTION BTApp_SendSPPData
DESCRIPTION
============================================================================= */
static void BTApp_SendSPPData( CBTApp* pMe )
{
  static boolean bDataGenerated = FALSE;
  int32 bytes2Send;
  int32 i, offset;
  
  if ( pMe->pMem == NULL )
  {
    pMe->pMem = MALLOC( NUM_SRC_BYTES );
  }

  if ( pMe->pMem == NULL )
  {
    MSG_FATAL( "SendSPPData - MALLOC failed", 0, 0, 0 );
  }
  else
  {
    if ( bDataGenerated == FALSE )
    {
      for ( i=0; i<NUM_SRC_BYTES; i++)
      {
        pMe->pMem[ i ] = (uint8)i;
      }
      bDataGenerated = TRUE;
    }

    bytes2Send = 
      (NUM_SRC_BYTES * pMe->mSPP.uNumDataBufs) - pMe->mSPP.uTotalBytesSent;

    while ( bytes2Send > 0 )
    {
      offset = pMe->mSPP.uTotalBytesSent % NUM_SRC_BYTES;
      i = IBTEXTSPP_Write( pMe->mSPP.po, (char*)&pMe->pMem[ offset ], 
                           NUM_SRC_BYTES - offset );
      if ( i > 0 )
      {
        pMe->mSPP.uTotalBytesSent += i;
        bytes2Send -= i;

        if ( i < (NUM_SRC_BYTES - offset) )
        {
          CALLBACK_Init( &pMe->mSPP.writeableCb, BTApp_SendSPPData, pMe );
          IBTEXTSPP_Writeable( pMe->mSPP.po, &pMe->mSPP.writeableCb );
          break;
        }
      }
      else
      {
        if ( i == AEE_STREAM_WOULDBLOCK )
        {
          ISHELL_SetTimer( pMe->m_pShell, SPP_CLIENT_WAIT_MS,
                           (PFNNOTIFY) BTApp_SendSPPData, pMe );
        }
        break;
      }
    }
    if ( bytes2Send <= 0)
    {
      MSG_HIGH( "SendSPPData - sent %d bytes total", 
                pMe->mSPP.uTotalBytesSent, 0, 0 );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_SendBackSPPData
DESCRIPTION
============================================================================= */
static void BTApp_SendBackSPPData( CBTApp* pMe )
{
  int32 bytesSent = 0;
  
  while ( !SPP_BUF_EMPTY() && (bytesSent >= 0) )
  {
    bytesSent = IBTEXTSPP_Write( pMe->mSPP.po, 
                                 (char*)(pMe->mSPP.buf + pMe->mSPP.buf_start),
                                 (pMe->mSPP.buf_end <= pMe->mSPP.buf_start ?
                                  SPP_BUF_SZ - pMe->mSPP.buf_start :
                                  pMe->mSPP.buf_end - pMe->mSPP.buf_start) );
    
    if ( bytesSent > 0)
    {
      pMe->mSPP.buf_start += bytesSent;
      pMe->mSPP.buf_start %= SPP_BUF_SZ;
      pMe->mSPP.buf_count -= bytesSent;
      pMe->mSPP.uTotalBytesSent += bytesSent;
    }
    else if ( bytesSent == AEE_STREAM_WOULDBLOCK )
    {
      CALLBACK_Cancel( &pMe->mSPP.readableCb );
      CALLBACK_Init( &pMe->mSPP.writeableCb, BTApp_SendBackSPPData, pMe );
      IBTEXTSPP_Writeable( pMe->mSPP.po, &pMe->mSPP.writeableCb );
    }
    else
    {
      MSG_FATAL( "SendBackSPPData - SPP_Write failed %d", bytesSent, 0, 0);
    }
  }
  
  if( !SPP_BUF_FULL() )
  {
    CALLBACK_Init( &pMe->mSPP.readableCb, BTApp_RetrieveSPPData, pMe );
    IBTEXTSPP_Readable( pMe->mSPP.po, &pMe->mSPP.readableCb );
  }
}

/* ==========================================================================
FUNCTION BTApp_RetrieveSPPData
DESCRIPTION
============================================================================= */
static void BTApp_RetrieveSPPData( CBTApp* pMe )
{
  int32 bytesRcvd;

  while ( !SPP_BUF_FULL() && 
          ( (bytesRcvd = 
             IBTEXTSPP_Read( pMe->mSPP.po, 
                             (char*)(pMe->mSPP.buf + pMe->mSPP.buf_end),
                             ( pMe->mSPP.buf_end < pMe->mSPP.buf_start ?
                               pMe->mSPP.buf_start - pMe->mSPP.buf_end :
                               SPP_BUF_SZ - pMe->mSPP.buf_end ))) > 0 )
        )
  {
    pMe->mSPP.buf_end += bytesRcvd;
    pMe->mSPP.buf_end %= SPP_BUF_SZ;
    pMe->mSPP.uTotalBytesRcvd += bytesRcvd;
    pMe->mSPP.buf_count += bytesRcvd;
  }

  if ( pMe->mSPP.bClient )
  {
    if ( pMe->mSPP.bDoVerify )
    {
      pMe->mSPP.bDoVerify = 
        BTApp_VerifyRcvdData( pMe, pMe->mSPP.uTotalBytesRcvd );
    }
    else
    {
      SPP_CLEAR_BUF();
    }
  }
  else if( !SPP_BUF_EMPTY() )
  {
    CALLBACK_Init( &pMe->mSPP.writeableCb, BTApp_SendBackSPPData, pMe );
    IBTEXTSPP_Writeable( pMe->mSPP.po, &pMe->mSPP.writeableCb );
  }

  if( !SPP_BUF_FULL() )
  {
    CALLBACK_Init( &pMe->mSPP.readableCb, BTApp_RetrieveSPPData, pMe );
    IBTEXTSPP_Readable( pMe->mSPP.po, &pMe->mSPP.readableCb );
  }
}

#ifdef FEATURE_PHONE_VR
/* ==========================================================================
FUNCTION BTApp_StartVR
DESCRIPTION
============================================================================= */
static void BTApp_StartVR( CBTApp* pMe )
{
  MSG_FATAL( "StartVR - as=%d ac=%d", pMe->mAG.bAudioSelected, 
           pMe->mAG.bAudioConnected, 0 );

#ifdef FEATURE_APP_PUREVOICE
  pMe->mAG.callState = BTAPP_AG_CALL_STARTVR;
  if ( BTApp_ConnectAudio( pMe, TRUE ) != FALSE )
  {
    MSG_FATAL( "StartVR - launching Purevoice", 0, 0, 0 );
    if ( ISHELL_StartApplet( pMe->m_pShell, 
                             AEECLSID_PUREVOICE ) != SUCCESS )
    {
      MSG_FATAL( "StartVR - failed to launch PureVoice", 0, 0, 0 );
      pMe->mAG.bStartedVr = FALSE;
      IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
    }
    else
    {
      pMe->mAG.bStartedVr = TRUE;
      pMe->mAG.bDevDialed = TRUE;
    }
  }
#else
  MSG_FATAL( "StartVR - PureVoice app not present", 0, 0, 0 );
#endif /* FEATURE_APP_PUREVOICE */
}

/* ==========================================================================
FUNCTION BTApp_EndVR
DESCRIPTION
============================================================================= */
static void BTApp_EndVR( CBTApp* pMe )
{
  MSG_FATAL( "EndVR - as=%d ac=%d", pMe->mAG.bAudioSelected, 
           pMe->mAG.bAudioConnected, 0 );
#ifdef FEATURE_APP_PUREVOICE
  if ( ISHELL_ActiveApplet( pMe->m_pShell ) == AEECLSID_PUREVOICE )
  {
    boolean bProcessed = FALSE;
    MSG_FATAL( "EndVR - closing PureVoice", 0, 0, 0 );
    bProcessed = ISHELL_SendEvent( pMe->m_pShell, AEECLSID_PUREVOICE, 
                                   EVT_USER, 0x88, 0 ); // 0x88 is signal for PureVoice to exit
    if ( bProcessed == FALSE )
    {
      MSG_FATAL( "EndVR - evt sent to PureVoice not processed", 0, 0, 0 );
    }
    pMe->mAG.bStartedVr = FALSE;
    IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
  }
#endif
}
#endif /* FEATURE_PHONE_VR */

/*****************************************************************************/
/*****                       Callback functions.                         *****/          
/*****************************************************************************/

/* ==========================================================================
FUNCTION BTApp_ClearDiscoverable
DESCRIPTION
============================================================================= */
static boolean BTApp_ClearDiscoverable( CBTApp* pMe )
{
  int result;

  ISHELL_CancelTimer( pMe->m_pShell, 
                      (PFNNOTIFY) BTApp_ClearDiscoverable, pMe );

  if ( (result = IBTEXTSD_SetDiscoverable( pMe->mSD.po, FALSE )) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_MSG_DISCOVERABLE_NOT_CLEARED, NULL, 2 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }

  return (result == SUCCESS);
}

/* ==========================================================================
FUNCTION BTApp_AcceptButtonPress
DESCRIPTION
============================================================================= */
static void BTApp_AcceptButtonPress( CBTApp* pMe )
{
  pMe->mAG.bIgnoreButton = FALSE;
  MSG_FATAL( "AcceptButtonPress - stop ignoring button press", 0, 0, 0 );
}

/* ==========================================================================
FUNCTION BTApp_BTApp_SuspendA2DP
DESCRIPTION
============================================================================= */
static boolean BTApp_SuspendA2DP( CBTApp* pMe )
{
  boolean bProceed = TRUE;

  MSG_FATAL("***zzg BTApp_SuspendA2DP bStreaming=%x, bAudioSelected=%x, bConnected=%x***", 
  			pMe->mA2DP.bStreaming, pMe->mA2DP.bAudioSelected, pMe->mA2DP.bConnected);
  
#ifdef FEATURE_BT_EXTPF_AV
  if ( pMe->mA2DP.bStreaming != FALSE )
  {
    bProceed = FALSE;
    MSG_FATAL("AG - wait for A2DP to suspend...", 0, 0, 0 );      
  }
  if ( pMe->mA2DP.bAudioSelected != FALSE )
  {
    MSG_FATAL( "AG - Releasing A2DP A2conn=%d", pMe->mA2DP.bConnected, 0, 0 );
    BTApp_ReleaseA2DPDevice( pMe ); // suspending won't do
  }
#endif
  return bProceed;
}

#ifdef FEATURE_AVS_BT_SCO_REWORK
/* ==========================================================================
FUNCTION BTApp_ReleaseBTDeviceDelayed
DESCRIPTION
  Called when the BT device release timer expires.
============================================================================= */
static void BTApp_ReleaseBTDeviceDelayed( CBTApp* pMe )
{
  BTApp_ReleaseBTDevice( pMe, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_ConnectAudio
DESCRIPTION
  This is the SCO_REWORK version of ConnectAudio().  In this version, SCO is 
  always brought up before PCM path is set.
============================================================================= */
static boolean BTApp_ConnectAudio( CBTApp* pMe, boolean bForceUnmute )
{
  boolean bDone = FALSE;

  MSG_FATAL("***zzg BTApp_ConnectAudio bForceUnmute=%x***", bForceUnmute, 0, 0);
  
  if ( IS_AG_CONNECTABLE() != FALSE )
  {
    BTApp_UseBTDevice( pMe, bForceUnmute );
    ISHELL_CancelTimer( pMe->m_pShell, 
                        (PFNNOTIFY) BTApp_DisconnectAudioDelayed, pMe );

    if ( pMe->mAG.bAudioConnected == TRUE )
    {
      if ( pMe->mAG.bAudioDisconnecting == FALSE )
      {
        bDone = TRUE;
      }
      else
      {
        /* else, wait until SCO is disconnected */
        pMe->mAG.bAudioConnecting = TRUE;
      }
    }
    else /* SCO is not established */
    {
      /* cancel any pending SCO disconnection */
      pMe->mAG.bAudioDisconnecting = FALSE;

	  MSG_FATAL("***zzg BTApp_SuspendA2DP 1***", 0, 0, 0);
	  
      if ( BTApp_SuspendA2DP( pMe ) == FALSE )
      {
        /* remember the reason for suspending A2DP streaming */
        pMe->mAG.bAudioConnecting = TRUE;
        pMe->mAG.bForceUnmute     = bForceUnmute;
      }
      else if ( pMe->mAG.bAudioConnecting == FALSE )
      {
        /* Register callback for sound device changes */
        MSG_HIGH( "AG - Register Dev Chg CB", 0, 0, 0 );
        uisnd_notify_register( (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack,
                               (void*)pMe );

        pMe->mAG.bAudioConnecting = TRUE;

		MSG_FATAL("***zzg IBTEXTAG_ConnectAudio***", 0, 0, 0);
        IBTEXTAG_ConnectAudio( pMe->mAG.po );
        /* set timer for SCO establishment attempts */
        ISHELL_SetTimer( pMe->m_pShell, BT_APP_RELEASE_BT_DEVICE_DELAY_MS,
                         (PFNNOTIFY) BTApp_ReleaseBTDeviceDelayed, pMe );
      }
      /* else, SCO connection request already sent to the stack */
    }
  }

  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_DisconnectAudio
DESCRIPTION
  This is the SCO_REWORK version of DisconnectAudio().  In this version, SCO is 
  always brought down before PCM path is clear.
============================================================================= */
static boolean BTApp_DisconnectAudio( CBTApp* pMe, boolean bForceUnmute )
{
  boolean  bDone        = FALSE;
  pMe->mAG.bForceUnmute = bForceUnmute;

  MSG_FATAL("***zzg BTApp_DisconnectAudio bAudioConnected=%x***", pMe->mAG.bAudioConnected, 0, 0);
  MSG_FATAL("***zzg BTApp_DisconnectAudio bAudioDisconnecting=%x***", pMe->mAG.bAudioDisconnecting, 0, 0);
	
  if ( pMe->mAG.bAudioConnected != FALSE )
  {
    MSG_FATAL( "AG - Disc Audio, Disconnecting=%d",
             pMe->mAG.bAudioDisconnecting, 0, 0 );

    /* Deregister Device Change Callback */
    if ( !BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) )
    {
      uisnd_notify_unregister(
        (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack,
        (void*)pMe );
    }

    if ( pMe->mAG.bAudioDisconnecting == FALSE )
    {
      IBTEXTAG_DisconnectAudio( pMe->mAG.po );
      pMe->mAG.bAudioDisconnecting = TRUE;
    }
    else
    {
      /* cancel any pending SCO connection */
      pMe->mAG.bAudioConnecting = FALSE;
    }
  }
  else if ( pMe->mAG.bAudioConnecting != FALSE )
  {
    MSG_FATAL( "AG - waiting to Disc Audio, fum=%d", bForceUnmute, 0, 0 );
    pMe->mAG.bAudioDisconnecting = TRUE; //let the connection be established first
  }
  else // already disconnected
  {
    pMe->mAG.bForceUnmute = FALSE;
    bDone                 = TRUE;
  }

  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_DisconnectAudioDelayed
DESCRIPTION
  Called when the audio delay timer expires.
============================================================================= */
static void BTApp_DisconnectAudioDelayed( CBTApp* pMe )
{
  MSG_FATAL("***zzg BTApp_DisconnectAudioDelayed***", 0, 0, 0);
  BTApp_DisconnectAudio( pMe, TRUE );
}

/* ==========================================================================
FUNCTION BTApp_SetInBandRing
DESCRIPTION
  This function exists only for SCO_REWORK.
============================================================================= */
static void BTApp_SetInBandRing( CBTApp* pMe, boolean bInBandRing )
{
  if ( bInBandRing != pMe->mAG.bInbandRing )
  {
    pMe->mAG.bInbandRing = bInBandRing;
    IBTEXTAG_SetInbandRingCapable( pMe->mAG.po, bInBandRing );
  }
  MSG_FATAL( "SetInBandRing - supported=%d", bInBandRing, 0, 0 );
}

/* ==========================================================================
FUNCTION BTApp_HandleAudioSetup
DESCRIPTION
  This function exists only for SCO_REWORK.  It sets up or tear down SCO 
  depending on the Sound Type AVS is about to play.
============================================================================= */
static void BTApp_HandleAudioSetup( CBTApp* pMe, BTAppCallType callPresent )
{
  boolean  bUp = FALSE;

  MSG_FATAL("***zzg BTApp_HandleAudioSetup Start***", 0, 0, 0);

  /* Audio handling happens if AG is active */
  if ( pMe->mAG.bEnabled != FALSE )
  {
    if ( pMe->mAG.uSoundType & (AEEBT_SND_BT_SCO | AEEBT_SND_BT_OTHER) )
    {
      BTAPP_AG_SETBIT( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B );

      if ( IS_SCO_4_VOICE_UP() )
      {
        /* set flag so that we can re-set up SCO later,
        ** if applicable.
        */
        BTAPP_AG_SETBIT( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B );
        bUp = TRUE;
      }
    }

	MSG_FATAL("***zzg BTApp_HandleAudioSetup uSoundType=%x***", pMe->mAG.uSoundType, 0, 0);

    switch ( pMe->mAG.uSoundType )
    {
      case AEEBT_SND_BT_SCO:
      {
        if ( bUp == FALSE )
        {
          BTApp_ConnectAudio( pMe, TRUE );
          if( pMe->mA2DP.bAudioSelected == FALSE )
          {
            BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr, 
                                      BTAPP_MGR_RELEASE_A2DP );
          }
        }
        break;
      }
      case AEEBT_SND_BT_OTHER:
      {
        BTApp_DisconnectAudio( pMe, TRUE );
#ifdef FEATURE_BT_EXTPF_AV
        if ( IS_A2DP_CONNECTABLE() )
        {
          if ((pMe->mA2DP.bConnected != FALSE) || \
              ((pMe->mAG.bConnected != FALSE) && \
               (pMe->mA2DP.bConnected == FALSE) && \
                AEEBT_BD_ADDRS_EQUAL(&(pMe->mAG.bdAddr), \
                                     &(pMe->mA2DP.bdAddr))))
          {
            BTApp_UseA2DPDevice( pMe );
            BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr,
                                    BTAPP_MGR_USE_A2DP );

          }
        }
#endif /* FEATURE_BT_EXTPF_AV */
        break;
      }
      case (AEEBT_SND_BT_SCO | AEEBT_SND_BT_OTHER):
      {
#ifdef FEATURE_BT_EXTPF_AV	
		uisnd_notify_data_s_type sndInfo;
		uisnd_get_device(&sndInfo);
		MSG_FATAL("***zzg dev=%d sMute=%d mMute=%d***", 
				sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);

		MSG_FATAL("***zzg AudioSetup AG bEnable=%x, bConnected=%x***", pMe->mAG.bEnabled, pMe->mAG.bConnected, 0);
		MSG_FATAL("***zzg AudioSetup A2DP bEnable=%x, bConnected=%x***", pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);
				
		//Add By zzg 2011_11_07
		if (pMe->mA2DP.bEnabled == TRUE)
		{
			MSG_FATAL("***zzg bMusicPlayerSuspend=%x***", pMe->bMusicPlayerSuspend, 0, 0);

			if (TRUE == BTApp_CheckMusicPlayrState(pMe->m_pShell))
			{
				//if (pMe->bMusicPlayerSuspend == FALSE)
				if ((pMe->bMusicPlayerSuspend == FALSE) && (pMe->mA2DP.bConnected == TRUE))
				{
					uisnd_set_device_status(SND_DEVICE_BT_A2DP_HEADSET, UISND_DEV_ENABLED);
				}
			}
			else
			{
				uisnd_set_device_status(SND_DEVICE_BT_A2DP_HEADSET, UISND_DEV_UNATTACHED);
				//pMe->bMusicPlayerSuspend = TRUE;
			}	 
			 
		}
		//Add End

		if (pMe->mAG.bConnected == TRUE)
		{
			if (TRUE == uisnd_is_device_enabled(SND_DEVICE_BT_A2DP_HEADSET))
			{
				MSG_FATAL("***zzg SND_DEVICE_BT_A2DP_HEADSET Enabled***", 0, 0, 0);
		        if ( IS_A2DP_CONNECTABLE() )
		        {
		        	MSG_FATAL("***zzg IS_A2DP_CONNECTABLE TRUE***", 0, 0, 0);

					MSG_FATAL("***zzg IS_A2DP_CONNECTABLE TRUE bStreaming=%x***", pMe->mA2DP.bStreaming, 0, 0);

					MSG_FATAL("***zzg IS_A2DP_CONNECTABLE mAG bEnabled=%x, bConnected=%x, bAudioConnected=%x***", 
								pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.bAudioConnected);

					MSG_FATAL("***zzg IS_A2DP_CONNECTABLE mA2DP bEnabled=%x, bConnected=%x***", 
								pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);
					
		          if ((pMe->mA2DP.bConnected != FALSE) || \
		              ((pMe->mAG.bConnected != FALSE) && \
		               (pMe->mA2DP.bConnected == FALSE) && \
		                AEEBT_BD_ADDRS_EQUAL(&(pMe->mAG.bdAddr), \
		                                     &(pMe->mA2DP.bdAddr))))
		          {
		            MSG_FATAL("***zzg IS_A2DP_CONNECTABLE TRUE 1***", 0, 0, 0);
		            BTApp_DisconnectAudio( pMe, FALSE );
		            BTApp_UseA2DPDevice( pMe );
		            BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr,
		                                    BTAPP_MGR_USE_A2DP );
		          }
		          // AVS will initiate A2DP connection
		        }
			}
	        else
#endif /* FEATURE_BT_EXTPF_AV */
	        {
	        	MSG_FATAL("***zzg SND_DEVICE_BT_A2DP_HEADSET Disabled***", 0, 0, 0);
	        	MSG_FATAL("***zzg IS_A2DP_CONNECTABLE TRUE 2***", 0, 0, 0);

				MSG_FATAL("***zzg AudioSetup TRUE 2***", 0, 0, 0);

				DBGPRINTF("***zzg AudioSetup mAG:%02x%02x%02x%02x%02x%02x***", 
											(uint16)(pMe->mAG.bdAddr.uAddr[5]), (uint16)(pMe->mAG.bdAddr.uAddr[4]),
						               		(uint16)(pMe->mAG.bdAddr.uAddr[3]), (uint16)(pMe->mAG.bdAddr.uAddr[2]), 
						               		(uint16)(pMe->mAG.bdAddr.uAddr[1]), (uint16)(pMe->mAG.bdAddr.uAddr[0]));
				DBGPRINTF("***zzg AudioSetup mA2DP:%02x%02x%02x%02x%02x%02x***", 
											(uint16)(pMe->mA2DP.bdAddr.uAddr[5]), (uint16)(pMe->mA2DP.bdAddr.uAddr[4]),
						               		(uint16)(pMe->mA2DP.bdAddr.uAddr[3]), (uint16)(pMe->mA2DP.bdAddr.uAddr[2]), 
						               		(uint16)(pMe->mA2DP.bdAddr.uAddr[1]), (uint16)(pMe->mA2DP.bdAddr.uAddr[0]));				
				
	          	BTApp_ConnectAudio( pMe, TRUE );	
				
	          if( pMe->mA2DP.bAudioSelected == FALSE )
	          {
	            BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr,
	                                    BTAPP_MGR_RELEASE_A2DP );
	          }
	        }  
#ifdef FEATURE_BT_EXTPF_AV	
		}
#endif		
        break;
      }
      case AEEBT_SND_OTHER:
      {
        BTApp_ReleaseBTDevice( pMe, FALSE );
        break;
      }
      case AEEBT_SND_DTMF_START:
      case AEEBT_SND_DTMF_RING_START:
      {
        /* AVS will determine where to play them if SCO is not connected */
        break;
      }
      case AEEBT_SND_NONE:
      {
	  	MSG_FATAL("***zzg BTApp Audio_Setup AEEBT_SND_NONE***", 0, 0, 0);
		
        BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B );

		//Add By zzg 2011_11_07
		if (pMe->mA2DP.bEnabled == TRUE)
		{
			uisnd_set_device_status(SND_DEVICE_BT_A2DP_HEADSET, UISND_DEV_UNATTACHED);
		}
		//Add End
		

        if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) )
        {
          BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_AUDIO_WANTED_B );

		  MSG_FATAL("***zzg AEEBT_SND_NONE uisnd_notify_unregister***", 0, 0, 0);
          uisnd_notify_unregister(
            (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack,
            (void*)pMe );
        }

        ISHELL_CancelTimer( pMe->m_pShell, 
                            (PFNNOTIFY) BTApp_ReleaseBTDeviceDelayed, pMe );

		MSG_FATAL("***zzg BTApp_HandleAudioSetup callPresent=%x, bDevDialed=%x***", callPresent, pMe->mAG.bDevDialed, 0);

        if ( callPresent != BT_APP_CALL_NONE )
        {
          if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B ) )
          {
            BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B );
            /* inband ringtone/media clip started and ended during this
               voice call session; need to have SCO in place now
            */
            BTApp_ConnectAudio( pMe, TRUE );
            if( pMe->mA2DP.bAudioSelected == FALSE )
            {
              BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr,
                                        BTAPP_MGR_RELEASE_A2DP );
            }
          }
          else if ( BTApp_DisconnectAudio( pMe, FALSE ) != FALSE )
          {
            /* could be A2DP used just now */
			 MSG_FATAL("***zzg BTApp_SuspendA2DP 2***", 0, 0, 0);
            BTApp_SuspendA2DP( pMe );
          }
        }
        else if ( pMe->mAG.bDevDialed == FALSE )
        {
          ISHELL_SetTimer( pMe->m_pShell, BT_APP_DISC_AUDIO_DELAY_MS, 
                           (PFNNOTIFY) BTApp_DisconnectAudioDelayed, pMe );
        }
        /* else, must be the case in which SCO audio is already playing
           and a call is being originated causing AVS to stop the SCO
           audio;  No need to bring down SCO.
        */

        /* deliberately continue to set BT device in use */
      }
      case AEEBT_SND_DTMF_STOP:
      case AEEBT_SND_OTHER_STOP:
      case AEEBT_SND_DTMF_RING_STOP:
      {
        if ( (pMe->mAG.bSLCUp != FALSE) &&
             (pMe->mA2DP.bAudioSelected == FALSE) &&
             (pMe->mAG.callState == BTAPP_AG_CALL_NONE) )
        {
          BTApp_UseBTDevice( pMe, FALSE );
        }
        break;
      }
      default:
        MSG_FATAL( "HdleAudioSetup: snd type 0x%02x ignored", 
                 pMe->mAG.uSoundType, 0, 0 );
        break;
    }
  } 
}

/* ==========================================================================
FUNCTION BTApp_HandleMTCall
DESCRIPTION
  This is the SCO_REWORK version of HandleMTCall().  In this version, the Sound
  Type AVS is about to play indicates whether inband ringing is supported and
  whether SCO should be brought up.
============================================================================= */
static void BTApp_HandleMTCall( CBTApp* pMe )
{
  MSG_FATAL( "AG - HndlMTCall cSt=%d snd=0x%x", 
           pMe->mAG.callState, pMe->mAG.uSoundType, 0 );

  MSG_FATAL("***zzg BTApp_HandleMTCall uSoundType=%x, callState=%x***", pMe->mAG.uSoundType, pMe->mAG.callState, 0);

  switch ( pMe->mAG.uSoundType )
  {
    case (AEEBT_SND_BT_SCO | AEEBT_SND_BT_OTHER):
    case AEEBT_SND_BT_SCO:
    case AEEBT_SND_BT_OTHER:
    {
      if ( pMe->mAG.bEnabled != FALSE )
      {
        BTAPP_AG_SETBIT( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B );
      }
      // deliberate continuation (no break!)
    }
    case AEEBT_SND_OTHER:
    case AEEBT_SND_DTMF_RING_START:
    {
      switch ( pMe->mAG.callState )
      {
        case BTAPP_AG_CALL_NONE:
        {
          boolean bInband = FALSE;
          pMe->mAG.callState = BTAPP_AG_CALL_INCOMING_NOINBAND;
          if ( pMe->mAG.uSoundType & AEEBT_SND_BT_SCO )
          {
            bInband = TRUE;
            pMe->mAG.callState = BTAPP_AG_CALL_INCOMING_INBAND;
          }
          else
          {
            BTApp_ReleaseBTDevice( pMe, FALSE );
          }
          BTApp_SetInBandRing( pMe, bInband );
          pMe->mAG.bDevPickedUp = FALSE;

          // deliberate continuation (no break!)
        }
        case BTAPP_AG_CALL_INCOMING_NOINBAND:
        case BTAPP_AG_CALL_INCOMING_INBAND:
        {
          if ( (pMe->mAG.bInbandRing == FALSE) || 
               (BTApp_ConnectAudio( pMe, TRUE ) != FALSE) )
          {
            BTApp_SendRing( pMe );
          }
          break;
        }
      }
      break;
    }
    case AEEBT_SND_OTHER_STOP:
    case AEEBT_SND_DTMF_RING_STOP:
    {
      if ( (pMe->mAG.bSLCUp != FALSE) &&
           (pMe->mA2DP.bAudioSelected == FALSE) )
      {
        BTApp_UseBTDevice( pMe, FALSE );
      }
      break;
    }
    case AEEBT_SND_NONE:
    {
      BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B );

      if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) )
      {
        BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_AUDIO_WANTED_B );

        uisnd_notify_unregister(
          (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack,
          (void*)pMe );
      }

	   MSG_FATAL("***zzg BTApp_SuspendA2DP 3***", 0, 0, 0);

      BTApp_SuspendA2DP( pMe );
      break;
    }
    default:
      MSG_FATAL( "HandleMTCall - snd type 0x%02x ignored", 
               pMe->mAG.uSoundType, 0, 0 );
      return;
  }
}
#else /* FEATURE_AVS_BT_SCO_REWORK not defined*/
/* ==========================================================================
FUNCTION BTApp_ConnectAudio
DESCRIPTION
  This is the non-SCO_REWORK version of ConnectAudio().  In this version, 
  AVS controls when to bring up SCO.
============================================================================= */
static boolean BTApp_ConnectAudio( CBTApp* pMe, boolean bForceUnmute )
{
  boolean bDone = FALSE;

  if ( (pMe->mAG.bAudioConnected == FALSE) &&
       (IS_AG_CONNECTABLE() != FALSE) )
  {
    if ( (pMe->bVocoderOn == FALSE) && (bForceUnmute != FALSE) )
    {
      pMe->mAG.bAudioConnecting = TRUE; // SCO link is coming up
    }
    BTApp_UseBTDevice( pMe, bForceUnmute );
  }
  else
  {
    bDone = TRUE;
  }
  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_DisconnectAudio
DESCRIPTION
  This is the non-SCO_REWORK version of DisconnectAudio().  In this version, 
  AVS controls when to bring down SCO.
============================================================================= */
static boolean BTApp_DisconnectAudio( CBTApp* pMe, boolean bForceUnmute )
{
  boolean bDone = FALSE;

  if ( pMe->mAG.bAudioConnecting != FALSE )
  {
    MSG_FATAL( "AG - waiting to Disc Audio, fum=%d", bForceUnmute, 0, 0 );
    pMe->mAG.bForceUnmute        = bForceUnmute;
    pMe->mAG.bAudioDisconnecting = TRUE; //let the connection be established first
  }
  else
  {
    BTApp_ReleaseBTDevice( pMe, bForceUnmute );
    bDone = TRUE;
  }
  return bDone;
}

/* ==========================================================================
FUNCTION BTApp_CheckInBandRing
DESCRIPTION
  This function exists only for non-SCO_REWORK.
============================================================================= */
static boolean BTApp_CheckInBandRing( CBTApp* pMe )
{
  nv_item_type nvi;
  char* FileExt = NULL;
  boolean bInBandRing = FALSE;

  if ( OEMNV_Get( NV_MM_RINGER_FILE_I, &nvi ) == NV_DONE_S )
  {
    FileExt = STRCHREND( (const char *)nvi.mm_ringer_file, '.');
    if ( FileExt != NULL )
    {
      FileExt++;
      if ( MEMCMP( (void *)FileExt, (void *)"mid", 3 ) == 0 )
      {
        bInBandRing = TRUE;
      }
      if ( MEMCMP( (void *)FileExt, (void *)"qcp", 3 ) == 0 )
      {
        bInBandRing = TRUE;
      }
    }
  }
  if ( bInBandRing != pMe->mAG.bInbandRing )
  {
    pMe->mAG.bInbandRing = bInBandRing;
    IBTEXTAG_SetInbandRingCapable( pMe->mAG.po, bInBandRing );
  }
  MSG_FATAL( "CheckInBandRing - supported=%d", bInBandRing, 0, 0 );
  return bInBandRing;
}

/* ==========================================================================
FUNCTION BTApp_HandleMTCall
DESCRIPTION
  This is the non-SCO_REWORK version of HandleMTCall().
============================================================================= */
static void BTApp_HandleMTCall( CBTApp* pMe )
{
  boolean bDoInband = BTApp_CheckInBandRing( pMe );

  MSG_FATAL("***zzg BTApp_HandleMTCall bDoInband=%x***", bDoInband, 0, 0);
  
  MSG_FATAL( "AG - HndlMTCall callSt=%d AGconn=%d AGas=%d", 
           pMe->mAG.callState, pMe->mAG.bConnected, pMe->mAG.bAudioConnected );
  pMe->mAG.bDevPickedUp = FALSE;
  pMe->mAG.callState = (bDoInband != FALSE) ? BTAPP_AG_CALL_INCOMING_INBAND :
                                              BTAPP_AG_CALL_INCOMING_NOINBAND;

   MSG_FATAL("***zzg BTApp_SuspendA2DP 4***", 0, 0, 0);
											  
  if ( (BTApp_SuspendA2DP( pMe ) != FALSE) )
  {
    if ( pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_INBAND )
    {
      BTApp_ConnectAudio( pMe, TRUE );
    }
    else
    {
      BTApp_SendRing( pMe );
    }
  }
  else
  {
    IBTEXTAG_Connect( pMe->mAG.po, &pMe->mAG.bdAddr, pMe->mAG.devType );
  }
  
}
#endif /* FEATURE_AVS_BT_SCO_REWORK */

/* ==========================================================================
FUNCTION BTApp_HandleCallAndMOSSetup
DESCRIPTION
============================================================================= */
static void BTApp_HandleCallAndMOSSetup( CBTApp* pMe )
{
  BTAppCallType callPresent = BTApp_CallPresent( pMe );
  BTAppCallType callIncoming = BTApp_CallIncoming( pMe );

  MSG_HIGH( "AG - HndlCall&MOSSetup c=%x i=%x", callPresent, callIncoming, 0 );

  MSG_FATAL("***zzg BTApp_HandleCallAndMOSSetup callIncoming=%x, callPresent=%x***", callIncoming, callPresent, 0);

  if ( callIncoming != BT_APP_CALL_NONE )
  {
    // an MT call is being setup
    BTApp_HandleMTCall( pMe );
  }
  else
  {
    if ( callPresent != BT_APP_CALL_NONE )
    {
      // should audio be on HS/HF?
      if ( (pMe->mAG.bDevPickedUp != FALSE) || 
           ((pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HANDSFREE) &&
            (pMe->mAG.bPrivateMode == FALSE)) )
      {
        pMe->mAG.bDevPickedUp = FALSE;
		MSG_FATAL("***zzg BTApp_ConnectAudio 1***", 0, 0, 0);
        BTApp_ConnectAudio( pMe, TRUE );
      }
      else
      {
#ifdef FEATURE_AVS_BT_SCO_REWORK
        BTApp_ReleaseBTDevice( pMe, FALSE );
      }
    }
    else /* else, no call */
    {
      if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B ) )
      {
        /* playback is on, bring up SCO to route it to headset */
		MSG_FATAL("***zzg BTApp_ConnectAudio 2***", 0, 0, 0);

		
		//Add By zzg 2011_11_07
		MSG_FATAL("***zzg AG bEnabled=%x, bConnected=%x***", pMe->mAG.bEnabled, pMe->mAG.bConnected, 0);
		MSG_FATAL("***zzg A2DP bEnabled=%x, bEnableA2DP=%x, bConnected=%x***", 
					pMe->mA2DP.bEnabled, pMe->mA2DP.bEnableA2DP, pMe->mA2DP.bConnected);

		if ((pMe->mA2DP.bEnabled == TRUE) && (pMe->mA2DP.bConnected == FALSE))
		{
			pMe->mA2DP.bdAddr = pMe->mAG.bdAddr;

			DBGPRINTF("***zzg HandleCallAndMOSSetup mA2DP:%02x%02x%02x%02x%02x%02x***", 
						(uint16)(pMe->mA2DP.bdAddr.uAddr[5]), (uint16)(pMe->mA2DP.bdAddr.uAddr[4]),
	               		(uint16)(pMe->mA2DP.bdAddr.uAddr[3]), (uint16)(pMe->mA2DP.bdAddr.uAddr[2]), 
	               		(uint16)(pMe->mA2DP.bdAddr.uAddr[1]), (uint16)(pMe->mA2DP.bdAddr.uAddr[0]));
			
			pMe->bConfigChanged = TRUE;
		    IBTEXTA2DP_SetDevice(pMe->mA2DP.po, &pMe->mA2DP.bdAddr);	

			//Add By zzg 2011_11_03
			//BTApp_WriteBtAddrConfigFile(pMe);			
			//BTApp_WriteConfigFile(pMe);
			//Add End				
			
			if ( IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr ) != SUCCESS )
			{
				//BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
			}  	
			else
			{
				BTApp_A2DPSetRetries(pMe, TRUE);
			}

			//Add By zzg 2011_11_09 for MO_call(The MT_Call has registered by the BTApp_ConnectAudio)
			//uisnd_notify_register( (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack, (void*)pMe );
			//Add End
			
		}
		else		
		//Add End	
        {
        	BTApp_ConnectAudio( pMe, TRUE );
		}
      }
      else
      {
        BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* FEATURE_AVS_BT_SCO_REWORK */
      }
    }

    if ( (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET) &&
         ((callPresent != BT_APP_CALL_NONE) ||
          BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_IGNORE_BUTTON_AT_ACL_B )) )
    {
      MSG_FATAL( "HndlCall&MOSSetup - start ignoring button press", 0, 0, 0 );
      pMe->mAG.bIgnoreButton = TRUE;
      ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND * 2, 
                       (PFNNOTIFY) BTApp_AcceptButtonPress, pMe );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleAudioConnEv
DESCRIPTION
  Prior to SCO_REWORK, AUDIO_CONNECTED event signifies SCO link establishment
  initiated by HF or by AVS.  With SCO_REWORK, either BTApp or HF can bring up
  SCO link.
============================================================================= */
static void BTApp_HandleAudioConnEv( CBTApp *pMe )
{
  BTAppCallType callIncoming = BTApp_CallIncoming( pMe );

  MSG_FATAL("***zzg BTApp_HandleAudioConnEv***", 0, 0, 0);
  
  MSG_HIGH( "AudioConnEv - ac|as=0x%04x deSelA=%d ci|callSt=0x%04x ", 
            ((uint16)(pMe->mAG.bAudioConnected << 8) | 
             BTApp_BTAudioSelected( pMe )), pMe->mAG.bAudioDisconnecting,
            ((uint16)(callIncoming << 8) | pMe->mAG.callState) );

#ifdef FEATURE_AVS_BT_SCO_REWORK
  ISHELL_CancelTimer( pMe->m_pShell,
                      (PFNNOTIFY) BTApp_ReleaseBTDeviceDelayed, pMe );
#endif /* FEATURE_AVS_BT_SCO_REWORK */

#if defined (FEATURE_UI_CORE) && defined (FEATURE_BT)
  BTApp_InitVolumes( pMe );
#endif /* defined (FEATURE_UI_CORE) && defined (FEATURE_BT) */

  pMe->mAG.bAudioConnected = TRUE;

  MSG_FATAL("***zzg bAudioConnecting=%d***", pMe->mAG.bAudioConnecting, 0, 0);
  
  if ( pMe->mAG.bAudioConnecting != FALSE ) // mobile initiated SCO
  {
    pMe->mAG.bAudioConnecting = FALSE;

	
    if ( pMe->mAG.bAudioDisconnecting != FALSE )
    {
      /* now carry out pending SCO disconnection */
      /* clear the flag so the following operation can get through */
      pMe->mAG.bAudioDisconnecting = FALSE;
	 
      BTApp_DisconnectAudio( pMe, TRUE );
    }
    else
    {    
      BTApp_UseBTDevice( pMe, pMe->mAG.bForceUnmute );
      pMe->mAG.bForceUnmute = FALSE;
    }

	MSG_FATAL("***zzg BTApp_HandleAudioConnEv callState=%x***", pMe->mAG.callState, 0, 0);
	
    switch ( pMe->mAG.callState )
    {
      case BTAPP_AG_CALL_INCOMING_INBAND:
      case BTAPP_AG_CALL_INCOMING_NOINBAND:
        if ( pMe->mAG.bDevPickedUp != FALSE )
        {
          BTApp_AnswerCall( pMe, callIncoming );
        }
        else if ( (callIncoming != BT_APP_CALL_NONE) &&
                  (ISHELL_GetTimerExpiration( pMe->m_pShell,
                                              (PFNNOTIFY) BTApp_SendRing,
                                              pMe ) == 0) )
        {
          BTApp_SendRing( pMe );
        }
        break;
      case BTAPP_AG_CALL_ANSWERED:
      case BTAPP_AG_CALL_CONNECTED:
#ifdef FEATURE_BT_EXTPF_AV
        if ( pMe->mA2DP.bStreaming != FALSE )
        {
          // how is this possible? locally initiated SCO while A2DP is streaming?
          MSG_FATAL( "AudioConnEv - SCO + A2DP streaming ", 0, 0, 0 );

          // gotta bring down one
          BTApp_PickAudioLink( pMe );
        }
#ifndef FEATURE_AVS_BT_SCO_REWORK
        else if ( IS_A2DP_CONNECTABLE() )
        {
          // SCO is settle, ok to reselect A2DP_SCO sound device type
          BTApp_UseA2DPDevice( pMe );
        }
#endif /* !FEATURE_AVS_BT_SCO_REWORK */
#endif /* FEATURE_BT_EXTPF_AV */
        break;
      case BTAPP_AG_CALL_STARTVR:
#ifdef FEATURE_PHONE_VR
        BTApp_StartVR( pMe );
#endif
        break;
    }
  }
  else
  {
    boolean bAGWins = TRUE;
    MSG_HIGH( "AudioConnEv - carkit initiated connection", 0, 0, 0 );
	
	MSG_FATAL("***zzg AudioConnEv - carkit initiated connection", 0, 0, 0 );

#ifdef FEATURE_BT_EXTPF_AV
    if ( pMe->mA2DP.bStreaming != FALSE )
    {      
      bAGWins = BTApp_PickAudioLink( pMe );
      if ( bAGWins != FALSE )
      {       
      	 MSG_FATAL("***zzg BTApp_SuspendA2DP 5***", 0, 0, 0);
        BTApp_SuspendA2DP( pMe );
      }
    }
#endif // FEATURE_BT_EXTPF_AV
    if ( bAGWins != FALSE )
    {     
      BTApp_UseBTDevice( pMe, FALSE );
    }
  }
    
  if ( TOP_MENU == BT_APP_MENU_MAIN )
  {  	
    BTApp_BuildMenu( pMe, TOP_MENU );
  }
  else if ( TOP_MENU == BT_APP_MENU_TESTS )
  {  	
    BTApp_HandleClearKey( pMe );
  }

  //ISHELL_CloseApplet(pMe->m_pShell, FALSE);	 //Add By zzg 2011_10_24

  MSG_FATAL("***zzg BTApp_HandleAudioConnEv 8***", 0, 0, 0);
}

/* ==========================================================================
FUNCTION BTApp_RefreshMyInfo
DESCRIPTION
============================================================================= */
static void BTApp_RefreshMyInfo( CBTApp* pMe )
{
  BTApp_BuildTopMenu( pMe );
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_RefreshDisplayPasskey
DESCRIPTION
============================================================================= */
static void BTApp_RefreshDisplayPasskey( CBTApp* pMe )
{
  if ( TOP_MENU == BT_APP_MENU_DISPLAY_PASSKEY )
  {
    BTApp_BuildTopMenu( pMe );
  }
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_ProcessAGNotifications
DESCRIPTION
============================================================================= */
static void BTApp_ProcessAGNotifications( 
  CBTApp*           pMe, 
  AEEEvent          evt, 
  NotificationData* pData
)
{
	MSG_FATAL("***zzg BTApp_ProcessAGNotifications evt=%d", evt, 0, 0);
	
  switch ( evt )
  {
    case AEEBT_AG_EVT_CONFIGURED:          // response to Config()
      if ( pData->uError != AEEBT_AG_ERR_NONE )
      {
        MSG_FATAL( "AG - Configure failed err=%d", pData->uError, 0, 0 );
      }
      break;
    case AEEBT_AG_EVT_ENABLED:             // response to Enable()
    {
      /* 
       * Either the static app was not enabled or the Static app AG 
       * was not selected both are the indication that this app should 
       * not handle this event
       */
      if ( (pMe->bEnableAtStart == FALSE) ||
           (pMe->mAG.bSelected == FALSE) )
      {
          /* Static app is not the one initiating this and so ignore it */
          break;
      }
      MSG_HIGH( "AG - Enabled err=%d", pData->uError, 0, 0 );
      if ( (pData->uError == AEEBT_AG_ERR_NONE) || 
           (pData->uError == AEEBT_AG_ERR_ALREADY_ENABLED) )
      {
        pMe->mAG.bEnabled = TRUE;
        pMe->mAG.callState = BTAPP_AG_CALL_NONE;
#ifdef FEATURE_BT_HFP_1_5
        pMe->mAG.errATMode = BTAPP_CMEE_ERR_DISABLE; // default err state
        pMe->mAG.bValidSSData = FALSE;
        pMe->mAG.pSSInfo = (AEETSSInfo*)MALLOC( sizeof(AEETSSInfo) );
        if ( pMe->mAG.pSSInfo == NULL )
        {
          MSG_FATAL( "ProcessAGNotifications - Mem alloc failed", 0, 0, 0 );
          return;
        }        
#endif

		MSG_FATAL("***zzg AEEBT_AG_EVT_ENABLED mAG:%04x %04x %04x***", 
					((uint16)(pMe->mAG.bdAddr.uAddr[ 5 ] << 8) | pMe->mAG.bdAddr.uAddr[ 4 ]),
		       		((uint16)(pMe->mAG.bdAddr.uAddr[ 3 ] << 8) | pMe->mAG.bdAddr.uAddr[ 2 ]),
		       		((uint16)(pMe->mAG.bdAddr.uAddr[ 1 ] << 8) | pMe->mAG.bdAddr.uAddr[ 0 ]));

		
        if ( BDADDR_VALID( &pMe->mAG.bdAddr ) &&
             (BTApp_CallPresent( pMe ) != BT_APP_CALL_NONE) )
            
        //if (BDADDR_VALID( &pMe->mAG.bdAddr ))	//Modify by zzg 2011_11_22 (first connect AG, then A2DP)
        {
        	MSG_FATAL("***zzg AEEBT_AG_EVT_ENABLED***", 0, 0, 0);
          IBTEXTAG_Connect( pMe->mAG.po, &pMe->mAG.bdAddr, pMe->mAG.devType );
        }
        pMe->mEnablingType++;		
        BTApp_EnableBT(pMe);	
      }
      break;
    }
    case AEEBT_AG_EVT_CONNECTED:
    {
      MSG_HIGH( "AG - Connected BDA=%04x %04x %04x", 
               ((uint16)(pData->bdAddr.uAddr[5] << 8) | pData->bdAddr.uAddr[4]), 
               ((uint16)(pData->bdAddr.uAddr[3] << 8) | pData->bdAddr.uAddr[2]), 
               ((uint16)(pData->bdAddr.uAddr[1] << 8) | pData->bdAddr.uAddr[0]) );
      pMe->mAG.connectedBDAddr = pData->bdAddr;
      pMe->mAG.bConnected = TRUE;
      pMe->mAG.bconnInPrgs = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

	  /*	
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
         BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONN, &pData->bdAddr, 2 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
	  */
/*	  
#ifdef FEATURE_VERSION_C316
    ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_USER,EVT_BT_AG_AUDIO_CONNECTED,0);
#endif
*/

	//Add By zzg 2011_10_24
	BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONN, &pData->bdAddr, 2 );
	//Add End
	  
	  MSG_FATAL("***zzg BTApp_ProcessAGNotifications devType=%d", pMe->mAG.devType, 0, 0);
	  
      if ( pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET )
      {
        BTAppMgr_UpdateProfileType( pMe, &pMe->mAG.bdAddr, BTAPP_MGR_AG, BTAPP_MGR_CONNECTED );
        BTApp_HandleCallAndMOSSetup( pMe );
      }
	  
	  //ISHELL_CloseApplet(pMe->m_pShell, FALSE);	 //Add By zzg 2011_10_24
      break;
    }
    case AEEBT_AG_EVT_CONNECT_FAILED:
    {
      MSG_FATAL( "AG - Connect failed BDA=%04x %04x %04x", 
               ((uint16)(pData->bdAddr.uAddr[5] << 8) | pData->bdAddr.uAddr[4]), 
               ((uint16)(pData->bdAddr.uAddr[3] << 8) | pData->bdAddr.uAddr[2]), 
               ((uint16)(pData->bdAddr.uAddr[1] << 8) | pData->bdAddr.uAddr[0]) );

	  BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONNF, &pData->bdAddr, 2 );

	  MSG_FATAL("***zzg AEEBT_AG_EVT_CONNECT_FAILED uError=%x", pData->uError, 0, 0);	  

      // clean up, in case connection attempt was done to transfer 
      // voice call to HS/HF
      pMe->mAG.bDevPickedUp = FALSE;
      pMe->mAG.bAudioConnecting = FALSE;
      pMe->mAG.bconnInPrgs = FALSE;

	  //ISHELL_CloseApplet(pMe->m_pShell, FALSE);	 //Add By zzg 2011_10_24
      break;
    }
    case AEEBT_AG_EVT_DISCONNECTED:
    {
      MSG_HIGH( "AG - Disconnected BDA=%04x %04x %04x", 
                ((uint16)(pData->bdAddr.uAddr[5] << 8) | pData->bdAddr.uAddr[4]), 
                ((uint16)(pData->bdAddr.uAddr[3] << 8) | pData->bdAddr.uAddr[2]), 
                ((uint16)(pData->bdAddr.uAddr[1] << 8) | pData->bdAddr.uAddr[0]) );
      pMe->mAG.bConnected          = FALSE;
      pMe->mAG.bAudioConnecting    = FALSE;
      pMe->mAG.bAudioDisconnecting = FALSE;
#ifdef FEATURE_AVS_BT_SCO_REWORK
      BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* FEATURE_AVS_BT_SCO_REWORK */

      if ( pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET )
      {
        BTAppMgr_UpdateProfileType( pMe, &pMe->mAG.bdAddr, BTAPP_MGR_AG, BTAPP_MGR_DISCONNECTED );
      }

      if ( BTApp_CallConnected(pMe ) != BT_APP_CALL_NONE )
      {
        pMe->mAG.bDevPickedUp = TRUE;
        // this is so that when connection comes up again, audio will be 
        // transfer to HF automatically
      }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
	  /*
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_AG_DISCONN, 
                          &pMe->mAG.connectedBDAddr, 2 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
	  */
/*	  
#ifdef FEATURE_VERSION_C316
    ISHELL_PostEvent(pMe->m_pShell,AEECLSID_CORE_APP,EVT_USER,EVT_BT_AG_AUDIO_DISCONNECTED,0);
#endif
*/

	//Add By zzg 2011_10_24
	BTApp_ShowDevMsg( pMe, IDS_MSG_AG_DISCONN, &pMe->mAG.connectedBDAddr, 2);
	//ISHELL_CloseApplet(pMe->m_pShell, FALSE);	
	//Add End
      break;
    }
    case AEEBT_AG_EVT_RUNG:                // response to Ring()
      MSG_FATAL("***zzg BTApp_ProcessAGNotify AEEBT_AG_EVT_RUNG uError=%x***", pData->uError, 0, 0);
	  
      MSG_FATAL( "AG - RUNG err=%d", pData->uError, 0, 0 );
      if ( pData->uError == AEEBT_AG_ERR_RING_FAILED )
      {
        BTApp_DisconnectAudio( pMe, TRUE );
      }
      break;
    case AEEBT_AG_EVT_VR_CAPABLE_SET:      // response to SetVRCapable()
    case AEEBT_AG_EVT_SPKR_GAIN_SET:       // response to SetSpkrGain()
    case AEEBT_AG_EVT_MIC_GAIN_SET:        // response to SetMicGain()
      MSG_FATAL( "AG Notif - evt=%d", evt, 0, 0 );
      break;
    case AEEBT_AG_EVT_AUDIO_CONNECTED:     // may be due to ConnectAudio()     
      MSG_FATAL("***zzg AGNotify AEEBT_AG_EVT_AUDIO_CONNECTED***", 0, 0, 0);
      BTApp_HandleAudioConnEv( pMe );
      break;
    case AEEBT_AG_EVT_AUDIO_CONNECT_FAILED:
    {
      MSG_FATAL( "AG - AudioConnFailed err=%d", pData->uError, 0, 0 );
#ifdef FEATURE_AVS_BT_SCO_REWORK
      ISHELL_CancelTimer( pMe->m_pShell,
                          (PFNNOTIFY) BTApp_ReleaseBTDeviceDelayed, pMe );
#endif /* FEATURE_AVS_BT_SCO_REWORK */
      BTApp_ReleaseBTDevice( pMe, TRUE );
      pMe->mAG.bAudioConnecting    = FALSE;
      pMe->mAG.bAudioDisconnecting = FALSE;
      if ( pMe->mAG.bDevDialed != FALSE )
      {
        pMe->mAG.bDevDialed = FALSE;
        pMe->mAG.callState = BTAPP_AG_CALL_NONE;
      }
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else if ( TOP_MENU == BT_APP_MENU_TESTS )
      {
        BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AEEBT_AG_EVT_AUDIO_DISCONNECTED:  // may be due to DisconnectAudio()
    {
      BTAppCallType callPresent = BTApp_CallPresent( pMe );
      MSG_HIGH( "AG - Audio Disc reason=0x%x cp=%d", 
                pData->reason, callPresent, 0 );
      pMe->mAG.bAudioConnected     = FALSE;

	  MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 1***", 0, 0, 0);
	  
#ifdef FEATURE_AVS_BT_SCO_REWORK
      pMe->mAG.bAudioDisconnecting = FALSE;

		MSG_FATAL("***zzg BTApp_AGNotify bAudioConnecting=%x***", pMe->mAG.bAudioConnecting, 0, 0);
      if ( pMe->mAG.bAudioConnecting == TRUE )
      {      
      	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 2***", 0, 0, 0);
        /* carry out pending SCO connection */
        pMe->mAG.bAudioConnecting = FALSE;
        BTApp_ConnectAudio( pMe, TRUE );
      }
#endif /* FEATURE_AVS_BT_SCO_REWORK */

		MSG_FATAL("***zzg BTApp_AGNotify bStartedVr=%x***", pMe->mAG.bStartedVr, 0, 0);
      if ( pMe->mAG.bStartedVr != FALSE )
      {        
      	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 3***", 0, 0, 0);
        pMe->mAG.bStartedVr = FALSE;
        IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );

		MSG_FATAL("***zzg BTApp_AGNotify callState=%x***", pMe->mAG.callState, 0, 0);
        if ( pMe->mAG.callState == BTAPP_AG_CALL_STARTVR )
        {
          pMe->mAG.callState = BTAPP_AG_CALL_NONE;
#ifndef FEATURE_AVS_BT_SCO_REWORK
			MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 4***", 0, 0, 0);
          BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* !FEATURE_AVS_BT_SCO_REWORK */
        }
      }

	  MSG_FATAL("***zzg BTApp_AGNotify callState2=%x***", pMe->mAG.callState, 0, 0);
	  
      if ( pMe->mAG.callState == BTAPP_AG_CALL_HANGUP )
      {      	
      	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 5***", 0, 0, 0);
        BTApp_EndCall( pMe, callPresent );
      }
      else
      {
      	MSG_FATAL("***zzg BTApp_AGNotify callPresent=%x, bAudioConnecting=%x, BTAPP_AG_ISBITSET=%x***", 
				callPresent, pMe->mAG.bAudioConnecting, BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ));
#ifdef FEATURE_AVS_BT_SCO_REWORK
		/*
        if ( (callPresent != BT_APP_CALL_NONE) &&
             (pMe->mAG.bAudioConnecting == FALSE) &&
             !BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) ) // disc'd due to non-BT device being used.
        {        	
        	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 6***", 0, 0, 0);
          BTApp_ReleaseBTDevice( pMe, FALSE );
        }
		*/

		//Modify By zzg 2011_10_25
		if ((pMe->mAG.bAudioConnecting == FALSE) &&
             !BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ))
		{
			MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 6.5***", 0, 0, 0);
			BTApp_ReleaseBTDevice( pMe, FALSE );	
		}
		//Add End
#else
        if ( pData->reason != AEEBT_REASON_LOCAL_HOST_TERMINATED_CONNECTION )
        {        	
        	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 7***", 0, 0, 0);
          BTApp_ReleaseBTDevice(
            pMe, (callPresent != BT_APP_CALL_NONE) ? TRUE : FALSE );
        }
        else
        {      
        	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 8***", 0, 0, 0);
          BTApp_ReleaseBTDevice( pMe, pMe->mAG.bForceUnmute );
          pMe->mAG.bForceUnmute = FALSE;
        }
#endif /* FEATURE_AVS_BT_SCO_REWORK */
      }

		MSG_FATAL("***zzg BTApp_AGNotify IS_A2DP_CONNECTABLE=%x, bConnected=%x***", IS_A2DP_CONNECTABLE(), pMe->mA2DP.bConnected, 0);
		
#ifdef FEATURE_BT_EXTPF_AV
      if ( IS_A2DP_CONNECTABLE() )
      {
        if (pMe->mA2DP.bConnected != FALSE)
        {           	
        	MSG_FATAL("***zzg BTApp_AGNotify AEEBT_AG_EVT_AUDIO_DISCONNECTED 9***", 0, 0, 0);
          BTApp_UseA2DPDevice( pMe );
        }
      }
#endif

      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {      	
        BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else if ( TOP_MENU == BT_APP_MENU_TESTS )
      {      	
        BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AEEBT_AG_EVT_DISABLED:            // may be due to Disable()
    {
      MSG_HIGH( "AG - Disabled err=%d", pData->uError, 0, 0 );
      if ( pData->uError == AEEBT_NA_ERR_NONE )
      {
        ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_AG_DISABLED, 0L);
      }
      break;
    }
    // the following events are not associated with any commands
    case AEEBT_AG_EVT_BUTTON_PRESSED:      // audio device's button pressed
      BTApp_HandleEventButtonPressed( pMe );
      break;
    case AEEBT_AG_EVT_SPKR_GAIN_REPORT:    // audio device's spkr gain report
      MSG_FATAL( "AG - SpkrGainReport gain=%d", pData->uVolumeGain, 0, 0 );
      if ( pMe->mAG.spkrGain != pData->uVolumeGain )
      {
        pMe->mAG.spkrGain = pData->uVolumeGain;
        BTApp_WriteConfigFile( pMe );

        if ( TOP_MENU == BT_APP_MENU_SPKR_VOL )
        {
          BTApp_UpdateAGSettings( pMe, pMe->mAG.spkrGain );
          BTApp_DoAGSettingsMenu( pMe, 0 );
        }
      }
      break;
    case AEEBT_AG_EVT_MIC_GAIN_REPORT:     // audio device's mic gain report
      MSG_FATAL( "AG - MicGainReport gain=%d", pData->uVolumeGain, 0, 0 );
      if ( pMe->mAG.micGain != pData->uVolumeGain )
      {
        pMe->mAG.micGain = pData->uVolumeGain;
        BTApp_WriteConfigFile( pMe );

        if ( TOP_MENU == BT_APP_MENU_MIC_VOL )
        {
          BTApp_UpdateAGSettings( pMe, pMe->mAG.micGain );
          BTApp_DoAGSettingsMenu( pMe, 0 );
        }
      }
      break;

    // the following events are applicable to HF only
    case AEEBT_AG_EVT_SLC_UP:              // Service Level Connection up
    {
      MSG_HIGH( "AG - SLC up", 0, 0, 0 );
      pMe->mAG.bSLCUp  = TRUE;

#ifdef FEATURE_AVS_BT_SCO_REWORK
      if ( pMe->mA2DP.bAudioSelected == FALSE )
      {
        BTApp_UseBTDevice( pMe, FALSE );
      }
#endif /* FEATURE_AVS_BT_SCO_REWORK */

      BTAppMgr_UpdateProfileType( pMe, &pMe->mAG.bdAddr, BTAPP_MGR_AG, BTAPP_MGR_CONNECTED );
      BTApp_HandleCallAndMOSSetup( pMe );
#ifdef FEATURE_BT_HFP_1_5
      if ( pMe->mAG.pSSInfo != NULL )
      {
        if ( ITELEPHONE_GetServingSystemInfo( pMe->pIPhone, pMe->mAG.pSSInfo, 
                                              sizeof(AEETSSInfo)) != SUCCESS )
        {
          MSG_FATAL( "AG EXT IO - SS Info cannot be initialized", 0, 0, 0 );
          pMe->mAG.bValidSSData = FALSE;
        }
        else
        {
          pMe->mAG.bValidSSData = TRUE;
        }
      }
#endif
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
      	MSG_FATAL("BTApp_ProcessAGNotifications  BTApp_BuildMainMenu",0,0,0);
        //BTApp_BuildMainMenu( pMe );	//Del by zzg 2011_10_20
      }
      break;
    }
    case AEEBT_AG_EVT_SLC_DOWN:            // Service Level Connection down
    {
      BTAppCallType callPresent = BTApp_CallPresent( pMe );
      MSG_HIGH( "AG - SLC down", 0, 0, 0 );
      pMe->mAG.bSLCUp  = FALSE;
      BTAppMgr_UpdateProfileType( pMe, &pMe->mAG.bdAddr, BTAPP_MGR_AG, BTAPP_MGR_DISCONNECTED );
      if ( callPresent != BT_APP_CALL_NONE )
      {
        /* transfer audio to phone */
        BTApp_DisconnectAudio( pMe, TRUE );
      }      
      break;
    }
    case AEEBT_AG_EVT_DEV_PICKUP:          // HF answered incoming call
      BTApp_HandleEventDevPickUp( pMe );
      break;
    case AEEBT_AG_EVT_DEV_HANGUP:          // HF hung up call
      BTApp_HandleEventDevHangUp( pMe );
      break;
#ifdef FEATURE_PHONE_VR
    case AEEBT_AG_EVT_VR_ON:               // HF turned VR on
      MSG_FATAL( "AG - VR On", 0, 0, 0 );
      if ( pMe->mAG.bVREnabled == FALSE )
      {
        pMe->mAG.bUpdateVrState = TRUE;
        //BTApp_Dial( pMe );
      }
      else
      {
        BTApp_StartVR( pMe );
      }
      break;
    case AEEBT_AG_EVT_VR_OFF:              // HF turned VR off
      MSG_FATAL( "AG - VR Off", 0, 0, 0 );
      if ( pMe->mAG.bVREnabled == FALSE )
      {
        if ( pMe->mAG.bStartedVr != FALSE )
        {
          pMe->mAG.bStartedVr = FALSE;
          IBTEXTAG_UpdateVRState( pMe->mAG.po, FALSE );
        }
      }
      else
      {
        BTApp_EndVR( pMe );
      }
      break;
#endif
    case AEEBT_AG_EVT_DEV_DIAL:            // HF dialed a number
    {
      BTApp_HandleEventDevDial( pMe );
      break;
    }
#ifndef FEATURE_BT_HFP_1_5
    case AEEBT_AG_EVT_DEV_MEM_DIAL:        // HF dialed a number in memory
    {
      BTApp_HandleEventDevMemDial( pMe );
      break;
    }
    case AEEBT_AG_EVT_DEV_REDIAL:          // HF redialed last number
    {
      BTApp_HandleEventDevRedial( pMe );
      break;
    }
#endif
    case AEEBT_AG_EVT_DEV_SEND_DTMF:       // HF sent DTMF tone
      MSG_FATAL( "AG - DevSendDTMF ch=%d", pData->uDTMFChar, 0, 0 );
      break;
    case AEEBT_AG_EVT_VR_STATE_UPDATED:
      MSG_FATAL( "AG - VR state updated", 0, 0, 0 );
      break;
#ifdef FEATURE_BT_HFP_1_5
    case AEEBT_AG_EVT_EXTERNAL_IO_ENABLED:
      MSG_FATAL( "AG - External I/O enabled", 0, 0, 0 );
      break;
    case AEEBT_AG_EVT_EXTERNAL_IO_DISABLED:
      MSG_FATAL( "AG - External I/O disabled", 0, 0, 0 );
      break;
    case AEEBT_AG_EVT_RSSI_SET:
      MSG_FATAL( "AG - RSSI value set", 0, 0, 0 );
      break;
    case AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET:
      MSG_FATAL( "AG - Battery charge level set", 0, 0, 0 );
      break;
#endif
    case AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET:
      MSG_FATAL( "AG - Inband Ring Capable Set", 0, 0, 0 );
      break;
#ifdef FEATURE_AVS_BT_SCO_REWORK
    case AEEBT_AG_EVT_SOUND_CHANGING:
    {
      BTAppCallType callPresent = BTApp_CallPresent( pMe );
      BTAppCallType callIncoming = BTApp_CallIncoming( pMe );
      boolean a2s = 0;
#ifdef FEATURE_BT_EXTPF_AV
      a2s = pMe->mA2DP.bAudioSelected;
#endif

      if ( (pData->sSoundType != AEEBT_SND_DTMF_START) &&
           (pData->sSoundType != AEEBT_SND_DTMF_STOP))
      {
        MSG_FATAL( "AG - Snd Chg Evt=0x%x as|a2s=0x%04x cp|ci=0x%04x", 
                 pData->sSoundType, 
                 ((uint16)(pMe->mAG.bAudioSelected << 8) | a2s),
                 ((uint16)(callPresent << 8) | callIncoming) );
      }

      pMe->mAG.uSoundType = pData->sSoundType;

	  MSG_FATAL("***zzg AGNotify AEEBT_AG_EVT_SOUND_CHANGING***", 0, 0, 0);
	  
		{
			uisnd_notify_data_s_type sndInfo;
			uisnd_get_device(&sndInfo);
			MSG_FATAL("***zzg dev=%d sMute=%d mMute=%d***", 
				sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);
				
			MSG_FATAL("***zzg dev_attached=%x, dev_enabled=%x***", 
					uisnd_is_device_attached(sndInfo.out_device),
					uisnd_is_device_enabled(sndInfo.out_device),
					0);					
		}

      if ( (callIncoming != BT_APP_CALL_NONE) &&
           (pMe->mAG.callState < BTAPP_AG_CALL_ANSWERED) && // call still in incoming state
           (pMe->mAG.bEnabled != FALSE) )
      {
        BTApp_HandleMTCall( pMe );
      }
      else 
      {
        BTApp_HandleAudioSetup( pMe, callPresent );
      }
      break;
    }
#endif /* FEATURE_AVS_BT_SCO_REWORK */
    default:
      MSG_FATAL( "AG - unexpected ev=%d", evt, 0, 0 );
  }
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_PadString
DESCRIPTION
============================================================================= */
/*static*/ void BTApp_PadString(char* Passkey, int passKeyLen)
{
  uint8 len=0;
  uint8 i=0;
  char NullString[ AEEBT_SSP_PASSKEY_LEN ] = {"000000"};
  len = AEEBT_SSP_PASSKEY_LEN - STRLEN( Passkey );

  MSG_HIGH("Padding Passkey PadLen:%d passKeyLen:%d",len,passKeyLen,0);

  while((len < AEEBT_SSP_PASSKEY_LEN ) && (i < passKeyLen))
  {
    NullString[len-1] = *( Passkey+i );
    len++;
    i++;
  }
  STRLCPY(Passkey,NullString, passKeyLen);
}
#endif /* FEATURE_BT_2_1 */

/* ==========================================================================
FUNCTION BTApp_ProcessNANotifications
DESCRIPTION
============================================================================= */
static void BTApp_ProcessNANotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  MSG_FATAL( "NA Notif - evt=%d", evt, 0, 0 );
  switch ( evt )
  {
    case AEEBT_NA_EVT_ENABLED:
      MSG_HIGH( "NA - Enabled err=%d", pData->uError, 0, 0 );
      if ( pData->uError == AEEBT_NA_ERR_NONE )
      {
        pMe->mNA.bEnabled = TRUE;
      }
      else
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      }
      pMe->mEnablingType++;    
	 
      BTApp_EnableBT(pMe);
      break;
    case AEEBT_NA_EVT_DISABLED:
      MSG_HIGH( "NA - Disabled err=%d", pData->uError, 0, 0 );
      if ( pData->uError == AEEBT_NA_ERR_NONE )
      {
        ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_NA_DISABLED, 0L);

      }
      break;
    case AEEBT_NA_EVT_CONNECTED:
      MSG_HIGH( "NA - Connected", 0, 0, 0 );
      pMe->mNA.bConnected = TRUE;
      pMe->mNA.connectedBDAddr = pData->bdAddr;

	  /*
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else
	  */	
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_NA_CONN, &pData->bdAddr, 2 );
      }
      break;
    case AEEBT_NA_EVT_DISCONNECTED:
      MSG_HIGH( "NA - Disconnected", 0, 0, 0 );
      pMe->mNA.bConnected = FALSE;
	  /*
      if ( TOP_MENU == BT_APP_MENU_MAIN )
      {
        BTApp_BuildMenu( pMe, TOP_MENU );
      }
      else
	  	*/
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_NA_DISCONN, &pData->bdAddr, 2 );
      }
      break;
  }
}


/* ==========================================================================
FUNCTION BTApp_ProcessSDNotifications
DESCRIPTION
============================================================================= */
static void BTApp_ProcessSDNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  MSG_FATAL( "SD Notif - evt=%d", evt, 0, 0 );

  MSG_FATAL("***zzg BTApp_ProcessSDNotifications evt=%d***", evt, 0, 0 );

  switch ( evt )
  {
    case AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP:
    {
		MSG_FATAL( "SD - DiscResp nD=%d", pData->uNumDevicesDiscovered, 0, 0 );

		MSG_FATAL("***zzg AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP uNumRecs=%d***", pData->uNumDevicesDiscovered, 0, 0 );

		pMe->mSD.uNumRecs = pData->uNumDevicesDiscovered;

	  /*
      if ((pMe->mSD.uNumRecs == 1) || (TOP_MENU == BT_APP_MENU_DEV_RESP))
      {
		 BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_RESP );
	  }
	  */

		//Add By zzg 2011_01_11
		// Find the first Device, create the dlg of searchresult......
		if (pMe->mSD.uNumRecs == 1) 
		{
			//CLOSE_DIALOG(DLGRET_SRHRESULT)
			pMe->m_eDlgRet = DLGRET_SRHRESULT;
			(void) ISHELL_EndDialog(pMe->m_pShell);  	//关闭BTMSGBOX      	

		}	  
		else
		{
			//find more Device, Update the dlg of searchresult............
			MSG_FATAL("***zzg EVT_USER_REDRAW 111 AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP uNumRecs=%d***", pMe->mSD.uNumRecs, 0 , 0);
			(void) ISHELL_PostEvent( pMe->m_pShell,
			                     AEECLSID_BLUETOOTH_APP,
			                     EVT_USER_REDRAW,
			                     0,
			                     0);
		}
		//Add End
		break;
    }
    case AEEBT_SD_EVT_DEVICE_DISCOVERY_STARTED:
    {
      MSG_FATAL( "SD - DiscStarted err=%d", pData->uError, 0, 0 );
      if ( pData->uError == AEEBT_SD_ERR_NONE )
      {
#ifdef FEATURE_BT_2_1
        int i;
        for ( i = 0; i < MAX_DEVICES; i++ )
        {
          pMe->mRM.uDevToDspIdxMap[i] = 0;
        }
#endif /* FEATURE_BT_2_1 */
        pMe->mSD.bDiscovering = TRUE;
        ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*15, (PFNNOTIFY) BTApp_StopDeviceDiscovery, pMe );
      }
      else
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DEV_DISC_FAILED, NULL, 2 );
      }
      break;
    }
    case AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE:
    {
      uint16  index;
      AEEBTDeviceInfo* pDev;
    
      if ( pMe->mRM.device != NULL )
      {
        // set the environment for name discovery again
        for ( index = 0; index < MAX_DEVICES; index++ )
        {
          pDev = &pMe->mRM.device[index];
          pDev->uValue1 = 0;
        }
      }
      MSG_FATAL( "SD - DiscComp n=%d err=%d", 
               pData->sDevDiscComplete.uNumDevicesDiscovered,
               pData->sDevDiscComplete.uError, 0 );

      ISHELL_CancelTimer( pMe->m_pShell, 
                          (PFNNOTIFY) BTApp_StopDeviceDiscovery, pMe );

      pMe->mSD.uNumRecs = pData->sDevDiscComplete.uNumDevicesDiscovered;
      pMe->mSD.bDiscovering = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if ( pMe->mSD.uNumRecs == 0 )
      {
        uint16 uMsgID = IDS_MSG_NO_DEV_DISC;
        if ( pData->sDevDiscComplete.uError != AEEBT_SD_ERR_NONE )
        {
          uMsgID = IDS_MSG_DEV_DISC_FAILED;
        }
        BTApp_ShowMessage( pMe, uMsgID, NULL, 4 );
      }
	  /*
      else if ( TOP_MENU == BT_APP_MENU_DEV_RESP )
      {
        BTApp_BuildMenu( pMe, BT_APP_MENU_DEV_RESP );
      }
      */
      //Add By zzg 2011_01_11
	  else
	  {
	  	MSG_FATAL("***zzg EVT_USER_REDRAW 222 AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE uNumRecs=%d***", pMe->mSD.uNumRecs, 0 , 0);
		
		(void) ISHELL_PostEvent( pMe->m_pShell,
                                 AEECLSID_BLUETOOTH_APP,
                                 EVT_USER_REDRAW,
                                 0,
                                 0);
	  }
	  //Add End
      break;
    }
    case AEEBT_SD_EVT_DEVICE_DISCOVERY_STOPPED:
    {
      MSG_FATAL( "SD - DiscStopped err=%d", pData->uError, 0, 0 );
      pMe->mSD.bDiscovering = FALSE;
      ISHELL_CancelTimer( pMe->m_pShell, 
                          (PFNNOTIFY) BTApp_StopDeviceDiscovery, pMe );
      break;
    }
    case AEEBT_SD_EVT_DISCOVERABLE_MODE_SET:
    {
      MSG_FATAL( "SD - SetDisc err=%d", pData->uError, 0, 0 );
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_SD_DISC_SET, pData->uError );
      break;
    }
    case AEEBT_SD_EVT_DISCOVERABLE_MODE_RESP:
    {
      MSG_FATAL( "SD - GetDisc m=%d", pData->sDiscoverableModeResp.mode, 0, 0 );
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_SD_DISC_RSP, 
                          pData->sDiscoverableModeResp.mode );
      break;
    }
    case AEEBT_SD_EVT_BROWSE_RESP:
    {
      MSG_FATAL( "SD - BrowseResp nR=%d", pData->uNumSvcRecsFound, 0, 0 );
      pMe->mSD.uNumBrowseRecs = pData->uNumSvcRecsFound;
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_DEV_BROWSE_RESP, 0L);        
      break;
    }
    case AEEBT_SD_EVT_BROWSE_FAILED:
    {
      MSG_FATAL( "SD - BrowseFailed err=%d", pData->uError, 0, 0 );
      BTApp_ShowDevMsg( pMe, IDS_MSG_SVC_BROWSE_FAILED, 
                        &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 2 );
      break;
    }
    case AEEBT_SD_EVT_BROWSE_SERVICE_CANCELED:
    {
      MSG_FATAL( "SD - BrowseService cancelled", 0, 0, 0 );
      BTApp_BuildTopMenu( pMe );
      break;
    }
    case AEEBT_SD_EVT_SEARCH_RESP:
    {
      MSG_FATAL( "SD - SrchResp nR=%d", pData->uNumSvcRecsFound, 0, 0 );
      pMe->mSD.uNumSvcRecs = pData->uNumSvcRecsFound;
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_DEV_SEARCH_RESP, 0L);        
      break;
    }
    case AEEBT_SD_EVT_SEARCH_FAILED:
    {
      MSG_FATAL( "SD - SearchFailed err=%d", pData->uError, 0, 0 );
      BTApp_ShowDevMsg( pMe, IDS_MSG_SVC_SEARCH_FAILED, 
                        &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 2 );
      break;
    }
    case AEEBT_SD_EVT_DEVICE_NAME_RESP:
    {
      AEEBTDeviceInfo* pDev = NULL;
      if ( pMe->mRM.uGetNameDevIdx == MAX_DEVICES )
      {
        MSG_FATAL( "SD - Unexpected NameResp", 0, 0, 0 );
      }
      else
      {
        pDev = &pMe->mRM.device[ pMe->mRM.uGetNameDevIdx ];
        MSG_FATAL( "SD - NameResp idx=%d name=%c%c", pMe->mRM.uGetNameDevIdx,
                 pDev->wName[0], pDev->wName[1] );
        pMe->mRM.uGetNameDevIdx = MAX_DEVICES;
        pDev->uValue1 = UD1_GET_NAME_SUCCESS;

        //ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) BTApp_RefreshDevInfo, pMe);
        ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_UpdateDeviceInfo, pMe);
        

		/*
		if ( (TOP_MENU == BT_APP_MENU_DEV_RESP) || 
             (TOP_MENU == BT_APP_MENU_DEV_INFO) )
        {
          //menu should not be rebuild in all cases as it will effect automation 
          BTApp_BuildMenu( pMe, TOP_MENU ); 
        }
		*/

		MSG_FATAL("***zzg EVT_USER_REDRAW 333 AEEBT_SD_EVT_DEVICE_NAME_RESP***", 0, 0 , 0);
		//Add By zzg 2011_01_11	 
		(void) ISHELL_PostEvent( pMe->m_pShell,
                                 AEECLSID_BLUETOOTH_APP,
                                 EVT_USER_REDRAW,
                                 0,
     							 0);
	    //Add End
      }
      break;
    }
    case AEEBT_SD_EVT_DEVICE_NAME_FAILED:
    {
      AEEBTDeviceInfo* pDev = NULL;
      if ( pMe->mRM.uGetNameDevIdx >= MAX_DEVICES )
      {
        MSG_FATAL( "SD - Unexpected GetNameFailed", 0, 0, 0 );
      }
      else
      {
        pDev = &pMe->mRM.device[ pMe->mRM.uGetNameDevIdx ];
        MSG_FATAL( "SD - GetNameFailed", 0, 0, 0 );
        pMe->mRM.uGetNameDevIdx = MAX_DEVICES;
        pDev->uValue1 = UD1_GET_NAME_FAILED;

        //ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY) BTApp_RefreshDevInfo, pMe);
		ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)BTApp_UpdateDeviceInfo, pMe);

		/*
        if ( (TOP_MENU == BT_APP_MENU_DEV_RESP) || 
             (TOP_MENU == BT_APP_MENU_DEV_INFO) )
        {
          // menu should not be rebuild in all cases as it will effect automation 
          BTApp_BuildMenu( pMe, TOP_MENU ); 
        }
        */
        MSG_FATAL("***zzg EVT_USER_REDRAW 444 AEEBT_SD_EVT_DEVICE_NAME_FAILED***", 0, 0 , 0);
		
        //Add By zzg 2011_01_11	 
		(void) ISHELL_PostEvent( pMe->m_pShell,
                                 AEECLSID_BLUETOOTH_APP,
                                 EVT_USER_REDRAW,
                                 0,
     							 0);
	    //Add End
      }
      break;
    }
    case AEEBT_SD_EVT_SERVICE_REGISTERED:
    {
      break;
    }
    case AEEBT_SD_EVT_SERVICE_UNREGISTERED:
    {
      break;
    }
    default:
    {
      MSG_FATAL( "SD - unexpected ev=%d", evt, 0, 0 );
      break;
    }
  }
}


/* ==========================================================================
FUNCTION BTApp_ProcessRMNotifications
DESCRIPTION
============================================================================= */
static void BTApp_ProcessRMNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
	uint16 msgID;
	
#ifdef FEATURE_BT_2_1
	AECHAR wBuf[8];
	uint8 len =0;
	STRTOWSTR ("*", wBuf , sizeof(wBuf));
#endif /* FEATURE_BT_2_1 */     

	MSG_FATAL("***zzg BTApp_ProcessRMNotifications evt=%d***", evt, 0, 0);

	switch (evt)
	{
		case AEEBT_RM_EVT_LOCAL_NAME_SET:  // response to SetName()      
		{
			ISHELL_SetTimer( pMe->m_pShell, 500, (PFNNOTIFY) BTApp_RefreshMyInfo, pMe );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
		 	break;
		}
		
#ifdef FEATURE_BT_2_1
		case AEEBT_RM_EVT_NICK_NAME_SET:  // response to SetName()
		{
			ISHELL_SetTimer( pMe->m_pShell, 500, (PFNNOTIFY) BTApp_RefreshMyInfo, pMe );
		  	break;
		}
		
		case AEEBT_RM_EVT_MANUF_DATA_SET:  // response to   SetManuData() 
		{
			ISHELL_SetTimer( pMe->m_pShell, 500, (PFNNOTIFY) BTApp_RefreshMyInfo, pMe );
		  	break;
		}
		
		case AEEBT_RM_EVT_KEYPRESS_NOTIFICATION:
		{
			MSG_FATAL("***zzg AEEBT_RM_EVT_KEYPRESS_NOTIFICATION keypressType=%d", pData->keypress, 0, 0 );

			switch (pData->keypress)
			{
				case AEEBT_RM_KPN_DIGIT_ENTERED:
				{
					//ACG
					//STRTOWSTR ("*", wBuf , sizeof(AECHAR) );
					STRTOWSTR ("*", wBuf , sizeof(wBuf) );
					WSTRLCAT(pMe->mRM.wDisplayPassKey, wBuf, sizeof(pMe->mRM.wDisplayPassKey));
					DBGPRINTF_FATAL (" pMe->mRM.wDisplayPassKey =%s",pMe->mRM.wDisplayPassKey);
					ISHELL_SetTimer(pMe->m_pShell, 500, (PFNNOTIFY) BTApp_RefreshDisplayPasskey, pMe);
					break;
				}
				
				case AEEBT_RM_KPN_DIGIT_ERASED:
				{
					len = WSTRLEN( pMe->mRM.wDisplayPassKey );
					pMe->mRM.wDisplayPassKey [len -1] = (AECHAR)'\0';
					DBGPRINTF_FATAL (" pMe->mRM.wDisplayPassKey =%s", pMe->mRM.wDisplayPassKey);
					ISHELL_SetTimer(pMe->m_pShell, 500, (PFNNOTIFY) BTApp_RefreshDisplayPasskey, pMe);
					break;
				}
				
				case AEEBT_RM_KPN_COMPLETED:
				{
					BTApp_ShowBusyIcon(pMe);
				  	break;
				}

				default:
				{
					break;
				}
			}
			break;
		}
		
		case AEEBT_RM_EVT_PASSKEY_REQUEST:
		{
			pMe->mRM.bPassKey = TRUE ;
			pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr = pData->bdAddr;
			IBTEXTRM_DeviceRead(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx]);      
			pMe->mRM.bpassKeyRqst = TRUE;

#if 1
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
  					                     AEECLSID_BLUETOOTH_APP,
  					                     EVT_USER, EVT_PIN_RQST, 0L);    
#else

			//Add By zzg 2011_06_23
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{							
				pMe->m_app_flag = TRUE;
				pMe->m_user_wParam = evt;	
				ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
			}	
			else
			{
				ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
				                   AEECLSID_BLUETOOTH_APP,
				                   EVT_USER, EVT_PIN_RQST, 0L);  
			}
			//Add End
#endif
			break;   
		}
		
		case AEEBT_RM_EVT_PASSKEY_REPLIED:
		{
			MSG_FATAL("***zzg BTApp RMNotify AEEBT_RM_EVT_PASSKEY_REPLIED pData->uError=%d***", pData->uError, 0, 0);

			pMe->mRM.bPassKey = FALSE ;
			if (pData->uError !=  AEEBT_RM_ERR_NONE)
			{
				if (pMe->mRM.bBonding)
				{
					pMe->mRM.bBonding = FALSE;
				}

#if 1
				BTApp_ShowDevMsg( pMe, IDS_MSG_BOND_FAILED, NULL, 0 );
#else
				//Add By zzg 2011_06_23
				if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
				{							
					pMe->m_app_flag = TRUE;
					pMe->m_user_wParam = evt;	
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
				}	
				else
				{
					BTApp_ShowDevMsg(pMe, IDS_MSG_BOND_FAILED, NULL, 0); 
				}
				//Add End	
#endif
			}
			break;
		}
		
#endif /* FEATURE_BT_2_1 */

		case AEEBT_RM_EVT_LOCAL_COD_SET:   // response to SetClassOfDevice()
		{
			break;
		}
		
		case AEEBT_RM_EVT_LOCAL_SECURITY_SET:
		{
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
			                   AEECLSID_BLUETOOTH_APP,
			                   EVT_USER, EVT_RM_LOCAL_SEC_SET, 0L);
		  	break;
		}
		
		case AEEBT_RM_EVT_PIN_REQUESTED:   // remote device initiated bonding
		{
			pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr = pData->bdAddr;
			IBTEXTRM_DeviceRead(pMe->mRM.po,
			                   &pMe->mRM.device[pMe->mRM.uCurDevIdx]);

#if 1
			ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
			                  AEECLSID_BLUETOOTH_APP,
			                  EVT_USER, EVT_PIN_RQST, 0L );   
#else
			//Add By zzg 2011_06_23
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{							
				pMe->m_app_flag = TRUE;
				pMe->m_user_wParam = evt;	
				ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
			}	
			else
			{
				ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
				                   AEECLSID_BLUETOOTH_APP,
				                   EVT_USER, EVT_PIN_RQST, 0L);  
			}
			//Add End
#endif

			break;
		}
		
		case AEEBT_RM_EVT_BONDED:          // when bonding completed
		{
#ifndef FEATURE_BT_2_1
			AEEBTDeviceInfo* pDev = &pMe->mRM.device[pMe->mRM.uCurDevIdx];
			MSG_FATAL("RM - Bonded err=%d", pData->pDevUpdateStatus->error, 0, 0);

			if (AEEBT_BD_ADDRS_EQUAL(&pDev->bdAddr,
			                         &pData->pDevUpdateStatus->bdAddr) == FALSE)
			{
				MSG_FATAL("RM - Bonded addr != curRMDev(%d)", pMe->mRM.uCurDevIdx, 0, 0);
				pDev->bdAddr = pData->pDevUpdateStatus->bdAddr;
				IBTEXTRM_DeviceRead(pMe->mRM.po, pDev);
			}

			pMe->mRM.bBonding = FALSE;

			if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE)
			{
				ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
				                   AEECLSID_BLUETOOTH_APP,
				                   EVT_USER, EVT_RM_BONDED, 0L);
			}
			else
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				MSG_FATAL("***zzg BTApp RMNotify AEEBT_RM_EVT_BONDED pData->error=%d***", pData->pDevUpdateStatus->error, 0, 0);
				BTApp_ShowDevMsg(pMe, IDS_MSG_BOND_FAILED, &pData->pDevUpdateStatus->bdAddr, 0);
			}
#else
			AEEBTDeviceInfo dev;

			/*
			if ((TOP_MENU == BT_APP_MENU_REBOND) || (TOP_MENU == BT_APP_MENU_DISPLAY_PASSKEY))
			{
			(void)POP_MENU();
			}

			if (ISHELL_GetActiveDialog( pMe->m_pShell ) != NULL)
			{
			MSG_FATAL("AEEBT_RM_EVT_BONDED received when there isalready an active dialog.exiting", 0, 0,0);
			break;
			}
			*/

			if ((pMe->mRM.bUserCfm != FALSE) 
				 && (AEEBT_BD_ADDRS_EQUAL(&pMe->mRM.BondBDAddr,&pData->sBonded.bdAddr)) == FALSE)
			{
				MSG_FATAL("RM -  SSP Bonded addr != one requested .. exiting", 0, 0, 0);
				break;
			}

			if (pData->sBonded.uError == AEEBT_RM_ERR_NONE)
			{
				pMe->mRM.BondBDAddr = pData->sBonded.bdAddr;
				dev.bdAddr = pData->sBonded.bdAddr;
				IBTEXTRM_DeviceRead(pMe->mRM.po, &dev);

				if (dev.majorDevClass == AEEBT_COD_MAJ_DEV_CLS_AUDIO)
				{
					if (AEEBT_BD_ADDRS_EQUAL(&pMe->mAG.bdAddr,&dev.bdAddr) == FALSE)
					{
						BTApp_BuildMenu( pMe, BT_APP_MENU_USE_AUDIO_DEV);
					}
					else
					{
						BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, &pData->sBonded.bdAddr, 2);
					}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				}
				else
				{
					BTApp_ShowDevMsg( pMe, IDS_MSG_BONDED, &pData->sBonded.bdAddr, 2);
				}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
			}
			else
			{
				MSG_FATAL("***zzg BTApp RMNotify AEEBT_RM_EVT_BONDED pData->error=%d***", pData->sBonded.uError, 0, 0);
				BTApp_ShowDevMsg(pMe, IDS_MSG_BOND_FAILED, &pData->sBonded.bdAddr, 0);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
			}
			
			pMe->mRM.bUserCfm = FALSE;
			pMe->mRM.bBonding = FALSE;
#endif /* FEATURE_BT_2_1 */
			break;
		}
		
		
#ifdef FEATURE_BT_2_1
		case AEEBT_RM_EVT_REBOND_REQUEST:
		{
			MSG_FATAL("***zzg AEEBT_RM_EVT_REBOND_REQUEST***", 0, 0, 0);
			pMe->mRM.BondBDAddr = pData->bdAddr;

#if 1
			//BTApp_BuildMenu( pMe, BT_APP_MENU_REBOND );
			BTApp_BuildPrompt(pMe, BT_APP_MENU_REBOND);
#else
			//Add By zzg 2011_06_23
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{							
				pMe->m_app_flag = TRUE;
				pMe->m_user_wParam = evt;	
				ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
			}	
			else
			{
				BTApp_BuildMenu(pMe, BT_APP_MENU_REBOND);
			}
			//Add End	
#endif			
					
			break;
		}
		
	    case AEEBT_RM_EVT_PASSKEY_NOTIFICATION:
	    {
			MSG_FATAL(" RM - received Passkey Notification ", 0, 0, 0);
			pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr = pData->sPasskey.bdAddr;
			IBTEXTRM_DeviceRead(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx]);

			DBGPRINTF("***zzg AEEBT_RM_EVT_PASSKEY_NOTIFICATION passkey=%s***", pData->sPasskey.passkey);

			STRTOWSTR((pData->sPasskey.passkey), (pMe->mRM.wPassKey), (AEEBT_SSP_PASSKEY_LEN*sizeof( AECHAR )) ); 
			pMe->mRM.wDisplayPassKey[0] =(AECHAR) '\0';
			
#if 1
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
			                   AEECLSID_BLUETOOTH_APP,
			                   EVT_USER, EVT_PASSKY, 0L);   
#else

			//Add By zzg 2011_06_24
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{							
				pMe->m_app_flag = TRUE;
				pMe->m_user_wParam = evt;	
				ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
			}	
			else
			{
				ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
			                       AEECLSID_BLUETOOTH_APP,
			                       EVT_USER, EVT_PASSKY, 0L);   
			}
			//Add End	
#endif
			
			break;
	    }
		
#endif /* FEATURE_BT_2_1 */

		case AEEBT_RM_EVT_UNBONDED:        // response to Unbond()
		{		
			MSG_FATAL( "RM - Unbonded err=%d", pData->pDevUpdateStatus->error, 0, 0);
			if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE)
			{
				msgID = IDS_MSG_DEV_UNBONDED;
			}
			else
			{
				msgID = IDS_MSG_DEV_UNBOND_FAILED;
			}
			BTApp_ShowDevMsg(pMe, msgID, &pData->pDevUpdateStatus->bdAddr, 2);
			break;
		}
		
#ifdef FEATURE_BT_2_1
		case AEEBT_RM_EVT_USER_CFM_REQUEST:      
		{		
			DBGPRINTF("***zzg AEEBT_RM_EVT_USER_CFM_REQUEST passkey=%s***", pData->sPasskey.passkey);

			STRTOWSTR(pData->sPasskey.passkey, pMe->mRM.wSSPPassKey, AEEBT_SSP_PASSKEY_LEN*sizeof(AECHAR));
			pMe->mRM.bUserCfm = TRUE;
			pMe->mRM.BondBDAddr = pData->sPasskey.bdAddr;

#if 1
			BTApp_BuildPrompt(pMe , BT_APP_MENU_USER_CFM_RQST);	 
#else
			//Add By zzg 2011_06_23
			if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
			{							
				pMe->m_app_flag = TRUE;
				pMe->m_user_wParam = evt;	
				ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
			}	
			else
			{
				BTApp_BuildPrompt(pMe , BT_APP_MENU_USER_CFM_RQST);	 
			}
			//Add End	
#endif			 
			break;
		}
#endif /* FEATURE_BT_2_1 */

		case AEEBT_RM_EVT_BONDABLE_MODE:   // response to SetBondable()
		{
			MSG_FATAL("RM - SetBondable err=%d", pData->uError, 0, 0);
			
			if (pData->uError == AEEBT_RM_ERR_NONE)
			{
				pMe->mRM.bBondable = pMe->mRM.bBondable ? FALSE : TRUE;
				if (TOP_MENU == BT_APP_MENU_MASTER_CONTROL)
				{
				   BTApp_ShowMessage(pMe, pMe->mRM.bBondable ? IDS_MSG_BONDABLE_SET : IDS_MSG_BONDABLE_CLEARED, NULL, 2);
				}
			}
			else
			{
				msgID = pMe->mRM.bBondable ? IDS_MSG_BONDABLE_NOT_CLEARED : IDS_MSG_BONDABLE_NOT_SET;
				BTApp_ShowMessage(pMe, msgID, NULL, 2);
			}
			break;
		}
    
		case AEEBT_RM_EVT_PIN_REPLIED:     // response to PinReply()
		{
			if (pData->uError != AEEBT_RM_ERR_NONE)
			{
				/* 
				 * In that case the variable should be restored as no more 
				 * related events would come 
				 */
				pMe->mRM.bBonding = FALSE;

#if 1				
				BTApp_ShowMessage(pMe, IDS_MSG_PIN_REPLY_FAILED, NULL, 2);
#else
				//Add By zzg 2011_06_24
				if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
				{							
					pMe->m_app_flag = TRUE;
					pMe->m_user_wParam = evt;	
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);
				}	
				else
				{
					BTApp_ShowMessage(pMe, IDS_MSG_PIN_REPLY_FAILED, NULL, 2);
				}
				//Add End	
#endif				
			}
			break;
		}
	  
		case AEEBT_RM_EVT_DEVICE_ADDED:    // response to DeviceAdd()
			MSG_FATAL( "RM - DevAdded err=%d m=%d bonding=%d", pData->pDevUpdateStatus->error, TOP_MENU, pMe->mRM.bBonding );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

			MSG_FATAL("***zzg ProcessRMNotify AEEBT_RM_EVT_DEVICE_ADDED***", 0, 0, 0);

	  /*
	  if ((TOP_MENU == BT_APP_MENU_DEV_INFO) && (pMe->mRM.bBonding == FALSE) )
      {
        if ( pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE )
        {
			msgID = IDS_MSG_DEV_ADDED;
#ifdef FEATURE_APP_TEST_AUTOMATION
			#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        }
        else
        {
			msgID = IDS_MSG_DEV_ADD_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
			#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        }
        BTApp_ShowDevMsg( pMe, msgID, &pData->pDevUpdateStatus->bdAddr, 2 );
      }
	  */
	  		break;		//Add By zzg 2012_11_28

		/*
	  //Add By zzg 2011_01_11
	  if (pMe->mRM.bBonding == FALSE)
      {
		if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE )
		{		
			//Add by zzg 2012_11_22
			if ((pMe->bStartFromOtherApp == TRUE) || (pMe->bStartFromPushReq == TRUE)|| (pMe->bStartFromAudio == TRUE))
			{
				MSG_FATAL("***zzg show message box IDS_MSG_DEV_ADDED return***", 0, 0, 0);				
				break;
			}
			//Add End
			
			msgID = IDS_MSG_DEV_ADDED;
#ifdef FEATURE_APP_TEST_AUTOMATION
			#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
		}
		else
		{
			msgID = IDS_MSG_DEV_ADD_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
			#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
		}
		
		pMe->m_msg_id = msgID;		
		pMe->m_bNeedStr = TRUE;

		{
			AEEBTDeviceInfo dev;
			const AEEBTBDAddr	*pAddr = &pData->pDevUpdateStatus->bdAddr;
			dev.bdAddr = *pAddr;

			if ((IBTEXTRM_DeviceRead( pMe->mRM.po, &dev ) != SUCCESS) 
				 || (WSTRLEN(dev.wName) == 0) )
			{
				BTApp_BDAddr2Wstr(pMe->wMsgBuf, pAddr);
			}	
			else
			{
				MEMSET(pMe->wMsgBuf, 0, WSTRLEN(pMe->wMsgBuf)*sizeof(AECHAR));
				WSTRCPY(pMe->wMsgBuf, dev.wName);				
			}
		}
		
		//CLOSE_DIALOG(DLGRET_BT_MSGBOX)   
		pMe->m_eDlgRet = DLGRET_BT_MSGBOX; 
        (void) ISHELL_EndDialog(pMe->m_pShell); 
		

		
      }
	  //Add End	  
	  */
	  	
      break;
	  
		case AEEBT_RM_EVT_DEVICE_UPDATED:  // response to DeviceUpdate()
		{
			MSG_FATAL("RM - DevUpdated err=%d bm=0x%x", pData->pDevUpdateStatus->error,pData->pDevUpdateStatus->updateBitmap, 0);
			break;
		}
		
    case AEEBT_RM_EVT_DEVICE_REMOVED:  // response to DeviceRemove()
      MSG_FATAL( "RM - DevRem err=%d", pData->pDevUpdateStatus->error, 0, 0 );

	  /*
	  if ((TOP_MENU == BT_APP_MENU_REMOVE_ONE) 
	  	   || (TOP_MENU == BT_APP_MENU_REMOVE_ALL))
      {
        if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE)
        {
          msgID = IDS_MSG_DEV_REM;
          if (AEEBT_BD_ADDRS_EQUAL(&pMe->mAG.bdAddr, 
                                   &pData->pDevUpdateStatus->bdAddr))
          {
            pMe->mAG.bdAddr = NULL_BD_ADDR;
            pMe->bConfigChanged = TRUE;
            
          }
#ifdef FEATURE_BT_EXTPF_AV
          if (AEEBT_BD_ADDRS_EQUAL(&pMe->mA2DP.bdAddr, 
                                   &pData->pDevUpdateStatus->bdAddr))
          {
            pMe->mA2DP.bdAddr = NULL_BD_ADDR;
            pMe->bConfigChanged = TRUE;
            if( pMe->mA2DP.po != NULL )
            {
            IBTEXTA2DP_SetDevice( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
            }
          }
#endif
          if ( pMe->bConfigChanged != FALSE )
          {
            BTApp_WriteConfigFile( pMe );
          }
        }
        else
        {
          msgID = IDS_MSG_DEV_REM_FAILED;
        }
        BTApp_ShowDevMsg( pMe, msgID, &pData->pDevUpdateStatus->bdAddr, 2 );
        */
	   
		if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE)
		{
			pMe->m_msg_id= IDS_MSG_DEV_REM;
			pMe->m_bNeedStr = TRUE;
			
			if (AEEBT_BD_ADDRS_EQUAL(&pMe->mAG.bdAddr, 
			                         &pData->pDevUpdateStatus->bdAddr))
			{
				pMe->mAG.bdAddr = NULL_BD_ADDR;
				pMe->bConfigChanged = TRUE;
			}
#ifdef FEATURE_BT_EXTPF_AV
			if (AEEBT_BD_ADDRS_EQUAL(&pMe->mA2DP.bdAddr, 
			                         &pData->pDevUpdateStatus->bdAddr))
			{				
				MSG_FATAL("***zzg BTApp_RMnotify Set mA2DP.bdAddr NULL***", 0, 0, 0);
				pMe->mA2DP.bdAddr = NULL_BD_ADDR;
				pMe->bConfigChanged = TRUE;
				if( pMe->mA2DP.po != NULL )
				{
					IBTEXTA2DP_SetDevice(pMe->mA2DP.po, &pMe->mA2DP.bdAddr);
				}
			}
#endif
			if (pMe->bConfigChanged != FALSE)
			{
				BTApp_WriteConfigFile(pMe);
			}
		}
		else
		{
			pMe->m_msg_id= IDS_MSG_DEV_REM_FAILED;
			pMe->m_bNeedStr = TRUE;			
		}

		//BTApp_ShowDevMsg( pMe, msgID, &pData->pDevUpdateStatus->bdAddr, 2 );	
		{
			AEEBTDeviceInfo dev;
			const AEEBTBDAddr	*pAddr = &pData->pDevUpdateStatus->bdAddr;
			dev.bdAddr = *pAddr;

			if ((IBTEXTRM_DeviceRead( pMe->mRM.po, &dev ) != SUCCESS) 
				 || (WSTRLEN(dev.wName) == 0) )
			{
				BTApp_BDAddr2Wstr(pMe->wMsgBuf, pAddr);
			}	
			else
			{
				MEMSET(pMe->wMsgBuf, 0, WSTRLEN(pMe->wMsgBuf)*sizeof(AECHAR));
				WSTRCPY(pMe->wMsgBuf, dev.wName);	
			}
		}
		
		//CLOSE_DIALOG(DLGRET_BT_MSGBOX)   
		pMe->m_eDlgRet = DLGRET_BT_MSGBOX; 
        (void) ISHELL_EndDialog(pMe->m_pShell); 
     
      break;
	  
		case AEEBT_RM_EVT_LINK_STATUS_REG:
		{
			break;
		}
		
		case AEEBT_RM_EVT_LINK_STATUS:     // status info of existing BT link
		{
			AEEBTDeviceInfo dev;

			dev.bdAddr = pData->pLinkStatus->bdAddr;
			IBTEXTRM_DeviceRead(pMe->mRM.po, &dev);
			
			if (dev.uValue2 != pData->pLinkStatus->linkMode)
			{
				MSG_FATAL("RM - Link mode change for %04x%04x lm=%d",
				         ((uint16)(pData->pLinkStatus->bdAddr.uAddr[3] << 8) | 
				          pData->pLinkStatus->bdAddr.uAddr[2]), 
				         ((uint16)(pData->pLinkStatus->bdAddr.uAddr[1] << 8) | 
				          pData->pLinkStatus->bdAddr.uAddr[0]),
				         pData->pLinkStatus->linkMode);
			}
			
			if (pMe->mRM.linkStatus->bMaster != pData->pLinkStatus->bMaster)
			{
				MSG_FATAL("RM - dev role chgd=%d", pData->pLinkStatus->bMaster, 0, 0);
			}
			
			MEMCPY(pMe->mRM.linkStatus, pData->pLinkStatus, sizeof(AEEBTLinkStatus));
			break;
		}
		
		case AEEBT_RM_EVT_DEVICE_SECURITY_SET:
		{
			MSG_FATAL( "RM - DevSecSet err=%d", pData->pDevUpdateStatus->error, 0, 0);
			
			if (pData->pDevUpdateStatus->error == AEEBT_RM_ERR_NONE)
			{
				msgID = EVT_RM_DEV_SEC_SET;
			}
			else
			{
				msgID = EVT_RM_DEV_SEC_FAILED;
			}
			
			ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
			                  AEECLSID_BLUETOOTH_APP,
			                  EVT_USER, 
			                  msgID, 0L);
			break;
		}
		
		case AEEBT_RM_EVT_SERVICE_SECURITY_SET:
		{
			//uint16 selection;
			boolean SecEnabled;

			MSG_FATAL( "RM - SvcSecSet err=%d", pData->uError, 0, 0 );

#ifdef FEATURE_BT_2_1
			//if (TOP_MENU != BT_APP_MENU_SECURITY_OPTIONS )
#endif
			{
				//selection = IMENUCTL_GetSel( pMe->m_pIMenu );

				if( pMe->mL2.dwSettingSec > 0 )
				{
					SecEnabled = pMe->mL2.bEnableSrvSvcSec;
					pMe->mL2.dwSettingSec--;
				}
				else
				{
					SecEnabled = pMe->mNA.bEnableSvcSec;
				}

				/*
				if( TOP_MENU != BT_APP_MENU_MAIN )
				{
				  BTApp_UpdateMenuItemImage( 
				  pMe->m_pIDisplay, pMe->m_pIMenu, selection,
				  SecEnabled ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
				}
				*/
			}
			break;
		}
		
		case AEEBT_RM_EVT_AUTHORIZE_REQUEST:
		{
			MSG_FATAL("RM - AuthReq sim=%d", pData->pAuthorizeReq->serviceId.serviceIdMethod, 0, 0);
			
			if ((pData->pAuthorizeReq->serviceId.serviceIdMethod == pMe->mRM.svcId.serviceIdMethod) &&
			    (pData->pAuthorizeReq->serviceId.id.uPSM == pMe->mRM.svcId.id.uPSM))
			{
				pMe->mRM.device[pMe->mRM.uCurDevIdx].bdAddr = pData->pAuthorizeReq->bdAddr;
				IBTEXTRM_DeviceRead(pMe->mRM.po, &pMe->mRM.device[pMe->mRM.uCurDevIdx]);
				ISHELL_PostEventEx(pMe->m_pShell, EVTFLG_ASYNC, 
				                   AEECLSID_BLUETOOTH_APP,
				                   EVT_USER, EVT_RM_AUTH_REQ, 0L);
			}
			else
			{
				IBTEXTRM_AuthorizeReply(pMe->mRM.po, &pData->pAuthorizeReq->serviceId,
				                        &pData->pAuthorizeReq->bdAddr, FALSE);
			}
			
			break;
		}
		
		case AEEBT_RM_EVT_AUTHORIZE_REPLIED:
		{
			BTApp_HandleClearKey(pMe);
			break;
		}
		
#ifdef FEATURE_BT_2_1
		case AEEBT_RM_EVT_USER_CFM_REPLIED:
		{
			if (pData->uError != AEEBT_RM_ERR_NONE)
			{
				MSG_FATAL("***zzg RMNotify uError=%d***", pData->uError, 0 , 0);

				//BTApp_ShowMessage( pMe, IDS_MSG_USER_CFM_FAILED, NULL, 2 );

				pMe->m_msg_id = IDS_MSG_USER_CFM_FAILED;	
				pMe->m_bNeedStr = FALSE;

				//CLOSE_DIALOG(DLGRET_BT_MSGBOX)   
				pMe->m_eDlgRet = DLGRET_BT_MSGBOX; 
				(void) ISHELL_EndDialog(pMe->m_pShell); 

			}
			else
			{
				BTApp_HandleClearKey(pMe);
			}
			break;
		}
		
#endif /* FEATURE_BT_2_1 */

		case AEEBT_RM_EVT_TEST_MODE_ENTERED:
		{
			MSG_FATAL( "RM - DUTEntered err=%d", pData->uError, 0, 0 );
			BTApp_ShowMessage( pMe, IDS_MSG_TEST_MODE, NULL, 0 );
			break;
		}
		
		case AEEBT_RM_EVT_TEST_MODE_EXITED:
		{
			MSG_FATAL( "RM - DUTExited err=%d", pData->uError, 0, 0 );
			BTApp_HandleClearKey( pMe );
			break;
		}
		
		case AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED:
		{
			MSG_FATAL( "RM - Auto Svc Srch Disabled err=%d", pData->uError, 0, 0 );
			break;
		}
		
		case AEEBT_RM_EVT_ROLE_SW_CTRL_SET:
		{
			if (pData->uError != AEEBT_RM_ERR_NONE)
			{
				msgID = IDS_MSG_ROLE_SW_CTRL_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
			}
			else
			{
				pMe->mRM.bRoleSwitchAllowed = !pMe->mRM.bRoleSwitchAllowed;
				msgID = IDS_MSG_ROLE_SW_CTRL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
			}
			
			MSG_FATAL("RoleSwEvt : RoleSwAlw=%d", pMe->mRM.bRoleSwitchAllowed, 0, 0);
			BTApp_ShowMessage(pMe, msgID, NULL, 3);
			break;
		}
		
		case AEEBT_RM_EVT_RADIO_STATUS:
		{
			AECHAR wShortText[5];
			STRTOWSTR(pMe->mRM.bRadioOn ? "OFF" : "ON", wShortText, sizeof(wShortText));
			MSG_FATAL("RM - Radio Status stat=%d", pData->uStatus, 0, 0);
			
			if (pMe->mRM.bRadioOn != pData->uStatus)
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				pMe->mRM.bRadioOn = pData->uStatus;
				BTApp_ShowMessage(pMe, IDS_MSG_RADIO_CTRL_SUCCESS, wShortText, 3);
			}
			else
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
				BTApp_ShowMessage(pMe, IDS_MSG_RADIO_CTRL_FAILED, wShortText, 3);
			}
			break;
		}
		
		case AEEBT_RM_EVT_VISIBILITY_STATUS:
		{
			AECHAR wShortText[5];
			STRTOWSTR(pMe->mRM.bVisibilityOn ? "OFF" : "ON", wShortText, sizeof(wShortText));
			MSG_FATAL("RM - Radio Status stat=%d", pData->uStatus, 0, 0);
			
			if (pMe->mRM.bVisibilityOn != pData->uStatus)
			{
				pMe->mRM.bVisibilityOn = pData->uStatus;				
				BTApp_ShowMessage(pMe, IDS_MSG_VISIBILITY_CTRL_SUCCESS, wShortText, 3);
			}
			else
			{
				BTApp_ShowMessage(pMe, IDS_MSG_VISIBILITY_CTRL_FAILED, wShortText, 3);
			}
			break;
		}
		
		case AEEBT_RM_EVT_CONN:
		{
			AEEBTDeviceInfo dev;

			MSG_HIGH("RM - Connected to %04x%04x%04x", 
			        ((uint16)(pData->bdAddr.uAddr[5] << 8) | pData->bdAddr.uAddr[4]), 
			        ((uint16)(pData->bdAddr.uAddr[3] << 8) | pData->bdAddr.uAddr[2]), 
			        ((uint16)(pData->bdAddr.uAddr[1] << 8) | pData->bdAddr.uAddr[0]));
			
			IBTEXTRM_RegisterLinkStatus(pMe->mRM.po, &pData->bdAddr, 0);

			// check if the device is already in the database
			dev.bdAddr = pData->bdAddr;
#ifdef FEATURE_BT_2_1
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* FEATURE_BT_2_1 */
			if (IBTEXTRM_DeviceRead(pMe->mRM.po, &dev) == SUCCESS)
			{
				if (dev.bValid == FALSE)
				{
					IBTEXTRM_DeviceAdd(pMe->mRM.po, &dev);
				}
				dev.uValue2 = AEEBT_LM_ACTIVE;
				IBTEXTRM_DeviceUpdate( pMe->mRM.po, AEEBT_RM_DU_VALUE_2_B, &dev);
			}
			else
			{
				MSG_FATAL("RM - DB entry missing!", 0, 0, 0);
			}	  	

			break;
		}
		
		case AEEBT_RM_EVT_DISCONN:
		{
			MSG_HIGH("RM - Disconnected from %04x%04x%04x", 
			        ((uint16)(pData->bdAddr.uAddr[5] << 8) | pData->bdAddr.uAddr[4]), 
			        ((uint16)(pData->bdAddr.uAddr[3] << 8) | pData->bdAddr.uAddr[2]), 
			        ((uint16)(pData->bdAddr.uAddr[1] << 8) | pData->bdAddr.uAddr[0]));
			// unregistering link status here would result in error because connection
			// no longer exists; RM automatically unreg link status when ACL is down
			break;
		}
		
		case AEEBT_RM_EVT_CONN_ROLE_SET:
		{
			MSG_FATAL("RM - Conn Role Set err=%d", pData->uError, 0, 0);
			
			if (pData->uError != AEEBT_RM_ERR_NONE)
			{
				BTApp_ShowMessage(pMe, IDS_MSG_SET_CONN_ROLE_FAILED, NULL, 0);
			}
			else
			{
				BTApp_ShowMessage(pMe, IDS_MSG_SET_CONN_ROLE_SUCCESS, NULL, 2);
			}
			break;
		}
		
		case AEEBT_RM_EVT_APPLICATION_CONFIGURED:
		{
			MSG_FATAL("RM - App Config done, err=%d", pData->uError, 0, 0);
			break;
		}
#ifdef FEATURE_BT_2_1
		case AEEBT_RM_EVT_KEYPRESS_NOTIFICATION_REPLIED:
		{
			MSG_FATAL("RM - KPNotif Sent, err=%d", pData->uError, 0, 0);
			break;
		}
		case AEEBT_RM_EVT_LPM_CONFIGURED:
		{
			MSG_FATAL("RM - Low Pwr Mode Configured, err=%d", pData->uError, 0, 0);
			break;
		}
#endif /* FEATURE_BT_2_1 */

		default:
		MSG_FATAL("RM - unexpected ev=%d", evt, 0, 0);
  }
}


/* ==========================================================================
FUNCTION BTApp_ProcessSPPNotifications
DESCRIPTION
============================================================================= */
static void BTApp_ProcessSPPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint16 userEvent;

  switch ( evt )
  {
    case AEEBT_SPP_EVT_OPENED:
      MSG_FATAL( "SPP - Opened err=%d", pData->pSppStatus->uReason, 0, 0 );
      pMe->mSPP.status = *pData->pSppStatus;
      if ( pData->pSppStatus->uReason == AEEBT_SPP_ERR_NONE )
      {
        userEvent = EVT_SPP_OPENED;
      }
      else
      {
        userEvent = EVT_SPP_OPEN_FAILED;
      }
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, userEvent, 0L);
      break;
    case AEEBT_SPP_EVT_CLOSED:
      MSG_FATAL( "SPP - Closed", 0, 0, 0 );
      pMe->mSPP.status = *pData->pSppStatus;
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_SPP_CLOSED, 0L );
      break;
    case AEEBT_SPP_EVT_CONNECTED:
      MSG_HIGH( "SPP - Connected", 0, 0, 0 );
      //clearing any unread data from the mSPP buffer 
      SPP_CLEAR_BUF();      
      pMe->mSPP.status = *pData->pSppStatus;
      if ( pMe->mSPP.bClient )
      {
        ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND, 
                         (PFNNOTIFY) BTApp_SendSPPData, pMe );
      }
      CALLBACK_Init( &pMe->mSPP.readableCb, BTApp_RetrieveSPPData, pMe );
      IBTEXTSPP_Readable( pMe->mSPP.po, &pMe->mSPP.readableCb );
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_SPP_CONNECTED, 0L );
      break;
    case AEEBT_SPP_EVT_DISCONNECTED:
      MSG_HIGH( "SPP - Disconnected", 0, 0, 0 );
      pMe->mSPP.status = *pData->pSppStatus;
      ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                          AEECLSID_BLUETOOTH_APP,
                          EVT_USER, EVT_SPP_DISCONNECTED, 0L );
      break;
    case AEEBT_SPP_EVT_CONFIG_CHANGED:
      break;
    case AEEBT_SPP_EVT_MODEM_STATUS:
      break;
    case AEEBT_SPP_EVT_LINE_ERROR:
      break;
    case AEEBT_SPP_EVT_DTR_CHANGED:
      break;
    case AEEBT_SPP_EVT_TX_FLUSHED:
      break;
    default:
      MSG_FATAL( "SPP - unexpected ev=%d", evt, 0, 0 );
  }
}

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_BT_HFP_1_5
/* ==========================================================================
FUNCTION BTApp_ProcessBattEvents
DESCRIPTION
============================================================================= */
static boolean BTApp_ProcessBattEvents( CBTApp* pMe, uint16  wParam, 
                                        uint32  dwParam )
{
  AEENotify *pNotifyInfo = (AEENotify*) dwParam;

  /* process the events only when AG is enabled */
  if ( pMe->mAG.bEnabled == FALSE )
  {
    return FALSE;
  }
  MSG_FATAL( "Received battery event=%d", pNotifyInfo->dwMask, 0, 0 ); 
  switch ( pNotifyInfo->dwMask )
  {
    case NMASK_BATTERY_LEVEL_CHANGE:
    {
      uint8 uLevel;
      if ( !( BTApp_GetBatteryLevel(pMe, &uLevel) ) || 
           (IBTEXTAG_SetBatteryChargeIndicator( pMe->mAG.po, 
             (AEEBTBatteryChargeLevel)uLevel )) != SUCCESS )
      {
        return FALSE;
      }
      break;
    }
    default:
      break;
  }
  return TRUE;
}
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif //FEATURE_BT_EXTPF_SAP
#ifdef FEATURE_BT_HFP_1_5
/* ==========================================================================
FUNCTION BTApp_ReportRSSIChange
DESCRIPTION
============================================================================= */
static void BTApp_ReportRSSIChange( CBTApp* pMe )
{
  AEEBTRSSI    rssiValue;
  uint16       rssiDB;

  /* check if we are in not in HDR mode */
  if ( (pMe->mAG.pSSInfo->sys_mode != AEET_SYS_MODE_HDR) &&
       ((pMe->mAG.pSSInfo->srv_status == AEET_SRV_STATUS_SRV) ||
       (pMe->mAG.pSSInfo->srv_status == AEET_SRV_STATUS_LIMITED)) )
  {
    rssiDB = pMe->mAG.pSSInfo->rssi;
    if ( rssiDB < 60 )
    { 
      rssiValue = AEEBT_AG_RSSI_LEVEL_5;
    }
    else if ( rssiDB < 75 )
    {
      rssiValue = AEEBT_AG_RSSI_LEVEL_4;
    }
    else if ( rssiDB < 90 )
    {
      rssiValue = AEEBT_AG_RSSI_LEVEL_3;
    }
    else if ( rssiDB < 105 )
    {
      rssiValue = AEEBT_AG_RSSI_LEVEL_2;
    }
    else if ( rssiDB < 120 )
    {
      rssiValue = AEEBT_AG_RSSI_LEVEL_1;
    }
    else
    {
      rssiValue = AEEBT_AG_RSSI_LEVEL_0;
    }
  }
  else
  {
    MSG_FATAL( "ReportRSSIChange - sys=%d srv=%d", 
             pMe->mAG.pSSInfo->sys_mode, pMe->mAG.pSSInfo->srv_status, 0 );
    return;
  }
  if ( IBTEXTAG_SetRSSI( pMe->mAG.po, rssiValue ) != SUCCESS )
  {
    MSG_FATAL( "ReportRSSIChange - error in setting RSSI", 0, 0, 0 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_ProcessTPSSEvents
DESCRIPTION
============================================================================= */
static boolean BTApp_ProcessTPSSEvents( CBTApp* pMe, uint16  wParam, 
                                        uint32  dwParam )
{
  boolean bEventProcessed = FALSE;
  AEETNotifyInfo* pEv = (((AEENotify*) dwParam)->pData);  

  /* process the events only when AG is enabled */
  if ( pMe->mAG.bEnabled == FALSE )
  {
    return FALSE;
  }
  MSG_FATAL( "AG - TP received SS event=%d", pEv->event, 0, 0 ); 
  /* copy the SS info to pMe structure */
  if ( (&pEv->event_data.ss != NULL) && (pMe->mAG.pSSInfo != NULL) )
  {
    MEMCPY( pMe->mAG.pSSInfo, (AEETSSInfo*)&(pEv->event_data.ss.ss_info),
            sizeof(AEETSSInfo) );
    pMe->mAG.bValidSSData = TRUE;
  }
  else
  {
    MSG_FATAL( "ProcessTPSSEvents - Invalid SS data in NotifyInfo", 0, 0, 0 );
    return FALSE;
  }
  switch ( pEv->event )
  {
    case AEET_EVENT_SS_SRV_CHANGED: /* Serving system information changed */
    {
      MSG_FATAL( "Service Status = %d", pMe->mAG.pSSInfo->srv_status, 0, 0 );
      /* check for status chnages */
      switch ( pMe->mAG.pSSInfo->srv_status )
      {
        case AEET_SRV_STATUS_SRV:
        {
          BTApp_ReportRSSIChange( pMe );
          break;
        }
        default:
          break;
      }
      return TRUE;
    }
    case AEET_EVENT_SS_RSSI: /* RSSI changed */
    {
      MSG_FATAL( "Received RSSI change event", 0, 0, 0 );
      BTApp_ReportRSSIChange( pMe );
      break;
    }
    default:
      break;
  }
  return bEventProcessed;
}
#endif  //FEATURE_BT_HFP_1_5

/* ==========================================================================
FUNCTION BTApp_CheckNeedForSCO
DESCRIPTION
  Check if we shall bring down SCO when receive snd_chg_evt indicating
  inband ringtone/media playback has been stopped.
============================================================================= */
static void BTApp_CheckNeedForSCO( CBTApp* pMe )
{
#ifdef FEATURE_AVS_BT_SCO_REWORK
  if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_NON_VOICE_AUDIO_ON_B ) )
  {
    /* NOT to bring down SCO when receiving snd_chg_evt
    ** that inband ringtone/media playback has been stopped.
    */
    BTAPP_AG_SETBIT( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B );
  }
#endif
}

/* ==========================================================================
FUNCTION BTApp_HandleCallOrig
DESCRIPTION
  Called when AEET_EVENT_CALL_ORIG or AEEVP_EVENT_VIDEO_CALL_ORIG is received
============================================================================= */
static void BTApp_HandleCallOrig( CBTApp* pMe )
{
  pMe->mAG.uNumCalls++; // another call to keep track of

  MSG_FATAL("***zzg BTApp_HandleCallOrig A2DP bConnected=%x***", pMe->mA2DP.bConnected, 0, 0);
  MSG_FATAL("***zzg BTApp_HandleCallOrig AG bConnected=%x, bDevDialed=%x***", 
  				pMe->mAG.bConnected, pMe->mAG.bDevDialed, 0);

	//Add By zzg 2011_11_02
	/*
	if (pMe->mA2DP.bConnected != FALSE)
	{
		BTApp_ReleaseA2DPDevice( pMe );
	} 
	
	if (pMe->mA2DP.bConnected != FALSE)
	{
		pMe->mAG.bdAddr = pMe->mA2DP.bdAddr;			

		if (pMe->mAG.bConnected == FALSE)
		{
			if ( pMe->mAG.bconnInPrgs == FALSE )
			{			
				if (BTApp_CallConnected(pMe) != BT_APP_CALL_NONE)
				{
					pMe->mAG.bDevPickedUp = TRUE; // signal self to send audio to HS/HF
				}
				pMe->mAG.bconnInPrgs = TRUE;
				IBTEXTAG_Connect( pMe->mAG.po, &pMe->mAG.bdAddr, pMe->mAG.devType);			
			}	
		}
	}
	*/
	//Add End

	 MSG_FATAL("***zzg BTApp_HandleCallOrig AG devType=%x, bPrivateMode=%x, uNumCalls=%x***", 
	 			pMe->mAG.devType, pMe->mAG.bPrivateMode, pMe->mAG.uNumCalls);
	
  if ( IS_AG_CONNECTABLE() != FALSE ) 
  {
    if ( pMe->mAG.bDevDialed == FALSE ) 
    {
      /* headset or HF private mode, don't transfer audio to HS/HF */
      /* if there's more than 1 call, don't bring down audio link
         that might already be up for the call on hold
      */
      //Del by zzg 2011_11_10
      /*
      if ( (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET) ||
           ((pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HANDSFREE) &&
            (pMe->mAG.bPrivateMode != FALSE)) &&
           (pMe->mAG.uNumCalls == 1) ) // not multiparty scenario?
      {
#ifdef FEATURE_AVS_BT_SCO_REWORK
        BTApp_ReleaseBTDevice( pMe, FALSE );
#endif 
        BTApp_DisconnectAudio( pMe, FALSE );
      }
      else
	  */	
	  //Del End
      {
        BTApp_CheckNeedForSCO( pMe );
        BTApp_ConnectAudio( pMe, TRUE );
      }
    }
    else
    {
      /* orig by HS/HF, audio is already connected, making sure */
      BTApp_CheckNeedForSCO( pMe );
      BTApp_ConnectAudio( pMe, TRUE );
    }

    /* resetting flags */
    pMe->mAG.bDevDialed = FALSE;
    pMe->mAG.callState = BTAPP_AG_CALL_ORGINATED;
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleCallIncom
DESCRIPTION
  Called when AEET_EVENT_CALL_INCOM or AEEVP_EVENT_VIDEO_CALL_INCOM
  is received.
============================================================================= */
static void BTApp_HandleCallIncom( CBTApp* pMe )
{
  
 /* Releasing the availability of A2dp device to prevent the AVS from  
  * sending the ring on a2dp media during the incoming call 
  */
  MSG_FATAL("***zzg BTApp_HandleCallIncom A2DP_Connected=%x AG_Connected=%x***", 
  			pMe->mA2DP.bConnected, pMe->mAG.bConnected, 0);
 
  if (pMe->mA2DP.bConnected != FALSE)
  {
    BTApp_ReleaseA2DPDevice( pMe );
  } 

	/*
	//Add By zzg 2011_11_02
	if (pMe->mA2DP.bConnected != FALSE)
	{
		if (FALSE == pMe->mAG.bConnected)
		{
			pMe->mAG.bdAddr = pMe->mA2DP.bdAddr;			
				
			if ( pMe->mAG.bconnInPrgs == FALSE )
			{			
				if (BTApp_CallConnected(pMe) != BT_APP_CALL_NONE)
				{
					pMe->mAG.bDevPickedUp = TRUE; // signal self to send audio to HS/HF
				}
				pMe->mAG.bconnInPrgs = TRUE;
				IBTEXTAG_Connect( pMe->mAG.po, &pMe->mAG.bdAddr, pMe->mAG.devType);			
			}		
		}
	}
	//Add End
	*/
  
  if ( IS_AG_CONNECTABLE() != FALSE )
  {
#ifndef FEATURE_AVS_BT_SCO_REWORK
    BTApp_HandleMTCall( pMe );
#endif
    BTApp_CheckAutoAnswer( pMe );
  }
  else
  {
    MSG_FATAL( "HandleCallIncom - AG inactive state", 0, 0, 0 );
  }
  pMe->mAG.uNumCalls++;
}

/* ==========================================================================
FUNCTION BTApp_HandleCallAnswer
DESCRIPTION
  Called when AEET_EVENT_CALL_ANSWER or AEEVP_EVENT_VIDEO_CALL_CONNECT
  is received.
============================================================================= */
static void BTApp_HandleCallAnswer( CBTApp* pMe )
{
	MSG_FATAL("***zzg BTApp_HandleCallAnswer bDevPickedUp=%x, devType=%x, bPrivateMode=%x***",
				pMe->mAG.bDevPickedUp, pMe->mAG.devType, pMe->mAG.bPrivateMode);

	MSG_FATAL("***zzg BTApp_HandleCallAnswer AG bEnabled=%x, bConnected=%x***",
					pMe->mAG.bEnabled, pMe->mAG.bConnected, 0);

	pMe->mAG.bDevPickedUp = TRUE;	//Add By zzg 2011_10_29
	
  if ( IS_AG_CONNECTABLE() != FALSE )
  {
    BTApp_StopRing( pMe );
    if ( pMe->mAG.bDevPickedUp == FALSE )
    {
      /* headset or private mode, don't transfer audio to HS/HF */
      if ( (pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HEADSET) ||
           ((pMe->mAG.devType == AEEBT_AG_AUDIO_DEVICE_HANDSFREE) &&
            (pMe->mAG.bPrivateMode != FALSE)) )
      {
#ifdef FEATURE_AVS_BT_SCO_REWORK
        BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* FEATURE_AVS_BT_SCO_REWORK */
        BTApp_DisconnectAudio( pMe, TRUE );
      }
      else
      {
        BTApp_CheckNeedForSCO( pMe );
        BTApp_ConnectAudio( pMe, TRUE );
      }
    }
    else 
    {
      BTApp_CheckNeedForSCO( pMe );
      // call has been answered on HS, so transfer audio to HS
      BTApp_ConnectAudio( pMe, TRUE );
    }
    pMe->mAG.callState    = BTAPP_AG_CALL_ANSWERED;
    pMe->mAG.bDevPickedUp = FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_HandleCallEnd
DESCRIPTION
  Called when AEET_EVENT_CALL_END or AEEVP_EVENT_VIDEO_CALL_END is received.
============================================================================= */
static void BTApp_HandleCallEnd( CBTApp* pMe )
{
  pMe->mAG.uNumCalls--; // one fewer call to keep track of

  MSG_FATAL("***zzg BTApp_HandleCallEnd***", 0, 0, 0);

  MSG_FATAL("***zzg BTApp_HandleCallEnd AG bEnabled=%x, bConnected=%x, callState=%x***", 
  				pMe->mAG.bEnabled, pMe->mAG.bConnected, pMe->mAG.callState);
  MSG_FATAL("***zzg BTApp_HandleCallEnd A2DP bEnabled=%x, bConnected=%x***", 
  				pMe->mA2DP.bEnabled, pMe->mA2DP.bConnected, 0);
  
  if ( pMe->mAG.bEnabled != FALSE )
  {
    pMe->mAG.bDevDialed = FALSE;
    if ( (pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_INBAND) ||
         (pMe->mAG.callState == BTAPP_AG_CALL_INCOMING_NOINBAND) )
    {
      BTApp_StopRing( pMe );
      pMe->mAG.bDevPickedUp = FALSE;
    }
    if ( pMe->mAG.uNumCalls == 0 )
    {
#ifdef FEATURE_AVS_BT_SCO_REWORK
      if ( (pMe->mAG.bSLCUp != FALSE) &&
           (pMe->mA2DP.bAudioSelected == FALSE) )
      {
        /* in case audio transfer happened during the call session */
        BTApp_UseBTDevice( pMe, FALSE );
      }

      if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_AUDIO_WANTED_B ) )
      {
        BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_AUDIO_WANTED_B );

        uisnd_notify_unregister( 
          (UISND_NOTIFY_FUNC)BTApp_DeviceChangeCallBack,
          (void*)pMe );
      }

      if ( BTAPP_AG_ISBITSET( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B ) )
      {
        BTAPP_AG_CLRBIT( BTAPP_AG_FLAG_SCO_4_VOICE_WANTED_B );
        /* SCO may be up and used for media playbacks, so leave it as it is */
      }
      else /* else, SCO is NOT used for media playbacks;
           ** safe to disconnect SCO.
           */
#endif /* FEATURE_AVS_BT_SCO_REWORK */
      {
        BTApp_DisconnectAudio( pMe, FALSE );
      }
      pMe->mAG.callState = BTAPP_AG_CALL_NONE;
    }
  }
  /* Using the a2dp device again if it is not already set once the call ends */
  if (pMe->mA2DP.bConnected != FALSE)
  {
    BTApp_UseA2DPDevice( pMe );
  }

}

/* ==========================================================================
FUNCTION BTApp_ProcessTPCallEvents
DESCRIPTION
============================================================================= */
static boolean BTApp_ProcessTPCallEvents( CBTApp* pMe, uint16  wParam, 
                                           uint32  dwParam )
{
  boolean bEventProcessed = FALSE;
  AEETNotifyInfo* pEv = (((AEENotify*) dwParam)->pData);
  MSG_FATAL("***zzg BTApp_ProcessTPCallEvents event=%x***", pEv->event, 0, 0);
  switch ( pEv->event )
  {
    case AEET_EVENT_CALL_ORIG:
    {
      MSG_FATAL( "AG - TP ev CALL_ORIG calls=%d devD=%d", 
               pMe->mAG.uNumCalls, pMe->mAG.bDevDialed, 0 );
      BTApp_HandleCallOrig( pMe );
      break;
    }
    case AEET_EVENT_CALL_INCOM:
    {
      MSG_FATAL( "AG - TP ev CALL_INCOM calls=%d", pMe->mAG.uNumCalls, 0, 0 );
      ICALLMGR_GetCall( pMe->mAG.pICallMgr, pEv->event_data.call.cd, 
                        &pMe->mAG.pIncomingCall );
      BTApp_HandleCallIncom( pMe );
      return TRUE;
    }
    case AEET_EVENT_CALL_ANSWER:
    {
      MSG_FATAL( "AG - TP ev CALL_ANSWER dpu=%d", pMe->mAG.bDevPickedUp, 0, 0 );
      BTApp_HandleCallAnswer( pMe );
      return TRUE;
    }
    case AEET_EVENT_CALL_END:
    {
      MSG_FATAL( "AG - TP ev CALL_END calls=%d", pMe->mAG.uNumCalls, 0, 0 );
      BTApp_HandleCallEnd( pMe );
      break;
    }
    case AEET_EVENT_CALL_CONNECT:
    {
      MSG_FATAL( "AG - TP ev CONNECT", 0, 0, 0 );
      if ( pMe->mAG.bEnabled != FALSE )
      {
        pMe->mAG.callState = BTAPP_AG_CALL_CONNECTED;
      }
      break;
    }
    case AEET_EVENT_CALL_OPS:
    {
      MSG_FATAL( "AG - TP ev FLASHED", 0, 0, 0 );
      break;
    }
    case AEET_EVENT_CALL_SIGNAL:
    {
      MSG_FATAL( "AG - TP ev SIGNAL", 0, 0, 0 );
      break;
    }
    case AEET_EVENT_CALL_PROGRESS_INFO_IND:
    {
      MSG_FATAL( "AG - TP ev REMOTE_PARTY_REACHED", 0, 0, 0 );
      break;
    }
    /* case AEECM_EVENT_CALL_ALL_CALLS_ENDED:
    {
      MSG_FATAL( "AG - CM ev ALL_CALLS_ENDED calls=%d", 
               pMe->mAG.uNumCalls, 0, 0 );
      pMe->mAG.uNumCalls = 0;
      break;
    }*/
    default:
    {
      MSG_FATAL( "AG - TP event %x", pEv->event, 0, 0 );
    }
  }

  return bEventProcessed;
}

/* ==========================================================================
FUNCTION BTApp_ProcessTELEPHONEEvents
DESCRIPTION
============================================================================= */
static boolean BTApp_ProcessTELEPHONEEvents( 
  CBTApp* pMe, 
  uint16  wParam, 
  uint32  dwParam )
{
  boolean bEventProcessed = FALSE;
  AEENotify* pNotify = (AEENotify*)dwParam;

  MSG_FATAL("***zzg BTApp_ProcessTELEPHONEEvents ***", 0, 0, 0);

  if ( pNotify->cls != AEECLSID_PHONENOTIFIER )
  {
    return TRUE;
  }
  if ( pNotify->dwMask & AEET_NMASK_VOICE_CALL ) 
  {
    return BTApp_ProcessTPCallEvents( pMe, wParam, dwParam );
  }
#ifdef FEATURE_BT_HFP_1_5
  else if ( pNotify->dwMask & AEET_NMASK_SS )
  {
    return BTApp_ProcessTPSSEvents( pMe, wParam, dwParam );
  }
#endif
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
  return bEventProcessed;
}

#ifdef FEATURE_BT_VT
/* ==========================================================================
FUNCTION BTApp_ProcessVTEvents
DESCRIPTION
============================================================================= */
static boolean BTApp_ProcessVTEvents( 
  CBTApp* pMe, 
  uint16  wParam, 
  uint32  dwParam )
{
  boolean bEventProcessed = FALSE;
  AEEVPNotifyInfo* pNotify = (AEEVPNotifyInfo*) ((AEENotify*)dwParam)->pData;

  if (((AEENotify*) dwParam)->cls != AEECLSID_VIDEOPHONE_NOTIFIER)
  {
    return TRUE;
  }

  MSG_FATAL("***zzg BTApp_ProcessVTEvents event=%x***", pNotify->event, 0, 0);

  switch ( pNotify->event )
  {
    case AEEVP_EVENT_VIDEO_CALL_INCOM:
    {
      MSG_FATAL( "AG - VT ev CALL_INCOM", 0, 0, 0 );
      BTApp_HandleCallIncom( pMe );
      return TRUE;
    }
    case AEEVP_EVENT_VIDEO_CALL_CONNECT:
    {
      MSG_FATAL( "AG - VT ev CALL_CONNECT", 0, 0, 0 );
      if ( pMe->mAG.callState != BTAPP_AG_CALL_ORGINATED )
      {
        BTApp_HandleCallAnswer( pMe );
      }
      // nothing else to do for MO call
      pMe->mAG.callState = BTAPP_AG_CALL_CONNECTED;
      return TRUE;
    }
    case AEEVP_EVENT_VIDEO_CALL_SETUP:
    {
      // the case in which there is an Incoming Video Call and the UE
      // answers it. Headset needs to stop ringing.
      MSG_FATAL( "AG - VT ev CALL_SETUP", 0, 0, 0 );
      BTApp_StopRing( pMe );
      return TRUE;
    }
    case AEEVP_EVENT_VIDEO_CALL_END:
    {
      // the case in which Video call setup fails, HS needs to stop rining.
      MSG_FATAL( "AG - VT ev CALL_END calls=%d", pMe->mAG.uNumCalls, 0, 0 );
      BTApp_HandleCallEnd( pMe );
      return TRUE;
    }
    case AEEVP_EVENT_VIDEO_CALL_ORIG:
    {
      MSG_FATAL( "AG - VT ev CALL_ORIG calls=%d devD=%d", 
               pMe->mAG.uNumCalls, pMe->mAG.bDevDialed, 0 );
      BTApp_HandleCallOrig( pMe );
      break;
    }
    default:
    {
      MSG_FATAL( "AG - VT event %x ignored", pNotify->event, 0, 0 );
      break;
    }
  }

  return bEventProcessed;
}
#endif /* FEATURE_BT_VT */

#ifndef UI_UISND_NO_DIRECT_STATUS_CALLS
/*****************************************************************************/
/*****                       Exported functions.                         *****/          
/*****************************************************************************/

/* ==========================================================================
FUNCTION BTAGEnabled
DESCRIPTION
============================================================================= */
boolean BTAGEnabled( void )
{
  boolean AGIsEnabled = FALSE;
  if ( (pTheBTApp != NULL) && (pTheBTApp->mAG.bEnabled != FALSE) )
  {
    AGIsEnabled = TRUE;
  }

  return AGIsEnabled;
}

/* ==========================================================================
FUNCTION BTDeviceSelected
DESCRIPTION
============================================================================= */
boolean BTDeviceSelected( void )
{
  boolean btDevSelected = FALSE;
  if ( (pTheBTApp != NULL) && (pTheBTApp->mAG.bEnabled != FALSE) )
  {
    btDevSelected = pTheBTApp->mAG.bAudioSelected;
  }
  return btDevSelected;
}

#ifdef FEATURE_BT_EXTPF_AV
/* ==========================================================================
FUNCTION BTA2DPDeviceSelected
DESCRIPTION
============================================================================= */
boolean BTA2DPDeviceSelected( void )
{
  boolean btDevSelected = FALSE;
  if ( (pTheBTApp != NULL) && (pTheBTApp->mA2DP.bEnabled != FALSE) )
  {
    btDevSelected = pTheBTApp->mA2DP.bAudioSelected;
  }
  return btDevSelected;
}

extern boolean BTDeviceSelected( void );
/* ==========================================================================
FUNCTION BTA2DP_SCO_DeviceSelected
DESCRIPTION
============================================================================= */
boolean BTA2DP_SCO_DeviceSelected( void )
{
  return (BTA2DPDeviceSelected() && BTDeviceSelected());
}
#endif /* FEATURE_BT_EXTPF_AV */
#endif /* UI_UISND_NO_DIRECT_STATUS_CALLS */

#endif /* FEATURE_APP_BLUETOOTH */
