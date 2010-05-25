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

all: locales.mif

clean:
	rm -f locales.mif

locales.mif widget_res.h: locales.mfx
	$(BREWRC) -o locales.mif -h $<
	cp -f locales.mif ../../locales.mif
   ifdef EMU_DIR
	cp -f locales.mif ../$(EMU_DIR)/locales.mif
   endif

