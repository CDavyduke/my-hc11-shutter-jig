# ShutterJig Makefile
# Author:       Corey Davyduke
# Created:      2012-06-14
# Modified:     2012-06-18
# Compiler:     GNU GCC
# Description:  This is the Makefile for my Shutter Jig project.

# Host generic commands
RM=rm -f
INSTALL=cp

# Device specific suffix and GCC compiler tool affixes
DEVC_PREFIX=m6811-elf-
CC=$(DEVC_PREFIX)gcc
SIZE=$(DEVC_PREFIX)size
OBJCOPY=$(DEVC_PREFIX)objcopy

# Libraries
LIBS=lib/libc.a lib/libbsp.a

# CPP flags passed during a compilation (include paths)
CPPFLAGS=-I. -I./include

# C flags used by default to compile the program
CFLAGS=-m68hc11 -mshort -Wall -Wmissing-prototypes -g -Os

# LDFLAGS used by default to link the program
LDFLAGS=-m68hc11 -mshort -Wl,-m,m68hc11elfb -L. -nostartfiles \
				-Wl,-defsym,_io_ports=0x1000 \
				-Wl,-defsym,_.tmp=0x0 \
				-Wl,-defsym,_.z=0x2

# Options to creates the .s19 or .b files from the elf
OBJCOPY_FLAGS=--only-section=.text --only-section=.rodata \
            --only-section=.vectors --only-section=.data

# Rule to create an S19 file from an ELF file.
.SUFFIXES: .elf .s19
.elf.s19:
	$(OBJCOPY) --output-target=srec $(OBJCOPY_FLAGS) $< $*.s19

# Project name (suffix to many other parts)
PROJECT=ShutterJig

# C Source file
CSRCS=$(PROJECT).c

OBJS=$(CSRCS:.c=.o)
PROGS=$(PROJECT).elf

all::	$(PROGS) $(PROJECT).s19

$(PROJECT).elf:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean::
	$(RM) *.o *.elf *.s19
