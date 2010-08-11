ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplewidgetsapp.mif

clean:
	rm -f samplewidgetsapp.mif
	rm -f ../../samplewidgetsapp.mif

samplewidgetsapp.mif widget_res.h: samplewidgetsapp.mfx
	$(BREWRC) -nh -o samplewidgetsapp.mif -h $<
	cp -f samplewidgetsapp.mif ../../samplewidgetsapp.mif
   ifdef EMU_DIR
	cp -f samplewidgetsapp.mif ../$(EMU_DIR)/samplewidgetsapp.mif
   endif

