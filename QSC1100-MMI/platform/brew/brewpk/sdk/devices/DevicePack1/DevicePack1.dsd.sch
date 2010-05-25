#dsd.sch
#
# Syntax:
# ------
# Resource, <ID>, <Category> [,<Name>]
#     uint32,   	<Name>   [,<Min>,       <Max>,           <Default>,   <Sep>]
#     string,   	<Name>                                 [,<Default>,   <Sep>]
#     enum,       <Name>,  <EnumList/EnumTable ID>       [,<Default>,   <Sep>]
#     complex,  	<Name>   [,<MinOccurs>, <MaxOccurs>,     <Default>,   <Sep>]		
#        // List of contained simple types. No containment of complex types.
#     }
# }
#
# EnumList, <ID>
#     <Value1>[, <Label1>]
#     <Value1>[, <Label1>]
#        :
#     <ValueN>[, <LabelN>]
# }
#
#
# Enum Lists
#
EnumList, IDEL_YESNO
   No, No
   Yes, Yes
}
EnumList, IDEL_LABEL_YESNO
   0, No
   1, Yes
}
EnumList, IDEL_FORMFACTOR
   1, Candy Bar
   2, Clamshell
   3, Slider
   99, Other
}
EnumList, IDEL_PROCESSORCORE
   1, ARM7
   2, ARM9,
   99, Other
}
EnumList, IDEL_FILESYSTEM
   1, EFS1
   2, EFS2
   99, Other
}
EnumList, IDEL_FLASH
   1, NAND
   2, NOR
   99, Other
}
EnumList, IDEL_NUM_MMC_SUPPORT
   1, 1
   2, 2
   99, Not Supported
}
EnumList, IDEL_BTIL_SUPPORT
   0, Not Installed
	 1, BTIL Capable
	 2, BTIL Supported
}
EnumList, IDEL_KEY_LIST
   0, None
	 57360,AVK_UNDEFINED
	 57376,AVK_FIRST
   57377,AVK_0
   57378,AVK_1
   57379,AVK_2
   57380,AVK_3
   57381,AVK_4
   57382,AVK_5
   57383,AVK_6
   57384,AVK_7
   57385,AVK_8
   57386,AVK_9
   57387,AVK_STAR
   57388,AVK_POUND
   57389,AVK_POWER
   57390,AVK_END
   57391,AVK_SEND
   57392,AVK_CLR
   57393,AVK_UP
   57394,AVK_DOWN
   57395,AVK_LEFT
   57396,AVK_RIGHT
   57397,AVK_SELECT
   57398,AVK_SOFT1
   57399,AVK_SOFT2
   57400,AVK_SOFT3
   57401,AVK_SOFT4
   57402,AVK_FUNCTION1
   57403,AVK_FUNCTION2
   57404,AVK_FUNCTION3
   57405,AVK_FUNCTION
   57406,AVK_MENU
   57407,AVK_INFO
   57408,AVK_SHIFT
   57409,AVK_MESSAGE
   57410,AVK_MUTE
   57411,AVK_STORE
   57412,AVK_RECALL
   57413,AVK_PUNC1
   57414,AVK_PUNC2
   57415,AVK_VOLUME_UP
   57416,AVK_VOLUME_DOWN
   57417,AVK_WEB_ACCESS
   57418,AVK_VOICE_MEMO
   57419,AVK_SPEAKER
   57420,AVK_TAP
   57421,AVK_PTT
   57422,AVK_LSHIFT
   57423,AVK_RSHIFT
   57424,AVK_LCTRL
   57425,AVK_RCTRL
   57426,AVK_LALT
   57427,AVK_RALT
   57428,AVK_CAPLK
   57429,AVK_SCRLK
   57430,AVK_NUMLK
   57431,AVK_LNGHANGUL
   57432,AVK_LNGJUNJA
   57433,AVK_LNGFIN
   57434,AVK_LNGHANJA
   57435,AVK_PRSCRN
   57436,AVK_BREAK
   57437,AVK_TXINSERT
   57438,AVK_TXDELETE
   57439,AVK_TXHOME
   57440,AVK_TXEND
   57441,AVK_TXPGUP
   57442,AVK_TXPGDOWN
   57443,AVK_GP_1   
   57444,AVK_GP_2   
   57445,AVK_GP_3   
   57446,AVK_GP_4   
   57447,AVK_GP_5   
   57448,AVK_GP_6   
   57449,AVK_GP_SL  
   57450,AVK_GP_SR  
   57451,AVK_CAMERA       
   57452,AVK_VIDEO_PHONE
   57453,AVK_APP_SWITCH
   57454,AVK_PLAY
   57455,AVK_PAUSE
   57456,AVK_SKIP_FWD
   57457,AVK_SKIP_RWD
   57458,AVK_FFWD
   57459,AVK_RWD
   57460,AVK_STOP
   57461,AVK_HEADSET_FUNC
   57462,AVK_FUNCTION4
   57463,AVK_FUNCTION5
   57464,AVK_FUNCTION6
   57465,AVK_FUNCTION7
   57466,AVK_FUNCTION8
   57467,AVK_FUNCTION9
   57468,AVK_FUNCTION10
   57469,AVK_FUNCTION11
   57470,AVK_FUNCTION12
   57471,AVK_SYMBOL
   57472,AVK_ENTER
   57473,AVK_SPACE
      97,AVK_A
      98,AVK_B
      99,AVK_C
     100,AVK_D
     101,AVK_E
     102,AVK_F
     103,AVK_G
     104,AVK_H
     105,AVK_I
     106,AVK_J
     107,AVK_K
     108,AVK_L
     109,AVK_M
     110,AVK_N
     111,AVK_O
     112,AVK_P
     113,AVK_Q
     114,AVK_R
     115,AVK_S
     116,AVK_T
     117,AVK_U
     118,AVK_V
     119,AVK_W
     120,AVK_X
     121,AVK_Y
     122,AVK_Z
   }
EnumList, IDEL_KEY_EVENT
0,   None
256, EVT_KEY                  
257, EVT_KEY_PRESS             
258, EVT_KEY_RELEASE
259, EVT_KEY_HELD
}
#<!--_______________________________________
#                Class IDs
#      ID Range: 8201+
#      IDS_CLSID_LAST: 8999  
#_______________________________________-->
EnumList, IDEL_CLSID
None, None
16798977, MIDI
16798978, MP3
16798979, QCP
16798980, PMD
16798982, MIDIOutQCP
16798983, MPEG4
16798984, MMF
16798985, PHRASE
16798986, IMA-ADPCM
16798987, AAC
16798988, IMelody
16798990, AMR
16798993, PCM
16798994, XMF
16798995, DLS
16798996, SVG
16793601, BMP
16793603, GIF
16793604, PNG
16793605, JPEG
16793606, BCI
16785427, Camera
16785418, Vocoder
16959124, Camera2
16959125, Camera3
16959126, Camera4
16915499, Joystick
17007713, SVGDOM
17167273, EVRCB
17175286, MediaVideo
17004525, AMRWB
0, Last
}

EnumList, IDEL_API_SUPPORT
1, Supported
2, Not Supported
3, Optional
}

#<!--=================================================================
#        Device Pack Info
#=================================================================-->
Resource, IDS_DD_SCHEMA_VER, Device Pack Info, ,RO
   string, Device Pack Schema Version
   HELP
      Enter the Device Pack Schema version in 3 digits. The first two digits specify the
      major BREW release and the last digit the schema version (for example, 3.1.0).
   }
}
Resource, IDS_DD_DEV_DEVPACK_VER, Device Pack Info
   string, Device Pack Version
   HELP
      Enter the Device Pack version.  This is a version number assigned
      by the OEM to this Device Pack.  It is expected that this version
      number will change each time an OEM submits a changed version of
      this Device Pack.

      Filename convention - The following convention will be used for the file version scheme for the Device Pack files:

      ___.dso/dst/dpk/sch
      where .dso/dst/dpk/sch refers to Device Pack related filenames.

      The following rules apply to the naming convention of the filename:

      1.  Excluded characters - periods (".") and underscores ("_") are not allowed within the OEM Name, Device Model, and Carrier Name fields
      2.  x.y = BREW Version
      3.  z = DDF or DPF version number
      4.  a = OEM version number

   }
}

#<!--=================================================================
#        Device Model Info
#=================================================================-->
Resource, IDS_DD_DEV_MFC, Device Model Info
   string, Manufacturer, , |
   HELP
	<Definition>
	    Manufacturer's Name
	<Instructions>
      Enter the device manufacturer's name (your company name).
   }
}
Resource, IDS_DD_DEV_MODEL, Device Model Info
   string, Model
   HELP
	<Definition>
	    Model name/number
	<Instructions>
      Enter the model number of the device.  This should be the commercial
      name/number, not an internal project name or code name.
   }
}
Resource, IDS_DD_DEV_ID, Device Model Info
   uint32, Platform Id
   HELP
	<Definition>
	<Instructions>
      Contact QUALCOMM Internet Services (QIS) for this value.
   }
}
Resource, IDS_DD_DEV_BREW_VER, Device Model Info
   string, BREW Version
   HELP
	<Definition>
	    Version of BREW on the device
	<Instructions>
      Base version of BREW on the device (for example, 3.1.5).  This version should not include any 
      patches or feature packages.  Patches and feature packages are specified in the Patch Name field.
   }
}
Resource, IDS_DD_DEV_CHIPSET, Device Model Info
   string, Chipset
   HELP
	<Definition>
                The AEEDEVICEITEM - AEE_DEVICEITEM_CHIP_ID
            <Instructions>	
            	Enter the chipset of the device.
		If AEE_DEVICEITEM_CHIP_ID is not supported, enter "not supported".     
   }
}
Resource, IDS_DD_DEV_BUILD_VER, Device Model Info
   string, Build Version
   HELP
	<Definition>
	    OEM Software version
	<Instructions>
      Enter the OEM device software version.  Also, document how this information
      can be accessed through the device UI in the Comment field.
   }
}
Resource, IDS_DD_DEV_FORMFACTOR, Device Model Info
   enum, Form Factor, IDEL_FORMFACTOR
   HELP
	<Definition>
	    Device Form Factor
	<Instructions>
      Select the device form factor (for example, candy bar, clamshell, slider, etc.).  If none of the
      listed form factors apply, select "Other" and describe the form factor in the
      Comment field.
   }
}
Resource, IDS_DD_DEV_PROCESSOR_CORE, Device Model Info
   enum, Processor Core, IDEL_PROCESSORCORE
   HELP
	<Definition>
	    Processor core used on the device
	<Instructions>
      Select device Processor Core (for example, ARM7, ARM9, etc.). If none of the listed processor
      cores apply, select "Other" and describe the processor core in the Comment field.
   }
}
Resource, IDS_DD_DEV_FILE_SYSTEM, Device Model Info
   enum, File System, IDEL_FILESYSTEM
   HELP
	<Definition>
	    File system type used on the device
	<Instructions>
      Select device File System (for example, EFS1, EFS2, etc.). If none of the listed file systems
      apply, select "Other" and describe the file system in the Comment field.
   }
}
Resource, IDS_DD_DEV_FLASH, Device Model Info
   enum, Flash Type, IDEL_FLASH
   HELP
	<Definition>
	    Flash type used on the device
	<Instructions>
      Select device Flash type (for example, NAND, NOR, etc.). If none of the listed flash types
      apply, select "Other" and describe the files system in the Comment field.
   }
} 
Resource, IDS_DD_SERIAL_BLOCKING_SUPPORT, Device Model Info
   enum, Serial Port Blocking, IDEL_LABEL_YESNO
	HELP
	<Definition>
	<Instructions>
	  Does the device support blocking of the serial port per the instructions in the BREW 
          Porting Guide?
   }
}
Resource, IDS_DD_PORT_NAMES, Device Model Info,Developer|OEM
   string, Serial Port Names, , |
	HELP
	<Definition>
	  Serial ports supported
	<Instructions>
	  If serial ports are supported through BREW, enter a comma separated list of port names (for example, 
          PORT1,PORT2,USB1,USB2); otherwise, leave blank.
   }
}
 
Resource, IDS_DD_RUIM_SUPPORTED, Device Model Info
   enum, RUIM Support, IDEL_LABEL_YESNO
	HELP
	<Definition>
	<Instructions>
	    Does the device support RUIM?
   }
}
Resource, IDS_DD_LANG_OFFSET, Language Support
   string, Languages Supported, , |
	HELP
	<Definition>
	   Language Codes of supported languages
	<Instructions>
	  Enter the language codes from AEELngCode.h for all languages supported
          by the device in a comma separated list. 
			
	  Enter the primary language first.
	}
}
Resource, IDS_DD_BTIL_VER, Device Model Info, Developer|OEM
   complex, BTIL Support
      enum, BTIL Support, IDEL_BTIL_SUPPORT
      string, Version
   }
   HELP 
	<Definition>
	 Level of BTIL support and Version of BTIL installed on the device, if any
	<Instructions>
      If the device does not support BTIL enter "Not Installed" for BTIL Support and "None" for Version.
      If the device does support BTIL but it is not pre-installed enter "Capable" for BTIL Support and "None" for Version.
      If the device does support BTIL and it is pre-installed enter "BTIL Supported" for BTIL Support and the version of BTIL installed in Version.
   }
}
Resource, IDS_DD_IMODEL_SUPPORT, Device Model Info,Developer|OEM
   enum, IModel Support, IDEL_LABEL_YESNO
   HELP
	<Definition>
	     IModel notification supported for IConfig
	<Instructions>
        Does the device support the IModel notification interface with IConfig?
       }
}
Resource, IDS_DD_BREW_DEBUGGER, Device Model Info, Developer|OEM
   enum, BREW Debugger Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     BREW Debugger supported by the device
	 <Instructions>
	     Enter Yes if FEATURE_BREW_DEBUGGER turned on
			 Enter No if FEATURE_BREW_DEBUGGER turned off
	 }
}
Resource, IDS_DD_DEV_BREW_UPDATE_VER, Device Model Info, Carrier|OEM
   string, Patch Name, , |
   HELP
	<Definition>
	   BREW patches used on the device
	<Instructions>
      Enter a comma separated list of BREW patches and/or feature patches applied. The naming 
      convention for patches is "U<num>" (for example, U02).  The naming convention for
      feature package is "FP<num>" (for example, FP01).  

      For example, if Patches 1 and 2 and Feature Package 3 have been applied, the list
      would read "U01,U02,FP03".

      If no patch is applied to the device, enter "None".
   }
}
Resource, IDS_DD_DEV_APPMGR_VER, Device Model Info
   string, BREW App Manager version
   HELP
      Enter the version of BREW Application Manager on the device including all Patches
      and Feature Packages (for example, 3.0.2U01FP02).

   }
}
Resource, IDS_DD_DEV_MSHOP_VER, Device Model Info
   string, uiOne mShop Version
   HELP
            <Definition>
                Version of uiOne mShop pre-installed into device
            <Instructions>
	      Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. In case this is a pre-installed package and the OEM does not possess this information, it is available in mshop.mif under Content tab, in box Module Version. Example 1.2.1SP01.
   }
}
Resource, IDS_DD_DEV_CONTENTMANAGER_VER, Device Model Info
   string, uiOne Content Manager Version
   HELP
           <Definition>
              Version of uiOne Content Manager pre-installed into device
           <Instructions>
	      Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. In case this is a pre-installed package and the OEM does not possess this information, it is available in contentmanager.mif under Content tab, in box Module Version. Example 1.0.1SP01.
   }
}
Resource, IDS_DD_DEV_HSK_VER, Device Model Info
   string, uiOne Homescreeen Version
   HELP
          <Definition>
             Version of uiOne Homescreeen pre-installed into device
          <Instructions>
             Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. In case this is a pre-installed package and the OEM does not possess this information, it is available in uionehomescreen.mif under Content tab, in box Module Version. Example 1.2.0
   }
}
Resource, IDS_DD_DEV_UIONE_SDK_VER, Device Model Info
   string, uiOne SDK Version
   HELP
          <Definition>
             Version of uiOne SDK pre-installed into device
          <Instructions>
             Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. Example 1.2.1
   }
}
Resource, IDS_DD_DEV_UPDATE_MANAGER_VER, Device Model Info
   string, uiOne Update Manager Version
   HELP
          <Definition>
             Version of uiOne Update Manager pre-installed into device
          <Instructions>
             Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. In case this is a pre-installed package and the OEM does not possess this information, it is available in updatemanager.mif under Content tab, in box Module Version. Example 1.1.0
   }
}
Resource, IDS_DD_DEV_GLANCE_MODULE_VER, Device Model Info
   string, uiOne Glance Modules Version
   HELP
          <Definition>
             Version of uiOne Glance Modules pre-installed into device
          <Instructions>
             Release version of the product labeled when OEM downloaded it from Qualcomm OEM Extranet. This version also shows on the Windows Start/Program Menu, in front of the product name. In case this is a pre-installed package and the OEM does not possess this information, it is available in glancepkgactors.mif under Content tab, in box Module Version. Example 1.1.0
   }
}

Resource, IDS_DD_DEV_UIONE_APPS_VERSIONS, Device Model Info
   complex, uiOne App Versions, , , , |
      string, Name
      string, Version
   }
   HELP
            <Definition>
                Names and versions of required UiOne applications and modules supported on the device 
            <Instructions>
	      Enter the names of the uiOne modules and applications in the Name field. i.e. "launcherapp"
              Enter the associated versions of the modules and applications in the Version field. i.e. "1.1.1"  
   }
}
Resource, IDS_DD_BATTERY_SUPPORT, Device Model Info
   enum, IBattery Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IBattery supported on the device.
	 <Instructions>
	     Enter Yes if IBattery is implemented on the device
	     Enter No if IBattery is not implemented on the device
	 }
}
Resource, IDS_DD_DOWNLOAD_SUPPORT, Device Model Info 
   enum, IDownload Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IDownload supported on the device.
	 <Instructions>
	     Enter Yes if IDownload is implemented on the device
	     Enter No if IDownload is not implemented on the device
	 }
}
Resource, IDS_DD_WIFI_SUPPORT, Device Model Info
   enum, IWIFI Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IWIFI supported on the device.
	 <Instructions>
	     Enter Yes if IWIFI is implemented on the device
	     Enter No if IWIFI is not implemented on the device
	 }
}
Resource, IDS_DD_DISPROT_SUPPORT, Device Model Info
   enum, IBITMAPROT Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IBITMAPROT supported on the device.
	 <Instructions>
	     Enter Yes if IBITMAPROT is implemented on the device
	     Enter No if IBITMAPROT is not implemented on the device
	 }
}
Resource, IDS_DD_SMS_UDH_SUPPORTED, Device Model Info
   enum, SMS UDH Support, IDEL_LABEL_YESNO
	HELP
	<Definition>
            SMS User Data Headers (UDH) support.
	<Instructions>
	    If the device supports SMS User Data Headers (UDH) select 'Yes'.
            Otherwise, select 'No'.
   }
}
Resource, IDS_DD_SMS_BCAST_SUPPORTED, Device Model Info
   enum, SMS Broadcast Support, IDEL_LABEL_YESNO
	HELP
	<Definition>
            Broadcast SMS support
	<Instructions>
	    If the device supports broadcast SMS select 'Yes'.
            Otherwise, select 'No'.
   }
}
Resource, IDS_DD_SMS_WAP_SUPPORTED, Device Model Info
   enum, SMS WAP Support, IDEL_LABEL_YESNO
         HELP
         <Definition>
             SMS WAP Push support.
        <Instructions>
             If the Device supports incoming WAP push messages, select "Yes".
             Otherwise, select "No".
   }
}
Resource, IDS_DD_NET_GSM_SUPPORT, Device Model Info
   enum, GSM Support, IDEL_LABEL_YESNO
         HELP
         <Definition>
         <Instructions>
             If the device is a GSM device, select "Yes".
             Otherwise, select "No".
   }
}
Resource, IDS_DD_NET_WLAN_SUPPORT, Device Model Info
   enum, WLAN Support, IDEL_LABEL_YESNO
         HELP
         <Definition>
				     WLAN support.
         <Instructions>
             If the device has WLAN support, select "Yes".
             Otherwise, select "No".
   }
}
Resource, IDS_DD_AFLAG_SUPPORT, Device Model Info
   enum, AFLAG_PHONE Support, IDEL_LABEL_YESNO
	    HELP
         <Definition>
             AFLAG_PHONE permission support
         <Instructions>
             If the device supports the AFLAG_PHONE permission from BREW, select "Yes".
	     Otherwise, select "No".
   }
}
Resource, IDS_DD_WIDGETS_SUPPORTED,  Device Model Info 
   enum, BREW UI Widgets Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW UI Widgets supported for device
	<Instructions>
      Select "Yes" if BREW UI Widgets is supported; otherwise, "No".
   }
}
Resource, IDS_DD_FORMS_SUPPORTED,  Device Model Info 
   enum, BREW UI Forms Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW UI Forms supported for device
	<Instructions>
      Select "Yes" if BREW UI Forms is supported; otherwise, "No".
   }
}
Resource, IDS_DD_HTMLWIDGET_SUPPORTED,  Device Model Info 
   enum, BREW UI HTML Widget Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW UI HTML Widget supported for device
	<Instructions>
      Select "Yes" if BREW UI HTML Widget is supported; otherwise, "No".
   }
}
Resource, IDS_DD_FRAMEWIDGET_SUPPORTED,  Device Model Info 
   enum, BREW UI Frame Widget Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW UI Frame Widget supported for device
	<Instructions>
      Select "Yes" if BREW UI Frame Widget is supported; otherwise, "No".
   }
}
Resource, IDS_DD_BBFE_SUPPORTED,  Device Model Info 
   enum, BREW Bitmap Font Extension Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW Bitmap Font Extension (BBFE) supported for device
	<Instructions>
      Select "Yes" if BREW Bitmap Font Extension is supported; otherwise, "No".
   }
}
Resource, IDS_DD_BTFE_SUPPORTED,  Device Model Info 
   enum, BREW True Type Font Extension Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW True Type Font Extension (BTFE) supported for device
	<Instructions>
      Select "Yes" if BREW True Type Font Extension is supported; otherwise, "No".
   }
}

Resource, IDS_DD_SQL_SUPPORTED,  Device Model Info 
   enum, BREW SQL Extension Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW SQL Extension supported for device
	<Instructions>
      Select "Yes" if BREW SQL Extension is supported; otherwise, "No".
   }
}

Resource, IDS_DD_LOCALES_SUPPORTED,  Device Model Info 
   enum, BREW Locales Extension Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    BREW Locales Extension supported for device
	<Instructions>
      Select "Yes" if BREW Locales Extension is supported; otherwise, "No".
   }
}
Resource, IDS_DD_USBDRIVER, Device Model Info
   enum, USBDriver Supported, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    USBDriver supported for BREW
	<Instructions>
      If USBDriver is available to BREW, select "YES".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_USBDRIVER_FULLSPEED_ONLY, Device Model Info
   enum, USBDriver Supported (FullSpeed Only), IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    USBDriver on this device does not support High-Speed
	<Instructions>
      If High-Speed USB is available , select "NO".
      Otherwise, select "YES".
   }
}
Resource, IDS_DD_POINTER_SUPPORT, Pointer
   enum, Pointer Input Support, IDEL_LABEL_YESNO
   enum, Display ID's Supported, IDEL_LABEL_YESNO
   complex, Pointer ID's, , , , |
      uint32, Pointer ID's Supported
   }
   HELP
      If the device supports Pointer Input, select "Yes" for Pointer Input Support. Then enter whether Display ID's are supported, and enter the all the pointer ID's supported.
      If the device does not support Pointer Input, select "No" for Pointer Input Support, Display ID's Supports, and "0" for Pointer ID's Supported.
   }
}

