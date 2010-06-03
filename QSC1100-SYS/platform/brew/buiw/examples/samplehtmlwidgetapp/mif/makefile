ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplehtmlwidgetapp.mif

clean:
	rm -f *.bar *.brh
	rm -f samplehtmlwidgetapp.mif
	rm -f ../../samplehtmlwidgetapp.mif

samplehtmlwidgetapp.mif : samplehtmlwidgetapp.mfx
	$(BREWRC) -nh -o samplehtmlwidgetapp.mif -h $<
	cp -f samplehtmlwidgetapp.mif ../../samplehtmlwidgetapp.mif
   ifdef EMU_DIR
	cp -f samplehtmlwidgetapp.mif ../$(EMU_DIR)/samplehtmlwidgetapp.mif
   endif

