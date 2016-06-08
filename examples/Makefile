-include misc/mk/config.mk

.SILENT:

SUBDIRS = $(wildcard */)
	
all:
	for dir	in $(SUBDIRS);					\
	do							\
		$(MAKE) -C $$dir/generated-code $@ || exit 1;	\
        done

clean:
	for dir	in $(SUBDIRS);					\
	do							\
		$(MAKE) -C $$dir/generated-code $@ || exit 1;	\
        done

distclean: clean

-include $(POK_PATH)/misc/mk/rules-common.mk
