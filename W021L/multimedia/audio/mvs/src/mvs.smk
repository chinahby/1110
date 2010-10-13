#******************************************************************************
# FILE:     mvs.smk
#
# PURPOSE:
#    Makefile script for the mvs sub-build 
#
# DESCRIPTION:
#    This file contains user-specified info about source groups, option switch
#    groups, build rules  for  specific code modules, etc., which serve as the
#    contents of the user's typical sub-build
#
# USAGE:
#    This file is normally used as the entry-point to the local sub-build from
#    the top-level build. Storage  directories  for certain derived objects may
#    differ between a build invoked  directly  from  this  directory  and  that
#    invoked  as  a  nested  sub-build  from  the  top-level. Moreover,  macros
#    exported  from  the  top-level  may  normally  not  be  available in the 
#    case  of  a  direct  build  invocation. Hence  invoking  directly in this
#    sub-directory is discouraged.
#
#    If the user must invoke a build directly in this directoty, the following
#    command-line may be used :
#
#         make -f mvs.smk [make-target] 
#
#    [make-target] is optional, for normal builds and rebuilds.
#
# PRE_REQUISITES:
#    1. On UNIX : GNU tools, normally available on most workstations
#    2. On WIndows : GNU tools for Windows, eg. Cygnus, and Perl 5+. 
#
#    Note  that  every  makefile  script that is included - either directly or 
#    indirectly - must be available and readable.
#
# Copyright (c) 2002 by Qualcomm, Inc.  All Rights Reserved. 
#******************************************************************************

#==============================================================================
# Set up the Makefiles List Driving the Build
#==============================================================================

MAKEFILELIST = mvs.smk

#==============================================================================
# Set up Sources and Other Definitions
#==============================================================================

# List of all buildable sources
SOURCES := $(wildcard *.c) $(wildcard *.s)

# Log level for this subsystem
SMK_CDEFINES := $(MVS_MSG_LVL)

# Include directory search dependencies
include $(BEP)/$(INC_PATH_PREFIX)incpaths.gmk

# List of compiler include header directory search paths
SMK_CINCPATH := $(MVS_CINCPATH)

# List of assembler include header directory search paths
SMK_AINCPATH := $(MVS_AINCPATH)

#==============================================================================
# Include Script for Shared Definitions and Setups
#==============================================================================

# Include project-wide shared macros, rules and procedures
include $(BEP)/shareall.gmk

#==============================================================================
# Inclusion of Top-level Target Build Rules Script
#==============================================================================

include $(BEAMUPCORE)/combuild.gmk

