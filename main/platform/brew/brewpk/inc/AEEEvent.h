#ifndef _AEEEvent_H_
#define _AEEEvent_H_

/*======================================================
FILE:  AEEEvent.h

SERVICES:  Include file event type definitions

GENERAL DESCRIPTION:
   Defines BREW Event types.

Copyright ?1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

//
// AEE Events - These events are passed to Applets and/or Controls
//
// Note:  User/App defined events are specified as starting at EVT_USER
//

#define EVT_APP_START            0     // Main App started - dwParam = (AEEAppStart *)
#define EVT_APP_STOP             0x1   // App stopped - dwParam = (boolean *).  Set to FALSE if not closed.
#define EVT_APP_SUSPEND          0x2   // App suspended.  wParam = AEESuspendReason, dwParam = AEESuspendInfo *
#define EVT_APP_RESUME           0x3   // App resume - dwParam = (AEEAppStart *)
#define EVT_APP_CONFIG           0x4   // Alternate App Start - configuration screen should be shown...
#define EVT_APP_HIDDEN_CONFIG    0x5   // Alternate App Start - hidden configuration screen...
#define EVT_APP_BROWSE_URL       0x6   // Called after EVT_APP_START - dwParam = (const char * pURL)
#define EVT_APP_BROWSE_FILE      0x7   // Called after EVT_APP_START
#define EVT_APP_MESSAGE          0x8   // Text message - wParam = AEESMSEncoding, dwParam = String format dependant on wParam value
#define EVT_ASYNC_ERROR          0x9   // Async error notification - dwParam = AEEAsyncError *
#define EVT_APP_TERMINATE        0xa   // Forcefully terminate app terminate
#define EVT_EXIT                 0xb   // BREW shutdown/exit message
#define EVT_APP_RESTART          0xc   // Sent to background apps after upgrades
#define EVT_EXT_STOP             0xd   // Sent to all open apps - dwParam = zero terminated list of AEECLSIDs of extension
#define EVT_EXT_CREATE           0xe   // Extension class created
#define EVT_EXT_RELEASE          0xf   // Extension class released
#define EVT_APP_MESSAGE_EX       0x10  // Text message - wParam = 0, dwParam = Transaction ID
#define EVT_APP_START_BACKGROUND 0x11  // App started in the background
#define EVT_APP_WOULD_START      0x12  // Notification to top-visible app of ISHELL_StartApplet() of same app
#define EVT_APP_POST_URL         0x13  // Called with dwParam as (const char * pURL) but no EVT_APP_START
#define EVT_APP_START_WINDOW     0x14  // reserved for future use
#define EVT_APP_LAST_EVENT       (EVT_APP_START_WINDOW)

// Key Events

#define EVT_KEY                  0x100 // App key - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_PRESS            0x101 // App keydown - wParam= KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_RELEASE          0x102 // App keyRelease - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#ifdef CUST_EDITION	
#define EVT_KEY_HELD             0x103 // App keyHeld - wParam = KEYCODE, dwParam = 0
#define EVT_KEY_KEYREPEAT        0X106  //KEY REPEAT

#endif /*CUST_EDITION*/
#define EVT_CHAR                 0x104 // Character Event wParam = AECHAR, dwParam = Bitflags for modifier keys
#define EVT_UPDATECHAR           0x105 // Char update Event wParam = AECHAR, dwParam = Bitflags for modifier keys

// key filtering/hook events.
//   These events are sent to running applications with AFLAG_PHONE set.
//   If an AFLAG_PHONE application handles the key hook event, the key event is not passed
//   to the current top visible app.  This provides a way for privileged apps to provide custom
//   handling for specific key events such as AVK_END.
#define EVT_KEY_HOOK             0x110 // hook key - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_HOOK_PRESS       0x111 // hook keydown - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_HOOK_RELEASE     0x112 // hook keyRelease - wParam = KEYCODE, dwParam = Bitflags for modifier keys


// Control Events...

