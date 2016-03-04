TOPDIR=../../

include $(TOPDIR)/misc/mk/config.mk
-include $(TOPDIR)/misc/mk/common-$(ARCH).mk

LO_TARGET=core.lo

LO_OBJS=timeget.o \
	eventlock.o \
	errno.o \
	eventunlock.o \
	eventcreate.o \
	eventwait.o \
	eventsignal.o \
	eventbroadcast.o \
	errorraise.o \
	threadcreate.o \
	threadresume.o \
	threadsuspend.o \
	threaddelayedstart.o \
	threadstart.o \
	threadperiod.o \
	threadsleep.o \
	threadid.o
#main.c \
#errorlog.o \
#errorget.o \
#errorhandlercreate.o \
#errorhandlerworker.o \
#errorhandlersetready.o \
#errorignore.o \
#errorconfirm.o \
#allocator.o \
#mutexcreate.o \
#mutextrylock.o \
#mutexlock.o \
#mutexunlock.o \
#semcreate.o \
#semsignal.o \
#semstatus.o \
#semwait.o \
#threadattrinit.o \
#timecomputedeadline.o \

LO_DEPS=

all: $(LO_TARGET)

.PHONY: clean distclean depend all

clean:
	$(RM) $(LO_OBJS)

distclean:
	$(RM) $(LO_OBJS) $(LO_TARGET)
	$(RM) .depend.mk

depend:
	$(if $(LO_OBJS), $(CC) $(CFLAGS) -MM $(wildcard *.c) $(wildcard *.S) > .depend.mk,)

include $(TOPDIR)/misc/mk/rules-common.mk
include $(TOPDIR)/misc/mk/rules-partition.mk

