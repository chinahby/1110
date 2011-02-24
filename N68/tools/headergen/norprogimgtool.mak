#============================================================================
#  Name:
#    NORIMGTOOL.MAK
#
#  Description:
#    Makefile to generate executable for the Nor Image tool software.
#
#   The above targets can be made with the following command:
#
#     make -f norprogimgtool.mak [all/norprogimgtool/clean]
#
# Copyright (c) 2004-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# when          who     what, where, why
# -------       ---     --------------------------------------------------------
# 11/30/07      ps      Adding support for SECBOOT 2.0
# 05/07/07      amw     Added NOR partition table support.
# 11/22/06      amw     Ported for SC2X.
# 05/20/05      chs     Force norprogimgtool to be recompiled every time a amss
#                       build is made.
# 01/11/04      chs     created.
#============================================================================

include ../../build/ms/incpaths.min
include ../../core/build/core_incpaths.min

NORIMG_C_SOURCES=       norprogimgtool.c \
                        pboot_melf.c     \
                        filetool.c       \
                        parse_system_partition.c

all: clean norprogimgtool

clean:
	@-if test -f norprogimgtool.exe; then rm -f  norprogimgtool.exe; fi

ifeq ($(BUILD_UNIX), yes)
  NOCYGWIN=
else
  NOCYGWIN=-mno-cygwin
endif

ifndef CUSTNAME
  CUSTNAME=SBAXP# default to SBAXP for cust file  name.
endif
BUILD_LC      := $(shell perl -e 'print lc("$(CUSTNAME)")')
CUSTFILE       = cust$(BUILD_LC).h
CUSTH   = -DCUST_H=\"$(CUSTFILE)\"#     # Feature include file definition

# Image file
#-------------------------------------------------------------------------------
# C code inference rules
#-------------------------------------------------------------------------------
norprogimgtool: $(NORIMG_C_SOURCES)
	@echo ------------------------------------------------------------------------
	@echo OBJECT $@
	@gcc -g -D BUILD_BOOT_CHAIN $(CUSTH) -I$(MIBIB) -I $(BOOT_COMMON_SHARED_BOOT) $(NORIMG_C_SOURCES) $(ALL_INCLUDES) -I. -I- -o norprogimgtool
	@echo ------------------------------------------------------------------------