#define EVT_COMMAND              0x200 // App custom control...
#define EVT_CTL_TAB              0x201 // App TAB event sent by controls - dwParam = control, wParam = 0-left, 1-right
#define EVT_CTL_SET_TITLE        0x202 // Message interface to set title - wParam = ID, dwParam = res file (if ID != 0) or text
#define EVT_CTL_SET_TEXT         0x203 // Message interface to set text - wParam = ID, dwParam = res file (if ID != 0) or text
#define EVT_CTL_ADD_ITEM         0x204 // Message interface to add item - dwParam = CtlAddItem
#define EVT_CTL_CHANGING         0x205 // App dwParam = CtlValChange
#define EVT_CTL_MENU_OPEN        0x206 // Sent by ITextCtl before menu activated...
#define EVT_CTL_SKMENU_PAGE_FULL 0x207 // Sent by IMenuCtl when SK menu page is full (dwParam = IMenuCtl *)
#define EVT_CTL_SEL_CHANGED      0x208 // Menu - wParam - selection ID, (dwParam = IMenuCtl *)
#define EVT_CTL_TEXT_MODECHANGED 0x209 // Sent by ITextCtl when mode was changed - wParam = AEETextInputMode, (dwParam = ITextCtl *)

// Dialog Events...

#define EVT_DIALOG_INIT          0x300 // Dialog Event - Controls created, pre-init values, flags, etc.
#define EVT_DIALOG_START         0x301 // Dialog Event - Dialog opening - wShortData = ID, dwParam = IDialog *
#define EVT_DIALOG_END           0x302 // Dialog Event - Dialog completed normally - wShortData = ID, dwParam = IDialog *
#define EVT_COPYRIGHT_END        0x303 // Dialog Event - Copyright dialog ended

// Shell Events...

#define EVT_ALARM                0x400 // App wShortData = uCode
#define EVT_NOTIFY               0x401 // dwParam = AEENotify *
#define EVT_APP_NO_CLOSE         0x404 // App should not be closed
#define EVT_APP_NO_SLEEP         0x405 // App is working - called after long periods of non-idle app
#define EVT_MOD_LIST_CHANGED     0x406 // List of modules changed.  May be sent while app suspended!
#define EVT_NOTIFY_FAILURE       0x407 // Notification registration failed - dwParam = AEENotifyFailure *

#define EVT_BUSY                 (EVT_APP_NO_CLOSE)

// Device Events...

#define EVT_FLIP                 0x500 // wParam = TRUE if open, FALSE if closed.
                                       // This type of event is sent when a device's LCD is blocked 
                                       // when a clamtype device is closed and reexposed when opened
                                       // dwParm = AEEFlipInfo (see AEEDeviceNotifier.h)
#define EVT_LOCKED               0x501 // Deprecated from BREW 3.1
#define EVT_KEYGUARD             0x502 // wParam = TRUE if keyguard is on.
                                       // This type of event is sent when a device's keypad is blocked 
                                       // when a clamtype device is closed and reexposed when opened. 
                                       // This event can also be used when keys are locked by software,
                                       // not by a piece of physical hardware.
                                       // dwParm = AEEFlipInfo (see AEEDeviceNotifier.h)
#define EVT_HEADSET              0x503 // wParam = TRUE if headset is plugged in, FALSE if otherwise
#define EVT_PEN_DOWN             0x504 // Deprecated, see EVT_POINTER_DOWN. dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_MOVE             0x505 // Deprecated, see EVT_POINTER_MOVE. dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_UP               0x506 // Deprecated, see EVT_POINTER_UP. dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_STALE_MOVE       0x507 // Deprecated, see EVT_POINTER_STALE_MOVE. dwParam = position: signed x in upper 16 bits, y in lower
                                       // Stale move events are reported when the event queue contains
                                       // additional moves
                                       // AEE_GET_X() and AEE_GET_Y() macros may be used to extract
                                       // the position from dwParam
#define EVT_JOYSTICK_POS         0x508 // dwParam = position: signed x in upper 16 bits, y in lower
                                       // wParam = 15 most significant bits of the wParam represent
                                       // 15 least significant bits of GETUPTIMEMS() and thus can be 
                                       // used to calculate the time lag between events.
                                       // The LSB in wParam is 1 if an EVT_JOYSTICK_POS event was 
                                       // dropped before the current EVT_JOYSTICK_POS event. If the 
                                       // previous joystick event is in the event queue, then 
                                       // the LSB of wParam will be 0.
#define EVT_JOYSTICK_STALE_POS   0x509 // dwParam = position: signed x in upper 16 bits, y in lower
                                       // Stale move events are reported when the event queue contains
                                       // additional moves
                                       // AEE_GET_X() and AEE_GET_Y() macros may be used to extract
                                       // the position from dwParam
#define EVT_SCR_ROTATE           0x50a // wParam = AEEScrOrientation (Portrait or Landscape)
                                       // dwParam = screen ID
