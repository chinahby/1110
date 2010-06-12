#============================================================================
#  Name:
#     AppMif.mak
#
#  Description:
#     Makefile to build .mif file for Brew Application Manager 2
#
#     The following nmake targets are available in this makefile:
#
#         all          - builds the brewappmgr.mif
#         clean        - deletes files created 
# 
#     BAM2_LANG can be specified to determine which language directory to build resources from
#	     examples:  en, fr, ja, ...
#        default:  en
#     BAM2_BIT_DEPTH can be specified to determine which display mode directory to build resources from
#	     examples:  256, 4grey, mono, ... 
#        default: 256
#     BAM2_BUILD can be specified to determine which Build to use.  This should match the Build chosen in AppMgrSettings.h
#	     examples:  standard, custom1.
#        default:  standard
#     NOTE:  If this file is being called from Makefile the above variables are already defined to their defaults,
#            so changing them here will not do anything.  They should be changed in Makefile or set in the environment.
#============================================================================

!IFNDEF BAM2_BUILD
BAM2_BUILD=standard
!ENDIF

!IFNDEF BAM2_LANG
BAM2_LANG=en
!ENDIF

!IFNDEF BAM2_BIT_DEPTH
BAM2_BIT_DEPTH=256
!ENDIF

!IFNDEF BREWRC
BREWRC="$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc"
!ENDIF
 
!IFNDEF TARGET
TARGET=brewappmgr
!ENDIF

!IFNDEF MIFDIR
MIFDIR=mif/$(BAM2_BUILD)/$(BAM2_LANG)/$(BAM2_BIT_DEPTH)
!ENDIF

#special trap, as not all resource language directories exist for mifs in bam 2:
!IF !EXIST (MIFDIR)
MIFDIR=mif/$(BAM2_BUILD)/en/$(BAM2_BIT_DEPTH)
!ENDIF

.SUFFIXES:
.SUFFIXES: .mfx .mif

all: $(TARGET).mif

clean:
	rm -f $(TARGET).mif
	rm -f $(TARGET).brh
	
$(TARGET).mif: $(MIFDIR)/$(TARGET).mfx
	$(BREWRC) -o $@ -i $(MIFDIR)/ $(MIFDIR)/$(TARGET).mfx
	
!IFNDEF BREWSDKTOOLSDIR
!MESSAGE ERROR: You must have SDK Tools installed to compile resource files
!ENDIF
