#============================================================================
#  Name:
#    BrewAppMgr.MAK
#
#  Description:
#    Makefile to build the BrewAppMgr downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
#     nmake /f BrewAppMgr.mak [target]
#
#  Assumptions:
#    1. The environment variable ADSHOME is set to the root directory of the
#       arm tools.
#    2. The version of ADS is 1.2 or above.
#
#  Notes:
#    None.
#
#
#        Copyright © 2000-2002 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================

# Path to sdk and pk directories
SDKHOME = $(BREWPK)\..\sdk
PKHOME = $(BREWPK)
CSHOME = $(BREWCS)
FONTHOME = $(BREWFONT)
WIDGETHOME = $(BREWUIWIDGETS)


!IF EXIST ($(SDKHOME)\oem)
#external development structure
MYINC  = -I"..\aeemenuctlex,$(SDKHOME)\oem\inc\,$(PKHOME)\inc,$(CSHOME)\inc,$(WIDGETHOME)\widgets\inc,$(FONTHOME)\btfe\inc,$(FONTHOME)\btfe\inc\bid,$(FONTHOME)\qcsans_ttf\inc"#
!ELSE
MYINC = -I"..\aeemenuctlex,$(SDKHOME)\inc,$(PKHOME)\inc,$(PKHOME)\..\inc,$(CSHOME)\inc,$(WIDGETHOME)\widgets\inc,$(FONTHOME)\btfe\inc,$(FONTHOME)\btfe\inc\bid,$(FONTHOME)\qcsans_ttf\inc"#
!ENDIF

TARGET         = brewappmgr
TARGETDIR = $(TARGET)

#------------------------------------------------------------------------------
# Application objects
#------------------------------------------------------------------------------

LOCAL_OBJS =  $(TARGETDIR)\$(TARGET).o \
              $(TARGETDIR)\BrewAppMgrUtils.o \
              $(TARGETDIR)\MobileShop.o

#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------

!INCLUDE "..\brewappmgr\common.mak"

# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


$(TARGETDIR)\$(TARGET).o:       BrewAppMgr.c
$(TARGETDIR)\$(TARGET).o:       BrewAppMgr.h
$(TARGETDIR)\$(TARGET).o:       BrewAppMgr.bid
$(TARGETDIR)\$(TARGET).o:       brewappmgrls_res.h
$(TARGETDIR)\$(TARGET).o:       brewappmgrln_res.h
$(TARGETDIR)\$(TARGET).o:       BrewAppMgrUtils.h
$(TARGETDIR)\$(TARGET).o:       MobileShop.bid
$(TARGETDIR)\$(TARGET).o:       BrewAppMgrSettings.h
$(TARGETDIR)\BrewAppMgrUtils.o:     BrewAppMgrUtils.c
$(TARGETDIR)\BrewAppMgrUtils.o:     BrewAppMgrUtils.h
$(TARGETDIR)\BrewAppMgrUtils.o:     MobileShop.bid
$(TARGETDIR)\BrewAppMgrUtils.o:     BrewAppMgr.bid
$(TARGETDIR)\BrewAppMgrUtils.o:     brewappmgrls_res.h
$(TARGETDIR)\BrewAppMgrUtils.o:     brewappmgrln_res.h
$(TARGETDIR)\BrewAppMgrUtils.o:     BrewAppMgrSettings.h
$(TARGETDIR)\MobileShop.o:      MobileShop.c
$(TARGETDIR)\MobileShop.o:      MobileShop.h
$(TARGETDIR)\MobileShop.o:      BrewAppMgrUtils.h
$(TARGETDIR)\MobileShop.o:      MobileShop.bid
$(TARGETDIR)\MobileShop.o:      BrewAppMgr.bid
$(TARGETDIR)\MobileShop.o:      brewappmgrls_res.h
$(TARGETDIR)\MobileShop.o:      brewappmgrln_res.h
$(TARGETDIR)\MobileShop.o:      BrewAppMgrSettings.h

clean: cleanvsproj
cleanvsproj:
	devenv /clean debug $(TARGET).sln
all: debug
debug: debugvsproj
debugvsproj:
	devenv /rebuild debug $(TARGET).sln
