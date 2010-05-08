#============================================================================
#  Name:
#    ap_arm.mak
#
#  Description:
#    Makefile to build the FLASHPRG software for all MSM targets
#    MSM specfic information is defined in ap_config.min
#
#  Execution:									  
#    To execute this make file execute the command line "make -f ap_arm.mak".
#
#  Targets:
#    Valid targets to build are:   clean depend all test
#
#
#  Copyright (c) 2003-2009 by QUALCOMM, Incorporated.  
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
#  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_arm.mak#19 $ $DateTime: 2009/04/23 00:23:19 $ $Author: spuliven $
#  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_arm.mak#2 
#  
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
#04/09/09    sp      Add UART1DM support
#12/17/08    sv      Fix norprg compilation issue
#12/16/08    rk      USB code refactoring changes mainlined
#11/25/08    rk      EHOSTDL - to build emergency hostdl code separately
#11/10/08    mm      Change dependence to ensure $(TARGETDIR)/exist gets made
#                    before any objects, even on parallel builds
#07/17/08    mm      Fix for "fork: can't reserve memory for stack" issue. 
#                    Use bash instead of sh.
#05/06/08    sv      Fix RVCT compiler warning
#05/06/08    mm      Fix for "fork: can't reserve memory for stack" due to 
#                    long incpaths
#05/06/08    mm      Updated the compiler detection section
#04/25/08    sv      Move HSU definitions to ap_config.min
#02/20/08    op      Support Nand/Nor for hostdl
#02/19/08    mm      Adapted to support Linux file case issues.
#02/12/08    mm      Adapted to retrieve flash sources from flash target 
#                    specific file.
#09/12/07    rt      Add RVCT support
#08/28/07    rt      Integrate HSU (High Speed USB) support
#02/27/07    mm      Updated flags for hostdl
#05/31/06    rt      Use correct case in path for building on Linux
#05/25/06    rt      Define NANDPRG as default tools target
#05/12/06    rt      Reference crc routines from crc.h in services\utils folder
#02/26/06    rt      Reference common files from tools/mjnand folder
#12/05/05    dp      NOR flash driver unification
#10/06/05    eav     Added chmod'ed $(TARGET).hex before copying
#07/26/05    drh     Move flash object list to flash MIN file
#07/21/05    drh     Build from AMSS build
#06/16/05    dng     Change shell to sh.
#03/24/04    drh     Change directory from which to pull NAND flash source
#10/15/03    drh     Add "full" target which does it all
#10/14/03    drh     Add dynamic creation of scatterload file from template
#                    by processing with CPP
#09/18/03    drh     Define USES_PRINTF to compile printing inclusion
#07/21/03    drh     Take from ARMPRG 5 series into 6 series and modify to
#                    support inclusion of flash objects external file.
#                    Use common NAND drivers from DMSS services\efs.
#
# 
#============================================================================

MAKEFILE=hostdl

#----------------------------------------------------------------------------
# Get includes from AMSS
#----------------------------------------------------------------------------
include ../../build/ms/incpaths.min

# if the TARGET name is not set, default it to nandprg. 
# For NOR builds, it will be overwritten to NORPRG below.
ifndef TARGET
   TARGET=nandprg
endif
#----------------------------------------------------------------------------
#  Include FLASH specific Make configuration file
#  This min file can condition values in ap_config.min as necessary, so
#  it is mandatory that it be included first.
#
#  The included MIN file will define the flash object list depending on
#  flash type in use.
#
#  Define target
#----------------------------------------------------------------------------
ifdef FLASHTYPE
	ifeq '$(FLASHTYPE)' 'NAND' 
		#   Directory for objects and .dep files
		#	Must be defined BEFORE including ap_flash.min
		include nandflash/ap_flash.min
		TOOL_DEFINE=BUILD_NANDPRG
	else
		ifeq '$(FLASHTYPE)' 'NOR'
			#   Directory for objects and .dep files
			#	Must be defined BEFORE including ap_flash.min
			TARGET=norprg
			include norflash/ap_flash.min
			TOOL_DEFINE=BUILD_ARMPRG
		else
			FLASHTYPE must be "NAND" or "NOR" only
		endif
	endif 
