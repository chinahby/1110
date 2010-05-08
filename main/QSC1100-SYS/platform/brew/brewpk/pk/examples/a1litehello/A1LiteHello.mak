#============================================================================
#  Name:
#    A1LiteHello.MAK
#
#  Description:
#    Makefile to build the A1LiteHello downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#     filename.mix -  mixes assembly and source
#
#   The above targets can be made with the following command:
#
#     nmake /f A1LiteHello.mak [target]
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

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

SDKHOME    = ..\..\..\sdk#

TARGET     = a1litehello#           # Target name for output files and object dir
TARGETDIR = $(TARGET)

MYINC      = -I..\..\inc#

#------------------------------------------------------------------------------
# Application objects
#------------------------------------------------------------------------------

NOAPPGEN = TRUE

LOCAL_OBJS =  $(TARGETDIR)\$(TARGET).o \
              $(TARGETDIR)\A1LiteAppGen.o \
              $(TARGETDIR)\A1LiteModGen.o \
              $(TARGETDIR)\A1LiteEnvGen.o

#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------

!INCLUDE "..\example_make_common.min"

{..\..\src}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) ..\..\src\$(SRC_FILE)
        @echo ---------------------------------------------------------------

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------
$(TARGETDIR)\$(TARGET).o : $(TARGET).c
$(TARGETDIR)\A1LiteAppGen.o : ..\..\src\A1LiteAppGen.c
$(TARGETDIR)\A1LiteModGen.o : ..\..\src\A1LiteModGen.c
$(TARGETDIR)\A1LiteEnvGen.o : ..\..\src\A1LiteEnvGen.c
#$(TARGETDIR)\$(TARGET).o : $(TARGET).h


