#============================================================================
#  Description:
#    Makefile to build ARM device image.
#
#   Due to prerequisites required in environment variables such as
#   SOURCES and HEADERS, this makefile should only be invoked via the
#   top-level makefiles per-project. Running make from the project
#   directory allows for the following targets:
#
#     mod           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#
# Assumptions:
#   2. ../xmod exists and contains the xmod_arm.min and xmodstub.c 
#       (the ARM mode AEEMod_Load entry stub)
#
#        Copyright © 2000-2007 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------

ifndef PROJECT
$(error PROJECT is undefined)
endif

#----------------------------------------------------------------------------
# Set module name
#----------------------------------------------------------------------------

MODNAME = $(PROJECT)

#----------------------------------------------------------------------------
# Set MODOBJS, these'll be compiled and pulled in
#----------------------------------------------------------------------------

MODOBJS = $(notdir $(patsubst %.c,%.o,$(SOURCES)))

MODINC = $(INCLUDES)

#----------------------------------------------------------------------------
# Overrideable from the command line
#----------------------------------------------------------------------------

ifndef XMODDIR
XMODDIR=$(DIRDEPTH)/xmod
endif

MODVPATH:=$(VPATH)

include $(XMODDIR)/xmod_arm.min
