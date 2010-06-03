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


EnumList, IDEL_UIONE_PROPERTIES
256,PROP_BORDERWIDTH               
257,PROP_ACTIVE_BORDERWIDTH        
258,PROP_INACTIVE_BORDERWIDTH      
259,PROP_BORDERRADIUS              
272,PROP_BORDERCOLOR               
273,PROP_ACTIVE_BORDERCOLOR        
274,PROP_INACTIVE_BORDERCOLOR      
275,PROP_SACTIVE_BORDERCOLOR       
276,PROP_SINACTIVE_BORDERCOLOR     
277,PROP_SELECTED_BORDERCOLOR      
281,PROP_BORDERSTYLE               
288,PROP_PADDING                   
289,PROP_LEFT_PAD                  
290,PROP_RIGHT_PAD                 
291,PROP_TOP_PAD                   
292,PROP_BOTTOM_PAD                
304,PROP_BGCOLOR                   
305,PROP_ACTIVE_BGCOLOR            
306,PROP_INACTIVE_BGCOLOR          
307,PROP_SACTIVE_BGCOLOR           
308,PROP_SINACTIVE_BGCOLOR         
309,PROP_SELECTED_BGCOLOR         
310,PROP_TEXT_SACTIVE_BGCOLOR      
311,PROP_TEXT_SINACTIVE_BGCOLOR    
312,PROP_TEXT_SELECTED_BGCOLOR     
320,PROP_FGCOLOR                   
321,PROP_ACTIVE_FGCOLOR            
322,PROP_INACTIVE_FGCOLOR          
323,PROP_SACTIVE_FGCOLOR           
324,PROP_SINACTIVE_FGCOLOR         
325,PROP_SELECTED_FGCOLOR          
326,PROP_TEXT_SELECTED_FGCOLOR     
327,PROP_TEXT_SACTIVE_FGCOLOR      
328,PROP_TEXT_SINACTIVE_FGCOLOR    
336,PROP_TRANSP_COLOR              
337,PROP_SELECTED                  
338,PROP_FONT                      
339,PROP_FLAGS                     
340,PROP_OFFSETX                   
341,PROP_OFFSETY                   
342,PROP_INCREMENT                 
343,PROP_TRANSPARENCY              
344,PROP_BGTRANSPARENCY            
345,PROP_BORDERTRANSPARENCY        
352,PROP_ROTATE                    
353,PROP_VIEWMODEL                 
354,PROP_CARETPOS                  
355,PROP_INDEXER                   
356,PROP_SELECTINDEX               
357,PROP_FOCUSINDEX                
358,PROP_TOPINDEX                  
359,PROP_ITEMHEIGHT                
360,PROP_ITEMWIDTH                 
361,PROP_ROWS                      
368,PROP_COLS                      
369,PROP_HINT_ROWS                 
370,PROP_HINT_COLS                 
371,PROP_HINT_WIDTH                
372,PROP_ITEMRECT                  
373,PROP_VARITEMSIZE               
374,PROP_SELITEMSIZE               
384,PROP_SCROLLHANDLEWIDTH         
385,PROP_SCROLLPAD                 
386,PROP_SCROLLSTYLE               
387,PROP_SCROLLREQ                 
388,PROP_ACTIVE_SCROLLCOLOR        
389,PROP_INACTIVE_SCROLLCOLOR      
390,PROP_SCROLLCOLOR              
391,PROP_ACTIVE_SCROLLHANDLECOLOR  
392,PROP_INACTIVE_SCROLLHANDLECOLOR
393,PROP_SCROLLHANDLECOLOR         
400,PROP_SCROLLFILLCOLOR           
401,PROP_SCROLLBARRECT             
402,PROP_VSCROLLBARRECT            
403,PROP_HSCROLLBARRECT            
404,PROP_SCROLLHANDLERECT          
405,PROP_VSCROLLHANDLERECT         
406,PROP_HSCROLLHANDLERECT         
407,PROP_SCROLLFILLRECT            
408,PROP_VSCROLLFILLRECT           
409,PROP_HSCROLLFILLRECT           
512,PROP_SOFTKEY1                  
513,PROP_SOFTKEY2                  
514,PROP_SHADOWTRANSPARENCY        
515,PROP_SHADOWOFFSET              
516,PROP_SHADOWOFFSETX             
517,PROP_SHADOWOFFSETY             
518,PROP_SHADOWCOLOR               
519,PROP_LAYOUTSTYLE               
520,PROP_LAYOUTFLAGS               
528,PROP_GRADIENT_STYLE            
529,PROP_CURSOR_SHOW               
530,PROP_CURSOR_DISMISS            
531,PROP_IMAGESTATIC_IMAGEWIDGET   
532,PROP_IMAGESTATIC_STATICWIDGET  
533,PROP_IMAGE_PARM                
534,PROP_ANIMATE                   
534,PROP_IMAGE_ANIMATE      
535,PROP_SLIDER_MAXPOS             
536,PROP_SLIDER_HANDLEBORDERCOLOR  
537,PROP_LIMITTEXT                 
544,PROP_INPUTMODE                 
545,PROP_STARTLINE                 
546,PROP_PREVLINE                  
547,PROP_NEXTLINE                  
548,PROP_TEXTCONTROLLER            
560,PROP_TEXTCONTROLLERMODEKEY     
561,PROP_READONLY                  
549,PROP_TITLEWIDGET               
550,PROP_SCROLLBARENDPAD           
551,PROP_PASSWORDMASKCHAR          
552,PROP_IMAGESTRIP                
552,PROP_IMAGESTRIP_H              
553,PROP_IMAGESTRIP_V              
568,PROP_GRADIENT                  
569,PROP_SELECTED_GRADIENT         
576,PROP_ACTIVE_GRADIENT           
577,PROP_INACTIVE_GRADIENT         
578,PROP_SACTIVE_GRADIENT          
579,PROP_SINACTIVE_GRADIENT        
592,PROP_SELECTED_SHADOWOFFSET     
593,PROP_SELECTED_SHADOWCOLOR      
594,PROP_SELECTED_SHADOWOFFSETX    
595,PROP_SELECTED_SHADOWOFFSETY    
768,PROP_CARETWIDGET               
769,PROP_CARETPULSE                
770,PROP_PASSWORDMASKDELAY         
771,PROP_LINEGAP                   
772,PROP_SCROLLTEXT                
784,PROP_TABSPACING_VERT           
785,PROP_TABSPACING_HORZ           
786,PROP_ACTIVE_TABWIDTH           
787,PROP_INACTIVE_TABWIDTH         
788,PROP_TABHEIGHT                 
789,PROP_FONT_CLASS                
790,PROP_FONT_OUTLINEWIDTH         
791,PROP_FONT_OUTLINECOLOR         
800,PROP_CLIENTRECT                
816,PROP_LOCALE                    
817,PROP_FORMATSTRING              
818,PROP_DELIMITER                 
819,PROP_FIELDPAD                  
820,PROP_ARROWPAD                  
832,PROP_APPLYWPROPS               
833,PROP_VISIBLE                   
834,PROP_TILED_BDT                 
835,PROP_TILED_BDB                 
836,PROP_TILED_BDL                 
837,PROP_TILED_BDR                 
838,PROP_IMAGE_FRAME               
839,PROP_ANIMATE_FLAGS             
840,PROP_DEFAULTTEXTALIGNMENT      
841,PROP_ELLIPSIS                  
848,PROP_BGIMAGE                   
849,PROP_BGIMAGE_FLAGS             
850,PROP_BGIMAGE_PARM              
851,PROP_BGIMAGE_FRAME             
852,PROP_BGIMAGE_ANIMATE           
853,PROP_BGIMAGE_TILED_BORDER      
854,PROP_BGIMAGE_ANIMATE_FLAGS     
1024,PROP_ROWPROPS                  
1025,PROP_COLPROPS                  
1040,PROP_RAISE                     
1041,PROP_RAISECONTAINER            
1042,PROP_FONTBIDIUTILCLASSID       
1043,PROP_TEXTRECTS                 
1044,PROP_IMAGE_NFRAMES             
1045,PROP_FONTLINEWIDTH             
1046,PROP_CURSOR_MOVE               
1047,PROP_TEXTLAYOUTCLASSID         
1048,PROP_FONTMAPMODEL              
1049,PROP_INPUT_MODES               
1050,PROP_FIELDMODE                 
1051,PROP_IGNORE_KEYS               
1052,PROP_TEXTCONTROLLERSHIFTKEY    
1053,PROP_LINEBREAKONGLYPH          
1280,PROP_BUFFERED                  
1281,PROP_ALPHA                     
1536,PROP_EX                        
4096,PROP_SPECIAL                   
32768,PROP_OEM_SPECIAL              
17036636,PROPEX_SCREEN              
17036500,PROPEX_DIRECT              
17053603,PROPEX_ITEMHEIGHT          
17053604,PROPEX_ITEMWIDTH           
17053605,PROPEX_SELITEMSIZE
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
   c1, c2: Carrier version. c1 is major and c2 is minor number.

   }
}
Resource,IDS_DD_OEM_VERSION, Device Pack Info
   string, Device Pack OEM Version
	 HELP
   This field is updated by OEM only when they update the DevicePack info.

   The format of this string is e1.e2
   where
   e1, e2: OEM version. e1 is major and e2 is minor number.

   }
}

