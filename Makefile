## Pd library template version 1.0.14
# For instructions on how to use this template, see:
#  http://puredata.info/docs/developer/Makefilelylib
LIBRARY_NAME = fd_lib

# add your .c source files, one object per file, to the SOURCES
# variable, help files will be included automatically, and for GUI
# objects, the matching .tcl file too
SOURCES = lorenz.c


# list all pd objects (i.e. myobject.pd) files here, and their helpfiles will
# be included automatically
PDOBJECTS = fd_2pass.pd, fd_3pass.pd, fd_3stagelin.pd, fd_5ms.pd, fd_5pass.pd, fd_GEMbuffsize.pd, fd_adsr.pd, fd_ain.pd, fd_allinputs365.pd, fd_alloutputs365.pd, fd_appendit.pd, fd_arcsin.pd, fd_arraysort1.pd, fd_arraysort2.pd, fd_auload.pd, fd_autopatch.pd, fd_autopoly-help.pd, fd_autopoly.pd, fd_bark-help.pd, fd_bark.pd, fd_bark~.pd, fd_bigmyvu.pd, fd_blackman.pd, fd_camara.pd, fd_canvasplot.pd, fd_cauchy.pd, fd_ceil.pd, fd_choose-windows.pd, fd_chord.pd, fd_chroma.pd, fd_circle-help.pd, fd_circle.pd, fd_continunous-distribution.pd, fd_cosline~.pd, fd_countdown.pd, fd_critical.pd, fd_cuacua.pd, fd_dacout.pd, fd_dft.pd, fd_diads.pd, fd_distributions-help.pd, fd_distributions.pd, fd_draw.pd, fd_envtables.pd, fd_etareti.pd, fd_exp.pd, fd_expo.pd, fd_factor-help.pd, fd_factor.pd, fd_falling-object-distnace.pd, fd_fftbark~-graph-help.pd, fd_fftbark~-graph.pd, fd_fftbark~-help.pd, fd_fftbark~.pd, fd_floor.pd, fd_fmosc.pd, fd_g-reader.pd, fd_gate.pd, fd_gate2.pd, fd_gauss-t.pd, fd_gauss.pd, fd_gaussian-distribution.pd, fd_gemcontext.pd, fd_gemset.pd, fd_gemsphere.pd, fd_geometric-mean.pd, fd_grain.pd, fd_hann.pd, fd_hann0.pd, fd_hompsec-help.pd, fd_hopmsec.pd, fd_impulse-help.pd, fd_impulse.pd, fd_input.pd, fd_interp~-help.pd, fd_interp~.pd, fd_iterate.pd, fd_key.pd, fd_kinetica.pd, fd_listcl.pd, fd_logistic-help.pd, fd_logistic.pd, fd_logo-help.pd, fd_logo.pd, fd_lorenz.pd, fd_lorsig.pd, fd_lspace.pd, fd_m-comb.pd, fd_markfour-help.pd, fd_markfour.pd, fd_markov-5-help-2.pd, fd_markov-5-help.pd, fd_markov-5.pd, fd_markov-test.pd, fd_markov.pd, fd_matrix-comb-help.pd, fd_matrix-comb.pd, fd_maudio-assign.pd, fd_maudio-receive.pd, fd_maudio.pd, fd_metro-help.pd, fd_metro.pd, fd_metronomo.pd, fd_midinote.pd, fd_miller-gauss.pd, fd_milliseconds-tempo.pd, fd_minimax.pd, fd_mult.pd, fd_n!.pd, fd_n=n-1.pd, fd_netreceive.pd, fd_netsend.pd, fd_next.pd, fd_nonpulmonic.pd, fd_norep.pd, fd_norm-abs.pd, fd_norm.pd, fd_nouns.pd, fd_nountxt.pd, fd_numkey.pd, fd_octaver.pd, fd_osc.pd, fd_oscil.pd, fd_out.pd, fd_oxy-ctlin.pd, fd_oxy-fad.pd, fd_oxy-toggle.pd, fd_oxy.pd, fd_oxygen-49.pd, fd_pan.pd, fd_parsenum.pd, fd_phasor16~.pd, fd_phatty.pd, fd_phrase.pd, fd_plott.pd, fd_plotter.pd, fd_pno-12.pd, fd_pno-24.pd, fd_pno.pd, fd_poisson-help.pd, fd_poisson.pd, fd_polea.pd, fd_preset.pd, fd_pshift.pd, fd_random-bar-distribution.pd, fd_rec.pd, fd_rec1.pd, fd_reflect.pd, fd_rest.pd, fd_rexpo.pd, fd_rgb.pd, fd_round.pd, fd_rsf-out.pd, fd_rspline-help.pd, fd_rspline.pd, fd_s-grain-envelope.pd, fd_s-prob-5.pd, fd_s-prob.pd, fd_scales-help.pd, fd_scales.pd, fd_sgrain.pd, fd_shader.pd, fd_shift.pd, fd_shuffle.pd, fd_siginfo.pd, fd_sinc.pd, fd_sinh-help.pd, fd_sinh.pd, fd_sinline.pd, fd_sinline~.pd, fd_sp.pd, fd_spat.pd, fd_spigot3.pd, fd_spline~.pd, fd_splittone.pd, fd_squareroo-testing.pd, fd_step.pd, fd_stepper.pd, fd_stopwatch.pd, fd_stp.pd, fd_stream-onoff.pd, fd_sum.pd, fd_switch.pd, fd_sync.pd, fd_t-read.pd, fd_tab.pd, fd_tabout.pd, fd_test-12ch.pd, fd_test-buffer-limit.pd, fd_test-maudio-video.pd, fd_test-plott.pd, fd_test-plott2.pd, fd_test-plott3.pd, fd_test-plott4.pd, fd_test-tone.pd, fd_test8.pd, fd_text-read.pd, fd_textsort2.pd, fd_tglvis.pd, fd_toroman.pd, fd_tovowel.pd, fd_tracktopol.pd, fd_trb-ranges.pd, fd_tri.pd, fd_triangle.pd, fd_untext.pd, fd_updown.pd, fd_vibrato.pd, fd_vu.pd, fd_weightor-control-help.pd, fd_weightor-test.pd, fd_weinlevy2-test-audio.pd, fd_weinlevy2-test.pd, fd_wetdry.pd, fd_wetdry~.pd, fd_wielevy-help.pd, fd_wielevy-test.pd, fd_wielevy.pd, fd_wielevy2.pd, fd_windows.pd, fd_write1.pd, fd_write2.pd, fd_write5.pd, fd_xyz_plot-test.pd, fd_xyz_plot.pd, fd_xyz_plot2.pd, fd_z12-help.pd, fd_z12.pd, fd_zero-crossing~-help.pd, fd_zero-crossing~-help2.pd, fd_zero-crossing~.pd

