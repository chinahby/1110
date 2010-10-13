#============================================================================
#  Name:
#    DBL.MAK
#
#  Description:
#    Makefile to build the Device Boot Loader software.
#
#   The following make targets are available in this makefile:
#
#     all           - make .elf and .mbn image files (default)
#     exe           - make the .elf file, but don't create a bin file
#     bin           - make the bin file when there is already an elf file
#     clean         - delete object directory and image files
#     depend        - update makefile dependencies
#     test          - list flags for assembler, compiler and linker
#     tools         - list ARM SDT tool versions
#     filename.mix  - make mixed source / assembly file
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     make -f dbl.mak [target]
#
# Assumptions:
#   1. The ARM tools are installed.
#   2. Perl is available either locally or via the environment path.
#
# Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# 
# $Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl.mak#2 $
#
# when       who     what, where, why
# -------    ---     --------------------------------------------------------
# 07/30/07   ANB     Initial version.
#============================================================================


#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

LINKOBJS_PATH = ../../..
TARGET     = dbl_$(BUILD)#  # Target name for output files
TARGETDIR  = $(LINKOBJS_PATH)/dblobj_$(BUILD)# Name for object directory
#TARGET     = dbl#           # Target name for output files
#TARGETDIR  = dblobj#         # Name for object directory
MAKEFILE   = DBL#           # Name of this make file (without extension)
TOOLFILE   = dbl_tools.min# # Make include file that specifies tools commands
OBJ_FILE   = dbl_objs.min#
SCLFILE_IN = dbl_$(FLASH_TYPE).scl#       # Target input scatter load file
SCLFILE    = $(LINKOBJS_PATH)/dbl_$(BUILD).scl#   # Target output scatter load file
LINT_PATH  = c:/apps/lint80/lint-nt# Lint path
LINT_CONFIG_FILE = ../../../../../../../build/ms/amss_std.lnt -u#Lint config file which applys to UMTS code.

ENABLE_LINT  =no#    # yes: enable LINT; no: disable LINT. 
LINTDIR = $(LINKOBJS_PATH)/dbllint_$(BUILD)#    # name of dbl's lint directory. 

#-------------------------------------------------------------------------------
# Common compile flag definitions
#-------------------------------------------------------------------------------
include dbl_flags.min
include dbl_incpaths.min
ifeq ($(STANDALONE),TRUE)
include dbl_standalone_incpaths.min
endif

#===============================================================================
#                             TOOL DEFINITIONS
#===============================================================================
# pull in the appropriate tools definitions based on the compiler being used.
include $(TOOLFILE)


#===============================================================================
#                         TARGET OBJECT FILE LIST
#===============================================================================
#-------------------------------------------------------------------------------
# Common Object group definitions
#-------------------------------------------------------------------------------
include dbl_libs.min
include $(OBJ_FILE)

#===============================================================================
#                               TARGET RULES
#===============================================================================
#-------------------------------------------------------------------------------
# Common target rules
#-------------------------------------------------------------------------------
include ../../../shared/src/dbl_rules.min

#-------------------------------------------------------------------------------
# Target rules specific to this target or set of objects
#-------------------------------------------------------------------------------
# INSERT RULES HERE

#===============================================================================
#                               DEPENDENCIES
#===============================================================================
# ------------------------------
# DO NOT EDIT BELOW THIS LINE

# End of auto generated dependencies.
