# Use this option to toggle the cache manager on or off.
WITH_CACHE=yes

EPIPHANY_HOME ?= /opt/adapteva/esdk
CROSS_COMPILE ?= arm-linux-gnueabihf-

HOST_ELIBS=$(EPIPHANY_HOME)/tools/host/lib
HOST_EINCS=$(EPIPHANY_HOME)/tools/host/include

ifeq ($(ARCH), armv7l)
	HOST_CC := gcc
else
	HOST_CC := $(CROSS_COMPILE)gcc
endif
HOST_CFLAGS ?= -g
HOST_LDFLAGS ?= -le-hal -le-loader -lpthread

TARGET_CC ?= epiphany-elf-gcc
TARGET_LD ?= e-ld
TARGET_OBJCOPY ?= epiphany-elf-objcopy

TARGET_EINCS=${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/sys-include
TARGET_ELIBS=${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/lib/libe-lib.a

ELF_LOCATION=bin/
E_DARTS_LOCATION=e_DARTS/

TARGET_CFLAGS= -g -ffast-math -mfp-mode=round-nearest -fpic
TARGET_LDFLAGS= -lm ${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/lib/libe-lib.a -le-lib -lgcc -lg -lc -lepiphany
TARGET_LDBINS= ${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/lib/crt0.o ${EPIPHANY_HOME}/tools/e-gnu/lib/gcc/epiphany-elf/5.2.0/crti.o ${EPIPHANY_HOME}/tools/e-gnu/lib/gcc/epiphany-elf/5.2.0/crtbegin.o ${EPIPHANY_HOME}/tools/e-gnu/lib/gcc/epiphany-elf/5.2.0/crtend.o ${EPIPHANY_HOME}/tools/e-gnu/lib/gcc/epiphany-elf/5.2.0/crtn.o
TARGET_LD_INCLUDES = -L${EPIPHANY_HOME}/tools/e-gnu/lib/gcc/epiphany-elf/5.2.0 -L${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/lib
TARGET_INCLUDES= -I ./

LDSCRIPT= fast_cacheman_codelets.ldf
LDSCRIPT_LOCATION=$(E_DARTS_LOCATION)/assets

.PHONY: all clean

all: $(ELF_LOCATION)/main.elf $(ELF_LOCATION)/e_DARTS.elf

# Current application
$(ELF_LOCATION)/main.elf: main.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -I$(HOST_EINCS) -L$(HOST_ELIBS) $(HOST_LDFLAGS)

# e_DARTS
$(ELF_LOCATION)/e_DARTS.o: $(E_DARTS_LOCATION)/runtime/e_DARTS.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $^ $(TARGET_INCLUDES)

$(ELF_LOCATION)/e_DARTS.elf: $(ELF_LOCATION)/e_DARTS.o $(ELF_LOCATION)/cachemanager.o
	$(TARGET_LD) -o $@ $^ $(TARGET_LDBINS) $(TARGET_LD_INCLUDES) $(TARGET_LDFLAGS) -T $(LDSCRIPT_LOCATION)/$(LDSCRIPT) 

$(ELF_LOCATION)/cachemanager.o: $(E_DARTS_LOCATION)/softcache/cachemanager.S
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $^

clean:
	rm -f $(ELF_LOCATION)/*
