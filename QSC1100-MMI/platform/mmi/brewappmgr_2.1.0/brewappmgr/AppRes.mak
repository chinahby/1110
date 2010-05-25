#============================================================================
#  Name:
#     AppRes.mak
#
#  Description:
#     Makefile to build .bar and _res.h files for Brew Application Manager 2
#
#     The following nmake targets are available in this makefile:
#
#         all          - builds the _res.h and .bar files 
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
 
!IFNDEF RESTARGET
RESTARGET=brewappmgr
!ENDIF

!IFNDEF MIFDIR
MIFDIR=mif/$(BAM2_BUILD)/$(BAM2_LANG)/$(BAM2_BIT_DEPTH)
!ENDIF

!IFNDEF LNDIR
LNDIR=res/$(BAM2_BUILD)/languageneutral/$(BAM2_BIT_DEPTH)
!ENDIF

!IFNDEF LSDIR
LSDIR=res/$(BAM2_BUILD)/languagespecific/$(BAM2_LANG)/$(BAM2_BIT_DEPTH)
!ENDIF

!IFNDEF COMMONDIR
COMMONDIR=res/$(BAM2_BUILD)
!ENDIF
  
.SUFFIXES:
.SUFFIXES: .brx .bar

all: $(RESTARGET)ln.bar $(RESTARGET)ln_res.h $(RESTARGET)ls.bar $(RESTARGET)ls_res.h $(RESTARGET)common.bar $(RESTARGET)common_res.h

clean:
	rm -f $(RESTARGET)ln.bar $(RESTARGET)ln_res.h  $(RESTARGET)ls.bar $(RESTARGET)ls_res.h $(RESTARGET)common.bar $(RESTARGET)common_res.h

$(RESTARGET)ln.bar $(RESTARGET)ln_res.h: $(LNDIR)/$(RESTARGET)ln.brx 
	$(BREWRC) -o $(RESTARGET)ln.bar -h $(RESTARGET)ln_res.h -i $(LNDIR) $(LNDIR)/$(RESTARGET)ln.brx

$(RESTARGET)ls.bar $(RESTARGET)ls_res.h: $(LSDIR)/$(RESTARGET)ls.brx 
	$(BREWRC) -o $(RESTARGET)ls.bar -h $(RESTARGET)ls_res.h -i $(LSDIR) $(LSDIR)/$(RESTARGET)ls.brx
	
$(RESTARGET)common.bar $(RESTARGET)common_res.h: $(COMMONDIR)/$(RESTARGET)common.brx 
	$(BREWRC) -o $(RESTARGET)common.bar -h $(RESTARGET)common_res.h -i $(COMMONDIR) $(COMMONDIR)/$(RESTARGET)common.brx
	
!IFNDEF BREWSDKTOOLSDIR
!MESSAGE ERROR: You must have SDK Tools installed to compile resource files
!ENDIF
