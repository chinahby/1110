# ############################################### #
# GNU Make file for locales
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=locales
export DIRDEPTH = ..

WIDGETSDIR=../widgets
EXTRAINCLUDES= -I"./inc/bid"

# -- additional lint suppression files
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt

### include generic makefile ###
include $(DIRDEPTH)/ext.mak
