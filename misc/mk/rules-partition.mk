ifeq ($(TOPDIR),)
CFLAGS += -I$(POK_PATH)/libpok/include -I.
else
CFLAGS += -I$(TOPDIR)/libpok/include -I.
endif

ifneq ($(LUSTRE_DIRECTORY),)
CFLAGS += -I$(LUSTRE_DIRECTORY)
endif

ifneq ($(DEPLOYMENT_HEADER),)
COPTS += -include $(DEPLOYMENT_HEADER)
endif

libpok: 
	$(CD) $(POK_PATH)/libpok && $(MAKE) distclean all

ifeq ($(ARCH), mppa)
$(TARGET): $(OBJS)
	$(ECHO) $(ECHO_FLAGS) $(ECHO_FLAGS_ONELINE) "[Assemble partition $@ "
	$(LD) $(LDFLAGS) --nostdlib --cref -T $(LINKERSCRIPT) -o $@ --whole-archive $+ -L$(POK_PATH)/libpok -lpok  -z muldefs -Map $@.map
	if test $$? -eq 0; then $(ECHO) $(ECHO_FLAGS) $(ECHO_GREEN) " OK "$(ECHO_RESET); else $(ECHO) $(ECHO_FLAGS) $(ECHO_RED) " KO" $(ECHO_RESET); fi
else
# PWD is the partition subdir
# $(@D)/partition_
$(TARGET): $(OBJS)
	$(ECHO) $(ECHO_FLAGS) $(ECHO_FLAGS_ONELINE) "[Assemble partition $@ "
	#$(LD) $(LDFLAGS) -T $(POK_PATH)/misc/ldscripts/$(ARCH)/$(BSP)/partition_$@.lds $+ -o $@ -L$(POK_PATH)/libpok -lpok -Map $@.map
	#$(LD) $(LDFLAGS) -T $(LINKERSCRIPT) $+ -o $@ -L$(POK_PATH)/libpok -lpok -Map $@.map
	# SKIP LOADER
	$(LD) $(LDFLAGS) --nostdlib --cref -T $(LINKERSCRIPT) -o $@ --whole-archive $+ -L$(POK_PATH)/libpok -lpok  -z muldefs -Map $@.map
	if test $$? -eq 0; then $(ECHO) $(ECHO_FLAGS) $(ECHO_GREEN) " OK "$(ECHO_RESET); else $(ECHO) $(ECHO_FLAGS) $(ECHO_RED) " KO"$(ECHO_RESET); fi
	# SKIP LOADER
	powerpc-elf-strip $@
endif

libpok-clean:
	$(CD) $(POK_PATH)/libpok && $(MAKE) clean
