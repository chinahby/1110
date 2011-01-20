#============================================================================
#  Name:
#    FSBL.MAK
#
#  Description:
#    Makefile to build the Fail Safe Boot Loader software.
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
#     make -f fsbl.mak [target]
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
# $Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl.mak#2 $
#
# when       who     what, where, why
# -------    ---     --------------------------------------------------------
# 07/30/07   ANB     Initial version.
#============================================================================


#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

LINKOBJS_PATH = ../..
TARGET     = fsbl_$(BUILD)#  # Target name for output files
TARGETDIR  = $(LINKOBJS_PATH)/fsblobj_$(BUILD)# Name for object directory
MAKEFILE   = FSBL#           # Name of this make file (without extension)
TOOLFILE   = fsbl_tools.min# # Make include file that specifies tools commands
OBJ_FILE   = fsbl_objs.min#
SCLFILE_IN = fsbl_$(FLASH_TYPE).scl# # Target input scatter load file
SCLFILE    = $(LINKOBJS_PATH)/fsbl_$(BUILD).scl#   # Target output scatter load file
LINT_PATH  = c:/apps/lint80/lint-nt# Lint path
LINT_CONFIG_FILE = ../../../../../../build/ms/amss_std.lnt -u#Lint config file which applys to UMTS code.

ENABLE_LINT  =no#    # yes: enable LINT; no: disable LINT. 
LINTDIR = $(LINKOBJS_PATH)/fsbllint_$(BUILD)#    # name of dbl's lint directory.

#-------------------------------------------------------------------------------
# Common compile flag definitions
#-------------------------------------------------------------------------------
include fsbl_flags.min
include fsbl_incpaths.min
include fsbl_standalone_incpaths.min

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
include fsbl_libs.min
include $(OBJ_FILE)

#===============================================================================
#                               TARGET RULES
#===============================================================================
#-------------------------------------------------------------------------------
# Common target rules
#-------------------------------------------------------------------------------
include ../../shared/src/fsbl_rules.min

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
