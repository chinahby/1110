#ifndef BREWAPPMGRSETTINGS_H
#define BREWAPPMGRSETTINGS_H 
/*=========================================================================
FILE:  BrewAppMgrSettings.h

SERVICES:  BREW Application Manager Application
  
GENERAL DESCRIPTION:
    This file contains OEM customizable settings of BREW Application Manager.
    
PUBLIC CLASSES AND STATIC FUNCTIONS:
      List the class(es) and static functions included in this file
      
INITIALIZATION & SEQUENCING REQUIREMENTS:
          
              Copyright © 2000-2008 QUALCOMM Incorporated.
              All Rights Reserved.
              QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "OEMFeatures.h"
#include "BREWVersion.h"

/*===========================================================================
 
  Following flags determine whether features function according to the 
  'Standard' or 'Custom1' build specifications.  
  Added after the merge of the custom and standard code branches:

===========================================================================*/
// only one build version should be defined.  Most users want BUILD_STANDARD
#define BUILD_STANDARD
//#define BUILD_CUSTOM1
// add other view definitions here

#ifdef BUILD_CUSTOM1
#undef ENABLE_LARGE_ICON_VIEW
#undef MSHOP_STATES
#define ADD_ITEM_REVERSE_ORDER TRUE
#undef LOAD_MSHOP_LOGO
#undef PROMPT_ALWAYS_USE_SK
#undef MSHOP_OPTION_MENU_ICONS
#define CONFIRM_SUBSCRIPTION_PURCHASE
#define SEARCH_FROM_CATALOG 
#define MSHOP_CMD_SEARCH_STRING
#undef MSHOP_CMD_CATALOGID
#define MSHOP_CMD_CATEGORYID
#undef MSHOP_CMD_ACQUIRE
#undef MSHOP_CMD_ITEMINFO
#undef FEATURE_UPGRADE_AUTO_PROMPT
#undef FEATURE_COREAPPUI
// At most, only one of the following two should be defined 
#define CMSHOP_SECURE_REMOVE
#undef CMSHOP_SECURE_ALL
#undef USE_TT_FONTS
#undef FEATURE_TRUSTED_TIME
#define FEATURE_TOUCHSCREEN_SUPPORT
#define FEATURE_HAPTIC_SUPPORT
#undef FEATURE_SILENT_UPGRADE_CHECK
#define APPINFO_IN_APPOPTIONS
#undef TITLE_4_APPINFO_IN_APPOTIONS
#define FEATURE_CANCELONCLOSE
#define FEATURE_HAS_MULTIPLE_DISPLAYS
#define FEATURE_SUPPORT_MULTIPLE_DISPLAYS
#define FEATURE_INTERACT_WITH_SA_APP
#define USE_IDL2_4GB_SUPPORT
#define FEATURE_LESS_THAN_6_KEYS
#undef FEATURE_SCREEN_ROTATE
#define FEATURE_EXIT_AFTER_DOWNLOAD
#define FEATURE_DISABLE_SS_DURING_DOWNLOAD
#endif //BUILD_CUSTOM1

#ifdef BUILD_STANDARD
#define ENABLE_LARGE_ICON_VIEW
#define MSHOP_STATES
#define ADD_ITEM_REVERSE_ORDER FALSE
#define LOAD_MSHOP_LOGO
#undef PROMPT_ALWAYS_USE_SK
#define MSHOP_OPTION_MENU_ICONS
#define CONFIRM_SUBSCRIPTION_PURCHASE
#undef SEARCH_FROM_CATALOG 
#undef MSHOP_CMD_SEARCH_STRING
#define MSHOP_CMD_CATALOGID
#define MSHOP_CMD_CATEGORYID
#define MSHOP_CMD_ACQUIRE
#define MSHOP_CMD_ITEMINFO
#undef FEATURE_UPGRADE_AUTO_PROMPT
#undef FEATURE_COREAPPUI
// At most, only one of the following two should be defined
#define CMSHOP_SECURE_REMOVE
#undef CMSHOP_SECURE_ALL
#undef USE_TT_FONTS
#undef FEATURE_TRUSTED_TIME
//#define FEATURE_TOUCHSCREEN_SUPPORT
//#define FEATURE_HAPTIC_SUPPORT
#define FEATURE_SILENT_UPGRADE_CHECK
#define APPINFO_IN_APPOPTIONS
#undef TITLE_4_APPINFO_IN_APPOTIONS
#define FEATURE_CANCELONCLOSE
#define FEATURE_HAS_MULTIPLE_DISPLAYS
#define FEATURE_SUPPORT_MULTIPLE_DISPLAYS
#define FEATURE_INTERACT_WITH_SA_APP
#define USE_IDL2_4GB_SUPPORT
#define FEATURE_LESS_THAN_6_KEYS
#undef FEATURE_SCREEN_ROTATE
#define FEATURE_EXIT_AFTER_DOWNLOAD
#define FEATURE_DISABLE_SS_DURING_DOWNLOAD
#endif //BUILD_STANDARD

