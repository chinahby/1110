# dso.sch

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

#<!--=================================================================
#       Version Info
#=================================================================-->
Resource, IDS_DD_TIMESTAMP, Device Pack Info,Carrier|OEM,RO
   string, Timestamp
	 HELP
	    The time the device pack was last saved at the UTC
      The format of this field is: MM/DD/YYYY hh:mm:ss UTC
      where
      MM: Month
      DD: Day
      YYYY: Year
      hh: Hour (00-23)
      mm: Minutes
      ss: Seconds
   }
}
Resource,IDS_DD_CARRIER_VERSION, Device Pack Info
   string, Device Pack Carrier Version
	 HELP
   This field is updated by Carrier only when they update the DevicePack info.

   The format of this string is c1.c2
   where
   c1, c2: Carrier version. c1 is major and c2 is minor number

   }
}
Resource,IDS_DD_OEM_VERSION, Device Pack Info
   string, Device Pack OEM Version
	 HELP
   This field is updated by OEM only when they update the DevicePack info.

   The format of this string is e1.e2
   where
   e1, e2: OEM version. e1 is major and e2 is minor number

   }
}

#<!--=================================================================
#       Model Info
#=================================================================-->
Resource, IDS_DD_DEV_BREW_UPDATE_VER, Device Model Info, OEM
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
Resource, IDS_DD_DEV_PEK_VER, Device Model Info
   string, PEK Version
   HELP
      Enter the PEK version used to test this device including all Patches
			and Feature Packages (for example, 3.1.3U01FP02).
   }
}
Resource, IDS_DD_DIAG_EXT_SUPPORT, Device Model Info
   enum, Diag Extension Support, IDEL_LABEL_YESNO
	 HELP
	  Does the device support the Diag Extension?
   }
}
Resource, IDS_DD_DEV_APPMGR_VER, Device Model Info
   string, BREW App Manager version
   HELP
      Enter the version of BREW Application Manager on the device including all Patches
      and Feature Packages (for example, 3.0.2U01FP02).

   }
}
Resource, IDS_TB_DEVICE_CATEGORIZATION_PATH, Device Model Info,OEM
   string, Device Categorization Path
	HELP
	<Definition>
	  Desired Class hierarchy for the device
	<Instructions>
	  Enter the path representing the desired device class hierarchy for the device.
	}
}

#<!--=================================================================
#       Test Support Info
#=================================================================-->
Resource, IDS_DD_UMTS_MODE, Test Support Info, OEM
   enum, UMTS Support, IDEL_LABEL_YESNO
   HELP
      Does the device support UMTS (WCDMA)?
   }
}
Resource, IDS_DD_LNP_CONF, Test Support Info,OEM
   enum, LNP Configuration, IDEL_LABEL_YESNO
   HELP
      Is the device configured for Local Number Portability? LNP means that the MDN of the device does not 
      equal the MIN of the device.
   }
}

