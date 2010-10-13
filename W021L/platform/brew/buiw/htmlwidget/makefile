# ############################################### #
# GNU Make file for htmlwidget
#
# Mandatory Vars to be exported from here
#	PROJECT,DIRDEPTH
# ############################################### #

# -- the target
export PROJECT=htmlwidget
export DIRDEPTH = ..

WIDGETSDIR=../widgets
EXTRASOURCES=AEEBase.c Border.c ModelBase.c WidgetBase.c modres.c FontBidiUtil.c ImageBase.c
EXTRAHEADERS=AEEBase.h Border.h ModelBase.h WidgetBase.h wutil.h modres.h

# -- additional lint suppression files
LINT_SUPPRESSION_FILES=$(WIDGETSDIR)/lint/_project.lnt

### include generic makefile ###
include $(DIRDEPTH)/ext.mak
