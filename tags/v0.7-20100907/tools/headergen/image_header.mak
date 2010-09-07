#============================================================================
#  Name:
#    IMAGE_HEADER.MAK
#
#  Description:
#    Makefile to generate binary OEMSBL and AMSS headers for QSC6055.
#
#
# Copyright (c) 2004-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //depot/asic/qsc1100/tools/headergen/image_header.mak#6 $
#
# when       who     what, where, why
# -------    ---     --------------------------------------------------------
# 03/19/09   dk      Added fix for building the image_header.exe in strip 
#                    and pack builds. Cygwin is removed and need proper 
#                    definitions for ecmake and gnumake, hence added this change.
# 11/30/07   ps      Adding support for SECBOOT 2.0
# 10/26/07   amw     Updated clean target to clean the executable.
# 11/15/06   rjain   Added support to take partition sizes from builds file.
# 11/09/05   eav     Image header will compile all the partitions
# 05/20/05   chs     Force image_header to be recompiled every time a amss
#                    build is made.
# 03/17/05   sd      Added paths to support secure boot.
# 10/21/04   chs     Changed ".bin" to ".mbn"
# 09/29/04   erwinv  Created
#============================================================================


#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

ifeq ($(BUILD_UNIX), yes)
        TARGET     = image_header # Target name for output files
else
        TARGET     = image_header.exe
endif
IMAGE_TYPE = $(IMG)

#===============================================================================
#                         COMPILE FLAG DEFINITIONS
#===============================================================================
#-------------------------------------------------------------------------------
# Include Path and path definitions
#-------------------------------------------------------------------------------
# Paths relative to SRCROOT for each subdir
#
SRCROOT     = ../..
include $(SRCROOT)/core/build/core_incpaths.min

HEADERGEN   = $(SRCROOT)/tools/headergen
PARTITION   = $(SRCROOT)/romboot/partition
BUILDMS        = $(SRCROOT)/build/ms
BUILDCUST      = $(SRCROOT)/build/cust
AMSSPATHFILE   = $(SRCROOT)/build/ms/incpaths.min # path file for AMSS.


ifeq ($(BUILD_UNIX), yes)
        NOCYGWIN=
else
        NOCYGWIN=-mno-cygwin
endif

ifndef CUSTNAME
  CUSTNAME=SNAXP# default to SNAXP for cust file name.
endif
BUILD_LC      := $(shell perl -e 'print lc("$(CUSTNAME)")')
CUSTFILE       = cust$(BUILD_LC).h
CUSTH   = -DCUST_H=\"$(CUSTFILE)\"#     # Feature include file definition

IMAGEHEADER_ALL_PATHS += $(MIBIB) $(HEADERGEN) $(PARTITION) $(BUILDMS) $(BUILDCUST) $(BOOT_COMMON_SHARED_BOOT) $(BOOT_COMMON_TARGET) $(BOOT_COMMON_SHARED_SEC)
IMAGEHEADER_ALL_INCLUDES = $(addprefix -I, $(IMAGEHEADER_ALL_PATHS))

#-------------------------------------------------------------------------------
# Common compile flag definitions
#-------------------------------------------------------------------------------
include $(AMSSPATHFILE)

#===============================================================================
#                         TARGET OBJECT FILE LIST
#===============================================================================
ifndef FLASH_TYPE
IMAGE_C_SOURCE = image_header.c \
                 $(PARTITION)/partition.c
else
IMAGE_C_SOURCE = image_header.c \
#                 $(PARTITION)/partition_$(FLASH_TYPE).c
endif


#===============================================================================
#                               TARGET RULES
#===============================================================================
#-------------------------------------------------------------------------------
# Common target rules
#-------------------------------------------------------------------------------

#===============================================================================
#                               TARGET RULES
#===============================================================================

all : clean $(TARGET)

$(TARGET): $(IMAGE_C_SOURCE)
	gcc -DBUILD_BOOT_CHAIN $(CUSTH) $(IMAGEHEADER_ALL_INCLUDES) $(ALL_INCLUDES) -o $(TARGET) $(IMAGE_C_SOURCE)

clean:
	@-if test -f $(TARGET); then rm -f  $(TARGET); fi
