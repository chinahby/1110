# ############################################### #
# GNU Make file for samplewidgets
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=samplewidgets
export DIRDEPTH=../..

EXTRASOURCES=$(WIDGETSDIR)/src/AEEBase.c
EXTRAHEADERS=$(WIDGETSDIR)/inc/AEEBase.h

# -- lint specific vars
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt
LINT_LIBDIRS= -I"$(WIDGETSDIR)/inc"

### include generic makefile ###
include $(DIRDEPTH)/ext.mak