else
	# Only one of NOR and NAND can be defined as default flash type 

	# Define NAND to be default FLASHTYPE
	    # FLASHTYPE=NAND
	    # TARGET=nandprg
	    # include NANDflash/ap_flash.min
	    # TOOL_DEFINE=BUILD_NANDPRG

	# Define NOR to be default FLASHTYPE
	    FLASHTYPE=NOR
	    TARGET=norprg
	    include norflash/ap_flash.min
	    TOOL_DEFINE=BUILD_ARMPRG
endif

#----------------------------------------------------------------------------
#   Directory for library archives
#----------------------------------------------------------------------------

LIBDIR = $(TARGET)/lib



#----------------------------------------------------------------------------
#  Include MSM specific Make configuration file
#  If command shell invoking this makefile defined a configuration file
#  via the environment variable APCONFIGFILE, use it, otherwise, default
#----------------------------------------------------------------------------

ifdef APCONFIGFILE
	include $(APCONFIGFILE)
else
  include ap_config.min
endif

#----------------------------------------------------------------------------
#   This section deals with FLASH drivers that are in the AMSS tree, and not
#   part of the FLASH source base. Make sure that list of sources is defined
#   in target dependent file.
#----------------------------------------------------------------------------
ifndef FLASH_SOURCES
  $(error FLASH_SOURCES not defined in min file)
endif

ifeq '$(FLASHTYPE)' 'NAND' 
  ifndef JNAND_SOURCES
    $(error JNAND_SOURCES not defined in min file)
  endif
  
  ifndef JNAND_FLAGS
    $(error JNAND_FLAGS not defined in min file)
  endif

  JNAND_OBJS :=  $(addprefix $(TARGET)/, $(JNAND_SOURCES:%.c=%.o))
  JNAND_DEPS :=  $(addprefix $(TARGET)/, $(JNAND_SOURCES:%.c=%.dep))

  #----------------------------------------------------------------------------
  #   Define MSM_FLASH_OBJS to over-ride standard definition for flash layer 
  #   objects
  #----------------------------------------------------------------------------
  MSM_FLASH_OBJS=	$(TARGET_FLASH_OBJS) $(JNAND_OBJS) $(EXTRA_FLASH_OBJS)

endif

#----------------------------------------------------------------------------
# Define Scatter load file as ap_armprg.scl, and ehostdl_nandprg.scl for emeregency feature
#----------------------------------------------------------------------------
APSCATTERFILE=ap_armprg.scl
EHOSTDL_SCATTER_FILE=ehostdl_nandprg.scl

#----------------------------------------------------------------------------
# Use passed CPU type if present, otherwise use default ARM926EJ-S
#----------------------------------------------------------------------------
ifdef ARM_CPU_TYPE
  CPU_TYPE=$(ARM_CPU_TYPE)
else
  CPU_TYPE=ARM926EJ-S
endif


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
	PRINT_OBJS=$(TARGET)/ap_printf.o
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

ASM_SCRIPT = asm.pl#         # Perl script to include .h files in assembly
CAT_SCRIPT = cat.pl#         # Perl script to print output for assembly files
MDEPEND_SCRIPT = mdepend.pl
GETDEP_SCRIPT = getdep.pl
REDIRECT_SCRIPT = redirect.pl
TEMP = aptemp


# Defined GEN, the tool which concatenates the hex files generated by the 
# HEXTOOL. This also generates the start record and appends it to the end
# of this file.
#
# Define SHELL, the type of command interpreter used by the environment

ifeq ($(BUILD_UNIX),yes)
  SHELL=sh
	GEN=ap_gen
else	
  SHELL=bash.exe
  GEN=ap_gen.exe
endif





#--------------------------------------------------------------------
# Target options:
#----------------------------------------------------------------------------

EXE=elf

