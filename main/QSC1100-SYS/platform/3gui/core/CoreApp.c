/*=============================================================================

FILE: CoreApp.c

SERVICES: QCT UI Core BREW Application.

GENERAL DESCRIPTION:
This applet is the starting point of 3GUI.

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
This should be the startup application automatically launched by BREW.

(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreApp.c#440 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/08   clm     Let alarm clock handle alarm annunciator if FEATURE_ALARMCLOCK_APP is defined.
03/26/08   cvs     Remove deprecated functions
03/05/08   cvs     Fix TTY volumes based on scaling
12/21/07   cvs     Fix issues relating to slower CM init, and new ICard behavior
10/17/07   cvs     Fix issue related to verifying PIN codes outside coreapp control
10/05/07   cvs     Fix mem leak in MMOverlay BG Image code.  Remove Overlay2 BG img code
09/28/07   clm     Add PLMN name CFGI update function.
08/14/07   cvs     Fix case where 1x targets would toggle RSSI when not in hybrid
08/03/07   cvs     Featurize secondary display code
06/29/07   jks     Added NULL pointer check
06/18/07   jas     Fixing intialization of a local variable.
06/12/07   cvs     Don't update hybrid RSSI when LCD off
04/06/07   sj      Update PIN lock state after receiving the VALID card event
                   if the PIN event was not received/lost.
03/20/07   jas/ak  Adding support for persistent preference values.
03/07/07   jas     Reimplement background images with overlays.
                   Support VGA background images.
02/26/07   cvs     Notify BREW for headset events
02/14/07   jas     Fixing the handling of the ICONFIG pointer
02/03/07   ypmw    Fix the compiler errors for the changes of sound dev 
                   attach variables
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/18/06   cvs     Fixed issue with getting past PIN entry screen.  Added debug msgs.
12/11/06   RT      Moved m_nCurrSrvc and m_pRdmDeviceList initialization to Auxsettings applet
12/11/06   RT      Nullified DlgEvtHandler entries for SIO configuration
12/06/06   RT      CoreApp Refactoring: Moved CoreApp_DisplayPLMNAnn and associated 
                   functions to CoreService.c
11/21/06   RT      CoreApp Refactoring: Moved ICM related functions to CoreService.c
                   Added CoreApp_InvokeDlgEventHandler 
11/17/06   ak      Display Protocol Revision for HDR
11/14/06   ypmw    Fix wlan compiler warnings
11/10/06   jmb     Added support for IMMOverlay management of BREW bitmap
11/09/06   ypmw    Fix the WIFI Security setting errors
11/01/06   ypmw    Add SSID for start adhoc network
10/26/06   cvs     Fix Subscriber ID length (from BREW)
09/28/06   alb     Always call CoreApp_ProcessSubscriptionStatus when going online.
                   Fixes CR 87961.
10/18/06   ypmw    Fix the murge error
10/17/06   cvs     Fixed compiler warnings
10/10/06   sj      Update CM about subscription status when AEECARD_REINIT
                   and AEE_VALID_CARD events are received.
10/09/06   ypmw    Add multi-port for data services
10/09/06   sj      Initialized a member of CCoreApp structure
                   m_nPersoStatus
10/06/06   sj      Change security mask slot when Pin is blocked.
09/19/06   ypmw      Modified plmn network scan dialog
09/18/06   cvs     Integrate brew 4.0 changes from BTG.
09/14/06   jas     Removing calls to JCDMA functions that were moved
                   to NetSettings
09/14/06   jas     Replacing app member for manual PLMN selection option with
                   an IConfig item
09/13/06   sj      Fix handling of card events in perso and LPM mode
09/11/06   cvs     Power save fixes
09/08/06   jas     Removing menus that were related to the NetSettings app
09/07/06   adb     Added support for handling of volume keys
08/23/06   jks     Added support for WLAN Adhoc networks
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added menu to set IncallMixing state
08/08/06   joe     Added support for NetSettings. Fixed a bug in resume.
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
07/12/06   jks     Fixed compiler warnings
06/15/06   cvs     Display power save 2 minutes after going to power save.
06/15/06   cvs     Fix Klocwork errors
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
05/24/06   JD      Added call to SelectFonts to increase size on VGA
05/10/06   jas     Modifying background image handling to work on 7x00
05/04/06   jas     Porting display mapping code from 7500
05/02/06   jas     Removing the background image outside CoreApp
04/25/06   alb     BCMCS annunciator.
04/17/06   alb     Remove handling of AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND,
                   HSDPA indication now uses CM_SS_EVENT_SRV_CHANGED.
04/17/06   jas     Removing unused local variables causing compiler warnings
04/13/06   jas     Added support for VGA resources
04/04/06   jas     Added support for larger fonts.
03/13/06   jks     Added support for WLAN scan mode selection
03/21/06   alb     Added handling of AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND for
                   HSDPA annunciator.
03/14/06   cvs     release IConfig at app free
02/22/06   sj      Modified condition for checking HDR mode
02/17/06   cvs     Adding functionality to make game mode persistent
02/13/06   jas     Added support for GCF language selection
02/01/06   jks     Fixed bug in annunciator behavior
01/31/06   jks     Fixed bug in In Use annunciator behavior
01/27/06   jas     Fixes WLAN RSSI featurization
01/24/06   jks     Fixed RSSI annunciators behavior in CoreApp_ProcessICMEvents
12/16/05   jks     Added security support for WLAN manual scans
12/07/05   ak      Decouple PowerDown Timer and the END/PWR Key
12/05/05   jks     Added support for WLAN Scan Mode Selection, added function
                   CheckWLANAutoScanNewAssociation,  Removed support for WLAN IP Version
11/21/05   jas     Support for disabling background images
11/17/05   sun     Do not do Restartsecurity if a PERSO Failure dialog is up
11/07/05   ak      Do not allow phone operations for blocked pin case
11/04/05   cvs     Fix class id.
11/03/05   sun     Do not go to NO Card when BT is connected and there is no 
                   subscription available
10/21/05   RI      Updated the NITZ menu dlgs.
10/20/05   sun     Removed support for BT-SAP
10/12/05   RI      Added EVT_LCD_ON event processing to restart the
                   time display in the annunciator.
10/10/05   jks     Added new WLAN General Config menu, Max Tx Power menu
09/30/05   cvs     Fix EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED when idle dialog 
                   is not the current dialog.
09/27/05   cvs     Support for BREW NativeNotifier
09/27/05   sun     Added new Callback and cmdStatus for CSP
09/22/05   cvs     Support for Stereo USB Carkit
09/05/05   RI      Added ISysClock object interface.
08/31/05   sun     Added support for BT-SAP
08/08/05   aps     Fixed NV volume level lookup for sound devices
08/03/05   cvs     Reduce RAM usage
07/27/05   AT      Fixing featurization
07/27/05   RI      Added processing of illegal card status.
07/27/05   cvs     Code Review cleanups
07/25/05   sun     Added ISTK member to pMe
07/20/05   AT      Added new API to set the subscription_status as available
                   not available for all technologies in one CM commands.
07/15/05   AT      Added CFGI items for display_duration and back_light_intensity.
07/12/05   AT      Fix issues related to the time displayed.
07/07/05   cvs     Poll for PIN status at startup
07/05/05   sun     Set to Automatic Mode only if PLMN bit is set to 0
06/30/05   jas     Move IsSubscriptionAvail and defines to utils file
06/28/05   cvs     Add menu to select sound device
06/27/05   ak      CPHS ONS files use GSM Default encoding only.
06/22/05   jas     Added support for WLAN RSSI
06/14/05   ak      Read Network Name from ONS files
06/10/05   cvs     Support for Smart USB Carkit
06/10/05   jas     Fixing compile error in dialog event handler table
06/08/05   cvs     Fix init of TTY
06/07/05   sun     Added support for CCBS
06/02/05   cvs     Initialize volumes after USB HFK is attached.
05/27/05   jas     Added WLAN network selection mode dialog
05/25/05   aps     Updated InitSound() to initialize silenceall from NV
05/25/05   RI      Fixed compiler error.
05/20/05   sun     Fixed Duplicate Callbacks
05/20/05   sun     Fixed Compilation error
05/16/05   sun     Register for all FCNs at once.
05/13/05   cvs     More powerdown changes--Don't call AEE_Exit() from AEE_Dispatch()
05/11/05   ak      Use a different decoding function a wildcard fix
05/11/05   sun     IdleText should not be emptied out after copying the information.
05/10/05   cvs     Get powerdown done in one call from TMC
05/09/05   ak      Using the packed string Decoding
05/02/05   jas     Change annunciator field name to match interface
04/29/05   jas     Removing User ID dialog for WPA with PSK WLAN security
04/29/05   sun     Do not pop dialogs when in LPM 
04/28/05   jas     Include file fixes for Linux
04/21/05   jas     Added support for WPA WLAN security types
04/20/05   jas     Adding subscription support for WLAN
04/19/05   sun     If the NV_MANUAL_PLMN_SEL_I is set, then do not check the 
                   files in the SIM Card
04/19/05   ak      Choose the first matched PLMN found
04/19/05   jas     Removing IDD_ENTER_WEP, replaced by other dialogs
04/08/05   jas     Reworked authentication menus
04/08/05   ak      Added Checks for buffer overflow in PNN/OPL Files
04/06/05   RI      Support for TTY.
04/01/05   cvs     Remove sending event to QVideophone when speakerphone changes
03/30/05   cvs     Rename feature
03/22/05   cvs     Poll for ICard events we missed after registering for ICard events
03/18/05   JAS     Support for WLAN Status screen
03/18/05   ak      Changes to Support Async Callback in CARD
03/18/05   cvs     Added support for Verizon's HDR RSSI indications
03/15/05   cvs     Support for updating time on screen less frequently
03/15/05   ak      Space Allignment and some more checks
03/15/05   cvs     Fix UI lockup in game mode
03/10/05   ak      Added CPHS EONS Support
03/02/05   cvs     Fix changes to tell BREW of subscriber change.
03/01/05   sun     Fixed the m_bManualNWSelItem based on the mode bit
02/25/05   sun     Added Support for Carkit
02/23/05   cvs     Fix blank screen if in FTM
02/23/05   cvs     Fix featurization of Enhanced Network Selection
02/22/05   jas     Updating menus for WLAN
02/17/05   sun     Fixed Typos
02/15/04   sun     Added Support for Enhanced Network Selection
02/15/05   cvs     Revert change to send pin verification event
02/14/05   cvs     Fix UI lockup pressing end key in game mode
02/14/05   cvs     Fix race condition where subscriptions could be sent
                   before phone went online, and then weren't resent.
01/28/05   sun     EVT_SUBSCRIPTION_CHANGED is not sent when in LPM mode,
                   no need to check for operating mode.
01/25/05   sun     Do not check for LPM Dialog in ICM OPRT Mode Event
01/25/05   sun     Allow other Apps to Change Backlight setting via CoreApp
01/21/05   cvs     Dualslot cleanups
01/21/05   cvs     Rearchitect uisnd_set_device()
01/21/05   tjc     Support annunciator hiding
01/12/05   sun     Clear IdleMenu Text on Refresh
01/06/05   sun     Fixed Multiple Initiaziations and mulitple Security events
12/21/04   cvs     Add menus to wakeup 1x and HDR
12/21/04   sun     Do not send online mode change event in
                   AEECM_OPRT_MODE_OFFLINE_CDMA or AEECM_OPRT_MODE_OFFLINE_AMPS
12/16/04   sun     Check for Service Capability and not Service Domain while displaying annunicators
12/16/04   sun     Update the idle test when the app resumes
12/16/04   jas     Fix MDP registration region for time/date on 2ary display
12/13/04   ssc     Changed include file name case to match the Linux names
                   of the files. Mismatches in the case break AMSS builds
                   on Linux servers.
12/10/04   cvs     Add Menu to avoid current serving system
12/09/04   sun     Set pMe->m_bUserPwrDown  to TRUE in CheckForEmergencyCall.
12/06/04   sun     Check External Power even if the EVT_EXT_PWR_OFF is received.
12/06/04   BW      Added LAN menu and related submenu
11/23/04    ak     Phone Gets Stuck in Startup Screen
11/23/04   sun     Added support for USB HFK
11/22/04   sun     Fixed IBatt event handling
11/18/04   sun     DTMF cleanup
11/12/04   ank,cvs Dualslot changes from 6300
11/11/04   sun     Set the Default value of LCD off to "Always on"
11/10/04   cvs     Initialize provisioning earlier in powerup
11/03/04   JAS     Only register secondary image if 2ary display exists
11/01/04   JAS     Fixing merge error
10/28/04   AT      Integrated changes to tell BREW of subscriber change.
10/27/04   sun     Free up memory on error
10/13/04   PA      Do not display idle dialog after emergency call
10/07/04   cjr     Added to set the MDP system bitmap to NULL
                   Removed the IDisplay foreground bitmap info save
10/06/04   sun     Fixed Manual PLMN Search screens
09/28/04   jas     Updated feature defines for secondary display content
09/22/04   cvs     Add Contrast menu, lint fixes
09/16/04   ak      Add NV Item for LPM mode
09/16/04   PA      If phone info is available at startup, execute initialization
                   sequence without waiting for AEECM_EVENT_PH_INFO_AVAIL.
09/14/04   sun     Changed LPM to use HS macro instead
09/14/04   cjr     IDisplay_Update overlay registration
09/14/04   sun     Added Null Pointer checks
09/09/04   ak      Fixed Compile Error for SNUK builds
09/08/04   JAS     Fix to correctly identify QVGA+ screens
09/03/04   tml     Added pin 2 verification for line switching
09/02/04   tml     Added CPHS and ALS supports
09/02/04   JAS     Fixing infinite loop in InitSecondaryDisplay
08/16/04   cjr     Added overlay transparency operation
08/02/04   PA      Display PLMN List again only for network selection error.
07/28/04   PMD     Clear off Idle Menu Text by passing NULL in Background.
07/27/04   sun     Remove the low battery warning, fixed manula PLMN selection
07/20/04   cjr     Added background and annunciator overlay support through MDP
07/18/04   sun     Added broadcast to idle menu while the app is suspended
07/15/04   tml     only create idle menu if proper status has been received
                   from the card
07/08/04   sun     Fix the Key Beep tone
07/01/04   ak      Fixed subscription change for NVRUIM
06/29/04   sun     Send SubcriptionAvailable again after going to ONLINE Oprt Mode
06/28/04   jas     Modifications to support dual-display targets
06/25/04   sun     Send SubscriptionAvailable before going into LPM
06/23/04   sun     Send a separate event to change the operating mode
06/18/06   sun     Do not display Domain in Limited Service either
06/15/04   sun     Do not initialize volumes all the time,
                   no Roam display when in limited services,
                   Do not end Data call if System is in Hybrid mode.
06/10/04   sun     Do not power down on Low Battery when Charger is plugged in
06/10/04   sun     Do not allow the "Select Operator" Dialog to display at power up
06/09/04   tml     Allowed CoreApp to go to Idle when PIN 1 is verified by
                   other method.  Fixed compilation warning
06/07/04   tml     Added personalization support
06/07/04 sun/tml   Linking error fix
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
06/02/04   sun     Do not change to LPM when in FTM Mode
06/01/04   ak      Added feature for LPM Mode
06/01/04   sun     Fixed Backlights
05/13/04   sun     Added LPM Changes
05/07/04   sun     Fixed TTY
04/29/04   sun     Fixed Powerdown time,cancel all timers on powerdown,
                   made plmn info modifications
04/16/04   sun     Made Backlight/kepad light modifications
04/14/04   sun     Added new menu item to set VR
03/19/04   jas     Added creation of IDiag object
03/04/04   sun     Do not play Service Alert twice
03/01/04   sun     Fixed banner for emergency call back mode
02/25/04   ram     Integration with IBatt Interface.
02/24/04   sun     Removed "Maintenance required" from annunciator area.
02/20/04   NNG     Added support for volume up/down keys.
02/20/04   sun     Added Backlight fix, Enable voice privacy settings through
                   diag
02/12/04   ram     Modified annunciators display
01/29/04   ram     Replace AVK_SPEAKER with EVT_SPEAKERPHONE_TOGGLE
01/29/04   tml     Added FEATURE_NEW_ICARD
01/28/04   ram     Add speaker-phone functionality.
01/21/04   sun     Changed BatteryDisplay Timer to 1 Second
01/20/04   sun     Added back ICARD API Changes
01/20/04   sun     Poll the battery every 10 seconds, backed out ICARD API
                   changes
01/15/04   tml     Added back ICARD API Changes
01/15/04   tml     Reverted ICARD API Changes and added CoreApp ICard card
                   status check during initialization
12/22/03   tml     ICARD API Changes
01/07/04   ram     Add Multimedia Volume setting
12/20/03   tml     Fixed dual slot RTRE issue
12/09/03   ram     Removed lint errors.
12/08/03   tml     FDN fix
12/05/03   tml     stack reload during user pref change fix
12/02/03   ram     Changes for exiting emergency mode.
12/02/03   ram     Fixed HDR RSSI annunciator.
12/02/03   sun     Backed out PBM Changes
12/02/03   ram     Fix compile erros.
11/31/03   BPW     Do not handle auto repeat key events in BREW 3.0
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/25/03   SUN     Renamed Functions
11/24/03   AT      Changed pbm_init to not assume its running in the UI task.  Updated
                   the call to pbm_init to provide new parameters.
11/24/03   kar     FTM changes. Added CS_DATA calls to PPP abort
11/22/03   kar     Fixed compile error, don't play svc change alert when phone is in use
11/20/03   ram     Added HDR_ABORT_PPP support and fixed annunciators.
11/17/03   kar     Added missing SS events
11/13/03   BPW     Removed Sups event registering and handling
11/11/03   ram     Added Game-Mode.
11/11/03   tml     Added Pref Slot support
11/06/03   sun     Checked m_pIAnn field before using
11/04/03   ram     Fixed backlight duration
11/03/03   kar     Added E911 Cb mode
11/03/03   ram     Added PLMN selection menu.
10/31/03   kar     Handle ICM events when suspended
10/23/03   PA      Integrated with latest ICM
10/21/03   sun     Fixed Compile Error
10/16/03   kar     Changes for ICM enum change
10/09/03   sun     Fixed Compile Error
10/06/03   tml     Added thermo sensors reading
09/30/03   tml     Removed CAT App SS/USSD handling from Coreapp
09/30/03   tml     Reflected changes in ICM API
09/24/03   kar     Fixed compile errors
09/23/03   kar     Fixed annunciator updates for inuse event
09/23/03   kar     Fixed SS info handling
09/21/03   tml     ICM support
09/10/03   kar     Changes to update annunciator for HDR events
08/29/03   ram     Don't start SKUI for data calls if FEATURE_APP_DATASTATS
                   is defined
08/26/03   kar     Fixed featurization issue
08/26/03   tml     Fixed keypad freeze issue on subscription change
08/25/03   tml     Fixes for tearing down all screen during security restart
08/24/03   tml     Fixed card application issue
08/22/03   sun     Added suppsvc message handling
08/20/03   tml     Fixes for perm disable pin
08/18/03   tml     Added sim initializing screen
08/06/03   tml     Dual Slot support
08/09/03   kar     Fixed merge error
08/08/03   sun     Added new CheckBoxDlgEventHandler
08/06/03   bpw     Removed dialer include
08/06/03   sg      Added support for STEREO_HEADSET device.
08/05/03   kar     Set time changes
07/31/03   kar     Changes to support BREW
07/25/03   bpw     Removed dialer state handling
07/18/03   bpw     Removed suppsvc message handling
07/16/03   tml     enable/disable pin1/pin2 cleanup
07/15/03   tml     Security clean up
07/01/03   kar     Initialize phonebook for Saber
06/30/03   kar     Initialize FDN for LTK
06/25/03   tml     Fixed subscription available for WCDMA
06/11/03   kar     Fixed incoming call issue caused by previous checkin
06/10/03   kar     Don't switch to SKUI on SO 33 calls
06/06/03   AT      Added support for dial digits and supplementary services.
05/23/03   kar     Originate voice call now goes automatically to SK UI
05/19/03   kar     Code cleanup
05/14/03   tml     Added more PIN Change support
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
05/13/03   kar     Support for call origination from other apps
05/08/03   kar     System menu changes
05/03/03   tml     Added NV Lock Dlg
04/29/03   kar     Fixed JCDMA compile error
04/25/03   kar     Changes to send appropriate subs status to protocol stacks
04/21/03   kar     Removed support for hybrid WMS
04/18/03   PA      Restore PBM initialization.
04/17/03   PA/AT   Fixed PBM initialization calls
04/14/03   kar     Code cleanup
04/10/03   kar     Featurized code. Send RTRE config to CM only during powerup
04/01/03   kar     RTRE changes. Code cleanup
04/01/03   kar     Code cleanup
03/24/03   kar     Fixed freeze when RTRE config set to 1xGSM
03/18/03   kar     Added NULL ptr checks. Increased duration for time disp.
03/14/03   kar     Fixed error with Virtual SIM targets
03/11/03   kar     Don't clear SMS icon on powerup
03/11/03   pa      Added support for bad battery event
03/04/03   AP      Don't start coreapp if suspended when recieving new MT SMS
                   event.
02/28/03   kar     Featurized handling for AEECARD_CARD_REINIT under F_GSTK
02/28/03   jar     Added Handling for AEECARD_CARD_REINIT.
02/27/03   tml     Added USAT Send SS, Send USSD support.
                   Fixed multiple sim inserted event behavior
02/27/03   PM/PA   Added call to pbm_init_ecc on SIM inserted
02/23/03   kar     Call pbm_init_file_info on valid card
02/18/03   kar     Removed animation for powerdown display and hardcoded strings
02/18/03   AP      Feature-ized out events that will be handled by the WMS app.
02/10/03   kar     Added support for numedit dialog
02/06/03   kar     Initialization for PBM
01/31/03   tml     Swtich to SKUI when call connected and when the call was
                   initiated by cat app
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
01/30/03   kar     FTM changes, removed PM1000 code
01/29/03   kar     Handle SUPS event, deactivate SK UI on suspend
01/28/03   tml     Compiler fixes for GSM/WCDMA
01/28/03   kar     Changes for AppComFunc API
01/22/03   ic      Updated parameter list in calls to
                   ui_cm_ph_subscription_available()
01/21/03   kar     Call ui_mode_init when phone info is ready
01/03/02   kar     Resume coreapp while activating SKUI. Don't activate SK UI
                   for error handling.
12/31/02   kar     Delete timers on powerdown
12/12/02   ic      Under FEATURE_MONITOR removed the code that activated SK UI.
12/04/02   tml     Added uim status command before sending online to other
                   layers, bug fixes for offline mode and add featurization
                   when !F_PLT
11/27/02   tml     Bug fixes for RTRE
11/25/02   tml     RTRE changes
11/21/02   jb      Featurized call to ui_init_provisioning
11/20/02   kar     Called Security_init for non MMGSDI builds
11/15/02   tml     RTRE changes with CM events
11/13/02   kar     Added RTRE config menu
11/13/02   sun     Post Event to WMSAPP when SMS State exits
11/13/03   ic      Changes to monitor object interface - replaced UI_MONIT_CHANNEL_F
                   and UI_MONIT_UPDATE_ASET_F with UI_MONIT_INFO_CHANGE_F
11/12/02   kar     Activated SKUI Dlg on incoming call if suspended
11/08/02   kar     External device keys are now EVT_USER events
11/08/02   aaa     Added ui_gsdi_init in CoreApp_InitAppData
11/08/02   kar     Fixed splash screen
11/08/02   tml     Remove FEATURE_MMGSDI and F_UIM_RUN_TIME_ENABLE
                   for creating idle menu dialog
11/08/02   kar     Inform SKUI when ready to receive key events
11/05/02   sun     Activate SK UI only for certain SMS events
11/04/02   kar     Activatate SK UI only for certain call events
10/31/02   sun     Featurized UI_GWSMS_WMSAPP_END_F & UI_GWSMS_WMSAPP_OPTIONS_F
10/30/02   sun     Removed extra feature define for WMSAPP
10/29/02   sun     Added Support for WMSAPP
10/29/02   tml     RTRE security changes
10/29/02   kar     Changed F_JCDMA_MONITOR to F_MONITOR
10/21/02   jk      Fixed compile warning
10/14/02   kar     Fixed compile warnings
10/08/02   tml     code review changes
10/07/02   kar     Got rid of update BAR mode,moved InitResources to ui_init
10/05/02   tml     Fixed malloc size for AEEPINCode
10/03/02   kar     Fixed compile warnings
10/03/02   kar     Fixed debug messages
10/02/02   kar     Code cleanup
10/01/02   kar     ROMized resource file
09/20/02   kar     BAR file update mode is triggered by CLR key
09/20/02   tml     Added FEATURE_MMGSDI for Security_init during
                   StartCoreApp
09/18/02   kar     Fixed mbUpdateBAR
09/17/02    lz     Added support to resume the CoreApp when getting an
                   incoming call.
09/17/02   tml     Added FEATURE_UIM_RUN_TIME_ENABLE support
                   and CoToUpdateBAR variable fix
09/17/02   kar     Use AEE library functions
09/17/02   kar     Support for factory testmode
09/16/02   AAA     Added Multimedia Ringer support
09/16/02   kar     Support for time display
09/11/02 att/tml   Modification for ICARD changed
09/11/02   kar     Call AEE_Exit() on UpdateBAR state
09/11/02    lz     Added ui_cm_init in CoreApp_InitAppData().
09/10/02   kar     Don't activate splash screen if in BAR file update mode
09/09/02  js/tml   Added new dialogs for PIN verification notification
                   Changed ICARD initialization procedure so that
                   the 2 second update bar mode option is preserved
08/27/02    lz     Used new BREWapi feature to keep dialogs alive when Core
                   App is suspended.
08/26/02   sun     Added support for broadcast sms
08/26/02   sun     Added support for WMS
08/23/02  att/tml  Add ICARD notification event supports
08/22/02   kar     Code cleanup
08/21/02  js/tml   Support for ICARD integration
08/15/02   kar     Updated new call events
08/13/02   kar     Core application
===========================================================================*/
//lint -save -e611 -e740 Suppress complaints about unusual ptr cast
/*===========================================================================

                      INCLUDES

===========================================================================*/
#include "OEMFeatures.h"
#include "comdef.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEAnnunciator.h"
#include "AEEDownload.h"
#include "AEEFile.h"
#include "AEE_OEMEvent.h"
#include "OEMDeviceNotifier.h"
#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#include "Diag.bid"
#endif

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#include "uiutils.h" //For DisplayPLMNAnn function
#endif
#include "CoreApp.h"        /* CoreApp external data and function prototypes */
#include "CoreMenu.h"       /* CoreApp menu data and function prototypes */
#include "CoreSecurity.h"   /* CoreApp security data and function protytypes */
#include "CoreUtils.h"
#include "CoreService.h"    /* CoreApp ICM event handler and related function prototypes */
#include "core_ui.h"
#include "AppComFunc.h"
#include "CoreApp_res.h"
#include "CoreApp_qvga_res.h"
#include "CoreApp_vga_res.h"
#ifdef FEATURE_USRSYSCLOCK
#include "AEEUsrSysClock.h"
#include "USRSYSCLOCK.BID"
#endif
#include "AEEClipboard.h"
#include "AEENativeNotifier.h"
#include "AEEFile.h"

#if defined(FEATURE_WMS_APP) && defined(FEATURE_CDSMS_WMSAPP)
#include "WmsApp.h"
#endif

#if defined (FEATURE_APP_CATAPP) && defined (FEATURE_SUPS)
#include "catapp.h"
#endif

#if defined (FEATURE_APP_FLDDBG)
#include "FLDDBGAPP.BID"
#endif

#include "AEEBatt.h"

#ifdef FEATURE_RECORD_BATTERY_EXPIRY_TIME
#include "clk.h"
#include "clkrtc.h"
#endif // FEATURE_RECORD_BATTERY_EXPIRY_TIME

#ifdef FEATURE_ENHANCED_OVERLAY
#include "OVERLAYMGR.BID"
#include "AEEBitmap.h"
#include "DISPLAYCFG.BID"
#include "IDisplayCfg.h"
#endif /* FEATURE_ENHANCED_OVERLAY */

#ifdef FEATURE_MMOVERLAY
#include "AEEBitmap.h"
#include "AEEMMOverlay.h"
#include "AEECLSID_MMOVERLAYMGR.BID"
#endif /* FEATURE_MMOVERLAY */

