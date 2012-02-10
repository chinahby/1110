#ifndef APPMGRSETTINGS_H
#define APPMGRSETTINGS_H 
/*=========================================================================
FILE:  AppMgrSettings.h

SERVICES:  BREW Application Manager Application
  
GENERAL DESCRIPTION:
	 This file contains OEM customizable settings of BREW Application Manager.
	 
PUBLIC CLASSES AND STATIC FUNCTIONS:
		List the class(es) and static functions included in this file
		
INITIALIZATION & SEQUENCING REQUIREMENTS:
			 
				  Copyright © 2000-2002 QUALCOMM Incorporated.
				  All Rights Reserved.
				  QUALCOMM Proprietary/GTDR
===========================================================================*/

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

// Sequence to start credit back
#define        CREDIT_SEQUENCE            "11111111"

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
#define  OEM_LOCK_MOBILESHOP_LOCATION     TRUE		//FALSE        // Lock MobileShop location

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

  Server Capability Flags - App Manager detects server capabilities on its first
  connection with the server. Till than it assumes that this is a 1.0 server that
  doesn't support search (1.1 server capability) and upgrade check (1.2 server
  capability). OEMs can use following flags to customize App Manager's default
  assumption e.g. if an OEM want's that in case App Manager doesn't know server
  capabilities (because user hasn't connected to the server yet), it should support
  search, OEM would set OEM_ADS_SEARCH_APPS to TRUE.

===========================================================================*/
#define  OEM_ADS_SEARCH_APPS              TRUE
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
  -1 to use OEM default.

===========================================================================*/
#define MSHOP_PPP_LINGER                  -1

#endif // APPMGRSETTINGS_H