#--------------------------------------------------------------------
# Additional INCLUDES
#--------------------------------------------------------------------
ifndef ALL_TOOL_INCLUDES
  ALL_TOOL_INCLUDES=$(ALL_INCLUDES)
endif

EXTRAINC=$(ALL_TOOL_INCLUDES) $(FLASHINC)

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
  SYMBOLS=-symbols
  LIST=-list
  VIA=-via
  I32=-i32
  VERSION=-vsn
  SUPRESS=
endif


#--------------------------------------------------------------------
# Additional INCLUDES
#--------------------------------------------------------------------

EXTRAINC=$(ALL_TOOL_INCLUDES) $(FLASHINC)



#----------------------------------------------------------------------------
#  optimize options
#  -O2 = full optimize
#  -Ospace = optimize for smallest code space
#  -O0 = no optimize
#
# OPT_FLAGS=-O0   # this is NO optimization

OPT_FLAGS=-Ospace -O2  # this is the default

OBJ_CMD=-o             # Create object files


#------------------- C code inference rules -------------------------
		 
#--------------------------------------------------------------------
# CFLAGS
#--------------------------------------------------------------------

ifeq ($(TARGET),enandprg)
DMSS_CFLAGS=-DT_ARM $(MSM_TYPE_CFLAGS) $(FLASH_TYPE_CFLAGS) \
            $(PRINT_FLAGS) -I. $(EXTRAINC) -DBUILD_HOSTDL 
else
DMSS_CFLAGS=-DT_ARM $(MSM_TYPE_CFLAGS) $(FLASH_TYPE_CFLAGS) \
            $(PRINT_FLAGS) -I. $(EXTRAINC) $(HSU_FLAGS) -DBUILD_HOSTDL
endif

CFLAGS = -c $(CPU) $(APCS) $(ENDIAN) -g $(DWARF) $(WARNS) -I. \
         $(OPT_FLAGS) $(SPLIT_SECTIONS) -D$(TOOL_DEFINE)


#------------------- Assembler inference rules -------------------------

#--------------------------------------------------------------------
# AFLAGS
#--------------------------------------------------------------------

DMSS_AFLAGS= -DT_ARM -D_ARM_ASM_ -DBUILD_HOSTDL -I. $(EXTRAINC) \
             $(MSM_TYPE_CFLAGS) $(PRINT_FLAGS) $(FLASH_TYPE_CFLAGS)

AFLAGS = $(CPU) $(APCS) $(ENDIAN) -g $(DWARF)


# Linker flag options:
#
#   $(XREF)  List references between input areas   
#   $(MAP)   Create a map pf the base and size of each are in output image
#   $(INFO)  sizes,totals,interwork 
#          sizes - give a detailed breakdown of the code and data sizes
#          totals - report total code and data sizes
#          interwork - list all calls for which ARM/Thumb interworking 
#                      veneer was necessary
#
#   $(SCATTER) generate scatter loaded image 
#
#----------------------------------------------------------------------------
ifeq ($(TARGET),enandprg)
LINKFLAGS=$(XREF) $(MAP) $(INFO) sizes,totals,veneers $(SCATTER) \
          $(EHOSTDL_SCATTER_FILE) $(SUPRESS)
else
LINKFLAGS=$(XREF) $(MAP) $(INFO) sizes,totals,veneers $(SCATTER) \
          $(APSCATTERFILE) $(SUPRESS)
endif
#-------------------------------------------------------------------
# Compile rules
#-------------------------------------------------------------------

$(TARGET)/%.o %.o: %.c
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	@$(ARMCC) $(CFLAGS) $(DMSS_CFLAGS) $(OBJ_CMD) $@ -DMSG_FILE=\"$<\" $<
	@echo -----------------------------------------------------------


$(TARGET)/%.o %.o: %.s
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	$(ARMCC) -c -E $(DMSS_AFLAGS) < $< | \
		perl $(ASM_SCRIPT) > ./$(TARGET)/$*.i 
	@$(ASM) $(AFLAGS) $(LIST) $(TARGET)/$*.lst $(TARGET)/$*.i -o $@
	@echo ----------------------------------------------------------------