#include "uiutils.h"

#ifdef FEATURE_APP_PUREVOICE
#include "purevoice.h"
#endif
static void RemoveLowBatteryWarning(void *pUser);
#ifdef FEATURE_CPHS_EONS
static int CoreApp_RegisterForFCN(CCoreApp *pMe);
static void CoreApp_ReadIMSIFileCb(void *pData);
static int CoreApp_FillGenericData
(
  CCoreApp *pMe,
  AEECardFilePathType *pCardFilePath,
  AEECardApp *pCardApp,
  uint8 *pSlot,
  AEECardFileItemEnumType *pFileList,
  AEECardFileItemEnumType usimFileName,
  AEECardFileItemEnumType simFileName
);
#endif

#if defined(FEATURE_ENHANCED_OVERLAY) || defined(FEATURE_MMOVERLAY)
static boolean CoreApp_InitBgImage(CCoreApp * pMe);
#endif /* FEATURE_ENHANCED_OVERLAY || FEATURE_MMOVERLAY */

/*===========================================================================

                      Constant Definitions

===========================================================================*/
#define POWERDOWNTIME 120000  /* 2 minutes power down time */

/* Dialog Event Handler function pointer structure */
typedef boolean (*PFNDLGHANDLER) (
                                 void *pUser,
                                 AEEEvent evt,
                                 uint16 w,
                                 uint32 dw );


//testing
#ifndef FEATURE_MMGSDI
#define FEATURE_TEST_NV_NV
#else
//#define FEATURE_TEST_NV_NV
//#define FEATURE_TEST_RUIM_RUIM
//#define FEATURE_TEST_FALL_BACK_RUIM
//#define FEATURE_TEST_FALL_BACK_NV
#endif

/*===========================================================================

                      STATIC DATA

===========================================================================*/

static const PFNDLGHANDLER DlgEventHandlers[] = {
  NULL,
  IdleMenuDlgEventHandler,          /* 1  IDD_CORE */
  MenuDlgEventHandler,              /* 2  IDD_SETTING */
  MenuDlgEventHandler,              /* 3  IDD_EXTRAS */
  RadioButtonDlgEventHandler,       /* 4  IDD_SILENCEALL */
  TimeDateDlgEventHandler,          /* 5  IDD_SETTIME_EDIT */
  RadioButtonDlgEventHandler,       /* 6  IDD_BEEPLENGTH */
  SecurityDlgEventHandler,          /* 7  IDD_SECURITY */
  MenuDlgEventHandler,              /* 8  IDD_DISPLAY */
  MenuDlgEventHandler,              /* 9  IDD_SOUND */
  MenuDlgEventHandler,              /* 10 IDD_VOLUME */
  StaticDlgEventHandler,            /* 11 IDD_RESTORE_SETTINGS */
  BannerDlgEventHandler,            /* 12 IDD_BANNER_EDIT */
  KeyGuardOnDlgEventHandler,        /* 13 IDD_KEYGUARD_ON */
  RestoreSettingsNowDlgEventHandler,/* 14 IDD_RESTORE_SETTINGS_NOW */
  HourGlassDlgEventHandler,         /* 15 IDD_HOURGLASS */
  MenuDlgEventHandler,              /* 16 IDD_USER_SLOT_PREF */
  MessageDlgEventHandler,           /* 17 IDD_GENERAL_MESSAGE */

  /* Dialogs displayed for PIN Verification States for CARD 2*/
  PinVerificationEventHandler,      /* 18 IDD_PIN2_REQUEST */
  OldPinVerificationEventHandler,   /* 19 IDD_OLD_PIN2_REQUEST */
  StaticDlgEventHandler,            /* 20 IDD_BANNER_VIEW */
  OldPinVerificationEventHandler,   /* 21 IDD_OLD_PIN_REQUEST */
  OldPinVerificationEventHandler,   /* 22 IDD_PUK2_REQUEST*/
  RadioButtonDlgEventHandler,       /* 23 IDD_DURATION */
  StaticDlgEventHandler,            /* 24 IDD_SETTIME_VIEW */
  KeyGuardDlgEventHandler,          /* 25 IDD_KEYGUARD */
  MenuDlgEventHandler,              /* 26 IDD_BACKLIGHT */
  StaticDlgEventHandler,            /* 27 IDD_MYINFO */
  RadioButtonDlgEventHandler,       /* 28 IDD_SERVICEALERT */
  SoundRadioButtonDlgEventHandler,  /* 29 IDD_RINGERVOLUME */
  RadioButtonDlgEventHandler,       /* 30 IDD_SMARTSOUND */
  SoundRadioButtonDlgEventHandler,  /* 31 IDD_KEYBEEPVOLUME */
  SoundRadioButtonDlgEventHandler,  /* 32 IDD_EARPIECEVOLUME */
  RestrictCheckBoxDlgEventHandler,  /* 33 IDD_RESTRICT */
  SoundRadioButtonDlgEventHandler,  /* 34 IDD_MESSAGEVOLUME */

  /* Dialogs displayed for NO card State */
  NoCardDlgEventHandler,            /* 35 IDD_NOCARD_EMERGENCY */
  NoCardDlgEventHandler,            /* 36 IDD_CARD_INVALID */

  RadioButtonDlgEventHandler,       /* 37 IDD_AUTOHYPHEN */
  RadioButtonDlgEventHandler,       /* 38 IDD_USER_SLOT_PREF_CDMA */
  RadioButtonDlgEventHandler,       /* 39 IDD_DISPLAYTIME */
  RadioButtonDlgEventHandler,       /* 40 IDD_INTENSITY */
  MenuDlgEventHandler,              /* 41 IDD_SIOCONFIGURATION */
  RadioButtonDlgEventHandler,       /* 42 IDD_AUTOANSWER */
  RadioButtonDlgEventHandler,       /* 43 IDD_AUTORETRY */
  RadioButtonDlgEventHandler,       /* 44 IDD_ONETOUCH */
  RadioButtonDlgEventHandler,       /* 45 IDD_PRIVACY */

  /* Dialogs displayed for PIN Locks/Unlocks */
  EnablePinDlgEventHandler,         /* 46 IDD_PINLOCK1 */
  EnablePinDlgEventHandler,         /* 47 IDD_PINLOCK2 */
  MenuDlgEventHandler,              /* 48 IDD_PINLOCK */
  PINChangeDlgEventHandler,         /* 49 IDD_PINCHANGE */

  RadioButtonDlgEventHandler,       /* 50 IDD_MUTE */
  SoundRadioButtonRingTypeDlgEventHandler,  /* 51 IDD_RINGTYPE */
  NewPinVerificationEventHandler,   /* 52 IDD_NEW_PIN2_REQUEST */
  NULL,                             /* 53 AVAILABLE */ 
  NULL,                             /* 54 AVAILABLE */ 
  RadioButtonDlgEventHandler,       /* 55 IDD_TTY */

  /* Dialogs displayed for PIN Verification for CARD1 */
  PinVerificationEventHandler,      /* 56 IDD_ENTERPIN */
  OldPinVerificationEventHandler,   /* 57 IDD_ENTERPUK */
  NewPinVerificationEventHandler,   /* 58 IDD_NEWPIN */
  NULL,                             /* 59 AVAILABLE */
  NULL,                             /* 60 AVAILABLE */
  MenuDlgEventHandler,              /* 61 IDD_CUG*/

  /* Dialog displayed for NV Pin Verification */
  PinVerificationEventHandler,      /* 62 IDD_ENTERNVCODE */
  RadioButtonDlgEventHandler,       /* 63 IDD_RTRE_CONFIG */
  RadioButtonDlgEventHandler,       /* 64 IDD_GAME_MODE */
  RadioButtonDlgEventHandler,       /* 65 IDD_CUG_ENABLE */
  RadioButtonDlgEventHandler,       /* 66 IDD_CUG_PREF_GROUP */
  RadioButtonDlgEventHandler,       /* 67 IDC_MENU_CUG_OUT_ACCESS */
  CUGGroupIDDlgEventHandler,        /* 68 IDD_CUG_GROUP_ID */

  /* Dialogs displayed for Pin Enable/Disable for NV/CARD 1/CARD2 */
  EnablePinDlgEventHandler,         /* 69 IDD_NVLOCK */
  OldPinVerificationEventHandler,   /* 70 IDD_OLD_NV_REQUEST */
  NewPinVerificationEventHandler,   /* 71 IDD_NEW_NV_CODE */
  PinVerificationEventHandler,      /* 72 IDD_ENTERPINENABLE */
  PinVerificationEventHandler,      /* 73 IDD_ENTERNVCODEENABLE */
  PinVerificationEventHandler,      /* 74 IDD_ENTERPIN2ENABLE */
  PinVerificationEventHandler,      /* 75 IDD_ENTERPINDISABLE */
  PinVerificationEventHandler,      /* 76 IDD_ENTERPIN2DISABLE */
  PinVerificationEventHandler,      /* 77 IDD_ENTERNVCODEDISABLE */
  PinVerificationEventHandler,      /* 78 IDD_ENTERPIN_SLOT2 */
  OldPinVerificationEventHandler,   /* 79 IDD_ENTERPUK_SLOT2 */
  OldPinVerificationEventHandler,   /* 80 IDD_OLD_PIN_REQUEST_SLOT2 */
  PinVerificationEventHandler,      /* 81 IDD_ENTERPINENABLE_SLOT2 */
  PinVerificationEventHandler,      /* 82 IDD_ENTERPINDISABLE_SLOT2 */
  PinVerificationEventHandler,      /* 83 IDD_PIN2_REQUEST_SLOT2 */
  PinVerificationEventHandler,      /* 84 IDD_ENTERPIN2ENABLE_SLOT2 */
  PinVerificationEventHandler,      /* 85 IDD_ENTERPIN2DISABLE_SLOT2 */
  OldPinVerificationEventHandler,   /* 86 IDD_PUK2_REQUEST_SLOT2 */
  OldPinVerificationEventHandler,   /* 87 IDD_OLD_PIN2_REQUEST_SLOT2 */
  EnablePinDlgEventHandler,         /* 88 IDD_PINLOCK1_SLOT2 */
  EnablePinDlgEventHandler,         /* 89 IDD_PINLOCK2_SLOT2 */
  RadioButtonDlgEventHandler,       /* 90 IDD_USER_SLOT_PREF_GSM */
  SimInitializingDlgEventHandler,   /* 91 IDD_SIM_INITIALIZING */
  TCTestModeDlgEventHandler,        /* 92 IDD_TC_TEST_MODE */
  PLMNListDlgEventHandler,          /* 93 IDD_PLMN_LIST */
  PLMNSearchingDlgEventHandler,     /* 94 IDD_PLMN_SEARCHING */
  RadioButtonDlgEventHandler,       /* 95 IDD_VR*/
  LPMDlgEventHandler,               /* 96 IDD_LPM*/

  /* Dialogs displayed for Personalization*/
  PersoIndicationDlgEventHandler,   /* 97 IDD_PERSO_FEATURE_INDICATION */
  EnterPersoKeyDlgEventHandler,     /* 98 IDD_ENTER_NW_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 99 IDD_ENTER_NW_SUB_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 100 IDD_ENTER_SP_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 101 IDD_ENTER_CP_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 102 IDD_ENTER_SIM_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 103 IDD_ENTER_ACTIVATE_NW_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 104 IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 105 IDD_ENTER_ACTIVATE_SP_PERSO_KEY */
  EnterPersoKeyDlgEventHandler,     /* 106 IDD_ENTER_ACTIVATE_CP_KEY */
  EnterPersoKeyDlgEventHandler,     /* 107 IDD_ENTER_ACTIVATE_SIM_PERSO_KEY */
  MenuDlgEventHandler,              /* 108 IDD_PERSO_FEATURE_DATA */
  ScanNetworkDlgEventHandler,       /* 109 IDD_SCAN_NETWORK*/
  RadioButtonDlgEventHandler,       /* 110 IDD_LCD */

  /* Dialogs displayed for Personalization */
  EnterPersoDataDlgEventHandler,    /* 111 IDD_ENTER_NW_PERSO_DATA */
  EnterPersoDataDlgEventHandler,    /* 112 IDD_ENTER_NW_SUB_PERSO_DATA */
  EnterPersoDataDlgEventHandler,    /* 113 IDD_ENTER_SP_PERSO_DATA */
  EnterPersoDataDlgEventHandler,    /* 114 IDD_ENTER_CP_PERSO_DATA */
  EnterPersoDataDlgEventHandler,    /* 115 IDD_ENTER_SIM_PERSO_DATA */
  ALSLineSelectDlgEventHandler,     /* 116 IDD_ALS_LINE_SELECTION */
  ALSLineSelectDlgEventHandler,     /* 117 IDD_ALS_LINE_SWITCHING */
  MenuDlgEventHandler,              /* 118 IDD_ALS */
  MenuDlgEventHandler,              /* 119 IDD_CPHS */
  ContrastDlgEventHandler,          /* 120 IDD_CONTRAST */

  /* Dialogs displayed for Card Selection State */
  SelectTechDlgEventHandler,        /* 121 IDD_SELECT_TECH_FOR_CARD_1 */
  SelectTechDlgEventHandler,        /* 122 IDD_SELECT_TECH_FOR_CARD_2 */
  StaticDlgEventHandler,            /* 123 IDD_CARD_1_ERROR*/
  StaticDlgEventHandler,            /* 124 IDD_CARD_2_ERROR*/
  SelectCardDlgEventHandler,        /* 125 IDD_SELECT_CARD */
  SelectCardDlgEventHandler,        /* 126 IDD_SELECT_CARD1*/
  SelectTechDlgEventHandler,        /* 127 IDD_SELECT_TECH_FOR_BOTH_CARDS */

  /* Dialog displayed REINIT Events */
  SimInitializingDlgEventHandler,   /* 128 IDD_SIM_REINIT*/
  RadioButtonDlgEventHandler,       /* 129 IDD_AUTO_ACCEPT_RECALL */
  RadioButtonDlgEventHandler,       /* 130 IDD_SOUND_DEVICE_DIALOG */
  RadioButtonDlgEventHandler,       /* 131 IDD_NETWORK_TIME */
  MenuDlgEventHandler,              /* 132 IDD_NITZ */
  RadioButtonDlgEventHandler,       /* 133 IDD_DELETE_NITZ_PLMN */
  RadioButtonDlgEventHandler,       /* 134 IDD_DISABLE_BG_IMAGE */
  RadioButtonDlgEventHandler,       /* 135 IDD_LANGUAGE_SELECT */
  MenuDlgEventHandler,              /* 136 IDD_DISPLAY_MAPPING */
  RadioButtonDlgEventHandler,       /* 137 IDD_DISPLAY_MAP_1 */
  RadioButtonDlgEventHandler,       /* 138 IDD_DISPLAY_MAP_2 */
  RadioButtonDlgEventHandler,       /* 139 IDD_DISPLAY_MAP_3 */
  RadioButtonDlgEventHandler,       /* 140 IDD_DISPLAY_MAP_4 */
  RadioButtonDlgEventHandler,       /* 141 IDD_INCALL_MIXING */
  RadioButtonDlgEventHandler,       /* 142 IDD_SIDETONE */  
}; /* End DlgEventHandlers[] */

/* Define our resource file.  Default to using the base resource file.
 * We'll switch during init if the screen resolution is large enough.
 */
char *gpszResourceFile = COREAPP_RES_FILE;

#ifdef AEE_STATIC
static AEEAppInfo gaiCoreApp = {AEECLSID_CORE_APP,
                    COREAPP_RES_FILE,
                    IDS_COREMENU_TITLE,
                    0,0,0,0 , AFLAG_PHONE | AFLAG_STATIC | AFLAG_HIDDEN};
#endif /* AEE_STATIC */


#ifdef FEATURE_MMGSDI_DUAL_SLOT
boolean IsBothSlotReady(CCoreApp *pMe);
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION: CoreApp_Invoke_DlgEventHandler

DESCRIPTION: 
  Invokes the DlgEventHandler corresponding to the Dlg ID.

SIDE EFFECTS:
  None
===========================================================================*/
boolean CoreApp_InvokeDlgEventHandler(uint16 wDlgID, CCoreApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if(wDlgID == 0) /* error checking */
  {
    CORE_ERR("Dialog ID == 0: Invalid Dialog ID %d", wDlgID, 0, 0); 
    return FALSE; /* error */
  }

  if(wDlgID >= ARR_SIZE(DlgEventHandlers)) {  /* error checking */
    CORE_ERR("Dialog ID out of bounds: Invalid Dialog ID %d", wDlgID, 0, 0); 
    return FALSE; /* error */
  }

  /* find the appropriate event handler */
  if (DlgEventHandlers[wDlgID] != NULL) {
    if(DlgEventHandlers[wDlgID](pMe, eCode, wParam, dwParam)) {
      /* event handled by dialog event handler */
      return TRUE;
    }
  }
  return FALSE; /* error */
}

/*=============================================================================
FUNCTION: CoreApp_ProcessICardEvents

DESCRIPTION:
  Process ICard Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_ProcessICardEvents(CCoreApp *pMe, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CoreApp_FreeAppData

DESCRIPTION
  Release the application data member

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void CoreApp_FreeAppData(IApplet* po)
{
  CCoreApp *pMe = (CCoreApp*)po;
  ASSERT( pMe != NULL );   /* error checking */

  if(pMe->m_pSplashImage != NULL) {     /* Release Splash image */
    IIMAGE_Release(pMe->m_pSplashImage);
    pMe->m_pSplashImage = NULL;
  }


  if(pMe->m_pIBatt!= NULL) {     /* Release IBatt */
     ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_CORE_APP,
                    AEECLSID_BATT_NOTIFIER,
                    0);

     IBATT_Release(pMe->m_pIBatt);
     pMe->m_pIBatt = NULL;
   }

  if (pMe->m_pIAnn) {               /* Release annunciator */
    IANNUNCIATOR_Release(pMe->m_pIAnn);
    pMe->m_pIAnn= NULL;
  }

  // Cleanup the secondary display-related items
  if (pMe->m_pIBitmap) {
    IBITMAP_Release(pMe->m_pIBitmap);
    pMe->m_pIBitmap = NULL;
  }

#ifdef FEATURE_SECONDARY_DISPLAY
  if (pMe->m_pIBitmap2) {
    IBITMAP_Release(pMe->m_pIBitmap2);
    pMe->m_pIBitmap2 = NULL;
  }

  if (pMe->m_pIDisp2) {
    IDISPLAY_Release(pMe->m_pIDisp2);
    pMe->m_pIDisp2 = NULL;
  }

  if (pMe->m_pTimeDate) {
    ISTATIC_Release(pMe->m_pTimeDate);
    pMe->m_pTimeDate = NULL;
  }
#endif

  /* ICard */
  if (pMe->m_pICard) {
    ISHELL_RegisterNotify(pMe->a.m_pIShell,  AEECLSID_CORE_APP, AEECLSID_CARD_NOTIFIER,0);
    ICARD_Release(pMe->m_pICard);
    pMe->m_pICard = NULL;
  }

  /* ICM */
  if (pMe->m_pICM) {
    ISHELL_RegisterNotify(pMe->a.m_pIShell,
      AEECLSID_CORE_APP,
      AEECLSID_CM_NOTIFIER,
      0 );

    ICM_Release(pMe->m_pICM);
    pMe->m_pICM = NULL;
  }

 #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  /* ISupp */
  if (pMe->m_pISUPPSVC) {
    ISUPPSVC_Release(pMe->m_pISUPPSVC);
    pMe->m_pISUPPSVC = NULL;
  }
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pMe->m_pIDiag != NULL)
    IDIAG_Release(pMe->m_pIDiag);
#endif

  /* IMedia */
  if (pMe->m_pIMedia) {
    IMEDIA_Release(pMe->m_pIMedia);
    pMe->m_pIMedia = NULL;
  }

#ifdef FEATURE_USRSYSCLOCK
  /* ISYSCLOCK */
  if(pMe->m_pIUsrSysClock) {
    (void)IUSRSYSCLOCK_Release(pMe->m_pIUsrSysClock);
    pMe->m_pIUsrSysClock = NULL;
  }
#endif // FEATURE_USRSYSCLOCK

  if (pMe->m_pIConfig) {
    ICONFIG_Release(pMe->m_pIConfig);
    pMe->m_pIConfig = NULL;
  }

  FREEIF(pMe->m_sPIN.code); /* free data */

  FREEIF(pMe->m_sPUK.code); /* free data */

  FREEIF(pMe->m_pszBanner);

  FREEIF(pMe->m_pszTime);
  FREEIF(pMe->m_pszLastTime);

  FREEIF(pMe->m_pszDialString);

  FREEIF(pMe->m_pszIdleMenuText);

#ifdef FEATURE_GSTK
  if (pMe->m_pISTK)
  {
    (void)ISTK_Release(pMe->m_pISTK);
    pMe->m_pISTK = NULL;
  }
#endif

  /* Release all of the elements related to the background image */
#if defined(FEATURE_MMOVERLAY)
  if (pMe->m_pBgImage != NULL)
  {
    (void)IIMAGE_Release(pMe->m_pBgImage);
    pMe->m_pBgImage = NULL;
  }
  if (pMe->m_pBgImgOverlay)
  {
    (void)IMMOVERLAY_Release(pMe->m_pBgImgOverlay);
    pMe->m_pBgImgOverlay = NULL;
  }
  if (pMe->m_pFgImgOverlay != NULL)
  {
    (void)IMMOVERLAY_Release(pMe->m_pFgImgOverlay);
    pMe->m_pFgImgOverlay = NULL;
  }
#endif /* FEATURE_MMOVERLAY */


} /* End CoreApp_FreeAppData */



/*===========================================================================
FUNCTION DrawImage

DESCRIPTION
  Start the animation at the center of the screen

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void DrawImage(CCoreApp *pMe, AEERect * pRect)
{
   AEEImageInfo   ii;
   int            nX;
   int            nY;

   IIMAGE_GetInfo(pMe->m_pSplashImage, &ii);

   // Do not display if image does not fit in the allocated rectangle.
   if (ii.cxFrame > pRect->dx || ii.cy > pRect->dy)
      return;

   nX = pRect->x + (pRect->dx / 2) - (ii.cxFrame / 2);
   nY = pRect->y + (pRect->dy / 2) - (ii.cy / 2);

   IIMAGE_Start(pMe->m_pSplashImage, nX, nY);
} /* End DrawImage */



/*===========================================================================
FUNCTION ShowSplash()

DESCRIPTION
  This function displays a splash image before the main idle menu is displayed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ShowSplash(CCoreApp *pMe)
{
  /* Load the image */
  pMe->m_pSplashImage = ISHELL_LoadResImage(  pMe->a.m_pIShell,
                        gpszResourceFile,
                        IDB_POWERUP );

  if (pMe->m_pSplashImage != NULL) { /* loaded successfully */
    AEERect  rect;
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    SETAEERECT(&rect, 0, 0, pMe->m_cxScreen, pMe->m_cyScreen);

    DrawImage(pMe, &rect);
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  }
} /* End ShowSplash */



/*===========================================================================
FUNCTION HideSplash

DESCRIPTION
  This function remove the splash image display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean HideSplash(CCoreApp *pMe)
{
  if(pMe->m_pSplashImage != NULL)
  {
    IIMAGE_Stop(pMe->m_pSplashImage);
    IIMAGE_Release(pMe->m_pSplashImage);
    pMe->m_pSplashImage = NULL;
    return TRUE;
  }
  return FALSE;
} /* End HideSplash */

#ifdef FEATURE_TEMPERATURE_MONITOR
/*===========================================================================
FUNCTION GetThermoSensors

DESCRIPTION
  Obtains the various thermo sensors value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void GetThermoSensors (CCoreApp *pMe)
{
  uitherm_read();
  if (pMe != NULL)
  {
    /* Restart timer to read various thermo sensors */
    ISHELL_SetTimer(pMe->a.m_pIShell,
                      60000,
                      (PFNNOTIFY) GetThermoSensors,
                      pMe);

  }

}
#endif

/*===========================================================================
FUNCTION TurnOffAnnunciators

DESCRIPTION
  This function turns off all the annunciators

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void TurnOffAnnunciators(CCoreApp *pMe)
{
  if ( pMe->m_pIAnn != NULL) {
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, ANNUN_STATE_OFF); /* reserved space */
  #if !defined (FEATURE_WMS_APP)
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG,      ANNUN_STATE_OFF); /* message alert */
  #endif
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_NET,      ANNUN_STATE_OFF); /* network */
#if !defined (FEATURE_ALARMCLOCK_APP)
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ALARM,    ANNUN_STATE_OFF); /* alarm */
#endif
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SECURITY, ANNUN_STATE_OFF); /* security */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MODE,     ANNUN_STATE_OFF); /* mode indicator */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ROAM,     ANNUN_STATE_OFF); /* roam */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_DOMAIN,   ANNUN_STATE_OFF); /* domain */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_RSSI,     ANNUN_STATE_OFF); /* RSSI */
#ifdef FEATURE_WLAN
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_WLAN_RSSI, ANNUN_STATE_OFF); /* WLAN RSSI */
#endif
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_INUSE,    ANNUN_STATE_OFF); /* in use */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BATT,     ANNUN_STATE_OFF); /* battery */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_TIME,     ANNUN_STATE_OFF); /* time */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BANNER,    ANNUN_STATE_OFF); /* banner */
  }
} /* End TurnOffAnnunciators */

/*===========================================================================
FUNCTION DisplayPowerDown

DESCRIPTION
  Display the power down animation

DEPENDENCIES
  DrawImage

SIDE EFFECTS
  None
===========================================================================*/
static void DisplayPowerDown(CCoreApp *pMe)
{
  pMe->m_pSplashImage = ISHELL_LoadResImage(pMe->a.m_pIShell, gpszResourceFile, IDB_POWERDN);

  if (pMe->m_pSplashImage != NULL) {

    AEERect  rect;
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    SETAEERECT(&rect, 0, 0, pMe->m_cxScreen, pMe->m_cyScreen);

    DrawImage(pMe, &rect);

    IDISPLAY_Update(pMe->a.m_pIDisplay);
  }
} /* DisplayPowerDown */



/*===========================================================================
FUNCTION ForcePowerDown

DESCRIPTION
  In case the phones doesn't power down in 2 minutes, this function will be
  called to forcefully release info from CoreApp

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ForcePowerDown(CCoreApp *pMe)
{
  ERR("Failed to power down correctly", 0, 0, 0);
  CoreApp_FreeAppData((IApplet*) pMe);
  ERR_FATAL("Failed to power down Correctly",0,0,0);
} /* End ForcePowerDown */



/*===========================================================================
FUNCTION PowerDown

DESCRIPTION
  This function start the power down procedure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void PowerDown(CCoreApp *pMe)
{
  int nReturnStatus;

  uisnd_snd_stop();
  uisnd_multi_stop();
  uisnd_tone_stop();


  CoreTask_SetPowerUp(FALSE);
  /* Cancel Timers */
  ISHELL_CancelTimer (pMe->a.m_pIShell, (PFNNOTIFY) PowerDown, pMe);
  ISHELL_CancelTimer (pMe->a.m_pIShell, (PFNNOTIFY) DisplayTimeWithRefresh, pMe);
#ifdef FEATURE_TEMPERATURE_MONITOR
  ISHELL_CancelTimer (pMe->a.m_pIShell, (PFNNOTIFY) GetThermoSensors, pMe);
#endif

  TurnOffAnnunciators(pMe);

  if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL) {
    /* clean up all dialogs */
    (void) DisplayIdleMenuDlg(pMe);
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    /* end the idle menu dialog too */
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  if (pMe->m_pSplashImage) {
    /* Release the existing splash image before using
       this for the power down display */
    IIMAGE_Release( pMe->m_pSplashImage );
    pMe->m_pSplashImage = NULL;
  }
  DisplayPowerDown(pMe);


  // to be removed after bringing over to CoreApp !!!!
  // TO DO - is this reqd ?
  //ui_nv_savestuff();        /* save call related and sms stuffs to NV */

  if (pMe->m_bUserPwrDown) {
    /* Send ICM command only for user initiated powerdowns */
    if(pMe->m_pICM == NULL)
    {
      ERR_FATAL("Pointer should not be NULL",0,0,0);
    }
    ICM_SetOperatingMode (pMe->m_pICM, AEECM_OPRT_MODE_PWROFF);
  }

  pMe->m_bPwr = FALSE; /* phone powered down */

  /* start the power down timer, so that if the phone is not powered down in
  2 minutes, manually release the powering off screen and other info as
  appropriate */
  ISHELL_SetTimer(pMe->a.m_pIShell, POWERDOWNTIME,
                  (PFNNOTIFY) ForcePowerDown, pMe);

    CoreTask_SetPwrDnComplete(TRUE);
} /* End PowerDown */

