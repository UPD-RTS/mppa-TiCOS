export DEPLOYMENT_HEADER=$(POK_PATH)/kernel/include/deployment.h
include $(POK_PATH)/misc/mk/config.mk
-include $(POK_PATH)/misc/mk/common-$(ARCH).mk

LO_TARGET=	prep.lo

LO_OBJS=	bsp.o \
		handlers.o

LO_DEPS= $(POK_PATH)/kernel/libc/libc.lo

all: $(LO_TARGET)

$(POK_PATH)/kernel/libc/libc.lo::
	$(CD) $(POK_PATH)/kernel/libc && $(MAKE)

.PHONY: clean distclean depend all

clean: common-clean

distclean: clean
	$(RM) .depend.mk

depend:
	$(if $(LO_OBJS), $(CC) $(CFLAGS) -MM $(wildcard *.c) $(wildcard *.S) > .depend.mk,)

include $(POK_PATH)/misc/mk/rules-common.mk
include $(POK_PATH)/misc/mk/rules-kernel.mk
