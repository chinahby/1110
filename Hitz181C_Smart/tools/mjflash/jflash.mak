#============================================================================
#  Name:
#    jflash.mak
#
#  Description:
#    Makefile to build the JFLASHPRG software for all MSM targets
#    MSM specfic information is defined in jf_config.min
#
#  Execution:
#    To execute this make file execute the command line "make -f jflash.mak".
#
#  Targets:
#    Valid targets to build are:   clean depend all test
#
#
#  Copyright (c) 2006-2008 Qualcomm Incorporated. 
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#----------------------------------------------------------------------------

#============================================================================
#
#                          EDIT HISTORY FOR MODULE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jflash.mak#9 $ $DateTime: 2008/11/13 11:42:30 $ $Author: c_surven $
#
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 11/07/08   sv      Supercharge changes to reduce build time
# 09/10/08   sv      Fix include path length issue
# 05/23/08   sv      Modified for RVCT compiler support
# 03/25/08   sv      Added check to allow only 0x0 as ROM BEGIN address
# 02/22/08   op      add MSM_TYPE_CFLAGS and include path
# 09/22/07   op      Remove dependency to Nor source files
# 03/02/07   rt      Supress printing of compiler command line arguments
# 02/28/07   rt      Add support for multi-image NOR targets
# 12/06/06   rt      Create initial version from JFLASH 2.1
#
#============================================================================

MAKEFILE=jflash


#----------------------------------------------------------------------------
#   Directory for objects and .dep files
#	Must be defined BEFORE including *.min
#----------------------------------------------------------------------------
TARGET=jflash

#----------------------------------------------------------------------------
# Include the AMSS include paths file to get all the paths
#----------------------------------------------------------------------------
include ../../build/ms/incpaths.min

#----------------------------------------------------------------------------
#   Define JFLASH default configuration here. There are typically defined
#     in target specific configuration. To support old targets, these
#     defaults will be used.
#----------------------------------------------------------------------------

ifndef RAM_TYPE
  RAM_TYPE="PSRAM"
endif
       


#----------------------------------------------------------------------------
#  Include MSM specific Make configuration file
#  If command shell invoking this makefile defined a configuration file
#  via the environment variable APCONFIGFILE, use it, otherwise, default
#----------------------------------------------------------------------------

ifdef JFCONFIGFILE
  include $(JFCONFIGFILE)
else
  include jf_config.min
endif

ifndef NOR_FLASH_SOURCES
  $(error NOR_FLASH_SOURCES not defined in jf_config.min.)
endif

#------------------------------------------------------------------------------
#  Verify that the ROM BEGIN value is set to 0x0 always.The physical address 
#  conversion is taken care by jf.c internally so the address provided by 
#  Trace32 should always be offset based with flash start address value
#  as 0x0
#------------------------------------------------------------------------------
ifneq ($(JFLASH_ROM_BEGIN), 0x00000000)
  $(error JFLASH_ROM_BEGIN should be defined as 0x00000000)
endif

#----------------------------------------------------------------------------
# Use passed CPU type if present, otherwise use default ARM7TDMI
#----------------------------------------------------------------------------
ifdef ARM_CPU_TYPE
  CPU_TYPE=$(ARM_CPU_TYPE)
else
  CPU_TYPE=ARM926EJ-S
endif


.SUFFIXES:
.SUFFIXES: .s .o .c .dep .h

#----------------------------------------------------------------------------
#
# Set USES_PRINTF to "YES" to build and use printf() as part of binary
# Any value other than exactly "YES" will not build in printf() or use of it
#
#   Independent of test code, but if test NOISY is set, this will be
#   automatically turned on for you
#----------------------------------------------------------------------------

USES_PRINTF=NO

TNOISYLEVEL=3

ifeq '$(USES_PRINTF)' 'YES'
  PRINT_FLAGS = -DCOMPILE_PRINTF -DNOISY -DTNOISY=$(TNOISYLEVEL)
  PRINT_SOURCES=jf_printf.c
endif

#----------------------------------------------------------------------------
#   Define ALL_TOOLS_INCLUDES to ALL_INCLUDES if it is not defined earlier
#----------------------------------------------------------------------------
ifndef ALL_TOOLS_INCLUDES
  ALL_TOOLS_INCLUDES = $(ALL_INCLUDES)