/*===========================================================================
FUNCTION CoreApp_UpdateBatteryAnnunciator

DESCRIPTION
  Updates the battery annunciator display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void CoreApp_UpdateBatteryAnnunciator(CCoreApp *pMe, unsigned int annunState)
{
  if(pMe->m_pIAnn)
  {
    if(annunState > ANNUN_STATE_BATT_FULL)
      annunState = ANNUN_STATE_BATT_FULL;

    if(annunState < ANNUN_STATE_BATT_LOW)
      annunState = ANNUN_STATE_BATT_LOW;

    if(pMe->m_bLowBatt)
      annunState = annunState | ANNUN_STATE_BLINK;

    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_BATT, annunState); /* battery */
  }
}


/*===========================================================================
FUNCTION CoreApp_AnimateBatteryAnnunciator

DESCRIPTION
  Animate the battery display while charging.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CoreApp_AnimateBatteryAnnunciator(void *pUser)
{
  CCoreApp *pMe = (CCoreApp*) pUser;
  if(pMe->m_nlastIcon == ANNUN_STATE_BATT_FULL)
    pMe->m_nlastIcon = ANNUN_STATE_BATT_LOW;
  else
    pMe->m_nlastIcon++;
  CoreApp_UpdateBatteryAnnunciator(pMe, pMe->m_nlastIcon);
  ISHELL_SetTimer(pMe->a.m_pIShell, 1000, CoreApp_AnimateBatteryAnnunciator, pUser);
}

/*===========================================================================
FUNCTION CoreAppDisplayBatteryStatus

DESCRIPTION
  Display the battery status.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreAppDisplayBatteryStatus(CCoreApp *pMe,   AEEBattStatus battStatus)
{
  AEEBattLevel level;

  switch(battStatus)
  {
    case AEEBATTSTATUS_NORMAL:
      pMe->m_bLowBatt = FALSE;
      RemoveLowBatteryWarning((void*) pMe);
      //Cancel the powerdown timer.
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) PowerDown, (void*) pMe);

      (void) IBATT_GetBattStatus(pMe->m_pIBatt, &level);
      CoreApp_UpdateBatteryAnnunciator(pMe, level.level+1);

      return TRUE;

    case AEEBATTSTATUS_POWERDOWN:
#ifdef FEATURE_RECORD_BATTERY_EXPIRY_TIME
      /* Record hhmmss/2 (i.e. a binary 24 hour timestamp with 2S resolution) in
         debug NV item SPARE_2 (a word, hence the /2)
      */
        clk_julian_type clk;
        nv_item_type nv_item;

        clk_rtc_get_time(&clk);

        nv_item.spare_2 = (word)(clk.hour * 60 * 60/2 + clk.minute * 60/2 + clk.second / 2);
        (void)ui_put_nv( NV_SPARE_2_I, &nv_item );
#endif //FEATURE_RECORD_BATTERY_EXPIRY_TIME

      uisnd_tone( UI_EARBEEP_SND, SND_ERR, 100 );  /* so power down */
      MSG_HIGH ("Battery Over",0,0,0);
      pMe->m_bUserPwrDown = TRUE;

      // Cast as void, since there's nothing to do if it fails
      (void)DisplayIdleMenuDlg(pMe);
      {
        uint32 nBufSize;
        AECHAR *pMessage;
        (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
                                    IDS_POWERDOWN, RESTYPE_STRING,(void*) -1, &nBufSize);

        nBufSize = (nBufSize+1)*sizeof(AECHAR);

        pMessage = (AECHAR*) MALLOC(nBufSize);
        if(pMessage == NULL)
        {
          CORE_ERR("CoreAppDisplayBatteryStatus: Unable to allocate in AEEBATTSTATUS_POWERDOWN", 0, 0, 0);
          return FALSE;
        }

        (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
             IDS_POWERDOWN, pMessage, (int)nBufSize);

        (void)SetIdleMenuText(pMessage);
        ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY) PowerDown,
                          (void*) pMe);
        FREE(pMessage);
      }
      return TRUE;

      case AEEBATTSTATUS_LOW:
        pMe->m_bLowBatt = TRUE;
        CoreApp_UpdateBatteryAnnunciator(pMe, ANNUN_STATE_BATT_LOW);

        if(!pMe->m_bSuspended)
        {
          uint32 nBufSize;
          AECHAR *pMessage;

          (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
                                IDS_LOW_BATTERY, RESTYPE_STRING, (void*)-1, &nBufSize);

          nBufSize = (nBufSize+1)*sizeof(AECHAR);

          pMessage = (AECHAR*) MALLOC(nBufSize);
          if(pMessage == NULL)
          {
            CORE_ERR("CoreAppDisplayBatteryStatus: Unable to allocate in AEEBATTSTATUS_LOW", 0, 0, 0);
            return FALSE;
          }

          (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
             IDS_LOW_BATTERY, pMessage, (int)nBufSize);

          (void)SetIdleMenuText(pMessage);
          ISHELL_SetTimer(pMe->a.m_pIShell, 2000, RemoveLowBatteryWarning, (void*) pMe);
          FREE(pMessage);
        }
        uisnd_tone( UI_EARBEEP_SND, SND_ERR, 100 );
        return TRUE;

      default:
        pMe->m_bLowBatt = FALSE;
        return FALSE;
    }



}
/*===========================================================================
FUNCTION ExternalPwrOn

DESCRIPTION
  Upon receiving external power on event, this function updates the
  corresponding info.

DEPENDENCIES
  UpdateBacklight

SIDE EFFECTS
  None
===========================================================================*/
static void ExternalPwrOn(CCoreApp * pMe)
{
  MSG_HIGH("EXT POWER ON",0,0,0);
  UpdateBacklight(pMe);
  uisnd_snd( UI_EXT_PWR_SND, SND_EXT_PWR_ON );
  if(pMe->m_bLowBatt)
   RemoveLowBatteryWarning((void*) pMe);

  // The Annunciator should not blink when the charger has been plugged in
  pMe->m_bLowBatt = FALSE;
  CoreApp_AnimateBatteryAnnunciator(pMe);
} /* End ExternalPwrOn */



/*===========================================================================
FUNCTION ExternalPwrOff

DESCRIPTION
  Upon receiving external power off event, this function updates the
  corresponding info.

DEPENDENCIES
  uivr_exit_standby, DisplayIdleMenuDlg, PowerDown, UpdateBacklight

SIDE EFFECTS
  None
===========================================================================*/
static void ExternalPwrOff(CCoreApp *pMe)
{
  AEEBattLevel level;

  if(HS_EXT_PWR_ON())
    return;

  MSG_HIGH("EXTERNAL POWER OFF",0,0,0);

  //Get the current battery level.
  ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_AnimateBatteryAnnunciator, (void*) pMe);

  (void)CoreAppDisplayBatteryStatus(pMe, IBATT_GetBattStatus(pMe->m_pIBatt, &level));

  if (uisnd_is_device_attached(SND_DEVICE_HFK) && !ui_is_in_call() ) {
    /* we are in cradle ( i.e attached to car kit )   */
    /* There is no call up. We just lost external     */
    /* power. Can it be that ignition is turned off ? */
    if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL) {
      (void) DisplayIdleMenuDlg(pMe);
    }
    pMe->m_bUserPwrDown = TRUE;
    PowerDown(pMe);
  }
  else if(pMe->m_bLPM)
  {
    pMe->m_bUserPwrDown = TRUE;
    PowerDown(pMe);
  }
  UpdateBacklight(pMe);
  uisnd_snd( UI_EXT_PWR_SND, SND_EXT_PWR_OFF );

} /*End ExternalPwrOff */

/*===========================================================================
FUNCTION HeadsetOn

DESCRIPTION
  Upon receiving headset on event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void HeadsetOn(CCoreApp * pMe)
{
  boolean headSetOn = TRUE;
  int     nRetVal;
  AEEDeviceNotify devnotify;

  MSG_HIGH("PHONE in Mono Headset",0,0,0);

  devnotify.wParam = TRUE;
  AEE_SEND_HEADSET_EVT(&devnotify);

  if ((pMe == NULL) || (pMe->m_pIConfig == NULL))
  {
    MSG_ERROR("NULL pointer, pMe=0x%x", pMe, 0, 0);
    return;
  }

  /* Change the audio path */
  uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_ENABLED);

  /*Also set the headset in ICONFIG because OEMCall will use it if WCDMA is defined*/
  nRetVal = ICONFIG_SetItem(pMe->m_pIConfig, CFGI_HEADSET, &headSetOn, 1);
  if (nRetVal != SUCCESS)
  {
    MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
  }

#ifdef FEATURE_TTY
  /* set tty device attached*/
  {  
    uint32 tty = CoreTask_GetTTY();

    if (tty == UI_TTY_FULL)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_HEADSET, UISND_DEV_ENABLED);
    }
    else if (tty == UI_TTY_HEAR)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_HCO, UISND_DEV_ENABLED);
    }
    else if (tty == UI_TTY_TALK)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_VCO, UISND_DEV_ENABLED);
    }
  }

  /* set the volume levels if it is a TTY device */
  (void)GetTTY(pMe);
  if(pMe->m_byTTY == OEMNV_TTY_FULL)
  {
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_HEADSET );
  } else if (pMe->m_byTTY == OEMNV_TTY_HEAR){
    uisnd_tty_vol( GetEarpieceVolume(pMe), SND_DEVICE_TTY_HCO );
  }else if (pMe->m_byTTY == OEMNV_TTY_TALK){
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_VCO );
  }
#endif /* FEATURE_TTY */
  uisnd_set_device_auto(NULL,NULL);
  InitializeVolumes(pMe);
} /* End HeadsetOn */


/*===========================================================================
FUNCTION StereoHeadsetOn

DESCRIPTION
  Upon receiving stereo headset on event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void StereoHeadsetOn(CCoreApp * pMe)
{
  boolean stereoHeadSetOn = TRUE;
  int     nRetVal;
  AEEDeviceNotify devnotify;

  MSG_HIGH("PHONE in Stereo Headset",0,0,0);

  devnotify.wParam = TRUE;
  AEE_SEND_HEADSET_EVT(&devnotify);

  if ((pMe == NULL) || (pMe->m_pIConfig == NULL))
  {
    MSG_ERROR("NULL pointer, pMe=0x%x", pMe, 0, 0);
    return;
  }

  /* Change the audio path */
  uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_ENABLED);

  /*Also set the headset in ICONFIG because OEMCall will use it if WCDMA is defined*/
  nRetVal = ICONFIG_SetItem(pMe->m_pIConfig, CFGI_STEREO_HEADSET,
                            &stereoHeadSetOn, 1);
  if (nRetVal != SUCCESS)
  {
    MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
  }

#ifdef FEATURE_TTY
  /* set tty device attached*/
  {  
    uint32 tty = CoreTask_GetTTY();

    if (tty == UI_TTY_FULL)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_HEADSET, UISND_DEV_ENABLED);
    }
    else if (tty == UI_TTY_HEAR)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_HCO, UISND_DEV_ENABLED);
    }
    else if (tty == UI_TTY_TALK)
    {
      uisnd_set_device_status(SND_DEVICE_TTY_VCO, UISND_DEV_ENABLED);
    }
  }

  /* set the volume levels if it is a TTY device */
  (void)GetTTY(pMe);
  if(pMe->m_byTTY == OEMNV_TTY_FULL)
  {
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_HEADSET );
  } else if (pMe->m_byTTY == OEMNV_TTY_HEAR){
    uisnd_tty_vol( GetEarpieceVolume(pMe), SND_DEVICE_TTY_HCO );
  }else if (pMe->m_byTTY == OEMNV_TTY_TALK){
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_VCO );
  }
#endif /* FEATURE_TTY */

  uisnd_set_device_auto(NULL,NULL);
  InitializeVolumes(pMe);
} /* End HeadsetOn */


/*===========================================================================
FUNCTION HeadsetOff

DESCRIPTION
  Upon receiving headset off event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void HeadsetOff(CCoreApp *pMe)
{
   boolean headSetOn = FALSE;
   int     nRetVal;
   AEEDeviceNotify devnotify;

   MSG_HIGH("PHONE not in Headset",0,0,0);

   devnotify.wParam = FALSE;
   AEE_SEND_HEADSET_EVT(&devnotify);

   if ((pMe == NULL) || (pMe->m_pIConfig == NULL))
   {
      MSG_ERROR("NULL pointer, pMe=0x%x", pMe, 0, 0);
      return;
   }

   /* Change the audio path */
   uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_UNATTACHED);
   uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_UNATTACHED);

   /*Also set the headset in ICONFIG because OEMCall will use it if WCDMA is defined*/
   nRetVal = ICONFIG_SetItem(pMe->m_pIConfig, CFGI_HEADSET, &headSetOn, 1);
   if (nRetVal != SUCCESS)
   {
      MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
   }

   nRetVal = ICONFIG_SetItem(pMe->m_pIConfig, CFGI_STEREO_HEADSET, &headSetOn, 1);
   if (nRetVal != SUCCESS)
   {
      MSG_HIGH("Failed to set config item, %d", nRetVal, 0, 0);
   }

#ifdef FEATURE_TTY
   uisnd_set_device_status(SND_DEVICE_TTY_HEADSET, UISND_DEV_UNATTACHED);
   uisnd_set_device_status(SND_DEVICE_TTY_HCO, UISND_DEV_UNATTACHED);
   uisnd_set_device_status(SND_DEVICE_TTY_VCO, UISND_DEV_UNATTACHED);
#endif

   uisnd_set_device_auto(NULL,NULL);
   InitializeVolumes(pMe);
} /*End HeadsetOff */

/*===========================================================================
FUNCTION PhoneOnHook

DESCRIPTION
  Upon receiving on hook event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void PhoneOnHook(CCoreApp *pMe)
{
  MSG_HIGH("PHONE On HOOK",0,0,0);

  /* Change the audio path */
  uisnd_set_device_status(SND_DEVICE_HFK, UISND_DEV_ENABLED);
  uisnd_set_device_auto(NULL,NULL);
  InitializeVolumes(pMe);
} /* End PhoneOnHook */



/*===========================================================================
FUNCTION PhoneOffHook

DESCRIPTION
  Upon receiving off hook event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void PhoneOffHook(CCoreApp *pMe)
{
  MSG_HIGH("PHONE OFF HOOK",0,0,0);

  /* Change the audio path */
  uisnd_set_device_status(SND_DEVICE_HFK, UISND_DEV_UNATTACHED);
  uisnd_set_device_auto(NULL,NULL);
  InitializeVolumes(pMe);
} /* End PhoneOffHook */



/*===========================================================================
FUNCTION TurnOnUSB_hfk

DESCRIPTION
  Upon receiving usb hfk On event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void TurnOnUSB_hfk(CCoreApp *pMe)
{
  if(GetUSBHFKAttached() || GetStereoUSBHFKAttached()) {
    MSG_ERROR("Tried to turn on USB HFK, but one is already attached.",0,0,0);
    return;
  }

  MSG_HIGH("USB HFK ON",0,0,0);
  
  //Set up variables so sound device detection can find the HFK
  SetUSBHFKAttached(TRUE);
} /* End TurnOnUSB_hfk */

/*===========================================================================
FUNCTION TurnOnStereoUSB_hfk

DESCRIPTION
  Upon receiving stereo usb hfk On event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void TurnOnStereoUSB_hfk(CCoreApp *pMe)
{
  if(GetUSBHFKAttached() || GetStereoUSBHFKAttached()) {
    MSG_ERROR("Tried to turn on Stereo USB HFK, but one is already attached.",0,0,0);
    return;
  }

  MSG_HIGH("Stereo USB HFK ON",0,0,0);
  
  //Set up variables so sound device detection can find the HFK
  SetStereoUSBHFKAttached(TRUE);
} /* End TurnOnStereoUSB_hfk */



/*===========================================================================
FUNCTION TurnOffUSB_hfk

DESCRIPTION
  Upon receiving usb hfk Off event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void TurnOffUSB_hfk(CCoreApp *pMe)
{
  if(!GetUSBHFKAttached() && !GetStereoUSBHFKAttached()) {
    MSG_ERROR("Tried to turn off USB HFK, but none are attached.",0,0,0);
    return;
  }

  MSG_HIGH("USB HFK OFF",0,0,0);

  SetUSBHFKAttached(FALSE);
  SetStereoUSBHFKAttached(FALSE);
} /* End TurnOffUSB_hfk */

/*===========================================================================
FUNCTION ReturnToCradleUSB_hfk

DESCRIPTION
  Upon receiving usb hfk Return to cradle event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void ReturnToCradleUSB_hfk(CCoreApp *pMe)
{
  if(!GetUSBHFKAttached() && !GetStereoUSBHFKAttached()) {
    MSG_ERROR("Got USB HFK return to cradle, but no USB HFK attached.",0,0,0);
    return;
  }

  MSG_HIGH("USB HFK RETURN TO CRADLE",0,0,0);

#ifdef FEATURE_USB_CARKIT
  if (GetUSBHFKAttached())
  {  
    uisnd_set_device_status(SND_DEVICE_USB, UISND_DEV_ENABLED);
  }
  else
  {  
    uisnd_set_device_status(SND_DEVICE_USB, UISND_DEV_UNATTACHED);
  }

  if (GetStereoUSBHFKAttached())
  {
    uisnd_set_device_status(SND_DEVICE_STEREO_USB, UISND_DEV_ENABLED);
  }
  else
  {
    uisnd_set_device_status(SND_DEVICE_STEREO_USB, UISND_DEV_UNATTACHED);
  }
#endif /* FEATURE_USB_CARKIT */

  uisnd_set_device_auto(NULL,NULL);

  InitializeVolumes(pMe);
} /* End ReturnToCradleUSB_hfk */



/*===========================================================================
FUNCTION RemoveFromCradleUSB_hfk

DESCRIPTION
  Upon receiving usb hfk remove from cradle event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void RemoveFromCradleUSB_hfk(CCoreApp *pMe)
{
  //Don't check attached here, because on disconnect, remove from cradle comes
  //after unavailable.
  MSG_HIGH("USB HFK REMOVE FROM CRADLE",0,0,0);

#ifdef FEATURE_USB_CARKIT
  uisnd_set_device_status(SND_DEVICE_USB, UISND_DEV_UNATTACHED);
  uisnd_set_device_status(SND_DEVICE_STEREO_USB, UISND_DEV_UNATTACHED);
#endif /* FEATURE_USB_CARKIT */

  uisnd_set_device_auto(NULL,NULL);

  InitializeVolumes(pMe);
} /* End RemoveFromCradleUSB_hfk */


/*===========================================================================
FUNCTION TurnOnSdac

DESCRIPTION
  Upon receiving sdac On event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void TurnOnSdac(CCoreApp * pMe)
{
   MSG_HIGH("SDAC ON",0,0,0);
   
   if (!ui_is_in_call() || ui_curr_call_is_data())
     uisnd_set_device_status(SND_DEVICE_SDAC, UISND_DEV_ENABLED);
   else
     uisnd_set_device_status(SND_DEVICE_SDAC, UISND_DEV_UNATTACHED);

   uisnd_set_device_auto(NULL,NULL);
   InitializeVolumes(pMe);
} /* End TurnOnSdac */



/*===========================================================================
FUNCTION TurnOffSdac

DESCRIPTION
  Upon receiving sdac Off event, this function updates the
  corresponding info.

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static void TurnOffSdac(CCoreApp *pMe)
{
  MSG_HIGH("SDAC OFF",0,0,0);

  uisnd_set_device_status(SND_DEVICE_SDAC, UISND_DEV_UNATTACHED);
  uisnd_set_device_auto(NULL,NULL);
  InitializeVolumes(pMe);
} /* End TurnOffSdac */


/*===========================================================================
FUNCTION StartCoreApp

DESCRIPTION
  Start the usual idle menu core app

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void StartCoreApp (CCoreApp *pMe)
{
  /* Start timer to display time */
  DisplayTimeUpdateScreen(pMe);
  ISHELL_SetTimer(pMe->a.m_pIShell,
                      1000,
                      (PFNNOTIFY) DisplayTimeWithRefresh,
                      pMe);

#ifdef FEATURE_TEMPERATURE_MONITOR
  /* Start timer to read various thermo sensors */
  ISHELL_SetTimer(pMe->a.m_pIShell,
                      60000,
                      (PFNNOTIFY) GetThermoSensors,
                      pMe);
#endif
} /* End StartCoreApp */

#ifdef FEATURE_SECONDARY_DISPLAY
/*===========================================================================
FUNCTION CoreApp_InitSecondaryDisplay

DESCRIPTION
  Initializes the secondary display and all associated values

RETURN VALUE
  boolean:  TRUE if the secondary display could be initialized; FALSE otherwise

DEPENDENCIES
  none

SIDE EFFECTS
  None

COMMENTS
  Note that this function will always return FALSE for devices that only
  support a single display.  As a result, a FALSE return value should not
  necessarily indicate an app initialization failure.

===========================================================================*/
boolean CoreApp_InitSecondaryDisplay(CCoreApp *pMe)
{
  int nReturnStatus;
  boolean     bIsFlipOpen;
  int         nFlipOpenSize = sizeof(bIsFlipOpen);
  int         nDevInfoSuccess;
  AEEBitmapInfo bmpInfo;           // Bitmap info for screen resolution
  AEERect  sStaticRect;            // Location of the static text
  boolean bRetVal = FALSE;         // Function return value

  /* Create secondary display.  Use a do..while loop to avoid nesting
   * the logic too deeply.
   */
  do
  {
    // Create everything needed for the display management
    nReturnStatus = IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay,
                                             &(pMe->m_pIBitmap));
    if ((SUCCESS != nReturnStatus) || (NULL == pMe->m_pIBitmap))
    {
      CORE_ERR("Unable to get primary display bitmap.", 0, 0, 0);
      break;
    }

    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                          AEECLSID_DISPLAY2,
                                          (void **) &pMe->m_pIDisp2);
    if (SUCCESS != nReturnStatus)
    {
      MSG_MED("Unable to create secondary display.", 0, 0, 0);
      bRetVal = TRUE;
      break;
    }

    nReturnStatus = IDISPLAY_GetDeviceBitmap(pMe->m_pIDisp2,
                                             &(pMe->m_pIBitmap2));
    if ((SUCCESS != nReturnStatus) || (NULL == pMe->m_pIBitmap2))
    {
      CORE_ERR("Unable to get secondary display bitmap.", 0, 0, 0);
      break;
    }

    pMe->m_pszTimeDate = NULL;
    IDISPLAY_MakeDefault(pMe->m_pIDisp2);
    IDISPLAY_SetColor(pMe->m_pIDisp2, CLR_USER_BACKGROUND, RGB_WHITE);

    // Create Static control for time/date on secondary display
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                          (void **)&pMe->m_pTimeDate);
    IDISPLAY_MakeDefault(pMe->a.m_pIDisplay);
    if ((SUCCESS != nReturnStatus) || (pMe->m_pTimeDate == NULL))
    {
       CORE_ERR("Unable to create TimeDate static control: %d",
                 nReturnStatus, 0, 0);
       break;
    }

    /* We want to center the text in the middle of the screen, so
     * we need to set its display rectangle correctly.
     */
    (void) IBITMAP_GetInfo(pMe->m_pIBitmap2, &bmpInfo, sizeof(AEEBitmapInfo));
    sStaticRect.x = 0;
    sStaticRect.y = 0;
    sStaticRect.dx = (int16)(bmpInfo.cx);
    sStaticRect.dy = (int16)(bmpInfo.cy);
    ISTATIC_SetRect(pMe->m_pTimeDate, &sStaticRect);
    ISTATIC_SetProperties(pMe->m_pTimeDate, ST_CENTERTEXT | ST_MIDDLETEXT);

    pMe->m_pszTimeDate = (AECHAR *)MALLOC((TIMEDATE_BUFFER_SIZE + 1) *
                                          sizeof(AECHAR));

    if (NULL == pMe->m_pszTimeDate)
    {
      CORE_ERR("Unable to allocate time/date string", 0, 0, 0);
      break;
    }

    pMe->m_pszTimeDate[0] = (AECHAR)'\0';

    // Now, determine which display should be active and set it up.
    /* Check clamshell status before turning the display on.  If the
     * clamshell is closed, turn on the secondary display.  If not,
     * turn on the primary.
     */
    nDevInfoSuccess = ISHELL_GetDeviceInfoEx(pMe->a.m_pIShell,
                                             AEE_DEVICESTATE_FLIP_OPEN,
                                             (void *)&bIsFlipOpen,
                                             &nFlipOpenSize);
    if (nDevInfoSuccess != SUCCESS)
    {
      /* If we couldn't get the status, error and default to
       * clamshell open.
       */
      MSG_MED("Unable to get clamshell status for backlight: %d.",
              nDevInfoSuccess, 0, 0);
      bIsFlipOpen = TRUE;
    }
    if ((bIsFlipOpen == TRUE)
#ifdef T_FFA6550_SS
        || (hs_query_articulated_state() == TRUE)
#endif /* T_FFA6550_SS */
        )
    {
      if (ToggleDisplays(pMe, TRUE))
      {
        bRetVal = TRUE;
      }
    }
    else
    {
      if (ToggleDisplays(pMe, FALSE))
      {
        bRetVal = TRUE;
      }
    }
  } while (0);

  if (! bRetVal)
  {
    if (pMe->m_pIDisp2)
    {
      IDISPLAY_Release(pMe->m_pIDisp2);
      pMe->m_pIDisp2 = NULL;
    }
    if (pMe->m_pIBitmap)
    {
      IBITMAP_Release(pMe->m_pIBitmap);
      pMe->m_pIBitmap = NULL;
    }
    if (pMe->m_pIBitmap2)
    {
      IBITMAP_Release(pMe->m_pIBitmap2);
      pMe->m_pIBitmap2 = NULL;
    }
    if (pMe->m_pTimeDate)
    {
      ISTATIC_Release(pMe->m_pTimeDate);
      pMe->m_pTimeDate = NULL;
    }
    FREEIF(pMe->m_pszTimeDate);
    pMe->m_pszTimeDate = NULL;
  }

  return bRetVal;

}
#endif //FEATURE_SECONDARY_DISPLAY


/*===========================================================================
FUNCTION CoreApp_SelectResFile

DESCRIPTION
  This function selects the appropriate resource file for CoreApp to use
  based on the size of the primary display.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
int CoreApp_SelectResFile(CCoreApp *pMe)
{
  IFileMgr      *pIFileMgr = NULL;
  IFile         *pIFile = NULL;
  int            nReturnStatus;
  uint32          nScreenSize = COREAPP_SUB_QVGA_RES;

  nScreenSize = GetPrimaryScreenSize();

  /* The algorithm for selecting the resource file is as follows:
   * 1) If the primary display is VGA or larger in either portrait or
   *    landscape orientation, attempt to use the VGA resource file.
   * 2) If the primary display is QVGA or larger in either portrait or
   *    landscape orientation, attempt to use the QVGA resource file.
   * 3) Otherwise, use the base (QCIF) resource file.
   */
  if (nScreenSize == COREAPP_VGA_RES) {
    /* The primary display is VGA or larger. */
    gpszResourceFile = COREAPP_VGA_RES_FILE;
//lint -save -e10 -e40 -e63 Suppress lint complaints about undeclared variable
    gaiCoreApp.pszMIF = COREAPP_VGA_RES_FILE;
//lint -restore
  }
  else if (nScreenSize == COREAPP_QVGA_RES) {
    /* The primary display is at least QVGA. */
    gpszResourceFile = COREAPP_QVGA_RES_FILE;
//lint -save -e10 -e40 -e63 Suppress lint complaints about undeclared variable
    gaiCoreApp.pszMIF = COREAPP_QVGA_RES_FILE;
//lint -restore
  }
  else {
    /* The primary display is smaller than QVGA. */
    gpszResourceFile = COREAPP_RES_FILE;
//lint -save -e10 -e40 -e63 Suppress lint complaints about undeclared variable
    gaiCoreApp.pszMIF = COREAPP_RES_FILE;
//lint -restore
  }

  do {
    /* Before indicating success, we want to verify that the file exists
     * and is readable.  If the file does not exist or isn't readable, we'll
     * fail.  If we cannot verify, we'll assume success, but log an error.
     */
    nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                                          (void **) &pIFileMgr);
    if (nReturnStatus != SUCCESS) {
      CORE_MSG("Unable to create file mgr, %d", nReturnStatus, 0, 0);
      break;
    }
  
    nReturnStatus = IFILEMGR_Test(pIFileMgr, gpszResourceFile);
    if (nReturnStatus != SUCCESS) {
      CORE_ERR("Resource file does not exist, %d, size=%d", nReturnStatus,
               nScreenSize, 0);
      break;
    }

    pIFile = IFILEMGR_OpenFile(pIFileMgr, gpszResourceFile, _OFM_READ);
    if (! pIFile)
    {
      nReturnStatus = EFAILED;
      CORE_ERR("Resource file cannot be read, %d, size=%d",
               IFILEMGR_GetLastError(pIFileMgr), nScreenSize, 0);
      break;
    }

  } while (0);

  if (pIFile) {
    (void)IFILE_Release(pIFile);
  }

  if (pIFileMgr) {
    (void)IFILEMGR_Release(pIFileMgr);
  }

  return nReturnStatus;

}