#define EVT_POINTER_DOWN         0x50b // Sent when the pointing device is put 
                                       // down on the capable screen.
                                       // dwParam = NUL-terminated single-byte-character 
                                       // string with delimiter-based name=hex value pair 
                                       // representing information about pointing device.
                                       // e.g.:"x=00000017,y=FFFFFFFB,time=00003456,clkcnt=2,modifiers=0000000000000000,", where
                                       // x = 23 & y = -5 are the decimal values.
                                       // Helper functions defined in AEEPointerHelpers.h
                                       // can be used to obtain the name-value pairs. See
                                       // AEE_POINTER_GET_XY() for more details and references
                                       // to other similar helper functions.
                                       // wParam = size of dwParam string in bytes including 
                                       // terminating NUL-character.
#define EVT_POINTER_MOVE         0x50c // Sent when the pointing device is moved across the 
                                       // capable screen while remaining in contact with the 
                                       // screen. This is equivalent to dragging the pointing device.
                                       // dwParam = NUL-terminated single-byte-character 
                                       // string with delimiter-based name=hex value pair 
                                       // representing information about pointing device.
                                       // e.g.:"x=00000017,y=FFFFFFFB,time=00003456,clkcnt=2,modifiers=0000000000000000,", where
                                       // x = 23 & y = -5 are the decimal values.
                                       // Helper functions defined in AEEPointerHelpers.h
                                       // can be used to obtain the name-value pairs. See
                                       // AEE_POINTER_GET_XY() for more details and references
                                       // to other similar helper functions.
                                       // wParam = size of dwParam string in bytes including 
                                       // terminating NUL-character.
#define EVT_POINTER_UP           0x50d // Sent when the pointing device is lifted 
                                       // up from the capable screen.
                                       // dwParam = NUL-terminated single-byte-character 
                                       // string with delimiter-based name=value pair 
                                       // representing information about pointing device.
                                       // e.g.:"x=00000017,y=FFFFFFFB,time=00003456,clkcnt=2,modifiers=0000000000000000,", where
                                       // x = 23 & y = -5 are the decimal values. 
                                       // Helper functions defined in AEEPointerHelpers.h
                                       // can be used to obtain the name-value pairs. See
                                       // AEE_POINTER_GET_XY() for more details and references
                                       // to other similar helper functions.
                                       // wParam = size of dwParam string in bytes including 
                                       // terminating NUL-character.
#define EVT_POINTER_STALE_MOVE   0x50e // Stale pointer move events are reported when 
                                       // the event queue contains additional moves.
                                       // dwParam = NUL-terminated single-byte-character 
                                       // string with delimiter-based name=value pair 
                                       // representing information about pointing device.
                                       // e.g.:"x=00000017,y=FFFFFFFB,time=00003456,clkcnt=2,modifiers=0000000000000000,", where
                                       // x = 23 & y = -5 are the decimal values. 
                                       // Helper functions defined in AEEPointerHelpers.h
                                       // can be used to obtain the name-value pairs. See
                                       // AEE_POINTER_GET_XY() for more details and references
                                       // to other similar helper functions.
                                       // wParam = size of dwParam string in bytes including 
                                       // terminating NUL-character.

// Clipboard events...

#define EVT_CB_CUT               0x600 // Cut request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_COPY              0x601 // Copy request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_PASTE             0x602 // Paste request - no parameters

// OEM Events...
#define EVT_OEM_START            0x5800   // OEM-specific events start @ 0x5800 

#ifdef CUST_EDITION	
// 自定义事件请全部在此定义
enum
{
    EVT_USER_REDRAW  = (EVT_OEM_START+1),
    EVT_DISPLAYDIALOGTIMEOUT,
    EVT_UPDATEIDLE,
    EVT_ORIGINATE_CALL,
    
    EVT_WMS_CFG_GW_READY,
    EVT_WMS_CFG_CDMA_READY,
    EVT_WMS_CFG_REFRESH_DONE,
    EVT_WMS_CFG_ROUTES,
    EVT_WMS_CFG_MEMORY_STATUS,
    EVT_WMS_CFG_MESSAGE_LIST,//10
    EVT_WMS_CFG_MEMORY_FULL,
    EVT_WMS_CFG_GW_DOMAIN_PREF,
    EVT_WMS_CFG_CELL_CHANGE,
    EVT_WMS_CFG_PRIMARY_CLIENT_SET,
    