#-------------------------------------------------------------------------------
# Expanded macros inference rule
#-------------------------------------------------------------------------------

%.i: %.c
	@echo -----------------------------------------------------------------
	@echo OBJECT $@
	$(ARMCC) -E $(CFLAGS) $(DMSS_CFLAGS) -DMSG_FILE=\"$<\" $(OBJ_CMD) $@ $<
	@echo -----------------------------------------------------------------



#----------------------------------------------------------------------------
#   Produce Transport OBJS and DEPS from SOURCES
#----------------------------------------------------------------------------
# UART_OBJS var to be set in ap_config.min to differentiate ap_uart.c
# (uses UART2)and ap_uart1dm.c (uses UART1DM) via USES_UART1DM_DLOAD
# flag defined in builds file. If not defined in ap_config.min, UART_OBJS
# is defaulted to ap_uart.c
#----------------------------------------------------------------------------
ifndef UART_OBJS
UART_OBJS=$(TARGET)/ap_uart.o
endif

TRANSPORT_OBJS=   $(UART_OBJS) $(USB_OBJS)

#----------------------------------------------------------------------------
#   Produce Flash driver OBJS and DEPS from FLASH_SOURCES
#----------------------------------------------------------------------------
ifeq '$(FLASHTYPE)' 'NAND' 
  FLASH_OBJS =  $(JNAND_OBJS) \
		        $(TARGET)/sbuffer.o
else
  FLASH_OBJS =  $(addprefix $(TARGET)/, $(FLASH_SOURCES:%.c=%.o))
endif

#----------------------------------------------------------------------------
#   All OBJECTS list
#----------------------------------------------------------------------------
		  
# All Object list for Emergency Hostdl
ifeq ($(TARGET),enandprg)
  OBJECTS=$(TARGET)/ap_startup.o \
		  $(TARGET)/ap_armprg.o \
		  $(TARGET)/ap_packet.o \
		  $(FLASH_OBJS) \
		  $(PRINT_OBJS) \
		  $(TARGET)/ap_uart.o \
		  $(TARGET)/crc.o \
		  $(TARGET)/ap_msm.o \
		  $(TARGET)/ap_flash.o \
		  $(TARGET)/ap_id.o \
		  $(TARGET)/mc_init.o
else
  OBJECTS=$(TARGET)/ap_startup.o \
		  $(TARGET)/ap_armprg.o \
		  $(TARGET)/ap_packet.o \
		  $(FLASH_OBJS) \
		  $(TRANSPORT_OBJS) \
		  $(PRINT_OBJS) \
		  $(TARGET)/crc.o \
		  $(TARGET)/ap_msm.o \
		  $(TARGET)/ap_flash.o \
		  $(TARGET)/ap_id.o
endif

ifeq '$(FLASHTYPE)' 'NAND' 
  HOSTDL_ARM_SOURCES = \
              ap_startup.s \
              ap_armprg.c \
              ap_packet.c \
              crc.c \
              ap_msm.c \
              $(JNAND_ARM_FLASH_SOURCES) \
              $(JNAND_HOSTDL_SOURCES) \
              $(JFLASH_ARM_FLASH_SOURCES) \
              $(JFLASH_HOSTDL_SOURCES)
  
  #----------------------------------------------------------------------------
  #   Rules to produce OBJS and DEPS from SOURCES
  #----------------------------------------------------------------------------
  ELF_OBJS :=  $(addprefix $(TARGET)/, $(HOSTDL_ARM_SOURCES:%.c=%.o))
  ELF_OBJS :=  $(ELF_OBJS:%.s=%.o)
endif

#----------------------------------------------------------------------------
#   All Libraries
#----------------------------------------------------------------------------

LIBS= $(HSU_LIBS)


##-- Default file to build ---------------------------------------------------
all: depend $(TARGET).$(EXE)


##-- Target to do everything ------------------------------------------------
full: clean depend all


##-- Target to build libraries ---------------------------------------------
libs : $(LIBDIR)/exist $(LIBS)