/*===========================================================================
FUNCTION CoreApp_InitAppData

DESCRIPTION
  Initializing the application data member

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_InitAppData(IApplet* po)
{
  CCoreApp *pMe = (CCoreApp*)po;
  AEEDeviceInfo di;
  int nReturnStatus;
  boolean bShowManPLMNSel = TRUE;
#ifdef FEATURE_NEW_ICARD
  AEECardPinStatus nv_status;
#else
  AEEPinStatus nv_status;
#endif

  nReturnStatus = CoreApp_SelectResFile(pMe);
  if (nReturnStatus != SUCCESS)
  {
    return FALSE;
  }

  pMe->m_pSplashImage = NULL;     /* Initialize Splash Screen image */

  /*The app has not really started at this point*/
  pMe->m_bSuspended = TRUE;      /* Initialize <suspend?> to TRUE */

  pMe->m_bExit = FALSE;           /* Initialize the Exit flag to FALSE */

  /* Create annunciator */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_ANNUNCIATOR,
                               (void **) &pMe->m_pIAnn);
  if(nReturnStatus != SUCCESS) {
    (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
    return FALSE;
  }

  if (pMe->m_pIAnn == NULL) { /* error */
    return FALSE;
  }

#ifdef FEATURE_SECONDARY_DISPLAY
  if (! CoreApp_InitSecondaryDisplay(pMe))
  {
    /* If we couldn't init the secondary display, log it, but don't
     * fail.  Some targets only have one display
     */
    CORE_ERR("Unable to init secondary display", 0, 0, 0);
  }
#endif

  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CARD,
                               (void **) &pMe->m_pICard);
  if(nReturnStatus != SUCCESS) {
    (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  if (pMe->m_pICard == NULL) { /* error */
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

  /* Create the Supplementary Serivce object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_SUPPSVC,
                               (void **) &pMe->m_pISUPPSVC);

  if(nReturnStatus != SUCCESS) {
    (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  if (pMe->m_pISUPPSVC == NULL) { /* error */
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

#endif /* F_WCDMA || F_GSM */

  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_BATT, (void**) &pMe->m_pIBatt);

  if(nReturnStatus != SUCCESS || pMe->m_pIBatt == NULL)
  {
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }


#ifdef FEATURE_APP_TEST_AUTOMATION
  // create IDiag object, core app doesn't need to register
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DIAG,
                           (void **)&pMe->m_pIDiag) != SUCCESS)
  {
    CORE_ERR("IDiag Create Failed", 0, 0, 0);
  }

#endif

  /* Create the Call Manager object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CM,
                               (void **) &pMe->m_pICM);

  if(nReturnStatus != SUCCESS) {
    (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
    CoreApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  if (pMe->m_pICM == NULL) { /* error */
    CoreApp_FreeAppData((IApplet*)  pMe);
    return FALSE;
  }

  /* Create the IConfig Serivce object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CONFIG,
                               (void **) &pMe->m_pIConfig);
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE || pMe->m_pIConfig == NULL)
  {
    CORE_ERR("Unable to create IConfig Object", 0, 0, 0);
    pMe->m_pIConfig = NULL;
  }
  /* Initializing cxScreen and cyScreen */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  pMe->m_cxScreen = di.cxScreen;
  pMe->m_cyScreen = di.cyScreen;

  /* Allocate buffers for various string pointers */
  pMe->m_pszBanner = (AECHAR *) MALLOC ((BANNER_BUFFER_SIZE + 1)*sizeof(AECHAR));
  pMe->m_pszIdleMenuText = (AECHAR *) MALLOC ((MAX_BROADCAST_MSG_SIZE + 1)*sizeof(AECHAR));

  pMe->m_pszTime   = (AECHAR *) MALLOC ((TIME_BUFFER_SIZE + 1)*sizeof(AECHAR));
  (void) STRTOWSTR ("\0", pMe->m_pszTime, (TIME_BUFFER_SIZE+1)*sizeof(AECHAR));
  pMe->m_pszLastTime   = (AECHAR *) MALLOC ((TIME_BUFFER_SIZE + 1)*sizeof(AECHAR));
  (void) STRTOWSTR ("\0", pMe->m_pszLastTime, (TIME_BUFFER_SIZE+1)*sizeof(AECHAR));

  pMe->m_pszDialString = (AECHAR *) MALLOC ((NUMBER_BUFFER_SIZE +1)*sizeof(AECHAR));

#ifdef FEATURE_MMGSDI
  pMe->m_nCardStatus = AEECARD_NOT_READY;
#else
  pMe->m_nCardStatus = AEECARD_NO_CARD;
#endif

  pMe->m_sPIN.code = (AECHAR*)MALLOC((AEECARD_PIN_LEN+1)*sizeof(AECHAR));
  pMe->m_sPUK.code = (AECHAR*)MALLOC((AEECARD_PIN_LEN+1)*sizeof(AECHAR));
  pMe->m_sPIN1_lock_state = AEECARD_PIN_UNKNOWN;
  pMe->m_sPIN2_lock_state = AEECARD_PIN_UNKNOWN;
  GetNVLockCodeStatus(&nv_status);
  pMe->m_sNV_lock_state = nv_status.lock_state;
  pMe->m_bPinVerified = FALSE;
  pMe->m_bConfigSent = FALSE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  pMe->m_nCardStatus_slot2 = AEECARD_NOT_READY;
  pMe->m_sPIN1_slot2_lock_state = AEECARD_PIN_UNKNOWN;
  pMe->m_sPIN2_slot2_lock_state = AEECARD_PIN_UNKNOWN;
  pMe->m_byGSMUserSlotPref = 0;
  pMe->m_byCDMAUserSlotPref = 0;
  pMe->m_bSlotChangePending = FALSE;
  pMe->m_bUserMadeFirstSelSub = FALSE;
  pMe->m_bTestAutomationMode = FALSE;
#endif
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  pMe->m_nPersoStatus = AEECARD_PERSO_NO_EVENT;
#endif
  pMe->m_bySubscriptionSent = COREAPP_NOT_READY_SUBSCRIPTION_MASK;

  pMe->m_MessageID = 0;
  pMe->m_pszMsgString = NULL;

  pMe->m_pDetailedPLMNList = NULL;
  pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
  /* Set security mask to FALSE for the
  ** ICARD event, CM event received
  ** pin1 verified and NV verified
  */
  pMe->m_dwSecurityMask = 0x0000;

#ifndef FEATURE_MMGSDI
  pMe->m_rtreControl = AEECM_RTRE_CONTROL_NV;
  pMe->m_rtreConfig  = AEECM_RTRE_CONFIG_NV_ONLY;
#else
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  pMe->m_rtreControl = AEECM_RTRE_CONTROL_NONE;
  pMe->m_rtreConfig  = AEECM_RTRE_CONFIG_NONE;
#else
  pMe->m_rtreControl = AEECM_RTRE_CONTROL_RUIM;
  pMe->m_rtreConfig  = AEECM_RTRE_CONFIG_RUIM_ONLY;
#endif /*FEATURE_UIM_RUN_TIME_ENABLE */
#endif /*FEATURE_MMGSDI */

#ifndef FEATURE_NEW_ICARD
  /* initialize to SUCCESS */
  pMe->m_sPin_rsp_data.pin_cmd_status = SUCCESS;
  pMe->m_sGeneric_rsp_data.cmd_status = SUCCESS;

#endif /*FEATURE_NEW_ICARD */

  /* do we need this eventually? */
  // This will set the RSSI Delta reporting threshold. Use 5 dBm
  ICM_SetRSSIDeltaThreshold(pMe->m_pICM,
                         5);
  /* Request the lastest phone and serving system information */
  ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_phInfo, sizeof(AEECMPhInfo));

  ICM_GetSSInfo(pMe->m_pICM,
                    &pMe->m_ssInfo, sizeof(AEECMSSInfo));
#ifdef FEATURE_USRSYSCLOCK
  CoreUtils_UpdatePLMNCFGI(pMe);
#endif //FEATURE_USRSYSCLOCK

#ifdef FEATURE_HDR
  pMe->m_bHDRDisplay = FALSE;
#endif

  pMe->m_bLowBatt = FALSE;

  pMe->m_bVR = TRUE;
  pMe->m_bExtPwr = FALSE;
  pMe->m_bLPM = FALSE;
  pMe->m_bNetSelErr = FALSE;
  pMe->m_bNetSel = FALSE;
  pMe->m_pCopyDetailedPLMNList = NULL;
  pMe->m_bProvisioned = FALSE;
  pMe->m_bLPMTimer = FALSE; // Timer to reset the phone and go into LPM
  pMe->mbReadFiles = FALSE;

  if (pMe->m_pIConfig != NULL)
  {
    (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_MANUAL_PLMN_SEL_ALLOWED,
                          (void *)&bShowManPLMNSel, sizeof(bShowManPLMNSel));
  }

#ifdef FEATURE_GSTK
  /*Create an instance of ISTK */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STK,
                                        (void **)&pMe->m_pISTK);
  if ((nReturnStatus != SUCCESS) || (pMe->m_pISTK == NULL))
  {
    MSG_ERROR("Unable to create for ISTK, 0x%x", nReturnStatus, 0, 0);
  }
#endif

#ifdef FEATURE_USRSYSCLOCK
  /* IUSRSYSCLOCK object */
  pMe->m_pIUsrSysClock = NULL;
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_USRSYSCLOCK,
                                        (void **)&pMe->m_pIUsrSysClock);
  if ((nReturnStatus != SUCCESS) || (pMe->m_pIUsrSysClock == NULL))
  {
    pMe->m_pIUsrSysClock = NULL;
    MSG_ERROR("Unable to create for IUSRSYSCLOCK, 0x%x", nReturnStatus, 0, 0);
  }

#endif // FEATURE_USRSYSCLOCK

  pMe->m_pIdleDialog = NULL;
  AppComFunc_SelectFonts(pMe->a.m_pIDisplay);

#if defined(FEATURE_MMOVERLAY)
  /* Background image has neither been rendered nor displayed. */
  pMe->m_bBgImageRendered = FALSE;
  pMe->m_bBgImageDisplayed = FALSE;
  pMe->m_pBgImage = NULL;
  pMe->m_pBgImgOverlay = NULL;
  pMe->m_pFgImgOverlay = NULL;
#endif /*  FEATURE_MMOVERLAY */

  return TRUE;

} /* End CoreApp_InitAppData */



/*===========================================================================
FUNCTION InitSound

DESCRIPTION
  initialize sound related menu and features.

DEPENDENCIES
  the corresponding Get/Set methods for Sound related features

SIDE EFFECTS
  None
===========================================================================*/
static void InitSound(CCoreApp *pMe)
{
  int i;
#ifdef FEATURE_TTY
  nv_item_type nvi;
#ifdef PLATFORM_LTK
  nvi.ear_level = 0;
#endif /* PLATFORM_LTK */
#endif /* FEATURE_TTY */

  /* silence the sound driver during initialization */
  uisnd_tone_stop();

  /* Mute the speaker and microphone, select audio device */
  uisnd_set_device_status(SND_DEVICE_HFK, UISND_DEV_UNATTACHED);

  uisnd_set_mute(UISND_MUTE_MUTED, UISND_MUTE_MUTED, NULL, NULL);
  uisnd_set_device_auto(NULL,NULL);

  pMe->m_bMMRinger = FALSE;
  pMe->m_bMMVolumeSetting = FALSE;
  for (i=0; i<MM_RINGER_MAX_FILES; i++) {
    pMe->m_pszFileArray[i] = NULL;
  }
  pMe->m_pszMMRingerFile = NULL;

  InitializeVolumes (pMe);

  (void)GetRingerType(pMe);
  (void)GetBeepLength(pMe);         /* beep length */

  (void)GetSmartSound(pMe);         /* smart sound */
  (void)GetServiceAlert(pMe);       /* service alert */

  /* Initialize silenceall from NV */
  (void)GetSilenceAll(pMe);

  pMe->m_bMute = FALSE;                  /* initialize to unmute */

#ifdef FEATURE_TTY
  (void)GetTTY(pMe);
  (void)SetTTY(pMe, pMe->m_byTTY);

  if(pMe->m_byTTY != OEMNV_TTY_OFF)
  {
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_HEADSET );
    uisnd_tty_vol( nvi.ear_level, SND_DEVICE_TTY_HCO );
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_VCO );
  }
#endif /* FEATURE_TTY */

  pMe->m_bDemoAlert = FALSE;                /* not having a demo alert now */

  /* power up sound is the message waiting chirp */
  uisnd_tone(UI_PWRUP_SND, SND_MSG_WAITING, 200);

} /* End InitSound */



/*===========================================================================
FUNCTION InitMenuSettings

DESCRIPTION
  initialize menu (except those for sound) and phone hardward variables.

DEPENDENCIES
  Various Get and Set method for the variables in CoreUtils.c

SIDE EFFECTS
  None
===========================================================================*/
static void InitMenuSettings(CCoreApp *pMe)
{

  /*==========================
          Non NV items
  ==========================*/

  /*==========================
           NV items
  ==========================*/

  RestoreStoredContrast(pMe);       /* contrast */

#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
  SetBackLightIntensity(pMe, GetBackLightIntensity(pMe)); /* Intensity */
#endif
  SetLCDValue(pMe, GetLCDValue(pMe));                     /* LCD Value */
  SetBackLightDuration(pMe, GetBackLightDuration(pMe));   /* backlight */
  (void)GetAutoHyphen(pMe);         /* auto hyphen */
  (void)GetPrivacy(pMe);            /* privacy */
  (void)GetAutoRetry(pMe);          /* auto retry */
  (void)GetAutoAnswer(pMe);         /* auto answer */
  (void)GetOneTouch(pMe);           /* one touch */
  (void)GetBanner(pMe);             /* banner */

  pMe->m_nKeyguardUnlockCodePos = 0;  /* keyguard unlock code check */

  pMe->m_bPwr = TRUE; /* phone powered up */

  /* Initialize other variables */
  pMe->m_bUserPwrDown = FALSE;

  /* Let UI know its ready to receive keys */
  EnableUIKeys (TRUE);

} /* End InitMenuSettings */

/*===========================================================================
FUNCTION Initialization

DESCRIPTION
  initialize menu variables, cm, gsdi, sms, phone book etc.

DEPENDENCIES
  the corresponding initialization functions

SIDE EFFECTS
  None
===========================================================================*/
static boolean Initialization(CCoreApp *pMe)
{
  uint32 dwMask;
#if defined (FEATURE_LPM)
  #if defined (FEATURE_LPM_TESTING_INTERNAL)
    boolean bSendLPM = FALSE;
  #else
    boolean bSendLPM = TRUE;
  #endif
#endif

#if (defined (FEATURE_LPM_TESTING_INTERNAL) && defined (FEATURE_LPM)) || defined (FEATURE_MMGSDI_DUAL_SLOT)
  nv_stat_enum_type     spare_item_nvstat = NV_NOTACTIVE_S;
  nv_item_type nvi;
#endif

  boolean bNeedToProcessICardEvent = FALSE;
  AEECardPinStatus pin_status;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  boolean bNeedToProcessICardEvent2 = FALSE;
  AEECardPinStatus pin_status2;
#endif
  AEENotify sCardNotify;

  /* Register with ICARD to receive event */
  dwMask = NMASK_CARD_STATUS | NMASK_PIN1_STATUS | NMASK_FILE_READ;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  dwMask |= NMASK_CARD_PERSO_STATUS;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  dwMask |= NMASK_CARD_PERSO_SLOT2_STATUS;
#endif
#endif
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  dwMask |= NMASK_CARD_SLOT2_STATUS | NMASK_PIN1_SLOT2_STATUS;
#endif

  ISHELL_RegisterNotify(pMe->a.m_pIShell,  AEECLSID_CORE_APP, AEECLSID_CARD_NOTIFIER,
                        dwMask);

  /* Get card status after registering for the event, if the card status is not
   * not ready, this implies that ICard has already sent out the event to client
   * and we missed it.  So, resend it */

  ICARD_GetCardStatus(pMe->m_pICard, (uint8 *) &(pMe->m_nCardStatus));
  if (pMe->m_nCardStatus != AEECARD_NOT_READY) {
    bNeedToProcessICardEvent = TRUE;
  }

  ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1, &pin_status);

  if (pin_status.lock_state != AEECARD_PIN_UNKNOWN)
  {
     sCardNotify.dwMask = NMASK_PIN1_STATUS;
     sCardNotify.pData = &pin_status;
     (void)CoreApp_ProcessICardEvents(pMe, 0, (uint32)&sCardNotify);
  }


#ifdef FEATURE_MMGSDI_DUAL_SLOT
  ICARD_GetCardStatusSlot(pMe->m_pICard, AEECARD_SLOT2, (uint8 *) &(pMe->m_nCardStatus_slot2));
  if (pMe->m_nCardStatus_slot2 != AEECARD_NOT_READY) {
    bNeedToProcessICardEvent2 = TRUE;
  }

  ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1_SLOT2, &pin_status2);

  if (pin_status2.lock_state != AEECARD_PIN_UNKNOWN)
  {
     sCardNotify.dwMask = NMASK_PIN1_SLOT2_STATUS;
     sCardNotify.pData = &pin_status2;
     (void)CoreApp_ProcessICardEvents(pMe, 0, (uint32)&sCardNotify);
  }

#endif

  if (bNeedToProcessICardEvent )
  {
     sCardNotify.dwMask = NMASK_CARD_STATUS;
     (void)CoreApp_ProcessICardEvents(pMe, 0, (uint32)&sCardNotify);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (bNeedToProcessICardEvent2 )
  {
     sCardNotify.dwMask = NMASK_CARD_SLOT2_STATUS;
     (void)CoreApp_ProcessICardEvents(pMe, 0, (uint32)&sCardNotify);
  }
#endif


/*Register with IBatt*/
  ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_CORE_APP,
                    AEECLSID_BATT_NOTIFIER,
                    NMASK_BATTNOTIFIER_BATTSTATUS_CHANGE |
                    NMASK_BATTNOTIFIER_BATTLEVEL_CHANGE |
                    NMASK_BATTNOTIFIER_CHARGERSTATUS_CHANGE);

  /* Register with Call Manager Notifier to receive event */
  ISHELL_RegisterNotify(pMe->a.m_pIShell,
      AEECLSID_CORE_APP,
      AEECLSID_CM_NOTIFIER,
      NMASK_CM_PHONE | NMASK_CM_SS );

  /* turns on reserve space on the annunciator */
  IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, ANNUN_STATE_ON); /* reserved space */
  IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_NO_SVC);

  InitMenuSettings(pMe);
  InitSound(pMe);
  CoreTask_SetPwrDnComplete(FALSE);

#if defined (FEATURE_LPM_TESTING_INTERNAL) && defined (FEATURE_LPM)
  spare_item_nvstat = ui_get_nv(NV_SPARE_1_I, &nvi);
  if (spare_item_nvstat == NV_DONE_S)
  {
    /* NV Item was active */
    if (nvi.spare_1 != 0x0) {
    bSendLPM = TRUE;
    }
  }
  else
  {
    CORE_ERR("Item Inactive or Not Readable Status %d",spare_item_nvstat,0,0);
  }
#endif /* FEATURE_LPM_TESTING_INTERNAL && FEATURE_LPM */

#ifdef FEATURE_LPM
  if ((HS_EXT_PWR_ON()) && bSendLPM )
  {
     CoreTask_SetCoreLPM(TRUE);
      ISHELL_PostEventEx (pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER,EVT_LPM,0L);
  }
  #endif


#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* initialized dual slot automation boolean */
  spare_item_nvstat = ui_get_nv( NV_SELECT_TECH_FOR_CARD_I, &nvi );

  if(spare_item_nvstat == NV_NOTACTIVE_S) {  /* TestAutomationMode is FALSE */
    pMe->m_bTestAutomationMode = FALSE;
  }

  if(spare_item_nvstat == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_bTestAutomationMode = nvi.select_tech_for_card;
  }
#endif

  /* If phone info is available, do not wait for PH_INFO_AVAIL event for
   * starting provisioning */
  if (!pMe->m_bProvisioned &&
      CheckAEEReturnStatus(ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_phInfo,
                                            sizeof(AEECMPhInfo)))) {
    InitAfterPhInfo(pMe, pMe->m_phInfo.oprt_mode);
  }
  return TRUE;
} /* End Initialization */

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
/*=============================================================================
FUNCTION: SendRTREConfig

DESCRIPTION:
  Read config from NV and send it to CM.
  This function is only executed once at powerup.

SIDE EFFECTS:
  None.

=============================================================================*/
void SendRTREConfig (CCoreApp *pMe)
{
  uint32 val;

  if ((pMe->m_bConfigSent == FALSE)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      &&
      (IsBothSlotReady(pMe))
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    val = GetRTREConfig(pMe);
    pMe->m_bConfigSent = SetRTREConfig(pMe, val);
  }
}
#endif /* F_UIM_RUN_TIME_ENABLE */

/*=============================================================================
FUNCTION: PopulateCardApplications

DESCRIPTION:
  Populate the card application accordingly

SIDE EFFECTS:
  None.

=============================================================================*/
int PopulateCardApplications (CCoreApp *pMe) {
#ifdef FEATURE_MMGSDI
  int nReturnStatus;
#endif /*FEATURE_MMGSDI */
  AEECardApp pCardApp;

  if (pMe == NULL) {
    return EFAILED;
  }

#ifdef FEATURE_MMGSDI
  nReturnStatus = ICARD_GetCardApps(pMe->m_pICard, &pCardApp);

  if (nReturnStatus != SUCCESS) {
    CORE_ERR("GetCardApps Failed",0,0,0);
    return nReturnStatus;
  }
#ifndef FEATURE_MMGSDI_DUAL_SLOT
  pCardApp.operational_slot = AEECARD_GSM_SLOT1_OPERATIONAL | AEECARD_CDMA_SLOT1_OPERATIONAL
                              | AEECARD_UMTS_SLOT1_OPERATIONAL;
#endif
#else
  pCardApp.slot1_present = TRUE;
  pCardApp.slot2_present = FALSE;
  pCardApp.apps1_enabled = AEECARD_RUIM_APP_MASK;
  pCardApp.operational_slot = AEECARD_CDMA_SLOT1_OPERATIONAL;
  pCardApp.apps2_enabled = AEECARD_NO_APP_MASK;
#endif /*FEATURE_MMGSDI */

  /* initialize security mask slot related info to zero */
  pMe->m_dwSecurityMask &= COREAPP_NOT_READY_MASK; /* only keep the upper nibble
                                                      which contains info on pin verirication */
  /* populate the apps info */
  if (pCardApp.slot1_present) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_PRESENT_MASK;
  }
  if (pCardApp.slot2_present) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_PRESENT_MASK;
  }
  if ((pCardApp.apps1_enabled & AEECARD_GSM_SIM_MASK) == AEECARD_GSM_SIM_MASK) {
    /* There is a GSM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_APP_GSM_MASK;
  }
  if ((pCardApp.apps1_enabled & AEECARD_USIM_APP_MASK) == AEECARD_USIM_APP_MASK) {
    /* There is a USIM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_APP_USIM_MASK;
  }
  if ((pCardApp.apps1_enabled & AEECARD_RUIM_APP_MASK) == AEECARD_RUIM_APP_MASK) {
    /* There is a RUIM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_APP_RUIM_MASK;
  }
  if ((pCardApp.apps2_enabled & AEECARD_GSM_SIM_MASK) == AEECARD_GSM_SIM_MASK) {
    /* There is a GSM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_APP_GSM_MASK;
  }
  if ((pCardApp.apps2_enabled & AEECARD_USIM_APP_MASK) == AEECARD_USIM_APP_MASK) {
    /* There is a USIM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_APP_USIM_MASK;
  }
  if ((pCardApp.apps2_enabled & AEECARD_RUIM_APP_MASK) == AEECARD_RUIM_APP_MASK) {
    /* There is a RUIM application on the card */
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_APP_RUIM_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_GSM_SLOT1_OPERATIONAL) == AEECARD_GSM_SLOT1_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_GSM_OPERATIONAL_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_GSM_SLOT2_OPERATIONAL) == AEECARD_GSM_SLOT2_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_GSM_OPERATIONAL_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_UMTS_SLOT1_OPERATIONAL) == AEECARD_UMTS_SLOT1_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_UMTS_OPERATIONAL_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_UMTS_SLOT2_OPERATIONAL) == AEECARD_UMTS_SLOT2_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_UMTS_OPERATIONAL_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_CDMA_SLOT1_OPERATIONAL) == AEECARD_CDMA_SLOT1_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT1_CDMA_OPERATIONAL_MASK;
  }
  if ((pCardApp.operational_slot & AEECARD_CDMA_SLOT2_OPERATIONAL) == AEECARD_CDMA_SLOT2_OPERATIONAL) {
    pMe->m_dwSecurityMask |= COREAPP_SLOT2_CDMA_OPERATIONAL_MASK;
  }
  CORE_MSG("Coreapp: security mask 0x%x", pMe->m_dwSecurityMask,0,0);
  return SUCCESS;
}


#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*=============================================================================
FUNCTION: IsBothSlotReady

DESCRIPTION:

  Check if both slot has received valid information

SIDE EFFECTS:
  None.

=============================================================================*/
boolean IsBothSlotReady(CCoreApp *pMe)
{
  /* check if both the card status has been set */
  return ((pMe->m_nCardStatus != AEECARD_NOT_READY) &&
          (pMe->m_nCardStatus_slot2 != AEECARD_NOT_READY));

}
#endif
/*=============================================================================
FUNCTION: ValidateCardsAndChangeState

DESCRIPTION:

  This function is used called everytime we receive a card event
  and it validates the cards and depending on the card events and
  pin states makes a transition to the right state

SIDE EFFECTS:
  None.

=============================================================================*/
void ValidateCardsAndChangeState (CCoreApp *pMe, boolean bAllowByPass)
{

  if (pMe == NULL) {
    CORE_ERR("NULL pMe", 0, 0, 0);
    return;
  }

  if(IsConfigCtrlReady(pMe))
  { /* if this returns false we are still waiting for CM Event
       else we have a valid config and control information
    */
    if (CoreSecurity_AllCardEventsReceived(pMe))
    {
      /* If the NV Pin has not been verified then verify the pin
      else start card verification */
      if (CoreSecurity_IsPinVerified(pMe,0))
      {
        /* If the rtre is set to NV only or fall back with control at NV
           do not go through card dialogs.Instead directly do to idle menu
        */
        if (((pMe->m_rtreConfig == AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK) &&
             (pMe->m_rtreControl == AEECM_RTRE_CONTROL_NV)) ||
             (pMe->m_rtreConfig == AEECM_RTRE_CONFIG_NV_ONLY))
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                                        EVT_USER, EVT_SUBSCRIPTION_CHANGED, 0);
          CreateIdleMenuDlg(pMe);
        }
        else
        {
          PopulateCardApplications(pMe);
          DetermineNextCardState(pMe,bAllowByPass);
        }
      }
      else
      {
        /* Starts NV verification process */
        VerifyNVCode(pMe);
      }
    }
  }
} /* ValidateCardsAndChangeState */

