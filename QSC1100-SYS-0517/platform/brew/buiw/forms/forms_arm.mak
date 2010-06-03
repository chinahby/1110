#============================================================================
#  Name:
#    forms_arm.mak
#
#  Description:
#    Makefile to build brap with ADS 1.0
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     make -f forms_arm.mak [target]
#
# Assumptions:
#   2. ../xmod exists and contains the include makefile and xmodstub.c 
#       (the ARM mode AEEMod_Load entry stub)
#
#        Copyright © 2000-2007 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Set module name
#----------------------------------------------------------------------------

MODNAME = forms

#----------------------------------------------------------------------------
# BREW SDK location
#
# Change this to location of the BREW sdk directory, or define it on 
#   the nmake command line
#  
#----------------------------------------------------------------------------

BREWDIR := $(subst \,/,$(BREWDIR))

WIDGETDIR := $(subst \,/,$(WIDGETDIR))

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
XMODDIR=../xmod
endif

MODVPATH:=$(VPATH)

# LINT: Use LINTOUTPUT=TRUE or LINTFILE=TRUE to enable lint
ifneq (,$(filter TRUE,$(LINTOUTPUT) $(LINTFILE)))
ifndef LINT_DIR
ifndef BUITOOLSDIR
$(error BUITOOLSDIR is not defined)
else
LINT_DIR=$(subst \,/,$(BUITOOLSDIR))/lint
endif
endif
export LINT_MIN=$(LINT_DIR)/lint_rules.min
export LINT_PROJECT=$(LINT_DIR)/$(MODNAME).lnt
include $(LINT_MIN)
export XMOD_POSTCC_CMDS = $(LINT_COMPILE)
export XMOD_POSTELF_CMDS = $(LINT_MODULE_CHECK)
endif

# LINT: we always want to try and remove the lint objects during clean
export XMOD_CLEAN_CMDS = rm -rf lint/*.lo*

#----------------------------------------------------------------------------
# Pull in rwalker's makie wakie thingie, include last!
#----------------------------------------------------------------------------

include $(XMODDIR)/xmod_arm.min


