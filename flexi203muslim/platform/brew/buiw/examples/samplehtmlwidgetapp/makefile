# ############################################### #
# GNU Make file for samplehtmlwidgetapp
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=samplehtmlwidgetapp
export DIRDEPTH=../..

EXTRAINCLUDES= -I"$(BUITDIR)/htmlwidget/inc"

# -- lint specific vars
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt
LINT_LIBDIRS= -I"$(WIDGETSDIR)/inc"

### include generic makefile ###
include $(DIRDEPTH)/ext.mak