/*===========================================================================
FUNCTION RemoveLowBatteryWarning

DESCRIPTION
  Remove the battery warning ONLY if it is being displayed currently.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void RemoveLowBatteryWarning(void *pUser)
  {
  CCoreApp *pMe = (CCoreApp*) pUser;
  uint32 nBufSize;
  AECHAR *pMessage;

  //First check if the current idle message is the low_battery message.
  (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
  IDS_LOW_BATTERY, RESTYPE_STRING, (void*)-1, &nBufSize);

  nBufSize = (nBufSize+1)*sizeof(AECHAR);

  pMessage = (AECHAR*) MALLOC(nBufSize);
  if(pMessage == NULL)
  {
    CORE_ERR("Unable to allocate", 0, 0, 0);
    return;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
      IDS_LOW_BATTERY, pMessage, (int)nBufSize);

  if(WSTRCMP(pMe->m_pszIdleMenuText, pMessage) == 0)
  {
    // Cast as void, since there's nothing to do if it fails
    (void)SetIdleMenuText(NULL);
  }
  FREE(pMessage);
}

/*===========================================================================
FUNCTION RemoveNoBatteryWarning

DESCRIPTION
  Remove the warning ONLY if it is a "No Battery" warning message currently on the screen

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void RemoveNoBatteryWarning(void *pUser)
{
  CCoreApp *pMe = (CCoreApp*) pUser;
  uint32 nBufSize;
  AECHAR *pMessage;

//change it to no battery
  //First check if the current idle message is the low_battery message.
  (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
  IDS_NO_BATTERY, RESTYPE_STRING, (void*)-1, &nBufSize);

  nBufSize = (nBufSize+1)*sizeof(AECHAR);

  pMessage = (AECHAR*) MALLOC(nBufSize);
  if(pMessage == NULL)
  {
    CORE_ERR("Unable to allocate", 0, 0, 0);
    return;
  }

//change it to no_battery
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
      IDS_NO_BATTERY, pMessage, (int)nBufSize);

  if(WSTRCMP(pMe->m_pszIdleMenuText, pMessage) == 0)
  {
    // Cast as void, since there's nothing to do if it fails
    (void)SetIdleMenuText(NULL);
  }
  FREE(pMessage);
}
/*===========================================================================
FUNCTION CheckForEmergencyCall

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CheckForEmergencyCall(void *pUser)
{
  CCoreApp *pMe = (CCoreApp*) pUser;
  uint32 nBufSize;
  AECHAR *pMessage;

  if((pMe->m_pICM != NULL) && (AEECM_IS_XCALL_PRESENT(pMe->m_pICM, AEECM_CALL_TYPE_EMERGENCY)))
  {
    // do not do anything
    return;
  }
  // Power down. Phone should power up in LPM
  pMe->m_bUserPwrDown = TRUE;
  MSG_HIGH("No Emergency call and Nod Battery - Phone powering down",0,0,0);

  (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
          IDS_POWERDOWN, RESTYPE_STRING, (void*)-1, &nBufSize);

  nBufSize = (nBufSize+1)*sizeof(AECHAR);

  pMessage = (AECHAR*) MALLOC(nBufSize);
  if(pMessage == NULL)
  {
    CORE_ERR("Unable to allocate", 0, 0, 0);
     return;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
              IDS_POWERDOWN, pMessage, (int)nBufSize);

  (void)SetIdleMenuText(pMessage);
   ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY) PowerDown,
                          (void*) pMe);
   FREE(pMessage);
 }

/*=============================================================================
FUNCTION: CoreApp_ProcessIBatteryEvents

DESCRIPTION:
  Process IBatt Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_ProcessIBatteryEvents(CCoreApp *pMe, uint16 wParam, uint32 dwParam)
/*lint -esym(715,wParam)*/
        {
  AEENotify *pNotifyInfo = (AEENotify*) dwParam;
  AEEBattLevel level;
  boolean  bReturnVal = TRUE;

  switch (pNotifyInfo->dwMask)
  {

  case NMASK_BATTNOTIFIER_BATTSTATUS_CHANGE:
  {
      AEEBattStatus battStatus = (*(AEEBattStatus*)(pNotifyInfo->pData));
      // Do not do anything when the phone is being charged.
      if(pMe->m_bExtPwr)
      {
        return TRUE;
      }
      bReturnVal = CoreAppDisplayBatteryStatus(pMe,battStatus);
  }
  return (bReturnVal);

  case NMASK_BATTNOTIFIER_BATTLEVEL_CHANGE:
    {
      //Do not update annuciator if charger is plugged in. The Annunciator is already being updated every second!
      if(!pMe->m_bExtPwr)
      {
        AEEBattLevel *battLevel = (AEEBattLevel*) (pNotifyInfo->pData);
        // OEM returns battery levels from 0 to 4. Annunciatros values (LOW - FULL) are 1 - 5.
        // So just add 1 to it.
        CoreApp_UpdateBatteryAnnunciator(pMe,battLevel->level+1);
      }
    }
    return TRUE;

  case NMASK_BATTNOTIFIER_CHARGERSTATUS_CHANGE:
    {
      AEEChargerStatus chargerStatus = (*(AEEChargerStatus*)(pNotifyInfo->pData));
      switch(chargerStatus)
      {
      case AEECHG_STATUS_CHARGING:
        pMe->m_nlastIcon = ANNUN_STATE_BATT_FULL;
        CoreApp_AnimateBatteryAnnunciator(pMe);
        return TRUE;

      case AEECHG_STATUS_NO_CHARGER:
        ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_AnimateBatteryAnnunciator, (void*) pMe);
        // We don't care about the actual status, so cast function as void
        (void)IBATT_GetBattStatus(pMe->m_pIBatt, &level);
        CoreApp_UpdateBatteryAnnunciator(pMe, level.level+1);
        return TRUE;

      case AEECHG_STATUS_FULLY_CHARGE:
        ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_AnimateBatteryAnnunciator, (void*) pMe);
        CoreApp_UpdateBatteryAnnunciator(pMe,ANNUN_STATE_BATT_FULL);
        return TRUE;

      // The battery is very weak or The charger may not be supplying sufficient power. Go to LPM
      case AEECHG_STATUS_DEAD_BATT:
      case AEECHG_STATUS_NO_CHARGE_NO_BATT:

        MSG_HIGH("The battery is dead/charger is too weak", 0, 0, 0);
        if(!pMe->m_bLPM && !pMe->m_bLPMTimer)
        {
          pMe->m_bLPMTimer = TRUE;

          ISHELL_SetTimer(pMe->a.m_pIShell,
                           20000,
                          (PFNNOTIFY) CheckForEmergencyCall,
                          (uint32*) pMe);


        }
        break;
      case AEECHG_STATUS_NO_BATT:
        // Display the warning message that there is no battery
        if(!pMe->m_bSuspended)
        {
          uint32 nBufSize;
          AECHAR *pMessage;
          (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, gpszResourceFile,
          IDS_NO_BATTERY, RESTYPE_STRING, (void*)-1, &nBufSize);

          nBufSize = (nBufSize+1)*sizeof(AECHAR);

          pMessage = (AECHAR*) MALLOC(nBufSize);
          if(pMessage == NULL)
          {
            CORE_ERR("Unable to allocate", 0, 0, 0);
            return FALSE;
          }

          (void) ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
              IDS_NO_BATTERY, pMessage, (int)nBufSize);
          (void)SetIdleMenuText(pMessage);
          ISHELL_SetTimer(pMe->a.m_pIShell, 2000, RemoveNoBatteryWarning, (void*) pMe);
          FREE(pMessage);
        }
        //fall through

      case AEECHG_STATUS_UNKNOWN_BATT:
      case AEECHG_STATUS_OVERVOLTAGE:
        ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_AnimateBatteryAnnunciator, (void*) pMe);
        CORE_ERR("Bad battery: %d", chargerStatus, 0, 0);
        CoreApp_UpdateBatteryAnnunciator(pMe, ANNUN_STATE_OFF);

        return TRUE;

      default:
        return FALSE;
      }
    }
    return TRUE; //lint !e527 unreachable

  default:
    CORE_ERR("Unhandled event for IBattNotifier", 0, 0, 0);
    return FALSE;
  }
}

/*=============================================================================
FUNCTION: EndLPMMode

DESCRIPTION:
  Ends Low Power mode

SIDE EFFECTS:
  None.

=============================================================================*/
void EndLPMMode(CCoreApp *pMe, boolean changeMode)
{

#ifdef FEATURE_MMGSDI
  int nReturnStatus;
#ifdef FEATURE_NEW_ICARD
  AEECardPinStatus pin_status;
#else
  AEEPinStatus pin_status;
#endif
#endif /* FEATURE_MMGSDI */

  if(changeMode)
  {
    if (!SetLPMMode(pMe, FALSE)) {
      CORE_ERR("Unable to clear LPM", 0,0,0);
    }
    CoreTask_SetCoreLPM(FALSE);
  }

  ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_MMGSDI
  ICARD_GetCardStatus(pMe->m_pICard, (uint8 *) &(pMe->m_nCardStatus));
  nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1, &pin_status);

  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
    CORE_ERR("ICARD_GetPinStatus Failed", 0, 0, 0);
      return;
  }
  pMe->m_sPIN1_lock_state = pin_status.lock_state;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* check what subscription available info should be sent to CM */
  ICARD_GetCardStatusSlot(pMe->m_pICard, AEECARD_SLOT2, (uint8 *) &(pMe->m_nCardStatus_slot2));
  nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1_SLOT2, &pin_status);
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
    CORE_ERR("ICARD_GetPinStatus Failed", 0, 0, 0);
    return;
  }
  pMe->m_sPIN1_slot2_lock_state = pin_status.lock_state;

#endif
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif

  ValidateCardsAndChangeState(pMe,FALSE);
}

/*===========================================================================
FUNCTION CoreApp_ReadFileCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_ReadFileCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;
  uint32             event=0;

  if(pData == NULL)
  {
    MSG_ERROR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_pCSPCardCmdStatus == NULL)
  {
    MSG_ERROR("Null Data",0,0,0);
    FREEIF(pMe->m_pCSPCardCmdStatus);
    pMe->m_pCSPCardCmdStatus = NULL;
    return;
  }

  if(pMe->m_pCSPCardCmdStatus->nCmdStatus != SUCCESS)
  {
    MSG_ERROR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_pCSPCardCmdStatus);
    pMe->m_pCSPCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    MSG_ERROR("No resources",0,0,0);
    FREEIF(pMe->m_pCSPCardCmdStatus);
    pMe->m_pCSPCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    MSG_ERROR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_pCSPCardCmdStatus);
    pMe->m_pCSPCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_pCSPCardCmdStatus->dwData,pReadData) == SUCCESS)
  {

    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      MSG_ERROR("Invalid Data Received from the Card for fileName %d",pMe->m_pCSPCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_pCSPCardCmdStatus);
      pMe->m_pCSPCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      MSG_ERROR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_pCSPCardCmdStatus);
      pMe->m_pCSPCardCmdStatus = NULL;
      return;
    }

    // Now get the actual data
    if(SUCCESS != ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_pCSPCardCmdStatus->dwData,pReadData))
    {
      MSG_ERROR("Could Not Get Response",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
    }

    switch(pMe->m_pCSPCardCmdStatus->fileName)
    {
      case AEECARD_GSM_CSP:
      case AEECARD_USIM_CSP:
        event = EVT_CSP_READ_SUCCESS;
       break;
#ifdef FEATURE_CPHS_EONS
      case AEECARD_GSM_IMSI:
      case AEECARD_USIM_IMSI:
        event = EVT_IMSI_READ_SUCCESS;
        break;
#endif /* FEATURE_CPHS_EONS */
       default:
         break;
    }

    if (event && pReadData) {
      ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                 EVT_USER, event, (uint32)pReadData);
    }

    FREEIF(pMe->m_pCSPCardCmdStatus);
    pMe->m_pCSPCardCmdStatus = NULL;
    return;


  }
}
#ifdef FEATURE_CPHS_EONS
/*===========================================================================
FUNCTION CoreApp_ReadIMSIFileCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_ReadIMSIFileCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;
  uint32             event=0;

  if(pData == NULL)
  {
    MSG_ERROR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_pCardCmdStatus == NULL)
  {
    MSG_ERROR("Null Data",0,0,0);
    FREEIF(pMe->m_pCardCmdStatus);
    pMe->m_pCardCmdStatus = NULL;
    return;
  }

  if(pMe->m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    MSG_ERROR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_pCardCmdStatus);
    pMe->m_pCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    MSG_ERROR("No resources",0,0,0);
    FREEIF(pMe->m_pCardCmdStatus);
    pMe->m_pCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    MSG_ERROR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_pCardCmdStatus);
    pMe->m_pCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {

    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      MSG_ERROR("Invalid Data Received from the Card for fileName %d",pMe->m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_pCardCmdStatus);
      pMe->m_pCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      MSG_ERROR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_pCardCmdStatus);
      pMe->m_pCardCmdStatus = NULL;
      return;
    }

    // Now get the actual data
    if(SUCCESS != ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_pCardCmdStatus->dwData,pReadData))
    {
      MSG_ERROR("Could Not Get Response",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
    }

    switch(pMe->m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_CSP:
      case AEECARD_USIM_CSP:
        event = EVT_CSP_READ_SUCCESS;
       break;
#ifdef FEATURE_CPHS_EONS
      case AEECARD_GSM_IMSI:
      case AEECARD_USIM_IMSI:
        event = EVT_IMSI_READ_SUCCESS;
        break;
#endif /* FEATURE_CPHS_EONS */
       default:
         break;
    }

    if (event && pReadData) {
      ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                 EVT_USER, event, (uint32)pReadData);
    }

    FREEIF(pMe->m_pCardCmdStatus);
    pMe->m_pCardCmdStatus = NULL;
    return;


  }
}
#endif

/*===========================================================================
FUNCTION CoreApp_ReadFiles

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadFiles
(
 CCoreApp *pMe
)
{
  AEECardFilePathType pCardFilePath;
  AEECardApp pCardApp;
  uint8 slot;
  int nResult = AEE_SUCCESS;
  boolean manualPLMNSelection = FALSE;
  boolean bShowManPLMNSel;

  if(pMe == NULL || pMe->m_pIConfig == NULL)
  {
    return EFAILED;
  }

  /*Check to see if the NV item is enabled. If it is then skip the reading of the files.
    The Manual PLMN Selection Menu will always be displayed */
 
  nResult = ICONFIG_GetItem(pMe->m_pIConfig, CFGI_MANUAL_PLMN_SEL,
                                         (void*)&manualPLMNSelection,
                                         sizeof(boolean));
  if(nResult == AEE_SUCCESS)
  {
    if(manualPLMNSelection)
    {
      //No Need to Proceed. Set the Flag to true and return.
      if (pMe->m_pIConfig != NULL)
      {
        bShowManPLMNSel = TRUE;
        (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_MANUAL_PLMN_SEL_ALLOWED,
                              (void *)&bShowManPLMNSel, sizeof(bShowManPLMNSel));
      }
      return SUCCESS;
    }
  }

  
  if(ICARD_GetCardApps(pMe->m_pICard, &pCardApp) != SUCCESS)
  {
    return EFAILED;
  }

  if(!pCardApp.slot1_present)
  {
    return EFAILED;
  }

  if(ICARD_GetUserSlotPref(pMe->m_pICard, AEECARD_GSM_APP, &slot)!= SUCCESS)
  {
    return EFAILED;
  }

  // Get File Attrib -> for failure , no need to proceed
  pCardFilePath.nAIDLen = 0;
  pCardFilePath.pAID = NULL;
  pCardFilePath.nPathLen = 0;
  pCardFilePath.pPath = NULL;
  pCardFilePath.nFileIndication = AEECARD_FILE_NAME;
  pCardFilePath.dwStructureSize = sizeof(AEECardFilePathType);

  //Check the apps enabled and put the appropriate file name
  if(pCardApp.apps1_enabled & AEECARD_USIM_APP_MASK)
  {
    pCardFilePath.nFileName = AEECARD_USIM_CSP;
  }
  else if(pCardApp.apps1_enabled & AEECARD_GSM_SIM_MASK)
  {
    pCardFilePath.nFileName = AEECARD_GSM_CSP;
  }
  else
  {
    return EFAILED;
  }

  pMe->m_pCSPCardCmdStatus = (AEECardGenericCmdStatus*)MALLOC(sizeof(AEECardGenericCmdStatus));
  if(pMe->m_pCSPCardCmdStatus == NULL)
  {
    return EFAILED;
  }

  // Read the Data from the SIM
  CALLBACK_Init(&pMe->m_cspCallback, (PFNNOTIFY) CoreApp_ReadFileCb,
                      (void*)(pMe));


  if(ICARD_ReadBinary(pMe->m_pICard,slot,pCardFilePath,0,0,pMe->m_pCSPCardCmdStatus,&pMe->m_cspCallback) != SUCCESS)
  {
     FREE(pMe->m_pCSPCardCmdStatus);
     pMe->m_pCSPCardCmdStatus = NULL;
     return EFAILED;
  }
  return SUCCESS;

}


#ifdef FEATURE_CPHS_EONS
/*===========================================================================
FUNCTION CoreApp_OPLFileReadCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_OPLFileReadCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;

  if(pData == NULL)
  {
    CORE_ERR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_OpListInfo.m_pCardCmdStatus == NULL)
  {
    CORE_ERR("Null Data",0,0,0);
    return;
  }

  if(pMe->m_OpListInfo.m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    CORE_ERR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
    pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
    pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
    pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_OpListInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {
    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      CORE_ERR("Invalid Data Received from the Card for fileName %d",pMe->m_OpListInfo.m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
      pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      CORE_ERR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
      pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
      return;
    }

    switch(pMe->m_OpListInfo.m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_OPL:
      case AEECARD_USIM_OPL:
        // Now get the actual data
        if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_OpListInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                          EVT_USER, EVT_OPL_READ_SUCCESS, (uint32)pReadData);
        }
        else
        {
          CORE_ERR("Could Not Get Response",0,0,0);
          FREEIF(pReadData->DataLen);
          FREEIF(pReadData->pData);
          FREEIF(pReadData);
        }
        break;

      default:
        break;
    }

    FREEIF(pMe->m_OpListInfo.m_pCardCmdStatus);
    pMe->m_OpListInfo.m_pCardCmdStatus = NULL;
    return;
  }
}


/*===========================================================================
FUNCTION CoreApp_SPNFileReadCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_SPNFileReadCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;

  if(pData == NULL)
  {
    CORE_ERR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_SPNInfo.m_pCardCmdStatus == NULL)
  {
    CORE_ERR("Null Data",0,0,0);
    return;
  }

  if(pMe->m_SPNInfo.m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    CORE_ERR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
    pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
    pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
    pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_SPNInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {
    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      CORE_ERR("Invalid Data Received from the Card for fileName %d",pMe->m_SPNInfo.m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
      pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      CORE_ERR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
      pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
      return;
    }

    switch(pMe->m_SPNInfo.m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_SPN:
      case AEECARD_USIM_SPN:
        // Now get the actual data
        if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_SPNInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                             EVT_USER, EVT_SPN_READ_SUCCESS, (uint32)pReadData);
        }
        else
        {
          CORE_ERR("Could Not Get Response",0,0,0);
          FREEIF(pReadData->DataLen);
          FREEIF(pReadData->pData);
          FREEIF(pReadData);
        }
        break;

      default:
        CORE_ERR("Invalid File Name in Cmd Status",0,0,0);
        break;
    }

    FREEIF(pMe->m_SPNInfo.m_pCardCmdStatus);
    pMe->m_SPNInfo.m_pCardCmdStatus = NULL;
    return;
  }
}


/*===========================================================================
FUNCTION CoreApp_CPHSOpNameFileReadCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_CPHSOpNameFileReadCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;

  if(pData == NULL)
  {
    CORE_ERR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus == NULL)
  {
    CORE_ERR("Null Data",0,0,0);
    return;
  }

  if(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    CORE_ERR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_CPHSOpNameInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {
    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      CORE_ERR("Invalid Data Received from the Card for fileName %d",pMe->m_CPHSOpNameInfo.m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      CORE_ERR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
      return;
    }

    switch(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_ONS:
      case AEECARD_USIM_ONS:
        // Now get the actual data
        if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_CPHSOpNameInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                        EVT_USER, EVT_CPHS_ONS_READ_SUCCESS, (uint32)pReadData);
        }
        else
        {
          CORE_ERR("Could Not Get Response",0,0,0);
          FREEIF(pReadData->DataLen);
          FREEIF(pReadData->pData);
          FREEIF(pReadData);
        }
        break;

      default:
        CORE_ERR("Invalid File Name in Cmd Status",0,0,0);
        break;
    }

    FREEIF(pMe->m_CPHSOpNameInfo.m_pCardCmdStatus);
    return;
  }
}

/*===========================================================================
FUNCTION CoreApp_SPDIFileReadCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_SPDIFileReadCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;

  if(pData == NULL)
  {
    CORE_ERR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_SPDInfo.m_pCardCmdStatus == NULL)
  {
    CORE_ERR("Null Data",0,0,0);
    return;
  }

  if(pMe->m_SPDInfo.m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    CORE_ERR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
    pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
    pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
    pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_SPDInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {
    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      CORE_ERR("Invalid Data Received from the Card for fileName %d",pMe->m_SPDInfo.m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
      pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      CORE_ERR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
      pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
      return;
    }

    switch(pMe->m_SPDInfo.m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_SPDI:
      case AEECARD_USIM_SPDI:
        // Now get the actual data
        if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_SPDInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                             EVT_USER, EVT_SPDI_READ_SUCCESS, (uint32)pReadData);
        }
        else
        {
          CORE_ERR("Could Not Get Response",0,0,0);
          FREEIF(pReadData->DataLen);
          FREEIF(pReadData->pData);
          FREEIF(pReadData);
        }
        break;

      default:
        break;
    }

    FREEIF(pMe->m_SPDInfo.m_pCardCmdStatus);
    pMe->m_SPDInfo.m_pCardCmdStatus = NULL;
    return;
  }
}



/*===========================================================================
FUNCTION CoreApp_PNNFileReadCb

DESCRIPTION
    Reads the Information received from the SIM and sends the appropriate
    event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static void CoreApp_PNNFileReadCb(void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;
  AEECardDataBufType *pReadData;

  if(pData == NULL)
  {
    CORE_ERR("Null Pointer",0,0,0);
    return;
  }

  if(pMe->m_PNNInfo.m_pCardCmdStatus == NULL)
  {
    CORE_ERR("Null Data",0,0,0);
    return;
  }

  if(pMe->m_PNNInfo.m_pCardCmdStatus->nCmdStatus != SUCCESS)
  {
    CORE_ERR("Could not Read File from ICard",0,0,0);
    FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
    pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // Read the Data and send to Client.
  pReadData = (AEECardDataBufType*)MALLOC(sizeof(AEECardDataBufType));

  if(pReadData == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
    pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  pReadData->DataLen = MALLOC(sizeof(int));
  pReadData->pData = NULL;

  if(pReadData->DataLen == NULL)
  {
    CORE_ERR("No resources",0,0,0);
    FREEIF(pReadData);
    FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
    pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
    return;
  }

  // First Get the length of the data
  if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_PNNInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
  {
    if((pReadData->DataLen == NULL) || (*pReadData->DataLen == 0))
    {
      CORE_ERR("Invalid Data Received from the Card for fileName %d",pMe->m_PNNInfo.m_pCardCmdStatus->fileName,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData);
      FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
      pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
      return;
    }

    pReadData->pData = MALLOC(*pReadData->DataLen);

    // Check for malloc
    if(pReadData->pData == NULL)
    {
      CORE_ERR("No Resources",0,0,0);
      FREEIF(pReadData->DataLen);
      FREEIF(pReadData->pData);
      FREEIF(pReadData);
      FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
      pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
      return;
    }

    switch(pMe->m_PNNInfo.m_pCardCmdStatus->fileName)
    {
      case AEECARD_GSM_PNN:
      case AEECARD_USIM_PNN:
        // Now get the actual data
        if(ICARD_GetRespData(pMe->m_pICard,(uint32)pMe->m_PNNInfo.m_pCardCmdStatus->dwData,pReadData) == SUCCESS)
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                             EVT_USER, EVT_PNN_READ_SUCCESS, (uint32)pReadData);
        }
        else
        {
          CORE_ERR("Could Not Get Response",0,0,0);
          FREEIF(pReadData->DataLen);
          FREEIF(pReadData->pData);
          FREEIF(pReadData);
        }
        break;

      default:
        break;
    }

    FREEIF(pMe->m_PNNInfo.m_pCardCmdStatus);
    pMe->m_PNNInfo.m_pCardCmdStatus = NULL;
    return;
  }
}


/*===========================================================================
FUNCTION CoreApp_RegisterForFCN

DESCRIPTION
    Register for File Change Notifications for ALL files that Core is 
    interested in.

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_RegisterForFCN(CCoreApp *pMe)
{
  int total = 0;
  AEECardFileItemEnumType pFileList[5];
  AEECardFilePathType pCardFilePath;
  AEECardApp pCardApp;
  uint8 slot;
  AEECardFileItemEnumType pTempFileList[1];
  
  if (SUCCESS == CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pTempFileList, AEECARD_USIM_SPN,AEECARD_GSM_SPN))
  {
    pFileList[total++] = pTempFileList[0];
  }
  

   if (SUCCESS == CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pTempFileList, AEECARD_USIM_PNN,AEECARD_GSM_PNN))
  {
    pFileList[total++] = pTempFileList[0];
  }


  if (SUCCESS == CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pTempFileList, AEECARD_USIM_SPDI,AEECARD_GSM_SPDI))
  {
    pFileList[total++] = pTempFileList[0];
  }
  

  if (SUCCESS == CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pTempFileList, AEECARD_USIM_IMSI,AEECARD_GSM_IMSI))
  {
   pFileList[total++] = pTempFileList[0];
  }
 
  if (SUCCESS == CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pTempFileList, AEECARD_USIM_CSP,AEECARD_GSM_CSP))
  {
    pFileList[total++] = pTempFileList[0];
  }
  

   // Also register for File Change Notifications
  ICARD_RegisterForFCN(pMe->m_pICard,AEECLSID_CORE_APP,total, pFileList);

  return SUCCESS;
}
/*===========================================================================
FUNCTION CoreApp_FillGenericData

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_FillGenericData
(
  CCoreApp *pMe,
  AEECardFilePathType *pCardFilePath,
  AEECardApp *pCardApp,
  uint8 *pSlot,
  AEECardFileItemEnumType *pFileList,
  AEECardFileItemEnumType usimFileName,
  AEECardFileItemEnumType simFileName
)
{
  if(pMe == NULL)
  {
    return EFAILED;
  }

  if(ICARD_GetCardApps(pMe->m_pICard, pCardApp) != SUCCESS)
  {
    CORE_ERR("EONS:Could not get Card Apps",0,0,0);
    return EFAILED;
  }

  if(pCardApp->slot1_present || pCardApp->slot2_present)
  {
    if(ICARD_GetUserSlotPref(pMe->m_pICard, AEECARD_GSM_APP, pSlot)!= SUCCESS)
    {
      CORE_ERR("EONS:Could not get GSM Card App",0,0,0);
      return EFAILED;
    }

    if (AEECARD_SLOT_NONE == *pSlot)
    {
      if(ICARD_GetUserSlotPref(pMe->m_pICard, AEECARD_USIM_APP, pSlot)!= SUCCESS)
      {
        CORE_ERR("EONS:Could not get USIM Card App",0,0,0);
        return EFAILED;
      }
    }
  }
  else
  {
    CORE_ERR("EONS:Both the Slots are empty",0,0,0);
    return EFAILED;
  }


  switch (*pSlot)
  {

    case AEECARD_SLOT1:
    {
      if(pCardApp->apps1_enabled & AEECARD_USIM_APP_MASK)
      {
        pCardFilePath->nFileName = usimFileName;
        pFileList[0] = usimFileName;
      }
      else if(pCardApp->apps1_enabled & AEECARD_GSM_SIM_MASK)
      {
        pCardFilePath->nFileName = simFileName;
        pFileList[0] = simFileName;
      }
      else
      {
        CORE_ERR("EONS:App Not Enabled",0,0,0);
        return EFAILED;
      }
    }
    break;

    case AEECARD_SLOT2:
    {

      if(pCardApp->apps2_enabled & AEECARD_USIM_APP_MASK)
      {
        pCardFilePath->nFileName = usimFileName;
        pFileList[0] = usimFileName;
      }
      else if(pCardApp->apps2_enabled & AEECARD_GSM_SIM_MASK)
      {
        pCardFilePath->nFileName = simFileName;
        pFileList[0] = simFileName;
      }
      else
      {
        CORE_ERR("EONS:App Not Enabled",0,0,0);
        return EFAILED;
      }
    }
    break;

    case AEECARD_SLOT_NONE:
    default:
      CORE_ERR("EONS:Both the Slots are empty",0,0,0);
      return EFAILED;
  }

  // Get File Attrib -> for failure , no need to proceed
  pCardFilePath->nAIDLen = 0;
  pCardFilePath->pAID = NULL;
  pCardFilePath->nPathLen = 0;
  pCardFilePath->pPath = NULL;
  pCardFilePath->nFileIndication = AEECARD_FILE_NAME;
  pCardFilePath->dwStructureSize = sizeof(AEECardFilePathType);

  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreApp_ReadSIMFile

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadSIMFile
(
 CCoreApp *pMe,
 int nNumBytes,
 AEECardFileItemEnumType usimFile,
 AEECardFileItemEnumType gsmFile,
 AEECardGenericCmdStatus **pCardCmdStatus,
 AEECallback             *pCallBack,
 PFNNOTIFY               pNotifyFunction
)
{
  AEECardFilePathType pCardFilePath;
  AEECardApp pCardApp;
  uint8 slot;
  AEECardFileItemEnumType pFileList[1];
  int readSucceed=EFAILED;

  if (SUCCESS != CoreApp_FillGenericData(pMe, &pCardFilePath, &pCardApp,
                  &slot, pFileList, usimFile, gsmFile))
  {
    return EFAILED;
  }


  *pCardCmdStatus = (AEECardGenericCmdStatus*)MALLOC(sizeof(AEECardGenericCmdStatus));
  if(*pCardCmdStatus == NULL)
  {
    return EFAILED;
  }

  // Read the Data from the SIM
  CALLBACK_Init(pCallBack, (PFNNOTIFY) pNotifyFunction,(void*)(pMe));

  switch (pFileList[0]) {
  case AEECARD_USIM_OPL:
  case AEECARD_GSM_OPL:
    readSucceed = ICARD_ReadRecord(pMe->m_pICard,slot,pCardFilePath,(int)(pMe->m_OpListInfo.numRecsRead+1),
                        *pCardCmdStatus,pCallBack);
  break;

  case AEECARD_USIM_PNN:
  case AEECARD_GSM_PNN:
    readSucceed = ICARD_ReadRecord(pMe->m_pICard,slot,pCardFilePath,(int)(pMe->m_PNNInfo.numRecsRead+1),
                        *pCardCmdStatus,pCallBack);
  break;

  case AEECARD_USIM_SPN:
  case AEECARD_GSM_SPN:
  case AEECARD_USIM_SPDI:
  case AEECARD_GSM_SPDI:
  case AEECARD_USIM_IMSI:
  case AEECARD_GSM_IMSI:
  case AEECARD_USIM_ONS:
  case AEECARD_GSM_ONS:
    readSucceed = ICARD_ReadBinary(pMe->m_pICard,slot,pCardFilePath,0,nNumBytes,
                        *pCardCmdStatus,pCallBack);
  break;

  default:
    CORE_ERR("EONS: Error in Sim File Type %d",pFileList[0],0,0);
  break;
  }

  if(readSucceed != SUCCESS)
  {
    FREE(*pCardCmdStatus);
    *pCardCmdStatus = NULL;
    return EFAILED;
  }
  else
  {
    return SUCCESS;
  }
}

/*===========================================================================
FUNCTION CoreApp_ReadSPNFile

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadSPNFile(CCoreApp *pMe)
{
  if ( SUCCESS != CoreApp_ReadSIMFile(pMe,0,AEECARD_USIM_SPN,AEECARD_GSM_SPN,
              &pMe->m_SPNInfo.m_pCardCmdStatus,
              &pMe->m_SPNInfo.m_spnCallback, CoreApp_SPNFileReadCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}


/*===========================================================================
FUNCTION CoreApp_ReadPNNFile

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadPNNFile(CCoreApp *pMe)
{
  if ( SUCCESS != CoreApp_ReadSIMFile(pMe,0,AEECARD_USIM_PNN,AEECARD_GSM_PNN,
              &pMe->m_PNNInfo.m_pCardCmdStatus,
              &pMe->m_PNNInfo.m_pnnCallback, CoreApp_PNNFileReadCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreApp_ReadSPDIFile

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadSPDIFile(CCoreApp *pMe)
{
  if ( SUCCESS != CoreApp_ReadSIMFile(pMe,0,AEECARD_USIM_SPDI,AEECARD_GSM_SPDI,
              &pMe->m_SPDInfo.m_pCardCmdStatus,
              &pMe->m_SPDInfo.m_spdiCallback, CoreApp_SPDIFileReadCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreApp_ReadOPLFile

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadOPLFile(CCoreApp *pMe)
{
  if (SUCCESS != CoreApp_ReadSIMFile(pMe,0,AEECARD_USIM_OPL,AEECARD_GSM_OPL,
              &pMe->m_OpListInfo.m_pCardCmdStatus,
              &pMe->m_OpListInfo.m_oplCallback, CoreApp_OPLFileReadCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreApp_ReadIMSIFile

DESCRIPTION
  Reads the IMSI from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadIMSIFile(CCoreApp *pMe)
{
  if (SUCCESS != CoreApp_ReadSIMFile(pMe,AEECARD_IMSI_LEN,AEECARD_USIM_IMSI,AEECARD_GSM_IMSI,
              &pMe->m_pCardCmdStatus,&pMe->m_sCallback, CoreApp_ReadIMSIFileCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreApp_ReadCPHSOpNameFile

DESCRIPTION
  Reads the CPHS Op Name from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadCPHSOpNameFile(CCoreApp *pMe)
{
  if (SUCCESS != CoreApp_ReadSIMFile(pMe,0,AEECARD_USIM_ONS,AEECARD_GSM_ONS,
              &pMe->m_CPHSOpNameInfo.m_pCardCmdStatus,
              &pMe->m_CPHSOpNameInfo.m_cphsCallback,
              CoreApp_CPHSOpNameFileReadCb))
  {
    return EFAILED;
  }

  return SUCCESS;
}
/*===========================================================================
FUNCTION CoreApp_FileAttributeReadCb

DESCRIPTION
  Call Back called with File Attributes

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/

static void CoreApp_FileAttributeReadCb( void *pData)
{
  CCoreApp *pMe = (CCoreApp*)pData;

  if (SUCCESS == pMe->m_OpListInfo.m_FileAttrib.nCmdStatus &&
      COREAPP_FILE_ID_HIGH_BYTE == pMe->m_OpListInfo.m_FileAttrib.byFileID[0] &&
      COREAPP_FILE_ID_LOW_BYTE_OPL == pMe->m_OpListInfo.m_FileAttrib.byFileID[1])
  {
    pMe->m_OpListInfo.m_FileAttrib.nCmdStatus = EFAILED;
    pMe->m_OpListInfo.m_FileAttrib.nNumberRecords =
      MIN(pMe->m_OpListInfo.m_FileAttrib.nNumberRecords, COREAPP_MAX_OPL_LIST_LEN);
    MSG_MED("Successfully read OPL File Attributes",0,0,0);
    CoreApp_ReadOPLFile(pMe);
    return;
  }

  if (SUCCESS == pMe->m_PNNInfo.m_FileAttrib.nCmdStatus &&
      COREAPP_FILE_ID_HIGH_BYTE == pMe->m_PNNInfo.m_FileAttrib.byFileID[0] &&
      COREAPP_FILE_ID_LOW_BYTE_PNN == pMe->m_PNNInfo.m_FileAttrib.byFileID[1])
  {
    pMe->m_PNNInfo.m_FileAttrib.nCmdStatus = EFAILED;
    pMe->m_PNNInfo.m_FileAttrib.nNumberRecords =
      MIN(pMe->m_PNNInfo.m_FileAttrib.nNumberRecords, COREAPP_MAX_PNN_ARRAY_SIZE);
    MSG_MED("Successfully read PNN File Attributes",0,0,0);
    CoreApp_ReadPNNFile(pMe);
    return;
  }

  CORE_ERR("EONS:Attributes Can't Be Read",0,0,0);
  return;
}
/*===========================================================================
FUNCTION CoreApp_GetFileAttributes

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_GetFileAttributes(CCoreApp *pMe,
                                     AEECardFileAttribStatus *pFileAttrib,
                                     AEECardFileItemEnumType usimFile,
                                     AEECardFileItemEnumType gsmFile,
                                     AEECallback             *pCallBack)
{
  AEECardApp                    pCardApp;
  uint8                         slot;
  AEECardFilePathType           filePathInfo;
  AEECardFileItemEnumType       pFileList[1];


  if (SUCCESS != CoreApp_FillGenericData(pMe,&filePathInfo,&pCardApp,
                                       &slot,
                                       pFileList,
                                       usimFile,
                                       gsmFile
                                       ))
  {
    CORE_ERR("EONS:Could Not Fill Generic Data",0,0,0);
    return EFAILED;
  }

  CALLBACK_Init(pCallBack, CoreApp_FileAttributeReadCb,(void*)(pMe));

  if (SUCCESS != ICARD_GetFileAttributes(pMe->m_pICard,
                        slot,
                        filePathInfo,
                        pFileAttrib,
                        pCallBack))
  {
    CORE_ERR("EONS:Could Not Get File Attributes",0,0,0);
    return EFAILED;
  }

  return SUCCESS;
}
/*===========================================================================
FUNCTION CoreApp_ReadAllEONSFiles

DESCRIPTION
  Reads the data from the SIM

DEPENDENCIES
 pMe->m_pICard


SIDE EFFECTS
  None.

===========================================================================*/
static int CoreApp_ReadAllEONSFiles(CCoreApp *pMe)
{
  pMe->m_OpListInfo.numRecsRead = 0;
  pMe->m_PNNInfo.numRecsRead = 0;
  pMe->m_CPHSOpNameInfo.m_NameLen = 0;

  pMe->m_OpListInfo.m_FileAttrib.nNumberRecords = COREAPP_INVALID_NUM_RECS;
  pMe->m_PNNInfo.m_FileAttrib.nNumberRecords = COREAPP_INVALID_NUM_RECS;
  pMe->m_OpListInfo.bIsOPLInfoAvailable = FALSE;
  pMe->m_PNNInfo.bIsPNNInfoAvailable = FALSE;

  CoreApp_GetFileAttributes(pMe,&pMe->m_OpListInfo.m_FileAttrib,
                            AEECARD_USIM_OPL,
                            AEECARD_GSM_OPL,
                            &pMe->m_OpListInfo.m_oplCallback);
  CoreApp_GetFileAttributes(pMe,&pMe->m_PNNInfo.m_FileAttrib,
                            AEECARD_USIM_PNN,
                            AEECARD_GSM_PNN,
                            &pMe->m_PNNInfo.m_pnnCallback);
  //Read the SPN,OPL and PNN files
  //Initialize OPL Info
  CoreApp_ReadSPNFile(pMe);
  CoreApp_ReadSPDIFile(pMe);
  CoreApp_ReadIMSIFile(pMe);
  CoreApp_ReadCPHSOpNameFile(pMe);

  return SUCCESS;
}

