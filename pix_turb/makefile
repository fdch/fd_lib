# change to your local directories!
PD_DIR = /usr/local/include/pd
GEM_DIR = /Users/federicocamarahalac/Documents/Pd/externals/Gem/dev
# build flags

INCLUDES = -I$(PD_DIR)/include
CPPFLAGS = -g -O2 -fPIC -freg-struct-return -Os -falign-loops=32 -falign-functions=32 -falign-jumps=32 -funroll-loops -ffast-math  -arch i386 -mmmx -fpascal-strings 

#linux doesnt work yet
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
 CPPFLAGS += -DDARWIN
 INCLUDES += -I$(GEM_DIR) -I$(PD_DIR)
 LDFLAGS = -c -arch i386 
 LIBS =  -lm -lfftw3f #-lopencv_legacy
 EXTENSION = pd_darwin
endif

.SUFFIXES = $(EXTENSION)

SOURCES = pix_turb

all:
	g++ $(LDFLAGS) $(INCLUDES) $(CPPFLAGS) -o $(SOURCES).o -c $(SOURCES).cpp
	g++ -o $(SOURCES).$(EXTENSION) -undefined dynamic_lookup -arch i386 -dynamiclib -undefined dynamic_lookup ./*.o -L/usr/local/lib $(LIBS) -ldl -lz -lpthread -L$(PD_DIR)
	rm -fr ./*.o
deploy:
	ifeq ($(UNAME),Darwin)
	  mkdir build/$(SOURCES)
	  ./MacOSX_editpath.sh .
	  mv *.dylib build/$(SOURCES)
	  mv *.$(EXTENSION) build/$(SOURCES)
	  cp *.pd build/$(SOURCES)
	  rm -fr $(PD_APP_DIR)/extra/$(SOURCES)
	  mv build/$(SOURCES) $(PD_APP_DIR)/extra/
	endif
clean:
	rm -f $(SOURCES)*.o
	rm -f $(SOURCES)*.$(EXTENSION)
distro: clean all
	rm *.o