#<!--=================================================================
#        Sensor Support
#=================================================================-->
Resource, IDS_DD_SENSOR_SUPPORT, Sensor
   enum, Sensor Support, IDEL_LABEL_YESNO
   enum, Accelerometer Support, IDEL_LABEL_YESNO
   enum, Compass Support, IDEL_LABEL_YESNO
   enum, Pedometer Support, IDEL_LABEL_YESNO
   enum, Gyroscope Support, IDEL_LABEL_YESNO 
   
   HELP
   <Definition> 
      Sensor support
	<Instructions>
      If AEECLSID_Port1Sensor interface is supported on device, select "Yes".
      Otherwise, select "No".
      If Accelerometer is supported on device, select "Yes".
      Otherwise, select "No".
      If Compass is supported on device, select "Yes".
      Otherwise, select "No".
      If Pedometer is supported on device, select "Yes".
      Otherwise, select "No".
      If Gyroscope is supported on device, select "Yes".
      Otherwise, select "No".
   }
}

#<!--=================================================================
#        IBitmapFX support
#=================================================================-->

Resource, IDS_DD_BITMAPFX_SUPPORT, BitmapFX
   enum, IBitmapFX Support, IDEL_LABEL_YESNO
   
   HELP 
   <Definition>
      IBitmapFX interface support. 
      The IBitmapFX interface is used to perform the image processing of bitmaps.
	<Instructions>
      If IBitmapFX is supported on device, select "Yes".
      Otherwise, select "No".
   }
}

#<!--=================================================================
#        Calendar storing and retrieving services support
#=================================================================-->
Resource, IDS_DD_VCALSTORE_SUPPORT, VCalStore
   enum, IvCalStore Support, IDEL_LABEL_YESNO
   
   HELP 
   <Definition>
      IvCalStore support. 
	<Instructions>
      If IvCalStore is supported on device, select "Yes".
      Otherwise, select "No".
   }
}

#<!--=================================================================
#       Path Info: Apps, Ringer, Shared, AddrBook, MIF, Logo
#=================================================================-->
Resource, IDS_DD_PATH_APPS, Path Info,Developer|OEM
   string, Application Directory
   HELP
     The path to applications (for example, "brew/mod").  Enter "NULL" if none.
   }
}
Resource, IDS_DD_PATH_RINGERS, Path Info,Developer|OEM
   string, Ringer Directory
   HELP
      The path to ringers (for example, "user/sound/ringers").  Enter "NULL" if none.  
   }
}
Resource, IDS_DD_PATH_SHARED, Path Info,Developer|OEM
   string, Shared Directory
   HELP
      The path to shared media directory (for example, "brew/shared").  Enter "NULL" if none.  
   }
}
Resource, IDS_DD_PATH_MIF, Path Info,Developer|OEM
   string, MIF Directory
   HELP
      The path to MIFs (for example, "brew/mif").  Enter "NULL" if none.  
   }
}
#<!--=================================================================
#        3D Graphics Specification
#===================================================================-->
EnumList, IDEL_OPENGL_VERSION
None, None
1.0 Common-Lite, 1.0 Common-Lite
1.0 Common, 1.0 Common
1.1 Common-Lite, 1.1 Common-Lite 
1.1 Common, 1.1 Common
1.2 Common-Lite, 1.2 Common-Lite
1.2 Common, 1.2 Common 
2.0 Common, 2.0 Common
}
EnumList, IDEL_EGL_VERSION
None, None
1.0, 1.0
1.1, 1.1
1.2, 1.2
}
Resource, IDS_DD_OPENGL_ES, 3D Graphics 
   enum, OpenGL ES Version enabled, IDEL_OPENGL_VERSION
	HELP
	<Definition>
	  Version of OpenGL ES supported by the device.
	<Instructions>
	  Version of OpenGL ES supported by the device.  Enter "None" if not supported.
   }
}
Resource, IDS_DD_EGL_ES, 3D Graphics
   enum, EGL Version enabled, IDEL_EGL_VERSION
	HELP
	<Definition>
	  Version of EGL supported by the device.
	<Instructions>
	  Version of EGL supported by the device.  Enter "None" if not supported.
   }
}
Resource, IDS_DD_OPENGL_GRAPHICS_CORE_NAME, 3D Graphics
   string, Graphics Core
	HELP
        <Definition>
	   Version of the Graphics Core on the device.
        <Instructions>
           Version of the Graphics Core on the device. Enter "None" if not supported.
   }
}
Resource, IDS_DD_OPENGL_GRAPHICS_SOFTWARE_RELEASE, 3D Graphics
   string, Graphics Software Version
	HELP
        <Definition>
	   Version of the Graphics software on the device.
        <Instructions>
           Version of the Graphics software on the device. Enter "None" if not supported.
   }
}
#<!--=================================================================
#        Network/Socket Specification
#=================================================================-->
Resource, IDS_DD_NET_RF_SUPP, Network/Socket Specification
   string, Radio Frequency, , |
   HELP 
	<Definition>
	  Frequency bands supported
	<Instructions>
    Enter a comma separated list of frequency bands supported (for 
    example, 800 MHz,1800 MHz,1900 MHz).
   }
}
Resource, IDS_DD_NET_STD_SUPP, Network/Socket Specification
   string, Wireless Standard, , |
   HELP 
	<Definition>
	  Wireless standards supported
	<Instructions>
    Enter a comma separated list of standards supported (for
    example, IS-95A,IS-95B,1x).
   }
}
Resource, IDS_DD_DATA_MAX_TCP, Network/Socket Specification
   uint32, Max TCP Sockets
   HELP 
	<Definition>
	  Maximum TCP sockets on the device
	<Instructions>
    Enter the number of simultaneous TCP sockets supported on the device.
   }
}
Resource, IDS_DD_DATA_MAX_UDP, Network/Socket Specification
   uint32, Max UDP Sockets
   HELP 
	<Definition>
	  Maximum UDP sockets on the device
	<Instructions>
    Enter the number of simultaneous UDP sockets supported on the device.
   }
}
Resource, IDS_DD_DATA_LINGER, Network/Socket Specification
   uint32, Linger timer duration (msecs)
   HELP 
	<Definition>
	  Length of the network linger timer on the device
	<Instructions>
    Enter the duration of the linger timer on the device (in ms).
   }
}
Resource, IDS_DD_DATA_DORMANCY, Network/Socket Specification
   enum, Dormancy Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	  Device-initiated dormancy support
	<Instructions>
    Does the device support device-initiated dormancy?
   }
}
Resource, IDS_DD_AIRPLANE_MODE, Network/Socket Specification
   enum, Airplane Mode Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	  Airplane/Game mode support
	<Instructions>
    Does the device support airplane mode? Airplane mode means the device does not transmit
    or receive RF.
   }
}
Resource, IDS_DD_HDR_MODE, Network/Socket Specification
   enum, 1xEVDO Support, IDEL_LABEL_YESNO
   HELP 
  <Definition>
    EV-DO support on the Device
	<Instructions>
    Does the device support 1xEV-DO (HDR)?
   }
}
Resource, IDS_DD_PRIMARY_QOS_SUPPORT, Network/Socket Specification,Developer|OEM
   enum, Primary QoS Support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
    Primary QoS support on the Device
	<Instructions>
    Does the device support Primary QoS?
   }
}
Resource, IDS_DD_QOS_SUPPORT, Network/Socket Specification,Developer|OEM
   enum, QoS Support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
      QoS support on the Device
	<Instructions>
      Does the device support Quality of Service?
   }
}
Resource, IDS_DD_QOS_BUNDLE_SUPPORT, Network/Socket Specification,Developer|OEM
   enum, QoS Bundle Support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
      QoS Bundle support on the Device
	<Instructions>
      Does the device support QoS Bundle?
   }
}
Resource, IDS_DD_BCMCS, Network/Socket Specification,Developer|OEM
   enum, BCMCS Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    EV-DO Broadcast Multicast Service support on the Device
	<Instructions>
      Does the device support EV-DO Broadcast Multicast Service?
   }
}
Resource, IDS_DD_MFLO, Network/Socket Specification
   enum, MediaFLO Support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
	    MediaFLO service support on the device.
	 <Instructions>
      Does the handset support MediaFLO service?
   }
}
Resource, IDS_DD_IPV6_SUPPORTED, Device Model Info
   enum, IPV6 Support, IDEL_LABEL_YESNO
   HELP
      Does the device support IPV6?
   }
}
Resource, IDS_DD_DVBH, Network/Socket Specification
   enum, DVB-H Support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
	    DVB-H service support on the device.
	 <Instructions>
      Does the handset support DVB-H service?
   }
}
Resource, IDS_DD_SIP_SRVR_INFO, Network/Socket Specification
   enum, SIP server info support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
	   SIP server IP addresses and Domain name information supported on the device.
	 <Instructions>
      Does the handset support SIP server information?
   }
}
Resource, IDS_DD_OUT_OF_COVERAGE, Network/Socket Specification,Developer|OEM
   enum, Close data networks when out of coverage, IDEL_LABEL_YESNO  
   HELP
   <Definition>
	    Data network teardown during loss of coverage.
   <Instructions>
      Does the device close data networks when out of coverage?
   }
}

#<!--=================================================================
#        Hardware Specification
#=================================================================-->
#<!--_______________________________________
#                Memory
#_______________________________________-->
Resource, IDS_DD_HW_RAM_SIZE, Memory
   uint32, Heap Size (bytes)
   HELP 
	<Definition>
	    Heap available to BREW in bytes
	<Instructions>
      Enter heap available to BREW in bytes.
   }
}
Resource, IDS_DD_HW_STACK_SIZE, Memory
   uint32, Stack Size (bytes)
   HELP 
	<Definition>
	    Stack available to BREW in bytes
	<Instructions>
      Enter stack available to BREW in bytes.  Generally, this is the stack size
      of the task that BREW runs in.
   }
}
Resource, IDS_DD_HW_EFS_SIZE, Memory
   uint32, EFS Size (bytes)
   HELP 
	<Definition>
	    Size of the embedded file system that can be used for BREW
      applications in bytes
	<Instructions>
      Enter the size of the embedded file system that can be used for BREW
      applications in bytes.
   }
}
Resource, IDS_DD_HW_EFS_SIZE_EX, Memory
   uint64, EFS Size (can be > 4GB) (bytes)
   HELP 
	<Definition>
	    Size of the embedded file system (even greater than 4 GB)that can be used for BREW
      applications in bytes
	<Instructions>
      Enter the size (even greater than 4 GB) of the embedded file system that can be used for BREW
      applications in bytes.
   }
}
Resource, IDS_DD_HW_EFS_MIN_PATH, Memory
   uint32, File Name Length
   HELP
      A value equal or less than the following:
      [Maximum path length supported in OEM's FS] - [path length of location where fs:/ is mapped]
      - 3 (reserved by BREW)
   }
}
Resource, IDS_DD_HW_EFS_MIN_FILES, Memory
   uint32, Maximum Number of BREW Files
   HELP 
	<Definition>
	    Maximum number of files that can be created in the BREW
      directory tree. "0" means no limit.
	<Instructions>
      Enter the maximum number of files that can be created in the BREW
      directory tree.
      
      Generally, this should only be limited by EFS limitations.  If the number
      of files is limited only by the EFS, enter "0" and "No Limit" in
      the OEM Comment field.
   }
}
Resource, IDS_DD_HW_EFS_MIN_FILESIZE, Memory
   uint32, File Size (bytes)
   HELP 
	<Definition>
	    File size limit on device. "O" means no limit.
	<Instructions>
      If there is any limitation on the size of an individual BREW file,
      specify that size in bytes.  Generally, this should be as large as
      the size of the EFS.
      
      If there is no limitation, specify "0" and enter "No Limit" in the OEM
      Comment field.
   }
}
Resource, IDS_DD_EFS_CASE_SENSITIVE, Memory
   enum, EFS Case Sensitivity Support, IDEL_LABEL_YESNO
   
   HELP
      If the EFS on the device is Case-Sensitive, select "Yes".
      Otherwise, select "No". 
   }
}
Resource, IDS_DD_HW_MMC_SUPPORT, Memory
   enum, Removable Media Card Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Removable media card supported on the device for BREW.
	<Instructions>
      Does the device have a removable media card available to BREW?  Typically,
      a removable media card would be available through the file system as
      "fs:/card0/..." (BREW 3.1 and later only).
   }
}
Resource, IDS_DD_HW_NUM_MMC_SUPPORT, Memory
   enum, Number Media Card Slot, IDEL_NUM_MMC_SUPPORT
   HELP 
	<Definition>
	    Number of removable media card slots supported on the device for BREW.
	<Instructions>
      How many MMC/RMC (multimedia card/removable media card) slots does the device have available to
      BREW? 

      Enter "Not Supported" if the device does not have any media card slots
      available to BREW; otherwise, select the number available.
   }
}
Resource, IDS_DD_MMC_MODELNUM_SERIALNUM_SUPPORTED, Memory
   enum, Media Card Model and Serial Number Support, IDEL_YESNO
   HELP 
	<Definition>
	    Remove media card model and serial query support available to BREW on the Device.
	<Instructions>
      Does the device provide the MMC/RMC (multimedia card/removable media card) model and serial number to BREW?
      If yes, select "Yes"; otherwise, select "No".   
   }
}
Resource, IDS_DD_HW_MMC_MIN_FILE_PATH, Memory
   uint32, RMC File Name Length
   
   HELP
      The maximum number of characters that can be included in a BREW filename or path from the "fs:/card0/"
      directory.  

      For example, the number of characters are counted as follows for the filename or path 
      ""fs:/card0/dir1/filename.txt": 

      Counting each character after "fs:/card0/", the number of characters equals 17 for "dir1/filename.txt". 
   }
}
Resource, IDS_DD_HW_MMC_MIN_SIZE, Memory
   uint32, Min Limit on file size 
   
   HELP
        <Definition>
	    File size limit on MMC cards. "O" means no limit.
	<Instructions>
      If there is any limitation on the size of an individual BREW file on an MMC card, specify that size in bytes.
      
      If there is no limitation, specify "0" and enter "No Limit" in the OEM
      Comment field.
   }
}
EnumList, IDEL_FILESYSTEM_TYPE
0, FAT16
1, FAT32
2, Not Supported
}
Resource, IDS_DD_MMC_FILE_SYSTEM_TYPE, Memory
   enum, MMC Case Sensitivity Support, IDEL_LABEL_YESNO
   
   HELP
      If the MMC card is Case-Sensitive, select "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_USB_MASS_STORAGE_FILE_SYSTEM_TYPE, Memory
   enum, USB Case Sensitivity Support, IDEL_LABEL_YESNO
   
   HELP
      If the USB storage device is Case-Sensitive, select "Yes".
      Otherwise, select "No". 
   }
}
Resource, IDS_DD_HW_MAX_EFS_FILESIZE, Memory
   uint32, Maximum File Size on the Flash Filesystem in KBytes.
   
   HELP
        <Definition>
           Maximum file size on the flash filesystem (EFS) of the phone.
           The value is specified in Kilo Bytes.
	<Instructions>
           The minimum value supported by the OATFS test cases for the 
           maximum file size is 8K bytes.
   }
}
Resource, IDS_DD_HW_MAX_MMC_FILESIZE, Memory
   uint32, Maximum File Size on the MMC Filesystem in KBytes.
   
   HELP
        <Definition>
           Maximum file size on the MMC filesystem of the phone.
           The value is specified in Kilo Bytes.
	<Instructions>
           The minimum value supported by the OATFS test cases for the 
           maximum file size is 8K bytes.
   }
}
Resource, IDS_DD_GETDEVICEINFOEX_FSSPACE_ROOT, Memory
   uint32, Main internal flash size support.
   
   HELP
        <Definition>
           0 - Not supported
           1 - Filesystem total space is less than or equal to 4GB
           2 - Filesystem total space is more than 4GB
	<Instructions>
           After the test verify that total and free space reported 
           in logs is accurate.
   }
}
Resource, IDS_DD_GETDEVICEINFOEX_FSSPACE_CARD0, Memory
   uint32, Primary MMC card size supported.
   
   HELP
        <Definition>
           0 - Not supported
           1 - Filesystem total space is less than or equal to 4GB
           2 - Filesystem total space is more than 4GB
	<Instructions>
           After the test verify that total and free space reported 
           in logs is accurate.
   }
}
Resource, IDS_DD_GETDEVICEINFOEX_FSSPACE_USBHMS1, Memory
   uint32, USB Host Mass Storage Device 1 size supported.
   
   HELP
        <Definition>
           0 - Not supported
           1 - Filesystem total space is less than or equal to 4GB
           2 - Filesystem total space is more than 4GB
	<Instructions>
           After the test verify that total and free space reported 
           in logs is accurate.
   }
}
Resource, IDS_DD_GETDEVICEINFOEX_FSSPACE_USBHMS2, Memory
   uint32, USB Host Mass Storage Device 2 size supported.
   
   HELP
        <Definition>
           0 - Not supported
           1 - Filesystem total space is less than or equal to 4GB
           2 - Filesystem total space is more than 4GB
	<Instructions>
           After the test verify that total and free space reported 
           in logs is accurate.
   }
}
#<!--___________________________________________________________________
#        Displays
#__________________________________________________________________-->
EnumList, IDEL_DISPLAY_CLSID
16852948, display1
16852949, display2
16852950, display3
16852951, display4   
}
#<!--___________________________________________________________________
#        Display 1
#__________________________________________________________________-->
Resource, IDS_DD_LCD_DIM_X, Display 1
   uint32, Size X
   HELP 
	<Definition>
	    Width of the display in pixels.
	<Instructions>
      Enter the width of the display in pixels.  This should be
      only the pixels available to BREW.  For example, if your display was
      100 pixels wide, but 10 pixels were used for status at the side of
      the display and are not available to BREW, enter "90".
   }
}
Resource, IDS_DD_LCD_DIM_Y, Display 1
   uint32, Size Y
   HELP 
	<Definition>
	    Height of the display in pixels.
	<Instructions>
      Enter the height of the display in pixels.  This should be
      only the pixels available to BREW.  For example, if your display was
      100 pixels high, but 10 pixels were used for status at the top of
      the display and are not available to BREW, enter "90".
   }
}
Resource, IDS_DD_LCD_DEPTH, Display 1
   uint32, BREW Default Color Depth (bits)
   HELP 
	<Definition>
	    Display's default color depth in BREW.
	<Instructions>
      Enter the display's default color depth (in bits) in BREW.
      This is the mode that the device bitmap is in when an application starts, unless specified
      otherwise in the application's MIF.

      For example, enter 16 for a device that has a bit depth of 16 and supports
      65536 colors.
   }
}
Resource, IDS_DD_LCD_DEPTH_HW, Display 1
   uint32, HW Color Depth (bits)
   HELP 
	<Definition>
	    Maximum color depth supported by the display.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported by the device.
      hardware and native UI.  This value is typically the same
      as BREW Color Depth, but may be different on some devices.  This value
      may be used by developers to create native UI wallpaper, etc.

      For example, enter 16 for a device that has a bit depth of 16 and supports
      65536 colors.
   }
}
Resource, IDS_DD_LCD_REF_RATE, Display 1
   uint32, Refresh Rate
   HELP 
	<Definition>
	    Refresh rate of the display in frames/sec.
	<Instructions>
      Enter the hardware refresh rate of the display in frames/sec.
   }
}
Resource, IDS_DD_LCD_TYPE, Display 1
   string, Display Type
   HELP 
	<Definition>
	    The display's LCD type.
	<Instructions>
      Enter the display's LCD type (for example, TFT, STN, etc.).
   }
}
Resource, IDS_DD_LCD_PITCH, Display 1
   string, Pixel Pitch, , |
   HELP 
	<Definition>
	    The display's pixel block size and spacing between pixels.
	<Instructions>
      Enter the display's pixel block size and spacing between pixels
      in mm in the form SizeX x SizeY, SpaceX x SpaceY (for example, 0.2mm x 0.2mm,
      0.01mm x 0.01mm).
   }
}
Resource, IDS_DD_LCD_SUPPORTED_DEPTHS, Display 1
   string, BREW Supported Color Depths (bits), , |
	HELP 
	<Definition>
	    All color depths supported by the display in BREW.
	<Instructions>
	    This is a comma separated list of all the color depth modes supported by the 
	    display in BREW.

	    For example, for a display that can be in either 16- or 18-bit mode, 
	    enter "16,18". 
	}
}
Resource, IDS_DD_LCD_ANNUN_DIM_Y, Display 1
   uint32, Annun Size Y
   HELP 
	<Definition>
	   Height of the status display available to BREW. "0" mean not available to BREW.
	<Instructions>
      Enter "0" if the status area is not available to BREW. 

      Enter the height of the display in pixels used to display the status at the top if available to BREW.  For example, 
      if your display was 100 pixels high, but 10 pixels were used for status at the top of the display and can be 
      controlled by BREW, enter "10". 

      If by default the whole display, including the status area, is available to BREW and the status area can
      be turned on or off inside BREW, then the size should be provided in -ve.
   }
}

#<!--___________________________________________________
#        Display 2
#_______________________________________________________-->
Resource, IDS_DD_LCD2_DIM_X, Display 2
   uint32, Size X
   HELP 
	<Definition>
	   Width of the display in pixels.
	<Instructions>
      Enter the width of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was
      100 pixels wide, but 10 pixels were used for status at the side of
      the display and are not available to BREW, you would enter "90".
      
      If there is no display, enter "0".
   }  
}
Resource, IDS_DD_LCD2_DIM_Y, Display 2
   uint32, Size Y
   HELP 
	<Definition>
	   Height of the display in pixels.
	<Instructions>
      Enter the height of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was 20 pixels
      high, but 5 pixels were used for status at the top of the display and
      are not available to BREW, you would enter "15".

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD2_DEPTH, Display 2
   uint32, BREW Default Color Depth
   HELP 
	<Definition>
	    Display's default color depth in BREW.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported in
      BREW. This is the mode that the device bitmap is in when an application starts, unless specified
      otherwise in the application's MIF.
			

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD2_DEPTH_HW, Display 2
   uint32, HW Color Depth (bits)
   HELP 
	<Definition>
	    Maximum color depth supported by the display.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported
      by the device hardware and native user interface.  This value is 
      typically the same as BREW Color Depth, but may be different on some
      devices.  This value may be used by developers to create native UI 
      wallpaper, etc.

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.
   }
}
Resource, IDS_DD_LCD2_REF_RATE, Display 2
   uint32, Refresh Rate
   HELP 
	<Definition>
      Refresh rate of the display in frames/sec.
	<Instructions>
      Enter the hardware refresh rate of the display in frames/seconds.

      If there is no secondary display, enter "0".
   }
}
Resource, IDS_DD_LCD2_TYPE, Display 2
   string, Type
   HELP 
	<Definition>
	    The display's LCD type.
	<Instructions>
      Enter the display's LCD type (for example, TFT, STN, etc.).

      If there is no secondary display, enter "none".
   }
}
Resource, IDS_DD_LCD2_PITCH, Display 2
   string, Pixel Pitch, , |
   HELP
	<Definition>
	    The display's pixel block size and spacing between pixels.
	<Instructions>
      Enter the display's pixel block size and spacing between pixels
      in millimeters in the form SizeX x SizeY, SpaceX x SpaceY (for example, 0.2mm x 0.2mm,
      0.01mm x 0.01mm).

      If there is no secondary display, enter "0".
   }
}
Resource, IDS_DD_LCD2_SUPPORTED_DEPTHS, Display 2
   string, BREW Supported Color Depths (bits), , |
	HELP
	<Definition>
	    All color depths supported by the display in BREW.	    
	<Instructions>
	    This is a comma separated list of all the color depth modes supported by the 
      display in BREW.
      
      For example, for a display that can be in either 16- or 18-bit mode, 
      enter "16,18". 
	}
}
Resource, IDS_DD_LCD2_ANNUN_DIM_Y, Display 2
   uint32, Annun Size Y
   HELP 
	<Definition>
	     Height of the status display available to BREW. "0" mean not available to BREW.
	<Instructions>
      Enter "0" if the status area is not available to BREW. 

      Enter the height of the display in pixels used to display the status at the top if available
      to BREW.  For example, if your display was 100 pixels high, but 10 pixels were used for status 
      at the top of the display and can be controlled by BREW, you would enter "10". 

      If by default the whole display, including the status area, is available to BREW and the 
      status area can be turned on or off inside BREW, then the size should be provided in -ve.
   }
}

