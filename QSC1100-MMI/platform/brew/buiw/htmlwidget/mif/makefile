ifndef BREWRC
  export BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
endif


.SUFFIXES:
.SUFFIXES: .mfx .mif

all: htmlwidget.mif

clean:
	rm -f htmlwidget.mif

htmlwidget.mif htmlwidget_res.h: htmlwidget.mfx
	$(BREWRC) -o htmlwidget.mif -h htmlwidget_res.h $<
	cp -f htmlwidget.mif ../../htmlwidget.mif
	cp -f htmlwidget_res.h ../src/htmlwidget_res.h
   ifdef EMU_DIR
	cp -f htmlwidget.mif ../$(EMU_DIR)/htmlwidget.mif
   endif