#if MIN_BREW_VERSIONEx(4,0,3)
#define USE_IDL2_DISABLED_SUPPORT
#endif

#if defined(USE_IDL2_4GB_SUPPORT) || defined(USE_IDL2_DISABLED_SUPPORT)
#define USE_IDL2
#endif



//add other view settings here

#if defined(FEATURE_CANCELONCLOSE) || defined(FEATURE_HAS_MULTIPLE_DISPLAYS) || defined(FEATURE_INTERACT_WITH_SA_APP)
#define FEATURE_OEMCFG
#endif

#ifdef FEATURE_TOUCHSCREEN_SUPPORT
#define USE_MENUCTL_EX
#endif //FEATURE_TOUCHSCREEN_SUPPORT



/*===========================================================================

	Definitions controlling TouchScreen look and feel.

   SCROLLBAR_WIDTH: If USE_MENUCTL_EX is defined (which is by default if FEATURE_TOUCHSCREEN_SUPPORT
      is enabled) this value is the number of pixes that the scroll bar for
      menus will take.
   TOUCHSCREEN_BACKGROUND: The background color of the screen, if there is no menu or
      button covering that area
   TOUCHMENU_BACKGROUND: The background color of the list menus
   TOUCHMENU_SK_BACKGROUND: The background color of the softkey/back button
   TOUCHMENU_SELECT_BACKGROUND: The background color for the currenly selected
      item in a list menu
   TOUCHMENU_ITEMFRAME: The color of the item frames for the list menus
   TOUCH_SOFTMENU_WIDTH: The minimum width of the softkey/back button
   TOUCH_SOFTMENU_HEIGHT: The height of the softkey/back button
   TOUCH_PIXELPAD: Extra pixels to add to the size of touch items, help make 
      things more touchable. 
   TOUCHMENU_ITEM_OFFSET: The menuitem offset for the touch menus

===========================================================================*/
#ifdef USE_MENUCTL_EX
#define SCROLLBAR_WIDTH                  4
#endif//USE_MENUCTL_EX
#define TOUCHSCREEN_BACKGROUND           MAKE_RGB(227,227,227)
#define TOUCHMENU_BACKGROUND             MAKE_RGB(255,255,255)
#define TOUCHMENU_SK_BACKGROUND          MAKE_RGB(255,255,255)
#define TOUCHMENU_SELECT_BACKGROUND      MAKE_RGB(192,192,192)
#define TOUCHMENU_ITEMFRAME              MAKE_RGB(215,215,215)
#define TOUCH_SOFTMENU_WIDTH             50
#define TOUCH_SOFTMENU_HEIGHT            26
#define TOUCH_PIXELPAD                   4
#define TOUCHMENU_ITEM_OFFSET            5



/*===========================================================================

	Definitions controlling TrueType font feature

===========================================================================*/
#if defined(USE_TT_FONTS)
#define APPMGR_TT_FONTSIZE 11
#define APPMGR_NORMAL_FONT_CLSID AEECLSID_QCSANSTTF
#define APPMGR_BOLD_FONT_CLSID AEECLSID_QCSANSBOLDTTF
#endif




/*===========================================================================

	Definitions controlling behavior of the upgrade auto-prompt feature

===========================================================================*/

//	Auto-prompt timer interval.  We will prompt the user about an
// available upgrade no more frequently than this.  Value in seconds.
#define UPGRADE_AUTO_PROMPT_INTERVAL 1209600	// 14 days

// Upgrade check timer interval.  We will ping the ADS for available
// upgrades no more frequently than this.  Value in seconds.
#define DB_UPDATE_TIMER_INTERVAL 604800		// 7 days



/*===========================================================================

	If SDK_COMMERCIAL_BUILD isn't defined, define FEATURE_BREW_DOWNLOAD

===========================================================================*/
#if !defined(SDK_COMMERCIAL_BUILD) && !defined(FEATURE_BREW_DOWNLOAD)
#define FEATURE_BREW_DOWNLOAD 1
#endif // SDK_COMMERCIAL_BUILD


/*===========================================================================

  Following defines can be tweaked by OEMs without making any code change
  
===========================================================================*/
// Thresholds for images
#define        APP_IMAGE_X_THRESHOLD      65
#define        APP_IMAGE_Y_THRESHOLD      42

// Thresholds for icons
#define        APP_ICON_X_THRESHOLD       26
#define        APP_ICON_Y_THRESHOLD       26

// Thresholds for thumbs
#define        APP_THUMB_X_THRESHOLD      16
#define        APP_THUMB_Y_THRESHOLD      16

// How much display area to use for error/warnings
#define        DISPLAY_AREA_PERCENT       75

// How many lines of text to display when showing App info within App Options
#define        NUM_APPINFO_LINES          4

// Sequence to start credit back
#if defined(BUILD_STANDARD)
#define        CREDIT_SEQUENCE            "11111111"
#elif defined(BUILD_CUSTOM1)
#define        CREDIT_SEQUENCE            "43848669"
#else
#error
#endif