#<!--=================================================================
#       Model Info
#=================================================================-->
Resource, IDS_DD_DEV_PEK_VER, Device Model Info
   string, PEK Version
   HELP
      Enter the PEK version used to test this device including all Patches
			and Feature Packages (for example, 3.1.3U01FP02).
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
      Note that devices intended for carriers supporting LNP should always be configured for LNP during PEK testing.
   }
}
Resource, IDS_DD_CFGI_SUBLCD_PROPS_SUPPORT, Test Support Info, OEM
  enum, SUBLCD Properties, IDEL_LABEL_YESNO
  HELP
      If the device supports the ICONFIG item CFGI_SUBLCD_PROPS, select "Yes".
      Otherwise, select "No".
  }
}
Resource, IDS_DD_CFGI_ANNUN_PROPS_SUPPORT, Test Support Info, OEM
  enum, SUBLCD Properties, IDEL_LABEL_YESNO
  HELP
      If the device supports the ICONFIG item CFGI_ANNUN_PROPS, select "Yes".
      Otherwise, select "No".
  }
}
Resource, IDS_DD_IMMGR_SUPPORT, IMM 
   enum, IMMGR Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IMMGR supported on the device.
	 <Instructions>
	     Enter Yes if IMMGR is implemented on the device
	     Enter No if IMMGR is not implemented on the device
	 }
}
Resource, IDS_DD_IMUI_SUPPORT, IMUI 
   enum, IMUI Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IMUI supported on the device.
	 <Instructions>
	     Enter Yes if IMUI is implemented on the device
	     Enter No if IMUI is not implemented on the device
	 }
}
Resource, IDS_DD_SYSCLOCK_SUPPORT, Test Support Info 
   enum, ISysClock Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     ISysClock supported on the device.
	 <Instructions>
	     Enter Yes if ISysClock is implemented on the device
	     Enter No if ISysClock is not implemented on the device
	 }
}
Resource, IDS_DD_GSM_1X_SUPPORT, Test Support Info
   enum, IGSM1X Support, IDEL_LABEL_YESNO
	 HELP
	 <Definition>
	     IGSM1X interfaces supported on the device.
	 <Instructions>
	     Enter Yes if the IGSM1X interfaces are implemented on the device
	     Enter No if the IGSM1X interfaces are not implemented on the device
	 }
}
Resource, IDS_DD_DEVICE_USBDRIVER_OPEN_VID , Test Support Info
   uint32, IUSBDRIVER Vendor ID when IUSBDriver is opened
   HELP
      Enter the Vendor ID in decimal of the device when it is in IUSBDriver mode.
   }
}
Resource, IDS_DD_DEVICE_USBDRIVER_OPEN_PID , Test Support Info
   uint32, IUSBDRIVER Product ID when IUSBDriver is opened
   HELP
      Enter the Product ID in decimal of the device when it is in IUSBDriver mode.
   }
}
Resource, IDS_DD_DEVICE_USBDRIVER_CLOSED_VID , Test Support Info
   uint32, IUSBDRIVER Vendor ID when IUSBDriver is not opened
   HELP
      Enter the Vendor ID in decimal of the device when it is not in IUSBDriver mode.
   }
}
Resource, IDS_DD_DEVICE_USBDRIVER_CLOSED_PID , Test Support Info
   uint32, IUSBDRIVER Product ID when IUSBDriver is not opened
   HELP
      Enter the Product ID in decimal of the device when it is not in IUSBDriver mode.
   }
}
Resource, IDS_DD_HW_MAX_FSFILL_TIME, Test Support Info, OEM
   uint32, Maximum Time in Minutes to Fillup a Filesystem.
   
   HELP
        <Definition>
           Test cases in OATFS that fillup a filesystem (on Flash, MMC etc) 
           need a timeout value to determine the maximum time it will take
           to fillup a filesystem. The timeout value is specified in minutes.
	      <Instructions>
           Make sure that the timeout value specified is sufficiently large to
           allow for the test cases to finish filling up any filesytem 
           supported on the device. If no value is specified a default value 
           of 200 minutes is used.
   }
}
Resource, IDS_DD_AVAILABLE_KEY_INFO, Test Support Info,OEM
   string, Key Info for Available Key Test in OATDevice, , |
   HELP
	<Definition>
	    Key info for conducting Available Key test in OATDevice.
	<Instructions>
	    Enter a comma separated list as follows:
	    First entry specifies if the test is supported or not. 0 = not supported,
	    1 = supported.
	    Second entry specifies if the keys are available at the start of test.
	    0 = not available, 1 = available.
	    After the first 2 entries enter the comma separated list of all the AVK codes
	    which need to be tested for availability.
	    e.g: 1,1,57377,57378
	    This example will test keys AVK_0 and AVK_1 for availability. At test start the
	    keys are indicated to be available.
   }
}
Resource, IDS_DD_OVERLOADED_KEY_INFO, Test Support Info,OEM
   string, Key Info for Overloaded Key Test in OATDevice, , |
   HELP
	<Definition>
	    Key info for conducting Overloaded Key test in OATDevice.
	<Instructions>
	    Enter a comma separated list as follows:
	    First entry specifies if the test is supported or not. 0 = not supported,
	    1 = supported.
	    After the first entry enter the comma separated list of the AVK code triplets
	    tested for overloading. The AVK code triplets are set as follows (Primary AVK
	    Code, Modifier AVK Code, Resultant AVK Code). So if "Shift" + "a" = "A", the
	    corresponding AVK Codes need to be entered for "a" then "Shift" then "A".
	    e.g: A hypothetical scenario where the following overload combination will be 
	    tested. AVK_0 + AVK_LSHIFT = AVK_1 and AVK_1 + AVK_LSHIFT = AVK_2
	    1,57377,57422,57378,57378,57422,57379
   }
}
Resource, IDS_DD_PRESSED_KEY_INFO, Test Support Info,OEM
   string, Key Info for Pressed Key Test in OATDevice, , |
   HELP
	<Definition>
	    Key info for conducting Pressed Key test in OATDevice.
	<Instructions>
	    First entry specifies if the test is supported or not. 0 = not supported.
	    If test is supported enter the AVK Code of the key to be tested.
	    Second entry specifies if the key to be tested is on external keyboard.
	    e.g: Test AVK_OEM_RANGE_START key code on external keyboard
	    63488,1
   }
}

