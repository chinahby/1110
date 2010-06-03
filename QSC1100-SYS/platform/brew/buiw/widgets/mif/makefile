ifdef BREWSDKTOOLSDIR
BREWRC="$(BREWSDKTOOLSDIR)"/ResourceEditor/brewrc
else
  ifdef BUIWDEVDIR
    BREWRC=../$(BUIWDEVDIR)/brewrc/brewrc
  else
    $(error BREWSDKTOOLSDIR in not defined)
  endif
endif

.SUFFIXES:
.SUFFIXES: .mfx .mif

all: widgets.mif

clean:
	rm -f widgets.mif

widgets.mif widget_res.h: widgets.mfx
	$(BREWRC) -o widgets.mif -h widget_res.h $<
	cp -f widgets.mif ../../widgets.mif
	cp -f widget_res.h ../src/widget_res.h
   ifdef EMU_DIR
	cp -f widgets.mif ../$(EMU_DIR)/widgets.mif
   endif