#<!--___________________________________________________
#        Display 3
#_______________________________________________________-->
Resource, IDS_DD_LCD3_DIM_X, Display 3
   uint32, Size X
   HELP 
	<Definition>
	  Width of the display in pixels.
	<Instructions>
      Enter the width of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was
      100 pixels wide, but 10 pixels were used for status at the side of
      the display and are not available to BREW, you would enter "90".
      
      If there is no display, enter "0".
   }  
}
Resource, IDS_DD_LCD3_DIM_Y, Display 3
   uint32, Size Y
   HELP 
	<Definition>
	    Height of the display in pixels.
	<Instructions>
      Enter the height of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was 20 pixels
      high, but 5 pixels were used for status at the top of the display and
      are not available to BREW, you would enter "15".

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD3_DEPTH, Display 3
   uint32, BREW Default Color Depth
   HELP 
	<Definition>
	    Display's default color depth in BREW.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported in
      BREW. This is the mode that the device bitmap is in when an application starts, unless specified
      otherwise in the application's MIF.
			

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD3_DEPTH_HW, Display 3
   uint32, HW Color Depth (bits)
   HELP 
	<Definition>
	    Maximum color depth supported by the display.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported
      by the device hardware and native UI.  This value is 
      typically the same as BREW Color Depth, but may be different on some
      devices.  This value may be used by developers to create native UI 
      wallpaper, etc.

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.
   }
}
Resource, IDS_DD_LCD3_REF_RATE, Display 3
   uint32, Refresh Rate (frames/sec)
   HELP 
	<Definition>
	    Refresh rate of the display in frames/sec.
	<Instructions>
      Enter the hardware refresh rate of the display in frames/seconds.

      If there is no third display, enter "0".
   }
}
Resource, IDS_DD_LCD3_TYPE, Display 3
   string, Type
   HELP 
	<Definition>
	    The display's LCD type.
	<Instructions>
      Enter the display's LCD type (for example, TFT, STN, etc.).

      If there is no third display, enter "none".
   }
}
Resource, IDS_DD_LCD3_PITCH, Display 3
   string, Pixel Pitch, , |
   HELP 
	 <Definition>
      The display's pixel block size and spacing between pixels.
	 <Instructions>
      Enter the display's pixel block size and spacing between pixels
      in millimeter in the form SizeX x SizeY, SpaceX x SpaceY (for example, 0.2mm x 0.2mm,
      0.01mm x 0.01mm).

      If there is no third display, enter "0".
   }
}
Resource, IDS_DD_LCD3_SUPPORTED_DEPTHS, Display 3
   string, BREW Supported Color Depths (bits), , |
	HELP 
	<Definition>
	    All color depths supported by the display in BREW.
	<Instructions>
	    This is a comma separated list of all the color depth modes supported by the 
			display in BREW.
      For example, for a display that can be in either 16- or 18-bit mode, 
			enter "16,18". 
	}
}
Resource, IDS_DD_LCD3_ANNUN_DIM_Y, Display 3
   uint32, Annun Size Y
   HELP 
	  <Definition>
		  Height of the status display available to BREW. "0" mean not available to BREW.
	  <Instructions>
      Enter "0" if the status area is not available to BREW. 

      Enter the height of the display in pixels used to display the status at the top if available to BREW.  
      For example, if your display was 100 pixels high, but 10 pixels were used for status at the top 
      of the display and can be controlled by BREW, enter "10". 

      If by default the whole display, including the status area, is available to BREW and the status area can
      be turned on or off inside BREW, then the size should be provided in -ve.
   }
}

#<!--___________________________________________________
#        Display 4
#_______________________________________________________-->
Resource, IDS_DD_LCD4_DIM_X, Display 4
   uint32, Size X
   HELP 
	<Definition>
	   Width of the display in pixels.
	<Instructions>
      Enter the width of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was
      100 pixels wide, but 10 pixels were used for status at the side of
      the display and are not available to BREW, enter "90".
      
      If there is no display, enter "0".
   }  
}
Resource, IDS_DD_LCD4_DIM_Y, Display 4
   uint32, Size Y
   HELP 
	<Definition>
	   Height of the display in pixels.
	<Instructions>
      Enter the height of the display in pixels.  This should be only the
      pixels available to BREW.  For example, if your display was 20 pixels
      high, but 5 pixels were used for status at the top of the display and
      are not available to BREW, you would enter "15".

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD4_DEPTH, Display 4
   uint32, BREW Default Color Depth
   HELP 
	<Definition>
	   Display's default color depth in BREW.
	<Instructions>
      Enter the display's maximum color depth (in bits) supported in
      BREW. This is the mode that the device bitmap is in when an application starts, unless specified
      otherwise in the application's MIF.
			

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.

      If there is no display, enter "0".
   }
}
Resource, IDS_DD_LCD4_DEPTH_HW, Display 4
   uint32, HW Color Depth (bits)
   HELP 
	 <Definition>
	    Maximum color depth supported by the display.
   <Instructions>
      Enter the display's maximum color depth (in bits) supported
      by the device hardware and native user interface.  This value is 
      typically the same as BREW Color Depth, but may be different on some
      devices.  This value may be used by developers to create native UI 
      wallpaper, etc.

      For example, enter "16" for a device that has a bit depth of 16 and supports
      65536 colors.
   }
}
Resource, IDS_DD_LCD4_REF_RATE, Display 4
   uint32, Refresh Rate (frames/sec)
   HELP 
	 <Definition>
	    Refresh rate of the display in frames/sec.
	 <Instructions>
      Enter the hardware refresh rate of the display in frames/seconds.

      If there is no fourth display, enter "0".
   }
}
Resource, IDS_DD_LCD4_TYPE, Display 4
   string, Type
   HELP
	  <Definition>
		  The display's LCD type.
	  <Instructions>
      Enter the display's LCD type (for example, TFT, STN, etc.).

      If there is no fourth display, "none".
   }
}
Resource, IDS_DD_LCD4_PITCH, Display 4
   string, Pixel Pitch, , |
   HELP 
	 <Definition>
	    The display's pixel block size and spacing between pixels.
   <Instructions>
      Enter the display's pixel block size and spacing between pixels
      in millimeters in the form SizeX x SizeY, SpaceX x SpaceY.  For example, 0.2mm x 0.2mm,
      0.01mm x 0.01mm.

      If there is no fourth display, enter "0".
   }
}
Resource, IDS_DD_LCD4_SUPPORTED_DEPTHS, Display 4
   string, BREW Supported Color Depths (bits), , |
	HELP 
	<Definition>
	   All color depths supported by the display in BREW.
	<Instructions>
	    This is a comma separated list of all the color depth modes supported by the 
      display in BREW.  For example, for a display that can be in either 16- or 18-bit mode, 
      enter "16,18". 
	}
}
Resource, IDS_DD_LCD4_ANNUN_DIM_Y, Display 4
   uint32, Annun Size Y
   HELP 
	 <Definition>
	    Height of the status display available to BREW. "0" mean not available to BREW.
	 <Instructions>
      Enter "0" if the status area is not available to BREW. 

      Enter the height of the display in pixels used to display the status at the top if available 
      to BREW.  For example, if your display was 100 pixels high, but 10 pixels were used for status 
      at the top of the display and can be controlled by BREW, enter "10". 

      If by default the whole display, including the status area, is available to BREW and the status area can
      be turned on or off inside BREW, then the size should be provided in -ve.
   }
}

#<!--_______________________________________
#        Backlight
#_______________________________________-->
Resource, IDS_DD_BACKLIGHT_SUPPORT, Backlight 
   enum, IBacklight Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IBacklight supported on the device.
	 <Instructions>
	     Enter Yes if IBacklight is implemented on the device
	     Enter No if IBacklight is not implemented on the device
	 }
}
Resource, IDS_DD_BACKLIGHT_MAX_BRIGHT_LEVEL, Backlight,Developer|OEM
   uint32, Maximum Brightness Level
	HELP 
	<Instructions>
          Enter the number that corresponds to the maximum backlight brightness level.	   
   }
}
Resource, IDS_DD_BACKLIGHT_MIN_BRIGHT_LEVEL, Backlight,Developer|OEM
   uint32, Minimum Brightness Level
	HELP 
	<Instructions>
          Enter the number that corresponds to the minimum backlight brightness level.
   }
}
Resource, IDS_DD_BACKLIGHT_DEF_BRIGHT_LEVEL, Backlight,Developer|OEM
   uint32, Default Brightness Level
	HELP 
	<Instructions>
          Enter the number that corresponds to the default backlight brightness level.
   }
}
Resource, IDS_DD_BACKLIGHT_COLOR, Backlight,Developer|OEM
   uint32, Backlight Color
	HELP 
	<Definition>
	    The RGB value of the Backlight on the device
	<Instructions>
      Enter the RGB value of the backlight on the device in hex. 

      The value consists of 3 bytes of hex data, the first byte for Red, the second byte for 
      Green, the third byte for Blue. For example, maximum red, no green, and no blue would result 
      in 0xFF0000.
   }
}


#<!--_______________________________________
#                Keypad
#_______________________________________-->
EnumList, IDEL_KEYPAD
   None, None
   AVK_SELECT
   AVK_LEFT
   AVK_RIGHT
   AVK_UP
   AVK_DOWN
   AVK_FUNCTION1
   AVK_FUNCTION2
   AVK_SOFT1
   AVK_SOFT2
   AVK_MENU
   AVK_MESSAGE
   AVK_SEND
   Other, Other
}

Resource, IDS_DD_KEY_APPMGR, Keypad
   enum, AppMgr Key, IDEL_KEYPAD
   HELP 
	<Definition>
	    Dedicated key to launch BREW Application Manager
	<Instructions>
      If there is a dedicated key for launching the BREW Application Manager, select that
      key from the list.  If the key is not listed, select "Other" and enter
      the key value in the Comment field.

      If there is not a dedicated key for launching the BREW Application Manager, select
      "None".
   }
}
Resource, IDS_DD_BAS_SKEY, Keypad
   enum, Simultaneous Keypress, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	   Simultaneous Keypress support
	<Instructions>
      Are simultaneous keypresses supported on the device?
   }
}
Resource, IDS_DD_KEYGUARD, Keypad
   enum, Keyguard support, IDEL_LABEL_YESNO
   HELP
	 <Definition>
	    Keyguard support.
	<Instructions>
      Is keyguard supported on the device?
   }
}
Resource, IDS_DD_AUD_VOL_KEY, Keypad
   enum, Volume Keys, IDEL_LABEL_YESNO
   HELP
	<Definition>
	    Dedicated Volume Keys on the device 
	<Instructions>
      Are there dedicated volume keys on the device?  These dedicated volume keys must be 
      mapped to AVK_VOLUME_UP and AVK_VOLUME_DOWN.
   }
}
Resource, IDS_DD_KEY_TONES_SUPPORTED, Keypad,Developer|OEM
   enum, Key Tones Support, IDEL_YESNO
   HELP 
	 <Definition>
	    Keytone support.
	<Instructions>
      Does the device support Key tones?
      If yes, select "Yes"; otherwise, select "No".   
   }
}
Resource, IDS_DD_EXT_KBD_KEY, Keypad,Developer|OEM
   uint32, AVK Key Code for External Keyboard Key
   HELP
	<Definition>
	    AVK Code of an External Keyboard Key. This key will be in the OEM defined AVK range and will be used to test the KB_EXTERNAL modifier flag presense in EVT_KEY event for the key. If external keyboard is not supported, enter a value of "0".
	<Instructions>
	    If there is an external keyboard with keys mapped to an OEM defined AVK range, enter the AVK Code of one such key.
   }
}
EnumList, IDEL_KEYCODES
   57360,AVK_UNDEFINED
   57377,AVK_0
   57378,AVK_1
   57379,AVK_2
   57380,AVK_3
   57381,AVK_4
   57382,AVK_5
   57383,AVK_6
   57384,AVK_7
   57385,AVK_8
   57386,AVK_9
   57387,AVK_STAR
   57388,AVK_POUND
   57389,AVK_POWER
   57390,AVK_END
   57391,AVK_SEND
   57392,AVK_CLR
   57393,AVK_UP
   57394,AVK_DOWN
   57395,AVK_LEFT
   57396,AVK_RIGHT
   57397,AVK_SELECT
   57398,AVK_SOFT1
   57399,AVK_SOFT2
   57400,AVK_SOFT3
   57401,AVK_SOFT4
   57402,AVK_FUNCTION1
   57403,AVK_FUNCTION2
   57404,AVK_FUNCTION3
   57405,AVK_FUNCTION
   57406,AVK_MENU
   57407,AVK_INFO
   57408,AVK_SHIFT
   57409,AVK_MESSAGE
   57410,AVK_MUTE
   57411,AVK_STORE
   57412,AVK_RECALL
   57413,AVK_PUNC1
   57414,AVK_PUNC2
   57415,AVK_VOLUME_UP
   57416,AVK_VOLUME_DOWN
   57417,AVK_WEB_ACCESS
   57418,AVK_VOICE_MEMO
   57419,AVK_SPEAKER
   57420,AVK_TAP
   57421,AVK_PTT
   57422,AVK_LSHIFT
   57423,AVK_RSHIFT
   57424,AVK_LCTRL
   57425,AVK_RCTRL
   57426,AVK_LALT
   57427,AVK_RALT
   57428,AVK_CAPLK
   57429,AVK_SCRLK
   57430,AVK_NUMLK
   57431,AVK_LNGHANGUL
   57432,AVK_LNGJUNJA
   57433,AVK_LNGFIN
   57434,AVK_LNGHANJA
   57435,AVK_PRSCRN
   57436,AVK_BREAK
   57437,AVK_TXINSERT
   57438,AVK_TXDELETE
   57439,AVK_TXHOME
   57440,AVK_TXEND
   57441,AVK_TXPGUP
   57442,AVK_TXPGDOWN
   57443,AVK_GP_1   
   57444,AVK_GP_2   
   57445,AVK_GP_3   
   57446,AVK_GP_4   
   57447,AVK_GP_5   
   57448,AVK_GP_6   
   57449,AVK_GP_SL  
   57450,AVK_GP_SR  
   57451,AVK_CAMERA       
   57452,AVK_VIDEO_PHONE
   57453,AVK_APP_SWITCH
   57454,AVK_PLAY
   57455,AVK_PAUSE
   57456,AVK_SKIP_FWD
   57457,AVK_SKIP_RWD
   57458,AVK_FFWD
   57459,AVK_RWD
   57460,AVK_STOP
   57461,AVK_HEADSET_FUNC
   57462,AVK_FUNCTION4
   57463,AVK_FUNCTION5
   57464,AVK_FUNCTION6
   57465,AVK_FUNCTION7
   57466,AVK_FUNCTION8
   57461,AVK_HEADSET_FUNC
   57462,AVK_FUNCTION4
   57463,AVK_FUNCTION5
   57464,AVK_FUNCTION6
   57465,AVK_FUNCTION7
   57466,AVK_FUNCTION8
   57467,AVK_FUNCTION9
   57468,AVK_FUNCTION10
   57469,AVK_FUNCTION11
   57470,AVK_FUNCTION12
   57471,AVK_SYMBOL
   57472,AVK_ENTER
   57473,AVK_SPACE
   57467,AVK_FUNCTION9
   57468,AVK_FUNCTION10
   57469,AVK_FUNCTION11
   57470,AVK_FUNCTION12
   57471,AVK_SYMBOL
   57472,AVK_ENTER
   57473,AVK_SPACE
      97,AVK_A
      98,AVK_B
      99,AVK_C
     100,AVK_D
     101,AVK_E
     102,AVK_F
     103,AVK_G
     104,AVK_H
     105,AVK_I
     106,AVK_J
     107,AVK_K
     108,AVK_L
     109,AVK_M
     110,AVK_N
     111,AVK_O
     112,AVK_P
     113,AVK_Q
     114,AVK_R
     115,AVK_S
     116,AVK_T
     117,AVK_U
     118,AVK_V
     119,AVK_W
     120,AVK_X
     121,AVK_Y
     122,AVK_Z
}
Resource, IDS_DD_SUPPORTED_KEYS, Keypad
   complex, Supported Keys, 1,0,0, |
      enum, Supported Keys List, IDEL_KEYCODES
   }  
   HELP
    	Select the key codes of all keys accessible on the device. 
   }
}
Resource, IDS_DD_EXTENDED_KEYPAD_SUPPORT, Keypad
   enum, Extended keypad support, IDEL_LABEL_YESNO 
   HELP
   Select "Yes" if the device supports the standard BREW extended keypad implementation. Otherwise, select "No".
   }
}

#<!--_______________________________________
#                Audio
#_______________________________________-->
EnumList, IDEL_HEADSET
   None, None
   Stereo, Stereo
   Mono, Mono
}
EnumList, IDEL_FARFIELD
   0, None
   1, Mono
   2, Stereo
}

Resource, IDS_DD_AUD_STR_HEADSET, Audio
   enum, Headset Port, IDEL_HEADSET
   HELP 
	<Definition>
	    The type of headset available to BREW
	<Instructions>
      Select the type of headset port available to BREW (stereo or mono).  Select
      "None" if no headset port is available to BREW.
   }
}
Resource, IDS_DD_AUD_VIB, Audio
   enum, Vibrator, IDEL_LABEL_YESNO
   HELP 
	 <Definition>
	    Is a vibrator supported
	 <Instructions>
      Is a vibrator available to BREW?  This should match the value returned
      by ISHELL_GetDeviceInfo().
   }
}
Resource, IDS_DD_AUD_RINGER_SPK, Audio
   enum, Ringer only speaker, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Ringer-only speaker on the device
	<Instructions>
      Is the Ringer-only speaker available to BREW?
   }
}
Resource, IDS_DD_AUD_FARFLD_SPK, Audio
   enum, Far field speaker, IDEL_FARFIELD
   HELP 
	<Definition>
	    Far field speaker type supported by the device 
	<Instructions>
      Select the type of far field speaker available to BREW (stereo or mono).  Select
      "None" if no far field speaker is available to BREW.
   }
}
Resource, IDS_DD_RECORD_INPUTS, Audio,Developer|OEM
   string, Recording Inputs, , |
   HELP
    <Definition>
		Recording Input types supported
    <Instructions>
		If media recording is supported through BREW, enter a comma separated list of 
		audio device types whose mics can be used for recording (for example, handset, 
		bluetooth headset, hands free kit); otherwise, leave blank.
   }
}

#<!--=================================================================
#        Media Formats
#=================================================================-->
Resource, IDS_DD_MULTIMEDIA_VERSION, Media Types
   string, Multimedia File Version
	HELP 
	<Definition>
	   CMX-type Multimedia file format support and exceptions
	<Instructions>
       Does the device support a non-CMX Multimedia file format? If so, enter the format and version number. Also, enter any types not supported in the Comment field. 
       If the device supports CMX instead or does not support a multimedia file format, enter "none".
   }
} 

#<!--=================================================================
#        Application Support
#=================================================================-->
EnumList, IDEL_ARCHITECTURE
   0, Little-Endian
   1, Big-Endian
}

Resource, IDS_DD_BAS_ARCH, Application Support,Developer|OEM
   enum, Processor Architecture, IDEL_ARCHITECTURE
   HELP 
		<Instructions>
      Select the architecture of the processor running BREW (Little-Endian 
      or Big-Endian).
   }
}
Resource, IDS_DD_BAS_COMP_NAME, Application Support,Developer|OEM
   string, Compiler Name and Version
   HELP 
	<Definition>
	   Recommended Compiler version
	<Instructions>
      Enter the name and version of the compiler recommended to build BREW
      application software (for example, ADS 1.2).
   }
}
Resource, IDS_DD_BAS_COMP_FLAGS, Application Support,Developer|OEM
   string, Compiler Flags
   HELP 
	<Definition>
	    Compiler flags needed to build for this device
	<Instructions>
      List any special compiler flags needed to build BREW application
      software to run on this device (for example, -bigend -fy).  

      Document the meaning and reason for each compiler flag in the Comment field.
   }
}
Resource, IDS_DD_DATA_PRCK_VC, Application Support
   enum, Voice Call Privacy Check, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    User prompted before a voice call is made.
	<Instructions>
      Is the user prompted before a voice call is made?
   }
}
Resource, IDS_DD_DATA_PRCK_GPSFIX, Application Support
   enum, GPS Privacy Check, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    User prompted before GPS call is made.
	<Instructions>
      Is the user prompted before the call for GPS service is made?
   }
}
Resource, IDS_DD_BAS_DISP_ANNUN, Application Support
   enum, Annunciators Display, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Annuciators active before BREW are also visible in BREW.
	<Instructions>
      Are the same annunciators that are active before launching BREW also
      visible in BREW?  

      Document any exceptions or special cases in the Comment field.
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_MSG, Application Support
   enum, Message annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Message annunciator controllable
	<Instructions>
      Can the Message annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_NET_MSG, Application Support
   enum, Net Message annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Net Message annunciator controllable
	<Instructions>
      Can the Net Message annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_ALARMCLOCK, Application Support
   enum, Alarm annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Alarm annunciator controllable
	<Instructions>
      Can the Alarm annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_NET_LOCK, Application Support
   enum, Net Lock annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Net Lock annunciator controllable
	<Instructions>
      Can the Net Lock annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_STOPWATCH, Application Support
   enum, Stopwatch annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Stopwatch annunciator controllable through BREW.
	<Instructions>
      Can the Stopwatch annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_COUNTDOWN, Application Support
   enum, Countdown annunciator, IDEL_LABEL_YESNO
   HELP
	<Definition>
	   Countdown annunciator controllable through BREW.
	<Instructions>
      Can the Countdown annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_BAS_BREW_ANNUN_ANNUN_SILENCEALL, Application Support
   enum, Silence All annunciator, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Silence All annunciator available through BREW
	<Instructions>
      Can the Silence All annunciator be controlled through BREW?
   }
}
Resource, IDS_DD_NATIVE_NOTIFIER, Application Support,Developer|OEM
   enum, Idle Native Notifier, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Native notification sent to BREW while exiting or entering the Idle screen.
	<Instructions>
      Are native notifcations sent to BREW when entering or exiting the Idle screen?  
   }
}
Resource, IDS_DD_MMCNTFY_SUPPORT, Application Support
   enum, MMC Insert and Remove Notification Support, IDEL_YESNO
   HELP 
	<Definition>
	    Device Support for RMC Insert and Remove Notifications.
	<Instructions>
      If the device supports RMC insert and remove notifications, select "Yes".
      Otherwise, select "No".   
   }
}
Resource, IDS_DD_APP_LIST, Preloaded Applications
   complex, Preloaded Applications, 1, 0, none, ,
      string, Class ID
   }
   HELP 
	<Definition>
	    Preloaded applications on the device and information about the applications
	<Instructions>
      List any preloaded applications on the device by entering their Class 
      IDs (decimal or hex).  

      Enter the text name or description and version number of each
      application in the Comment field.
      
      Enter "none" if no applications are preloaded.
   }
}
Resource, IDS_DD_MMS_APP_SUPPORT, Applications,OEM
   enum, MMS Application Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Is there a BREW or native MMS application pre-loaded on the device?
	<Instructions>
      Select "Yes" if there is a BREW or native MMS application pre-loaded on the device.
   }
}
Resource, IDS_DD_WAP_APP_SUPPORT, Applications,OEM
   enum, WAP Application Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Is there a BREW or native WAP application pre-loaded on the device?
	<Instructions>
      Select "Yes" if there is a BREW or native WAP application pre-loaded on the device.
   }
}
Resource, IDS_DD_PRE_EXT_IDS, Preloaded Extensions,Developer|OEM
   complex, Developer Extensions, 1, 0, none, ,
      string, Class ID
   }
   HELP 
	<Definition>
	    Preloaded developer extensions and information about them.
	<Instructions>
      List any preloaded developer extensions on the device by entering their
      Class IDs (decimal or hex).  

      Enter the text name or description and version number of each
      extension in the Comment field.
      
      Enter "none" if no developer extensions are preloaded.
   }
}

