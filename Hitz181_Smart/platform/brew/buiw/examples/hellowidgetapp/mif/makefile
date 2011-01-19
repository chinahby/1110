ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: hellowidgetapp.mif

clean:
	rm -f *.bar *.brh
	rm -f hellowidgetapp.mif
	rm -f ../../hellowidgetapp.mif

hellowidgetapp.mif : hellowidgetapp.mfx
	$(BREWRC) -nh -o hellowidgetapp.mif -h $<
	cp -f hellowidgetapp.mif ../../hellowidgetapp.mif
   ifdef EMU_DIR
	cp -f hellowidgetapp.mif ../$(EMU_DIR)/hellowidgetapp.mif
   endif

