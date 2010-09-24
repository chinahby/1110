#============================================================================
#  Name:
#    pboot_utils.mak
#
#  Description:
#    Makefile to generate executable for the secure ELF tool.
#
#   The above targets can be made with the following command:
#
#     make -f pboot_utils.mak
#
# Copyright (c) 2005-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //depot/asic/qsc1100/tools/headergen/pboot_utils.mak#5 $
# when          who     what, where, why
# -------       ---     --------------------------------------------------------
# 03/19/09      dk      Removing proper definitons for windows builds.
# 11/30/07      ps      Adding support for SECBOOT 2.0
# 05/07/07      amw     Added NOR partition table support.
# 10/31/06      amw     Ported for Harrier build.
# 08/31/06      rjain   Modified for Simple elf support
#============================================================================

# Path to ROOT from the directory where builds are invoked
#
SRCROOT=../..
ROOT=..
include $(SRCROOT)/core/build/core_incpaths.min

CFLAGS=-g -DBUILD_BOOT_CHAIN -I$(MIBIB) -I$(BOOT_COMMON_SHARED_BOOT)

ADD_HASH_OBJS=          pboot_elf_utils.o \
                        pboot_add_hash.o

GENELF_OBJS=            pboot_gen_elf.o \
                        pboot_gen_hash.o \
                        pboot_elf_utils.o

GENHASH_C_OBJS=	        pboot_hash2C.o

MOD_ELF_FLAGS_OBJS=     modify_elf_flags.o \
                        pboot_elf_utils.o

MOD_OFFSETS_OBJS=       dite_modify_offsets.o \
                        pboot_elf_utils.o

READ_FLAGS_OBJS=        read_flags.o \
                        pboot_elf_utils.o

RESERVE_HASH_SEG_OBJS=  reserve_hash_seg.o \
                        pboot_elf_utils.o

GEN_CMM_SCRIPTS_OBJS=   gen_cmm_scripts.o \
                        parse_system_partition.o \
                        pboot_elf_utils.o \
                        filetool.o

GENMIADDRESS_HEADER_OBJS= gen_miaddress_header.o \
                          parse_system_partition.o \
                          filetool.o

GENELF_BIN=pboot_gen_elf
HASH2C_BIN=pboot_hash2C
ADD_HASH_BIN=pboot_add_hash
MODIFY_ELF_FLAGS_BIN=modify_elf_flags
MOD_OFFSETS_BIN=dite_modify_offsets
READ_FLAGS_BIN=read_flags
RESERVE_HASH_SEG_BIN=reserve_hash_seg
GEN_CMM_SCRIPTS_BIN=gen_cmm_scripts
GENMIADDRESS_HEADER_BIN=genmiaddress_header

ADD_HASH_SRC=           pboot_elf_utils.c \
                        pboot_add_hash.c

GENELF_SRC=             pboot_gen_elf.c \
                        pboot_gen_hash.c \
                        pboot_elf_utils.c

GENHASH_C_SRC=          pboot_hash2C.c

MOD_ELF_FLAGS_SRC=      modify_elf_flags.c \
                        pboot_elf_utils.c

MOD_OFFSETS_SRC=        dite_modify_offsets.c \
                        pboot_elf_utils.c

READ_FLAGS_SRC=         read_flags.c \
                        pboot_elf_utils.c

RESERVE_HASH_SEG_SRC=   reserve_hash_seg.c \
                        pboot_elf_utils.c

GEN_CMM_SCRIPTS_SRC=    gen_cmm_scripts.c \
                        parse_system_partition.c \
                        pboot_elf_utils.c \
                        filetool.c

GENMIADDRESS_HEADER_SRC= gen_miaddress_header.c \
                         parse_system_partition.c \
                         filetool.c

CLEAN_FILES=$(ADD_HASH_BIN) $(ADD_HASH_BIN).exe $(GENELF_BIN) $(GENELF_BIN).exe $(HASH2C_BIN) $(HASH2C_BIN).exe $(MOD_OFFSETS_BIN).exe $(READ_FLAGS_BIN).exe $(RESERVE_HASH_SEG_BIN).exe $(ADD_HASH_OBJS) $(GENELF_OBJS) $(MOD_ELF_FLAGS_OBJS) $(MOD_OFFSETS_OBJS) $(READ_FLAGS_OBJS) $(RESERVE_HASH_SEG_OBJS) $(GEN_CMM_SCRIPTS_OBJS) $(GENMIADDRESS_HEADER_OBJS)


# Image file
#-------------------------------------------------------------------------------
# C code inference rules
#-------------------------------------------------------------------------------

add_hash: $(ADD_HASH_SRC)
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(ADD_HASH_SRC) -o $(ADD_HASH_BIN)
else
	gcc $(CFLAGS) $(ADD_HASH_SRC) -o $(ADD_HASH_BIN).exe
endif

gen_elf: $(GENELF_SRC)
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(GENELF_SRC) -o $(GENELF_BIN)
else
	gcc $(CFLAGS) $(GENELF_SRC) -o $(GENELF_BIN).exe
endif

gen_hash_c: $(GENHASH_C_SRC)
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(GENHASH_C_SRC) -o $(HASH2C_BIN)
else
	gcc $(CFLAGS) $(GENHASH_C_SRC) -o $(HASH2C_BIN).exe
endif

mod_elf_flags: $(MOD_ELF_FLAGS_SRC)
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(MOD_ELF_FLAGS_SRC) -o $(MODIFY_ELF_FLAGS_BIN)
else
	gcc $(CFLAGS) $(MOD_ELF_FLAGS_SRC) -o $(MODIFY_ELF_FLAGS_BIN).exe
endif

mod_offsets: $(MOD_OFFSETS_SRC)
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(MOD_OFFSETS_SRC) -o $(MOD_OFFSETS_BIN)
else
	gcc $(CFLAGS) $(MOD_OFFSETS_SRC) -o $(MOD_OFFSETS_BIN).exe
endif


rd_flags:
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(READ_FLAGS_SRC) -o $(READ_FLAGS_BIN)
else
	gcc $(CFLAGS) $(READ_FLAGS_SRC) -o $(READ_FLAGS_BIN).exe
endif

res_hash_seg:
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(RESERVE_HASH_SEG_SRC) -o $(RESERVE_HASH_SEG_BIN)
else
	gcc $(CFLAGS) $(RESERVE_HASH_SEG_SRC) -o $(RESERVE_HASH_SEG_BIN).exe
endif

gen_cmm_scripts:
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(GEN_CMM_SCRIPTS_SRC) -o $(GEN_CMM_SCRIPTS_BIN)
else
	gcc $(CFLAGS) $(GEN_CMM_SCRIPTS_SRC) -o $(GEN_CMM_SCRIPTS_BIN).exe
endif

genmiaddress_header:
ifeq ($(BUILD_UNIX), yes)
	gcc $(CFLAGS) $(GENMIADDRESS_HEADER_SRC) -o $(GENMIADDRESS_HEADER_BIN)
else
	gcc $(CFLAGS) $(GENMIADDRESS_HEADER_SRC) -o $(GENMIADDRESS_HEADER_BIN).exe
endif


all: clean gen_elf gen_hash_c add_hash mod_elf_flags mod_offsets rd_flags res_hash_seg gen_cmm_scripts

clean:
	rm -f $(CLEAN_FILES)