endif

#===============================================================================
#                             TOOL DEFINITIONS
#===============================================================================
# The following environment variables must be defined prior to using this make
# file: ARMBIN, ARMLIB, and ARMINC. In addition the PATH must be updated for
# the ARM tools.

#-------------------------------------------------------------------------------
# Tool path definitions (may be tool set dependent - ADS vs RVCT)
#-------------------------------------------------------------------------------

ifndef ARMHOME
    $(error Compiler not defined! Run batch file to select compiler.)
else
    ARMHOME := $(subst \,/,$(ARMHOME))
    ARMHOME := $(shell echo $(ARMHOME) | tr "[:lower:]" "[:upper:]")
endif

ARM_COMPILER := $(strip $(foreach ID,RVDS ADS, \
    $(findstring $(ID), $(ARMHOME))))

ifdef ARM_COMPILER
  ARM_COMPILER := $(shell echo $(ARM_COMPILER) | tr "[:lower:]" "[:upper:]")
else
    $(error Unknown compiler!)
endif

ifeq ($(ARM_COMPILER), ADS)
  ARMBIN = $(ARMHOME)/bin
  ARMLIB = $(ARMHOME)/lib
endif

ARMBIN := $(subst \,/,$(ARMBIN))
ARMLIB := $(subst \,/,$(ARMLIB))


#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

CC         = @$(ARMBIN)/tcc#        # Thumb 16-bit inst. set ANSI C compiler
ARMCC      = @$(ARMBIN)/armcc#      # ARM 32-bit inst. set ANSI C compiler
ARMCPP     = @$(ARMBIN)/armcpp#     # ARM 32-bit inst. set ANSI C++ compiler
TCPP       = @$(ARMBIN)/tcpp#       # Thumb 16-bit inst. set ANSI C++ compiler
ASM        = @$(ARMBIN)/armasm#     # ARM assembler
ARMAR      = @$(ARMBIN)/armar#      # ARM library archiver
LD         = @$(ARMBIN)/armlink#    # ARM linker
HEXTOOL    = @$(ARMBIN)/fromelf#    # Utility to create hex file from image
BINTOOL    = @$(ARMBIN)/fromelf#    # Utility to create binary file from image

ASM_SCRIPT = asm.pl#                  # Perl script to include .h files in assembly
MDEPEND_SCRIPT = mdepend.pl
GETDEP_SCRIPT = getdep.pl
REDIRECT_SCRIPT = redirect.pl
TEMP = aptemp
JFSCATTERFILE=jflash.scl
JFLASHPARAM=jflash_param.cmm
EXT_ERROR_CMM=jflash_ext_error.cmm

# Defined GEN, the tool which concatenates the hex files generated by the HEXTOOL.
# This also generates the start record and appends it to the end of this file.
#
# Define SHELL, the type of command interpreter used by the environment

ifeq ($(BUILD_UNIX),yes)
  SHELL=sh
#	GEN=ap_gen
else	
  SHELL=bash.exe
#  GEN=ap_gen.exe
endif



GCC=gcc


#----------------------------------------------------------------------------
# Target options:
#
#----------------------------------------------------------------------------

EXE=elf

# Target executable format options:
#   abs  Genelink absolute-positioned binary format (Geneprobe)
#   axe  Paradigm Locate AXE86 format (Debug)
#
#----------------------------------------------------------------------------

#--------------------------------------------------------------------
# Compiler Options
#--------------------------------------------------------------------

ifneq ($(ARM_COMPILER), ADS)
  CPU=--cpu $(CPU_TYPE)
  APCS=--apcs /noswst/interwork
  ENDIAN=--littleend
  DWARF=--dwarf2
  SPLIT_SECTIONS=--split_sections
  WARNS=
  XREF=--xref
  MAP=--map
  ELF=--elf
  INFO=--info
  SCATTER=--scatter
  DEBUG=--debug
  NODEBUG=--no_debug
  SYMBOLS=--symbols
  LIST=--list
  VIA=--via
  I32=--i32
  VERSION=--vsn
  SUPRESS=--diag_suppress 6730
