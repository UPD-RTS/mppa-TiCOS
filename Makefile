-include misc/mk/config.mk

.SILENT:

SUBDIRS = examples kernel libpok 
EXAMPLES_SUBDIRS = $(patsubst examples/%,%,$(dir $(wildcard examples/*/)))
BUILD_DIR = build-all


all:
	$(MAKE) -C examples $@ || exit 1;
	for dir in $(EXAMPLES_SUBDIRS);	\
	do \
		mkdir -p $(BUILD_DIR)/$$dir; \
		cp ./examples/$$dir/generated-code/cpu/pok.elf ./$(BUILD_DIR)/$$dir/pok.elf; \
		cp ./examples/$$dir/generated-code/cpu/pok.elf.map ./$(BUILD_DIR)/$$dir/pok.elf.map; \
	done

clean:
	for dir	in $(SUBDIRS);			\
	do					\
		$(MAKE) -C $$dir $@ || exit 1;	\
        done
	$(RM) -rf $(BUILD_DIR)	

distclean:
	for dir	in $(SUBDIRS);			\
	do					\
		$(MAKE) -C $$dir $@ || exit 1;	\
        done
	$(RM) -rf $(BUILD_DIR); 
	$(RM) pok-[0-9]*.tgz;


-include $(POK_PATH)/misc/mk/rules-common.mk
