ifdef BREWSDKTOOLSDIR
BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
else
  ifdef BUIWDEVDIR
    BREWRC=../$(BUIWDEVDIR)/brewrc/brewrc
  else
    $(error BREWSDKTOOLSDIR in not defined)
  endif
endif

.SUFFIXES:
.SUFFIXES: .mfx .mif

all: forms.mif

clean:
	rm -f forms.mif

forms.mif: forms.mfx
	$(BREWRC) -nh -o forms.mif $<
