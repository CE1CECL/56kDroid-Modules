#
# Copyright (c) 2003 Linuxant inc.
# Copyright (c) 2001-2003 Conexant Systems, Inc.
#
# NOTE: The use and distribution of this software is governed by the terms in
# the file LICENSE, which is included in the package. You must read this and
# agree to these terms before using or distributing this software.
# 
TOP = ..

include common.mak

ALL_MODS= riptideint.o riptidelow.o riptidesound.o

all: $(ALL_MODS)

CFLAGS-int_mdl = -DEXPORT_SYMTAB
CFLAGS-riptidem = -DEXPORT_SYMTAB
CFLAGS-linux = -D__NO_VERSION__
CFLAGS-rt_hw = -D__NO_VERSION__
CFLAGS-rt_mdl = -D__NO_VERSION__

CFLAGS-rp_cmdif = -D__NO_VERSION__ -DRIPFIRMWR_FILE=\"$(RIPLIBDIR)/riptide.hex\"

#serial_hcf.o: serial_core.c serial_core.h

riptideint.o: int_mdl.o rp_cmdif.o linux.o
	@echo "> linking kernel module $@"
	@$(LD) -r -o $@ $^
	@echo ""

riptidelow.o: rt_mdl.o rt_hw.o linux.o
	@echo "> linking kernel module $@"
	@$(LD) -r -o $@ $^
	@echo ""

riptidesound.o: riptidem.o linux.o
	@echo "> linking kernel module $@"
	@$(LD) -r -o $@ $^
	@echo ""

RIP_MODS_DIR= $(KMODS_DIR)/misc

.PHONY: check_kernelver modules_install minstall mi
check_kernelver:
	@if [ -z "$(KERNELVER)" ]; then \
		echo 1>&2 "Unable to determine version of kernel source directory $(KERNELSRC)"; false; \
	else \
		true; \
	fi

modules_install minstall mi: check_kernelver all $(RIP_MODS_DIR)
	rm -f "$(RIP_MODS_DIR)/riptide"*.o
	$(INSTALL) -m 644 $(ALL_MODS) $(RIP_MODS_DIR)

.PHONY: link-identical-binaries install uninstall
link-identical-binaries: # to save space
	prevsum=""; md5sum `find binaries -type f -print` </dev/null | sort | while read sum file ; do \
		if [ "$$sum" = "$$prevsum" ] && cmp -s "$$file" "$$prevfile"; then \
			rm -f "$$file"; \
			ln "$$prevfile" "$$file"; \
		else \
			prevsum="$$sum"; \
			prevfile="$$file"; \
		fi; \
	done

install: uninstall $(RIPMODDIR) link-identical-binaries
	(cd $(TOP) && find config.mak -depth -print | cpio -pdm $(RIPLIBDIR))
	find . \( -name '*.sh' -o -name '*.[ch]' -o -name '*.mak' -o -name '[Mm]akefile' \) -print | cpio -pdm $(RIPMODDIR)
	find binaries -depth -print | cpio -pdm $(RIPMODDIR)

$(RIP_MODS_DIR) $(RIPMODDIR):
	$(MKDIR) -p $@

uninstall:
	rm -rf "$(RIPLIBDIR)/config.mak" "$(RIPMODDIR)"
