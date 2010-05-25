#============================================================================
#  Name:
#    ntp.MAK
#
#  Description:
#    Makefile to build the ntp downloadable module.
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
#     nmake /f ntp_client.mak [target]
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

TARGET     = ntp#           # Target name for output files and object dir
TARGETDIR = $(TARGET)

#------------------------------------------------------------------------------
# Application objects
#------------------------------------------------------------------------------

LOCAL_OBJS =  $(TARGETDIR)\$(TARGET).o

#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------

!INCLUDE "..\example_make_common.min"

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------
$(TARGETDIR)\$(TARGET).o : $(TARGET).c
#$(TARGETDIR)\$(TARGET).o : $(TARGET).h


