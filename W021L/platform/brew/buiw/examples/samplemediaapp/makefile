# ############################################### #
# GNU Make file for samplemediaapp
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=samplemediaapp
export DIRDEPTH=../..

EXTRAINCLUDES= -I"$(BUITDIR)/framewidget/inc"

# -- lint specific vars
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt
LINT_LIBDIRS= -I"$(WIDGETSDIR)/inc"

### include generic makefile ###
include $(DIRDEPTH)/ext.mak

# Need to tell ext.mak to also compile bar file too (and generate _res.h)
mif: $(PROJECT).bar
