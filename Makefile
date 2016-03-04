-include misc/mk/config.mk

.SILENT:
.PHONY: doc

SUBDIRS = examples kernel libpok doc

all:
	@echo "Nothing to do here; browse through examples.";

clean:
	for dir	in $(SUBDIRS);			\
	do					\
		$(MAKE) -C $$dir $@ || exit 1;	\
        done

distclean:
	for dir	in $(SUBDIRS);			\
	do					\
		$(MAKE) -C $$dir $@ || exit 1;	\
        done
	$(RM) misc/mk/config.mk pok-[0-9]*.tgz


-include $(POK_PATH)/misc/mk/rules-common.mk