Resource, IDS_DD_CARR_EXT_LIST, Preloaded Extensions
   complex, Carrier Extensions, 1, 0, none, ,
      string, Class ID
   }
   HELP 
	<Definition>
	    Preloaded Carrier extension and information about them
	<Instructions>
      List any preloaded carrier extensions on the device by entering their
      Class IDs (decimal or hex).  

      Enter the text name or description and version number of each
      extension in the Comment field.
      
      Enter "none" if no carrier extensions are preloaded.
   }
}
Resource, IDS_DD_OEM_EXT_LIST, Preloaded Extensions
   complex, OEM Extensions, 1, 0, none, ,
      string, Class ID
   }
   HELP 
	<Definition>
	    Preloaded extenstions and information about them
	<Instructions>
      List any preloaded OEM extensions on the device by entering their
      Class IDs (decimal or hex).  

      Enter the text name or description and version number of each
      extension in the Comment field.
      
      Enter "none" if no OEM extensions are preloaded.
   }
}
Resource, IDS_DD_LANG_LIST, Language Support
   string, Language list, , |
   HELP 
	<Definition>
	    List of the language codes of the langauges supported by the device
	<Instructions>
      List the languages supported by this device when in BREW (for example,
      English, Spanish).  The default language must be the first entry in the list.
   }
}


#<!--=================================================================
#        Text Entry
#=================================================================-->

# note a common ASCII character 's' is specified as a delimiter so no
# symbols are treated as such.
Resource, IDS_DD_TXT_SYM, Text Entry
   string, Symbols, , s
   HELP 
	<Definition>
	   Symbols supported by the BREW text input control
	<Instructions>
      Enter all symbols supported by the BREW text input control.  List
      only symbols.  Do not use spaces, commas, or other characters
      as delimiters.  For example, -.&'()_!?{}[]^;></\@~*#%":+=
   }
}
Resource, IDS_DD_TXT_PRED_ENTRY, Text Entry
   enum, Predictive entry, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Predict entry support on the device
	<Instructions>
      Is predictive text entry (for example, T9 or eZiText) supported on this
      device?  List the type in Comment field.
   }
}
Resource, IDS_DD_TXT_MODES, Text Entry
   string, Modes, , |
   HELP 
	<Definition>
	    Text input modes supported through the BREW text input control.
	<Instructions>
            Enter all text input modes available in the BREW text input control.
            For example, 
   }
}

#<!--=================================================================
#        Font
#=================================================================-->
EnumList, IDEL_WIDTH_TYPE
   0, Fixed
   1, Dynamic
}

Resource, IDS_DD_FONT_NORMAL_ASCENT, Font
   uint32, Normal Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_NORMAL_DESCENT, Font
   uint32, Normal Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_NORMAL_WIDTH, Font
   enum, Normal Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_NORMAL_WIDTH_ASCII, Font
   string, Normal Font ASCII Character Width
   HELP 
	    If normal ASCII characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If normal ASCII characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If normal ASCII characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_NORMAL_WIDTH_ASIAN, Font
   string, Normal Font Asian Character Width
   HELP 
	    If normal Asian characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If normal Asian characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If normal Asian characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_NORMAL_GLYPH, Font
   complex, Supported Glyphs, 1, ,
      string, Glyph / Glyph Range
   }
   HELP 
	<Definition>
	    The Unicode values for each glyph supported on the device, can be listed individually or in ranges
	<Instructions>
      Enter the Unicode value for each glyph supported on the
      device.  You many enter individual glyphs or glyph ranges.  To specify a
      glyph range, using the format <starting glyph>-<ending glyph> (for
      example, 0x5b-0x60).  Values may be entered in decimal or hex format.

      It is expected that the normal, bold, and large fonts will all support
      the same glyphs.  Document any discrepancies in the Comment field.
   }
}
Resource, IDS_DD_FONT_BOLD_ASCENT, Font
   uint32, Bold Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_BOLD_DESCENT, Font
   uint32, Bold Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_BOLD_WIDTH, Font
   enum, Bold Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_BOLD_WIDTH_ASCII, Font
   string, Bold Font ASCII Character Width
   HELP 
      If bold ASCII characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If bold ASCII characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If bold ASCII characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_BOLD_WIDTH_ASIAN, Font
   string, Bold Font Asian Character Width
   HELP 
      If bold Asian characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If bold Asian characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If bold Asian characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_LARGE_ASCENT, Font
   uint32, Large Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_LARGE_DESCENT, Font
   uint32, Large Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_LARGE_WIDTH, Font
   enum, Large Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_LARGE_WIDTH_ASCII, Font
   string, Large Font ASCII Character Width
   HELP 
      If large ASCII characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If large ASCII characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If large ASCII characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_LARGE_WIDTH_ASIAN, Font
   string, Large Font Asian Character Width
   HELP 
      If large Asian characters have a variable (dynamic) width (also called a 
      proportional font), enter "0".

      If large Asian characters have a fixed width (also called a monospace font),
      enter the width of a character in pixels.

      If large Asian characters are not supported, enter "-1".
   }
}
Resource, IDS_DD_FONT_ITALICS_ASCENT, Font
   uint32, Italics Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_ITALICS_DESCENT, Font
   uint32, Italics Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_ITALICS_WIDTH, Font
   enum, Italics Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_BOLD_ITALICS_ASCENT, Font
   uint32, Italics Bold Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_BOLD_ITALICS_DESCENT, Font
   uint32, Italics Bold Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_BOLD_ITALICS_WIDTH, Font
   enum, Italics Bold Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_LARGE_ITALICS_ASCENT, Font
   uint32, Italics Large Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_LARGE_ITALICS_DESCENT, Font
   uint32, Italics Large Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_LARGE_ITALICS_WIDTH, Font
   enum, Italics Large Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for ASCII characters.
   }
}
Resource, IDS_DD_FONT_USER_1_ASCENT, Font
   uint32, USER_1 Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_USER_1_DESCENT, Font
   uint32, USER_1 Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_USER_1_WIDTH, Font
   enum, USER_1 Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for User Defined characters.
   }
}
Resource, IDS_DD_FONT_USER_2_ASCENT, Font
   uint32, USER_2 Font - Ascent
   HELP 
      The height of the font above the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_USER_2_DESCENT, Font
   uint32, USER_2 Font - Descent
   HELP 
      The height of the font below the baseline in pixels.
   }
}
Resource, IDS_DD_FONT_USER_2_WIDTH, Font
   enum, USER_2 Font Width, IDEL_WIDTH_TYPE
   HELP 
      Select Fixed or Dynamic (variable) width for User Defined characters.
   }
}
Resource, IDS_DD_QCSANS_SUPPORTED, Font
   enum, QCSANS Font Support, IDEL_YESNO
   HELP 
	<Definition>
	<Instructions>
      Does the device support QCSANS Font?
      If yes, select "Yes"; otherwise, select "No".   
   }
}

EnumList, IDEL_ENCODING
   UNICODE
   KSC5601 
   S-JIS
   CP949
   ISOLATIN1
}
Resource, IDS_DD_ENCODING, Font
   enum, Encoding, IDEL_ENCODING
   HELP 
	<Definition>
	    Encoding type used for character made available to BREW.
	<Instructions>
      Specify the encoding type for characters made available to BREW.
   }   
}

#<!--=================================================================
#        Position Determination
#=================================================================-->
EnumList, IDEL_LBS_AUTH_TYPE
   0, Trusted
   1, V1 Non-trusted
   2, V2 Non-trusted
   3, Other
}
Resource, IDS_DD_POS_GPS, Position Determination
   enum, GPS Supported, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    GPS-based position determination supported for BREW
	<Instructions>
      If GPS-based position determination available to BREW, select "YES".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_POS_SECTOR, Position Determination
   enum, Sector-based Supported, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Sector-based position determination supported for BREW
	<Instructions>
      If sector-based position determination available to BREW, select "YES".
      Otherwise, select "No"
   }
}
Resource, IDS_DD_POS_LBS_AUTH_POLICY, Position Determination
   enum, Device Support for Network LBS Authorization Policy, IDEL_LBS_AUTH_TYPE
   HELP 
	<Definition>
      Device support for network authorization via GPS LBS(Location-Based Services).
	<Instructions>
      This entry is not applicable if the device does not support GPS LBS.
      The network authorization policy is determined by the carrier.  
      
      NOTE:  Applications on Trusted LBS networks do not go through authorization. 

      "Other" is the category for any authorization mechanism not described in the list 
      that should be further described or identified in the Comment field. 

   }
}
Resource, IDS_DD_POS_STANDALONE, Position Determination
   enum, Standalone mode Supported, IDEL_LABEL_YESNO
   HELP
	<Definition>
	 Standalone mode position determination supported for BREW
	<Instructions>
       If Standalone mode position determination available to BREW, select "Yes".
       Otherwise, select "No".
   }
}
#<!--=================================================================
#        Exception LIst
#=================================================================-->
Resource, IDS_DD_EXCEPT_LIST, OEM Exceptions
   complex, Exceptions, 0, , ,
      string, Function
      string, Interface
      string, Reason
   }
   HELP 
	<Definition>
	    BREW functionality exceptions on device and information about them.
	<Instructions>
      For each exception, enter the following: 
      
      Function - The name of the function at the AEE layer that has not been
      implemented.

      Interface - The name of the interface that the function belongs to.

      Reason - The reason why the function/interface has not been implemented.
   }
}

#<!--=================================================================
#        Bluetooth
#=================================================================-->

EnumList, IDEL_BT_SUPPORT
   0, Not Supported
   1, Generic Bluetooth Support
}
EnumList, IDEL_SPP_BT_SUPPORT
   0, Not Supported
   1, Bluetooth SPP Support
}
EnumList, IDEL_OBEX_BT_SUPPORT
   0, Not Supported
   1, Bluetooth OBEX Support
}
EnumList, IDEL_AG_BT_SUPPORT
   0, Not Supported
   1, Bluetooth AG Support
}
EnumList, IDEL_A2DP_BT_SUPPORT
   0, Not Supported
   1, Bluetooth A2DP Support
}
EnumList, IDEL_AVRCP_BT_SUPPORT
   0, Not Supported
   1, Bluetooth AVRCP Support
}
EnumList, IDEL_RM_BT_SUPPORT
   0, Not Supported
   1, Bluetooth RM Support
}
EnumList, IDEL_SD_BT_SUPPORT
   0, Not Supported
   1, Bluetooth SD Support
}

Resource, IDS_DD_BLUETOOTH_SUPP, Bluetooth
   enum, Bluetooth Headset Support, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Bluetooth Headset support.
	<Instructions>
      If Bluetooth headsets supported as an ISound device type by the
      device, select "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_BT_PROFILE_SUPP, Bluetooth
   complex, Bluetooth Supported 
   enum, Bluetooth Support, IDEL_BT_SUPPORT
   enum, SPP Profile Support, IDEL_SPP_BT_SUPPORT
   enum, OBEX Profile Support, IDEL_OBEX_BT_SUPPORT
   enum, AG Profile Support, IDEL_AG_BT_SUPPORT
   enum, A2DP Profile Support, IDEL_A2DP_BT_SUPPORT
   enum, AVRCP Profile Support, IDEL_AVRCP_BT_SUPPORT
   enum, RM Profile Support, IDEL_RM_BT_SUPPORT
   enum, SD Profile Support, IDEL_SD_BT_SUPPORT
   }
   HELP 
      <Definition>
          Bluetooth IBT interface support.
      <Instructions>
          If the device supports the  Bluetooth General Profile, select "Yes" for Bluetooth Support.
          Otherwise, select "No".
          If the device supports the Bluetooth SPP Profile, select "Yes" for SPP Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth OBEX Profile, select "Yes" for OBEX Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth AG Profile, select "Yes" for AG Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth A2DP Profile, select "Yes" for A2DP Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth AVRCP Profile, select "Yes" for AVRCP Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth RM Profile, select "Yes" for RM Profile Support.
          Otherwise, select "No".
          If the device supports the Bluetooth SD Profile, select "Yes" for SD Profile Support.
          Otherwise, select "No".
   }
}


#<!--=================================================================
#        Ringer
#=================================================================-->
Resource, IDS_DD_RINGER_MAX_SIZE, Ringer
   uint32, Max Ringer Size
   HELP 
	<Definition>
	    Maximum ringer size in bytes. "0" mean no limit.
	<Instructions>
      Specify the maximum ringer size in bytes or 0 for no limit.
   }
}
Resource, IDS_DD_RINGER_FORMATS, Ringer
   complex, Supported Formats
      string, ID
      string, File Extension
   }
   HELP 
	<Definition>
	    Media Formats supported for ringers by AEESoundPlayerFile ID and extension. 
	<Instructions>
      For each media format supported for ringers, list the following:

      ID - The AEESoundPlayerFile ID for the format (decimal or hex).  For
      example, 12 or 0xC.
      
      File Extension -  The filename extension associated with the ID (for
      example, for an ID of 1 you would typically enter "mid").

      Note 1: pmd = CMX: Compact MIDI, SAF, PNG, JPEG, QCELP, ADPCM,MPEG4=AAC Video.
      
      Note 2: The ID is the value returned in pwFormats by IRINGERMGR_GetFormats().

      Note 3: Here is a mapping of AEESoundplayerFile IDs to the corresponding extension

      0x1  -  MIDI

      0x2  -  MP3 

      0x3  -  QCELP

      0x4  -  MMF

      0x5  -  Phrase

      0x6  -  iMelody

      0x7  -  ADPCM WAV

      0x8  -  Linear WAV

      0x9  -  AAC

      0xA  -  AMR

      0xB  -  CMX PMD

      0xC  -  MPEG4

      0xD  -  3GP

      0xE  -  3G2

      0xF  -  WMA

      0x10 -  WMV

      0x11 -  ASF

      0x12 -  RA

      0x13 -  RM

      0x14 -  MOV

      0x15 -  QT

      0x16 -  AVI

      0x17 -  AIFF 
   }
}
Resource, IDS_DD_RINGER_MAX_COUNT, Ringer
   uint32, Max Ringers
   HELP 
	<Definition>
	    Maximum number of ringer. "O" means no limit,
	<Instructions>
      Specify the maximum number of ringers that can be placed on the device
      (on many devices this is the maximum number of files that can be 
      stored in the ringer directory).  Enter "0" if there is no limit.
   }
}
Resource, IDS_DD_RINGER_CATEGORIES, Ringer
   complex, Categories
      string, ID
      string, Name
   }
   HELP 
      List the categories to which ringers can be assigned by developers.  For
      each category, enter the following:

      ID - The AEERingerCatID value (for example, 1).  If ringer categories
      are not supported, enter "0".
      
      Name - The name of the ringer category (for example, "Friends").  If
      ringer categories are not supported, enter "All".
   }
}
Resource, IDS_DD_RINGER_NATIVE_UI_ACTIVATE, Ringer
   enum, Native UI Activate, IDEL_LABEL_YESNO
   HELP 
      Can ringers created by BREW be activated using the native UI?
   }
}
Resource, IDS_DD_RINGER_DEL_NATIVE, Ringer
   enum, Native UI Delete, IDEL_LABEL_YESNO
   HELP 
      Can ringers created by BREW be deleted using the native UI?
   }
}
Resource, IDS_DD_RINGER_FILE_MENU, Ringer
   enum, Integrated Ringer File Menu, IDEL_LABEL_YESNO
   HELP 
      Is it possible to manage native and BREW downloaded ringers from the
      same native UI?
   }
}
EnumList, IDEL_LABEL_RINGER_AUDIO_PATH
   Ringer-only Speaker, Ringer-only Speaker
   Far-Field Speaker, Far-Field Speaker
   Earpiece Speaker, Earpiece Speaker
   Other, Other
}

Resource, IDS_DD_RINGER_AUDIO_PATH, Ringer
   enum, Audio playback path, IDEL_LABEL_RINGER_AUDIO_PATH
   HELP
      Select the ringer audio playback path.  If "Other", explain
      in the Comment field.
   }
}


#<!--=================================================================
#        AddrBook
#=================================================================-->
EnumList, IDEL_ADDR_FIELD_TYPE
   0, AEEDB_FT_NONE
   1, AEEDB_FT_BYTE
   2, AEEDB_FT_WORD
   3, AEEDB_FT_DWORD
   4, AEEDB_FT_STRING
   5, AEEDB_FT_BINARY
   6, AEEDB_FT_PHONE
   7, AEEDB_FT_BITMAP
   8, AEEDB_FT_MAX
}

EnumList, IDEL_ADDR_CAT
	0,      AEE_ADDR_CAT_NONE    
	1,      AEE_ADDR_CAT_PERSONAL
	2,      AEE_ADDR_CAT_BUSINESS
	32768,  AEE_ADDR_CAT_USER + 0
	32769,  AEE_ADDR_CAT_USER + 1
	32770,  AEE_ADDR_CAT_USER + 2
	32771,  AEE_ADDR_CAT_USER + 3
	32772,  AEE_ADDR_CAT_USER + 4
	32773,  AEE_ADDR_CAT_USER + 5
	32774,  AEE_ADDR_CAT_USER + 6
	32775,  AEE_ADDR_CAT_USER + 7
	32776,  AEE_ADDR_CAT_USER + 8
	32777,  AEE_ADDR_CAT_USER + 9
	32778,  AEE_ADDR_CAT_USER + 10
	32779,  AEE_ADDR_CAT_USER + 11
	32780,  AEE_ADDR_CAT_USER + 12
	32781,  AEE_ADDR_CAT_USER + 13
	32782,  AEE_ADDR_CAT_USER + 14
	32783,  AEE_ADDR_CAT_USER + 15
	32784,  AEE_ADDR_CAT_USER + 16
	32785,  AEE_ADDR_CAT_USER + 17
	32786,  AEE_ADDR_CAT_USER + 18
	32787,  AEE_ADDR_CAT_USER + 19
	32788,  AEE_ADDR_CAT_USER + 20
	32789,  AEE_ADDR_CAT_USER + 21
	32790,  AEE_ADDR_CAT_USER + 22
	32791,  AEE_ADDR_CAT_USER + 23
	32792,  AEE_ADDR_CAT_USER + 24
	32793,  AEE_ADDR_CAT_USER + 25
	32794,  AEE_ADDR_CAT_USER + 26
	32795,  AEE_ADDR_CAT_USER + 27
	32796,  AEE_ADDR_CAT_USER + 28
	32797,  AEE_ADDR_CAT_USER + 29
	32798,  AEE_ADDR_CAT_USER + 30
	32799,  AEE_ADDR_CAT_USER + 31
	32800,  AEE_ADDR_CAT_USER + 32
	32801,  AEE_ADDR_CAT_USER + 33
	32802,  AEE_ADDR_CAT_USER + 34
	32803,  AEE_ADDR_CAT_USER + 35
	32804,  AEE_ADDR_CAT_USER + 36
	32805,  AEE_ADDR_CAT_USER + 37
	32806,  AEE_ADDR_CAT_USER + 38
	32807,  AEE_ADDR_CAT_USER + 39
	32808,  AEE_ADDR_CAT_USER + 40
	32809,  AEE_ADDR_CAT_USER + 41
	32810,  AEE_ADDR_CAT_USER + 42
	32811,  AEE_ADDR_CAT_USER + 43
	32812,  AEE_ADDR_CAT_USER + 44
	32813,  AEE_ADDR_CAT_USER + 45
	32814,  AEE_ADDR_CAT_USER + 46
	32815,  AEE_ADDR_CAT_USER + 47
	32816,  AEE_ADDR_CAT_USER + 48
	32817,  AEE_ADDR_CAT_USER + 49
	32818,  AEE_ADDR_CAT_USER + 50
}
EnumList, IDEL_ADDR_FIELD
	0,      AEE_ADDRFIELD_NONE         
	1,      AEE_ADDRFIELD_FIRSTNAME    
	2,      AEE_ADDRFIELD_MIDDLENAME   
	3,      AEE_ADDRFIELD_LASTNAME     
	4,      AEE_ADDRFIELD_NICKNAME     
	5,      AEE_ADDRFIELD_NAME         
	6,      AEE_ADDRFIELD_ORG          
	7,      AEE_ADDRFIELD_JOBTITLE     
	8,      AEE_ADDRFIELD_ADDRESS      
	9,      AEE_ADDRFIELD_CITY         
	10,     AEE_ADDRFIELD_STATE        
	11,     AEE_ADDRFIELD_COUNTRY      
	12,     AEE_ADDRFIELD_ZIPCODXE     
	13,     AEE_ADDRFIELD_URL          
	14,     AEE_ADDRFIELD_DEPARTMENT   
	15,     AEE_ADDRFIELD_EMAIL        
	16,     AEE_ADDRFIELD_GENDER       
	17,     AEE_ADDRFIELD_PHONE_WORK   
	18,     AEE_ADDRFIELD_PHONE_HOME   
	19,     AEE_ADDRFIELD_PHONE_FAX    
	20,     AEE_ADDRFIELD_PHONE_VOICE  
	21,     AEE_ADDRFIELD_PHONE_PREF   
	22,     AEE_ADDRFIELD_PHONE_CELL   
	23,     AEE_ADDRFIELD_PHONE_PAGER  
	24,     AEE_ADDRFIELD_PHONE_GENERIC
	25,     AEE_ADDRFIELD_PHONE_OTHER  
	26,     AEE_ADDRFIELD_BDAY         
	27,     AEE_ADDRFIELD_SORTSTRING
	28,     AEE_ADDRFIELD_NOTES
   29,     AEE_ADDRFIELD_SIP_URL
	30,     AEE_ADDRFIELD_DIAL_STRING
	31,     AEE_ADDRFIELD_INDEX
	32,     AEE_ADDRFIELD_VOICETAG
	33,     AEE_ADDRFIELD_RINGTONE
	34,     AEE_ADDRFIELD_SPEED_DIAL
	35,     AEE_ADDRFIELD_NAMESUFFIX
	36,     AEE_ADDRFIELD_NAMEPREFIX
	37,     AEE_ADDRFIELD_OTHERNAME
	38,     AEE_ADDRFIELD_PHOTO
	39,     AEE_ADDRFIELD_PHOTOURL
	40,     AEE_ADDRFIELD_POBOX
	41,     AEE_ADDRFIELD_ADDREXTRA
	42,     AEE_ADDRFIELD_STREET
	43,     AEE_ADDRFIELD_UID
	44,     AEE_ADDRFIELD_PUBLICKEY
	45,     AEE_ADDRFIELD_PUBLICKEYSTRING
	197,    AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE
   28672,  AEE_ADDRFIELD_ADMIN
   28673,  AEE_ADDRFIELD_TIME_STAMP
   28674,  AEE_ADDRFIELD_LAST_CONNECT
   28675,  AEE_ADDRFIELD_CONNECT_COUNT
	32768,  AEE_ADDRFIELD_USER + 0
	32769,  AEE_ADDRFIELD_USER + 1
	32770,  AEE_ADDRFIELD_USER + 2
	32771,  AEE_ADDRFIELD_USER + 3
	32772,  AEE_ADDRFIELD_USER + 4
	32773,  AEE_ADDRFIELD_USER + 5
	32774,  AEE_ADDRFIELD_USER + 6
	32775,  AEE_ADDRFIELD_USER + 7
	32776,  AEE_ADDRFIELD_USER + 8
	32777,  AEE_ADDRFIELD_USER + 9
	32778,  AEE_ADDRFIELD_USER + 10
	32779,  AEE_ADDRFIELD_USER + 11
	32780,  AEE_ADDRFIELD_USER + 12
	32781,  AEE_ADDRFIELD_USER + 13
	32782,  AEE_ADDRFIELD_USER + 14
	32783,  AEE_ADDRFIELD_USER + 15
	32784,  AEE_ADDRFIELD_USER + 16
	32785,  AEE_ADDRFIELD_USER + 17
	32786,  AEE_ADDRFIELD_USER + 18
	32787,  AEE_ADDRFIELD_USER + 19
	32788,  AEE_ADDRFIELD_USER + 20
	32789,  AEE_ADDRFIELD_USER + 21
	32790,  AEE_ADDRFIELD_USER + 22
	32791,  AEE_ADDRFIELD_USER + 23
	32792,  AEE_ADDRFIELD_USER + 24
	32793,  AEE_ADDRFIELD_USER + 25
	32794,  AEE_ADDRFIELD_USER + 26
	32795,  AEE_ADDRFIELD_USER + 27
	32796,  AEE_ADDRFIELD_USER + 28
	32797,  AEE_ADDRFIELD_USER + 29
	32798,  AEE_ADDRFIELD_USER + 30
	32799,  AEE_ADDRFIELD_USER + 31
	32800,  AEE_ADDRFIELD_USER + 32
	32801,  AEE_ADDRFIELD_USER + 33
	32802,  AEE_ADDRFIELD_USER + 34
	32803,  AEE_ADDRFIELD_USER + 35
	32804,  AEE_ADDRFIELD_USER + 36
	32805,  AEE_ADDRFIELD_USER + 37
	32806,  AEE_ADDRFIELD_USER + 38
	32807,  AEE_ADDRFIELD_USER + 39
	32808,  AEE_ADDRFIELD_USER + 40
	32809,  AEE_ADDRFIELD_USER + 41
	32810,  AEE_ADDRFIELD_USER + 42
	32811,  AEE_ADDRFIELD_USER + 43
	32812,  AEE_ADDRFIELD_USER + 44
	32813,  AEE_ADDRFIELD_USER + 45
	32814,  AEE_ADDRFIELD_USER + 46
	32815,  AEE_ADDRFIELD_USER + 47
	32816,  AEE_ADDRFIELD_USER + 48
	32817,  AEE_ADDRFIELD_USER + 49
	32818,  AEE_ADDRFIELD_USER + 50
}

