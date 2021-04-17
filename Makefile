TARGET = ciplswitcher
OBJS = main.o pspIplUpdate.o kpspident.o

LIBS = -lpspkubridge -lpsppower
INCDIR = ../Common/ ../include/
LIBDIR = ../libs

BUILD_PRX = 1

PSP_FW_VERSION = 660

CFLAGS = -O2 -G0 -Wall -fshort-wchar
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 660 cIPL Switcher

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak