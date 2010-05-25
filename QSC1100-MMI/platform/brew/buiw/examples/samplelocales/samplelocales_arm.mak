#============================================================================
#  Name:
#    samplelocales_arm.mak
#
#  Description:
#    Makefile to build with ADS 1.0
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     make -f widget_arm.mak [target]
#
#        Copyright © 2000-2001 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Set module name
#----------------------------------------------------------------------------

MODNAME = samplelocales

#----------------------------------------------------------------------------
# BREW SDK location
#
# Change this to location of the BREW sdk directory, or define it on 
#   the nmake command line
#  
#----------------------------------------------------------------------------

BREWDIR := $(subst \,/,$(BREWDIR))

#----------------------------------------------------------------------------
# Set MODOBJS, these'll be compiled and pulled in
#----------------------------------------------------------------------------

export MODOBJS = $(notdir $(patsubst %.c,%.o,$(SOURCES)))

MODLIBS = 

MODINC = -I./inc -I"$(WIDGETDIR)/inc" -I"$(BREWDIR)/inc" -I"$(BREWDIR)/../OEM/inc"  -I"$(BREWPK)/inc" -I"$(BREWDIR)/inc"

#----------------------------------------------------------------------------
# Overrideable from the command line
#----------------------------------------------------------------------------

ifndef XMODDIR
XMODDIR=../../xmod
endif

MODVPATH=src

#----------------------------------------------------------------------------
# Pull in rwalker's makie wakie thingie, include last!
#----------------------------------------------------------------------------

include $(XMODDIR)/xmod_arm.min