#<!--=================================================================
#       Memory Info
#=================================================================-->
Resource, IDS_DD_HW_MMC1_SIZE, Memory,OEM
   uint32, Media Card Size Slot 1 (bytes)
   HELP
      What is the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 1 of the device?
      If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_HW_MMC1_SIZE_VARIANCE, Memory,OEM
   uint32, Media Card Size Measurement Variance Slot 1 (bytes)
   HELP
      What is the allowable variance of the measurement of the size of the MMC/RMC (multimedia card/removable 
      media card) inserted in slot 1 of the device?  If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_HW_MMC2_SIZE, Memory,OEM
   uint32, Media Card Size Slot 2 (bytes)
   HELP
      What is the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 2 of the device?
      If the device does not have a media card slot 2, enter "0".
   }
}
Resource, IDS_DD_HW_MMC2_SIZE_VARIANCE, Memory,OEM
   uint32, Media Card Size Measurement Variance Slot 2 (bytes)
   HELP
      What is the allowable variance of the measurement of the size of the MMC/RMC (multimedia card/removable media card)
      inserted in slot 2 of the device?  If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_MMC_MODELNUM_SERIALNUM_INFO, Memory, OEM
   string, Media Card info, , |
   HELP
	    If the model and serial number for an MMC/RMC (multimedia card/removable media card) is supported through BREW,
      enter the comma separated model and serial number for each card 
      (for example, 1,SMD-1,200,2,SMD-1,201, meaning for card 1, the model number is SMD-1
      and the serial number is 200, and for card 2, the model number is SMD-1 and the serial
      number is 201.  Card 1 is treated as fs:/card0, which is the only card
      currently tested).
   }
}
Resource, IDS_DD_MAXEFS_SPACE_AFTER_FULLWRTIE, Memory, OEM
   uint32, Maximum EFS Free Space After Full Write
   HELP
	<Definition>
           Maximum Free Space left in EFS after writing constant sized buffers
           to the file system. This parameter is used only by OATFile and
           OATFS tests and takes into account the buffer size used by these
           modules.
	<Instructions>
           Do not change the default value of 32000 without contacting
           QUALCOMM Internet Services (QIS).
   }
}

#<!--=================================================================
#       Address Book Info
#=================================================================-->
Resource, IDS_DD_ADDR_REMOVE_ALL_SUPPORTED, Address Book,Carrier|OEM

   enum, RemoveAllRecs Support, IDEL_YESNO
   HELP 
      Does the device support the IADDRBOOK_RemoveAllRecs() method?
   }
}

#<!--=================================================================
#       Font Info
#=================================================================-->
Resource, IDS_DD_FONT_MONOBITMAPFONTS, Font,OEM
   enum, Monochrome bitmap fonts, IDEL_YESNO
   HELP 
	<Definition>
	    All fonts on the device are monochrome bitmaps.
	<Instructions>
      If all of the fonts on the device are represented by monochrome bitmaps,
      select "Yes".  If one or more of the fonts is not, select
      "No" (for example, if a font is a vector font, uses multiple colors,
      or uses alpha-blending or anti-aliasing).
   }
}

#<!--=================================================================
#       Path Info: Apps, Ringer, Shared, AddrBook, MIF, Logo
#=================================================================-->
Resource, IDS_DD_PATH_APPS, Path Info,OEM
   string, Application Directory
   HELP
      The path to applications (for example, "brew/mod").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_RINGERS, Path Info,OEM
   string, Ringer Directory
   HELP
      The path to ringers (for example, "user/sound/ringers").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_SHARED, Path Info,OEM
   string, Shared Directory
   HELP
      The path to shared media directory (for example, "brew/shared").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_ADDRBOOK, Path Info,OEM
   string, Address Book Directory
   HELP
      The path to the address book directory (for example, "brew/addrbook").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_MIF, Path Info,OEM
   string, MIF Directory
   HELP
      The path to MIFs (for example, "brew/mif").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_LOGO, Path Info,OEM
   string, Base UI Directory
   HELP
      The path to the base-level UI (for example, "ui/base").  Enter "NULL" if none.  

      NOTE:  This value should be the same as the value returned from OEM_GetPath().
   }
}
Resource, IDS_DD_PATH_SYSTEM, Path Info,OEM
   string, System Directory
   HELP
      The path to the brew system directory.
   }
}
Resource, IDS_DD_PATH_PICTURE, Media Content File (MCF) Directories
   string, Picture Directory
	 HELP
	    The path to the MCF Picture directory. 


	    Enter the full path including "fs:" (that is, 
            "fs:/~0x0102382c" not "~0x0102382c").
	 }
}
Resource, IDS_DD_PATH_MUSIC, Media Content File (MCF) Directories
   string, Music Directory
	 HELP
	    The path to the MCF Music directory.

  
	    Enter the full path including "fs:" (that is, "fs:/~0x0103201e" 
            not "~0x0103201e").
	 }
}
Resource, IDS_DD_PATH_MOVIE, Media Content File (MCF) Directories
   string, Movie Directory
	 HELP
	    The path to the the MCF Movie directory.


            Enter the full path including "fs:" (that is, "fs:/~0x0102382e" 
            not "~0x0102382e").
	 }
}
Resource, IDS_DD_PATH_THEME, Media Content File (MCF) Directories
   string, Theme Directory
	 HELP
	    The path to the MCF Theme directory.
      
            Enter the full path including "fs:" (that is, "fs:/~0x0102382f" 
            not "~0x0102382f").
	 }
}
Resource, IDS_DD_PATH_TEXT, Media Content File (MCF) Directories
   string, Text Directory
	 HELP
	    The path to the MCF Text directory.

            Enter the full path including "fs:" (that is, "fs:/~0x01023830" 
            not "~0x01023830").
	 }
}
Resource, IDS_DD_PATH_SOUND, Media Content File (MCF) Directories
   string, Sound Directory
	 HELP
	    The path to the MCF Sound directory.


            Enter the full path including "fs:" (that is, "fs:/~0x0102e5e7" 
            not "~0x0102e5e7").
	 }
}
Resource, IDS_DD_PATH_LINK, Media Content File (MCF) Directories
   string, Link Directory
	 HELP
	    The path to the MCF Link directory.


            Enter the full path including "fs:" (that is, "fs:/~0x0102e5e7" 
            not "~0x0102e5e7").
	 }
}

