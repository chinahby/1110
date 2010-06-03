ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: helloformapp.mif

clean:
	rm -f *.bar *.brh
	rm -f helloformapp.mif
	rm -f ../../helloformapp.mif

helloformapp.mif : helloformapp.mfx
	$(BREWRC) -nh -o helloformapp.mif -h $<
	cp -f helloformapp.mif ../../helloformapp.mif
   ifdef EMU_DIR
	cp -f helloformapp.mif ../$(EMU_DIR)/helloformapp.mif
   endif

