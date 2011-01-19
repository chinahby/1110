ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .brx .bar

all: samplecameraapp.bar

clean:
	rm -f *.bar *.brh
	rm -f samplecameraapp.bar
	rm -f ../samplecameraapp.bar
	rm -f samplecameraapp_res.h
	rm -f ../inc/samplecameraapp_res.h

samplecameraapp.bar : samplecameraapp.brx
	$(BREWRC) -o samplecameraapp.bar -h samplecameraapp_res.h $<
	cp -f samplecameraapp.bar ../samplecameraapp.bar
	cp -f samplecameraapp_res.h ../inc/samplecameraapp_res.h
   ifdef EMU_DIR
	cp -f samplecameraapp.bar ../$(EMU_DIR)/samplecameraapp.bar
   endif