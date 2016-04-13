export DEPLOYMENT_HEADER=$(POK_PATH)/kernel/include/deployment.h
include ../misc/mk/config.mk
-include ../misc/mk/common-$(ARCH).mk

LO_TARGET=pok.lo

LO_DEPS=core/core.lo \
        middleware/middleware.lo \
        libc/libc.lo \
        arch/arch.lo

ifeq ($(strip $(ARCH)), )
all:
	@echo "";
	@echo "Aieeeee, you forgot to provide an ARCH variable"
	@echo "-----------------------------------------------";
	@echo "";
	@echo "Please note that pok Makefile build must be run from code generation directory."
	@echo "";
	@exit 0
else
all: check-compiler .depend $(LO_TARGET)
endif

arch/arch.lo::
	$(CD) arch && $(MAKE)

core/core.lo::
	$(CD) core && $(MAKE)

core/core.lo::
	$(CD) libc && $(MAKE)

middleware/middleware.lo::
	$(CD) middleware && $(MAKE)

.PHONY: all clean distclean depend

clean: common-clean
	$(CD) arch && $(MAKE) clean
	$(CD) core && $(MAKE) clean
	$(CD) libc && $(MAKE) clean
	$(CD) middleware && $(MAKE) clean
	$(RM) $(LO_TARGET)

distclean: clean
	$(CD) arch && $(MAKE) distclean
	$(CD) core && $(MAKE) distclean
	$(CD) libc && $(MAKE) distclean
	$(CD) middleware && $(MAKE) distclean
	$(RM) .depend

depend .depend:
	$(CD) arch && $(MAKE) depend
	$(CD) core && $(MAKE) depend
	$(CD) libc && $(MAKE) depend
	$(CD) middleware && $(MAKE) depend
	$(TOUCH) .depend

include ../misc/mk/rules-common.mk
include ../misc/mk/rules-kernel.mk

LD = k1-gcc
LDFLAGS = $(CONFIG_MPPA_CC_LFLAGS)
LDFLAGS += $(CONFIG_MPPA_CC_CFLAGS)
LDFLAGS += -L$(POK_PATH)/kernel/libc/
LDOPTS = -std=c11 -v -nodefaultlibs -nostdinc -nostdlib -Wl,--print-map -Wl,--whole-archive -lmOS -Wl,--no-whole-archive -T$(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/kernel.ld

$(LO_TARGET): $(LO_DEPS) $(LO_OBJS)
	$(ECHO) $(ECHO_FLAGS) $(ECHO_FLAGS_ONELINE) "[LD] $@ "
	$(LD) $(LO_DEPS) $(LDFLAGS) $(LDOPTS)  $(LO_OBJS) -o $(LO_TARGET)
	if test $$? -eq 0; then $(ECHO) $(ECHO_FLAGS) $(ECHO_GREEN) " OK "; else $(ECHO) $(ECHO_FLAGS)    $(ECHO_RED) " KO"; fi