Resource, IDS_DD_PATH_PICTURE_LOCKED, Media Content File (MCF) Directories
   string, Picture Locked Directory
	 HELP
	    The path to the MCF Picture Locked directory. 


	    Enter the full path including "fs:" (that is, 
            "fs:/~0x0102e5fc" not "~0x0102e5fc").
	 }
}
Resource, IDS_DD_PATH_MUSIC_LOCKED, Media Content File (MCF) Directories
   string, Music Locked Directory
	 HELP
	    The path to the MCF Music Locked directory.

  
	    Enter the full path including "fs:" (that is, "fs:/~0x0102382d" 
            not "~0x0102382d").
	 }
}
Resource, IDS_DD_PATH_MOVIE_LOCKED, Media Content File (MCF) Directories
   string, Movie Locked Directory
	 HELP
	    The path to the the MCF Movie Locked directory.


            Enter the full path including "fs:" (that is, "fs:/~0x0102e5fd" 
            not "~0x0102e5fd").
	 }
}
Resource, IDS_DD_PATH_THEME_LOCKED, Media Content File (MCF) Directories
   string, Theme Locked Directory
	 HELP
	    The path to the MCF Theme Locked directory.
      
            Enter the full path including "fs:" (that is, "fs:/~0x01031fd5" 
            not "~0x01031fd5").
	 }
}
Resource, IDS_DD_PATH_TEXT_LOCKED, Media Content File (MCF) Directories
   string, Text Locked Directory
	 HELP
	    The path to the MCF Text Locked directory.

            Enter the full path including "fs:" (that is, "fs:/~0x01031fd6" 
            not "~0x01031fd6").
	 }
}
Resource, IDS_DD_PATH_SOUND_LOCKED, Media Content File (MCF) Directories
   string, Sound Locked Directory
	 HELP
	    The path to the MCF Sound Locked directory.


            Enter the full path including "fs:" (that is, "fs:/~~0x0102e5fb" 
            not "~~0x0102e5fb").
	 }
}
Resource, IDS_DD_PATH_LINK_LOCKED, Media Content File (MCF) Directories
   string, Link Locked Directory
	 HELP
	    The path to the MCF Link Locked directory.


            Enter the full path including "fs:" (that is, "fs:/~0x0103201c" 
            not "~0x0103201c").
	 }
}
Resource, IDS_DD_PATH_LICENSE_LOCKED, Media Content File (MCF) Directories
   string, License Directory
	 HELP
	    The path to the MCF License directory.


            Enter the full path including "fs:" (that is, "fs:/~0x010315fa" 
            not "~0x010315fa").
	 }
}

