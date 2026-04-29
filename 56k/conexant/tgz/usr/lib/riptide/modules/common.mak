
include $(TOP)/config.mak

# Configure location of current 2.4 kernel source directory for compiling
KERNELSRC= /lib/modules/$(shell uname -r)/build
# Debian 3.0 example:
#KERNELSRC= /usr/src/kernel-headers-2.4.18-686

KERNELSRC_EXISTS= $(shell test -r ${KERNELSRC}/include/linux/version.h && echo y || echo n)
ifneq ($(KERNELSRC_EXISTS),y)
$(error Is the kernel-source package installed? KERNELSRC does not point to a proper directory ($(KERNELSRC)))
endif

KERNELVER= $(shell echo UTS_RELEASE | gcc -E ${DISTRO_CFLAGS} -I${KERNELSRC}/include -include ${KERNELSRC}/include/linux/version.h - | grep '^"' | tr -d '" 	')

# Configure compiler (on some systems, kgcc must be used to compile kernel code)
#
#CC= kgcc
#CC= gcc
CC := $(shell $(TOP)/modules/kernelcompiler.sh $(KERNELVER) $(KERNELSRC))

KMODS_DIR= /lib/modules/$(KERNELVER)

CFLAGS =

CFLAGS += $(OTHER_INCLUDES)
CFLAGS += $(DISTRO_CFLAGS)

# Configure various machine specific CFLAGS for compiling kernel modules
#CFLAGS+= -mpreferred-stack-boundary=2
#CFLAGS+= -fomit-frame-pointer
CFLAGS+= -msoft-float
CFLAGS+= -Wall -Wno-unknown-pragmas -O2 -I$(KERNELSRC)/include
CFLAGS+= -D__KERNEL__ -DMODULE
# The following is for 2.5
CFLAGS+= -I$(KERNELSRC)/arch/i386/mach-generic
CFLAGS+= -DRIPLINUXVERSION=\"$(RIPLINUXVERSION)\"

CONFIG_MODVERSIONS= $(shell echo CONFIG_MODVERSIONS | gcc -E ${DISTRO_CFLAGS} -I${KERNELSRC}/include -include ${KERNELSRC}/include/linux/config.h - | grep -q 'CONFIG_MODVERSIONS' && echo n || echo y)
ifeq ($(CONFIG_MODVERSIONS),y)
MODVERFLAGS= -DMODVERSIONS -include $(KERNELSRC)/include/linux/modversions.h
endif

CFLAGS+= -include $(KERNELSRC)/include/linux/config.h
CFLAGS+= $(MODVERFLAGS)

## END OF CONFIGURATION SECTION
## no changes should be necessary beyond this point
#####################################################################

.PHONY: default
default: all

.c.o:
	@echo "* compiling ($(CC)) $<"
	@$(COMPILE.c) $(CFLAGS-$*) $(OUTPUT_OPTION) $<

.cpp.o:
	@echo "* compiling ($(CXX)) $<"
	@$(COMPILE.cc) $(OUTPUT_OPTION) $<

ifdef SUBDIRS
.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ all
endif

CLEAN= *.o *.a

.PHONY: clean
clean::
	rm -f $(CLEAN)
ifdef SUBDIRS
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
	done
endif


