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
0, Last
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
      Base version of BREW on the device (for example, 3.1.4).  This version should not include any 
      patches or feature packages.  Patches and feature packages are specified in the Patch Name field.
   }
}
Resource, IDS_DD_DEV_CHIPSET, Device Model Info
   string, Chipset
   HELP
	<Definition>
	    Chipset in the device
	<Instructions>
      Enter the chipset used by this device (for example, 6100).      
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
Resource, IDS_DD_PORT_NAMES, Device Model Info
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
   string, BTIL Version
   HELP
	<Definition>
	    BTIL Version supported by the device
	<Instructions>
      Enter the version of BTIL (BREW Tools Interface Layer) used if the device supports BTIL. If the device
      does not support BTIL, enter "0".
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
   enum, OpenGL ES Version enabeled, IDEL_OPENGL_VERSION
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
    Enter the duration of the linger timer on the device in milliseconds.
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
	<Instructions>
    Does the device support 1xEV-DO (HDR)?
   }
}
Resource, IDS_DD_QOS_SUPPORT, Network/Socket Specification,Developer|OEM
   enum, QoS Support, IDEL_LABEL_YESNO
   HELP
	<Instructions>
      Does the device support Quality of Service?
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
Resource, IDS_DD_HW_EFS_MIN_PATH, Memory
   uint32, File Name Length
   HELP
      The maximum number of characters that can be included in a BREW filename or path from the ".../brew/mod/"
      directory.  

      For example, the number of characters are counted as follows for the filename or path 
      ".../brew/mod/dir1/filename.txt": 

      Counting each character after ".../brew/mod/", the number of characters equals 17 for "dir1/filename.txt". 
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

      If there is no secondary display, leave this entry blank.
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

      If there is no secondary display, leave this entry blank.
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

      If there is no secondary display, enter "0".
   }
}
Resource, IDS_DD_LCD3_TYPE, Display 3
   string, Type
   HELP 
	<Definition>
	    The display's LCD type.
	<Instructions>
      Enter the display's LCD type (for example, TFT, STN, etc.).

      If there is no secondary display, leave this entry blank.
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

      If there is no secondary display, leave this entry blank.
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

      If there is no secondary display, enter "0".
   }
}
Resource, IDS_DD_LCD4_TYPE, Display 4
   string, Type
   HELP
	  <Definition>
		  The display's LCD type.
	  <Instructions>
      Enter the display's LCD type.  For example, TFT, STN, etc.

      If there is no secondary display, leave this entry blank.
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

      If there is no secondary display, leave this entry blank.
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
      What is the RGB value of the backlight on the device? 

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
	<Instructions>
      Does the device support Key tones?
      If yes, select "Yes"; otherwise, select "No".   
   }
}
EnumList, IDEL_KEYCODES
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

Resource, IDS_DD_AUD_STR_SPK, Audio
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

