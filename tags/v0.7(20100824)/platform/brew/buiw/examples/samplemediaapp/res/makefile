ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .brx .bar

all: samplemediaapp.bar

clean:
	rm -f *.bar *.brh
	rm -f samplemediaapp.bar
	rm -f ../samplemediaapp.bar
	rm -f samplemediaapp_res.h
	rm -f ../inc/samplemediaapp_res.h

samplemediaapp.bar : samplemediaapp.brx
	$(BREWRC) -o samplemediaapp.bar -h samplemediaapp_res.h $<
	cp -f samplemediaapp.bar ../samplemediaapp.bar
	cp -f samplemediaapp_res.h ../inc/samplemediaapp_res.h
   ifdef EMU_DIR
	cp -f samplemediaapp.bar ../$(EMU_DIR)/samplemediaapp.bar
   endif