$(TARGET)/exist:
	@echo
	@echo
	@echo ========================================================
	@echo BUILDING $(TARGET)
	@echo ========================================================
	@echo
	@if [ -f   $(TARGET)/exist ]; then "" ; \
	elif [ -d   $(TARGET) ]; then echo Building   $(TARGET) >   $(TARGET)/exist ;\
	else mkdir $(TARGET); echo Building $(TARGET) >   $(TARGET)/exist ; fi

$(LIBDIR)/exist:  $(TARGET)/exist
	@if test ! -d $(LIBDIR); then mkdir $(LIBDIR); fi
ifeq ($(MAKE_LIBS), yes)
  include $(HSU_CORE_MIN_FILE)
endif

$(OBJECTS) :  $(TARGET)/exist

#-- List of objects ----------------------------------------------------------
OBJECT_LISTFILE = objects.txt

#------- List of objects for emergency hostdl      ---------------------------------
EHOSTDL_OBJECT_LISTFILE = ehostdl_objects.txt

#----------------------------------------------------
#  TARGET BINARY
#
#     link, create HEX file, add S-Record to HEX file
#-----------------------------------------------------
nandprg.$(EXE): time_update ap_armprg.scl  libs $(OBJECTS)
	@echo
	@echo ------------------------------------------------------------------
	@echo LINKING $@
	@echo ------------------------------------------------------------------
	@-if [ -f $(OBJECT_LISTFILE) ]; then rm $(OBJECT_LISTFILE); fi	
	@perl $(REDIRECT_SCRIPT) $(OBJECT_LISTFILE) $(OBJECTS) $(LIBS)
	@$(LD) $(ELF) $(LINKFLAGS) $(DEBUG) $(SYMBOLS) $(LIST) $@.map -o $@ $(LIBS) \
		$(VIA) $(OBJECT_LISTFILE)
	@$(HEXTOOL) $(I32) -o $(TARGET).hex $(TARGET).$(EXE) 
	@mv $(TARGET).hex $(TEMP).hex
	@chmod +x ./$(GEN)
	@./$(GEN) -t $(TARGET).hex -s $(S_ADDR) -f $(TEMP).hex
	@chmod +rwx $(TARGET).hex
	@echo ------------------------------------------------------------------
	@echo CREATING   $(FINALNAME).hex
	@echo ------------------------------------------------------------------
	@cp $(TARGET).hex $(FINALNAME).hex
	@echo
	@echo

norprg.$(EXE): time_update ap_armprg.scl  libs $(OBJECTS)
	@echo
	@echo ------------------------------------------------------------------
	@echo LINKING $@
	@echo ------------------------------------------------------------------
	@-if [ -f $(OBJECT_LISTFILE) ]; then rm $(OBJECT_LISTFILE); fi	
	@perl $(REDIRECT_SCRIPT) $(OBJECT_LISTFILE) $(OBJECTS) $(LIBS)
	@$(LD) $(ELF) $(LINKFLAGS) $(DEBUG) $(SYMBOLS) $(LIST) $@.map -o $@ $(LIBS) \
		$(VIA) $(OBJECT_LISTFILE)
	@$(HEXTOOL) $(I32) -o $(TARGET).hex $(TARGET).$(EXE) 
	@mv $(TARGET).hex $(TEMP).hex
	@chmod +x ./$(GEN)
	@./$(GEN) -t $(TARGET).hex -s $(S_ADDR) -f $(TEMP).hex
	@chmod +rwx $(TARGET).hex
	@echo ------------------------------------------------------------------
	@echo CREATING   $(FINALNAME).hex
	@echo ------------------------------------------------------------------
	@cp $(TARGET).hex $(FINALNAME).hex
	@echo
	@echo