Resource, IDS_DD_MAX_ADDR_COUNT, Address Book
   uint32, Max Entries
   HELP 
	<Definition>
	    Maximum number of entries in the address book. "0" means no limit.
	<Instructions>
      Specify the maximum number of entries in the address book.  Enter
      "0" if there is no limit.
   }
}
Resource, IDS_DD_ADDR_CAT_30, Address Book
   complex, Categories, 1, 0, 0, |
      enum, Category ID, IDEL_ADDR_CAT
      string, Category Name
      enum, Sim Card Required for Testing, IDEL_LABEL_YESNO
      enum, Testing Enabled, IDEL_LABEL_YESNO
      uint32, Minimum number of fields required for an address book entry
      uint32, Number of Autogenerated Fields
   }
   HELP 
	 <Definition>
	    The type of address book categories the device supports. AEE_CAT_NONE
			means categories are not supported
	<Instructions>
      List each category the device supports.  See AEEAddrBook.h and
      the BREW API Reference for more information.
      
      Category ID - Enter the unique AEEAddrCat value for this category
      in terms of the AEE_ADDR_CAT_xxx macros from AEEAddrBook.h.
      OEM-defined categories begin at AEE_ADDR_CAT_USER.
			
			If the device does not support
      categories, enter AEE_ADDR_CAT_NONE.
      
      Category Name - Enter the text name of this category (for example,
      Business, Personal, Friends, etc.).  If the device does not support
      categories, enter "None".

      Sim Card Required for Testing - This entry is used to capture where 
      Address Book entries are stored.  For testing purposes it must be known if
      certain categories require a SIM card to be present to test them.
      
      Testing Enabled - This flag is used to disable testing of certain categories
      
      Minimum number of fields required for an address book entry -This entry is used
      to indicate the minimum number of fields required for an address record
      (of the specified category) to exist.For example, if this value is 1, then for
      any address record to be created in that category,it should have a minimum of one field.
      
      Number of Autogenerated Fields - This entry is used to indicate the number of fields 
      automatically added to an address record. These fields are not specified by the user,
      but are automatically added by the address book implementation.
    
   }
}
Resource, IDS_DD_ADDR_FIELDS_30, Address Book
   complex, Fields, 1, 0, 0, |
      enum, Category, IDEL_ADDR_CAT
      enum, Field ID, IDEL_ADDR_FIELD
      string, Field Name
      uint32, Max Items
      enum, Field Type, IDEL_ADDR_FIELD_TYPE
      enum, Field Search, IDEL_LABEL_YESNO
      enum, Category Search, IDEL_LABEL_YESNO
      enum, Required, IDEL_LABEL_YESNO
      uint32, Max Length
   }
   HELP 
      List each field the device supports, and specify the characteristics
      of each field.  See AEEAddrBook.h and the BREW API Reference
      for more information on the AEEAddrFieldInfo data structure.

      IMPORTANT:  List all fields available in all categories with a category
      of AEE_ADDR_CAT_NONE.
   
      Category - Enter the category in which this field is available.  Most
      fields are available in all categories -- these fields must be
      listed once for each category in which they are available.
      
      Field Name - Enter the text name of the field (for example, "Name", 
      "email", "Work Phone", etc.).

      Field ID - Enter the AEEAddrFieldID for this field.  Predefined field
      IDs are at the beginning of the list.  OEM-defined values are at the end
      of the list. OEM-defined values begin at AEE_ADDRFIELD_USER.
			AEE_ADDR_FIELD_xxx values are defined in AEEAddrBook.h.
      You may need technical assistance from the developer of OEMAddrBook.c
      to determine these values.

      Field Type - Enter the AEEAddrFieldType for this field.

      Max Items - Enter the maximum allowed number of items of this fieldID
      in each record.  For example, if fieldID is AEE_ADDRFIELD_EMAIL, then 
      maxItems could be 2 if OEM address book supports only a maximum of 2 
      email entries per record.

      Field Search - Can a search (EnumRec) be done based on this field?  That
      is, can we make a call to IADDRBOOK_EnumRecInit() to search for all
      records having a specific value for this given field ID?

      Category Search - Can a search (EnumRec) be done based on this field
      within a category?  That is, can we use IADDRBOOK_EnumRecInit() to
      search for all records of a specific category having a specific value for
      this field?

      Required - Is this field required to be present in all records?

      Max Length - Enter the maximum length of this field in units of the
      AEEAddrFieldType.  For example, if this is a string field that can hold
      a maximum of 50 AECHARs, enter 50 bytes, not 100.
   }
}
Resource, IDS_DD_ADDR_USER_DEF_CAT, Address Book,Developer|OEM
   enum, App Category Definition, IDEL_YESNO
   HELP 
	<Definition>
	    Can a BREW application create a record with an AEEAddrCat that is not
      one of the categories returned by IADDRBOOK_EnumNextCategory()?
	<Instructions>
      Can a BREW application create a record with an AEEAddrCat that is not
      one of the categories returned by IADDRBOOK_EnumNextCategory()?  

      Select "Yes" if applications can create records with their own unique 
      categories.  Select "No" if applications are restricted to the categories
      defined by the OEM.
   }
}
Resource, IDS_DD_ADDR_USER_DEF_FIELD, Address Book,Developer|OEM
   enum, App Field Definition, IDEL_YESNO
   HELP 
     <Definition>
      Can a BREW application create an address book field with an AEEAddrFieldID
      that is not one of the field IDs returned by 
      IADDRBOOK_EnumNextFieldsInfo()?
	<Instructions>
      Can a BREW application create an address book field with an AEEAddrFieldID
      that is not one of the field IDs returned by 
      IADDRBOOK_EnumNextFieldsInfo()?  

      Enter "Yes" if applications can create their own field IDs.  Select "No" if applications are restricted to the field
      IDs defined by the OEM.
   }
}
Resource, IDS_DD_ADDR_REMOVE_ALL_SUPPORTED, Address Book,Carrier|OEM

   enum, RemoveAllRecs Support, IDEL_YESNO
   HELP 
     If the device supports the IADDRBOOK_RemoveAllRecs() method, select "Yes".
     Otherwise, select "No".
   }
}
Resource, IDS_DD_ADDRBOOK_SETCATEGORY, Address Book
   enum, Category change, IDEL_LABEL_YESNO
   HELP
      If the device supports changing the category of existing records, select "Yes".  
      Otherwise, select "No".
   }
}

#<!--=================================================================
#        AddrBook RUIM
#=================================================================-->

Resource, IDS_DD_ADDR_RUIM_SUPPORTED, Address Book - RUIM
   enum, RUIM Support, IDEL_YESNO
   HELP 
      Does the device support an RUIM-based address book?

      (To support an RUIM address book, the device must implement
      AEECLSID_ADDRBOOK_RUIM.)
   }
}

Resource, IDS_DD_MAX_ADDR_COUNT_RUIM, Address Book - RUIM
   uint32, Max Entries
   HELP 
     <Definition>
	  Maximum number of entries in the RUIM address book."0" means no limit.
	<Instructions>
      Specify the maximum number of entries in the RUIM address book.  Enter
      "0" if there is no limit.
   }
}
Resource, IDS_DD_ADDR_CAT_30_RUIM, Address Book - RUIM
   complex, Categories, 1, 0, 0, |
      enum, Category ID, IDEL_ADDR_CAT
      string, Category Name
      enum, Sim Card Required for Testing, IDEL_LABEL_YESNO
      enum, Testing Enabled, IDEL_LABEL_YESNO
      uint32, Minimum number of fields required for an address book entry
      uint32, Number of Autogenerated Fields
   }
   HELP 
     <Definition>
	  Each category that the RUIM address book supports.  See AEEAddrBook.h and
      the BREW API Reference for more information.
      
	  AEE_ADDR_CAT_NONE mean categories not supported.
	<Instructions>
      List each category that the RUIM address book supports.  See AEEAddrBook.h and
      the BREW API Reference for more information.
      
      Category ID - Enter the unique AEEAddrCat value for this category
      in terms of the AEE_ADDR_CAT_xxx macros from AEEAddrBook.h.
      OEM-defined categories begin at AEE_ADDR_CAT_USER.
			
      If the device does not support categories, enter AEE_ADDR_CAT_NONE.
      
      Category Name - Enter the text name of this category (for example,
      Business, Personal, Friends, etc.).  If the device does not support
      categories, enter "None".

      Sim Card Required for Testing - This entry is used to capture where 
      Address Book entries are stored.  For testing purposes it must be known if
      certain categories require a SIM card to be present to test them.
      
      Testing Enabled - This flag is used to disable testing of certain categories
      
      Minimum number of fields required for an address book entry -This entry is used
      to indicate the minimum number of fields required for an address record
      (of the specified category) to exist.For example, if this value is 1, then for
      any address record to be created in that category,it should have a minimum of one field.
      
      Number of Autogenerated Fields - This entry is used to indicate the number of fields 
      automatically added to an address record. These fields are not specified by the user,
      but are automatically added by the address book implementation.
   }
}
Resource, IDS_DD_ADDR_FIELDS_30_RUIM, Address Book - RUIM
   complex, Fields, 1, 0, 0, |
      enum, Category, IDEL_ADDR_CAT
      enum, Field ID, IDEL_ADDR_FIELD
      string, Field Name
      uint32, Max Items
      enum, Field Type, IDEL_ADDR_FIELD_TYPE
      enum, Field Search, IDEL_LABEL_YESNO
      enum, Category Search, IDEL_LABEL_YESNO
      enum, Required, IDEL_LABEL_YESNO
      uint32, Max Length
   }
   HELP 
      List each field the RUIM address book supports, and specify the characteristics
      of each field.  See AEEAddrBook.h and the BREW API Reference
      for more information on the AEEAddrFieldInfo data structure.

      IMPORTANT:  List all fields available in all categories with a category
      of AEE_ADDR_CAT_NONE.
   
      Category - Enter the category this field is available in.  Most
      fields are available in all categories -- these fields must be
      listed once for each category in which they are available.
      
      Field Name - Enter the text name of the field (for example, "Name", 
      "email", "Work Phone", etc.).

      Field ID - Enter the AEEAddrFieldID for this field.  Predefined field
      IDs are at the beginning of the list.  OEM-defined values are at the end
      of the list.  For BREW 2.1, the OEM-defined values begin at
      AEE_ADDRFIELD_USER_DEFINED.  For BREW 3.0 and later, the OEM-defined
      values begin at AEE_ADDRFIELD_USER (AEE_ADDRFIELD_USER_DEFINED is
      deprecated).  AEE_ADDR_FIELD_xxx values are defined in AEEAddrBook.h.
      You may need technical assistance from the developer of OEMAddrBook.c
      to determine these values.

      Field Type - Enter the AEEAddrFieldType for this field.

      Max Items - Enter the maximum allowed number of items of this fieldID
      in each record.  For example, if fieldID is AEE_ADDRFIELD_EMAIL, then 
      maxItems could be 2 if OEM address book supports only a maximum of 2 
      email entries per record.

      Field Search - Can a search (EnumRec) be done based on this field?  That
      is, can we make a call to IADDRBOOK_EnumRecInit() to search for all
      records having a specific value for this given field ID?

      Category Search - Can a search (EnumRec) be done based on this field
      within a category?  That is, can we use IADDRBOOK_EnumRecInit() to
      search for all records of a specific category having a specific value for
      this field?

      Required - Is this field required to be present in all records?

      Max Length - Enter the maximum length of this field in units of the
      AEEAddrFieldType.  For example, if this is a string field that can hold
      a maximum of 50 AECHARs, enter 50 bytes, not 100. 
   }
}
Resource, IDS_DD_ADDR_USER_DEF_CAT_RUIM, Address Book - RUIM
   enum, App Category Definition, IDEL_YESNO
   HELP 
     <Definition>
	  Can a BREW application create a record in the RUIM-based address book
      with an AEEAddrCat that is not one of the categories returned by
      IADDRBOOK_EnumNextCategory()?
	<Instructions>
      Can a BREW application create a record in the RUIM-based address book
      with an AEEAddrCat that is not one of the categories returned by
      IADDRBOOK_EnumNextCategory()?  Select "Yes" if applications can create records 
      with their own unique categories.  Select "No" if applications are restricted to 
      the categories defined by the OEM.
   }
}
Resource, IDS_DD_ADDR_USER_DEF_FIELD_RUIM, Address Book - RUIM
   enum, App Field Definition, IDEL_YESNO
   HELP 
     <Definition>
	  Can a BREW application create an address book field in the RUIM-based
      address book with an AEEAddrFieldID that is not one of the field IDs returned by 
      IADDRBOOK_EnumNextFieldsInfo()?
	<Instructions>
      Can a BREW application create an address book field in the RUIM-based
      address book with an AEEAddrFieldID that is not one of the field IDs returned by 
      IADDRBOOK_EnumNextFieldsInfo()?  Select "Yes" if applications can create 
      their own field IDs.  Select "No" if applications are restricted to the field
      IDs defined by the OEM.
   }
}
Resource, IDS_DD_ADDR_REMOVE_ALL_SUPPORTED_RUIM, Address Book - RUIM,OEM
   enum, RemoveAllRecs Support, IDEL_YESNO
   HELP 
      Does the RUIM-based address book support the IADDRBOOK_RemoveAllRecs() method?
   }
}
#<!--=================================================================
#        Camera
#=================================================================-->

Resource, IDS_DD_CAMERA_SUPP, Camera Services
   enum, Camera Support, IDEL_LABEL_YESNO
   HELP 
      Is the ICamera interface implemented on this device?
   }
}

Resource, IDS_DD_CAMERA_BREW_PIC_PATH, Camera Services
   string, Picture Path
   HELP
      BREW location where native camera saves pictures, for example "fs:/cam/pics".
      Enter "unsupported" if camera does not exist or camera does not support BREW.
   }
}

Resource, IDS_DD_CAMERA_DAM_SUPP, Camera Services
   enum, Direct Access Mode Support, IDEL_LABEL_YESNO
   HELP 
      Does the Camera Support Direct Access Mode ?
   }
}

Resource, IDS_DD_CAMERA_RES, Camera Services
   string, Resolution
   HELP 
     <Definition>
	  Resolution size in pixels
	<Instructions>
      Enter the resolution in pixels.  In the Comment field, enter all supported sizes of the
      encoded picture in pixels, for example, 1280x960.
      
   }
}
Resource, IDS_DD_CAMERA_MIME_TYPES, Camera Services,Developer|OEM
   string, Camera MIME Types, , |
   HELP 
     <Definition>
	  Camera MIME Types supported
	<Instructions>
      Enter a comma separated list of all applicable MIME types for camera 
      services available to BREW (for example, image/jpg).
   }
}
Resource, IDS_DD_CAMERA_FORMATS, Camera Services
   string, Camera Formats, , |
   HELP 
	<Instructions>
      Enter a comma separated list of supported camera formats (for
      example, "bmp,png").
   }
}
Resource, IDS_DD_CAMERA_MCF, Camera Services,
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible MCF directories where camera files can be stored.
   }
}
Resource, IDS_DD_CAMERA_RESTRICTIONS, Camera Services
   string, Restrictions
	HELP 
	<Definition>
	   Camera restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			bitmap depth, number of files, etc.
   }
}
Resource, IDS_DD_CAMERA_DELETE, Camera Services
   enum, Delete Camera files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete camera images in the MCF directories from the native UI?
   }
}
EnumList, CAMERA_FEATURE_LIST
5000,None
5001,Snapshot
5002,Movie
5003,Movie Postcard
5004,Movie PauseResume
5005,Preview PauseResume
5006,Postcard PauseResume
5007,MovieRecToBuffer
1,CAM_PARM_MEDIA_DATA
2,CAM_PARM_VIDEO_ENCODE
3,CAM_PARM_AUDIO_ENCODE
4,CAM_PARM_SIZE
5,CAM_PARM_DISPLAY_SIZE
6,CAM_PARM_DEFER_ENCODE
7,CAM_PARM_MODE
8,CAM_PARM_IS_SUPPORT
9,CAM_PARM_IS_MOVIE
10,CAM_PARM_PIXEL_COUNT
11,CAM_PARM_VIDEO_ENCODE_LIST
12,CAM_PARM_AUDIO_ENCODE_LIST
13,CAM_PARM_SIZE_LIST
14,CAM_PARM_DISPLAY_SIZE_LIST
15,CAM_PARM_FPS_LIST
16,CAM_PARM_OVERLAY
17,CAM_PARM_GPSINFO
18,CAM_PARM_EXIF_IFDTAG
19,CAM_PARM_LCD_DIRECT_ACCESS
20,CAM_PARM_PREVIEW_TYPE
21,CAM_PARM_THUMBNAIL_INFO
22,CAM_PARM_SENSOR_INFO
23,CAM_PARM_PREVIEW_FPS
24,CAM_PARM_FRAME_TIMESTAMP
257,CAM_PARM_QUALITY
258,CAM_PARM_FPS
259,CAM_PARM_CONTRAST
260,CAM_PARM_BRIGHTNESS
261,CAM_PARM_SHARPNESS
262,CAM_PARM_ZOOM
263,CAM_PARM_ROTATE_PREVIEW
264,CAM_PARM_ROTATE_ENCODE
265,CAM_PARM_EXPOSURE
266,CAM_PARM_WHITE_BALANCE
267,CAM_PARM_EFFECT
268,CAM_PARM_FLASH
269,CAM_PARM_RED_EYE_REDUCTION
270,CAM_PARM_REFLECT
271,CAM_PARM_FILE_SIZE
272,CAM_PARM_NIGHTSHOT_MODE
273,CAM_PARM_ANTIBANDING
274,CAM_PARM_FOCUS
275,CAM_PARM_FOCUS_RECT
276,CAM_PARM_BITRATE
277,CAM_PARM_FADE
378,CAM_PARM_EXPOSURE_METERING
379,CAM_PARM_EXPOSURE_COMP
380,CAM_PARM_ISO
381,CAM_PARM_APERTURE
382,CAM_PARM_SHUTTER_SPEED
383,CAM_PARM_DEBLUR
384,CAM_PARM_LUMA_ADAPTATION
385,CAM_PARM_FLASH_CTL
386,CAM_PARM_AF_INFO
387,CAM_PARM_BESTSHOT
}
Resource, IDS_AEECLSID_CAMERA, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, CAMERA Features
      enum, Feature, CAMERA_FEATURE_LIST
   }
   HELP 
If the device supports the ICamera camera 1 interface, select "Yes" for Supported. Otherwise, select "No". 

If supported, create and select each supported feature for this interface.
   }
}
Resource, IDS_AEECLSID_CAMERA2, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, CAMERA2 Features
      enum, Feature, CAMERA_FEATURE_LIST
   }
   HELP 
If the device supports the ICamera camera 2 interface, select "Yes" for Supported. Otherwise, select "No". 

If supported, create and select each supported feature for this interface.

   }
}
Resource, IDS_AEECLSID_CAMERA3, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, CAMERA3 Features
      enum, Feature, CAMERA_FEATURE_LIST
   }
   HELP 
If the device supports the ICamera camera 3 interface, select "Yes" for Supported. Otherwise, select "No". 

If supported, create and select each supported feature for this interface.

   }
}
Resource, IDS_AEECLSID_CAMERA4, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, CAMERA4 Features
      enum, Feature, CAMERA_FEATURE_LIST
   }
   HELP 
If the device supports the ICamera camera 4 interface, select "Yes" for Supported. Otherwise, select "No". 

If supported, create and select each supported feature for this interface.

   }
}

