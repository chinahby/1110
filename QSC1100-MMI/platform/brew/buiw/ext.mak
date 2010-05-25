#============================================================================
#  Description:
#    Define make targets for all extension libraries
#    This file is included by 'makefile' in each applet directory.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .mod & .dll
#     mod           - make .mod
#     dll           - make .dll
#     clean         - delete object directory and image files
#     distclean     - delete all files other than distributable sources
#
#   The above targets can be made with the following command:
#
#     make
#     make all
#     make clean
#     make clean all
#     make distclean
#
# Assumptions:
#     1. This file exists in ".." in relation to each applet dir.
#     2. mod_arm.mak also exists in ".." in relation to each applet dir.
#     3. 'makefile' in each applet dir includes this file
#     4. 'makefile' in each applet dir defines PROJECT, DIRDEPTH, 
#          EXTRASOURCES, EXTRAHEADERS
#
#        Copyright © 2000-2007 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------


ifndef BREWPK
	export BREWPK=$(BREWDIR)/../pk
endif
# -- invoked tools and makefiles can use this to determine derivation
# -- this file uses it to control recursive calls
ifdef EXTMAK
PRECMD := @echo Skipping recursive command: 
endif
export EXTMAK := 1

# -- checks on required variables
ifndef PROJECT
$(error PROJECT not defined)
endif
ifndef DIRDEPTH
$(error DIRDEPTH not defined)
endif
ifndef BUITDIR
$(error BUITDIR not defined)
endif
ifndef BREWDIR
$(error BREWDIR not defined)
endif

MSDEV = cmd /c msdev

CFG=$(PROJECT) - Win32 Debug

WIDGETSDIR ?= $(subst \,/,$(BUITDIR)/widgets)
FORMSDIR   ?= $(subst \,/,$(BUITDIR)/forms)

export INCLUDES = -I"./" -I"./src" -I"./inc" -I"./inc/bid" -I"$(DIRDEPTH)/xmod" -I"$(WIDGETSDIR)/src" -I"$(WIDGETSDIR)/inc" -I"$(FORMSDIR)/inc" $(EXTRAINCLUDES) -I"$(BREWDIR)/inc" -I"$(BREWDIR)/../OEM/inc" -I"$(BREWPK)/inc" -I"$(BREWDIR)/inc"
export SOURCES  = $(wildcard src/*.c) $(EXTRASOURCES)
export HEADERS  = $(wildcard inc/*.h) $(EXTRAHEADERS)
export VPATH    += ./src $(WIDGETSDIR)/src $(WIDGETSDIR)/inc

# -- Default TARGET ARM compiler to all targets if not already set
TARGETS ?= ADS12_ARM7 RVCT21_ARM9

# MOD file output locations
MODDIR   := mod
MODFILES := $(patsubst %,$(MODDIR)/%/$(PROJECT).mod,$(TARGETS))

# -- Begin Target Rules

.PHONY: all dll mod distclean clean mif $(PROJECT).dll

all: mif dll mod

dll: mif $(PROJECT).dll

mod: mif $(MODFILES)

mif: ../$(PROJECT).mif

distclean:
	find . \( -name "*~" -o -name "*.lib" -o -name "*.elf" -o -name "*.o" \) -exec rm {} \;
	find . \( -name "*.ilk" -o -name "*.exp" -o -name "*.pdb" -o -name "*.plg" -o -name "*.opt" -o -name "*.ncb" \) -exec rm {} \;
	rm -rf Debug Release

clean:
	@echo -------------------CLEAN---------------------------------------
	rm -f *.o *.elf *.mod *.lib
	rm -f ../$(PROJECT).mif
	if test -d mif; then (cd mif && $(MAKE) clean); fi;
	if test -d res; then (cd res && $(MAKE) clean); fi;
	$(PRECMD) $(MAKE) -f $(DIRDEPTH)/mod_arm.mak clean
	$(MSDEV) $(PROJECT).dsp /MAKE "$(CFG)" /CLEAN

$(PROJECT).bar:
	@echo --------------------BAR----------------------------------------
	(cd res && $(MAKE))

../$(PROJECT).mif:
	@echo --------------------MIF----------------------------------------
ifndef PRECMD
	(cd mif && $(MAKE))
	(cp -u -f mif/$(PROJECT).mif ../$(PROJECT).mif)
else
	$(PRECMD) MIF Compilation
endif

$(PROJECT).dll:
	@echo --------------------DLL----------------------------------------	
	$(MSDEV) $(PROJECT).dsp /MAKE "$(CFG)"


$(MODFILES): $(SOURCES) $(HEADERS)
	@echo --------------------MOD----------------------------------------
ifeq "$(firstword $(subst ut,ut xx,$(PROJECT)))" "ut"
	@echo MOD compilation not configured for $(PROJECT). Skipping make mod
else
	-$(PRECMD) $(MAKE) -f $(DIRDEPTH)/mod_arm.mak $@ TARGET=$(patsubst $(MODDIR)/%,%,$(@D))
endif