# example patches and related files, in the 'examples' subfolder
EXAMPLES =  

# manuals and related files, in the 'manual' subfolder
MANUAL = 

# if you want to include any other files in the source and binary tarballs,
# list them here.  This can be anything from header files, test patches,
# documentation, etc.  README.txt and LICENSE.txt are required and therefore
# automatically included
EXTRA_DIST = 

# unit tests and related files here, in the 'unittests' subfolder
UNITTESTS = 



#------------------------------------------------------------------------------#
#
# things you might need to edit if you are using other C libraries
#
#------------------------------------------------------------------------------#

ALL_CFLAGS = -I
ALL_LDFLAGS =  
SHARED_LDFLAGS =
ALL_LIBS = 


#------------------------------------------------------------------------------#
#
# you shouldn't need to edit anything below here, if we did it right :)
#
#------------------------------------------------------------------------------#

# these can be set from outside without (usually) breaking the build
CFLAGS = -Wall -W -g
LDFLAGS =
LIBS =

# get library version from meta file
LIBRARY_VERSION = $(shell sed -n 's|^\#X text [0-9][0-9]* [0-9][0-9]* VERSION \(.*\);|\1|p' $(LIBRARY_NAME)-meta.pd)

ALL_CFLAGS += -DPD -DVERSION='"$(LIBRARY_VERSION)"'

PD_INCLUDE = $(PD_PATH)/include/pd
# where to install the library, overridden below depending on platform
prefix = /usr/local
libdir = $(prefix)/lib
pkglibdir = $(libdir)/pd-externals
objectsdir = $(pkglibdir)

INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -p -m 644
INSTALL_DATA = $(INSTALL) -p -m 644
INSTALL_DIR     = $(INSTALL) -p -m 755 -d

ALLSOURCES := $(SOURCES) $(SOURCES_android) $(SOURCES_cygwin) $(SOURCES_macosx) \
	         $(SOURCES_iphoneos) $(SOURCES_linux) $(SOURCES_windows)

