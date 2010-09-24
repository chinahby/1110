#============================================================================
#  Name:
#    example_make_common.min
#
#  Description:
#    Common include for Example Makefiles
#
#    To use this common include, the including makefile (or
#    environment variables) must define
#    ARMHOME  - root directory of the ADS toolkit
#    SDKHOME  - root directory of the SDK
#    TARGET   - name of the target, minus extension
#    TARGETDIR - target directory for object files
#    LOCAL_OBJS - list of object files
#
#    The makefile may optionally define
#    EXTRA_OBJS - list of additional support (sdk) files needed
#    MYINC - include commands for any additional include directories
#    MYLIBS - additional libraries to link
#    NOAPPGEN - if defined, don't include AEEAPPGEN and AEEMODGEN in build
#    THUMBCOMPILER - if defined, compile in Thumb mode
#    PLATFORM  - Platform target, currently supports "BIGEND32ENUM",
#                anything else implies little endian.
#
#    Pseudo targets:
#      all
#      clean
#
#    Targets:
#       filename.mix  - make mixed source / assembly file
#       filename.o    - make object file
#
#
#    In addition, the makefile must define appropriate dependency rules
#    for each of the objects in the LOCAL_OBJS list.
#
#        Copyright  2000-2002 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#============================================================================

!IFNDEF ARMHOME
!  ERROR ARMHOME must be defined
!ENDIF
!IFNDEF LOCAL_OBJS
!   ERROR LOCAL_OBJS must be defined
!ENDIF
!IFNDEF SDKHOME
!   ERROR SDKHOME must be defined
!ENDIF
!IFNDEF TARGET
!   ERROR TARGET must be defined
!ENDIF
!IFNDEF TARGETDIR
!   ERROR TARGETDIR must be defined
!ENDIF


#-------------------------------------------------------------------------------
# Determine processor target
#-------------------------------------------------------------------------------
#
!IFNDEF PLATFORM
PLATFORM = LITTLEENDIAN
!ELSE
!MESSAGE -- Platform target is $(PLATFORM) --
!ENDIF

#-------------------------------------------------------------------------------
# Path Definitions
#-------------------------------------------------------------------------------

# Use internal development directory structure for sdk/src and sdk/inc if
# present, otherwise use the commercial release directory structure.
SUPPORT_DIR    = $(SDKHOME)\src#
!IF EXIST ($(SUPPORT_DIR))
SUPPORT_INCDIR = $(SDKHOME)\inc#
!ELSE
SUPPORT_DIR    = $(SDKHOME)\..\src#
SUPPORT_INCDIR = $(SDKHOME)\..\inc#
!ENDIF

COMMONDIR = $(SDKHOME)\examples\common

#-------------------------------------------------------------------------------
# Target type definitions
#-------------------------------------------------------------------------------

EXETYPE = elf#                  # Target image file format
MODULE  = mod#                  # Downloadable module extension

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#-------------------------------------------------------------------------------

DEFINES = -DDYNAMIC_APP#

!IF "$(PLATFORM)" == "BIGEND32ENUM"
PLATFORM_DEFINES = -DFEATURE_BIG_ENDIAN#
!ENDIF

#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

ARMBIN = $(ARMHOME)\bin\#           # ARM ADS application directory
ARMINC = $(ARMHOME)\include\#       # ARM ADS include file directory
ARMLIB = $(ARMHOME)\lib\armlib\#    # ARM ADS library directory

ARMCC   = $(ARMBIN)armcc#       # ARM ADS ARM 32-bit inst. set ANSI C compiler
ARMCPP  = $(ARMBIN)armcpp#      # ARM ADS 32-BIT inst. set ANSI CPP compiler
THUMBCC = $(ARMBIN)tcc#         # ARM ADS Thumb inst. set ANSI C compiler
LD      = $(ARMBIN)armlink#     # ARM ADS linker
HEXTOOL = $(ARMBIN)fromelf#     # ARM ADS utility to create hex file from image
ARMAR   = $(ARMBIN)armar#       # ARM ADS library archiver

