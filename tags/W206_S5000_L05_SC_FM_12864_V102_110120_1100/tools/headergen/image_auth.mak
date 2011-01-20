#============================================================================
#  Name:
#    IMAGE_AUTH.MAK
#
#  Description:
#    Makefile to generate executable for the image authentication software.
#
#   The above targets can be made with the following command:
#
#     make -f image_auth.mak [all/clean]
#
# Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# when		who     what, where, why
# -------    	---     --------------------------------------------------------
# 11/30/07      ps      Adding support for SECBOOT 2.0
# 12/15/05      eav     Included incpaths.min to define $(CFG_DATA)
# 04/14/05      tkuo    Add -mno-cygwin for static cygwin and change image_auth
#                       to all.
# 01/11/04	chs	created.
#============================================================================

include ../../build/ms/incpaths.min

IMGAUTH_C_SOURCES=	image_auth.c	\
			filetool.c
all:image_auth
clean:
	@-if test -f image_auth.exe; then rm -f  image_auth.exe; fi

ifeq ($(BUILD_UNIX), yes)
	NOCYGWIN=
else
	NOCYGWIN=-mno-cygwin
endif

ifndef CUSTNAME
  CUSTNAME=RXBRLA# default to SMBRLA for cust file  name.
endif
BUILD_LC      := $(shell perl -e 'print lc("$(CUSTNAME)")')
CUSTFILE       = cust$(BUILD_LC).h
CUSTH   = -DCUST_H=\"$(CUSTFILE)\"#     # Feature include file definition

# Image file
#-------------------------------------------------------------------------------
# C code inference rules
#-------------------------------------------------------------------------------
image_auth:	$(IMGAUTH_C_SOURCES)
	@echo ------------------------------------------------------------------------
	@echo OBJECT $@
	@gcc -DBUILD_BOOT_CHAIN $(CUSTH) -I $(MIBIB) -I $(BOOT_COMMON_SHARED_BOOT) $(CFG_DATA) $(IMGAUTH_C_SOURCES) $(ALL_INCLUDES) -I. -I- -o image_auth
	@echo ------------------------------------------------------------------------