#endif /* #ifdef FEATURE_CPHS_EONS */

/*=============================================================================
FUNCTION: CoreApp_GetCardStatus

DESCRIPTION:
Obtains Card Status for a given slot

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_GetCardStatus(CCoreApp *pMe,uint8 slot)
{
  /* Get the card status from card */
  if (AEECARD_SLOT1 == slot) {
    ICARD_GetCardStatus(pMe->m_pICard, (uint8 *) &(pMe->m_nCardStatus));
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (AEECARD_SLOT2 == slot) {
    ICARD_GetCardStatusSlot(pMe->m_pICard, AEECARD_SLOT2, (uint8 *) &(pMe->m_nCardStatus_slot2));
  }
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */
  else {
    return FALSE;
  }

  /* populate the pMe structure */
  PopulateCardApplications(pMe);
  return TRUE;
}

/*=============================================================================
FUNCTION: CoreApp_ProcessValidCardEvent

DESCRIPTION:
  Process AEECARD_VALID_CARD event

SIDE EFFECTS:
  None.

=============================================================================*/
static void CoreApp_ProcessValidCardEvent(CCoreApp *pMe)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif
  if(!pMe->mbReadFiles)
  {
    pMe->mbReadFiles = TRUE;
    CoreApp_ReadFiles(pMe);
#ifdef FEATURE_CPHS_EONS
    CoreApp_ReadAllEONSFiles(pMe);
    CoreApp_RegisterForFCN(pMe);
#endif /* FEATURE_CPHS_EONS */
  }
  ValidateCardsAndChangeState(pMe,FALSE);
  CoreApp_ProcessSubscriptionStatus(pMe);
}

/*=============================================================================
FUNCTION: CoreApp_ProcessNoCardEvent

DESCRIPTION:
  Process ICard Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
static void CoreApp_ProcessNoCardEvent(CCoreApp *pMe,uint8 slot)
{
  if (AEECARD_SLOT1 == slot) {
    pMe->m_dwSecurityMask &= ~COREAPP_SLOT1_INFO_MASK;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (AEECARD_SLOT2 == slot) {
    /* remove the security mask for pin1 code slot 2 and nv */
    pMe->m_dwSecurityMask &= ~COREAPP_SLOT2_INFO_MASK;
  }
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */
  else {
    CORE_ERR("Unknown slot id %d ",slot,0,0);
    return;
  }


#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif
  if(pMe->m_bSuspended) {
    ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP);
  }
  pMe->mbReadFiles = FALSE;
  /* Subscription Status not available will trigger release of calls */
  ValidateCardsAndChangeState(pMe,FALSE);
  CoreApp_ProcessSubscriptionStatus(pMe);
}

/*=============================================================================
FUNCTION: CoreApp_SetPinMask

DESCRIPTION:
  Determines pin lock state and sets the security mask.

SIDE EFFECTS:
  None.

=============================================================================*/
static void CoreApp_SetPinMask(CCoreApp *pMe,uint8 pin_lock_state,uint8 slot)
{
  uint32 slot_code_mask=0;  

  //Set Mask
  if ( slot == AEECARD_SLOT1 )
  {
    slot_code_mask = COREAPP_PIN1_CODE_MASK;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else
  {
    slot_code_mask = COREAPP_PIN1_SLOT2_CODE_MASK;
  }
#endif
   
  switch (pin_lock_state)
  {
    case AEECARD_PIN_PERM_DISABLED:
    case AEECARD_PIN_BLOCKED:
      //Reset security mask slot
      pMe->m_dwSecurityMask = pMe->m_dwSecurityMask & ~(slot_code_mask);          
      break;

    case AEECARD_PIN_VERIFIED:
    case AEECARD_PIN_DISABLED:
      pMe->m_dwSecurityMask |= slot_code_mask;
      break;

    default:
      break;       
  }
}

/*=============================================================================
FUNCTION: CoreApp_ProcessICardEvents

DESCRIPTION:
  Process ICard Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_ProcessICardEvents(CCoreApp *pMe, uint16 wParam, uint32 dwParam)
/*lint -esym(715,wParam)*/
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  boolean bShowManPLMNSel;
#endif

  if (pMe == NULL) {
    CORE_ERR("NULL pMe", 0, 0, 0);
    return FALSE;
  }

  CORE_ERR("Coreapp: Got ICARD event.  Mask %d", ((AEENotify *) dwParam)->dwMask,0,0);

  switch (((AEENotify *) dwParam)->dwMask )
  {
    case NMASK_CARD_STATUS:
      if (FALSE == CoreApp_GetCardStatus(pMe,AEECARD_SLOT1))
      {
        CORE_ERR("CoreApp_GetCardStatus Failed for Slot One",0, 0, 0);
        return FALSE;
      }

      CORE_ERR("Got ICARD CARD_STATUS event.  Status %d",pMe->m_nCardStatus,0,0);

      switch(pMe->m_nCardStatus) {
        case AEECARD_VALID_CARD: /* received SIM_INIT_COMPLETED_EVENT */
          /* Check if we have missed the PIN event. If we have update
           * the PIN lock state at the time of receving VALID card event
           */
          
          if ( pMe->m_sPIN1_lock_state == AEECARD_PIN_UNKNOWN )
          {
            AEECardPinStatus pin_status;
            (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN1, &pin_status);
            pMe->m_sPIN1_lock_state = pin_status.lock_state;            

            CORE_MSG("Coreapp: Valid card event.PIN1_STATUS %d",pMe->m_sPIN1_lock_state,0,0);

            //Set Security masks
            CoreApp_SetPinMask(pMe,pin_status.lock_state,AEECARD_SLOT1);
          }     
          // Note: There is an intentional fall through at this point.
          // Hence there is no return statement for this case.       
        case AEECARD_AVAIL_CARD:                    
          CoreApp_ProcessValidCardEvent(pMe);
          return FALSE; //needs state specific processing

        case AEECARD_ILLEGAL_CARD:
        case AEECARD_NO_CARD:      /* No card in the slot */
        case AEECARD_INVALID_CARD: /* card has been permanently blocked */
          CoreApp_ProcessNoCardEvent(pMe,AEECARD_SLOT1);
          return TRUE;

#if defined(FEATURE_GSTK) || defined (FEATURE_MMGSDI_DUAL_SLOT)
        case AEECARD_CARD_REINIT:
          if(pMe->m_bSuspended) {
              (void) ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP);
          }
          pMe->mbReadFiles = FALSE;
          (void)SetIdleMenuText(NULL);
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
          return FALSE;  //needs state specific processing
#endif

        case AEECARD_NOT_INIT:
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
          /* Send RTRE config changed to CM - response from CM will */
          /* generate subscription changed event */
          SendRTREConfig(pMe);
#endif
          pMe->mbReadFiles = FALSE;
          ValidateCardsAndChangeState(pMe,FALSE);
          return TRUE;
     
        default:
          return FALSE;
      }

    case NMASK_PIN1_STATUS:
      pMe->m_sPIN1_lock_state = ((AEECardPinStatus *) (((AEENotify *)dwParam)->pData))->lock_state;

      CORE_MSG("Coreapp: Got ICARD PIN1_STATUS event.  Status %d",pMe->m_sPIN1_lock_state,0,0);
      //Set Security masks
      CoreApp_SetPinMask(pMe,pMe->m_sPIN1_lock_state,AEECARD_SLOT1);

      switch (pMe->m_sPIN1_lock_state)
      {
        case AEECARD_PIN_PERM_DISABLED:
        case AEECARD_PIN_BLOCKED:
          ValidateCardsAndChangeState(pMe,FALSE);
          CoreApp_ProcessSubscriptionStatus(pMe);
          break;

        case AEECARD_PIN_ENABLED:
          ValidateCardsAndChangeState(pMe,FALSE);
          break;

        case AEECARD_PIN_UNKNOWN:
          CORE_ERR("Unknown Pin Status...Software Error",0,0,0);
          break;

        case AEECARD_PIN_VERIFIED:
        case AEECARD_PIN_DISABLED:
          break;

        default:
          return FALSE;
      }
      return TRUE;

      
#ifdef FEATURE_MMGSDI_PERSONALIZATION
    case NMASK_CARD_PERSO_STATUS:
      ICARD_GetPersoStatus(pMe->m_pICard, AEECARD_SLOT1, (uint8 *) &(pMe->m_nPersoStatus));
      ValidateCardsAndChangeState(pMe,FALSE);
      return TRUE;
#endif /*FEATURE_MMGSDI_PERSONALIZATOIN */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case NMASK_CARD_SLOT2_STATUS:
      if (FALSE == CoreApp_GetCardStatus(pMe,AEECARD_SLOT2))
      {
          CORE_ERR("CoreApp_GetCardStatus Failed for Slot Two",0, 0, 0);
        return FALSE;
      }

      switch(pMe->m_nCardStatus_slot2) {
        case AEECARD_VALID_CARD:
          /* Check if we have missed the PIN event. If we have update
           * the PIN lock state at the time of receving VALID card event
           */
          
          if ( pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_UNKNOWN )
          {
            AEECardPinStatus pin_status;
            (void) ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2, &pin_status);
            pMe->m_sPIN1_slot2_lock_state = pin_status.lock_state;            

            CORE_MSG("Coreapp: Valid card event.PIN1_STATUS %d",pMe->m_sPIN1_lock_state,0,0);
            //Set Security masks
            CoreApp_SetPinMask(pMe,pin_status.lock_state,AEECARD_SLOT2);   
          }
          // Note: There is an intentional fall through at this point.
          // Hence there is no return statement for this case. 
        case AEECARD_AVAIL_CARD:          
          pMe->m_dwSecurityMask |= COREAPP_PIN1_SLOT2_CODE_MASK;
          CoreApp_ProcessValidCardEvent(pMe);
          return FALSE; //needs state specific processing

        case AEECARD_ILLEGAL_CARD:
        case AEECARD_NO_CARD:      /* No card in slot */
        case AEECARD_INVALID_CARD: /* card has been permanently blocked */
          CoreApp_ProcessNoCardEvent(pMe,AEECARD_SLOT2);
          return TRUE;


#if defined(FEATURE_GSTK) || defined (FEATURE_MMGSDI_DUAL_SLOT)
      case AEECARD_CARD_REINIT:
        if(pMe->m_bSuspended) {
            ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP);
        }
        (void)SetIdleMenuText(NULL);
        pMe->mbReadFiles = FALSE;
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
        return FALSE;  //needs state specific processing
#endif

      case AEECARD_NOT_INIT:
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
        /* Send RTRE config changed to CM - response from CM will */
        /* generate subscription changed event */
        SendRTREConfig(pMe);
#endif
        pMe->mbReadFiles = FALSE;
        ValidateCardsAndChangeState(pMe,FALSE);
        return TRUE;

      default:
        return FALSE;
      }
      return TRUE;

    case NMASK_PIN1_SLOT2_STATUS:
      pMe->m_sPIN1_slot2_lock_state = ((AEECardPinStatus *) (((AEENotify *)dwParam)->pData))->lock_state;
      //Set Security masks
      CoreApp_SetPinMask(pMe,pMe->m_sPIN1_slot2_lock_state,AEECARD_SLOT1);
      
      switch (pMe->m_sPIN1_slot2_lock_state)
      {
        case AEECARD_PIN_PERM_DISABLED:
        case AEECARD_PIN_BLOCKED:                      
          ValidateCardsAndChangeState(pMe,FALSE);
          CoreApp_ProcessSubscriptionStatus(pMe);
          break;

        case AEECARD_PIN_ENABLED:
          ValidateCardsAndChangeState(pMe,FALSE);
          break;

        case AEECARD_PIN_UNKNOWN:          
          CORE_ERR("Unknown Pin Status...Software Error",0,0,0);
          break;

        case AEECARD_PIN_VERIFIED:
        case AEECARD_PIN_DISABLED:
          break;

        default:
          return FALSE;
      }
      return TRUE;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    case NMASK_FILE_READ:
      {
        AEECardFileContents *pFileContents = (AEECardFileContents*)((AEENotify *)dwParam)->pData;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        int i;
#endif

        if(pFileContents == NULL)
        {
          return FALSE;
        }
        MSG_HIGH("File Change Notification Received for file %x",pFileContents->filename,0,0);

        switch(pFileContents->filename)
        {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
          case AEECARD_GSM_CSP:
          case AEECARD_USIM_CSP:
            i =0;

            /*Every resource has two bytes - Byte A and Byte B*/
            while(i<pFileContents->length)
            {
              if(pFileContents->data[i] == COREAPP_CPHS_VAS_OFFSET)
              {
                break;
              }
              i = i+2; /*Look at every alternate byte to locate the offset*/
            }

            // If i is still less than the total length, then we have found the offset
            if(i < pFileContents->length)
            {
              // Check the next byte to find out the data(PLMN Mode Bit)
              if(pFileContents->data[i+1] & COREAPP_PLMN_MODE_BIT)
              {
                // The Manual PLMN Selection items should be displayed
                bShowManPLMNSel = TRUE;
              }
              else
              {
                //The Manual Selection Item should be removed
                bShowManPLMNSel = FALSE;
              }
              if (pMe->m_pIConfig != NULL)
               {
                 (void)ICONFIG_SetItem(pMe->m_pIConfig,
                                       CFGI_MANUAL_PLMN_SEL_ALLOWED,
                                       (void *)&bShowManPLMNSel,
                                       sizeof(bShowManPLMNSel));
               }

              //In both cases, the mode should be reverted to automatic.
              ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_phInfo, sizeof(AEECMPhInfo));

              if( pMe->m_phInfo.network_sel_mode_pref != AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC)
              {
                SetNetworkAutoSelection( pMe,
                                         AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                                         AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                         AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                                         AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE);
              }

            }

            break;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#ifdef FEATURE_CPHS_EONS
          case AEECARD_GSM_OPL:
          case AEECARD_USIM_OPL:
            pMe->m_OpListInfo.numRecsRead = 0;
            pMe->m_OpListInfo.bIsOPLInfoAvailable = FALSE;
            pMe->m_OpListInfo.m_FileAttrib.nNumberRecords = COREAPP_INVALID_NUM_RECS;
            CoreApp_GetFileAttributes(pMe,&pMe->m_OpListInfo.m_FileAttrib,
                                      AEECARD_USIM_OPL,
                                      AEECARD_GSM_OPL,
                                      &pMe->m_OpListInfo.m_oplCallback);
          break;


          case AEECARD_GSM_SPN:
          case AEECARD_USIM_SPN:
            CoreApp_ReadSPNFile(pMe);
          break;


          case AEECARD_GSM_PNN:
          case AEECARD_USIM_PNN:
            pMe->m_PNNInfo.numRecsRead = 0;
            pMe->m_PNNInfo.bIsPNNInfoAvailable = FALSE;
            pMe->m_PNNInfo.m_FileAttrib.nNumberRecords = COREAPP_INVALID_NUM_RECS;
            CoreApp_GetFileAttributes(pMe,&pMe->m_PNNInfo.m_FileAttrib,
                                      AEECARD_USIM_PNN,
                                      AEECARD_GSM_PNN,
                                      &pMe->m_PNNInfo.m_pnnCallback);
          break;

          case AEECARD_GSM_IMSI:
          case AEECARD_USIM_IMSI:
            CoreApp_ReadIMSIFile(pMe);
            break;

          case AEECARD_GSM_SPDI:
          case AEECARD_USIM_SPDI:
            CoreApp_ReadSPDIFile(pMe);
          break;

          case AEECARD_GSM_ONS:
          case AEECARD_USIM_ONS:
            pMe->m_CPHSOpNameInfo.m_NameLen =0;
            CoreApp_ReadCPHSOpNameFile(pMe);
          break;

#endif /* #ifdef FEATURE_CPHS_EONS */
          default:
            CORE_ERR("Invalid File %d",pFileContents->filename,0,0);
            break;
        }
        break;
      }
    default:
      break;
  }
  return FALSE;
}

#if defined(FEATURE_ENHANCED_OVERLAY) || defined(FEATURE_MMOVERLAY)

