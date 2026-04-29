#
# Copyright (c) 2003 Linuxant inc.
# Copyright (c) 2001-2003 Conexant Systems, Inc.
#
# NOTE: The use and distribution of this software is governed by the terms in
# the file LICENSE, which is included in the package. You must read this and
# agree to these terms before using or distributing this software.
# 

# The version string specified here is propagated everywhere
RIPLINUXVERSION=0.6lnxtbeta03122800
RIPLINUXRPM_REL=1

ifneq ($(IMPORTED_DEBUG_DEFS),)
ifneq ($(IMPORTED_DEBUG_DEFS),-DUSE_TRACE_ONLY_ERRORS)
RIPLINUXVERSION:=$(RIPLINUXVERSION)dbg
endif
endif

ROOT=
PREFIX=$(ROOT)/usr

ETCDIR=$(ROOT)/etc
SBINDIR=$(PREFIX)/sbin
BINDIR=$(PREFIX)/bin
LIBDIR=$(PREFIX)/lib

RIPSBINDIR=$(SBINDIR)
RIPBINDIR=$(BINDIR)

RIPLIBDIR=$(LIBDIR)/riptide
RIPMODDIR=$(RIPLIBDIR)/modules

INSTALL= install
MKDIR=   mkdir -m 755

