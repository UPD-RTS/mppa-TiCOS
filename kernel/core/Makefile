TOPDIR=		../../

include $(TOPDIR)/misc/mk/config.mk
-include $(TOPDIR)/misc/mk/common-$(ARCH).mk

LO_TARGET=	core.lo

ifeq ($(ARCH), mppa)
LO_OBJS=	partition.o \
		debug.o \
		lockobj.o \
		thread.o \
		sched.o \
		time.o \
		syscall.o \
		error.o \
		loader.o \
		boot.o \
		cons.o
else
LO_OBJS=	partition.o \
		div64.o \
		debug.o \
		lockobj.o \
		thread.o \
		sched.o \
		time.o \
		syscall.o \
		error.o \
		loader.o \
		boot.o \
		cons.o
endif

LO_DEPS=

all: $(LO_TARGET)

.PHONY: clean distclean depend all

clean: common-clean

depend:
	$(if $(LO_OBJS), $(CC) $(CFLAGS) -MM $(wildcard *.c) $(wildcard *.S) > .depend.mk,)

include $(TOPDIR)/misc/mk/rules-common.mk
include $(TOPDIR)/misc/mk/rules-kernel.mk
