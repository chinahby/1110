#============================================================================
#  Name:
#     Makefile
#
#  Description:
#     Makefile to build components that make up Brew Application Manager 2
#
#     The following nmake targets are available in this makefile:
#
#         module       - builds .elf and .mod files using AppMgr.mak
#         resources    - builds _res.h and .bar files using AppRes.mak
#         .mif         - builds the brewappmgr.mif and moves it to be parallel
#                        to the brewappmgr directory.
#         all          - does all of the above
#         clean        - deletes any files created 
# 
#     BAM2_LANG can be specified to determine which language directory to
#     build resources from
#	     examples:  en, fr, ja, ...
#        default:  en
#     BAM2_BIT_DEPTH can be specified to determine which display mode
#     directory to build resources from
#	     examples:  256, 4grey, mono, ... 
#        default: 256
#     BAM2_BUILD can be specified to determine which Build to use.  This should
#     match the Build chosen in AppMgrSettings.h
#	     examples:  standard, custom1.
#        default:  standard
#============================================================================
 
!IFNDEF BREWRC
BREWRC="$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc"
!ENDIF

!IFNDEF TARGET
TARGET=aeemenuctlex
!ENDIF



MAKEEXPORT = TARGET

all: .mif module

clean:
	@echo -----------------------------CLEAN MIF---------------------------------
	rm -f ../$(TARGET).mif
	nmake -f AeeMenuCtlMif.mak clean
	@echo -----------------------------CLEAN MODULE-----------------------------
	nmake -f aeemenuctlex.mak clean


.mif:
	@echo -----------------------------MIF---------------------------------------
	nmake -f AeeMenuCtlMif.mak all
	cp -u -f $(TARGET).mif ../
	nmake -f AeeMenuCtlMif.mak clean		

module:
	@echo -----------------------------MODULE------------------------------------
	nmake -f aeemenuctlex.mak all


