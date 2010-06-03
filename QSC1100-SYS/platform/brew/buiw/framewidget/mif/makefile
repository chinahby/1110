.SUFFIXES:
.SUFFIXES: .mfx .mif

all: framewidget.mif

clean:
	rm -f framewidget.mif

framewidget.mif:
	"$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc" -o framewidget.mif framewidget.mfx
	cp -f framewidget.mif ../../framewidget.mif
   	ifdef EMU_DIR
		cp -f framewidget.mif ../$(EMU_DIR)/framewidget.mif
   	endif