DISTDIR=$(LIBRARY_NAME)-$(LIBRARY_VERSION)
ORIGDIR=pd-$(LIBRARY_NAME:~=)_$(LIBRARY_VERSION)

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
  CPU := $(shell uname -p)
  ifeq ($(CPU),arm) # iPhone/iPod Touch
    SOURCES += $(SOURCES_iphoneos)
    EXTENSION = pd_darwin
    SHARED_EXTENSION = dylib
    OS = iphoneos
    PD_PATH = /Applications/Pd-extended.app/Contents/Resources
    IPHONE_BASE=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin
    CC=$(IPHONE_BASE)/gcc
    CPP=$(IPHONE_BASE)/cpp
    CXX=$(IPHONE_BASE)/g++
    ISYSROOT = -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.0.sdk
    IPHONE_CFLAGS = -miphoneos-version-min=3.0 $(ISYSROOT) -arch armv6
    OPT_CFLAGS = -fast -funroll-loops -fomit-frame-pointer
    ALL_CFLAGS := $(IPHONE_CFLAGS) $(ALL_CFLAGS)
    ALL_LDFLAGS += -arch armv6 -bundle -undefined dynamic_lookup $(ISYSROOT)
    SHARED_LDFLAGS += -arch armv6 -dynamiclib -undefined dynamic_lookup $(ISYSROOT)
    ALL_LIBS += -lc $(LIBS_iphoneos)
    STRIP = strip -x
    DISTBINDIR=$(DISTDIR)-$(OS)
  else # Mac OS X
    SOURCES += $(SOURCES_macosx)
    EXTENSION = pd_darwin
    SHARED_EXTENSION = dylib
    OS = macosx
    PD_PATH = /Applications/Pd-extended.app/Contents/Resources
    OPT_CFLAGS = -ftree-vectorize -fast
# build universal 32-bit on 10.4 and 32/64 on newer
    ifeq ($(shell uname -r | sed 's|\([0-9][0-9]*\)\.[0-9][0-9]*\.[0-9][0-9]*|\1|'), 8)
      FAT_FLAGS = -arch ppc -arch i386 -mmacosx-version-min=10.4
    else
      SOURCES += $(SOURCES_iphoneos)
# Starting with Xcode 4.0, the PowerPC compiler is not installed by default
      ifeq ($(wildcard /usr/llvm-gcc-4.2/libexec/gcc/powerpc*), )
        FAT_FLAGS = -arch i386 -arch x86_64 -mmacosx-version-min=10.5
      else
        FAT_FLAGS = -arch ppc -arch i386 -arch x86_64 -mmacosx-version-min=10.4
      endif
    endif
    ALL_CFLAGS += $(FAT_FLAGS) -fPIC -I/sw/include
    # if the 'pd' binary exists, check the linking against it to aid with stripping
    BUNDLE_LOADER = $(shell test ! -e $(PD_PATH)/bin/pd || echo -bundle_loader $(PD_PATH)/bin/pd)
    ALL_LDFLAGS += $(FAT_FLAGS) -headerpad_max_install_names -bundle $(BUNDLE_LOADER) \
	-undefined dynamic_lookup -L/sw/lib
    SHARED_LDFLAGS += $(FAT_FLAGS) -dynamiclib -undefined dynamic_lookup \
	-install_name @loader_path/$(SHARED_LIB) -compatibility_version 1 -current_version 1.0
    ALL_LIBS += -lc $(LIBS_macosx)
    STRIP = strip -x
    DISTBINDIR=$(DISTDIR)-$(OS)
# install into ~/Library/Pd on Mac OS X since /usr/local isn't used much
    pkglibdir=$(HOME)/Library/Pd
  endif
