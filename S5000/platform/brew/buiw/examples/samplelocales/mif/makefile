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

all: samplelocales.mif

clean:
	rm -f samplelocales.mif

samplelocales.mif widget_res.h: samplelocales.mfx
	$(BREWRC) -nh -o samplelocales.mif -h $<
	cp -f samplelocales.mif ../../samplelocales.mif
   ifdef EMU_DIR
	cp -f samplelocales.mif ../$(EMU_DIR)/samplelocales.mif
   endif

