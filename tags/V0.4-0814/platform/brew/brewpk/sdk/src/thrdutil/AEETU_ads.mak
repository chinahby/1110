#============================================================================
#  Name:
#    AEETU_ads.mak
#
#  Description:
#    Makefile to build static library of ThreadUtil helper functions,
#      thumb interop mode
#
#   The following nmake targets are available in this makefile:
#
#     all           - make AEEThread.lib (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake [target]
#
# Assumptions:
#   1. The ARM ADS tools are installed in $(ARMHOME)
#   2. The BREW sdk include files are installed in $(BREWDIR)/inc
#   3. The variable TARGET below has be set to the proper base name of the
#      module being built
#
# Copyright (c) 2001 by QUALCOMM Incorported.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================

#----------------------------------------------------------------------------
# Target file name and type and path definitions
#----------------------------------------------------------------------------

!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE No configuration specified. Defaulting to AEETU - arm Debug.
!ENDIF

!IF "$(CFG)" == "Debug"
INTERIM_DIR = .\ads_Debug       # name for object dir
!ELSEIF "$(CFG)" == "Release"
INTERIM_DIR = .\ads_Release     # name for object dir
!ELSE
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "AEETU_ads.mak" CFG="Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Release"
!MESSAGE "Debug"
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

OUTPUT_DIR = ..\..\lib\ads

!IFNDEF ARMHOME
!ERROR ARMHOME must be defined (either in the environment or on the command
!ERROR  line) as the installation dir of the ADS tool suite with Bin,
!ERROR  Include & Lib subdirectories.
!ENDIF

#----------------------------------------------------------------------------
# override SDK environment variable.  If you move this project or makefile
#  redefine BREWDIR
#----------------------------------------------------------------------------

BREWDIR = ..\..

#----------------------------------------------------------------------------
# Target compile time symbol definitions
#----------------------------------------------------------------------------

DEFINES = -DDYNAMIC_APP

#----------------------------------------------------------------------------
# Software tool and environment definitions
#----------------------------------------------------------------------------

ARMBIN = $(ARMHOME)\Bin                         # ARM ADS application directory
ARMINC = $(ARMHOME)\Include                     # ARM ADS include file directory
ARMLIB = $(ARMHOME)\Lib                         # ARM ADS library directory

CC      = "$(ARMBIN)\tcc"      # ARM ADS ARM 16-bit ANSI C compiler
CPP     = "$(ARMBIN)\tcpp"     # ARM ADS ARM 16-bit ANSI CPP compiler
LD      = "$(ARMBIN)\armlink"  # ARM ADS linker
AR      = "$(ARMBIN)\armar"    # ARM ADS librarian
FROMELF = "$(ARMBIN)\fromelf"  # ARM ADS utility to create bin file from image

#----------------------------------------------------------------------------
# Processor architecture options
#----------------------------------------------------------------------------

CPU = -littleend -cpu ARM7TDMI    # ARM7TDMI target processor

#----------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#----------------------------------------------------------------------------

APCS = -apcs /noswst/inter/ropi

#----------------------------------------------------------------------------
# C Compiler options
#----------------------------------------------------------------------------

COPT = -zo -Wbns+a -fa

#----------------------------------------------------------------------------
# Compiler/assembler debug options
#----------------------------------------------------------------------------

!IF "$(CFG)" == "Debug"
DBG = -g                        # Enable debug
!ELSE
DBG =                           # No debug
!ENDIF

#----------------------------------------------------------------------------
# Compiler optimization options
#----------------------------------------------------------------------------

OPT = -Ospace -O2               # Full compiler optimization for space

#----------------------------------------------------------------------------
# Include file search path options
#----------------------------------------------------------------------------

INC = -I. -I$(BREWDIR)\inc

#----------------------------------------------------------------------------
# Compiler flag definitions
#----------------------------------------------------------------------------

CFLAGS = $(CPU) $(APCS) $(COPT) $(DBG) $(OPT) $(DEFINES) $(INC)

#----------------------------------------------------------------------------
# Librarian flag definitions
#----------------------------------------------------------------------------

ARFLAGS = -create

#----------------------------------------------------------------------------
# Linker options
#----------------------------------------------------------------------------

LFLAGS = -remove -ropi -strict

#============================================================================
#   module-specific defines
#============================================================================

OBJS = $(INTERIM_DIR)\AEECallbackUtil.o \
       $(INTERIM_DIR)\AEETU_CondVar.o \
       $(INTERIM_DIR)\AEETU_Connect.o \
       $(INTERIM_DIR)\AEETU_GetHostByName.o \
       $(INTERIM_DIR)\AEETU_Mutex.o \
       $(INTERIM_DIR)\AEETU_Recv.o \
       $(INTERIM_DIR)\AEETU_Send.o \
       $(INTERIM_DIR)\AEETU_Sleep.o \
       $(INTERIM_DIR)\AEETU_Socket.o \
       $(INTERIM_DIR)\AEETU_TLS.o \
       $(INTERIM_DIR)\AEETU_Web.o \
       $(INTERIM_DIR)\AEETU_Yield.o \

#----------------------------------------------------------------------------
# Rule helper definitions
#----------------------------------------------------------------------------

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

SRC_FILE = $(@F:.o=.c)                  # Input source file specification
MSG_FILE = -DMSG_FILE=\"$(@F:.o=.c)\"   # MSG_FILE symbol definition

#============================================================================
#  rules
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

.SUFFIXES :
.SUFFIXES : .o .c .cpp

#----------------------------------------------------------------------------
# Default target
#----------------------------------------------------------------------------

all : $(INTERIM_DIR)\exist $(OUTPUT_DIR)\exist $(OUTPUT_DIR)\AEEThreadUtil.lib

#----------------------------------------------------------------------------
# echo fun
#----------------------------------------------------------------------------

echo :
   @echo DIRS = $(DIRS)

#----------------------------------------------------------------------------
# Object directory target
#----------------------------------------------------------------------------

# make directories..

$(INTERIM_DIR)\exist :
         @-if not exist $@ mkdir $(@D)
         @echo Building $(@D) > $@

$(OUTPUT_DIR)\exist :
         @-if not exist $@ mkdir $(@D)
         @echo Building $(@D) > $@

#----------------------------------------------------------------------------
# Clean target
#----------------------------------------------------------------------------

# The object subdirectory, target image file, and target hex file are deleted.

clean :
        @echo ---------------------------------------------------------------
        @echo CLEAN
        -if exist $(INTERIM_DIR) rmdir /s/q $(INTERIM_DIR)
        -if exist $(OUTPUT_DIR) rmdir /s/q $(OUTPUT_DIR)
        @echo ---------------------------------------------------------------

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c{$(OBJ_DIR)}.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $<
        $(CC) -ansic -c $(CFLAGS) -o $@ $<
        @echo ---------------------------------------------------------------

#----------------------------------------------------------------------------
# Target rules
#----------------------------------------------------------------------------

$(OUTPUT_DIR)\AEEThreadUtil.lib : $(OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(AR) $(ARFLAGS) $@ $(OBJS)

# ------------
# DEPENDENCIES

