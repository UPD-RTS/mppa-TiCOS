TOPDIR=		../../

include $(TOPDIR)/misc/mk/config.mk
-include $(TOPDIR)/misc/mk/common-$(ARCH).mk

LO_TARGET=	arch.lo

LO_OBJS=		

LO_DEPS=	$(ARCH)/$(ARCH).lo

all: $(LO_TARGET)

$(ARCH)/$(ARCH).lo::
	$(CD) $(ARCH) && $(MAKE)

.PHONY: clean distclean depend all

clean: common-clean
ifdef ARCH
	$(MAKE) -C $(ARCH) $@
else
	for dir in $(shell find -mindepth 1 -maxdepth 1 -type d ! -name ".svn");	\
	do										\
		$(MAKE) -C $$dir $@ || exit 1;						\
	done
endif

distclean: clean
ifdef ARCH
	$(MAKE) -C $(ARCH) $@
else
	for dir in $(shell find -mindepth 1 -maxdepth 1 -type d ! -name ".svn");	\
	do										\
		$(MAKE) -C $$dir $@ || exit 1;						\
	done
endif

depend:
	$(CD) $(ARCH) && $(MAKE) depend

include $(TOPDIR)/misc/mk/rules-common.mk
include $(TOPDIR)/misc/mk/rules-kernel.mk

LD = k1-gcc
LDFLAGS += $(CONFIG_MPPA_CC_CFLAGS)
LDOPTS = -Wl,-nostdlib -Wl,-M -Wl,-whole-archive -lmOS -T$(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.ld -Wl,-no-whole-archive

$(LO_TARGET): $(LO_DEPS) $(LO_OBJS)
	$(ECHO) $(ECHO_FLAGS) $(ECHO_FLAGS_ONELINE) "[LD] $@ "
	$(LD) $(LDFLAGS) $(LDOPTS) $(LO_DEPS) $(LO_OBJS) -o $(LO_TARGET)
	if test $$? -eq 0; then $(ECHO) $(ECHO_FLAGS) $(ECHO_GREEN) " OK "; else $(ECHO) $(ECHO_FLAGS)    $(ECHO_RED) " KO"; fi