#<!--=================================================================
#       Memory Info
#=================================================================-->
Resource, IDS_DD_HW_MMC1_SIZE, Memory,OEM
   uint32, Media Card Size Slot 1 (bytes)
   HELP
       Enter the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 1 of the device.
      If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_HW_MMC1_SIZE_VARIANCE, Memory,OEM
   uint32, Media Card Size Measurement Variance Slot 1 (bytes)
   HELP
      Enter the allowable variance of the measurement of the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 1 of the device.
      If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_HW_MMC2_SIZE, Memory,OEM
   uint32, Media Card Size Slot 2 (bytes)
   HELP
      Enter the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 2 of the device.
      If the device does not have a media card slot 2, enter "0".
   }
}
Resource, IDS_DD_HW_MMC2_SIZE_VARIANCE, Memory,OEM
   uint32, Media Card Size Measurement Variance Slot 2 (bytes)
   HELP
      Enter the allowable variance of the measurement of the size of the MMC/RMC (multimedia card/removable media card) inserted in slot 2 of the device.
      If the device does not have a media card slot 1, enter "0".
   }
}
Resource, IDS_DD_MMC_MODELNUM_SERIALNUM_INFO, Memory, OEM
   string, Media Card info, , |
   HELP
      If the reading of model and serial numbers for an MMC/RMC (multimedia card/removable media card) is supported through BREW, enter the comma separated model and serial number for the card. For example, 1,SMD-1,200,2,SMD-1,201, meaning for card 1, the model number is SMD-1 and the serial number is 200, and for card 2, the model number is SMD-1 and the serial number is 201.  Card 1 is treated as fs:/card0, which is the only card currently tested.
   }
}
Resource, IDS_DD_MAXEFS_SPACE_AFTER_FULLWRITE, Memory, OEM
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
Resource, IDS_DD_PATH_ADDRBOOK, Path Info,OEM
   string, Address Book Directory
   HELP
      The path to the address book directory (for example, "brew/addrbook").  Enter "NULL" if none.  

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
      separate each with commas.

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
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, enter "0".
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
Resource, IDS_DD_BP_SENSITIVE_SID, Configuration Info, Carrier|OEM
   enum, IDownload Sensitive SID, IDEL_LABEL_YESNO
   HELP
      Is the SID Sensitive? If so, the device will not send SID information in clear for IDownload.

      NOTE:  This is the DIF_SENSITIVE_SID bit of the wFlags value returned by OEM_GetConfig() for item
      CFGI_DOWNLOAD.  If OEM_GetConfig() returns EUNSUPPORTED, select "No".
   }
}
Resource, IDS_DD_PDP_ID, Configuration Info, Carrier|OEM
   string, IDownload PDP Profile ID
   HELP 
    PDP ID for IDownload

    Enter the PDP ID for IDownload. If not supported, enter "not supported"
   }
}
Resource, IDS_DD_PROXY, Configuration Info, Carrier|OEM
   string, IDownload Proxy Address
   HELP 
    Proxy address for IDownload

    Enter proxy address "http://addr:port" for IDownload. If not supported, enter "not supported"
   }
}
Resource, IDS_DD_PROXYAUTH, Configuration Info, Carrier|OEM
   string, IDownload Proxy Credential
   HELP 
    Proxy Credential for IDownload

    Enter Proxy credential "username:password" for IDownload. If not supported, enter "not supported"
   }
}

Resource, IDS_DD_OEMAUTH_CHALLENGE_CAP, Configuration Info, Carrier|OEM
   string, Device's capability of NN based challenge response 
   HELP 
    Devices capability to support NN based challenge-response by RUIM
    
    Enter "1" if device is capable, otherwise "0".
    If item is not supported, enter "not supported".
   }
}

Resource, IDS_DD_OEMAUTH_CHALLENGE_RESPONSE_LEN, Configuration Info, Carrier|OEM
   string, Maximum length of RUIM generated response to NN challenge
   HELP 
    Maximum length of RUIM generateed response to NN challenge

    Enter the maximum length of response. If not supported, enter "not supported"
   }
}
Resource, IDS_DD_REQ_PIN_ENTRY, Configuration Info,OEM
   string, PIN Required
   HELP
	<Definition>
           IConfig item CFGI_REQUIREPINENTRY
        <Instructions>
           If the device requires a PIN entry for normal phone operation enter "1".
           If the device does not require a PIN entry enter "0"
           If the device does not support CFGI_REQUIREPINENTRY, enter "not supported".
    }
}

Resource, IDS_DD_DOWNLOAD_BUFFER, Configuration Info
uint32, Download Buffer 

   HELP
	<Definition>
           IConfig item CFGI_DOWNLOAD_BUFFER
        <Instructions>
           Enter the size of the download buffer (in bytes) used by the device.
           If the device does not support CFGI_DOWNLOAD_BUFFER, enter "0".
   }
}

Resource, IDS_DD_HTTP_BUFFER, Configuration Info
uint32, HTTP Buffer 

   HELP
	<Definition>
           IConfig item CFGI_HTTP_BUFFER
        <Instructions>
           Enter the size of the http buffer (in bytes) used by the device.
           If the device does not support CFGI_HTTP_BUFFER, enter "0".
   }
}

Resource, IDS_DD_MAX_DISPATCH_TIME, Configuration Info
   string, Maximum Dispatch time
   HELP
	<Definition>
           IConfig item CFGI_MAX_DISPATCH_TIME
        <Instructions>
           Enter the maximum time (in ms) BREW should spend in control of the dispatcher on the device.
           If the device does not supports CFGI_MAX_DISPATCH_TIME enter "not supported".
   }
}