    EVT_WMS_MSG_SEND,
    EVT_WMS_MSG_ACK,
    EVT_WMS_MSG_READ,
    EVT_WMS_MSG_WRITE,
    EVT_WMS_MSG_DELETE,
    EVT_WMS_MSG_COPY,//20
    EVT_WMS_MSG_DELETE_BOX,
    EVT_WMS_MSG_MODIFY_TAG,
    EVT_WMS_MSG_READ_TEMPLATE,
    EVT_WMS_MSG_WRITE_TEMPLATE,
    EVT_WMS_MSG_DELETE_TEMPLATE,
    EVT_WMS_MSG_RECEIVED_MESSAGE,
    EVT_WMS_MSG_SUBMIT_REPORT,
    EVT_WMS_MSG_STATUS_REPORT,
    EVT_WMS_MSG_DELETE_ALL,
    EVT_WMS_MSG_DELETE_TEMPLATE_ALL, //30
    EVT_WMS_MSG_READ_STATUS_REPORT,
    EVT_WMS_MSG_DELETE_STATUS_REPORT,
    EVT_WMS_MSG_DUPLICATE_CB_PAGE,
    
    EVT_WMS_CMD_STATUS,
    EVT_WMS_BC_EVENT,
    EVT_WMS_DLG_RESUME,
    EVT_WMS_CONFIRM,
    EVT_WMS_PROCESS_NEW_MSG,
    
    EVT_WMS_DC_CONNECTED,
    EVT_WMS_DC_ABORTED,//40
    EVT_WMS_DC_ENABLE_AUTO_DISCONNECT,
    EVT_WMS_DC_DISABLE_AUTO_DISCONNECT,
    EVT_WMS_DC_INCOMING,
    EVT_WMS_DC_DISCONNECTED,
    EVT_WMS_DC_CONNECTING,
    
    EVT_WMSAPP_MESSAGE_READ,
    EVT_WMSAPP_MESSAGE_COMPOSE,
    
    EVT_WMSAPP_CARDFILE_CPHS_MWI,/* CPHS Message Waiting Indicator Status File */
    EVT_WMSAPP_CARDFILE_CPHS_MAILBOX_NUM,/* CPHS Mailbox Dailing Number File */
    EVT_WMSAPP_CARDFILE_MWI,/* EF-Message Waiting Indication Status File *///50
    EVT_WMSAPP_CARDFILE_MAILBOX_NUM,/* EF-Mailbox Dailing Number File */
    EVT_WMSAPP_CARDFILE_MAILBOX_ID,/* EF-Mailbox Identifier File */
    
    EVT_UPDATE,
    EVT_NEWMSGIN,
    EVT_NEXTSEND,
    EVT_UPDATAMENU,
    EVT_SENDSMSEND,
    EVT_MSGCOPY,
    EVT_RESERVEDMSGALERT,
    EVT_SENDREGINFOR,//60
    EVT_SENDSTATUS,
    EVT_UPDATEREGSTATUS,
    EVT_CALLVOICEMAILNUM,
    EVT_CTL_TEXT_TEXTFULL,
    EVT_RUIM_PROACTIVE,
    EVT_AUTO_POWERDOWN,
    EVT_BATT_POWERDOWN,
    EVT_ALARMOFF,
    EVT_FOCUS_SWITCH,   
    EVT_UTKREFRESH,//70
    EVT_RTC,
    EVT_UPDATE_ARROW_BUTTON,
    EVT_BLUETOOTH_EVENT,
    EVT_CLOSEAPP,
    EVT_GSENSOR_SHAKE,
    EVT_DRAWMUSICNAME,
    EVT_OPENSHAKE,
    EVT_PLAYNEXT,       //play next video / music file. 
                        //dwParam == (char*)pszFilename ,point to filename
    EVT_POPMSGDIALOG,  // notify pop playfailed msg when play failed 
    EVT_PLANEMODEON,//80
    EVT_APP_DIALOG_TIMEOUT,    
    EVT_APP_INTERRUPT,
    EVT_APP_EXIT,
    EVT_EDITOR_OK,	        // Send to the invoker APP when the editor input is OK.
    EVT_EDITOR_CANCEL,   // Send to the invoker APP when the editor input is cancelled.
    EVT_EDITOR_END,        // Send to the invoker APP when the END key is pressed.
	EVT_MMS_RECEIVED,		// notify CoreApp that MMS is received.
	EVT_SET_OPERATING_MODE,
    EVT_SEND_SEAMLESSSMS,
    EVT_CAMERA_NOTIFY,//90
    EVT_CARD_STATUS,
    EVT_HEADSET_CONNECT,
    EVT_HEADSET_DISCONNECT,
    EVT_ATV_AUTOSCANFINISH,
    EVT_ATV_OPST_MENU,
    EVT_ATV_AUTOSCAN,
    EVT_NO_CLOSEBACKLIGHT,
    EVT_CHANGEPIN,
    EVT_UNBLOCKPIN,
    EVT_DISPLAYADN,   //100
#ifdef FEATURE_OEMOMH
    EVT_OMH_PROMPT,
#endif
#ifdef FEATURE_USES_MMS
    EVT_MMS_MSG_SEND_FINSH,
    EVT_MMS_MSG_GET_FINISH,
    EVT_MMS_PDUDECODE,
#endif
#ifdef FEATURE_QQ_APP
    EVT_QQ_ANNU_UPDATE,
#endif    
    EVT_MOBILE_TRACKER,
    EVT_SALES_TRACKER,
    EVT_WMS_DRAFT_SAVED,
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)||defined(FEATURE_SALESTRACKER)||defined(FEATURE_VERSION_IC241A_MMX)||defined(FEATURE_VERSION_K232_Y100A)
	EVT_SMS_TRACKER,