#<!--=================================================================
#        Image And Other Media Interaction
#=================================================================-->
Resource, IDS_DD_MEDIA_WALLPAPER_MIME_TYPES, Wallpaper Services,Developer|OEM
   string, Wallpaper MIME Types, , |
   HELP 
     <Definition>
	  Wallpaper MIME Types supported
	<Instructions>
      Enter a comma separated list of all applicable MIME types for Wallpaper 
			services available to BREW (for example, image/jpg).
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_FORMATS, Wallpaper Services
   string, Wallpaper Formats, , |
   HELP 
	<Instructions>
      Enter a comma separated list of supported wallpaper formats (for
      example, "bmp,png").
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_MCF, Wallpaper Services
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible MCF directories where wallpaper files can be stored.
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_RESTRICTIONS, Wallpaper Services
   string, Restrictions
	HELP 
	<Definition>
	   Wallpaper restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			bitmap depth, number of files, etc.
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_DELETE, Wallpaper Services
   enum, Delete Wallpaper files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete wallpaper images in the MCF directories from the native UI?
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_IMG_X, Wallpaper Services
   uint32, Wallpaper Size X, 
   HELP 
     <Definition>
	  Recommended width for wallpapers
	<Instructions>
      Enter the recommended width of images used for wallpaper.  Document
      any minimum or maximum width limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_WALLPAPER_IMG_Y, Wallpaper Services
   uint32, Wallpaper Size Y
   HELP 
     <Definition>
	  Recommended height for wallpapers
	<Instructions>
      Enter the recommended height of images used for wallpaper.  Document
      any minimum or maximum height limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_MIME_TYPES, Caller ID Services,Developer|OEM
   string, CallerID MIME Type
   HELP 
     <Definition>
	  Caller ID MIME types supported
	<Instructions>
      Enter a comma separated of all applicable MIME types for Caller ID services available to BREW (for example, image/bmp).
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_FORMATS, Caller ID Services
   string, CallerID Formats, , |
   HELP 
	<Instructions>
      Enter a comma separated list of supported image formats for CallerID (for example, "bmp,png").
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_MCF, Caller ID Services
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible MCF directories where Caller ID files can be stored. 
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_RESTRICTIONS, Caller ID Services
   string, Restrictions
	HELP 
	<Definition>
	    Caller ID media restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			bitmap depth, number of files, etc.
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_DELETE, Caller ID Services
   enum, Delete Caller ID files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete Caller ID images in the MCF directories from the native UI?
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_IMG_X, Caller ID Services
   uint32, CallerID Image Size X
   HELP 
     <Definition>
	  Recommended width of images used for CallerID
	<Instructions>
      Enter the recommended width of images used for CallerID images.  Document
      any minimum or maximum width limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_CALLER_ID_IMG_Y, Caller ID Services
   uint32, CallerID Image Size Y
   HELP 
     <Definition>
	  Recommended height of images used for CallerID
	<Instructions>
      Enter the recommended height of images used for CallerID images.  Document
      any minimum or maximum height limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_SSAVER_MIME_TYPES, Native Screen Saver Services,Developer|OEM
   string, Screen Saver MIME Types, , |
   HELP 
     <Definition>
	  Screen saver MIME Types supported
	<Instructions>
      Enter a comma separated of all applicable MIME types for native screen savers that are available to BREW (for example, image/png).
   }
}
Resource, IDS_DD_MEDIA_SSAVER_FORMATS, Native Screen Saver Services
   string, ScreenSaver Formats, , |
   HELP
	<Instructions>
      Enter a comma separated list of supported screen saver formats (for
      example, "bmp,png,pmd").
   }
}
Resource, IDS_DD_MEDIA_SSAVER_MCF, Native Screen Saver Services
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible MCF directories where native screen saver files can be stored.
   }
}
Resource, IDS_DD_MEDIA_SSAVER_RESTRICTIONS, Native Screen Saver Services
   string, Restrictions
	HELP 
	<Definition>
	    Screensaver Media restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			bitmap depth, number of files, etc.
   }
}
Resource, IDS_DD_MEDIA_SSAVER_DELETE, Native Screen Saver Services
   enum, Delete Native Screen Saver files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete native screen saver images in the MCF directories from the native UI?
   }
}
Resource, IDS_DD_MEDIA_SSAVER_IMG_X, Native Screen Saver Services
   uint32, ScreenSaver Size X
   HELP 
     <Definition>
	  Recommended width of images used for screen savers
	<Instructions>
      Enter the recommended width of images used for screen saver images.  Document
      any minimum or maximum width limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_SSAVER_IMG_Y, Native Screen Saver Services
   uint32, ScreenSaver Size Y
   HELP 
     <Definition>
	  Recommended height of images used for screen savers
	<Instructions>
      Enter the recommended height of images used for screen saver images.  Document
      any minimum or maximum height limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_MIME_TYPES, Power ON/OFF Services,Developer|OEM
   string, Power On/Off MIME Types
   HELP 
     <Definition>
     Power ON/OFF Service MIME types supported
	<Instructions>
      Enter a comma separated of all applicable MIME types for Power ON/OFF services available to BREW (for example, bmp).
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_FORMATS, Power ON/OFF Services
   string, Power On/Off Formats, , |
   HELP 
	<Instructions>
      Enter a comma separated list of supported Power On/Off formats (for
      example, "bmp,png,pmd").
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_MCF, Power ON/OFF Services
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible directories where Power On/Off files can be stored.
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_RESTRICTIONS, Power ON/OFF Services
   string, Restrictions
	HELP 
	<Definition>
	    Power ON/OFF Media Restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			bitmap depth, number of files, etc.
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_DELETE, Power ON/OFF Services
   enum, Delete Power ON/OFF files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete Power On/Off images in the MCF directories from the native UI?
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_IMG_X, Power ON/OFF Services
   uint32, Power On/Off Size X
   HELP 
     <Definition>
	  Recommended width of images used for Power On/Off.
	<Instructions>
      Enter the recommended width of images used for Power On/Off images.  Document
      any minimum or maximum width limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_POWERONOFF_IMG_Y, Power ON/OFF Services
   uint32, Power On/Off Size Y
   HELP 
     <Definition>
	  Recommended height of images used for Power On/Off.
	<Instructions>
      Enter the recommended height of images used for Power On/Off images.  Document
      any minimum or maximum height limitations in the Comment field.
   }
}
Resource, IDS_DD_MEDIA_AUDIO_MIME_TYPES, Audio Services,Developer|OEM
   string, Audio MIME Types
   HELP 
     <Definition>
	  Audio Services MIME Types supported
	<Instructions>
      Enter a comma separated list of all applicable MIME types for Audio services available to BREW (for example, audio/mp3).
   }
}
Resource, IDS_DD_MEDIA_AUDIO_FORMATS, Audio Services
   string, Audio Formats, , |
   HELP 
	<Instructions>
      Enter a comma separated list of supported Audio formats (for
      example, "mid,aac,mp3").
   }
}
Resource, IDS_DD_MEDIA_AUDIO_MCF, Audio Services
   enum, Pictures, IDEL_LABEL_YESNO
	enum, Music, IDEL_LABEL_YESNO
	enum, Movie, IDEL_LABEL_YESNO
	enum, Theme, IDEL_LABEL_YESNO
	enum, Text, IDEL_LABEL_YESNO
	enum, Sound, IDEL_LABEL_YESNO
	enum, Link, IDEL_LABEL_YESNO
	enum, Pictures Locked, IDEL_LABEL_YESNO
	enum, Music Locked, IDEL_LABEL_YESNO
	enum, Movie Locked, IDEL_LABEL_YESNO
	enum, Theme Locked, IDEL_LABEL_YESNO
	enum, Text Locked, IDEL_LABEL_YESNO
	enum, Sound Locked, IDEL_LABEL_YESNO
	enum, Link Locked, IDEL_LABEL_YESNO
	enum, License Locked, IDEL_LABEL_YESNO
	HELP 
	    Select "Yes" for all the possible directories where Audio files can be stored.
   }
}
Resource, IDS_DD_MEDIA_AUDIO_RESTRICTIONS, Audio Services
   string, Restrictions
	HELP 
	<Definition>
	    Audio Services media restrictions
	<Instructions>
	    List any restrictions associated with the file types in this category -- size, 
			number of files, etc.
   }
}
Resource, IDS_DD_MEDIA_AUDIO_DELETE, Audio Services
   enum, Delete Audio files from Native UI, IDEL_LABEL_YESNO
	HELP 
	    Is it possible to delete Audio files in the MCF directories from the native UI?
   }
}

#<!--=================================================================
#       BREW Screen Saver
#=================================================================-->
EnumList, IDEL_SSAVER_TYPE
   BREW Only, BREW Only
   Native Only, Native Only
   BREW and Native, BREW and Native
}
EnumList, IDEL_SSAVER_BACKLIGHT
   BREW, BREW
   Native UI, Native UI
   N/A, N/A
}
Resource, IDS_DD_SSAVER_TYPE, BREW Screen Saver
   enum, Screen Saver Type, IDEL_SSAVER_TYPE
   HELP 
     <Definition>
	Screen saver types supported
	<Instructions>
       Enter whether the device supports BREW-only, Native UI-only, or both BREW
      and Native UI screen savers.
   }
}
Resource, IDS_DD_SSAVER_TIMER, BREW Screen Saver
   uint32, Timer
   HELP 
     Enter the default number in seconds of inactivity after which the screen saver be activated.
   }
}

#<!--=================================================================
#        Call History
#=================================================================-->

EnumList, IDEL_CALL_HISTORY_FIELDS
   none, None
   1, AEECALLHISTORY_FIELD_CALL_TYPE
   2, AEECALLHISTORY_FIELD_NUMBER_TYPE
   3, AEECALLHISTORY_FIELD_NUMBER_PLAN
   4, AEECALLHISTORY_FIELD_DATE_TIME
   5, AEECALLHISTORY_FIELD_CALL_DURATION
   6, AEECALLHISTORY_FIELD_NUMBER
   7, AEECALLHISTORY_FIELD_NAME
   8, AEECALLHISTORY_FIELD_HANDLER_CLSID
   9, AEECALLHISTORY_FIELD_NUMBER_EX
   10, AEECALLHISTORY_FIELD_PI
   11, AEECALLHISTORY_FIELD_ID
   12, AEECALLHISTORY_FIELD_CALLACTIVE
   13, AEECALLHISTORY_FIELD_DURATIONKNOWN
   14, AEECALLHISTORY_FIELD_NOTVIEWED
   15, AEECALLHISTORY_FIELD_TECH_TYPE
}

EnumList, IDEL_CALL_HISTORY_CALL_TYPES
   none, None
   1, AEECALLHISTORY_CALL_TYPE_TO
   2, AEECALLHISTORY_CALL_TYPE_FROM
   3, AEECALLHISTORY_CALL_TYPE_MISSED
}

EnumList, IDEL_CALL_HISTORY_NUMBER_TYPES
   none, None
   1, AEECALLHISTORY_NUMBER_TYPE_INTERNATIONAL
   2, AEECALLHISTORY_NUMBER_TYPE_NATIONAL
   3, AEECALLHISTORY_NUMBER_TYPE_NETWORK
   4, AEECALLHISTORY_NUMBER_TYPE_SUBSCRIBER
   5, AEECALLHISTORY_NUMBER_TYPE_ABBREVIATED
   6, AEECALLHISTORY_NUMBER_TYPE_QCHAT
}

EnumList, IDEL_CALL_HISTORY_PLAN_TYPES
   none, None
   1, AEECALLHISTORY_NUMBER_PLAN_TELEPHONY
   2, AEECALLHISTORY_NUMBER_PLAN_DATA
   3, AEECALLHISTORY_NUMBER_PLAN_TELEX
   4, AEECALLHISTORY_NUMBER_PLAN_PRIVATE
}

Resource, IDS_DD_CH_SORTED_BY_TYPE, Call History
   enum, Sorted By Type, IDEL_YESNO
   HELP 
      Are call history records sorted into incoming, outgoing, and/or
      missed categories?
   }
}
Resource, IDS_DD_CH_MAX_ENTRIES, Call History
   uint32, Max Entries
   HELP 
      Enter the maximum number of entries stored in call history.  If entries
      are sorted by type, this is the maximum number of entries per type.
   }
}
Resource, IDS_DD_CH_REQUIRED_FIELDS, Call History
   complex, Required Fields, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_FIELDS
   }
   HELP 
	 <Definition>
	    Required fields for call history entries.
	<Instructions>
      List any fields that must be present in a call history entry.
   }
}
Resource, IDS_DD_CH_UNSUPPORTED_FIELDS, Call History
   complex, Unsupported Fields, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_FIELDS
   }
   HELP 
	 	 <Instructions>
      List any fields that are not stored by device's call history
      implementation.
   }
}
Resource, IDS_DD_CH_ADDITIONAL_FIELDS, Call History
   complex, Additional Fields, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_FIELDS
   }
   HELP 
	 <Definition>
	   Fields the device adds to a call history entry
     For example, if ICallHistory automatically adds a timestamp field
      to each entry.

	<Instructions>
      List any fields that the device adds to a call history entry
      in addition to those fields passed to ICALLHISTORY_AddEntry().  For
      example, if ICallHistory automatically adds a timestamp field
      to each entry.
   }
}
Resource, IDS_DD_CH_SUPPORTS_USRDEF_FIELDS, Call History
   enum, User Fields, IDEL_YESNO
   HELP 
      Are user-defined field types supported?
   }
}
Resource, IDS_DD_CH_UPDATE_ENTRY_SUPPORTED, Call History
   enum, Update Entry Supported, IDEL_YESNO
   HELP 
      Are call history entries allowed to be modified by BREW?
   }
}
Resource, IDS_DD_CH_NOTIFY_SUPPORTED, Call History
   enum, Notify Supported, IDEL_YESNO
   HELP 
      Does the device support the capability to notify BREW when call history entries are changed?
   }
}
Resource, IDS_DD_CH_UNSUPPORTED_CALL_TYPES, Call History
   complex, Unsupported Call Types, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_CALL_TYPES
   }
   HELP 
      List any call types that are not supported.
   }
}
Resource, IDS_DD_CH_UNSUPPORTED_NUMBER_TYPES, Call History
   complex, Unsupported Number Types, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_NUMBER_TYPES
   }
   HELP 
      List any number types that are not supported.
   }
}
Resource, IDS_DD_CH_UNSUPPORTED_NUMBER_PLANS, Call History
   complex, Unsupported Plan Types, 0, , ,
      enum, Field ID, IDEL_CALL_HISTORY_PLAN_TYPES
   }
   HELP 
      List any plan types that are not supported.
   }
}
Resource, IDS_DD_CH_DUP_PHONE_NUMS_ALLOWED, Call History
   enum, Duplicate Entries Allowed, IDEL_YESNO
   HELP 
      Are duplicate call history entries allowed?
   }
}
Resource, IDS_DD_CALLHISTORY_IN_MISS_SUPPORTED, Call History
   enum, Incoming and Missed calls, IDEL_LABEL_YESNO
   HELP
      If the device supports storing Incoming and Missed calls in the same category, select "Yes".
      Otherwise, select "No".
   }
}
#<!--=================================================================
#        Additional Socket Parameters
#        ID Range: 280..299
#=================================================================-->
Resource, IDS_DATA_SERVER_SOCKETS, Network/Socket Specification,Developer|OEM
   enum, Server Sockets, IDEL_LABEL_YESNO
   HELP 
      Does the device support server sockets?

      Server socket support means that ISOCKET_Listen() and 
      ISOCKET_Accept() are supported.
   }
}
Resource, IDS_DATA_SHUTDOWN, Network/Socket Specification,Developer|OEM
   enum, Shutdown, IDEL_LABEL_YESNO
   HELP 
      Does the device support TCP socket shutdown (ISOCKET_Shutdown())?
   }
}
Resource, IDS_DATA_LOOPBACK, Network/Socket Specification,Developer|OEM
   enum, Loopback, IDEL_LABEL_YESNO
   HELP 
      Does the device support a loopback interface (for example,
      127.0.0.1)?
   }
}
Resource, IDS_DATA_UDP_PAYLOAD_MAX, Network/Socket Specification,Developer|OEM
   uint32, Max UDP Payload
   HELP 
     <Definition>
	  Maximum payload size in bytes for UDP datagrams.
	<Instructions>
      Enter the maximum payload size in bytes for UDP datagrams
      sent via ISocket.
   }
}
Resource, IDS_DATA_MAX_SIMULT_PDPS, Network/Socket Specification,Developer|OEM
   uint32, Max Simultaneous PDPs
   HELP 
     <Definition>     
	  Maximum number of simultaneous active connections, each one using a different Packet Data Profile.
	<Instructions>
      Enter the maximum number of simultanous PDPs.
   }
}
Resource, IDS_DD_PDP_INFO,  Network/Socket Specification

    complex, PDP Info, 1,0,0,| 
	    enum, Profile Validity, IDEL_LABEL_YESNO
	    string, PDP Profile Name
    }
   HELP
      PDP info as in the structure AEEPDPInfo. 

      
      NOTE: the above <validity, Name> pair will repeat for each pdp profile.
   }
}
#<!--=================================================================
#        MIF Image Info
#        ID Range: 300..319
#=================================================================-->
Resource, IDS_MIF_IMAGE_SMALL_X, MIF Information,Developer|OEM
   uint32, MIF Small Image Width
   HELP 
     <Definition>
	  Maximum width (x) of the Small Image in a MIF file.
	<Instructions>
      Enter the maximum width (x) of the Small Image in a MIF file.

      This is typically the APP_THUMB_X_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for all images in List View and the bottom icons in Large Icon View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}
Resource, IDS_MIF_IMAGE_SMALL_Y, MIF Information,Developer|OEM
   uint32, MIF Small Image Height
   HELP 
     <Definition>
	  Maximum height (y) of the Small Image in a MIF file.
	<Instructions>
      Enter the maximum height (y) of the Small Image in a MIF file.

      This is typically the APP_THUMB_Y_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for all images in List View and the bottom icons in Large Icon View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}
Resource, IDS_MIF_IMAGE_MEDIUM_X, MIF Information,Developer|OEM
   uint32, MIF Medium Image Width
   HELP 
     <Definition>
	  Maximum width (x) of the Medium Image in a MIF file.
	<Instructions>
      Enter the maximum width (x) of the Medium Image in a MIF file.

      This is typically the APP_ICON_X_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for all images in Grid View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}
Resource, IDS_MIF_IMAGE_MEDIUM_Y, MIF Information,Developer|OEM
   uint32, MIF Medium Image Height
   HELP 
     <Definition>
	  Maximum height (y) of the Medium Image in a MIF file.
	<Instructions>
      Enter the maximum height (y) of the Medium Image in a MIF file.

      This is typically the APP_ICON_Y_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for all images in Grid View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}
Resource, IDS_MIF_IMAGE_LARGE_X, MIF Information,Developer|OEM
   uint32, MIF Large Image Width
   HELP 
     <Definition>
	  Maximum width (x) of the Large Image in a MIF file.
	<Instructions>
      Enter the maximum width (x) of the Large Image in a MIF file.

      This is typically the APP_IMAGE_X_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for the large centered image in Large Icon View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}
Resource, IDS_MIF_IMAGE_LARGE_Y, MIF Information,Developer|OEM
   uint32, MIF Large Image Height
   HELP 
     <Definition>
	  Maximum height (y) of the Large Image in a MIF file.
	<Instructions>
      Enter the maximum height (y) of the Large Image in a MIF file.

      This is typically the APP_IMAGE_Y_THRESHOLD value from
      AppMgrSettings.h.  This is the image size used by the BREW Application
      Manager for the large centered image in Large Icon View.

      In the MIF Editor 3.0, this image is specified on the Applets tab.
   }
}

#<!--_______________________________________
#                Flips 
#_______________________________________-->
EnumList, IDEL_FLIP_ID
0,First Flip
1,Second Flip
2,Third Flip
3,Fourth Flip
4,Fifth Flip
}
EnumList, IDEL_FLIP_POSITION_TYPE
0,Position is in angles
1,Position is linear
}
EnumList, IDEL_FLIP_PARM
1,FLIP_PARM_IS_SUPPORT
2,FLIP_PARM_DISPLAYLIST
3,FLIP_PARM_KEYLIST
}
EnumList, IDEL_FLIP_CAPABILITIES
1,Flip
2,KeyGuard
16,ScreenRotate

HE <Definition>
<Instructions>
   This enum list specifies the type of device notifier events a flip
   can generate.
}
Resource, IDS_DD_FLIP_INFO, Flip,Developer|OEM
   uint32,Number Flips,0,255,0
   complex,Flip Info,1,255,1,|
      enum,Flip ID,IDEL_FLIP_ID
      enum,Flip Position Type,IDEL_FLIP_POSITION_TYPE
      uint32,Flip Capability Mask
   }
   HELP 
      Enter the Number of flips on the device.
      For each flip enter ther following:
          Enter the ID of the flip in FlipID.
          Enter the position type of the flip in FlipPosType.
          Enter a bit-wise OR'ed mask of the flip capabilities in Flip Capability Mask.
          The capabilities and values for the mask are:      
      1,Flip
      2,KeyGuard
      16,ScreenRotate   
   }
}
Resource, IDS_DD_FLIP_POS_MIN_MAX, Flip,Developer|OEM
   complex,Flip Min Max Positions,1,0,0,|
      enum,FlipID,IDEL_FLIP_ID
      uint32,Min X
      uint32,Min Y
      uint32,Min Z
      uint32,Max X
      uint32,Max Y
      uint32,Max Z
   } 
   HELP 
      Minimum and Maximum positions of each flip on the
      X,Y,Z axes.
      For linear flips, units are OEM defined.
      For angular flips, each unit represents two degrees.
   }
}
Resource, IDS_DD_FLIP_COMMON_PARM_LIST, Flip Common Parameter List,Developer|OEM
   complex,ParmSupported
      enum,ParmID,IDEL_FLIP_PARM
   }
   HELP 
      Lists the parameters that are not specific to a flip.              
   }
}
Resource, IDS_DD_FLIP_1_OPEN_KEY_LIST, Flip 1 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      List the key codes of accessible keys on the device when the first flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_2_OPEN_KEY_LIST, Flip 2 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      List the key codes of accessible keys on the device when the second flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_3_OPEN_KEY_LIST, Flip 3 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      List the key codes of accessible keys on the device when the third flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_4_OPEN_KEY_LIST, Flip 4 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      List the key codes of accessible keys on the device when the fourth flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_5_OPEN_KEY_LIST, Flip 5 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys WhenFlip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP  
      List the key codes of accessible keys on the device when the fifth flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_CLOSED_KEY_LIST,All Flip Closed Key List,Developer|OEM
   complex,Keys Accessible When Flips Closed
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      List the key codes of keys accessible on the device when
      all flips are fully closed.
      Use AVK_UNDEFINED to indicate that flip closed key list is not applicable - for example, in a
      candy bar phone.
   }
}
Resource, IDS_DD_FLIP_1_OPEN_DISPLAY_LIST, Flip 1 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      List the class IDs of each display accessible on the device when the first flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_2_OPEN_DISPLAY_LIST, Flip 2 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      List the class IDs of each display accessible on the device when the second is fully open and all other flips are closed.

   }
}
Resource, IDS_DD_FLIP_OPEN_3_DISPLAY_LIST, Flip 3 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      List the class IDs of each display accessible on the device when the third flip is fully open and all other flips are closed. 
   }
}
Resource, IDS_DD_FLIP_4_OPEN_DISPLAY_LIST, Flip 4 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      List the class IDs of each display accessible on the device when the fourth flip is fully open and all other flips are closed. 
   }
}
Resource, IDS_DD_FLIP_5_OPEN_DISPLAY_LIST, Flip 5 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      List the class IDs of each display accessible on the device when the fifth flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_CLOSED_DISPLAY_LIST, Flip Closed Display List,Developer|OEM
   complex,Flips Closed Accessible Displays
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
  <Definition>
	  Display Class IDs accessible when all flips are closed.
	<Instructions>
      List the display class IDs of displays accessible to the device when all flips are fully closed.
   }
}

Resource, IDS_DD_SCR_ROTATE, ScrRotate,Developer|OEM
   enum, ScrRotate support, IDEL_LABEL_YESNO
   HELP 
      Is a screen rotate supported on the device?
   }
}
Resource, IDS_DD_SECONDARY_DISPLAY_TEST, SecondaryDisplayTest,OEM
   enum, Execute Secondary Display Test, IDEL_LABEL_YESNO
   HELP 
      Indicates whether the secondary display should be tested, using a 
      background app. 
   }
}
Resource, IDS_DD_AEECLSID_SECONDARY_DISPLAY, SecondaryDisplayClassID,OEM
   enum, SecondaryDisplayClassID, IDEL_DISPLAY_CLSID
   HELP 
      Indicates the display to use for the secondary display test. 
      This value is used only if the resource IDS_DD_SECONDARY_DISPLAY_TEST 
      has a value of 1 (Yes).      
   }
}