Resource, IDS_DD_MIN_IDLE_TIME, Configuration Info
string,Minimum Idle time
   HELP
	<Definition>
           IConfig item CFGI_MIN_IDLE_TIME
        <Instructions>
           Enter the minimum time (in ms) BREW must relinquish control to the dispatcher on the device.
           If the device does not supports CFGI_MIN_IDLE_TIME enter "not supported".
   }
}
Resource, IDS_DD_SLEEP_TIMER_RESOLUTION, Configuration Info
string, Sleep timer resolution
   
   HELP
	<Definition>
           IConfig item CFGI_SLEEP_TIMER_RESOLUTION
        <Instructions>
           Enter the minimum timer resolution (in ms) when the device is in sleep mode.
           If the device does not supports CFGI_SLEEP_TIMER_RESOLUTION enter "not supported".
   }
}
Resource, IDS_DD_SYSMEM_SIZE, Configuration Info
uint32, System Memory reserved
   
   HELP
	<Definition>
           IConfig item CFGI_SYSMEM_SIZE
        <Instructions>
           Enter the number of bytes reserved for the system when at low available memory on the device.
           If the device does not support CFGI_SYSMEM_SIZE, enter "0".
   }
}
Resource, IDS_DD_MDN, Configuration Info,OEM
   string, Mobile Directory Number
   HELP
	<Definition>
           IConfig item CFGI_MOBILE_DIR_NUM
        <Instructions>
           Enter the Mobile Directory Number of the device.
           If the device does not support CFGI_MOBILE_DIR_NUM enter "not supported".
   }
}
Resource, IDS_DD_CONFIG_ANNUN_PROPS, Configuration Info
   complex, Annunciator properties,0,0,0,| 
      enum, Annuciator Properties, IDEL_UIONE_PROPERTIES
      string, RGBA Value
   }
   HELP
      <Definition>
      The ICONFIG item CFGI_ANNUN_PROPS
      <Instructions>
      For each property supported, enter the property and the corresponding Red,Green,Blue,Alpha (RGBA) values for the Annuciators. For example PROP_FGCOLOR,65280.
      If CFGI_ANNUN_PROPS is not supported, leave blank.
   }
}
Resource, IDS_DD_CONFIG_SUBLCD_PROPS, Configuration Info
   complex, Sub LCD Properties,0,0,0,| 
      enum, Sub LCD Properties, IDEL_UIONE_PROPERTIES
      string, RGBA Value
   }
   HELP
      <Definition>
      The ICONFIG item CFGI_SUBLCD_PROPS
      <Instructions>
      For each property supported, enter the property and the corresponding Red,Green,Blue,Alpha (RGBA) values for the Sub LCD. For example PROP_FGCOLOR,65280.

      If CFGI_SUBLCD_PROPS is not supported, leave blank.
   }
}
Resource, IDS_DD_BUSY_CURSOR_OFFSET,  Configuration Info
   complex, Busy Cursor
	uint32, Dim X
	uint32, Dim Y
	uint32, x
	uint32, y
}
   HELP
	<Definition>
           IConfig item CFGI_BUSY_CURSOR_OFFSET
        <Instructions>
           Enter the width of the busy cursor(hourglass) in Dim X.
           Enter the height of the busy cursor in Dim Y.
           Enter the horizontal position of the busy cursor in x
           Enter the vertical position of the cursor in y.

           If the device does not support CFGI_BUSY_CURSOR_OFFSET, enter "0" for each value.
   }
}
Resource, IDS_DD_DOWNLOAD_FS_AVAIL,  Configuration Info
uint32, Download File System Available
   
   HELP
	<Definition>
           IConfig item CFGI_DOWNLOAD_FS_INFO
        <Instructions>
           Enter the size (in bytes) of the available space for downloads.

           If the device does not support CFGI_DOWNLOAD_FS_INFO, enter "0".
   }
}

Resource, IDS_DD_DOWNLOAD_FS_AVAIL_EX,  Configuration Info
string, Download File System Available on file system larger than 4GB
   
   HELP
	<Definition>
           IConfig item CFGI_DOWNLOAD_FS_INFOEX
        <Instructions>
           Enter the size (in bytes) of the available space for downloads on file system larger than 4GB.

           If the device does not support CFGI_DOWNLOAD_FS_INFO, enter "0".
   }
}
Resource, IDS_DD_CONFIG_RUIM_CARD_ID, Configuration Info,OEM
   string, Card ID
   
   HELP
	<Definition>
           IConfig item CFGI_CLOSE_KEYS
        <Instructions>
           If the device supports RUIM cards, enter the RUIM Card ID in decimal format (i.e. 0xAB325F35).

           If the device does not support RUIM cards, enter "not supported".
   }
}
Resource, IDS_DD_CURR_NAM, Configuration Info,OEM
   string, NAM
   HELP
	<Definition>
           IConfig item CFGI_MOBILEINFO AEEMobileInfo - nCurrNAM
        <Instructions>
           Enter the current NAM for the device.

           If the device does not support CFGI_MOBILEINFO, enter "not supported".
   }
}
Resource, IDS_DD_MOBILE_ID, Configuration Info,OEM
   string, MIN  
   HELP
	<Definition>
           IConfig item CFGI_MOBILEINFO AEEMobileInfo - nCurrNAM
        <Instructions>
           Enter the MIN of the device.

           If the device does not support CFGI_MOBILEINFO, enter "not supported".
   }
}
Resource, IDS_DD_LAZY_ACK, Configuration Info, Carrier|OEM
   enum, Preload and queued ACK behavior, IDEL_YES_NO_NOTSUPPORTED
   HELP 
    Controls device's behavior in regards to flushing preinstall and queued ACKs after device powers up and
    governs whether preinstall and queued ACKs are flushed on powerup or until a data connection is made.
    
    Enter "No" if flushing of ACKs is desired to happen at power up. Enter "Yes" if to delay ACKing until a data connection is made.
    Enter "not supported" if the device does not support the config item: IDS_DD_LAZY_ACK.
    
    NOTE:  This is the value returned by OEM_GetConfig() for item
      CFGI_LAZY_ACK. It is set to TRUE in OEM reference implementation.
   }
}
###################################################################
#
#       FOTA
#
###################################################################
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
      Select "Yes" if the download server supports FOTA file downloads otherwise, select "No".
}
   }   
