#============================================================================
#  Name:
#    $(TARGET).MAK
#
#  Description:
#    Makefile to build the $(TARGET) downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake /f $(TARGET).mak [target]
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
#        Copyright © 2000-2003 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
BREW_HOME      =$(BREWDIR)
ARM_HOME       =$(ARMHOME)
TARGET         =editextension
OBJS           =AEEAppGen.o AEEModGen.o EditExtension.o Xmod.o 
APP_INCLUDES   = -I "../EditApplet"

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

EXETYPE    = elf                # Target image file format
MODULE     = mod                # Downloadable module extension

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#
# Tells the SDK source stuffs that we're building a dynamic app.
#-------------------------------------------------------------------------------

DYNAPP          = -DDYNAMIC_APP


#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

AEESRCPATH = $(BREW_HOME)\src
AEEINCPATH = $(BREW_HOME)\inc

ARMBIN = $(ARM_HOME)\bin        # ARM ADS application directory
ARMINC = $(ARM_HOME)\include    # ARM ADS include file directory
ARMLIB = $(ARM_HOME)\lib        # ARM ADS library directory

ARMCC   = $(ARMBIN)\armcc       # ARM ADS ARM 32-bit inst. set ANSI C compiler
LD      = $(ARMBIN)\armlink     # ARM ADS linker
HEXTOOL = $(ARMBIN)\fromelf     # ARM ADS utility to create hex file from image

OBJ_CMD    = -o                 # Command line option to specify output filename

#-------------------------------------------------------------------------------
# Processor architecture options
#-------------------------------------------------------------------------------

CPU = -cpu ARM7TDMI             # ARM7TDMI target processor

#-------------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#-------------------------------------------------------------------------------

ROPI     = ropi                 # Read-Only(code) Position independence
INTERWRK = interwork            # Allow ARM-Thumb interworking

APCS = -apcs /$(ROPI)/$(INTERWRK)/norwpi

#-------------------------------------------------------------------------------
# Additional compile time error checking options
#-------------------------------------------------------------------------------

CHK = -fa                       # Check for data flow anomolies

#-------------------------------------------------------------------------------
# Compiler output options
#-------------------------------------------------------------------------------

OUT = -c                        # Object file output only

#-------------------------------------------------------------------------------
# Compiler/assembler debug options
#-------------------------------------------------------------------------------

DBG = -g                        # Enable debug

#-------------------------------------------------------------------------------
# Compiler optimization options
#-------------------------------------------------------------------------------

OPT = -Ospace -O2               # Full compiler optimization for space

#-------------------------------------------------------------------------------
# Compiler code generation options
#-------------------------------------------------------------------------------

END = -littleend                # Compile for little endian memory architecture
ZA  = -zo                       # LDR may only access 32-bit aligned addresses

CODE = $(END) $(ZA)


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

INC = -I. -I$(AEEINCPATH) $(APP_INCLUDES)


#-------------------------------------------------------------------------------
# Compiler pragma emulation options
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Linker options
#-------------------------------------------------------------------------------

LINK_CMD = -o                    #Command line option to specify output file
                                 #on linking

ROPILINK = -ropi                 #Link image as Read-Only Position Independent

LINK_ORDER = -first AEEMod_Load

#-------------------------------------------------------------------------------
# HEXTOOL options
#-------------------------------------------------------------------------------

BINFORMAT = -bin


#-------------------------------------------------------------------------------
# Compiler flag definitions
#-------------------------------------------------------------------------------

CFLAGS0 = $(OUT) $(DYNAPP) $(CPU) $(APCS) $(CODE) $(CHK) $(DBG)
CFLAGS  = $(CFLAGS0) $(OPT)

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

# the -entry flag is not really needed, but it keeps the linker from reporting
# warning L6305W (no entry point).  The address
LFLAGS = $(ROPILINK) -rwpi -entry 0x8000#

#----------------------------------------------------------------------------
# Default target
#----------------------------------------------------------------------------

all :  $(TARGET).$(MODULE)

#----------------------------------------------------------------------------
# Clean target
#----------------------------------------------------------------------------

# The object subdirectory, target image file, and target hex file are deleted.

clean :
	@echo ---------------------------------------------------------------
	@echo CLEAN
	-del /f $(OBJS)
	-del /f $(TARGET).$(EXETYPE)
	-del /f $(TARGET).$(MODULE)
	@echo ---------------------------------------------------------------

#============================================================================
#                           DEFAULT SUFFIX RULES
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