enandprg.$(EXE): time_update ehostdl_nandprg.scl  $(OBJECTS)
	@echo
	@echo ------------------------------------------------------------------
	@echo LINKING $@
	@echo ------------------------------------------------------------------
	@-if [ -f $(EHOSTDL_OBJECT_LISTFILE) ]; then rm $(EHOSTDL_OBJECT_LISTFILE); fi	
	@perl $(REDIRECT_SCRIPT) $(EHOSTDL_OBJECT_LISTFILE) $(OBJECTS)
	@$(LD) $(ELF) $(LINKFLAGS) $(DEBUG) $(SYMBOLS) $(LIST) $@.map -o $@ \
		$(VIA) $(EHOSTDL_OBJECT_LISTFILE)
	@$(HEXTOOL) $(I32) -o $(TARGET).hex $(TARGET).$(EXE) 
	@mv $(TARGET).hex $(TEMP).hex
	@chmod +x ./$(GEN)
	@./$(GEN) -t $(TARGET).hex -s $(EHOSTDL_CODE_ADDR) -f $(TEMP).hex
	@chmod +rwx $(TARGET).hex
	@echo ------------------------------------------------------------------
	@echo CREATING   $(EHOSTDL_FINALNAME).hex
	@echo ------------------------------------------------------------------
	@cp $(TARGET).hex $(EHOSTDL_FINALNAME).hex
	@echo
	@echo


time_update: force.frc $(TARGET)/exist
	@-rm -f $(TARGET)/ap_id.o