####################################################################
#
#       UI One
#
####################################################################
Resource, IDS_DD_DEVICE_UIONE, UI One,OEM
   string, Customized uiOne 1
   HELP
      State the string based on the uiOne customization. 

      Supported  - Execute all generic tests.

      None - Not a uiOne Device.
   }
}
Resource, IDS_DD_CONFIG_ANNUN_PROPS_RESTORE, UI One,OEM
   complex, Annunciator properties to restore,0,0,0,| 
      enum, Annuciator Properties, IDEL_UIONE_PROPERTIES
      string, RGBA Value
   }   
   
   HELP
      Enter the property and the Red,Green,Blue,Alpha (RGBA) values for the Annunciator to restore (for example PROP_BGCOLOR,65280).

      

   }
}
Resource, IDS_DD_CONFIG_SUBLCD_PROPS_RESTORE, UI One,OEM
   complex, Sub LCD properties to restore,0,0,0,| 
      enum, Sub LCD Properties, IDEL_UIONE_PROPERTIES
      string, RGBA Value
}
   HELP
      Enter the property and the Red,Green,Blue,Alpha (RGBA) values for the sub LCD to restore (for example, PROP_FGCOLOR,65280).
   }
}
Resource, IDS_DD_CFGI_BANNER_NAME, UI One,OEM
    string, Banner Name   
   HELP
      Enter the Banner Name.
      If the device does not support CFGI_BANNERNAME, enter "None".
   }
}
#<!--=================================================================
#       New DPK entries
#=================================================================-->
Resource, IDS_DD_ALLOW_SETNAM_NOTZERO, Telephone,OEM
   enum, NAM set to 0 support, IDEL_LABEL_YESNO
   
   HELP
      If the device supports setting the NAM to 0 enter "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_CHANGE_NAM_MODE, Telephone,OEM
   enum,NAM mode change support, IDEL_LABEL_YESNO

   HELP
      If the device supports setting the NAM mode enter "Yes".
      Otherwise, select "No".
   }
}  
Resource, IDS_DD_NETWORK_CHANGE, Telephone,OEM
   enum, Preferred Network Change, IDEL_LABEL_YESNO
   
   HELP
      If the device supports setting the NAM mode enter "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_STANDBY_WAKEUP, Telephone,OEM
   enum, Wakeup From Standby, IDEL_YESNO
   
   HELP
      If the device supports wakeup from standby enter "Yes"
      Otherwise, select "No".
   }
}
Resource, IDS_DD_RTRE_CHANGE, Telephone,OEM
   enum, RTRE change, IDEL_LABEL_YESNO
   
   HELP
      If the device supports changing the RTRE configuration enter "Yes"
      Otherwise, select "No".
   }
}
Resource, IDS_DD_RESET_ACM, Telephone,OEM
   enum, Reset ACM, IDEL_LABEL_YESNO
   HELP
      If the device supports resetting the Accumulated Call Meter(ACM) enter "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_MAX_ACM, Telephone,OEM
   enum, MAX ACM, IDEL_LABEL_YESNO

   HELP
      If the device supports setting the Maximum Accumulated Call Meter(ACM) enter "Yes".
      Otherwise, select "No".
   }
}
Resource, IDS_DD_CHANGE_DDTM_MODE, Telephone,OEM
   enum, DDTM Change, IDEL_LABEL_YESNO

   HELP
      If the device supports changing the phone's Dedicated Data Transmission Mode enter "Yes".
      Otherwise, select "No". 
   }
}

Resource, IDS_DD_CFGI_DOWNLOAD_SET, Device
   enum, Download Support, IDEL_LABEL_YESNO
   
   HELP
      If the device supports the changing IDownload settings enter "Yes".  
      Otherwise, select "No".
   }
}

Resource, IDS_DD_EXTENDED_KEYPAD_MAP_FILE_LOCATION, Keypad,OEM
   string, Extended keys mapping file location
   
   HELP
      If the device supports extended keypad, enter path location of map.csv on the handset.   
	  If device does not support extended keypads, enter "NULL" 
   }
}
#######################################################################
#
#  Performance
#
#######################################################################
Resource, IDS_DD_DLSTAT_NOUI, Performance,OEM
     uint32, Download - No UI
   HELP
      Enter required minimum user experienced throughput (in Kbps) for downloading an application without the UI.
      If there is no requirement, or IDownload is not supported, enter "0"
   }
}

Resource, IDS_DD_DLSTAT_UI, Performance,OEM
     uint32, Download - UI
   HELP
      Enter required minimum user experienced throughput (in Kbps) for downloading an application with the UI.
      If there is no requirement, or IDownload is not supported, enter "0"  
 }
}
Resource, IDS_DD_DLSTAT_NOANIM, Performance,OEM
     uint32, Download - Animation
   HELP
      Enter required minimum user experienced throughput (in Kbps) for downloading an application with an animation.
      If there is no requirement, or IDownload is not supported, enter "0".  
   }
}
Resource, IDS_DD_DATA_RATE, Performance,OEM
   uint32, Web - Download
   HELP
	Enter the required minimum speed (in bps) for Web downloads.
        If there is no requirement, or IWeb is not supported, then enter "0".
   }
}
Resource, IDS_DD_RMC_ENUMINIT, Performance,OEM
     uint32, RMC - IFILEMGR_EnumInit
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_EnumInit operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_ENUMNEXT, Performance,OEM
    uint32, RMC - IFILEMGR_EnumNext
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_EnumNext operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_READFIXEDDELAY, Performance,OEM
    uint32, RMC - IFILE_Read
   HELP
	Enter the required minimum average time (in ms) to perform the IFILE_Read operation on an RMC card for 1 byte.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_WRITEFIXEDDELAY, Performance,OEM
    uint32, RMC - IFILE_Write
   HELP
	Enter the required minimum average time (in ms) to perform the IFILE_Write operation on an RMC card for 1 byte.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_RMDIR, Performance,OEM
    uint32, RMC - IFILEMGR_RmDir
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_RmDir operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_MKDIR, Performance,OEM
   uint32, RMC - IFILEMGR_MkDir
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_MkDir operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_OPEN, Performance,OEM
   uint32, RMC - IFILEMGR_OpenFile - _OFM_READ
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_OpenFile operation on an RMC card with the OpenMode set to _OFM_READ .
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_CLOSE, Performance,OEM   
   uint32, RMC - IFILE_Release
   HELP
	Enter the required minimum average time (in ms) to perform the IFILE_Release operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_RENAME, Performance,OEM
   uint32, RMC - IFILEMGR_Rename
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_Rename operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_REMOVE, Performance,OEM
   uint32, RMC - IFILEMGR_Remove
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_Remove operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_CREATE, Performance,OEM
   uint32, RMC - IFILEMGR_OpenFile - _OFM_CREATE
   HELP
	Enter the required minimum average time (in ms) to perform the IFILEMGR_OpenFile operation on an RMC card with the OpenMode set to _OFM_CREATE .
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_SEEK, Performance,OEM
   uint32,RMC - IFILE_Seek
   HELP
	Enter the required minimum average time (in ms) to perform the IFILE_Seek operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
Resource, IDS_DD_RMC_TRUNCATE, Performance,OEM
   uint32, RMC - IFILE_Truncate
   HELP
	Enter the required minimum average time (in ms) to perform the IFILE_Truncate operation on an RMC card.
        If there is no requirement, or RMC cards are not supported, enter "0".
   }
}
#<!--=================================================================
#       Suspend Resume Info
#=================================================================-->
Resource, IDS_DD_EVDO_NODATASESSION, EVDO No Data Session BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall, IDEL_LABEL_YESNO
      enum, Receive OTAPACall, IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Headset, IDEL_LABEL_YESNO
      enum, Remove Headset, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Close Clamshell with Headset, IDEL_LABEL_YESNO
	  enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
	  enum, Press END key, IDEL_LABEL_YESNO
     
	  HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On:
 
	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On:
 
	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off:
 
	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         Receive OTAPACall: 

	     Initiate a OTAPA call to the device under the test. In such a scenario on receiving the OTAPA call if BREW is to be suspended and 
         resumed after the OTAPA call is completed, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

         An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Headset: 

	     The headset is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the headset insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Headset: 

	     The headset is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the headset removed notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 

	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Close Clamshell with Headset: 

	     Make sure that a headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}