/*=============================================================================
FUNCTION: CoreApp_SetupOvParams

DESCRIPTION:
  This function sets up Overlay parameters for targets with
  enhanced overlays

RETURN VALUE:
  SUCCESS - Overlay was correctly set up
  Any other value - Overlay could not be correctly set up

SIDE EFFECTS:
  None.

=============================================================================*/
int CoreApp_SetupOvParams(CCoreApp *pMe) {
#if defined(FEATURE_ENHANCED_OVERLAY)
  IDisplayCfg    *pIDisplayCfg = NULL;
  IOverlay       *pIOverlay = NULL;
  int             nSuccess;

  if (pMe == NULL) {
    CORE_ERR("Null pMe pointer.", 0, 0, 0);
    return EFAILED;
  }

  nSuccess = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DISPLAYCFG1,
                                   (void **)&pIDisplayCfg);
  if ((nSuccess != SUCCESS) || (pIDisplayCfg == NULL)) {
    CORE_ERR("Unable to create display cfg, %d.", nSuccess, 0, 0);
    return nSuccess;
  }

  do {
    OverlayTypeUnion p1;
    OverlayTypeUnion p2;

    pIOverlay = IDISPLAYCFG_GetPrimaryOverlay(pIDisplayCfg);
    if (pIOverlay == NULL) {
      nSuccess = EFAILED;
      CORE_ERR("Could not get primary overlay.", 0, 0, 0);
      break;
    }

    /* Make sure this overlay is on the foreground. */
    p1.uint_data = OV_LAYER_TOP_LAYER;
    p2.uint_data = 0;
    nSuccess = IOVERLAY_SetParam(pIOverlay, OV_PARM_LAYER, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set fg layer, %d", nSuccess, 0, 0);
      break;
    }

    /* Turn transparency on, so that we can see any background image. */
    p1.uint_data = 1;
    p2.rgb_data = MAKE_RGB(0xFF, 0xff, 0xCF);
    nSuccess = IOVERLAY_SetParam(pIOverlay, OV_PARM_TRANSPARENCY, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set transparency, %d", nSuccess, 0, 0);
      break;
    }

    p1.float_data = 0.5; 
    p2.uint_data =  0;
    nSuccess = IOVERLAY_SetParam(pIOverlay, OV_PARM_ALPHA_BLEND_PCT, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set alpha percent, %d", nSuccess, 0, 0);
      break;
    }

  } while (0);

  /* Clean up after ourselves. */
  if (pIOverlay != NULL) {
    (void)IOVERLAY_Release(pIOverlay);
  }
  (void)IDISPLAYCFG_Release(pIDisplayCfg);
  return nSuccess;
#else
  IMMOverlayMgr  *pIMMOverlayMgr = NULL;
  int             nSuccess;
  AEEPoint        destPoint;
  IBitmap        *pIBitmap = NULL;

  if (pMe == NULL) {
    CORE_ERR("Null pMe pointer.", 0, 0, 0);
    return EBADPARM;
  }

  //If this is already set up, leave it alone
  if (pMe->m_pFgImgOverlay != NULL) {
    CORE_MSG("FG Overlay already set up",0,0,0);
    return SUCCESS;
  }

   nSuccess = IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay,
                                       &(pIBitmap));
   if ((SUCCESS != nSuccess) || (NULL == pIBitmap)) {
      CORE_ERR("Unable to get primary display bitmap.", 0, 0, 0);
      return nSuccess;
}

  nSuccess = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MMOVERLAYMGR,
                                   (void **)&pIMMOverlayMgr);
  if ((nSuccess != SUCCESS) || (pIMMOverlayMgr == NULL)) {
    CORE_ERR("Unable to create MMOverlayMgr, %d.", nSuccess, 0, 0);
    (void) IBITMAP_Release(pIBitmap);
    return nSuccess;
  }

  destPoint.x = 0;
  destPoint.y = 0;

  nSuccess = IMMOVERLAYMGR_CreateOverlay(pIMMOverlayMgr, pIBitmap,
                                         AEECLSID_DISPLAY1, destPoint,
                                         &pMe->m_pFgImgOverlay);

  (void) IBITMAP_Release(pIBitmap);
  (void) IMMOVERLAYMGR_Release(pIMMOverlayMgr);
  pIMMOverlayMgr = NULL;
 
  if ((nSuccess != SUCCESS) || (pMe->m_pFgImgOverlay == NULL)) {
    CORE_ERR("Unable to create MMOverlay, %d.", nSuccess, 0, 0);
    return nSuccess;
  }

  do {
    uint32 p1, p2;

    /* Make sure this overlay is on the foreground. */
    p1 = OV_LAYER_TOP_LAYER;
    p2 = 0;
    nSuccess = IMMOVERLAY_SetParam(pMe->m_pFgImgOverlay, OV_PARM_LAYER, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set fg layer, %d", nSuccess, 0, 0);
      break;
    }

    /* Turn transparency on, so that we can see any background image. */
    p1 = 1;
    p2 = MAKE_RGB(0xFF, 0xFF, 0xCF);
    nSuccess = IMMOVERLAY_SetParam(pMe->m_pFgImgOverlay, OV_PARM_TRANSPARENCY, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set transparency, %d", nSuccess, 0, 0);
      break;
    }

    p1 = 0xC0; 
    p2 = 0;
    nSuccess = IMMOVERLAY_SetParam(pMe->m_pFgImgOverlay, OV_PARM_ALPHA_BLEND_PCT, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set alpha percent, %d", nSuccess, 0, 0);
      break;
    }

    p1 = 1; 
    p2 = 0;
    nSuccess = IMMOVERLAY_SetParam(pMe->m_pFgImgOverlay, OV_PARM_SYSTEM_OVERLAY, p1, p2);
    if (nSuccess != SUCCESS) {
      CORE_ERR("Could not set as system overlay, %d", nSuccess, 0, 0);
      break;
    }

  } while (0);

  return nSuccess;
#endif /* FEATURE_ENHANCED_OVERLAY */
}

/*=============================================================================
FUNCTION: CoreApp_ShowBgImage

DESCRIPTION:
  This function creates the background image.  If background image
  display is enabled, then it displays the background image.

SIDE EFFECTS:
  None.

=============================================================================*/
#if defined(FEATURE_MMOVERLAY)
static void CoreApp_ShowBgImage(CCoreApp *pMe, IImage *pIImage)
{
  IConfig       *pIConfig = NULL;
  int      nRetVal;
  boolean  bDisableBgImage = FALSE;  // Default to BG images not disabled
  AEEPoint       sPoint;
  IDisplay       *pDispClone = NULL;
  IBitmap        *pDevBitmap = NULL;
  IBitmap        *pDevBitmapCopy = NULL;
  AEEBitmapInfo   sBitmapInfo;
  IMMOverlayMgr *pIMMOvMgr = NULL;
  IMMOverlay    *pIMMOverlay = NULL;
  uint32            ovParm1;
  uint32            ovParm2;
  uint32            unRetVal;

  if (pMe == NULL)
  {
    CORE_MSG("NULL applet pointer.", 0, 0, 0);
    return;
  }

  do
  {
    if (pMe->m_pBgImage == NULL)
    {
      CORE_MSG("No background image found.", 0, 0, 0);
      break; 
    }

    /* First clone the primary display and get its bitmap. */
    nRetVal = IDISPLAY_Clone(pMe->a.m_pIDisplay, &pDispClone);
    if ((nRetVal != SUCCESS) || (pDispClone == NULL))
    {
      CORE_MSG("Failed cloning display, %d, 0x%x.", nRetVal, pDispClone, 0);
      break; 
    }

    nRetVal = IDISPLAY_GetDeviceBitmap(pDispClone, &pDevBitmap);
    if ((nRetVal != SUCCESS) || (pDevBitmap == NULL))
    {
      CORE_MSG("Failed getting device bitmap, %d, 0x%x.", nRetVal, pDevBitmap, 0);
      break; 
    }

    /* Now create a compatible bitmap (i.e. create a copy), and set
     * the cloned display to render to the new bitmap
     */
    nRetVal = IBITMAP_GetInfo(pDevBitmap, &sBitmapInfo, sizeof(sBitmapInfo));
    if (nRetVal != SUCCESS)
    {
      CORE_MSG("Failed getting bitmap info, %d.", nRetVal, 0, 0);
      break; 
    }

    nRetVal = IBITMAP_CreateCompatibleBitmap(pDevBitmap, &pDevBitmapCopy,
		                             (uint16)sBitmapInfo.cx,
                                             (uint16)sBitmapInfo.cy);
    if ((nRetVal != SUCCESS) || (pDevBitmapCopy == NULL))
    {
      CORE_MSG("Failed creating compatible bitmap, %d, 0x%x.",
               nRetVal, pDevBitmapCopy, 0);
      break; 
    }

    nRetVal = IDISPLAY_SetDestination(pDispClone, pDevBitmapCopy);
    if (nRetVal != SUCCESS)
    {
      CORE_MSG("Failed creating compatible bitmap, %d.", nRetVal, 0, 0);
      break; 
    }

    /* Finally, tell the IImage to draw to the cloned display, which will
     * result in the background image being written to the new bitmap.
     */
    IIMAGE_SetDisplay(pMe->m_pBgImage, pDispClone);
    IIMAGE_Draw(pMe->m_pBgImage, 0, 0);

    // Now, we need to create an overlay in the background layer and assign
    // pDevBitmapCopy to that overlay.
    nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MMOVERLAYMGR,
                                    (void **)&pIMMOvMgr);
    if ((nRetVal != SUCCESS) || (pIMMOvMgr == NULL))
    {
      CORE_MSG("Unable to create overlay manager, %d., 0x%s",
               nRetVal, pIMMOvMgr, 0);
      break;
    }

    // Create an overlay, assigning to it the bitmap containing the
    // background image.
    sPoint.x = 0;
    sPoint.y = 0;
    nRetVal = IMMOVERLAYMGR_CreateOverlay(pIMMOvMgr, pDevBitmapCopy,
                                          AEECLSID_DISPLAY1,
                                          sPoint, &(pMe->m_pBgImgOverlay));
    if (pMe->m_pBgImgOverlay == NULL)
    {
      CORE_MSG("Unable to create overlay.", 0, 0, 0);
      break;
    }

    // Now move the overlay to the background layer.
    ovParm1 = OV_LAYER_BASE_LAYER;
    ovParm2 = 0;
    nRetVal = IMMOVERLAY_SetParam(pMe->m_pBgImgOverlay, OV_PARM_LAYER,
                                ovParm1, ovParm2);
    if (nRetVal != SUCCESS)
    {
      // We couldn't move it to the background, so destroy it.
      CORE_MSG("Unable to move overlay to bg layer, %d.", nRetVal, 0, 0);
      break;
    }

    /* Update the rectangle so the param change takes effect.  Setting
     * the dirty rectangle pointer to NULL forces update of the entire
     * overlay region.
     */
    unRetVal = (int)IMMOVERLAY_UpdateAsync(pMe->m_pBgImgOverlay, NULL, 0);
    if (unRetVal == 0xFFFFFFFF)
    {
      /* Failed to update the overlay. */
      CORE_MSG("Unable to update overlay, %d.", unRetVal, 0, 0);
      break;
    }

   /* Set out "displayed" flag, so we don't try to re-display it. */
   pMe->m_bBgImageDisplayed = TRUE;

   // If the background image is disabled by user request, disable
   // the overlay.
  nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void **)&pIConfig);
  if ((nRetVal == SUCCESS) && pIConfig) {
    (void)ICONFIG_GetItem(pIConfig, CFGI_DISABLE_BG_IMAGE, &bDisableBgImage,
                          sizeof(bDisableBgImage));
  } else {
     CORE_MSG("Failed to create IConfig", 0, 0, 0);
  }

  if (bDisableBgImage == TRUE)
  {
     CORE_MSG("Disabling bg overlay.", 0, 0, 0);
     CoreApp_ToggleBgImage(pMe, FALSE);
   }

  }
  while (0);

  /* Now cleanup everything we created, except for the
   * overlay.  It needs to stay around for the background
   * image.
   */
  if (pIConfig != NULL)
  {
    (void)ICONFIG_Release(pIConfig);
  }
  if (pDispClone != NULL)
  {
    (void)IDISPLAY_Release(pDispClone);
  }
  if (pDevBitmap != NULL)
  {
    (void)IBITMAP_Release(pDevBitmap);
  }
  /* Release this bitmap, too, as the overlay AddRef'd it. */
  if (pDevBitmapCopy != NULL)
  {
    (void)IBITMAP_Release(pDevBitmapCopy);
  }
  if (pMe->m_pBgImage != NULL)
  {
    (void)IIMAGE_Release(pMe->m_pBgImage);
    pMe->m_pBgImage = NULL;
  }

  if (pIMMOvMgr != NULL)
  {
    (void)IMMOVERLAYMGR_Release(pIMMOvMgr);
  }
  if (pIMMOverlay != NULL)
  {
    (void)IMMOVERLAY_Release(pIMMOverlay);
  }
}
#endif /* FEATURE_MMOVERLAY */

/*=============================================================================
 FUNCTION: CoreApp_StartShowBgImage

DESCRIPTION:
  This function starts the process of displaying the background image.

  Because the display is only available while the applet is running,
  this routine checks if the app is running (i.e. not suspended).
   > If the app has been suspended, it sets a flag to indicate that the
     background image has been rendered and then returns.  The background
     image will then be rendered whenever the applet resumes.
   > If the app is running, it still sets the flag to indicate that the
     background image has been rendered.  It then displays the image and
     sets a flag to indicate that the image has been displayed.

  Note that, if the applet has ended, the IImage will have been released,
  and the notification to invoke this function will (should) have been
  cancelled by the IIMAGE_Release.

SIDE EFFECTS:
  None.

=============================================================================*/
#if defined(FEATURE_MMOVERLAY)
static void CoreApp_StartShowBgImage (
      void *         pUser, 
      IImage *       pIImage, 
      AEEImageInfo * pi,
      int nErr
   )
{
  CCoreApp *pMe = NULL;

  if (pUser == NULL)
  {
    CORE_MSG("NULL data pointer.", 0, 0, 0);
    return;
  }
  pMe = (CCoreApp *)pUser;

  if (nErr != SUCCESS)
  {
    CORE_MSG("Image could not be rendered, %d.", nErr, 0, 0);
    return;
  }

  /* Image has now been rendered into the bitmap of the IImage. */
  pMe->m_bBgImageRendered = TRUE;

  /* If the applet is suspended, don't display the image now.  The applet
   * will display it when we receive and EVT_APP_RESUME.
   */
  if (pMe->m_bSuspended == TRUE)
    {
    return;
  }

  /* Otherwise, display the image now. */
  CoreApp_ShowBgImage(pMe, pIImage);
}
#endif /* FEATURE_MMOVERLAY */

/*=============================================================================
FUNCTION: CoreApp_InitBgImage

DESCRIPTION:
  This function loads the background image and initiates the
  process of displaying it.

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_InitBgImage(CCoreApp *pMe)
{
#if defined(FEATURE_MMOVERLAY)
  AEEDeviceInfo  sDevInfo;
  uint32         nScreenSize;

  if (pMe == NULL)
  {
    CORE_MSG("NULL applet pointer", 0, 0, 0);
    return FALSE;
  }

  if (pMe->m_pBgImgOverlay != NULL)
  {
    CORE_MSG("Non-NULL bg overlay pointer at init.", 0, 0, 0);
    (void)IMMOVERLAY_Release(pMe->m_pBgImgOverlay);
    pMe->m_pBgImgOverlay = NULL;
  }

  nScreenSize = GetPrimaryScreenSize();
  /* Only load the bitmap for targets that support a background image.
   * At the same time, make sure to correctly load the portrait versus
   * landscape image.
   */
  if (nScreenSize == APPCOM_VGA_RES)
  {
    // If it's VGA, we need to figure out if it's landscape or portrait
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &sDevInfo);
    if (sDevInfo.cxScreen == APPCOM_VGA_DIM1)
    {
      // If it's narrower in the x direction, then it's portrait
      pMe->m_pBgImage = ISHELL_LoadResImage(pMe->a.m_pIShell, gpszResourceFile, 
                                      IDB_BGIMG_PORTRAIT);
    }
    else
    {
      // Otherwise, it's landscape
      pMe->m_pBgImage = ISHELL_LoadResImage(pMe->a.m_pIShell, gpszResourceFile, 
                                      IDB_BGIMG_LANDSCAPE);
    }
  }
  else if (nScreenSize == APPCOM_QVGA_RES)
  {
    // Only portrait is supported in QVGA
    pMe->m_pBgImage = ISHELL_LoadResImage(pMe->a.m_pIShell, gpszResourceFile, 
                                    IDB_BGIMG_PORTRAIT);
  }

  if (pMe->m_pBgImage == NULL)
  {
    MSG_LOW("No background bitmap.", 0, 0, 0);
    return FALSE;
  }

  IIMAGE_Notify(pMe->m_pBgImage, CoreApp_StartShowBgImage, (void *)pMe);
  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_MMOVERLAY */
}

/*=============================================================================
FUNCTION: CoreApp_ToggleBgImage

DESCRIPTION:
  This function enables or disables the background image.

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_ToggleBgImage(CCoreApp * pMe, boolean bTurnOn)
{
#if defined(FEATURE_MMOVERLAY)
  int nRetVal;
  int nEnableFlag = 0;
  uint32 ovParm1;
  uint32 ovParm2;

  if (pMe == NULL)
  {
    MSG_ERROR("Null pMe.", 0, 0, 0);
    return;
  }

  if (pMe->m_pBgImgOverlay == NULL)
  {
    MSG_ERROR("NULL bg image pointer.", 0, 0, 0);
    return;
  }

  if (bTurnOn == TRUE)
  {
    nEnableFlag = 1;
  }

  ovParm1 = (uint32)nEnableFlag;
  ovParm2 = 0;
  nRetVal = IMMOVERLAY_SetParam(pMe->m_pBgImgOverlay, OV_PARM_ENABLE,
                                ovParm1, ovParm2);
  if (nRetVal != SUCCESS)
  {
    MSG_ERROR("Unable to toggle the overlay, retVal=%d, toggle flag = %d",
              nRetVal, nEnableFlag, 0);
  }

  return;
#endif /* FEATURE_MMOVERLAY */

}
#endif /* FEATURE_ENHANCED_OVERLAY || FEATURE_MMOVERLAY */

/*===========================================================================

FUNCTION CoreApp_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

   Note - The switch statement in the routine is to demonstrate how event handlers are
   generally structured.  However, realizing the simplicity of the example, the code could
   have been reduced as follows:

   if(eCode == EVT_APP_START){
      IDISPLAY_DrawText();
      IDISPLAY_Update();
      return(TRUE);
   }
   return(FALSE);

   However, while doing so would have demonstrated how BREW apps can be written in about 8
   lines of code (including the app creation function), it might have confused those who wanted
   a bit more practical example.

   Also note that the use of "szText" below is provided only for demonstration purposes.  As
   indicated in the documentation, a more practical approach is to load text resources
   from the applicaton's resource file.

   Finally, note that the ONLY event that an applet must process is EVT_APP_START.  Failure
   to return TRUE from this event will indicate that the app cannot be started and BREW
   will close the applet.

PROTOTYPE:
   boolean CoreApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If the app has processed the event
   FALSE: If the app did not process the event

SIDE EFFECTS
   none
===========================================================================*/
static boolean CoreApp_HandleEvent(CCoreApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  IDialog *pIDialog;
  uint16 wDlgID;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  boolean bShowManPLMNSel;
#endif

  ASSERT( pMe != NULL ); /* error checking */

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((eCode == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch (eCode){
    case EVT_APP_START:
      MSG_HIGH ("Got EVT_APP_START", 0, 0, 0);

#if defined(FEATURE_ENHANCED_OVERLAY) || defined(FEATURE_MMOVERLAY)
      if (CoreApp_InitBgImage(pMe) == FALSE)
      {
        MSG_ERROR("Unable to init the background image.", 0, 0, 0);
      }
      CoreApp_SetupOvParams(pMe);
#endif /* FEATURE_ENHANCED_OVERLAY || FEATURE_MMOVERLAY */

      if (!pMe->m_bPwr)
      {
        if (pMe->m_pSplashImage == NULL) {
          /*lint -e620*/
          IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
          /*lint +e620*/
          ShowSplash(pMe); /* Start splash screen */
        }
        /* This is the first time the app starts so do all necessary initialization */
        (void) Initialization(pMe); /* to be moved to under EVT_SECURITY_SUCCESS
                                later on after fully integrated!!!!!!!! */

        StartCoreApp(pMe);

      }
      else
      {
        /* Handle this as if the App received EVT_APP_RESUME */
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pIDialog)
        {
          IDIALOG_Redraw(pIDialog);
        }
        else
        {
          /* Do more authentications or display idle menu dialog */
          ValidateCardsAndChangeState(pMe,TRUE);
        }
      }

      pMe->m_bSuspended = FALSE;
      ISHELL_Notify(pMe->a.m_pIShell, AEECLSID_NATIVENOTIFIER, 
                    NOTIFY_MASK_NATIVE_IDLE_EVENT, NULL);

      return(TRUE);

    case EVT_APP_STOP:
    {
      MSG_HIGH ("Got EVT_APP_STOP", 0, 0, 0);
      if (!pMe->m_bExit)
      {
        boolean *pb = (boolean *)dwParam;

        if(pb)
          *pb = FALSE;  /* Set the app to background app */
      }
      else
      {
        /* Power down since we are exiting */

        if (pMe->m_bPwr) {
          pMe->m_bUserPwrDown = FALSE;
          PowerDown(pMe);
        }
      }
      return(TRUE);
    }

    case EVT_APP_SUSPEND:
      {
        AEESuspendInfo * pi = (AEESuspendInfo *)dwParam;
        MSG_HIGH ("Got EVT_APP_SUSPEND", 0, 0, 0);
        if(pi)
          pi->bCloseDialogs = FALSE;

        pMe->m_bSuspended = TRUE;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY) SetSelOperatorDisplay,pMe);
#endif
        ISHELL_Notify(pMe->a.m_pIShell, AEECLSID_NATIVENOTIFIER, 
                      NOTIFY_MASK_NATIVE_NONIDLE_EVENT, NULL);

        return TRUE;
      }

    case EVT_APP_RESUME:
      {
        MSG_HIGH ("Got EVT_APP_RESUME", 0, 0, 0);
        pMe->m_bSuspended = FALSE;

#if defined(FEATURE_MMOVERLAY)
        /* If the background image has been rendered but not displayed,
         * then display it now.
         */
        if ((pMe->m_bBgImageDisplayed == FALSE) &&
            (pMe->m_bBgImageRendered == TRUE))
        {
          CoreApp_ShowBgImage(pMe, pMe->m_pBgImage);
        }
#endif /* FEATURE_MMOVERLAY */

        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pIDialog)
        {
          UpdateCoreText(pMe);
          IDIALOG_Redraw(pIDialog);
          if(IDIALOG_GetID(pIDialog) == IDD_CORE)
          {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
            ISHELL_SetTimer(pMe->a.m_pIShell,
                            COREAPPTIMEOUT,
                            (PFNNOTIFY) SetSelOperatorDisplay,
                            (uint32*) pMe);
#endif
          }
        }
        else
        {
          /* Do more authentications or display idle menu dialog */
          ValidateCardsAndChangeState(pMe,TRUE);
        }
        ISHELL_Notify(pMe->a.m_pIShell, AEECLSID_NATIVENOTIFIER, 
                      NOTIFY_MASK_NATIVE_IDLE_EVENT, NULL);
        return TRUE;
      }

    case EVT_DIALOG_INIT:
      MSG_LOW ("Creating Dialog %d", wParam, 0, 0);
      ASSERT(wParam < ARR_SIZE(DlgEventHandlers)); /* error checking */
      if(wParam < ARR_SIZE(DlgEventHandlers)) { /* check for array bound */
        if (DlgEventHandlers[wParam] != NULL) {
          IDIALOG_SetEventHandler((IDialog *) dwParam,
                                   DlgEventHandlers[wParam], pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
          return TRUE;
        }
      }
      return FALSE;


    case EVT_KEY:
      switch(wParam) {
        case AVK_END:
          if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL) {
            (void) DisplayIdleMenuDlg(pMe);
          }
          /* Exit E911 CB mode if reqd */
          if (pMe->m_phInfo.mode_pref == AEECM_MODE_PREF_EMERGENCY) {
            (void) ExitEmergencyMode(pMe);

          }
          return TRUE;

        case AVK_POWER: /* power key */
          /* Start powerdown timer */
#ifdef FEATURE_LPM
          if(pMe->m_bLPM)
          {
            EndLPMMode(pMe, TRUE);
            return TRUE;
          }
#endif /* FEATURE_LPM */
          pMe->m_bUserPwrDown = TRUE;
          PowerDown(pMe);
          return TRUE;

        case AVK_CLR:
          return  TRUE;

#ifdef FEATURE_AVK_VOLUME_KEYS
        /*
         * Convert vol keys to user volume events for ear-piece volume
         * Do not consume vol keys
         */
        case AVK_VOLUME_UP:
          (void)CoreApp_HandleEvent(pMe, EVT_USER, EVT_VOLUME_UP, 0);
          return FALSE;

        case AVK_VOLUME_DOWN:
          (void)CoreApp_HandleEvent(pMe, EVT_USER, EVT_VOLUME_DOWN, 0);
          return FALSE;
#endif // FEATURE_AVK_VOLUME_KEYS

        default: /* do not handle other keys */
          return FALSE;
      } /* end of key events */

    case EVT_COMMAND:
      return TRUE;

    case EVT_NOTIFY:

      MSG_HIGH ("Got EVT_NOTIFY class %d mask %d", ((AEENotify *) dwParam)->cls,\
                ((AEENotify *) dwParam)->dwMask, 0);


      if (((AEENotify *) dwParam)->cls == AEECLSID_CARD_NOTIFIER)
      {
        if (FALSE == CoreApp_ProcessICardEvents(pMe, wParam, dwParam))
        {
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          /* there can be no Active dialog at this point! */

          if (pIDialog) {  /* if there is an existing dialog, pass the event to the dialog */
            wDlgID = IDIALOG_GetID(pIDialog);
            if (TRUE == CoreApp_InvokeDlgEventHandler(wDlgID, pMe, eCode, wParam, dwParam))
            {
                  return TRUE;
            }
          }
        }
        else
        {
          return TRUE;
        }
      }
      else if (((AEENotify *) dwParam)->cls == AEECLSID_CM_NOTIFIER)
      {
        return CoreApp_ProcessICMEvents(pMe, wParam, dwParam);
      }
      else if (((AEENotify *) dwParam)->cls == AEECLSID_BATT_NOTIFIER)
      {
        return CoreApp_ProcessIBatteryEvents(pMe, wParam, dwParam);
      }
      return FALSE;

    case EVT_USER:
      MSG_HIGH ("Got EVT_USER %d", wParam, 0,0);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      /* there can be no Active dialog at this point! */

      if (pIDialog) {  /* if there is an existing dialog, pass the event to the dialog */
        wDlgID = IDIALOG_GetID(pIDialog);
        if (TRUE == CoreApp_InvokeDlgEventHandler(wDlgID, pMe, eCode, wParam, dwParam))
        {
              return TRUE;
        }
      }
      switch(wParam) {

#ifdef FEATURE_NETSETTINGS_APP
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        case EVT_SELECT_NETWORK:
        {
          // If we're in this case, we know dwParm is really a word.
          ShowManualPLMNList(pMe, (uint16)dwParam);
          return TRUE;
        }
#endif // (FEATURE_WCDMA) || defined (FEATURE_GSM)
#endif // FEATURE_NETSETTINGS_APP
        
        case EVT_EXT_PWR_ON:
          pMe->m_bExtPwr =TRUE;
          ExternalPwrOn(pMe);
          break;

        case EVT_EXT_PWR_OFF:
          pMe->m_bExtPwr = FALSE;
          ExternalPwrOff(pMe);
          break;

        case EVT_SET_OPERATING_MODE:
          if(CoreTask_GetCoreLPM())
          {
             break;
          }
          //This sets the operating mode to either LPM or ONLINE 
          //based on game mode flag
          SetGameMode(pMe, GetGameMode(pMe));
          break;

        case EVT_LPM:
#ifdef FEATURE_LPM
          if (pMe->m_phInfo.oprt_mode != AEECM_OPRT_MODE_FTM)
          {
            if (!SetLPMMode(pMe, TRUE)) {
              CORE_ERR("Cant go to LPM", 0,0,0);
            }
          }
#endif /* FEATURE_LPM */
          break;

        case EVT_UI_EXIT:
          /* AEE is about to exit, set the exit flag to TRUE */
          pMe->m_bExit = TRUE;
          return TRUE;

        case EVT_TASK_OFFLINE:
          (void) CoreApp_ProcessOffLine(pMe);
          break;

        case EVT_MC_PWR_DOWN:
          pMe->m_bUserPwrDown = FALSE;
          PowerDown(pMe);
          break;

#ifdef FEATURE_CPHS_EONS
        case EVT_SPN_READ_SUCCESS:
          MSG_MED("EONS:SPN Read Success",0,0,0);
          if(((AEECardDataBufType*) dwParam) != NULL)
          {
            int i= 0;
            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;
            pMe->m_SPNInfo.m_displayCond = pReadData->pData[i];
            i++;

            while(*pReadData->DataLen > i && (*pReadData->DataLen < COREAPP_MAX_SPN_NAME_LEN))
            {
              pMe->m_SPNInfo.m_operatorName[i-1] = (pReadData->pData[i] & COREAPP_GSM_DFLT_CODING_MASK);
              i++;
            }

            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);
          }
          break;

        case EVT_CPHS_ONS_READ_SUCCESS:
          MSG_MED("EONS:CPHS ONS Read Success",0,0,0);
          if(((AEECardDataBufType*) dwParam) != NULL)
          {
            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;
            pMe->m_CPHSOpNameInfo.m_NameLen = 0;

            while(*pReadData->DataLen > pMe->m_CPHSOpNameInfo.m_NameLen &&
                  (*pReadData->DataLen < COREAPP_MAX_CPHS_OPNAME_LEN))
            {
              if (0xFF != pReadData->pData[pMe->m_CPHSOpNameInfo.m_NameLen])
              {
                pMe->m_CPHSOpNameInfo.m_operatorName[pMe->m_CPHSOpNameInfo.m_NameLen] =
                  pReadData->pData[pMe->m_CPHSOpNameInfo.m_NameLen];
                pMe->m_CPHSOpNameInfo.m_NameLen++;
              }
              else
              {
                break;
              }
            }

            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);
        }
        break;

        case EVT_IMSI_READ_SUCCESS:
          {
            byte         imsi[AEECARD_IMSI_LEN];
            AEECMPLMNID  hplmn;
            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;

            if (pReadData && pReadData->pData) {
              MEMCPY(imsi,pReadData->pData,AEECARD_IMSI_LEN);
            }
            else
            {
              return FALSE;
            }

            if (SUCCESS == ICM_GetPLMNFromIMSI(pMe->m_pICM, &hplmn,imsi))
            {
              MEMCPY(pMe->m_hplmn.identity, hplmn.identity, COREAPP_NETWORK_PLMN_LEN);
            }
            else
            {
              return FALSE;
            }
          }
          break;

        case EVT_PNN_READ_SUCCESS:
        {
          MSG_MED("EONS:PNN Read Success",0,0,0);

          if(((AEECardDataBufType*) dwParam) != NULL && pMe->m_PNNInfo.numRecsRead < COREAPP_MAX_PNN_ARRAY_SIZE)
          {
            int i= 0,len=0;

            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;

            if (COREAPP_NETWORK_LONG_NAME_IEI == pReadData->pData[i] ){
              i++;
              len = pReadData->pData[i];
              if (len < COREAPP_MAX_NETWORK_LONGNAME_LEN) {
                i++;
                len--;
                //Extract the CI and NW Coding Scheme from Octet 3
                pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].bIsAddCI = (COREAPP_NETWORK_CI_MASK == (pReadData->pData[i] & COREAPP_NETWORK_CI_MASK));
                pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].codingScheme   = (pReadData->pData[i] & COREAPP_NETWORK_CS_MASK);
                pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].spareBitsLongName  = (pReadData->pData[i] & COREAPP_SPARE_BITS_MASK);

                i++;
                //Copy Long Network Name
                MEMCPY(pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].szLongNetworkName,(pReadData->pData+i),len);
                pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].longNameLen = len;

                //Extract The Short Network Name Now
                {
                  int shortNamePos = i+len+1;
                  int shortNamelen=0;

                  if (COREAPP_NETWORK_SHORT_NAME_IEI == pReadData->pData[shortNamePos] )
                  {
                    shortNamePos++;
                    shortNamelen = pReadData->pData[shortNamePos];

                    if (shortNamelen < COREAPP_MAX_NETWORK_SHORTNAME_LEN)
                    {
                      shortNamePos++;
                      shortNamelen--;

                      pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].spareBitsShortName  = (pReadData->pData[shortNamePos] & COREAPP_SPARE_BITS_MASK);
                      //We do not care about octet 3 in Short Name since CI and Coding scheme
                      //is goverened by Long name
                      shortNamePos++;
                      //Copy Short Network Name
                      MEMCPY(pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].szShortNetworkName,
                           (pReadData->pData+shortNamePos),
                           shortNamelen);
                      pMe->m_PNNInfo.m_pnnArray[pMe->m_PNNInfo.numRecsRead].shortNameLen = shortNamelen;

                    }
                    else
                    {
                      CORE_ERR("EONS:Short Name Len %d expecting %d",shortNamelen,COREAPP_MAX_NETWORK_SHORTNAME_LEN,0);
                    }
                  }
                  else
                  {
                    CORE_ERR("EONS:Short Name IEI %d expecting %d",pReadData->pData[shortNamePos],COREAPP_NETWORK_SHORT_NAME_IEI,0);
                  }
                }
              }
              else
              {
                CORE_ERR("EONS:Long Name Len %d expecting %d",len,COREAPP_MAX_NETWORK_LONGNAME_LEN,0);
                pMe->m_PNNInfo.bIsPNNInfoAvailable = FALSE;
                FREEIF(pReadData->DataLen);
                FREEIF(pReadData->pData);
                FREEIF(pReadData);
                return FALSE;
              }
            }
            else
            {
              CORE_ERR("EONS:Long Name IEI %d expecting %d",pReadData->pData[i],COREAPP_NETWORK_LONG_NAME_IEI,0);
              //PNN Info is Invalidated
              pMe->m_PNNInfo.bIsPNNInfoAvailable = FALSE;
              FREEIF(pReadData->DataLen);
              FREEIF(pReadData->pData);
              FREEIF(pReadData);
              return FALSE;
            }
            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);

            pMe->m_PNNInfo.numRecsRead++;
            if (pMe->m_PNNInfo.m_FileAttrib.nNumberRecords != COREAPP_INVALID_NUM_RECS) {
              if (pMe->m_PNNInfo.numRecsRead <  pMe->m_PNNInfo.m_FileAttrib.nNumberRecords ) {
              //Read the Next Record
              CoreApp_ReadPNNFile(pMe);
              }
              else
              {
                pMe->m_PNNInfo.bIsPNNInfoAvailable = TRUE;
              }
            }
            else
            {
              pMe->m_PNNInfo.bIsPNNInfoAvailable = FALSE;
            }
          }
          else
          {
            MSG_MED("EONS:Max PNN Records Read ",0,0,0);
          }
        }
        break;

        case EVT_SPDI_READ_SUCCESS:
          MSG_MED("EONS:SPDI Read Success",0,0,0);
          if(((AEECardDataBufType*) dwParam) != NULL)
          {
            int i= 0;
            int16 numPLMNs=0;

            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;
            pMe->m_SPDInfo.numPLMNs=0;
            //1st BYte is the Tag
            if (pReadData->pData[i] != COREAPP_NETWORK_SPDI_TAG)
            {
              FREEIF(pReadData->DataLen);
              FREEIF(pReadData->pData);
              FREEIF(pReadData);
              return FALSE;
            }
            i++;

            //2nd Byte is the length
            numPLMNs = (pReadData->pData[i])/COREAPP_NETWORK_PLMN_LEN;
            i++;

            if (numPLMNs > 0 ) {
              pMe->m_SPDInfo.pPLMNIds = (AEECMPLMNID*) MALLOC((numPLMNs)* sizeof(AEECMPLMNID));
            }
            else
            {
              FREEIF(pReadData->DataLen);
              FREEIF(pReadData->pData);
              FREEIF(pReadData);
              return FALSE;
            }

            while (numPLMNs) {
              MEMCPY(pMe->m_SPDInfo.pPLMNIds+pMe->m_SPDInfo.numPLMNs,((uint8*)pReadData->pData)+i,COREAPP_NETWORK_PLMN_LEN);
              i+= COREAPP_NETWORK_PLMN_LEN;
              --numPLMNs;
              pMe->m_SPDInfo.numPLMNs++;
            }

            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);
          }
        break;

        case EVT_OPL_READ_SUCCESS:
        {
          MSG_MED("EONS:OPL Read Success",0,0,0);

          if(((AEECardDataBufType*) dwParam) != NULL && pMe->m_OpListInfo.numRecsRead < COREAPP_MAX_OPL_LIST_LEN)
          {
            int i= 0;
            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;

            MEMCPY(pMe->m_OpListInfo.plmnInfo[pMe->m_OpListInfo.numRecsRead].PLMNId.identity,
                 (uint8*)pReadData->pData+i,COREAPP_NETWORK_PLMN_LEN);
            i+=3;


            pMe->m_OpListInfo.plmnInfo[pMe->m_OpListInfo.numRecsRead].lac1 =
                (((pReadData->pData[i] & 0x00FF) << 8 ) | (pReadData->pData[i+1] & 0x00FF));
            i+=2;

            pMe->m_OpListInfo.plmnInfo[pMe->m_OpListInfo.numRecsRead].lac2 =
                (((pReadData->pData[i] & 0x00FF) << 8 ) | (pReadData->pData[i+1] & 0x00FF));
            i+=2;

            //PNN Record Number
            pMe->m_OpListInfo.plmnInfo[pMe->m_OpListInfo.numRecsRead].pnnRecNum = pReadData->pData[i];
            i+=1;

            pMe->m_OpListInfo.numRecsRead++;
            pMe->m_OpListInfo.numPLMNs++;

            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);

            if (COREAPP_INVALID_NUM_RECS != pMe->m_OpListInfo.m_FileAttrib.nNumberRecords) {

              if (pMe->m_OpListInfo.numRecsRead <  pMe->m_OpListInfo.m_FileAttrib.nNumberRecords )
              {
                //Read the Next Record
                CoreApp_ReadOPLFile(pMe);
              }
              else
              {
                pMe->m_OpListInfo.bIsOPLInfoAvailable = TRUE;
              }
            }
            else
            {
              pMe->m_OpListInfo.bIsOPLInfoAvailable = FALSE;
            }
          } // End of if
          else
          {
            MSG_MED("EONS:Max OPL Records Read ",0,0,0);
          }
        }
        break;