#<!--=================================================================
#         Auto-start App Configuration
#=================================================================-->
Resource, IDS_DD_APP_AUTOSTART, Auto-start App,Developer|OEM
   string, Auto-start App's ID
   HELP 
	 <Definition>
	   The Class ID of the auto-start application.

		 This the same as the value for CFGI_AUTOSTART.
	 <Instructions>
      Enter the CLSID of the auto-start application as a hexadecimal number preceded by 0x (for example, 
      0x12345678). The default is 0x1008000, which is the CLSID of the BREW Application Manager (AEECLSID_APPMANAGER).
      If there has been no change to the CLSID of the BREW Application Manager, the default value of 0x1008000 is 
      used.
      

      NOTE:  This is the value returned by OEM_GetConfig() for item CFGI_AUTOSTART.
   }
}
Resource, IDS_DD_APP_AUTOSTARTARGS, Auto-start App,Developer|OEM
   string, Arguments to Auto-start App
   HELP 
	 <Definition>
	   Arguments that are passes to the auto-start application.

     This the same as the value for CFGI_AUTOSTARTARGS.
	 <Instructions>
      Enter the arguments to be passed to the auto-start application.
      
      NOTE:  This is the value returned by OEM_GetConfig() for item CFGI_AUTOSTARTARGS.
   }
}

#<!--=================================================================
#        Class ID Configuration
#=================================================================
Resource, IDS_CLSID_FIRST, Obsolete,NONE
   uint32, CLSID Start ID
}
Resource, IDS_CLSID_MAX, Obsolete,NONE
   uint32, No. of CLSIDs
}

EnumList, IDEL_MEDIA_FEATURE
0,None
4,Volume          
5,Mute            
6,Tempo           
7,Tune            
8,Pan             
65536,PauseResume     
65537,Seek            
65538,GetTotalTime    
65539,Record          
65540,RecPauseResume  
65541,StreamingPlay
65542,ChannelShare
65543,AudioPostcard
65544,FrameCallback
65545,AudioSync
65546,PolyphonyMode
65547,PauseResumeRaw
}

#<!--_______________________________________
#                Vocoders Supported
#      IS96   : QCELP-8k
#      IS96A  : QCELP-8k
#      IS733  : QCELP-13k
#      IS127  : EVRC-8k
#      SMV    : SMV for QChat Demo
#      PCM    : PCM     
#      AMR    : AMR
#      G711   : G.711
#      4GV_NB : 4GV Narrowband
#_______________________________________-->
EnumList, IDEL_VOCODERS_SUPPORTED
0,None
1,IS96
2,IS96A
3,IS733
4,IS127
5,SMV
6,PCM
7,AMR
8,G711
9,VOC_4GV_NB
}
EnumList, IDEL_CMX_VERSION
None,None
2.2,2.2
3.0,3.0
3.1,3.1
4.0,4.0
4.1,4.1
4.2,4.2
4.3,4.3
4.4,4.4
}
Resource, IDS_AEECLSID_MEDIAMIDI, MIDI Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, MIDI Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP
	<Instructions>
      IMedia-based MIDI configuration.
   }
}
Resource, IDS_DD_AUD_MIDI_MIME_TYPES, MIDI Configuration
   string, MIDI MIME Types, , |
   HELP 
     <Definition>
	  MIDI MIME Types supported
	<Instructions>
      Enter a comma separated list of all MIDI MIME Types supported on the device (for example, audio/mid). 
   }
}
Resource, IDS_DD_AUD_MIDI_NUM_VOICES, MIDI Configuration
   uint32, Number of tones
   HELP 
      Number of MIDI-Polyphonic tones supported by the device.
   }
} 
Resource, IDS_DD_AUD_MIDI_NUM_CHAN, MIDI Configuration
   uint32, Simultaneous MIDI channels
   HELP 
      Number of MIDI channels that can be played simultaneously.
   }
}     
Resource, IDS_AEECLSID_MEDIAMP3, MP3 Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based MP3 configuration.
   }
}
Resource, IDS_DD_AUD_MP3_MIME_TYPES, MP3 Configuration
   string, MP3 MIME Types, , |
   HELP 
     <Definition>
	  MP3 MIME Types supported
	<Instructions>
      Enter a comma separated list of all MP3 MIME Types supported on the device (for example, audio/mp3). 
   }
}
Resource, IDS_AEECLSID_MEDIAQCP, QCP Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based QCP configuration.
   }
}
Resource, IDS_DD_AUD_QCP_MIME_TYPES, QCP Configuration
   string, QCP MIME Types, , |
   HELP 
     <Definition>
	  QCP MIME Types supported
	<Instructions>
      Enter a comma separated list of all QCP MIME Types supported on the device (for example, audio/qcp). 
   }
}
Resource, IDS_DD_AUD_QCELP_PLAY, QCP Configuration
   enum, QCELP Play, IDEL_LABEL_YESNO
   HELP 
      Is QCELP playback supported on this device?
   }
}
Resource, IDS_DD_AUD_QCELP_RECORD, QCP Configuration
   enum, QCELP Record, IDEL_LABEL_YESNO
   HELP 
      Is QCELP recording supported on this device?
   }
}
Resource, IDS_DD_AUD_QCELP_RINGER, QCP Configuration
   enum, QCELP Ringer, IDEL_LABEL_YESNO
   HELP 
      Are QCELP ringers supported on this device?
   }
}
Resource, IDS_AEECLSID_MEDIAPMD, PMD Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based PMD configuration.
   }
}
Resource, IDS_DD_MMEDIA_PMD_MIME_TYPES, PMD Configuration
   string, PMD MIME Types, , |
   HELP 
     <Definition>
	  PMD MIME Types supported
	<Instructions>
      Enter a comma separated list of all PMD MIME Types supported on the device (for example, video/pmd). 
   }
}
Resource, IDS_DD_MMEDIA_PMD, PMD Configuration
   enum, CMX(.pmd) Version, IDEL_CMX_VERSION
   HELP 
     <Definition>
	  CMX Version supported and exceptions
	<Instructions>
          Enter the CMX version supported by the device. 
          Enter any types not supported in the Comment 
			field.
			
          If CMX is not supported enter "None".
      
          NOTE: If CMX is supported, it needs to be supported through BREW and the device's native UI.  CMX files must be playable as ringers, wallpapers, and screensavers.
   }
}
Resource, IDS_AEECLSID_MEDIAMIDIOUTQCP, MIDIOUTQCP Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based MIDIOUTQCP configuration.
   }
}
Resource, IDS_AEECLSID_MPEG4, MPEG4 Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based MPEG4 configuration.
   }
}
Resource, IDS_DD_MMEDIA_MPEG4_MIME_TYPES, MPEG4 Configuration
   string, MPEG4 MIME Types, , |
   HELP 
     <Definition>
	  MPEG4 MIME Types supported
	<Instructions>
      Enter a comma separated list of all MPEG4 MIME Types supported on the device (for example, video/mp4). 
   }
}
Resource, IDS_AEECLSID_MEDIAMMF, MMF/SMAF Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based MMF configuration.
   }
}
Resource, IDS_DD_AUD_MMF_MIME_TYPES, MMF/SMAF Configuration
   string, MMF MIME Types, , |
   HELP 
     <Definition>
	  MMF MIME Types
	<Instructions>
      Enter a comma separated list of all the MMF MIME Types supported on the device (for example, audio/mmf). 
   }
}
Resource, IDS_AEECLSID_MEDIAPHR, PHRASE Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based PHRASE configuration.
   }
}
Resource, IDS_DD_AUD_PHRASE_MIME_TYPES, PHRASE Configuration
   string, PHRASE MIME Types, , |
   HELP 
     <Definition>
	  PHRASE MIME Types supported
	<Instructions>
      Enter a comma separated list of all PHRASE MIME Types supported on the device (for example, audio/spf).
   }
}
Resource, IDS_AEECLSID_MEDIAADPCM, ADPCM Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based ADPCM configuration.
   }
}
Resource, IDS_DD_AUD_ADPCM_MIME_TYPES, ADPCM Configuration
   string, ADPCM MIME Types, , |
   HELP 
     <Definition>
	  ADPCM MIME Types supported.
	<Instructions>
      Enter a comma separated list of all the ADPCM MIME Types supported on the device (for example, audio/wav).
   }
}
Resource, IDS_AEECLSID_MEDIAIMELODY, IMELODY Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based IMELODY configuration.
   }
}
Resource, IDS_DD_AUD_IMELODY_MIME_TYPES, IMELODY Configuration
   string, IMELODY MIME Types, , |
   HELP 
     <Definition>
	  IMELODY MIME Types supported.
	<Instructions>
      Enter a comma separated list of all the IMELODY MIME Types supported on the device (for example, audio/imy). 
   }
}
Resource, IDS_AEECLSID_MEDIAAMR, AMR Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based AMR configuration.
   }
}
Resource, IDS_DD_AUD_AMR_MIME_TYPES, AMR Configuration
   string, AMR MIME Types, , |
   HELP 
     <Definition>
	  AMR MIME Types supported.
	<Instructions>
      Enter a comma separated list of all AMR MIME Types supported on the device (for example, audio/amr). 
   }
}
Resource, IDS_AEECLSID_MEDIAAMRWB, AMR WB Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based AMR WB configuration.
   }
}
Resource, IDS_DD_AUD_AMRWB_MIME_TYPES, AMR WB Configuration
   string, AMR WB MIME Types, , |
   HELP 
     <Definition>
	  AMR WB MIME Types supported.
	<Instructions>
      Enter a comma separated list of all AMR WB MIME Types supported on the device (for example, audio/awb). 
   }
}
Resource, IDS_AEECLSID_MEDIAAAC, AAC Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based AAC configuration.
   }
}
Resource, IDS_DD_AUD_AAC_MIME_TYPES, AAC Configuration
   string, AAC MIME Types, , |
   HELP 
     <Definition>
	  AAC MIME Types supported.
	<Instructions>
      Enter a comma separated list of all the AAC MIME Types supported on the device (for example, audio/aac).
   }
}
Resource, IDS_DD_AUD_AAC_PLUS, AAC Configuration
   enum, AAC+, IDEL_LABEL_YESNO
	HELP 
	    Is the AAC+ format supported on this device?
   }
}
Resource, IDS_DD_AUD_ENH_AAC_PLUS, AAC Configuration
   enum, Enhanced AAC+, IDEL_LABEL_YESNO
	HELP 
	    Is the enhanced AAC+ format supported on this device?
   }
}
Resource, IDS_AEECLSID_BMP, BMP Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IImage-based BMP configuration.
   }
}
Resource, IDS_DD_IMG_BMP_MIME_TYPES, BMP Configuration
   string, BMP MIME Types, , |
   HELP 
     <Definition>
	  BMP MIME Types supported.
	<Instructions>
      Enter a comma separated list of all BMP MIME Types supported on the device. (for example, image/bmp).
   }
}
Resource, IDS_AEECLSID_GIF, GIF Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IImage-based GIF configuration.
   }
}
Resource, IDS_DD_IMG_GIF_MIME_TYPES, GIF Configuration
   string, GIF MIME Types, , |
   HELP 
     <Definition>
	  GIF MIME Types supported.
	<Instructions>
      Enter a comma separated list of all GIF MIME Types supported on the device (for example, image/gif). 
   }
}
Resource, IDS_AEECLSID_PNG, PNG Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IImage-based PNG configuration.
   }
}
Resource, IDS_DD_IMG_PNG_MIME_TYPES, PNG Configuration
   string, PNG MIME Types, , |
   HELP 
     <Definition>
	  PNG MIME Types supported.
	<Instructions>
      Enter a comma separated list of all the PNG MIME Types supported on the device (for example, image/png).
   }
}
Resource, IDS_AEECLSID_JPEG, JPEG Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IImage-based JPEG configuration.
   }
}
Resource, IDS_DD_IMG_JPEG_MIME_TYPES, JPEG Configuration
   string, JPEG MIME Types, , |
   HELP 
     <Definition>
	  JPEG MIME Types supported.
	<Instructions>
      Enter a comma separated list of all JPEG MIME Types supported on the device (for example, image/jpg).
   }
}
Resource, IDS_AEECLSID_BCI, BCI Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IImage-based BCI configuration.
   }
}
Resource, IDS_DD_IMG_BCI_MIME_TYPES, BCI Configuration
   string, BCI MIME Types, , |
   HELP 
     <Definition>
	  BCI MIME Types supported supported.
	<Instructions>
      Enter a comma separated list of all BCI MIME Types supported on the device (for example, image/bci). 
   }
}
Resource, IDS_AEECLSID_MEDIAPCM, PCM Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based PCM configuration.
   }
}
Resource, IDS_DD_AUD_PCM_MIME_TYPES, PCM Configuration
   string, PCM MIME Types, , |
   HELP 
     <Definition>
	  PCM MIME Types supported on the device
	<Instructions>
      Enter a comma separated list of all PCM MIME Types supported on the device (for example, audio/wav). 
   }
}
Resource, IDS_DD_AUD_PCM_RAW, PCM Configuration
   enum, PCM-raw, IDEL_LABEL_YESNO
	HELP 
	    Is the PCM-raw format supported on this device?
   }
}
Resource, IDS_AEECLSID_MEDIAXMF, XMF Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based XMF configuration.
   }
}
Resource, IDS_AEECLSID_MEDIASVG, SVG Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IMedia-based SVG configuration.
   }
}
Resource, IDS_AEECLSID_MEDIADLS, DLS Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   enum, Global,IDEL_LABEL_YESNO
   HELP 
      DLS class ID configuration. If global is Yes, then DLS when loaded applies to all
      the active media and DLS linking to specific media objects is not supported.
   }
}
Resource, IDS_AEECLSID_SVGDOM, SVGDOM Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      ISVGDOM Class ID configuration. Specify whether ISVGDOM is supported in your device
   }
}
Resource, IDS_AEECLSID_MEDIAEVRCB, EVRCB Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based EVRCB configuration.
   }
}
Resource, IDS_AEECLSID_MEDIAVIDEO, MediaVideo Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Features
      enum, Feature, IDEL_MEDIA_FEATURE
   }
   HELP 
      IMedia-based Video configuration.
   }
}
Resource, IDS_AEECLSID_LAST, Class ID Configuration,NONE
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      Do not edit this item.
   }
}
Resource, IDS_AEECLSID_VOCODER, Vocoder 
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Vocoders
   enum, Vocoder, IDEL_VOCODERS_SUPPORTED
   }    
   HELP 
      <Definition>
      IVocoder support and capabilities.
      If IVocoder is supported, select "Yes" for Supported.
      Otherwise, select "No".
      If IVocoder is supported, select all the Vocoders supported by the device in Vocoders.
   }
}
Resource, IDS_AEECLSID_JOYSTICK, Joystick 
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IJoystick class ID configuration.
   }
}
#<!--=================================================================
#       New DPK entries
#=================================================================-->



EnumList, IDEL_UI_COLOR_ITEMS 
1, CLR_USER_TEXT
2, CLR_USER_BACKGROUND  
3, CLR_USER_LINE  
4, CLR_SYS_TITLE
5, CLR_SYS_TITLE_TEXT
6, CLR_SYS_ITEM
7, CLR_SYS_ITEM_TEXT
8, CLR_SYS_ITEM_SEL
9, CLR_SYS_ITEM_SEL_TEXT
10, CLR_SYS_WIN  
11, CLR_SYS_FRAME_HI
12, CLR_SYS_FRAME_LO
13, CLR_SYS_LT_SHADOW
14, CLR_SYS_DK_SHADOW
15, CLR_SYS_SCROLLBAR
16, CLR_SYS_SCROLLBAR_FILL 
}
 
Resource, IDS_DD_DEFAULT_UI_ITEM_COLORS, UI   

   complex, UI Color Items, 0,0, 0, | 
     enum, UI Items, IDEL_UI_COLOR_ITEMS
      string, RED
      string, GREEN
      string, BLUE
   }  
   HELP
      Enter each UI item supported, and the RGB values of the items.
      If the device does not support any items, leave the entry blank.
   }
}
Resource, IDS_DD_IMUI_TRAP_EVENTS_SUPPORT, IMUI
   enum, Unknown Events, IDEL_LABEL_YESNO
   
   HELP
      If the device supports handling of any Unknown Event by the IMUI, select "Yes"
      If the device doesn't, select "No" 
  }
}
Resource, IDS_DD_FORGIVEDRAW_SUPPORT, Viewer
   enum, Render Corrupt Images, IDEL_LABEL_YESNO
   
   HELP
      If the device supports forgiveness when drawing corrupted images, select "Yes".  
      If the device does not support forgiveness draw, select "No"
   }
}
Resource, IDS_DD_PROGRESSDRAW_SUPPORT, Viewer
   enum, Progessive Draw Support, IDEL_LABEL_YESNO
   
   HELP
      If the device supports progressive drawing of images, select "Yes".  
      If the device does not support progressive drawing, select "No"
   }
}
Resource, IDS_DD_SETDISP_SUPPORT, Viewer
   enum, Change display, IDEL_LABEL_YESNO
   
   HELP
      If the device supports the IIMAGE_SetDisplay interface, select "Yes".
      If the device does not, select "No".
   }
}
Resource, IDS_DD_DIMX_SCROLLBAR, Device
   uint32,Scrollbar Width
   HELP
      <Definition>
	     AEEDeviceInfo member cxScrollBar
      <Instructions>
		 Enter the width of the standard scroll bars.
   }
}
Resource, IDS_DD_MENU_TXT_SCRL_WIDTH, Device

uint32,Scroll Delay

   HELP
      <Definition>
	     AEEDeviceInfo member wMenuTextScroll
      <Instructions>
		 Enter the delay (in ms) to be used before starting autoscroll.
   }
}

Resource, IDS_DD_MENU_IMAGE_DELAY, Device

uint32,Image Delay

   HELP
      <Definition>
	     AEEDeviceInfo member wMenuImageDelay
      <Instructions>
		 Enter the delay (in ms) to be used before scrolling menu images.
   }
}
Resource, IDS_DD_VOICE_RECOGNITION, Device
   enum, Voice Recognition Support, IDEL_LABEL_YESNO
   
   HELP
      <Definition>
	     AEEDeviceInfo member bVR
	  <Instructions>
	     If the device supports Voice Recognition, select "Yes".
		 If the device does not, select "No".
   }
}
EnumList, IDEL_PROMPT_PROP_FLAG
65536, ST_TEXT
131072, ST_TITLE
262144, ST_NOSCROLL
524288, ST_TEXTALLOC
1048576, ST_TITLEALLOC
2097152, ST_MIDDLETEXT
4194304, ST_UNDERLINE 
8388608, ST_ICONTEXT 
16777216, ST_ASCII 
33554432, ST_ENABLETAB
67108864, ST_ENABLE_HLGHT
134217728, ST_WRAP_NOBREAK 
268435456, ST_QUARTER_SEC
536870912, ST_HALF_SEC
805306368, ST_SCROLL_TWO_SEC
}
Resource, IDS_DD_PROMPT_PROP_FLAG, Device

  complex, Prompt Flags, 0,0,0,| 
     enum, PROMPT Flag, IDEL_PROMPT_PROP_FLAG
  }
   HELP
      <Definition>
	     AEEDeviceInfo member dwPromptProps
	  <Instructions>
         Enter all default prompt properties supported by the device.
   }
}
Resource, IDS_DD_SLEEPDEFER, Device
uint32,Time for Sleep Defer

   HELP
      <Definition>
	     AEEDeviceInfo member dwSleepDefer
      <Instructions>
             Enter the time (in ms) a device waits before attempting to enter sleep mode
   }
}
Resource, IDS_DD_SOFTKEY_COUNT, Device
   string, SoftKey Count

   HELP
         <Definition>
             The AEEDEVICEITEM - AEE_DEVICEITEM_SOFTKEY_COUNT
         <Instructions>	
             Enter the number of softkeys the device supports. 
             If AEE_DEVICEITEM_SOFTKEY_COUNT is not supported, enter "not supported".
   }
}
Resource, IDS_DD_MANUFACTURER_NAME, Device
   string, Manufacturer Name 
   HELP
            <Definition>
                The AEEDEVICEITEM - AEE_DEVICEITEM_MANUFACTURER_NAME
            <Instructions>	
            	Enter the name of the manufacture of the device.
		If AEE_DEVICEITEM_MANUFACTURER_NAME is not supported, enter "not supported".
   }
}
Resource, IDS_DD_MODEL_NAME, Device
   string, Model Name 
   HELP
            <Definition>
                The AEEDEVICEITEM - AEE_DEVICEITEM_MODEL_NAME
            <Instructions>	
            	Enter the device's offical Model Name, not the one used by customers.
		If AEE_DEVICEITEM_MODEL_NAME is not supported, enter "not supported".
   }
}
Resource, IDS_DD_FRIENDLY_NAME, Device
   string, Friendly Name 
   HELP
            <Definition>
                The AEEDEVICEITEM - AEE_DEVICEITEM_FRIENDLY_NAME
            <Instructions>	
            	Enter the device's Model Name used by customers.
		If AEE_DEVICEITEM_FRIENDLY_NAME is not supported, enter "not supported".
   }
}
Resource, IDS_DD_POSDET_EMERGENCY_ONLY, Device
   string, Emergency Only 
   HELP
            <Definition>
                The AEEDEVICEITEM - AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY
            <Instructions>	
            	If by default, position determination is set to "Emergency(911) Only", enter "1"
                If by default, position determination is set to "Location On", enter a "0" 
		If AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY is not supported, enter "not supported".
   }
}
EnumList, IDEL_IMM_INPUT_MODES
0,AEE_TM_NONE
1,AEE_TM_CURRENT
2,AEE_TM_SYMBOLS
3,AEE_TM_LETTERS
4,AEE_TM_RAPID
5,AEE_TM_NUMBERS
6,AEE_TM_KANJI
7,AEE_TM_HANKAKU_KANA
8,AEE_TM_HANGUL
9,AEE_TM_PINYIN
10,AEE_TM_STROKE
11,AEE_TM_EZTEXT
12,AEE_TM_T9
13,AEE_TM_ARABIC
14,AEE_TM_ARABIC_R
15,AEE_TM_HEBREW
16,AEE_TM_HEBREW_R
17,AEE_TM_CROATIAN
18,AEE_TM_CROATIAN_R
19,AEE_TM_CZECH
20,AEE_TM_CZECH_R
21,AEE_TM_HUNGARIAN
22,AEE_TM_HUNGARIAN_R
23,AEE_TM_ROMANIAN
24, AEE_TM_ROMANIAN_R
25, AEE_TM_RUSSIAN
26, AEE_TM_RUSSIAN_R
27, AEE_TM_SERBIAN
28, AEE_TM_SERBIAN_R
29, AEE_TM_SLOVAK
30, AEE_TM_SLOVAK_R
31, AEE_TM_SLOVENIAN
32, AEE_TM_SLOVENIAN_R
33, AEE_TM_TURKISH
34, AEE_TM_TURKISH_R
35, AEE_TM_UKRAINIAN
36, AEE_TM_UKRAINIAN_R
37,AEE_TM_DANISH
38,AEE_TM_DANISH_R
39,AEE_TM_DUTCH
40,AEE_TM_DUTCH_R
41,AEE_TM_FINNISH
42,AEE_TM_FINNISH_R
43,AEE_TM_FRENCH_EU
44,AEE_TM_FRENCH_EU_R
45,AEE_TM_GERMAN
46,AEE_TM_GERMAN_R
47,AEE_TM_GREEK
48,AEE_TM_GREEK_R
49,AEE_TM_ITALIAN
50,AEE_TM_ITALIAN_R
51,AEE_TM_NORWEGIAN
52,AEE_TM_NORWEGIAN_R
53,AEE_TM_POLISH
54,AEE_TM_POLISH_R  
55,AEE_TM_PORTUGUESE_EU 
56,AEE_TM_PORTUGUESE_EU_R 
57,AEE_TM_SPANISH_EU
58,AEE_TM_SPANISH_EU_R
59,AEE_TM_SWEDISH
60,AEE_TM_SWEDISH_R
61,AEE_TM_INDONESIAN
62,AEE_TM_INDONESIAN_R
63,AEE_TM_MALAY
64,AEE_TM_MALAY_R
65,AEE_TM_TAGALOG
66,AEE_TM_TAGALOG_R
67,AEE_TM_THAI
68,AEE_TM_THAI_R
69,AEE_TM_VIETNAMESE
70,AEE_TM_VIETNAMESE_R
71,AEE_TM_FRENCH_CA
72,AEE_TM_FRENCH_CA_R
73,AEE_TM_PORTUGUESE_BR
74,AEE_TM_PORTUGUESE_BR_R
75,AEE_TM_SPANISH_LA
76,AEE_TM_SPANISH_LA_R
77,AEE_TM_DIALED_DIGITS
78,AEE_TM_ENGLISH_UK_R
79,AEE_TM_ENGLISH_US
80,AEE_TM_ENGLISH_US_R
81,AEE_TM_ENGLISH_UK
82,AEE_TM_HINDI
83,AEE_TM_HINDI_R
}
Resource, IDS_DD_IMM_INPUT_MODES, Device
  complex, Input Modes, 0,0,0,| 
     enum, Input Modes, IDEL_IMM_INPUT_MODES
   }
   HELP
      Enter the Input Modes supported by the device. 
   }
}
########################################################################
#
# Configuration info
#
########################################################################
Resource, IDS_DD_KB_AUTO_REPEAT, Configuration Info

    uint32, KB Auto Repeat - dwStart
    uint32, KB Auto Repeat - dwRate
    HELP
	<Definition>
           IConfig item CFGI_KB_AUTOREPEAT
	<Instructions>
           Enter the length of the time (in ms) the device waits to send a second EVT_KEY event when a key is held down in dwStart.
           Enter the length of time (in ms) the device waits after the second EVT_KEY event to send the third and subsequent EVT_KEY events while a key is held down in dwRate.
	   If the device does not support CFGI_KB_AUTOREPEAT, set dwStart and dwRate to "0".
   }
}
Resource, IDS_DD_NET_CONNECTION_TIMEOUT, Configuration Info
uint32, Net Timeout

   HELP
	<Definition>
           IConfig item CFGI_NET_CONNTIMEOUT
        <Instructions>
           Enter the time (in ms) the device will wait for a connect to complete.
           If the device does not support CFGI_NET_CONNTIMEOUT, enter "0".
   }
}
Resource, IDS_DD_SCREEN_SAVER,  Configuration Info
   complex, Screen Saver
      uint32, Default 
      uint32, Minimum
      uint32, Maximum
}
   HELP
	<Definition>
           IConfig item CFGI_SCREEN_SAVER
        <Instructions>
           Enter the default timer (in s) for the screensaver in Default.
           Enter the minimum settable timer (in s) for the screensaver in Minimum.
           Enter the maximum settable timer (in s) for the screensaver in Maximum.
   
           If the device does not support CFGI_SCREEN_SAVER, enter "0" for each value.
   }
   }
