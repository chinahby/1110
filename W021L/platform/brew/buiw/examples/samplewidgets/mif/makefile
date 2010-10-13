ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplewidgets.mif

clean:
	rm -f samplewidgets.mif
	rm -f ../../samplewidgets.mif

samplewidgets.mif widget_res.h: samplewidgets.mfx
	$(BREWRC) -nh -o samplewidgets.mif -h $<
	cp -f samplewidgets.mif ../../samplewidgets.mif
   ifdef EMU_DIR
	cp -f samplewidgets.mif ../$(EMU_DIR)/samplewidgets.mif
   endif