#endif
    EVT_ESN_EDIT_TRACKER,
    EVT_ADS_ACCOUNT_EDIT,
    EVT_PROXY_PARAMS_EDIT,
#if defined(FEATURE_SHOW_RSSI_INFO)
    EVT_RSSI_INFO,
#endif
    EVT_ESN_TRACKER,
#if defined(FEATURE_ESN_NETWORK_INITIATED)
    EVT_ESN_NETWORK_INITIATED,
#endif
    EVT_CUST_MAX
};
#endif /*CUST_EDITION*/

#define EVT_OEM_END              0x5fff   // OEM-specific events end   @ 0x5fff

// Events registered via ISHELL_RegisterEvent

#define EVT_DYNAMIC_START        0x6000
#define EVT_DYNAMIC_END          0x6fff

#define EVT_USER                 0x7000   // Start of App/User defined Events.  Private to apps...

//Add By zzg 2011_11_10
#define EVT_MUSICPLAYER_DISCONNECT_FROM_BT	0x7101			//Disconnect BtHeadSet for MusicPlayer
#define EVT_CALLAPP_CHECK_BT_STATUS			0x7102			//Check BTAG connect for callapp
#define EVT_MUSICPLAYER_ALARM_SUSPEND_BT	0x7103			//MusicPlayer Suspend
#define EVT_MUSICPLAYER_ALARM_RESUME_BT		0x7104			//MusicPlayer Resume
	
#define EVT_BT_AG_AUDIO_CONNECTED			0x7005			//AG connected
#define EVT_BT_A2DP_AUDIO_CONNECTED			0x7006			//A2DP Connected
#define EVT_CALLAPP_USE_BT_HEADSET			0x7007			//CallApp use Bluetooth Headset
#define EVT_BT_AG_AUDIO_DISCONNECTED		0x7008			//AG Disconnected
#define EVT_BT_A2DP_AUDIO_DISCONNECTED		0x7009			//A2DP DisConnected

#define EVT_CALLAPP_DISCONNECT_FROM_BT		0x710a			//Disconnect BtHeadSet for CallAppr

#define EVT_BT_A2DP_PAUSE					0x710b			
#define EVT_BT_A2DP_RESUME					0x710c
//Add End

#define AEEEvent  uint16

// Event ranges for privilege verification
#define START_SYSTEM_EVENT        0           // First system event id. An app can send/post system events
                                              // to itself or other in-process apps.
#define LAST_SYSTEM_EVENT         0x7         // Last system event id
#define START_PROTECTED_EVENT     0x8         // First protected event id. An app can send/post protected
                                              // events to an app in other process if it has AEEPRIVID_EVENT_PROT
                                              // privilege.
#define LAST_PROTECTED_EVENT      0x4fff      // Last protected event id
#define START_PUBLIC_EVENT        0x5000      // First public event id. An app can send public events to an app
                                              // in other process if it has AEEPRIVID_SENDEVENT_PUB privilege
#define LAST_PUBLIC_EVENT         0xFFFF      // Last public event id

#endif