SRC_FILE = $(@F:.o=.c)                  # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@F)   # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SRC_FILE)
	@echo ---------------------------------------------------------------

.c.mix:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) -S -fs $(CFLAGS) $(INC) $(OBJ_FILE) $<
	@echo ---------------------------------------------------------------


{$(AEESRCPATH)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(AEESRCPATH)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS = $(OBJS)


#----------------------------------------------------------------------------
# Lib file targets
#----------------------------------------------------------------------------

$(TARGET).$(MODULE) : $(TARGET).$(EXETYPE)
	@echo ---------------------------------------------------------------
	@echo TARGET $@
	$(HEXTOOL)  $(TARGET).$(EXETYPE) $(BINFORMAT) $(TARGET).$(MODULE)

$(TARGET).$(EXETYPE) : $(APP_OBJS)
	@echo ---------------------------------------------------------------
	@echo TARGET $@
	$(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LFLAGS) $(APP_OBJS) $(LINK_ORDER)

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------


# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


editextension.o:	editextension.c
editextension.o:	editextension.h
editextension.o:	$(BREWDIR)\inc\aeeshell.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeegroupids.h
editextension.o:	$(BREWDIR)\inc\aeeclassids.h
editextension.o:	$(BREWDIR)\inc\aeeclsid_filemgr.bid
editextension.o:	$(BREWDIR)\inc\aeeerror.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeneterr.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeporterr.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeedatetime.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestderr.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeergbval.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeerasterop.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeepoint.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeimageinfo.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiapplet.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
editextension.o:	$(BREWDIR)\inc\aeequeryinterface.h
editextension.o:	$(BREWDIR)\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
editextension.o:	$(BREWDIR)\inc\aeevcodes.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
editextension.o:	$(BREWDIR)\inc\aeedisp.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeebitmap.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmapdev.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
editextension.o:	$(BREWDIR)\inc\aeefont.h
editextension.o:	$(BREWDIR)\inc\aeebitmap.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
editextension.o:	$(BREWDIR)\inc\aeeimage.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeesoundplayer.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeesound.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeisound.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeeresourcectl.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeeclsid_respriv_sound.bid
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeresctlsound.bid
editextension.o:	$(BREWDIR)\inc\aeecontrols_res.h
editextension.o:	$(BREWDIR)\inc\aeelngcode.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeedeviceitems.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeaflags.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeemif.h
editextension.o:	$(BREWDIR)\inc\aeestdlib.h
editextension.o:	$(BREWDIR)\inc\aeeoldvalist.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\aee.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	aeeeditextension.h
editextension.o:	$(BREWDIR)\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
editextension.o:	$(BREWDIR)\inc\aeecomdef.h
editextension.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	xmod.c
xmod.o:	$(BREWDIR)\inc\aeestdlib.h
xmod.o:	$(BREWDIR)\inc\aeeoldvalist.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeegroupids.h
xmod.o:	$(BREWDIR)\inc\aeeclassids.h
xmod.o:	$(BREWDIR)\inc\aeeclsid_filemgr.bid
xmod.o:	$(BREWDIR)\inc\aeeerror.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeneterr.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeporterr.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeedatetime.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestderr.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeergbval.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeerasterop.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeepoint.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeimageinfo.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiapplet.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
xmod.o:	$(BREWDIR)\inc\aeequeryinterface.h
xmod.o:	$(BREWDIR)\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
xmod.o:	editextension.h
xmod.o:	$(BREWDIR)\inc\aeeshell.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeevcodes.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
xmod.o:	$(BREWDIR)\inc\aeedisp.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeebitmap.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmapdev.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
xmod.o:	$(BREWDIR)\inc\aeefont.h
xmod.o:	$(BREWDIR)\inc\aeebitmap.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
xmod.o:	$(BREWDIR)\inc\aeeimage.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeesoundplayer.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeesound.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeisound.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeeresourcectl.h
xmod.o:	$(BREWDIR)\inc\aee.h
xmod.o:	$(BREWDIR)\inc\aeeclsid_respriv_sound.bid
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeresctlsound.bid
xmod.o:	$(BREWDIR)\inc\aeecontrols_res.h
xmod.o:	$(BREWDIR)\inc\aeelngcode.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeedeviceitems.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeaflags.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeemif.h
xmod.o:	$(BREWDIR)\inc\aeestdlib.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	aeeeditextension.h
xmod.o:	$(BREWDIR)\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
xmod.o:	$(BREWDIR)\inc\aeecomdef.h
xmod.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
