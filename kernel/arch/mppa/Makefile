TOPDIR=		../../../

include $(TOPDIR)/misc/mk/config.mk
-include $(TOPDIR)/misc/mk/common-$(ARCH).mk

LO_TARGET=	mppa.lo

LO_OBJS=	entry.o \
		bin_descriptor.o \
		arch.o \
		space.o \
		thread.o \
		syscalls.o \
		isr.o \
		cswitch.o \
		timer.o

LO_DEPS=	$(BSP)/$(BSP).lo

all: $(LO_TARGET)

$(BSP)/$(BSP).lo::
	$(CD) $(BSP) && $(MAKE)

.PHONY: clean distclean depend all

clean: common-clean
	$(MAKE) -C prep $@
ifdef BSP
	$(CD) $(BSP) && $(MAKE) clean
endif

distclean: clean
	$(MAKE) -C prep $@
	$(RM) .depend.mk
ifdef BSP
	$(CD) $(BSP) && $(MAKE) distclean
endif

depend:
	$(CC) $(CFLAGS) -MM $(wildcard *.c) $(wildcard *.S) > .depend.mk
ifdef BSP
	$(CD) $(BSP) && $(MAKE) depend
endif

include $(TOPDIR)/misc/mk/rules-common.mk
include $(TOPDIR)/misc/mk/rules-kernel.mk