OBJ_CMD = -o#                   # Command line option to specify output filename


!IFDEF THUMBCOMPILER
!MESSAGE building "$(TARGET)" in 16-bit ISA (Thumb mode)
SUPPORT_DIR_CC = $(ARMCC)#
APP_DIR_CC     = $(THUMBCC)#
!ELSE
SUPPORT_DIR_CC = $(ARMCC)#
APP_DIR_CC     = $(ARMCC)#
APP_DIR_CPP    = $(ARMCPP)#
!ENDIF

#-------------------------------------------------------------------------------
# Processor architecture options
#-------------------------------------------------------------------------------

CPU = -cpu ARM7TDMI#            # ARM7TDMI target processor

#-------------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#-------------------------------------------------------------------------------

ROPI     = ropi#                # Read-Only(code) Position independence
INTERWRK = interwork#           # Allow ARM-Thumb interworking
NORWPI   = norwpi#              # No read/write data position-independent

APCS = -apcs /$(ROPI)/$(INTERWRK)/$(NORWPI)#

#-------------------------------------------------------------------------------
# Additional compile time error checking options
#-------------------------------------------------------------------------------

CHK = -fa#                      # Check for data flow anomolies

#-------------------------------------------------------------------------------
# Compiler output options
#-------------------------------------------------------------------------------

OUT = -c#                       # Object file output only

#-------------------------------------------------------------------------------
# Compiler/assembler debug options
#-------------------------------------------------------------------------------

DBG = -g#                       # Enable debug

#-------------------------------------------------------------------------------
# Compiler optimization options
#-------------------------------------------------------------------------------

OPT = -Ospace -O2#              # Full compiler optimization for space

#-------------------------------------------------------------------------------
# Compiler code generation options
#-------------------------------------------------------------------------------

!IF "$(PLATFORM)" == "BIGEND32ENUM"
ENUM = -fy#                         # Enums stored as ints
END = -bigend#                      # Compile for big endian memory architecture
!ELSE
END = -littleend#                   # Compile for little endian memory architecture
!ENDIF
ZO = -zo#                           # Compile each function in a separate module

CODE = $(END) $(ZO) $(ENUM)#


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

INC = -I. -I"$(SUPPORT_INCDIR)"#

#-------------------------------------------------------------------------------
# Compiler pragma emulation options
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Linker options
#-------------------------------------------------------------------------------

LINK_CMD = -o#                   #Command line option to specify output file
                                 #on linking

ROPILINK = -ropi#     #Link image as Read-Only Position Independent

LINK_ORDER = -first AEEMod_Load#

#-------------------------------------------------------------------------------
# HEXTOOL options
#-------------------------------------------------------------------------------

BINFORMAT = -bin#

#-------------------------------------------------------------------------------
# Compiler flag definitions
#-------------------------------------------------------------------------------

CFLAGS = $(OUT) $(DEFINES) $(PLATFORM_DEFINES) $(CPU) $(APCS) $(CODE) $(CHK) \
         $(DBG) $(OPT) $(MYINC) $(INC)#

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

# the -entry flag is not really needed, but it keeps the linker from reporting
# warning L6305W (no entry point).
LFLAGS = $(ROPILINK) -rwpi -rw-base 0 -ro-base 0 -entry AEEMod_Load

#============================================================================
#                           DEFAULT SUFFIX RULES
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

!IFDEF NOAPPGEN
SHARED_OBJ = $(EXTRA_OBJS)
!ELSE
SHARED_OBJ = $(COMMONDIR)\AEEAppGen.o $(COMMONDIR)\AEEModGen.o $(EXTRA_OBJS)
!ENDIF

SRC_FILE = $(@F:.o=.c)#                 # Input source file specification
SRC_CPP_FILE = $(@F:.o=.cpp)#           # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@)#             # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c .cpp

