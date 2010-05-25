#============================================================================
#  Name:
#    NORIMGTTOOL.MAK
#
#  Description:
#    Makefile to generate executable for the Nor Image tool software.
#
#   The above targets can be made with the following command:
#
#     make -f norimgtool.mak [norimgtool/clean]
#
# Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# when		who     what, where, why
# -------    	---     --------------------------------------------------------
# 05/20/05      chs     Force norimgtool to be recompiled every time a amss
#                       build is made.
# 01/11/04	chs	created.
#============================================================================

NORIMG_C_SOURCES=	norimgtool.c	\
			filetool.c
all: clean norimgtool

clean:
	@-if test -f norimgtool.exe; then rm -f  norimgtool.exe; fi

ifeq ($(BUILD_UNIX), yes)
	NOCYGWIN=
else
	NOCYGWIN=-mno-cygwin
endif

# Image file
#-------------------------------------------------------------------------------
# C code inference rules
#-------------------------------------------------------------------------------
norimgtool: $(NORIMG_C_SOURCES)
	@echo ------------------------------------------------------------------------
	@echo OBJECT $@
	gcc $(NOCYGWIN) -D BUILD_BOOT_CHAIN -I../../drivers/mibib -I $(CFG_DATA) $(NORIMG_C_SOURCES) -o norimgtool
	@echo ------------------------------------------------------------------------