Resource, IDS_DD_EVDO_DATASESSION, EVDO Data Session BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall , IDEL_LABEL_YESNO
      enum, Receive OTAPACall, IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Close Clamshell with Headset, IDEL_LABEL_YESNO
      enum, Press SEND key, IDEL_LABEL_YESNO
      enum, Press Directional key, IDEL_LABEL_YESNO
      enum, Press OK key, IDEL_LABEL_YESNO
      enum, Press Numeric Key 0 to 9, IDEL_LABEL_YESNO
      enum, Press * key, IDEL_LABEL_YESNO
      enum, Press # key, IDEL_LABEL_YESNO
      enum, Press Clear key, IDEL_LABEL_YESNO
      enum, Press VOICE DIAL Key / VOICE COMMAND Key, IDEL_LABEL_YESNO
      enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
      enum, Press END key, IDEL_LABEL_YESNO

      HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         Receive OTAPACall: 

	     Initiate a OTAPA call to the device under the test. In such a scenario on receiving the OTAPA call if BREW is to be suspended and 
         resumed after the OTAPA call is completed, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

         An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 

	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Close Clamshell with Headset: 

	     Make sure that a headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press SEND key: 

	     Press the SEND key on the device under the test. In such a scenario when the user presses the SEND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Directional key: 

	     Press the Directional key on the device under the test. In such a scenario when the user presses the Directional Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press OK key: 

	     Press the OK key on the device under the test. In such a scenario when the user presses the OK Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Numeric key 0 to 9: 

	     Press the Numeric key (0 to 9) on the device under the test. In such a scenario if BREW is to be suspended on pressing any of the Numeric Key and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press * key: 

	     Press the * key on the device under the test. In such a scenario when the user presses the * Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press # key: 

	     Press the # key on the device under the test. In such a scenario when the user presses the # Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Clear key: 

	     Press the Clear key on the device under the test. In such a scenario when the user presses the Clear Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press VOICE DIAL key / VOICE COMMAND key: 

	     Press the VOICE DIAL Key or VOICE COMMAND key on the device under the test. In such a scenario when the user presses the VOICE DIAL Key or VOICE COMMAND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}
Resource, IDS_DD_EVDO_DOWNLOAD, EVDO Download BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall, IDEL_LABEL_YESNO
      enum, Receive OTAPACall, IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Close Clamshell with Headset, IDEL_LABEL_YESNO
      enum, Press SEND key, IDEL_LABEL_YESNO
      enum, Press Directional key, IDEL_LABEL_YESNO
      enum, Press OK key, IDEL_LABEL_YESNO
      enum, Press Numeric Key 0 to 9, IDEL_LABEL_YESNO
      enum, Press * key, IDEL_LABEL_YESNO
      enum, Press # key, IDEL_LABEL_YESNO
      enum, Press Clear key, IDEL_LABEL_YESNO
      enum, Press VOICE DIAL Key / VOICE COMMAND Key, IDEL_LABEL_YESNO
      enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
      enum, Press END key, IDEL_LABEL_YESNO
 
      HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         Receive OTAPACall: 

	     Initiate a OTAPA call to the device under the test. In such a scenario on receiving the OTAPA call if BREW is to be suspended and 
         resumed after the OTAPA call is completed, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

         An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 

	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Close Clamshell with Headset: 

	     Make sure that a headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press SEND key: 

	     Press the SEND key on the device under the test. In such a scenario when the user presses the SEND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Directional key: 

	     Press the Directional key on the device under the test. In such a scenario when the user presses the Directional Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press OK key: 

	     Press the OK key on the device under the test. In such a scenario when the user presses the OK Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Numeric key 0 to 9: 

	     Press the Numeric key (0 to 9) on the device under the test. In such a scenario if BREW is to be suspended on pressing any of the Numeric Key and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press * key: 

	     Press the * key on the device under the test. In such a scenario when the user presses the * Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press # key: 

	     Press the # key on the device under the test. In such a scenario when the user presses the # Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Clear key: 

	     Press the Clear key on the device under the test. In such a scenario when the user presses the Clear Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press VOICE DIAL key / VOICE COMMAND key: 

	     Press the VOICE DIAL Key or VOICE COMMAND key on the device under the test. In such a scenario when the user presses the VOICE DIAL Key or VOICE COMMAND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}
Resource, IDS_DD_RTT_NODATASESSION, RTT No Data Session BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall, IDEL_LABEL_YESNO
      enum, Receive OTAPACall, IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Headset, IDEL_LABEL_YESNO
      enum, Remove Headset, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Close Clamshell with Headset, IDEL_LABEL_YESNO
	  enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
      enum, Press END key, IDEL_LABEL_YESNO

	  HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         Receive OTAPACall: 

	     Initiate a OTAPA call to the device under the test. In such a scenario on receiving the OTAPA call if BREW is to be suspended and 
         resumed after the OTAPA call is completed, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

             An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Headset: 

	     The headset is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the headset insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Headset: 

	     The headset is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the headset removed notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 
	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Close Clamshell with Headset: 

	     Make sure that a headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}
Resource, IDS_DD_RTT_DATASESSION, RTT Data Session BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall , IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Press SEND key, IDEL_LABEL_YESNO
      enum, Press Directional key, IDEL_LABEL_YESNO
      enum, Press OK key, IDEL_LABEL_YESNO
      enum, Press Numeric Key 0 to 9, IDEL_LABEL_YESNO
      enum, Press * key, IDEL_LABEL_YESNO
      enum, Press # key, IDEL_LABEL_YESNO
      enum, Press Clear key, IDEL_LABEL_YESNO
      enum, Press VOICE DIAL Key / VOICE COMMAND Key, IDEL_LABEL_YESNO
      enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
      enum, Press END key, IDEL_LABEL_YESNO
   
      HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

         An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 

	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press SEND key: 

	     Press the SEND key on the device under the test. In such a scenario when the user presses the SEND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Directional key: 

	     Press the Directional key on the device under the test. In such a scenario when the user presses the Directional Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press OK key: 

	     Press the OK key on the device under the test. In such a scenario when the user presses the OK Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Numeric key 0 to 9: 

	     Press the Numeric key (0 to 9) on the device under the test. In such a scenario if BREW is to be suspended on pressing any of the Numeric Key and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press * key: 

	     Press the * key on the device under the test. In such a scenario when the user presses the * Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press # key: 

	     Press the # key on the device under the test. In such a scenario when the user presses the # Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Clear key: 

	     Press the Clear key on the device under the test. In such a scenario when the user presses the Clear Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press VOICE DIAL key / VOICE COMMAND key: 

	     Press the VOICE DIAL Key or VOICE COMMAND key on the device under the test. In such a scenario when the user presses the VOICE DIAL Key or VOICE COMMAND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}