// Time to show visual confirmation
#define        VISUAL_RB_CONFIRM_TIME     750         // RadioButton Selection Change
#define        VISUAL_MSG_CONFIRM_TIME    1500        // Message Indicating Change

/*===========================================================================

  OEM customization flag to define a device as small screen device. If a device
  is designated as small screen device, large icon view would use icons instead
  of image and will not contain application status and application size preview 
  in settings -> manage apps will not be displayed.
  
===========================================================================*/
#define  OEM_SMALL_SCREEN_DEVICE          FALSE        // Small Screen Device

/*===========================================================================

  OEM customization flag to lock MobileShop location
  
===========================================================================*/
#if defined(BUILD_STANDARD)
#define  OEM_LOCK_MOBILESHOP_LOCATION     FALSE        // Lock MobileShop location
#elif defined(BUILD_CUSTOM1)
#define  OEM_LOCK_MOBILESHOP_LOCATION     TRUE        // Lock MobileShop location
#else
#error
#endif

/*===========================================================================

  OEM customization flag to make Application Manager/Mobile Shop menus use
  default color and style. If this flag is TRUE, Application Manager/MobileShop
  will use default color and style for menu controls. Otherwise, Application
  Manager/MobileShop will use custom colors.
  
===========================================================================*/
#define  OEM_USE_DEFAULT_COLOR_STYLE      TRUE        // Use default color & style

/*===========================================================================

  OEM customization flag to define a device as test device. if a device is designated 
  as test device, settings will provide options to "Set Server"
  
===========================================================================*/
#define  OEM_CONFIGURE_TEST_DEVICE        TRUE        // Test Device

/*===========================================================================

  Warnings Flags - Can be used by OEMs to customize AIRTIME use warnings as
  factory settings. For disabling a warning, change definition to FALSE e.g. for
  disabling CreditBack Airtime warning, define OEM_SHOW_CREDITBACK_WARNING FALSE
  
===========================================================================*/
#define  OEM_SHOW_BROWSE_WARNING          FALSE        // Show Browse Airtime Warning
#define  OEM_SHOW_SEARCH_WARNING          FALSE        // Show Search Airtime Warning
#define  OEM_SHOW_BUY_WARNING             FALSE        // Show Buy Airtime Warning
#define  OEM_SHOW_RESTORE_WARNING         FALSE        // Show Restore Airtime Warning
#define  OEM_SHOW_UPGRADE_WARNING         FALSE        // Show Upgrade Airtime Warning
#define  OEM_SHOW_CREDITBACK_WARNING      FALSE        // Show CreditBack Airtime Warning

/*===========================================================================

  Server Capability Flags - App Manager detects server capabilities on its
  first connection with the server. Till than it assumes that this is a 1.0
  server that doesn't support search (1.1 server capability) and upgrade check
  (1.2 server capability). OEMs can use following flags to customize App
  Manager's default assumption e.g. if an OEM want's that in case App Manager
  doesn't know server capabilities (because user hasn't connected to the server
  yet), it should support search, OEM would set OEM_ADS_SEARCH_APPS to TRUE.

===========================================================================*/
#if defined(BUILD_STANDARD)
#define  OEM_ADS_SEARCH_APPS              TRUE
#elif defined(BUILD_CUSTOM1)
#define  OEM_ADS_SEARCH_APPS              FALSE
#else
#error
#endif

#define  OEM_ADS_UPGRADE_CHECK            FALSE

/*===========================================================================

  Following flags enables OEMs to turn sound/image configuration feature On/Off
  and specify paths to pick sound/image files. By default, sound image files are
  picked from AEE_SHARED_DIR

===========================================================================*/
#define  OEM_SET_SOUND                    FALSE
#define  OEM_SOUND_FILE_PATH              AEE_SHARED_DIR
#define  OEM_SET_IMAGE                    FALSE
#define  OEM_IMAGE_FILE_PATH              AEE_SHARED_DIR

/*===========================================================================

  Following flags enables OEMs to turn screen savers feature On/Off.

===========================================================================*/
#define  OEM_SCREEN_SAVERS                TRUE

/*===========================================================================

  Following flags enables OEMs to underline menu titles.

===========================================================================*/
#define  OEM_UNDERLINE_MENU_TITLE         TRUE

/*===========================================================================

  For category icon IDs indicated by BDS, MobileShop first looks for icons in
  ADS_ICONS_FILE, then in CAT_ICONS_RES_FILE and then if necessary uses default
  icon from APPMGR_RES_FILE. This can be used by OEMs to configure resource file
  to be used to load BDS indicated category icon IDs.

===========================================================================*/
#define CAT_ICONS_RES_FILE                "caticons.bar"

/*===========================================================================

  Following flag enables OEMs to configure linger time for MobileShop. Set to
  1 have the PPP connection avalable to be taken down 1 second after MobileShop
  exits.  If any other apps are using the network, their values and uses
  will keep this linger value from kicking in.

===========================================================================*/
#define MSHOP_PPP_LINGER                  1

#endif // BREWAPPMGRSETTINGS_H