clean:
	rm -f $(TARGET)/*.o
	rm -f $(TARGET)/*.i
	rm -f $(TARGET)/*.lst
	rm -f $(TARGET)/exist
	rm -f $(TARGET).sym
	rm -f $(TARGET).map
	rm -f $(TARGET).hex
	rm -f $(TARGET).elf
	rm -f $(TARGET).elf.map
	rm -f $(TARGET).exe
	rm -f $(FINALNAME).hex
	rm -f $(TEMP).hex
	rm -f $(EHOSTDL_FINALNAME).hex

cleanlibs:
ifeq ($(MAKE_LIBS), yes)
	@echo rm -fR $(LIBDIR)
	@-if test -f $(HSU_CORE_MIN_FILE); then rm -fR $(LIBDIR); fi
endif

veryclean:	clean cleanlibs
	rm -f ap_depend.dep
	rm -f $(TARGET)/*.dep
 

#-------------------------------------------------------------------------------
# Install target
#-------------------------------------------------------------------------------
install:
	@echo ========================================================
	@echo INSTALLING $(TARGET)
	@cp $(FINALNAME).hex ../../build/ms/bin/$(BUILDID)/.
	@cp $(EHOSTDL_FINALNAME).hex ../../build/ms/bin/$(BUILDID)/.
	@echo
	@echo DONE
	@echo ========================================================


#-------------------------------------------------------------------------------
# Scatterload file target
#-------------------------------------------------------------------------------
ap_armprg.scl:    force.frc ap_in.scl 
	@echo ---------------------------------------------------------------
	@echo "========================================================== "
	@echo Creating Scatter Load File
	@echo "========================================================== "
	@echo "###################################################" > $(APSCATTERFILE) 
	@echo "##  GENERATED FILE - DO NOT EDIT" >> $(APSCATTERFILE) 
	@echo "##                                    " >> $(APSCATTERFILE)
	@echo "## generated:  `date`                 " >> $(APSCATTERFILE)
	@echo "###################################################" >> $(APSCATTERFILE) 
	@$(CC) -E -DCODE_START_ADDR=$(S_ADDR) -DDATA_ADDR=+0x0 < ap_in.scl | \
		perl $(ASM_SCRIPT) - >> $(APSCATTERFILE)
	@echo 
	@echo Done
	@echo ---------------------------------------------------------------

ehostdl_nandprg.scl:    force.frc ap_in.scl 
	@echo ---------------------------------------------------------------
	@echo "========================================================== "
	@echo Creating Scatter Load File for Emergency Host Download
	@echo "========================================================== "
	@echo "###################################################" > $(EHOSTDL_SCATTER_FILE) 
	@echo "##  GENERATED FILE - DO NOT EDIT" >> $(EHOSTDL_SCATTER_FILE) 
	@echo "##                                    " >> $(EHOSTDL_SCATTER_FILE)
	@echo "## generated:  `date`                 " >> $(EHOSTDL_SCATTER_FILE)
	@echo "###################################################" >> $(EHOSTDL_SCATTER_FILE) 
	@$(CC) -E -DCODE_START_ADDR=$(EHOSTDL_CODE_ADDR) -DDATA_ADDR=$(EHOSTDL_DATA_ADDR) < ap_in.scl | \
		perl $(ASM_SCRIPT) - >> $(EHOSTDL_SCATTER_FILE)
	@echo 
	@echo Done
	@echo ---------------------------------------------------------------
 
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
	@echo MAKE_LIBS : $(MAKE_LIBS)
	@echo ------------------------------------------------------------------
	@echo FLASH_OBJS : $(FLASH_OBJS)
	@echo ------------------------------------------------------------------
	@echo OBJECTS : $(OBJECTS)
	@echo ------------------------------------------------------------------
	@echo FLASHINC : $(FLASHINC)
	@echo ------------------------------------------------------------------
	@echo EXTRAINC : $(EXTRAINC)
	@echo ------------------------------------------------------------------
	@echo FLASH_OBJS : $(FLASH_OBJS)
	@echo ------------------------------------------------------------------
	@echo OBJECTS : $(OBJECTS)
	@echo ------------------------------------------------------------------


#===============================================================================
#                               DEPENDENCIES
#===============================================================================

# The dependencies included at the end of this makefile can be automatically
# updated by making the 'depend' target to invoke the following rules.

DEPFILE=ap_depend
DEPFILE_NAME   = $(DEPFILE).dep
DEPFILE_BACKUP = $(DEPFILE).bak
DEPFILE_TMP    = $(DEPFILE).dep.tmp

.SUFFIXES:
.SUFFIXES: .s .o .c .dep .h


%.dep:%.c 
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_CFLAGS) -E < $< | perl $(GETDEP_SCRIPT) \
	$(basename $@).o $< > $*.de_
	@rm -f  $(TARGET)/$(@F).dep
	@mv $*.de_ $*.dep
	@echo -----------------------------------------------------------------

$(TARGET)/%.dep:%.c
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_CFLAGS) -E < $< | perl $(GETDEP_SCRIPT) \
	$(basename $@).o $< >  $(TARGET)/$*.de_
	@rm -f  $(TARGET)/$*.dep
	@mv  $(TARGET)/$*.de_  $(TARGET)/$*.dep
	@echo -----------------------------------------------------------------


$(TARGET)/%.dep:%.s
	@echo -----------------------------------------------------------------
	@echo DEPENDENCY $@
	@$(CC) $(CFLAGS) $(DMSS_AFLAGS) -E < $< | \
	perl $(GETDEP_SCRIPT) $(basename $@).o $< >  $(TARGET)/$*.de_
	@rm -f  $(TARGET)/$*.dep
	@mv  $(TARGET)/$*.de_  $(TARGET)/$*.dep
	@echo -----------------------------------------------------------------

$(OBJECTS:.o=.dep) : $(TARGET)/exist 

depend:  $(OBJECTS:.o=.dep) force.frc
	@echo -----------------------------------------------------------------
	@echo Processing Dependencies
	@perl $(MDEPEND_SCRIPT) $(DEPFILE_NAME)  $(TARGET) > $(DEPFILE_TMP)
	@-rm -f $(DEPFILE_BACKUP)
	@-mv $(DEPFILE_NAME) $(DEPFILE_BACKUP)
	@mv $(DEPFILE_TMP) $(DEPFILE_NAME)
	@echo -----------------------------------------------------------------

ap_depend.dep:
	@echo " "
	@echo "Creating ap_depend.dep"
	@echo " "
	@echo "# ------------------------------" > ap_depend.dep
	@echo "# DO NOT EDIT BELOW THIS LINE" >> ap_depend.dep


force.frc:

sources:

# If dependency file is not present, do not complain. There is a rule to make
# one.
-include ap_depend.dep