#<!--=================================================================
#        Media Formats
#=================================================================-->
Resource, IDS_DD_MULTIMEDIA_VERSION, Media Types
   string, Multimedia File Version
	HELP 
	<Definition>
	   Non-CMX Multimedia file format support and exceptions
	<Instructions>
      Does the device support a non-CMX Multimedia file format? If so, enter
      the format and version number. Also, enter any types not supported in the
      Comment field.
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
      List all text input modes available in the BREW text input control.
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
      glyph range, using the format <starting glyph >-<ending glyph> (for
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
      Is GPS-based position determination available to BREW?
   }
}
Resource, IDS_DD_POS_SECTOR, Position Determination
   enum, Sector-based Supported, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Sector-based position determination supported for BREW
	<Instructions>
      Is sector-based position determination available to BREW?
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
Resource, IDS_DD_BLUETOOTH_SUPP, Bluetooth
   enum, Bluetooth Supported, IDEL_LABEL_YESNO
   HELP 
	<Definition>
	    Bluetooth Headset support.
	<Instructions>
      Are Bluetooth headsets supported as an ISound device type by the
			device?
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

      ID - The AEESoundPlayerFile ID for the format (decimal or hex) (for
      example, 128 or 0x80).
      
      File Extension -  The filename extension associated with the ID (for
      example, for an ID of 1 you would typically enter "mid").

      Note 1: pmd = CMX: Compact MIDI, SAF, PNG, JPEG, QCELP, ADPCM,MPEG4=AAC Video.
      
      Note 2: The ID (1, 2, and 3 in the example) is the value returned in
      pwFormats by IRINGERMGR_GetFormats().
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
Resource, IDS_AEECLSID_CAMERA, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      ICamera class ID configuration.
   }
}
Resource, IDS_AEECLSID_CAMERA2, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      ICamera class ID configuration.
   }
}
Resource, IDS_AEECLSID_CAMERA3, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      ICamera class ID configuration.
   }
}
Resource, IDS_AEECLSID_CAMERA4, Camera Services
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      ICamera class ID configuration.
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
      Enter a comma separated of all applicable MIME types for Power ON/OFF services available to BREW (for example, bmp)
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
	HELP 
	    Select "Yes" for all the possible directories where Power On/Off files can be stored?
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
      Does the device support a BREW-only, Native UI-only, or both BREW
      and Native UI screen savers?
   }
}
Resource, IDS_DD_SSAVER_TIMER, BREW Screen Saver
   uint32, Timer
   HELP 
      After what period of inactivity will the screen saver be activated?
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
      This is a medium image .

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
      NumFlips: Number of flips on the device.
      
      FlipID - ID of the flip
      
      FlipPosType - Position type of the flip, see IDEL_FLIP_POSITION_TYPE.
            
      FlipCapabilityMask - OR-ed mask of all flip capabilities.
      Flip capabilities are:
      
      1,Flip
      
      2,KeyGuard
      
      16,ScreenRotate   

      See also IDEL_FLIP_CAPABILITIES.
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
   }
}
Resource, IDS_DD_FLIP_COMMON_PARM_LIST, Flip Common Parmeter List,Developer|OEM
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
      Lists the key codes of keys accessible on the device when the first
	  flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_2_OPEN_KEY_LIST, Flip 2 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      Lists the key codes of keys accessible on the device when the 
      second flip is fully open and all other flips are closed.
      
   }
}
Resource, IDS_DD_FLIP_3_OPEN_KEY_LIST, Flip 3 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      Lists the key codes of keys accessible on the device when a 
      specific flip is fully open and all other flips are closed.
      
     
   }
}
Resource, IDS_DD_FLIP_4_OPEN_KEY_LIST, Flip 4 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys When Flip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      Lists the key codes of keys accessible on the device when a 
      specific flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_5_OPEN_KEY_LIST, Flip 5 Open Key List,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Keys WhenFlip Open
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP  
      Lists the key codes of keys accessible on the device when a 
      specific flip is fully open and all other flips are closed.
      
   }
}
Resource, IDS_DD_FLIP_CLOSED_KEY_LIST,All Flip Closed Key List,Developer|OEM
   complex,Keys Accessible When Flips Closed
      enum,KeyCode,IDEL_KEYCODES
   }
   HELP 
      Lists the key codes of keys accessible on the device when
      all flips are fully closed.
   }
}
Resource, IDS_DD_FLIP_1_OPEN_DISPLAY_LIST, Flip 1 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      Lists the display class IDs of displays accessible on the device when 
      the first flip is fully open and all other flips are closed.
   }
}
Resource, IDS_DD_FLIP_2_OPEN_DISPLAY_LIST, Flip 2 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      Lists the display class IDs of displays accessible on the device when 
      the second flip is fully open and all other flips are closed.

   }
}
Resource, IDS_DD_FLIP_OPEN_3_DISPLAY_LIST, Flip 3 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      Lists the display class IDs of displays accessible on the device when 
      the third flip is fully open and all other flips are closed. 
   }
}
Resource, IDS_DD_FLIP_4_OPEN_DISPLAY_LIST, Flip 4 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      Lists the display class IDs of displays accessible on the device when 
      the fourth flip is fully open and all other flips are closed. 
   }
}
Resource, IDS_DD_FLIP_5_OPEN_DISPLAY_LIST, Flip 5 Open Displays,Developer|OEM
   enum,FlipID,IDEL_FLIP_ID
   complex,Open Display List
      enum,ClassID,IDEL_DISPLAY_CLSID
   }
   HELP 
      Lists the display class IDs of displays accessible on the device when 
      the fifth flip is fully open and all other flips are closed. 
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
      Lists the display class IDs of displays accessible on the device when 
      all flips are fully closed.
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
      Enter a comma separated list of all the MIDI MIME Types supported on the device.
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
      Enter a comma separated list of all the MP3 MIME Types supported on the device.
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
      Enter a comma separated list of all the QCP MIME Types supported on the device.
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
      Enter a comma separated list of all the PMD MIME Types supported on the device.
   }
}
Resource, IDS_DD_MMEDIA_PMD, PMD Configuration
   enum, CMX, IDEL_LABEL_YESNO
   HELP 
     <Definition>
	  CMX Version supported and exceptions
	<Instructions>
      Is the CMX (.pmd) Multimedia File Format (Compact MIDI, SAF, PNG, JPEG,
      QCELP, ADPCM) supported on this device? If so, enter the CMX version
			supported by the device in the Comment field. Enter any types not supported
			in the Comment field.
			
      
      NOTE: CMX needs to be supported through BREW and the device's native
      UI.  CMX files must be playable as ringers, wallpapers, and screensavers.
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
      Enter a comma separated list of all the MPEG4 MIME Types supported on the device.
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
      Enter a comma separated list of all the MMF MIME Types supported on the device.
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
      Enter a comma separated list of all the PHRASE MIME Types supported on the device.
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
      Enter a comma separated list of all the ADPCM MIME Types supported on the device.
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
      Enter a comma separated list of all the IMELODY MIME Types supported on the device.
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
      Enter a comma separated list of all the AMR MIME Types supported on the device.
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
      Enter a comma separated list of all the AAC MIME Types supported on the device.
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
      Enter a comma separated list of all the BMP MIME Types supported on the device.
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
      Enter a comma separated list of all the GIF MIME Types supported on the device.
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
      Enter a comma separated list of all the PNG MIME Types supported on the device.
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
      Enter a comma separated list of all the JPEG MIME Types supported on the device.
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
      Enter a comma separated list of all the BCI MIME Types supported on the device.
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
      Enter a comma separated list of all the PCM MIME Types supported on the device.
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
Resource, IDS_AEECLSID_VOCODER, Vocoder Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   complex, Vocoders
   enum, Vocoder, IDEL_VOCODERS_SUPPORTED
   }    
   HELP 
      IVocoder class ID configuration.
   }
}
Resource, IDS_AEECLSID_JOYSTICK, Joystick Configuration
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      IJoystick class ID configuration.
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
Resource, IDS_AEECLSID_LAST, Class ID Configuration,NONE
   enum, CLSID, IDEL_CLSID
   enum, Supported, IDEL_LABEL_YESNO
   HELP 
      Do not edit this item.
   }
}