Resource, IDS_DD_RTT_DOWNLOAD, RTT Download BREW Suspend/Resume
      enum, Receive SMS/EMS AutoView Off, IDEL_LABEL_YESNO
      enum, Receive SMS/EMS AutoView On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive On, IDEL_LABEL_YESNO
      enum, Receive MMS AutoReceive Off, IDEL_LABEL_YESNO
      enum, Receive VoiceMail, IDEL_LABEL_YESNO
      enum, Receive MissedCall, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView Off, IDEL_LABEL_YESNO
      enum, Receive WAPPush AutoView On, IDEL_LABEL_YESNO
      enum, Receive VoiceCall, IDEL_LABEL_YESNO
      enum, PushToTalk Session, IDEL_LABEL_YESNO
      enum, WiFi, IDEL_LABEL_YESNO
      enum, Embedded Bluetooth Session, IDEL_LABEL_YESNO
      enum, Embedded Stereo Bluetooth Session, IDEL_LABEL_YESNO
      enum, IM Conversation, IDEL_LABEL_YESNO
      enum, Timer or PIM Alert, IDEL_LABEL_YESNO
      enum, Alarm Notification, IDEL_LABEL_YESNO
      enum, Low battery condition, IDEL_LABEL_YESNO
      enum, Loss of Service condition, IDEL_LABEL_YESNO
      enum, Roaming Status Change condition, IDEL_LABEL_YESNO
      enum, Insert Memory Card, IDEL_LABEL_YESNO
      enum, Remove Memory Card, IDEL_LABEL_YESNO
      enum, Insert Data Cable, IDEL_LABEL_YESNO
      enum, Remove Data Cable, IDEL_LABEL_YESNO
      enum, Insert Car Kit, IDEL_LABEL_YESNO
      enum, Remove Car Kit, IDEL_LABEL_YESNO
      enum, Insert VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Remove VPC or Travel  Charger or Desktop Charger or Global  Charger, IDEL_LABEL_YESNO
      enum, Charger Attached Device Charging, IDEL_LABEL_YESNO
      enum, Charger Attached Charging Complete, IDEL_LABEL_YESNO
      enum, Close Clamshell without Headset, IDEL_LABEL_YESNO
      enum, Close Clamshell with Headset, IDEL_LABEL_YESNO
      enum, Press SEND key, IDEL_LABEL_YESNO
      enum, Press Directional key, IDEL_LABEL_YESNO
      enum, Press OK key, IDEL_LABEL_YESNO
      enum, Press Numeric Key 0 to 9, IDEL_LABEL_YESNO
      enum, Press * key, IDEL_LABEL_YESNO
      enum, Press # key, IDEL_LABEL_YESNO
      enum, Press Clear key, IDEL_LABEL_YESNO
      enum, Press VOICE DIAL Key / VOICE COMMAND Key, IDEL_LABEL_YESNO
      enum, Press Push to Talk Button, IDEL_LABEL_YESNO
      enum, Press Volume UP key, IDEL_LABEL_YESNO
      enum, Press Volume DOWN Key, IDEL_LABEL_YESNO
      enum, Switch Speaker Off To On, IDEL_LABEL_YESNO
      enum, Switch Speaker On To Off, IDEL_LABEL_YESNO
      enum, Press END key, IDEL_LABEL_YESNO
  
      HELP
         Receive SMS/EMS AutoView Off: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition the SMS or EMS should not be automatically displayed for preview on being received. The user can view this message by manually
         opening it up. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive SMS/EMS AutoView On: 

	     A SMS or EMS is to be sent to the device under the test. As a precondition  the SMS or EMS should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the SMS or EMS notification(s) and resumed after the user dismisses the SMS or EMS notification(s),
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive On: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should be automatically downloaded on receiving MMS Notification. The user can view this downloaded MMS message.
         In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive MMS AutoReceive Off: 

	     A MMS is to be sent to the device under the test. As a precondition the MMS should not be automatically downloaded on receiving MMS Notification. The user has to manually request for MMS download and 
         view this downloaded MMS message. In such a scenario if BREW is to be suspended on receiving the MMS notification(s) and resumed after the user dismisses the MMS notification(s), 
         please select "Yes", otherwise select "No".

         Receive VoiceMail: 

       	 A VoiceMail is to be sent to the device under the test. The user can listen to this voice mail from his/her voice mail box.In such a scenario if BREW is to be
         suspended on receiving the VoiceMail notification and resumed after the user dismisses the voice mail notification, please select "Yes", otherwise select "No".

         Receive MissedCall: 

	     Leave a missed call on the device under the test. The user can view the number from which he/she has received the missed call. In such a scenario if BREW is to be suspended on receiving 
         a missed call notification and resumed after the user dismisses the missed call notification, please select "Yes", otherwise select "No".

         Receive WAPPush AutoView Off: 

    	 A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should not be automatically displayed for preview on being received. The user can view 
         the WAP Push information by manually opening it up. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive WAPPush AutoView On: 

	     A WAP Push is to be sent to the device under the test. As a precondition the WAP Push information should be automatically displayed for preview on being received. In such a scenario if BREW is to be suspended on receiving the WAP Push notification(s) and 
         resumed after the user dismisses the WAP Push notification(s) ,please select "Yes", otherwise select "No".

         Receive VoiceCall: 

	     Make a voice call to the device under the test.In such a scenario if BREW is to be suspended on receiving the voice call and resumed after the user ends the voice call, please select "Yes", otherwise select "No".

         PushToTalk Session: 

	     Initiate a Push to talk session with the device under the test. In such a scenario if BREW is to be suspended when Push to talk session is started and resumed after the Push to talk session is ended, please select "Yes", otherwise select "No".
       
	     WiFi: 

	     Initiate a WiFi session with the device under the test. In such a scenario if BREW is to be suspended when the WiFi session is started and resumed after the WiFi session is ended, please select "Yes", otherwise select "No".

         Embedded Bluetooth Session: 

	     Initiate a Embedded Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Bluetooth Session is started and resumed after the Embedded Bluetooth Session is ended, please select "Yes", otherwise select "No".

         Embedded Stereo Bluetooth Session: 

	     Initiate a Embedded Stereo Bluetooth Session with the device under the test. In such a scenario if BREW is to be suspended when Embedded Stereo Bluetooth Session is started and resumed after the Stereo Embedded Bluetooth Session is ended, please select "Yes", otherwise select 
         "No".

         IM Conversation: 

	     Start an instant messaging conversation with the the device under the test. As the precondition the user should already have been logged on the 
         Instant Messenger on the device under test. In such a scenario on receiving the Instant Message notification if BREW is to be suspended and resumed after the Instant message conversation
         is ended, please select "Yes", otherwise select "No".

         Timer or PIM Alert: 

	     A timer alert or PIM (Personal Information Manager) Alert should occur on the device under the test. The user can view more information regarding
         the timer or PIM Alert. In such a scenario if BREW is to be suspended on receiving the Alert and resumed after the user dismisses the Alert,
         please select "Yes", otherwise select "No".

         Alarm Notification: 

         An alarm notification should occur on the device under the test. The user can view more information regarding this Alarm notification.
         In such a scenario if BREW is to be suspended on receiving the Alarm notification and resumed after the user dismisses the alarm notification,
         please select "Yes", otherwise select "No".

         Low battery condition: 

	     A notification showing that battery is low should occur on the device under the test. In such a scenario if BREW is to be suspended on receiving the low battery notification and resumed after the user dismisses the low battery notification, please select "Yes", otherwise select "No".

         Loss of Service condition: 

	     A notification should be shown on the device under the test when service is lost . In such a scenario if BREW is 
         to be suspended on receiving the loss of service notification  and resumed after the user dismisses the loss of service notification,please select "Yes", otherwise select "No".

         Roaming Status Change condition: 

         The user should be notified about any change in roaming status by the roaming Indicator during the test. In such a scenario if BREW is to suspended on receiving
         the roaming indication and resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Insert Memory Card: 

	     The Memory card is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Memory Card: 

	     The Memory card is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Memory card removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Data Cable: 

    	 The Data cable is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Data Cable: 

	     The Data Cable is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Data Cable removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert Car Kit: 

	     The Car Kit is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove Car Kit: 

	     The Car Kit is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Car Kit removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Insert VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be inserted into the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger insertion notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Remove VPC or Travel  Charger or Desktop Charger or Global  Charger: 

	     The Charger (VPC or Travel Charger or Desktop Charger or Global Charger) is to be removed from the device under the test. In such a scenario if BREW is to be suspended on receiving the Charger removal notification and 
         resumed after the user dismisses the notification, please select "Yes", otherwise select "No".

         Charger Attached Device Charging: 

	     On being connected to a charger the device charging indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Charger Attached Charging Complete: 

	     When the battery charging is completed a charging complete indication should be seen on the device under the test. In such a scenario if BREW is to be suspended on receiving charging complete indication and 
         resumed after the user dismisses this indication, please select "Yes", otherwise select "No".

         Close Clamshell without Headset: 

	     Make sure that no headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Close Clamshell with Headset: 

	     Make sure that a headset is attached to the clamshell device under the test and close the clamshell. In such a scenario after closing the clamshell if BREW is to be suspended and 
         resumed after Clamshell is opened, please select "Yes", otherwise select "No".

         Press SEND key: 

	     Press the SEND key on the device under the test. In such a scenario when the user presses the SEND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Directional key: 

	     Press the Directional key on the device under the test. In such a scenario when the user presses the Directional Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press OK key: 

	     Press the OK key on the device under the test. In such a scenario when the user presses the OK Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Numeric key 0 to 9: 

	     Press the Numeric key (0 to 9) on the device under the test. In such a scenario if BREW is to be suspended on pressing any of the Numeric Key and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press * key: 

	     Press the * key on the device under the test. In such a scenario when the user presses the * Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press # key: 

	     Press the # key on the device under the test. In such a scenario when the user presses the # Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Clear key: 

	     Press the Clear key on the device under the test. In such a scenario when the user presses the Clear Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press VOICE DIAL key / VOICE COMMAND key: 

	     Press the VOICE DIAL Key or VOICE COMMAND key on the device under the test. In such a scenario when the user presses the VOICE DIAL Key or VOICE COMMAND Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Push to Talk Button: 

	     Press the Push to Talk Button on the device under the test. In such a scenario when the user presses the Push to Talk Button if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Up key: 

	     Press the Volume Up key on the device under the test. In such a scenario when the user presses the Volume Up key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Press Volume Down Key: 

	     Press the Volume Down on the device under the test. In such a scenario when the user presses the Volume Down Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

         Switch Speaker Off to On: 

	     Switch on the speaker on the device under the test. In such a scenario when the user switches on the speaker if BREW is to be suspended
         and resumed after the action is completed, please select "Yes", otherwise select "No".

         Switch Speaker On to Off: 

	     Switch off the speaker on the device under the test. As a precondition the speaker phone should already be on. In such a scenario when the 
         user switches off the speaker if BREW is to be suspended and resumed after the action is completed, please select "Yes", otherwise select "No".

		 Press END key: 

	     Press the END key on the device under the test. In such a scenario when the user presses the END Key if BREW is to be suspended and 
         resumed after the Key is released, please select "Yes", otherwise select "No".

   }
}


