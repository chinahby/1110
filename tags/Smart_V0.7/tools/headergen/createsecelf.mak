#============================================================================
#  Name:
#    CREATESECELF.MAK
#
#  Description:
#    Makefile to generate executable for the secure ELF tool.
#
#   The above targets can be made with the following command:
#
#     make -f createsecelf.mak [createsecelf/clean]
#
# Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
# when          who     what, where, why
# -------       ---     --------------------------------------------------------
# 04/10/05      chs     Added pboot_melf.c and pboot_genhash.c
# 01/14/05      chs     created.
#============================================================================

ifeq ($(BUILD_UNIX), yes)
	NOCYGWIN=
else
	NOCYGWIN=-mno-cygwin
endif

SECELF_C_SOURCES=       createsecelf.c  \
			pboot_melf.c	\
			pboot_genhash.c	\
                        filetool.c

clean:
	@-if test -f createsecelf.exe; then rm -f  createsecelf.exe; fi

all:    clean createsecelf
# Image file
#-------------------------------------------------------------------------------
# C code inference rules
#-------------------------------------------------------------------------------
createsecelf: $(SECELF_C_SOURCES)
	@echo ------------------------------------------------------------------------
	@echo OBJECT $@
	gcc $(NOCYGWIN) -g -D BUILD_BOOT_CHAIN -I../../drivers/mibib -I $(CFG_DATA) $(SECELF_C_SOURCES) -o createsecelf
	@echo ------------------------------------------------------------------------
