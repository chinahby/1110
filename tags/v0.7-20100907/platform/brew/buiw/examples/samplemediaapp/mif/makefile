ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: samplemediaapp.mif

clean:
	rm -f *.bar *.brh
	rm -f samplemediaapp.mif
	rm -f ../../samplemediaapp.mif

samplemediaapp.mif : samplemediaapp.mfx
	$(BREWRC) -o $(PROJECT).mif $<
	cp -f samplemediaapp.mif ../../samplemediaapp.mif
	#cp -f samplemediaapp_res.h ../inc/samplemediaapp.mif
   ifdef EMU_DIR
	cp -f samplemediaapp.mif ../$(EMU_DIR)/samplemediaapp.mif
   endif

