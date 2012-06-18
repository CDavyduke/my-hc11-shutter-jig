# ShutterJig Makefile
# Author:			Corey Davyduke
# Created:			2012-06-14
# Modified:			2012-06-18
# Description:		This is the Makefile for my Shutter Jig project.

# Use the cme11 board configuration files (compilation flags
# and specific includes).
override TARGET_BOARD=m68hc11-corey

# Makefile is now dependent on an environment variable being set
# for GEL_BASEDIR
include $(GEL_BASEDIR)/config/make.defs

PROJECT=ShutterJig

CSRCS=$(PROJECT).c

OBJS=$(CSRCS:.c=.o)

PROGS=$(PROJECT).elf

all::	$(PROGS) $(PROJECT).s19

$(PROJECT).elf:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(GEL_LIBS)

install::	$(PROGS)
	$(INSTALL) $(PROGS) $(GEL_INSTALL_BIN)

