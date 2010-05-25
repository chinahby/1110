#============================================================================
#  Name:
#    OSBL.MAK
#
#  Description:
#    Makefile to build the Operating System Secondary Boot Loader
#    code software.
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
#     make -f osbl.mak [target] FLASH_TYPE=<flash_type> BUILD_ID=<build_id>
#
# Assumptions:
#   1. The ARM tools are installed.
#   2. Perl is available either locally or via the environment path.
#
# Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# 
# $Header: //depot/asic/qsc1100/romboot/osbl/osbl.mak#2 $
#
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 04/01/08   MJS     Merge SB20 sandbox with msmshared.
#============================================================================


#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

LINKOBJS_PATH   = ../..
TARGET          = osbl_$(BUILD)#   # Target name for output files
TARGETDIR       = $(LINKOBJS_PATH)/osblobj_$(BUILD)#         # Name for object directory
MAKEFILE        = osbl#            # Name of this make file (without extension)
TOOL_FILE       = osbl_tools.min#  # Make include file that specifies tools commands
OBJ_FILE        = osbl_objs.min#
SCLFILE_IN      = osbl_$(FLASH_TYPE).scl#       # Target input scatter load file
SCLFILE         = $(LINKOBJS_PATH)/osbl_$(BUILD).scl#   # Target output scatter load file
LIB_FILE	= osbl_libs.min#   # Target library file
FLAG_FILE       = osbl_flags.min#
INCPATH_FILE    = osbl_incpaths.min#
RULE_FILE       = ../../shared/src/osbl_rules.min#
TARGET_FILE     = osbl_target.min

LINT_PATH       = c:/apps/lint80/lint-nt# Lint path
LINT_CONFIG_FILE = ../../../../../../build/ms/amss_std.lnt -u#Lint config file which applys to UMTS code.
ENABLE_LINT  =no#    # yes: enable LINT; no: disable LINT.
LINTDIR = $(LINKOBJS_PATH)/osbllint_$(BUILD)#    # name of osbl's lint directory.

#===============================================================================
#                         COMPILE FLAG DEFINITIONS
#===============================================================================
#-------------------------------------------------------------------------------
# Common compile flag definitions
#-------------------------------------------------------------------------------
include $(FLAG_FILE)
include $(INCPATH_FILE)

#===============================================================================
#                             TOOL DEFINITIONS
#===============================================================================
# pull in the appropriate tools definitions based on the compiler being used.
include $(TOOL_FILE)


#===============================================================================
#                         TARGET OBJECT FILE LIST
#===============================================================================
#-------------------------------------------------------------------------------
# Common Object group definitions
#-------------------------------------------------------------------------------
include $(LIB_FILE)
include $(OBJ_FILE)

#-------------------------------------------------------------------------------
# Object groups that are specific to this target
#-------------------------------------------------------------------------------
# INSERT OBJECTS MACROS HERE
# Build the full object list for this target

include $(TARGET_FILE)

#===============================================================================
#                               TARGET RULES
#===============================================================================
#-------------------------------------------------------------------------------
# Common target rules
#-------------------------------------------------------------------------------
include $(RULE_FILE)


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
