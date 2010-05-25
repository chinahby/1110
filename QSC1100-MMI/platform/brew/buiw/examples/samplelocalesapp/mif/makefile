ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplelocalesapp.mif

clean:
	rm -f samplelocalesapp.mif
	rm -f ../../samplelocalesapp.mif

samplelocalesapp.mif widget_res.h: samplelocalesapp.mfx
	$(BREWRC) -nh -o samplelocalesapp.mif -h $<
	cp -f samplelocalesapp.mif ../../samplelocalesapp.mif
   ifdef EMU_DIR
	cp -f samplelocalesapp.mif ../$(EMU_DIR)/samplelocalesapp.mif
   endif