endif
# Tho Android uses Linux, we use this fake uname to provide an easy way to
# setup all this things needed to cross-compile for Android using the NDK
ifeq ($(UNAME),ANDROID)
  CPU := arm
  SOURCES += $(SOURCES_android)
  EXTENSION = so
  SHARED_EXTENSION = so
  OS = android
  PD_PATH = /usr
  NDK_BASE := /usr/local/android-ndk
  NDK_PLATFORM_LEVEL ?= 5
  NDK_ABI=arm
  NDK_SYSROOT=$(NDK_BASE)/platforms/android-$(NDK_PLATFORM_LEVEL)/arch-$(NDK_ABI)
  NDK_UNAME := $(shell uname -s | tr '[A-Z]' '[a-z]')
  NDK_COMPILER_VERSION=4.6
  NDK_TOOLCHAIN=$(wildcard \
	$(NDK_BASE)/toolchains/$(NDK_ABI)*-$(NDK_COMPILER_VERSION)/prebuilt/$(NDK_UNAME)-x86)
  CC := $(wildcard $(NDK_TOOLCHAIN)/bin/*-linux-android*-gcc) --sysroot=$(NDK_SYSROOT)
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  CFLAGS += 
  LDFLAGS += -rdynamic -shared
  SHARED_LDFLAGS += -Wl,-soname,$(SHARED_LIB) -shared
  LIBS += -lc $(LIBS_android)
  STRIP := $(wildcard $(NDK_TOOLCHAIN)/bin/$(NDK_ABI)-linux-android*-strip) \
	--strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),Linux)
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_linux)
  EXTENSION = pd_linux
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -Wl,-soname,$(SHARED_LIB) -shared
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),GNU)
  # GNU/Hurd, should work like GNU/Linux for basically all externals
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_linux)
  EXTENSION = pd_linux
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),GNU/kFreeBSD)
  # Debian GNU/kFreeBSD, should work like GNU/Linux for basically all externals
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_linux)
  EXTENSION = pd_linux
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq (CYGWIN,$(findstring CYGWIN,$(UNAME)))
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_cygwin)
  EXTENSION = dll
  SHARED_EXTENSION = dll
  OS = cygwin
  PD_PATH = $(shell cygpath $$PROGRAMFILES)/pd
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += 
  ALL_LDFLAGS += -rdynamic -shared -L"$(PD_PATH)/src" -L"$(PD_PATH)/bin"
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc -lpd $(LIBS_cygwin)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)
endif
ifeq (MINGW,$(findstring MINGW,$(UNAME)))
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_windows)
  EXTENSION = dll
  SHARED_EXTENSION = dll
  OS = windows
  PD_PATH = $(shell cd "$$PROGRAMFILES/pd" && pwd)
  # MinGW doesn't seem to include cc so force gcc
  CC=gcc
  OPT_CFLAGS = -O3 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -mms-bitfields
  ALL_LDFLAGS += -s -shared -Wl,--enable-auto-import
  SHARED_LDFLAGS += -shared
  ALL_LIBS += -L"$(PD_PATH)/src" -L"$(PD_PATH)/bin" -L"$(PD_PATH)/obj" \
	-lpd -lwsock32 -lkernel32 -luser32 -lgdi32 -liberty $(LIBS_windows)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)
endif

# in case somebody manually set the HELPPATCHES above
HELPPATCHES ?= $(SOURCES:.c=-help.pd) $(PDOBJECTS:.pd=-help.pd)

ALL_CFLAGS := $(ALL_CFLAGS) $(CFLAGS) $(OPT_CFLAGS)
ALL_LDFLAGS := $(LDFLAGS) $(ALL_LDFLAGS)
ALL_LIBS := $(LIBS) $(ALL_LIBS)

SHARED_SOURCE ?= $(wildcard lib$(LIBRARY_NAME).c)
SHARED_HEADER ?= $(shell test ! -e $(LIBRARY_NAME).h || echo $(LIBRARY_NAME).h)
SHARED_LIB ?= $(SHARED_SOURCE:.c=.$(SHARED_EXTENSION))
SHARED_TCL_LIB = $(wildcard lib$(LIBRARY_NAME).tcl)

.PHONY = install libdir_install single_install install-doc install-examples install-manual install-unittests clean distclean dist etags $(LIBRARY_NAME)

all: $(SOURCES:.c=.$(EXTENSION)) $(SHARED_LIB)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -o "$*.o" -c "$*.c"

%.$(EXTENSION): %.o $(SHARED_LIB)
	$(CC) $(ALL_LDFLAGS) -o "$*.$(EXTENSION)" "$*.o"  $(ALL_LIBS) $(SHARED_LIB)
	chmod a-x "$*.$(EXTENSION)"

# this links everything into a single binary file
$(LIBRARY_NAME): $(SOURCES:.c=.o) $(LIBRARY_NAME).o lib$(LIBRARY_NAME).o
	$(CC) $(ALL_LDFLAGS) -o $(LIBRARY_NAME).$(EXTENSION) $(SOURCES:.c=.o) \
		$(LIBRARY_NAME).o lib$(LIBRARY_NAME).o $(ALL_LIBS)
	chmod a-x $(LIBRARY_NAME).$(EXTENSION)

$(SHARED_LIB): $(SHARED_SOURCE:.c=.o)
	$(CC) $(SHARED_LDFLAGS) -o $(SHARED_LIB) $(SHARED_SOURCE:.c=.o) $(ALL_LIBS)

install: libdir_install

# The meta and help files are explicitly installed to make sure they are
# actually there.  Those files are not optional, then need to be there.
libdir_install: $(SOURCES:.c=.$(EXTENSION)) $(SHARED_LIB) install-doc install-examples install-manual install-unittests
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd \
		$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SOURCES))" || (\
		$(INSTALL_PROGRAM) $(SOURCES:.c=.$(EXTENSION)) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME) && \
		$(STRIP) $(addprefix $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/,$(SOURCES:.c=.$(EXTENSION))))
	test -z "$(strip $(SHARED_LIB))" || \
		$(INSTALL_DATA) $(SHARED_LIB) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(wildcard $(SOURCES:.c=.tcl)))" || \
		$(INSTALL_DATA) $(wildcard $(SOURCES:.c=.tcl)) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(PDOBJECTS) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SHARED_TCL_LIB))" || \
		$(INSTALL_DATA) $(SHARED_TCL_LIB) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)

# install library linked as single binary
single_install: $(LIBRARY_NAME) install-doc install-examples install-manual install-unittests
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_PROGRAM) $(LIBRARY_NAME).$(EXTENSION) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(STRIP) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/$(LIBRARY_NAME).$(EXTENSION)

install-doc:
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SOURCES) $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(HELPPATCHES) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_DATA) README.txt $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/README.txt
	$(INSTALL_DATA) LICENSE.txt $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/LICENSE.txt

install-examples:
	test -z "$(strip $(EXAMPLES))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/examples && \
		for file in $(EXAMPLES); do \
			$(INSTALL_DATA) examples/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/examples; \
		done

install-manual:
	test -z "$(strip $(MANUAL))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/manual && \
		for file in $(MANUAL); do \
			$(INSTALL_DATA) manual/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/manual; \
		done

install-unittests:
	test -z "$(strip $(UNITTESTS))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/unittests && \
		for file in $(UNITTESTS); do \
			$(INSTALL_DATA) unittests/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/unittests; \
		done

clean:
	-rm -f -- $(SOURCES:.c=.o) $(SOURCES_LIB:.c=.o) $(SHARED_SOURCE:.c=.o)
	-rm -f -- $(SOURCES:.c=.$(EXTENSION))
	-rm -f -- $(LIBRARY_NAME).o
	-rm -f -- $(LIBRARY_NAME).$(EXTENSION)
	-rm -f -- $(SHARED_LIB)

distclean: clean
	-rm -f -- $(DISTBINDIR).tar.gz
	-rm -rf -- $(DISTBINDIR)
	-rm -f -- $(DISTDIR).tar.gz
	-rm -rf -- $(DISTDIR)
	-rm -f -- $(ORIGDIR).tar.gz
	-rm -rf -- $(ORIGDIR)


$(DISTBINDIR):
	$(INSTALL_DIR) $(DISTBINDIR)

libdir: all $(DISTBINDIR)
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd  $(DISTBINDIR)
	$(INSTALL_DATA) $(SOURCES) $(SHARED_SOURCE) $(SHARED_HEADER) $(DISTBINDIR)
	$(INSTALL_DATA) $(HELPPATCHES) $(DISTBINDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTBINDIR)
#	tar --exclude-vcs -czpf $(DISTBINDIR).tar.gz $(DISTBINDIR)

$(DISTDIR):
	$(INSTALL_DIR) $(DISTDIR)

$(ORIGDIR):
	$(INSTALL_DIR) $(ORIGDIR)

dist: $(DISTDIR)
	$(INSTALL_DATA) Makefile  $(DISTDIR)
	$(INSTALL_DATA) README.txt $(DISTDIR)
	$(INSTALL_DATA) LICENSE.txt $(DISTDIR)
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd  $(DISTDIR)
	test -z "$(strip $(ALLSOURCES))" || \
		$(INSTALL_DATA) $(ALLSOURCES)  $(DISTDIR)
	test -z "$(strip $(wildcard $(ALLSOURCES:.c=.tcl)))" || \
		$(INSTALL_DATA) $(wildcard $(ALLSOURCES:.c=.tcl))  $(DISTDIR)
	test -z "$(strip $(wildcard $(LIBRARY_NAME).c))" || \
		$(INSTALL_DATA) $(LIBRARY_NAME).c  $(DISTDIR)
	test -z "$(strip $(SHARED_HEADER))" || \
		$(INSTALL_DATA) $(SHARED_HEADER)  $(DISTDIR)
	test -z "$(strip $(SHARED_SOURCE))" || \
		$(INSTALL_DATA) $(SHARED_SOURCE)  $(DISTDIR)
	test -z "$(strip $(SHARED_TCL_LIB))" || \
		$(INSTALL_DATA) $(SHARED_TCL_LIB)  $(DISTDIR)
	test -z "$(strip $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(PDOBJECTS)  $(DISTDIR)
	test -z "$(strip $(HELPPATCHES))" || \
		$(INSTALL_DATA) $(HELPPATCHES) $(DISTDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTDIR)
	test -z "$(strip $(EXAMPLES))" || \
		$(INSTALL_DIR) $(DISTDIR)/examples && \
		for file in $(EXAMPLES); do \
			$(INSTALL_DATA) examples/$$file $(DISTDIR)/examples; \
		done
	test -z "$(strip $(MANUAL))" || \
		$(INSTALL_DIR) $(DISTDIR)/manual && \
		for file in $(MANUAL); do \
			$(INSTALL_DATA) manual/$$file $(DISTDIR)/manual; \
		done
	test -z "$(strip $(UNITTESTS))" || \
		$(INSTALL_DIR) $(DISTDIR)/unittests && \
		for file in $(UNITTESTS); do \
			$(INSTALL_DATA) unittests/$$file $(DISTDIR)/unittests; \
		done
	tar --exclude-vcs -czpf $(DISTDIR).tar.gz $(DISTDIR)

# make a Debian source package
dpkg-source:
	debclean
	make distclean dist
	mv $(DISTDIR) $(ORIGDIR)
	tar --exclude-vcs -czpf ../$(ORIGDIR).orig.tar.gz $(ORIGDIR)
	rm -f -- $(DISTDIR).tar.gz
	rm -rf -- $(DISTDIR) $(ORIGDIR)
	cd .. && dpkg-source -b $(LIBRARY_NAME)

etags: TAGS

TAGS: $(wildcard $(PD_INCLUDE)/*.h) $(SOURCES) $(SHARED_SOURCE) $(SHARED_HEADER)
	etags $(wildcard $(PD_INCLUDE)/*.h)
	etags -a *.h $(SOURCES) $(SHARED_SOURCE) $(SHARED_HEADER)
	etags -a --language=none --regex="/proc[ \t]+\([^ \t]+\)/\1/" *.tcl

showsetup:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIBS: $(LIBS)"
	@echo "ALL_CFLAGS: $(ALL_CFLAGS)"
	@echo "ALL_LDFLAGS: $(ALL_LDFLAGS)"
	@echo "ALL_LIBS: $(ALL_LIBS)"
	@echo "PD_INCLUDE: $(PD_INCLUDE)"
	@echo "PD_PATH: $(PD_PATH)"
	@echo "objectsdir: $(objectsdir)"
	@echo "LIBRARY_NAME: $(LIBRARY_NAME)"
	@echo "LIBRARY_VERSION: $(LIBRARY_VERSION)"
	@echo "SOURCES: $(SOURCES)"
	@echo "SHARED_HEADER: $(SHARED_HEADER)"
	@echo "SHARED_SOURCE: $(SHARED_SOURCE)"
	@echo "SHARED_LIB: $(SHARED_LIB)"
	@echo "SHARED_TCL_LIB: $(SHARED_TCL_LIB)"
	@echo "PDOBJECTS: $(PDOBJECTS)"
	@echo "ALLSOURCES: $(ALLSOURCES)"
	@echo "ALLSOURCES TCL: $(wildcard $(ALLSOURCES:.c=.tcl))"
	@echo "UNAME: $(UNAME)"
	@echo "CPU: $(CPU)"
	@echo "pkglibdir: $(pkglibdir)"
	@echo "DISTDIR: $(DISTDIR)"
	@echo "ORIGDIR: $(ORIGDIR)"
	@echo "NDK_TOOLCHAIN: $(NDK_TOOLCHAIN)"
	@echo "NDK_BASE: $(NDK_BASE)"
	@echo "NDK_SYSROOT: $(NDK_SYSROOT)"