else
  CPU=-cpu $(CPU_TYPE)
  APCS=-apcs /noswst/interwork
  ENDIAN=-littleend
  DWARF=-dwarf2
  SPLIT_SECTIONS=-zo
  WARNS=-Wb -fa
  XREF=-xref
  MAP=-map
  ELF=-elf
  INFO=-info
  SCATTER=-scatter
  DEBUG=-debug
  NODEBUG=-nodebug
  SYMBOLS=-symbols
  LIST=-list
  VIA=-via
  I32=-i32
  VERSION=-vsn
  SUPRESS=
endif


#-- C code inference rules --------------------------------------------------


# Compiler options for code generation
# Do not change the "xs" order in /Oxs, as that removes the "s" option
#
# FLAGS    are the compiler flags you want (without /DCUST_H)  This needs
#          to be separate so DIRCL can construct a special version of
#          /DCUST_H
# CFLAGS   The compile flags everyone EXCEPT DIRCL want.
#

#---------------------------------------------------------
#  Flags to define the MSM types needed
#---------------------------------------------------------

MSM_TYPE_CFLAGS = -DCUST_H=\"$(CUSTFILE)\" $(EXTRA_MSM_DEFINES) \
        $(ALL_TOOLS_INCLUDES) -DBUILD_JFLASH

MSM_TYPE_AFLAGS = -DCUST_H=\"$(CUSTFILE)\" $(EXTRA_MSM_DEFINES) \
        $(ALL_TOOLS_INCLUDES) -DBUILD_JFLASH

#--------------------------------------------------------------------
# CFLAGS for ARM
#--------------------------------------------------------------------
# Compiler flag options:
#   -c   compile only
#   -fK  use Kernighan and Ritchie search rules for locating include files
#   -fl
#   -ffa
#   -fz  in-line SWI may overwrite contents of link register
#   -ez  suppress errors resulting from zero-length array
#   -za0  LDR may not access word aligned addresses
#   -zpi1  Emulate #pragma with -i1, include only once
#   -zpe1  Emulate #pragma with -e1, continue after hash error
#   -apcs  3/32bit/nofp/$(REENT)/$(FPTYPE)/$(INTERWORK)/$(STACK_CHECK)
#               32-bit APCS
#               Do not use frame pointer
#               Re-entrant or
#               Non-rentrant APCS
#               Compile code for ARM/Thumb Interworking
#               SW Stack checking enabled/disabled
#   -g  generate extended debug information for Codeview/Debug
#   -$(ENDIAN)  Select little or big endian, we use little endian
#   -I          Additional search path directories set to "."
#
#----------------------------------------------------------------------------
#  optimize options
#  -O2 = full optimize
#  -Ospace = optimize for smallest code space
#  -O0 = no optimize
#
# OPT_FLAGS=-O0   # this is NO optimization
OPT_FLAGS=-Ospace -O2  #this is the default

OBJ_CMD=-o

#------------------- C code inference rules -------------------------

#--------------------------------------------------------------------
# CFLAGS
#--------------------------------------------------------------------

DMSS_CFLAGS=-DT_ARM $(MSM_TYPE_CFLAGS) $(FLASH_TYPE_CFLAGS) \
            $(PRINT_FLAGS)    

CFLAGS = -c $(CPU) $(APCS) $(ENDIAN) -g $(DWARF) $(WARNS) -I. \
         $(OPT_FLAGS) $(SPLIT_SECTIONS) 

# Assembler flag options:
#
#   -arch       ARM Architecture type 4
#   -apcs       3/32bit$(APCSA)/$(STACK_CHECK)
#               32-bit APCS
#               SW Stack checking enabled/disabled
#   -g  generate extended debug information for Codeview/Debug
#   -DT_MSM3    MSM3000 ASIC
#   -DT_ARM     ARM Target
#   -D_ARM_ASM_ ARM Assembly
#   -DDEBUG     Enable Debug
#
#----------------------------------------------------------------------------


DMSS_AFLAGS= -DT_ARM -D_ARM_ASM_ $(MSM_TYPE_AFLAGS) $(PRINT_FLAGS) -I. 

AFLAGS = $(CPU) $(APCS) $(ENDIAN) -g $(DWARF)

