# makefile
# project: Free Heroes2
#

TARGET := fheroes2
SDL_LIBS := $(shell sdl-config --libs) -lSDL_net
SDL_FLAGS := $(shell sdl-config --cflags)

CFLAGS := $(CFLAGS) -Wall -fsigned-char
LDFLAGS := $(LDFLAGS)
LIBS :=

ifdef DEBUG
CFLAGS := $(CFLAGS) -O0 -g -pedantic -DWITH_DEBUG
else
CFLAGS := -O2 $(CFLAGS)
endif

ifndef WITHOUT_MIXER
CFLAGS := $(CFLAGS) -DWITH_MIXER
SDL_LIBS := $(SDL_LIBS) -lSDL_mixer
endif

ifndef WITHOUT_IMAGE
CFLAGS := $(CFLAGS) -DWITH_IMAGE $(shell libpng-config --cflags) -DWITH_ZLIB
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(shell libpng-config --libs) -lz
endif

ifndef WITHOUT_UNICODE
CFLAGS := $(CFLAGS) -DWITH_TTF
SDL_LIBS := $(SDL_LIBS) -lSDL_ttf
endif

ifndef WITHOUT_XML
CFLAGS := $(CFLAGS) -DWITH_XML
endif

ifndef WITHOUT_ZLIB
CFLAGS := $(CFLAGS) -DWITH_ZLIB
LIBS := $(LIBS) -lz
endif

ifdef RELEASE
CFLAGS := $(CFLAGS) -DBUILD_RELEASE
endif

CFLAGS := $(SDL_FLAGS) $(CFLAGS)
LIBS := $(SDL_LIBS) $(LIBS)

# platform specific flags
ifndef PLATFORM
ifndef OS
OS := $(shell uname)
endif

ifeq ($(OS),Windows_NT)
PLATFORM := mingw
endif
ifeq ($(OS),FreeBSD)
PLATFORM := bsd
endif
ifeq ($(OS),Darwin)
PLATFORM := osx
endif
ifeq ($(OS),Linux)
PLATFORM := all
endif
endif

include Makefile.$(PLATFORM)

export CXX AR LINK WINDRES LDFLAGS CFLAGS LIBS PLATFORM

.PHONY: clean

all:
ifndef WITHOUT_XML
	$(MAKE) -C xmlccwrap
endif
	$(MAKE) -C engine
	$(MAKE) -C dist
ifndef WITHOUT_UNICODE
	$(MAKE) -C dist pot
endif

clean:
	$(MAKE) -C xmlccwrap clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
