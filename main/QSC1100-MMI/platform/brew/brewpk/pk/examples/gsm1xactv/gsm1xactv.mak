#============================================================================
#  Name:
#    gsm1xactv.MAK
#
#  Description:
#    Makefile to build the GSM1xActv downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake /f helloworld.mak [target]
#
#  Assumptions:
#    1. The ARM ADS 1.0.1 and/or 1.1 tools are installed in the c:\ads directory.
#    2. This Makefile can be used with the ARM ADS 1.0.1 & 1.1 Compilers only. 
#
#  Notes:
#    1. While building the application using this make file, the following warnings may be 
#       received. They can be safely ignored:
#       "Warning: C2067I: option -zas will not be supported in future releases of the compiler
#       "Warning: C2071W: option -za will not be supported in future releases of the compiler 
#       "Warning: L6305W: Image does not have an entry point. (Not specified or not set due to 
#          multiple choices).
#  
#
#        Copyright © 1999-2002 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

SUPPORT_DIR      = $(BREWDIR)\src 
SUPPORT_INCDIR   = $(BREWDIR)\inc
SUPPORT_APPIDDIR = ..\.. 

TARGET     = gsm1xactv      # Target name for output files and object dir
EXETYPE    = elf            # Target image file format
MODULE     = mod            # Downloadable module extension
MAKE_TARGET= gsm1xactv      # Target Makefile name

GSM1x_INC  = ..\..\inc\msm

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#-------------------------------------------------------------------------------

DYNAPP          = -DDYNAMIC_APP


#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

ARMBIN = $(ARMHOME)\bin        # ARM ADS application directory
ARMINC = $(ARMHOME)\include    # ARM ADS include file directory
ARMLIB = $(ARMHOME)\lib        # ARM ADS library directory

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
ZA  = -zo                      # LDR may only access 32-bit aligned addresses
ZAS = -zas4                     # Min byte alignment for structures

CODE = $(END) $(ZA)


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

INC = -I. -I$(SUPPORT_INCDIR) -I$(SUPPORT_APPIDDIR) -I$(GSM1x_INC)


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

CFLAGS0 = $(OUT) $(DYNAPP) $(INC) $(CPU) $(APCS) $(CODE) $(CHK) $(DBG)
CFLAGS  = $(CFLAGS0) $(OPT)

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

# the -entry flag is not really needed, but it keeps the linker from reporting
# warning L6305W (no entry point).
LFLAGS = $(ROPILINK) -rwpi -ro-base 0 -entry AEEMod_Load

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
        -del /f *.o
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

SRC_FILE = $(@F:.o=.c)        # Input source file specification
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


{$(SUPPORT_DIR)}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SUPPORT_DIR)\$(SRC_FILE)
        @echo ---------------------------------------------------------------


#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS =  AEEAppGen.o \
            AEEModGen.o \
	    gsm1xactvfile.o \
	    gsm1xactvapi.o \
	    gsm1xactvprl.o \
            gsm1xactv.o 


#----------------------------------------------------------------------------
# Lib file targets
#----------------------------------------------------------------------------

$(TARGET).$(MODULE) : $(TARGET).$(EXETYPE)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(HEXTOOL)  $(TARGET).$(EXETYPE) $(BINFORMAT) -output $(TARGET).$(MODULE)

$(TARGET).$(EXETYPE) : $(APP_OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LFLAGS) $(APP_OBJS) $(LINK_ORDER)


# ------------------------------
# DO NOT EDIT BELOW THIS LINE

AEEAppGen.o : $(SUPPORT_DIR)\AEEAppGen.c
AEEAppGen.o : $(SUPPORT_INCDIR)\AEEAppGen.h

AEEModGen.o : $(SUPPORT_DIR)\AEEModGen.c
AEEModGen.o : $(SUPPORT_INCDIR)\AEEModGen.h

$(TARGET).o : $(TARGET).c
#$(TARGET).o : $(TARGET).h
$(TARGET).o : $(SUPPORT_INCDIR)\AEEAppGen.h
$(TARGET).o : $(SUPPORT_INCDIR)\AEEModGen.h