# Linker flag options:
#
#   -Xref  List references between input areas
#   -MAP   Create a map pf the base and size of each are in output image
#   -info  sizes,totals,interwork
#          sizes - give a detailed breakdown of the code and data sizes
#          totals - report total code and data sizes
#          interwork - list all calls for which ARM/Thumb interworking
#                      veneer was necessary
#   -SCATTER generate scatter loaded image
#
#----------------------------------------------------------------------------

LINKFLAGS=$(XREF) $(MAP) $(INFO) sizes,totals,veneers $(SCATTER) \
          $(JFSCATTERFILE) $(SUPRESS) 

#-------------------------------------------------------------------
# New compile rules
#-------------------------------------------------------------------

 $(TARGET)/%.o %.o: %.c
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	@$(CC) $(CFLAGS) $(DMSS_CFLAGS) $(OBJ_CMD) $@ $<
	@echo -----------------------------------------------------------

 $(TARGET)/%.obj: %.c
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	@$(GCC) -g -Wall -U_cdecl -I. $(MSM_TYPE_CFLAGS) $(OBJ_CMD) $@ $<
	@echo -----------------------------------------------------------
	@echo

 $(TARGET)/%.o : %.s
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	@$(CC) -c -E $(DMSS_AFLAGS) $(DMSS_REX_AFLAGS) < jfstart.s | \
	perl $(ASM_SCRIPT) - > ./$(TARGET)/$*.i
	-@$(ASM) $(AFLAGS) $(LIST) ./$(TARGET)/$*.lst ./$(TARGET)/$*.i -o $@
	@echo -----------------------------------------------------------



#  If flash drivers for other than standard NOR flash are desired,
#  define MSM_FLASH_OBJS in ap_flash.min with the objects
#  that are desired.  If not defined, the standard flash objects
#  will be used.
#

JFLASH_COMMON_SOURCES = jf.c \
			jf_id.c \
			jfparam.c


ELF_SOURCES =   jfstart.s \
		jf_mem_init.c \
		$(JFLASH_COMMON_SOURCES) \
		$(NOR_FLASH_SOURCES) \
		$(PRINT_SOURCES)


EXE_SOURCES =   $(JFLASH_COMMON_SOURCES) \
	        $(NOR_FLASH_SOURCES) \
	        $(PRINT_SOURCES)



#-- Object files ------------------------------------------------------------

#----------------------------------------------------------------------------
#   Produce ELF/EXE OBJS and DEPS from SOURCES
#----------------------------------------------------------------------------
ELF_OBJS :=  $(addprefix $(TARGET)/, $(ELF_SOURCES:%.c=%.o))
ELF_OBJS :=  $(ELF_OBJS:%.s=%.o)
EXE_OBJS :=  $(addprefix $(TARGET)/, $(EXE_SOURCES:%.c=%.obj))

vpath %.c $(FLASH) 



##-- Default file to build ---------------------------------------------------

all: depend $(TARGET).$(EXE) $(EXT_ERROR_CMM)  
genexe: $(TARGET)/gccexist mjflash.dep m$(TARGET).exe


$(TARGET)/exist:
	@echo
	@echo ========================================================
	@echo BUILDING $(TARGET)
	@echo ========================================================
	@if [ -f   $(TARGET)/exist ]; then "" ; \
	elif [ -d   $(TARGET) ]; then echo Building   $(TARGET) >   $(TARGET)/exist ;\
	else mkdir $(TARGET); echo Building $(TARGET) >   $(TARGET)/exist ; fi

$(TARGET)/gccexist:
	@echo
	@echo ========================================================
	@echo BUILDING m$(TARGET).exe
	@echo ========================================================
	@if [ -f  $(TARGET)/gccexist ]; then "" ; \
	elif [ -d   $(TARGET) ]; then echo Building m$(TARGET) >   $(TARGET)/gccexist ;\
	else mkdir $(TARGET); echo Building $(TARGET) >   $(TARGET)/gccexist ; fi



#-- Target linking ----------------------------------------------------------

OBJECT_LISTFILE = $(TARGET)/objects.txt

GCC_OBJ_LISTFILE = $(TARGET)/gcc_objects.txt

