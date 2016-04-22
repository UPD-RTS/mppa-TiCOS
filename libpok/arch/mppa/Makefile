TOPDIR=../../../

include $(TOPDIR)/misc/mk/config.mk
-include $(TOPDIR)/misc/mk/common-$(ARCH).mk

LO_TARGET=	mppa.lo

LO_OBJS=	syscall.o \
		arch.o

LO_DEPS=

all: $(LO_TARGET)

.PHONY: clean distclean depend all

clean: common-clean
	$(RM) $(LO_OBJS)

distclean: clean
	$(RM) .depend.mk

depend:
	$(CC) $(CFLAGS) -MM $(wildcard *.c) $(wildcard *.S) > .depend.mk

include $(TOPDIR)/misc/mk/rules-common.mk
include $(TOPDIR)/misc/mk/rules-partition.mk
