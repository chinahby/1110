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
MYINC  = -I"$(SDKHOME)\oem\inc\,$(PKHOME)\inc,$(CSHOME)\inc,$(WIDGETHOME)\widgets\inc,$(FONTHOME)\btfe\inc,$(FONTHOME)\btfe\inc\bid,$(FONTHOME)\qcsans_ttf\inc"#
!ELSE
MYINC = -I"$(SDKHOME)\inc,$(PKHOME)\inc,$(PKHOME)\..\inc,$(CSHOME)\inc,$(WIDGETHOME)\widgets\inc,$(FONTHOME)\btfe\inc,$(FONTHOME)\btfe\inc\bid,$(FONTHOME)\qcsans_ttf\inc"#
!ENDIF

TARGET         = aeemenuctlex
TARGETDIR = $(TARGET)

#------------------------------------------------------------------------------
# Application objects
#------------------------------------------------------------------------------

LOCAL_OBJS =  $(TARGETDIR)\$(TARGET).o \
              $(TARGETDIR)\QcList.o \
              $(TARGETDIR)\aeemenuctlutils.o

#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------

!INCLUDE "..\brewappmgr\common.mak"

# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


$(TARGETDIR)\$(TARGET).o:       aeemenuctlex.c
$(TARGETDIR)\$(TARGET).o:       AEEMenuCtlEx.h
$(TARGETDIR)\$(TARGET).o:       aeemenuctlex_priv.h
$(TARGETDIR)\aeemenuctlutils.o: aeemenuctlutils.c
$(TARGETDIR)\aeemenuctlutils.o: aeemenuctlutils.h
$(TARGETDIR)\aeemenuctlutils.o: AEEMenuCtlEx.h
$(TARGETDIR)\aeemenuctlutils.o: aeemenuctlex_priv.h
$(TARGETDIR)\QcList.o:          QcList.c
$(TARGETDIR)\QcList.o:          QcList.h

clean: cleanvsproj
cleanvsproj:
	devenv /clean debug $(TARGET).sln
all: debug
debug: debugvsproj
debugvsproj:
	devenv /rebuild debug $(TARGET).sln