$(ELF_OBJS) : $(TARGET)/exist

#----------------------------------------------------
#  HEX target
#
#     link, create HEX file
#-----------------------------------------------------
$(TARGET).$(EXE): jflash.scl jflash_param.cmm time_update $(ELF_OBJS)
	@echo
	@echo ========================================================
	@echo TARGET : $@
	@echo ========================================================
	@-if [ -f $(OBJECT_LISTFILE) ]; then rm $(OBJECT_LISTFILE); fi
	@perl $(REDIRECT_SCRIPT) $(OBJECT_LISTFILE) $(ELF_OBJS)
	@$(LD) $(ELF) $(LINKFLAGS) $(DEBUG) $(SYMBOLS) $(LIST) $@.map -o $@ \
		$(LIBS) $(VIA) $(OBJECT_LISTFILE)

m$(TARGET).exe: $(EXE_SOURCES) 
	@echo
	@echo ========================================================
	@echo OBJECT $@
	@echo ========================================================
	@$(GCC) $^ -g -Wall -U_cdecl -I. $(MSM_TYPE_CFLAGS) -I- $(OBJ_CMD) $@
	@echo
	@echo -----------------------------------------------------------
	@-if [ -f $(GCC_OBJ_LISTFILE) ]; then rm $(GCC_OBJ_LISTFILE); fi
	@perl $(REDIRECT_SCRIPT) $(GCC_OBJ_LISTFILE) $(EXE_SOURCES)
	@echo -----------------------------------------------------------

$(TARGET)/jf_id.o : $(TARGET)/exist

time_update: force.frc
	@echo ==========================================================
	-rm -f $(TARGET)/jf_id.o
	@echo ==========================================================
	@echo


clean:
	rm -fr   $(TARGET)
	rm -f $(TARGET).sym
	rm -f $(TARGET).map
	rm -f $(TARGET).hex
	rm -f $(TARGET).elf
	rm -f $(TARGET).elf.map
	rm -f m$(TARGET).exe
	rm -f $(TEMP).hex

veryclean:	clean
	rm -f $(DEPFILE_NAME)
	rm -f jflash.scl
	rm -f jflash_param.cmm
	rm -f *.bak
	rm -f *.map
	rm -f $(EXT_ERROR_CMM)




#-------------------------------------------------------------------------------
# Scatterload file target
#-------------------------------------------------------------------------------
jflash.scl:    force.frc jfin.scl
	@echo ---------------------------------------------------------------
	@echo "========================================================== "
	@echo Creating Scatter Load File
	@echo "========================================================== "
	@echo "###################################################" > $(JFSCATTERFILE)
	@echo "##  GENERATED FILE - DO NOT EDIT" >> $(JFSCATTERFILE)
	@echo "##                                    " >> $(JFSCATTERFILE)
	@echo "## generated:  `date`                 " >> $(JFSCATTERFILE)
	@echo "###################################################" >> $(JFSCATTERFILE)
	@$(CC) -E -DSADDR=$(START_ADDR) -DPADDR=$(PARAM_ADDR) < jfin.scl | \
	perl $(ASM_SCRIPT) - >> $(JFSCATTERFILE)
	@echo
	@echo Done
	@echo ---------------------------------------------------------------


#----------------------------------------------------
#  Generate JFLASH extended error CMM file
#-----------------------------------------------------
$(EXT_ERROR_CMM):	jflash_ext_error.h force.frc
	@echo " "
	@echo " "
	@echo "========================================================== "
	@echo "Generating JFLASH_EXT_ERROR.CMM"
	@echo "========================================================== "
	@echo " "
	@echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; " > $(EXT_ERROR_CMM)
	@echo ";;  GENERATED FILE - DO NOT EDIT      " >> $(EXT_ERROR_CMM)
	@echo ";;                                    " >> $(EXT_ERROR_CMM)
	@echo ";; generated:  `date`                 " >> $(EXT_ERROR_CMM)
	@echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; " >> $(EXT_ERROR_CMM)
	@echo "  " >> $(EXT_ERROR_CMM)
	@echo "  " >> $(EXT_ERROR_CMM)
	@cat jflash_ext_error.h | perl jflash_ext_error.pl >> $(EXT_ERROR_CMM)
	@echo Done
	@echo ---------------------------------------------------------------
	@echo " "
	@echo " "