#endif /* FEATURE_CPHS_EONS */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        case EVT_CSP_READ_SUCCESS:
          if(((AEECardDataBufType*) dwParam) != NULL)
          {
            int i= 0;
            AEECardDataBufType* pReadData = (AEECardDataBufType*) dwParam;

            while(i<*pReadData->DataLen)
            {
              if(pReadData->pData[i] == COREAPP_CPHS_VAS_OFFSET)
              {
                break;
              }
              i = i+2; /*Look at every alternate byte to locate the offset*/
            }

            // If i is still less than the total length, then we have found the offset
            if(i < *pReadData->DataLen)
            {
              // Check the next byte to find out the data(PLMN Mode Bit)
              if(pReadData->pData[i+1] & COREAPP_PLMN_MODE_BIT)
              {
                // The Manual PLMN Selection items should be displayed
                bShowManPLMNSel = TRUE;
              }
              else
              {
                //The Manual Selection Item should be removed
                bShowManPLMNSel = FALSE;
              }
              if (pMe->m_pIConfig != NULL)
               {
                 (void)ICONFIG_SetItem(pMe->m_pIConfig,
                                       CFGI_MANUAL_PLMN_SEL_ALLOWED,
                                       (void *)&bShowManPLMNSel,
                                       sizeof(bShowManPLMNSel));
               }

              //Only if the mode bit is 0, the mode should be reverted to automatic.
              ICM_GetPhoneInfo(pMe->m_pICM, &pMe->m_phInfo, sizeof(AEECMPhInfo));

              if ((bShowManPLMNSel == FALSE) && 
                  (pMe->m_phInfo.network_sel_mode_pref != AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC))
              {
                SetNetworkAutoSelection( pMe,
                                         AEECM_MODE_PREF_NO_CHANGE, AEECM_PREF_TERM_PERMANENT,
                                         AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                         AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                                         AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE);

              }
            }
            FREEIF(pReadData->DataLen);
            FREEIF(pReadData->pData);
            FREEIF(pReadData);
          }
          break;
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        case EVT_MESSAGE_END: // to be replaced by a proper SUPS notification
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
          if(pMe->m_bNetSelErr)
          {
            pMe->m_bNetSelErr = FALSE;
            DisplayPLMNListAgain(pMe);
          }
#endif
          break;
#endif /* FEATURE_WCDMA || F_GSM */

        case EVT_SUBSCRIPTION_CHANGED:
          /* Phone subscription changed */
          CoreApp_ProcessSubscriptionStatus (pMe);
          (void)GetRestrict(pMe);           /* restrict */

          /* Inform BREW sub changed */
          {
            IDownload* pDownload = NULL;
            IConfig*   pConfig   = NULL;

            /* Create the objects which contain the information. */
            ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_DOWNLOAD,
                                  (void**)&pDownload);
            ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void**)&pConfig);

            if(pConfig && pDownload)
            {
               int nSIDLen;
               char* pSID = NULL;
               char* pSIDToSet = NULL;
               int nResult = AEE_SUCCESS;
               AEEDownloadInfo DownloadInfo;

               nResult = ICONFIG_GetItem(pConfig, CFGI_DOWNLOAD,
                                         (void*)&DownloadInfo,
                                         sizeof(DownloadInfo));
               if(nResult == AEE_SUCCESS)
               {
                  if(DownloadInfo.wFlags & DIF_MIN_FOR_SID)
                  {
                     AEEMobileInfo MobileInfo;
                     nResult = ICONFIG_GetItem(pConfig, CFGI_MOBILEINFO,
                                               &MobileInfo, sizeof(MobileInfo));
                     if(nResult == AEE_SUCCESS)
                     {
                        pSIDToSet = STRDUP(MobileInfo.szMobileID);
                     }
                     else
                     {
                       ERR("Could not get MOBILEINFO %d", nResult, 0, 0);
                     }
                  }
                  else
                  {
                     nResult = ICONFIG_GetItem(pConfig, CFGI_SUBSCRIBERID_LEN,
                                               &nSIDLen, sizeof(nSIDLen));
                     if(nResult == AEE_SUCCESS)
                     {
                        pSID = (char*)MALLOC(nSIDLen);
                        if(pSID)
                        {
                          nResult = ICONFIG_GetItem(pConfig, CFGI_SUBSCRIBERID,
                                                    pSID, nSIDLen);
                          if(nResult == AEE_SUCCESS)
                          {
                             pSIDToSet = STRDUP(pSID);
                          }
                          else
                          {
                            ERR("Could not get SUBSCRIBER ID %d", nResult, 0, 0);
                          }
                        }
                        else
                        {
                          ERR("Could not get memory for SID %d", nSIDLen, 0, 0);
                        }
                     }
                     else
                     {
                       ERR("Could not get SUBSCRIBERID_LEN %d", nResult, 0, 0);
                     }
                  }
               }
               else
               {
                 ERR("Could not get CFGI_DOWNLOAD", 0, 0, 0);
               }

               if(pSIDToSet)
               {
                 int i;
                 nSIDLen = STRLEN(pSIDToSet);
                 MSG_HIGH("Setting Subscriber ID len %d ...", nSIDLen,0, 0);
                 for (i = 0; i < nSIDLen; i+=3)
                 {
                   MSG_MED("Subscriber ID %c%c%c", pSIDToSet[i],
                           ((i+1)<nSIDLen?pSIDToSet[i+1]:'x'),
                           ((i+2)<nSIDLen?pSIDToSet[i+2]:'x'));
                 }
                 IDOWNLOAD_SetSubscriberID(pDownload, pSIDToSet, nSIDLen+1);
               }
               FREEIF(pSIDToSet);
               FREEIF(pSID);
            }
            else
            {
              ERR("Could not create interfaces %x %x", pDownload, pConfig, 0);
            }

            if(pDownload)
               IDOWNLOAD_Release(pDownload);
            if(pConfig)
               ICONFIG_Release(pConfig);
          }
          break;

        case EVT_SYS_PREF_ERR:
          /* Display the message */
          {
            uint16 wStringID = 0;
            wStringID = LookupResIDForSysPrefErrCode(dwParam);

            if (wStringID != 0)
            {
              /* Set m_bNetSelErr to TRUE only if this error was due
               * to network selection. */
              if (pMe->m_bNetSel)
                pMe->m_bNetSelErr = TRUE;
              DisplayMessageDlg(pMe, wStringID, 3);
            }
            else
            {
              FREEIF(pMe->m_pCopyDetailedPLMNList);
            }

            if (pMe->m_bNetSel){
              pMe->m_bNetSel = FALSE; /* Default back to FALSE */
            }

            return TRUE;
          }

        case EVT_HEADSET_ON: /* mono headset on */
          HeadsetOn(pMe);
          return TRUE;

        case EVT_STEREO_HEADSET_ON: /* stereo headset on */
          StereoHeadsetOn(pMe);
          return TRUE;

        case EVT_HEADSET_OFF: /* headset off */
          HeadsetOff(pMe);
          return TRUE;

        case EVT_USB_GENERIC_BUTTON_PRESS:
        case EVT_USB_VOICEDIAL_BUTTON_PRESS:
          // Nothing should be done until coreapp starts.
          if(pMe->m_bLPM)
            return TRUE;

          //Send the event to the active app if not me
          //Also check to see if this is a valid car kit device.
          if(GetUSBHFKAttached() || GetStereoUSBHFKAttached())
          {
            if(pMe->m_bSuspended)
            {
              AEECLSID clsID = ISHELL_ActiveApplet(pMe->a.m_pIShell);
              if(clsID !=0 && clsID != AEECLSID_CORE_APP)
              {
#ifdef FEATURE_APP_PUREVOICE
                /*CoreApp is suspended. Check to see it Purevoice is already up*/
                /*If not , start Purevoice*/
                if(clsID!=AEECLSID_PUREVOICE && !AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM)) {
                  ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_PUREVOICE);
                }
                else
#endif
                {
                  ISHELL_PostEvent (pMe->a.m_pIShell, clsID,
                                EVT_USER,wParam,0L);
                }
              }
            }
#ifdef FEATURE_APP_PUREVOICE
            else //CoreApp is the active App
            {
              //Check to see if there is any call up. If not, start Purevoice
              if(!AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM)) {
                ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_PUREVOICE);
              }
            }
#endif
          }
          return TRUE;

        case EVT_HFK_ON: /* on hook */
          PhoneOnHook(pMe);
          return TRUE;

        case EVT_HFK_OFF: /* phone off hook */
          PhoneOffHook(pMe);
          return TRUE;

        case EVT_USB_HFK_ON: /* usb hfk on */
          TurnOnUSB_hfk(pMe);
          return TRUE;

        case EVT_USB_STEREO_HFK_ON: /* usb hfk on */
          TurnOnStereoUSB_hfk(pMe);
          return TRUE;

        case EVT_USB_HFK_OFF: /* usb hfk off */
          TurnOffUSB_hfk(pMe);
          return TRUE;

        case EVT_USB_RETURN_TO_CRADLE: /* usb hfk back in cradle */
          ReturnToCradleUSB_hfk(pMe);
          return TRUE;

        case EVT_USB_REMOVE_FROM_CRADLE: /* usb hfk out of cradle */
          RemoveFromCradleUSB_hfk(pMe);
          return TRUE;

        case EVT_SDAC_ON: /* sdac on */
          TurnOnSdac(pMe);
          return TRUE;

        case EVT_SDAC_OFF: /* sdac off */
          TurnOffSdac(pMe);
          return TRUE;

        case EVT_SPEAKERPHONE_TOGGLE:
          {
#ifdef FEATURE_SPEAKER_PHONE
            boolean bSpeakerPhone = uisnd_is_device_enabled(SND_DEVICE_SPEAKER_PHONE);
            if (bSpeakerPhone)
            { //turn off the speaker phone 
              uisnd_set_device_status(SND_DEVICE_SPEAKER_PHONE, UISND_DEV_DISABLED);
            }
            else
            { //turn on the speaker phone
              uisnd_set_device_status(SND_DEVICE_SPEAKER_PHONE, UISND_DEV_ENABLED);
            }
#endif 
            uisnd_set_device_auto(NULL,NULL);
            InitializeVolumes(pMe);
          }
          return TRUE;

        case EVT_VOLUME_UP:
             {
               uint32 wVolumeLvl = GetEarpieceVolume(pMe);
               switch(wVolumeLvl)
               {
               case UISND_4TH_VOL:
                 break;

               case UISND_3RD_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_4TH_VOL);
                 break;

               case UISND_2ND_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_3RD_VOL);
                 break;

               case UISND_1ST_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_2ND_VOL);
                 break;

               default:
                 break;
               }

             }
             return TRUE;

        case EVT_VOLUME_DOWN:
             {
               uint32 wVolumeLvl = GetEarpieceVolume(pMe);
               switch(wVolumeLvl)
               {
               case UISND_4TH_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_3RD_VOL);
                 break;

               case UISND_3RD_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_2ND_VOL);
                 break;

               case UISND_2ND_VOL:
                 (void) SetEarpieceVolume(pMe, (uint64)UISND_1ST_VOL);
                 break;

               case UISND_1ST_VOL:
                 break;

               default:
                 break;
               }

             }
             return TRUE;

        case EVT_SECURITY_SUCCESS:
        case EVT_SECURITY_FAIL:
          /*Do not need to handle the event if the app is not up!*/
          if(pMe->m_bSuspended)
            return TRUE;

          if(pMe->m_bLPM)
          {
            // Do Nothing
            return TRUE;
          }
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
          if (pMe->m_bConfigSent)
#endif
          {
            /* Subscription status changed */
            ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                    EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
          }
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
          else {
            /* Send RTRE config changed to CM - response from CM will */
            /* generate subscription changed event */
            SendRTREConfig(pMe);
          }
#endif /* UIM_RUN_TIME_ENABLE */
          return TRUE;

        case EVT_PIN_VERIFY_SUCCESS: /* Verify PIN1, PIN2 or NV successfully */
        case EVT_PIN_ENABLE_SUCCESS:
        case EVT_PIN_CHANGE_SUCCESS:
        case EVT_PUK_VERIFY_SUCCESS:  /* PUK verification success */
        case EVT_PIN_VERIFY_FAIL:
        case EVT_PIN_ENABLE_FAIL:
        case EVT_PIN_CHANGE_FAIL:
        case EVT_PUK_VERIFY_FAIL:
          ValidateCardsAndChangeState(pMe,FALSE);
          return TRUE;

        case EVT_START_IDLE_DIALOG:
          return DisplayIdleMenuDlg (pMe);

        case EVT_CORE_SET_IDLETEXT:
          if(((AECHAR*) dwParam) != NULL)
          {
            (void) WSTRLCPY(pMe->m_pszIdleMenuText,(AECHAR*) dwParam, MAX_BROADCAST_MSG_SIZE + 1);
            if(pMe->m_bSuspended)
            {
              ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP);
            }
            FREE((AECHAR*) dwParam);
          }
          else
          {
            pMe->m_pszIdleMenuText[0] = (AECHAR) '\0';
          }
          (void) DisplayIdleMenuDlg(pMe);
          UpdateCoreText(pMe);
          return TRUE;

        case EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED:
          if(((AECHAR*) dwParam) != NULL)
          {
            (void) WSTRLCPY(pMe->m_pszIdleMenuText,(AECHAR*) dwParam, MAX_BROADCAST_MSG_SIZE + 1);
            FREE((AECHAR*) dwParam);
          }
          else
          {
            pMe->m_pszIdleMenuText[0] = (AECHAR) '\0';
          }
          UpdateCoreText(pMe);
          return TRUE;

         case EVT_SET_BACKLIGHT:
          if((boolean) dwParam == TRUE)
          {
            pMe->m_byBacklight = UI_BKLT_TIME_MAX;
            UpdateBacklight(pMe);
          }
          else
            GetBackLightDuration(pMe);
          return TRUE;

        case EVT_FLIP:
#if defined(FEATURE_SECONDARY_DISPLAY)
#ifdef T_FFA6550_SS
          if (((boolean)dwParam == FALSE) &&
              (hs_query_articulated_state() == TRUE))
          {
            /* Do nothing, since main display is still visible */
          } else
#endif /* T_FFA6550_SS */
          {
          // There's nothing to do if it failed, so just cast as void
          (void)ToggleDisplays(pMe, (boolean)dwParam);
          }
#endif // FEATURE_SECONDARY_DISPLAY
          return TRUE;

        case EVT_LCD_ON:
          if((boolean) dwParam == TRUE) {
            IANNUNCIATOR_EnableBlink(pMe->m_pIAnn, TRUE);
            /* Start timer to display time */
            ISHELL_SetTimer(pMe->a.m_pIShell,
                            1000,
                            (PFNNOTIFY) DisplayTimeWithRefresh,
                            pMe);

#ifdef FEATURE_HDR
            RefreshRSSIAnnunciator(pMe, TRUE);
#endif
          }
          else 
          {
            IANNUNCIATOR_EnableBlink(pMe->m_pIAnn, FALSE);
          }
          return TRUE;
        
        default:
          CORE_ERR ("EVT_USER not handled %d", wParam, 0, 0);
          return FALSE;

      }

      return FALSE;

      case EVT_VOICE_PRIVACY:
      {
         IClipboard * clBd = NULL;
         void *input_data; /* get data from diag */
         char clipboardEntryName[CLIPBOARD_NAME_MAX_LENGTH];
         uint32 len;
         len = dwParam;

         // initiailize
         input_data = (void *) MALLOC(sizeof(uint32));
         if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CLIPBOARD, (void**)(&clBd)) != SUCCESS)
         {
           return (FALSE);
         }
         *((uint32*)input_data)=0;
         (void)SNPRINTF(clipboardEntryName, ARR_SIZE(clipboardEntryName),"AUTOMATION_%d", wParam);
         // get data from clipboard
         if(ICLIPBOARD_GetData(clBd,clipboardEntryName,(void  *)input_data ,(int*)&len)!=SUCCESS)
         {
           return (FALSE);
         }

         if (SetPrivacy(pMe, (uint64)*((uint32*)input_data)) != TRUE)
         {
           MSG_LOW("Failed to set privacy", 0, 0, 0);
         }

         // finished task. Cleanup..
         FREE(input_data);
         ICLIPBOARD_SetData(clBd,clipboardEntryName,0,0);
         return (TRUE);
      }


    default:
       break;
  }
  return(FALSE);
}  /* End CoreApp_HandleEvent */



/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
  int AEEAppCreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,IApplet** ppApplet)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int CoreApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(AEEApplet_New( sizeof(CCoreApp),                    /* Size of our private class */
                     ClsId,                               /* Our class ID */
                     pIShell,                             /* Shell interface */
                     pMod,                                /* Module instance */
                     (IApplet**)ppObj,                    /* Return object */
                     (AEEHANDLER)CoreApp_HandleEvent,     /* Our event handler */
                     (PFNFREEAPPDATA)CoreApp_FreeAppData))/* Cleanup function */
   {
     if(CoreApp_InitAppData((IApplet*)*ppObj)) {
       return(AEE_SUCCESS);
     }
     //Release the applet. This will free the memory allocated for the applet when
     IAPPLET_Release(((IApplet*)*ppObj));
   }
  return (EFAILED);
} /* End CoreApp_CreateInstance / AEEClsCreateInstance */



#ifdef AEE_STATIC
/*=============================================================================
FUNCTION: CoreApp_Load

DESCRIPTION:
    This function loads the application.

PROTOTYPE:
static int CoreApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> pHelpers:
--> pMod:

RETURN VALUE:


COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
int CoreApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,
          CoreApp_CreateInstance,NULL));
} /* End CoreApp_Load */



/*=============================================================================
FUNCTION: CoreApp_GetModInfo

DESCRIPTION:
    Gets the CoreApp Info since it is a static application.

PROTOTYPE:
    PFNMODENTRY CoreApp_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                                 AEEAppInfo ** pApps, uint16 * pnApps,
                   uint16 * pwMinPriv)

PARAMETERS:
--> ps: Pointer to ISHELL interface
--> ppClasses:
--> pApps:
--> pnApps:
--> pwMinPriv:

RETURN VALUE:
 A pointer to the function to load the Application.

COMMENTS:


SIDE EFFECTS:
none

SEE ALSO:
none
=============================================================================*/
PFNMODENTRY CoreApp_GetModInfo(IShell * ps, AEECLSID ** ppClasses,
                               AEEAppInfo ** pApps,
                               uint16 * pnApps,uint16 * pwMinPriv)
/*lint -esym(715,ps)*/
/*lint -esym(715,ppClasses)*/
{
   *pApps = (AEEAppInfo *)&gaiCoreApp;
   *pnApps = 1;
   *pwMinPriv = PL_SYSTEM;   /* CoreApp has system priviliges */
   return((PFNMODENTRY)CoreApp_Load);
} /* End CoreApp_GetModInfo */

//lint -restore
#endif  /* AEE_STATIC */
