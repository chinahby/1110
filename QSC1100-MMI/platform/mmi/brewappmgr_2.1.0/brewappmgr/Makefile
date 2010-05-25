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

!IFNDEF BAM2_BUILD
BAM2_BUILD=standard
!ENDIF

MAKEEXPORT = BAM2_LANG:BAM2_BIT_DEPTH:BAM2_BUILD:TARGET

all: .mif resources module

clean:
	@echo -----------------------------CLEAN MIF---------------------------------
	rm -f ../$(TARGET).mif
	nmake -f AppMif.mak clean
    @echo -----------------------------CLEAN RES--------------------------------
	nmake -f AppRes.mak clean
    @echo -----------------------------CLEAN MODULE-----------------------------
	nmake -f BrewAppMgr.mak clean


.mif:
	@echo -----------------------------MIF---------------------------------------
	nmake -f AppMif.mak all
	cp -u -f $(TARGET).mif ../
	nmake -f AppMif.mak clean		

resources:
	@echo -----------------------------RES---------------------------------------
	nmake -f AppRes.mak all

module: resources 
	@echo -----------------------------MODULE------------------------------------
	nmake -f BrewAppMgr.mak all