#----------------------------------------------------
#  PARTITION.BIN target
#
#     Not usually built here, but built as part
#     of AMSS.  This is for convenience of developer
#     modifying the file and not wanting to rebuild
#     AMSS to get it to change.
#-----------------------------------------------------
partition.mbn: $(TARGET)/exist $(TARGET)/partition.o 
	@echo ========================================================
	@echo BUILDING partition.mbn
	@echo ========================================================
	@armlink $(ELF) $(XREF) $(MAP) $(INFO) sizes,totals,veneers \
	$(NODEBUG) $(SYMBOLS) $(LIST) partition.map \
	-o partition.elf $(TARGET)/partition.o
	@echo " "
	@echo " "
	@echo " "
	@echo "---->  Error message above is OK:: Image does not have an entry point. "
	@echo " " 
	fromelf -bin -o partition.mbn partition.elf
	@echo " "
	@echo " "

#----------------------------------------------------
#  Generate JFLASH parameter init CMM file
#-----------------------------------------------------
.PHONY : jflash_param.cmm

jflash_param.cmm:	force.frc
	@echo " "
	@echo " "
	@echo "========================================================== "
	@echo "Generating $(JFLASHPARAM)"
	@echo "========================================================== "
	@echo " "
	@echo " "
	@echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  " > $(JFLASHPARAM)
	@echo ";;  GENERATED FILE - DO NOT EDIT       " >> $(JFLASHPARAM)
	@echo ";;                                     " >> $(JFLASHPARAM)
	@echo ";; generated:  `date`                  " >> $(JFLASHPARAM)
	@echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  " >> $(JFLASHPARAM)
	@echo " global &JflashCodeAddr                " >> $(JFLASHPARAM)
	@echo " &JflashCodeAddr=$(JFLASH_CODE_ADDR)   " >> $(JFLASHPARAM)
	@echo " global &JflashParamAddr               " >> $(JFLASHPARAM)
	@echo " &JflashParamAddr=$(JFLASH_PARAM_ADDR) " >> $(JFLASHPARAM)
	@echo " global &JflashRomBegin                " >> $(JFLASHPARAM)
	@echo " &JflashRomBegin=$(JFLASH_ROM_BEGIN)   " >> $(JFLASHPARAM)
	@echo " global &JflashRomEnd                  " >> $(JFLASHPARAM)
	@echo " &JflashRomEnd=$(JFLASH_ROM_END)       " >> $(JFLASHPARAM)
	@echo " global &ExternalRamType               " >> $(JFLASHPARAM)
	@echo " &ExternalRamType=\"$(RAM_TYPE)\"      " >> $(JFLASHPARAM)
	@echo " global &UseTerm                       " >> $(JFLASHPARAM)
	@echo " &UseTerm=\"$(USES_PRINTF)\"           " >> $(JFLASHPARAM)
	@echo "                                       " >> $(JFLASHPARAM)



#-------------------------------------------------------------------------------
# Test targets
#-------------------------------------------------------------------------------

# The flags and symbol definitions for the compiler, assembler and linker are
# listed for makefile test purposes.

test :
	@echo ------------------------------------------------------------------
	@echo AFLAGS : $(AFLAGS)
	@echo ------------------------------------------------------------------
	@echo DMSS_AFLAGS : $(DMSS_AFLAGS)
	@echo ------------------------------------------------------------------
	@echo CFLAGS : $(CFLAGS)
	@echo ------------------------------------------------------------------
	@echo DMSS_CFLAGS : $(DMSS_CFLAGS)
	@echo ------------------------------------------------------------------
	@echo MSM_TYPE_CFLAGS : $(MSM_TYPE_CFLAGS)
	@echo ------------------------------------------------------------------
	@echo LINKFLAGS : $(LINKFLAGS)
	@echo ------------------------------------------------------------------
	@echo LIBS : $(LIBS)
	@echo ------------------------------------------------------------------
	@echo FLASHINC : $(FLASHINC)
	@echo ------------------------------------------------------------------
	@echo EXTRAINC : $(EXTRAINC)
	@echo ------------------------------------------------------------------
	@echo OBJECTS : $(OBJECTS)
	@echo ------------------------------------------------------------------
	@echo PRINT_OBJS : $(PRINT_OBJS)
	@echo ------------------------------------------------------------------
	@echo NOR_FLASH_SOURCES : $(NOR_FLASH_SOURCES)
	@echo ------------------------------------------------------------------
	@echo ELF_OBJS : $(ELF_OBJS)
	@echo ------------------------------------------------------------------