Resource, IDS_DD_FILE_CACHE_INFO,  Configuration Info
   complex, File Cache
     uint32, Default 
     string, Minimum 
     string, Maximum
}
   HELP
	<Definition>
           IConfig item CFGI_FILE_CACHE_INFO
        <Instructions>
           Enter the default file cache size (in bytes) for the device in Default.
           Enter the minimum settable file cache size  (in bytes) in Minimum.
           Enter the maximum settable file cache size  (in bytes) in Maximum.

           If the device does not support CFGI_FILE_CACHE_INFO, enter "0" for each value.
           If the device does not have a minimum or maxmimum cache size, enter "-1" accordingly.
   }
}
Resource, IDS_DD_MODULE_FSLIMIT,  Configuration Info
   uint32, MaxFiles 
   uint32, MaxSpace
   HELP
	<Definition>
           IConfig item CFGI_MODULE_FSLIMIT
        <Instructions>
           Enter the default maximum number of files allowed for a module.
           Enter the default maximum EFS space (in bytes) allowed for a module.

           If the device does not support CFGI_MODULE_FSLIMIT, enter "0" for each value.
   }
}
Resource, IDS_DD_CLOSE_KEYS,  Configuration Info
   complex, Close Keys
   enum, Close App Key, IDEL_KEY_LIST
   enum, Key Event, IDEL_KEY_EVENT
   enum, Close All App Key,IDEL_KEY_LIST
   enum, Key Event, IDEL_KEY_EVENT
}
   HELP
	<Definition>
           IConfig item CFGI_CLOSE_KEYS
        <Instructions>
	   Select the key that will close the current foreground applet only for Close App Key.
           Select the key event that will trigger the foreground applet shutdown.
           Select the key that will close all currently loaded applets.
           Select the key event that will trigger the applet shutdown.

           If the device does not support IDS_DD_CLOSE_KEYS, enter "None" for each value.
   }
}

Resource, IDS_DD_API_SUPPORTED, Interface/Font Support
enum, IDownload, IDEL_API_SUPPORT
enum, IADSQuery, IDEL_API_SUPPORT
enum, IShell, IDEL_API_SUPPORT
enum, IDisplay, IDEL_API_SUPPORT
enum, IHeap, IDEL_API_SUPPORT
enum, IFileMgr_312, IDEL_API_SUPPORT
enum, INetMgr_10, IDEL_API_SUPPORT
enum, ITAPI, IDEL_API_SUPPORT
enum, IDBMgr, IDEL_API_SUPPORT
enum, IDialog, IDEL_API_SUPPORT
enum, IStatic_10, IDEL_API_SUPPORT
enum, ISound_10, IDEL_API_SUPPORT
enum, IMemAStream, IDEL_API_SUPPORT
enum, ILicense, IDEL_API_SUPPORT
enum, ISourceUtil, IDEL_API_SUPPORT
enum, IUnzipAstream, IDEL_API_SUPPORT
enum, IHashMD5, IDEL_API_SUPPORT
enum, IRSCPOOL, IDEL_API_SUPPORT
enum, IThread, IDEL_API_SUPPORT
enum, ARC4, IDEL_API_SUPPORT
enum, IRSA, IDEL_API_SUPPORT
enum, IPosDet_11, IDEL_API_SUPPORT
enum, Provision, IDEL_API_SUPPORT
enum, IClipboard, IDEL_API_SUPPORT
enum, IConfig, IDEL_API_SUPPORT
enum, ISprite, IDEL_API_SUPPORT
enum, IPosDet_20, IDEL_API_SUPPORT
enum, DispClone, IDEL_API_SUPPORT
enum, INetMgr, IDEL_API_SUPPORT
enum, IX509Chain, IDEL_API_SUPPORT
enum, SSLRootCerts, IDEL_API_SUPPORT
enum, IHashMD2, IDEL_API_SUPPORT
enum, IHashSHA1, IDEL_API_SUPPORT
enum, IHashCTXMD5CTX, IDEL_API_SUPPORT
enum, IHashCTXMD2CTX, IDEL_API_SUPPORT
enum, IHashCTXSHA1CTX, IDEL_API_SUPPORT
enum, Random, IDEL_API_SUPPORT
enum, IRamCache, IDEL_API_SUPPORT
enum, Registry, IDEL_API_SUPPORT
enum, PosDet_302, IDEL_API_SUPPORT
enum, Serial, IDEL_API_SUPPORT
enum, IFIFO, IDEL_API_SUPPORT
enum, SEEDEncrypt, IDEL_API_SUPPORT
enum, SEEDDecrypt, IDEL_API_SUPPORT
enum, IHashHMAC_MD5, IDEL_API_SUPPORT
enum, IHashHMAC_MD2, IDEL_API_SUPPORT
enum, IHashHMAC_SHA1, IDEL_API_SUPPORT
enum, IHashCTXHMAC_MD5CTX, IDEL_API_SUPPORT
enum, IHashCTXHMAC_MD2CTX, IDEL_API_SUPPORT
enum, IHashCTXHMAC_SHA1CTX, IDEL_API_SUPPORT
enum, IAppHistory, IDEL_API_SUPPORT
enum, ITelephone, IDEL_API_SUPPORT
enum, IPhoneNotifier, IDEL_API_SUPPORT
enum, ICallMgr, IDEL_API_SUPPORT
enum, IPhoneCtl, IDEL_API_SUPPORT
enum, ISound, IDEL_API_SUPPORT
enum, ICallOrigOpts, IDEL_API_SUPPORT
enum, IAPPLETCTL, IDEL_API_SUPPORT
enum, IPosDet_312, IDEL_API_SUPPORT
enum, IPosDet, IDEL_API_SUPPORT
enum, ISoundPlayer, IDEL_API_SUPPORT
enum, IGraphics, IDEL_API_SUPPORT
enum, IRingerMgr, IDEL_API_SUPPORT
enum, IAddrBook_20, IDEL_API_SUPPORT
enum, IVocoder, IDEL_API_SUPPORT
enum, ICallHistory_30, IDEL_API_SUPPORT
enum, IRUIM, IDEL_API_SUPPORT
enum, ILogger_File, IDEL_API_SUPPORT
enum, ILogger_DMSS, IDEL_API_SUPPORT
enum, ILogger_WIN, IDEL_API_SUPPORT
enum, ICamera, IDEL_API_SUPPORT
enum, IAddrBook_21, IDEL_API_SUPPORT
enum, IAddrBook, IDEL_API_SUPPORT
enum, ICallHistory, IDEL_API_SUPPORT
enum, IMenuCtl_10, IDEL_API_SUPPORT
enum, SoftKeyCtl_10, IDEL_API_SUPPORT
enum, ListCtl_10, IDEL_API_SUPPORT
enum, IconViewCtl_10, IDEL_API_SUPPORT
enum, DatePickCtl, IDEL_API_SUPPORT
enum, IDateCtl, IDEL_API_SUPPORT
enum, StopWatchCtl, IDEL_API_SUPPORT
enum, ClockCtl, IDEL_API_SUPPORT
enum, CountDownCtl, IDEL_API_SUPPORT
enum, ITextCtl_10, IDEL_API_SUPPORT
enum, IAClockCtl, IDEL_API_SUPPORT
enum, IImageCtl, IDEL_API_SUPPORT
enum, INetMgr_11, IDEL_API_SUPPORT
enum, IStatic, IDEL_API_SUPPORT
enum, IMenuCtl, IDEL_API_SUPPORT
enum, SoftKeyCtl, IDEL_API_SUPPORT
enum, ListCtl, IDEL_API_SUPPORT
enum, IconViewCtl, IDEL_API_SUPPORT
enum, ITextCtl, IDEL_API_SUPPORT
enum, WinBMP, IDEL_API_SUPPORT
enum, NativeBMP, IDEL_API_SUPPORT
enum, GIF, IDEL_API_SUPPORT
enum, PNG, IDEL_API_SUPPORT
enum, JPEG, IDEL_API_SUPPORT
enum, BCI, IDEL_API_SUPPORT
enum, IHTMLViewer, IDEL_API_SUPPORT
enum, IWeb, IDEL_API_SUPPORT
enum, IWebUtil, IDEL_API_SUPPORT
enum, IWebOpts, IDEL_API_SUPPORT
enum, IDNS, IDEL_API_SUPPORT
enum, ISSL_313, IDEL_API_SUPPORT
enum, ISMSMSG, IDEL_API_SUPPORT
enum, ISMS, IDEL_API_SUPPORT
enum, ISMSNotifier, IDEL_API_SUPPORT
enum, ISMSStorage, IDEL_API_SUPPORT
enum, ISMSBCSRVOPTS, IDEL_API_SUPPORT
enum, ISMSBCCONFIG, IDEL_API_SUPPORT
enum, IMediaMIDI, IDEL_API_SUPPORT
enum, IMediaMP3, IDEL_API_SUPPORT
enum, IMediaQCP, IDEL_API_SUPPORT
enum, IMediaPMD, IDEL_API_SUPPORT
enum, IMediaMIDIOutMsg, IDEL_API_SUPPORT
enum, IMediaMIDIOutQCP, IDEL_API_SUPPORT
enum, IMediaMPEG4, IDEL_API_SUPPORT
enum, IMediaMMF, IDEL_API_SUPPORT
enum, IMediaPHR, IDEL_API_SUPPORT
enum, IMediaADPCM, IDEL_API_SUPPORT
enum, IMediaAAC, IDEL_API_SUPPORT
enum, IMediaIMelody, IDEL_API_SUPPORT
enum, IMediaUtil, IDEL_API_SUPPORT
enum, IMediaAMR, IDEL_API_SUPPORT
enum, IMediaHVS, IDEL_API_SUPPORT
enum, IMediaSAF, IDEL_API_SUPPORT
enum, IMediaPCM, IDEL_API_SUPPORT
enum, IMediaXMF, IDEL_API_SUPPORT
enum, IMediaDLS, IDEL_API_SUPPORT
enum, IMediaSVG, IDEL_API_SUPPORT
enum, IGL, IDEL_API_SUPPORT
enum, IEGL, IDEL_API_SUPPORT
enum, ITopVisibleCtl, IDEL_API_SUPPORT
enum, IFOTA, IDEL_API_SUPPORT
enum, IJoystick, IDEL_API_SUPPORT
enum, BMPDecoder, IDEL_API_SUPPORT
enum, JPEGDecoder, IDEL_API_SUPPORT
enum, PNGDecoder, IDEL_API_SUPPORT
enum, IBattery, IDEL_API_SUPPORT
enum, IQoSSession, IDEL_API_SUPPORT
enum, IQoSFlow, IDEL_API_SUPPORT
enum, IQoSFilter, IDEL_API_SUPPORT
enum, IBacklight, IDEL_API_SUPPORT
enum, ICipherFactory, IDEL_API_SUPPORT
enum, IFileMgr, IDEL_API_SUPPORT
enum, IWIFI, IDEL_API_SUPPORT
enum, IWIFIOpts, IDEL_API_SUPPORT
enum, IAlarmMgr, IDEL_API_SUPPORT
enum, ISSL, IDEL_API_SUPPORT
enum, INetwork, IDEL_API_SUPPORT
enum, FONT_FIXED4X6, IDEL_API_SUPPORT
enum, FONT_BASIC6, IDEL_API_SUPPORT
enum, FONT_BASIC9, IDEL_API_SUPPORT
enum, FONT_BASIC10, IDEL_API_SUPPORT
enum, FONT_BASIC11, IDEL_API_SUPPORT
enum, FONT_BASIC11B, IDEL_API_SUPPORT
enum, FONT_BASIC14, IDEL_API_SUPPORT
enum, FONT_BASIC12, IDEL_API_SUPPORT
enum, FONT_BASIC12B, IDEL_API_SUPPORT
enum, FONT_BASIC15, IDEL_API_SUPPORT
enum, FONT_STANDARD11, IDEL_API_SUPPORT
enum, FONT_STANDARD11B, IDEL_API_SUPPORT
enum, FONT_STANDARD15, IDEL_API_SUPPORT
enum, FONT_STANDARD15B, IDEL_API_SUPPORT
enum, FONT_STANDARD18, IDEL_API_SUPPORT
enum, FONT_STANDARD18B, IDEL_API_SUPPORT
enum, FONT_STANDARD23, IDEL_API_SUPPORT
enum, FONT_STANDARD23B, IDEL_API_SUPPORT
enum, FONT_STANDARD26, IDEL_API_SUPPORT
enum, FONT_STANDARD26B, IDEL_API_SUPPORT
enum, FONT_STANDARD36, IDEL_API_SUPPORT
enum, IBCMCSDB, IDEL_API_SUPPORT
enum, IFlip, IDEL_API_SUPPORT
enum, ISecureClock, IDEL_API_SUPPORT
enum, IMcastSession, IDEL_API_SUPPORT
enum, ISecSysClock, IDEL_API_SUPPORT
enum, ISysClock, IDEL_API_SUPPORT
enum, IUSBDriver, IDEL_API_SUPPORT
enum, IBT, IDEL_API_SUPPORT
enum, IBTSPPClient, IDEL_API_SUPPORT
enum, IBTSPPServer, IDEL_API_SUPPORT
enum, ISMSStorage2, IDEL_API_SUPPORT
enum, ISignalCBFactory, IDEL_API_SUPPORT
enum, IItemStore/GPSOneLock, IDEL_API_SUPPORT
enum, IPrimaryQoSSession, IDEL_API_SUPPORT
enum, IBitmapFX, IDEL_API_SUPPORT
enum, IvCalStore, IDEL_API_SUPPORT
enum, IvObjectParser, IDEL_API_SUPPORT
enum, IvProperties, IDEL_API_SUPPORT
enum, IPort1, IDEL_API_SUPPORT
enum, IKeysMapping, IDEL_API_SUPPORT
enum, IOBEXCli, IDEL_API_SUPPORT
enum, IOBEXSrv, IDEL_API_SUPPORT
enum, IBitmapScale, IDEL_API_SUPPORT
   HELP
      If the device support(is required to support) a BREW interface/font listed above enter "Supported".
      If the device does not support(is required not to support) an interface/font listed above enter "Not Supported".
      If the device has no requirements about an interface/font listed above enter "Optional" - This setting should only be used for requirements definition and not device specific device packs.
   }
}
Resource, IDS_DD_EXTERNAL_API_REQUIRED, Interface/Font Support
   complex, External Interface/Font Supported
   string, Name
   uint32,  ID
   }
   HELP
      Enter the name of external Interfaces/Fonts supported(required to support) in the Name section.
      Enter the ClassId of the Interfaces/Fonts supported(required to support) in the Id section.
   }
}
Resource, IDS_DD_EXTERNAL_API_PROHIBITED, Interface/Font Support
   complex, External Interface/Font Prohibited
   string, Name
   uint32,  ID
   }
   HELP
      Enter the name of external Interfaces/Fonts prohibited(required to not support) in the Name section.
      Enter the ClassId of the Interfaces/Fonts prohibited(required to not support) in the Id section.
   }
}
Resource, IDS_DD_SIMULTANEOUS_VOICE_DATA, Telephone,OEM
   enum, Simultaneous Voice and Data Support, IDEL_LABEL_YESNO
   
   HELP
      If the device supports simultaneous voice and data calls, enter "Yes".
      Otherwise, select "No".
   }
}

Resource, IDS_DD_IMM_OPTIONS, IMM
    enum, IMMgr System Context supported, IDEL_LABEL_YESNO
   HELP
      If the device support the IMMGR System Context option, select "Yes".
	  If the device does not support the option or the IMMGR interface is not supported, select "No". 

}
    }
Resource, IDS_DD_IMM_IMGRO_MAX_CANDIDATE_NUM, IMM
   uint32, Max Candidate Number
   HELP
      Enter the maximum candidate options in IMMGR_SetOpt the device can set to a non-default value. 
      If the device does not support non-default options or the IMMGR interface is not supported, enter "0".
   }
}
EnumList, IDEL_KEY_PRESS_RESULT_OPTIONS
	0x0000, IMMKEY_DEFAULT     
	0x0001, IMMKEY_MATCHONE     
	0x0002, IMMKEY_MATCHMANY    
}
Resource, IDS_DD_IMM_IMGRO_KEYOPTIONS_SUPPORTED, IMM
   complex, IMGRO KeyOptions Supported, 0,0,0, |   
     enum, Key Press Result Options, IDEL_KEY_PRESS_RESULT_OPTIONS
   }
   HELP
      Enter the Key Press Result Options supported by IM Manager on the device.  
      If the device does not support Key Press result options, or the IMMGR interface is not supported, leave blank.
   }
}
Resource, IDS_DD_IMM_PINYIN_SUPPORTED, IMM

   enum, Pinyin Input Mode supported, IDEL_LABEL_YESNO
   
   HELP
      If Pinyin input mode is supported by the device, select "Yes".
      If not supported, select "No"
   }
}
Resource, IDS_DD_IMM_STROKE_SUPPORTED, IMM

   enum, Stroke Mode supported, IDEL_LABEL_YESNO
   
   HELP
      If Stroke input mode is supported by the device, select "Yes".
      If not supported, select "No".
   }
}	

EnumList, IDEL_SOUND_DEVICE
   0,AEE_SOUND_DEVICE_UNKNOWN
   1,AEE_SOUND_DEVICE_HANDSET
   2,AEE_SOUND_DEVICE_HFK   
   3,AEE_SOUND_DEVICE_HEADSET
   4,AEE_SOUND_DEVICE_AHFK 
   5,AEE_SOUND_DEVICE_SDAC
   6,AEE_SOUND_DEVICE_TTY_HFK  
   7,AEE_SOUND_DEVICE_TTY_HEADSET
   8,AEE_SOUND_DEVICE_CURRENT
   9,AEE_SOUND_DEVICE_SPEAKER 
   10,AEE_SOUND_DEVICE_BT_HEADSET 
   11,AEE_SOUND_DEVICE_STEREO_HEADSET
   12,AEE_SOUND_DEVICE_USB
   13,AEE_SOUND_DEVICE_BT_STEREO_HEADSET
   14,AEE_SOUND_DEVICE_TTY_HCO
   15,AEE_SOUND_DEVICE_TTY_VCO 
   16,AEE_SOUND_DEVICE_TTY_FULL
}

Resource, IDS_DD_SOUND_DEVICES, Sound
   complex, Supported Devices, , , , |
      enum, Supported Device, IDEL_SOUND_DEVICE
      string, Device Name
   }
   HELP
      Select every ISound device type supported.
      Enter the device name for each type supported.
      Sound list of supported device types.
      AEE_SOUND_DEVICE_UNKNOWN : Unknown device
      AEE_SOUND_DEVICE_HANDSET : Handset
      AEE_SOUND_DEVICE_HFK : Hands Free Kit (HFK)
      AEE_SOUND_DEVICE_HEADSET : Mono Headset
      AEE_SOUND_DEVICE_AHFK : Analog HFK
      AEE_SOUND_DEVICE_SDAC : Stereo DAC
      AEE_SOUND_DEVICE_TTY_HFK : TTY HFK
      AEE_SOUND_DEVICE_TTY_HEADSET : TTY Headset
      AEE_SOUND_DEVICE_CURRENT : Currently selected device
      AEE_SOUND_DEVICE_SPEAKER: Built-In Speaker device
      AEE_SOUND_DEVICE_BT_HEADSET: BlueTooth headset
      AEE_SOUND_DEVICE_STEREO_HEADSET: Stereo headset
      AEE_SOUND_DEVICE_USB: USB
      AEE_SOUND_DEVICE_BT_STEREO_HEADSET: BlueTooth stereo headset
      AEE_SOUND_DEVICE_TTY_HCO: forward link = tty data, reverse link = voice
      AEE_SOUND_DEVICE_TTY_VCO: forward link = voice, reverse link = tty data
      AEE_SOUND_DEVICE_TTY_FULL: tty data both directions.
   }
}
#<!--=================================================================
#       FMRadio
#=================================================================-->
Resource, IDS_DD_FMRADIO_RECEIVER_SUPPORTED, FM Radio
   enum, FM Radio Receiver Support, IDEL_LABEL_YESNO

   HELP
      Enter "Yes" if FM Radio Receiver is supported; otherwise, "No". 
   }
}
#<!--=================================================================
#       FMRDS
#=================================================================-->
Resource, IDS_DD_FMRDS_RECEIVER_SUPPORTED, FM Radio
   enum, FMRDS Receiver Support, IDEL_LABEL_YESNO

   HELP
      Enter "Yes" if FMRDS Receiver is supported; otherwise, "No". 
   }
}