#----------------------------------------------------------------------------
# default rule - place this early in file so it is picked up first
#----------------------------------------------------------------------------
all : $(TARGETDIR)\exist $(COMMONDIR)\exist $(TARGET).$(MODULE)


#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c{$(TARGETDIR)}.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(APP_DIR_CC) $(CFLAGS) $(OBJ_FILE) $(SRC_FILE)
        @echo ---------------------------------------------------------------


{$(SUPPORT_DIR)}.c{$(COMMONDIR)}.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(SUPPORT_DIR_CC) $(CFLAGS) $(OBJ_FILE) $(SUPPORT_DIR)\$(SRC_FILE)
        @echo ---------------------------------------------------------------

.cpp{$(TARGETDIR)}.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(APP_DIR_CPP) $(CFLAGS) $(OBJ_FILE) $(SRC_CPP_FILE)
        @echo ---------------------------------------------------------------

#----------------------------------------------------------------------------
# Mixed source/assembly inference rule
#----------------------------------------------------------------------------

.c.mix:
       @echo ---------------------------------------------------------------
       @echo OBJECT $(@F)
       $(APP_DIR_CC) -S -fs $(CFLAGS) $(OBJ_CMD) $@ $<
       @echo ---------------------------------------------------------------

#----------------------------------------------------------------------------
# Shared object targets
#----------------------------------------------------------------------------

APP_OBJS = $(SHARED_OBJ) $(LOCAL_OBJS)


$(TARGET).$(MODULE) : $(TARGET).$(EXETYPE)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(HEXTOOL)  -output $(TARGET).$(MODULE) $(BINFORMAT) $(TARGET).$(EXETYPE)

$(TARGET).$(EXETYPE) : $(APP_OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LFLAGS) $(APP_OBJS) $(MYLIBS) $(LINK_ORDER)


#----------------------------------------------------------------------------
# Clean target
#----------------------------------------------------------------------------

# The object subdirectory, target image file, and target hex file are deleted.

clean :
        @echo ---------------------------------------------------------------
        @echo CLEAN
        -del $(SHARED_OBJ) $(LOCAL_OBJS)
        -del /f $(TARGET).$(EXETYPE)
        -del /f $(TARGET).$(MODULE)
!       IF EXIST($(TARGETDIR))
        -rmdir /S /Q $(TARGETDIR)
!       ENDIF
!       IF EXIST($(COMMONDIR))
        -rmdir /S /Q $(COMMONDIR)
!       ENDIF#COMMONDIR EXIST
        @echo ---------------------------------------------------------------

#----------------------------------------------------------------------------
# Directory targets
#----------------------------------------------------------------------------

$(TARGETDIR)\exist :
!       IF !EXIST($(TARGETDIR)\exist)
        @echo ---------------------------------------------------------------
        @echo Creating $(@D) Directory
        @mkdir $(@D)
        @echo Building $(@D) > $(@D)\exist
        @echo ---------------------------------------------------------------
!       ENDIF

$(COMMONDIR)\exist :
!       IF !EXIST($(COMMONDIR)\exist)
        @echo ---------------------------------------------------------------
        @echo Creating $(@D) Directory
        @mkdir $(@D)
        @echo Building $(@D) > $(@D)\exist
        @echo ---------------------------------------------------------------
!       ENDIF

$(COMMONDIR)\AEEAppGen.o : $(SUPPORT_DIR)\AEEAppGen.c \
                           $(SUPPORT_INCDIR)\AEEAppGen.h

$(COMMONDIR)\AEEModGen.o : $(SUPPORT_DIR)\AEEModGen.c \
                           $(SUPPORT_INCDIR)\AEEModGen.h

$(TARGETDIR)\$(TARGET).o : $(SUPPORT_INCDIR)\AEEModGen.h \
                           $(SUPPORT_INCDIR)\AEEAppGen.h