#<!--=================================================================
#	BREW Distribution System Config Info
#=================================================================-->
EnumList, IDEL_AKEY_BKEY
   0, Use BKey
   1, Use AKey
   not supported, not supported
}
EnumList, IDEL_YES_NO_NOTSUPPORTED
   0, No
   1, Yes
   not supported, not supported
}
EnumList, IDEL_AUTH_POLICY
   APOLICY_NONE, APOLICY_NONE
   APOLICY_SID, APOLICY_SID
   APOLICY_TEXT, APOLICY_TEXT
   APOLICY_NUM, APOLICY_NUM
   not supported, not supported
}
EnumList, IDEL_APP_EXEC_POLICY
   PPOLICY_BREW, PPOLICY_BREW
   PPOLICY_CARRIER, PPOLICY_CARRIER
   PPOLICY_BREW_AND_CARRIER, PPOLICY_BREW_AND_CARRIER
   PPOLICY_BREW_OR_CARRIER, PPOLICY_BREW_OR_CARRIER
   not supported, not supported
}
Resource, IDS_DD_BP_ADS_URL, Configuration Info
   string, BREW ADS IP/URL, , |
   HELP
      Enter the IP address or URL of the BREW ADS.  If more than one server,
      separate them with commas.

      NOTE:  This is the szServer value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_BP_DNS1, Configuration Info
   string, Primary DNS
   HELP
      Enter the IP address of the primary domain name server (DNS).

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_DNS_IP1.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_BP_DNS2, Configuration Info
   string, Secondary DNS
   HELP
      Enter the IP address of the secondary domain name server (DNS).

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_DNS_IP2.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_BP_CARR_ID, Configuration Info
   uint32, Carrier ID
   HELP
      Enter the Carrier ID.

      NOTE:  This is the dwCarrierID value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_BP_BKEY, Configuration Info
   string, BKey
   HELP
      Enter the BKEY as a series of decimal values separated by spaces (for example, 
      "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0").

      NOTE:  This is the BKey value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_BP_ABKEY_SPECIFIER, Configuration Info
   enum, AKey/BKey specifier, IDEL_AKEY_BKEY
   HELP
      Select whether AKey or BKey is used.  

      NOTE:  This is the DIF_USE_A_KEY bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".  
      If OEM_GetConfig() does not return EUNSUPPORTED and DIF_USE_A_KEY is not explicity set,
      BREW will assume a value of BKey.
   }
}
Resource, IDS_DD_BP_SID_MIN, Configuration Info
   enum, Use MIN for SID, IDEL_LABEL_YESNO
   HELP
      Use the Mobile Identification Number (MIN) for the Subscriber ID (SID)?

      NOTE:  This is the DIF_MIN_FOR_SID bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_PREPAY, Configuration Info
   enum, Prepay, IDEL_LABEL_YESNO
   HELP
      Is this a prepay phone?

      NOTE:  This is the DIF_PREPAY bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_NO_AUTO_ACK, Configuration Info
   enum, No Auto ACK, IDEL_LABEL_YESNO
   HELP
      Is the no auto ACK flag set in the CFGI_DOWNLOAD wFlags parameter?

      NOTE:  This is the DIF_NO_AUTO_ACK bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_SID_ENCODE, Configuration Info
   enum, SID Encode, IDEL_LABEL_YESNO
   HELP
      Use SID rather than ESN for application encoding?  (Used only for RUIM devices.)

      NOTE:  This is the DIF_SID_ENCODE bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_SID_VALIDATE_ALL, Configuration Info
   enum, SID Validate All, IDEL_LABEL_YESNO
   HELP
      Validate all applications rather than just subscription applications?  (Used only for RUIM devices.)

      NOTE:  This is the DIF_SID_VALIDATE_ALL bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_RUIM_DEL_OVERRIDE, Configuration Info
   enum, RUIM Delete Override, IDEL_LABEL_YESNO
   HELP
      Allow one RUIM user to delete applications owned by another user?  (Used only for RUIM devices.)

      NOTE:  This is the DIF_RUIM_DEL_OVERRIDE bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_AUTO_UPGRADE, Configuration Info
   enum, Auto Upgrade, IDEL_LABEL_YESNO
   HELP
      Perform automatic upgrade when new uses purchased?

      NOTE:  This is the DIF_AUTO_UPGRADE bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_BP_AUTH_FLAG, Configuration Info
   enum, User Authentication Policy, IDEL_AUTH_POLICY
   HELP
      APOLICY_NONE: No authentication required.

      APOLICY_SID: User's "subscriber ID" is passed to ADS before any set of transactions started.

      APOLICY_TEXT: User should be prompted for text "key" and this sent to ADS.

      APOLICY_NUM: User should be prompted for numeric "key" and this sent to ADS.

      NOTE:  This is the nAuth value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "not supported".
   }
}
Resource, IDS_DD_BP_APP_EXEC_PLCY, Configuration Info
   enum, Application Execution Policy, IDEL_APP_EXEC_POLICY
   HELP
      Application Execution Policy decides which type of applications are allowed to execute.

      PPOLICY_BREW: TRUE-BREW-signed applet only.
      
      PPOLICY_CARRIER: Carrier-signed applet only.

      PPOLICY_BREW_AND_CARRIER: TRUE-BREW- and carrier-signed applet only.

      PPOLICY_BREW_OR_CARRIER: TRUE-BREW- or carrier-signed applet.

      NOTE:  This is the nPolicy value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "not supported".
   }
}
Resource, IDS_DD_BP_SID, Configuration Info
   string, Subscriber ID
   HELP
      Enter the Subscriber ID in decimal format (for example, 8585551212).

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_SUBSCRIBERID.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_DEV_IMEI, Configuration Info
   string, Device IMEI
   HELP
      Enter the IMEI for the device as a 15-digit number.  The IMEI is a device serial number
      for devices that support GSM or UMTS.
      
      NOTE:  This is the value returned by OEM_GetDeviceInfoEx() for item
      AEE_DEVICEITEM_IMEI.  If OEM_GetDeviceInfoEx() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_DEV_ESN, Configuration Info
   string, Device ESN
   HELP
      Enter the ESN for the device as an 8-digit hexadecimal number preceded by 0x. The ESN can 
      also be specified as a 10-digit decimal number (for example, 0xbabababa OR 3132799674).
      
      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_MOBILEINFO.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
   }
}
Resource, IDS_DD_DEV_MEID, Configuration Info
   string, Device MEID
	 HELP
      Enter the MEID for the device as a 14-digit hexadecimal number.  The MEID is a device 
      serial number for devices that support CDMA2000 standards.

      NOTE:  This is one of the values returned by OEM_GetDeviceInfoEx() for item
      AEE_DEVICEITEM_HWID.  If OEM_GetConfig() returns EUNSUPPORTED, enter "not supported".
	 }
}	    
Resource, IDS_DD_BREWDIR_SMS_TELESERVICE, Configuration Info
   uint32, Teleservice ID
   HELP
      Enter the Teleservice ID of BREW-directed SMS.  The default is 4098.  If the teleservice ID
      is not supported as part of BREW-directed SMS, enter "0".
   }
}
Resource, IDS_DD_BP_ALLOW_3G_2G_FAILOVER, Configuration Info
   enum, 3G to 2G Failover, IDEL_LABEL_YESNO
   HELP
      If the device attempts a packet data call and fails, does the device
      failover to an IS-95 data and retry?

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_ALLOW_3G_2G_FAILOVER.  If OEM_GetConfig() returns EUNSUPPORTED, 
      enter "No".
   }
}
Resource, IDS_DD_BP_APPX5_ENABLED, Configuration Info
   enum, APPX5 Support, IDEL_LABEL_YESNO
   HELP
      Does the device support the APPX5 mechanism for position location services?  This corresponds
      to the item CFGI_APPX5_ON.  If OEM_GetConfig() returns EUNSUPPORTED, enter "No".
   }
}
Resource, IDS_DD_BP_MPC_ADDR, Configuration Info
   string, Mobile Positioning Center (MPC) Address
   HELP
      The IP address of the APPX5 MPC.

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_APPX5_MPC_ADDR.  If OEM_GetConfig() returns EUNSUPPORTED, 
      enter "not supported".
   }
}
Resource, IDS_DD_BP_MPC_PORT, Configuration Info
   uint32, Mobile Positioning Center (MPC) Port
   HELP
      The IP port of the APPX5 MPC.

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_APPX5_MPC_PORT.  If OEM_GetConfig() returns EUNSUPPORTED, 
      enter "0".
   }
}
Resource, IDS_DD_BP_DEST_NUM_EMAIL_SMS, Configuration Info
   string, Destination Number for EMail directed SMS
   HELP
      For device-originated SMS messages whose destination is an email address, carriers
      can require that the email be sent to predefined number rather than the actual
      destination address. These email messages would then be forwarded to the proper destination address.  
      If the device is configured to use this feature, enter the number; otherwise, enter "0".
   }
}
Resource, IDS_DD_BP_DORMANCY_TIMEOUT_DEFAULT, Configuration Info
   uint32, Mobile Originated Dormancy Timeout
   HELP
      Enter the duration of inactivity that the device waits before initiating dormancy 
      on a BREW-initiated packet data call.  This value is not used
      by BREW, but used by the device.  The unit of measure is milliseconds.

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_DORMANCY_TIMEOUT_DEFAULT.  If OEM_GetConfig() returns EUNSUPPORTED, enter "0".
   }
}
Resource, IDS_DD_BP_GPSONE_LOCATION_SERVER_NAME1, Configuration Info
   string, Location Server 1
   HELP
      Fully-qualified name of the first default location server used by the
      position determination process. This value is used if the application 
      does not explicitly specify a location server.  

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_GPSONE_LOCATION_SERVER_NAME1.  If OEM_GetConfig() returns EUNSUPPORTED, 
			enter "not supported".
   }
}
Resource, IDS_DD_BP_GPSONE_LOCATION_SERVER_NAME2, Configuration Info
   string, Location Server 2
   HELP
      Fully-qualified name of the second default location server used by the
      position determination process. This value is used if the first location
      server's name can't be resolved.  

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_GPSONE_LOCATION_SERVER_NAME2.  If OEM_GetConfig() returns EUNSUPPORTED, 
      enter "not supported".
	 }
}
Resource, IDS_DD_BP_SMS_GATEWAY_ADDRESS_GSM, Configuration Info
   string, Gateway Service Center Address for GSM Systems
   HELP
      The Service Center IP Address used by the device to send mobile-originated
      SMS messages on GSM systems.

      NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_SMS_GW_SCADDRESS.  If OEM_GetConfig() returns EUNSUPPORTED, 
      enter "not supported".
	 }
}