# The variables which may have been passed in the environment are
# listed for makefile test purposes.

testpass:
	@echo ------------------------------------------------------------------
ifdef JFCONFIGFILE
	@echo "    using passed config file $(JFCONFIGFILE)"
else
	@echo "    using default configfile jf_config.min"
endif
	@echo ------------------------------------------------------------------
ifdef JFSCLFILE
	@echo "    using passed scatter load file $(JFSCATTERFILE)"
else
	@echo "    using default scatter load file $(JFSCATTERFILE)"
endif
	@echo ------------------------------------------------------------------





#===============================================================================
#                               DEPENDENCIES
#===============================================================================

# The dependencies included at the end of this makefile can be automatically
# updated by making the 'depend' target to invoke the following rules.

DEPFILE=jflash
DEPFILE_NAME   = $(DEPFILE).dep
DEPFILE_BACKUP = $(DEPFILE).bak
DEPFILE_TMP    = $(DEPFILE).dep.tmp

%.dep:%.c
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_CFLAGS) -E < $< | perl $(GETDEP_SCRIPT) \
	@$(basename $@).o $< > $*.de_
	@rm -f  $(TARGET)/$(@F).dep
	@mv $*.de_ $*.dep
	@echo -----------------------------------------------------------------

$(TARGET)/%.dep:%.c
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_CFLAGS) -E < $< | perl $(GETDEP_SCRIPT) \
	@$(basename $@).o $< >  $(TARGET)/$*.de_
	@rm -f  $(TARGET)/$*.dep
	@mv  $(TARGET)/$*.de_  $(TARGET)/$*.dep
	@echo -----------------------------------------------------------------


$(TARGET)/%.depend:%.c
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(GCC) -MM -I. $(MSM_TYPE_CFLAGS) $(basename $<).c > $@ 
	@echo -----------------------------------------------------------------

$(TARGET)/%.dep:%.s
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_AFLAGS) $(DMSS_CFLAGS) -E < $< | \
	perl $(GETDEP_SCRIPT) $(basename $@).o $< >  $(TARGET)/$*.de_
	@rm -f  $(TARGET)/$*.dep
	@mv  $(TARGET)/$*.de_  $(TARGET)/$*.dep
	@echo -----------------------------------------------------------------


DEPOBJS := $(ELF_OBJS:.o=.dep)

$(DEPOBJS) : $(TARGET)/exist

depend: $(DEPOBJS) force.frc
	@echo -----------------------------------------------------------------
	@echo Processing dependencies
	@perl $(MDEPEND_SCRIPT) $(DEPFILE_NAME)  $(TARGET) > $(DEPFILE_TMP)
	@-rm -f $(DEPFILE_BACKUP)
	@-mv $(DEPFILE_NAME) $(DEPFILE_BACKUP)
	@mv $(DEPFILE_TMP) $(DEPFILE_NAME)
	@echo -----------------------------------------------------------------

mjflash.dep :  $(TARGET)/gccexist $(EXE_OBJS:.obj=.depend) force.frc 
	@echo 
	@echo -----------------------------------------------------------------
	@echo   Creating $@
	@echo -----------------------------------------------------------------
	@echo "#" > $@
	@for i in ./$(TARGET)/*.depend; do cat "$${i}" >> $@; done
	@echo 

$(DEPFILE_NAME):
	@echo " "
	@echo "Creating $(DEPFILE_NAME)"
	@echo " "
	@echo "# ------------------------------" > $(DEPFILE_NAME)
	@echo "# DO NOT EDIT BELOW THIS LINE" >> $(DEPFILE_NAME)



force.frc:

sources:

-include $(DEPFILE_NAME)
