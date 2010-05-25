#============================================================================
#  Name:
#     AeeMenuCtlMif.mak
#
#  Description:
#     Makefile to build .mif file for AeeMenuCtl
#
#     The following nmake targets are available in this makefile:
#
#         all          - builds the brewappmgr.mif
#         clean        - deletes files created 
# 
#     NOTE:  If this file is being called from Makefile the above variables are already defined to their defaults,
#            so changing them here will not do anything.  They should be changed in Makefile or set in the environment.
#============================================================================

!IFNDEF BREWRC
BREWRC="$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc"
!ENDIF
 
!IFNDEF TARGET
TARGET=aeemenuctlex
!ENDIF


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: $(TARGET).mif

clean:
	rm -f $(TARGET).mif
	
$(TARGET).mif: $(TARGET).mfx
	$(BREWRC) -o $@ -i $(TARGET).mfx
	
!IFNDEF BREWSDKTOOLSDIR
!MESSAGE ERROR: You must have SDK Tools installed to compile resource files
!ENDIF