EnumList, IDEL_OAT_TYPE_LIST
   0, Boolean
   1, Int32
   2, UnsignedInt32
   3, String
   4, Wide String
   5, Verify
	 6, Unsupported
}
#<!--_______________________________________
#                Configuration Automated
#      ID Range: 6000+
#      IDS_DD_CONFIG_NUMBER_ITEMS: 6000  
#_______________________________________-->
Resource, IDS_DD_CONFIG_NUMBER_ITEMS, Configuration Automated,OEM
   uint32, Number of Automated Configuration Testing Items
	 HELP
	    Enter the number of the item to be tested using the Automated Configuration tests.
			
	    NOTE: The ID of the entries for automated test must be contiguous starting after
	    IDS_DD_CONFIG_NUMBER_ITEMS.
	 }
}

#<!--_______________________________________
#                Device Info Automated
#      ID Range: 7000+
#      IDS_DD_CONFIG_NUMBER_ITEMS: 7000  
#_______________________________________-->
Resource, IDS_DD_DEVICE_INFO_NUMBER_ITEMS, Device Information,OEM
   uint32, Number of Automated Device Info Testing Items
	 HELP
	    Enter the number of the item to be tested using the Automated Configuration tests.
			
            NOTE: the ID of the entries for automated test must be contiguous starting after
            IDS_DD_DEVICE_INFO_NUMBER_ITEMS.
	 }
}
Resource, IDS_DD_DEVICE_MANUFACTURER_NAME, Device Information,OEM
   complex, Manufacturer Name 
      enum, Type, IDEL_OAT_TYPE_LIST
      uint32, ID
      string, Value
   }
	 HELP
            Enter the Manufacturer Name as follows:

		TYPE is the type of information that the Device Information requested is stored in.
			
		ID is the ID number of information.

		VALUE is the value of the information to be compared to; enter "0" if the TYPE is verify.

		Note: For structures, use verify as the type and it will print out the structure as a hexadecimal
		string.
   }
}	
Resource, IDS_DD_DEVICE_MODEL_NAME, Device Information,OEM
   complex, Model Name
      enum, Type, IDEL_OAT_TYPE_LIST
      uint32, ID
      string, Value
   }
	 HELP
            Enter the Model Name as follows:

		TYPE is the type of information that the Device Information requested is stored in.

		ID is the AEEDeviceItem number of information.

		VALUE is the value of the information to be compared to; enter "0" if the TYPE is verify.

		Note: For structures, use verify as the type and it will print out the structure as a hexadecimal
			string.
   }
}	
Resource, IDS_DD_DEVICE_FRIENDLY_NAME, Device Information,OEM
   complex, Friendly Name
      enum, Type, IDEL_OAT_TYPE_LIST
      uint32, ID
      string, Value
   }
	 HELP
          Enter the Friendly Name as follows:

	  Type is the type of information that the Device Information requested is stored in.

          ID is the AEEDeviceItem number of information.

          Value is the value of the information to be compared to; not required if the TYPE is verify.

          Note: For structures, use verify as the type and it will print out the structure as a hexadecimal string.
   }
}
Resource, IDS_DD_DEVICE_POSDET_EMERGENCY_ONLY, Device Information,OEM
   complex, Position Detection Emergency Only
      enum, Type, IDEL_OAT_TYPE_LIST
      uint32, ID
      string, Value
   }
	 HELP
            Can Position Detection only be used for Emergency calls?
	 
		TYPE is the type of information that the Device Information requested is stored in.
		
		ID is the AEEDeviceItem number of information.

		VALUE is the value of the information to be compared to; not required if the TYPE is verify.

		Note: For structures, use verify as the type and it will print out the structure as a hexadecimal
		string.
   }
}	
Resource, IDS_DD_FOTA_SUPPORTED, FOTA Support,OEM
   enum, Is FOTA Supported, IDEL_YESNO
   HELP
      Select "Yes" if FOTA is supported; otherwise, "No".
   }
}
Resource, IDS_DD_FOTA_FILETOCOPY, FOTA Support,OEM
   string, File path name which needs to be copied to FOTA partition
   HELP
      Name of the file that needs to be copied to the FOTA partition.
      If the file specified is fs:/shared/tmpdata, the tmpdata file is
      created with dummy data before writing to the FOTA partition.
   }
}
Resource, IDS_DD_SERVER_FOTA_SUPPORTED, FOTA Support,OEM
   enum, Does Server support FOTA file download, IDEL_YESNO
   HELP
      Select "Yes" if the download server supports FOTA file downloads; otherwise, "No".
   }
}
Resource, IDS_DD_DEVICE_UIONE, uiOne Device,OEM
   string, Customized uiOne 1
   HELP
      State the string based on the uiOne customization. 

      Supported  - Execute all generic tests.

      None - Not a uiOne Device.
   }
}