#<!--=================================================================
#       PEAnalyze Items 
#=================================================================-->

Resource, IDS_DD_USR_LAT, Position Determination 
   string, Latitude

   HELP
      Enter the latitude for device under test.
	  Latitude must be in the range [-90,90] and must include a decimal point up to at most 8 decimal digits.
	  Ground truth for latitude must be determined using a method with accurracy that is an order of magnitude greater than the GPS accuracy of the device under test.  
   }
}
Resource, IDS_DD_USR_LON, Position Determination 
   string, Longitude
   
   HELP
      Enter the longitude for device under test.
	  Longitude must be in the range [-180,180] and must include a decimal point up to at most 8 decimal digits.
	  Ground truth for longitude must be determined using a method with accurracy that is an order of magnitude greater than the GPS accuracy of the device under test.
   }
}
Resource, IDS_DD_USR_ALT, Position Determination 
   string, Altitude

   HELP
      Enter the altitude of device under test.
	  Altitude is in meters and must include a decimal point up to at most 8 decimal digits.
	  Ground truth for altitude must be determined using a method with accurracy that is an order of magnitude greater than the GPS accuracy of the device under test.  
   }
}
Resource, IDS_DD_PEA_RUN, Position Determination 
   enum, Run PEAnalyze, IDEL_LABEL_YESNO

   HELP
      Enter 'Yes' to run PEAnalyze , Otherwise 'No'.
   }
}
Resource, IDS_DD_PEA_MAX_HOR_UNC, Position Determination 
   uint32, Horizontal uncertainty

   HELP
      Enter the Maximum allowable value in meters for horizontal uncertainty.
   }
}
Resource, IDS_DD_PEA_MAX_VER_UNC, Position Determination 
   uint32, Vertical uncertainty

   HELP
      Enter the Maximum allowable value in meters for vertical uncertainty.  
   }
}
Resource, IDS_DD_PEA_MAX_ERR_GND, Position Determination 
   uint32, Maximum allowable error

   HELP
     Enter the Maximum allowable error in position from ground truth in meters.
   }
}
Resource, IDS_DD_PEA_MAX_DUR, Position Determination 
   uint32, Fix duration

   HELP
      Enter the Maximum allowable fix duration (time to fix) in seconds.  
   }
}
Resource, IDS_DD_PEA_CEP_68_PCT, Position Determination
   uint32, CEP FOR 68% of fixes

   HELP
      Enter the Maximum CEP for 68% of fixes.
   }
}
Resource, IDS_DD_PEA_CEP_95_PCT, Position Determination
   uint32, CEP for 95% of fixes

   HELP
      Enter the Maximum CEP for 95% of fixes. 
   }
}
