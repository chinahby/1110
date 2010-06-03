ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplecameraapp.mif

clean:
	rm -f *.bar *.brh
	rm -f samplecameraapp.mif
	rm -f ../../samplecameraapp.mif

samplecameraapp.mif : samplecameraapp.mfx
	$(BREWRC) -nh -o samplecameraapp.mif -h $<
	cp -f samplecameraapp.mif ../../samplecameraapp.mif
   ifdef EMU_DIR
	cp -f samplecameraapp.mif ../$(EMU_DIR)/samplecameraapp.mif
   endif

