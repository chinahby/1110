# ############################################### #
# GNU Make file for samplewidgetsapp
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=samplewidgetsapp
export DIRDEPTH=../..
export VPATH= ./src ../samplewidgets/inc $(WIDGETSDIR)/inc $(WIDGETSDIR)/src $(WIDGETSDIR)/inc

EXTRAINCLUDES= -I"../samplewidgets/inc"

# -- lint specific vars
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt
LINT_LIBDIRS= -I"$(WIDGETSDIR)/inc"

### include generic makefile ###
include $(DIRDEPTH)/ext.mak
