# Project: mapslang
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows executable run:
# make all
# To create the linux executable run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug executable, run with the command:
# make all DEBUG=1 (un-tested)
# To clean up map and libadikted run:
# make clean
# To perform a normal clean, and a clean of libslang run:
# make clean-inc-libs

# Windows version can be built with:
# - Windows   : MSYS2 (MinGW 32-bit) - make
# - Windows   : MSYS2 (MinGW 32-bit) - mingw32-make
# - Windows   : MinGW (MSYS) - make
# - WSL/Linux : i686-w64-mingw32-gcc - make

	# -static-libgcc is used to remove DLL dependancies (e.g. libgcc_s_dw2-1.dll, libwinpthread-1.dll, and libgcc_s_sjlj-1.dll)

# Host environment
ifneq (,$(findstring Windows,$(OS)))
  CROSS_EXEEXT = .exe
else
  CROSS_EXEEXT =
  CROSS_COMPILE = i686-w64-mingw32-
endif																													   

# Executable file extensions on target environment
BUILD_FOR_LINUX ?= 0
ifeq ($(BUILD_FOR_LINUX), 1)
  EXEEXT =
  OTHER_EXEEXT = .exe
  LIBEXT = .so
  OTHER_LIBEXT = .dll
  DLL_FLAG =
else
  EXEEXT = .exe
  OTHER_EXEEXT =
  LIBEXT = .dll
  OTHER_LIBEXT = .so
							 

endif
# Allow debug executables to be created
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  DEBUGME = yes
else
  DEBUGME = no
endif

WARNFLAGS          = -Wall -Wextra -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare
PLATFORM_FLAGS     = -march=i486
CSTANDARD          = -std=gnu99
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. $(DLL_FLAG) $(PLATFORM_FLAGS)
CFLAGS_DEBUG       = -c -g -I. $(DLL_FLAG) $(PLATFORM_FLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
#RES                = obj/map_private.res
#RC                 = map_private.rc
LFLAGS             = -static-libgcc $(PLATFORM_FLAGS)
LFLAGS_DEBUG       = -static-libgcc $(PLATFORM_FLAGS) -DDEBUG
LIBS               = -L. -L../ADiKtEd/libadikted/bin -ladikted
SRC                = \
 dk2mtool.c \
 dk2mfile.c \
 things.c \
 script.c \
 lbfileio.c
OBJ                = $(addprefix obj/,$(SRC:.c=.o))
BIN                = bin/dk2mconv$(EXEEXT)
LIB_ADIKTED        = adikted$(LIBEXT)
RM                 = rm -f
MKDIR              = mkdir -p
CP                 = cp -f
STRIP              = strip --strip-all

.PHONY: pre-build post-build all clean

all: make-directories pre-build $(SRC) $(BIN) post-build

make-directories:
	$(MKDIR) ../lib bin obj pkg

create-package: fill-pkg-directory

strip-executables:
	$(STRIP) pkg/dk2mconv$(EXEEXT) pkg/$(LIB_ADIKTED)

fill-pkg-directory:
	$(CP) $(BIN) pkg/
	$(CP) ../ADiKtEd/libadikted/bin/$(LIB_ADIKTED) pkg/

post-build: create-package

pre-build: $(LIB_ADIKTED)

clean:
	$(RM) $(OBJ) $(BIN) $(BIN:$(EXEEXT)=$(OTHER_EXEEXT)) $(RES)
	$(RM)r pkg/*
	cd ../ADiKtEd/libadikted && make clean

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS_DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

obj/%.o: %.c
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS_DEBUG),$(CFLAGS)) $< -o $@

$(RES): $(RC)
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff

$(LIB_ADIKTED):
	cd ../ADiKtEd/libadikted && make
